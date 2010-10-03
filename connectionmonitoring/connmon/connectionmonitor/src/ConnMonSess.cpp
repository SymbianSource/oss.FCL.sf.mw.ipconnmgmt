/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Connection Monitor session.
*
*/

#include <featmgr.h>
#include <etelmm.h>
#include <rconnmon.h>

#include "ConnMonSess.h"
#include "ConnMonServ.h"
#include "ConnMonIAP.h"
#include "ConnMonDef.h"
#include "CEventQueue.h"
#include "CIapSupport.h"
#include "CUiDialogAO.h"
#include "ConnMonUtils.h"
#include "ConnMonAvailabilityManager.h"
#include "ConnMonBearerGroupManager.h"
#include "connmonwlannetwork.h"
#include "CPlugin.h"
#include "log.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TConnSettings::TConnSettings
// -----------------------------------------------------------------------------
//
TConnSettings::TConnSettings(
        TUint aActivityTimeThreshold,
        TUint aDLDataThreshold,
        TUint aULDataThreshold )
        :
        iActivityTimeThreshold( aActivityTimeThreshold ),
        iDLDataThreshold( aDLDataThreshold ),
        iULDataThreshold( aULDataThreshold )
    {
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TConnRequest::TConnRequest
// -----------------------------------------------------------------------------
//
TConnRequest::TConnRequest( TUint8 aType, const RMessage2 aMessage )
        :
        iType( aType ),
        iMessage( aMessage )
    {
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TConnParams::TConnParams
// -----------------------------------------------------------------------------
//
TConnParams::TConnParams( TUint aConnectionId )
        :
        iConnectionId( aConnectionId ),
        iSettings( 0, 0, 0 )
    {
    iRequests.Reset();
    }


// -----------------------------------------------------------------------------
// TConnParams::MatchId
// Finds TConnParams item matching by connection Id.
// -----------------------------------------------------------------------------
//
TBool TConnParams::MatchId( const TConnParams& aFirst, const TConnParams& aSecond )
    {
    if ( aFirst.iConnectionId == aSecond.iConnectionId )
        {
        return ETrue;
        }
    return EFalse;
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonSession::CConnMonSession
// -----------------------------------------------------------------------------
//
CConnMonSession::CConnMonSession( CConnMonServer* aServer )
        :
        CSession2(),
        iCmServer( aServer )
    {
    }


// -----------------------------------------------------------------------------
// CConnMonSession::ConstructL
// -----------------------------------------------------------------------------
//
void CConnMonSession::ConstructL()
    {
    //LOGENTRFN("CConnMonSession::ConstructL()")

    iListening = EFalse;
    iEnumConnInfo = new( ELeave ) CArrayFixFlat<TUint>( 1 );
    iEnumConnInfo->Reset();

    // Initialize last event time
    iLastEventTime = iCmServer->EventQueue()->LastEventTime();

    // Create a new object index (it stores the plug-in objects owned by this session)
    iPlugins = CObjectIx::NewL();

    // Initialize the object container using the object container index in the server
    // (Object container provides unique ids for the objects owned by this session)
    iContainer = iCmServer->NewContainerL();

    //LOGEXITFN("CConnMonSession::ConstructL()")
    }


// -----------------------------------------------------------------------------
// CConnMonSession::~CConnMonSession
// -----------------------------------------------------------------------------
//
CConnMonSession::~CConnMonSession()
    {
    LOGIT(".")
    LOGENTRFN("CConnMonSession::~CConnMonSession()")

    if ( iListening && ( iBearerAvailabilityThreshold == 1 ) )
        {
        // Resets plugin threshold, so plugin can stop sending bearer
        // availability events when no longer needed.
        SetPluginThreshold( EBearerAvailabilityThreshold, 0 );
        }

    // Cancel any 'receive' request
    CancelReceiveEvent();

    for ( TInt i = 0; i < iConnParams.Count(); i++ )
        {
        iConnParams[i].iRequests.Close();
        }

    iConnParams.Close();

    if ( iEnumConnInfo != 0 )
        {
        iEnumConnInfo->Reset();
        delete iEnumConnInfo;
        iEnumConnInfo = NULL;
        }

    // Delete the object index
    delete iPlugins;
    iPlugins = NULL;

    // Delete the object container
    if ( iContainer != 0 )
        {
        iCmServer->RemoveContainer( iContainer );
        iContainer = NULL;
        }

    // Check WLAN support
    if ( iCmServer->Iap()->WlanSupport() )
        {
        // Delete WLAN session from WlanSessions array if exists
        iCmServer->Iap()->WlanSupport()->DeleteWlanSession( this );
        }

    // Decrement session count, so server may terminate if no longer needed
    iCmServer->DecrementSessions();

    iCmServer = NULL;
    iReceiveBuffer = NULL;

    LOGEXITFN("CConnMonSession::~CConnMonSession()")
    }


// -----------------------------------------------------------------------------
// CConnMonSession::ServiceL
// -----------------------------------------------------------------------------
//
void CConnMonSession::ServiceL( const RMessage2& aMessage )
    {
    //LOGENTRFN("CConnMonSession::ServiceL()")
    TInt rc( KErrNone );

    #ifdef _DEBUG
    if ( aMessage.Function() != EReqReceiveEvent )
        {
        LOGIT(".")
        LOGIT2("ServiceL: function %d, id %X", aMessage.Function(), aMessage.SecureId().iId)
        }
    #endif // _DEBUG

    iMessage2 = aMessage;
    switch ( aMessage.Function() )
        {
        case EReqGetConnectionCount:
            rc = GetConnectionCountL();
            CompleteMessage( rc );
            break;

        case EReqGetConnectionInfo:
            rc = GetConnectionInfo();
            CompleteMessage( rc );
            break;

        case EReqGetSubConnectionInfo:
            rc = GetSubConnectionInfo();
            CompleteMessage( rc );
            break;

        case EReqGetIntAttribute:
            rc = GetIntAttributeL();
            CompleteMessage( rc );
            break;

        case EReqGetUintAttribute:
            rc = GetUintAttributeL();
            CompleteMessage( rc );
            break;

        case EReqGetBoolAttribute:
            rc = GetBoolAttributeL();
            CompleteMessage( rc );
            break;

        case EReqGetStringAttribute:
            rc = GetStringAttributeL();
            CompleteMessage( rc );
            break;

        case EReqGetPckgAttribute:
            rc = GetPckgAttributeL();
            CompleteMessage( rc );
            break;

        case EReqSetIntAttribute:
            rc = SetIntAttributeL();
            CompleteMessage( rc );
            break;

        case EReqSetUintAttribute:
            rc = SetUintAttributeL();
            CompleteMessage( rc );
            break;

        case EReqSetBoolAttribute:
            rc = SetBoolAttribute();
            CompleteMessage( rc );
            break;

        case EReqSetStringAttribute:
            rc = SetStringAttributeL();
            CompleteMessage( rc );
            break;

        case EReqSetPckgAttribute:
            rc = SetPckgAttribute();
            CompleteMessage( rc );
            break;

        case EReqCancelAsyncRequest:
            rc = CancelAsyncRequest();
            CompleteMessage( rc );
            break;

        case EReqReceiveEvent: // TAny& aBuffer - async
            ReceiveEventL( aMessage.Ptr0() );
            break;

        case EReqCancelReceiveEvent:
            CancelReceiveEvent();
            CompleteMessage( 0 );
            break;

        // Plug-in specific, fall-through intended
        case EReqPluginCreateSubSession:
        case EReqPluginCloseSubSession:
        case EReqPluginRegisterAttribute:
        case EReqPluginCancelRegisterAttribute:
        case EReqPluginEvent:
        case EReqPluginGetQuery:
        case EReqPluginCancelGetQuery:
        case EReqPluginAttribute:
            ServicePluginL( aMessage );
            break;

        default:
            iCmServer->PanicClient( EBadRequest );
            break;
        }
    //LOGEXITFN("CConnMonSession::ServiceL()")
    }


// -----------------------------------------------------------------------------
// CConnMonSession::CompleteMessage
// -----------------------------------------------------------------------------
//
void CConnMonSession::CompleteMessage( const TInt aStatus )
    {
    // Check if async message
    if ( aStatus != KRequestPending )
        {
        // Complete message
        Message().Complete( aStatus );
        }
    }


// -----------------------------------------------------------------------------
// CConnMonSession::GetConnectionCountL
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::GetConnectionCountL()
    {
    LOGENTRFN("CConnMonSession::GetConnectionCountL()")
    TInt err( KErrNone );
    TUint count( 0 );

    // Call comms APIs
    err = iCmServer->Iap()->EnumerateConnectionsL( count, *iEnumConnInfo );
    LOGIT2("SERVER: EReqGetConnectionCount OUT: -> count %d, ret <%d>", count, err)

    if ( KErrNone == err )
        {
        // Write "count" back to the client's address space
        TPtrC8 d( reinterpret_cast<TUint8*>( &count ), sizeof( count ) );

        err = Message().Write( 0, d );
        if ( err != KErrNone )
            {
            LOGIT1("SERVER: EReqGetConnectionCount: -> error, Message().Write(0, d) returned <%d>", err)
            iCmServer->PanicClient( EBadDescriptor );
            }

        LOGIT("SERVER: EReqGetConnectionCount. -> RemoveObsoleteConnectionParams()")
        // Remove obsolete connection settings
        RemoveObsoleteConnectionParams();
        }

    LOGEXITFN1("CConnMonSession::GetConnectionCountL()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CConnMonSession::GetConnectionInfo
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::GetConnectionInfo()
    {
    LOGENTRFN("CConnMonSession::GetConnectionInfo()")
    TInt err( KErrNone );
    TUint id( 0 );
    TUint subConnectionCount( 1 );

    TInt index = Message().Int0();

    if ( !iEnumConnInfo )
        {
        err = KErrUnknown;
        }
    // Indexes given to client start from 1
    else if ( ( index <= 0 ) || ( index > iEnumConnInfo->Count() ) )
        {
        err = KErrArgument;
        }
    else
        {
        // Enumerated connection IDs are stored in session
        id = (*iEnumConnInfo)[index - 1];

        // Get subconnection count
        err = iCmServer->Iap()->GetConnectionInfo( id, subConnectionCount );
        LOGIT4("SERVER: EReqGetConnectionInfo: index %d -> id %d, subConnCount %d, ret <%d>",
                index, id, subConnectionCount, err)

        if ( KErrNone == err )
            {
            // Write "id" back to the client's address space
            TPtrC8 d( reinterpret_cast<TUint8*>( &id ), sizeof( id ) );

            err = Message().Write( 1, d );
            if ( KErrNone != err )
                {
                iCmServer->PanicClient( EBadDescriptor );
                }
            else
                {
                // Write "subConnectionCount" back to the client's address space
                TPtrC8 e(
                        reinterpret_cast<TUint8*>( &subConnectionCount ),
                        sizeof( subConnectionCount ) );

                err = Message().Write( 2, e );
                if ( KErrNone != err )
                    {
                    iCmServer->PanicClient( EBadDescriptor );
                    }
                }
            }
        }
    LOGEXITFN1("CConnMonSession::GetConnectionInfo()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CConnMonSession::GetSubConnectionInfo
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::GetSubConnectionInfo() const
    {
    LOGENTRFN("CConnMonSession::GetSubConnectionInfo()")
    LOGEXITFN1("CConnMonSession::GetSubConnectionInfo()", KErrNotSupported)
    return KErrNotSupported;
    }


// -----------------------------------------------------------------------------
// CConnMonSession::GetIntAttributeL
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::GetIntAttributeL()
    {
    LOGENTRFN("CConnMonSession::GetIntAttributeL()")
    TInt err( KErrNone );
    TInt value( 0 );

    // Call comms APIs
    err = iCmServer->Iap()->GetIntAttributeL( Message(), value );
    LOGIT1("SERVER: CConnMonSession::GetIntAttributeL: Iap()->GetIntAttributeL <%d>", err)

    // Check if this request should be served by a plug-in engine
    if ( ( Message().Int0() >= EBearerIdGPRS ) &&
            ( err != KErrNone ) &&
            ( err != KRequestPending ) )
        {
        RMessage2 msg = Message();
        err = iCmServer->SendRequestToPlugin( EReqGetIntAttribute, msg );
        }

    LOGIT5("SERVER: EReqGetIntAttribute. id %d, subConnId %d, attr %d -> value %d, ret <%d>",
            Message().Int0(), Message().Int1(), Message().Int2(), value, err)
    if ( KErrNone == err )
        {
        // Write "value" back to the client's address space
        TPtrC8 d( reinterpret_cast<TUint8*>( &value ), sizeof( value ) );

        err = Message().Write( KAttributeParameterIndex, d );
        if ( KErrNone != err )
            {
            iCmServer->PanicClient( EBadDescriptor );
            }
        }

    LOGEXITFN1("CConnMonSession::GetIntAttributeL()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CConnMonSession::GetUintAttributeL
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::GetUintAttributeL()
    {
    LOGENTRFN("CConnMonSession::GetUintAttributeL()")
    TInt err( KErrNone );
    TUint value( 0 );

    TUint connectionId( Message().Int0() );
    TInt attribute( Message().Int2() );

    switch ( attribute )
        {
        case KDownlinkDataThreshold:
        case KUplinkDataThreshold:
        case KActivityTimeThreshold:
            {
            if ( iCmServer->Iap()->ConnectionExists( connectionId ) )
                {
                TConnSettings settings( 0, 0, 0 );
                err = GetConnectionSettings( connectionId, settings );
                if ( KErrNone == err )
                    {
                    if ( attribute == KDownlinkDataThreshold )
                        {
                        value = settings.iDLDataThreshold;
                        }
                    else if ( attribute == KUplinkDataThreshold )
                        {
                        value = settings.iULDataThreshold;
                        }
                    else
                        {
                        value = settings.iActivityTimeThreshold;
                        }
                    }
                }
            else
                {
                err = KErrNotFound;
                }
            }
            break;

        case KBearerAvailabilityThreshold:
            value = iBearerAvailabilityThreshold;
            break;
        case KSignalStrengthThreshold:
            value = iSignalStrengthThreshold;
            break;
        case KBearerGroupThreshold:
            value = iBearerGroupThreshold;
            break;

        case KMobilePhoneNetworkMode:
            {
            LOGIT("GetUintAttributeL: case KMobilePhoneNetworkMode")

            // Check network registration first
            TInt registrationMode( ENetworkRegistrationNotAvailable );
            err = iCmServer->Iap()->GetNetworkRegistrationMode( registrationMode );
            if ( KErrNone == err )
                {
                if ( registrationMode == ENetworkRegistrationUnknown ||
                        registrationMode == ENetworkRegistrationNoService )
                    {
                    value = EConnMonNetworkModeUnknown;
                    }
                else
                    {
                    RMobilePhone::TMobilePhoneNetworkMode networkMode;
                    err = iCmServer->Iap()->MobilePhone()->GetCurrentMode( networkMode );
                    if ( KErrNone == err )
                        {
                        LOGIT1("GetUintAttributeL: current phone network mode %d", networkMode)

                        TConnMonMobilePhoneNetworkMode connMonNetworkMode;
                        err = ConnMonUtils::MobilePhoneNetworkModeToConnMonMobilePhoneNetworkMode(
                                networkMode,
                                connMonNetworkMode );
                        if ( KErrNone == err )
                            {
                            value = connMonNetworkMode;
                            }
                        }
                    }
                }
            }
            break;

        case KWlanScanMaxDelay:
            // Check WLAN support for the given ID
            err = iCmServer->Iap()->WlanSupportCheck( connectionId );
            if ( KErrNone == err )
                {
                err = iCmServer->Iap()->WlanSupport()->GetUintAttributeL( Message(), value );
                }
            break;

        default:
            {
            LOGIT1("GetUintAttributeL: case %d", attribute)

            // Call comms APIs
            err = iCmServer->Iap()->GetUintAttributeL( Message(), value );

            // Check if message will be completed later
            if ( KRequestPending == err )
                {
                if ( KDownlinkData == attribute )
                    {
                    TConnRequest request( KDownlinkData, Message() );
                    SetConnectionRequest( connectionId, request );
                    }
                else if ( KUplinkData == attribute )
                    {
                    TConnRequest request( KUplinkData, Message() );
                    SetConnectionRequest( connectionId, request );
                    }
                }

            // Check if this request should be served by a plug-in engine
            if ( ( Message().Int0() >= EBearerIdGPRS ) &&
                    ( err != KErrNone ) &&
                    ( err != KRequestPending ) )
                {
                RMessage2 msg = Message();
                err = iCmServer->SendRequestToPlugin( EReqGetUintAttribute, msg );
                }
            }
            break;
        }
    LOGIT5("SERVER: EReqGetUintAttribute: id %d, subConnId %d, attr %d -> value %d, ret <%d>",
            Message().Int0(), Message().Int1(), Message().Int2(), value, err )

    if ( KErrNone == err )
        {
        // Write "value" back to the client's address space
        TPtrC8 d( reinterpret_cast<TUint8*>( &value ), sizeof( value ) );

        err = Message().Write( KAttributeParameterIndex, d );
        if ( err != KErrNone )
            {
            iCmServer->PanicClient( EBadDescriptor );
            }
        }

    LOGEXITFN1("CConnMonSession::GetUintAttributeL()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonSession::GetBoolAttributeL
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::GetBoolAttributeL()
    {
    LOGENTRFN("CConnMonSession::GetBoolAttributeL()")
    TInt err( KErrNone );
    TBool value( EFalse );
    TUint connectionId( Message().Int0() );
    TUint attribute( Message().Int2() );

    LOGIT2("SERVER: EReqGetBoolAttribute IN: id %d, attr %d", connectionId, attribute)
    // Call comms APIs
    err = iCmServer->Iap()->GetBoolAttributeL( Message(), value );

    // Check if request was KConnectionActive and should be completed later
    if ( ( err == KRequestPending ) && ( attribute == KConnectionActive ) )
        {
        TConnRequest request( KConnectionActive, Message() );
        SetConnectionRequest( connectionId, request );
        }

    // Check if this request should be served by a plug-in engine
    if ( ( ( connectionId >= EBearerIdGPRS ) &&
           ( err != KErrNone ) &&
           ( err != KRequestPending ) )
         ||
         ( ( attribute == KBearerAvailability ) &&
           ( err == KErrCancel ) ) )
        {
        RMessage2 msg = Message();
        err = iCmServer->SendRequestToPlugin( EReqGetBoolAttribute, msg );
        }
    LOGIT5("SERVER: EReqGetBoolAttribute OUT: id %d, subConnId %d, attr %d -> value %d, ret <%d>",
            connectionId, Message().Int1(), attribute, value, err)

    // Return if error or the request is pending and will be completed later.
    if ( KErrNone == err )
        {
        // Write "value" back to the client's address space
        TPtrC8 d( reinterpret_cast<TUint8*>( &value ), sizeof( value ) );

        err = Message().Write( KAttributeParameterIndex, d );
        if ( KErrNone != err )
            {
            iCmServer->PanicClient( EBadDescriptor );
            }
        }

    LOGEXITFN1("CConnMonSession::GetBoolAttributeL()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CConnMonSession::GetStringAttributeL
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::GetStringAttributeL()
    {
    LOGENTRFN("CConnMonSession::GetStringAttributeL()")
    TInt err( KErrNone );
    TUint connectionId( Message().Int0() );
    TUint attribute( Message().Int2() );
    LOGIT2("SERVER: EReqGetStringAttribute IN: id %d, attribute %d", connectionId, attribute)

    HBufC16* value = HBufC16::NewL( KConnMonMaxStringAttributeLength );
    CleanupStack::PushL( value );

    TPtr16 auxPtr( const_cast<TUint16*>( value->Des().Ptr() ), KConnMonMaxStringAttributeLength );
    auxPtr.FillZ(); // Fill descriptor with zeros

    // Check attribute
    switch ( attribute )
        {
        case KWlanSsid:
            // Check WLAN support
            err = iCmServer->Iap()->WlanSupportCheck( connectionId );
            if ( KErrNone == err )
                {
                // Get string attribute from WLAN session, max SSID length is 32
                err = iCmServer->Iap()->WlanSupport()->GetStringAttributeL( Message(), auxPtr );
                }
            break;

        default:
            // Call comms APIs
            err = iCmServer->Iap()->GetStringAttribute( Message(), auxPtr );

            // Check if this request should be served by a plug-in engine
            if ( ( connectionId >= EBearerIdGPRS ) &&
                    ( KErrNone != err ) &&
                    ( KRequestPending != err ) )
                {
                RMessage2 msg = Message();
                err = iCmServer->SendRequestToPlugin( EReqGetStringAttribute, msg );
                }
            break;
        }
    // Length of 'value' should be max 63 chars.
    LOGIT5("SERVER: EReqGetStringAttribute OUT: id %d, subConnId %d, attr %d -> value >%s<, ret <%d>",
            connectionId, Message().Int1(), attribute, auxPtr.PtrZ(), err)

    CleanupStack::Pop( value );

    if ( KErrNone == err )
        {
        // Write "value" back to the client's address space
        err = Message().Write( KAttributeParameterIndex, auxPtr );
        if ( err != KErrNone )
            {
            iCmServer->PanicClient( EBadDescriptor );
            }
        }
    delete value;

    LOGEXITFN1("CConnMonSession::GetStringAttributeL()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CConnMonSession::GetPckgAttributeL
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::GetPckgAttributeL()
    {
    LOGENTRFN("CConnMonSession::GetPckgAttributeL()")
    TInt err( KErrNone );

    TUint connectionId( Message().Int0() );
    TUint attribute( Message().Int2() );

    switch ( attribute )
        {
        case KStartTime:
            {
            LOGIT("GetPckgAttributeL: attr == KStartTime")
            TTime time;

            // Call comms APIs
            err = iCmServer->Iap()->GetStartTime( connectionId, time );
            LOGIT2("GetPckgAttributeL: id %d, ret <%d>", connectionId, err)
            if ( KErrNone != err )
                {
                return err;
                }

            // Write "value" back to the client's address space
            TPckgBuf<TTime> timePckg( time );
            err = Message().Write( KAttributeParameterIndex, timePckg );
            if ( KErrNone != err )
                {
                iCmServer->PanicClient( EBadDescriptor );
                return KErrBadDescriptor;
                }

#ifdef _DEBUG
            TBuf<KConnMonMaxStringAttributeLength> buf;
            _LIT( KFormatStartTime, "%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B" );
            time.FormatL( buf, KFormatStartTime );
            LOGIT1("GetPckgAttributeL: universal start time >%s<", buf.PtrZ())
#endif // _DEBUG

            break;
            }

        case KClientInfo:
            {
            LOGIT("GetPckgAttributeL: attr == KClientInfo")
            // Client uids
            TConnMonClientEnum clientEnum;

            clientEnum.iCount = 0;
            for ( TUint i = 0; i < KConnMonMaxClientUids; i++ )
                {
                clientEnum.iUid[i].iUid = 0;
                }

            // Call comms APIs
            err = iCmServer->Iap()->GetUids( connectionId, clientEnum );
            LOGIT2("GetPckgAttributeL: clients %d, ret <%d>", clientEnum.iCount, err)
            if ( KErrNone != err )
                {
                return err;
                }

            // Write "value" back to the client's address space
            TPckgBuf<TConnMonClientEnum> clientEnumPckg( clientEnum );
            err = Message().Write( KAttributeParameterIndex, clientEnumPckg );
            if ( KErrNone != err )
                {
                iCmServer->PanicClient( EBadDescriptor );
                return KErrBadDescriptor;
                }
            break;
            }

        case KNetworkNames:
            {
            LOGIT("GetPckgAttributeL: attr == KNetworkNames")

            // Check if WLAN attribute and WLAN is supported
            err = iCmServer->Iap()->WlanSupportCheck( connectionId );
            if ( KErrNone == err )
                {
                // This attribute generates WLAN scanning via WLAN engine.
                // Async GetScanResults method is called and message will be completed
                // later in session specific WlanSession class object
                err = iCmServer->Iap()->WlanSupport()->GetNetworkNamesL( Message() );
                }
            return err;
            }

        case KWLANNetworks:
            {
            LOGIT("GetPckgAttributeL: attr == KWLANNetworks")

            // Check buffer max length
            TInt maxSize( Message().GetDesMaxLength( KAttributeParameterIndex ) );
            if ( KErrBadDescriptor == maxSize )
                {
                LOGIT("GetPckgAttributeL: error, bad descriptor argument")
                iCmServer->PanicClient( EBadDescriptor );
                return KErrBadDescriptor;
                }

            // Buffer's max length needs to be at least 2 beacause
            // minimum information to be returned is the count of all
            // networks and the count of returned networks.
            if ( maxSize < 2 )
                {
                LOGIT1("GetPckgAttributeL: buffer too small %d", maxSize)
                return KErrArgument;
                }

            // Get WLAN networks
            RConnMonWLANNetworksArray wlanNetworks;
            CleanupClosePushL( wlanNetworks );
            iCmServer->Iap()->WlanSupport()->GetWLANNetworksL( wlanNetworks );

            ConnMonWLANNetworksArrayPckg arr( wlanNetworks, maxSize );
            CleanupStack::PopAndDestroy( &wlanNetworks );

            if ( !arr.Buf() )
                {
                return KErrNoMemory;
                }

            // Write result back to the client
            TInt res = Message().Write( KAttributeParameterIndex, ( *arr.Buf() ) );
            if ( KErrNone != res )
                {
                iCmServer->PanicClient( EBadDescriptor );
                return KErrBadDescriptor;
                }

            break;
            }

        case KWlanNetworks:
            {
            LOGIT("GetPckgAttributeL: attr == KWlanNetworks")

            // Check if WLAN attribute and WLAN is supported
            err = iCmServer->Iap()->WlanSupportCheck( connectionId );
            if ( KErrNone != err )
                {
                return err;
                }

            // Check buffer max length
            TInt maxSize( Message().GetDesMaxLength( KAttributeParameterIndex ) );
            if ( KErrBadDescriptor == maxSize )
                {
                LOGIT("GetPckgAttributeL: error, bad descriptor argument")
                iCmServer->PanicClient( EBadDescriptor );
                return KErrBadDescriptor;
                }

            // Buffer's max length needs to be at least 2 because
            // minimum information to be returned is the count of all
            // networks and the count of returned networks.
            if ( maxSize < 2 )
                {
                LOGIT1("GetPckgAttributeL: buffer too small %d", maxSize)
                return KErrArgument;
                }

            // This attribute generates WLAN scanning via WLAN engine.
            // Async GetScanResults method is called and message will be completed
            // later in session specific WlanSession class object
            err = iCmServer->Iap()->WlanSupport()->GetWlanNetworksL( Message() );
            return err;
            }

        case KWlanSsidNetworks:
            {
            LOGIT("GetPckgAttributeL: attr == KWlanSsidNetworks")

            // Check if WLAN attribute and WLAN is supported
            err = iCmServer->Iap()->WlanSupportCheck( connectionId );
            if ( KErrNone != err )
                {
                return err;
                }

            // Check buffer max length
            TInt maxSize( Message().GetDesMaxLength( KAttributeParameterIndex ) );
            if ( KErrBadDescriptor == maxSize )
                {
                LOGIT("GetPckgAttributeL: error, bad descriptor argument")
                iCmServer->PanicClient( EBadDescriptor );
                return KErrBadDescriptor;
                }

            // Buffer's max length needs to be at least 2 because
            // minimum information to be returned is the count of all
            // networks and the count of returned networks.
            if ( maxSize < 2 )
                {
                LOGIT1("GetPckgAttributeL: buffer too small %d", maxSize)
                return KErrArgument;
                }

            // This attribute generates WLAN scanning via WLAN engine.
            // Async GetScanResults method is called and message will be completed
            // later in session specific WlanSession class object
            err = iCmServer->Iap()->WlanSupport()->GetWlanSsidNetworksL( Message() );
            return err;
            }

        case KWlanCurrentNetwork:
            {
            LOGIT("GetPckgAttributeL: attr == KCurrentWlanNetwork")

            // Check if WLAN attribute and WLAN is supported
            err = iCmServer->Iap()->WlanSupportCheck( connectionId );
            if ( err != KErrNone )
                {
                return err;
                }

            // Check buffer max length
            TInt maxSize( Message().GetDesMaxLength( KAttributeParameterIndex ) );
            if ( KErrBadDescriptor == maxSize )
                {
                LOGIT("GetPckgAttributeL: error, bad descriptor argument")
                iCmServer->PanicClient( EBadDescriptor );
                return KErrBadDescriptor;
                }

            // Buffer's max length needs to be at least 2 because
            // minimum information to be returned is the count of all
            // networks and the count of returned networks.
            if ( maxSize < 2 )
                {
                LOGIT1("GetPckgAttributeL: buffer too small %d", maxSize)
                return KErrArgument;
                }

            RConnMonWlanNetworksPtrArray wlanNetwork;
            ConnMonCleanupResetAndDestroyPushL( wlanNetwork );

            err = iCmServer->Iap()->WlanSupport()->GetCurrentWlanNetworkL( wlanNetwork );
            if ( err != KErrNone )
                {
                CleanupStack::PopAndDestroy( &wlanNetwork );
                return err;
                }

            CConnMonWlanNetworksPtrArrayPckg arr( wlanNetwork, maxSize );
            CleanupStack::PopAndDestroy( &wlanNetwork );

            if ( !arr.Buf() )
                {
                return KErrNoMemory;
                }

            // Write "value" back to the client's address space
            err = Message().Write( KAttributeParameterIndex, ( *arr.Buf() ) );
            if ( err != KErrNone )
                {
                // Panic client
                iCmServer->PanicClient( EBadDescriptor );
                }
            return err;
            }

        case KWlanProbeRawBuffers:
            {
            LOGIT("GetPckgAttributeL: attr == KWlanProbeRawBuffers")

            // Check if WLAN attribute and WLAN is supported
            err = iCmServer->Iap()->WlanSupportCheck( connectionId );
            if ( KErrNone != err )
                {
                return err;
                }

            // Check buffer max length
            TInt maxSize( Message().GetDesMaxLength( KAttributeParameterIndex ) );
            if ( KErrBadDescriptor == maxSize )
                {
                LOGIT("GetPckgAttributeL: error, bad descriptor argument")
                iCmServer->PanicClient( EBadDescriptor );
                return KErrBadDescriptor;
                }

            // Buffer's max length needs to be at least 2 because
            // minimum information to be returned is the count of all
            // networks and the count of returned networks.
            if ( maxSize < 2 )
                {
                LOGIT1("GetPckgAttributeL: buffer too small %d", maxSize)
                return KErrArgument;
                }

            // This attribute generates WLAN scanning via WLAN engine.
            // Async GetScanResults method is called and message will be completed
            // later in session specific WlanSession class object
            err = iCmServer->Iap()->WlanSupport()->GetWlanProbeRawBuffersL( Message() );
            return err;
            }

        case KIapAvailability:
            {
            LOGIT1("GetPckgAttributeL: KIapAvailability, id %d", connectionId)
            TConnMonIapInfo iaps;

            // Connection ID attribute validity check
            if ( ( connectionId != 0 ) &&
                    ( ( connectionId < EBearerIdGPRS ) ||
                    ( connectionId > EBearerIdVirtualVPN ) ) )
                {
                err = KErrArgument;
                }
            else if ( ( connectionId == EBearerIdGSM ) ||
                    ( connectionId == EBearerIdCDMA2000 ) )
                {
                err = KErrArgument;
                }

            if ( !err )
                {
                // Enable WLAN scan only if asking for all, WLAN or virtual IAPs
                if ( ( connectionId == EBearerIdAll ) ||
                        ( connectionId == EBearerIdWLAN ) ||
                        ( connectionId == EBearerIdVirtualVPN ) )
                    {
                    iCmServer->Iap()->EnableWlanScan();
                    }

                // If WLAN background scan is ON and request is asking for all-
                // or virtual iaps, ConnMon will send availability changed
                // event if availability change is detected.
                err = iCmServer->GetAvailableIaps( connectionId, iaps );
                }

            LOGIT2("SERVER: GetPckgAttributeL: -> IAP count %d, err <%d>", iaps.iCount, err)
            if ( err )
                {
                return err;
                }

            // Write result back to client
            TConnMonIapInfoBuf iapBuf( iaps );
            err = Message().Write( KAttributeParameterIndex, iapBuf );
            if ( err )
                {
                iCmServer->PanicClient( EBadDescriptor );
                return KErrBadDescriptor;
                }
            }
            break;

        case KSNAPsAvailability:
            {
            LOGIT("GetPckgAttributeL: KSNAPsAvailability")
            TConnMonSNAPInfo snaps;

            iCmServer->Iap()->EnableWlanScan();

            // If WLAN background scan is ON, ConnMon will send availability
            // changed event if availability change is detected.
            iCmServer->GetAvailableSnaps( snaps );

            // Write result back to the client
            TConnMonSNAPInfoBuf snapBuf( snaps );

            err = Message().Write( KAttributeParameterIndex, snapBuf );
            if ( err )
                {
                iCmServer->PanicClient( EBadDescriptor );
                return KErrBadDescriptor;
                }
            }
            break;

        case KAvailableSNAPsIds:
            {
            LOGIT("GetPckgAttributeL: KAvailableSNAPsIds")

            TInt maxSize( Message().GetDesMaxLength( KAttributeParameterIndex ) ); // Max buffer length
            if (  KErrBadDescriptor == maxSize )
                {
                LOGIT("SERVER: EReqGetPckgAttribute -> KAvailableSNAPsIds: bad descriptor")
                iCmServer->PanicClient( EBadDescriptor );
                return KErrBadDescriptor;
                }
            // Buffer's max length needs to be at least 2 beacause minimum
            // information to be returned is the count of all networks and
            // the count of returned networks.
            if ( maxSize < 2 )
                {
                LOGIT("SERVER: EReqGetPckgAttribute -> KAvailableSNAPsIds: descriptor too small")
                return KErrArgument;
                }

            iCmServer->Iap()->EnableWlanScan();

            // If WLAN background scan is ON, ConnMon will send availability
            // changed event if availability change is detected.
            RArray<TConnMonId> availableSnapIds;
            err = iCmServer->GetAvailableSnaps( availableSnapIds );
            if ( err )
                {
                availableSnapIds.Close();
                return err;
                }

            ConnMonIdsArrayPckg arr( availableSnapIds, maxSize );
            availableSnapIds.Close();
            if ( !arr.Buf() )
                {
                return KErrNoMemory;
                }

            err = Message().Write( KAttributeParameterIndex, (*arr.Buf()) );
            if ( err )
                {
                iCmServer->PanicClient( EBadDescriptor );
                return KErrBadDescriptor;
                }
            }
            break;

        case KBearerGroupInfo:
            {
            LOGIT("GetPckgAttributeL: KBearerGroupInfo")

            TBearerInfo bearerInfo;
            TInt err = iCmServer->Iap()->GetBearerInfo( connectionId, bearerInfo );
            if ( KErrNone != err )
                {
                return err;
                }
            LOGIT1("CConnMonSession::GetPckgAttributeL, bearer %d", bearerInfo.iBearer)

            TUint bearerMask( 0 );
            TUint bearerMask2( 0 );

            err = iCmServer->BearerGroupManager()->GetBearerGroups(
                    (TConnMonBearerInfo)bearerInfo.iBearer,
                    bearerMask,
                    bearerMask2 );
            if ( KErrNone != err )
                {
                return err;
                }
            LOGIT2("CConnMonSession::GetPckgAttributeL, bearerMask %d, bearerMask2 %d", bearerMask, bearerMask2)

            TConnMonBearerGroupInfo groupInfo;
            groupInfo.iInternal = bearerInfo.iInternal;
            groupInfo.iBearerGroups = bearerMask;
            groupInfo.iBearerGroups2 = bearerMask2;

            TConnMonBearerGroupInfoBuf groupInfoBuf( groupInfo );
            err = Message().Write( KAttributeParameterIndex, groupInfoBuf );
            if ( KErrNone != err )
                {
                LOGIT1("GetPckgAttributeL: error writing result to client <%d>", err)
                iCmServer->PanicClient( EBadDescriptor );
                return KErrBadDescriptor;
                }

            break;
            }

        default:
            {
            LOGIT("GetPckgAttributeL: UNKNOWN attribute")
            return KErrNotSupported;
            }
        }

    LOGEXITFN("CConnMonSession::GetPckgAttributeL()")
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CConnMonSession::SetIntAttribute
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::SetIntAttributeL()
    {
    LOGENTRFN("CConnMonSession::SetIntAttributeL()")
    TInt err( KErrNone );

    TUint connectionId( Message().Int0() );
    TUint attribute( Message().Int2() );
    TUint value( Message().Int3() );

    if ( KWlanScanCacheLifetime == attribute )
        {
        // Check WLAN support for the given id
        err = iCmServer->Iap()->WlanSupportCheck( connectionId );
        if ( KErrNone == err )
            {
            err = iCmServer->Iap()->WlanSupport()->SetIntAttributeL( Message(), value );
            }
        }
    else if ( connectionId >= EBearerIdGPRS )
        {
        RMessage2 msg = Message();
        err = iCmServer->SendRequestToPlugin( EReqSetIntAttribute, msg );
        }
    else
        {
        err = KErrNotSupported;
        }

    LOGEXITFN1("CConnMonSession::SetIntAttributeL()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CConnMonSession::SetUintAttribute
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::SetUintAttributeL()
    {
    LOGENTRFN("CConnMonSession::SetUintAttributeL()")
    TInt err( KErrNone );

    TUint connectionId( Message().Int0() );
    TUint attribute( Message().Int2() );
    TUint value( Message().Int3() );

    switch ( attribute )
        {
        case KDownlinkDataThreshold:
        case KUplinkDataThreshold:
        case KActivityTimeThreshold:
            {
            if ( connectionId > KMaxConnectionId )
                {
                err = KErrArgument;
                }
            else if ( !iCmServer->Iap()->ConnectionExists( connectionId ) )
                {
                err = KErrNotFound;
                }
            else
                {
                err = AdjustThresholdToAllowedRange( connectionId, attribute, value );
                if ( KErrNone == err )
                    {
                    TConnSettings settings( 0, 0, 0 );
                    GetConnectionSettings( connectionId, settings );
                    if ( attribute == KDownlinkDataThreshold )
                        {
                        settings.iDLDataThreshold = value;
                        }
                    else if ( attribute == KUplinkDataThreshold )
                        {
                        settings.iULDataThreshold = value;
                        }
                    else // KActivityTimeThreshold
                        {
                        settings.iActivityTimeThreshold = value;
                        }
                    err = SetConnectionSettings( connectionId, settings );
                    }
                }
            }
            break;

        case KBearerAvailabilityThreshold:
            err = SetPluginThreshold( EBearerAvailabilityThreshold, value );
            if ( err != KErrNone )
                {
                // Set anyway
                iBearerAvailabilityThreshold = value;
                err = KErrNone;
                }
            if ( iListening && ( iBearerAvailabilityThreshold > 0 ) )
                {
                iCmServer->Iap()->WakeUpNotifications();
                }
            break;

        case KSignalStrengthThreshold:
            iSignalStrengthThreshold = value;
            if ( iListening && ( iSignalStrengthThreshold > 0 ) )
                {
                iCmServer->Iap()->WakeUpNotifications();
                }
            err = KErrNone;
            break;

        case KBearerGroupThreshold:
            iBearerGroupThreshold = value;
            LOGIT1("SetUintAttributeL: bearer group threshold set %d", iBearerGroupThreshold)
            break;

        case KWlanScanMaxDelay:
            // Check WLAN support for the given id
            err = iCmServer->Iap()->WlanSupportCheck( connectionId );
            if ( KErrNone == err )
                {
                err = iCmServer->Iap()->WlanSupport()->SetUintAttributeL( Message(), value );
                }
            break;

        default:
            LOGIT1("SERVER: CConnMonSession::SetUintAttributeL() default case %d", attribute)
            // Call comms APIs
            err = iCmServer->Iap()->SetUintAttribute( Message(), value );

            if ( ( Message().Int0() >= EBearerIdGPRS ) && ( err != KErrNone ) )
                {
                RMessage2 msg = Message();
                err = iCmServer->SendRequestToPlugin( EReqSetUintAttribute, msg );
                }
            break;
        }

    LOGEXITFN1("CConnMonSession::SetUintAttributeL()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonSession::SetBoolAttributeL
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::SetBoolAttribute()
    {
    LOGENTRFN("CConnMonSession::SetBoolAttribute()")
    TInt err( KErrNotSupported );
    TUint attribute( Message().Int2() );
    LOGIT3("SERVER: EReqSetBoolAttribute IN: id %d, attr %d, value %d",
            Message().Int0(), attribute, Message().Int3())

    if ( ( attribute == KConnectionStop ) || ( attribute == KConnectionStopAll ) )
        {
        err = iCmServer->Iap()->SetBoolAttribute( Message() );
        }
    else if ( Message().Int0() >= EBearerIdGPRS )
        {
        RMessage2 msg = Message();
        err = iCmServer->SendRequestToPlugin( EReqSetBoolAttribute, msg );
        }

    LOGEXITFN1("CConnMonSession::SetBoolAttribute()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CConnMonSession::SetStringAttribute
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::SetStringAttributeL()
    {
    LOGENTRFN("CConnMonSession::SetStringAttributeL()")
    TInt err( KErrNone );
    TUint connectionId( Message().Int0() );
    TUint attribute( Message().Int2() );
    LOGIT2("SERVER: EReqSetStringAttribute: id %d, attr %d", connectionId, attribute)

    // Check attribute
    switch ( attribute )
        {
        case KWlanSsid:
            {
            // Check WLAN support
            err = iCmServer->Iap()->WlanSupportCheck( connectionId );
            if ( KErrNone == err )
                {
                // Get descriptor argument length
                TInt size = Message().GetDesLength( KAttributeParameterIndex );
                if ( size < 0 )
                    {
                    LOGIT("SetStringAttributeL: error with descriptor argument")
                    err = size; // KErrArgument or KErrBadDescriptor
                    }
                else if ( size > CConnMonWlanNetwork::KMaxNameLength )
                    {
                    LOGIT1("SetStringAttributeL: string too long %d", size)
                    err = KErrTooBig;
                    }
                else
                    {
                    // Read descriptor argument from message
                    TBuf<CConnMonWlanNetwork::KMaxNameLength> value;
                    Message().Read( KAttributeParameterIndex, value );

                    #ifdef _DEBUG
                    // One letter longer than string, since we are adding a '\0' when printing
                    TBuf<CConnMonWlanNetwork::KMaxNameLength + 1> debugName( value );
                    LOGIT2("SetStringAttributeL: string >%s<, length %d", debugName.PtrZ(), value.Length())
                    #endif // _DEBUG

                    // Set string
                    err = iCmServer->Iap()->WlanSupport()->SetStringAttributeL( Message(), value );
                    }
                }
            }
            break;

        default:
            err = KErrNotSupported;
            break;
        }

    LOGEXITFN1("CConnMonSession::SetStringAttributeL()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonSession::SetPckgAttribute
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::SetPckgAttribute() const
    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CConnMonSession::CancelAsyncRequest
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::CancelAsyncRequest()
    {
    LOGENTRFN("CConnMonSession::CancelAsyncRequest()")

    // Defined in TConnMonAsyncRequest
    TInt requestToCancel = Message().Int0();
    LOGIT1("SERVER: EReqCancelAsyncRequest, req %d", requestToCancel)

    if ( ( EConnMonGetPckgAttribute == requestToCancel ) ||
         ( EConnMonGetBoolAttribute == requestToCancel ) ||
         ( EConnMonGetIntAttribute == requestToCancel ) )
        {
        // Check WLAN support
        if ( iCmServer->Iap()->WlanSupport() )
            {
            // Cancel possible wlan requests
            iCmServer->Iap()->WlanSupport()->CancelAsyncRequest( Message() );
            }
        }

    if ( EConnMonGetUintAttribute == requestToCancel )
        {
        // Complete all pending data volume requests with status 'KErrCancel'
        for ( TInt i = 0; i < iConnParams.Count(); i++ )
            {
            CompleteDataVolumeRequests( iConnParams[i].iConnectionId, 0, 0, KErrCancel );
            }
        }
    else if ( EConnMonGetBoolAttribute == requestToCancel )
        {
        // Complete all pending activity requests with status 'KErrCancel'
        for ( TInt i = 0; i < iConnParams.Count(); i++ )
            {
            CompleteActivityRequests( iConnParams[i].iConnectionId, EFalse, KErrCancel );
            }
        }

    // Complete all pending plugin requests (of type Message().Int0()) with 'KErrCancel'
    RThread clientThread;

    TInt err = Message().Client( clientThread );
    if ( KErrNone == err )
        {
        iCmServer->CancelAttributeRequestsFromPlugins( clientThread, Message().Int0() );
        clientThread.Close();
        }

    LOGEXITFN1("CConnMonSession::CancelAsyncRequest()", err)
    return err;

    // It is not possible to cancel other async requests at the moment because they
    // are completed right away in the server side. Note. the client's request for a new
    // event (EReqReceiveEvent) is cancelled by calling CancelNotifications()
    }

// -----------------------------------------------------------------------------
// CConnMonSession::ReceiveEventL
// -----------------------------------------------------------------------------
//
void CConnMonSession::ReceiveEventL( const TAny* aBuffer )
    {
    __ASSERT_DEBUG( !iReceiveActive, PanicServer( EReceiveAlreadyActive ) );

    //LOGIT("SERVER: EReqReceiveEvent")

    if ( !iListening )
        {
        // Set status first
        iListening = ETrue;

        // Start event notifiers if not already there
        iCmServer->Iap()->ListenL();

        // Want events from now on...
        iLastEventTime = iCmServer->EventQueue()->LastEventTime();

        // Set the threshold to the plugin (to start the plugin events)
        if ( iBearerAvailabilityThreshold == 1 )
            {
            iBearerAvailabilityThreshold = 0; // To cause a change 0 -> 1
            SetPluginThreshold( EBearerAvailabilityThreshold, 1 );
            }
        }

    // Remember the 'receive request'
    iReceiveMessage = Message();
    iReceiveBuffer  = aBuffer;
    iReceiveActive  = ETrue;

    // Check for immediate fulfilment
    iCmServer->EventQueue()->CheckEvents( this );
    }

// -----------------------------------------------------------------------------
// CConnMonSession::CancelReceiveEvent
// -----------------------------------------------------------------------------
//
void CConnMonSession::CancelReceiveEvent()
    {
    if ( iReceiveActive )
        {
        LOGIT("SERVER: EReqCancelReceiveEvent")
        iReceiveMessage.Complete( KErrCancel );
        iReceiveActive = EFalse;
        }

    // Stop event watchers (if this is the last session wanting events)
    if ( iListening )
        {
        // Reset plugin's threshold relating to availability events
        // (to stop the plugin events when not needed)
        if ( iBearerAvailabilityThreshold == 1 )
            {
            SetPluginThreshold( EBearerAvailabilityThreshold, 0 );
            iBearerAvailabilityThreshold = 1; // Restore
            }

        // Stop listening
        iCmServer->Iap()->CancelListen();
        iListening = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CConnMonSession::IsListening
// -----------------------------------------------------------------------------
//
TBool CConnMonSession::IsListening() const
    {
    return iListening;
    }

// -----------------------------------------------------------------------------
// CConnMonSession::CanReceiveEvent
// -----------------------------------------------------------------------------
//
TBool CConnMonSession::CanReceiveEvent() const
    {
    return iReceiveActive;
    }

// -----------------------------------------------------------------------------
// CConnMonSession::SendEventToClient
// -----------------------------------------------------------------------------
//
TBool CConnMonSession::SendEventToClient( const TEvent& aEvent )
    {
    if ( aEvent.iTime > iLastEventTime )
        {
        // Client wants only new events
        iLastEventTime = aEvent.iTime;

        switch ( aEvent.iInfo.iEventType )
            {
            case EConnMonDownlinkDataThreshold:
            case EConnMonUplinkDataThreshold:
                {
                TUint threshold( 0 );
                TUint smallestThreshold( 0 );
                CalculateAdjustedThreshold( aEvent.iInfo, threshold, smallestThreshold );

                if ( ( threshold > 0 ) && ( smallestThreshold > 0 ) )
                    {
                    TUint rounded = smallestThreshold * ( aEvent.iInfo.iData / smallestThreshold );
                    if ( ( aEvent.iInfo.iData % smallestThreshold ) > ( smallestThreshold / 2 ) )
                        {
                        rounded += smallestThreshold;
                        }
                    if ( rounded % threshold != 0 )
                        {
                        return EFalse;
                        }
                    }
                else
                    {
                    return EFalse;
                    }
                }
                break;

            case EConnMonConnectionActivityChange:
                {
                TConnSettings settings( 0, 0, 0 );
                GetConnectionSettings( aEvent.iInfo.iConnectionId, settings );
                if ( settings.iActivityTimeThreshold == 0 )
                    {
                    return EFalse;
                    }
                }
                break;

            case EConnMonBearerAvailabilityChange:
                {
                if ( iBearerAvailabilityThreshold == 0 )
                    {
                    return EFalse;
                    }
                }
                break;

            case EConnMonSignalStrengthChange:
                {
                if ( iSignalStrengthThreshold == 0 )
                    {
                    return EFalse;
                    }
                }
                break;

            case EConnMonBearerInfoChange:
            case EConnMonBearerChange:
                {
                // Send only either EConnMonBearerChange or EConnMonBearerInfoChange event

                if ( iBearerGroupThreshold == 0 &&
                        aEvent.iInfo.iEventType == EConnMonBearerInfoChange )
                    {
                    return EFalse;
                    }
                else if ( iBearerGroupThreshold > 0 &&
                        aEvent.iInfo.iEventType == EConnMonBearerChange )
                    {
                    return EFalse;
                    }
                }
                break;

            case EConnMonBearerGroupChange:
                {
                if ( iBearerGroupThreshold == 0 )
                    {
                    return EFalse;
                    }
                }
                break;

            case EConnMonIapAvailabilityChange:
            case EConnMonSNAPsAvailabilityChange:
                {
                if ( aEvent.iExtra.iPtr )
                    {
                    TPtrC8 tempDes( aEvent.iExtra.iPtr, aEvent.iExtra.iSize );
                    TConnMonExtraBuf notiBuf( tempDes );
                    TInt err = iReceiveMessage.Write( 1, notiBuf );
                    if ( err != KErrNone )
                        {
                        iCmServer->PanicClient( EBadDescriptor );
                        iReceiveActive = EFalse;
                        return ETrue;
                        }
                    }
                }
                break;

            default:
                break;
            }

        // Send the event info to the client
        TPtr8 n( (TUint8*) &( aEvent.iInfo ), sizeof( TEventInfo ), sizeof( TEventInfo ) );

        TInt res = iReceiveMessage.Write( 0, n );
        if ( res != KErrNone )
            {
            iCmServer->PanicClient( EBadDescriptor );
            }
        else
            {
            // Complete the message
            iReceiveMessage.Complete( KErrNone );
            }

        iReceiveActive = EFalse;
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CConnMonSession::CompleteDataVolumeRequests
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::CompleteDataVolumeRequests(
        const TUint& aConnectionId,
        const TUint& aDlVolume,
        const TUint& aUlVolume,
        const TInt&  aStatus )
    {
    TInt index( 0 );

    // Find requests for the current connection
    for ( index = 0; index < iConnParams.Count(); index++ )
        {
        if ( iConnParams[index].iConnectionId == aConnectionId )
            {
            break;
            }
        }

    if ( index == iConnParams.Count() )
        {
        return KErrArgument;
        }

    // Send values to the the client
    // Loop to find any pending data volume requests on this connection
    for ( TInt i = 0; i < iConnParams[index].iRequests.Count(); i++ )
        {
        TConnRequest* req = &( iConnParams[index].iRequests[i] );
        TInt err( KErrNone );

        if ( req->iType == KDownlinkData || req->iType == KUplinkData )
            {
            if ( ( aStatus == KErrNone ) || ( aStatus == KErrCancel ) )
                {
                if ( req->iType == KDownlinkData )
                    {
                    TPtr8 n( (TUint8*) &aDlVolume, sizeof( TUint ), sizeof( TUint ) );

                    err = req->iMessage.Write( KAttributeParameterIndex, n );
                    }
                else
                    {
                    TPtr8 m( (TUint8*) &aUlVolume, sizeof( TUint ), sizeof( TUint ) );

                    err = req->iMessage.Write( KAttributeParameterIndex, m );
                    }

                if ( err != KErrNone )
                    {
                    iCmServer->PanicClient( req->iMessage, EBadDescriptor );
                    }
                }

            // Complete the request
            req->iMessage.Complete( aStatus );

            // Remove the completed request from the array
            iConnParams[index].iRequests.Remove( i );
            i--;
            }
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CConnMonSession::CompleteActivityRequests
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::CompleteActivityRequests(
        const TUint& aConnectionId,
        const TBool& aActivity,
        const TInt&  aStatus )
    {
    TInt index( 0 );

    // Find requests for the current connection
    for ( index = 0; index < iConnParams.Count(); index++ )
        {
        if ( iConnParams[index].iConnectionId == aConnectionId )
            {
            break;
            }
        }

    if ( index == iConnParams.Count() )
        {
        return KErrArgument;
        }

    // Send values to the the client
    // Loop to find any pending activity requests on this connection
    for ( TInt i = 0; i < iConnParams[index].iRequests.Count(); i++ )
        {
        TConnRequest* req = &( iConnParams[index].iRequests[i] );
        TInt err( KErrNone );

        if ( req->iType == KConnectionActive )
            {
            if ( aStatus == KErrNone )
                {
                TPtr8 n( (TUint8*) &aActivity, sizeof( TBool ), sizeof( TBool ) );

                err = req->iMessage.Write( KAttributeParameterIndex, n );
                if ( err != KErrNone )
                    {
                    iCmServer->PanicClient( req->iMessage, EBadDescriptor );
                    }
                }

            // Complete the request
            req->iMessage.Complete( aStatus );

            // Remove the completed request from the array
            iConnParams[index].iRequests.Remove( i );
            i--;
            }
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CConnMonSession::SetConnectionSettings
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::SetConnectionSettings(
        const TUint aConnId,
        const TConnSettings& aSettings )
    {
    if ( aConnId <= KMaxConnectionId )
        {
        if ( !IsConnectionValid( aConnId ) )
            {
            RemoveConnectionParams( aConnId );
            return KErrNotFound;
            }
        }
    else
        {
        return KErrArgument;
        }

    TConnParams connParams( aConnId );
    TInt index = iConnParams.Find( connParams, TConnParams::MatchId );

    if ( KErrNotFound == index )
        {
        connParams.iSettings = aSettings;
        iConnParams.Append( connParams );
        }
    else
        {
        iConnParams[index].iSettings = aSettings;
        }

    if ( iListening )
        {
        iCmServer->Iap()->WakeUpNotifications( aConnId );
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CConnMonSession::GetConnectionSettings
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::GetConnectionSettings(
        const TUint aConnId,
        TConnSettings& aSettings )
    {
    TInt index( 0 );

    for ( index = 0; index < iConnParams.Count(); index++ )
        {
        if ( iConnParams[index].iConnectionId == aConnId )
            {
            break;
            }
        }

    if ( index < iConnParams.Count() )
        {
        aSettings = iConnParams[index].iSettings;
        }
    else
        {
        return KErrNotFound;
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CConnMonSession::SetConnectionRequest
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::SetConnectionRequest(
        const TUint aConnId,
        const TConnRequest& aRequest )
    {
    if ( !IsConnectionValid( aConnId ) )
        {
        RemoveConnectionParams( aConnId );
        return KErrNotFound;
        }

    TConnParams connParams( aConnId );
    TInt index = iConnParams.Find( connParams, TConnParams::MatchId );

    if ( KErrNotFound == index )
        {
        connParams.iRequests.Append( aRequest );
        iConnParams.Append( connParams );
        }
    else
        {
        iConnParams[index].iRequests.Append( aRequest );
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CConnMonSession::RemoveConnectionParams
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::RemoveConnectionParams( const TUint aConnId )
    {
    TInt index( 0 );

    for ( index=0; index < iConnParams.Count(); index++ )
        {
        if ( iConnParams[index].iConnectionId == aConnId )
            {
            break;
            }
        }

    if ( index == iConnParams.Count() )
        {
        return KErrNotFound;
        }
    else
        {
        // Complete any outstanding requests on this connection
        CompleteDataVolumeRequests( aConnId, 0, 0, KErrDisconnected );
        CompleteActivityRequests( aConnId, EFalse, KErrDisconnected );

        // Free resources
        iConnParams[index].iRequests.Close();

        // Remove the whole entry
        iConnParams.Remove( index );
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CConnMonSession::GetBearerThreshold
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::GetBearerThreshold(
        const TInt& aThresholdType,
        TUint& aThreshold ) const
    {
    if ( !iListening )
        {
        aThreshold = 0;
        return KErrNone;
        }

    if ( aThresholdType == EBearerAvailabilityThreshold )
        {
        aThreshold = iBearerAvailabilityThreshold;
        }
    else if ( aThresholdType == ESignalStrengthThreshold )
        {
        aThreshold = iSignalStrengthThreshold;
        }
    else
        {
        return KErrArgument;
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CConnMonSession::ServicePluginL
// -----------------------------------------------------------------------------
//
void CConnMonSession::ServicePluginL( const RMessage2& aMessage )
    {
    TInt rc( 0 );

    if ( !FeatureManager::FeatureSupported( KFeatureIdConnMonExtension ) )
        {
        aMessage.Complete( KErrNotSupported );
        LOGIT("SERVER: Plugins are not supported")
        return;
        }

    switch ( aMessage.Function() )
        {
        // CREATE PLUGIN
        case EReqPluginCreateSubSession:
            {
            if ( aMessage.Int0() < EBearerIdGPRS )
                {
                aMessage.Complete( KErrArgument );
                }
            else
                {
                NewPluginL( aMessage.Int0() );
                aMessage.Complete( KErrNone );
                LOGIT1("SERVER: Plugin has registered, id %d", aMessage.Int0())
                }

            break;
            }

        // DELETE PLUGIN
        case EReqPluginCloseSubSession:
            {
            DeletePlugin( aMessage.Int3() );
            aMessage.Complete( KErrNone );

            break;
            }

        // SERVICE PLUGIN'S REQUEST
        default:
            {
            CPlugin* plugin = PluginFromHandle( aMessage.Int3() );

            if ( !plugin )
                {
                iCmServer->PanicClient( EConnMonBadHandle );
                break;
                }

            switch ( aMessage.Function() )
                {
                case EReqPluginRegisterAttribute:
                    {
                    TUint attribute( aMessage.Int0() );

                    rc = plugin->RegisterAttribute( attribute );
                    aMessage.Complete( rc );

                    // Threshold can be set to the plugin only after completing the plugin's request
                    if ( ( rc == KErrNone ) && ( attribute == KBearerAvailabilityThreshold ) )
                         {
                         plugin->GetQuery();
                         }
                    break;
                    }
                case EReqPluginCancelRegisterAttribute:
                    {
                    rc = plugin->CancelRegisterAttribute( aMessage.Int0() );
                    aMessage.Complete( rc );
                    break;
                    }
                case EReqPluginEvent:
                    {
                    TEventInfo eventInfo;
                    TUint8* ptrData( NULL );
                    TInt dataSize( 0 );

                    rc = plugin->Event( aMessage, eventInfo, ptrData, dataSize );
                    aMessage.Complete( rc );

                    if ( rc == KErrNone )
                        {
                        // Send the event to the clients
                        iCmServer->EventQueue()->Add( eventInfo, ptrData, dataSize );

                        LOGIT3("SERVER: EVENT from PLUGIN-> id %d, event %d, data %d",
                                eventInfo.iConnectionId, eventInfo.iEventType, eventInfo.iData)

                        if ( ( eventInfo.iConnectionId == EBearerIdLAN ) &&
                             ( eventInfo.iEventType == EConnMonBearerAvailabilityChange ) )
                            {
                            if ( eventInfo.iData == 1 )
                                {
                                // LAN bearer has become available
                                // Check if there are any LAN IAPs.
                                TFullIapInfo* iaps = NULL;
                                iaps = new( ELeave ) TFullIapInfo;
                                CleanupStack::PushL( iaps );

                                CIapSupport* iapSupport = CIapSupport::NewL();
                                CleanupStack::PushL( iapSupport );

                                iapSupport->GetIapsFromCommDbL( EBearerIdLAN, *iaps );

                                CleanupStack::PopAndDestroy( iapSupport );
                                iapSupport = NULL;

                                // Show the UI dialog only if there are LAN IAPs in the commDB.
                                if ( iaps->iCount > 0 )
                                    {
                                    CUiDialogAO* dialog = NULL;
                                    // Show a dialog on the UI and ask the user to select the
                                    // LAN network. CUiDialog is a one shot active object. It
                                    // will save the user's LAN network preference to ConnMonIap.
                                    // Use TRAPD because all SDKs don't have this dialog that is OK.
                                    TRAPD( leaveCode, dialog = CUiDialogAO::NewL( iCmServer ) );

                                    if ( leaveCode == KErrNone )
                                        {
                                        dialog->Start();
                                        }
                                    }
                                CleanupStack::PopAndDestroy( iaps );
                                }
                            else
                                {
                                // LAN bearer is not anymore available

                                // Reset user's LAN network selection
                                iCmServer->Iap()->ResetEndUserLanPreference();

                                LOGIT("CConnMonSession::ServicePluginL triggered HandleAvailabilityChange()")

                                ( (CConnMonServer*)Server() )->AvailabilityManager()
                                        ->HandleAvailabilityChange();
                                }
                            }
                        }
                    else // ( rc != KErrNone )
                        {
                        if ( ptrData )
                            {
                            delete ptrData;
                            }
                        }
                    break;
                    }
                case EReqPluginGetQuery:
                    {
                    rc = plugin->GetQuery( aMessage );

                    // This will be completed later by the CPlugin object
                    break;
                    }
                case EReqPluginCancelGetQuery:
                    {
                    rc = plugin->CancelGetQuery();
                    aMessage.Complete( rc );
                    break;
                    }
                case EReqPluginAttribute:
                    {
                    rc = plugin->Attribute( aMessage );
                    aMessage.Complete( rc );
                    break;
                    }

                default:
                    break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CConnMonSession::PluginFromHandle
// Will return NULL if parameter aHandle is invalid
// -----------------------------------------------------------------------------
//
CPlugin* CConnMonSession::PluginFromHandle( const TUint aHandle )
    {
    CPlugin* plugin = reinterpret_cast<CPlugin*>( iPlugins->At( aHandle ) );
    return plugin;
    }

// -----------------------------------------------------------------------------
// CConnMonSession::NewPluginL
// -----------------------------------------------------------------------------
//
void CConnMonSession::NewPluginL( const TUint aConnectionId )
    {
    // Create a new plugin object
    CPlugin* plugin = CPlugin::NewL( this, aConnectionId );

    // Ensure that the reference count is decremented if we leave.
    // When ref count becomes zero, CObject::Close() deletes the object.
    CleanupClosePushL( *plugin );

    // Add object to object container to generate unique id
    iContainer->AddL( ( CObject* ) plugin );

    // Add object to object index; this returns a unique handle
    TInt handle = iPlugins->AddL( ( CObject* ) plugin );

    // Object is now owned by this session, reference count is correct.
    CleanupStack::Pop( plugin );

    // Write the handle to the client
    TPckg<TInt> handlePckg( handle );

    TInt res = Message().Write( KAttributeParameterIndex, handlePckg );

    if ( res != KErrNone )
        {
        // Removes from object index and destroys the object
        iPlugins->Remove( handle );

        iCmServer->PanicClient( EBadDescriptor );
        return;
        }
    }

// -----------------------------------------------------------------------------
// CConnMonSession::DeletePlugin
// -----------------------------------------------------------------------------
//
void CConnMonSession::DeletePlugin( const TUint aHandle )
    {
    CPlugin* plugin = reinterpret_cast<CPlugin*>( iPlugins->At( aHandle ) );

    if ( !plugin )
        {
        iCmServer->PanicClient( EConnMonBadHandle );
        }
    else
        {
        // Removes from object index and destroys the object
        iPlugins->Remove( aHandle );
        }
    }

// -----------------------------------------------------------------------------
// CConnMonSession::SendRequestToPlugin
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::SendRequestToPlugin( const TInt aType, const RMessage2& aMessage )
    {
    //LOGENTRFN("CConnMonSession::SendRequestToPlugin()")
    TInt ret( KErrNotSupported );

    TInt count = iPlugins->ActiveCount();

    for ( TInt i = 0; i < count; i++ )
        {
        CPlugin* plugin = reinterpret_cast<CPlugin*>( ( *iPlugins )[i] );

        if ( plugin )
            {
            if ( aType == EReqGetIntAttribute ||
                 aType == EReqGetUintAttribute ||
                 aType == EReqGetBoolAttribute ||
                 aType == EReqGetStringAttribute ||
                 aType == EReqGetPckgAttribute )
                {
                ret = plugin->GetAttributeForClient( aType, aMessage );

                // Asynchronous
                if ( ret == KRequestPending )
                    {
                    return ret;
                    }
                }
            else if ( aType == EReqSetIntAttribute ||
                      aType == EReqSetUintAttribute ||
                      aType == EReqSetBoolAttribute ||
                      aType == EReqSetStringAttribute ||
                      aType == EReqSetPckgAttribute ||
                      aType == EReqInternalSetThreshold ||
                      aType == EReqInternalResetThreshold )
                {
                ret = plugin->SetAttributeForClient( aType, aMessage );

                // Synchronous
                if ( ret == KErrNone )
                    {
                    return ret;
                    }
                }
            else
                {
                return KErrNotSupported;
                }
            }
        }

    //LOGEXITFN("CConnMonSession::SendRequestToPlugin()")
    return ret;
    }

// -----------------------------------------------------------------------------
// CConnMonSession::CancelAttributeRequestFromPlugin
// -----------------------------------------------------------------------------
//
void CConnMonSession::CancelAttributeRequestFromPlugin(
        const RThread& aClient,
        const TInt aType )
    {
    TInt count = iPlugins->ActiveCount();

    for ( TInt i = 0; i < count; i++ )
        {
        CPlugin* plugin = reinterpret_cast<CPlugin*>( ( *iPlugins )[i] );

        if ( plugin )
            {
            plugin->CancelGetAttributeForClient( aClient, aType );
            }
        }
    }

// -----------------------------------------------------------------------------
// CConnMonSession::SetPluginThreshold
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::SetPluginThreshold( const TInt aType, const TUint aValue )
    {
    TUint globalThBefore( 0 );
    TUint globalThAfter( 0 );
    TUint connectionId( 0 );
    TBool internal( EFalse );

    if ( !FeatureManager::FeatureSupported( KFeatureIdConnMonExtension ) )
        {
        return KErrNotSupported;
        }

    if ( aType != EBearerAvailabilityThreshold && aType != ESignalStrengthThreshold )
        {
        return KErrNotSupported;
        }

    // At the moment a common threshold is used for all the bearers.
    // When 'internal' is true threshold will be set in every plugin.
    internal = ETrue;

    // Get the global threshold before changing session threshold
    iCmServer->CalculateThreshold( connectionId, aType, globalThBefore );

    if ( aType == EBearerAvailabilityThreshold )
        {
        // Set the new threshold value
        iBearerAvailabilityThreshold = aValue;
        }
    else if ( aType == ESignalStrengthThreshold )
        {
        // Set the new threshold value
        iSignalStrengthThreshold = aValue;
        }

    // Get the global threshold after changing our threshold
    iCmServer->CalculateThreshold( connectionId, aType, globalThAfter );

    if ( globalThBefore != globalThAfter )
        {
        // Global threshold has changed -> send to plug-in engines
        // Dead code in else-clause is for possible future use. Static analysis exception added.
        // coverity[dead_error_line]
        if ( internal )
            {
            if ( aValue == 1 )
                {
                iCmServer->SendRequestToPlugin( EReqInternalSetThreshold, Message(), ETrue );
                }
            else
                {
                iCmServer->SendRequestToPlugin( EReqInternalResetThreshold, Message(), ETrue );
                }
            }
        else
            {
            iCmServer->SendRequestToPlugin( EReqSetUintAttribute, Message(), ETrue );
            }
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CConnMonSession::IsConnectionValid
// -----------------------------------------------------------------------------
//
TBool CConnMonSession::IsConnectionValid( const TUint aConnId )
    {
    LOGENTRFN("CConnMonSession::IsConnectionValid()")
    TBool result( EFalse );

    // Is connection valid anymore
    TUint subConnectionCount( 0 );
    TInt err = iCmServer->Iap()->GetConnectionInfo( aConnId, subConnectionCount );
    if ( err != KErrNone )
        {
        result = EFalse;
        }
    else
        {
        result = ETrue;
        }

    LOGEXITFN1("CConnMonSession::IsConnectionValid()", result)
    return result;
    }

// -----------------------------------------------------------------------------
// CConnMonSession::RemoveObsoleteConnectionParams
// -----------------------------------------------------------------------------
//
void CConnMonSession::RemoveObsoleteConnectionParams()
    {
    TInt numSettings = iConnParams.Count();
    if ( numSettings > 0 )
        {
        TInt i( 0 );
        do
            {
            if ( !IsConnectionValid( iConnParams[i].iConnectionId ) )
                {
                // Complete any outstanding requests on this connection
                CompleteDataVolumeRequests(
                        iConnParams[i].iConnectionId,
                        0,
                        0,
                        KErrDisconnected );

                CompleteActivityRequests(
                        iConnParams[i].iConnectionId,
                        EFalse,
                        KErrDisconnected );

                // Free resources
                iConnParams[i].iRequests.Close();

                // Remove the whole entry
                iConnParams.Remove( i );
                numSettings = iConnParams.Count();
                }
            else
                {
                ++i;
                }

            } while ( i < numSettings );
        }
    }

// -----------------------------------------------------------------------------
// CConnMonSession::CalculateAdjustedThreshold
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::CalculateAdjustedThreshold(
        const TEventInfo& aEvent,
        TUint& aAdjustedTh,
        TUint& aSmallestTh )
    {
    TConnSettings settings( 0, 0, 0 );
    TUint modulo( 0 );

    aAdjustedTh = 0;
    GetConnectionSettings( aEvent.iConnectionId, settings );

    if ( aEvent.iEventType == EConnMonDownlinkDataThreshold )
        {
        iCmServer->CalculateThreshold(
                aEvent.iConnectionId,
                EDownlinkThreshold,
                aSmallestTh );

        if ( aSmallestTh > 0 )
            {
            modulo = settings.iDLDataThreshold % aSmallestTh;
            }

        if ( modulo == 0 )
            {
            aAdjustedTh = settings.iDLDataThreshold;
            }
        else if ( modulo < aSmallestTh / 2 )
            {
            aAdjustedTh = settings.iDLDataThreshold - modulo;
            }
        else
            {
            aAdjustedTh = settings.iDLDataThreshold + aSmallestTh - modulo;
            }
        }
    else if ( aEvent.iEventType == EConnMonUplinkDataThreshold )
        {
        iCmServer->CalculateThreshold(
                aEvent.iConnectionId,
                EUplinkThreshold,
                aSmallestTh );

        if ( aSmallestTh > 0 )
            {
            modulo = settings.iULDataThreshold % aSmallestTh;
            }

        if ( modulo == 0 )
            {
            aAdjustedTh = settings.iULDataThreshold;
            }
        else if ( modulo < aSmallestTh / 2 )
            {
            aAdjustedTh = settings.iULDataThreshold - modulo;
            }
        else
            {
            aAdjustedTh = settings.iULDataThreshold + aSmallestTh - modulo;
            }
        }
    else
        {
        return KErrArgument;
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CConnMonSession::AdjustThresholdToAllowedRange
// -----------------------------------------------------------------------------
//
TInt CConnMonSession::AdjustThresholdToAllowedRange(
        const TInt& aConnId,
        const TInt& aThresholdType,
        TUint& aThreshold ) const
    {
    LOGENTRFN("CConnMonSession::AdjustThresholdToAllowedRange()")
    TInt err( KErrNone );

    // Zero value is always allowed
    if ( aThreshold == 0 )
        {
        LOGEXITFN1("CConnMonSession::AdjustThresholdToAllowedRange()", err)
        return err;
        }

    switch ( aThresholdType )
        {
        case KActivityTimeThreshold:
            // Already checked for zero earlier
            if ( aThreshold < KMinimumActivityThreshold )
                {
                aThreshold = KMinimumActivityThreshold;
                }
            break;

        // Currently same minimum threshold values for both uplink and downlink
        case KDownlinkDataThreshold:
        case KUplinkDataThreshold:
            {
            TInt bearer( EBearerUnknown );
            TBearerInfo bearerInfo;
            TUint minimum( KMinimumDataThreshold );

            // This method gets the bearer only for an internal connection. This is OK
            // since data volume events are not supported for external connections.
            (void) iCmServer->Iap()->GetBearer( aConnId, bearer, bearerInfo );

            // Set minimum
            if ( bearer == EBearerWLAN || bearer == EBearerLAN )
                {
                minimum = KMinimumWLANDataThreshold;
                }
            else if ( bearer == EBearerGPRS || bearer == EBearerEdgeGPRS )
                {
                minimum = KMinimumGPRSDataThreshold;
                }
            else if ( bearer == EBearerWCDMA )
                {
                minimum = KMinimumWCDMADataThreshold;
                }
            
            // Check threshold
            if ( aThreshold < minimum )
                {
                aThreshold = minimum;
                }
            }
            break;

        case KBearerAvailabilityThreshold:
        case KSignalStrengthThreshold:
        case KBearerGroupThreshold:
            // All alowed
            break;
        default:
            LOGIT1("AdjustThresholdToAllowedRange: Illegal value for threshold type %d", aThresholdType)
            err = KErrNotFound;
            break;
        }

    LOGEXITFN1("CConnMonSession::AdjustThresholdToAllowedRange()", err)
    return err;
    }

// End-of-file
