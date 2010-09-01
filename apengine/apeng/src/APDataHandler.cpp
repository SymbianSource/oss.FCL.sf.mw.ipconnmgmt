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
* Description:  Declaration of class CApDataHandler
*
*/


// INCLUDE FILES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <nifvar_internal.h>
#endif
#include "APEngineVariant.hrh"

#include <cdbcols.h>
#include <commdb.h>
#include <cdbpreftable.h>
#include <etelpckt.h>
#include <featmgr.h>
#ifdef __TEST_USE_SHARED_DATA
    #include <SharedDataClient.h>
    #include <sharedDataValues.h>
#endif //__TEST_USE_SHARED_DATA
#include <WlanCdbCols.h>

#include "ApDataHandler.h"
#include "ApAccessPointItem.h"
#include "ApEngineCommons.h"
#include "ApUtils.h"
#include "ApEngineVer.h"
#include "ApEngineLogger.h"
#include "ApNetworkItem.h"
#include "APItemExtra.h"
#include "APItemCdmaData.h"
#include "APItemWlanData.h"
#include "ApSpeedLookup.h"

#include <WEPSecuritySettingsUI.h>
#include <WPASecuritySettingsUI.h>


// CONSTANTS
#if defined(_DEBUG)
    _LIT( KErrInvalidIntendedType, "Invalid intended default type" );
#endif // (_DEBUG)
_LIT( KErrNoSuchCase, "No such case" );


// Agreed GPRS_QOS_WARNING_TIMEOUT Value
const TInt KGprsOsTimeout = -1;

/**
* General Settings UID
*/
#ifdef __TEST_USE_SHARED_DATA
    LOCAL_D const TUid KGeneralSettingsUid = { 0X100058EC };
#endif // __TEST_USE_SHARED_DATA






_LIT( KWlanBearerName, "WLANBearer" );
_LIT( KWlanBearerAgent, "wlanagt.agt" );
_LIT( KWlanBearerNif, "wlannif" );

_LIT( KWlanLDDName, "not used" );
_LIT( KWlanPDDName, "not used" );

const TInt KWlanLastSocketActivityTimeout = -1;
const TInt KWlanLastSessionClosedTimeout = 1;
const TInt KWlanLastSocketClosedTimeout = -1;






// MACROS


// LOCAL FUNCTION PROTOTYPES


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
// ---------------------------------------------------------
// CApDataHandler::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApDataHandler* CApDataHandler::NewLC( CCommsDatabase& aDb )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::NewLC" ) ) );

    CApDataHandler* db = new( ELeave ) CApDataHandler;
    CleanupStack::PushL( db );
    db->ConstructL( aDb );
    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::NewLC" ) ) );

    return db;
    }



// ---------------------------------------------------------
// CApDataHandler::~CApDataHandler
// ---------------------------------------------------------
//
EXPORT_C CApDataHandler::~CApDataHandler()
    {
    if ( iExt )
        {
        if ( iExt->iIsFeatureManagerInitialised )
            {
            FeatureManager::UnInitializeLib();
            }
        }
    delete iExt;
    }


// ---------------------------------------------------------
// CApDataHandler::CApDataHandler
// ---------------------------------------------------------
//
EXPORT_C CApDataHandler::CApDataHandler( )
    {
    iDb = NULL;
    }


// ---------------------------------------------------------
// CApDataHandler::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::ConstructL( CCommsDatabase& aDb )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::ConstructL" ) ) );

    iExt = new ( ELeave )THandlerExtra;
    iExt->iIsFeatureManagerInitialised = EFalse;
    iExt->iIsIpv6Supported = EFalse;
    
    FeatureManager::InitializeLibL();
    iExt->iIsFeatureManagerInitialised = ETrue;

    iExt->iIsIpv6Supported = 
                            FeatureManager::FeatureSupported( KFeatureIdIPv6 );
#ifdef __TEST_IPV6_SUPPORT    
    iExt->iIsIpv6Supported = ETrue;
#endif //  __TEST_IPV6_SUPPORT    
    
    iDb = &aDb;

#ifdef __TEST_USE_SHARED_DATA
    iExt->iIsAppHscsdSupport = 
        ApCommons::IsGivenSharedDataSupportL( KGeneralSettingsUid, 
                                              KGSHSCSDAccessPoints );
#else
    iExt->iIsAppHscsdSupport = ETrue;
#endif // __TEST_USE_SHARED_DATA


#ifdef __TEST_HSCSD_SUPPORT
    iExt->iIsAppHscsdSupport = ETrue;
#endif // __TEST_HSCSD_SUPPORT

    iExt->iVariant = ApCommons::GetVariantL();

#ifdef __TEST_CDMA_WRITE_PROTECT
    iExt->iVariant |= KApUiEditOnlyVPNs;
#endif // __TEST_CDMA_WRITE_PROTECT


    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::ConstructL" ) ) );
    }


// query
// ---------------------------------------------------------
// CApDataHandler::AccessPointDataL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::AccessPointDataL( TUint32 aUid,
                                              CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::AccessPointDataL" ) ) );

    // returns the AP data of the record with the UID aUid
    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    DoAccessPointDataL( aUid, aApItem );
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }
    aApItem.SanityCheckOk();

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::AccessPointDataL" ) ) );
    }



// update
// ---------------------------------------------------------
// CApDataHandler::UpdateAccessPointDataL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::UpdateAccessPointDataL
                                                (
                                                CApAccessPointItem& aApItem,
                                                TBool& aNameChanged
                                                )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::UpdateAccessPointDataL" ) ) );

    if ( iExt->iVariant & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }

    // update access point data.
    aApItem.SanityCheckOk();

    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    DoUpdateAccessPointDataL( aApItem, EFalse, aNameChanged );
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::UpdateAccessPointDataL" ) ) );
    }



// Creating new AP
// ---------------------------------------------------------
// CApDataHandler::CreateCopyFromL
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApDataHandler::CreateCopyFromL( TUint32 aBaseId )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::CreateCopyFromL" ) ) );

    if ( iExt->iVariant & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }

    CApAccessPointItem* holder = CApAccessPointItem::NewLC();

    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    AccessPointDataL( aBaseId, *holder );
    TUint32 oldwlanid( 0 );
    holder->ReadUint( EApIapServiceId, oldwlanid );
    
    TUint32 retval( 0 );
    TBool aNameChanged( EFalse );
    
    TApBearerType bearer = holder->BearerTypeL();
    CWEPSecuritySettings* wepSecSettings = NULL;
    CWPASecuritySettings* wpaSecSettings( NULL );
    // only one of the above might be loaded once, so after copying
    // we might need to PopAndDestroy only one item
    TBool pushed(EFalse);

    if ( bearer == EApBearerTypeWLAN )
        {
        TUint32 secmode( 0 );
        holder->ReadUint( EApWlanSecurityMode, secmode );
        switch ( secmode )
            {
            case EOpen:
                {
                break;
                }
            case EWep:
                {
                wepSecSettings = CWEPSecuritySettings::NewL();
                CleanupStack::PushL( wepSecSettings );
                pushed = ETrue;
                TUint32 wlanid( 0 );
                holder->ReadUint( EApIapServiceId, wlanid );
                wepSecSettings->LoadL( wlanid, *iDb );
                break;
                }
            case E802_1x:
                {
                wpaSecSettings = 
                    CWPASecuritySettings::NewL( ESecurityMode8021x );
                CleanupStack::PushL( wpaSecSettings );
                pushed = ETrue;
                TUint32 wlanid( 0 );
                holder->ReadUint( EApIapServiceId, wlanid );
                wpaSecSettings->LoadL( wlanid, *iDb );
                break;
                }
            case EWpa:
            case EWpa2:
                {
                wpaSecSettings = 
                        CWPASecuritySettings::NewL( ESecurityModeWpa );
                CleanupStack::PushL( wpaSecSettings );
                pushed = ETrue;
                TUint32 wlanid( 0 );
                holder->ReadUint( EApIapServiceId, wlanid );
                wpaSecSettings->LoadL( wlanid, *iDb );
                break;
                }
            default:
                {
                __ASSERT_DEBUG( EFalse, ApCommons::Panic( ENotSupported ) );
                // do nothing in urel
                break;
                }
            }
        
        }
    
    retval = DoUpdateAccessPointDataL( *holder, ETrue, aNameChanged );
    
    if ( bearer == EApBearerTypeWLAN )
        {
        TUint32 wlanid(0);
        holder->ReadUint( EApIapServiceId, wlanid );        
        // now check if it is WEP...
        // read up security mode
        TUint32 secmode( 0 );
        holder->ReadUint( EApWlanSecurityMode, secmode );
        switch ( secmode )
            {
            case EOpen:
                {
                break;
                }
            case EWep:
                {
                // we have to try to save
                wepSecSettings->SaveL( wlanid, *iDb );
                break;
                }
            case E802_1x:
                {
                wpaSecSettings->SaveL( wlanid, *iDb, 
                                       ESavingNewAPAsACopy, oldwlanid );
                break;
                }
            case EWpa:
            case EWpa2:
                {
                wpaSecSettings->SaveL( wlanid, *iDb,
                                       ESavingNewAPAsACopy, oldwlanid );
                break;
                }
            default:
                {
                __ASSERT_DEBUG( EFalse, ApCommons::Panic( ENotSupported ) );
                // do nothing in urel
                break;
                }
            }
        }
    if ( pushed )        
        {
        CleanupStack::PopAndDestroy(); // the sec. settings
        }
        
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }
    CleanupStack::PopAndDestroy( holder );    // holder

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::CreateCopyFromL" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// CApDataHandler::CreateFromDataL
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApDataHandler::CreateFromDataL( CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::CreateFromDataL" ) ) );

    if ( iExt->iVariant & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }

    aApItem.SanityCheckOk();

    TBool aNameChanged( EFalse );
    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    TUint32 retval = DoUpdateAccessPointDataL( aApItem, ETrue, aNameChanged );
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransaction
        }

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::CreateFromDataL" ) ) );
    return retval;
    }



// Remove
// ---------------------------------------------------------
// CApDataHandler::RemoveAP
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::RemoveAPL( TUint32 aUid )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::RemoveAPL" ) ) );

    if ( iExt->iVariant & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }

    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    DoRemoveAPL( aUid );
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::RemoveAPL" ) ) );
    }


// Getting default access point
// ---------------------------------------------------------
// CApDataHandler::DefaultL
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApDataHandler::DefaultL( TBool aIsWap ) const
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::DefaultL" ) ) );

    // get default AP...
    // first get global settings...
    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    TUint32 value = DoGetDefaultL( aIsWap, NULL );
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::DefaultL" ) ) );
    return value;
    }



// Setting as default
// ---------------------------------------------------------
// CApDataHandler::SetAsDefaultL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::SetAsDefaultL( TUint32 aUid,
                                            TCommsDbIspType aIntendedType )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::SetAsDefaultL" ) ) );

    if ( iExt->iVariant & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }

    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    DoSetAsDefaultL( aUid, aIntendedType );
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::SetAsDefaultL" ) ) );
    }


// ---------------------------------------------------------
// ---------------------------------------------------------
// Connection Preferene Handling
// ---------------------------------------------------------
// ---------------------------------------------------------


// ---------------------------------------------------------
// CApDataHandler::SetPreferredIfDbIapTypeL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::SetPreferredIfDbIapTypeL
                                    (
                                    TUint32 aRank,
                                    TCommDbConnectionDirection aDirection,
                                    TUint32 aBearers,
                                    TCommDbDialogPref aPrompt,
                                    TUint32 aIAP,
                                    TBool aOverwrite
                                    )
    {
    CLOG( ( EHandler, 0, 
        _L( "-> CApDataHandler::SetPreferredIfDbIapTypeL" ) ) );

    if ( iExt->iVariant & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }

    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    DoSetPreferredIfDbIapTypeL( aRank, aDirection, aBearers,
                                           aPrompt, aIAP, aOverwrite);
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EHandler, 1, 
        _L( "<- CApDataHandler::SetPreferredIfDbIapTypeL" ) ) );
    }




// ---------------------------------------------------------
// CApDataHandler::SetPreferredIfDbIspTypeL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::SetPreferredIfDbIspTypeL
                                    (
                                    TUint32 aRank,
                                    TCommDbConnectionDirection aDirection,
                                    TUint32 aBearers,
                                    TCommDbDialogPref aPrompt,
                                    TUint32 aISP,
                                    TUint32 aChargeCard,
                                    const TDesC& aServiceType,
                                    TBool aOverwrite
                                    )
    {
    CLOG( ( EHandler, 0, 
        _L( "-> CApDataHandler::SetPreferredIfDbIspTypeL" ) ) );

    if ( iExt->iVariant & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }

    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    DoSetPreferredIfDbIspTypeL( aRank, aDirection, aBearers, aPrompt,
                                aISP, aChargeCard, aServiceType, aOverwrite);
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EHandler, 1, 
        _L( "<- CApDataHandler::SetPreferredIfDbIspTypeL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::GetPreferredIfDbIapTypeL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::GetPreferredIfDbIapTypeL
                                    (
                                    TUint32 aRank,
                                    TCommDbConnectionDirection aDirection,
                                    TUint32& aBearers,
                                    TUint32& aPrompt,
                                    TUint32& aIAP
                                    )
    {
    CLOG( ( EHandler, 0, 
        _L( "-> CApDataHandler::GetPreferredIfDbIapTypeL" ) ) );

    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    CCommsDbConnectionPrefTableView* preft =
           iDb->OpenConnectionPrefTableViewOnRankLC( aDirection, aRank );
    TInt err = preft->GotoFirstRecord(); // OK.
    if ( err == KErrNone )
        { // record has been found
        CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref;
        preft->ReadConnectionPreferenceL( pref );
        aBearers = pref.iBearer.iBearerSet;
        aPrompt = pref.iDialogPref;
        aIAP = pref.iBearer.iIapId;
        }
    else
        {
        User::Leave( err );
        }
    CleanupStack::PopAndDestroy( preft ); // preft

    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EHandler, 1, 
        _L( "<- CApDataHandler::GetPreferredIfDbIapTypeL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::GetPreferredIfDbIspTypeL
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CApDataHandler::GetPreferredIfDbIspTypeL
                                    (
                                    TUint32 /*aRank*/,
                                    TCommDbConnectionDirection /*aDirection*/,
                                    TUint32& /*aBearers*/,
                                    TUint32& /*aPrompt*/,
                                    TUint32& /*aISP*/,
                                    TUint32& /*aChargeCard*/
                                    )
    {
    CLOG( ( EHandler, 0, 
        _L( "<-> CApDataHandler::GetPreferredIfDbIspTypeL" ) ) );

    User::Leave( KErrNotSupported );

    return KErrNoSuchCase;

    }



// ---------------------------------------------------------
// CApDataHandler::GetPreferredIfDbIspTypeL
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApDataHandler::DefaultL( TBool aIsWap,
                                                 TBool& aReadOnly ) const
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::DefaultL" ) ) );

    // get default AP...
    // first get global settings...
    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    TUint32 value = DoGetDefaultL( aIsWap, &aReadOnly );
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::DefaultL" ) ) );
    return value;
    }




// ---------------------------------------------------------
// CApDataHandler::RemoveNetworkL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::RemoveNetworkL( TUint32 aUid )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::RemoveNetworkL" ) ) );

    if ( iExt->iVariant & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }

    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    CCommsDbTableView* nettable =
        iDb->OpenViewMatchingUintLC( TPtrC(NETWORK), 
                                     TPtrC(COMMDB_ID), 
                                     aUid );
    TInt err = nettable->GotoFirstRecord();
    if ( err == KErrNone )
        {
        nettable->DeleteRecord();
        }
    else
        {
        if ( err != KErrNotFound )
            {
            User::Leave( err );
            }
        }
    CleanupStack::PopAndDestroy( nettable ); // RollbackTransactionOnLeave
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::RemoveNetworkL" ) ) );
    }


// ---------------------------------------------------------
// CApDataHandler::CreateNetworkL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::CreateNetworkL( CApNetworkItem& aNetwork )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::CreateNetworkL" ) ) );

    if ( iExt->iVariant & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }

    CApAccessPointItem* aApItem = CApAccessPointItem::NewLC();
    aApItem->WriteTextL( EApNetworkName, aNetwork.Name() );
    WriteNetworkDataL( ETrue, *aApItem );
    TUint32 tempuint( 0 );
    aApItem->ReadUint( EApNetworkID, tempuint );
    aNetwork.SetUid( tempuint );
    CleanupStack::PopAndDestroy( aApItem );

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::CreateNetworkL" ) ) );
    }


// ---------------------------------------------------------
// CApDataHandler::ReadNetworkPartL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::ReadNetworkPartL( CApAccessPointItem& aItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::ReadNetworkPartL" ) ) );

    ReadNetworkDataL( aItem );

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::ReadNetworkPartL" ) ) );
    }


// ---------------------------------------------------------
// CApDataHandler::ReadNetworkL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::ReadNetworkL( TUint32 aUid, 
                                            CApNetworkItem& aNetwork )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::ReadNetworkL" ) ) );

    CApAccessPointItem* aItem = CApAccessPointItem::NewLC();
    aItem->WriteUint( EApNetworkID, aUid );
    ReadNetworkDataL( *aItem );
    TUint32 length = aItem->ReadTextLengthL( EApNetworkName );
    HBufC* buf = HBufC::NewLC( length );
    TPtr16 ptr = buf->Des();
    aItem->ReadTextL( EApNetworkName, ptr );
    aNetwork.SetUid( aUid );
    aNetwork.SetNameL( ptr );
    CleanupStack::PopAndDestroy( buf );
    CleanupStack::PopAndDestroy( aItem );

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::ReadNetworkL" ) ) );
    }


// ================= OTHER EXPORTED FUNCTIONS ==============


// ================= PROTECTED FUNCTIONS ==============

// ---------------------------------------------------------
// CApDataHandler::ReadWapDataL
// ---------------------------------------------------------
//
void CApDataHandler::ReadWapDataL( TUint32 aWapId,
                                  CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::ReadWapDataL" ) ) );

    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CCommsDbTableView* wapt;
    wapt = iDb->OpenViewMatchingUintLC( TPtrC(WAP_ACCESS_POINT),
                                       TPtrC(COMMDB_ID), aWapId );


    User::LeaveIfError( wapt->GotoFirstRecord() );

    aApItem.WriteUint( EApWapAccessPointID, aWapId );

    TInt err;
    HBufC* buf = ApCommons::ReadLongTextLC( wapt,
                                             TPtrC(WAP_START_PAGE),
                                             err );
    if ( err != KErrUnknown )
        {
        aApItem.WriteLongTextL( EApWapStartPage, *buf );
        CleanupStack::PopAndDestroy( buf ); 
        }

    buf = ApCommons::ReadText16ValueLC( wapt, TPtrC(WAP_CURRENT_BEARER) );
    aApItem.WriteTextL( EApWapCurrentBearer, *buf );
    CleanupStack::PopAndDestroy( buf );

    buf = ApCommons::ReadText16ValueLC( wapt, TPtrC(COMMDB_NAME) );
    aApItem.WriteTextL( EApWapAccessPointName, *buf );
    CleanupStack::PopAndDestroy( buf );


    TBool readonly = ApCommons::DoGetReadOnlyL( *wapt );
    // not WriteBool, as it is 'write protected'...
    aApItem.iIsReadOnly = readonly;

    CCommsDbTableView* bearert;
    bearert = iDb->OpenViewMatchingUintLC( *aApItem.iWapBearer,
                                          TPtrC(WAP_ACCESS_POINT_ID), aWapId );

    User::LeaveIfError( bearert->GotoFirstRecord() );
    // now we can read common data
    buf = ApCommons::ReadText16ValueLC( bearert,
                                         TPtrC( WAP_GATEWAY_ADDRESS ) );
    aApItem.WriteTextL( EApWapGatewayAddress, *buf );
    CleanupStack::PopAndDestroy( buf );


    TUint32 tempint( 0 );
    TBool tempbool( EFalse );

    ApCommons::ReadUintL( bearert, TPtrC(WAP_WSP_OPTION), tempint );
    aApItem.WriteUint( EApWapWspOption, tempint );

    ApCommons::ReadBoolL( bearert, TPtrC(WAP_SECURITY), tempbool );
    aApItem.WriteBool( EApWapSecurity, tempbool );

    aApItem.SpecifyWap( ETrue );
    // decide which bearer, read bearer spec. data.
    if ( *aApItem.iWapBearer == TPtrC(WAP_IP_BEARER) )
        {
        // init it, if it is specified in IAP, will override this...
        aApItem.WriteUint( EApIapServiceId, tempint );

        ApCommons::ReadUintL( bearert, TPtrC(WAP_IAP), tempint );
        aApItem.WriteUint( EApWapIap, tempint );

        ApCommons::ReadUintL( bearert, TPtrC(WAP_PROXY_PORT), tempint );
        aApItem.WriteUint( EApWapProxyPort, tempint );

        buf = ApCommons::ReadText16ValueLC( bearert, 
                                            TPtrC(WAP_PROXY_LOGIN_NAME) );
        aApItem.WriteTextL( EApProxyLoginName, *buf );
        CleanupStack::PopAndDestroy( buf );


        buf = ApCommons::ReadText16ValueLC( bearert, 
                                            TPtrC(WAP_PROXY_LOGIN_PASS) );
        aApItem.WriteTextL( EApProxyLoginPass, *buf );
        CleanupStack::PopAndDestroy( buf );



        aApItem.SpecifyIpBearer( ETrue );
        }
    else
        {
        User::Leave( KErrInvalidBearer );
        }
    CleanupStack::PopAndDestroy( 2, wapt );    // bearert, wapt

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::ReadWapDataL" ) ) );
    }


// ---------------------------------------------------------
// CApDataHandler::ReadIapDataL
// ---------------------------------------------------------
//
void CApDataHandler::ReadIapDataL( TUint32 aIapId,
                                   CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::ReadIapDataL" ) ) );

    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CCommsDbTableView* iapt = iDb->OpenViewMatchingUintLC( TPtrC(IAP),
                                           TPtrC(COMMDB_ID), aIapId );

    User::LeaveIfError( iapt->GotoFirstRecord() );
    // can not be more than one record,
    // because we are looking up about UID!
    aApItem.WriteUint( EApWapIap, aIapId );

    HBufC* buf = ApCommons::ReadText16ValueLC( iapt, TPtrC(COMMDB_NAME) );
    aApItem.WriteTextL( EApIapName, *buf );
    CleanupStack::PopAndDestroy( buf );

    TUint32 tempint;

    ApCommons::ReadUintL( iapt, TPtrC(IAP_SERVICE), tempint );
    aApItem.WriteUint( EApIapServiceId, tempint );


    buf = ApCommons::ReadText16ValueLC( iapt, TPtrC(IAP_SERVICE_TYPE) );
    aApItem.WriteTextL( EApIapServiceType, *buf );
    CleanupStack::PopAndDestroy( buf );


    ApCommons::ReadUintL( iapt, TPtrC(IAP_BEARER), tempint );
    aApItem.WriteUint( EApIapBearerID, tempint );

    buf = ApCommons::ReadText16ValueLC( iapt, TPtrC(IAP_BEARER_TYPE) );
    aApItem.WriteTextL( EApIapBearerType, *buf );
    CleanupStack::PopAndDestroy( buf );

    ReadModemBearerNameL( aApItem );

    ApCommons::ReadUintL( iapt, TPtrC(IAP_NETWORK), tempint );
    aApItem.iNetworkId = tempint;

    // Network weighting!
    ApCommons::ReadUintL( iapt, TPtrC(IAP_NETWORK_WEIGHTING), tempint );
    
/*
    // seems that location is fixed so no read...
    ApCommons::ReadUintL( iapt, TPtrC(IAP_LOCATION), tempint );
    aApItem.i = tempint;
*/


    aApItem.SpecifyIAP( ETrue );
    CleanupStack::PopAndDestroy( iapt );  // iapt

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::ReadIapDataL" ) ) );
    }





// ---------------------------------------------------------
// CApDataHandler::ReadIspDataL
// ---------------------------------------------------------
//
void CApDataHandler::ReadIspDataL( TUint32 aIspId, 
                                   CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::ReadIspDataL" ) ) );

    /*****************************************************
    *   Series 60 Customer / ETel
    *   Series 60  ETel API
    *****************************************************/
    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    HBufC* buf = HBufC::NewLC( KModifiableTextLength );
    TPtr16 ptr = buf->Des();
    aApItem.ReadTextL( EApIapServiceType, ptr );

    if ( ( buf->Compare( TPtrC(OUTGOING_WCDMA ) ) == 0 ) ||
        ( buf->Compare( TPtrC(INCOMING_WCDMA ) ) == 0 ) )
        {
        ReadGprsDataL( aIspId, aApItem );
        }
    else
        {
        if ( ( buf->Compare( TPtrC(DIAL_OUT_ISP ) ) == 0 ) ||
            ( buf->Compare( TPtrC(DIAL_IN_ISP ) ) == 0 ) )
            {
            ReadDialInOutDataL( aIspId, aApItem );
            }
        else
            { // e.g. vpn...
            if ( buf->Compare( TPtrC(VPN_SERVICE) ) == 0 )
                { // VPN
                aApItem.iExt->iIsVpnAp = ETrue;
                // get vpn bearer type
                ReadVpnInfoL( aIspId, aApItem );
                }
            else
                {
                if ( buf->Compare( TPtrC(LAN_SERVICE) ) == 0 )
                    { // LAN
                    // get LAN data (WLAN)
                    ReadLanDataL( aIspId, aApItem );
                    }
                else
                    {
                    User::Leave( KErrInvalidBearer );
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy( buf );   // buf

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::ReadIspDataL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::ReadGprsDataL
// ---------------------------------------------------------
//
void CApDataHandler::ReadGprsDataL( TUint32 aIspId,
                                    CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::ReadGprsDataL" ) ) );

    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CCommsDbTableView* ispt = NULL;

    HBufC* servtype = HBufC::NewLC( KModifiableTextLength );
    TPtr16 ptr = servtype->Des();
    aApItem.ReadTextL( EApIapServiceType, ptr );

    if ( servtype->Compare( TPtrC(OUTGOING_WCDMA ) ) == 0 )
        {
        ispt = iDb->OpenViewMatchingUintLC( TPtrC(OUTGOING_WCDMA),
                                           TPtrC(COMMDB_ID),
                                           aIspId );
        }
    else
        {
        if ( servtype->Compare( TPtrC(INCOMING_WCDMA ) ) == 0 )
            {
            ispt = iDb->OpenViewMatchingUintLC( TPtrC(INCOMING_WCDMA),
                                               TPtrC(COMMDB_ID),
                                               aIspId );
            }
        else
            {
            User::Leave( KErrInvalidBearer );
            }
        }


    User::LeaveIfError( ispt->GotoFirstRecord() );

    // now read record data
    HBufC* buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(COMMDB_NAME) );
    aApItem.WriteTextL( EApIspName, *buf );
    CleanupStack::PopAndDestroy( buf );

    TInt err( KErrNone );
    buf = ApCommons::ReadLongTextLC( ispt, TPtrC(GPRS_APN), err );
    if ( err != KErrUnknown )
        {
        aApItem.WriteLongTextL( EApGprsAccessPointName, *buf );
        CleanupStack::PopAndDestroy( buf );
        }

    TUint32 tempint( 0 );
    TBool   tempbool( EFalse );

    ApCommons::ReadUintL( ispt, TPtrC(GPRS_PDP_TYPE), tempint );
    if ( tempint == RPacketContext::EPdpTypeIPv6 )
        {
        aApItem.WriteUint( EApGprsPdpType, EIPv6 );
        }
    else
        {
        aApItem.WriteUint( EApGprsPdpType, EIPv4 );
        }

    buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(GPRS_PDP_ADDRESS) );
    aApItem.WriteTextL( EApGprsPdpAddress, *buf );
    CleanupStack::PopAndDestroy( buf );

    ApCommons::ReadUintL( ispt, TPtrC(GPRS_REQ_PRECEDENCE), tempint );
    aApItem.WriteUint( EApGprsReqPrecedence, tempint);

    ApCommons::ReadUintL( ispt, TPtrC(GPRS_REQ_DELAY), tempint );
    aApItem.WriteUint( EApGprsReqDelay, tempint);

    ApCommons::ReadUintL( ispt, TPtrC(GPRS_REQ_RELIABILITY), tempint );
    aApItem.WriteUint( EApGprsReqReliability, tempint);

    ApCommons::ReadUintL( ispt, TPtrC(GPRS_REQ_PEAK_THROUGHPUT), tempint );
    aApItem.WriteUint( EApGprsReqPeakThroughput, tempint);

    ApCommons::ReadUintL( ispt, TPtrC(GPRS_REQ_MEAN_THROUGHPUT), tempint );
    aApItem.WriteUint( EApGprsReqMeanPeakThroughput, tempint);

    ApCommons::ReadUintL( ispt, TPtrC(GPRS_MIN_PRECEDENCE), tempint );
    aApItem.WriteUint( EApGprsMinPrecedence, tempint);

    ApCommons::ReadUintL( ispt, TPtrC(GPRS_MIN_DELAY), tempint );
    aApItem.WriteUint( EApGprsMinDelay, tempint);

    ApCommons::ReadUintL( ispt, TPtrC(GPRS_MIN_RELIABILITY), tempint );
    aApItem.WriteUint( EApGprsMinReliability, tempint);

    ApCommons::ReadUintL( ispt, TPtrC(GPRS_MIN_PEAK_THROUGHPUT), tempint );
    aApItem.WriteUint( EApGprsMinPeakThroughput, tempint);

    ApCommons::ReadUintL( ispt, TPtrC(GPRS_MIN_MEAN_THROUGHPUT), tempint );
    aApItem.WriteUint( EApGprsMinMeanThroughput, tempint);

    ApCommons::ReadBoolL( ispt, TPtrC(GPRS_DATA_COMPRESSION), tempbool );
    aApItem.WriteBool( EApGprsDataCompression, tempbool );

    ApCommons::ReadBoolL( ispt, TPtrC(GPRS_HEADER_COMPRESSION), tempbool );
    aApItem.WriteBool( EApGprsHeaderCompression, tempbool );

    ApCommons::ReadBoolL( ispt, TPtrC(GPRS_ANONYMOUS_ACCESS), tempbool );
    aApItem.WriteBool( EApGprsUseAnonymAccess, tempbool );


    buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(GPRS_IF_PARAMS) );
    aApItem.WriteTextL( EApGprsIfParams, *buf );
    CleanupStack::PopAndDestroy( buf );

    buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(GPRS_IF_NETWORKS) );
    aApItem.WriteTextL( EApGprsIfNetworks, *buf );
    CleanupStack::PopAndDestroy( buf );

    ApCommons::ReadBoolL( ispt, TPtrC(GPRS_IF_PROMPT_FOR_AUTH), tempbool );
    aApItem.WriteBool( EApGprsIfPromptForAuth, tempbool );


    buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(GPRS_IF_AUTH_NAME) );
    aApItem.WriteTextL( EApGprsIfAuthName, *buf );
    CleanupStack::PopAndDestroy( buf );

    buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(GPRS_IF_AUTH_PASS) );
    aApItem.WriteTextL( EApGprsIfAuthPassword, *buf );
    CleanupStack::PopAndDestroy( buf ); 

    ApCommons::ReadUintL( ispt, TPtrC(GPRS_IF_AUTH_RETRIES), tempint );
    aApItem.WriteUint( EApGprsIfAuthRetries, tempint );

    buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(GPRS_IP_NETMASK) );
    aApItem.WriteTextL( EApGprsIpNetMask, *buf );
    CleanupStack::PopAndDestroy( buf );

    buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(GPRS_IP_GATEWAY) );
    aApItem.WriteTextL( EApGprsIpGateway, *buf );
    CleanupStack::PopAndDestroy( buf );

    ApCommons::ReadBoolL( ispt, TPtrC(GPRS_IP_ADDR_FROM_SERVER), tempbool );
    aApItem.WriteBool( EApGprsIpAddrFromServer, tempbool );

    buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(GPRS_IP_ADDR) );
    aApItem.WriteTextL( EApGprsIpAddr, *buf );
    CleanupStack::PopAndDestroy( buf );

    ApCommons::ReadBoolL( ispt, TPtrC(GPRS_IP_DNS_ADDR_FROM_SERVER),
                           tempbool );
    aApItem.WriteBool( EApGprsIpDnsAddrFromServer, tempbool );

    buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(GPRS_IP_NAME_SERVER1) );
    aApItem.WriteTextL( EApGprsIPNameServer1, *buf );
    CleanupStack::PopAndDestroy( buf );

    buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(GPRS_IP_NAME_SERVER2) );
    aApItem.WriteTextL( EApGprsIPNameServer2, *buf );
    CleanupStack::PopAndDestroy( buf ); 

    ApCommons::ReadBoolL( ispt, TPtrC(GPRS_ENABLE_LCP_EXTENSIONS), tempbool );
    aApItem.WriteBool( EApGprsEnableLCPExtensions, tempbool );


    ApCommons::ReadBoolL( ispt, TPtrC(GPRS_DISABLE_PLAIN_TEXT_AUTH),
                           tempbool );
    aApItem.WriteBool( EApGprsDisablePlainTextAuth, tempbool );

    ApCommons::ReadUintL( ispt, TPtrC(GPRS_AP_TYPE), tempint );
    aApItem.WriteUint( EApIspIspType, TCommsDbIspType( tempint ) );

    if ( iExt->iIsIpv6Supported )
        {
        ReadServiceIp6L( *ispt, aApItem );
        }

    aApItem.SpecifyGPRS( ETrue );
    CleanupStack::PopAndDestroy( 2, servtype );  // ispt, servtype

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::ReadGprsDataL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::ReadLanDataL
// ---------------------------------------------------------
//
void CApDataHandler::ReadLanDataL( TUint32 aIspId,
                                   CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::ReadLanDataL" ) ) );

    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CCommsDbTableView* ispt = NULL;

    ispt = iDb->OpenViewMatchingUintLC( TPtrC(LAN_SERVICE),
                                           TPtrC(COMMDB_ID),
                                           aIspId );
    User::LeaveIfError( ispt->GotoFirstRecord() );

    // now read record data
    ReadServiceL( *ispt, aApItem );

    CCommsDbTableView* lant = NULL;
    // now see if it is WLAN
    
    if ( aApItem.iExt->iWlanData )
        { // WLAN is supported
        lant = iDb->OpenViewMatchingUintLC( TPtrC(WLAN_SERVICE ),
                                            TPtrC(WLAN_SERVICE_ID),
                                            aIspId );
    
        TInt err = lant->GotoFirstRecord();
        // it can be Ethernet Lan, WLAN and LanModem, 
        // from those, only WLAN has these fields,
        // so if we can read them, it is wlan. If not, it is something else...
        if ( err == KErrNone )
            {
            // now read record data
            ReadWlanL( *lant, aApItem );
            aApItem.iExt->iWlanData->iIsWlan = ETrue;
            }
        else
            { 
            if ( err != KErrNotFound )
                {
                User::LeaveIfError( err );
                }
            // can not leave on not found error, as it still might be 
            // a valid bearer after all...
            aApItem.iExt->iWlanData->iIsWlan = EFalse;
            }
        aApItem.SpecifyWLAN( ETrue );
        CleanupStack::PopAndDestroy( lant ); 
        }
        
    if ( iExt->iIsIpv6Supported )
        {
        ReadServiceIp6L( *ispt, aApItem );
        }



    CleanupStack::PopAndDestroy( ispt ); 

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::ReadLanDataL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::ReadDialInOutDataL
// ---------------------------------------------------------
//
void CApDataHandler::ReadDialInOutDataL( TUint32 aIspId,
                                         CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::ReadDialInOutDataL" ) ) );
    /*****************************************************
    *   Series 60 Customer / ETel
    *   Series 60  ETel API
    *****************************************************/

    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CCommsDbTableView* ispt = NULL;

    HBufC* servtype = HBufC::NewLC( KModifiableTextLength );
    TPtr16 ptr = servtype->Des();
    aApItem.ReadTextL( EApIapServiceType, ptr );

    if ( servtype->Compare( TPtrC(DIAL_OUT_ISP) ) == 0 )    
        {
        ispt = iDb->OpenViewMatchingUintLC( TPtrC(DIAL_OUT_ISP),
                                            TPtrC(COMMDB_ID), aIspId );
        }
    else
        {
        ispt = iDb->OpenViewMatchingUintLC( TPtrC(DIAL_IN_ISP),
                                            TPtrC(COMMDB_ID), aIspId );
        }

    User::LeaveIfError( ispt->GotoFirstRecord() );

    TUint32 tempint( 0 );
    TBool   tempbool( EFalse );

    HBufC* buf = NULL;

    aApItem.WriteUint( EApIapServiceId, aIspId );


    buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(COMMDB_NAME) );
    aApItem.WriteTextL( EApIspName, *buf );
    CleanupStack::PopAndDestroy( buf ); 

    buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(ISP_DESCRIPTION) );
    aApItem.WriteTextL( EApIspDescription, *buf );
    CleanupStack::PopAndDestroy( buf );


    ApCommons::ReadUintL( ispt, TPtrC(ISP_TYPE), tempint );
    aApItem.WriteUint( EApIspIspType, TCommsDbIspType( tempint ) );

    buf = ApCommons::ReadText16ValueLC( ispt,
                                         TPtrC(ISP_DEFAULT_TEL_NUM) );
    aApItem.WriteTextL( EApIspDefaultTelNumber, *buf );
    CleanupStack::PopAndDestroy( buf );


    ApCommons::ReadBoolL( ispt, TPtrC(ISP_DIAL_RESOLUTION), tempbool );
    aApItem.WriteBool( EApIspDialResolution, tempbool );

    ApCommons::ReadBoolL( ispt, TPtrC(ISP_USE_LOGIN_SCRIPT), tempbool );
    aApItem.WriteBool( EApIspUseLoginScript, tempbool );

    TInt err;
    buf = ApCommons::ReadLongTextLC( ispt, TPtrC(ISP_LOGIN_SCRIPT), err );
    aApItem.WriteLongTextL( EApIspLoginScript, *buf );
    CleanupStack::PopAndDestroy( buf );


    ApCommons::ReadBoolL( ispt, TPtrC(ISP_PROMPT_FOR_LOGIN), tempbool );
    aApItem.WriteBool( EApIspPromptForLogin, tempbool );

    buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(ISP_LOGIN_NAME) );
    // as same UserName & AuthName is required, it is readed from username
    aApItem.WriteTextL( EApIspLoginName, *buf );
    aApItem.WriteTextL( EApIspIfAuthName, *buf );
    CleanupStack::PopAndDestroy( buf );


    buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(ISP_LOGIN_PASS) );
    // as same Login_pass & AuthPassword is required,
    // it is readed from login_pass
    aApItem.WriteTextL( EApIspLoginPass, *buf );
    aApItem.WriteTextL( EApIspIfAuthPass, *buf );
    CleanupStack::PopAndDestroy( buf );


    ApCommons::ReadBoolL( ispt, TPtrC(ISP_DISPLAY_PCT), tempbool );
    aApItem.WriteBool( EApIspDisplayPCT, tempbool );


    buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(ISP_IF_PARAMS) );
    aApItem.WriteTextL( EApIspIfParams, *buf );
    CleanupStack::PopAndDestroy( buf );

    buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(ISP_IF_NETWORKS) );
    aApItem.WriteTextL( EApIspIfNetworks, *buf );
    CleanupStack::PopAndDestroy( buf );

    ApCommons::ReadBoolL( ispt, TPtrC(ISP_IF_PROMPT_FOR_AUTH), tempbool );
    aApItem.WriteBool( EApIspIfPromptForAuth, tempbool );


    ApCommons::ReadUintL( ispt, TPtrC(ISP_IF_AUTH_RETRIES), tempint );
    aApItem.WriteUint( EApIspAuthRetries, tempint );

    ApCommons::ReadBoolL( ispt,
                           TPtrC(ISP_IF_CALLBACK_ENABLED), tempbool );

    aApItem.WriteBool( EApIspIfCallbackEnabled, tempbool );


    ApCommons::ReadUintL( ispt, TPtrC(ISP_IF_CALLBACK_TYPE), tempint );
    aApItem.WriteUint( EApIspIfCallbackType, ( TCallbackAction )tempint );


    HBufC8* buf8 = ApCommons::ReadText8ValueLC
                                    (
                                    ispt,
                                    TPtrC(ISP_IF_CALLBACK_INFO)
                                    );
    aApItem.WriteTextL( EApIspIfCallbackInfo, *buf8 );
    CleanupStack::PopAndDestroy( buf8 ); 

    ApCommons::ReadUintL( ispt, TPtrC(ISP_CALLBACK_TIMEOUT), tempint );
    aApItem.WriteUint( EApIspCallBackTimeOut, tempint );


    ApCommons::ReadBoolL( ispt,
                           TPtrC(ISP_IP_ADDR_FROM_SERVER), tempbool );

    aApItem.WriteBool( EApIspIPAddrFromServer, tempbool );

    buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(ISP_IP_ADDR) );
    aApItem.WriteTextL( EApIspIPAddr, *buf );
    CleanupStack::PopAndDestroy( buf );

    buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(ISP_IP_NETMASK) );
    aApItem.WriteTextL( EApIspIPNetMask, *buf );
    CleanupStack::PopAndDestroy( buf );

    buf = ApCommons::ReadText16ValueLC( ispt, TPtrC(ISP_IP_GATEWAY) );
    aApItem.WriteTextL( EApIspIPGateway, *buf );
    CleanupStack::PopAndDestroy( buf );

    ApCommons::ReadBoolL( ispt, TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),
                           tempbool );

    aApItem.WriteBool( EApIspIPDnsAddrFromServer, tempbool );

    buf = ApCommons::ReadText16ValueLC( ispt,
                                         TPtrC(ISP_IP_NAME_SERVER1) );

    aApItem.WriteTextL( EApIspIPNameServer1, *buf );
    CleanupStack::PopAndDestroy( buf ); 

    buf = ApCommons::ReadText16ValueLC( ispt,
                                         TPtrC(ISP_IP_NAME_SERVER2) );

    aApItem.WriteTextL( EApIspIPNameServer2, *buf );
    CleanupStack::PopAndDestroy( buf );


    ApCommons::ReadBoolL( ispt, TPtrC(ISP_ENABLE_IP_HEADER_COMP),
                           tempbool );

    aApItem.WriteBool( EApIspEnableIpHeaderComp, tempbool );


    ApCommons::ReadBoolL( ispt, TPtrC(ISP_ENABLE_LCP_EXTENSIONS),
                           tempbool );

    aApItem.WriteBool( EApIspEnableLCPExtensions, tempbool );


    ApCommons::ReadBoolL( ispt, TPtrC(ISP_DISABLE_PLAIN_TEXT_AUTH),
                           tempbool );

    aApItem.WriteBool( EApIspDisablePlainTextAuth, tempbool );


    ApCommons::ReadBoolL( ispt, TPtrC(ISP_ENABLE_SW_COMP), tempbool );

    aApItem.WriteBool( EApIspEnableSWCompression, tempbool );


    ApCommons::ReadUintL( ispt, TPtrC(ISP_BEARER_NAME), tempint );
    aApItem.WriteUint( EApIspBearerName,
                       (RMobileCall::TMobileCallDataServiceCaps)tempint );

    ApCommons::ReadUintL( ispt, TPtrC(ISP_BEARER_SPEED), tempint );
    aApItem.iExt->iBearerSpeed = tempint;

    ApCommons::ReadUintL( ispt, TPtrC(ISP_BEARER_PROTOCOL), tempint );
    aApItem.iExt->iBearerProtocol = tempint;

    ApCommons::ReadUintL( ispt, TPtrC(ISP_BEARER_CE), tempint );
    aApItem.WriteUint( EApIspBearerCE, 
                      (RMobileCall::TMobileCallDataQoSCaps) tempint );

    ApCommons::ReadUintL( ispt, TPtrC(ISP_BEARER_SERVICE), tempint );
    aApItem.WriteUint( EApIapBearerService, tempint );



    buf8 = ApCommons::ReadText8ValueLC( ispt,
                                         TPtrC(ISP_INIT_STRING) );

    aApItem.WriteTextL( EApIspInitString, *buf8 );
    CleanupStack::PopAndDestroy( buf8 );

    ApCommons::ReadUintL( ispt, TPtrC(ISP_BEARER_TYPE), tempint );
    aApItem.WriteUint( EApIspBearerType, tempint);

    ApCommons::ReadUintL( ispt, TPtrC(ISP_CHANNEL_CODING), tempint );
    aApItem.WriteUint( EApIspChannelCoding, tempint);

    ApCommons::ReadUintL( ispt, TPtrC(ISP_AIUR), tempint );
    aApItem.WriteUint( EApIspAIUR, tempint);

    ApCommons::ReadUintL( ispt,
                           TPtrC(ISP_REQUESTED_TIME_SLOTS),
                           tempint );
    aApItem.WriteUint( EApIspRequestedTimeSlots, tempint);

    ApCommons::ReadUintL( ispt, TPtrC(ISP_MAXIMUM_TIME_SLOTS), tempint );
    aApItem.WriteUint( EApIspMaximumTimeSlots, tempint);


    if ( iExt->iIsIpv6Supported )
        {
        ReadServiceIp6L( *ispt, aApItem );
        }

    EtelSpeed2ApL( aApItem );

    aApItem.SpecifyISP( ETrue );
    CleanupStack::PopAndDestroy( 2, servtype );   // ispt, servtype

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::ReadDialInOutDataL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::ReadVpnInfoL
// ---------------------------------------------------------
//
void CApDataHandler::ReadVpnInfoL( TUint32 aIspId, 
                                   CApAccessPointItem& aApItem)
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::ReadVpnInfoL" ) ) );

    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CCommsDbTableView* ispt = NULL;

    ispt = iDb->OpenViewMatchingUintLC( TPtrC(VPN_SERVICE),
                                        TPtrC(COMMDB_ID), aIspId );
    User::LeaveIfError( ispt->GotoFirstRecord() );

    TUint32 tempint;

    HBufC* buf = NULL;
    // get the real iap
    ApCommons::ReadUintL( ispt, TPtrC(VPN_SERVICE_IAP), tempint );

    CCommsDbTableView* iapt = NULL;
    iapt = iDb->OpenViewMatchingUintLC( TPtrC(IAP),
                                        TPtrC(COMMDB_ID), tempint );

    User::LeaveIfError( iapt->GotoFirstRecord() );

    TUint32 iapservice;
    ApCommons::ReadUintL( iapt, TPtrC(IAP_SERVICE), iapservice );
    
    buf = ApCommons::ReadText16ValueLC( iapt, TPtrC(IAP_SERVICE_TYPE) );
    
    if ( ( buf->Compare( TPtrC(OUTGOING_WCDMA ) ) == 0 ) ||
        ( buf->Compare( TPtrC(INCOMING_WCDMA ) ) == 0 ) )
        {
        aApItem.iExt->iVPnRealIapBearerType = EApBearerTypeGPRS;
        }
    else
        {
        if ( ( buf->Compare( TPtrC(DIAL_OUT_ISP ) ) == 0 ) ||
            ( buf->Compare( TPtrC(DIAL_IN_ISP ) ) == 0 ) )
            {
            // need to get ISP,
            CCommsDbTableView* doutt = NULL;
            doutt = iDb->OpenViewMatchingUintLC( *buf,
                                                 TPtrC(COMMDB_ID), 
                                                 iapservice );
            User::LeaveIfError( doutt->GotoFirstRecord() );
            ApCommons::ReadUintL( doutt , TPtrC(ISP_BEARER_TYPE), tempint );
            if ( tempint == EBearerTypeCSD )
                {
                aApItem.iExt->iVPnRealIapBearerType = EApBearerTypeCSD;
                }
            else
                {
                aApItem.iExt->iVPnRealIapBearerType = EApBearerTypeHSCSD;
                }
            CleanupStack::PopAndDestroy( doutt ); // doutt
            }
        else
            {
            // VPN, having a bearer something else than CSD, HSCSD,
            // GPRS & CDMA?
            if ( buf->Compare( TPtrC(LAN_SERVICE) ) == 0 )
                { // LAN
                ReadLanDataL( aIspId, aApItem );
                }
            else
                { // otherwise, error
                User::Leave( KErrInvalidBearer );
                }
            }
        }

    CleanupStack::PopAndDestroy( 3, ispt ); // buf, iapt, ispt

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::ReadVpnInfoL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::WriteWapBearerDataL
// ---------------------------------------------------------
//
void CApDataHandler::WriteWapBearerDataL( TBool aIsNew, TUint32 aWapId,
                                   CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::WriteWapBearerDataL" ) ) );

    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CCommsDbTableView* bearert;
    if ( aIsNew )
        {
        bearert = iDb->OpenTableLC( TPtrC(*aApItem.iWapBearer) );
        // CommDB's back-link makes returned UID value unusable,
        // variable needed just to be able to make the call...
        TUint32 dummy;
        User::LeaveIfError( bearert->InsertRecord( dummy ) );
        }
    else
        {
        bearert = iDb->OpenViewMatchingUintLC( TPtrC(*aApItem.iWapBearer),
                                              TPtrC(WAP_ACCESS_POINT_ID),
                                              aWapId );
        User::LeaveIfError( bearert->GotoFirstRecord() );
        User::LeaveIfError( bearert->UpdateRecord() );
        }
    bearert->WriteTextL( TPtrC(WAP_GATEWAY_ADDRESS),
                         *aApItem.iWapGatewayAddress );
    bearert->WriteUintL( TPtrC(WAP_WSP_OPTION),
                         aApItem.iIsConnectionTypeContinuous );
    bearert->WriteBoolL( TPtrC(WAP_SECURITY), aApItem.iIsWTLSSecurityOn );
    bearert->WriteUintL( TPtrC(WAP_ACCESS_POINT_ID), aWapId );

    if ( *aApItem.iWapBearer == TPtrC(WAP_IP_BEARER) )
        {
        bearert->WriteUintL( TPtrC(WAP_IAP), aApItem.iWapIap );        
        bearert->WriteUintL( TPtrC(WAP_PROXY_PORT), aApItem.iWapProxyPort );
        bearert->WriteTextL( TPtrC(WAP_PROXY_LOGIN_NAME),
                             *aApItem.iWapProxyLoginName );
        bearert->WriteTextL( TPtrC(WAP_PROXY_LOGIN_PASS),
                             *aApItem.iWapProxyLoginPass );
        }
    else
        {
        User::Leave( KErrInvalidBearer );
        }

    User::LeaveIfError( bearert->PutRecordChanges( EFalse, EFalse ) );

    CleanupStack::PopAndDestroy( bearert );   // bearert

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::WriteWapBearerDataL" ) ) );
    }




// ---------------------------------------------------------
// CApDataHandler::WriteWapApDataL
// ---------------------------------------------------------
//
void CApDataHandler::WriteWapApDataL( TBool aIsNew, TUint32 aWapId,
                                   CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::WriteWapApDataL" ) ) );

    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CCommsDbTableView* wapt;

    if ( aIsNew )
        {
        wapt = iDb->OpenTableLC( TPtrC(WAP_ACCESS_POINT) );
        TInt err = wapt->InsertRecord( aApItem.iWapUid );
        if ( err )
            {
            User::LeaveIfError( err );
            }
        // CommDB's back-link makes returned UID value unusable,
        // variable needed just to be able to make the call...
        }
    else
        {
        wapt = iDb->OpenViewMatchingUintLC( TPtrC(WAP_ACCESS_POINT),
                                           TPtrC(COMMDB_ID), aWapId );
        User::LeaveIfError( wapt->GotoFirstRecord() );
        User::LeaveIfError( wapt->UpdateRecord() );
        }

    wapt->WriteTextL( TPtrC(WAP_CURRENT_BEARER), *aApItem.iWapBearer );
    wapt->WriteTextL( TPtrC(COMMDB_NAME), *aApItem.iWapAccessPointName );

    wapt->WriteLongTextL( TPtrC(WAP_START_PAGE), *aApItem.iStartingPage );

    User::LeaveIfError( wapt->PutRecordChanges( EFalse, EFalse ) );

    CleanupStack::PopAndDestroy( wapt );   // wapt

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::WriteWapApDataL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::WriteIapDataL
// ---------------------------------------------------------
//
void CApDataHandler::WriteIapDataL( TBool aIsNew, TUint32 aIapId,
                                   CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::WriteIapDataL" ) ) );

    CCommsDbTableView* iapt;
    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    if ( aIsNew )
        {
        iapt = iDb->OpenTableLC( TPtrC(IAP) );
        User::LeaveIfError( iapt->InsertRecord( aApItem.iWapIap ) );
        }
    else
        {
        iapt = iDb->OpenViewMatchingUintLC( TPtrC(IAP),
                                               TPtrC(COMMDB_ID), aIapId );
        User::LeaveIfError( iapt->GotoFirstRecord() );
        User::LeaveIfError( iapt->UpdateRecord() );
        }

    iapt->WriteTextL( TPtrC(COMMDB_NAME), *aApItem.iIapName );
    iapt->WriteUintL( TPtrC(IAP_SERVICE), aApItem.iIapServiceId );
    iapt->WriteTextL( TPtrC(IAP_SERVICE_TYPE), *aApItem.iIapServiceType );

    // hardcoded...
    TApBearerType bearer = aApItem.BearerTypeL();
    switch ( bearer )
        {
        case EApBearerTypeWLAN:
            {
            if ( aApItem.iExt->iWlanData )
                {
                iapt->WriteTextL( TPtrC(IAP_BEARER_TYPE), TPtrC(LAN_BEARER) );
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
#ifdef __TEST_LAN_BEARER 
        case EApBearerTypeLAN:
            {
            iapt->WriteTextL( TPtrC(IAP_BEARER_TYPE), TPtrC(LAN_BEARER) );
            break;
            }
#endif // __TEST_LAN_BEARER        
        case EApBearerTypeLANModem:
            {
            iapt->WriteTextL( TPtrC(IAP_BEARER_TYPE), TPtrC(LAN_BEARER) );
            break;
            }
        default:
            {
            iapt->WriteTextL( TPtrC(IAP_BEARER_TYPE), TPtrC(MODEM_BEARER) );
            break;
            }            
        }
        
    TUint32 id = GetModemBearerIDL( aApItem );
    iapt->WriteUintL( TPtrC(IAP_BEARER), id );
    
    iapt->WriteUintL( TPtrC(IAP_NETWORK), aApItem.iNetworkId );
    // Network weighting!
    iapt->WriteUintL( TPtrC(IAP_NETWORK_WEIGHTING), 0 );
    
    TUint32 loc = GetLocationIdL();
    iapt->WriteUintL( TPtrC(IAP_LOCATION), loc );

    // now put changes
    User::LeaveIfError( iapt->PutRecordChanges( EFalse, EFalse ) );

    CleanupStack::PopAndDestroy( iapt );  // iapt

    if ( aIsNew )
        {
        CCommsDbConnectionPrefTableView* view = 
            iDb->OpenConnectionPrefTableViewOnRankLC( 
                    ECommDbConnectionDirectionOutgoing, 1 );
        
        TInt ret = view->GotoFirstRecord();
        
        CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref tablePref;
        
        if ( ret == KErrNone )
            {
            view->ReadConnectionPreferenceL( tablePref );
                
            TUint32 iapId = tablePref.iBearer.iIapId;
                
            // Check if iap exists
            CCommsDbTableView* view2 = 
                iDb->OpenViewMatchingUintLC( TPtrC(IAP), 
                                             TPtrC(COMMDB_ID), 
                                             iapId );
        
            TInt exists = view2->GotoFirstRecord();
            CleanupStack::PopAndDestroy(); // view2

            if ( iapId == 0 || exists != KErrNone )
                {
                if ( bearer & 
                   ( EApBearerTypeCSD 
                     + EApBearerTypeHSCSD 
                     + EApBearerTypeGPRS 
                     + EApBearerTypeCDMA ) )
                    {
                    // Get the current settings and then overwrite with the Iap
                    tablePref.iBearer.iIapId = aApItem.iWapIap;
                    tablePref.iBearer.iBearerSet = 
                            KCommDbBearerCSD | KCommDbBearerWcdma;
                    view->UpdateBearerL( tablePref.iBearer );
                    }
                }
            }
        CleanupStack::PopAndDestroy(); // view
        }

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::WriteIapDataL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::WriteIspDataL
// ---------------------------------------------------------
//
void CApDataHandler::WriteIspDataL( TBool aIsNew, TUint32 aIspId,
                                   CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::WriteIspDataL" ) ) );

    aApItem.WriteUint( EApIapServiceId, aIspId );
    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    if ( ( aApItem.iIapServiceType->Compare( TPtrC(OUTGOING_WCDMA ) ) == 0 ) ||
        ( aApItem.iIapServiceType->Compare( TPtrC(INCOMING_WCDMA ) ) == 0 ) )
        {
        WriteGprsDataL( aIsNew, aApItem );
        }
    else
        {
        if ( ( aApItem.iIapServiceType->Compare( TPtrC(DIAL_IN_ISP) ) == 0 ) ||
            ( aApItem.iIapServiceType->Compare( TPtrC(DIAL_OUT_ISP) ) == 0 ) )
            {
            WriteCsdDataL( aIsNew, aApItem );
            }
        else
            { // check if VPN
            if ( aApItem.iIapServiceType->Compare( TPtrC(VPN_SERVICE) ) == 0 )
                { // currently, KErrNotSupported
                User::Leave( KErrNotSupported );
                }
            else
                { 
                if ( aApItem.iIapServiceType->Compare( 
                                TPtrC(LAN_SERVICE) ) == 0 )
                    {
                    WriteLanDataL( aIsNew, aApItem );
                    }
                else
                    {
                    // something unknown, 
                    User::Leave( KErrInvalidBearer );
                    }
                }
            }
        }

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::WriteIspDataL" ) ) );
    }




// ---------------------------------------------------------
// CApDataHandler::WriteGprsDataL
// ---------------------------------------------------------
//
void CApDataHandler::WriteGprsDataL( TBool aIsNew,
                                    CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::WriteGprsDataL" ) ) );

    CCommsDbTableView* table;

    if ( aIsNew )
        {
        table = iDb->OpenTableLC( *aApItem.iIapServiceType );
        User::LeaveIfError( table->InsertRecord( aApItem.iIapServiceId ) );
        }
    else
        {
        table = iDb->OpenViewMatchingUintLC( *aApItem.iIapServiceType,
                                           TPtrC(COMMDB_ID),
                                           aApItem.iIapServiceId );
        User::LeaveIfError( table->GotoFirstRecord() );
        User::LeaveIfError( table->UpdateRecord() );
        }
    // now write record data
    table->WriteTextL( TPtrC(COMMDB_NAME), *aApItem.iIspName );

    table->WriteLongTextL( TPtrC(GPRS_APN), *aApItem.iGprsAccessPointName );

    
    if ( aApItem.iGprsPdpType == EIPv6 )
        {
        table->WriteUintL( TPtrC(GPRS_PDP_TYPE), 
                           RPacketContext::EPdpTypeIPv6 );
        }
    else
        {
        table->WriteUintL( TPtrC(GPRS_PDP_TYPE), 
                           RPacketContext::EPdpTypeIPv4 );
        }

    table->WriteTextL( TPtrC(GPRS_PDP_ADDRESS), *aApItem.iGprsPdpAddress );

    table->WriteUintL( TPtrC(GPRS_REQ_PRECEDENCE),
                       aApItem.iGprsReqPrecedence );

    table->WriteUintL( TPtrC(GPRS_REQ_DELAY), aApItem.iGprsReqDelay );

    table->WriteUintL( TPtrC(GPRS_REQ_RELIABILITY),
                       aApItem.iGprsReqReliability );

    table->WriteUintL( TPtrC(GPRS_REQ_PEAK_THROUGHPUT),
                       aApItem.iGprsReqPeakThroughput );

    table->WriteUintL( TPtrC(GPRS_REQ_MEAN_THROUGHPUT),
                       aApItem.iGprsReqMeanPeakThroughput );

    table->WriteUintL( TPtrC(GPRS_MIN_PRECEDENCE),
                       aApItem.iGprsMinPrecedence );

    table->WriteUintL( TPtrC(GPRS_MIN_DELAY), aApItem.iGprsMinDelay );

    table->WriteUintL( TPtrC(GPRS_MIN_RELIABILITY),
                       aApItem.iGprsMinReliability );

    table->WriteUintL( TPtrC(GPRS_MIN_PEAK_THROUGHPUT),
                       aApItem.iGprsMinPeakThroughput );

    table->WriteUintL( TPtrC(GPRS_MIN_MEAN_THROUGHPUT),
                       aApItem.iGprsMinMeanThroughput );

    table->WriteBoolL( TPtrC(GPRS_DATA_COMPRESSION),
                     aApItem.iIspEnableSwCompression );

    table->WriteBoolL( TPtrC(GPRS_HEADER_COMPRESSION),
                     aApItem.iEnablePPPCompression );

    table->WriteBoolL( TPtrC(GPRS_ANONYMOUS_ACCESS),
                     aApItem.iGprsUseAnonymAccess );

/*
// Keep as long as replacing IF_NAME does not works as specified 
    table->WriteTextL( TPtrC(IF_NAME), *aApItem.iIspIfName );
*/

    table->WriteTextL( TPtrC(GPRS_IF_PARAMS), *aApItem.iIspIfParams );

    table->WriteTextL( TPtrC(GPRS_IF_NETWORKS), *aApItem.iIspIfNetworks );

    table->WriteBoolL( TPtrC(GPRS_IF_PROMPT_FOR_AUTH),
                     aApItem.iIspIfPromptForAuth );

    table->WriteTextL( TPtrC(GPRS_IF_AUTH_NAME), *aApItem.iIspIfAuthName );

    table->WriteTextL( TPtrC(GPRS_IF_AUTH_PASS), *aApItem.iIspIfAuthPass );

    table->WriteUintL( TPtrC(GPRS_IF_AUTH_RETRIES),
                     aApItem.iIspIfAuthRetries );

    table->WriteTextL( TPtrC(GPRS_IP_NETMASK), *aApItem.iIspIPNetMask );

    table->WriteTextL( TPtrC(GPRS_IP_GATEWAY), *aApItem.iIspGateway );

    table->WriteBoolL( TPtrC(GPRS_IP_ADDR_FROM_SERVER),
                       aApItem.iIspIPAddrFromServer );

    table->WriteTextL( TPtrC(GPRS_IP_ADDR), *aApItem.iIspIPAddr );

    table->WriteBoolL( TPtrC(GPRS_IP_DNS_ADDR_FROM_SERVER),
                     aApItem.iGetDNSIPFromServer );

    table->WriteTextL( TPtrC(GPRS_IP_NAME_SERVER1), *aApItem.iPrimaryDNS );

    table->WriteTextL( TPtrC(GPRS_IP_NAME_SERVER2), *aApItem.iSecondaryDNS );

    table->WriteBoolL( TPtrC(GPRS_ENABLE_LCP_EXTENSIONS),
                     aApItem.iIspEnableLCPExtensions );

    table->WriteBoolL( TPtrC(GPRS_DISABLE_PLAIN_TEXT_AUTH),
                     aApItem.iIsPasswordAuthenticationSecure );

    table->WriteUintL( TPtrC(GPRS_AP_TYPE), aApItem.iIspType );


    // Agreed to use const...
    table->WriteUintL( TPtrC(GPRS_QOS_WARNING_TIMEOUT), 
                       TUint32(KGprsOsTimeout) );

    if ( iExt->iIsIpv6Supported )
        {
        WriteServiceIp6L( *table, aApItem );
        }

//    WriteDaemonL( *table, aApItem ); starts here
    if ( aApItem.iIspIPAddrFromServer )
        {
        table->WriteTextL( TPtrC(SERVICE_CONFIG_DAEMON_MANAGER_NAME), 
                           KDaemonManagerName );
        table->WriteTextL( TPtrC(SERVICE_CONFIG_DAEMON_NAME), 
                           KConfigDaemonName );
        }
    else
        {
        // 3.1 it is changed!!
        if ( FeatureManager::FeatureSupported( KFeatureIdIPv6 ) )
            {
            table->WriteTextL( TPtrC(SERVICE_CONFIG_DAEMON_MANAGER_NAME), 
                               KDaemonManagerName );
            table->WriteTextL( TPtrC(SERVICE_CONFIG_DAEMON_NAME), 
                               KConfigDaemonName );            
            }
        else
            {
            table->WriteTextL( TPtrC(SERVICE_CONFIG_DAEMON_MANAGER_NAME), KEmpty );
            table->WriteTextL( TPtrC(SERVICE_CONFIG_DAEMON_NAME), KEmpty );            
            }
        }
//    WriteDaemonL() Ends here
    
    // now put changes
    User::LeaveIfError( table->PutRecordChanges( EFalse, EFalse ) );

    CleanupStack::PopAndDestroy( table );  // table

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::WriteGprsDataL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::WriteLanDataL
// ---------------------------------------------------------
//
void CApDataHandler::WriteLanDataL( TBool aIsNew,
                                    CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::WriteLanDataL" ) ) );

    CCommsDbTableView* table;

    if ( aIsNew )
        {
        table = iDb->OpenTableLC( *aApItem.iIapServiceType );
        User::LeaveIfError( table->InsertRecord( aApItem.iIapServiceId ) );
        }
    else
        {
        table = iDb->OpenViewMatchingUintLC( *aApItem.iIapServiceType,
                                           TPtrC(COMMDB_ID),
                                           aApItem.iIapServiceId );
        User::LeaveIfError( table->GotoFirstRecord() );
        User::LeaveIfError( table->UpdateRecord() );
        }

    // now write record data
    WriteServiceL( *table, aApItem );

    TApBearerType bearer = aApItem.BearerTypeL();

   
    if ( bearer == EApBearerTypeWLAN )
        {
        if ( aApItem.iExt->iWlanData )
            {
        
            CCommsDbTableView* wlantable = NULL;

            if ( aIsNew )
                {        
                TUint32 dummy;
                wlantable = iDb->OpenTableLC( TPtrC(WLAN_SERVICE) );
                User::LeaveIfError( wlantable->InsertRecord( dummy ) );
                aApItem.iExt->iWlanData->iWlanId = dummy;        
                }
            else
                {
                // WLAN settings table backlinks to LAN service, 
                // so we query WLAN settings for a record 
                // where WLAN_SERVICE_ID is LAN_SERVICE's COMMDB_ID, 
                // which is aApItem.iIapServiceId...
                wlantable = iDb->OpenViewMatchingUintLC( TPtrC(WLAN_SERVICE),
                                                         TPtrC(WLAN_SERVICE_ID),
                                                         aApItem.iIapServiceId );
                User::LeaveIfError( wlantable->GotoFirstRecord() );
                User::LeaveIfError( wlantable->UpdateRecord() );
                }

            // Add WLAN specific data storage
            WriteWlanL( *wlantable, aApItem );
            User::LeaveIfError( wlantable->PutRecordChanges( EFalse, EFalse ) );
            CleanupStack::PopAndDestroy( wlantable );  // wlantable
            }
        else
            {                        
            // WLAN not supported, do not try to write it....
            User::Leave( KErrInvalidBearer );
            }

        }
    
    if ( iExt->iIsIpv6Supported )
        {
        WriteServiceIp6L( *table, aApItem );
        }

    // now put changes
    User::LeaveIfError( table->PutRecordChanges( EFalse, EFalse ) );
    CleanupStack::PopAndDestroy( table );  // table

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::WriteLanDataL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::DoUpdateAccessPointDataL
// ---------------------------------------------------------
//
TUint32 CApDataHandler::DoUpdateAccessPointDataL
                                        (
                                        CApAccessPointItem& aApItem,
                                        TBool aIsNew,
                                        TBool& aNameChanged
                                        )
    {
    CLOG( ( EHandler, 0, 
        _L( "-> CApDataHandler::DoUpdateAccessPointDataL" ) ) );

    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    if ( aApItem.iWapAccessPointName->Length() == 0 )
        {
        User::Leave( KErrInvalidName );
        }

    if ( aIsNew )
        {
        HBufC* sgd = ApCommons::DoMakeValidNameL
                                        (
                                        *iDb,
                                        aNameChanged,
                                        aApItem.iWapAccessPointName,
                                        aApItem.iWapUid,
                                        aIsNew
                                        );
        if ( aNameChanged )
            {
            CleanupStack::PushL( sgd );
            aApItem.SetNamesL( *sgd );
            CleanupStack::Pop( sgd );
            }
        delete sgd;
        }
    else
        {
        // first check the existence of the access point,
        // just to be on the sure side...
        CCommsDbTableView* wapt;
        wapt = iDb->OpenViewMatchingUintLC( TPtrC(WAP_ACCESS_POINT),
                                           TPtrC(COMMDB_ID), aApItem.iWapUid );
        // if no other problem, will return KErrNone if it exists
        // so we will go through to the next step.
        // if returns KErrNotFound or any other problem, leave here.
        User::LeaveIfError( wapt->GotoFirstRecord() );
        // now we need to destroy to create a new 'view'
        CleanupStack::PopAndDestroy( wapt ); // wapt

        // now we can go on with the real stuff here...
        // check if a record with the currently given name exists.
        // No problem, if the UID is the same as ours,
        // otherwise, leave because name already exists...
        wapt = iDb->OpenViewMatchingTextLC( TPtrC(WAP_ACCESS_POINT),
                                            TPtrC(COMMDB_NAME),
                                            *aApItem.iWapAccessPointName );
        TInt res =  wapt->GotoFirstRecord();
        if ( res == KErrNotFound )
            { // no such name
            aNameChanged = EFalse;
            }
        else
            {
            if ( res == KErrNone )
                {
                TUint32 id;
                wapt->ReadUintL( TPtrC(COMMDB_ID), id );
                if ( id != aApItem.iWapUid )
                    {
                    User::Leave( KErrAlreadyExists );
                    }
                }
            else
                {
                User::LeaveIfError( res );
                }
            }
        CleanupStack::PopAndDestroy( wapt );   // wapt
        }

    TBool NeedManualUpdate( ETrue );

    // additional code for managing bearer-change starts here...
    if ( !aIsNew )
        { // only check if update op.
        // if bearer changed, it also UPDATES everything !
        // if no change, update MUST BE DONE HERE MANUALLY!
        if ( HandleBearerChangeIfNeededL( aApItem ) )
            {
            NeedManualUpdate = EFalse;
            }
        }
    if ( NeedManualUpdate )
        {
        WriteIspDataL( aIsNew, aApItem.iIapServiceId, aApItem );
        WriteNetworkDataL( aIsNew, aApItem );
        WriteIapDataL( aIsNew, aApItem.iWapIap, aApItem );
        WriteWapApDataL( aIsNew, aApItem.iWapUid, aApItem );
        WriteWapBearerDataL( aIsNew, aApItem.iWapUid, aApItem );
        if ( aApItem.HasProxy() )
            {
            WriteProxyDataL( aIsNew, aApItem );
            }
        else
            {
            if ( !aIsNew )
                {
                TRAP_IGNORE( RemoveProxyDataL( aApItem.iIapServiceType, 
                                               aApItem.iIapServiceId ) );
                }
            }
        }

    CLOG( ( EHandler, 1, 
        _L( "<- CApDataHandler::DoUpdateAccessPointDataL" ) ) );
    return aApItem.iWapUid;
    }



// ---------------------------------------------------------
// CApDataHandler::SetDefaultIapIspL
// ---------------------------------------------------------
//
void CApDataHandler::SetDefaultIapIspL( TUint32 aUid )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::SetDefaultIapIspL" ) ) );
    
    CApAccessPointItem* ap = CApAccessPointItem::NewLC();
    AccessPointDataL( aUid, *ap );
    // Get IAP uid from aUid record
    // ap.iWapIap
    // Dial In or Dial Out?
    // ap.iWapIspType
    // store it accordingly
    switch ( ap->BearerTypeL() )
        {
        case EApBearerTypeCSD:
        case EApBearerTypeHSCSD:
            {
//            iDb->SetGlobalSettingL( TPtrC(DIAL_OUT_IAP), ap->iWapIap );
            SetPreferredIfDbIapTypeL
                ( 1,                                    // aRank,
                  ECommDbConnectionDirectionOutgoing,   //aDirection,
                  KCommDbBearerCSD,                     // aBearers,
                  ECommDbDialogPrefDoNotPrompt,         //aPrompt,
                  ap->iWapIap,                          //aIAP,
                  ETrue                                 //aOverwrite
                );
            break;
            }
        case EApBearerTypeGPRS:
        case EApBearerTypeCDMA:
            {
            // Not applicable !
            User::Leave( KErrArgument );
            break;
            }
        default:
            {
            User::Leave( KErrInvalidIspRequest );
            }
        }
    CleanupStack::PopAndDestroy( ap );    // ap

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::SetDefaultIapIspL" ) ) );
    }





// ---------------------------------------------------------
// CApDataHandler::RemoveWapBearerApL
// ---------------------------------------------------------
//
void CApDataHandler::RemoveWapBearerApL( TUint32 aUid )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::RemoveWapBearerApL" ) ) );

    CCommsDbTableView* bearert;
    bearert = iDb->OpenViewMatchingUintLC( TPtrC(WAP_IP_BEARER),
                                   TPtrC(WAP_ACCESS_POINT_ID), aUid );

    TInt res = bearert->GotoFirstRecord();  // OK.
    if ( res == KErrNone )
        { // exists, delete it
        User::LeaveIfError( bearert->DeleteRecord() );
        }
    else
        {
        // silently ignore KErrNotFound. It is caused by incorrect DB,
        // we are 'repairing it' this way.
        if ( res != KErrNotFound )
            {
            User::LeaveIfError( res );
            }
        }
    CleanupStack::PopAndDestroy( bearert ); // bearert

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::RemoveWapBearerApL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::RemoveWapApL
// ---------------------------------------------------------
//
void CApDataHandler::RemoveWapApL( TUint32 aUid )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::RemoveWapApL" ) ) );

    CCommsDbTableView* wapt = iDb->OpenViewMatchingUintLC(
                                        TPtrC(WAP_ACCESS_POINT),
                                        TPtrC(COMMDB_ID), aUid );
    TInt res = wapt->GotoFirstRecord(); // OK.
    if ( res == KErrNone )
        { // exists, delete it
        User::LeaveIfError( wapt->DeleteRecord() );
        }
    else
        {
        // silently ignore KErrNotFound. It is caused by incorrect DB,
        // we are 'repairing it' this way.
        if ( res != KErrNotFound )
            {
            User::Leave( res );
            }
        }
    CleanupStack::PopAndDestroy( wapt ); // wapt

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::RemoveWapApL" ) ) );
    }


// ---------------------------------------------------------
// CApDataHandler::RemoveIapL
// ---------------------------------------------------------
//
void CApDataHandler::RemoveIapL( TUint32 aUid )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::RemoveIapL" ) ) );

    CCommsDbTableView* iapt;
    iapt = iDb->OpenViewMatchingUintLC( TPtrC(IAP),
                                   TPtrC(COMMDB_ID), aUid );
    TInt res = iapt->GotoFirstRecord(); // OK.
    if ( res == KErrNone )
        { // exists, delete it
        User::LeaveIfError( iapt->DeleteRecord() );
        }
    else
        {
        // silently ignore KErrNotFound. It is caused by incorrect DB,
        // we are 'repairing it' this way.
        if ( res != KErrNotFound )
            {
            User::Leave( res );
            }
        }
    CleanupStack::PopAndDestroy( iapt ); // iapt

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::RemoveIapL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::RemoveIspL
// ---------------------------------------------------------
//
void CApDataHandler::RemoveIspL( TUint32 aUid, TBool aOut )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::RemoveIspL" ) ) );

    CCommsDbTableView* ispt;
    if ( aOut )
        {
        ispt = iDb->OpenViewMatchingUintLC( TPtrC(DIAL_OUT_ISP),
                                       TPtrC(COMMDB_ID), aUid );
        }
    else
        {
        ispt = iDb->OpenViewMatchingUintLC( TPtrC(DIAL_IN_ISP),
                                       TPtrC(COMMDB_ID), aUid );
        }

    TInt res = ispt->GotoFirstRecord(); // OK.
    if ( res == KErrNone )
        { // exists, delete it
        User::LeaveIfError( ispt->DeleteRecord() );
        }
    else
        {
        // silently ignore KErrNotFound. It is caused by incorrect DB,
        // we are 'repairing it' this way.
        if ( res != KErrNotFound )
            {
            User::Leave( res );
            }
        }
    CleanupStack::PopAndDestroy( ispt ); // ispt

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::RemoveIspL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::RemoveGprsL
// ---------------------------------------------------------
//
void CApDataHandler::RemoveGprsL( TUint32 aUid, TBool aOut )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::RemoveGprsL" ) ) );

    CCommsDbTableView* table;
    if ( aOut )
        {
        table = iDb->OpenViewMatchingUintLC( TPtrC(OUTGOING_WCDMA),
                                       TPtrC(COMMDB_ID), aUid );
        }
    else
        {
        table = iDb->OpenViewMatchingUintLC( TPtrC(INCOMING_WCDMA),
                                       TPtrC(COMMDB_ID), aUid );
        }

    TInt res = table->GotoFirstRecord(); // O.K.
    if ( res == KErrNone )
        { // exists, delete it
        User::LeaveIfError( table->DeleteRecord() );
        }
    else
        {
        // silently ignore KErrNotFound. It is caused by incorrect DB,
        // we are 'repairing it' this way.
        if ( res != KErrNotFound )
            {
            User::Leave( res );
            }
        }

    CleanupStack::PopAndDestroy( table ); // table

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::RemoveGprsL" ) ) );
    }


// ---------------------------------------------------------
// CApDataHandler::RemoveLanL
// ---------------------------------------------------------
//
void CApDataHandler::RemoveLanL( TUint32 aUid )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::RemoveLanL" ) ) );

    CCommsDbTableView* table = iDb->OpenViewMatchingUintLC
        ( TPtrC(LAN_SERVICE), TPtrC(COMMDB_ID), aUid );

    TInt res = table->GotoFirstRecord(); // O.K.
    if ( res == KErrNone )
        { // exists, delete it
        User::LeaveIfError( table->DeleteRecord() );
        }
    else
        {
        // silently ignore KErrNotFound. It is caused by incorrect DB,
        // we are 'repairing it' this way.
        if ( res != KErrNotFound )
            {
            User::Leave( res );
            }
        }
    CleanupStack::PopAndDestroy( table ); // table
    
    // try to remove WLAN part, if it has any
    // TRAP and ignore if not found
    TRAP( res, RemoveWlanL( aUid ) );
    if ( res != KErrNotFound )
        {
        User::LeaveIfError( res );
        }

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::RemoveLanL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::RemoveWlanL
// ---------------------------------------------------------
//
void CApDataHandler::RemoveWlanL( TUint32 aUid )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::RemoveWlanL" ) ) );

    CCommsDbTableView* table = NULL;

    // now check the WLAN table for corresponding record and delete them, too
    table = iDb->OpenViewMatchingUintLC
            ( TPtrC(WLAN_SERVICE), TPtrC(WLAN_SERVICE_ID), aUid );

    TInt res = table->GotoFirstRecord(); // O.K.
    if ( res == KErrNone )
        { // exists, delete it
        TUint32 tempint( 0 );
        ApCommons::ReadUintL( table, TPtrC(WLAN_SECURITY_MODE), tempint );
        // now tempint holds the security mode
        // we have to delete security settings, too
        switch ( tempint )
            {
            case EOpen:
            case EWep:
                {
                break;
                }
            case E802_1x:
                {
                CWPASecuritySettings* wpa = 
                        CWPASecuritySettings::NewL( ESecurityMode8021x );
                CleanupStack::PushL( wpa );
                wpa->DeleteL( aUid );
                CleanupStack::PopAndDestroy( wpa );
                break;
                }
            case EWpa:
            case EWpa2:
                {
                CWPASecuritySettings* wpa = 
                        CWPASecuritySettings::NewL( ESecurityModeWpa );
                CleanupStack::PushL( wpa );
                wpa->DeleteL( aUid );
                CleanupStack::PopAndDestroy( wpa );
                break;
                }
            default:
                {
                // some weird error, repair it...
                __ASSERT_DEBUG( EFalse, ApCommons::Panic( ENotSupported ) );
                break;
                }
            }
        
        User::LeaveIfError( table->DeleteRecord() );
        }
    else
        {
        // silently ignore KErrNotFound. It is caused by incorrect DB,
        // we are 'repairing it' this way.
        if ( res != KErrNotFound )
            {
            User::Leave( res );
            }
        }

    // only need to destroy if it was successfully created!
    CleanupStack::PopAndDestroy( table ); // table            
    
    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::RemoveWlanL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::HandleBearerChangeIfNeededL
// ---------------------------------------------------------
//
TBool CApDataHandler::HandleBearerChangeIfNeededL
                                    ( CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, 
        _L( "-> CApDataHandler::HandleBearerChangeIfNeededL" ) ) );

    TApBearerType anew = aApItem.BearerTypeL();

    CApUtils* au = CApUtils::NewLC( *iDb );
    TApBearerType aold = au->BearerTypeL( aApItem.iWapUid );
    // if different WAP_Bearer table, OR different IAP_SERVICE table,
    // delete old ones and save current as NEW records
    // under the SAME iWapUid !

    TBool retval( EFalse );
    if ( ( aold != anew ) &&
        !(
           (( aold == EApBearerTypeHSCSD ) && ( anew == EApBearerTypeCSD ) )
           ||
           ( ( aold == EApBearerTypeCSD ) && ( anew == EApBearerTypeHSCSD ) )
         )
        )
        {// different bearers!
        HBufC* buf = HBufC::NewLC( KModifiableTextLength );
        TPtr ptr( buf->Des() );
        au->WapBearerTypeL( aApItem.iWapUid, ptr );

        CApAccessPointItem* apitem = CApAccessPointItem::NewLC();
        AccessPointDataL( aApItem.iWapUid, *apitem );

        if ( *aApItem.iWapBearer != ptr )
            {
            // if different WAP_Bearer table, delete old wap_bearer
            // & if needed, IAP, ISP, GPRS, CDMA, too
            //  and Crete new wap bearer & if needed, IAP, ISP, GPRS, CDMA, too
            if ( *apitem->iWapBearer == TPtrC(WAP_IP_BEARER) )
                {
                // e.g.:RemoveIpBearer(iIapServiceType, iIapServiceId,
                //                     iWapIap )
                if ( ( *apitem->iIapServiceType == TPtrC(DIAL_OUT_ISP) )
                    || ( *apitem->iIapServiceType == TPtrC(DIAL_IN_ISP) ) )
                    { // ISP
                    RemoveIspL( apitem->iIapServiceId,
                        ( *apitem->iIapServiceType == TPtrC(DIAL_OUT_ISP) ) );
                    }
                else if( ( *apitem->iIapServiceType ==
                                        TPtrC(OUTGOING_WCDMA) )
                        || ( *apitem->iIapServiceType ==
                                        TPtrC(INCOMING_WCDMA) ) )
                    {// Gprs
                    RemoveGprsL( apitem->iIapServiceId,
                        ( *apitem->iIapServiceType == 
                                TPtrC(OUTGOING_WCDMA) ) );
                    }
                else
                    {
                    // check if WLAN
                    if ( aApItem.iExt->iWlanData )
                        {
                        RemoveLanL( apitem->iIapServiceId );
                        }
                    else
                        {                            
                        // no leave on else Unknown, as we are deleting 
                        // so silently ignore the error...
                        }
                    }
                RemoveIapL( apitem->iWapIap );
                RemoveWapBearerApL( aApItem.iWapUid );
                }
            else
                { // SMS
                User::Leave( KErrInvalidBearerType );
                }
            RemoveProxyDataL( apitem->iIapServiceType, apitem->iIapServiceId );
            WriteIspDataL( ETrue, aApItem.iIapServiceId, aApItem );
            WriteIapDataL( ETrue, aApItem.iWapIap, aApItem );

            // gets the UID needed for Bearer table
            WriteWapApDataL( EFalse, aApItem.iWapUid, aApItem );
            // needs new bearer record
            WriteWapBearerDataL( ETrue, aApItem.iWapUid, aApItem );
            if ( aApItem.HasProxy() )
                {
                WriteProxyDataL( ETrue, aApItem );
                }
            }
        else
            { // bearer table is same, but IAP_SERVICE is different
            if ( ( aold == EApBearerTypeHSCSD ) ||
                 ( aold == EApBearerTypeCSD ) )
                {
                RemoveIspL( apitem->iIapServiceId,
                    ( *apitem->iIapServiceType == TPtrC(DIAL_OUT_ISP) ) );
                }
            else if ( ( *apitem->iIapServiceType ==
                                        TPtrC(OUTGOING_WCDMA) )
                     || ( *apitem->iIapServiceType ==
                                        TPtrC(INCOMING_WCDMA) ) )
                {
                RemoveGprsL( apitem->iIapServiceId,
                    ( *apitem->iIapServiceType == TPtrC(OUTGOING_WCDMA) ) );
                }
            else 
                {
                // check if WLAN
                if ( aApItem.iExt->iWlanData )
                    {
                    RemoveLanL( apitem->iIapServiceId );
                    }
                else
                    {
                    // no leave on else Unknown, as we are deleting 
                    // so silently ignore the error...
                    }
                }
            RemoveProxyDataL( apitem->iIapServiceType, apitem->iIapServiceId );
            WriteIspDataL( ETrue, aApItem.iIapServiceId, aApItem );
            WriteIapDataL( EFalse, aApItem.iWapIap, aApItem );
            // gets the UID needed for Bearer table
            WriteWapApDataL( EFalse, aApItem.iWapUid, aApItem );
            WriteWapBearerDataL( EFalse, aApItem.iWapUid, aApItem );
            if ( aApItem.HasProxy() )
                {
                WriteProxyDataL( ETrue, aApItem );
                }
            }
        CleanupStack::PopAndDestroy( 2, buf ); // apitem, buf
        retval = ETrue;
        }
    CleanupStack::PopAndDestroy( au ); // au
    // additional code for managing bearer-change ends here...

    CLOG( ( EHandler, 1, 
        _L( "<- CApDataHandler::HandleBearerChangeIfNeededL" ) ) );
    return retval;
    }





// ---------------------------------------------------------
// CApDataHandler::DoAccessPointDataL
// ---------------------------------------------------------
//
void CApDataHandler::DoAccessPointDataL( TUint32 aUid,
                                         CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::DoAccessPointDataL" ) ) );

    ReadWapDataL( aUid, aApItem );

    if ( *aApItem.iWapBearer == TPtrC(WAP_IP_BEARER) )
        {
        ReadIapDataL( aApItem.iWapIap, aApItem );
        ReadNetworkDataL( aApItem );
        ReadIspDataL( aApItem.iIapServiceId, aApItem );
        }
    ReadProxyDataL( aApItem );

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::DoAccessPointDataL" ) ) );
    }


// ---------------------------------------------------------
// CApDataHandler::DoRemoveAPL
// ---------------------------------------------------------
//
void CApDataHandler::DoRemoveAPL( TUint32 aUid )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::DoRemoveAPL" ) ) );

    CApUtils* utils = CApUtils::NewLC( *iDb );
    if ( utils->IsAPInUseL( aUid ) )
        {
        User::Leave( KErrInUse );
        }
    CleanupStack::PopAndDestroy( utils );   // utils

    CCommsDbTableView* wapt;
    wapt = iDb->OpenViewMatchingUintLC( TPtrC(WAP_ACCESS_POINT),
                                       TPtrC(COMMDB_ID), aUid );
    User::LeaveIfError( wapt->GotoFirstRecord() );
    HBufC* buf = HBufC::NewLC( KCommsDbSvrMaxFieldLength );
    TPtr ptr( buf->Des() );
    wapt->ReadTextL( TPtrC(WAP_CURRENT_BEARER),
                     ptr ); // see declaration for comments!

    if ( *buf == TPtrC(WAP_IP_BEARER) )
        {
        // get serviceType
        CCommsDbTableView* bearert;
        bearert = iDb->OpenViewMatchingUintLC( TPtrC(WAP_IP_BEARER),
                                               TPtrC(WAP_ACCESS_POINT_ID),
                                               aUid );

        User::LeaveIfError( bearert->GotoFirstRecord() );
        TUint32 iapid;
        ApCommons::ReadUintL( bearert, TPtrC(WAP_IAP), iapid );

        CCommsDbTableView* iapt;
        iapt = iDb->OpenViewMatchingUintLC( TPtrC(IAP), TPtrC(COMMDB_ID),
                                            iapid );

        User::LeaveIfError( iapt->GotoFirstRecord() );
        HBufC* servicetype = HBufC::NewLC( KCommsDbSvrMaxFieldLength );
        TPtr servptr( servicetype->Des() );
        iapt->ReadTextL( TPtrC(IAP_SERVICE_TYPE), servptr );
        TUint32 ispid;
        iapt->ReadUintL( TPtrC(IAP_SERVICE), ispid );
        TUint32 networkid;
        iapt->ReadUintL( TPtrC(IAP_NETWORK), networkid );
        RemoveNetworkDataL( networkid );
        if ( ( *servicetype == TPtrC(DIAL_OUT_ISP) ) ||
             ( *servicetype == TPtrC(DIAL_IN_ISP) ) )
            { // ISP
            RemoveIspL( ispid, ( *servicetype == TPtrC(DIAL_OUT_ISP)) );
            RemoveIapL( iapid );
            }
        else
            { // Gprs
            if ( ( *servicetype == TPtrC(OUTGOING_WCDMA) ) ||
                 ( *servicetype == TPtrC(INCOMING_WCDMA) ) )
                {
                RemoveGprsL( ispid, ( *servicetype == TPtrC(OUTGOING_WCDMA)) );
                RemoveIapL( iapid );
                }
            else
                { 
                if ( *servicetype ==TPtrC(LAN_SERVICE) )
                    {
                    // LAN, WLAN
                    RemoveLanL( ispid );
                    RemoveIapL( iapid );
                    }
                else
                    {
                    // VPN...
                    }
                }
            }
        RemoveProxyDataL( servicetype, ispid );
        RemoveWapBearerApL( aUid );
        CleanupStack::PopAndDestroy( 3, bearert ); // servicetype,iapt,bearert
        }
    else
        { // sms,
        User::Leave( KErrInvalidBearerType );
        }

    RemoveWapApL( aUid );

    CleanupStack::PopAndDestroy( 2, wapt );   // buf, wapt

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::DoRemoveAPL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::DoSetAsDefaultL
// ---------------------------------------------------------
//
void CApDataHandler::DoSetAsDefaultL( TUint32 aUid,
                                      TCommsDbIspType aIntendedType )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::DoSetAsDefaultL" ) ) );

    // Due to changes in the database, ONLY WAP_ACCESS_POINT_ID
    switch ( aIntendedType )
        {
        case EIspTypeInternetOnly:
            {
            SetDefaultIapIspL( aUid );
            break;
            }
        case EIspTypeWAPOnly:
            {
            iDb->SetGlobalSettingL( TPtrC(WAP_ACCESS_POINT), aUid );
            break;
            }
        case EIspTypeInternetAndWAP:
            {
            iDb->SetGlobalSettingL( TPtrC(WAP_ACCESS_POINT), aUid );
            SetDefaultIapIspL( aUid );
            break;
            }
        default :
            {
            __ASSERT_DEBUG( EFalse, User::Panic( KErrInvalidIntendedType,
                                                 aIntendedType ) );
            break;
            }
        }

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::DoSetAsDefaultL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::DoSetPreferredIfDbIapTypeL
// ---------------------------------------------------------
//
void CApDataHandler::DoSetPreferredIfDbIapTypeL(
                                 TUint32 aRank,
                                 TCommDbConnectionDirection aDirection,
                                 TUint32 aBearers,
                                 TCommDbDialogPref aPrompt,
                                 TUint32 aIAP,
                                 TBool aOverwrite
                                    )
    {
    CLOG( ( EHandler, 0, 
        _L( "-> CApDataHandler::DoSetPreferredIfDbIapTypeL" ) ) );

    CCommsDbConnectionPrefTableView* preft =
             iDb->OpenConnectionPrefTableViewOnRankLC( aDirection, aRank );
    if ( aOverwrite )
        {
        TInt res = preft->GotoFirstRecord(); // OK.
        if ( res == KErrNone )
            { // already exists, delete original and store the new one...
            preft->DeleteConnectionPreferenceL();
            }
        else
            {
            // silently ignore KErrNotFound.
            // It is not an error in this case.
            if ( res != KErrNotFound )
                {
                User::Leave( res );
                }
            }
        }
    CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref;
    pref.iRanking = aRank;
    pref.iBearer.iBearerSet = aBearers;
    pref.iDialogPref = aPrompt;
    pref.iBearer.iIapId = aIAP;
    pref.iDirection = aDirection;
    preft->InsertConnectionPreferenceL( pref );
    CleanupStack::PopAndDestroy( preft ); // preft

    CLOG( ( EHandler, 1, 
        _L( "<- CApDataHandler::DoSetPreferredIfDbIapTypeL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::DoSetPreferredIfDbIspTypeL
// ---------------------------------------------------------
//
void CApDataHandler::DoSetPreferredIfDbIspTypeL(
                                 TUint32 /*aRank*/,
                                 TCommDbConnectionDirection /*aDirection*/,
                                 TUint32 /*aBearers*/,
                                 TCommDbDialogPref /*aPrompt*/,
                                 TUint32 /*aISP*/,
                                 TUint32 /*aChargeCard*/,
                                 const TDesC& /*aServiceType*/,
                                 TBool /*aOverwrite*/
                                    )
    {
    /*
    DEPRECATED!!!
    */
    CLOG( ( EHandler, 0, 
        _L( "<-> CApDataHandler::DoSetPreferredIfDbIspTypeL" ) ) );

    User::Leave( KErrNotSupported );
    }



// ---------------------------------------------------------
// CApDataHandler::ApSpeed2EtelL
// ---------------------------------------------------------
//
void CApDataHandler::ApSpeed2EtelL( CApAccessPointItem* aItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::ApSpeed2EtelL" ) ) );
    
    __ASSERT_DEBUG( (aItem), ApCommons::Panic( ENullPointer ) ) ;
    
    /*****************************************************
    *   Series 60 Customer / ETel
    *   Series 60  ETel API
    *****************************************************/
    
    TInt bear = CSD;
    switch ( aItem->BearerTypeL() )
        {
        case EApBearerTypeCSD:
            {
            // search for (CSD, x, x)
            bear = CSD;
            break;
            }
        case EApBearerTypeHSCSD:
            {
            // HSCSD
            // search for (HCSD, x, x)
            bear = HSCSD;
            break;
            }
        default:
            {
            // it is called only for CSD/HSCSD bearers, but to be sure...
            __ASSERT_DEBUG( EFalse, ApCommons::Panic( ENotSupported ) ) ;
            User::Leave( KErrInvalidBearerType );
            break;
            }
        }
    
    TInt type = Analogue;
    switch ( aItem->iBearerCallTypeIsdn )
        {
        case ECallTypeAnalogue:
            {
            type = Analogue;
            break;
            }
        case ECallTypeISDNv110:
            {
            type = V110;
            break;
            }
        case ECallTypeISDNv120:
            {
            type = V120;
            break;
            }
        default:
            {
            // it has only 3 states, but to be sure...
            __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidSpeed ) ) ;
            User::Leave( KErrInvalidBearerType );
            break;            
            }
        }
        
    TBool found(EFalse);
    // just get count once...
    TInt count = sizeof(SpeedData)/sizeof(SpeedData[0]);    
    
    for( TInt i=0; i<count; i++)
        {
        if ( ( SpeedData[i][0] == bear ) 
            && ( SpeedData[i][1] == type ) 
            && ( SpeedData[i][2] == aItem->iMaxConnSpeed ) )
            {
            // wwe found it, get values, break
            // Bearer_name, Bearer speed, Bearer_ce, Bearer_type
            aItem->WriteUint( EApIspBearerName, SpeedData[i][3] );
            aItem->iExt->iBearerSpeed = SpeedData[i][4];
            aItem->WriteUint( EApIspBearerCE, SpeedData[i][5] );
            aItem->WriteUint( EApIspBearerType, SpeedData[i][6] );
            
            
            // Channel coding, Aiur, Req.Slot, MaxSlot, 
            aItem->WriteUint( EApIspChannelCoding, SpeedData[i][7] );
            aItem->WriteUint( EApIspAIUR, SpeedData[i][8] );
            aItem->WriteUint( EApIspRequestedTimeSlots, SpeedData[i][9] );
            aItem->WriteUint( EApIspMaximumTimeSlots, SpeedData[i][10] );
            
            // Bearer_service, Bearer_Protocol
            aItem->WriteUint( EApIapBearerService, SpeedData[i][11] );
            aItem->iExt->iBearerProtocol = SpeedData[i][12];

            i = count;
            found = ETrue;
            }
        }
    if ( !found )
        {
        // if not found, someone set an invalid speed in the engine
        // which already should have been checked, 
        // just to be on the sure side
        __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidSpeed ) ) ;
        User::Leave(KErrNotFound);
        }

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::ApSpeed2EtelL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::EtelSpeed2ApL
// ---------------------------------------------------------
//
void CApDataHandler::EtelSpeed2ApL( CApAccessPointItem& aItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::EtelSpeed2ApL" ) ) );

    // ************* ALERT ***************
    // We utilise the fact that this is called right after the data 
    // had been read from the DB and the 
    // aItem.iMaxConnSpeed and aItem.iBearerCallTypeIsdn members 
    // are holding the CommsDb style values.
    // now we are converting it to our style as the API is fixed
    // and so are the used enums...

    /*****************************************************
    *   Series 60 Customer / ETel
    *   Series 60  ETel API
    *****************************************************/

    TBool found(EFalse);
    // just get count once...
    TInt count = sizeof(SpeedData)/sizeof(SpeedData[0]);    
    
    for( TInt i=0; i<count; i++)
        {
        if ( ( SpeedData[i][3] == aItem.iIspBearerName )
            && ( SpeedData[i][4] == aItem.iExt->iBearerSpeed )
            && ( SpeedData[i][5] == aItem.iIspBearerCE )
            && ( SpeedData[i][6] == aItem.iIspBearerType )
            && ( SpeedData[i][7] == aItem.iIspChannelCoding )
            && ( SpeedData[i][8] == aItem.iIspAIUR )
            && ( SpeedData[i][9] == aItem.iIspRequestedTimeSlots )
            && ( SpeedData[i][10] == aItem.iIspMaximumTimeSlots )
            && ( SpeedData[i][11] == aItem.iApIapBearerService )
            && ( SpeedData[i][12] == aItem.iExt->iBearerProtocol ) )
            {
            // we found it, get values, break
            switch ( SpeedData[i][0] )
                { 
                case CSD:
                    {
                    aItem.iIspBearerType = EBearerTypeCSD;
                    break;
                    }
                case HSCSD:
                    {
                    aItem.iIspBearerType = EBearerTypeHSCSD;
                    break;            
                    }
                default:
                    { // incorrect db, repair...
                    aItem.iIspBearerType = EBearerTypeCSD;
                    break;
                    }
                }

            
            switch ( SpeedData[i][1])
                {
                case Analogue:
                    {
                    aItem.iBearerCallTypeIsdn = ECallTypeAnalogue;
                    break;
                    }
                case V110:
                    {
                    aItem.iBearerCallTypeIsdn = ECallTypeISDNv110;
                    break;
                    }
                case V120: 
                    {
                    aItem.iBearerCallTypeIsdn = ECallTypeISDNv120;
                    break;
                    }                    
                default:
                    {
                    // incorrect db, repair...
                    aItem.iBearerCallTypeIsdn = ECallTypeAnalogue;
                    break;
                    }
                }
            
            aItem.iMaxConnSpeed = TApCallSpeed(SpeedData[i][2]);
            i = count;
            found = ETrue;
            }
        }
    if ( !found )
        {
        // if not found, someone set an invalid speed in the engine
        // which already should have been checked, 
        // just to be on the sure side
        __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidSpeed ) ) ;
        User::Leave(KErrNotFound);
        }
  
    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::EtelSpeed2ApL" ) ) );

    }



// Getting default access point
// ---------------------------------------------------------
// CApDataHandler::DoGetDefaultL
// ---------------------------------------------------------
//
TUint32 CApDataHandler::DoGetDefaultL( TBool aIsWap,
                                             TBool* aReadOnly ) const
    { // client MUST initiate a transaction, we will NOT !
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::DoGetDefaultL" ) ) );

    TUint32 value( KErrNone );
    if ( aIsWap )
        {
        iDb->GetGlobalSettingL( TPtrC(WAP_ACCESS_POINT), value );
        }
    else
        { 
        TUint32 tmp( 0 );
        // CONST_CAST added later, originally used other code, 
        // when moved to 2.0, support was removed and needed 
        // to use own (non-const)
        // method which can not be made const as it is part of the public API
        CONST_CAST( CApDataHandler*, this )->GetPreferredIfDbIapTypeL
            ( 1, // Rank
              ECommDbConnectionDirectionOutgoing,
              tmp,    // aBearers,
              tmp,    // aPrompt,
              value   // aIAP
            );
        }

    if ( aReadOnly )
        { // get read-only flag for an AP
        CCommsDbTableView* wapt =
                iDb->OpenViewMatchingUintLC( TPtrC(WAP_ACCESS_POINT),
                                             TPtrC(COMMDB_ID), value);
        User::LeaveIfError( wapt->GotoFirstRecord() );
        *aReadOnly = ApCommons::DoGetReadOnlyL( *wapt );
        CleanupStack::PopAndDestroy( wapt );
        }
    
    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::DoGetDefaultL" ) ) );
    return value;
    }


// ---------------------------------------------------------
// CApDataHandler::WriteProxyDataL
// ---------------------------------------------------------
//
void CApDataHandler::WriteProxyDataL( TBool aIsNew, 
                                      const CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::WriteProxyDataL" ) ) );

    CCommsDbTableView* proxies;
    if ( aIsNew )
        {
        proxies = iDb->OpenTableLC( TPtrC(PROXIES) );
        // variable needed just to be able to make the call...
        TUint32 dummy;
        User::LeaveIfError( proxies->InsertRecord( dummy ) );
        }
    else
        {
        // Add test cases covering this route
        proxies = iDb->OpenViewMatchingTextLC( TPtrC(PROXIES), 
                                               TPtrC(PROXY_SERVICE_TYPE),
                                               *aApItem.iIapServiceType );
        TInt err = proxies->GotoFirstRecord();
        if ( err == KErrNotFound )
            { // originally there were NO proxy, but now it is added
            // variable needed just to be able to make the call...
            // Add test cases covering this route
            TUint32 dummy;
            User::LeaveIfError( proxies->InsertRecord( dummy ) );
            }
        else
            {
            // Add test cases covering this route
            // now check if we have our record
            TUint32 uval;
            TBool found( EFalse );
            do
                {
                // Add test cases covering this route
                proxies->ReadUintL( TPtrC(PROXY_ISP), uval );
                if ( uval == aApItem.iIapServiceId )
                    {
                    found = ETrue;
                    }
                else
                    {
                    err = proxies->GotoNextRecord();
                    }
                } while ( (!err) && (!found) );
            if ( !found )
                { // no proxy record, add it now
                // variable needed just to be able to make the call...
                TUint32 dummy;
                User::LeaveIfError( proxies->InsertRecord( dummy ) );
                }
            else
                { // we found our proxy, use it...
                User::LeaveIfError( proxies->UpdateRecord() );
                }
            }
        }
    proxies->WriteUintL( TPtrC(PROXY_ISP), aApItem.iIapServiceId );
    proxies->WriteTextL( TPtrC(PROXY_SERVICE_TYPE),
                         *aApItem.iIapServiceType );


    proxies->WriteLongTextL( TPtrC(PROXY_SERVER_NAME),
                            *aApItem.iApProxyServerAddress);

    proxies->WriteTextL( TPtrC(PROXY_PROTOCOL_NAME),
                         *aApItem.iApProxyProtocolName );

    proxies->WriteUintL( TPtrC(PROXY_PORT_NUMBER),
                         aApItem.iApProxyPortNumber );

    proxies->WriteBoolL( TPtrC(PROXY_USE_PROXY_SERVER), 
                         aApItem.iApProxyUseProxy );

    proxies->WriteLongTextL( TPtrC(PROXY_EXCEPTIONS),
                            *aApItem.iApProxyExceptions );

    User::LeaveIfError( proxies->PutRecordChanges( EFalse, EFalse ) );

    CleanupStack::PopAndDestroy( proxies );   // proxies

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::WriteProxyDataL" ) ) );
    }


// ---------------------------------------------------------
// CApDataHandler::ReadProxyDataL
// ---------------------------------------------------------
//
void CApDataHandler::ReadProxyDataL( CApAccessPointItem& aApItem )
    {
    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::ReadProxyDataL" ) ) );

    // Add test cases covering ALL routes of this function

    CCommsDbTableView* proxies;    
    proxies = iDb->OpenViewMatchingTextLC( TPtrC(PROXIES), 
                                           TPtrC(PROXY_SERVICE_TYPE),
                                           *aApItem.iIapServiceType );

    TInt err = proxies->GotoFirstRecord();

    if ( err != KErrNotFound )
        {
        User::LeaveIfError( err );
        }

    if ( err != KErrNotFound )
        {
        // we have some proxy, now found our record...
        TUint32 uval;
        TBool found( EFalse );
        do
            {
            proxies->ReadUintL( TPtrC(PROXY_ISP), uval );
            if ( uval == aApItem.iIapServiceId )
                {
                found = ETrue;
                }
            else
                {
                err = proxies->GotoNextRecord();
                }
            } while ( (!err) && (!found) );
        if ( found )
            {
            HBufC* buf = ApCommons::ReadLongTextLC( proxies,
                                                    TPtrC(PROXY_SERVER_NAME),
                                                    err );
            if ( err != KErrUnknown )
                {
                aApItem.WriteLongTextL( EApProxyServerAddress, *buf );
                }

            CleanupStack::PopAndDestroy( buf ); 
            buf = ApCommons::ReadText16ValueLC( proxies, 
                                                TPtrC(PROXY_PROTOCOL_NAME) );
            aApItem.WriteTextL( EApProxyProtocolName, *buf );
            CleanupStack::PopAndDestroy( buf ); 

            TUint32 tempint( 0 );
            TBool tempbool( EFalse );

            ApCommons::ReadUintL( proxies, TPtrC(PROXY_PORT_NUMBER), tempint );
            aApItem.WriteUint( EApProxyPortNumber, tempint );


            ApCommons::ReadBoolL( proxies, 
                                  TPtrC(PROXY_USE_PROXY_SERVER), 
                                  tempbool );
            aApItem.WriteBool( EApProxyUseProxy, tempbool );

            buf = ApCommons::ReadLongTextLC( proxies,
                                             TPtrC(PROXY_EXCEPTIONS),
                                             err );
            aApItem.WriteLongTextL( EApProxyExceptions, *buf );
    
            CleanupStack::PopAndDestroy( buf );
            }
        }
    CleanupStack::PopAndDestroy( proxies ); // proxies, sqlbuf

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::ReadProxyDataL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::RemoveProxyDataL
// ---------------------------------------------------------
//
void CApDataHandler::RemoveProxyDataL( HBufC* aServiceType, TUint32 aIspId )
    {
    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::RemoveProxyDataL" ) ) );

    // Add test cases covering ALL routes of this function

    CCommsDbTableView* proxies;    
    proxies = iDb->OpenViewMatchingTextLC( TPtrC(PROXIES), 
                                           TPtrC(PROXY_SERVICE_TYPE),
                                           *aServiceType );

    TInt err = proxies->GotoFirstRecord();

    if ( err != KErrNotFound )
        {
        User::LeaveIfError( err );
        }

    if ( err != KErrNotFound )
        {
        // we have some proxy, now found our record...
        TUint32 uval;
        TBool found( EFalse );
        do
            {
            proxies->ReadUintL( TPtrC(PROXY_ISP), uval );
            if ( uval == aIspId )
                {
                found = ETrue;
                }
            else
                {
                err = proxies->GotoNextRecord();
                }
            } while ( (!err) && (!found) );
        if ( found )
            {
            if ( err == KErrNone )
                {
                User::LeaveIfError( proxies->DeleteRecord() );
                }
            else
                {
                // silently ignore KErrNotFound. It is caused by incorrect DB,
                // we are 'repairing it' this way.
                if ( err != KErrNotFound )
                    {
                    User::LeaveIfError( err );
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy( proxies ); // proxies
    
    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::RemoveProxyDataL" ) ) );
    }




// ---------------------------------------------------------
// CApDataHandler::WriteNetworkDataL
// ---------------------------------------------------------
//
void CApDataHandler::WriteNetworkDataL( TBool aIsNew, 
                                        CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::WriteNetworkDataL" ) ) );

    if ( aApItem.iNetworkName->Compare( KEmpty ) != 0 )
        {
        CCommsDbTableView* networks;
        HBufC* buf = HBufC::NewLC( KApEngineMaxSql );
        if ( aIsNew )
            {
            networks = iDb->OpenTableLC( TPtrC(NETWORK) );
            TUint32 netid;
            User::LeaveIfError( networks->InsertRecord( netid ) );
            aApItem.iNetworkId = netid;
            }
        else
            {
            networks = iDb->OpenViewMatchingUintLC( TPtrC(NETWORK), 
                                                    TPtrC(COMMDB_ID), 
                                                    aApItem.iNetworkId );
            TInt err = networks->GotoFirstRecord();
            if ( err == KErrNotFound )
                { 
                // originally there were NO network, but now it is added
                TUint32 netid;
                User::LeaveIfError( networks->InsertRecord( netid ) );
                aApItem.iNetworkId = netid;
                }
            else
                {
                User::LeaveIfError( networks->UpdateRecord() );
                }
            }
        networks->WriteTextL( TPtrC(COMMDB_NAME), *aApItem.iNetworkName );

        User::LeaveIfError( networks->PutRecordChanges( EFalse, EFalse ) );

        CleanupStack::PopAndDestroy( networks );   // networks
        CleanupStack::PopAndDestroy( buf );   // buf
        }
    
    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::WriteNetworkDataL" ) ) );
    }


// ---------------------------------------------------------
// CApDataHandler::ReadNetworkDataL
// ---------------------------------------------------------
//
void CApDataHandler::ReadNetworkDataL( CApAccessPointItem& aApItem )
    {
    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::ReadNetworkDataL" ) ) );
        
    CCommsDbTableView* networks;
    networks = iDb->OpenViewMatchingUintLC( TPtrC(NETWORK), 
                                            TPtrC(COMMDB_ID), 
                                            aApItem.iNetworkId );

    TInt err = networks->GotoFirstRecord();

    if ( err != KErrNotFound )
        {
        User::LeaveIfError( err );
        }
    if ( err != KErrNotFound )
        {
        HBufC* buf = ApCommons::ReadText16ValueLC( networks, 
                                                   TPtrC(COMMDB_NAME) );
        if ( err != KErrUnknown )
            {
            aApItem.WriteTextL( EApNetworkName, *buf );
            }

        CleanupStack::PopAndDestroy( buf );
        }
    else
        { // not found, set to NONE
        aApItem.WriteTextL( EApNetworkName, KEmpty );
        }
    CleanupStack::PopAndDestroy( networks ); // networks
    
    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::ReadNetworkDataL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::RemoveNetworkDataL
// ---------------------------------------------------------
//
void CApDataHandler::RemoveNetworkDataL( TUint32 aIspId )
    {
    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::RemoveNetworkDataL" ) ) );
        
    CCommsDbTableView* networks;
    networks = iDb->OpenViewMatchingUintLC( TPtrC(NETWORK), 
                                            TPtrC(COMMDB_ID), 
                                            aIspId );

    TInt err = networks->GotoFirstRecord();

    if ( err == KErrNone )
        {
        User::LeaveIfError( networks->DeleteRecord() );
        }
    else
        {
        // silently ignore KErrNotFound. It is caused by incorrect DB,
        // we are 'repairing it' this way.
        if ( err != KErrNotFound )
            {
            User::LeaveIfError( err );
            }
        }
    CleanupStack::PopAndDestroy( networks ); // networks, buf

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::RemoveNetworkDataL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::GetLocationIdL
// ---------------------------------------------------------
//
TUint32 CApDataHandler::GetLocationIdL()
    {
    // agreed that the location record having the name 'Mobile' is used
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::GetLocationIdL" ) ) );

    CCommsDbTableView* loc;
    loc = iDb->OpenViewMatchingTextLC( TPtrC(LOCATION), TPtrC(COMMDB_NAME), 
                                       KLocation );
    TInt err = loc->GotoFirstRecord();

    User::LeaveIfError( err );
    TUint32 id( 0 );
    loc->ReadUintL( TPtrC(COMMDB_ID), id );

    CleanupStack::PopAndDestroy( loc ); // loc

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::GetLocationIdL" ) ) );
    return id;    
    }



// ---------------------------------------------------------
// CApDataHandler::GetModemBearerIDL
// ---------------------------------------------------------
//
TUint32 CApDataHandler::GetModemBearerIDL( CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::GetModemBearerIDL" ) ) );

    // agreed that the modem bearer record having the name 
    // 'CSD Modem' is used for (HS)CSD Access Points and
    // 'GPRS Modem' is used for GPRS/WCDMA Access Points
    // 'CDMA Modem' is used for CDMA Access Points
    CCommsDbTableView* view = NULL;
    TApBearerType bearer = aApItem.BearerTypeL();
    switch ( bearer )
        {
        case EApBearerTypeCSD:
        case EApBearerTypeHSCSD:
            {
            view = iDb->OpenViewMatchingTextLC( TPtrC(MODEM_BEARER), 
                                               TPtrC(COMMDB_NAME), 
                                               KModemBearerCSD );
            break;
            }
        case EApBearerTypeCDMA:
            {
            view = iDb->OpenViewMatchingTextLC( TPtrC(MODEM_BEARER), 
                                               TPtrC(COMMDB_NAME), 
                                               KModemBearerCDMA );
            break;
            }
        case EApBearerTypeGPRS:
            {
            view = iDb->OpenViewMatchingTextLC( TPtrC(MODEM_BEARER), 
                                               TPtrC(COMMDB_NAME), 
                                               KModemBearerGPRS );
            break;
            }
#ifdef __TEST_LAN_BEARER
        case EApBearerTypeLAN:
            {
            view = iDb->OpenViewMatchingTextLC( TPtrC(LAN_BEARER), 
                                           TPtrC(COMMDB_NAME), 
                                           KModemBearerLAN );
            break;
            }
#endif // __TEST_LAN_BEARER            
        case EApBearerTypeWLAN:
            {
            if ( aApItem.iExt->iWlanData )
                {                
                view = iDb->OpenViewMatchingTextLC( TPtrC(LAN_BEARER), 
                                                    TPtrC(COMMDB_NAME), 
                                                    KModemBearerWLAN );
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
        case EApBearerTypeLANModem:
            {
            view = iDb->OpenViewMatchingTextLC( TPtrC(LAN_BEARER), 
                                           TPtrC(COMMDB_NAME), 
                                           KModemBearerLANModem );
            break;
            }            
        default:
            {
            User::Leave( KErrInvalidBearerType );
            }
        }
        
    TUint32 id( 0 );
    
    if ( view )
        {
        TInt err = view->GotoFirstRecord();
        if ( err == KErrNone )
            {
            view->ReadUintL( TPtrC(COMMDB_ID), id );
            }
        else
            {
            if ( err != KErrNotFound ) 
                {
                User::LeaveIfError( err );
                }
            else
                {
                if ( aApItem.iExt->iWlanData )
                    {
                    id = CreateLanBearerIfNeededL();
                    }
                else
                    {
#ifdef      __TEST_LAN_BEARER
                    id = CreateLanBearerIfNeededL();
#else // __TEST_LAN_BEARER                
                    User::Leave( KErrNotSupported );
#endif //__TEST_LAN_BEARER
                    }
                }
            }
        CleanupStack::PopAndDestroy( view ); // view
        }

    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::GetModemBearerIDL" ) ) );
    return id;
    }




// ---------------------------------------------------------
// CApDataHandler::ReadServiceL
// ---------------------------------------------------------
//
void CApDataHandler::ReadServiceL( CCommsDbTableView& ispt,
                                   CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::ReadServiceL" ) ) );

    HBufC* buf = ApCommons::ReadText16ValueLC( &ispt, TPtrC(COMMDB_NAME) );
    aApItem.WriteTextL( EApIspName, *buf );
    CleanupStack::PopAndDestroy( buf );
    
//    TUint32 tempint( 0 );
    TBool   tempbool( EFalse );

/*
    buf = ApCommons::ReadText16ValueLC( &ispt, TPtrC(SERVICE_IF_PARAMS) );
    aApItem.WriteTextL( EApIspIfParams, *buf );
    CleanupStack::PopAndDestroy( buf );
*/
    buf = ApCommons::ReadText16ValueLC( &ispt, TPtrC(SERVICE_IF_NETWORKS) );
    aApItem.WriteTextL( EApWlanIfNetworks, *buf );
    CleanupStack::PopAndDestroy( buf );

/*
    ApCommons::ReadBoolL( &ispt, TPtrC(SERVICE_IF_PROMPT_FOR_AUTH), tempbool );
    aApItem.WriteBool( EApWlanIfPromptForAuth, tempbool );

    buf = ApCommons::ReadText16ValueLC( &ispt, TPtrC(SERVICE_IF_AUTH_NAME) );
    aApItem.WriteTextL( EApWlanIfAuthName, *buf );
    CleanupStack::PopAndDestroy( buf );

    buf = ApCommons::ReadText16ValueLC( &ispt, TPtrC(SERVICE_IF_AUTH_PASS) );
    aApItem.WriteTextL( EApWlanIfAuthPassword, *buf );
    CleanupStack::PopAndDestroy( buf );

    ApCommons::ReadUintL( &ispt, TPtrC(SERVICE_IF_AUTH_RETRIES), tempint );
    aApItem.WriteUint( EApWlanIfAuthRetries, tempint );

    ApCommons::ReadBoolL( &ispt,
                           TPtrC(SERVICE_IF_CALLBACK_ENABLED), tempbool );
    aApItem.WriteBool( EApIspIfCallbackEnabled, tempbool );

    ApCommons::ReadUintL( &ispt, TPtrC(SERVICE_IF_CALLBACK_TYPE), tempint );
    aApItem.WriteUint( EApIspIfCallbackType, ( TCallbackAction )tempint );

    HBufC8* buf8 = ApCommons::ReadText8ValueLC
                                ( &ispt, TPtrC(SERVICE_IF_CALLBACK_INFO) );
    aApItem.WriteTextL( EApIspIfCallbackInfo, *buf8 );
    CleanupStack::PopAndDestroy( buf8 ); 

    ApCommons::ReadUintL( &ispt, TPtrC(SERVICE_CALLBACK_TIMEOUT), tempint );
    aApItem.WriteUint( EApIspCallBackTimeOut, tempint );
*/

    buf = ApCommons::ReadText16ValueLC( &ispt, TPtrC(SERVICE_IP_NETMASK) );
    aApItem.WriteTextL( EApWlanIpNetMask, *buf );
    CleanupStack::PopAndDestroy( buf );

    buf = ApCommons::ReadText16ValueLC( &ispt, TPtrC(SERVICE_IP_GATEWAY) );
    aApItem.WriteTextL( EApWlanIpGateway, *buf );
    CleanupStack::PopAndDestroy( buf );

    ApCommons::ReadBoolL( &ispt, 
                          TPtrC(SERVICE_IP_ADDR_FROM_SERVER), tempbool );
    aApItem.WriteBool( EApWlanIpAddrFromServer, tempbool );

    buf = ApCommons::ReadText16ValueLC( &ispt, TPtrC(SERVICE_IP_ADDR) );
    aApItem.WriteTextL( EApWlanIpAddr, *buf );
    CleanupStack::PopAndDestroy( buf );

    ApCommons::ReadBoolL( &ispt, TPtrC(SERVICE_IP_DNS_ADDR_FROM_SERVER),
                           tempbool );
    aApItem.WriteBool( EApWlanIpDnsAddrFromServer, tempbool );

    buf = ApCommons::ReadText16ValueLC( &ispt, 
                                        TPtrC(SERVICE_IP_NAME_SERVER1) );
    aApItem.WriteTextL( EApWlanIPNameServer1, *buf );
    CleanupStack::PopAndDestroy( buf );

    buf = ApCommons::ReadText16ValueLC( &ispt, 
                                        TPtrC(SERVICE_IP_NAME_SERVER2) );
    aApItem.WriteTextL( EApWlanIPNameServer2, *buf );
    CleanupStack::PopAndDestroy( buf );

/*
    ApCommons::ReadBoolL( &ispt, TPtrC(SERVICE_ENABLE_SW_COMP), tempbool );
    aApItem.WriteBool( EApIspEnableSWCompression, tempbool );

    ApCommons::ReadBoolL( &ispt, 
                          TPtrC(SERVICE_ENABLE_IP_HEADER_COMP), 
                          tempbool );
    aApItem.WriteBool( EApIspEnableIpHeaderComp, tempbool);

    ApCommons::ReadBoolL( &ispt, 
                          TPtrC(SERVICE_ENABLE_LCP_EXTENSIONS), tempbool );
    aApItem.WriteBool( EApIspEnableLCPExtensions, tempbool );

    ApCommons::ReadBoolL( &ispt, TPtrC(SERVICE_DISABLE_PLAIN_TEXT_AUTH),
                           tempbool );
    aApItem.WriteBool( EApIspDisablePlainTextAuth, tempbool );
*/
    CLOG( ( EHandler, 0, _L( "<- CApDataHandler::ReadServiceL" ) ) );
    }


// ---------------------------------------------------------
// CApDataHandler::ReadServiceIp6L
// ---------------------------------------------------------
//
void CApDataHandler::ReadServiceIp6L( CCommsDbTableView& ispt,
                                   CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::ReadServiceIp6L" ) ) );

    if ( iExt->iIsIpv6Supported )
        {
        TBool tempbool( EFalse );
        HBufC* buf;

        ApCommons::ReadBoolL( &ispt, TPtrC(SERVICE_IP6_DNS_ADDR_FROM_SERVER),
                               tempbool );

        aApItem.WriteBool( EApIP6DNSAddrFromServer, tempbool );

        buf = ApCommons::ReadText16ValueLC( &ispt,
                                            TPtrC(SERVICE_IP6_NAME_SERVER1) );

        aApItem.WriteTextL( EApIP6NameServer1, *buf );
        CleanupStack::PopAndDestroy( buf );

        buf = ApCommons::ReadText16ValueLC( &ispt,
                                            TPtrC(SERVICE_IP6_NAME_SERVER2) );

        aApItem.WriteTextL( EApIP6NameServer2, *buf );
        CleanupStack::PopAndDestroy( buf );
        }

    CLOG( ( EHandler, 0, _L( "<- CApDataHandler::ReadServiceIp6L" ) ) );
    }




// ---------------------------------------------------------
// CApDataHandler::WriteServiceL
// ---------------------------------------------------------
//
void CApDataHandler::WriteServiceL( CCommsDbTableView& aTable,
                                    CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::WriteServiceL" ) ) );
    
    aTable.WriteTextL( TPtrC(COMMDB_NAME), *aApItem.iIspName );
    
//    aTable.WriteTextL( TPtrC(SERVICE_IF_PARAMS), *aApItem.iIspIfParams );

    aTable.WriteTextL( TPtrC(SERVICE_IF_NETWORKS), *aApItem.iIspIfNetworks );

//    aTable.WriteBoolL( TPtrC(SERVICE_IF_PROMPT_FOR_AUTH),
//                     aApItem.iIspIfPromptForAuth );

//    aTable.WriteTextL( TPtrC(SERVICE_IF_AUTH_NAME), 
//                       *aApItem.iIspIfAuthName );

//    aTable.WriteTextL( TPtrC(SERVICE_IF_AUTH_PASS), 
//                       *aApItem.iIspIfAuthPass );

//    aTable.WriteUintL( TPtrC(SERVICE_IF_AUTH_RETRIES),
//                     aApItem.iIspIfAuthRetries );

    aTable.WriteTextL( TPtrC(SERVICE_IP_NETMASK), *aApItem.iIspIPNetMask );

    aTable.WriteTextL( TPtrC(SERVICE_IP_GATEWAY), *aApItem.iIspGateway );

    aTable.WriteBoolL( TPtrC(SERVICE_IP_ADDR_FROM_SERVER),
                       aApItem.iIspIPAddrFromServer );

    aTable.WriteTextL( TPtrC(SERVICE_IP_ADDR), *aApItem.iIspIPAddr );

    aTable.WriteBoolL( TPtrC(SERVICE_IP_DNS_ADDR_FROM_SERVER),
                     aApItem.iGetDNSIPFromServer );

    aTable.WriteTextL( TPtrC(SERVICE_IP_NAME_SERVER1), 
                       *aApItem.iPrimaryDNS );

    aTable.WriteTextL( TPtrC(SERVICE_IP_NAME_SERVER2), 
                       *aApItem.iSecondaryDNS );

/*
    aTable.WriteBoolL( TPtrC(SERVICE_ENABLE_LCP_EXTENSIONS),
                     aApItem.iIspEnableLCPExtensions );

    aTable.WriteBoolL( TPtrC(SERVICE_DISABLE_PLAIN_TEXT_AUTH),
                     aApItem.iIsPasswordAuthenticationSecure );

    aTable.WriteBoolL( TPtrC(SERVICE_ENABLE_SW_COMP),
                     aApItem.iIspEnableSwCompression );

    aTable.WriteBoolL( TPtrC(SERVICE_ENABLE_IP_HEADER_COMP),
                     aApItem.iEnablePPPCompression );
        
    aTable.WriteBoolL( TPtrC(SERVICE_IF_CALLBACK_ENABLED),
                     aApItem.iUseCallBack );

    aTable.WriteUintL( TPtrC(SERVICE_IF_CALLBACK_TYPE),
                     aApItem.iCallBackTypeIsServerNum );

    aTable.WriteTextL( TPtrC(SERVICE_IF_CALLBACK_INFO),
                     *aApItem.iIspIfCallbackInfo );

    aTable.WriteUintL( TPtrC(SERVICE_CALLBACK_TIMEOUT),
                     aApItem.iIspCallbackTimeOut );    
*/    

//    WriteDaemonL( aTable, aApItem ); starts here
    if ( aApItem.iIspIPAddrFromServer )
        {
        aTable.WriteTextL( TPtrC(SERVICE_CONFIG_DAEMON_MANAGER_NAME), 
                           KDaemonManagerName );
        aTable.WriteTextL( TPtrC(SERVICE_CONFIG_DAEMON_NAME), 
                           KConfigDaemonName );
        }
    else
        {
        // 3.1 it is changed!!
        if ( FeatureManager::FeatureSupported( KFeatureIdIPv6 ) )
            {
            aTable.WriteTextL( TPtrC(SERVICE_CONFIG_DAEMON_MANAGER_NAME), 
                               KDaemonManagerName );
            aTable.WriteTextL( TPtrC(SERVICE_CONFIG_DAEMON_NAME), 
                               KConfigDaemonName );            
            }
        else
            {
            aTable.WriteTextL( TPtrC(SERVICE_CONFIG_DAEMON_MANAGER_NAME), KEmpty );
            aTable.WriteTextL( TPtrC(SERVICE_CONFIG_DAEMON_NAME), KEmpty );            
            }
        }
//    WriteDaemonL() Ends here


    CLOG( ( EHandler, 0, _L( "<- CApDataHandler::WriteServiceL" ) ) );
    }


// ---------------------------------------------------------
// CApDataHandler::WriteServiceIp6L
// ---------------------------------------------------------
//
void CApDataHandler::WriteServiceIp6L( CCommsDbTableView& ispt,
                                       CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::WriteServiceIp6L" ) ) );

    ispt.WriteBoolL( TPtrC(SERVICE_IP6_DNS_ADDR_FROM_SERVER),
                     aApItem.iExt->iIpv6GetDNSIPFromServer );

    ispt.WriteTextL( TPtrC(SERVICE_IP6_NAME_SERVER1), 
                    *aApItem.iExt->iIpv6PrimaryDNS );

    ispt.WriteTextL( TPtrC(SERVICE_IP6_NAME_SERVER2), 
                    *aApItem.iExt->iIpv6SecondaryDNS );
        
    CLOG( ( EHandler, 0, _L( "<- CApDataHandler::WriteServiceIp6L" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::WriteCsdDataL
// ---------------------------------------------------------
//
void CApDataHandler::WriteCsdDataL( TBool aIsNew,
                                    CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::WriteCsdDataL" ) ) );

    TInt isCsdSupported = 
         FeatureManager::FeatureSupported( KFeatureIdAppCsdSupport ); 
#ifdef __TEST_CSD_SUPPORT
    isCsdSupported = ETrue;
#endif // __TEST_CSD_SUPPORT
   
    TCommsDbBearerType bearerType = 
                    STATIC_CAST( TCommsDbBearerType, aApItem.iIspBearerType );

    if ( ( ( bearerType == EBearerTypeCSD ) && ( !isCsdSupported ) ) || 
         ( ( bearerType == EBearerTypeHSCSD ) && 
           ( !isCsdSupported || !iExt->iIsAppHscsdSupport ) ) )
        {
        User::Leave( KErrNotSupported );
        }

    CCommsDbTableView* ispt;

    if ( aIsNew )
        {
        ispt = iDb->OpenTableLC( *aApItem.iIapServiceType );
        User::LeaveIfError( ispt->InsertRecord( aApItem.iIapServiceId ) );
        }
    else
        {
        ispt = iDb->OpenViewMatchingUintLC( *aApItem.iIapServiceType,
                                           TPtrC(COMMDB_ID),
                                           aApItem.iIapServiceId );
        User::LeaveIfError( ispt->GotoFirstRecord() );
        User::LeaveIfError( ispt->UpdateRecord() );
        }
    // now write record data
    ispt->WriteTextL( TPtrC(COMMDB_NAME), *aApItem.iIspName );

    ispt->WriteTextL( TPtrC(ISP_DESCRIPTION), *aApItem.iIspDescription );

    ispt->WriteUintL( TPtrC(ISP_TYPE), aApItem.iIspType );

    ispt->WriteTextL( TPtrC(ISP_DEFAULT_TEL_NUM),
                     *aApItem.iIspDefaultTelNumber );

    ispt->WriteBoolL( TPtrC(ISP_DIAL_RESOLUTION),
                     aApItem.iIspDialResolution );

    ispt->WriteBoolL( TPtrC(ISP_USE_LOGIN_SCRIPT),
                      aApItem.iUseLoginScript );

    ispt->WriteLongTextL( TPtrC(ISP_LOGIN_SCRIPT), *aApItem.iLoginScript );

    ispt->WriteBoolL( TPtrC(ISP_PROMPT_FOR_LOGIN),
                      aApItem.iPromptPassword );

    ispt->WriteTextL( TPtrC(ISP_LOGIN_NAME), *aApItem.iUserName );

    ispt->WriteTextL( TPtrC(ISP_LOGIN_PASS), *aApItem.iPassword );

    ispt->WriteBoolL( TPtrC(ISP_DISPLAY_PCT),
                     aApItem.iDisplayTerminalWindow );

    ApSpeed2EtelL( &aApItem );

/*
// Keep as long as replacing IF_NAME does not works as specified
    ispt->WriteTextL( TPtrC(IF_NAME), *aApItem.iIspIfName );
*/

    ispt->WriteTextL( TPtrC(ISP_IF_PARAMS), *aApItem.iIspIfParams );

    ispt->WriteTextL( TPtrC(ISP_IF_NETWORKS), *aApItem.iIspIfNetworks );

    ispt->WriteBoolL( TPtrC(ISP_IF_PROMPT_FOR_AUTH),
                     aApItem.iIspIfPromptForAuth );

    // as same UserName & Auth_name is required
    ispt->WriteTextL( TPtrC(ISP_IF_AUTH_NAME), *aApItem.iUserName );

    // as same login_pass & Auth_password is required
    ispt->WriteTextL( TPtrC(ISP_IF_AUTH_PASS), *aApItem.iPassword );

    ispt->WriteUintL( TPtrC(ISP_IF_AUTH_RETRIES),
                      aApItem.iIspIfAuthRetries );

    ispt->WriteBoolL( TPtrC(ISP_IF_CALLBACK_ENABLED),
                      aApItem.iUseCallBack );

    ispt->WriteUintL( TPtrC(ISP_IF_CALLBACK_TYPE),
                     aApItem.iCallBackTypeIsServerNum );

    ispt->WriteTextL( TPtrC(ISP_IF_CALLBACK_INFO),
                      *aApItem.iIspIfCallbackInfo );

    ispt->WriteUintL( TPtrC(ISP_CALLBACK_TIMEOUT),
                     aApItem.iIspCallbackTimeOut );

    ispt->WriteBoolL( TPtrC(ISP_IP_ADDR_FROM_SERVER),
                     aApItem.iIspIPAddrFromServer );

    ispt->WriteTextL( TPtrC(ISP_IP_ADDR), *aApItem.iIspIPAddr );

    ispt->WriteTextL( TPtrC(ISP_IP_NETMASK), *aApItem.iIspIPNetMask );

    ispt->WriteTextL( TPtrC(ISP_IP_GATEWAY), *aApItem.iIspGateway );

    ispt->WriteBoolL( TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),
                     aApItem.iGetDNSIPFromServer );

    ispt->WriteTextL( TPtrC(ISP_IP_NAME_SERVER1), *aApItem.iPrimaryDNS );

    ispt->WriteTextL( TPtrC(ISP_IP_NAME_SERVER2), *aApItem.iSecondaryDNS );

    ispt->WriteBoolL( TPtrC(ISP_ENABLE_IP_HEADER_COMP),
                     aApItem.iEnablePPPCompression );

    ispt->WriteBoolL( TPtrC(ISP_ENABLE_LCP_EXTENSIONS),
                     aApItem.iIspEnableLCPExtensions );

    ispt->WriteBoolL( TPtrC(ISP_DISABLE_PLAIN_TEXT_AUTH),
                     aApItem.iIsPasswordAuthenticationSecure );

    ispt->WriteBoolL( TPtrC(ISP_ENABLE_SW_COMP),
                     aApItem.iIspEnableSwCompression );

    ispt->WriteUintL( TPtrC(ISP_BEARER_NAME), aApItem.iIspBearerName );

    ispt->WriteUintL( TPtrC(ISP_BEARER_SPEED), aApItem.iExt->iBearerSpeed );
    
    ispt->WriteUintL( TPtrC(ISP_BEARER_PROTOCOL), 
                      aApItem.iExt->iBearerProtocol );

    ispt->WriteUintL( TPtrC(ISP_BEARER_CE), aApItem.iIspBearerCE );

    ispt->WriteUintL( TPtrC(ISP_BEARER_SERVICE), aApItem.iApIapBearerService );



    ispt->WriteTextL( TPtrC(ISP_INIT_STRING), *aApItem.iIspInitString );

    ispt->WriteUintL( TPtrC(ISP_BEARER_TYPE), aApItem.iIspBearerType );

    ispt->WriteUintL( TPtrC(ISP_CHANNEL_CODING),
                      aApItem.iIspChannelCoding );

    ispt->WriteUintL( TPtrC(ISP_AIUR), aApItem.iIspAIUR );

    ispt->WriteUintL( TPtrC(ISP_REQUESTED_TIME_SLOTS),
                     aApItem.iIspRequestedTimeSlots );

    ispt->WriteUintL( TPtrC(ISP_MAXIMUM_TIME_SLOTS),
                     aApItem.iIspMaximumTimeSlots );

    if ( iExt->iIsIpv6Supported )
        {
        WriteServiceIp6L( *ispt, aApItem );
        }

    // No DHCP for CSD/HSCSD, so set it empty
    ispt->WriteTextL( TPtrC(SERVICE_CONFIG_DAEMON_MANAGER_NAME), KEmpty );
    ispt->WriteTextL( TPtrC(SERVICE_CONFIG_DAEMON_NAME), KEmpty );
    
    // now put changes
    User::LeaveIfError( ispt->PutRecordChanges( EFalse, EFalse ) );

    CleanupStack::PopAndDestroy( ispt );  // ispt

    CLOG( ( EHandler, 0, _L( "<- CApDataHandler::WriteCsdDataL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::ReadWlanL
// ---------------------------------------------------------
//
void CApDataHandler::ReadWlanL( CCommsDbTableView& lant,
                                CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::ReadWlanL" ) ) );

    HBufC* buf = ApCommons::ReadText16ValueLC( &lant, TPtrC(NU_WLAN_SSID) );
    aApItem.WriteTextL( EApWlanNetworkName, *buf );
    CleanupStack::PopAndDestroy( buf );
    
    TUint32 tempint( 0 );

    ApCommons::ReadUintL( &lant, TPtrC(COMMDB_ID), tempint );
    aApItem.WriteUint( EApWlanSettingsId, tempint );

    ApCommons::ReadUintL( &lant, TPtrC(WLAN_CONNECTION_MODE), tempint );
    aApItem.WriteUint( EApWlanNetworkMode, tempint );

    ApCommons::ReadUintL( &lant, TPtrC(WLAN_SECURITY_MODE), tempint );
    aApItem.WriteUint( EApWlanSecurityMode, tempint );

    TBool b;
    ApCommons::ReadBoolL( &lant, TPtrC(WLAN_SCAN_SSID), b );
    aApItem.WriteBool( EApWlanScanSSID, b );

    ApCommons::ReadUintL( &lant, TPtrC(NU_WLAN_CHANNEL_ID), tempint );
    aApItem.WriteUint( EApWlanChannelId, tempint );

    CLOG( ( EHandler, 0, _L( "<- CApDataHandler::ReadWlanL" ) ) );
    }



// ---------------------------------------------------------
// CApDataHandler::WriteWlanL
// ---------------------------------------------------------
//
void CApDataHandler::WriteWlanL( CCommsDbTableView& aTable,
                                 CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::WriteWlanL" ) ) );

    aTable.WriteTextL( TPtrC(COMMDB_NAME), *aApItem.iIspName );
    aTable.WriteUintL( TPtrC(WLAN_SERVICE_ID), aApItem.iIapServiceId );
    aTable.WriteTextL( TPtrC(NU_WLAN_SSID), 
                       *aApItem.iExt->iWlanData->iWlanNetworkName );
    aTable.WriteUintL( TPtrC(WLAN_CONNECTION_MODE), 
                      aApItem.iExt->iWlanData->iWlanNetworkMode );

    aTable.WriteUintL( TPtrC(WLAN_SECURITY_MODE), 
                      aApItem.iExt->iWlanData->iWlanSecurityMode );

    aTable.WriteBoolL( TPtrC(WLAN_SCAN_SSID),
                     aApItem.iExt->iWlanData->iScanSSID );
                     
    aTable.WriteUintL( TPtrC(NU_WLAN_CHANNEL_ID), 
                      aApItem.iExt->iWlanData->iChannelId );
    
    CLOG( ( EHandler, 0, _L( "<- CApDataHandler::WriteWlanL" ) ) );
    }




// ---------------------------------------------------------
// CApDataHandler::CreateLanBearerIfNeededL
// ---------------------------------------------------------
//
TUint32 CApDataHandler::CreateLanBearerIfNeededL()
    {
    CLOG( ( EApItem, 0, 
            _L( "-> CApDataHandler::CreateLanBearerIfNeededL" ) ) );
    
    TUint32 retval( 0 );

    CCommsDbTableView* t = NULL;

    t = iDb->OpenViewMatchingTextLC( TPtrC(LAN_BEARER), 
                                     TPtrC(LAN_BEARER_AGENT), 
                                     KWlanBearerAgent );
    if ( t->GotoFirstRecord() == KErrNotFound )
        { // no such yet, create one
        TInt err = t->InsertRecord( retval );
        if ( err )
            {
            User::LeaveIfError( err );
            }
        t->WriteTextL( TPtrC(COMMDB_NAME), KWlanBearerName );
        t->WriteTextL( TPtrC(LAN_BEARER_AGENT), KWlanBearerAgent );
        t->WriteTextL( TPtrC(LAN_BEARER_NIF_NAME), KWlanBearerNif );

        t->WriteTextL( TPtrC(LAN_BEARER_LDD_NAME), KWlanLDDName );
        t->WriteTextL( TPtrC(LAN_BEARER_PDD_NAME), KWlanPDDName );



        t->WriteUintL( TPtrC(LAST_SOCKET_ACTIVITY_TIMEOUT), 
                       (TUint32)KWlanLastSocketActivityTimeout ); 
        t->WriteUintL( TPtrC(LAST_SESSION_CLOSED_TIMEOUT), 
                       KWlanLastSessionClosedTimeout ); 
        t->WriteUintL( TPtrC(LAST_SOCKET_CLOSED_TIMEOUT), 
                       (TUint32)KWlanLastSocketClosedTimeout ); 

        User::LeaveIfError( t->PutRecordChanges( EFalse, EFalse ) );

        }
    else
        { // there is one, use it
        t->ReadUintL( TPtrC(COMMDB_ID), retval );
        }

    CleanupStack::PopAndDestroy( t );   // t
    
    CLOG( ( EApItem, 0, 
            _L( "<- CApDataHandler::CreateLanBearerIfNeededL" ) ) );

    return retval;
    }






// ---------------------------------------------------------
// CApDataHandler::ReadModemBearerNameL
// ---------------------------------------------------------
//
void CApDataHandler::ReadModemBearerNameL( CApAccessPointItem& aApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CApDataHandler::ReadModemBearerNameL" ) ) );

    // agreed that the modem bearer record having the name 
    // 'CSD Modem' is used for (HS)CSD Access Points and
    // 'GPRS Modem' is used for GPRS/WCDMA Access Points
    // 'CDMA Modem' is used for CDMA Access Points
    CCommsDbTableView* view = NULL;
//    TApBearerType bearer = aApItem.BearerTypeL();

    view = iDb->OpenViewMatchingUintLC( *aApItem.iExt->iIapBearerType, 
                                       TPtrC(COMMDB_ID), 
                                       aApItem.iExt->iIapBearerID );
//                                       0 );
    HBufC* buf = NULL;
    buf = ApCommons::ReadText16ValueLC( view, TPtrC(COMMDB_NAME) );
    aApItem.WriteTextL( EApLanBearerName, *buf );
    CleanupStack::PopAndDestroy( buf );

    CleanupStack::PopAndDestroy( view );


    CLOG( ( EHandler, 1, _L( "<- CApDataHandler::ReadModemBearerNameL" ) ) );
    }

//  End of File
