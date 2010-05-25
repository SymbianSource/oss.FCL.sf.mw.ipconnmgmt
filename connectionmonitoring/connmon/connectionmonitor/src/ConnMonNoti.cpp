/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Event notifiers (active objects).
*
*/

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_enum_partner.h>
#endif
#include <e32base.h>
#include <rmmcustomapi.h>
#include <featmgr.h>
#include <gsmerror.h>     // KErrPacketDataTsyMaxPdpContextsReached 
#include <etelpckt.h>     // KErrUmtsMaxNumOfContextExceededByNetwork

#include "ConnMonServ.h"
#include "ConnMonSess.h"
#include "CEventQueue.h"
#include "ConnMonIAP.h"
#include "ConnMonNoti.h"
#include "ConnMonAvailabilityManager.h"
#include "cellulardatausagekeyupdater.h"
#include "log.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnUpDownNotifier::CConnUpDownNotifier
// -----------------------------------------------------------------------------
//
CConnUpDownNotifier::CConnUpDownNotifier(
        CConnMonServer* aServer )
        :
        CActive( EConnMonPriorityNormal ),
        iServer( aServer )
    {
    }

// -----------------------------------------------------------------------------
// CConnUpDownNotifier::ConstructL
// -----------------------------------------------------------------------------
//
void CConnUpDownNotifier::Construct()
    {
    //LOGENTRFN("CConnUpDownNotifier::Construct()")
    CActiveScheduler::Add( this );
    LOGIT("Created CConnUpDownNotifier")
    //LOGEXITFN("CConnUpDownNotifier::Construct()")
    }

// Destructor
CConnUpDownNotifier::~CConnUpDownNotifier()
    {
    Cancel();
    iServer = NULL;
    }

// -----------------------------------------------------------------------------
// CConnUpDownNotifier::Receive
// Requests a new event (connection up/down) from RConnection
// -----------------------------------------------------------------------------
//
void CConnUpDownNotifier::Receive()
    {
    if ( IsActive() )
        {
        Cancel();
        }

    iServer->Iap()->Rconnection()->AllInterfaceNotification( iInfoBuf, iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CConnUpDownNotifier::DoCancel
// Cancels the request from RConnection.
// -----------------------------------------------------------------------------
//
void CConnUpDownNotifier::DoCancel()
    {
    if ( IsActive() )
        {
        iServer->Iap()->Rconnection()->CancelAllInterfaceNotification();
        }
    }

// -----------------------------------------------------------------------------
// CConnUpDownNotifier::RunL
// Handles the event that has arrived from RConnection
// -----------------------------------------------------------------------------
//
void CConnUpDownNotifier::RunL()
    {
    //LOGENTRFN("CConnUpDownNotifier::RunL()")

    // All RunL():s outside CServer-derived main class MUST NOT LEAVE.
    // Use TRAPD when needed.

    LOGIT(".")
    LOGIT1("RunL: CConnUpDownNotifier, status <%d>", iStatus.Int())

    if ( iStatus.Int() != KErrNone )
        {
        iErrorCounter++;
        if ( iErrorCounter > KConnMonEventErrorThreshold )
            {
            LOGIT1("Over %d consecutive errors, stopping", KConnMonEventErrorThreshold)
            return;
            }
        }
    else
        {
        iErrorCounter = 0;
        // Send event to clients
        TInt err( KErrNone );
        TBearerInfo bearerInfo;
        TConnInfo connInfo(
                iInfoBuf().iConnectionInfo.iIapId,
                iInfoBuf().iConnectionInfo.iNetId,
                0,
                0,
                bearerInfo );

        TInt listeners( iServer->NumberOfListeners() );
        LOGIT3("CConnUpDownNotifier, %d listeners, iap id %d, net id %d",
                listeners, connInfo.iIapId, connInfo.iNetId)
        iEventInfo.Reset();

        if ( iInfoBuf().iState == EInterfaceUp )
            {
            // Add to the connection table and fill in the new connectioId to connInfo
            TRAPD( ret, ( err = iServer->Iap()->AddConnectionL( connInfo ) ) );

            if ( ( ret != KErrNone ) || ( err != KErrNone ) )
                {
                LOGIT2("SERVER: FAILED to add a new connection <%d>,<%d>", ret, err)
                Receive();
                return; // Can't leave
                }

            // Send event to all clients that are listening
            if ( listeners > 0)
                {
                iEventInfo.iEventType    = EConnMonCreateConnection;
                iEventInfo.iConnectionId = connInfo.iConnectionId;

                iServer->EventQueue()->Add( iEventInfo );
                }

            LOGIT1("SERVER: EVENT -> connection %d created.", connInfo.iConnectionId)
            }
        else if ( iInfoBuf().iState == EInterfaceDown )
            {
            if ( listeners == 0 )
                {
                // Remove from the connection array. Note. when the notifications
                // are ON event handlers will remove the connection from the array.
                iServer->Iap()->RemoveConnection( connInfo );
                }
            else
                {
                TBool alreadyNotified( EFalse );

                // This updates the correct 'iConnectionId' and 'iBearer' to connInfo
                TInt ret = iServer->Iap()->GetDeleteNotifyStatus( connInfo, alreadyNotified );

                if ( ret == KErrNone )
                    {
                    CSubConnUpDownNotifier* subConnUpDownNotifier = 0;

                    ret = iServer->Iap()->GetSubConnUpDownNotifier(
                            connInfo.iConnectionId,
                            &subConnUpDownNotifier );

                    if ( ret == KErrNone )
                        {
                        subConnUpDownNotifier->SetInterfaceClosed();

                        if ( !subConnUpDownNotifier->DeleteSent() )
                            {
                            TNifProgress progress;
                            err = iServer->Iap()->LastProgressError(
                                    connInfo.iConnectionId,
                                    progress );

                            if ( progress.iError == KErrDisconnected )
                                {
                                // Enable WLAN scan for the next IAP availability check
                                // because the current bearer has been lost (-36). MPM needs
                                // a fresh list of available iaps on connection closure.
                                iServer->Iap()->EnableWlanScan();
                                }

                            // Progress and subconn notifiers have died and no delete has been sent
                            subConnUpDownNotifier->SendDeletedEvent();
                            }
                        else
                            {
                            // Delete has been sent. Just remove from internal table.
                            iServer->Iap()->RemoveConnection( connInfo );
                            }
                        }
                    }

                }
            LOGIT3("SERVER: EVENT (allinterface) -> connection %d (%d, %d) closed.",
                    connInfo.iConnectionId, connInfo.iIapId, connInfo.iNetId)
            }
        else
            {
            LOGIT("SERVER: Unknown event")
            Receive();
            return;
            }

        LOGIT("CConnUpDownNotifier::RunL triggered HandleAvailabilityChange()")
        iServer->AvailabilityManager()->HandleAvailabilityChange();
        }
    // New request
    Receive();
    //LOGEXITFN("CConnUpDownNotifier::RunL()")
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDataNotifier::CDataNotifier
// -----------------------------------------------------------------------------
//
CDataNotifier::CDataNotifier(
        CConnMonServer* aServer,
        RConnection* aConnection,
        const TUint& aConnectionId,
        const TUint& aSubConnectionId,
        const TInt& aDirection )
        :
        CActive( EConnMonPriorityNormal ),
        iServer( aServer ),
        iConnection( aConnection ),
        iConnectionId( aConnectionId),
        iSubConnectionId( aSubConnectionId ),
        iDirection( aDirection ),
        iVolume( 0 ),
        iPckgVolume( iVolume )
    {
    }

// -----------------------------------------------------------------------------
// CDataNotifier::Construct
// -----------------------------------------------------------------------------
//
void CDataNotifier::Construct()
    {
    //LOGENTRFN("CDataNotifier::Construct()")
    CActiveScheduler::Add( this );
    LOGIT("Created CDataNotifier")
    //LOGEXITFN("CDataNotifier::Construct()")
    }

// Destructor
CDataNotifier::~CDataNotifier()
    {
    Cancel();

    iServer = NULL;
    iConnection = NULL;
    }

// -----------------------------------------------------------------------------
// CDataNotifier::Receive
// Requests a new event (data transferred) from RConnection
// -----------------------------------------------------------------------------
//
void CDataNotifier::Receive()
    {
    // Don't make a new request if previous
    // status indicates that connection is closing
    if ( iStatus == KErrCancel )
        {
        return;
        }

    if ( IsActive() )
        {
        return;
        }

    iVolume = 0;

    if ( iDirection == EDownlink )
        {
        iServer->CalculateThreshold(
                iConnectionId,
                EDownlinkThreshold,
                iThreshold );

        if ( iThreshold > 0 )
            {
            iConnection->DataReceivedNotificationRequest(
                    iThreshold,
                    iPckgVolume,
                    iStatus );
            SetActive();
            }
        }
    else
        {
        iServer->CalculateThreshold(
                iConnectionId,
                EUplinkThreshold,
                iThreshold );
        if ( iThreshold > 0 )
            {
            iConnection->DataSentNotificationRequest(
                    iThreshold,
                    iPckgVolume,
                    iStatus );
            SetActive();
            }
        }
    }

// -----------------------------------------------------------------------------
// CDataNotifier::DoCancel
// Cancels the request from RConnection.
// -----------------------------------------------------------------------------
//
void CDataNotifier::DoCancel()
    {
    if ( IsActive() )
        {
        if ( iDirection == EDownlink )
            {
            iConnection->DataReceivedNotificationCancel();
            }
        else
            {
            iConnection->DataSentNotificationCancel();
            }
        }
    }

// -----------------------------------------------------------------------------
// CDataNotifier::RunL
// Handles the event that has arrived from RConnection
// -----------------------------------------------------------------------------
//
void CDataNotifier::RunL()
    {
    //LOGENTRFN("CDataNotifier::RunL()")

    // All RunL():s outside CServer-derived main class MUST NOT LEAVE.
    // Use TRAPD when needed.

    LOGIT(".")
    LOGIT1("RunL: CDataNotifier, status %d", iStatus.Int())

    if ( iStatus.Int() != KErrNone )
        {
        //LOGIT2("SERVER: Data volume event FAILED <%d>, id <%d>", iStatus.Int(), iConnectionId )
        iErrorCounter++;
        if ( iErrorCounter > KConnMonEventErrorThreshold )
            {
            LOGIT1("Over %d consecutive errors, stopping", KConnMonEventErrorThreshold)
            return;
            }
        }
    else
        {
        iErrorCounter = 0;
        iEventInfo.Reset();

        // Send event(s) to clients
        iEventInfo.iConnectionId = iConnectionId;
        iEventInfo.iSubConnectionId = iSubConnectionId;
        iEventInfo.iData = iPckgVolume();

        if ( iDirection == EDownlink )
            {
            iEventInfo.iEventType = EConnMonDownlinkDataThreshold;
            LOGIT2("SERVER: EVENT -> Downlink data <%d>, id <%d>", iEventInfo.iData, iEventInfo.iConnectionId )
            }
        else
            {
            iEventInfo.iEventType = EConnMonUplinkDataThreshold;
            LOGIT2("SERVER: EVENT -> Uplink data <%d>, id <%d>", iEventInfo.iData, iEventInfo.iConnectionId )
            }

        iServer->EventQueue()->Add( iEventInfo );
        }
    // New request
    Receive();
    //LOGEXITFN("CDataNotifier::RunL()")
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CProgressNotifier::CProgressNotifier
// Priority is set to 10. Progress notifier must have higher priority than
// connection up/down notifier because otherwise connection delete event will
// arrive before status reaches KLinkLayerClosed.
// -----------------------------------------------------------------------------
//
CProgressNotifier::CProgressNotifier(
        CConnMonServer* aServer,
        RConnection* aConnection,
        const TUint& aConnectionId,
        const TUint& aSubConnectionId )
        :
        CActive( EConnMonPriorityHigh ),
        iServer( aServer ),
        iConnection( aConnection ),
        iConnectionId( aConnectionId),
        iSubConnectionId( aSubConnectionId ),
        iFilter( KNoFiltering )
    {
    }

// -----------------------------------------------------------------------------
// CProgressNotifier::Construct
// -----------------------------------------------------------------------------
//
void CProgressNotifier::Construct()
    {
    //LOGENTRFN("CProgressNotifier::Construct()")
    CActiveScheduler::Add( this );
    LOGIT("Created CProgressNotifier")
    //LOGEXITFN("CProgressNotifier::Construct()")
    }

// Destructor
CProgressNotifier::~CProgressNotifier()
    {
    if ( IsActive() )
        {
        Cancel();
        }

    iServer = NULL;
    iConnection = NULL;
    }

// -----------------------------------------------------------------------------
// CProgressNotifier::Receive
// Requests a new event (connection status changed) from RConnection
// -----------------------------------------------------------------------------
//
void CProgressNotifier::Receive()
    {
    if ( IsActive() )
        {
        Cancel();
        }

    iInfoBuf().iStage = 0;
    iInfoBuf().iError = 0;

    iConnection->ProgressNotification( iInfoBuf, iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CProgressNotifier::Receive
// Requests a new event (connection status changed) from RConnection
// Uses filtering until status reaches aFilter
// -----------------------------------------------------------------------------
//
void CProgressNotifier::Receive( const TInt aFilter )
    {
    iFilter = aFilter;
    Receive();
    }

// -----------------------------------------------------------------------------
// CProgressNotifier::IsAuthDelete
// ETrue if the user has deactivated the connection
// by using Connection Monitor or if the applications issued
// RConnection::Stop( RConnection::EStopAuthoritative )
// -----------------------------------------------------------------------------
//
TBool CProgressNotifier::IsAuthDelete()
    {
    if ( iInfoBuf().iError == KErrConnectionTerminated )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CProgressNotifier::CanReconnect
// ETrue if might reconnect after closing.
// -----------------------------------------------------------------------------
//
TBool CProgressNotifier::CanReconnect() const
    {
    // Reconnect is disabled for the moment
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CProgressNotifier::DoCancel
// Cancels the request from RConnection.
// -----------------------------------------------------------------------------
//
void CProgressNotifier::DoCancel()
    {
    if ( IsActive() )
        {
        iConnection->CancelProgressNotification();
        }
    }

// -----------------------------------------------------------------------------
// CProgressNotifier::SendClosedEvent
// -----------------------------------------------------------------------------
//
void CProgressNotifier::SendClosedEvent()
    {
    iEventInfo.Reset();

    iEventInfo.iEventType       = EConnMonConnectionStatusChange;
    iEventInfo.iConnectionId    = iConnectionId;
    iEventInfo.iSubConnectionId = iSubConnectionId;
    iEventInfo.iData            = KLinkLayerClosed;

    // Send event to all clients that are listening
    iServer->EventQueue()->Add( iEventInfo );
    }

// -----------------------------------------------------------------------------
// CProgressNotifier::RunL
// Handles the event that has arrived from RConnection
// -----------------------------------------------------------------------------
//
void CProgressNotifier::RunL()
    {
    //LOGENTRFN("CProgressNotifier::RunL()")

    // All RunL():s outside CServer-derived main class MUST NOT LEAVE.
    // Use TRAPD when needed.

    LOGIT(".")
    LOGIT1("RunL: CProgressNotifier, status %d", iStatus.Int())

    if ( iStatus.Int() != KErrNone )
        {
        LOGIT2("SERVER: connection status event FAILED <%d>, id %d", iStatus.Int(), iConnectionId)
        }
    else
        {
        if ( iInfoBuf().iStage != static_cast< TInt >( iEventInfo.iData ) )
            {
            iEventInfo.Reset();
            
            // Send only new stage info to clients
            iEventInfo.iEventType       = EConnMonConnectionStatusChange;
            iEventInfo.iConnectionId    = iConnectionId;
            iEventInfo.iSubConnectionId = iSubConnectionId;
            iEventInfo.iData            = iInfoBuf().iStage;

            LOGIT3("SERVER: EVENT -> Connection %d, status %d <%d>",
                    iEventInfo.iConnectionId, iEventInfo.iData, iInfoBuf().iError)

            if ( ( iFilter == KNoFiltering ) || ( iInfoBuf().iStage > iFilter ) )
                {
                // Send event to all clients that are listening
                iServer->EventQueue()->Add( iEventInfo );

                // Start other event watchers when the connection status reaches
                // KLinkLayerOpen for the first time.
                if ( iInfoBuf().iStage == KLinkLayerOpen )
                    {
                    // Start subconnection up/down notifier when stage reaches KLinkLayerOpen
                    CSubConnUpDownNotifier* subConnUpDownNotifier = 0;

                    TInt err = iServer->Iap()->GetSubConnUpDownNotifier(
                            iConnectionId,
                            &subConnUpDownNotifier );

                    if ( KErrNone == err )
                        {
                        if ( !subConnUpDownNotifier->IsActive() )
                            {
                            subConnUpDownNotifier->Receive();
                            }
                        }

                    // Start activity notifier when stage reaches KLinkLayerOpen
                    TRAPD( ret, ( iServer->Iap()->LaunchActivityNotifierL( iConnectionId ) ) );

                    if ( ret != KErrNone )
                        {
                        LOGIT1("SERVER: FAILED to start activity notifier <%d>", ret)
                        return; // Can't leave
                        }

                    // Start data (uplink & downlink) notifiers when stage reaches KLinkLayerOpen
                    iServer->Iap()->LaunchDataNotifiers( iConnectionId );

                    ret = iServer->Iap()->LaunchBearerNotifier( iConnectionId );
                    if ( ret != KErrNone )
                        {
                        LOGIT1("SERVER: FAILED to start bearer(new) notifier <%d>", ret)
                        }
                        
                    // Update IAP availability if WLAN connection is fully started.
                    // When WLAN connection status is KLinkLayerOpen it is the only
                    // WLAN available.
                    //
                    TBearerInfo bearerInfo;

                    ret = iServer->Iap()->GetBearerInfo( iConnectionId, bearerInfo );
                    
                    if ( ret == KErrNone && bearerInfo.iBearer == EBearerInfoWLAN )
                        {
                        LOGIT("CProgressNotifier::RunL triggered HandleAvailabilityChange()")
                        iServer->AvailabilityManager()->HandleAvailabilityChange();	
                        }	    
                    }

                iFilter = KNoFiltering;
                }
            else
                {
                LOGIT("SERVER: This is old info. Packet was filtered out.")
                }
            }

        if ( iInfoBuf().iError == KErrNone ||
             iInfoBuf().iError == KErrGprsInsufficientResources            || // -4154
             iInfoBuf().iError == KErrPacketDataTsyMaxPdpContextsReached   || // -6000
             iInfoBuf().iError == KErrUmtsMaxNumOfContextExceededByNetwork || // -4179
             iInfoBuf().iError == KErrUmtsMaxNumOfContextExceededByPhone )    // -4178
            {            	
            // New request, DisconnectDlg might be shown for the error codes above.
            Receive();
            }
        else
            {
            LOGIT1("CProgressNotifier::RunL() - connection closing - iInfoBuf().iError: %d", iInfoBuf().iError)
            // Connection is closing.
            CSubConnUpDownNotifier* subConnUpDownNotifier = 0;
            TInt err = iServer->Iap()->GetSubConnUpDownNotifier(
                    iConnectionId,
                    &subConnUpDownNotifier );

            if ( KErrNone == err )
                {
                // Subconn down notifier has stopped and allinterface closed event has arrived
                if ( !subConnUpDownNotifier->IsActive() )
                    {
                    subConnUpDownNotifier->SendDeletedEvent();
                    }
                }

            if ( iInfoBuf().iError == KErrDisconnected )
                {
                // Enable WLAN scan when IAP availability is check for the
                // next time because current bearer has been lost (-36).
                // MPM needs a fresh list of available iaps.
                iServer->Iap()->EnableWlanScan();
                }
            }
        }
    //LOGEXITFN("CProgressNotifier::RunL()")
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNetwStatusNotifier::CNetwStatusNotifier
// -----------------------------------------------------------------------------
//
CNetwStatusNotifier::CNetwStatusNotifier(
        CConnMonServer* aServer )
        :
        CActive( EConnMonPriorityNormal ),
        iServer( aServer )
    {
    }

// -----------------------------------------------------------------------------
// CNetwStatusNotifier::Construct
// -----------------------------------------------------------------------------
//
void CNetwStatusNotifier::Construct()
    {
    //LOGENTRFN("CNetwStatusNotifier::Construct()")

    CActiveScheduler::Add( this );
    TInt ret = iServer->Iap()->PacketService()->GetStatus( iPacketStatus );
    if ( ret != KErrNone )
        {
        iPacketStatus = RPacketService::EStatusUnattached;
        }
    LOGIT("Created CNetwStatusNotifier")

    //LOGEXITFN("CNetwStatusNotifier::Construct()")
    }

// -----------------------------------------------------------------------------
// CNetwStatusNotifier::NewL
// -----------------------------------------------------------------------------
//
CNetwStatusNotifier* CNetwStatusNotifier::NewL( CConnMonServer* aServer )
    {
    CNetwStatusNotifier* self = new( ELeave ) CNetwStatusNotifier( aServer );
    self->Construct();
    return self;
    }

// Destructor
CNetwStatusNotifier::~CNetwStatusNotifier()
    {
    Cancel();
    iServer = NULL;
    }

// -----------------------------------------------------------------------------
// CNetwStatusNotifier::Receive
// Requests a new event (network status changed) from ETel
// -----------------------------------------------------------------------------
//
void CNetwStatusNotifier::Receive()
    {
    if ( IsActive() )
        {
        return;
        }

    iServer->Iap()->PacketService()->NotifyStatusChange( iStatus, iPacketStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CNetwStatusNotifier::DoCancel
// Cancels the request from ETel.
// -----------------------------------------------------------------------------
//
void CNetwStatusNotifier::DoCancel()
    {
    if ( IsActive() )
        {
        iServer->Iap()->PacketService()->CancelAsyncRequest( EPacketNotifyStatusChange );
        }
    }

// -----------------------------------------------------------------------------
// CNetwStatusNotifier::RunL
// Handles the event that has arrived from ETel
// -----------------------------------------------------------------------------
//
void CNetwStatusNotifier::RunL()
    {
    //LOGENTRFN("CNetwStatusNotifier::RunL()")

    // All RunL():s outside CServer-derived main class MUST NOT LEAVE.
    // Use TRAPD when needed.

    LOGIT(".")
    LOGIT1("RunL: CNetwStatusNotifier, status %d", iStatus.Int())

    if ( iStatus.Int() != KErrNone )
        {
        LOGIT1("SERVER: network status event FAILED <%d>", iStatus.Int())
        }
    else
        {
        iEventInfo.Reset();

        iEventInfo.iEventType = EConnMonNetworkStatusChange;
        iEventInfo.iSubConnectionId = 0;
        iEventInfo.iData = iServer->Iap()->CalculateNetworkStatus( iPacketStatus );

        // Find out bearer ID.
        iServer->Iap()->GetBearerId( iEventInfo.iConnectionId );

        LOGIT2("SERVER: EVENT -> Network status %d, id %d", iEventInfo.iData, iEventInfo.iConnectionId)

        // Send event to all clients that are listening
        iServer->EventQueue()->Add( iEventInfo );

        LOGIT("CNetwStatusNotifier::RunL triggered HandleAvailabilityChange()")
        iServer->AvailabilityManager()->HandleAvailabilityChange();
        }

    // New request
    Receive();
    //LOGEXITFN("CNetwStatusNotifier::RunL()")
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNetwRegistrationNotifier::CNetwRegistrationNotifier
// Notifies changes in network registration status.
// Notifies also changes in CSD bearer availability and CSD IAP availability.
// -----------------------------------------------------------------------------
//
CNetwRegistrationNotifier::CNetwRegistrationNotifier(
        CConnMonServer* aServer,
        RMobilePhone& aMobilePhone )
        :
        CActive( EConnMonPriorityNormal ),
        iServer( aServer ),
        iMobilePhone( aMobilePhone )
    {
    }

// -----------------------------------------------------------------------------
// CNetwRegistrationNotifier::Construct
// -----------------------------------------------------------------------------
//
void CNetwRegistrationNotifier::Construct()
    {
    //LOGENTRFN("CNetwRegistrationNotifier::Construct()")
    CActiveScheduler::Add( this );

    iRegistration = RMobilePhone::ERegistrationUnknown;
    iEventInfo.iData = RMobilePhone::ERegistrationUnknown;

    TBool cellSupportsCSD( EFalse );

    // Is CSD supported both by the cell and the phone
    iServer->Iap()->GetBearerSupportInfo(
            EBearerIdCSD,
            cellSupportsCSD,
            iPhoneSupportsCSD );

    if ( cellSupportsCSD && iPhoneSupportsCSD )
         {
         iCSDSupported = ETrue;
         }
    LOGIT("Created CNetwRegistrationNotifier")

    //LOGEXITFN("CNetwRegistrationNotifier::Construct()")
    }

// -----------------------------------------------------------------------------
// CNetwRegistrationNotifier::NewL
// -----------------------------------------------------------------------------
//
CNetwRegistrationNotifier* CNetwRegistrationNotifier::NewL(
        CConnMonServer* aServer,
        RMobilePhone& aMobilePhone )
    {
    CNetwRegistrationNotifier* self = new( ELeave ) CNetwRegistrationNotifier(
            aServer,
            aMobilePhone );

    self->Construct();
    return self;
    }

// Destructor
CNetwRegistrationNotifier::~CNetwRegistrationNotifier()
    {
    Cancel();
    iServer = NULL;
    }

// -----------------------------------------------------------------------------
// CNetwRegistrationNotifier::Receive
// Requests a new event (network registration changed) from ETel
// -----------------------------------------------------------------------------
//
void CNetwRegistrationNotifier::Receive()
    {
    if ( IsActive() )
        {
        return;
        }

    iMobilePhone.NotifyNetworkRegistrationStatusChange( iStatus, iRegistration );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CNetwRegistrationNotifier::DoCancel
// Cancels the request from ETel.
// -----------------------------------------------------------------------------
//
void CNetwRegistrationNotifier::DoCancel()
    {
    if ( IsActive() )
        {
        iMobilePhone.CancelAsyncRequest( EMobilePhoneNotifyNetworkRegistrationStatusChange );
        }
    }

// -----------------------------------------------------------------------------
// CNetwRegistrationNotifier::RunL
// Handles the event that has arrived from ETel
// -----------------------------------------------------------------------------
//
void CNetwRegistrationNotifier::RunL()
    {
    //LOGENTRFN("CNetwRegistrationNotifier::RunL()")

    // All RunL():s outside CServer-derived main class MUST NOT LEAVE.
    // Use TRAPD when needed.

    LOGIT(".")
    LOGIT1("RunL: CNetwRegistrationNotifier, status <%d>", iStatus.Int())

    if ( iStatus.Int() != KErrNone )
        {
        LOGIT1("SERVER: network registration event FAILED <%d>", iStatus.Int())
        }
    else
        {
        TUint value( RMobilePhone::ERegistrationUnknown );

        value = iServer->Iap()->CalculateNetworkRegistration( iRegistration );

        // Is this a new registration status value
        if ( iEventInfo.iData != value )
            {
            // Send Network registration status changed event when FeatureFlag is ON
            if ( FeatureManager::FeatureSupported( KFeatureIdNetworkRegistration ) )
                {
                iEventInfo.Reset();

                iEventInfo.iData = value;
                iEventInfo.iEventType = EConnMonNetworkRegistrationChange;

                // Find out bearer ID.
                iServer->Iap()->GetBearerId( iEventInfo.iConnectionId );

                if ( iEventInfo.iConnectionId == EBearerIdGPRS )
                    {
                    iEventInfo.iConnectionId = EBearerIdGSM;
                    }

                iServer->EventQueue()->Add( iEventInfo );
                }

            LOGIT2("SERVER: EVENT -> Network registration %d, id %d", iEventInfo.iData, iEventInfo.iConnectionId)

            if ( iPhoneSupportsCSD )
                {
                iEventInfo.Reset();
                iEventInfo.iEventType = EConnMonBearerAvailabilityChange;

                if ( iRegistration == RMobilePhone::ERegisteredBusy ||
                     iRegistration == RMobilePhone::ERegisteredOnHomeNetwork ||
                     iRegistration == RMobilePhone::ERegisteredRoaming )
                    {
                    iEventInfo.iData = ETrue;
                    }

                if ( static_cast< TUint >( iCSDSupported ) != iEventInfo.iData )
                    {
                    // Find out bearer ID for CSD.
                    iServer->Iap()->GetBearerId( iEventInfo.iConnectionId, ETrue );

                    // Send the event only if threshold is set by some of the clients.
                    TUint threshold( 0 );

                    iServer->CalculateThreshold(
                            EBearerIdAll,
                            EBearerAvailabilityThreshold,
                            threshold );

                    if ( threshold > 0 )
                        {
                        iServer->EventQueue()->Add( iEventInfo );
                        }

                    iCSDSupported = iEventInfo.iData;

                    LOGIT2("SERVER: EVENT -> Bearer availability changed: data %d, id %d", iEventInfo.iData, iEventInfo.iConnectionId)

                    // Send CSD IAP availability event only when packet services are not available.
                    // Otherwise CSD IAP availability information goes along with packet IAP event.
                    TUint32 capsPhone( 0 );
                    TInt err = iMobilePhone.GetMultimodeCaps( capsPhone );
                    }
                }

            // Store to compare next event correctly
            iEventInfo.iData = value;
            
            // Update KCurrentCellularDataUsage -key in Repository KCRUidCmManage.
            // Key tells applications whether it is allowed to use packet data or not.
            TRAP_IGNORE( iServer->CellularDataUsageKeyUpdater()->UpdateKeyL(
                iServer->Iap()->CalculateNetworkRegistration_v2( iRegistration ) ); )
            }
        LOGIT("CNetwRegistrationNotifier::RunL triggered HandleAvailabilityChange()")
        iServer->AvailabilityManager()->HandleAvailabilityChange();
        }
    // New request
    Receive();
    //LOGEXITFN("CNetwRegistrationNotifier::RunL()")
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBearerNotifier::CBearerNotifier
// Notifies when the bearer changes from EGPRS to GPRS on vice versa.
// This information is cell specific.
// -----------------------------------------------------------------------------
//
CBearerNotifier::CBearerNotifier(
        CConnMonServer* aServer,
        RMobilePhone& aMobilePhone )
        :
        CActive( EConnMonPriorityNormal ),
        iServer( aServer ),
        iMobilePhone( aMobilePhone ),
        iGprsInfo(),
        iGprsInfoPckg( iGprsInfo ),
        iErrorCounter( 0 )
    {
    }

// -----------------------------------------------------------------------------
// CBearerNotifier::ConstructL
// -----------------------------------------------------------------------------
//
void CBearerNotifier::ConstructL()
    {
    //LOGENTRFN("CBearerNotifier::ConstructL()")
    CActiveScheduler::Add( this );

    // Open ETel custom API
    TInt ret = iCustomApi.Open( iMobilePhone );
    if ( ret != KErrNone )
        {
        LOGIT1("ERROR starting bearer notifier. RMmCustomAPI.Open() failed <%d>", ret)
        User::Leave( ret );
        }

    iEventInfo.iData = EBearerUnknown;
    LOGIT("Created CBearerNotifier")

    //LOGEXITFN("CBearerNotifier::ConstructL()")
    }

// -----------------------------------------------------------------------------
// CBearerNotifier::NewL
// -----------------------------------------------------------------------------
//
CBearerNotifier* CBearerNotifier::NewL(
        CConnMonServer* aServer,
        RMobilePhone& aMobilePhone )
    {
    CBearerNotifier* self = new( ELeave ) CBearerNotifier(
            aServer,
            aMobilePhone );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CBearerNotifier::~CBearerNotifier()
    {
    Cancel();

    // Close ETel custom API
    iCustomApi.Close();

    iServer = NULL;
    }

// -----------------------------------------------------------------------------
// CBearerNotifier::Receive
// Requests a new event (bearer changed) from ETel Custom API
// -----------------------------------------------------------------------------
//
void CBearerNotifier::Receive()
    {
    if ( IsActive() )
        {
        return;
        }

    iCustomApi.NotifyEGprsInfoChange( iStatus, iGprsInfoPckg );

    SetActive();
    }

// -----------------------------------------------------------------------------
// CBearerNotifier::DoCancel
// Cancels the request from ETel Custom API.
// -----------------------------------------------------------------------------
//
void CBearerNotifier::DoCancel()
    {
    if ( IsActive() )
        {
        iCustomApi.CancelAsyncRequest( ECustomNotifyEGprsInfoChange );
        }
    }

// -----------------------------------------------------------------------------
// CBearerNotifier::RunL
// Handles the event that has arrived from ETel
// -----------------------------------------------------------------------------
//
void CBearerNotifier::RunL()
    {
    //LOGENTRFN("CBearerNotifier::RunL()")

    // All RunL():s outside CServer-derived main class MUST NOT LEAVE.
    // Use TRAPD when needed.

    LOGIT(".")
    LOGIT1("RunL: CBearerNotifier, status <%d>", iStatus.Int())

    if ( iStatus.Int() != KErrNone )
        {
        iErrorCounter++;
        if ( iErrorCounter > KConnMonEventErrorThreshold )
            {
            LOGIT1("Over %d consecutive errors, stopping", KConnMonEventErrorThreshold)
            //LOGEXITFN("CBearerNotifier::RunL()")
            return;
            }
        }
    else
        {
        iErrorCounter = 0;
        LOGIT1("SERVER: CBearerNotifier: iGprsInfo <%d>", iGprsInfoPckg().iGprsInfo)
        iEventInfo.Reset();

        // Check that that we are in GSM mode. This event will be sent only in GSM mode.
        RMobilePhone::TMobilePhoneNetworkMode mode( RMobilePhone::ENetworkModeUnknown );

        TInt ret = iMobilePhone.GetCurrentMode( mode );

        if ( ( ret == KErrNone ) && ( mode == RMobilePhone::ENetworkModeGsm ) )
            {
            if ( iGprsInfoPckg().iGprsInfo == RMmCustomAPI::EEdgeGprs )
                {
                iEventInfo.iData = EBearerEdgeGPRS;
                }
            else
                {
                iEventInfo.iData = EBearerGPRS;
                }

            iEventInfo.iEventType = EConnMonBearerChange;
            iEventInfo.iConnectionId = EBearerIdGPRS;

            iServer->EventQueue()->Add( iEventInfo );

            LOGIT3("SERVER: EVENT -> Bearer changed: %d, data %d, id %d", iGprsInfoPckg().iGprsInfo, iEventInfo.iData, iEventInfo.iConnectionId)

            // New Impl.
            iEventInfo.Reset();
            iEventInfo.iEventType = EConnMonBearerInfoChange;
            iEventInfo.iConnectionId = EBearerIdAll; // means that this is not a connection related event

            if ( iGprsInfoPckg().iGprsInfo == RMmCustomAPI::EEdgeGprs )
                {
                iEventInfo.iData = EBearerInfoEdgeGPRS;
                }
            else
                {
                iEventInfo.iData = EBearerInfoGPRS;
                }

            iServer->EventQueue()->Add( iEventInfo );

            LOGIT2("SERVER: EVENT -> Bearer info changed: %d, data %d", iGprsInfoPckg().iGprsInfo, iEventInfo.iData)
            }
        }

    // New request
    Receive();
    //LOGEXITFN("CBearerNotifier::RunL()")
    }

// -----------------------------------------------------------------------------
// CBearerNotifier::CWcdmaBearerNotifier
// Notifies when the bearer changes from Wcdma to Hsdpa on vice versa.
// This information is cell specific.
// -----------------------------------------------------------------------------
//
CWcdmaBearerNotifier::CWcdmaBearerNotifier(
        CConnMonServer* aServer,
        RTelServer& aTelServer )
        :
        CActive( EConnMonPriorityNormal ),
        iServer( aServer ),
        iTelServer( aTelServer ),
        iNetInfo(),
        iNetInfoPckg( iNetInfo )
    {
    }

// -----------------------------------------------------------------------------
// CWcdmaBearerNotifier::ConstructL
// -----------------------------------------------------------------------------
//
void CWcdmaBearerNotifier::ConstructL()
    {
    //LOGENTRFN("CWcdmaBearerNotifier::ConstructL()")
    CActiveScheduler::Add( this );

    iPreviousBearerInfo = EBearerInfoWCDMA;
    iEventInfo.iData = EBearerUnknown;

    RTelServer::TPhoneInfo info;
    User::LeaveIfError( iTelServer.GetPhoneInfo( 0, info ) );
    User::LeaveIfError( iMobilePhone.Open( iTelServer, info.iName ) );

    LOGIT("Created CWcdmaBearerNotifier")
    //LOGEXITFN("CWcdmaBearerNotifier::ConstructL()")
    }

// -----------------------------------------------------------------------------
// CWcdmaBearerNotifier::NewL
// -----------------------------------------------------------------------------
//
CWcdmaBearerNotifier* CWcdmaBearerNotifier::NewL(
        CConnMonServer* aServer,
        RTelServer& aTelServer )
    {
    CWcdmaBearerNotifier* self = new( ELeave ) CWcdmaBearerNotifier(
            aServer,
            aTelServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CWcdmaBearerNotifier::~CWcdmaBearerNotifier()
    {
    Cancel();
    iServer = NULL;

    iMobilePhone.Close();
    }

// -----------------------------------------------------------------------------
// CWcdmaBearerNotifier::Receive
// Requests a new event (bearer changed) from ETel Custom API
// -----------------------------------------------------------------------------
//
void CWcdmaBearerNotifier::Receive()
    {
    if ( IsActive() )
        {
        return;
        }

    iMobilePhone.NotifyCurrentNetworkChange( iStatus, iNetInfoPckg );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CWcdmaBearerNotifier::DoCancel
// Cancels the request from ETel Custom API.
// -----------------------------------------------------------------------------
//
void CWcdmaBearerNotifier::DoCancel()
    {
    if ( IsActive() )
        {
        iMobilePhone.CancelAsyncRequest( EMobilePhoneNotifyCurrentNetworkNoLocationChange );
        }
    }

// -----------------------------------------------------------------------------
// CWcdmaBearerNotifier::RunL
// Handles the event that has arrived from ETel
// -----------------------------------------------------------------------------
//
void CWcdmaBearerNotifier::RunL()
    {
    //LOGENTRFN("CWcdmaBearerNotifier::RunL()")

    // All RunL():s outside CServer-derived main class MUST NOT LEAVE.
    // Use TRAPD when needed.

    LOGIT(".")
    LOGIT1("RunL: CWcdmaBearerNotifier, status <%d>", iStatus.Int())

    if ( KErrNone != iStatus.Int() )
        {
        LOGIT1("SERVER: CWcdmaBearerNotifier:Bearer changed event FAILED <%d>", iStatus.Int())
        }
    else
        {
        LOGIT2("SERVER: CWcdmaBearerNotifier: EGPRS %d, HSDPA %d",
                iNetInfoPckg().iEgprsAvailableIndicator, iNetInfoPckg().iHsdpaAvailableIndicator)
        iEventInfo.Reset();

        // Check that that we are in Wcdma mode. This event will be
        // sent only in Wcdma mode.
        RMobilePhone::TMobilePhoneNetworkMode mode( RMobilePhone::ENetworkModeUnknown );

        TInt ret = iMobilePhone.GetCurrentMode( mode );
        LOGIT1("SERVER: CWcdmaBearerNotifier: current mode %d", mode)

        if ( ( KErrNone == ret ) && ( mode == RMobilePhone::ENetworkModeWcdma ) )
            {
            iEventInfo.iEventType = EConnMonBearerInfoChange;
            iEventInfo.iConnectionId = EBearerIdAll; // Means that this is not a connection related event

            if ( iNetInfoPckg().iHsdpaAvailableIndicator )
                {
                iEventInfo.iData = EBearerInfoHSDPA;
                }
            else
                {
                iEventInfo.iData = EBearerInfoWCDMA;
                }

            if ( iPreviousBearerInfo != iEventInfo.iData )
                {
                iPreviousBearerInfo = (TConnMonBearerInfo)iEventInfo.iData;

                LOGIT1("SERVER: EVENT -> Wcdma bearer info changed, new bearer info %d", iEventInfo.iData)
                iServer->EventQueue()->Add( iEventInfo );
                }
            else
                {
                LOGIT1("SERVER: Wcdma bearer info NOT changed, bearer info %d", iEventInfo.iData)
                }
            }
        }
    // New request
    Receive();
    //LOGEXITFN("CWcdmaBearerNotifier::RunL()")
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CModeNotifier::CModeNotifier
// Notifies when the bearer changes from 3G (WCDMA) to 2G (GPRS/EGPRS) or vice versa.
// -----------------------------------------------------------------------------
//
CModeNotifier::CModeNotifier(
        CConnMonServer* aServer,
        RMobilePhone& aMobilePhone )
        :
        CActive( EConnMonPriorityNormal ),
        iServer( aServer ),
        iMobilePhone( aMobilePhone )
    {
    }

// -----------------------------------------------------------------------------
// CModeNotifier::Construct
// -----------------------------------------------------------------------------
//
void CModeNotifier::Construct()
    {
    //LOGENTRFN("CModeNotifier::Construct()")
    CActiveScheduler::Add( this );
    LOGIT("Created CModeNotifier")
    //LOGEXITFN("CModeNotifier::Construct()")
    }

// -----------------------------------------------------------------------------
// CModeNotifier::NewL
// -----------------------------------------------------------------------------
//
CModeNotifier* CModeNotifier::NewL(
        CConnMonServer* aServer,
        RMobilePhone& aMobilePhone )
    {
    CModeNotifier* self = new( ELeave ) CModeNotifier( aServer, aMobilePhone );
    self->Construct();
    return self;
    }

// Destructor
CModeNotifier::~CModeNotifier()
    {
    Cancel();
    iServer = NULL;
    }

// -----------------------------------------------------------------------------
// CModeNotifier::Receive
// Requests a new event (bearer changed) from ETel Custom API
// -----------------------------------------------------------------------------
//
void CModeNotifier::Receive()
    {
    if ( IsActive() )
        {
        return;
        }

    iMobilePhone.NotifyModeChange( iStatus, iMode );

    SetActive();
    }

// -----------------------------------------------------------------------------
// CModeNotifier::DoCancel
// Cancels the request from ETel Custom API.
// -----------------------------------------------------------------------------
//
void CModeNotifier::DoCancel()
    {
    if ( IsActive() )
        {
        iMobilePhone.CancelAsyncRequest( EMobilePhoneNotifyModeChange );
        }
    }

// -----------------------------------------------------------------------------
// CModeNotifier::RunL
// Handles the event that has arrived from ETel
// -----------------------------------------------------------------------------
//
void CModeNotifier::RunL()
    {
    //LOGENTRFN("CModeNotifier::RunL()")
    TBool edgeCellDetected = EFalse;

    // All RunL():s outside CServer-derived main class MUST NOT LEAVE.
    // Use TRAPD when needed.

    LOGIT(".")
    LOGIT1("RunL: CModeNotifier, status %d", iStatus.Int())

    if ( iStatus.Int() != KErrNone )
        {
        LOGIT1("SERVER: Mode changed event FAILED <%d>", iStatus.Int() )
        }
    else
        {
        LOGIT1("SERVER: CModeNotifier: mode <%d>", iMode )
        iEventInfo.Reset();

        iEventInfo.iEventType = EConnMonBearerChange;

        if ( iMode == RMobilePhone::ENetworkModeWcdma )
            {
            iEventInfo.iConnectionId = EBearerIdWCDMA;
            iEventInfo.iData = EBearerWCDMA;
            }
        else if ( iMode == RMobilePhone::ENetworkModeGsm )
            {
            iEventInfo.iConnectionId = EBearerIdGPRS;

            edgeCellDetected = iServer->Iap()->IsEdgeCell();
            if ( edgeCellDetected )
                {
                iEventInfo.iData = EBearerEdgeGPRS;
                }
            else
                {
                iEventInfo.iData = EBearerGPRS;
                }
            }
        else
            {
            // Unknown
            iEventInfo.iData = 0;
            }

        if ( iEventInfo.iData != 0 )
            {
            iServer->EventQueue()->Add( iEventInfo );

            LOGIT3("SERVER: EVENT -> Mode changed: %d, data %d, id %d",
                    iMode, iEventInfo.iData, iEventInfo.iConnectionId)
            }

        // New implemantation
        iEventInfo.Reset();

        iEventInfo.iEventType = EConnMonBearerInfoChange;
        iEventInfo.iConnectionId = EBearerIdAll; // means that this is not a connection related event

        if ( iMode == RMobilePhone::ENetworkModeWcdma )
            {
            iEventInfo.iData = (TUint)iServer->Iap()->HsxpaStatus();
            }
        else if ( iMode == RMobilePhone::ENetworkModeGsm )
            {
            if ( edgeCellDetected )
                {
                iEventInfo.iData = EBearerInfoEdgeGPRS;
                }
            else
                {
                iEventInfo.iData = EBearerInfoGPRS;
                }
            }
        else
            {
            // Unknown
            iEventInfo.iData = 0;
            }

        if ( iEventInfo.iData != 0 )
            {
            iServer->EventQueue()->Add( iEventInfo );
            LOGIT2("SERVER: EVENT -> Mode changed(new impl.): %d, bearerinfo %d", iMode, iEventInfo.iData)
            }

        TConnMonBearerInfo bI = ( TConnMonBearerInfo ) iEventInfo.iData;
        iServer->Iap()->SendConnectionEvents( bI ); // BearerInfoChanged, BearerGroupChanged
        }

    // New request
    Receive();
    //LOGEXITFN("CModeNotifier::RunL()")
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBearerAvailabilityNotifier::CBearerAvailabilityNotifier
// Notifies when GPRS/WCDMA bearer availability changes.
// Notifies also changes in GPRS/WCDMA IAP availability.
// -----------------------------------------------------------------------------
//
CBearerAvailabilityNotifier::CBearerAvailabilityNotifier(
        CConnMonServer* aServer )
        :
        CActive( EConnMonPriorityNormal ),
        iServer( aServer )
    {
    }

// -----------------------------------------------------------------------------
// CBearerAvailabilityNotifier::Construct
// -----------------------------------------------------------------------------
//
void CBearerAvailabilityNotifier::Construct()
    {
    //LOGENTRFN("CBearerAvailabilityNotifier::Construct()")
    CActiveScheduler::Add( this );
    iRegStatus = RPacketService::EUnknown;
    LOGIT("Created CBearerAvailabilityNotifier")
    //LOGEXITFN("CBearerAvailabilityNotifier::Construct()")
    }

// -----------------------------------------------------------------------------
// CBearerAvailabilityNotifier::NewL
// -----------------------------------------------------------------------------
//
CBearerAvailabilityNotifier* CBearerAvailabilityNotifier::NewL( CConnMonServer* aServer )
    {
    CBearerAvailabilityNotifier* self = new( ELeave ) CBearerAvailabilityNotifier( aServer );
    self->Construct();
    return self;
    }

// Destructor
CBearerAvailabilityNotifier::~CBearerAvailabilityNotifier()
    {
    Cancel();
    iServer = NULL;
    }

// -----------------------------------------------------------------------------
// CBearerAvailabilityNotifier::Receive
// Requests a new event (bearer (GPRS or WCDMA availability changed) from ETel
// -----------------------------------------------------------------------------
//
void CBearerAvailabilityNotifier::Receive( TBool aKickOff )
    {
    if ( IsActive() )
        {
        return;
        }

    if ( aKickOff )
        {
        // Get current status of iPSD
        TBool byCell( EFalse );

        // Is GPRS or WCDMA supported
        iServer->Iap()->GetBearerSupportInfo(
                EBearerIdGPRS,
                byCell,
                iGPRSSupportedByPhone );

        iServer->Iap()->GetBearerSupportInfo(
                EBearerIdWCDMA,
                byCell,
                iWCDMASupportedByPhone );

        if ( byCell && ( iGPRSSupportedByPhone || iWCDMASupportedByPhone ) )
            {
            iPSD = ETrue;
            }
        }

    // Start receiving notifications if the phone supports some bearer
    if ( iGPRSSupportedByPhone || iWCDMASupportedByPhone )
        {
        iServer->Iap()->PacketService()->NotifyChangeOfNtwkRegStatus(
                iStatus,
                iRegStatus );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CBearerAvailabilityNotifier::DoCancel
// Cancels the request from ETel.
// -----------------------------------------------------------------------------
//
void CBearerAvailabilityNotifier::DoCancel()
    {
    if ( IsActive() )
        {
        iServer->Iap()->PacketService()->CancelAsyncRequest( EPacketNotifyChangeOfNtwkRegStatus );
        }
    }

// -----------------------------------------------------------------------------
// CBearerAvailabilityNotifier::RunL
// Handles the event that has arrived from ETel
// -----------------------------------------------------------------------------
//
void CBearerAvailabilityNotifier::RunL()
    {
    //LOGENTRFN("CBearerAvailabilityNotifier::RunL()")

    // All RunL():s outside CServer-derived main class MUST NOT LEAVE.
    // Use TRAPD when needed.

    LOGIT(".")
    LOGIT1("RunL: CBearerAvailabilityNotifier, status %d", iStatus.Int())

    if ( iStatus.Int() != KErrNone )
        {
        LOGIT1("SERVER: Bearer availability event FAILED <%d>", iStatus.Int() )
        }
    else
        {
        iEventInfo.Reset();

        iEventInfo.iEventType = EConnMonBearerAvailabilityChange;

        if ( iGPRSSupportedByPhone || iWCDMASupportedByPhone )
            {
            if ( iRegStatus != RPacketService::ENotRegisteredNotSearching &&
                 iRegStatus != RPacketService::ENotRegisteredSearching    &&
                 iRegStatus != RPacketService::ERegistrationDenied        &&
                 iRegStatus != RPacketService::ENotRegisteredAndNotAvailable )
                {
                // Check the capabilities of the cell
                RPacketService::TDynamicCapsFlags capsDynamic;
                TInt err = iServer->Iap()->PacketService()->GetDynamicCaps( capsDynamic );

                if ( err != KErrNone )
                    {
                    capsDynamic = 0;
                    }

                if ( ( capsDynamic & RPacketService::KCapsManualAttach ) ||
                     ( capsDynamic & RPacketService::KCapsManualDetach ) )
                    {
                    // Query the TSY for the attach mode
                    RPacketService::TAttachMode attachMode( RPacketService::EAttachWhenNeeded );
                    err = iServer->Iap()->PacketService()->GetAttachMode( attachMode );

                    // Query packet network status (but only if the TSY is set to attach when possible)
                    if ( err == KErrNone && attachMode == RPacketService::EAttachWhenPossible )
                        {
                        RPacketService::TStatus status( RPacketService::EStatusUnattached );
                        err = iServer->Iap()->PacketService()->GetStatus( status );

                        if ( err == KErrNone && status != RPacketService::EStatusUnattached )
                            {
                            iEventInfo.iData = ETrue;
                            }
                        }
                    else
                        {
                        // Attach mode is 'WhenNeeded'
                        iEventInfo.iData = ETrue;
                        }
                    }
                }

            if ( static_cast< TUint >( iPSD ) != iEventInfo.iData )
                {
                // Send BEARER availability changed -event.
                // Find out the bearer ID (GPRS/WCDMA).
                iServer->Iap()->GetBearerId( iEventInfo.iConnectionId );

                // Send the event only if threshold is set by some of the clients.
                TUint threshold( 0 );

                iServer->CalculateThreshold(
                        EBearerIdAll,
                        EBearerAvailabilityThreshold,
                        threshold );

                // Don't send the BEARER availability -event if nobody wants it (threshold = 0)
                if ( threshold > 0 )
                    {
                    iServer->EventQueue()->Add( iEventInfo );
                    }

                LOGIT2("SERVER: EVENT -> Bearer availability changed: data %d, id %d",
                        iEventInfo.iData, iEventInfo.iConnectionId)

                iPSD = iEventInfo.iData;
                }
            }

        LOGIT("CBearerAvailabilityNotifier::RunL triggered HandleAvailabilityChange()")
        iServer->AvailabilityManager()->HandleAvailabilityChange();

        // This is due to off-line mode not triggering the event and user might
        // think that the packet data is available still..
        iServer->Iap()->DtmStateChanged();
        }
    // New request (with no initializations)
    Receive( EFalse );
    //LOGEXITFN("CBearerAvailabilityNotifier::RunL()")
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGsmSignalNotifier::CGsmSignalNotifier
// Notifies when GSM signal strength changes.
// -----------------------------------------------------------------------------
//
CGsmSignalNotifier::CGsmSignalNotifier(
        CConnMonServer* aServer,
        RMobilePhone& aMobilePhone )
        :
        CActive( EConnMonPriorityNormal ),
        iServer( aServer ),
        iMobilePhone( aMobilePhone )
    {
    }

// -----------------------------------------------------------------------------
// CGsmSignalNotifier::Construct
// -----------------------------------------------------------------------------
//
void CGsmSignalNotifier::Construct()
    {
    //LOGENTRFN("CGsmSignalNotifier::Construct()")
    CActiveScheduler::Add( this );
    LOGIT("Created CGsmSignalNotifier")
    //LOGEXITFN("CGsmSignalNotifier::Construct()")
    }

// -----------------------------------------------------------------------------
// CGsmSignalNotifier::NewL
// -----------------------------------------------------------------------------
//
CGsmSignalNotifier* CGsmSignalNotifier::NewL(
        CConnMonServer* aServer,
        RMobilePhone& aMobilePhone )
    {
    CGsmSignalNotifier* self = new( ELeave ) CGsmSignalNotifier( aServer, aMobilePhone );
    self->Construct();
    return self;
    }

// Destructor
CGsmSignalNotifier::~CGsmSignalNotifier()
    {
    Cancel();

    iServer = NULL;
    }

// -----------------------------------------------------------------------------
// CGsmSignalNotifier::Receive
// Requests a new event (signal strength changed) from ETel
// -----------------------------------------------------------------------------
//
void CGsmSignalNotifier::Receive()
    {
    if ( IsActive() )
        {
        return;
        }

    TUint threshold( 0 );

    // Threshold is owned by the session (not connection)
    iServer->CalculateThreshold( EBearerIdAll, ESignalStrengthThreshold, threshold );

    if ( threshold > 0 )
        {
        iMobilePhone.NotifySignalStrengthChange( iStatus, iSignalStrength, iBar );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CGsmSignalNotifier::DoCancel
// Cancels the request from ETel.
// -----------------------------------------------------------------------------
//
void CGsmSignalNotifier::DoCancel()
    {
    if ( IsActive() )
        {
        iMobilePhone.CancelAsyncRequest( EMobilePhoneNotifySignalStrengthChange );
        }
    }

// -----------------------------------------------------------------------------
// CGsmSignalNotifier::RunL
// Handles the event that has arrived from ETel
// -----------------------------------------------------------------------------
//
void CGsmSignalNotifier::RunL()
    {
    LOGENTRFN("CGsmSignalNotifier::RunL()")

    // All RunL():s outside CServer-derived main class MUST NOT LEAVE.
    // Use TRAPD when needed.

    if ( iStatus.Int() != KErrNone )
        {
        LOGIT1("SERVER: GSM signal strength event FAILED <%d>", iStatus.Int() )
        }
    else
        {
        // Is this a new registration status value
        if ( iEventInfo.iData != static_cast< TUint >( iSignalStrength ) )
            {
            iEventInfo.Reset();

            iEventInfo.iData = iSignalStrength;
            iEventInfo.iEventType = EConnMonSignalStrengthChange;

            // Find out bearer ID.
            TInt err = iServer->Iap()->GetBearerId( iEventInfo.iConnectionId );

            if ( err == KErrNone )
                {
                iServer->EventQueue()->Add( iEventInfo );
                }
            }

        LOGIT2("SERVER: EVENT -> Signal strength <%d>, id <%d>", iEventInfo.iData, iEventInfo.iConnectionId )

        // New request
        Receive();
        }
    LOGEXITFN("CGsmSignalNotifier::RunL()")
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CActivityNotifier::CActivityNotifier
// -----------------------------------------------------------------------------
//
CActivityNotifier::CActivityNotifier(
        CConnMonServer* aServer,
        RConnection* aConnection,
        const TUint& aConnectionId,
        const TUint& aSubConnectionId )
        :
        CActive( EConnMonPriorityNormal ),
        iServer( aServer ),
        iConnection( aConnection ),
        iConnectionId( aConnectionId),
        iSubConnectionId( aSubConnectionId ),
        iActivity( 0 ),
        iPckgActivity( iActivity )
    {
    }

// -----------------------------------------------------------------------------
// CActivityNotifier::Construct
// -----------------------------------------------------------------------------
//
void CActivityNotifier::Construct()
    {
    //LOGENTRFN("CActivityNotifier::Construct()")
    CActiveScheduler::Add( this );
    LOGIT("Created CActivityNotifier")
    //LOGEXITFN("CActivityNotifier::Construct()")
    }

// Destructor
CActivityNotifier::~CActivityNotifier()
    {
    Cancel();

    iServer = NULL;
    iConnection = NULL;
    }

// -----------------------------------------------------------------------------
// CActivityNotifier::Receive
// Requests a new event (activity changed) from RConnection
// -----------------------------------------------------------------------------
//
void CActivityNotifier::Receive( TBool aActivity )
    {
    if ( IsActive() )
        {
        return;
        }

    // Don't make a new request if previous
    // status indicates that connection is closing
    if ( iStatus == KErrDisconnected )
        {
        return;
        }

    iActivity = aActivity;
    iServer->CalculateThreshold( iConnectionId, EActivityTimeThreshold, iThreshold );

    if ( iThreshold > 0 )
        {
        // Will complete when the connection activity has changed from given
        // value (iActivity) for given period (threshold).
        iConnection->IsConnectionActiveRequest( iThreshold, iPckgActivity, iStatus );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CActivityNotifier::ReceiveOne
// Requests a new event (activity changed) from RConnection
// -----------------------------------------------------------------------------
//
void CActivityNotifier::ReceiveOne( TBool aActivity )
    {
    if ( IsActive() )
        {
        Cancel();
        }

    iActivity = aActivity;
    iThreshold = KOneTimeQueryThreshold; // 1 second

    // NOTE. Timer will expire once in a second.
    // This should be used only with one shot querries ( -> cancel after a period
    // if this has not completed otherwise ).
    iConnection->IsConnectionActiveRequest( iThreshold, iPckgActivity, iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CActivityNotifier::DoCancel
// Cancels the request from RConnection.
// -----------------------------------------------------------------------------
//
void CActivityNotifier::DoCancel()
    {
    if ( IsActive() )
        {
        // Complete any outstanding one time queries if the are any
        iServer->SendActivityToSessions( iConnectionId, iPckgActivity(), 0 );

        LOGIT2("Activity completed (DoCancel): %d, id %d", iPckgActivity(), iConnectionId)

        // Cancel the request from RConnection
        iConnection->IsConnectionActiveCancel();
        }
    }

// -----------------------------------------------------------------------------
// CActivityNotifier::RunL
// Handles the event that has arrived from RConnection
// -----------------------------------------------------------------------------
//
void CActivityNotifier::RunL()
    {
    //LOGENTRFN("CActivityNotifier::RunL()")

    // All RunL():s outside CServer-derived main class MUST NOT LEAVE.
    // Use TRAPD when needed.

    LOGIT(".")
    LOGIT1("RunL: CActivityNotifier, status %d", iStatus.Int())

    if ( iThreshold == KOneTimeQueryThreshold )
        {
        // One time query has been issued
        iServer->SendActivityToSessions( iConnectionId, iPckgActivity(), iStatus.Int() );

        LOGIT2("Activity req completed: %d, id %d", iPckgActivity(), iConnectionId)

        // Update internal table
        iActivity = iPckgActivity();
        iServer->Iap()->UpdateActivity( iConnectionId, iActivity );

        // Somebody has started notifications while we were waiting
        // one time call to return -> go on with notifications
        if ( ( iServer->NumberOfListeners() > 0 ) && ( iStatus.Int() == KErrNone ) )
            {
            // Send event to clients
            iEventInfo.Reset();

            iEventInfo.iConnectionId    = iConnectionId;
            iEventInfo.iSubConnectionId = iSubConnectionId;
            iEventInfo.iEventType       = EConnMonConnectionActivityChange;
            iEventInfo.iData            = iPckgActivity();

            iServer->EventQueue()->Add( iEventInfo );

            Receive( iActivity );
            }
        }
    else
        {
        // A real event has been requested
        if ( iStatus.Int() != KErrNone )
            {
            if ( iStatus.Int() == KErrCancel )
                {
                iStatus = KErrDisconnected;
                }

            LOGIT2("Activity event FAILED <%d>, id %d", iStatus.Int(), iConnectionId)
            }
        else
            {
            iEventInfo.Reset();

            // Send event(s) to clients
            iEventInfo.iConnectionId    = iConnectionId;
            iEventInfo.iSubConnectionId = iSubConnectionId;
            iEventInfo.iEventType       = EConnMonConnectionActivityChange;
            iEventInfo.iData            = iPckgActivity();

            LOGIT2("SERVER: EVENT -> Activity change: %d, id %d", iEventInfo.iData, iEventInfo.iConnectionId)

            iServer->EventQueue()->Add( iEventInfo );

            // Update internal table
            iActivity = iPckgActivity();
            iServer->Iap()->UpdateActivity( iConnectionId, iActivity );

            // New request
            Receive( iActivity );
            }
        }
    //LOGEXITFN("CActivityNotifier::RunL()")
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSubConnUpDownNotifier::CSubConnUpDownNotifier
// -----------------------------------------------------------------------------
//
CSubConnUpDownNotifier::CSubConnUpDownNotifier(
        CConnMonServer* aServer,
        RConnection* aConnection,
        const TUint& aConnectionId,
        const TUint& aSubConnectionId )
        :
        CActive( EConnMonPriorityMedium ),
        iServer( aServer ),
        iConnection( aConnection ),
        iConnectionId( aConnectionId),
        iSubConnectionId( aSubConnectionId )
    {
    }

// -----------------------------------------------------------------------------
// CSubConnUpDownNotifier::Construct
// -----------------------------------------------------------------------------
//
void CSubConnUpDownNotifier::Construct()
    {
    //LOGENTRFN("CSubConnUpDownNotifier::Construct()")
    CActiveScheduler::Add( this );
    LOGIT("Created CSubConnUpDownNotifier")
    //LOGEXITFN("CSubConnUpDownNotifier::Construct()")
    }

// Destructor
CSubConnUpDownNotifier::~CSubConnUpDownNotifier()
    {
    if ( IsActive() )
        {
        Cancel();
        }

    iServer = NULL;
    iConnection = NULL;
    }

// -----------------------------------------------------------------------------
// CSubConnUpDownNotifier::Receive
// Requests a new event (subconnection up/down) from RConnection
// -----------------------------------------------------------------------------
//
void CSubConnUpDownNotifier::Receive()
    {
    if ( IsActive() )
        {
        Cancel();
        }

    if ( iStatus == KErrDisconnected )
        {
        return;
        }

    iConnection->AllSubConnectionNotification( iSubConnEventBuf, iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CSubConnUpDownNotifier::SendDeletedEvent
// Sends connection deleted event to the clients
// -----------------------------------------------------------------------------
//
void CSubConnUpDownNotifier::SendDeletedEvent()
    {
    if ( !iDeleteSent )
        {
        iEventInfo.Reset();

        iEventInfo.iEventType       = EConnMonDeleteConnection;
        iEventInfo.iConnectionId    = iConnectionId;
        iEventInfo.iSubConnectionId = iSubConnectionId;
        iEventInfo.iData            = iTotalDownlinkDataVolume;
        iEventInfo.iData2           = iTotalUplinkDataVolume;

        // Find out type of the delete
        CProgressNotifier* progressNotifier = 0;
        TInt err = iServer->Iap()->GetProgressNotifier( iConnectionId, &progressNotifier );

        if ( err == KErrNone )
            {
            iEventInfo.iData3 = progressNotifier->IsAuthDelete();
            }

        // Send event to all clients that are listening
        iServer->EventQueue()->Add( iEventInfo );

        iDeleteSent = ETrue;

        if ( iInterfaceClosed )
            {
            // Remove connection from the connection table if interface
            // has closed.
            TBearerInfo bearerInfo;
            TConnInfo connInfo( 0, 0, iConnectionId, 0, bearerInfo );
            iServer->Iap()->RemoveConnection( connInfo );
            }
        else
            {
            if ( progressNotifier->CanReconnect() )
                {
                // Go on listening subinterface events
                iConnection->AllSubConnectionNotification( iSubConnEventBuf, iStatus );
                SetActive();
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CSubConnUpDownNotifier::DeleteSent
// -----------------------------------------------------------------------------
//
TBool CSubConnUpDownNotifier::DeleteSent() const
    {
    return iDeleteSent;
    }

// -----------------------------------------------------------------------------
// CSubConnUpDownNotifier::SetInterfaceClosed
// -----------------------------------------------------------------------------
//
void CSubConnUpDownNotifier::SetInterfaceClosed()
    {
    iInterfaceClosed = ETrue;
    }


// -----------------------------------------------------------------------------
// CSubConnUpDownNotifier::DoCancel
// Cancels the request from RConnection.
// -----------------------------------------------------------------------------
//
void CSubConnUpDownNotifier::DoCancel()
    {
    if ( IsActive() )
        {
        iConnection->CancelAllSubConnectionNotification();
        }
    }

// -----------------------------------------------------------------------------
// CSubConnUpDownNotifier::RunL
// Handles the event that has arrived from RConnection
// -----------------------------------------------------------------------------
//
void CSubConnUpDownNotifier::RunL()
    {
    //LOGENTRFN("CSubConnUpDownNotifier::RunL()")

    // All RunL():s outside CServer-derived main class MUST NOT LEAVE.
    // Use TRAPD when needed.

    LOGIT(".")
    LOGIT1("RunL: CSubConnUpDownNotifier, status %d", iStatus.Int())

    if ( iStatus.Int() != KErrNone )
        {
        LOGIT2("SERVER: subconn up/down event for id %d FAILED <%d>", iConnectionId, iStatus.Int())

        if ( iStatus.Int() == KErrNotReady )
            {
            // Subinterface has disappered
            CProgressNotifier* progressNotifier = 0;
            TInt err = iServer->Iap()->GetProgressNotifier( iConnectionId, &progressNotifier );
            if ( err == KErrNone )
                {
                // Is progress notifier still alive
                if ( !progressNotifier->IsActive() )
                    {
                    SendDeletedEvent();
                    }
                }
            }

        iStatus = KErrDied;
        }
    else
        {
        TSubConnectionEvent& event = ( TSubConnectionEvent& )( *iSubConnEventBuf.Ptr() );

        if ( event.iSubConnectionUniqueId == 0 && event.iEventType == ESubConnectionClosed )
            {
            TSubConnectionClosedEvent& closedEvent =
                    (TSubConnectionClosedEvent&)( *iSubConnEventBuf.Ptr() );
            iTotalDownlinkDataVolume = closedEvent.iTotalDownlinkDataVolume;
            iTotalUplinkDataVolume   = closedEvent.iTotalUplinkDataVolume;

            LOGIT3("SERVER: EVENT -> Connection %d closed, u: %d, d: %d",
                    iConnectionId, iTotalUplinkDataVolume, iTotalDownlinkDataVolume)

            CProgressNotifier* progressNotifier = 0;
            TInt err = iServer->Iap()->GetProgressNotifier( iConnectionId, &progressNotifier );
            if ( err == KErrNone )
                {
                // Progess notifier has stopped and allinterface closed event has arrived
                if ( !progressNotifier->IsActive() )
                    {
                    SendDeletedEvent();
                    }
                }
            else
                {
                SendDeletedEvent();
                }

            iStatus = KErrDisconnected;
            }
        else if ( ( event.iSubConnectionUniqueId == 0 ) &&
                  ( event.iEventType == ESubConnectionOpened ) )
            {
            // IS THIS A RECONNECT
            if ( iDeleteSent && !iInterfaceClosed )
                {
                TUint newId( iConnectionId );
                TInt err = iServer->Iap()->Reconnect( newId );

                if ( err == KErrNone )
                    {
                    iEventInfo.Reset();
                    iEventInfo.iEventType    = EConnMonCreateConnection;
                    iEventInfo.iConnectionId = newId;

                    // Send event to all clients that are listening
                    iServer->EventQueue()->Add( iEventInfo );

                    LOGIT1("SERVER: EVENT -> new connection %d created (RECONNECTED)", newId)
                    delete this;
                    }
                }
             else
                {
                // New request
                Receive();
                }
            }
        else
            {
            // New request
            iConnection->AllSubConnectionNotification( iSubConnEventBuf, iStatus );
            SetActive();
            }
        }
    //LOGEXITFN("CSubConnUpDownNotifier::RunL()")
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTimerAO::CTimerAO
// -----------------------------------------------------------------------------
//
CTimerAO::CTimerAO(
        CConnMonServer* aServer,
        const TUint& aConnectionId,
        const TUint8& aActivity )
        :
        CActive( EConnMonPriorityNormal ),
        iServer( aServer ),
        iConnectionId( aConnectionId ),
        iActivity( aActivity )
    {
    }

// -----------------------------------------------------------------------------
// CTimerAO::ConstructL
// -----------------------------------------------------------------------------
//
void CTimerAO::ConstructL()
    {
    //LOGENTRFN("CTimerAO::ConstructL()")
    CActiveScheduler::Add( this );
    User::LeaveIfError( iTimer.CreateLocal() );
    //LOGEXITFN("CTimerAO::ConstructL()")
    }

// Destructor
CTimerAO::~CTimerAO()
    {
    Cancel();
    iTimer.Close();

    iServer = NULL;
    }

// -----------------------------------------------------------------------------
// CTimerAO::Start
// -----------------------------------------------------------------------------
//
void CTimerAO::Start()
    {
    if ( !IsActive() )
        {
        iTimer.After( iStatus, KActivityTimeout );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CTimerAO::DoCancel
// -----------------------------------------------------------------------------
//
void CTimerAO::DoCancel()
    {
    if ( IsActive() )
        {
        iTimer.Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CTimerAO::RunL
// -----------------------------------------------------------------------------
//
void CTimerAO::RunL()
    {
    CActivityNotifier* activityNotifier = 0;

    TInt err = iServer->Iap()->GetActivityNotifier( iConnectionId, &activityNotifier );

    if ( ( err == KErrNone ) && ( activityNotifier != 0 ) )
        {
        if ( activityNotifier->IsActive() )
            {
            TUint threshold( activityNotifier->Threshold() );

            if ( threshold == KOneTimeQueryThreshold )
                {
                // Timer has completed before one time activity query.
                // Cancel it to complete client requests
                activityNotifier->Cancel();

                // Someboby has started notifications while we were waiting this to
                // complete -> go on with the notifications
                if ( iServer->NumberOfListeners() > 0 )
                    {
                    activityNotifier->Receive( iActivity );
                    }
                }
            }
        }

    delete this;
    }

// End-of-file
