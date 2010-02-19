/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Implementation of CApSettingsModel.
*
*/


// INCLUDE FILES
#include "APSettingsHandlerUIVariant.hrh"

#include <ActiveApDb.h>
#include <ApListItemList.h>
#include <ApListItem.h>
#include <ApSelect.h>
#include <apsetui.rsg>
#include <VpnApEngine.h>
#include <hlplch.h>
#include <eikenv.h>
#include <eikappui.h>
#include <StringLoader.h>
#include <featmgr.h>

#include "ApSettingsHandlerUI.hrh"

#include "ApSettingsModel.h"
#include <ApSettingsHandlerCommons.h>
#include "Apsettingsdeleteoperation.h"

#include <sysutil.h>
#include <ErrorUI.h>


#include "ApNetworks.h"
#include "FeatureManagerWrapper.h"

#include <WEPSecuritySettingsUI.h>
#include <WPASecuritySettingsUI.h>

#include <ApAccessPointItem.h>

#include "ApSettingsHandlerLogger.h"

//#define __TEST_PERFORMANCE

// CONSTANTS




/**
* Estimated number of bytes in FFS for one writing operation. This amount
* is checked to be available before each transaction which places write-lock.
* Exact size is not known (because what happens in a transaction is unknown).
* A typical access point is estimated to be less than ~2000 bytes; 
* so assuming the typical operation of adding one access point 
* (with database transaction overhead and safety) is estimated 
* to be 2000 bytes. This should be a safe guess.
*/
LOCAL_C const TInt KEstimatedFfsUsage = 2000;



// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CApSettingsModel::NewL
// ---------------------------------------------------------
//
CApSettingsModel* CApSettingsModel::NewL( TInt aIspFilter, TInt aBearerFilter,
                                          TInt aSortType, TInt aReqIpvType,
                                          TVpnFilterType aVpnFilterType,
                                          TInt aVariant,
                                          TBool aIncludeEasyWlan,
                                          TBool aNoEdit )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::NewL")    
    CApSettingsModel* db = 
        new( ELeave ) CApSettingsModel( aIspFilter, aBearerFilter, 
                                        aSortType, aReqIpvType, aVariant,
                                        aVpnFilterType, aIncludeEasyWlan,
                                        aNoEdit );
    CleanupStack::PushL( db );
    db->ConstructL( NULL );
    CleanupStack::Pop(); // db
    APSETUILOGGER_LEAVEFN( EModel,"Model::NewL")    
    return db;
    }



// ---------------------------------------------------------
// CApSettingsModel::NewL
// ---------------------------------------------------------
//
CApSettingsModel* CApSettingsModel::NewL( CActiveApDb& aDb, TInt aIspFilter,
                                          TInt aBearerFilter, TInt aSortType,
                                          TInt aReqIpvType, 
                                          TVpnFilterType aVpnFilterType,
                                          TInt aVariant,
                                          TBool aIncludeEasyWlan,
                                          TBool aNoEdit )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::NewL2")
    CApSettingsModel* db = 
        new( ELeave ) CApSettingsModel( aIspFilter, aBearerFilter, 
                                        aSortType, aReqIpvType, aVariant,
                                        aVpnFilterType, aIncludeEasyWlan,
                                        aNoEdit );
    CleanupStack::PushL( db );
    db->ConstructL( &aDb );
    CleanupStack::Pop(); // db
    APSETUILOGGER_LEAVEFN( EModel,"Model::NewL2")    
    return db;
    }


/**
* Destructor.
*/
CApSettingsModel::~CApSettingsModel( )
    {
    APSETUILOGGER_ENTERFN( EModel,"~Model")
    delete iFeatureManagerWrapper;

    delete iApData;
    delete iUtils;
    delete iVpnApEngine;
    if ( iDbOwned )
        {
        delete iDb;
        }
    if (iop)
        {
        delete iop;
        }

    delete iWepSecSettings;
    delete iWepSecSettingsUi;

    delete iWpaSecSettings;
    delete iWpaSecSettingsUi;
    APSETUILOGGER_LEAVEFN( EModel,"~Model")    
    }


/**
* Constructor
*/
CApSettingsModel::CApSettingsModel( TInt aIspFilter, TInt aBearerFilter, 
                                    TInt aSortType, TInt aReqIpvType,
                                    TInt aVariant,
                                    TVpnFilterType aVpnFilterType,
                                    TBool aIncludeEasyWlan,
                                    TBool aNoEdit )

    {
    APSETUILOGGER_ENTERFN( EModel,"Model")
    
    iWepUiExitReason = 0;
    iParams.iIspFilter = aIspFilter;
    iParams.iBearerFilter = aBearerFilter;
    iParams.iSortType = aSortType;
    iReqIpvType = aReqIpvType;
    iApData = NULL;
    iDb = NULL;
    iUtils = NULL;
    iDbOwned = ETrue;
    iVpnFilterType = aVpnFilterType;
    iVariant = aVariant;
    iIncludeEasyWlan = aIncludeEasyWlan;
    iNoEdit = aNoEdit;
    iInRefresh = EFalse;
    iNeedRefresh = EFalse;
    APSETUILOGGER_LEAVEFN( EModel,"Model")    
    }



// ---------------------------------------------------------
// CApSettingsModel::ConstructL
// ---------------------------------------------------------
//
void CApSettingsModel::ConstructL( CActiveApDb* aDb )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::ConstructL")
    if ( aDb )
        {
        iDbOwned = EFalse;
        iDb = aDb;
        }
    else
        {
        iDb = CActiveApDb::NewL( EDatabaseTypeIAP );
        iDbOwned = ETrue;
        }
    iApData = CApDataHandler::NewLC( *iDb->Database() );
    CleanupStack::Pop( iApData ); // as it is a member
    iUtils = CApUtils::NewLC( *iDb->Database() );
    CleanupStack::Pop( iUtils ); //
    iVpnApEngine = CVpnApEngine::NewLC( iDb->Database() );
    CleanupStack::Pop( iVpnApEngine ); //

    iFeatureManagerWrapper = CFeatureManagerWrapper::NewL();
    iHelpSupported = FeatureManager::FeatureSupported( KFeatureIdHelp );
    iProtectionSupported = FeatureManager::FeatureSupported( 
                                            KFeatureIdSettingsProtection );
    
    APSETUILOGGER_LEAVEFN( EModel,"Model::ConstructL")    
    }



// ---------------------------------------------------------
// CApSettingsModel::Database
// ---------------------------------------------------------
//
CActiveApDb* CApSettingsModel::Database()
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::Database<->")
    return iDb;
    }




// ---------------------------------------------------------
// CApSettingsModel::AllListItemDataL
// ---------------------------------------------------------
//
TInt CApSettingsModel::AllListItemDataL( TBool& aFailedLocked, 
                                         CApListItemList& aList, 
                                         TInt aIspFilter, TInt aBearerFilter,
                                         TInt aSortType, TInt aReqIpvType )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::AllListItemDataL<->")
    return AllListItemDataL( aFailedLocked, aList, aIspFilter, 
                             aBearerFilter, aSortType, aReqIpvType,
                             iVpnFilterType, iIncludeEasyWlan );
    }



// ---------------------------------------------------------
// CApSettingsModel::AllListItemDataL
// ---------------------------------------------------------
//
TInt CApSettingsModel::AllListItemDataL( 
                                        TBool& aFailedLocked,
                                        CApListItemList& aList,
                                        TInt aIspFilter,
                                        TInt aBearerFilter,
                                        TInt aSortType,
                                        TInt aReqIpvType,
                                        TVpnFilterType aVpnFilterType,
                                        TBool aIncludeEasyWlan
                                        )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::AllListItemDataL2")
    TInt retval( 0 );
    if (iInRefresh)
        {
        APSETUILOGGER_WRITE( EModel,"AllListItemData overrun->");
        iNeedRefresh = ETrue;
        }
    else
        {
        iInRefresh = ETrue;
        aFailedLocked = EFalse;
        CApSelect* apselect = CApSelect::NewLC( *iDb->Database(), aIspFilter, 
                                                aBearerFilter, aSortType,
                                                aReqIpvType,
                                                aVpnFilterType,
                                                aIncludeEasyWlan );
        APSETUILOGGER_WRITE( EModel,"ApSelect::NewLC returned");
        
        retval = apselect->AllListItemDataL( aList );
        
        APSETUILOGGER_WRITE( EModel,"ApSelect::AllListItemDataL returned");

        CleanupStack::PopAndDestroy( apselect ); // apselect
        retval = aList.Count();
    
        iInRefresh = EFalse;
        }
    if (iNeedRefresh && !iInRefresh)
        {
        APSETUILOGGER_WRITE( EModel,"add. refresh starts");
        retval = AllListItemDataL( aFailedLocked, aList, aIspFilter, 
                                   aBearerFilter, aSortType, aReqIpvType,
                                   aVpnFilterType, aIncludeEasyWlan );
        iNeedRefresh = EFalse;
        APSETUILOGGER_WRITE( EModel,"add. refresh ends");
        }
    APSETUILOGGER_LEAVEFN( EModel,"Model::AllListItemDataL2")
    return retval;
    }



// ---------------------------------------------------------
// CApSettingsModel::AllListItemDataL
// ---------------------------------------------------------
//
TInt CApSettingsModel::AllListItemDataL( TBool& aFailedLocked, 
                                         CApListItemList& aList )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::AllListItemDataL ->")
    return AllListItemDataL( aFailedLocked, aList,  iParams.iIspFilter, 
                                                iParams.iBearerFilter, 
                                                iParams.iSortType,
                                                iReqIpvType, iVpnFilterType,
                                                iIncludeEasyWlan );
    }



// ---------------------------------------------------------
// CApSettingsModel::AccessPointDataL
// ---------------------------------------------------------
//
void CApSettingsModel::AccessPointDataL( TUint32 aUid, 
                                         CApAccessPointItem& aApItem )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::AccessPointDataL")
    TTransactionResult ownTransaction = StartTransactionLC( /*aWrite*/EFalse,
                                               /*aShowNote*/ETrue,
                                               /*aRetry*/ETrue );
    if ( ownTransaction != EFailedWithLocked )
        {
        iApData->AccessPointDataL( aUid, aApItem );    // adds normal ap
        // Load wlan data, only loads if WLAN ap.
        TRAPD( err, LoadWlanL( aApItem ) );
        if ( err != KErrUnknown)
            {
            User::LeaveIfError( err );
            }

        if ( ownTransaction == EOwnTransaction )
            {
            TInt err = CommitTransaction();
            User::LeaveIfError( err );
            CleanupStack::Pop(); // RollbackTransactionOnLeave
            }
        }
    
    APSETUILOGGER_LEAVEFN( EModel,"Model::AccessPointDataL")    
    }



// ---------------------------------------------------------
// CApSettingsModel::UpdateAccessPointDataL
// ---------------------------------------------------------
//
TBool CApSettingsModel::UpdateAccessPointDataL( CApAccessPointItem& aApItem )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::UpdateAccessPointDataL")
    TBool nameChanged( EFalse );
// It should be aWrite ETrue, but there is an error in Symbian stuff
// causing a memory leak
//    TTransactionResult ownTransaction = StartTransactionLC( /*aWrite*/ETrue,
    TTransactionResult ownTransaction = StartTransactionLC( /*aWrite*/EFalse,
                                               /*aShowNote*/ETrue,
                                               /*aRetry*/ETrue );
    if ( ownTransaction != EFailedWithLocked )
        {
        TBool isnew(EFalse);
        if ( aApItem.BearerTypeL() == EApBearerTypeWLAN )
            { // it is WLAN...
            // check that there were no bearer-change, because if orig. bearer
            // was NOT wlan, then we shouls consider it as a new record!!!
            if ( iUtils->BearerTypeL( aApItem.WapUid() ) != EApBearerTypeWLAN )
                {
                isnew = ETrue;
                }
            }
        iApData->UpdateAccessPointDataL( aApItem, nameChanged );
        // write wlan data, only writes if WLAN ap.
        WriteWlanL( aApItem, isnew );

#ifdef __WINS__
        ShowNoteL( R_APUI_VIEW_SAVED );
#endif
        if ( ownTransaction == EOwnTransaction )
            {
            TInt err = CommitTransaction();
            User::LeaveIfError( err );
            CleanupStack::Pop(); // RollbackTransactionOnLeave
            }
        }

    APSETUILOGGER_LEAVEFN( EModel,"Model::UpdateAccessPointDataL")
    return nameChanged;
    }




// ---------------------------------------------------------
// CApSettingsModel::DataHandler
// ---------------------------------------------------------
//
CApDataHandler* CApSettingsModel::DataHandler()
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::DataHandler <->")
    return iApData;
    }



// ---------------------------------------------------------
// CApSettingsModel::ApUtils
// ---------------------------------------------------------
//
CApUtils* CApSettingsModel::ApUtils()
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::ApUtils <->")
    return iUtils;
    }


// ---------------------------------------------------------
// CApSettingsModel::SetParams
// ---------------------------------------------------------
//
void CApSettingsModel::SetParams( TApSettingsParams aParams )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::SetParams <->")
    iParams = aParams;
    }


// ---------------------------------------------------------
// CApSettingsModel::Params
// ---------------------------------------------------------
//
TApSettingsParams CApSettingsModel::Params()
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::Params <->")
    return iParams;
    }



// ---------------------------------------------------------
// CApSettingsModel::VpnEngine
// ---------------------------------------------------------
//
CVpnApEngine* CApSettingsModel::VpnEngine()
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::VpnEngine <->")
    return iVpnApEngine;
    }


// ---------------------------------------------------------
// CApSettingsModel::LaunchHelpL
// ---------------------------------------------------------
//
void CApSettingsModel::LaunchHelpL()
    {
#ifdef __SERIES60_HELP
    APSETUILOGGER_ENTERFN( EModel,"Model::LaunchHelpL")
    HlpLauncher::LaunchHelpApplicationL(
                        EikEnv()->WsSession(),
                        EikEnv()->EikAppUi()->AppHelpContextL() );
    APSETUILOGGER_LEAVEFN( EModel,"Model::LaunchHelpL")    
#endif // __SERIES60_HELP
    }


// ---------------------------------------------------------
// CApSettingsModel::RemoveApL
// ---------------------------------------------------------
//
void CApSettingsModel::RemoveApL( TUint32 aUid )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::RemoveApL")
// DELETE OPERATION STARTS HERE
    TBuf<KModifiableTextLength> sgd;
    iUtils->NameL( aUid, sgd );
    HBufC* temp = StringLoader::LoadLC( R_APUI_VIEW_DELETING_AP, sgd );    

    APSETUILOGGER_WRITE( EModel,"Creating thread");
    
    if (!iop)
        {
        iop = CApSettingsDeleteOperation::NewL(
                        KApWaitNote, R_APUI_VIEW_WAIT_NOTE,
                        aUid, *temp
                        );
        APSETUILOGGER_WRITE( EModel,"Thread created");
        }
    else
        {
        if (iop->State() != CApSettingsDeleteOperation::EProcOffDialogOff )
            {
            ShowNoteL( R_APUI_VIEW_TEXT_CANNOT_ACCESS_DB );
            User::Leave( KErrInUse );
            }
        else
            {
            iop->SetUid( aUid );
            iop->SetNoteTextL( *temp );
            }
        }
    
    TRAPD(err, iop->StartL() );
    
    APSETUILOGGER_WRITE_FORMAT( EModel, "Thread StartL returned, code:%d", err);
    
    if ( err != KErrNone )
        { 
        User::Leave( err );
        }
    
    CleanupStack::PopAndDestroy( temp );
    APSETUILOGGER_LEAVEFN( EModel,"Model::RemoveApL")    
// DELETE OPERATION ENDS HERE

    }




// ---------------------------------------------------------
// CApSettingsModel::StarTransactionLC
// ---------------------------------------------------------
//
TTransactionResult CApSettingsModel::StartTransactionLC(
                        TBool aWrite, TBool aShowNote,
                        TBool aRetry, TBool aLFFSChecking /* ETrue */ )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::StartTransactionLC")
    TTransactionResult retval( EOwnTransaction );

    // Before attempting db modification, check if we would go below Critical
    // Level in FFS (disk). The value used for checking is an estimate.
    // Reading the db also allocates some bytes (transaction log), but
    // this is not estimated and not checked, because it should be small
    // enough, and would slow down operation. (Typical use is to read a lot
    // and write much less.)
    if (    aLFFSChecking &&
            aWrite &&
            FFSSpaceBelowCriticalLevelL
                ( /*aShowErrorNote=*/EFalse, KEstimatedFfsUsage )
       )
        {
        User::Leave( KErrDiskFull );
        }

    CCommsDatabase* aDb = iDb->Database();
    TInt err( KErrNone );
    if ( !aDb->InTransaction() )
        {
        TInt retriesLeft( aRetry ? KRetryCount : 1 );
        err = aDb->BeginTransaction();
        while ( ( ( err == KErrLocked ) || ( err == KErrAccessDenied ) )
                && --retriesLeft )
            {
            User::After( KRetryWait );
            err = aDb->BeginTransaction();
            if (aWrite )
                {
                TRAP(err, GetWriteLockL() );
                }
            }
        if ( err && aShowNote )
            {
            ShowNoteL( R_APUI_VIEW_TEXT_CANNOT_ACCESS_DB );
            }
        switch ( err )
            {
            case KErrNone:
                {
                CleanupStack::PushL( 
                    TCleanupItem( RollbackTransactionOnLeave, aDb ) );
                break;
                }
            case KErrLocked:
            case KErrAccessDenied:
                {
                retval = EFailedWithLocked;
                break;
                }
            default:
                {
                User::LeaveIfError( err );
                break;
                }
            }
        }
    else
        {
        if ( aWrite )
            {
            TRAP(err, GetWriteLockL() );
            }
        if ( err && aShowNote )
            {
            ShowNoteL( R_APUI_VIEW_TEXT_CANNOT_ACCESS_DB );
            }
        if ( err != KErrLocked )
            {
            User::LeaveIfError( err );
            }
        if ( err )
            {
            retval = EFailedWithLocked;
            }
        else
            {
            retval = EUsingAlreadyStarted;
            }
        }
    APSETUILOGGER_LEAVEFN( EModel,"Model::StartTransactionLC")    
    return retval;
    }


// ---------------------------------------------------------
// CApSettingsModel::GetWriteLockL
// ---------------------------------------------------------
//
void CApSettingsModel::GetWriteLockL( )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::GetWriteLockL")
    CCommsDbTableView* v = 
        iDb->Database()->OpenTableLC( TPtrC(WAP_ACCESS_POINT ) );
    TUint32 dummyUid( KApNoneUID );
    TInt err = v->InsertRecord( dummyUid );
    User::LeaveIfError( err );
    v->CancelRecordChanges();
    User::LeaveIfError( err );
    CleanupStack::PopAndDestroy( v );
    APSETUILOGGER_LEAVEFN( EModel,"Model::GetWriteLockL")    
    }

// ---------------------------------------------------------
// CApSettingsModel::RollbackTransactionOnLeave
// ---------------------------------------------------------
//
void CApSettingsModel::RollbackTransactionOnLeave( TAny* aDb )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::RollbackTransactionOnLeave")
    CCommsDatabase* db = STATIC_CAST( CCommsDatabase*, aDb );
    db->RollbackTransaction();
    APSETUILOGGER_LEAVEFN( EModel,"Model::RollbackTransactionOnLeave")    
    }



// ---------------------------------------------------------
// CApSettingsModel::CommitTransaction
// ---------------------------------------------------------
//
TInt CApSettingsModel::CommitTransaction()
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::CommitTransaction")
    TInt err = iDb->Database()->CommitTransaction();
    APSETUILOGGER_LEAVEFN( EModel,"Model::CommitTransaction")    
    return err;
    }



// ---------------------------------------------------------
// CApSettingsModel::FFSSpaceBelowCriticalLevelL
// ---------------------------------------------------------
//
TBool CApSettingsModel::FFSSpaceBelowCriticalLevelL
        ( TBool aShowErrorNote, TInt aBytesToWrite /*=0*/ )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::FFSSpaceBelowCriticalLevelL")
    TBool ret( EFalse );
    if ( SysUtil::FFSSpaceBelowCriticalLevelL
                ( &( EikEnv()->FsSession()), aBytesToWrite ) )
        {
        ret = ETrue;
        if ( aShowErrorNote )
            {
            CErrorUI* errorUi = CErrorUI::NewLC( *EikEnv() );
            errorUi->ShowGlobalErrorNoteL( KErrDiskFull );
            CleanupStack::PopAndDestroy();  // errorUi
            }
        }
    APSETUILOGGER_LEAVEFN( EModel,"Model::FFSSpaceBelowCriticalLevelL")    
    return ret;
    }




// ---------------------------------------------------------
// CApSettingsModel::NetWorkListDataL
// ---------------------------------------------------------
//
TInt CApSettingsModel::NetWorkListDataL( TBool& aFailedLocked, 
                                         CApNetworkItemList& aList )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::NetWorkListDataL")
    TInt retval( KErrNone );
    aFailedLocked = EFalse;
    TTransactionResult ownTransaction = StartTransactionLC( /*aWrite*/EFalse,
                                               /*aShowNote*/EFalse,
                                               /*aRetry*/ETrue );
    if ( ownTransaction != EFailedWithLocked )
        {
        CApNetworks* apnet = CApNetworks::NewLC( *iDb->Database() );
        retval = apnet->AllListItemDataL( aList );
        CleanupStack::PopAndDestroy( apnet ); // apSelect
        if ( ownTransaction == EOwnTransaction )
            {
            TInt err = CommitTransaction();
            User::LeaveIfError( err );
            CleanupStack::Pop(); // RollbackTransactionOnLeave
            }
        }
    else
        {
        aFailedLocked = ETrue;
        }
    APSETUILOGGER_LEAVEFN( EModel,"Model::NetWorkListDataL")    
    return retval;
    }



// ---------------------------------------------------------
// CApSettingsModel::RequestedIPvType
// ---------------------------------------------------------
//
TInt CApSettingsModel::RequestedIPvType( )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::RequestedIPvType<->")
    return iReqIpvType;
    }




// ---------------------------------------------------------
// CApSettingsModel::Need2DeleteSelect
// called when menu needs to be displayed
// ---------------------------------------------------------
//
TBool CApSettingsModel::Need2DeleteSelect(  TInt aResourceId, TInt aCount )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::Need2DeleteSelect")
    TBool retval( EFalse );

    if ( ( aCount == 0 )
         &&
         ( aResourceId == R_APSETTINGSUI_MENU_SELECT_NORMAL ) )
        {
        retval = ETrue;
        }
    APSETUILOGGER_LEAVEFN( EModel,"Model::Need2DeleteSelect")    
    return retval;
    }


// ---------------------------------------------------------
// CApSettingsModel::Need2DeleteOpen
// called when menu needs to be displayed
// ---------------------------------------------------------
//
TBool CApSettingsModel::Need2DeleteOpen(  TInt aResourceId, TInt aCount, 
                                          TUint32 aUid, TBool aNeedsNone )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::Need2DeleteOpen")
    TBool retval( EFalse );
    TBool noItem( EFalse );
    TBool onlyItemIsNone( EFalse );
    TBool editOnlyVPN( EFalse );
    TBool noEdit( EFalse );

    noItem = ( aCount == 0) && 
            ( ( aResourceId == R_APSETTINGSUI_MENU_NORMAL ) ||
              ( aResourceId == R_APSETTINGSUI_MENU_SELECT_NORMAL ) );
    
    onlyItemIsNone = ( aNeedsNone ) && ( aUid == KApNoneUID ) 
         && ( ( aResourceId == R_APSETTINGSUI_MENU_SELECT_ONLY ) ||
              ( aResourceId == R_APSETTINGSUI_MENU_SELECT_NORMAL ) ||
              ( aResourceId == R_APSETTINGSUI_MENU_NORMAL ) );

    editOnlyVPN = ( iVariant & KApUiEditOnlyVPNs ) 
        && ( ( aResourceId == R_APSETTINGSUI_SELECT_ONLY_MENUBAR ) ||
             ( aResourceId == R_APSETTINGSUI_SELECT_NORMAL_MENUBAR ) ||
             ( aResourceId == R_APSETTINGSUI_MENU_NORMAL ) );
    noEdit = ( iNoEdit )
        && ( ( aResourceId == R_APSETTINGSUI_MENU_SELECT_NORMAL ) ||
             ( aResourceId == R_APSETTINGSUI_MENU_NORMAL ) );

    retval = noItem || onlyItemIsNone || editOnlyVPN || noEdit;
    
    APSETUILOGGER_LEAVEFN( EModel,"Model::Need2DeleteOpen")    
    return retval;
    }


// ---------------------------------------------------------
// CApSettingsModel::Need2DeleteDeleteL
// called when menu needs to be displayed
// ---------------------------------------------------------
//
TBool CApSettingsModel::Need2DeleteDeleteL(  TInt aResourceId, TInt aCount, 
                                             TUint32 aUid, TBool aNeedsNone )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::Need2DeleteDeleteL")
    TBool retval( EFalse );
    TBool noItem( EFalse );
    TBool onlyItemIsNone( EFalse );    
    TBool editOnlyVPN( EFalse );
    TBool noEdit( EFalse );

    noItem = ( aCount == 0) && 
            ( ( aResourceId == R_APSETTINGSUI_MENU_NORMAL ) ||
              ( aResourceId == R_APSETTINGSUI_MENU_SELECT_NORMAL ) );

    onlyItemIsNone = ( aNeedsNone ) && ( aUid == KApNoneUID ) 
        && ( ( aResourceId == R_APSETTINGSUI_MENU_SELECT_ONLY ) ||
             ( aResourceId == R_APSETTINGSUI_MENU_SELECT_NORMAL ) ||
             ( aResourceId == R_APSETTINGSUI_MENU_NORMAL ) );

    editOnlyVPN = ( iVariant & KApUiEditOnlyVPNs )
        && ( ( aResourceId == R_APSETTINGSUI_SELECT_ONLY_MENUBAR ) ||
             ( aResourceId == R_APSETTINGSUI_SELECT_NORMAL_MENUBAR ) ||
             ( aResourceId == R_APSETTINGSUI_MENU_NORMAL ) );
             
    noEdit = ( iNoEdit )
        && ( ( aResourceId == R_APSETTINGSUI_MENU_SELECT_NORMAL ) ||
             ( aResourceId == R_APSETTINGSUI_MENU_NORMAL ) );

    if ( editOnlyVPN )
        {
        // check if current item is VPN...        
        if ( ( aCount != 0 ) && ( aUid != KApNoneUID ) )
            {
            retval = ! iVpnApEngine->IsVpnApL( aUid );                
            }
        else
            {
            retval = ETrue;
            }
        }
    else
        {
        if ( onlyItemIsNone || noItem )
            {
            retval = ETrue;
            }
        }
    if ( !retval )
        {
        if (noEdit)
            {
            retval = ETrue;
            }
        }        
    APSETUILOGGER_LEAVEFN( EModel,"Model::Need2DeleteDeleteL")    
    return retval;
    }


// ---------------------------------------------------------
// CApSettingsModel::Need2DeleteNewUseExisting
// called when menu needs to be displayed
// ---------------------------------------------------------
//
TBool CApSettingsModel::Need2DeleteNewUseExisting(  
                                            TInt aResourceId, 
                                            TInt aCount )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::Need2DeleteNewUseExisting")
    TBool noEdit( EFalse );
    TBool retval( EFalse );
    
    if ( iProtectionSupported )
        {
        if (  aResourceId == R_APSETTINGSUI_MENU_SELECT_NORMAL  ||
                aResourceId == R_APSETTINGSUI_MENU_NORMAL )
            {
            TRAP_IGNORE( retval = CApProtHandler::IsTableProtectedL( iDb->Database() ));
            }
        }

    if ( aCount == 0)
        {
        if ( ( aResourceId == R_APSETTINGSUI_MENU_NORMAL ) ||
            ( aResourceId == R_APSETTINGSUI_MENU_SELECT_NORMAL ) )
            {
            retval = ETrue;
            }
        }

    noEdit = ( iNoEdit )
        && ( ( aResourceId == R_APSETTINGSUI_MENU_SELECT_NORMAL ) ||
             ( aResourceId == R_APSETTINGSUI_MENU_NORMAL ) );

    if (noEdit)
        {
        retval = ETrue;
        }
    APSETUILOGGER_LEAVEFN( EModel,"Model::Need2DeleteNewUseExisting")    
    return retval;
    }


// ---------------------------------------------------------
// CApSettingsModel::Need2DeleteNewL
// called when menu needs to be displayed
// ---------------------------------------------------------
//
TBool CApSettingsModel::Need2DeleteNewL(  TInt aResourceId )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::Need2DeleteNewL")
    TBool noEdit( EFalse );
    TBool retval( EFalse );

    if ( iProtectionSupported )
        {
        if (  aResourceId == R_APSETTINGSUI_MENU_SELECT_NORMAL  ||
                aResourceId == R_APSETTINGSUI_MENU_NORMAL )
            {      
            if ( CApProtHandler::IsTableProtectedL( iDb->Database() ))
                {
                retval = ETrue;
                }
            }
        }

    if ( iVariant & KApUiEditOnlyVPNs )
        {
        if ( ( aResourceId == R_APSETTINGSUI_MENU_SELECT_NORMAL ) ||
             ( aResourceId == R_APSETTINGSUI_MENU_NORMAL ) 
           )
            {
            retval = ETrue;
            }
        }

    noEdit = ( iNoEdit )
        && ( ( aResourceId == R_APSETTINGSUI_MENU_SELECT_NORMAL ) ||
             ( aResourceId == R_APSETTINGSUI_MENU_NORMAL ) );

    if (noEdit)
        {
        retval = ETrue;
        }
    APSETUILOGGER_LEAVEFN( EModel,"Model::Need2DeleteNewL")    
    return retval;
    }



// ---------------------------------------------------------
// CApSettingsModel::Need2DeleteHelp
// called when menu needs to be displayed
// ---------------------------------------------------------
//
TBool CApSettingsModel::Need2DeleteHelp(  TInt aResourceId )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::Need2DeleteHelp")
    TBool retval( EFalse );

	if ( !iHelpSupported )
		{
	    if ( aResourceId == R_APSETTINGSUI_MENU_NORMAL )
	        {
	        retval = ETrue;
	        }
		}
    
    APSETUILOGGER_LEAVEFN( EModel,"Model::Need2DeleteHelp")    
    return retval;
    }









// ---------------------------------------------------------
// CApSettingsModel::GetLinkedVpnAccessPointLC
// ---------------------------------------------------------
//
HBufC* CApSettingsModel::GetLinkedVpnAccessPointLC( TUint32 aUid )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::GetLinkedVpnAccessPointLC")
    HBufC* retval = HBufC::NewLC( KCommsDbSvrMaxFieldLength );    
    
    // Get iap for the above WAP, aUid
    TUint32 homeiap = iUtils->IapIdFromWapIdL( aUid );

    // now get ALL VPN_SERVICE entry ID where HOME_IAP == iap
    // Select * from VPN_SERVICE where HOME_IAP == homeiap
    CCommsDbTableView* t;
    t = iDb->Database()->OpenViewMatchingUintLC( TPtrC(VPN_SERVICE), 
                                                 TPtrC(VPN_SERVICE_IAP), 
                                                 homeiap );
    TInt err = t->GotoFirstRecord();
    // if not found, no such, simply exit, no change in passed array
    if ( err != KErrNotFound )
        {
        User::LeaveIfError( err );
        TPtr sgd = retval->Des();
        t->ReadTextL( TPtrC(COMMDB_NAME), sgd );
        CleanupStack::PopAndDestroy( t );//t
        }
    else
        {
        CleanupStack::PopAndDestroy( 2, retval );// retval, t
        retval = NULL;
        }
    
    APSETUILOGGER_LEAVEFN( EModel,"Model::GetLinkedVpnAccessPointLC")    
    return retval;
    }



// ---------------------------------------------------------
// CApSettingsModel::EikEnv
// ---------------------------------------------------------
//
CEikonEnv* CApSettingsModel::EikEnv()
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::EikEnv<->")
    if ( !iEikEnv )
        {
        iEikEnv = CEikonEnv::Static();
        }
    return iEikEnv;
    }



// ---------------------------------------------------------
// CApSettingsModel::CreateFromDataL
// ---------------------------------------------------------
//
TUint32 CApSettingsModel::CreateFromDataL( CApAccessPointItem& aApItem )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::CreateFromDataL")
    TTransactionResult ownTransaction = StartTransactionLC( /*aWrite*/EFalse,
                                               /*aShowNote*/ETrue,
                                               /*aRetry*/ETrue );
    if ( ownTransaction != EFailedWithLocked )
        {
        iApData->CreateFromDataL( aApItem );    // adds normal ap
        // write wlan data, only writes if WLAN ap.
        WriteWlanL( aApItem, ETrue );

        if ( ownTransaction == EOwnTransaction )
            {
            TInt err = CommitTransaction();
            User::LeaveIfError( err );
            CleanupStack::Pop(); // RollbackTransactionOnLeave
            }
        }

    APSETUILOGGER_LEAVEFN( EModel,"Model::CreateFromDataL")    
    return aApItem.WapUid();
    }





// ---------------------------------------------------------
// CApSettingsModel::ChangeWepSettingsL
// ---------------------------------------------------------
//
TInt CApSettingsModel::ChangeWepSettingsL( CApAccessPointItem* aApItem )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::ChangeWepSettingsL")
    TInt retval(0);
        
    if ( !iWepSecSettings )
        {
        iWepSecSettings = CWEPSecuritySettings::NewL();
        TUint32 iapid( 0 );
        aApItem->ReadUint( EApIapServiceId, iapid );
        iWepSecSettings->LoadL( iapid, *Database()->Database() );        
        }

    if ( !iWepSecSettingsUi )
        {
        iWepSecSettingsUi = CWEPSecuritySettingsUi::NewL( *EikEnv() );
        }

    iWepUiExitReason = iWepSecSettings->EditL( *iWepSecSettingsUi, 
                                               aApItem->ConnectionName() );
                                               
    if ( iWepUiExitReason & CWEPSecuritySettings::EExitReq )
        {
        retval += KApUiEventExitRequested;
        }
    if ( iWepUiExitReason & CWEPSecuritySettings::EShutDownReq )
        {
        retval += KApUiEventShutDownRequested;
        }
    APSETUILOGGER_LEAVEFN( EModel,"Model::ChangeWepSettingsL")
    return retval;
    }



// ---------------------------------------------------------
// CApSettingsModel::ChangeWpaSettingsL
// ---------------------------------------------------------
//
TInt CApSettingsModel::ChangeWpaSettingsL( CApAccessPointItem* aApItem )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::ChangeWpaSettingsL")
    TInt retval(0);

    if ( !iWpaSecSettings )
        {
        iWpaSecSettings = CWPASecuritySettings::NewL( ESecurityModeWpa );
        TUint32 iapid( 0 );
        aApItem->ReadUint( EApIapServiceId, iapid );
        iWpaSecSettings->LoadL( iapid, *Database()->Database() );
        }

    if ( !iWpaSecSettingsUi )
        {
        iWpaSecSettingsUi = CWPASecuritySettingsUi::NewL( *EikEnv() );
        }

    iWpaUiExitReason = iWpaSecSettings->EditL( *iWpaSecSettingsUi, 
                                               aApItem->ConnectionName() );
                                               
    if ( iWpaUiExitReason & CWPASecuritySettings::EExitReq )
        {
        retval += KApUiEventExitRequested;
        }
    if ( iWpaUiExitReason & CWPASecuritySettings::EShutDownReq )
        {
        retval += KApUiEventShutDownRequested;
        }
    APSETUILOGGER_LEAVEFN( EModel,"Model::ChangeWpaSettingsL")    
    return retval;
    }



// ---------------------------------------------------------
// CApSettingsModel::Change8021xSettingsL
// ---------------------------------------------------------
//
TInt CApSettingsModel::Change8021xSettingsL( CApAccessPointItem* aApItem )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::Change8021xSettingsL")
    TInt retval(0);

    if ( !iWpaSecSettings )
        {
        iWpaSecSettings = CWPASecuritySettings::NewL( ESecurityMode8021x );
        TUint32 iapid( 0 );
        aApItem->ReadUint( EApIapServiceId, iapid );
        iWpaSecSettings->LoadL( iapid, *Database()->Database() );        
        }
        
    if ( !iWpaSecSettingsUi )
        {
        iWpaSecSettingsUi = CWPASecuritySettingsUi::NewL( *EikEnv() );
        }
        
    iWpaUiExitReason = iWpaSecSettings->EditL( *iWpaSecSettingsUi, 
                                               aApItem->ConnectionName() );

    if ( iWpaUiExitReason & CWPASecuritySettings::EExitReq )
        {
        retval += KApUiEventExitRequested;
        }
    if ( iWpaUiExitReason & CWPASecuritySettings::EShutDownReq )
        {
        retval += KApUiEventShutDownRequested;
        }
    APSETUILOGGER_LEAVEFN( EModel,"Model::Change8021xSettingsL")       
    return retval;
    }



// ---------------------------------------------------------
// CApSettingsModel::ClearWEPAndWPASettings
// ---------------------------------------------------------
//
void CApSettingsModel::ClearWEPAndWPASettings()
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::ClearWEPAndWPASettings")
    
    delete iWepSecSettings;
    iWepSecSettings = NULL;

    delete iWpaSecSettings;
    iWpaSecSettings = NULL;
    
    APSETUILOGGER_LEAVEFN( EModel,"Model::ClearWEPAndWPASettings") 
    }



// ---------------------------------------------------------
// CApSettingsModel::WriteWlanL
// ---------------------------------------------------------
//
void CApSettingsModel::WriteWlanL( CApAccessPointItem& aApItem,
                                   TBool aIsNew )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::WriteWlanL")
    // now check if it is WLAN and handle security savings...
    if ( aApItem.BearerTypeL() == EApBearerTypeWLAN )
        { // it is WLAN...
        // check that there were no bearer-change, because if orig. bearer 
        // was NOT wlan, then we shouls consider it as a new record!!!
        TUint32 wlanid(0);
        aApItem.ReadUint( EApIapServiceId, wlanid );        
        
        TUint32 oldwlanid(wlanid);
        
        TTypeOfSaving savetype( ESavingEditedAP );
        
        if ( aIsNew )
            {
            savetype = ESavingBrandNewAP;
            oldwlanid = 0;
            }
        // now check if it is WEP...
        // read up security mode
        TUint32 secmode( 0 );
        aApItem.ReadUint( EApWlanSecurityMode, secmode );
        
        
        TBool fresh(EFalse); 
        
        
        switch ( secmode )
            {
            case EOpen:
                {
                break;
                }
            case EWep:
                {
                // we have to try to save if:
                // 1. it is a changed AP, it is possible that only 
                // sec. settings have been changed. 
                // In this case, iWepSecSettings might be NULL!!
                // 2. it's sec. settings had been edited
                
                if ( !iWepSecSettings )
                    {
                    iWepSecSettings = CWEPSecuritySettings::NewL();
                    fresh = ETrue;
                    iWepSecSettings->LoadL( oldwlanid, 
                                            *Database()->Database() );
                    }
                
                if ( ( iWepUiExitReason & CWEPSecuritySettings::EValid ) 
                    || ( fresh ) )
                    { // save WEP settings, too, into same transaction...
                    iWepSecSettings->SaveL( wlanid, *iDb->Database() );
                    }
                break;
                }
            case E802_1x:
                {
                
                if ( !iWpaSecSettings )
                    {
                    iWpaSecSettings = 
                        CWPASecuritySettings::NewL( ESecurityMode8021x );
                    fresh = ETrue;
                    iWpaSecSettings->LoadL( oldwlanid, 
                                            *Database()->Database() );
                    }                
                
                if ( ( iWpaUiExitReason & CWPASecuritySettings::EValid )
                    || ( fresh ) )
                    { // save WPA settings, too, into same transaction...
                    iWpaSecSettings->SaveL( wlanid, *iDb->Database(), 
                                            savetype, oldwlanid );
                    }
                break;
                }
            case EWpa:
            case EWpa2:
                {
                
                if ( !iWpaSecSettings )
                    {
                    iWpaSecSettings = 
                          CWPASecuritySettings::NewL( ESecurityModeWpa );
                    fresh = ETrue;
                    iWpaSecSettings->LoadL( oldwlanid, 
                                            *Database()->Database() );
                    }                
                
                if ( ( iWpaUiExitReason & CWPASecuritySettings::EValid )
                    || ( fresh ) )
                    { // save WPA settings, too, into same transaction...
                    iWpaSecSettings->SaveL( wlanid, *iDb->Database(),
                                            savetype, oldwlanid );
                    }
                break;
                }
            default:
                {
                __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
                // do nothing in urel
                break;
                }
            }
        }
    APSETUILOGGER_LEAVEFN( EModel,"Model::WriteWlanL")    
    }





// ---------------------------------------------------------
// CApSettingsModel::LoadWlanL
// ---------------------------------------------------------
//
void CApSettingsModel::LoadWlanL( CApAccessPointItem& aApItem )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::LoadWlanL")
    // now check if it is WLAN and handle security savings...
    if ( aApItem.BearerTypeL() == EApBearerTypeWLAN )
        { // it is WLAN...
        // now check if it is WEP...
        // read up security mode
        TUint32 secmode( 0 );
        aApItem.ReadUint( EApWlanSecurityMode, secmode );
        switch ( secmode )
            {
            case EOpen:
                {
                break;
                }
            case EWep:
                {
                if ( !iWepSecSettings )
                    {
                    iWepSecSettings = CWEPSecuritySettings::NewL();
                    }
    
                if ( !iWepSecSettingsUi )
                    {
                    iWepSecSettingsUi = 
                        CWEPSecuritySettingsUi::NewL( *EikEnv() );
                    }
                TUint32 wlanid( 0 );
                aApItem.ReadUint( EApIapServiceId, wlanid );
                iWepSecSettings->LoadL( wlanid, *Database()->Database() );
                break;
                }
            case E802_1x:
                {
                if ( !iWpaSecSettings )
                    {
                    iWpaSecSettings = 
                        CWPASecuritySettings::NewL( ESecurityMode8021x );
                    }
    
                if ( !iWpaSecSettingsUi )
                    {
                    iWpaSecSettingsUi = 
                        CWPASecuritySettingsUi::NewL( *EikEnv() );
                    }
                TUint32 wlanid( 0 );
                aApItem.ReadUint( EApIapServiceId, wlanid );
                iWpaSecSettings->LoadL( wlanid, *Database()->Database() );
                break;
                }
            case EWpa:
            case EWpa2:
                {
                if ( !iWpaSecSettings )
                    {
                    iWpaSecSettings = 
                        CWPASecuritySettings::NewL( ESecurityModeWpa );
                    }
    
                if ( !iWpaSecSettingsUi )
                    {
                    iWpaSecSettingsUi = 
                        CWPASecuritySettingsUi::NewL( *EikEnv() );
                    }
                TUint32 wlanid( 0 );
                aApItem.ReadUint( EApIapServiceId, wlanid );
                iWpaSecSettings->LoadL( wlanid, *Database()->Database() );
                break;
                }
            default:
                {
                __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
                // do nothing in urel
                break;
                }
            }
        }
    APSETUILOGGER_LEAVEFN( EModel,"Model::LoadWlanL")    
    }





//----------------------------------------------------------
// CApSettingsModel::HasWlanSecSettingsFilledL
//----------------------------------------------------------
//
TBool CApSettingsModel::HasWlanSecSettingsFilledL( CApAccessPointItem& aApItem )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::HasWlanSecSettingsFilledL")
    TBool retval(EFalse);
    
    TUint32 secmode( 0 );
    aApItem.ReadUint( EApWlanSecurityMode, secmode );
    
    TUint32 wlanid(0);
    aApItem.ReadUint( EApIapServiceId, wlanid );        
        
    TUint32 oldwlanid(wlanid);

    TUint32 wapuid = aApItem.WapUid();

    if ( ( wapuid == KApNoneUID )
        || ( iUtils->BearerTypeL( wapuid ) != EApBearerTypeWLAN ) )
        {
        oldwlanid = 0;
        }

    switch ( secmode )
        {
        case EOpen:
            {
            retval = ETrue;
            break;
            }
        case EWep:
            {
            if ( !iWepSecSettings )
                {
                iWepSecSettings = CWEPSecuritySettings::NewL();
                iWepSecSettings->LoadL( oldwlanid, 
                                        *Database()->Database() );
                }
            retval = iWepSecSettings->IsValid();
            break;
            }
        case E802_1x:
            {
            if ( !iWpaSecSettings )
                {
                iWpaSecSettings = 
                    CWPASecuritySettings::NewL( ESecurityMode8021x );
                iWpaSecSettings->LoadL( oldwlanid, 
                                        *Database()->Database() );
                }
            retval = iWpaSecSettings->IsValid();
            break;
            }
        case EWpa:
        case EWpa2:
            {
            if ( !iWpaSecSettings )
                {
                iWpaSecSettings = 
                      CWPASecuritySettings::NewL( ESecurityModeWpa );
                iWpaSecSettings->LoadL( oldwlanid, 
                                        *Database()->Database() );
                }                
            retval = iWpaSecSettings->IsValid();                
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
            // do nothing in urel
            break;
            }
        }
    APSETUILOGGER_LEAVEFN( EModel,"Model::HasWlanSecSettingsFilledL")    
    return retval;
    }


//----------------------------------------------------------
// CApSettingsModel::UpdateSecurityModeL
//----------------------------------------------------------
//
void CApSettingsModel::UpdateSecurityModeL( CApAccessPointItem& aApItem )
    {
    APSETUILOGGER_ENTERFN( EModel,"Model::UpdateSecurityModeL")
    TUint32 secmode( 0 );
    aApItem.ReadUint( EApWlanSecurityMode, secmode );

    TUint32 wlanid(0);
    aApItem.ReadUint( EApIapServiceId, wlanid );

    switch ( secmode )
        {
        case EOpen:
            {
            break;
            }
        case EWep:
            {
            if ( iWepSecSettings )
                {
                delete iWepSecSettings;
                iWepSecSettings = NULL; // to satisfy CodeScanner
                }
            iWepSecSettings = CWEPSecuritySettings::NewL();
            iWepSecSettings->LoadL( wlanid, 
                                    *Database()->Database() );
            break;
            }
        case E802_1x:
            {
            if ( iWpaSecSettings )
                {
                delete iWpaSecSettings;
                iWpaSecSettings = NULL; // to satisfy CodeScanner
                }
            iWpaSecSettings = 
                CWPASecuritySettings::NewL( ESecurityMode8021x );
            iWpaSecSettings->LoadL( wlanid, 
                                    *Database()->Database() );
            break;
            }
        case EWpa:
        case EWpa2:
            {
            if ( iWpaSecSettings )
                {
                delete iWpaSecSettings;
                iWpaSecSettings = NULL; // to satisfy CodeScanner
                }
            iWpaSecSettings = 
                  CWPASecuritySettings::NewL( ESecurityModeWpa );
            iWpaSecSettings->LoadL( wlanid, 
                                    *Database()->Database() );
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
            // do nothing in urel
            break;
            }
        }
    APSETUILOGGER_LEAVEFN( EModel,"Model::UpdateSecurityModeL")    
    }

// End of File
