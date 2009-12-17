/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Plug-in object subsession.
*
*/

#include "ConnMonServ.h"
#include "ConnMonIAP.h"
#include "CEventQueue.h"
#include "CPlugin.h"
#include "log.h"

// ============================ MEMBER FUNCTIONS ===============================

// Constructor
TAttributeRequest::TAttributeRequest(
        TUint aType,
        TUint aAttribute,
        TUint aData )
        :
        iType( aType ),
        iAttribute( aAttribute ),
        iData( aData )
    {
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPlugin::CPlugin
// -----------------------------------------------------------------------------
//
CPlugin::CPlugin()
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CPlugin::~CPlugin()
    {
    // Complete any outstanding plug-in request
    CancelGetQuery();

    // Close the attributes array
    iAttributesSupported.Close();

    // Complete all outstanding client requests
    TInt count = iClientRequests.Count();

    for ( TInt i = 0; i < count; i++ )
        {
        const TClientRequest* req = &( iClientRequests[i] );

        // Complete only asynchronous client requests
        if ( req->iType == EReqGetIntAttribute ||
             req->iType == EReqGetUintAttribute ||
             req->iType == EReqGetBoolAttribute ||
             req->iType == EReqGetStringAttribute ||
             req->iType == EReqGetPckgAttribute )
            {
            req->iMessage.Complete( KErrCancel );
            }
        }

    iClientRequests.Close();

    iSession = 0;

    LOGIT1("SERVER: Plugin destroyed, id <%d>", iPluginId )
    }

// -----------------------------------------------------------------------------
// CPlugin::NewL
// -----------------------------------------------------------------------------
//
CPlugin* CPlugin::NewL( CConnMonSession* aSession, const TUint aPluginId )
    {
    CPlugin* self = new ( ELeave ) CPlugin;

    CleanupStack::PushL( self );
    self->ConstructL( aSession, aPluginId );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CPlugin::ConstructL
// -----------------------------------------------------------------------------
//
void CPlugin::ConstructL( CConnMonSession* aSession, const TUint aPluginId )
    {
    TName name;

    name.Format( KPluginName, this );
    SetNameL( &name );

    iSession = aSession;
    iPluginId = aPluginId;
    }

// -----------------------------------------------------------------------------
// CPlugin::RegisterAttribute
// -----------------------------------------------------------------------------
//
TInt CPlugin::RegisterAttribute( const TUint aAttribute )
    {
    LOGIT2("SERVER: EReqPluginRegisterAttribute, id <%d>, attr <%d>", iPluginId, aAttribute )

    for ( TInt i = 0; i < iAttributesSupported.Count(); i++ )
        {
        if ( iAttributesSupported[i] == aAttribute )
            {
            // Attribute has already been registered
            return ( KErrNone );
            }
        }

    iAttributesSupported.Append( aAttribute );

    // Check if the clients have set 'KBearerAvailabilityThreshold' earlier
    if ( aAttribute == KBearerAvailabilityThreshold )
       {
       TUint th( 0 );

       // Have clients set the threshold earlier
       iSession->iCmServer->CalculateThreshold( 0, EBearerAvailabilityThreshold, th );

       if ( th == 1 )
           {
           // Add the request (set threshold) to the queue (aPending is EFalse)
           const RMessage2 dummy;

           TClientRequest clientReq( EReqInternalSetThreshold, EFalse, dummy );
           iClientRequests.Append( clientReq );
           }
       }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPlugin::CancelRegisterAttribute
// -----------------------------------------------------------------------------
//
TInt CPlugin::CancelRegisterAttribute( const TUint aAttribute )
    {
    LOGIT2("SERVER: EReqPluginCancelRegisterAttribute, id <%d>, attr <%d>", iPluginId, aAttribute )

    for ( TInt i = 0; i < iAttributesSupported.Count(); i++ )
        {
        if ( iAttributesSupported[i] == aAttribute )
            {
            // Complete any outstanding client requets
            TInt count = iClientRequests.Count();

            for ( TInt j = 0; j < count; j++ )
                {
                const TClientRequest* req = &( iClientRequests[j] );

                if  ( static_cast< TInt >( aAttribute ) == req->iMessage.Int2( ) )
                    {
                    // Complete only asynchronous client requests
                    if ( req->iType == EReqGetIntAttribute ||
                         req->iType == EReqGetUintAttribute ||
                         req->iType == EReqGetBoolAttribute ||
                         req->iType == EReqGetStringAttribute ||
                         req->iType == EReqGetPckgAttribute )
                        {
                        req->iMessage.Complete( KErrCancel );
                        }
                    iClientRequests.Remove( j );
                    count--;
                    j--;
                    }
                }

            iAttributesSupported.Remove( i );
            return ( KErrNone );
            }
        }

    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CPlugin::Event
// -----------------------------------------------------------------------------
//
TInt CPlugin::Event(
        const RMessage2& aMessage,
        TEventInfo& aEventInfo,
        TUint8*& aPtr,
        TInt& aSize ) const
    {
    TUint eventId = aMessage.Int0();

    TUint desLen = aMessage.GetDesLength( 1 );

    // Max allowed data size is 512 bytes
    if ( desLen > KConnMonMaxPluginDataSize )
        {
        return KErrTooBig;
        }

    HBufC8* buf = HBufC8::New( desLen );

    if ( !buf )
        {
        return KErrNoMemory;
        }

    TPtr8 initptr = buf->Des();

    // Read the contents of the client pointer into a TPtr.
    TInt res = aMessage.Read( 1, initptr );

    if ( res != KErrNone )
        {
        delete buf;
        iSession->iCmServer->PanicClient( EBadDescriptor );
        return KErrBadHandle;
        }

    // Create the event to be forwarded to the clients
    aEventInfo.Reset();

    aEventInfo.iEventType = eventId;
    aEventInfo.iConnectionId = iPluginId;

    if ( ( eventId == EConnMonSignalStrengthChange ) && ( desLen == sizeof( TInt ) ) )
        {
        // Signal strength changed event
        aEventInfo.iData = static_cast<TInt>( *initptr.Ptr() );
        }
    else if ( ( eventId == EConnMonBearerAvailabilityChange ) && ( desLen == sizeof( TBool ) ) )
        {
        // Bearer availability changed event
        aEventInfo.iData = static_cast<TBool>( *initptr.Ptr() );
        }
    else if ( ( eventId == EConnMonIapAvailabilityChange ) &&
              ( desLen == sizeof( TConnMonIapInfo ) ) )
        {
        // IAP availability changed event
        TConnMonIapInfo* receivedIapInfo =
            const_cast<TConnMonIapInfo*>(
                reinterpret_cast<const TConnMonIapInfo*>( initptr.Ptr() ) );

        // Event contains only limited info on IAPs (= count + IDs)
        TConnMonIapInfo* eventIaps( NULL );
        eventIaps = new TConnMonIapInfo;

        if ( eventIaps )
            {
            aEventInfo.iData = receivedIapInfo->Count();

            eventIaps->iCount = receivedIapInfo->Count();

            for ( TInt i = 0; i < KConnMonMaxIAPCount && i < receivedIapInfo->Count(); i++ )
                {
                eventIaps->iIap[i].iIapId = receivedIapInfo->iIap[i].iIapId;
                }

            aPtr = reinterpret_cast<TUint8*>( eventIaps );
            aSize = sizeof( TConnMonIapInfo );
            }
        }
    else if ( ( eventId >= EConnMonPluginEventBase ) && ( desLen == sizeof( TUint ) ) )
        {
        // Plugin specific event. Only 32-bit data (desLen is 4 bytes) supported so far
        aEventInfo.iData = static_cast<TUint>( *initptr.Ptr() );
        aEventInfo.iData2 = desLen;
        }
    else
        {
        // Event is not supported
        LOGIT2("SERVER: Plugin sent unsupported event or length of the data was invalid. -> id <%d>, event <%d>", iPluginId, eventId )

        delete buf;
        return KErrNotSupported;
        }

    delete buf;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPlugin::Attribute
// -----------------------------------------------------------------------------
//
TInt CPlugin::Attribute( const RMessage2& aMessage )
    {
    TUint attribute( aMessage.Int0() );
    TInt  returnCode( aMessage.Int1() );

    TUint desLen = aMessage.GetDesLength( 2 );

    // Max allowed data size is 512 bytes
    if ( desLen > KConnMonMaxPluginDataSize )
        {
        return KErrTooBig;
        }

    // Allocate a buffer for reading
    HBufC8* buf = HBufC8::New( desLen );
    TPtr8 initptr = buf->Des();

    // read the contents of the client pointer into a TPtr.
    TInt res = aMessage.Read( 2, initptr );

    if ( res != KErrNone )
        {
        delete buf;
        iSession->iCmServer->PanicClient( EBadDescriptor );

        return KErrBadHandle;
        }

    TInt count = iClientRequests.Count();

    // Complete all outstanding client request for this attribute
    for ( TInt i = 0; i < count; i++ )
        {
        const TClientRequest* req = &( iClientRequests[i] );

        if ( static_cast< TInt >( attribute ) == req->iMessage.Int2( ) )
            {
            // Found a request matching the attribute
            switch ( req->iType )
                {
                case EReqGetIntAttribute:
                    {
                    if ( desLen != sizeof( TInt ) )
                        {
                        req->iMessage.Complete( KErrArgument );
                        break;
                        }

                    TInt value = static_cast< TInt >( *initptr.Ptr() );

                    LOGIT4("SERVER: Attribute from plugin, id <%d>, attr <%d>, -> value <%d>, ret <%d>", iPluginId, attribute, value, returnCode )

                    TPtr8 n( reinterpret_cast< TUint8* >( &value ),
                             sizeof( TInt ),
                             sizeof( TInt ) );

                    res = req->iMessage.Write( KAttributeParameterIndex, n );

                    if ( res != KErrNone )
                        {
                        PanicWaitingClient( EBadDescriptor, req->iMessage );
                        }
                    else
                        {
                        req->iMessage.Complete( returnCode );
                        }

                    break;
                    }
                case EReqGetUintAttribute:
                    {
                    if ( desLen != sizeof( TUint ) )
                        {
                        req->iMessage.Complete( KErrArgument );
                        break;
                        }

                    TUint value = static_cast< TUint >( *initptr.Ptr() );

                    LOGIT4("SERVER: Attribute from plugin, id <%d>, attr <%d>, -> value <%d>, ret <%d>", iPluginId, attribute, value, returnCode )

                    TPtr8 n( reinterpret_cast< TUint8* >( &value ),
                             sizeof( TUint ),
                             sizeof( TUint ) );

                    res = req->iMessage.Write( KAttributeParameterIndex, n );

                    if ( res != KErrNone )
                        {
                        PanicWaitingClient( EBadDescriptor, req->iMessage );
                        }
                    else
                        {
                        req->iMessage.Complete( returnCode );
                        }
                    break;
                    }
                case EReqGetBoolAttribute:
                    {
                    if ( desLen != sizeof( TBool ) )
                        {
                        req->iMessage.Complete( KErrArgument );
                        break;
                        }

                    TBool value = static_cast< TBool >( *initptr.Ptr() );

                    LOGIT4("SERVER: Attribute from plugin, id <%d>, attr <%d>, -> value <%d>, ret <%d>", iPluginId, attribute, value, returnCode )

                    TPtr8 n( reinterpret_cast< TUint8* >( &value ),
                             sizeof( TBool ),
                             sizeof( TBool ) );

                    res = req->iMessage.Write( KAttributeParameterIndex, n );

                    if ( res != KErrNone )
                        {
                        PanicWaitingClient( EBadDescriptor, req->iMessage );
                        }
                    else
                        {
                        req->iMessage.Complete( returnCode );
                        }

                    break;
                    }
                 case EReqGetPckgAttribute:
                    {
                    // Write as such to the client area since this is TDes8
                    LOGIT3("SERVER: Attribute from plugin, id <%d>, attr <%d>, -> ret <%d>", iPluginId, attribute, returnCode )

                    res = req->iMessage.Write( KAttributeParameterIndex, initptr );

                    if ( res != KErrNone )
                        {
                        PanicWaitingClient( EBadDescriptor, req->iMessage );
                        }
                    else
                        {
                        req->iMessage.Complete( returnCode );
                        }
                    break;
                    }
                 case EReqGetStringAttribute:
                    {
                    // String is a 16-bit descriptor.
                    // Allocate a 16-bit buffer for reading the string.
                    HBufC* buf16 = HBufC::New( desLen );
                    TPtr initptr16 = buf16->Des();

                    // read the contents of the client pointer into a TPtr.
                    res = aMessage.Read( 2, initptr16 );

                    if ( res != KErrNone )
                        {
                        delete buf;
                        delete buf16;
                        iSession->iCmServer->PanicClient( EBadDescriptor );

                        return KErrBadHandle;
                        }

                    LOGIT3("SERVER: Attribute from plugin, id <%d>, attr <%d>, -> ret <%d>", iPluginId, attribute, returnCode )

                    res = req->iMessage.Write( KAttributeParameterIndex, initptr16 );

                    if ( res != KErrNone )
                        {
                        PanicWaitingClient( EBadDescriptor, req->iMessage );
                        }
                    else
                        {
                        req->iMessage.Complete( returnCode );
                        }

                    delete buf16;

                    break;
                    }
                 default:
                    {
                    delete buf;
                    return KErrNotSupported;
                    }
                } // switch

            // Remove the query because it has been served
            iClientRequests.Remove( i );
            count--;
            i--;
            }  // if
        } // For

    delete buf;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPlugin::GetQuery
// -----------------------------------------------------------------------------
//
TInt CPlugin::GetQuery()
    {
    if ( !iPluginReqActive )
        {
        return KErrNotReady;
        }

    // Check if there is a client request waiting in the queue
    TInt count = iClientRequests.Count();

    for ( TInt i = 0; i < count; i++ )
        {
        if ( !iClientRequests[i].iPending )
            {
            TInt attribute( iClientRequests[i].iMessage.Int2() );
            TInt data( iClientRequests[i].iMessage.Int3() );

            // Send the attribute request to the plug-in
            TAttributeRequest req( iClientRequests[i].iType, attribute, 0 );

            if ( req.iType == EReqSetIntAttribute ||
                 req.iType == EReqSetUintAttribute ||
                 req.iType == EReqSetBoolAttribute )
                {
                req.iData = data;
                }
            else if ( req.iType == EReqInternalSetThreshold )
                {
                req.iType      = EReqSetUintAttribute;
                req.iAttribute = KBearerAvailabilityThreshold;
                req.iData      = 1;
                }
            else if ( req.iType == EReqInternalResetThreshold )
                {
                req.iType      = EReqSetUintAttribute;
                req.iAttribute = KBearerAvailabilityThreshold;
                req.iData      = 0;
                }

            TPtr8 n( reinterpret_cast< TUint8* >( &req ),
                     sizeof( TAttributeRequest ),
                     sizeof( TAttributeRequest ) );

            TInt res = iPluginReqMessage.Write( 0, n );

            if ( res != KErrNone )
                {
                iSession->iCmServer->PanicClient( EBadDescriptor );
                }
            else
                {
                // Complete the message
                iPluginReqMessage.Complete( KErrNone );
                }

            iPluginReqActive=EFalse;

            if ( req.iType == EReqSetIntAttribute ||
                 req.iType == EReqSetUintAttribute ||
                 req.iType == EReqSetBoolAttribute ||
                 req.iType == EReqSetStringAttribute ||
                 req.iType == EReqSetPckgAttribute )
                {
                // This request is synchronous and has already been completed
                iClientRequests.Remove( i );
                count--;
                i--;
                }

            return KErrNone;
            }
        }

    // Queue was empty.
    // Just wait for a client request to arrive.
    return KRequestPending;
    }

// -----------------------------------------------------------------------------
// CPlugin::GetQuery
// -----------------------------------------------------------------------------
//
TInt CPlugin::GetQuery( const RMessage2& aMessage )
    {
    __ASSERT_DEBUG( !iPluginReqActive, PanicServer( EReceiveAlreadyActive ) );

    LOGIT("SERVER: EReqPluginGetQuery")

    // Remember attribute request
    iPluginReqMessage = aMessage;
    iPluginReqBuffer = aMessage.Ptr0();
    iPluginReqActive = ETrue;

    return GetQuery();
    }

// -----------------------------------------------------------------------------
// CPlugin::CancelGetQuery
// -----------------------------------------------------------------------------
//
TInt CPlugin::CancelGetQuery()
    {
    LOGIT("SERVER: EReqPluginCancelGetQuery")

    if ( iPluginReqActive )
        {
        iPluginReqMessage.Complete( KErrCancel );
        iPluginReqActive = EFalse;
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPlugin::GetAttributeForClient
// From a CLIENT.
// -----------------------------------------------------------------------------
//
TInt CPlugin::GetAttributeForClient( const TInt aType, const RMessage2& aMessage )
    {
    LOGENTRFN("CPlugin::GetAttributeForClient()")
    TUint connectionId( aMessage.Int0() );
    TUint attribute( aMessage.Int2() );
    TInt data( aMessage.Int3() );

    if ( aType == EReqInternalSetThreshold || aType == EReqInternalResetThreshold )
        {
        connectionId = iPluginId;
        attribute = KBearerAvailabilityThreshold;
        }

    if ( connectionId <= KMaxConnectionId )
        {
        // Client is asking an attribute using a normal connectionID instead of pluginID.
        TInt bearer( EBearerUnknown );
        TBearerInfo bearerInfo;

        TInt ret = iSession->iCmServer->Iap()->GetBearer( connectionId, bearer, bearerInfo );

        if ( ret == KErrNone )
            {
            // Find the corresponding pluginID
            if ( bearer == EBearerWLAN )
                {
                connectionId = EBearerIdWLAN;
                }
            else if ( bearer == EBearerLAN )
                {
                connectionId = EBearerIdLAN;
                }
            }
        }

    // Check that request is aimed to this plugin.
    if ( connectionId != iPluginId )
        {
        LOGEXITFN1("CPlugin::GetAttributeForClient()", KErrNotSupported)
        return KErrNotSupported;
        }

    if ( aType == EReqSetStringAttribute || aType == EReqSetPckgAttribute )
        {
        // Setting string or packaged attributes is not supported
        LOGEXITFN1("CPlugin::GetAttributeForClient()", KErrNotSupported)
        return KErrNotSupported;
        }

    for ( TInt i = 0; i < iAttributesSupported.Count(); i++ )
        {
        if ( iAttributesSupported[i] == attribute )
            {
            TAttributeRequest req( aType, attribute, 0 );

            if ( aType == EReqSetIntAttribute ||
                 aType == EReqSetUintAttribute ||
                 aType == EReqSetBoolAttribute )
                {
                req.iData = data;
                }
            else if ( aType == EReqInternalSetThreshold )
                {
                req.iType = EReqSetUintAttribute;
                req.iData = 1;
                }
            else if ( aType == EReqInternalResetThreshold )
                {
                req.iType = EReqSetUintAttribute;
                req.iData = 0;
                }

            if ( iPluginReqActive )
                {
                TBool pending( ETrue );

                // Send the attribute request to the plug-in
                TPtr8 n( reinterpret_cast< TUint8* >( &req ),
                         sizeof( TAttributeRequest ),
                         sizeof( TAttributeRequest ) );

                TInt res = iPluginReqMessage.Write( 0, n );

                if ( res != KErrNone )
                    {
                    PanicWaitingClient( EBadDescriptor, iPluginReqMessage );
                    pending = EFalse;
                    }
                else
                    {
                    // Complete the message
                    iPluginReqMessage.Complete( KErrNone );
                    }

                iPluginReqActive=EFalse;

                // Add the request to queue
                // aPending is ETrue
                if ( aType == EReqGetIntAttribute ||
                     aType == EReqGetUintAttribute ||
                     aType == EReqGetBoolAttribute ||
                     aType == EReqGetStringAttribute ||
                     aType == EReqGetPckgAttribute )
                    {
                    TClientRequest clientReq( aType, pending, aMessage );
                    iClientRequests.Append( clientReq );
                    }
                }
            else
                {
                // Add the request to the queue
                // aPending is EFalse
                TClientRequest clientReq( aType, EFalse, aMessage );
                iClientRequests.Append( clientReq );
                }

            LOGEXITFN1("CPlugin::GetAttributeForClient()", KRequestPending)
            return KRequestPending;
            }
        }

    LOGEXITFN1("CPlugin::GetAttributeForClient()", KErrNotSupported)
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CPlugin::CancelGetAttributeForClient
// From a CLIENT.
// -----------------------------------------------------------------------------
//
void CPlugin::CancelGetAttributeForClient( const RThread& aClient, const TInt aType )
    {
    TInt count = iClientRequests.Count();

    for ( TInt i = 0; i < count; i++ )
        {
        const TClientRequest* req = &( iClientRequests[i] );

        RThread client;

        TInt err = req->iMessage.Client( client );

        if ( err == KErrNone )
            {
            if  ( ( static_cast< TUint >( aType ) == req->iType )  &&
                  ( client.Id() == aClient.Id() ) )
                {
                req->iMessage.Complete( KErrCancel );

                iClientRequests.Remove( i );
                count--;
                i--;
                }

            client.Close();
            }
        }
    }

// -----------------------------------------------------------------------------
// CPlugin::SetAttributeForClient
// From a CLIENT.
// -----------------------------------------------------------------------------
//
TInt CPlugin::SetAttributeForClient( const TInt aType, const RMessage2& aMessage )
    {
    TInt ret = GetAttributeForClient( aType, aMessage );

    if ( ret == KRequestPending )
        {
        // Complete immediately
        return KErrNone;
        }
    else
        {
        return ret;
        }
    }

// -----------------------------------------------------------------------------
// CPlugin::PanicWaitingClient
// -----------------------------------------------------------------------------
//
void CPlugin::PanicWaitingClient( TInt aPanic, const RMessage2& aMessage ) const
    {
    // Let's have a look before we panic the client
    __DEBUGGER()

    // Ok, go for it
    RThread client;

    TInt err =  aMessage.Client( client );

    if ( err == KErrNone )
        {
        _LIT( KPanicMessage, "ConnMonServ" );
        client.Panic( KPanicMessage, aPanic );
        client.Close();
        }
    }

// End-of-file
