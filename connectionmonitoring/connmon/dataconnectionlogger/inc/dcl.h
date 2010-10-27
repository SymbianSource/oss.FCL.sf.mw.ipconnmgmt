/*
* Copyright (c) 2003-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Data Connection Logger.
*
*/

#ifndef DCLENGINE_H
#define DCLENGINE_H

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logwraplimits.h>
#endif
#include <e32std.h>
#include <es_sock.h>
#include <e32base.h>
#include <commdb.h>
#include <commsdat.h>
#include <wlancontainer.h>
#include <rconnmon.h>
#include <mmtsy_names.h>
#include <etel.h>
#include <etelmm.h>
#include <etelpckt.h>
#include <pcktcs.h>

#ifdef __WLAN
#include <wlanloguid.h>
#endif // __WLAN

#include "dclevent.h"
#include "saeobserver.h"

// CONSTANTS
// UID of DataConnectionLogger
const TLogId    KGenconnLogWaitingForLogId      = KLogNullId -1;
const TInt      KBytesInfoNotAvailable          = -1;
const TUint     KGPRSDataThreshold              = 20480;
const TUint     KWCDMADataThreshold             = 51200;
const TUint     KWLANDataThreshold              = 102400;
const TUint     KSubConnectionId                = 0;
const TInt      KMaxAPName                      = KCommsDbSvrMaxFieldLength;
const TInt      KPhoneRetryCount                = 7; // Must be at least 1
const TInt      KPhoneRetryTimeout              = 100000;
const TInt      KConnectionStatusIdNotAvailable = -1;
// The granularity with which iLogEventQueue and iConnections will
// allocate memory chunks. If set to 20 there will be space for 20
// instances of CEvent before new memory will be allocated.
const TInt      KEventStateMaxCount             = 20;
const TInt      KDatabufferSize                 = 64;

// Timer mode constants
const TInt      KTimerInterval                  = 10;

_LIT( KDCLName, "DataConnectionLogger" );

#ifndef __WLAN
#define KLOGWLANDATAEVENTTYPE 0x1000595f
const TUid KLogWlanDataEventTypeUid = {KLOGWLANDATAEVENTTYPE};
#endif // __WLAN

//  DATA TYPES
enum TDCLPanic
    {
    ECannotCreateCleanup            = 600,
    ECannotCreateActiveScheduler,
    ECannotCreateCEngine,
    ECannotRequestNotification
    };

// FORWARD DECLARATIONS
class CDclTimerAO;

// CLASS DECLARATION

/**
*  CEngine
*  Gets information from connection monitor server using RCcnnectionMonitor
*  object. Implements MConnectionMonitorObserver interface.
*
*/
class CEngine : public CActive,
                private MConnectionMonitorObserver
    {
    public:
        // Constructors
        static CEngine* NewL( );
        static CEngine* NewLC( );
        // Destructor
        virtual ~CEngine();

    public: // New methods
        /**
        * Adds log events.
        */
        void LogDataAddEventL(
                TLogId        aConnectionId,
                TInt          aRConnectionStatusId,
                const TDesC&  aRemote,
                TInt          aLogDir,
                const TDesC&  aTelNum,
                const TUid&   aDataEventType );

        /**
        * Changes the remote party string for existing log events.
        */
        void LogDataChangeEventL(
                TLogId        aConnectionId,
                const TDesC&  aRemote );

        /**
        * Updates log events.
        */
        TInt LogDataUpdateEventL(
                TLogId        aConnectionId,
                TInt          aRConnectionStatusId,
                const TUid&   aDataEventType,
                const TInt64& aBytesSent,
                const TInt64& aBytesReceived );

        /**
        * Starts notifications.
        */
        void NotifyL() ;

        /**
        * Updates log event parameters.
        */
        TInt UpdateLogEventParamL(
                TLogId        aConnectionId,
                CLogEvent&    aLogEvent,
                TInt          aRConnectionStatusId,
                const TUid&   aDataEventType,
                const TInt64& aBytesSent,
                const TInt64& aBytesReceived );

        /**
        * Updates cumulative counters of SharedData
        */
        void UpdateSharedDataL(
                TLogId        aConnectionId,
                const TInt64& aBytesSent,
                const TInt64& aBytesReceived );

        /**
        * Gets the Attach Mode setting from CommDb and
        * sets ETel to use that value.
        */
        void SetAttachModeL() const;

        /**
        * Gets the default Access Point Name from CommDb and
        * sets ETel to use that APN.
        */
        void SetAPNL() const;

        // Following methods are from CActive.
        /**
        * Handles a cancel request to the service provider.
        * This function is defined as pure virtual in CActive and any
        * class derived from CActive must define and implement it.
        */
        void DoCancel();

        /**
        * Handles the completion of an asynchronous request.
        * This function is defined as pure virtual in CActive and any
        * class derived from CActive must define and implement it.
        */
        void RunL();

        /**
        * Writes to log databases in timer mode.
        */
        TInt LogDataInTimerModeL( const TUint aConnectionId );

        /**
        * Switches logging to the threshold mode.
        */
        void SwitchToThresholdMode( const TUint aConnectionId, const TUint aThreshold = 0 );

    private : // From MConnectionMonitorObserver
        void EventL( const CConnMonEventBase& aConnMonEvent ) ;

    private: // New methods
        CEngine();
        void ConstructL();
        void AddEventTypeL();

        /**
        * Copies aName to aResult as such. Both descriptors are 16-bit.
        */
        void ReplaceAPN( TDes& aResult, const TDesC& aName ) const ;

        /**
        * Converts aName to UTF-8 and copies it to aResult which is 8-bit descriptor.
        */
        void ReplaceAPN( TDes8& aResult, const TDesC& aName ) const;

        /**
        * Switches logging to the timer mode.
        */
        void SwitchToTimerMode( const TUint aConnectionId );

        /**
        * Gets duration of a connection.
        */
        TBool GetDuration( const TUint aConnectionId, TInt& aDuration );

        /**
        * Gets the default threshold for a connection.
        */
        TInt GetDefaultThreshold( const TUint aConnectionId, TUint& aThreshold );

        /**
        * Reads ini file settings.
        */
        void ReadIniFile();

    private: // Data
        RConnectionMonitor          iConnectionMonitor;
        CLogWrapper*                iLogWrap;
        CArrayPtrFlat<CEvent>*      iLogEventQueue;
        CArrayPtrFlat<CEvent>*      iConnections;
        CLogEvent*                  iCurrentLogEvent;
        RFs                         iFsEventLog;
        RPacketContext              iContext;

        // Handle to the telephony server
        RTelServer                  iTelServer;

        // Handle to the packet service
        RPacketService              iPacketService;

        // RMobilePhone handle
        RMobilePhone                iMobilePhone;

        // 1 = mmtsy is loaded, 0 = mmtsy is not loaded
        TUint8                      iTSYLoaded;

        // 1 = packet services are loaded, 0 = not loaded
        TUint8                      iPacketServLoaded;

        // to add wlan event type
        CLogEventType*              iLogEventType;
        CLogClient*                 iLogClient;

        // A pointer to the active object that logs connections that are in timer mode.
        CDclTimerAO*                iDclTimerAO;

        TInt                        iTimerInterval;
        TUint                       iGPRSThreshold;
        TUint                       iWCDMAThreshold;
        TUint                       iWLANThreshold;

        // a pointer to SaeObserver that updates P&S variables
        CSaeObserver*               iSaeObserver;
    };

#endif // DCLENGINE_H

// End of File
