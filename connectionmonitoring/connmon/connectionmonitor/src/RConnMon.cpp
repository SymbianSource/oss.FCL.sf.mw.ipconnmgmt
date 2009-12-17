/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for Connection Monitor Server API methods.
*
*/

#include <rconnmon.h>

#include "ConnMonDef.h"
#include "ConnMonCli.h"
#include "ConnMonServ.h"
#include "log.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RConnectionMonitor::ConnectL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RConnectionMonitor::ConnectL()
    {
    //LOGENTRFN("RConnectionMonitor::ConnectL()")
    LOGIT1("Client [%d]: calling ConnectL()", this)

    if ( IsConnected() )
        {
        LOGIT1("Client [%d]: already connected", this)
        return KErrNone;
        }

    // Connect to server
    TInt errConnect( KErrNone );
    TInt errLaunch( KErrNone );

    for ( TInt tries = 0; tries < 2; tries++ )
        {
        // This initialization is needed if some application starts
        // ConnMon server just between CreateSession() and
        // LaunchFromClient() here.
        errLaunch = KErrNone;

        errConnect = CreateSession(
                KConnectionMonitorServerName,
                TVersion( 0, 0, 0 ),
                KNumMessageSlots );
        if ( errConnect )
            {
            if ( errConnect != KErrNotFound && errConnect != KErrServerTerminated )
                {
                break; // Problems other than server not here - propagate error
                }

            errLaunch = CConnMonScheduler::LaunchFromClient();

            // If server launched ok (KErrNone), try again to connect
            // If someone else got there first (KErrAlreadyExists), try again to connect
            if ( errLaunch != KErrNone && errLaunch != KErrAlreadyExists )
                {
                break; // Server not launched: don't cycle round again
                }
            }
        else
            {
            break;
            }
        }
    User::LeaveIfError( errConnect );
    User::LeaveIfError( errLaunch );

    // Initialize an array for holding the non-descriptor parameters that are
    // passed along asyncronous methods to the server. (Inter-thread reads/writes
    // are possible only on desc data.)
    if ( iPtrArray.Count() == 0 )
        {
        TPtr8 ptr( reinterpret_cast<TUint8*>( NULL ), 0 );

        for ( TInt i = 0; i < KNumPtrs; i++ )
            {
            User::LeaveIfError( iPtrArray.Append( ptr ) );
            }
        }

#ifdef _DEBUG
    TName processName = RProcess().Name();
    if ( processName.Length() < KMaxName )
        {
        LOGIT2("Client [%d]: process name [%s]", this, processName.PtrZ())
        }
#endif // _DEBUG

    //LOGEXITFN("RConnectionMonitor::ConnectL()")
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::Close
// -----------------------------------------------------------------------------
//
EXPORT_C void RConnectionMonitor::Close()
    {
    //LOGENTRFN("RConnectionMonitor::Close()")
    // Destroy pointer array
    iPtrArray.Close();

    // Destroy notification handler
    delete iHandler;
    iHandler = NULL;

    // Destroy server session
    RSessionBase::Close();

    LOGIT1("Client [%d]: closed!", this)
    //LOGEXITFN("RConnectionMonitor::Close()")
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::GetConnectionCount
// -----------------------------------------------------------------------------
//
EXPORT_C void RConnectionMonitor::GetConnectionCount(
        TUint& aConnectionCount,
        TRequestStatus& aStatus )
    {
    LOGIT1("Client [%d]: GetConnectionCount()", this)

    TPtr8& pckg = NextPtr();
    pckg.Set(
            reinterpret_cast<TUint8*>( &aConnectionCount ),
            sizeof( aConnectionCount ),
            sizeof( aConnectionCount ) );

    TIpcArgs args( &pckg );

    SendReceive( EReqGetConnectionCount, args, aStatus );
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::GetConnectionInfo
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RConnectionMonitor::GetConnectionInfo(
        const TUint aIndex,
        TUint& aConnectionId,
        TUint& aSubConnectionCount ) const
    {
    LOGIT2("Client [%d]: GetConnectionInfo(), index %d", this, aIndex)

    TPtr8 n( reinterpret_cast<TUint8*>( &aConnectionId ), sizeof( aConnectionId ) );
    TPtr8 m( reinterpret_cast<TUint8*>( &aSubConnectionCount ), sizeof( aSubConnectionCount ) );

    TIpcArgs args( aIndex, &n, &m );

    return( SendReceive( EReqGetConnectionInfo, args ) );
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::GetSubConnectionInfo
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RConnectionMonitor::GetSubConnectionInfo(
        const TUint aConnectionId,
        const TUint aIndex,
        TUint& aSubConnectionId ) const
    {
    LOGIT3("Client [%d]: GetSubConnectionInfo(), conn.id %d, index %d", this, aConnectionId, aIndex)

    TPtr8 n( reinterpret_cast<TUint8*>( &aSubConnectionId ), sizeof( aSubConnectionId ) );

    TIpcArgs args( aConnectionId, aIndex, &n );

    return( SendReceive( EReqGetSubConnectionInfo, args ) );
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::GetIntAttribute
// -----------------------------------------------------------------------------
//
EXPORT_C void RConnectionMonitor::GetIntAttribute(
        const TUint aConnectionId,
        const TUint aSubConnectionId,
        const TUint aAttribute,
        TInt& aValue,
        TRequestStatus& aStatus )
    {
    LOGIT4("Client [%d]: GetIntAttribute(), conn.id %d, sub.conn.id %d, attribute %d",
            this, aConnectionId, aSubConnectionId, aAttribute)

    TPtr8& pckg = NextPtr();
    pckg.Set( reinterpret_cast<TUint8*>( &aValue ), sizeof( aValue ), sizeof( aValue ) );

    TIpcArgs args( aConnectionId, aSubConnectionId, aAttribute, &pckg );

    SendReceive( EReqGetIntAttribute, args, aStatus );
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::GetUintAttribute
// -----------------------------------------------------------------------------
//
EXPORT_C void RConnectionMonitor::GetUintAttribute(
        const TUint aConnectionId,
        const TUint aSubConnectionId,
        const TUint aAttribute,
        TUint& aValue,
        TRequestStatus& aStatus )
    {
    LOGIT4("Client [%d]: GetUintAttribute(), conn.id %d, sub.conn.id %d, attribute %d",
            this, aConnectionId, aSubConnectionId, aAttribute)

    TPtr8& pckg = NextPtr();
    pckg.Set( reinterpret_cast<TUint8*>( &aValue ), sizeof( aValue ), sizeof( aValue ) );

    TIpcArgs args( aConnectionId, aSubConnectionId, aAttribute, &pckg );

    SendReceive( EReqGetUintAttribute, args, aStatus );
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::GetBoolAttribute
// -----------------------------------------------------------------------------
//
EXPORT_C void RConnectionMonitor::GetBoolAttribute(
        const TUint aConnectionId,
        const TUint aSubConnectionId,
        const TUint aAttribute,
        TBool& aValue,
        TRequestStatus& aStatus )
    {
    LOGIT4("Client [%d]: GetBoolAttribute(), conn.id %d, sub.conn.id %d, attribute %d",
            this, aConnectionId, aSubConnectionId, aAttribute)

    TPtr8& pckg = NextPtr();
    pckg.Set( reinterpret_cast<TUint8*>( &aValue ), sizeof( aValue ), sizeof( aValue ) );

    TIpcArgs args( aConnectionId, aSubConnectionId, aAttribute, &pckg );

    SendReceive( EReqGetBoolAttribute, args, aStatus );
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::GetStringAttribute
// -----------------------------------------------------------------------------
//
EXPORT_C void RConnectionMonitor::GetStringAttribute(
        const TUint aConnectionId,
        const TUint aSubConnectionId,
        const TUint aAttribute,
        TDes& aValue,
        TRequestStatus& aStatus ) const
    {
    LOGIT4("Client [%d]: GetStringAttribute(), conn.id %d, sub.conn.id %d, attribute %d",
            this, aConnectionId, aSubConnectionId, aAttribute)

    TIpcArgs args( aConnectionId, aSubConnectionId, aAttribute, &aValue );

    SendReceive( EReqGetStringAttribute, args, aStatus );
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::GetPckgAttribute
// -----------------------------------------------------------------------------
//
EXPORT_C void RConnectionMonitor::GetPckgAttribute(
        const TUint aConnectionId,
        const TUint aSubConnectionId,
        const TUint aAttribute,
        TDes8& aValue,
        TRequestStatus& aStatus ) const
    {
    LOGIT4("Client [%d]: 8GetPckgAttribute(), conn.id %d, sub.conn.id %d, attribute %d",
            this, aConnectionId, aSubConnectionId, aAttribute)

    TIpcArgs args( aConnectionId, aSubConnectionId, aAttribute, &aValue );

    SendReceive( EReqGetPckgAttribute, args, aStatus );
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::GetPckgAttribute
// -----------------------------------------------------------------------------
//
EXPORT_C void RConnectionMonitor::GetPckgAttribute(
        const TUint aConnectionId,
        const TUint aSubConnectionId,
        const TUint aAttribute,
        TDes16& aValue,
        TRequestStatus& aStatus ) const
    {
    LOGIT4("Client [%d]: 16GetPckgAttribute(), conn.id %d, sub.conn.id %d, attribute %d",
            this, aConnectionId, aSubConnectionId, aAttribute)

    TIpcArgs args( aConnectionId, aSubConnectionId, aAttribute, &aValue );

    SendReceive( EReqGetPckgAttribute, args, aStatus );
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::SetIntAttribute
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RConnectionMonitor::SetIntAttribute(
        const TUint aConnectionId,
        const TUint aSubConnectionId,
        const TUint aAttribute,
        const TInt aValue ) const
    {
    LOGIT5("Client [%d]: SetIntAttribute(), conn.id %d, sub.conn.id %d, attribute %d, value %d",
            this, aConnectionId, aSubConnectionId, aAttribute, aValue)

    TIpcArgs args( aConnectionId, aSubConnectionId, aAttribute, aValue );

    return( SendReceive( EReqSetIntAttribute, args ) );
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::SetUintAttribute
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RConnectionMonitor::SetUintAttribute(
        const TUint aConnectionId,
        const TUint aSubConnectionId,
        const TUint aAttribute,
        const TUint aValue ) const
    {
    LOGIT5("Client [%d]: SetUintAttribute(), conn.id %d, sub.conn.id %d, attribute %d, value %d",
            this, aConnectionId, aSubConnectionId, aAttribute, aValue)

    TIpcArgs args( aConnectionId, aSubConnectionId, aAttribute, aValue );

    return( SendReceive( EReqSetUintAttribute, args ) );
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::SetBoolAttribute
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RConnectionMonitor::SetBoolAttribute(
        const TUint aConnectionId,
        const TUint aSubConnectionId,
        const TUint aAttribute,
        const TBool aValue ) const
    {
    LOGIT5("Client [%d]: SetBoolAttribute(), conn.id %d, sub.conn.id %d, attribute %d, value %d",
            this, aConnectionId, aSubConnectionId, aAttribute, aValue)

    TIpcArgs args( aConnectionId, aSubConnectionId, aAttribute, aValue );

    return( SendReceive( EReqSetBoolAttribute, args ) );
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::SetStringAttribute
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RConnectionMonitor::SetStringAttribute(
        const TUint aConnectionId,
        const TUint aSubConnectionId,
        const TUint aAttribute,
        const TDes& aValue ) const
    {
    LOGIT4("Client [%d]: SetStringAttribute(), conn.id %d, sub.conn.id %d, attribute %d",
            this, aConnectionId, aSubConnectionId, aAttribute)

    TIpcArgs args( aConnectionId, aSubConnectionId, aAttribute, &aValue );

    return( SendReceive( EReqSetStringAttribute, args ) );
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::SetPckgAttribute
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RConnectionMonitor::SetPckgAttribute(
        const TUint aConnectionId,
        const TUint aSubConnectionId,
        const TUint aAttribute,
        const TDes8& aValue ) const
    {
    LOGIT4("Client [%d]: SetPckgAttribute(), conn.id %d, sub.conn.id %d, attribute %d",
            this, aConnectionId, aSubConnectionId, aAttribute)

    TIpcArgs args( aConnectionId, aSubConnectionId, aAttribute, &aValue );

    return( SendReceive( EReqSetPckgAttribute, args ) );
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::CancelAsyncRequest
// -----------------------------------------------------------------------------
//
EXPORT_C void RConnectionMonitor::CancelAsyncRequest( TInt aReqToCancel )
    {
    LOGIT2("Client [%d]: CancelAsyncRequest(), request to cancel: %d", this, aReqToCancel)

    TIpcArgs args( aReqToCancel );

    SendReceive( EReqCancelAsyncRequest, args );
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::NotifyEventL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RConnectionMonitor::NotifyEventL( MConnectionMonitorObserver& aObserver )
    {
    LOGIT1("Client [%d]: NotifyEventL()", this)

    if ( iHandler == 0)
        {
        // Create active object to receive events and add it to scheduler
        iHandler = new( ELeave ) CConnMonEventHandler( &aObserver, *this );
        iHandler->Construct();

        // Kick off receive handler active object to start listening and start a receive
        iHandler->ReceiveNotification();
        }
    else
        {
        // Continue paused event handler.
        iHandler->Continue( &aObserver );
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::CancelNotifications
// -----------------------------------------------------------------------------
//
EXPORT_C void RConnectionMonitor::CancelNotifications()
    {
    LOGIT1("Client [%d]: CancelNotifications()", this)

    if ( iHandler != 0 )
        {
        if ( iHandler->IsActive() )
            {
            // Cancel event receiver
            iHandler->Cancel();

            // Delete and remove from active scheduler
            //iHandler->Deque;
            delete iHandler;
            iHandler = NULL;
            }
        else
            {
            // Client has cancelled notifications from
            // EventL() method. -> can't delete the iHandler object.
            // CancelReceiveEvent() needs to be called to set
            // iListening flag of this session in the server to 'false'.
            CancelReceiveEvent();
            iHandler->Pause();
            }
        }
    }


// Private methods in conjunction with CConnMonEventHandler

// -----------------------------------------------------------------------------
// RConnectionMonitor::ReceiveEvent
// -----------------------------------------------------------------------------
//
void RConnectionMonitor::ReceiveEvent(
        TDes8& aBuffer,
        TDes8& aExtraBuf,
        TRequestStatus& aStatus )
    {
    //LOGIT1("Client [%d]: waiting for next event", this)

    TIpcArgs args( &aBuffer, &aExtraBuf );

    SendReceive( EReqReceiveEvent, args, aStatus );
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::CancelReceiveEvent
// -----------------------------------------------------------------------------
//
void RConnectionMonitor::CancelReceiveEvent()
    {
    LOGIT1("Client [%d]: CancelReceiveEvent()", this)

    SendReceive( EReqCancelReceiveEvent, TIpcArgs( TIpcArgs::ENothing ) );
    }

// Private internal methods

// -----------------------------------------------------------------------------
// RConnectionMonitor::IsConnected
// -----------------------------------------------------------------------------
//
TBool RConnectionMonitor::IsConnected() const
    {
    if ( iPtrArray.Count() > 0 )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// RConnectionMonitor::NextPtr
// -----------------------------------------------------------------------------
//
TPtr8& RConnectionMonitor::NextPtr()
    {
    __ASSERT_ALWAYS( IsConnected(), Panic( EConnMonPanicClientNotConnected ) );

    if ( iIndex >= iPtrArray.Count() )
        {
        iIndex = 0;
        }

    return iPtrArray[iIndex++];
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonEventBase::CConnMonEventBase
// -----------------------------------------------------------------------------
//
CConnMonEventBase::CConnMonEventBase(
        const TInt aEventType,
        const TUint aConnectionId )
    {
    iEventType = aEventType;
    iConnectionId = aConnectionId;
    }

// Destructor
CConnMonEventBase::~CConnMonEventBase()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonEventBase::EventType
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CConnMonEventBase::EventType() const
    {
    return iEventType;
    }

// -----------------------------------------------------------------------------
// CConnMonEventBase::ConnectionId
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CConnMonEventBase::ConnectionId() const
    {
    return iConnectionId;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonCreateConnection::CConnMonCreateConnection
// -----------------------------------------------------------------------------
//
CConnMonCreateConnection::CConnMonCreateConnection(
        const TUint aConnectionId )
        :
        CConnMonEventBase( EConnMonCreateConnection, aConnectionId )
    {
    }

// Destructor
CConnMonCreateConnection::~CConnMonCreateConnection()
    {
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonDeleteConnection::CConnMonDeleteConnection
// -----------------------------------------------------------------------------
//
CConnMonDeleteConnection::CConnMonDeleteConnection(
        const TUint aConnectionId,
        const TUint aDownlinkData,
        const TUint aUplinkData,
        const TBool aAuthDelete )
        :
        CConnMonEventBase( EConnMonDeleteConnection, aConnectionId )
    {
    iDownlinkData = aDownlinkData;
    iUplinkData = aUplinkData;
    iAuthDelete = aAuthDelete;
    }

// Destructor
CConnMonDeleteConnection::~CConnMonDeleteConnection()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonDeleteConnection::DownlinkData
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CConnMonDeleteConnection::DownlinkData() const
    {
    return iDownlinkData;
    }

// -----------------------------------------------------------------------------
// CConnMonDeleteConnection::UplinkData
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CConnMonDeleteConnection::UplinkData() const
    {
    return iUplinkData;
    }

// -----------------------------------------------------------------------------
// CConnMonDeleteConnection::AuthoritativeDelete
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CConnMonDeleteConnection::AuthoritativeDelete() const
    {
    return iAuthDelete;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonCreateSubConnection::CConnMonCreateSubConnection
// -----------------------------------------------------------------------------
//
CConnMonCreateSubConnection::CConnMonCreateSubConnection(
        const TUint aConnectionId,
        const TUint aSubConnectionId )
        :
        CConnMonEventBase( EConnMonCreateSubConnection, aConnectionId )
    {
    iSubConnectionId = aSubConnectionId;
    }

// Destructor
CConnMonCreateSubConnection::~CConnMonCreateSubConnection()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonCreateSubConnection::SubConnectionId
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CConnMonCreateSubConnection::SubConnectionId() const
    {
    return iSubConnectionId;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonDeleteSubConnection::CConnMonDeleteSubConnection
// -----------------------------------------------------------------------------
//
CConnMonDeleteSubConnection::CConnMonDeleteSubConnection(
        const TUint aConnectionId,
        const TUint aSubConnectionId,
        const TUint aDownlinkData,
        const TUint aUplinkData,
        const TBool aAuthDelete )
        :
        CConnMonEventBase( EConnMonDeleteSubConnection, aConnectionId )
    {
    iSubConnectionId = aSubConnectionId;
    iDownlinkData = aDownlinkData;
    iUplinkData = aUplinkData;
    iAuthDelete = aAuthDelete;
    }

// Destructor
CConnMonDeleteSubConnection::~CConnMonDeleteSubConnection()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonDeleteSubConnection::SubConnectionId
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CConnMonDeleteSubConnection::SubConnectionId() const
    {
    return iSubConnectionId;
    }

// -----------------------------------------------------------------------------
// CConnMonDeleteSubConnection::DownlinkData
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CConnMonDeleteSubConnection::DownlinkData() const
    {
    return iDownlinkData;
    }

// -----------------------------------------------------------------------------
// CConnMonDeleteSubConnection::UplinkData
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CConnMonDeleteSubConnection::UplinkData() const
    {
    return iUplinkData;
    }

// -----------------------------------------------------------------------------
// CConnMonDeleteSubConnection::AuthoritativeDelete
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CConnMonDeleteSubConnection::AuthoritativeDelete() const
    {
    return iAuthDelete;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonDownlinkDataThreshold::CConnMonDownlinkDataThreshold
// -----------------------------------------------------------------------------
//
CConnMonDownlinkDataThreshold::CConnMonDownlinkDataThreshold(
        const TUint aConnectionId,
        const TUint aSubConnectionId,
        const TUint aDownlinkData )
        :
        CConnMonEventBase( EConnMonDownlinkDataThreshold, aConnectionId )
    {
    iSubConnectionId = aSubConnectionId;
    iDownlinkData = aDownlinkData;
    }

// Destructor
CConnMonDownlinkDataThreshold::~CConnMonDownlinkDataThreshold()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonDownlinkDataThreshold::SubConnectionId
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CConnMonDownlinkDataThreshold::SubConnectionId() const
    {
    return iSubConnectionId;
    }

// -----------------------------------------------------------------------------
// CConnMonDownlinkDataThreshold::DownlinkData
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CConnMonDownlinkDataThreshold::DownlinkData() const
    {
    return iDownlinkData;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonUplinkDataThreshold::CConnMonUplinkDataThreshold
// -----------------------------------------------------------------------------
//
CConnMonUplinkDataThreshold::CConnMonUplinkDataThreshold(
        const TUint aConnectionId,
        const TUint aSubConnectionId,
        const TUint aUplinkData )
        :
        CConnMonEventBase( EConnMonUplinkDataThreshold, aConnectionId )
    {
    iSubConnectionId = aSubConnectionId;
    iUplinkData = aUplinkData;
    }

// Destructor
CConnMonUplinkDataThreshold::~CConnMonUplinkDataThreshold()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonUplinkDataThreshold::SubConnectionId
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CConnMonUplinkDataThreshold::SubConnectionId() const
    {
    return iSubConnectionId;
    }

// -----------------------------------------------------------------------------
// CConnMonUplinkDataThreshold::UplinkData
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CConnMonUplinkDataThreshold::UplinkData() const
    {
    return iUplinkData;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonNetworkStatusChange::CConnMonNetworkStatusChange
// -----------------------------------------------------------------------------
//
CConnMonNetworkStatusChange::CConnMonNetworkStatusChange(
        const TUint aConnectionId,
        const TInt  aNetworkStatus )
        :
        CConnMonEventBase( EConnMonNetworkStatusChange, aConnectionId )
    {
    iNetworkStatus = aNetworkStatus;
    }

// Destructor
CConnMonNetworkStatusChange::~CConnMonNetworkStatusChange()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonNetworkStatusChange::NetworkStatus
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CConnMonNetworkStatusChange::NetworkStatus() const
    {
    return iNetworkStatus;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonConnectionStatusChange::CConnMonConnectionStatusChange
// -----------------------------------------------------------------------------
//
CConnMonConnectionStatusChange::CConnMonConnectionStatusChange(
        const TUint aConnectionId,
        const TUint aSubConnectionId,
        const TInt  aConnectionStatus )
        :
        CConnMonEventBase( EConnMonConnectionStatusChange, aConnectionId )
    {
    iSubConnectionId = aSubConnectionId;
    iConnectionStatus = aConnectionStatus;
    }

// Destructor
CConnMonConnectionStatusChange::~CConnMonConnectionStatusChange()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonConnectionStatusChange::SubConnectionId
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CConnMonConnectionStatusChange::SubConnectionId() const
    {
    return iSubConnectionId;
    }

// -----------------------------------------------------------------------------
// CConnMonConnectionStatusChange::ConnectionStatus
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CConnMonConnectionStatusChange::ConnectionStatus() const
    {
    return iConnectionStatus;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonConnectionActivityChange::CConnMonConnectionActivityChange
// -----------------------------------------------------------------------------
//
CConnMonConnectionActivityChange::CConnMonConnectionActivityChange(
        const TUint aConnectionId,
        const TUint aSubConnectionId,
        const TBool aActivity )
        :
        CConnMonEventBase( EConnMonConnectionActivityChange, aConnectionId )
    {
    iSubConnectionId = aSubConnectionId;
    iActivity = aActivity;
    }

// Destructor
CConnMonConnectionActivityChange::~CConnMonConnectionActivityChange()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonConnectionActivityChange::SubConnectionId
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CConnMonConnectionActivityChange::SubConnectionId() const
    {
    return iSubConnectionId;
    }

// -----------------------------------------------------------------------------
// CConnMonConnectionActivityChange::ConnectionActivity
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CConnMonConnectionActivityChange::ConnectionActivity() const
    {
    return iActivity;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonNetworkRegistrationChange::CConnMonNetworkRegistrationChange
// -----------------------------------------------------------------------------
//
CConnMonNetworkRegistrationChange::CConnMonNetworkRegistrationChange(
        const TUint aConnectionId,
        const TInt  aRegistrationStatus )
        :
        CConnMonEventBase( EConnMonNetworkRegistrationChange, aConnectionId )
    {
    iRegistrationStatus = aRegistrationStatus;
    }

// Destructor
CConnMonNetworkRegistrationChange::~CConnMonNetworkRegistrationChange()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonNetworkRegistrationChange::RegistrationStatus
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CConnMonNetworkRegistrationChange::RegistrationStatus() const
    {
    return iRegistrationStatus;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonBearerChange::CConnMonBearerChange
// -----------------------------------------------------------------------------
//
CConnMonBearerChange::CConnMonBearerChange(
        const TUint aConnectionId,
        const TInt aBearer )
        :
        CConnMonEventBase( EConnMonBearerChange, aConnectionId )
    {
    iBearer = aBearer;
    }

// Destructor
CConnMonBearerChange::~CConnMonBearerChange()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonBearerChange::RegistrationStatus
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CConnMonBearerChange::Bearer() const
    {
    return iBearer;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonSignalStrengthChange::CConnMonSignalStrengthChange
// -----------------------------------------------------------------------------
//
CConnMonSignalStrengthChange::CConnMonSignalStrengthChange(
        const TUint aConnectionId,
        const TInt  aSignalStrength )
        :
        CConnMonEventBase( EConnMonSignalStrengthChange, aConnectionId )
    {
    iSignalStrength = aSignalStrength;
    }

// Destructor
CConnMonSignalStrengthChange::~CConnMonSignalStrengthChange()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonSignalStrengthChange::SignalStrength
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CConnMonSignalStrengthChange::SignalStrength() const
    {
    return iSignalStrength;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonBearerAvailabilityChange::CConnMonBearerAvailabilityChange
// -----------------------------------------------------------------------------
//
CConnMonBearerAvailabilityChange::CConnMonBearerAvailabilityChange(
        const TUint aConnectionId,
        const TBool aAvailability )
        :
        CConnMonEventBase( EConnMonBearerAvailabilityChange, aConnectionId )
    {
    iAvailability = aAvailability;
    }

// Destructor
CConnMonBearerAvailabilityChange::~CConnMonBearerAvailabilityChange()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonBearerAvailabilityChange::Availability
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CConnMonBearerAvailabilityChange::Availability() const
    {
    return iAvailability;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonGenericEvent::CConnMonGenericEvent
// -----------------------------------------------------------------------------
//
CConnMonGenericEvent::CConnMonGenericEvent(
        const TUint aType,
        const TUint aConnectionId,
        TAny* aData )
        :
        CConnMonEventBase( aType, aConnectionId )
    {
    iData = aData;
    }

// Destructor
CConnMonGenericEvent::~CConnMonGenericEvent()
    {
    iData = 0;
    }

// -----------------------------------------------------------------------------
// CConnMonGenericEvent::Data
// -----------------------------------------------------------------------------
//
EXPORT_C TAny* CConnMonGenericEvent::Data() const
    {
    return iData;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonIapAvailabilityChange::CConnMonIapAvailabilityChange
// -----------------------------------------------------------------------------
//
CConnMonIapAvailabilityChange::CConnMonIapAvailabilityChange(
        const TUint aConnectionId,
        const TConnMonIapInfo* aIapInfoPtr )
        :
        CConnMonEventBase( EConnMonIapAvailabilityChange, aConnectionId )
    {
    iIapInfo.iCount = 0;

    if ( aIapInfoPtr )
        {
        iIapInfo = *aIapInfoPtr;
        }
    }

// Destructor
CConnMonIapAvailabilityChange::~CConnMonIapAvailabilityChange()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonIapAvailabilityChange::IapAvailability
// -----------------------------------------------------------------------------
//
EXPORT_C TConnMonIapInfo CConnMonIapAvailabilityChange::IapAvailability() const
    {
    return iIapInfo;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonTransmitPowerChange::CConnMonTransmitPowerChange
// -----------------------------------------------------------------------------
//
CConnMonTransmitPowerChange::CConnMonTransmitPowerChange(
        const TUint aConnectionId,
        const TUint aTransmitPower )
        :
        CConnMonEventBase( EConnMonTransmitPowerChange, aConnectionId )
    {
    iTransmitPower = aTransmitPower;
    }

// Destructor
CConnMonTransmitPowerChange::~CConnMonTransmitPowerChange()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonTransmitPowerChange::TransmitPower
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CConnMonTransmitPowerChange::TransmitPower() const
    {
    return iTransmitPower;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonSNAPAvailabilityChange::CConnMonSNAPAvailabilityChange
// -----------------------------------------------------------------------------
//
CConnMonSNAPsAvailabilityChange::CConnMonSNAPsAvailabilityChange(
        const TUint aConnectionId,
        const TUint aSNAPsAvailable,
        const TConnMonSNAPInfo* aSNAPInfoPtr )
        :
        CConnMonEventBase( EConnMonSNAPsAvailabilityChange, aConnectionId )
    {

    iSNAPInfo.iCount = 0;
    iSNAPsAvailable = aSNAPsAvailable;

    if ( aSNAPInfoPtr )
        {
        iSNAPInfo = *aSNAPInfoPtr;
        }
    }

// Destructor
CConnMonSNAPsAvailabilityChange::~CConnMonSNAPsAvailabilityChange()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonSNAPAvailabilityChange::SNAPAvailability
// -----------------------------------------------------------------------------
//
EXPORT_C TConnMonSNAPInfo CConnMonSNAPsAvailabilityChange::SNAPAvailability() const
    {
    return iSNAPInfo;
    }

// -----------------------------------------------------------------------------
// CConnMonSNAPAvailabilityChange::SNAPsAvailable
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CConnMonSNAPsAvailabilityChange::SNAPsAvailabile() const
    {
    return iSNAPsAvailable;
    }

// -----------------------------------------------------------------------------
// CConnMonNewWLANNetworkDetected::CConnMonNewWLANNetworkDetected
// -----------------------------------------------------------------------------
//

CConnMonNewWLANNetworkDetected::CConnMonNewWLANNetworkDetected(
        const TUint aConnectionId )
        :
        CConnMonEventBase( EConnMonNewWLANNetworkDetected, aConnectionId )
    {
    }

// Destructor
CConnMonNewWLANNetworkDetected::~CConnMonNewWLANNetworkDetected()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonNewWLANNetworkDetected::CConnMonNewWLANNetworkDetected
// -----------------------------------------------------------------------------
//
CConnMonOldWLANNetworkLost::CConnMonOldWLANNetworkLost(
        const TUint aConnectionId )
        :
        CConnMonEventBase( EConnMonOldWLANNetworkLost, aConnectionId )
    {
    }

// Destructor
CConnMonOldWLANNetworkLost::~CConnMonOldWLANNetworkLost()
    {
    }


// -----------------------------------------------------------------------------
// CConnMonPacketDataUnavailable::CConnMonPacketDataUnavailable
// -----------------------------------------------------------------------------
//
CConnMonPacketDataUnavailable::CConnMonPacketDataUnavailable(
        const TUint aConnectionId )
        :
        CConnMonEventBase( EConnMonPacketDataUnavailable, aConnectionId )
    {
    }

// -----------------------------------------------------------------------------
// CConnMonPacketDataUnavailable::~CConnMonPacketDataUnavailable
// -----------------------------------------------------------------------------
//
CConnMonPacketDataUnavailable::~CConnMonPacketDataUnavailable()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonPacketDataAvailable::CConnMonPacketDataAvailable
// -----------------------------------------------------------------------------
//
CConnMonPacketDataAvailable::CConnMonPacketDataAvailable(
        const TUint aConnectionId )
        :
        CConnMonEventBase( EConnMonPacketDataAvailable, aConnectionId )
    {
    }

// -----------------------------------------------------------------------------
// CConnMonPacketDataAvailable::~CConnMonPacketDataAvailable
// -----------------------------------------------------------------------------
//
CConnMonPacketDataAvailable::~CConnMonPacketDataAvailable()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonBearerGroupChange::CConnMonBearerInfoChange
// -----------------------------------------------------------------------------
//
CConnMonBearerInfoChange::CConnMonBearerInfoChange(
        const TUint aConnectionId,
        const TInt aBearerInfo )
        :
        CConnMonEventBase( EConnMonBearerInfoChange, aConnectionId )
    {
    iBearerInfo = aBearerInfo;
    }

// -----------------------------------------------------------------------------
// CConnMonBearerGroupChange::~CConnMonBearerInfoChange
// -----------------------------------------------------------------------------
//
CConnMonBearerInfoChange::~CConnMonBearerInfoChange()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonBearerGroupChange::BearerGroups
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CConnMonBearerInfoChange::BearerInfo() const
    {
    return iBearerInfo;
    }

// -----------------------------------------------------------------------------
// CConnMonBearerGroupChange::CConnMonBearerGroupChange
// -----------------------------------------------------------------------------
//
CConnMonBearerGroupChange::CConnMonBearerGroupChange(
        const TUint aConnectionId,
        const TUint aBearerGroups1,
        const TUint aBearerGroups2,
        const TBool aInternal )
        :
        CConnMonEventBase( EConnMonBearerGroupChange, aConnectionId )
    {
    iInternal = aInternal;
    iBearerGroups1 = aBearerGroups1;
    iBearerGroups2 = aBearerGroups2;
    }

// -----------------------------------------------------------------------------
// CConnMonBearerGroupChange::~CConnMonBearerGroupChange
// -----------------------------------------------------------------------------
//
CConnMonBearerGroupChange::~CConnMonBearerGroupChange()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonBearerGroupChange::BearerGroups
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CConnMonBearerGroupChange::Internal() const
    {
    return iInternal;
    }

// -----------------------------------------------------------------------------
// CConnMonBearerGroupChange::BearerGroups
// -----------------------------------------------------------------------------
//
EXPORT_C void CConnMonBearerGroupChange::BearerGroups(
        TUint& aBearerGroups1,
        TUint& aBearerGroups2 ) const
    {
    aBearerGroups1 = iBearerGroups1;
    aBearerGroups2 = iBearerGroups2;
    }

// End-of-file
