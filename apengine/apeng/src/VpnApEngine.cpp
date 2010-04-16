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
* Description:  Declaration of the CVpnApEngine class.
*
*/


// INCLUDE FILES

#include    "VpnApEngine.h"
#include    "VpnApItem.h"
#include    "ApEngineLogger.h"
#include    "ApEngineConsts.h"
#include    "ApEngineCommons.h"

#include    <ApDataHandler.h>
#include    <ApAccessPointItem.h>
#include    <ApUtils.h>

#include    <txtetext.h>

#include    <vpnapi.h>
#include    <vpnapidefs.h>


// CONSTANTS
_LIT( KVpnVirtualBearerName, "vpnbearer" );
_LIT( KVpnVirtualBearerAgent, "vpnconnagt.agt" );
_LIT( KVpnVirtualBearerNif, "tunnelnif" );
_LIT( KDefProxyProtocolName, "http" );                      // required !
_LIT( KDefWapGatewayIpAddress, "0.0.0.0" );



// constatnts needed for the virtualbearer table
const TInt KNoTimeout = -1;
const TUint32 KLastSessionTimeout = 3;


// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CVpnApEngine::NewLC
// ---------------------------------------------------------
//
EXPORT_C CVpnApEngine* CVpnApEngine::NewLC( CCommsDatabase* aDb )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApEngine::NewL" ) ) );

    CVpnApEngine* self = new ( ELeave ) CVpnApEngine;
    CleanupStack::PushL( self );
    self->ConstructL( aDb );

    CLOG( ( EApItem, 1, _L( "<- CVpnApEngine::NewL" ) ) );
    
    return self;
    }


// ---------------------------------------------------------
// CVpnApEngine::~CVpnApEngine
// ---------------------------------------------------------
//
EXPORT_C CVpnApEngine::~CVpnApEngine()
    {
    if ( iOwnDb )
        {
        delete iDb;
        }

    CLOG( ( EApItem, 0, _L( "<-> CVpnApEngine::~CVpnApEngine" ) ) );
    }

        
// ---------------------------------------------------------
// CVpnApEngine::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CVpnApEngine::ConstructL( CCommsDatabase* aDb )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApEngine::ConstructL" ) ) );

    if ( aDb )
        {
        iDb = aDb;
        iOwnDb = EFalse;
        }
    else
        {
        iDb = CCommsDatabase::NewL();
        iOwnDb = ETrue;
        }

    CLOG( ( EApItem, 0, _L( "<- CVpnApEngine::ConstructL" ) ) );
    }


// ---------------------------------------------------------
// CVpnApEngine::CVpnApEngine
// ---------------------------------------------------------
//
EXPORT_C CVpnApEngine::CVpnApEngine()
:iOwnDb( EFalse )
    {
    CLOG( ( EApItem, 0, _L( "<-> CVpnApEngine::CVpnApEngine" ) ) );

    }



// ---------------------------------------------------------
// CVpnApEngine::CreateVpnApL
// ---------------------------------------------------------
//
EXPORT_C TUint32 CVpnApEngine::CreateVpnApL( CVpnApItem& aVpnApItem )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApEngine::CreateVpnApL" ) ) );

    TUint32 retval( 0 );

    // Start  a transaction
    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    
    retval = DoCreateVpnApL( ETrue, aVpnApItem );

    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }



    CLOG( ( EApItem, 1, _L( "<- CVpnApEngine::CreateVpnApL" ) ) );

    return retval;
    }





// ---------------------------------------------------------
// CVpnApEngine::CreateVpnL
// ---------------------------------------------------------
//
EXPORT_C TUint32 CVpnApEngine::CreateVpnApL(  TDesC& aName, 
                                              TUint32 aIapId, 
                                              TDesC& aPolicy, 
                                              TDesC* aProxyServerAddress, 
                                              TUint32 aProxyPort )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApEngine::CreateVpnApL..." ) ) );
    TUint32 retval( 0 );

    CVpnApItem* item = CVpnApItem::NewLC();

    item->WriteTextL( EApVpnWapAccessPointName, aName );
    item->WriteUint( EApVpnRealIapID, aIapId );
    item->WriteTextL( EApVpnPolicyId, aPolicy );
    item->WriteLongTextL( EApVpnProxyServerAddress, *aProxyServerAddress );
    item->WriteUint( EApVpnProxyPortNumber, aProxyPort );
    
    
    RVpnServ ps;
    User::LeaveIfError( ps.Connect() );
    CleanupClosePushL<RVpnServ>( ps );
    
    TVpnPolicyDetails det;

    TVpnPolicyName pName( aPolicy );

    TInt err = ps.GetPolicyDetails( pName, det );
   
    // When the policy is deleted (err!=0):
    // have this leave, in UI display a note about needing to be reconfigured
    // & set it & policyID to none to look like there is NO policy
    // which will force the reconfiguration if in the UI
    User::LeaveIfError( err );

    item->WriteTextL( EApVpnPolicy, det.iName );

    CreateVpnApL( *item );

    CleanupStack::PopAndDestroy(); // ps

    CleanupStack::PopAndDestroy( item );

    CLOG( ( EApItem, 1, _L( "<- CVpnApEngine::CreateVpnApL..." ) ) );

    return retval;
    }




// ---------------------------------------------------------
// CVpnApEngine::DeleteVpnL
// ---------------------------------------------------------
//
EXPORT_C void CVpnApEngine::DeleteVpnL( TUint32 aVpnApId )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApEngine::DeleteVpnL" ) ) );

    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    DoDeleteVpnL( aVpnApId );
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EApItem, 1, _L( "<- CVpnApEngine::DeleteVpnL" ) ) );
    }






// ---------------------------------------------------------
// CVpnApEngine::VpnDataL
// ---------------------------------------------------------
//
EXPORT_C void CVpnApEngine::VpnDataL( TUint32 aUid, 
                                      CVpnApItem& aVpnApItem )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApEngine::VpnDataL" ) ) );

    ReadVpnWapBearerDataL( aUid, aVpnApItem );
    ReadVpnWapApDataL( aVpnApItem );
    ReadVpnIapRecordL( aVpnApItem );
    ReadNetworkDataL( aVpnApItem );
    ReadVpnDataL( aVpnApItem );
    ReadProxyDataL( aVpnApItem );

    CLOG( ( EApItem, 1, _L( "<- CVpnApEngine::VpnDataL" ) ) );
    }




// ---------------------------------------------------------
// CVpnApEngine::UpdateVpnDataL
// ---------------------------------------------------------
//
EXPORT_C void CVpnApEngine::UpdateVpnDataL( CVpnApItem& aVpnApItem )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApEngine::UpdateVpnDataL" ) ) );

    // Start  a transaction
    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    
    // ignore return value as it is used only when creating new
    DoCreateVpnApL( EFalse, aVpnApItem );

    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EApItem, 1, _L( "<- CVpnApEngine::UpdateVpnDataL" ) ) );
    }




// ---------------------------------------------------------
// CVpnApEngine::IsVpnApL()
// ---------------------------------------------------------
//
EXPORT_C TBool CVpnApEngine::IsVpnApL( const TUint32 aUid )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApEngine::IsVpnApL" ) ) );

    TBool retval( EFalse );
    // Start  a transaction
    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *iDb );
    
    CCommsDbTableView* wapt;
    wapt = iDb->OpenViewMatchingUintLC( TPtrC(WAP_IP_BEARER),
                                       TPtrC(WAP_ACCESS_POINT_ID), aUid );
    User::LeaveIfError( wapt->GotoFirstRecord() );

    TUint32 iapid( 0 );
    ApCommons::ReadUintL( wapt, TPtrC(WAP_IAP), iapid );
    

    
    CCommsDbTableView* iapt;
    iapt = iDb->OpenViewMatchingUintLC( TPtrC(IAP),
                                       TPtrC(COMMDB_ID), iapid );
    User::LeaveIfError( iapt->GotoFirstRecord() );

    HBufC* iapServiceType = ApCommons::ReadText16ValueLC
                                    (
                                    iapt,
                                    TPtrC(IAP_SERVICE_TYPE)
                                    );

    retval = ( iapServiceType->Compare( TPtrC(VPN_SERVICE) ) == 0 );

    CleanupStack::PopAndDestroy( iapServiceType ); // iapServiceType 

    CleanupStack::PopAndDestroy( iapt ); // iapt
    
    CleanupStack::PopAndDestroy( wapt ); // wapt

    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    CLOG( ( EApItem, 1, _L( "<- CVpnApEngine::IsVpnApL" ) ) );

    return retval;
    }



// ---------------------------------------------------------
// CVpnApEngine::Cvt()
// ---------------------------------------------------------
//
EXPORT_C TInt CVpnApEngine::Cvt()
    {
    return KErrNone;
    }




// ---------------------------------------------------------
// CVpnApEngine::DoCreateVpnApL
// ---------------------------------------------------------
//
TUint32 CVpnApEngine::DoCreateVpnApL( TBool aIsNew, CVpnApItem& aVpnApItem )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApEngine::DoCreateVpnApL" ) ) );

    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    // create 1 single virtual bearer if not already present
    // Create a new VPN access point (WAP, etc. with VPN IAP
    // Create a new VPN record in VPN table
    
    CApUtils* util = CApUtils::NewLC( *iDb );        
    TBool valid = 
            util->IsValidNameL( *aVpnApItem.iName, aVpnApItem.iVpnWapId );
    CleanupStack::PopAndDestroy( util );

    if ( !valid )
        {
        User::Leave( KErrAlreadyExists );
        }

    WriteVpnDataL( aIsNew, aVpnApItem );
    TUint32 virtbearerid = CreateVirtBearerIfNeededL();
    aVpnApItem.iVirtualBearerID = virtbearerid;
    WriteNetworkDataL( aIsNew, aVpnApItem );
    WriteVpnIapRecordL( aIsNew, aVpnApItem );
 
    WriteVpnWapApDataL( aIsNew, aVpnApItem );
    WriteVpnWapBearerDataL( aIsNew, aVpnApItem );
    if ( aVpnApItem.HasProxy() )
        {
        WriteProxyDataL( aIsNew, aVpnApItem );
        }
    else
        {
        if ( !aIsNew )
            {
            CCommsDbTableView* iapt;
            iapt = iDb->OpenViewMatchingUintLC( TPtrC(IAP), TPtrC(COMMDB_ID),
                                                aVpnApItem.iVpnIapId );

            User::LeaveIfError( iapt->GotoFirstRecord() );
            HBufC* servicetype = HBufC::NewLC( KCommsDbSvrMaxFieldLength );
            TPtr servptr( servicetype->Des() );
            iapt->ReadTextL( TPtrC(IAP_SERVICE_TYPE), servptr );

            TRAP_IGNORE( RemoveVpnProxyDataL( aVpnApItem.iVpnTableId ) );

            CleanupStack::PopAndDestroy( servicetype );
            CleanupStack::PopAndDestroy( iapt );
            }
        }
    
    CLOG( ( EApItem, 0, _L( "<- CVpnApEngine::DoCreateVpnApL" ) ) );

    return aVpnApItem.iVpnWapId;
    }


// ---------------------------------------------------------
// CVpnApEngine::WriteVpnDataL
// ---------------------------------------------------------
//
void CVpnApEngine::WriteVpnDataL( TBool aIsNew, CVpnApItem& aVpnApItem )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApEngine::WriteVpnDataL" ) ) );

    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CCommsDbTableView* t;

    if ( aIsNew )
        {
        t = iDb->OpenTableLC( TPtrC(VPN_SERVICE) );
        TInt err = t->InsertRecord( aVpnApItem.iVpnTableId );
        if ( err )
            {
            User::LeaveIfError( err );
            }
        }
    else
        {
        t = iDb->OpenViewMatchingUintLC( TPtrC(VPN_SERVICE),
                                         TPtrC(COMMDB_ID), 
                                         aVpnApItem.iVpnTableId );
        User::LeaveIfError( t->GotoFirstRecord() );
        User::LeaveIfError( t->UpdateRecord() );
        }

    t->WriteTextL( TPtrC(COMMDB_NAME), *aVpnApItem.iName );
    
    t->WriteTextL( TPtrC(VPN_SERVICE_POLICY), *aVpnApItem.iPolicyId );

    t->WriteUintL( TPtrC(VPN_SERVICE_IAP), aVpnApItem.iRealIapId );

    t->WriteUintL( TPtrC(VPN_SERVICE_NETWORKID), aVpnApItem.iHomeNetwork );

    User::LeaveIfError( t->PutRecordChanges( EFalse, EFalse ) );

    CleanupStack::PopAndDestroy( t );   // t

    CLOG( ( EApItem, 0, _L( "<- CVpnApEngine::WriteVpnDataL" ) ) );

    }




// ---------------------------------------------------------
// CVpnApEngine::CreateVirtBearerIfNeededL
// ---------------------------------------------------------
//
TUint32 CVpnApEngine::CreateVirtBearerIfNeededL()
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApEngine::CreateVirtBearerIfNeededL" ) ) );
    
    TUint32 retval( 0 );

    CCommsDbTableView* t = NULL;

    t = iDb->OpenViewMatchingTextLC( TPtrC(VIRTUAL_BEARER), 
                                     TPtrC(VIRTUAL_BEARER_AGENT), 
                                     KVpnVirtualBearerAgent );
    if ( t->GotoFirstRecord() == KErrNotFound )
        { // no such yet, create one
        TInt err = t->InsertRecord( retval );
        if ( err )
            {
            User::LeaveIfError( err );
            }        
        t->WriteTextL( TPtrC(COMMDB_NAME), KVpnVirtualBearerName );
        t->WriteTextL( TPtrC(VIRTUAL_BEARER_AGENT), KVpnVirtualBearerAgent );
        t->WriteTextL( TPtrC(VIRTUAL_BEARER_NIF), KVpnVirtualBearerNif );

        t->WriteUintL( TPtrC(LAST_SOCKET_ACTIVITY_TIMEOUT), 
                       (TUint32)KNoTimeout ); 
        t->WriteUintL( TPtrC(LAST_SESSION_CLOSED_TIMEOUT), 
                       KLastSessionTimeout ); 
        t->WriteUintL( TPtrC(LAST_SOCKET_CLOSED_TIMEOUT), 
                       (TUint32)KNoTimeout ); 

        User::LeaveIfError( t->PutRecordChanges( EFalse, EFalse ) );

        }
    else
        { // there is one, use it
        t->ReadUintL( TPtrC(COMMDB_ID), retval );
        }

    CleanupStack::PopAndDestroy( t );   // t
    
    CLOG( ( EApItem, 0, _L( "<- CVpnApEngine::CreateVirtBearerIfNeededL" ) ) );

    return retval;
    }



// ---------------------------------------------------------
// CVpnApEngine::WriteVpnIapRecordL
// ---------------------------------------------------------
//
TUint32 CVpnApEngine::WriteVpnIapRecordL( TBool aIsNew, 
                                          CVpnApItem& aVpnApItem )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApEngine::WriteVpnIapRecordL" ) ) );

    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CCommsDbTableView* t;

    if ( aIsNew )
        {
        t = iDb->OpenTableLC( TPtrC(IAP) );
        TInt err = t->InsertRecord( aVpnApItem.iVpnIapId );
        if ( err )
            {
            User::LeaveIfError( err );
            }
        }
    else
        {
        t = iDb->OpenViewMatchingUintLC( TPtrC(IAP),
                                         TPtrC(COMMDB_ID), 
                                         aVpnApItem.iVpnIapId );
        User::LeaveIfError( t->GotoFirstRecord() );
        User::LeaveIfError( t->UpdateRecord() );
        }

    t->WriteTextL( TPtrC(COMMDB_NAME), *aVpnApItem.iName );

    t->WriteUintL( TPtrC(IAP_SERVICE), aVpnApItem.iVpnTableId );
    t->WriteTextL( TPtrC(IAP_SERVICE_TYPE), TPtrC(VPN_SERVICE) );

    // hardcoded...
    t->WriteTextL( TPtrC(IAP_BEARER_TYPE), TPtrC(VIRTUAL_BEARER) );
    t->WriteUintL( TPtrC(IAP_BEARER), aVpnApItem.iVirtualBearerID );

    t->WriteUintL( TPtrC(IAP_NETWORK), aVpnApItem.iVpnIapNetwork );

    // Network weighting hardcoded to 0!
    t->WriteUintL( TPtrC(IAP_NETWORK_WEIGHTING), 0 );
    
    TUint32 loc = GetLocationIdL();
    t->WriteUintL( TPtrC(IAP_LOCATION), loc );

    User::LeaveIfError( t->PutRecordChanges( EFalse, EFalse ) );

    CleanupStack::PopAndDestroy( t );   // t

   
    CLOG( ( EApItem, 0, _L( "<- CVpnApEngine::WriteVpnIapRecordL" ) ) );

    return aVpnApItem.iVpnIapId;
    }




// ---------------------------------------------------------
// CVpnApEngine::GetLocationIdL
// ---------------------------------------------------------
//
TUint32 CVpnApEngine::GetLocationIdL()
    {
    // agreed that the location record having the name 'Mobile' is used
    CLOG( ( EHandler, 0, _L( "-> CVpnApEngine::GetLocationIdL" ) ) );

    CCommsDbTableView* loc;
    loc = iDb->OpenViewMatchingTextLC( TPtrC(LOCATION), 
                                       TPtrC(COMMDB_NAME), 
                                       KLocation );
    TInt err = loc->GotoFirstRecord();

    User::LeaveIfError( err );
    TUint32 id( 0 );
    loc->ReadUintL( TPtrC(COMMDB_ID), id );

    CleanupStack::PopAndDestroy( loc ); // loc

    CLOG( ( EHandler, 1, _L( "<- CVpnApEngine::GetLocationIdL" ) ) );
    return id;    
    }



// ---------------------------------------------------------
// CVpnApEngine::WriteVpnWapApDataL
// ---------------------------------------------------------
//
void CVpnApEngine::WriteVpnWapApDataL( TBool aIsNew, CVpnApItem& aVpnApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CVpnApEngine::WriteVpnWapApDataL" ) ) );

    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CCommsDbTableView* wapt;

    if ( aIsNew )
        {
        wapt = iDb->OpenTableLC( TPtrC(WAP_ACCESS_POINT) );
        TInt err = wapt->InsertRecord( aVpnApItem.iVpnWapId );
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
                                            TPtrC(COMMDB_ID), 
                                            aVpnApItem.iVpnWapId );
        User::LeaveIfError( wapt->GotoFirstRecord() );
        User::LeaveIfError( wapt->UpdateRecord() );
        }

    // it is IP_BEARER
    wapt->WriteTextL( TPtrC(WAP_CURRENT_BEARER), TPtrC(WAP_IP_BEARER) );
    wapt->WriteTextL( TPtrC(COMMDB_NAME), *aVpnApItem.iName );

    wapt->WriteLongTextL( TPtrC(WAP_START_PAGE), KEmpty );

    User::LeaveIfError( wapt->PutRecordChanges( EFalse, EFalse ) );

    CleanupStack::PopAndDestroy( wapt );   // wapt

    CLOG( ( EHandler, 1, _L( "<- CVpnApEngine::WriteVpnWapApDataL" ) ) );
    }



// ---------------------------------------------------------
// CVpnApEngine::WriteWapBearerDataL
// ---------------------------------------------------------
//
void CVpnApEngine::WriteVpnWapBearerDataL( TBool aIsNew, 
                                           CVpnApItem& aVpnApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CVpnApEngine::WriteWapBearerDataL" ) ) );

    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CCommsDbTableView* bearert;
    if ( aIsNew )
        {
        bearert = iDb->OpenTableLC( TPtrC(WAP_IP_BEARER) );
        // CommDB's back-link makes returned UID value unusable,
        // variable needed just to be able to make the call...
        TUint32 alma;
        User::LeaveIfError( bearert->InsertRecord( alma ) );
        }
    else
        {
        bearert = iDb->OpenViewMatchingUintLC( TPtrC(WAP_IP_BEARER),
                                               TPtrC(WAP_ACCESS_POINT_ID),
                                               aVpnApItem.iVpnWapId );
        User::LeaveIfError( bearert->GotoFirstRecord() );
        User::LeaveIfError( bearert->UpdateRecord() );
        }
    bearert->WriteTextL( TPtrC(WAP_GATEWAY_ADDRESS), KDefWapGatewayIpAddress );

/*
    bearert->WriteUintL( TPtrC(WAP_WSP_OPTION), 
                         aApItem.iIsConnectionTypeContinuous );
    bearert->WriteBoolL( TPtrC(WAP_SECURITY), aApItem.iIsWTLSSecurityOn );
*/
    bearert->WriteUintL( TPtrC(WAP_ACCESS_POINT_ID), aVpnApItem.iVpnWapId );

    bearert->WriteUintL( TPtrC(WAP_IAP), aVpnApItem.iVpnIapId );        
    
    // Hardcoded to 0
    bearert->WriteUintL( TPtrC(WAP_PROXY_PORT), 0 );

    User::LeaveIfError( bearert->PutRecordChanges( EFalse, EFalse ) );

    CleanupStack::PopAndDestroy( bearert );   // bearert

    CLOG( ( EHandler, 1, _L( "<- CVpnApEngine::WriteWapBearerDataL" ) ) );
    }




// ---------------------------------------------------------
// CVpnApEngine::ReadVpnWapBearerDataL
// ---------------------------------------------------------
//
void CVpnApEngine::ReadVpnWapBearerDataL( TUint32 aUid, 
                                          CVpnApItem& aVpnApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CVpnApEngine::ReadVpnWapBearerDataL" ) ) );

    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CCommsDbTableView* bearert;
    bearert = iDb->OpenViewMatchingUintLC( TPtrC(WAP_IP_BEARER),
                                           TPtrC(WAP_ACCESS_POINT_ID),
                                           aUid );
    User::LeaveIfError( bearert->GotoFirstRecord() );
    aVpnApItem.iVpnWapId = aUid;
    
    // for vpn, it is not used....
    // bearert->WriteTextL( TPtrC(WAP_GATEWAY_ADDRESS), KEmpty );

/*
    bearert->WriteUintL( TPtrC(WAP_WSP_OPTION), 
                         aApItem.iIsConnectionTypeContinuous );
    bearert->WriteBoolL( TPtrC(WAP_SECURITY), aApItem.iIsWTLSSecurityOn );
*/

    // not neccessary, as we already queried for it and set it...
    // bearert->ReadUintL( TPtrC(WAP_ACCESS_POINT_ID), aVpnApItem.iVpnWapId );

    bearert->ReadUintL( TPtrC(WAP_IAP), aVpnApItem.iVpnIapId );        
    
    // Hardcoded to 0, not used, skip it 
    // bearert->WriteUintL( TPtrC(WAP_PROXY_PORT), 0 );

    CleanupStack::PopAndDestroy( bearert );   // bearert

    CLOG( ( EHandler, 1, _L( "<- CVpnApEngine::ReadVpnWapBearerDataL" ) ) );
    }




// ---------------------------------------------------------
// CVpnApEngine::ReadVpnWapApDataL
// ---------------------------------------------------------
//
void CVpnApEngine::ReadVpnWapApDataL( CVpnApItem& aVpnApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CVpnApEngine::ReadVpnWapApDataL" ) ) );

    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CCommsDbTableView* wapt;

    wapt = iDb->OpenViewMatchingUintLC( TPtrC(WAP_ACCESS_POINT),
                                        TPtrC(COMMDB_ID), 
                                        aVpnApItem.iVpnWapId );
    User::LeaveIfError( wapt->GotoFirstRecord() );

    // it is IP_BEARER, but as hardcoded, skip it...
    // wapt->ReadTextL( TPtrC(WAP_CURRENT_BEARER), TPtrC(WAP_IP_BEARER) );

    HBufC* buf = NULL;
    buf = ApCommons::ReadText16ValueLC( wapt, TPtrC(COMMDB_NAME) );
    aVpnApItem.WriteTextL( EApVpnWapAccessPointName, *buf );
    CleanupStack::PopAndDestroy( buf ); // buf

    // HARDCODED, skip it
    //wapt->WriteLongTextL( TPtrC(WAP_START_PAGE), KEmpty );

    CleanupStack::PopAndDestroy( wapt );   // wapt

    CLOG( ( EHandler, 1, _L( "<- CVpnApEngine::ReadVpnWapApDataL" ) ) );
    }



// ---------------------------------------------------------
// CVpnApEngine::ReadVpnIapRecordL
// ---------------------------------------------------------
//
void CVpnApEngine::ReadVpnIapRecordL( CVpnApItem& aVpnApItem )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApEngine::ReadVpnIapRecordL" ) ) );

    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CCommsDbTableView* t;

    t = iDb->OpenViewMatchingUintLC( TPtrC(IAP),
                                     TPtrC(COMMDB_ID), 
                                     aVpnApItem.iVpnIapId );
    User::LeaveIfError( t->GotoFirstRecord() );

    // Name is already set from WAP, skip it
    // t->WriteTextL( TPtrC(COMMDB_NAME), *aVpnApItem.iName );

    t->ReadUintL( TPtrC(IAP_SERVICE), aVpnApItem.iVpnTableId );
    
    // Hardcoded table name, not used, skip it
    // t->WriteTextL( TPtrC(IAP_SERVICE_TYPE), TPtrC(VPN_SERVICE) );

    // hardcoded..., not used, skip it
    //t->WriteTextL( TPtrC(IAP_BEARER_TYPE), TPtrC(VIRTUAL_BEARER) );

    t->ReadUintL( TPtrC(IAP_BEARER), aVpnApItem.iVirtualBearerID );

    t->ReadUintL( TPtrC(IAP_NETWORK), aVpnApItem.iHomeNetwork );

    // Network weighting hardcoded to 0!, not used, skip it
    //t->WriteUintL( TPtrC(IAP_NETWORK_WEIGHTING), 0 );
    
    // as location is only written, skip it
    /*
    TUint32 loc = GetLocationIdL();
    t->WriteUintL( TPtrC(IAP_LOCATION), loc );
    */

    CleanupStack::PopAndDestroy( t );   // t
   
    CLOG( ( EApItem, 0, _L( "<- CVpnApEngine::ReadVpnIapRecordL" ) ) );
    }



// ---------------------------------------------------------
// CVpnApEngine::ReadVpnDataL
// ---------------------------------------------------------
//
void CVpnApEngine::ReadVpnDataL( CVpnApItem& aVpnApItem )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApEngine::ReadVpnDataL" ) ) );

    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CCommsDbTableView* t;

    t = iDb->OpenViewMatchingUintLC( TPtrC(VPN_SERVICE),
                                     TPtrC(COMMDB_ID), 
                                     aVpnApItem.iVpnTableId );
    User::LeaveIfError( t->GotoFirstRecord() );

    // already got from WAP table, skip it here
    //t->WriteTextL( TPtrC(COMMDB_NAME), *aVpnApItem.iName );
    
    HBufC* buf = NULL;
    buf = ApCommons::ReadText16ValueLC( t, TPtrC(VPN_SERVICE_POLICY) );
    aVpnApItem.WriteTextL( EApVpnPolicyId, *buf );
    
    RVpnServ ps;
    User::LeaveIfError( ps.Connect() );
    CleanupClosePushL<RVpnServ>( ps );
    
    TVpnPolicyName pName( *buf );

    TInt policyCount(0);
    TInt err = ps.EnumeratePolicies(policyCount);
    if ( !err && policyCount)
        {                
        CArrayFixFlat<TVpnPolicyInfo>* policyList;
        policyList = new (ELeave) CArrayFixFlat<TVpnPolicyInfo>(policyCount);
        CleanupStack::PushL(policyList);

        err = ps.GetPolicyInfoList(policyList);
        if ( err )
            {
            aVpnApItem.WriteTextL( EApVpnPolicy, KEmpty );
            }
        else
            {
            TBool found(EFalse);
            for (TInt i(0); i<policyList->Count(); ++i)
                {
                if (pName.Compare((*policyList)[i].iId) == 0 )
                    {
                    aVpnApItem.WriteTextL( EApVpnPolicy, 
                        (*policyList)[i].iName );
                    found = ETrue;
                    break;    
                    }
                }
            if (!found)
                {
    // When the policy is deleted (err!=0):
    // have this leave, in UI display a note about needing to be reconfigured &
    // set it & policyID to none to look like there is NO policy
    // which will force the reconfiguration if in the UI
    //    User::LeaveIfError( err );
                aVpnApItem.WriteTextL( EApVpnPolicy, KEmpty );
                }
            }
        CleanupStack::PopAndDestroy(policyList);
        }

    CleanupStack::PopAndDestroy(); // ps

    CleanupStack::PopAndDestroy( buf ); // buf

    // These fields might be missing from CommsDat.
    // ApEngine does not support SNAPs, use CmManager API to get full support.
    TRAP_IGNORE( t->ReadUintL( TPtrC(VPN_SERVICE_IAP), aVpnApItem.iRealIapId ) )
    TRAP_IGNORE( t->ReadUintL( TPtrC(VPN_SERVICE_NETWORKID), aVpnApItem.iHomeNetwork ) )
    
    CleanupStack::PopAndDestroy( t );   // t

    // Get real IAP name !!!!!
    CCommsDbTableView* iapt;

    iapt = iDb->OpenViewMatchingUintLC( TPtrC(IAP),
                                     TPtrC(COMMDB_ID), 
                                     aVpnApItem.iRealIapId );

    err = iapt->GotoFirstRecord();
    if ( err == KErrNotFound ) 
        {
        aVpnApItem.WriteTextL( EApVpnRealIapName, KEmpty );
        aVpnApItem.iVpnRealWapId = 0;
        }
    else
        {        
        User::LeaveIfError( err );
        buf = ApCommons::ReadText16ValueLC( iapt, TPtrC(COMMDB_NAME) );
        aVpnApItem.WriteTextL( EApVpnRealIapName, *buf );
        CleanupStack::PopAndDestroy( buf ); // buf

        CApUtils* util = CApUtils::NewLC( *iDb );
        
        TUint32 realwapid = util->WapIdFromIapIdL( aVpnApItem.iRealIapId );
        aVpnApItem.iVpnRealWapId = realwapid;

        CleanupStack::PopAndDestroy( util );

        }

    CleanupStack::PopAndDestroy( iapt );   // t

    CLOG( ( EApItem, 0, _L( "<- CVpnApEngine::ReadVpnDataL" ) ) );
    }











// ---------------------------------------------------------
// CVpnApEngine::WriteProxyDataL
// ---------------------------------------------------------
//
void CVpnApEngine::WriteProxyDataL( TBool aIsNew, 
                                      const CVpnApItem& aVpnApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CVpnApEngine::WriteProxyDataL" ) ) );

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
        proxies = iDb->OpenViewMatchingTextLC( TPtrC(PROXIES),
                                               TPtrC(PROXY_SERVICE_TYPE),
                                               TPtrC(VPN_SERVICE) );
        TInt err = proxies->GotoFirstRecord();
        if ( err == KErrNotFound )
            { // originally there were NO proxy, but now it is added
            // variable needed just to be able to make the call...
            TUint32 dummy;
            User::LeaveIfError( proxies->InsertRecord( dummy ) );
            }
        else
            { 
            // check if it is our proxy...
            // Add test cases covering this route
            TUint32 uval;
            TBool found( EFalse );
            do
                {
                // Add test cases covering this route
                proxies->ReadUintL( TPtrC(PROXY_ISP), uval );
                if ( uval == aVpnApItem.iVpnTableId )
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
                TUint32 alma;
                User::LeaveIfError( proxies->InsertRecord( alma ) );
                }
            else
                { // we found our proxy, use it...
                User::LeaveIfError( proxies->UpdateRecord() );
                }
            }
        }
    proxies->WriteTextL( TPtrC(PROXY_SERVICE_TYPE),
                         TPtrC(VPN_SERVICE) );

    proxies->WriteUintL( TPtrC(PROXY_ISP), aVpnApItem.iVpnTableId );


    proxies->WriteLongTextL( TPtrC(PROXY_SERVER_NAME),
                            *aVpnApItem.iProxyServerAddress );

    proxies->WriteTextL( TPtrC(PROXY_PROTOCOL_NAME),
                         KDefProxyProtocolName );

    proxies->WriteUintL( TPtrC(PROXY_PORT_NUMBER),
                         aVpnApItem.iProxyPort );

    TBool useproxy( EFalse );
    if ( ( aVpnApItem.iProxyPort ) 
        || ( aVpnApItem.iProxyServerAddress->Compare( KEmpty ) != 0 ) )
        {
        useproxy = ETrue;
        }

    proxies->WriteBoolL( TPtrC(PROXY_USE_PROXY_SERVER), 
                         useproxy );

    /*
    not used, skipped
    proxies->WriteLongTextL( TPtrC(PROXY_EXCEPTIONS),
                            *aApItem.iApProxyExceptions );
    */
    proxies->WriteLongTextL( TPtrC(PROXY_EXCEPTIONS),
                            KEmpty );

    User::LeaveIfError( proxies->PutRecordChanges( EFalse, EFalse ) );

    CleanupStack::PopAndDestroy( proxies );   // proxies

    CLOG( ( EHandler, 1, _L( "<- CVpnApEngine::WriteProxyDataL" ) ) );
    }



// ---------------------------------------------------------
// CVpnApEngine::ReadProxyDataL
// ---------------------------------------------------------
//
void CVpnApEngine::ReadProxyDataL( CVpnApItem& aVpnApItem )
    {
    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CLOG( ( EHandler, 0, _L( "-> CVpnApEngine::ReadProxyDataL" ) ) );

    CCommsDbTableView* proxies;    
    proxies = iDb->OpenViewMatchingTextLC( TPtrC(PROXIES),
                                           TPtrC(PROXY_SERVICE_TYPE),
                                           TPtrC(VPN_SERVICE) );
    TInt err = proxies->GotoFirstRecord();
    if ( err != KErrNotFound )
        {
        User::LeaveIfError( err );
        }
    if ( err != KErrNotFound )
        {
        // check if it is our proxy...
        // Add test cases covering this route
        TUint32 uval;
        TBool found( EFalse );
        do
            {
            // Add test cases covering this route
            proxies->ReadUintL( TPtrC(PROXY_ISP), uval );
            if ( uval == aVpnApItem.iVpnTableId )
                {
                found = ETrue;
                }
            else
                {
                err = proxies->GotoNextRecord();
                }
            } while ( (!err) && (!found) );
        if ( found )
            { // it is our proxy record, use it 
            HBufC* buf = ApCommons::ReadLongTextLC( proxies,
                                                    TPtrC(PROXY_SERVER_NAME),
                                                    err );
            if ( err != KErrUnknown )
                {
                aVpnApItem.WriteLongTextL( EApVpnProxyServerAddress, *buf );
                }

            CleanupStack::PopAndDestroy( buf ); // buf

            TUint32 tempint;

            ApCommons::ReadUintL( proxies, TPtrC(PROXY_PORT_NUMBER), tempint );
            aVpnApItem.WriteUint( EApVpnProxyPortNumber, tempint );

            /*
            buf = ApCommons::ReadLongTextLC( proxies,
                                             TPtrC(PROXY_EXCEPTIONS),
                                             err );
            aVpnApItem.WriteLongTextL( EApProxyExceptions, *buf );
            CleanupStack::PopAndDestroy( buf ); // buf
            */
            }
        }
    CleanupStack::PopAndDestroy( proxies ); // proxies

    CLOG( ( EHandler, 1, _L( "<- CVpnApEngine::ReadProxyDataL" ) ) );
    }




// ---------------------------------------------------------
// CVpnApEngine::RemoveVpnProxyDataL
// ---------------------------------------------------------
//
void CVpnApEngine::RemoveVpnProxyDataL( TUint32 aIspId )
    {
    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CLOG( ( EHandler, 0, _L( "-> CVpnApEngine::RemoveProxyDataL" ) ) );

    // Add test cases covering all this functions routes

    CCommsDbTableView* proxies;    
    proxies = iDb->OpenViewMatchingTextLC( TPtrC(PROXIES), 
                                           TPtrC(PROXY_SERVICE_TYPE),
                                           TPtrC(VPN_SERVICE) );
    TInt err = proxies->GotoFirstRecord();
    if ( err == KErrNone )
        { // now get OUR proxy record
        TUint32 uval;
        TBool found( EFalse );
        do
            {
            // Add test cases covering this route
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
    else
        {
        // silently ignore KErrNotFound. It is caused by incorrect DB,
        // we are 'repairing it' this way.
        if ( err != KErrNotFound )
            {
            User::LeaveIfError( err );
            }
        }
    CleanupStack::PopAndDestroy( proxies ); // proxies
    
    CLOG( ( EHandler, 1, _L( "<- CVpnApEngine::RemoveProxyDataL" ) ) );
    }



// ---------------------------------------------------------
// CVpnApEngine::DoDeleteVpnL
// ---------------------------------------------------------
//
void CVpnApEngine::DoDeleteVpnL( TUint32 aUid )
    {
    CLOG( ( EHandler, 0, _L( "-> CVpnApEngine::DoDeleteVpnL" ) ) );

    // when deleting, need to delete: 
    // homenetwork
    // VPN_TABLE record
    // IAP's VPN record
    // WAP's VPN record
    // IP_BEARER's record
    // proxy record

    // first make sure that the given AP is not in use
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

        TUint32 ipbearerid;
        ApCommons::ReadUintL( bearert, TPtrC(COMMDB_ID), ipbearerid );

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

        if ( *servicetype == TPtrC(VPN_SERVICE) )
            {
            RemoveRecordL( TPtrC(VPN_SERVICE), ispid );
            RemoveRecordL( TPtrC(IAP), iapid );
            }
        else
            { // what to do if none of them???
            User::Leave( KErrInvalidBearer );
            }
        RemoveVpnProxyDataL( ispid );
        RemoveRecordL( TPtrC(WAP_IP_BEARER), ipbearerid );
        // servicetype, iapt, bearert
        CleanupStack::PopAndDestroy( 3, bearert ); 
        }
    else
        { // sms,
        User::Leave( KErrInvalidBearerType );
        }

    RemoveRecordL( TPtrC(WAP_ACCESS_POINT), aUid );

    CleanupStack::PopAndDestroy( 2, wapt );   // buf, wapt

    CLOG( ( EHandler, 1, _L( "<- CVpnApEngine::DoDeleteVpnL" ) ) );
    }



// ---------------------------------------------------------
// CVpnApEngine::RemoveRecordL
// ---------------------------------------------------------
//
void CVpnApEngine::RemoveRecordL( const TDesC& aTable, TUint32 aUid )
    {
    CLOG( ( EHandler, 0, _L( "-> CVpnApEngine::RemoveRecordL" ) ) );

    CCommsDbTableView* t = iDb->OpenViewMatchingUintLC( 
                                            aTable, 
                                            TPtrC(COMMDB_ID), 
                                            aUid );
    TInt res = t->GotoFirstRecord(); // OK.
    if ( res == KErrNone )
        { // exists, delete it
        User::LeaveIfError( t->DeleteRecord() );
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
    CleanupStack::PopAndDestroy( t ); // t

    CLOG( ( EHandler, 1, _L( "<- CVpnApEngine::RemoveRecordL" ) ) );
    }





// ---------------------------------------------------------
// CVpnApEngine::WriteNetworkDataL
// ---------------------------------------------------------
//
void CVpnApEngine::WriteNetworkDataL( TBool aIsNew, 
                                        CVpnApItem& aVpnApItem )
    {
    CLOG( ( EHandler, 0, _L( "-> CVpnApEngine::WriteNetworkDataL" ) ) );

    if ( aVpnApItem.iVpnIapNetwork == 0 )
        {
        CCommsDbTableView* networks;
        HBufC* buf = HBufC::NewLC( KApEngineMaxSql );
        if ( aIsNew )
            {
            networks = iDb->OpenTableLC( TPtrC(NETWORK) );
            TUint32 netId;
            User::LeaveIfError( networks->InsertRecord( netId ) );
            aVpnApItem.iVpnIapNetwork = netId;
            }
        else
            {
            networks = iDb->OpenViewMatchingUintLC( 
                                TPtrC(NETWORK), TPtrC(COMMDB_ID), 
                                aVpnApItem.iVpnIapNetwork );

            TInt err = networks->GotoFirstRecord();
            if ( err == KErrNotFound )
                { // originally there were NO network, but now it is added
                TUint32 netId;
                User::LeaveIfError( networks->InsertRecord( netId ) );
                aVpnApItem.iVpnIapNetwork = netId;
                }
            else
                {
                User::LeaveIfError( networks->UpdateRecord() );
                }
            }
        networks->WriteTextL( TPtrC(COMMDB_NAME), *aVpnApItem.iName );

        User::LeaveIfError( networks->PutRecordChanges( EFalse, EFalse ) );

        CleanupStack::PopAndDestroy( networks );   // networks
        CleanupStack::PopAndDestroy( buf );   // buf
        }
    
    CLOG( ( EHandler, 1, _L( "<- CVpnApEngine::WriteNetworkDataL" ) ) );
    }


// ---------------------------------------------------------
// CVpnApEngine::ReadNetworkDataL
// ---------------------------------------------------------
//
void CVpnApEngine::ReadNetworkDataL( CVpnApItem& aVpnApItem )
    {    
    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CLOG( ( EHandler, 0, _L( "-> CVpnApEngine::ReadNetworkDataL" ) ) );
        
    CCommsDbTableView* networks;
    networks = iDb->OpenViewMatchingUintLC( TPtrC(NETWORK), 
                                            TPtrC(COMMDB_ID), 
                                            aVpnApItem.iVpnIapNetwork );

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
            aVpnApItem.WriteTextL( EApVpnIapNetworkName, *buf );
            }

        CleanupStack::PopAndDestroy( buf ); // buf
        }
    else
        { // not found, set to NONE
        aVpnApItem.WriteTextL( EApVpnIapNetworkName, KEmpty );
        }
    CleanupStack::PopAndDestroy( networks ); // networks
    
    CLOG( ( EHandler, 1, _L( "<- CVpnApEngine::ReadNetworkDataL" ) ) );
  }



// ---------------------------------------------------------
// CVpnApEngine::RemoveNetworkDataL
// ---------------------------------------------------------
//
void CVpnApEngine::RemoveNetworkDataL( TUint32 aIspId )
    {
    // internal function, caller MUST initiate a transaction, WE WILL NOT.
    CLOG( ( EHandler, 0, _L( "-> CVpnApEngine::RemoveNetworkDataL" ) ) );

    RemoveRecordL( TPtrC(NETWORK), aIspId );

    CLOG( ( EHandler, 1, _L( "<- CVpnApEngine::RemoveNetworkDataL" ) ) );
    }

//  End of File

