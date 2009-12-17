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
* Description:  Declaration of the class CApUtils
*     Contains utility functions for access point handling
*
*/


// INCLUDE FILES
#include "APEngineVariant.hrh"

#include "ApUtils.h"
#include "ApAccessPointItem.h"
#include "ApDataHandler.h"
#include "ApEngineCommons.h"
#include "ApEngineLogger.h"
#include <e32des16.h>
#include <es_sock.h>
#include <es_enum.h>

#include    <featmgr.h>

#include <WlanCdbCols.h>


// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES


// ================= MEMBER FUNCTIONS =======================

// construct/destruct/copy
// ---------------------------------------------------------
// CApUtils::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApUtils* CApUtils::NewLC( CCommsDatabase& aDb )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::NewLC" ) ) );

    CApUtils* db = new( ELeave ) CApUtils;
    CleanupStack::PushL( db );
    db->ConstructL( aDb );

    CLOG( ( EUtil, 1, _L( "<- CApUtils::NewLC" ) ) );
    return db;
    }



// Destructor
// ---------------------------------------------------------
// CApUtils::~CApUtils
// ---------------------------------------------------------
//
EXPORT_C CApUtils::~CApUtils()
    {
    if ( iExt )
        {
        if ( iExt->iIsFeatureManagerInitialised )
            {
            FeatureManager::UnInitializeLib();
            }
        }    
    delete iExt;
    CLOG( ( EUtil, 0, _L( "<-> CApUtils::~CApUtils" ) ) );
    }


// C++ default constructor can NOT contain any code that
// might leave.
//
// ---------------------------------------------------------
// CApUtils::CApUtils
// ---------------------------------------------------------
//
EXPORT_C CApUtils::CApUtils()
    {
    }


// Symbian OS default constructor can leave.
// ---------------------------------------------------------
// CApUtils::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApUtils::ConstructL( CCommsDatabase& aDb )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::ConstructL" ) ) );

    iDb = &aDb;
    iExt = new ( ELeave )TUtilsExtra;
    
    FeatureManager::InitializeLibL();
    iExt->iIsFeatureManagerInitialised = ETrue;    

    iExt->iVariant = ApCommons::GetVariantL();
#ifdef __TEST_CDMA_WRITE_PROTECT
    iExt->iVariant |= KApUiEditOnlyVPNs;
#endif // __TEST_CDMA_WRITE_PROTECT

    iExt->iWlanSupported = 
        FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );
    
    
#ifdef __TEST_WLAN_SUPPORT
    iExt->iWlanSupported = ETrue;
#endif // __TEST_WLAN_SUPPORT
    
    CLOG( ( EUtil, 1, _L( "<- CApUtils::ConstructL" ) ) );
    }





// utilities

// ---------------------------------------------------------
// CApUtils::IsAPInUseL
// ---------------------------------------------------------
//
EXPORT_C TBool CApUtils::IsAPInUseL( TUint32 aUid )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::IsAPInUseL" ) ) );



    TBool retval( EFalse );
#ifdef __WINS__
    // just to satisfy compiler...
    aUid++;
#else // 
    RSocketServ rs;
    RConnection rc;

    TUint32 iap( 0 );
    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb, ETrue );
    TRAP_IGNORE( iap = IapIdFromWapIdL( aUid ) );

    User::LeaveIfError( rs.Connect() );    
    User::LeaveIfError( rc.Open( rs ) );

    TUint count( 0 );
    User::LeaveIfError( rc.EnumerateConnections( count ) );
    // Go on only if there is any
    if ( count )
        {
        for( TUint i = 1; i <= count; i++)
            {
            TPckgBuf<TConnectionInfo> connInfo;
            User::LeaveIfError( rc.GetConnectionInfo( i, connInfo ) );
            if ( iap == connInfo().iIapId )
                { 
                // IAP is in use, break
                retval = ETrue;
                break;
                }
            }
        }

    rc.Close();
    rs.Close();    

    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }
#endif // __WINS__     

    CLOG( ( EUtil, 1, _L( "<- CApUtils::IsAPInUseL" ) ) );
    return ( retval );
    }




// ---------------------------------------------------------
// CApUtils::WapBearerTypeL
// ---------------------------------------------------------
//
EXPORT_C void CApUtils::WapBearerTypeL( TUint32 aUid, TDes& aBearer )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::WapBearerTypeL" ) ) );
    
    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );

    DoWapBearerTypeL( aUid, aBearer );

    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EUtil, 1, _L( "<- CApUtils::WapBearerTypeL" ) ) );
    }


// ---------------------------------------------------------
// CApUtils::BearerTypeL
// ---------------------------------------------------------
//
EXPORT_C TApBearerType CApUtils::BearerTypeL( TUint32 aUid )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::BearerTypeL" ) ) );
    
    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    TApBearerType retval = EApBearerTypeAllBearers;
    retval = DoBearerTypeL( aUid );
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EUtil, 1, _L( "<- CApUtils::BearerTypeL" ) ) );
    return retval;
    }


// ---------------------------------------------------------
// CApUtils::ISPTypeL
// ---------------------------------------------------------
//
EXPORT_C void CApUtils::ISPTypeL( TUint32 aUid, TCommsDbIspType& aIspType )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::ISPTypeL" ) ) );

    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    DoISPTypeL( aUid, aIspType );
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EUtil, 1, _L( "<- CApUtils::ISPTypeL" ) ) );
    }



// ---------------------------------------------------------
// CApUtils::IsValidNameL
// ---------------------------------------------------------
//
EXPORT_C TBool CApUtils::IsValidNameL( const TDesC& aNameText )
    {
    CLOG( ( EUtil, 0, _L( "<-> CApUtils::IsValidNameL" ) ) );
    
    return IsValidNameL( aNameText, 0 );
    }


// ---------------------------------------------------------
// CApUtils::MakeValidName
// ---------------------------------------------------------
//
EXPORT_C TBool CApUtils::MakeValidNameL( TDes& aNameText )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::MakeValidNameL" ) ) );
   
    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );

    HBufC* buf = HBufC::NewLC( KModifiableTextLength );
    TPtr ptr( buf->Des() );
    ptr.Copy( aNameText );
    TBool nameChanged( EFalse );
    HBufC* sgd = ApCommons::DoMakeValidNameL( *iDb,
                                               nameChanged,
                                               buf,
                                               0,
                                               EFalse );

    if ( nameChanged )
        {
        aNameText.Copy( *sgd );
        }
    CleanupStack::PopAndDestroy( buf ); // buf
    delete sgd;
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EUtil, 1, _L( "<- CApUtils::MakeValidNameL" ) ) );
    return nameChanged;
    }


// ---------------------------------------------------------
// CApUtils::NameL
// ---------------------------------------------------------
//
EXPORT_C void CApUtils::NameL( TUint32 aUid, TDes& aName )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::NameL" ) ) );

    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    DoNameL( aUid, aName );
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EUtil, 1, _L( "<- CApUtils::NameL" ) ) );
    }



// ---------------------------------------------------------
// CApUtils::SetNameL
// ---------------------------------------------------------
//
EXPORT_C void CApUtils::SetNameL( TDes& aName, TUint32 aUid )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::SetNameL" ) ) );

    if ( iExt->iVariant & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }

    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    DoSetNameL( aName, aUid );
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EUtil, 1, _L( "<- CApUtils::SetNameL" ) ) );
    }



// ---------------------------------------------------------
// CApUtils::SetISPTypeL
// ---------------------------------------------------------
//
EXPORT_C void CApUtils::SetISPTypeL( TUint32 aUid, TCommsDbIspType aIspType )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::SetISPTypeL" ) ) );

    if ( iExt->iVariant & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }
    
    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    DoSetISPTypeL( aUid, aIspType );
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EUtil, 1, _L( "<- CApUtils::SetISPTypeL" ) ) );
    }



// ---------------------------------------------------------
// CApUtils::IapIdFromWapIdL
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApUtils::IapIdFromWapIdL( TUint32 aUid )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::IapIdFromWapIdL" ) ) );
    
    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    TUint32 retval( 0 );

    CCommsDbTableView* wapt;
    wapt = iDb->OpenViewMatchingUintLC( TPtrC(WAP_ACCESS_POINT),
                                       TPtrC(COMMDB_ID), aUid );
    User::LeaveIfError( wapt->GotoFirstRecord() );

    HBufC* wapBearer = ApCommons::ReadText16ValueLC
                                                (
                                                wapt,
                                                TPtrC(WAP_CURRENT_BEARER)
                                                );

    if ( *wapBearer == TPtrC(WAP_IP_BEARER) )
        {
        CCommsDbTableView* bearert;
        bearert = iDb->OpenViewMatchingUintLC( TPtrC(*wapBearer),
                                            TPtrC(WAP_ACCESS_POINT_ID), aUid );
        User::LeaveIfError( bearert->GotoFirstRecord() );
        ApCommons::ReadUintL( bearert, TPtrC(WAP_IAP), retval );
        CleanupStack::PopAndDestroy( bearert ); // bearert
        }
    else
        {
        User::Leave( KErrInvalidBearerType );
        }
    CleanupStack::PopAndDestroy( 2, wapt ); // wapBearer, wapt
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EUtil, 1, _L( "<- CApUtils::IapIdFromWapIdL" ) ) );
    return retval;
    }


// ---------------------------------------------------------
// CApUtils::WapIdFromIapIdL
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApUtils::WapIdFromIapIdL( TUint32 aUid )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::WapIdFromIapIdL" ) ) );

    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    TUint32 retval( 0 );

    CCommsDbTableView* bearert;
    bearert = iDb->OpenViewMatchingUintLC( TPtrC(WAP_IP_BEARER),
                                          TPtrC(WAP_IAP), aUid );
    User::LeaveIfError( bearert->GotoFirstRecord() );
    ApCommons::ReadUintL( bearert, TPtrC(WAP_ACCESS_POINT_ID), retval );
    CleanupStack::PopAndDestroy( bearert ); // bearert

    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EUtil, 1, _L( "<- CApUtils::WapIdFromIapIdL" ) ) );
    return retval;
    }


// ---------------------------------------------------------
// CApUtils::HasMandatoryFieldsFilledL
// ---------------------------------------------------------
//
EXPORT_C TBool CApUtils::HasMandatoryFieldsFilledL( TUint32 aUid )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::HasMandatoryFieldsFilledL" ) ) );

    TBool retval( EFalse );
    TBool isOwn = ApCommons::StartPushedTransactionLC( *iDb );
    retval= DoHasMandatoryFieldsFilledL( aUid );
    if ( isOwn )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EUtil, 1, _L( "<- CApUtils::HasMandatoryFieldsFilledL" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// CApUtils::IAPExistsL
// ---------------------------------------------------------
//
EXPORT_C TBool CApUtils::IAPExistsL( TUint32 aUid )
    {
    CLOG( ( EUtil, 0, _L( "<-> CApUtils::IAPExistsL" ) ) );

    return DoesExistsL( TPtrC( IAP ), aUid );
    }


// ---------------------------------------------------------
// CApUtils::WapApExistsL
// ---------------------------------------------------------
//
EXPORT_C TBool CApUtils::WapApExistsL( TUint32 aUid )
    {
    CLOG( ( EUtil, 0, _L( "<-> CApUtils::WapApExistsL" ) ) );

    return DoesExistsL( TPtrC( WAP_ACCESS_POINT ), aUid );
    }



// ---------------------------------------------------------
// CApUtils::DoHasMandatoryFieldsFilledL
// ---------------------------------------------------------
//
EXPORT_C TBool CApUtils::DoHasMandatoryFieldsFilledL(
                                                  TUint32 aUid,
                                                  CCommsDbTableView* aWapt,
                                                  TApBearerType aBearertype )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::DoHasMandatoryFieldsFilledL" ) ) );
    
    TBool retval( EFalse );
    TInt need2pop( 0 );
    // if not contains, retval = False...
    // Mandatory WAP settings are:
    // CSD: StartPage, GatewayAddr
    // Note: SMS bearer had been dropped.
    // SMS: ServerNumber, ServiceNumber
    // Gprs: StartPage, GatewayAddr
    // Cdma: StartPage, GatewayAddr
    // Start with CSD/GPRS:

    if ( aBearertype == EApBearerTypeAllBearers )
        {
        aBearertype = BearerTypeL( aUid );
        }
    if ( aBearertype == EApBearerTypeAllBearers )
        {
        User::Leave( KErrInvalidBearerType );
        }

    if ( !aWapt )
        {
        aWapt = iDb->OpenViewMatchingUintLC( TPtrC(WAP_ACCESS_POINT ),
                                             TPtrC( COMMDB_ID ), aUid );
        need2pop++;
        TInt res = aWapt->GotoFirstRecord();
        if ( res != KErrNone )
            {
            aWapt = NULL;
            User::Leave( res );
            }
        }
    switch ( aBearertype )
        {
        case EApBearerTypeCSD:
        case EApBearerTypeHSCSD:
        case EApBearerTypeGPRS:
        case EApBearerTypeCDMA:
            {
            TInt aLength( 0 );
            aWapt->ReadColumnLengthL( TPtrC(WAP_START_PAGE), aLength );
            if ( aLength )
                {
                CCommsDbTableView* bearert = iDb->OpenViewMatchingUintLC(
                                                TPtrC(WAP_IP_BEARER),
                                                TPtrC(WAP_ACCESS_POINT_ID),
                                                aUid );
                User::LeaveIfError( bearert->GotoFirstRecord() );
                bearert->ReadColumnLengthL( TPtrC(WAP_GATEWAY_ADDRESS),
                                            aLength );
                if ( aLength )
                    {
                    HBufC* txtbuf = HBufC::NewLC( aLength );
                    TPtr16 txt = txtbuf->Des();
                    bearert->ReadTextL( TPtrC(WAP_GATEWAY_ADDRESS), txt );
                    if ( txt.Compare( KDynIpAddress ) )
                        {
                        retval = ETrue;
                        }
                    }
                CleanupStack::PopAndDestroy( 2, bearert ); // txtbuf,bearert
                }
            break;
            }
        default:
            {
            User::Leave( KErrInvalidBearerType );
            break;
            }
        }
    if ( need2pop )
        {
        CleanupStack::PopAndDestroy( need2pop );
        }

    CLOG( ( EUtil, 1, _L( "<- CApUtils::DoHasMandatoryFieldsFilledL" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// CApUtils::IsValidNameL
// ---------------------------------------------------------
//
EXPORT_C TBool CApUtils::IsValidNameL( const TDesC& aNameText,
                                       TUint32 aAcceptUid )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::IsValidNameL" ) ) );

    // Valid name: it means the name of the Connection,
    // that is the COMMDB_NAME in WAP_ACCESS_POINT table
    //
    TBool ret( ETrue ) ;
    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb, ETrue );

    HBufC* esc = ApCommons::EscapeTextLC( aNameText );
    CCommsDbTableView* wapt;
    wapt = iDb->OpenViewMatchingTextLC( TPtrC(WAP_ACCESS_POINT), 
                                        TPtrC(COMMDB_NAME), 
                                        *esc );

    TInt res;
    res = wapt->GotoFirstRecord(); // OK
    do
        {
        if ( res == KErrNone )
            {
            // Check UID
            TUint32 arecid;
            ApCommons::ReadUintL( wapt, TPtrC(COMMDB_ID), arecid);
            if ( arecid != aAcceptUid )
                {
                // some other record is already using this name,
                // need to change it
                ret = EFalse;
                }
            }
        else
            {
            if ( res == KErrNotFound )
                {
                ret = ETrue;
                }
            else
                {
                User::Leave( res );
                }
            }
        if ( ret && ( res == KErrNone ) )
            {
            res = wapt->GotoNextRecord();
            }
        }while ( ( res == KErrNone ) && ( ret ) );

    CleanupStack::PopAndDestroy( wapt ); // wapt
    CleanupStack::PopAndDestroy( esc ); // esc

    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EUtil, 1, _L( "<- CApUtils::IsValidNameL" ) ) );
    return ret;
    }


// ---------------------------------------------------------
// CApUtils::IsReadOnlyL
// ---------------------------------------------------------
//
EXPORT_C TBool CApUtils::IsReadOnlyL( TUint32 aUid )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::IsReadOnlyL" ) ) );
    
    TBool retval( EFalse );
    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb, ETrue );
    CCommsDbTableView* wapt;
    wapt = iDb->OpenViewMatchingUintLC( TPtrC(WAP_ACCESS_POINT),
                                       TPtrC(COMMDB_ID), aUid );
    User::LeaveIfError( wapt->GotoFirstRecord() );
    retval = ApCommons::DoGetReadOnlyL( *wapt );
    CleanupStack::PopAndDestroy( wapt ); // wapt
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EUtil, 1, _L( "<- CApUtils::IsReadOnlyL" ) ) );
    return retval;
    }




// ---------------------------------------------------------
// CApUtils::IsWlanL
// ---------------------------------------------------------
//
TBool CApUtils::IsWlanL( TUint32 aLanCommdbId )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::IsWlanL" ) ) );
    
    TBool retval( EFalse );
    TRAPD( err, retval = DoIsWlanL( aLanCommdbId ) );
    
    if ( err == KErrNotFound )
        {
        retval = EFalse;
        }
    else
        {
        User::LeaveIfError( err );
        }
    
    CLOG( ( EUtil, 1, _L( "<- CApUtils::IsWlanL" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// CApUtils::HasWlanSSIDL
// ---------------------------------------------------------
//
TBool CApUtils::HasWlanSSIDL( TUint32 aLanCommdbId )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::HasWlanSSIDL" ) ) );
    
    TBool retval( EFalse );
    CCommsDbTableView* wlantable = NULL;

    // WLAN settings table backlinks to LAN service, 
    // so we query WLAN settings for a record 
    // where WLAN_SERVICE_ID is LAN_SERVICE's COMMDB_ID, 
    // which is aApItem.iIapServiceId...
    wlantable = iDb->OpenViewMatchingUintLC( TPtrC(WLAN_SERVICE),
                                             TPtrC(WLAN_SERVICE_ID),
                                             aLanCommdbId );
    TInt err = wlantable->GotoFirstRecord();
    if ( err == KErrNotFound )
        {
        retval = EFalse;
        }
    else
        {
        User::LeaveIfError( err );
        HBufC* wlanssid = ApCommons::ReadText16ValueLC
                            ( wlantable, TPtrC(NU_WLAN_SSID) );
        if ( wlanssid->Compare( KEmpty ) != 0 )
            {            
            retval = ETrue;
            }
        CleanupStack::PopAndDestroy( wlanssid );
        }
    CleanupStack::PopAndDestroy( wlantable );
    
    CLOG( ( EUtil, 1, _L( "<- CApUtils::HasWlanSSIDL" ) ) );
    return retval;
    }


// ==== OTHER PRIVATE/protected, NON-EXPORTED FUNCTIONS ====

// ---------------------------------------------------------
// CApUtils::DoWapBearerTypeL
// ---------------------------------------------------------
//
void CApUtils::DoWapBearerTypeL( TUint32 aUid, TDes& aBearer )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::DoWapBearerTypeL" ) ) );
    
    CCommsDbTableView* wapt;
    wapt = iDb->OpenViewMatchingUintLC( TPtrC(WAP_ACCESS_POINT),
                                       TPtrC(COMMDB_ID), aUid );
    User::LeaveIfError( wapt->GotoFirstRecord() );
    HBufC* wapBearer = ApCommons::ReadText16ValueLC
                                            (
                                            wapt,
                                            TPtrC(WAP_CURRENT_BEARER)
                                            );
    if ( aBearer.MaxLength() < wapBearer->Length() )
        {
        User::Leave( KErrDescOverflow );
        }
    else
        {
        aBearer.SetLength( wapBearer->Length() );
        aBearer.Format( KFormat, &*wapBearer );
        }
    CleanupStack::PopAndDestroy( 2, wapt );   // wapBearer, wapt

    CLOG( ( EUtil, 1, _L( "<- CApUtils::DoWapBearerTypeL" ) ) );
    }



// ---------------------------------------------------------
// CApUtils::DoBearerTypeL
// ---------------------------------------------------------
//
TApBearerType CApUtils::DoBearerTypeL( TUint32 aUid )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::DoBearerTypeL" ) ) );
    
    TApBearerType retval = EApBearerTypeAllBearers;

    CCommsDbTableView* wapt;
    wapt = iDb->OpenViewMatchingUintLC( TPtrC(WAP_ACCESS_POINT), 
                                        TPtrC(COMMDB_ID), 
                                        aUid );
    
    // TRAPped for some record that doesn't have a WAP -table. 
    // If KErrNotFound, then just return EApBearerTypeWLAN
    TInt wapErr = wapt->GotoFirstRecord();
    if ( wapErr == KErrNotFound )
        {
        CleanupStack::PopAndDestroy( wapt );
        return EApBearerTypeWLAN;
        }
    else
    	{
    	User::LeaveIfError( wapErr );
    	}
    
    HBufC* wapBearer = ApCommons::ReadText16ValueLC
                                            (
                                            wapt,
                                            TPtrC(WAP_CURRENT_BEARER)
                                            );

    CCommsDbTableView* bearert = iDb->OpenViewMatchingUintLC( *wapBearer,
                                          TPtrC(WAP_ACCESS_POINT_ID), aUid );

    User::LeaveIfError( bearert->GotoFirstRecord() );
    // decide which bearer, read bearer spec. data.
    if ( *wapBearer == TPtrC(WAP_IP_BEARER) )
        {
        TUint32 wapIap;
        ApCommons::ReadUintL( bearert, TPtrC(WAP_IAP), wapIap );

        CCommsDbTableView* iapt = iDb->OpenViewMatchingUintLC( TPtrC(IAP),
                                               TPtrC(COMMDB_ID), wapIap );

        User::LeaveIfError( iapt->GotoFirstRecord() );
        HBufC* aIapServiceType = ApCommons::ReadText16ValueLC
                                        (
                                        iapt,
                                        TPtrC(IAP_SERVICE_TYPE)
                                        );
        if ( ( *aIapServiceType == TPtrC(DIAL_OUT_ISP) ) ||
             ( *aIapServiceType == TPtrC(DIAL_IN_ISP) ) )
            {
            // need to get 'speed' data ( HS or normal CSD )
            TUint32 wapIsp;
            ApCommons::ReadUintL( iapt, TPtrC(IAP_SERVICE), wapIsp );
            CCommsDbTableView* ispt = iDb->OpenViewMatchingUintLC(
                                            *aIapServiceType,
                                            TPtrC(COMMDB_ID),
                                            wapIsp );
            User::LeaveIfError( ispt->GotoFirstRecord() );
            TUint32 bear;
            if ( ApCommons::ReadUintL( ispt, TPtrC(ISP_BEARER_TYPE), bear )
                                  == KErrUnknown )
                {
                bear = EBearerTypeCSD;
                }
            if ( bear == EBearerTypeCSD )
                {
                retval = EApBearerTypeCSD;
                }
            else
                {
                if ( bear == EBearerTypeHSCSD )
                    {
                    retval = EApBearerTypeHSCSD;
                    }
                else
                    {
                    User::Leave( KErrInvalidBearer );
                    }
                }
            CleanupStack::PopAndDestroy( ispt ); // ispt
            }
        else
            {
            if ( ( *aIapServiceType == TPtrC(OUTGOING_WCDMA) ) ||
                 ( *aIapServiceType == TPtrC(INCOMING_WCDMA) ) )
                {
                retval = EApBearerTypeGPRS;
                }
            else
                { // some other bearer, currently it might be VPN or WLAN
                if ( *aIapServiceType == TPtrC(VPN_SERVICE) ) 
                    { // Get real VPN bearertype...
                    TUint32 service;
                    ApCommons::ReadUintL( iapt, TPtrC(IAP_SERVICE), service );

                    CCommsDbTableView* ispt;
                    ispt = iDb->OpenViewMatchingUintLC( TPtrC(VPN_SERVICE),
                                                        TPtrC(COMMDB_ID), 
                                                        service );
                    User::LeaveIfError( ispt->GotoFirstRecord() );
                    TUint32 tempint;
                    HBufC* buf = NULL;
                    // get the real iap
                    ApCommons::ReadUintL( ispt, 
                                          TPtrC(VPN_SERVICE_IAP), 
                                          tempint );

                    // now open the real IAP an get it's bearer...
                    CCommsDbTableView* iapt2 = NULL;
                    iapt2 = iDb->OpenViewMatchingUintLC( TPtrC(IAP),
                                                         TPtrC(COMMDB_ID), 
                                                         tempint );                                                         
                    User::LeaveIfError( iapt2->GotoFirstRecord() );

                    buf = ApCommons::ReadText16ValueLC( iapt2, 
                                                 TPtrC(IAP_SERVICE_TYPE) );
                    TUint32 service2;
                    ApCommons::ReadUintL( iapt2, TPtrC(IAP_SERVICE), 
                                          service2 );

                    if ( ( buf->Compare( TPtrC(OUTGOING_WCDMA ) ) == 0 ) ||
                        ( buf->Compare( TPtrC(INCOMING_WCDMA ) ) == 0 ) )
                        {
                        retval = EApBearerTypeGPRS;
                        }
                    else
                        {
                        if ( ( buf->Compare( TPtrC(DIAL_OUT_ISP ) ) == 0 ) ||
                            ( buf->Compare( TPtrC(DIAL_IN_ISP ) ) == 0 ) )
                            {
                            // need to get ISP,
                            CCommsDbTableView* doutt = NULL;
                            doutt = iDb->OpenViewMatchingUintLC( 
                                            *buf,
                                            TPtrC(COMMDB_ID), 
                                            service2 );
                                            
                            User::LeaveIfError( doutt ->GotoFirstRecord() );
                            ApCommons::ReadUintL( doutt , 
                                                  TPtrC(ISP_BEARER_TYPE), 
                                                  tempint );
                            if ( tempint == EBearerTypeCSD )
                                {
                                retval  = EApBearerTypeCSD;
                                }
                            else
                                {
                                retval = EApBearerTypeHSCSD;
                                }
                            CleanupStack::PopAndDestroy( doutt ); // doutt
                            }
                        else
                            { 
                            // VPN, having a bearer something 
                            // else than CSD, HSCSD & GPRS?
                            // check for WLAN
                            if ( iExt->iWlanSupported )
                                {
                                if ( buf->Compare( TPtrC(LAN_SERVICE) ) == 0 )
                                    { // LAN or WLAN
                                    if ( IsWlanL( service2 ) )
                                        {
                                        retval = EApBearerTypeWLAN;
                                        }
                                    else
                                        {
                                        User::Leave( KErrInvalidBearer );
                                        }
                                    }
                                else
                                    {
                                    User::Leave( KErrInvalidBearer );
                                    }
                                }
                            else
                                {
                                User::Leave( KErrInvalidBearer );
                                }
                            }
                        }
                    CleanupStack::PopAndDestroy( 3, ispt ); // buf, iapt2, ispt
                    }
                else
                    { // other, but not VPN, check for WLAN
                    if ( iExt->iWlanSupported )
                        {
                        if ( *aIapServiceType == TPtrC(LAN_SERVICE) ) 
                            { // check existence...
                            TUint32 service;
                            TUint32 bearer;
                            ApCommons::ReadUintL( iapt, TPtrC(IAP_SERVICE), 
                                                  service );
                            ApCommons::ReadUintL( iapt, TPtrC(IAP_BEARER), 
                                                  bearer );


                            CCommsDbTableView* ispt;
                            ispt = iDb->OpenViewMatchingUintLC( 
                                                    TPtrC(LAN_SERVICE),
                                                    TPtrC(COMMDB_ID), 
                                                    service );
                            User::LeaveIfError( ispt->GotoFirstRecord() );
                            // now we know it is either WLAN or some kinda LAN
                            if ( IsWlanL( service ) )
                                {
                                retval = EApBearerTypeWLAN;
                                }
                            else
                                { // other, 
                                // check for LANModem first, then for LAN
                                CCommsDbTableView* lanbearer;
                                lanbearer = 
                                    iDb->OpenViewMatchingUintLC( 
                                                TPtrC(LAN_BEARER),
                                                TPtrC(COMMDB_ID), 
                                                bearer );
                                User::LeaveIfError( 
                                        lanbearer->GotoFirstRecord() );
                                // LANMODEM? check commdb_name
                                HBufC* lbuf;
                                lbuf = ApCommons::ReadText16ValueLC( lanbearer,
                                                         TPtrC(COMMDB_NAME) );
                                // if it is KModemBearerLANModem, 
                                // it is LANModem
                                if ( *lbuf == KModemBearerLANModem )
                                    {
                                    retval = EApBearerTypeLANModem;
                                    }
                                else
                                    {
    #ifdef __TEST_LAN_BEARER
                                    // LAN?
                                    if ( *lbuf == KModemBearerLAN )
                                        {                                
                                        retval = EApBearerTypeLAN;
                                        }
                                    else
                                        {
                                        // other, but not VPN, or not supported
                                        User::Leave( KErrInvalidBearer );
                                        }
    #else
                                // other, but not VPN, or not supported
                                User::Leave( KErrInvalidBearer );
    #endif // __TEST_LAN_BEARER           
                                    }
                                CleanupStack::PopAndDestroy( lbuf );
                                CleanupStack::PopAndDestroy( lanbearer );
                                }
                            CleanupStack::PopAndDestroy( ispt );
                            }
                        }
                    else
                        {                        
    #ifdef __TEST_LAN_BEARER
                        if ( *aIapServiceType == TPtrC(LAN_SERVICE) ) 
                            { // check existence...
                            TUint32 service;
                            TUint32 bearer;
                            ApCommons::ReadUintL( iapt, TPtrC(IAP_SERVICE), 
                                                  service );
                            ApCommons::ReadUintL( iapt, TPtrC(IAP_BEARER), 
                                                  bearer );

                            CCommsDbTableView* ispt;
                            ispt = iDb->OpenViewMatchingUintLC( 
                                                    TPtrC(LAN_SERVICE),
                                                    TPtrC(COMMDB_ID), 
                                                    service );
                            User::LeaveIfError( ispt->GotoFirstRecord() );
                            // now we know it is some kinda LAN
                            // check for LANModem first, then for LAN
                            CCommsDbTableView* lanbearer;
                            lanbearer = 
                                iDb->OpenViewMatchingUintLC( 
                                            TPtrC(LAN_BEARER),
                                            TPtrC(COMMDB_ID), 
                                            bearer );
                            User::LeaveIfError( 
                                    lanbearer->GotoFirstRecord() );
                            // LANMODEM? check commdb_name
                            HBufC* lbuf;
                            lbuf = ApCommons::ReadText16ValueLC( lanbearer,
                                                         TPtrC(COMMDB_NAME) );
                            // if it is KModemBearerLANModem, it is LANModem
                            if ( *lbuf == KModemBearerLANModem )
                                {
                                retval = EApBearerTypeLANModem;
                                }
                            else
                                {
                                // LAN?
                                if ( *lbuf == KModemBearerLANModem )
                                    {                                
                                    retval = EApBearerTypeLAN;
                                    }
                                else
                                    {
                                    // other, but not VPN, or not supported
                                    User::Leave( KErrInvalidBearer );
                                    }
                                }
                            CleanupStack::PopAndDestroy( lbuf );
                            CleanupStack::PopAndDestroy( lanbearer );
                            CleanupStack::PopAndDestroy( ispt );
                            }
                        else
                            {// other, but not LAN
                            User::Leave( KErrInvalidBearer );
                            }
    #endif // __TEST_LAN_BEARER 
                        }
                    }
                }
            }
        CleanupStack::PopAndDestroy( 2, iapt );// aIapServiceType, iapt
        }
    else
        {
        User::Leave( KErrInvalidBearer );
        }

    CleanupStack::PopAndDestroy( bearert ); // bearert
    CleanupStack::Pop( wapBearer );    // wapBearer,
    // cannot PopAndDestroy because it has been re-allocated !
    delete wapBearer;
    CleanupStack::PopAndDestroy( wapt );    // wapt

    if ( retval == EApBearerTypeAllBearers )
        {
        User::Leave( KErrInvalidBearer );
        }

    CLOG( ( EUtil, 1, _L( "<- CApUtils::DoBearerTypeL" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// CApUtils::DoISPTypeL
// ---------------------------------------------------------
//
void CApUtils::DoISPTypeL( TUint32 aUid, TCommsDbIspType& aIspType )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::DoISPTypeL" ) ) );
    
    TCommsDbIspType retval = EIspTypeInternetAndWAP;

    CCommsDbTableView* wapt;
    wapt = iDb->OpenViewMatchingUintLC( TPtrC(WAP_ACCESS_POINT), 
                                        TPtrC(COMMDB_ID), 
                                        aUid );

    User::LeaveIfError( wapt->GotoFirstRecord() );
    HBufC* wapBearer = ApCommons::ReadText16ValueLC
                                        (
                                        wapt,
                                        TPtrC(WAP_CURRENT_BEARER)
                                        );

    if ( *wapBearer == TPtrC(WAP_IP_BEARER) )
        {
        CCommsDbTableView* bearert;
        bearert = iDb->OpenViewMatchingUintLC( TPtrC(*wapBearer), 
                                               TPtrC(WAP_ACCESS_POINT_ID), 
                                               aUid );

        User::LeaveIfError( bearert->GotoFirstRecord() );
        TUint32 aWapIap;
        ApCommons::ReadUintL( bearert, TPtrC(WAP_IAP), aWapIap );
        // now we have IAP uid, can go for Service type, ID

        CCommsDbTableView* iapt;
        iapt = iDb->OpenViewMatchingUintLC( TPtrC(IAP), 
                                            TPtrC(COMMDB_ID), 
                                            aWapIap );

        User::LeaveIfError( iapt->GotoFirstRecord() );
        HBufC* aIapServiceType = ApCommons::ReadText16ValueLC
                                    (
                                    iapt,
                                    TPtrC(IAP_SERVICE_TYPE)
                                    );
        // decide which bearer, read bearer spec. data.
        TUint32 aIsp;
        ApCommons::ReadUintL( iapt, TPtrC(IAP_SERVICE), aIsp );

        // No need to be defensive, if *aIapServiceType is invalid,
        // it shall Leave,
        // otherwise, we will handle the difference...
        CCommsDbTableView* ispt = iDb->OpenViewMatchingUintLC(
                                        *aIapServiceType,
                                        TPtrC(COMMDB_ID), aIsp );
        User::LeaveIfError( ispt->GotoFirstRecord() );
        TUint32 sgd;
        if ( ( *aIapServiceType == TPtrC(DIAL_OUT_ISP) ) ||
             ( *aIapServiceType == TPtrC(DIAL_IN_ISP) ) )
            { // get real IspType
            ApCommons::ReadUintL( ispt, TPtrC(ISP_TYPE), sgd );
            }
        else
            {
            // OUTGOING_WCDMA/INCOMING_WCDMA
            // read ISP type
            ApCommons::ReadUintL( ispt, TPtrC(GPRS_AP_TYPE), sgd );
            }
        retval = TCommsDbIspType( sgd );
        CleanupStack::PopAndDestroy( ispt ); // ispt
        CleanupStack::Pop( aIapServiceType );// aIapServiceType,
        // cannot PopAndDestroy because it has been re-allocated !
        delete aIapServiceType;
        CleanupStack::PopAndDestroy( 2, bearert ); // iapt, bearert
        }
    else
        {
        User::Leave( KErrInvalidBearer );
        }

    CleanupStack::Pop( wapBearer );    // wapBearer,
    // cannot PopAndDestroy because it has been re-allocated !
    delete wapBearer;
    CleanupStack::PopAndDestroy( wapt );    // wapt
    aIspType = retval;

    CLOG( ( EUtil, 1, _L( "<- CApUtils::DoISPTypeL" ) ) );
    }


// ---------------------------------------------------------
// CApUtils::DoNameL
// ---------------------------------------------------------
//
void CApUtils::DoNameL( TUint32 aUid, TDes& aName )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::DoNameL" ) ) );

    CCommsDbTableView* wapt = iDb->OpenViewMatchingUintLC(
                                    TPtrC(WAP_ACCESS_POINT),
                                    TPtrC(COMMDB_ID), aUid );
    User::LeaveIfError( wapt->GotoFirstRecord() );
    HBufC* sgd = ApCommons::ReadText16ValueLC( wapt, TPtrC(COMMDB_NAME) );

    if ( aName.MaxLength() < sgd->Length()  )
        {
        User::Leave( KErrDescOverflow );
        }
    else
        {
        aName.SetLength( sgd->Length() );
        aName.Format( KFormat, &*sgd );
        }
    CleanupStack::PopAndDestroy( 2, wapt );   //  sgd, wapt

    CLOG( ( EUtil, 1, _L( "<- CApUtils::DoNameL" ) ) );
    }


// ---------------------------------------------------------
// CApUtils::DoSetNameL
// ---------------------------------------------------------
//
void CApUtils::DoSetNameL( TDes& aName, TUint32 aUid )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::DoSetNameL" ) ) );

    if ( iExt->iVariant & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }

    CCommsDbTableView* wapt;

    wapt = iDb->OpenViewMatchingUintLC( TPtrC(WAP_ACCESS_POINT),
                                       TPtrC(COMMDB_ID), aUid );
    User::LeaveIfError( wapt->GotoFirstRecord() );
    HBufC* aCurrent = ApCommons::ReadText16ValueLC( wapt,
                                                     TPtrC(COMMDB_NAME) );
    if ( *aCurrent != aName )
        {
        TBool aNameChanged( EFalse );
        HBufC* namebuf = aName.AllocL();
        CleanupStack::PushL( namebuf );
        HBufC* sgd = ApCommons::DoMakeValidNameL
                                        (
                                        *iDb,
                                        aNameChanged,
                                        namebuf,
                                        aUid,
                                        EFalse
                                        );
        if ( aNameChanged )
            {
            aName.Copy( *sgd );
            delete sgd;
            }
        CleanupStack::PopAndDestroy( namebuf ); // namebuf
        User::LeaveIfError( wapt->UpdateRecord() );
        wapt->WriteTextL( TPtrC(COMMDB_NAME), aName );
        User::LeaveIfError( wapt->PutRecordChanges() );
        }
    CleanupStack::PopAndDestroy( 2, wapt );   // aCurrent,wapt

    CLOG( ( EUtil, 1, _L( "<- CApUtils::DoSetNameL" ) ) );
    }


// ---------------------------------------------------------
// CApUtils::DoSetISPTypeL
// ---------------------------------------------------------
//
void CApUtils::DoSetISPTypeL( TUint32 aUid, TCommsDbIspType aIspType )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::DoSetISPTypeL" ) ) );

    if ( iExt->iVariant & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }

    // first check requested ISP-type validity
    if ( ( aIspType == EIspTypeInternetOnly ) ||
        ( aIspType == EIspTypeWAPOnly ) ||
        ( aIspType == EIspTypeInternetAndWAP ) )
        {
        CCommsDbTableView* wapt;
        wapt = iDb->OpenViewMatchingUintLC( TPtrC(WAP_ACCESS_POINT),
                                           TPtrC(COMMDB_ID), aUid );
        User::LeaveIfError( wapt->GotoFirstRecord() );
        HBufC* wapBearer = ApCommons::ReadText16ValueLC
                                            (
                                            wapt,
                                            TPtrC(WAP_CURRENT_BEARER)
                                            );
        if ( *wapBearer == TPtrC(WAP_IP_BEARER) )
            {
            CCommsDbTableView* bearert;
            bearert = iDb->OpenViewMatchingUintLC(
                            *wapBearer,
                            TPtrC(WAP_ACCESS_POINT_ID), aUid );

            User::LeaveIfError( bearert->GotoFirstRecord() );
            TUint32 aWapIap;
            ApCommons::ReadUintL( bearert, TPtrC(WAP_IAP), aWapIap );
            // now we have IAP uid, can go for Service type, ID

            CCommsDbTableView* iapt;
            iapt = iDb->OpenViewMatchingUintLC( TPtrC(IAP),
                                                TPtrC(COMMDB_ID),
                                                aWapIap );
            User::LeaveIfError( iapt->GotoFirstRecord() );

            HBufC* aIapServiceType = ApCommons::ReadText16ValueLC
                                            (
                                            iapt,
                                            TPtrC(IAP_SERVICE_TYPE)
                                            );

            // get ISP ID
            TUint32 aIsp;
            ApCommons::ReadUintL( iapt, TPtrC(IAP_SERVICE), aIsp );

            CCommsDbTableView* ispt;
            ispt = iDb->OpenViewMatchingUintLC( *aIapServiceType,
                                                TPtrC(COMMDB_ID), aIsp );
            User::LeaveIfError( ispt->GotoFirstRecord() );
            // now we have it, just write:
            User::LeaveIfError( ispt->UpdateRecord() );
            // decide which bearer, write into bearer spec. column.
            if ( ( *aIapServiceType == TPtrC(DIAL_OUT_ISP) ) ||
                 ( *aIapServiceType == TPtrC(DIAL_IN_ISP) ) )
                { // write real IspType
                ispt->WriteUintL( TPtrC(ISP_TYPE), aIspType );
                }
            else
                {
                // OUTGOING_WCDMA / INCOMING_WCDMA
                ispt->WriteUintL( TPtrC(GPRS_AP_TYPE), aIspType );
                }
            User::LeaveIfError( ispt->PutRecordChanges( EFalse, EFalse ) );
            // ispt, aIapServiceType, iapt, bearert
            CleanupStack::PopAndDestroy( 4, bearert );
            }
        else
            {
            User::Leave( KErrInvalidBearer );
            }
        CleanupStack::PopAndDestroy( 2, wapt );    // wapBearer, wapt
        }
    else
        {
        User::Leave( KErrInvalidIspRequest );
        }

    CLOG( ( EUtil, 1, _L( "<- CApUtils::DoSetISPTypeL" ) ) );
    }


// ---------------------------------------------------------
// CApUtils::DoesExistsL
// ---------------------------------------------------------
//
TBool CApUtils::DoesExistsL( TPtrC aTable, TUint32 aUid )
    {
    CLOG( ( EUtil, 0, _L( "-> CApUtils::DoesExistsL" ) ) );
    
    TBool retval( EFalse );
    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb, ETrue );
    CCommsDbTableView* table;
    table = iDb->OpenViewMatchingUintLC( aTable,
                                       TPtrC(COMMDB_ID), aUid );
    TInt res = table->GotoFirstRecord();
    CleanupStack::PopAndDestroy( table ); // table
    if ( res == KErrNone )
        {
        retval = ETrue;
        }
    if ( res != KErrNotFound )
        {
        User::LeaveIfError( res );
        }
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EUtil, 1, _L( "<- CApUtils::DoesExistsL" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// CApUtils::DoIsWlanL
// ---------------------------------------------------------
//
TBool CApUtils::DoIsWlanL( TUint32 aLanCommdbId )
    {
    CLOG( ( EUtil, 1, _L( "-> CApUtils::DoIsWlanL" ) ) );
    
    TBool retval( EFalse );
    
    CCommsDbTableView* wlantable = NULL;

    // WLAN settings table backlinks to LAN service, 
    // so we query WLAN settings for a record 
    // where WLAN_SERVICE_ID is LAN_SERVICE's COMMDB_ID, 
    // which is aApItem.iIapServiceId...
    wlantable = iDb->OpenViewMatchingUintLC( TPtrC(WLAN_SERVICE),
                                             TPtrC(WLAN_SERVICE_ID),
                                             aLanCommdbId );
                                             
    TInt err = wlantable->GotoFirstRecord();        
    if ( err == KErrNotFound )
        {
        retval = EFalse;
        }
    else
        {
        User::LeaveIfError( err );
        retval = ETrue;
        }

    CleanupStack::PopAndDestroy( wlantable );
    
    CLOG( ( EUtil, 1, _L( "<- CApUtils::DoIsWlanL" ) ) );
    return retval;
    }

//  End of File
