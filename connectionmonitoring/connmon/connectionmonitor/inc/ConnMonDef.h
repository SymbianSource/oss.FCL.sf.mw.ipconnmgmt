/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definitions common to server and client API.
*
*/

#ifndef CONNMONDEF_H
#define CONNMONDEF_H

#include <e32base.h>
#include <rconnmon.h>

// Constants

// Maximum connection identifier
const TUint KMaxConnectionId = 1000000;

// Number of message slots in the client-server interface
// 1 common slot for all synchronous methods + 1 for each asyncronous method
// = 1 common + 6 async client api methods + 1 for events
const TInt KNumMessageSlots = 8;

// Number of slots in CEventQueue
const TInt KEventQueueSize = 25;

// Number of pointers allocated for client-server inter-thread reads/writes
const TInt KNumPtrs = 12;

// Server will shutdown after a 2 second period after the last client
// has disconnected. Note. Don't make this smaller. CTimerAO relies on this.
const TUint KConnMonShutdownInterval = 2000000;

// Update ConnMon documentation (attributes KDownlinkDataThreshold and
// KUplinkDataThreshold) if changing these values (4096, 20480, 51200, 102400).
const TUint KMinimumDataThreshold      = 4096;
const TUint KMinimumGPRSDataThreshold  = 20480;
const TUint KMinimumWCDMADataThreshold = 51200;
const TUint KMinimumWLANDataThreshold  = 102400;

// Update ConnMon documentation (attribute KActivityTimeThreshold) if changing
// this value (KMinimumActivityThreshold = 5)
const TUint KMinimumActivityThreshold = 5;

// CSD/HSCSD in is not valid untill call is open
const TInt KBearerQueryCount   = 5;
const TInt KBearerQueryTimeout = 100000;

const TUid KConnMonServerUid = { 0x101F6CF0 };
const TUid KConnMonDllUid    = { 0x101F6CEE };
const TUid KGenericDllUid    = { 0x1000008c };

_LIT( KConnectionMonitorServerName, "!ConnectionMonitorServer" );

_LIT( KConnectionMonitorServerExe, "ConnMonExe" );
_LIT( KDoubleColon, "::" );

_LIT( KPanicCaughtAnError, "ConnMon: cought an error." );
_LIT( KPanicBadParameter, "ConnMon: bad parameter." );

// Data types
enum TConnMonPanic
    {
    // Server
    ECannotStartServer              = 100,
    EBadRequest                     = 101,
    EBadDescriptor                  = 102,
    EErrorFromNonClientObject       = 103,
    EReceiveAlreadyActive           = 104,
    EIapArrayError                  = 105,
    EConnMonBadHandle               = 106,

    // Used by Client API
    EConnMonPanicClientNotConnected = 150
    };

enum TConnMonThreshold
     {
     EDownlinkThreshold = 0,
     EUplinkThreshold,
     EActivityTimeThreshold,
     EBearerAvailabilityThreshold,
     ESignalStrengthThreshold
     };

enum TConnMonConnFlags
    {
    EConnTypeGPRS   = 0x01,
    EConnTypeWCDMA  = 0x02,
    EConnTypeCSD    = 0x04
    };

enum TConnMonNotifierPriority
    {
    EConnMonPriorityNormal = 0,
    EConnMonPriorityMedium = 5,
    EConnMonPriorityHigh   = 10
    };

/**
 * Status values for dial-up PDP context override functionality.
 */
enum TConnMonDialUpOverrideStatus
    {
    EConnMonDialUpOverrideDisabled = 0,
    EConnMonDialUpOverrideInactive,
    EConnMonDialUpOverrideActive
    };

/**
 * TEventInfo
 */
class TEventInfo
    {
public:
    inline void Reset()
        {
        iEventType = 0;
        iConnectionId = 0;
        iSubConnectionId = 0;
        iData = 0;
        iData2 = 0;
        iData3 = 0;
        }

public: // Data
    TInt iEventType;
    TUint iConnectionId;
    TUint iSubConnectionId;
    TUint iData;
    TUint iData2;
    TUint iData3;
    };

// An additional buffer for events that contain a lot of data.
// E.g. EConnMonIapAvailabilityChange -event uses this buffer.
typedef TBuf8<256> TConnMonExtraBuf;

/**
 * TExtra
 */
class TExtra
    {
public:
    inline void Reset()
        {
        iPtr = NULL;
        iSize = 0;
        }
public: // Data
    const TUint8* iPtr;
    TUint iSize;
    };

/**
 * TEvent
 */
class TEvent
    {
public:
    TEventInfo iInfo;
    TTime iTime;
    TExtra iExtra;
    };

/**
 * TFullIap
 */
class TFullIap
    {
public:
    TUint iIapId;
    TUint iNetId;
    TUint iBearerId;
    };

class TFullIapInfo
    {
public:
    inline void Reset()
        {
        iCount = 0;
        for ( TUint i = 0; i < KConnMonMaxIAPCount; i++ )
            {
            iIap[i].iIapId = 0;
            iIap[i].iNetId = 0;
            iIap[i].iBearerId = 0;
            }
        }

public:
    TUint iCount;
    TFullIap iIap[KConnMonMaxIAPCount];
    };

class TBearerInfo
    {
public:
    TBearerInfo()
        {
        iBearer = 0;
        iInternal = ETrue;
        };
    TBearerInfo( TInt aBearer, TBool aInternal )
        {
        iBearer = aBearer,
        iInternal = aInternal;
        }
    inline void Reset()
        {
        iBearer = 0;
        iInternal = ETrue;
        }
    TInt iBearer;
    TBool iInternal;
    };

// Prefix 'ConnMon' used to avoid any future problems or name overlapping with
// other similar implementations.
template <class T>
class ConnMonCleanupResetAndDestroy
    {
public:
    inline static void PushL( T& aRef );

private:
    static void ResetAndDestroy( TAny *aPtr );
    };

template <class T>
inline void ConnMonCleanupResetAndDestroyPushL( T& aRef );

template <class T>
inline void ConnMonCleanupResetAndDestroy<T>::PushL( T& aRef )
    {
    CleanupStack::PushL( TCleanupItem( &ResetAndDestroy, &aRef ) );
    }

template <class T>
void ConnMonCleanupResetAndDestroy<T>::ResetAndDestroy( TAny *aPtr )
    {
    (STATIC_CAST(T*,aPtr))->ResetAndDestroy();
    }

template <class T>
inline void ConnMonCleanupResetAndDestroyPushL( T& aRef )
    {
    ConnMonCleanupResetAndDestroy<T>::PushL( aRef );
    }

#endif // CONNMONDEF_H

// End-of-file
