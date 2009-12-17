/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Connection Monitor Extension.
*
*/

#include "ConnMonExt.h"
#include "log.h"

// ============================ LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Panic
// Panics the client in case of programming error.
// -----------------------------------------------------------------------------
//
void Panic( TInt aPanic )
    {
    _LIT( KPanicCategory, "ConnectionMonitor Extension API" );
    User::Panic( KPanicCategory, aPanic );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnectionMonitorPlugin::CConnectionMonitorPlugin
// -----------------------------------------------------------------------------
//
EXPORT_C CConnectionMonitorPlugin::CConnectionMonitorPlugin()
    {
    iPluginAO = 0;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CConnectionMonitorPlugin::~CConnectionMonitorPlugin()
    {
    delete iPluginAO;
    }

// -----------------------------------------------------------------------------
// CConnectionMonitorPlugin::RegisterPlugin
// Starts plugin session and registers the plug-in.
// -----------------------------------------------------------------------------
//
EXPORT_C void CConnectionMonitorPlugin::RegisterPluginL( const TUint aConnectionId )
    {
    if ( iPluginAO == 0 )
        {
        // Create the receiver (active object)
        iPluginAO = new ( ELeave ) CPluginAO( iSession, *this );
        iPluginAO->ConstructL();

        // Connect to the Connection Monitor server
        iConnMonServer.ConnectL();

        // Open plug-in subsession to the Connection Monitor server
        TInt ret = iSession.Open( iConnMonServer, aConnectionId );

        if ( ret != KErrNone )
            {
            User::Leave( ret );
            }
        }

    iPluginAO->ReceiveQuery();

    LOG( Log::Printf( _L("Plugin [%d]: registered: Id:<%d>\n"), &iSession, aConnectionId ) );
    }

// -----------------------------------------------------------------------------
// CConnectionMonitorPlugin::CancelRegisterPlugin
// Cancels the plug-in's registration.
// -----------------------------------------------------------------------------
//
EXPORT_C void CConnectionMonitorPlugin::CancelRegisterPlugin()
    {
    if ( iPluginAO != 0 )
        {
        // Delete receiver
        delete iPluginAO;
        iPluginAO = 0;

        // Close plug-in subsession
        iSession.Close();

        // Disconnect from Connection Monitor server
        iConnMonServer.Close();

        LOG( Log::Printf( _L("Plugin [%d]: Cancelled registration.\n"), &iSession ) );
        }
    }

// -----------------------------------------------------------------------------
// CConnectionMonitorPlugin::RegisterAttribute
// Registers a new attribute to the Connection Monitor server.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CConnectionMonitorPlugin::RegisterAttribute( const TUint aAttribute ) const
    {
    LOG( Log::Printf( _L("Plugin [%d]: register attribute: Id:<%d>\n"), &iSession, aAttribute ) );

    return ( iSession.RegisterAttribute( aAttribute ) );
    }

// -----------------------------------------------------------------------------
// CConnectionMonitorPlugin::CancelRegisterAttribute
// Cancels the attribute registration from the Connection Monitor server.
// -----------------------------------------------------------------------------
//
EXPORT_C void CConnectionMonitorPlugin::CancelRegisterAttribute( const TUint aAttribute ) const
    {
    LOG( Log::Printf( _L("Plugin [%d]: Cancel attribute registration: Id:<%d>\n"), &iSession, aAttribute ) );

    iSession.CancelRegisterAttribute( aAttribute );
    }

// -----------------------------------------------------------------------------
// CConnectionMonitorPlugin::Event
// Sends an event to Connnection Monitor server.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CConnectionMonitorPlugin::Event( const TUint aEventId,
                                               const TAny* aData,
                                               const TUint aSize ) const
    {
    LOG( Log::Printf( _L("Plugin [%d]: Send event: EventId:<%d>\n"), &iSession, aEventId ) );

    return ( iSession.SendEvent( aEventId, aData, aSize ) );
    }

// -----------------------------------------------------------------------------
// CConnectionMonitorPlugin::Version
// Returns the client side version number.
// -----------------------------------------------------------------------------
//
EXPORT_C TVersion CConnectionMonitorPlugin::Version() const
    {
    LOG( Log::Printf( _L("Plugin [%d]: Get version.\n"), &iSession ) );

    return ( iSession.Version() );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RConnMonPlugin::Open
// Creates a new subsession with the Connection Monitor server.
// -----------------------------------------------------------------------------
//
TInt RConnMonPlugin::Open( RConnectionMonitor &aSession, const TUint aConnectionId  )
    {
    //p[0]= ( TAny* )aConnectionId ;

    TIpcArgs args( aConnectionId );

    return CreateSubSession( aSession, EReqPluginCreateSubSession, args );
    }

// -----------------------------------------------------------------------------
// RConnMonPlugin::Close
// Closes the subsession.
// -----------------------------------------------------------------------------
//
void RConnMonPlugin::Close()
    {
    RSubSessionBase::CloseSubSession( EReqPluginCloseSubSession );
    }

// -----------------------------------------------------------------------------
// RConnMonPlugin::Version
// Returns the client side version number.
// -----------------------------------------------------------------------------
//
TVersion RConnMonPlugin::Version() const
    {
    return( TVersion( KPluginMajorVersionNumber,
                      KPluginMinorVersionNumber,
                      KPluginBuildVersionNumber ) );
    }

// -----------------------------------------------------------------------------
// RConnMonPlugin::SendEvent
// Sends an event to the Connection Monitor server.
// -----------------------------------------------------------------------------
//
TInt RConnMonPlugin::SendEvent( const TUint aEventId,
                                const TAny* aData,
                                const TUint aSize ) const
    {
    if ( aSize > KConnMonMaxPluginDataSize )
        {
        return KErrTooBig;
        }

    TPtrC8 n( reinterpret_cast< const TUint8* >( aData ), aSize );

    //p[0]= ( TAny* )aEventId;
    //p[1]= ( TAny* )&n;

    TIpcArgs args( aEventId, &n );

    return SendReceive( EReqPluginEvent, args );
    }

// -----------------------------------------------------------------------------
// RConnMonPlugin::RegisterAttribute
// Registers an attribute to the Connection Monitor server.
// -----------------------------------------------------------------------------
//
TInt RConnMonPlugin::RegisterAttribute( const TUint aAttribute ) const
    {
    //p[0]= ( TAny* )aAttribute;

    TIpcArgs args( aAttribute );

    return SendReceive( EReqPluginRegisterAttribute, args );
    }

// -----------------------------------------------------------------------------
// RConnMonPlugin::CancelRegisterAttribute
// Cancels the attribute registration from the Connection Monitor server.
// -----------------------------------------------------------------------------
//
TInt RConnMonPlugin::CancelRegisterAttribute( const TUint aAttribute ) const
    {
    //p[0]= ( TAny* )aAttribute;

    TIpcArgs args( aAttribute );

    return SendReceive( EReqPluginCancelRegisterAttribute, args );
    }

// -----------------------------------------------------------------------------
// RConnMonPlugin::GetQuery
// Gets the next attribute request from the Connection Monitor server.
// -----------------------------------------------------------------------------
//
void RConnMonPlugin::GetQuery( TDes8& aBuffer, TRequestStatus& aStatus ) const
    {
    //p[ 0 ]= ( TAny* )&aBuffer;

    TIpcArgs args( &aBuffer );

    SendReceive( EReqPluginGetQuery, args, aStatus );
    }

// -----------------------------------------------------------------------------
// RConnMonPlugin::CancelGetQueyry
// -----------------------------------------------------------------------------
//
void RConnMonPlugin::CancelGetQuery() const
    {
    //SendReceive( EReqPluginCancelGetQuery, 0 );
    SendReceive( EReqPluginCancelGetQuery, TIpcArgs( TIpcArgs::ENothing ) );
    }

// -----------------------------------------------------------------------------
// RConnMonPlugin::SendAttribute
// Sends an attribute to the Connection Monitor server.
// -----------------------------------------------------------------------------
//
TInt RConnMonPlugin::SendAttribute( const TInt  aType,
                                    const TUint aAttribute,
                                    const TInt  aRet,
                                    const TAny* aValue,
                                    const TUint aSize ) const
    {
    if ( aSize > KConnMonMaxPluginDataSize )
        {
        return KErrTooBig;
        }

    //p[0]= ( TAny* )aAttribute;
    //p[1]= ( TAny* )aRet;

    if ( ( aType == EReqGetIntAttribute ) || ( aType == EReqGetUintAttribute ) ||
         ( aType == EReqGetBoolAttribute ) )
        {
        TPtrC8 n( reinterpret_cast< const TUint8* >( aValue ), aSize );

        //p[2]= ( TAny* )&n;

        TIpcArgs args( aAttribute, aRet, &n );

        return SendReceive( EReqPluginAttribute, args );
        }
    else
        {
        //p[2]= const_cast< TAny* >( aValue );

        TIpcArgs args( aAttribute, aRet, aValue );

        return SendReceive( EReqPluginAttribute, args );
        }
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPluginAO::CPluginAO
// -----------------------------------------------------------------------------
//
CPluginAO::CPluginAO( RConnMonPlugin& aSession, CConnectionMonitorPlugin& aPlugin )
    :
    CActive( EPriorityStandard ),
    iSession( aSession ),
    iPlugin( aPlugin ),
    iBuf( NULL, 0, 0 )
    {
    }

// -----------------------------------------------------------------------------
// CPluginAO::ConstructL
// -----------------------------------------------------------------------------
//
void CPluginAO::ConstructL()
    {
    // Plug-in engine must have an active scheduler
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CPluginAO::~CPluginAO()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CPluginAO::ReceiveQuery
// Requests a new query from Connection Monitor server.
// -----------------------------------------------------------------------------
//
void CPluginAO::ReceiveQuery()
    {
    if ( IsActive() )
        {
        return;
        }
    else
        {
        // Must be passed as a descriptor
        iReqInfo.Reset();

        iBuf.Set( reinterpret_cast< TUint8* >( &iReqInfo ), sizeof( TReqInfo ), sizeof( TReqInfo ) );

        iSession.GetQuery( iBuf, iStatus );

        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CPluginAO::RunL
// Receives the a request from the Connection Monitor server and passes it to the
// plug-in module. Sends the result back to the server.
// -----------------------------------------------------------------------------
//
void CPluginAO::RunL()
    {
    if ( iStatus.Int() == KErrServerBusy )
        {
        // Message slot was reserved
        // Try again
        ReceiveQuery();
        }
    else if ( iStatus.Int() == KErrNone )
        {
        // A new request has arrived

        switch ( iReqInfo.iType )
            {
            case EReqGetIntAttribute:
                {
                TInt value( 0 );
                TInt ret = iPlugin.GetIntAttribute( iReqInfo.iAttribute, value );

                iSession.SendAttribute( EReqGetIntAttribute,
                                        iReqInfo.iAttribute,
                                        ret,
                                        &value,
                                        sizeof( TInt ) );

                break;
                }

            case EReqGetUintAttribute:
                {
                TUint value( 0 );
                TInt ret = iPlugin.GetUintAttribute( iReqInfo.iAttribute, value );

                iSession.SendAttribute( EReqGetUintAttribute,
                                        iReqInfo.iAttribute,
                                        ret,
                                        &value,
                                        sizeof( TUint ) );

                break;
                }

            case EReqGetBoolAttribute:
                {
                TBool value( EFalse );
                TInt ret = iPlugin.GetBoolAttribute( iReqInfo.iAttribute, value );

                iSession.SendAttribute( EReqGetBoolAttribute,
                                        iReqInfo.iAttribute,
                                        ret,
                                        &value,
                                        sizeof( TBool ) );

                break;
                }

            case EReqGetStringAttribute:
                {
                HBufC16* value = HBufC16::NewL( KConnMonMaxStringAttributeLength );

                TPtr16 auxPtr( const_cast< TUint16* >( value->Des().Ptr() ), KConnMonMaxStringAttributeLength );
                auxPtr.FillZ();

                TInt ret = iPlugin.GetStringAttribute( iReqInfo.iAttribute, auxPtr );

                iSession.SendAttribute( EReqGetStringAttribute,
                                        iReqInfo.iAttribute,
                                        ret,
                                        &auxPtr,
                                        auxPtr.MaxSize() );
                delete value;

                break;
                }

            case EReqGetPckgAttribute:
                {
                HBufC8* buf = HBufC8::New( KConnMonMaxPluginDataSize );
                TPtr8 initptr = buf->Des();

                TInt ret = iPlugin.GetPckgAttribute( iReqInfo.iAttribute, initptr );

                iSession.SendAttribute( EReqGetPckgAttribute,
                                        iReqInfo.iAttribute,
                                        ret,
                                        &initptr,
                                        KConnMonMaxPluginDataSize );

                delete buf;

                break;
                }

            case EReqSetIntAttribute:
                {
                iPlugin.SetIntAttribute( iReqInfo.iAttribute, iReqInfo.iData );

                break;
                }

            case EReqSetUintAttribute:
                {
                iPlugin.SetUintAttribute( iReqInfo.iAttribute, static_cast< TUint >( iReqInfo.iData ) );

                break;
                }

            case EReqSetBoolAttribute:
                {
                iPlugin.SetBoolAttribute( iReqInfo.iAttribute, static_cast< TBool >( iReqInfo.iData ) );

                break;
                }

            case EReqSetStringAttribute:
                {
                // Not supported
                break;
                }

            case EReqSetPckgAttribute:
                {
                // Not supported
                break;
                }

            default:
                break;
            }

        LOG( Log::Printf( _L("Plugin [%d]: served attribute query: Type:<%d>, Attribute:<%d> \n"),
                          &iSession,
                          iReqInfo.iType,
                          iReqInfo.iAttribute ) );

        // initiate the next receive
        ReceiveQuery();
        }
    else
        {
        // Log error
        LOG( Log::Printf( _L("Error in CPluginAO::RunL(): [%d].\n"), iStatus.Int() ) );
        }
    }

// -----------------------------------------------------------------------------
// CPluginAO::DoCancel
// Cancels the request from Connection Monitor server.
// -----------------------------------------------------------------------------
//
void CPluginAO::DoCancel()
    {
    if ( IsActive() )
        {
        iSession.CancelGetQuery();
        }
    }

