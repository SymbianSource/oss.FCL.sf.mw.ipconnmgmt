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
* Description: 
*     Defines the main handler, CApSettingsHandlerImpl for 
*       the Access Point settings.
*
*/


// INCLUDE FILES
#include "APSettingsHandlerUIVariant.hrh"

#include <bautils.h>
#include <StringLoader.h>
#include <apsetui.rsg>
#include <ActiveApDb.h>

#include "ApsettingshandleruiImpl.h"
#include "ApSettingsHandlerUI.hrh"
#include "ApSelectorDialog.h"

#include "ApSettingsDlg.h"

#include "TextOverrides.h"
#include "ApSettingsModel.h"
#include "ApSelPopupList.h"
#include "ApSelQueryDialog.h"

#include <aknnotedialog.h>
#include <ApListItemList.h>

#include <f32file.h>

#include "ApSettingsHandlerLogger.h"
#include <ApSettingsHandlerCommons.h>


#include "ApSettingsHandlerUiPrivateCRKeys.h"
#include <centralrepository.h>

#include <sysutil.h>

#include <ApNetworkItem.h>

#include <featmgr.h>
#include <VpnApEngine.h>

#include <data_caging_path_literals.hrh>

#include <shareddataclient.h>

// UID of general settings app, in which help texts are included
const   TUid    KHelpUidApSettingsHandler = { 0x100058EC };


//#define __TEST_PERFORMANCE
/// ROM drive.
_LIT( KApSettingsResDriveZ, "z:" );
/// Access point settings handler UI resource file name.
_LIT( KApSettingsResFileName, "apsetui.rsc" );



/// Estimated FFS overhead for deleting (in bytes).
LOCAL_C const TInt KEstimatedDeleteFfsOverhead = 48 * 1024;



//KFeatureIdIPv6
// #define KFeatureIdIPv6 KOtherFeatureFirst+88

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CApSettingsHandlerImpl::NewLC
// ---------------------------------------------------------
//
CApSettingsHandlerImpl* CApSettingsHandlerImpl::NewLC(
                                         TBool aStartWithSelection,
                                         TSelectionListType aListType,
                                         TSelectionMenuType aSelMenuType,
                                         TInt aIspFilter,
                                         TInt aBearerFilter,
                                         TInt aSortType,
                                         TBool aNoEdit )
    {
    APSETUILOGGER_CREATE;
    
    CApSettingsHandlerImpl* db = 
            new( ELeave ) CApSettingsHandlerImpl( aStartWithSelection,
                                              aListType,
                                              aSelMenuType,
                                              aNoEdit );
    CleanupStack::PushL( db );

    db->ConstructL( aIspFilter, aBearerFilter, aSortType );
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::NewLC")
    
    return db;
    }




// ---------------------------------------------------------
// CApSettingsHandlerImplImpl::NewLC
// ---------------------------------------------------------
//
CApSettingsHandlerImpl* CApSettingsHandlerImpl::NewLC(
                                            CActiveApDb& aDb,
                                            TBool aStartWithSelection,
                                            TSelectionListType aListType,
                                            TSelectionMenuType aSelMenuType,
                                            TInt aIspFilter,
                                            TInt aBearerFilter,
                                            TInt aSortType,
                                            TBool aNoEdit
                                            )
    {
    APSETUILOGGER_CREATE;

    CApSettingsHandlerImpl* db = 
            new( ELeave ) CApSettingsHandlerImpl( aStartWithSelection,
                                              aListType,
                                              aSelMenuType,
                                              aNoEdit );
    CleanupStack::PushL( db );

    db->ConstructL( aDb, aIspFilter, aBearerFilter, aSortType );
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::NewLC2")
    
    return db;
    }



// ---------------------------------------------------------
// CApSettingsHandlerImpl::NewLC
// ---------------------------------------------------------
//
CApSettingsHandlerImpl* CApSettingsHandlerImpl::NewLC(
                                         TBool aStartWithSelection,
                                         TSelectionListType aListType,
                                         TSelectionMenuType aSelMenuType,
                                         TInt aIspFilter,
                                         TInt aBearerFilter,
                                         TInt aSortType,
                                         TInt aReqIpvType,
                                         TBool aNoEdit
                                         )
    {
    APSETUILOGGER_CREATE;
    
    CApSettingsHandlerImpl* db = 
        new( ELeave ) CApSettingsHandlerImpl( aStartWithSelection,
                                          aListType,
                                          aSelMenuType,
                                          aReqIpvType,
                                          aNoEdit
                                          );
    CleanupStack::PushL( db );
    
    db->ConstructL( aIspFilter, aBearerFilter, aSortType );
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::NewLC3")
    
    return db;
    }




// ---------------------------------------------------------
// CApSettingsHandlerImpl::NewLC
// ---------------------------------------------------------
//
CApSettingsHandlerImpl* CApSettingsHandlerImpl::NewLC(
                                            CActiveApDb& aDb,
                                            TBool aStartWithSelection,
                                            TSelectionListType aListType,
                                            TSelectionMenuType aSelMenuType,
                                            TInt aIspFilter,
                                            TInt aBearerFilter,
                                            TInt aSortType,
                                            TInt aReqIpvType,
                                            TBool aNoEdit
                                            )
    {
    APSETUILOGGER_CREATE;
    
    CApSettingsHandlerImpl* db = 
        new( ELeave ) CApSettingsHandlerImpl( aStartWithSelection,
                                          aListType,
                                          aSelMenuType,
                                          aReqIpvType,
                                          aNoEdit
                                          );
    CleanupStack::PushL( db );

    db->ConstructL( aDb, aIspFilter, aBearerFilter, aSortType );
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::NewLC4")
    
    return db;
    }





// ---------------------------------------------------------
// CApSettingsHandlerImpl::NewLC
// ---------------------------------------------------------
//
CApSettingsHandlerImpl* CApSettingsHandlerImpl::NewLC(
                                        TBool aStartWithSelection,
                                        TSelectionListType aListType,
                                        TSelectionMenuType aSelMenuType,
                                        TInt aIspFilter,
                                        TInt aBearerFilter,
                                        TInt aSortType,
                                        TInt aReqIpvType,
                                        TVpnFilterType aVpnFilterType,
                                        TBool aNoEdit
                                            )
    {
    APSETUILOGGER_CREATE;
    
    CApSettingsHandlerImpl* db = 
        new( ELeave ) CApSettingsHandlerImpl( aStartWithSelection,
                                          aListType,
                                          aSelMenuType,
                                          aReqIpvType,
                                          aNoEdit
                                          );
    CleanupStack::PushL( db );

    db->ConstructL( aIspFilter, aBearerFilter, 
                    aSortType, aVpnFilterType );
                    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::NewLC5")
    
    return db;
    }


// ---------------------------------------------------------
// CApSettingsHandlerImpl::NewLC
// ---------------------------------------------------------
//
CApSettingsHandlerImpl* CApSettingsHandlerImpl::NewLC(
                                            CActiveApDb& aDb,
                                            TBool aStartWithSelection,
                                            TSelectionListType aListType,
                                            TSelectionMenuType aSelMenuType,
                                            TInt aIspFilter,
                                            TInt aBearerFilter,
                                            TInt aSortType,
                                            TInt aReqIpvType,
                                            TVpnFilterType aVpnFilterType,
                                            TBool aNoEdit
                                            )
    {
    APSETUILOGGER_CREATE;
    
    CApSettingsHandlerImpl* db = 
        new( ELeave ) CApSettingsHandlerImpl( aStartWithSelection,
                                          aListType,
                                          aSelMenuType,
                                          aReqIpvType,
                                          aNoEdit
                                          );
    CleanupStack::PushL( db );

    db->ConstructL( aDb, aIspFilter, aBearerFilter, 
                    aSortType, aVpnFilterType );
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::NewLC6")
    
    return db;
    }






// ---------------------------------------------------------
// CApSettingsHandlerImpl::NewLC
// ---------------------------------------------------------
//
CApSettingsHandlerImpl* CApSettingsHandlerImpl::NewLC(
                                        CActiveApDb& aDb,
                                        TBool aStartWithSelection,
                                        TSelectionListType aListType,
                                        TSelectionMenuType aSelMenuType,
                                        TInt aIspFilter,
                                        TInt aBearerFilter,
                                        TInt aSortType,
                                        TInt aReqIpvType,
                                        TVpnFilterType aVpnFilterType,
                                        TBool aIncludeEasyWlan,
                                        TBool aNoEdit
                                        )
    {
    APSETUILOGGER_CREATE;
    
    CApSettingsHandlerImpl* db = 
        new( ELeave ) CApSettingsHandlerImpl( aStartWithSelection,
                                          aListType,
                                          aSelMenuType,
                                          aReqIpvType,
                                          aNoEdit
                                          );
    CleanupStack::PushL( db );
    db->ConstructL( aDb, aIspFilter, aBearerFilter, 
                    aSortType, aVpnFilterType, aIncludeEasyWlan );
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::NewLC7")
    
    return db;    
    }



// Destructor
CApSettingsHandlerImpl::~CApSettingsHandlerImpl()
    {
    APSETUILOGGER_DELETE;

    if ( iTextOverrides )
        {
        delete iTextOverrides;
        iTextOverrides = NULL;
        APSETUILOGGER_WRITE( EAPI,"EApImpl::txtoverrides deleted");
        }
    if ( iExt )
        {
        if ( iExt->iResOffset )
            {
            iModel->EikEnv()->DeleteResourceFile( iExt->iResOffset );
            APSETUILOGGER_WRITE( EAPI,"EApImpl::Resource file deleted");
            }
        if ( iExt->iIsFeatureManagerInitialised )
            {
            FeatureManager::UnInitializeLib();
            APSETUILOGGER_WRITE( EAPI,"EApImpl::FeatMan. uninitialised");
            }
        }

    if ( iModel )
        {
        delete iModel;
        iModel = NULL;
        }

    APSETUILOGGER_WRITE( EAPI,"EApImpl::imodel deleted");

    delete iExt;
    
    APSETUILOGGER_WRITE( EAPI,"EApImpl::iext deleted");
    }


// Constructor
CApSettingsHandlerImpl::CApSettingsHandlerImpl( TBool aStartWithSelection,
                                                TSelectionListType aListType,
                                                TSelectionMenuType aSelMenuType,
                                                TBool aNoEdit
                                                )
:iStartWithSelection( aStartWithSelection ),
iListType( aListType ),
iSelMenuType( aSelMenuType ),
iIspFilter( KEApIspTypeAll ),
iBearerFilter( EApBearerTypeAll ),
iReqIpvType( EIPv4 ),
iEventStore( KApUiEventNone ),
iNoEdit( aNoEdit )
    {
    iHelpMajor = KHelpUidApSettingsHandler;
    }





// Constructor
CApSettingsHandlerImpl::CApSettingsHandlerImpl( TBool aStartWithSelection,
                                        TSelectionListType aListType,
                                        TSelectionMenuType aSelMenuType,
                                        TInt aReqIpvType,
                                        TBool aNoEdit 
                                        )
:iStartWithSelection( aStartWithSelection ),
iListType( aListType ),
iSelMenuType( aSelMenuType ),
iIspFilter( KEApIspTypeAll ),
iBearerFilter( EApBearerTypeAll ),
iReqIpvType( aReqIpvType ),
iEventStore( KApUiEventNone ),
iNoEdit( aNoEdit )
    {
    iHelpMajor = KHelpUidApSettingsHandler;
    }



// ---------------------------------------------------------
// CApSettingsHandlerImpl::ConstructL
// ---------------------------------------------------------
//
void CApSettingsHandlerImpl::ConstructL(   TInt aIspFilter,
                                                TInt aBearerFilter,
                                                TInt aSortType )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::ConstructL")
    
    DoConstructL( NULL, aIspFilter, aBearerFilter, 
                  aSortType, EVpnFilterBoth, EFalse );
                  
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::ConstructL")
    }



// ---------------------------------------------------------
// CApSettingsHandlerImpl::ConstructL
// ---------------------------------------------------------
//
void CApSettingsHandlerImpl::ConstructL(   CActiveApDb& aDb,
                                                TInt aIspFilter,
                                                TInt aBearerFilter,
                                                TInt aSortType )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::ConstructL2")
    
    DoConstructL( &aDb, aIspFilter, aBearerFilter, 
                  aSortType, EVpnFilterBoth, EFalse );

    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::ConstructL2")    
    }



// ---------------------------------------------------------
// CApSettingsHandlerImpl::ConstructL
// ---------------------------------------------------------
//
void CApSettingsHandlerImpl::ConstructL(   
                                             TInt aIspFilter,
                                             TInt aBearerFilter,
                                             TInt aSortType,
                                             TVpnFilterType aVpnFilterType
                                            )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::ConstructL3")
    
    DoConstructL( NULL, aIspFilter, aBearerFilter, 
                  aSortType, aVpnFilterType, EFalse );
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::ConstructL3")
    }



// ---------------------------------------------------------
// CApSettingsHandlerImpl::ConstructL
// ---------------------------------------------------------
//
void CApSettingsHandlerImpl::ConstructL(
                                             CActiveApDb& aDb,
                                             TInt aIspFilter,
                                             TInt aBearerFilter,
                                             TInt aSortType,
                                             TVpnFilterType aVpnFilterType
                                             )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::ConstructL4")
    
    DoConstructL( &aDb, aIspFilter, aBearerFilter, 
                  aSortType, aVpnFilterType, EFalse );
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::ConstructL4")
    }




// ---------------------------------------------------------
// CApSettingsHandlerImpl::ConstructL
// ---------------------------------------------------------
//
void CApSettingsHandlerImpl::ConstructL(
                                        CActiveApDb& aDb,
                                        TInt aIspFilter,
                                        TInt aBearerFilter,
                                        TInt aSortType,
                                        TVpnFilterType aVpnFilterType,
                                        TBool aIncludeEasyWlan
                                        )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::ConstructL5")
    
    DoConstructL( &aDb, aIspFilter, aBearerFilter, 
                  aSortType, aVpnFilterType, aIncludeEasyWlan );
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::ConstructL5")
    }


// ---------------------------------------------------------
// CApSettingsHandlerImpl::SetFilter
// ---------------------------------------------------------
//
void CApSettingsHandlerImpl::SetFilterL(   TInt aIspFilter,
                                                TInt aBearerFilter,
                                                TInt aSortType )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::SetFilterL")
    
    iIspFilter = aIspFilter;
    iBearerFilter = aBearerFilter;
    iExt->iSortType = aSortType;
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::SetFilterL")
    }




// ---------------------------------------------------------
// CApSettingsHandlerImpl::SetTextOverrideL
// ---------------------------------------------------------
//
void CApSettingsHandlerImpl::SetTextOverrideL( TTextID aText2Change,
                                                    const TDesC& aNewText )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::SetTextOverrideL")
    
    __ASSERT_DEBUG( iTextOverrides, Panic( ENullPointer ) );
    iTextOverrides->SetTextOverrideL( aText2Change, aNewText );
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::SetTextOverrideL")
    }


// ---------------------------------------------------------
// CApSettingsHandlerImpl::TextOverrideL
// ---------------------------------------------------------
//
const TDesC& CApSettingsHandlerImpl::TextOverrideL( TTextID aTextId )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::TextOverrideL <- ->")
    
    __ASSERT_DEBUG( iTextOverrides, Panic( ENullPointer ) );
    return iTextOverrides->TextOverrideL( aTextId );
    }



// ---------------------------------------------------------
// CApSettingsHandlerImpl::RunSettingsL
// ---------------------------------------------------------
//
TInt CApSettingsHandlerImpl::RunSettingsL( TUint32 aHighLight,
                                                TUint32& aSelected )
    {    
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::RunSettingsL")
    
    // this function is only called from outside, need to init iEventStore
    iEventStore = KApUiEventNone;

    TRAPD( err, HandleRunSettingsL( aHighLight, aSelected ) );
    if ( iEventStore & KApUiEventInternal )
        {
        // internal 'exit', not an error, 
        // clear flag and return normally
        iEventStore &= (~KApUiEventInternal);
        }
    else
        {
        User::LeaveIfError(err);
        }

    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::RunSettingsL")

    return iEventStore;
    }



// ---------------------------------------------------------
// CApSettingsHandlerImpl::Cvt()
// ---------------------------------------------------------
//
TInt CApSettingsHandlerImpl::Cvt()
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::Cvt")
    
    return KErrNone;
    }


// ---------------------------------------------------------
// CApSettingsHandlerImpl::DoRunSettingsL
// ---------------------------------------------------------
//
TInt CApSettingsHandlerImpl::DoRunSettingsL( TUint32 aHighLight,
                                         TUint32& aSelected )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::DoRunSettingsL")
    
    __ASSERT_DEBUG( iTextOverrides, Panic( ENullPointer ) );

    if ( ( iListType == EApSettingsSelListIsPopUp ) ||
         ( iListType == EApSettingsSelListIsPopUpWithNone ) )
        {
        DoRunSetPageSettingsL( aHighLight, aSelected );
        }
    else
        {
        DoRunListSettingsL( aHighLight, aSelected );
        }
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::DoRunSettingsL")
    return 0;
    }


// ---------------------------------------------------------
// CApSettingsHandlerImpl::RunViewerL
// ---------------------------------------------------------
//
TInt CApSettingsHandlerImpl::RunViewerL( TUint32 aUid )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::RunViewerL")
    
    // this function is only called from outside, need to init iEventStore
    iEventStore = KApUiEventNone;

#ifdef __TEST_OOMDEBUG
    if ( iMemTestOn )
        {
        TInt leavecode( KErrNoMemory );
        TInt ts( GetTestStateL() );

        TBuf<48> buf;
        buf.Format( _L("\r\n\t( ts: %d )"), ts );
        APSETUILOGGER_WRITE_BUF( EApImpl, buf );

        if ( ts == KViewerTest )
            {
            TBuf<48> buf;
            APSETUILOGGER_WRITE_BUF( EApImpl, _L("Starting KViewerTest memtest") );

            TUint32 rate( 0 );
            TUint32 retrycount( KOOMRetryCount );
            TUint32 retryoffset( KOOMRetryOffset );
            for (
                rate = 1;
                ((rate < 50000) && (leavecode == KErrNoMemory))
                    || retrycount;
                rate++
                )
                {
                __UHEAP_SETFAIL( RHeap::EDeterministic, rate );
                TRAP( leavecode, DoRunViewerL( aUid ) );
                __UHEAP_RESET;
                if ( !leavecode )
                    {
                    retrycount--;
                    rate += retryoffset;
                    }
                }
            User::LeaveIfError( leavecode );
            buf.Format( _L("\t Memtest finished, max. rate was: %d )"), rate );
            APSETUILOGGER_WRITE_BUF( EApImpl, buf );
            }
        else
            {
            DoRunViewerL( aUid );
            }
        }
    else
        {
        DoRunViewerL( aUid );
        }
#else
    DoRunViewerL( aUid );
#endif // __TEST_OOMDEBUG

    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::RunViewerL")
    
    return iEventStore;
    }



// ---------------------------------------------------------
// CApSettingsHandlerImpl::DoRunViewerL
// ---------------------------------------------------------
//
TInt CApSettingsHandlerImpl::DoRunViewerL( TUint32 aUid )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::DoRunViewerL")
    
    TInt retval( KErrNone );

    TBool protSupported = FeatureManager::FeatureSupported( 
                                           KFeatureIdSettingsProtection );


    TTransactionResult ownTransaction = iModel->StartTransactionLC
                                                    (
                                                    /*aWrite*/EFalse,
                                                    /*aShowNote*/ETrue,
                                                    /*aRetry*/ETrue
                                                    );
    if ( ownTransaction != EFailedWithLocked )
        {
        TBool isinuse = iModel->ApUtils()->IsAPInUseL( aUid );
        TBool isprotected(EFalse);
        if ( protSupported )
            {
            isprotected = iModel->ApUtils()->IsReadOnlyL( aUid );
            isprotected |= CApProtHandler::IsTableProtectedL( 
                                iModel->Database()->Database() );
            }

        TBool isvpn = iModel->VpnEngine()->IsVpnApL( aUid );

        if ( ownTransaction == EOwnTransaction )
            {
            TInt err = iModel->CommitTransaction();
            User::LeaveIfError( err );
            CleanupStack::Pop(); // RollbackTransactionOnLeave
            }

        if ( isinuse )
            {
            // Ap is in use right now, can not edit or delete,
            // show information note qtn_set_note_iap_in_use
            ShowNoteL( R_APUI_NOTE_AP_IN_USE_EDIT );
            }
        else 
            {
            if ( protSupported && isprotected )
                {
                ShowNoteL( R_APUI_NOTE_PROTECTED_AP_SETT );
                }
            else
                { // not in use, not protected, check if it is vpn
                if ( isvpn )
                    {
                    ShowNoteL( R_APUI_NOTE_VPN_AP_SETT );
                    }
                else
                    { // may start wondering about editing...
                    TApBearerType bearer = iModel->ApUtils()->BearerTypeL( aUid );
#ifdef __TEST_LAN_BEARER
                    if ( bearer == EApBearerTypeLAN )
                        {
                        User::Leave( KErrNotSupported );
        //                ShowNoteL( R_APUI_AP_LAN_NOT_SUPPORTED );
                        // Leave needed?
                        }
                    else
                        {
                        CApSettingsDlg* iMyDialog = 
                                CApSettingsDlg::NewL( iIspFilter,
                                                      iBearerFilter,
                                                      iExt->iSortType,
                                                      iEventStore,
                                                      iReqIpvType
                                                      );

                        iMyDialog->SetTextOverrides( iTextOverrides );

                        retval = iMyDialog->ConstructAndRunLD( 
                                                *iModel, *this, aUid );
                        }
#else
                    if ( bearer == EApBearerTypeLANModem )
                        {
                        User::Leave( KErrNotSupported );
        //                ShowNoteL( R_APUI_AP_LAN_NOT_SUPPORTED );
                        // Leave needed?
                        }
                    else
                        {
                        CApSettingsDlg* iMyDialog = 
                                CApSettingsDlg::NewL( iIspFilter,
                                                      iBearerFilter,
                                                      iExt->iSortType,
                                                      iEventStore,
                                                      iReqIpvType
                                                      );

                        iMyDialog->SetTextOverrides( iTextOverrides );

                        retval = iMyDialog->ConstructAndRunLD( 
                                                *iModel, *this, aUid );
                        }
#endif // __TEST_LAN_BEARER
                    }
                }
            }
        }
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::DoRunViewerL")
    
    return retval;
    }



//----------------------------------------------------------
// CApSettingsHandlerImpl::HandleApDeleteCmdL
//----------------------------------------------------------
//
void CApSettingsHandlerImpl::HandleApDeleteCmdL( TUint32 aUid, TBool aIsLast )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::HandleApDeleteCmdL")
    
    // OOD handling. If disk space is low,
    // we try to get some spare.
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL<RFs>( fs );
    TBool diskLow = SysUtil::FFSSpaceBelowCriticalLevelL
            ( &fs, KEstimatedDeleteFfsOverhead );

    RSharedDataClient sdClient;
    if ( diskLow )
        {
        User::LeaveIfError( sdClient.Connect() );
        CleanupClosePushL<RSharedDataClient>( sdClient );
        sdClient.RequestFreeDiskSpaceLC
                    ( KEstimatedDeleteFfsOverhead );
        }
    
        // check that the Ap is not used so it can be deleted!!
    TTransactionResult ownTransaction = iModel->StartTransactionLC(
                                                /*aWrite*/EFalse,
                                                /*aShowNote*/ETrue,
                                                /*aRetry*/ETrue );
    if ( ownTransaction != EFailedWithLocked )
        {
        TBool isinuse = iModel->ApUtils()->IsAPInUseL( aUid );
        TApBearerType bearer = iModel->ApUtils()->BearerTypeL( aUid );
#ifdef __TEST_LAN_BEARER        
        if ( bearer == EApBearerTypeLAN )
            {
            User::Leave( KErrNotSupported );
            }
#endif // __TEST_LAN_BEARER
        if ( bearer == EApBearerTypeLANModem )
            {
            User::Leave( KErrNotSupported );
            // Leave needed?
            }

        TBool protSupported = FeatureManager::FeatureSupported( 
                                               KFeatureIdSettingsProtection );
        TBool isprotected(EFalse);
        if ( protSupported )
            {
            isprotected = iModel->ApUtils()->IsReadOnlyL( aUid );
            }

        if ( ownTransaction == EOwnTransaction )
            {
            TInt err = iModel->CommitTransaction();
            User::LeaveIfError( err );
            CleanupStack::Pop(); // RollbackTransactionOnLeave
            }

        if ( isinuse )
            {
            // Ap is in use right now, can not edit or delete,
            // show information note qtn_set_note_iap_in_use
            ShowNoteL( R_APUI_NOTE_AP_IN_USE_DELETE );
            }
        else
            { // not in use, maybe protected?
            if ( protSupported && isprotected )
                {
                ShowNoteL( R_APUI_NOTE_PROTECTED_AP );
                }
            else
                { // not in use or protected, may start wondering about delete...
                // need to check whether it is the last one...
                TBool candelete( ETrue );
                // Linked to VPN? 
                TBool isvpn = iModel->VpnEngine()->IsVpnApL( aUid );
                if ( !isvpn )
                    {
                    if ( iExt->iVariant & KApUiEditOnlyVPNs )
                        {
                        candelete = EFalse;
                        }
                    else
                        {
                        HBufC* sgd = NULL;
                        sgd = iModel->GetLinkedVpnAccessPointLC( aUid ); 
                        if ( sgd )
                            {// has linked VPN, 
                            // According to current spec. there should
                            // be something like linked to VPN, Delete? 
                            // BUT it causes incomplete access point to exist
                            // hard to handle, therefore last minute change
                            // is that it can not be delted.
                            /*
                            if ( !AskQueryL( R_APUI_IAP_DEL_IAP_ASSOCIATED, sgd ) )
                                {
                                candelete = EFalse;
                                }
                            */
                            ShowNoteL( R_APUI_NOTE_AP_IN_USE_DELETE );
                            candelete = EFalse;
                            
                            CleanupStack::PopAndDestroy( sgd );
                            }
                        else
                            { // simply confirm
                            candelete = ConfirmDeleteL( aUid, aIsLast );
                            }
                        }
                    }
                else
                    { // simply confirm
                    candelete = ConfirmDeleteL( aUid, aIsLast );
                    }
                if ( candelete )
                    { // seems that we can delete it, so let's go on
                    iModel->RemoveApL( aUid );
                    iEventStore |= KApUiEventDeleted;
                    }
                }
            }
        }
    
    if ( diskLow )
        {
        // Cancel RequestFreeDiskSpaceLC (by popping off an destroy);
        // close sdClient.
        CleanupStack::PopAndDestroy( 2 );
        }
    CleanupStack::PopAndDestroy(); // fs, will also close it
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::HandleApDeleteCmdL")
    }


//----------------------------------------------------------
// CApSettingsHandlerImpl::HandleApBlankNewL
//----------------------------------------------------------
//
TUint32 CApSettingsHandlerImpl::HandleApBlankNewL( TInt aBearers, TInt aIsps )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::HandleApBlankNewL")
    
    TUint32 retval( KErrNone );

#ifdef __TEST_OOMDEBUG
    if ( iMemTestOn )
        {
        TInt leavecode( KErrNoMemory );

        TInt ts( GetTestStateL() );

        if ( ts == KCreateBlankTest )
            {
            TBuf<48> buf;
            APSETUILOGGER_WRITE_BUF( EApImpl, _L("Starting KCreateBlankTest memtest") );

            TUint32 rate( 0 );
            TUint32 retrycount( KOOMRetryCount );
            TUint32 retryoffset( KOOMRetryOffset );
            for (
                rate = 1;
                ((rate < 50000) && (leavecode == KErrNoMemory))
                    || retrycount;
                rate++
                )
                {
                __UHEAP_SETFAIL( RHeap::EDeterministic, rate );
                TRAP( leavecode,
                      retval = DoHandleApBlankNewL( aBearers, aIsps ) );
                __UHEAP_RESET;
                if ( !leavecode )
                    {
                    retrycount--;
                    rate += retryoffset;
                    }
                }
            User::LeaveIfError( leavecode );
            buf.Format( _L("\t Memtest finished, max. rate was: %d )"), rate );
            APSETUILOGGER_WRITE_BUF( EApImpl, buf );
            }
        else
            {
            retval = DoHandleApBlankNewL( aBearers, aIsps );
            }
        }
    else
        {
        retval = DoHandleApBlankNewL( aBearers, aIsps );
        }
#else
    retval = DoHandleApBlankNewL( aBearers, aIsps );
#endif // __TEST_OOMDEBUG

    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::HandleApBlankNewL")
    return retval;
    }




//----------------------------------------------------------
// CApSettingsHandlerImpl::HandleApUseOldNewL
//----------------------------------------------------------
//
TUint32 CApSettingsHandlerImpl::HandleApUseOldNewL()
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::HandleApUseOldNewL")
    
    TUint32 retval = KErrNone;

#ifdef __TEST_OOMDEBUG
    if ( iMemTestOn )
        {
        TInt leavecode( KErrNoMemory );
        TInt ts( GetTestStateL() );

        TBuf<48> buf;
        buf.Format( _L("\r\n\t( ts: %d )"), ts );
        APSETUILOGGER_WRITE_BUF( EApImpl, buf );

        if ( ts == KQueryTest )
            {
            TBuf<48> buf;
            APSETUILOGGER_WRITE_BUF( EApImpl, _L("Starting KQueryTest memtest") );
            TUint32 rate( 0 );
            TUint32 retrycount( KOOMRetryCount );
            TUint32 retryoffset( KOOMRetryOffset );
            for (
                rate = 1;
                ((rate < 50000) && (leavecode == KErrNoMemory))
                    || retrycount;
                rate++
                )
                {
                __UHEAP_MARK;
                __UHEAP_SETFAIL( RHeap::EDeterministic, rate );
                TRAP( leavecode, retval = SelectApToCopyL() );
                __UHEAP_RESET;
                __UHEAP_MARKEND;
                if ( !leavecode )
                    {
                    retrycount--;
                    rate += retryoffset;
                    }
                }
            User::LeaveIfError( leavecode );
            buf.Format( _L("\t Memtest finished, max. rate was: %d )"), rate );
            APSETUILOGGER_WRITE_BUF( EApImpl, buf );
            }
        else
            {
            retval = SelectApToCopyL();
            }
        }
    else
        {
        retval = SelectApToCopyL();
        }
#else
    retval = SelectApToCopyL();
#endif // __TEST_OOMDEBUG

    if ( retval != (TUint32)KErrNone )
        { // so user had chosen something
#ifdef __TEST_OOMDEBUG
        if ( iMemTestOn )
            {
            TInt leavecode( KErrNoMemory );
            TInt ts( GetTestStateL() );
            if ( ts == KCreateUseExistTest )
                {
                TBuf<48> buf;
                APSETUILOGGER_WRITE_BUF
                    ( _L("Starting KCreateUseExistTest memtest") );
                TUint32 rate( 0 );
                TUint32 retrycount( KOOMRetryCount );
                TUint32 retryoffset( KOOMRetryOffset );
                for (
                    rate = 1;
                    ((rate < 50000) && (leavecode == KErrNoMemory))
                        || retrycount;
                    rate++
                    )
                    {
                    __UHEAP_SETFAIL( RHeap::EDeterministic, rate );
                    TRAP( leavecode, retval = DoHandleApUseOldNewL( retval ) );
                    __UHEAP_RESET;
                    if ( !leavecode )
                        {
                        retrycount--;
                        rate += retryoffset;
                        }
                    }
                User::LeaveIfError( leavecode );
                buf.Format( _L("\t Memtest finished, max. rate was: %d )"),
                            rate );
                APSETUILOGGER_WRITE_BUF( EApImpl, buf );
                }
            else
                {
                retval = DoHandleApUseOldNewL( retval );
                }
            }
        else
            {
            retval = DoHandleApUseOldNewL( retval );
            }
#else
        retval = DoHandleApUseOldNewL( retval );
#endif // __TEST_OOMDEBUG
        }
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::HandleApUseOldNewL")
    
    return retval;
    }



//----------------------------------------------------------
// CApSettingsHandlerImpl::EditAfterCreateL
//----------------------------------------------------------
//
TUint32 CApSettingsHandlerImpl::EditAfterCreateL( TUint32 aUid,
                                              CApAccessPointItem* aItem )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::EditAfterCreateL")
    
    CApSettingsDlg* iMySetDialog = CApSettingsDlg::NewL
                                                    (
                                                    KEApIspTypeAll,
                                                    EApBearerTypeAll,
                                                    KEApSortNameAscending,
                                                    iEventStore,
                                                    iReqIpvType
                                                    );

    iMySetDialog->SetTextOverrides( iTextOverrides );
    iMySetDialog->ConstructAndRunLD( *iModel, *this, aUid, aItem );
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::EditAfterCreateL")
    
    return aUid;
    }



//----------------------------------------------------------
// CApSettingsHandlerImpl::DoConstructL
//----------------------------------------------------------
//
void CApSettingsHandlerImpl::DoConstructL( CActiveApDb* aDb,
                                       TInt aIspFilter,
                                       TInt aBearerFilter,
                                       TInt aSortType,
                                       TVpnFilterType aVpnFilterType,
                                       TBool aIncludeEasyWlan )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::DoConstructL")
    
    iExt = new ( ELeave )TApSetHandlerImplExtra;
    iExt->iResOffset = 0;
    iExt->iIsIpv6Supported = EFalse;
    iExt->iIsFeatureManagerInitialised = EFalse;
    iExt->iExtra = 0;
    iExt->iSortType = KEApSortNameAscending;
    iExt->iVpnFilterType = aVpnFilterType;
    iExt->iVariant = GetVariantL();
    iExt->iIncludeEasyWlan = aIncludeEasyWlan;
    
#ifdef __TEST_CDMA_WRITE_PROTECT
    iExt->iVariant |= KApUiEditOnlyVPNs;
#endif // __TEST_CDMA_WRITE_PROTECT
    iIspFilter = KEApIspTypeAll;
    iBearerFilter = aBearerFilter;

    // get IPv6 features from feature manager
    FeatureManager::InitializeLibL();
    iExt->iIsFeatureManagerInitialised = ETrue;

    iExt->iIsIpv6Supported = 
            FeatureManager::FeatureSupported( KFeatureIdIPv6 );
#ifdef __TEST_IPV6_SUPPORT    
    iExt->iIsIpv6Supported = ETrue;
#endif //  __TEST_IPV6_SUPPORT    

    // we ask IPv6 only if it is asked by client AND supported by the phone
    iReqIpvType = iExt->iIsIpv6Supported ? iReqIpvType : EIPv4;


#ifdef __TEST_OOMDEBUG
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL<RFs>( fs );
    TUint att;
    if ( fs.Att( KTestFileName, att ) == KErrNone )
        {
        iMemTestOn = ETrue;
        }
    else
        {
        iMemTestOn = EFalse;
        }
    CleanupStack::PopAndDestroy(); // fs, will also close it
#endif // __TEST_OOMDEBUG
    iTextOverrides = CTextOverrides::NewL();
    // No need to PushL( iTextOverrides ), it's pointer is owned
    // & will be deleted in destructor, because 'this' is already
    // on the CleanupStack...
    if ( aDb )
        {
        iModel = CApSettingsModel::NewL( *aDb, aIspFilter,
                                         aBearerFilter, aSortType, 
                                         iReqIpvType,
                                         iExt->iVpnFilterType,
                                         iExt->iVariant,
                                         iExt->iIncludeEasyWlan,
                                         iNoEdit );
        }
    else
        {
        iModel = CApSettingsModel::NewL( aIspFilter, aBearerFilter,
                                         aSortType, iReqIpvType,
                                         iExt->iVpnFilterType,
                                         iExt->iVariant,
                                         iExt->iIncludeEasyWlan,
                                         iNoEdit );
        }
    SetFilterL( aIspFilter, aBearerFilter, aSortType );

    // can't use resource here because it has not been added yet....
    TFileName resourceFile;
    resourceFile.Append( KApSettingsResDriveZ );
    resourceFile.Append( KDC_RESOURCE_FILES_DIR );
    resourceFile.Append( KApSettingsResFileName );
    BaflUtils::NearestLanguageFile( iModel->EikEnv()->FsSession(),
                                    resourceFile );
    // as we are not an Application having iCoeEnv, we must figure it out,
    // no matter what it takes. Also it is getted once per construction...
    iExt->iResOffset = iModel->EikEnv()->AddResourceFileL( resourceFile );
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::DoConstructL")    
    }




//----------------------------------------------------------
// CApSettingsHandlerImpl::DoRunSetPageSettingsL
//----------------------------------------------------------
//
void CApSettingsHandlerImpl::DoRunSetPageSettingsL( TUint32 aHighLight,
                                                TUint32& aSelected )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::DoRunSetPageSettingsL")
    
    TInt aHigh = aHighLight;

    HBufC* titlebuf;

    if ( iTextOverrides->IsTextOverridenL( EPopupPromptText ) )
        {
        titlebuf = iTextOverrides->TextOverrideL(
                                                EPopupPromptText
                                                ).AllocLC();
        }
    else
        {
        titlebuf = iModel->EikEnv()->AllocReadResourceLC
                                (
                                R_APUI_VIEW_DEFAULT_AP
                                );
        }
    /*
    * We need to pass an int& as starting selection to
    * CAknRadioButtonSettingPage
    * But we use preferred UID & not list-item nuber...
    * Also it sets selection in it's ConstructL() but our list is filled
    * up dynamically and it must be done after ConstructL()
    * (list must be already constructed...)
    * Therefore we pass a dummy int& initialized to 0 as
    * selecting the 0th element does no harm...
    */
    TInt Dummy( -1 );
    // needs this Dummy array to pass it to CAknSettingPage's constructor...
    CDesCArrayFlat* aItemArray = new ( ELeave ) CDesCArrayFlat( 1 );
    CleanupStack::PushL( aItemArray );
    CApSelPopupList* dlg = CApSelPopupList::NewL
                            (
                            *iModel,
                            *this,
                            Dummy,
                            aHigh,
                            iSelMenuType,
                            iIspFilter,
                            iBearerFilter,
                            iExt->iSortType,
                            iEventStore,
                            aItemArray,
                            iListType == EApSettingsSelListIsPopUpWithNone,
                            iReqIpvType,
                            iExt->iVpnFilterType,
                            iExt->iIncludeEasyWlan,
                            iNoEdit
                            );
    CleanupStack::PushL( dlg );
    TPtrC ptr( titlebuf->Des() );
    dlg->SetSettingTextL( ptr );
    // dlg must be Pop-ed because ExecuteLD starts with PushL()...
    CleanupStack::Pop();
    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
        {
        aSelected = aHigh;
        }
    CleanupStack::PopAndDestroy( 2 ); // aItemArray, titlebuf
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::DoRunSetPageSettingsL")
    }



//----------------------------------------------------------
// CApSettingsHandlerImpl::DoRunListSettingsL
//----------------------------------------------------------
//
void CApSettingsHandlerImpl::DoRunListSettingsL( TUint32 aHighLight,
                                             TUint32& aSelected )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::DoRunListSettingsL")
    
    CApSelectorDialog* iMyDialog = CApSelectorDialog::NewL
                                            (
                                            iListType,
                                            iSelMenuType,
                                            iIspFilter,
                                            iBearerFilter,
                                            iExt->iSortType,
                                            iEventStore,
                                            iNoEdit,
                                            iReqIpvType
                                            );
    iMyDialog->SetTextOverrides( iTextOverrides );

    iMyDialog->ConstructAndRunLD( *iModel, *this, aHighLight, aSelected );
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::DoRunListSettingsL")
    }



//----------------------------------------------------------
// CApSettingsHandlerImpl::DoHandleApUseOldNewL
//----------------------------------------------------------
//
TUint32 CApSettingsHandlerImpl::DoHandleApUseOldNewL( TUint32 aSelected )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::DoHandleApUseOldNewL")
    
    TInt retval = KErrNone;

    TTransactionResult ownTransaction = iModel->StartTransactionLC
                                                        ( /*aWrite*/ETrue,
                                                        /*aShowNote*/ETrue,
                                                        /*aRetry*/ETrue
                                                        );
    if ( ownTransaction != EFailedWithLocked )
        {
        // check the original bearer's type
        TApBearerType bearer = iModel->ApUtils()->BearerTypeL( aSelected );
#ifdef __TEST_LAN_BEARER        
        if ( bearer == EApBearerTypeLAN )
            {
            User::Leave( KErrNotSupported );
//            ShowNoteL( R_APUI_AP_LAN_NOT_SUPPORTED );
//            User::Leave( KLeaveWithoutAlert );
            }
#endif // __TEST_LAN_BEARER
        if ( bearer == EApBearerTypeLANModem )
            {
            User::Leave( KErrNotSupported );
//            ShowNoteL( R_APUI_AP_LAN_NOT_SUPPORTED );
//            User::Leave( KLeaveWithoutAlert );
            }
        TBool isprotected( EFalse );

        TBool protSupported = FeatureManager::FeatureSupported( 
                                               KFeatureIdSettingsProtection );

        if ( protSupported )
            {
            isprotected = iModel->ApUtils()->IsReadOnlyL( aSelected );
            isprotected |= CApProtHandler::IsTableProtectedL( 
                                iModel->Database()->Database() );
            }
        if ( isprotected )
            {
            ShowNoteL( R_APUI_NOTE_PROTECTED_AP_SETT );
            }
        if ( !isprotected )
            {
            TUint32 aUid = iModel->DataHandler()->CreateCopyFromL( aSelected );
            if ( ownTransaction == EOwnTransaction )
                {
                TInt err = iModel->CommitTransaction();
                User::LeaveIfError( err );
                CleanupStack::Pop(); // RollbackTransactionOnLeave
                }
            iEventStore |= KApUiEventCreatedUsedOld;
            retval = aUid;
            EditAfterCreateL( aUid );
            }
        else
            {
            if ( ownTransaction == EOwnTransaction )
                {
                TInt err = iModel->CommitTransaction();
                User::LeaveIfError( err );
                CleanupStack::Pop(); // RollbackTransactionOnLeave
                }
            }
        }

    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::DoHandleApUseOldNewL")
    return retval;
    }



//----------------------------------------------------------
// CApSettingsHandlerImpl::SelectApToCopyL
//----------------------------------------------------------
//
TUint32 CApSettingsHandlerImpl::SelectApToCopyL()
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::SelectApToCopyL")
    
    // = KErrNone just to suppress compiler warning C4701
    TUint32 aSelected = KErrNone;
    TUint32 retval = KErrNone;

    CApSelQueryDialog* dlg = new ( ELeave ) CApSelQueryDialog( *iModel,
                                                               &aSelected );
    TInt iQueryOk = dlg->ExecuteLD( R_LIST_QUERY );
    if ( iQueryOk )
        {
        retval = aSelected;
        }
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::SelectApToCopyL")
    return retval;
    }





//----------------------------------------------------------
// CApSettingsHandlerImpl::DoHandleApBlankNewL
//----------------------------------------------------------
//
TUint32 CApSettingsHandlerImpl::DoHandleApBlankNewL( TInt aBearers, TInt aIsps )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::DoHandleApBlankNewL")

    CApAccessPointItem* apitem = CApAccessPointItem::NewLC();
/*
KEApIspTypeInternetOnly:
    EApBearerTypeCSD, EApBearerTypeHSCSD, EApBearerTypeGPRS

KEApIspTypeInternetAndWAP
    EApBearerTypeCSD, EApBearerTypeHSCSD, EApBearerTypeGPRS

KEApIspTypeWAPOnly
    EApBearerTypeCSD, EApBearerTypeHSCSD, EApBearerTypeSMS, EApBearerTypeGPRS

KEApIspTypeWAPMandatory
    EApBearerTypeCSD, EApBearerTypeHSCSD, EApBearerTypeSMS, EApBearerTypeGPRS

KEApIspTypeAll
    All


    EIspTypeInternetOnly,
    EIspTypeWAPOnly,
    EIspTypeInternetAndWAP

*/
    ChooseBearerTypeL( aBearers, *apitem );
    
    if ( aIsps & KEApIspTypeAll )
        {
        apitem->WriteUint( EApIspIspType, EIspTypeInternetAndWAP );
        }
    else
        {
        if ( aIsps & KEApIspTypeWAPOnly )
            { // EApBearerTypeCSD, EApBearerTypeHSCSD, EApBearerTypeGPRS,
            apitem->WriteUint( EApIspIspType, EIspTypeWAPOnly );
            }
        else
            {
            if ( aIsps & 
                     ( KEApIspTypeWAPMandatory | KEApIspTypeMMSMandatory ) )
                { // EApBearerTypeCSD, EApBearerTypeHSCSD, EApBearerTypeGPRS
                apitem->WriteUint( EApIspIspType, EIspTypeInternetAndWAP );
                }
            else
                {
                if ( aIsps & KEApIspTypeInternetAndWAP )
                    {
                    apitem->WriteUint( EApIspIspType, EIspTypeInternetAndWAP );
                    }
                else
                    {
                    if ( aIsps & KEApIspTypeInternetOnly )
                        {
                        apitem->WriteUint( EApIspIspType,
                                           EIspTypeInternetOnly );
                        }
                    else
                        {
                        __ASSERT_DEBUG( EFalse, Panic( EInvalidIspRequest ) );
                        }
                    }
                }
            }
        }
/*
KEApIspTypeInternetOnly
KEApIspTypeWAPOnly
KEApIspTypeInternetAndWAP
KEApIspTypeWAPMandatory
KEApIspTypeAll
*/

//    apitem->WriteUint( EApIspIspType, EIspTypeInternetOnly );

    TUint32 aUid( KApNoneUID );
    HBufC* buf = HBufC::NewLC( KApMaxConnNameLength );
    TPtr16 ptr( buf->Des() );
    apitem->ReadTextL( EApWapAccessPointName, ptr );

    TTransactionResult ownTransaction = iModel->StartTransactionLC(
                                                    ETrue, // aWrite
                                                    ETrue, // aShowNote
                                                    ETrue ); // aRetry
    if ( ownTransaction != EFailedWithLocked )
        {
        if ( iModel->ApUtils()->MakeValidNameL( ptr ) )
            {
            apitem->SetNamesL( ptr );
            }
        if ( ownTransaction == EOwnTransaction )
            {
            TInt err = iModel->CommitTransaction();
            User::LeaveIfError( err );
            CleanupStack::Pop(); // RollbackTransactionOnLeave
            }
        }

    aUid = EditAfterCreateL( KApNoneUID, apitem );
    CleanupStack::PopAndDestroy( 2 ); // apitem, buf
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::DoHandleApBlankNewL")
    return aUid;
    }




//----------------------------------------------------------
// CApSettingsHandlerImpl::HandleNetworkDeleteCmdL
//----------------------------------------------------------
//
void CApSettingsHandlerImpl::HandleNetworkDeleteCmdL( TUint32 aUid )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::HandleNetworkDeleteCmdL")
    
        // check that the Ap is not used so it can be deleted!!
    TTransactionResult ownTransaction = iModel->StartTransactionLC(
                                                /*aWrite*/EFalse,
                                                /*aShowNote*/ETrue,
                                                /*aRetry*/ETrue );
    if ( ownTransaction != EFailedWithLocked )
        {
        TBool protSupported = FeatureManager::FeatureSupported( 
                                               KFeatureIdSettingsProtection );
        if ( protSupported )
            {
            TBool isprotected = iModel->ApUtils()->IsReadOnlyL( aUid );
            if ( isprotected )
                {
                ShowNoteL( R_APUI_NOTE_PROTECTED_AP );
                return;
                }
            }
        if ( ownTransaction == EOwnTransaction )
            {
            TInt err = iModel->CommitTransaction();
            User::LeaveIfError( err );
            CleanupStack::Pop(); // RollbackTransactionOnLeave
            }

        // not protected, may start wondering about delete...
        // simply confirm

        // OOD handling. If disk space is low,
        // we try to get some spare.
        RFs fs;
        User::LeaveIfError( fs.Connect() );
        CleanupClosePushL<RFs>( fs );
        TBool diskLow = SysUtil::FFSSpaceBelowCriticalLevelL
                ( &fs, KEstimatedDeleteFfsOverhead );
        RSharedDataClient sdClient;
        if ( diskLow )
            {
            User::LeaveIfError( sdClient.Connect() );
            CleanupClosePushL<RSharedDataClient>( sdClient );
            sdClient.RequestFreeDiskSpaceLC
                        ( KEstimatedDeleteFfsOverhead );
            }
        // Handle getting the name, showing the query,
        // and deleting the network group.
        // get the name:
        CApNetworkItem* net = CApNetworkItem::NewLC();
        iModel->DataHandler()->ReadNetworkL( aUid, *net );
        HBufC* buf = net->Name().AllocLC();
        if ( AskQueryL( R_APUI_IAP_ARE_YOU_SURE_TO_DEL, buf ) )
            {
            iModel->DataHandler()->RemoveNetworkL( aUid );
            }
        CleanupStack::PopAndDestroy( buf ); // buf
        CleanupStack::PopAndDestroy( net );
        if ( diskLow )
            {
            // Cancel RequestFreeDiskSpaceLC (by popping off an destroy);
            // close scClient.
            CleanupStack::PopAndDestroy( 2 );
            }
        CleanupStack::PopAndDestroy(); // fs, will also close it
        }
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::HandleNetworkDeleteCmdL")
    }



//----------------------------------------------------------
// CApSettingsHandlerImpl::GetVariantL
//----------------------------------------------------------
//
TInt CApSettingsHandlerImpl::GetVariantL()
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::GetVariantL")
    
    TInt variant( 0 );
    // Connecting and initialization:
    CRepository* repository = 
            CRepository::NewL( KCRUidApSettingsHandlerUiLV );
    repository->Get( KApSettingsHandlerUiLVFlags, variant );
    delete repository;

    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::GetVariantL")
    return variant;
    }


//----------------------------------------------------------
// CApSettingsHandlerImpl::ConfirmDeleteL
//----------------------------------------------------------
//
TInt CApSettingsHandlerImpl::ConfirmDeleteL( TUint32 aUid, TBool aIsLast )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::ConfirmDeleteL")
    
    TInt retval( 0 );
    HBufC* sgd = HBufC::NewLC( KModifiableTextLength );
    TPtr ptr( sgd->Des() );

    // OOD handling. If disk space is low,
    // we try to get some spare.
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL<RFs>( fs );
    TBool diskLow = SysUtil::FFSSpaceBelowCriticalLevelL
            ( &fs, KEstimatedDeleteFfsOverhead );

    RSharedDataClient sdClient;
    if ( diskLow )
        {
        User::LeaveIfError( sdClient.Connect() );
        CleanupClosePushL<RSharedDataClient>( sdClient );
        sdClient.RequestFreeDiskSpaceLC
                    ( KEstimatedDeleteFfsOverhead );
        }

    TTransactionResult ownTransaction = iModel->StartTransactionLC
                                               (
                                               /*aWrite*/ETrue,
                                               /*aShowNote*/ETrue,
                                               /*aRetry*/ETrue,
                                               /*aLFFSChecking*/EFalse
                                               );
    if ( ownTransaction != EFailedWithLocked )
        {
        iModel->ApUtils()->NameL( aUid, ptr );
        if ( ownTransaction == EOwnTransaction )
            {
            TInt err = iModel->CommitTransaction();
            User::LeaveIfError( err );
            CleanupStack::Pop(); // RollbackTransactionOnLeave
            }

        if ( aIsLast )
            { // we are deleting the last one, needs to confirm
            retval = AskQueryL( R_APUI_IAP_DEL_LAST_AP, sgd );
            }
        else
            { // not the last one, simply confirm
            // handle exotic language number conversion here
            TPtr t(sgd->Des());
            AknTextUtils::LanguageSpecificNumberConversion( t );
            retval = AskQueryL( R_APUI_IAP_ARE_YOU_SURE_TO_DEL, sgd );
            }
        }


    CleanupStack::PopAndDestroy(); // sgd
    if ( diskLow )
        {
        // Cancel RequestFreeDiskSpaceLC (by popping off an destroy);
        // close scClient.
        CleanupStack::PopAndDestroy( 2 );
        }
    CleanupStack::PopAndDestroy(); // fs, will also close it
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::ConfirmDeleteL")
    return retval;
    }





//----------------------------------------------------------
// CApSettingsHandlerImpl::HandleRunSettingsL
//----------------------------------------------------------
//
void CApSettingsHandlerImpl::HandleRunSettingsL( TUint32 aHighLight, 
                                             TUint32& aSelected )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::HandleRunSettingsL")
    
    if ( iStartWithSelection == EFalse )
        {
        RunViewerL( aHighLight );
        }
    else
        {
#ifdef __TEST_OOMDEBUG
        if ( iMemTestOn )
            {
            TInt leavecode( KErrNoMemory );
            TInt state( GetTestStateL() );

            if ( ( (
                   ( iListType == EApSettingsSelListIsPopUp ) ||
                   ( iListType == EApSettingsSelListIsPopUpWithNone ) ) &&
                   ( state == KPopupListTest ) )
                 ||
                 (
                 ( iListType == EApSettingsSelListIsListPane ) &&
                 ( state == KDialogListTest ) ) )
                {
                TBuf<48> buf;
                if ( state == KDialogListTest )
                    {
                    APSETUILOGGER_WRITE_BUF( EApImpl, \
                        _L("Doing memtest KDialogListTest") );
                    }
                if ( state == KPopupListTest )
                    {
                    APSETUILOGGER_WRITE_BUF( EApImpl, \ 
                        _L("Doing memtest KPopupListTest") );
                    }
                TUint32 rate( 0 );
                TUint32 retrycount( KOOMRetryCount );
                TUint32 retryoffset( KOOMRetryOffset );
                for (
                    rate = 1;
                    ((rate < 50000) && (leavecode == KErrNoMemory))
                        || retrycount;
                    rate++
                    )
                    {
                    __UHEAP_SETFAIL( RHeap::EDeterministic, rate );
                    TRAP( leavecode,
                          DoRunSettingsL( aHighLight, aSelected ) );
                    __UHEAP_RESET;
                    if ( !leavecode )
                        {
                        retrycount--;
                        rate += retryoffset;
                        }
                    }
                User::LeaveIfError( leavecode );
                buf.Format( _L("\t Memtest finished, max. rate was: %d )"), 
                           rate );
                APSETUILOGGER_WRITE_BUF( EApImpl, buf );
                }
            else
                {
                DoRunSettingsL( aHighLight, aSelected );
                }
            }
        else
            {
            DoRunSettingsL( aHighLight, aSelected );
            }
#else
        DoRunSettingsL( aHighLight, aSelected );
#endif // __TEST_OOMDEBUG
        }
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::HandleRunSettingsL")
    }


//----------------------------------------------------------
// CApSettingsHandlerImpl::ChooseBearerTypeL
//----------------------------------------------------------
//
void CApSettingsHandlerImpl::ChooseBearerTypeL( TInt aBearers, 
                                            CApAccessPointItem& aItem )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::ChooseBearerTypeL")
    
    // Check for support in each case!!!!!
    TBool gprssupported = ETrue;
    TBool csdsupported = ETrue;         // OK
    TBool hscsdsupported = ETrue;       // NO WAY TO CHECK IT
    TBool wlansupported = ETrue;        // OK
    TBool lansupported = ETrue;         // NO WAY TO CHECK IT
    TBool lanmodemsupported = ETrue;    // NO WAY TO CHECK IT
    TBool cdmasupported = ETrue;        // OK
    
    if ( iExt->iIsFeatureManagerInitialised )
        {
        cdmasupported = FeatureManager::FeatureSupported( KFeatureIdProtocolWcdma );
        csdsupported = FeatureManager::FeatureSupported( KFeatureIdAppCsdSupport );
        wlansupported = FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );
        }

    if ( ( ( aBearers & EApBearerTypeAll ) || 
        ( aBearers & EApBearerTypeGPRS ) ) &&
        gprssupported )
        {
        aItem.SetBearerTypeL( EApBearerTypeGPRS );
        }
    else
        {
        if ( ( ( aBearers & EApBearerTypeAll ) || 
            ( aBearers & EApBearerTypeHSCSD ) ) 
            && hscsdsupported ) 
            {
            aItem.SetBearerTypeL( EApBearerTypeHSCSD );
            }
        else
            {
            if ( ( ( aBearers & EApBearerTypeAll ) || 
                (aBearers & EApBearerTypeCSD ) ) && csdsupported )
                {
                aItem.SetBearerTypeL( EApBearerTypeCSD );
                }
            else
                {
                if ( ( ( aBearers & EApBearerTypeAll ) || 
                    ( aBearers & EApBearerTypeWLAN ) ) 
                    && wlansupported )
                    {
                    aItem.SetBearerTypeL( EApBearerTypeWLAN );
                    }
                else
                    {
                    if ( ( ( aBearers & EApBearerTypeAll ) || 
                        ( aBearers & EApBearerTypeLAN ) ) 
                        && lansupported )
                        {
                        aItem.SetBearerTypeL( EApBearerTypeLAN );
                        }
                    else
                        {
                        if ( ( ( aBearers & EApBearerTypeAll ) || 
                            ( aBearers & EApBearerTypeLANModem ) )
                            && lanmodemsupported )
                            {
                            aItem.SetBearerTypeL( EApBearerTypeLANModem );
                            }
                        else
                            {
                            if ( ( ( aBearers & EApBearerTypeAll ) || 
                                ( aBearers & EApBearerTypeCDMA ) )
                                && cdmasupported )
                                {
                                aItem.SetBearerTypeL( EApBearerTypeCDMA );
                                }
                            else
                                {
                                User::Leave( KErrInvalidBearerType );
                                }
                            }
                        }
                    }
                }
            }
        }    
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::ChooseBearerTypeL")
    }




//----------------------------------------------------------
// CApSettingsHandlerImpl::HandleDuplicateL
//----------------------------------------------------------
//
TUint32 CApSettingsHandlerImpl::HandleDuplicateL( TUint32 aUid )
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::HandleDuplicateL")
    
    TUint32 retval(KErrNone);
    // so user had chosen something
#ifdef __TEST_OOMDEBUG
    if ( iMemTestOn )
        {
        TInt leavecode( KErrNoMemory );
        TInt ts( GetTestStateL() );
        if ( ts == KCreateUseExistTest )
            {
            TBuf<48> buf;
            APSETUILOGGER_WRITE_BUF
                ( _L("Starting KCreateUseExistTest memtest") );
            TUint32 rate( 0 );
            TUint32 retrycount( KOOMRetryCount );
            TUint32 retryoffset( KOOMRetryOffset );
            for (
                rate = 1;
                ((rate < 50000) && (leavecode == KErrNoMemory))
                    || retrycount;
                rate++
                )
                {
                __UHEAP_SETFAIL( RHeap::EDeterministic, rate );
                TRAP( leavecode, retval = DoHandleApUseOldNewL( aUid ) );
                __UHEAP_RESET;
                if ( !leavecode )
                    {
                    retrycount--;
                    rate += retryoffset;
                    }
                }
            User::LeaveIfError( leavecode );
            buf.Format( _L("\t Memtest finished, max. rate was: %d )"),
                        rate );
            APSETUILOGGER_WRITE_BUF( EApImpl, buf );
            }
        else
            {
            retval = DoHandleApUseOldNewL( aUid );
            }
        }
    else
        {
        retval = DoHandleApUseOldNewL( aUid );
        }
#else
        retval = DoHandleApUseOldNewL( aUid );
#endif // __TEST_OOMDEBUG
    
    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::HandleDuplicateL")
    return retval;
    }


// End of File

