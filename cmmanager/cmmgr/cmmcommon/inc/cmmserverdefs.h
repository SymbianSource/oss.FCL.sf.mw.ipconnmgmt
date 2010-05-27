/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Common definitions and enumerations for connection method manager server
* (internal use).
*
*/


#ifndef CMMSERVERDEFS_H
#define CMMSERVERDEFS_H

#include <e32base.h>

static const TUid KCmmUid3 = { 0x20016A7A };

/**
 * CmManager server name.
 */
_LIT( KCmmServer, "!CmmServer" );

/**
 * CmManager server filename.
 */
_LIT( KCmmServerFile, "cmmserver" );

const TInt KBitsInOneByte = 8;
const TInt KBitsInTwoBytes = 16;
const TInt KBitsInThreeBytes = 24;

/**
 * Amount of time in microseconds that the server will wait before shutting
 * down, after last client has gone.
 */
#if defined(__WINSCW__)
const TUint KCmmShutdownDelay = 5000000; // 5 seconds, for testing purposes.
#else
const TUint KCmmShutdownDelay = 70000000; // 70 seconds.
#endif

const TUint32 KCmDefaultDestinationAPTagId = 0x1000;
const TUint32 KCmMaxDestinationAPTagId = 0x1100;

const TUint32 KCmmConnMethodIdIntervalMin = 0;
const TUint32 KCmmConnMethodIdIntervalMax = 0x0100;

const TUint32 KCmmDestIdIntervalLegacyMin = 0;
const TUint32 KCmmDestIdIntervalLegacyMax = 0x0100;

const TUint32 KCmmDestIdIntervalMin = 0x1000;
const TUint32 KCmmDestIdIntervalMax = 0x1100;

const TInt KCmmStringLengthMax = 1024; //TODO, need to find out correct value here. 1024 is correct, but can that be received from commsdat?

const TUint32 KTemporaryIdCounterStart = 32768;

const TUint32 KDestProtectionLevelMask = 0xF0000000;
const TInt KBitsToShiftDestProtectionLevel = 28;


// Buffer size for initial connection method query
const TInt KCmmConnMethodAmountNormal = 64; //TODO, Check value
// Maximum theoretical amount of connection methods
const TInt KCmmConnMethodAmountMax = 256;

// Buffer size for initial destination query
const TInt KCmmDestAmountNormal = 32;
// Maximum theoretical amount of connection methods
const TInt KCmmDestAmountMax = 256;

// Buffer size for supported bearers query
const TInt KCmmBearerAmountNormal = 10;

// Buffer size for bearer priority array query, first attempt
const TInt KCmmDefaultBearerPriorityArraySize = 256;

// iPriority:           2 16-bit values,
// iUIPriority:         2 16-bit values,
// iServiceType length: 1 16-bit value.
const TInt KCmmBearerPriorityHeaderLength = 5;


// Opcodes used in message passing between client and server
enum TCmmRequests
    {
    ECmmGetBearerInfoInt                     = 0,
    ECmmGetBearerInfoBool                    = 1,
    ECmmGetBearerInfoString                  = 2,
    ECmmGetBearerInfoString8                 = 3,
    ECmmGetConnMethodArray                   = 4,
    ECmmGetAllDestinations                   = 5,
    ECmmGetBearerPriorityArray               = 6,
    ECmmGetEasyWlanId                        = 7,
    ECmmReadDefaultConnection                = 8,
    ECmmReadGeneralConnectionSettings        = 9,
    ECmmGetSupportedBearers                  = 10,
    ECmmGetUncategorizedIcon                 = 11,

    ECmmGetConnMethodInfoInt                 = 12,
    ECmmGetConnMethodInfoBool                = 13,
    ECmmGetConnMethodInfoString              = 14,
    ECmmGetConnMethodInfoString8             = 15,

    ECmmUpdateBearerPriorityArray            = 16,
    ECmmWriteDefaultConnection               = 17,
    ECmmWriteGeneralConnectionSettings       = 18,
    ECmmCopyConnMethod                       = 19,
    ECmmMoveConnMethod                       = 20,
    ECmmRemoveConnMethod                     = 21,
    ECmmRemoveAllReferences                  = 22, // Connection method becomes uncategorized.

    // RCmDestination starts from 100
    EDestGetDestination                      = 100,
    EDestRefresh                             = 101,
    EDestCloseDestination                    = 102,
    EDestGetConnMethodCount                  = 103,
    EDestGetConnMethodPriority               = 104,
    EDestGetName                             = 105,
    EDestGetId                               = 106,
    EDestGetElementId                        = 107,
    EDestMetadata                            = 108,
    EDestGetProtectionLevel                  = 109,
    EDestIsHidden                            = 110,
    EDestIsEqual                             = 111,
    EDestGetEmbeddedDestination              = 112,
    EDestGetIcon                             = 113,

    EDestCreateDestinationWithName           = 114,
    EDestCreateDestinationWithNameAndId      = 115,
    EDestIsConnected                         = 116,
    EDestAddConnMethod                       = 117,
    EDestAddEmbeddedDestination              = 118,
    EDestDeleteConnMethod                    = 119,
    EDestRemoveConnMethod                    = 120,
    EDestModifyPriority                      = 121,
    EDestSetName                             = 122,
    EDestSetMetadata                         = 123,
    EDestSetProtection                       = 124,
    EDestSetHidden                           = 125,
    EDestUpdate                              = 126,
    EDestDelete                              = 127,
    EDestSetIcon                             = 128,

    //RCmConnectionMethod starts from 200
    ECMGetConnMethodWithId                   = 200,
    ECMRefresh                               = 201,
    ECMGetConnMethodFromDestWithIndex        = 202,
    ECMGetConnMethodFromDestWithId           = 203,
    ECMCloseConnMethod                       = 204,
    ECMIsEqual                               = 205,

    ECMCreateConnMethod                      = 206,
    ECMCreateConnMethodWithId                = 207,
    ECMCreateConnMethodToDest                = 208,
    ECMCreateConnMethodToDestWithId          = 209,
    ECMCreateCopyOfExisting                  = 210,

    ECMGetIntAttribute                       = 211,
    ECMGetBoolAttribute                      = 212,
    ECMGetStringAttribute                    = 213,
    ECMGetString8Attribute                   = 214,

    ECMSetIntAttribute                       = 215,
    ECMSetBoolAttribute                      = 216,
    ECMSetStringAttribute                    = 217,
    ECMSetString8Attribute                   = 218,
    ECMDelete                                = 219,
    ECMUpdate                                = 220
    };

// Panic categories
_LIT( KCmmPanicCategoryApi, "CmManager API" );

// Panic codes
enum TCmmServerPanic
    {
    ECreateTrapCleanup = 1,
    ECreateServer,
    EBadDescriptor,
    EBadRequest,
    EInvalidImportState,
    EDestNotConnectedToServer = 201
    };

// Status flag values for database records.
enum TCmmRecordStatus
    {
    ECmmRecordStatusBlank = 0x0001,
    ECmmRecordStatusLoaded = 0x0002, // Loaded and up-to-date.
    ECmmRecordStatusExpired = 0x0004, // Loaded but out-of-date (Cache side only).
    ECmmRecordStatusModified = 0x0008, // Loaded and modified (Session side only).
    ECmmRecordStatusUnsaved = 0x0010 // Doesn't yet exist in database (Cache side only).
    };

// Status flag values for connection method instances.
enum TCmmConnMethodStatus
    {
    ECmmConnMethodStatusNotSaved = 1, // Connection method doesn't yet exist in database (cache side only).
    ECmmConnMethodStatusValid = 2, // Connection method exists in database and sessions can open handles to it.
    ECmmConnMethodStatusChanged = 3, // Contents modified, but not saved (session side only).
    ECmmConnMethodStatusToBeDeleted = 4 // Connection method will be deleted when reference count goes to 0 (cache side only).
    };

// Status flag values for destination instances.
enum TCmmDestinationStatus
    {
    ECmmDestinationStatusNotSaved = 1, // Destination doesn't yet exist in database (Cache side only).
    ECmmDestinationStatusValid = 2, // Destination exists in database and sessions can open handles to it.
    ECmmDestinationStatusChanged = 3, // Contents modified, but not saved (Session side only).
    ECmmDestinationStatusToBeDeleted = 4 // Destination will be deleted when reference count goes to 0 (cache side only).
    };

// Identifiers for different destination related database records for internal use.
enum TCmmDbRecords
    {
    ECmmDestNetworkRecord = 1,  //
    ECmmDestApRecord,           //
    ECmmDestMetadataRecord,     // CCDSNAPMetadataRecord
    ECmmDbSnapRecord,           // CCDDataMobilitySelectionPolicyRecord
    ECmmDbBearerPriorityRecord, //
    ECmmDbDefConnRecord         //
    };

NONSHARABLE_CLASS( TCmmIpcStructGetConnMethods )
    {
public:
    TBool iCheckBearerType;
    TBool iLegacyOnly;
    TBool iEasyWlan;
    };

NONSHARABLE_CLASS( TCmmIpcStructMoveConnMethod )
    {
public:
    TInt iSourceDestHandle;
    TInt iTargetDestHandle;
    TInt iConnMethodHandle;
    TInt iIndex;
    };

// Prefix 'Cmm' used to avoid any future problems or name overlapping with
// other similar implementations.
template <class T>
class CmmCleanupResetAndDestroy
    {
public:
    inline static void PushL( T& aRef );

private:
    static void ResetAndDestroy( TAny *aPtr );
    };

template <class T>
inline void CmmCleanupResetAndDestroyPushL( T& aRef );

template <class T>
inline void CmmCleanupResetAndDestroy<T>::PushL( T& aRef )
    {
    CleanupStack::PushL( TCleanupItem( &ResetAndDestroy, &aRef ) );
    }

template <class T>
void CmmCleanupResetAndDestroy<T>::ResetAndDestroy( TAny *aPtr )
    {
    (STATIC_CAST(T*,aPtr))->ResetAndDestroy();
    }

template <class T>
inline void CmmCleanupResetAndDestroyPushL( T& aRef )
    {
    CmmCleanupResetAndDestroy<T>::PushL( aRef );
    }

#endif // CMMSERVERDEFS_H

// End of file
