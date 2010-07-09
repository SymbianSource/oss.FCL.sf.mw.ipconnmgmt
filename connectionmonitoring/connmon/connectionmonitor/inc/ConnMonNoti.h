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

#ifndef __CONNMONNOTI_H
#define __CONNMONNOTI_H

#include <es_sock.h>
#include <in_sock.h>
#include <es_enum.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_enum_internal.h>
#endif

class CConnMonIAP;
class CConnMonServer;

const TInt KNoFiltering = -1;
const TInt KActivityTimeout = 1800000; // 1,8 sec
// Update ConnMon documentation (attribute KConnectionActive) if changing this
// value (KOneTimeQueryThreshold = 1)
const TUint KOneTimeQueryThreshold = 1;
const TUint KInvalidIapCount = 99999;

// Stop requesting new notifications after this many consecutive errors (CBearerNotifier)
const TInt KConnMonEventErrorThreshold = 20;

/**
*  CConnUpDownNotifier
*  @lib CONNMON.LIB
*  @since
*/
NONSHARABLE_CLASS( CConnUpDownNotifier ) : public CActive
    {
    public: // Constructors and destructor
        CConnUpDownNotifier( CConnMonServer* aServer );
        void Construct();
        virtual ~CConnUpDownNotifier();

    public:  // New methods
        /**
        * Requests a new event (connection up/down) from RConnection.
        * @since
        * @return void
        */
        void Receive();

    private: // Methods from base classes
         /**
        * From CActive Cancels the asyncronous request
        * @since
        * @return void
        */
        void DoCancel();

         /**
        * From CActive Handles the the event that has arrived from RConnection.
        * @since
        * @return void
        */
        void RunL();

    private: // Data
        CConnMonServer*           iServer;
        TEventInfo                iEventInfo;
        TInterfaceNotificationBuf iInfoBuf;
        TInt                      iErrorCounter;
    };

/**
*  CDataNotifier
*  @lib CONNMON.LIB
*  @since
*/
NONSHARABLE_CLASS( CDataNotifier ) : public CActive
    {
    public: // Constructors and destructor
        enum TDataDirection
             {
             EDownlink = 0,
             EUplink
             };

        CDataNotifier(
                CConnMonServer* aServer,
                RConnection* aConnection,
                const TUint& aConnectionId,
                const TUint& aSubConnectionId,
                const TInt& aDirection );
        void Construct();
        virtual ~CDataNotifier();


    public: // New methods
        /**
        * Requests a new event (data transferred) from RConnection.
        * @since
        * @return void
        */
        void Receive();

    private: // Methods from base classes
         /**
        * From CActive Cancels the asyncronous request
        * @since
        * @return void
        */
        void DoCancel();

        /**
        * From CActive Handles the the event that has arrived from RConnection.
        * @since
        * @return void
        */
        void RunL();

    private: // Data
        CConnMonServer* iServer;
        RConnection*    iConnection;
        const TUint     iConnectionId;
        const TUint     iSubConnectionId;
        const TInt      iDirection;
        TUint           iVolume;
        TPckg<TUint>    iPckgVolume;
        TUint           iThreshold;
        TEventInfo      iEventInfo;
        TInt            iErrorCounter;
    };

/**
*  CProgressNotifier
*  @lib CONNMON.LIB
*  @since
*/
NONSHARABLE_CLASS( CProgressNotifier ) : public CActive
    {
    public: // Constructors and destructor
        CProgressNotifier(
                CConnMonServer* aServer,
                RConnection* aConnection,
                const TUint& aConnectionId,
                const TUint& aSubConnectionId );
        void Construct();
        virtual ~CProgressNotifier();

    public: // New methods
        /**
        * Requests a new event (connection status has changed) from RConnection.
        * @since
        * @return void
        */
        void Receive();

        /**
        * Requests a new event (connection status has changed) from RConnection.
        * @since
        * @param aFilter Notifications will be dropped until status reaches aFilter.
        * @return void
        */
        void Receive( const TInt aFilter );

        /**
        * Return ETrue if the progress value returned from RConnection indicates that
        * the user has deactivated the connection by using Connection Monitor or
        * some application has issued RConnection::Stop( RConnection::EStopAuthoritative ).
        * @since
        * @return ETrue if connection has been deactivated in an
        *         authoritative way, otherwise returns EFalse.
        */
        TBool IsAuthDelete();

        /**
        * Return ETrue if the connection might reconnect (e.g. CSD after network is down for a while)
        * @since
        * @return ETrue if the connection might reconnect, otherwise returns EFalse.
        */
        TBool CanReconnect() const;

        /**
        * Send KLinkLayerClosed event to clients
        * @since
        */
        void SendClosedEvent();

    private: // Methods from base classes
         /**
        * From CActive Cancels the asyncronous request
        * @since
        * @return void
        */
        void DoCancel();

        /**
        * From CActive Handles the the event that has arrived from RConnection.
        * @since
        * @return void
        */
        void RunL();

    private: // Data
        CConnMonServer* iServer;
        RConnection*    iConnection;
        const TUint     iConnectionId;
        const TUint     iSubConnectionId;
        TEventInfo      iEventInfo;
        TNifProgressBuf iInfoBuf;
        TInt            iFilter;
    };

/**
*  CNetwStatusNotifier
*  @lib CONNMON.LIB
*  @since
*/
NONSHARABLE_CLASS( CNetwStatusNotifier ) : public CActive
    {
    public: // Constructor and destructor
        static CNetwStatusNotifier* NewL( CConnMonServer* aServer );
        virtual ~CNetwStatusNotifier();

    public: // New methods
        /**
        * Requests a new event (network status changed) from ETel.
        * @since
        * @return void
        */
        void Receive();

    private: // Methods from base classes
         /**
        * From CActive Cancels the asyncronous request
        * @since
        * @return void
        */
        void DoCancel();

        /**
        * From CActive Handles the the event that has arrived from ETel.
        * @since
        * @return void
        */
        void RunL();

    private: // Private constructors
        CNetwStatusNotifier( CConnMonServer* aServer );
        void Construct();

    private: // Data
        CConnMonServer*         iServer;
        RPacketService::TStatus iPacketStatus;
        TEventInfo              iEventInfo;
    };

/**
*  CNetwRegistrationNotifier
*  @lib CONNMON.LIB
*  @since
*/
NONSHARABLE_CLASS( CNetwRegistrationNotifier ) : public CActive
    {
    public: // Constructor and destructor
        static CNetwRegistrationNotifier* NewL(
                CConnMonServer* aServer,
                RMobilePhone& aMobilePhone );
        virtual ~CNetwRegistrationNotifier();

    public: // New methods
        /**
        * Requests a new event (network registration changed) from ETel.
        * @since
        * @return void
        */
        void Receive();

    private: // Methods from base classes
         /**
        * From CActive Cancels the asyncronous request
        * @since
        * @return void
        */
        void DoCancel();

        /**
        * From CActive Handles the the event that has arrived from ETel.
        * @since
        * @return void
        */
        void RunL();

    private: // Private constructors
        CNetwRegistrationNotifier(
                CConnMonServer* aServer,
                RMobilePhone& aMobilePhone );
        void Construct();


    private: // Data
        CConnMonServer*                              iServer;
        RMobilePhone&                                iMobilePhone;
        RMobilePhone::TMobilePhoneRegistrationStatus iRegistration;
        TBool                                        iCSDSupported;
        TBool                                        iPhoneSupportsCSD;
        TEventInfo                                   iEventInfo;
    };

/**
* CBearerNotifier
* @lib CONNMON.LIB
*/
NONSHARABLE_CLASS( CBearerNotifier ) : public CActive
    {
    public: // Constructors and destructor
        CBearerNotifier(
                CConnMonServer* aServer,
                RMobilePhone& aMobilePhone );
        void ConstructL();

        static CBearerNotifier* NewL(
                CConnMonServer* aServer,
                RMobilePhone& aMobilePhone );
        virtual ~CBearerNotifier();

    public: // New methods
        /**
        * Requests a new event (bearer changed) from ETel Custom API.
        * @return void
        */
        void Receive();

    private: // Methods from base classes
        /**
        * From CActive, cancels the asyncronous request.
        * @return void
        */
        void DoCancel();

        /**
        * From CActive Handles the the event that has arrived from ETel Custom API.
        * @return void
        */
        void RunL();

    private: // Data
        CConnMonServer* iServer;
        RMobilePhone& iMobilePhone;
        RMmCustomAPI::TGprsInformation iGprsInfo;
        RMmCustomAPI::TGprsInformationPckg iGprsInfoPckg;
        RMmCustomAPI iCustomApi;
        TEventInfo iEventInfo;
        TInt iErrorCounter;
    };

/**
*  CWcdmaBearerNotifier
*  @lib CONNMON.LIB
*  @since
*/
NONSHARABLE_CLASS( CWcdmaBearerNotifier ) : public CActive
    {
    public: // Constructors and destructor
        CWcdmaBearerNotifier(
                CConnMonServer* aServer,
                RTelServer& aTelServer );
        void ConstructL();

        static CWcdmaBearerNotifier* NewL(
                CConnMonServer* aServer,
                RTelServer& aTelServer );
        virtual ~CWcdmaBearerNotifier();

    public: // New methods
        /**
        * Requests a new event (bearer changed) from ETel Custom API.
        * @since
        * @return void
        */
        void Receive();

    private: // Methods from base classes
        /**
        * From CActive Cancels the asyncronous request
        * @since
        * @return void
        */
        void DoCancel();

        /**
        * From CActive Handles the the event that has arrived from ETel Custom API.
        * @since
        * @return void
        */
        void RunL();

    private: // Data
        CConnMonServer*                             iServer;
        RTelServer&                                 iTelServer;
        RMobilePhone                                iMobilePhone;
        RMobilePhone::TMobilePhoneNetworkInfoV5     iNetInfo;
        RMobilePhone::TMobilePhoneNetworkInfoV5Pckg iNetInfoPckg;
        TConnMonBearerInfo                          iPreviousBearerInfo;
        TEventInfo                                  iEventInfo;
    };


/**
*  CModeNotifier
*  @lib CONNMON.LIB
*  @since
*/
NONSHARABLE_CLASS( CModeNotifier ) : public CActive
    {
    public: // Constructor and destructor
        static CModeNotifier* NewL(
                CConnMonServer* aServer,
                RMobilePhone& aMobilePhone );
        virtual ~CModeNotifier();

    public: // New methods
        /**
        * Requests a new event (mode changed) from ETel.
        * @since
        * @return void
        */
        void Receive();

    private: // Methods from base classes
         /**
        * From CActive Cancels the asyncronous request
        * @since
        * @return void
        */
        void DoCancel();

        /**
        * From CActive Handles the the event that has arrived from ETel.
        * @since
        * @return void
        */
        void RunL();

    private: // Private constructors
        CModeNotifier(
                CConnMonServer* aServer,
                RMobilePhone& aMobilePhone );
        void Construct();

    private: // Data
        CConnMonServer*                       iServer;
        RMobilePhone&                         iMobilePhone;
        RMobilePhone::TMobilePhoneNetworkMode iMode;
        TEventInfo                            iEventInfo;
    };

/**
*  CBearerAvailabilityNotifier
*  @lib CONNMON.LIB
*  @since
*/
NONSHARABLE_CLASS( CBearerAvailabilityNotifier ) : public CActive
    {
    public: // Constructors and destructor
        static CBearerAvailabilityNotifier* NewL( CConnMonServer* aServer );
        virtual ~CBearerAvailabilityNotifier();

    public: // New methods
        /**
        * Requests a new event (bearer (PSD) availability changed) from ETel.
        * @since
        * @param aKickOff If aKickOff is ETrue initial bearer status will be obtained from ETel.
        * @return void
        */
        void Receive( TBool aKickOff=ETrue );

    private: // Methods from base classes
         /**
        * From CActive Cancels the asyncronous request
        * @since
        * @return void
        */
        void DoCancel();

        /**
        * From CActive Handles the the event that has arrived from ETel.
        * @since
        * @return void
        */
        void RunL();

    private: // Private constructors
        CBearerAvailabilityNotifier( CConnMonServer* aServer );
        void Construct();


    private: // Data
        CConnMonServer*                     iServer;
        TBool                               iPSD;
        TBool                               iGPRSSupportedByPhone;
        TBool                               iWCDMASupportedByPhone;
        RPacketService::TRegistrationStatus iRegStatus;
        TEventInfo                          iEventInfo;
    };

/**
*  CGsmSignalNotifier
*  @lib CONNMON.LIB
*  @since
*/
NONSHARABLE_CLASS( CGsmSignalNotifier ) : public CActive
    {
    public: // Constructor and destructor
        static CGsmSignalNotifier* NewL(
                CConnMonServer* aServer,
                RMobilePhone& aMobilePhone );
        virtual ~CGsmSignalNotifier();

    public: // New methods
        /**
        * Requests a new event (GSM signal strength changed) from ETel.
        * @since
        * @return void
        */
        void Receive();

    private: // Methods from base classes
         /**
        * From CActive Cancels the asyncronous request
        * @since
        * @return void
        */
        void DoCancel();

        /**
        * From CActive Handles the the event that has arrived from ETel.
        * @since
        * @return void
        */
        void RunL();

    private: // Private constructors
        CGsmSignalNotifier(
                CConnMonServer* aServer,
                RMobilePhone& aMobilePhone );
        void Construct();

    private: // Data
        CConnMonServer* iServer;
        RMobilePhone&   iMobilePhone;
        TInt32          iSignalStrength;
        TInt8           iBar;
        TEventInfo      iEventInfo;
    };

/**
*  CActivityNotifier
*  @lib CONNMON.LIB
*  @since
*/
NONSHARABLE_CLASS( CActivityNotifier ) : public CActive
    {
    public: // Constructors and destructor
        CActivityNotifier(
                CConnMonServer* aServer,
                RConnection* aConnection,
                const TUint& aConnectionId,
                const TUint& aSubConnectionId );
        void Construct();
        virtual ~CActivityNotifier();


    public: // New methods
        /**
        * Requests a new event (activity change) from RConnection.
        * Will use the smallest threshold set by the clients as the
        * threshold.
        * @since
        * @param aActivity The current assumption of activity.
        * @return void
        */
        void Receive( TBool aActivity );

        /**
        * Requests a new event (activity change) from RConnection.
        * Threshold is one second.
        * @since
        * @param aActivity The current assumption of activity.
        * @return void
        */
        void ReceiveOne( TBool aActivity );

        inline TUint Threshold() const { return iThreshold; }

    private: // Methods from base classes
         /**
        * From CActive Cancels the asyncronous request
        * @since
        * @return void
        */
        void DoCancel();

        /**
        * From CActive Handles the the event that has arrived from RConnection.
        * @since
        * @return void
        */
        void RunL();

    private: // Data
        CConnMonServer* iServer;
        RConnection*    iConnection;
        const TUint     iConnectionId;
        const TUint     iSubConnectionId;
        TBool           iActivity;
        TPckg<TBool>    iPckgActivity;
        TUint           iThreshold;
        TEventInfo      iEventInfo;
    };

/**
*  CSubConnUpDownNotifier
*  @lib CONNMON.LIB
*  @since
*/
NONSHARABLE_CLASS( CSubConnUpDownNotifier ) : public CActive
    {
    public: // Constructors and destructor
        CSubConnUpDownNotifier(
                CConnMonServer* aServer,
                RConnection* aConnection,
                const TUint& aConnectionId,
                const TUint& aSubConnectionId );
        void Construct();
        virtual ~CSubConnUpDownNotifier();

    public: // New methods
        /**
        * Requests a new event (connection status has changed) from RConnection.
        * @since
        * @return void
        */
        void Receive();

        /**
        * Sends connection deleted event to clients and removes the connection
        * from internal tables.
        * @since
        * @return void
        */
        void SendDeletedEvent();

        /**
        * Tells whether the deleted event has been sent.
        * @since
        * @return ETrue if delete has been sent, EFalse otherwise.
        */
        TBool DeleteSent() const;

        /**
        * Sets interface status to closed.
        * @since
        * @return void.
        */
        void SetInterfaceClosed();
        
        /**
        * Tells whether interface has been closed or not.
        * @since
        * @return ETrue if interface has been closed, otherwise EFalse.
        */
        TBool InterfaceClosed();

        /**
        * Sets link layer status to closed.
        * @since
        * @return void.
        */
        void SetLinkLayerClosed();
        
        /**
        * Sets link layer status to NOT closed.
        * @since
        * @return void.
        */
        void SetLinkLayerOpen();
        
        /**
        * Return ETrue if KLinkLayerClosed has been received.
        * @since
        * @return ETrue if KLinkLayerClosed has been received, otherwise returns EFalse.
        */
        TBool LinkLayerClosed();


    private: // Methods from base classes
         /**
        * From CActive Cancels the asyncronous request
        * @since
        * @return void
        */
        void DoCancel();

        /**
        * From CActive Handles the the event that has arrived from RConnection.
        * @since
        * @return void
        */
        void RunL();

    private: // Data
        CConnMonServer*               iServer;
        RConnection*                  iConnection;
        const TUint                   iConnectionId;
        const TUint                   iSubConnectionId;
        TEventInfo                    iEventInfo;
        TSubConnectionNotificationBuf iSubConnEventBuf;
        TUint                         iTotalDownlinkDataVolume;
        TUint                         iTotalUplinkDataVolume;
        TBool                         iDeleteSent;
        TBool                         iInterfaceClosed;
        TBool                         iLinkLayerClosed;
    };

/**
*  CTimerAO
*  Timer that cancel activity notifier after one second.
*  @lib CONNMON.LIB
*  @since
*/
NONSHARABLE_CLASS( CTimerAO ) : public CActive
    {
    public: // Constructors and destructor
        CTimerAO(
                CConnMonServer* aServer,
                const TUint& aConnectionId,
                const TUint8& aActivity );
        void ConstructL();
        virtual ~CTimerAO();

    public: // New methods
        /**
        * Starts canceller timer.
        * @since
        * @return KErrNone if OK, otherwise a system wide error code.
        */
        void Start();

    private: // Methods from base classes
        /**
        * From CActive Cancels the timer.
        * @since
        * @return void
        */
        void DoCancel();

        /**
        * From CActive Cancels the activity notifier and destroys itself.
        * @since
        * @return void
        */
        void RunL();

    private: // Data
        RTimer          iTimer;
        CConnMonServer* iServer;
        const TUint     iConnectionId;
        TUint8          iActivity;
    };

#endif // __CONNMONNOTI_H

// End-of-file
