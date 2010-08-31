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
* Cache side object representing a destination.
*
*/


#include <datamobilitycommsdattypes.h>
#include <in_sock.h> // KAfInet

#include "cmmdestinationstruct.h"
#include "cmmtransactionhandler.h"
#include "cmmdestinationinstance.h"
#include "cmmcache.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmdestinationstructTraces.h"
#endif

const TUint KDefaultTierManagerTagId = KAfInet;


// ---------------------------------------------------------------------------
// Two phased construction.
// Opens a destination from database based on ID.
// ---------------------------------------------------------------------------
//
CCmmDestinationStruct* CCmmDestinationStruct::NewL(
        CCmmCache* aCache,
        CCmmTransactionHandler* aTransactionHandler,
        const TUint32 aDestinationId )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_NEWL_ENTRY );

    CCmmDestinationStruct* self = CCmmDestinationStruct::NewLC(
            aCache,
            aTransactionHandler,
            aDestinationId );
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_NEWL_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// Two phased construction.
// Creates a new destination with name and ID (optional).
// If ID is 0, a free ID will be used when destination is saved to database.
// ---------------------------------------------------------------------------
//
CCmmDestinationStruct* CCmmDestinationStruct::NewL(
        CCmmCache* aCache,
        CCmmTransactionHandler* aTransactionHandler,
        const TDesC& aName,
        const TUint32 aDestinationId )
    {
    OstTraceFunctionEntry0( DUP1_CCMMDESTINATIONSTRUCT_NEWL_ENTRY );

    CCmmDestinationStruct* self = CCmmDestinationStruct::NewLC(
            aCache,
            aTransactionHandler,
            aName,
            aDestinationId );
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( DUP1_CCMMDESTINATIONSTRUCT_NEWL_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// Two phased construction.
// Opens a destination from database based on ID.
// ---------------------------------------------------------------------------
//
CCmmDestinationStruct* CCmmDestinationStruct::NewLC(
        CCmmCache* aCache,
        CCmmTransactionHandler* aTransactionHandler,
        const TUint32 aDestinationId )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_NEWLC_ENTRY );

    CCmmDestinationStruct* self = new( ELeave ) CCmmDestinationStruct(
            aCache,
            aTransactionHandler );
    CleanupStack::PushL( self );
    self->ConstructL( aDestinationId );

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_NEWLC_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// Two phased construction.
// Creates a new destination with name and ID (optional).
// If ID is 0, a free ID will be used when destination is saved to database.
// ---------------------------------------------------------------------------
//
CCmmDestinationStruct* CCmmDestinationStruct::NewLC(
        CCmmCache* aCache,
        CCmmTransactionHandler* aTransactionHandler,
        const TDesC& aName,
        const TUint32 aDestinationId )
    {
    OstTraceFunctionEntry0( DUP1_CCMMDESTINATIONSTRUCT_NEWLC_ENTRY );

    CCmmDestinationStruct* self = new( ELeave ) CCmmDestinationStruct(
            aCache,
            aTransactionHandler );
    CleanupStack::PushL( self );
    self->ConstructL( aName, aDestinationId );

    OstTraceFunctionExit0( DUP1_CCMMDESTINATIONSTRUCT_NEWLC_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// Cancels any active requests.
// ---------------------------------------------------------------------------
//
CCmmDestinationStruct::~CCmmDestinationStruct()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_CCMMDESTINATIONSTRUCT_ENTRY );

    if ( iReferenceCounter != 0 )
        {
        ASSERT(0); // Error.
        }
    delete iNetworkRecord;
    delete iDestApRecord;
    delete iMetadataRecord;
    delete iName;

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_CCMMDESTINATIONSTRUCT_EXIT );
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CCmmDestinationStruct::CCmmDestinationStruct(
        CCmmCache* aCache,
        CCmmTransactionHandler* aTransactionHandler )
        :
        iCache( aCache ),
        iTransactionHandler( aTransactionHandler )
    {
    OstTraceFunctionEntry0( DUP1_CCMMDESTINATIONSTRUCT_CCMMDESTINATIONSTRUCT_ENTRY );

    iDestApRecord = NULL;
    iNetworkRecord = NULL;
    iMetadataRecord = NULL;
    iDestApRecordStatus = ECmmRecordStatusBlank;
    iNetworkRecordStatus = ECmmRecordStatusBlank;
    iMetadataRecordStatus = ECmmRecordStatusBlank;
    iReferenceCounter = 0;
    iStatus = ECmmDestinationStatusNotSaved;
    iId = 0;
    iName = NULL;

    OstTraceFunctionExit0( DUP1_CCMMDESTINATIONSTRUCT_CCMMDESTINATIONSTRUCT_EXIT );
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// Opens an existing destination based on ID.
// ---------------------------------------------------------------------------
//
void CCmmDestinationStruct::ConstructL( const TUint32 aDestinationId )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_CONSTRUCTL_ENTRY );

    // Check ID is in valid range.
    if ( aDestinationId <= KCmDefaultDestinationAPTagId ||
            aDestinationId >= KCmMaxDestinationAPTagId )
        {
        User::Leave( KErrArgument );
        }
    iId = aDestinationId;

    iDestApRecord = static_cast<CommsDat::CCDAccessPointRecord*>(
            CommsDat::CCDRecordBase::RecordFactoryL( CommsDat::KCDTIdAccessPointRecord ) );
    iNetworkRecord = static_cast<CommsDat::CCDNetworkRecord*>(
            CommsDat::CCDRecordBase::RecordFactoryL( CommsDat::KCDTIdNetworkRecord ) );
    iMetadataRecord = new( ELeave ) CCDSNAPMetadataRecord(
            iCache->TableId( ECmmDestMetadataRecord ) );

    // Load access point record.
    iDestApRecord->iRecordTag = iId;
    if ( iDestApRecord->FindL( iTransactionHandler->Session() ) )
        {
        iDestApRecord->LoadL( iTransactionHandler->Session() );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    iDestApRecordStatus = ECmmRecordStatusLoaded;

    // Load network record.
    iNetworkRecord->SetRecordId( iDestApRecord->iCustomSelectionPolicy );
    iNetworkRecord->LoadL( iTransactionHandler->Session() );
    iNetworkRecordStatus = ECmmRecordStatusLoaded;

    // Load metadata record.
    iMetadataRecord->iSNAP = iId;
    if ( !iMetadataRecord->FindL( iTransactionHandler->Session() ) )
        {
        // Not found -> fill in with default values.
        iMetadataRecord->iSNAP = iId;
        iMetadataRecord->iMetadata = 0;
        iMetadataRecord->SetRecordId( KCDNewRecordRequest );
        iMetadataRecordStatus = ECmmRecordStatusUnsaved;
        }
    else
        {
        iMetadataRecord->LoadL( iTransactionHandler->Session() );
        iMetadataRecordStatus = ECmmRecordStatusLoaded;
        }

    SetStatus( ECmmDestinationStatusValid );

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// Creates a new destination. With name and optionally also with ID.
// ---------------------------------------------------------------------------
//
void CCmmDestinationStruct::ConstructL( const TDesC& aName, const TUint32 aDestinationId )
    {
    OstTraceFunctionEntry0( DUP1_CCMMDESTINATIONSTRUCT_CONSTRUCTL_ENTRY );

    SetStatus( ECmmDestinationStatusNotSaved );
    iId = aDestinationId;
    iName = aName.AllocL();

    iDestApRecord = static_cast<CommsDat::CCDAccessPointRecord*>(
            CommsDat::CCDRecordBase::RecordFactoryL( CommsDat::KCDTIdAccessPointRecord ) );
    iNetworkRecord = static_cast<CommsDat::CCDNetworkRecord*>(
            CommsDat::CCDRecordBase::RecordFactoryL( CommsDat::KCDTIdNetworkRecord ) );
    iMetadataRecord = new( ELeave ) CCDSNAPMetadataRecord(
            iCache->TableId( ECmmDestMetadataRecord ) );

    if ( aDestinationId >= KTemporaryIdCounterStart )
        {
        // Get a real ID from database during UpdateL().
        iNetworkRecord->SetRecordId( KCDNewRecordRequest );
        iDestApRecord->iRecordTag = 0;
        iDestApRecord->iCustomSelectionPolicy = 0;
        // Access Point record tag ID and custom selection policy value need to
        // be set during UpdateL(), when network record's record ID is known.
        // Also metadata record SNAP ID needs to be set.
        iMetadataRecord->iSNAP = 0;
        }
    else
        {
        // Client provided an actual ID.
        iNetworkRecord->SetRecordId( aDestinationId - KCmDefaultDestinationAPTagId );
        iDestApRecord->iRecordTag = aDestinationId;
        iDestApRecord->iCustomSelectionPolicy = ( aDestinationId - KCmDefaultDestinationAPTagId );
        iMetadataRecord->iSNAP = aDestinationId;
        }
    iDestApRecord->SetRecordId( KCDNewRecordRequest );

    iMetadataRecord->iMetadata = 0;
    iMetadataRecord->SetRecordId( KCDNewRecordRequest );

    iNetworkRecord->iRecordName.SetL( aName );
    iDestApRecord->iRecordName.SetL( aName );

    InitializeDestApRecordL();

    iDestApRecordStatus = ECmmRecordStatusUnsaved;
    iNetworkRecordStatus = ECmmRecordStatusUnsaved;
    iMetadataRecordStatus = ECmmRecordStatusUnsaved;

    OstTraceFunctionExit0( DUP1_CCMMDESTINATIONSTRUCT_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------
// Set the destination ID.
// ---------------------------------------------------------------------------
//
void CCmmDestinationStruct::SetId( const TUint32 aId )
    {
    iId = aId;
    }

// ---------------------------------------------------------------------------
// Get the destination ID.
// ---------------------------------------------------------------------------
//
TUint32 CCmmDestinationStruct::GetId() const
    {
    return iId;
    }

// ---------------------------------------------------------------------------
// Get the destination ID from network record. To get a valid ID, this must be
// called only during or after a call to UpdateL().
// ---------------------------------------------------------------------------
//
TUint32 CCmmDestinationStruct::GetRealId() const
    {
    TUint32 id( iId );
    if ( iNetworkRecord )
        {
        id = ( TUint32 )iNetworkRecord->RecordId() + KCmDefaultDestinationAPTagId;
        }
    return id;
    }

// ---------------------------------------------------------------------------
// Get the destination name.
// ---------------------------------------------------------------------------
//
TDesC& CCmmDestinationStruct::GetDestinationNameL()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_GETDESTINATIONNAMEL_ENTRY );

    // No network record exists if destination has not been saved yet.
    if ( iStatus == ECmmDestinationStatusNotSaved && iName )
        {
        OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_GETDESTINATIONNAMEL_EXIT );
        return *iName;
        }

    if ( iNetworkRecord )
        {
        return iNetworkRecord->iRecordName.GetL();
        }

    User::Leave( KErrCorrupt );

    OstTraceFunctionExit0( DUP1_CCMMDESTINATIONSTRUCT_GETDESTINATIONNAMEL_EXIT );
    return *iName;
    }

// ---------------------------------------------------------------------------
// Gets the current status of this destination struct.
// ---------------------------------------------------------------------------
//
TCmmDestinationStatus CCmmDestinationStruct::GetStatus() const
    {
    return iStatus;
    }

// ---------------------------------------------------------------------------
// Get the current reference count.
// ---------------------------------------------------------------------------
//
TInt CCmmDestinationStruct::GetReferenceCount() const
    {
    return iReferenceCounter;
    }

// ---------------------------------------------------------------------------
// Set the current status of this destination.
// ECmmDestinationStatusValid: Destination exists in database.
// ECmmDestinationStatusNotSaved: Destination does NOT exist in database yet.
// ---------------------------------------------------------------------------
//
void CCmmDestinationStruct::SetStatus( const TCmmDestinationStatus& aStatus )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_SETSTATUS_ENTRY );

    switch ( aStatus )
        {
        case ECmmDestinationStatusValid:
            {
            if ( iStatus == ECmmDestinationStatusNotSaved )
                {
                delete iName;
                iName = NULL;
                }
            }
            // Fallthrough intended.
        case ECmmDestinationStatusNotSaved:
        case ECmmDestinationStatusToBeDeleted:
            {
            // Set the status.
            iStatus = aStatus;
            }
            break;
        default:
            ASSERT( 0 ); // Error, invalid status.
            break;
        }

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_SETSTATUS_EXIT );
    }

// ---------------------------------------------------------------------------
// Notify about a possible change in database on specified record table.
// ---------------------------------------------------------------------------
//
void CCmmDestinationStruct::NotifyRecordChange( const TCmmDbRecords aRecordType )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_NOTIFYRECORDCHANGE_ENTRY );

    // Only change status if it is currently ECmmRecordStatusLoaded.
    switch ( aRecordType )
        {
        case ECmmDestNetworkRecord:
            {
            if ( iNetworkRecordStatus == ECmmRecordStatusLoaded )
                {
                iNetworkRecordStatus = ECmmRecordStatusExpired;
                }
            }
            break;
        case ECmmDestApRecord:
            {
            if ( iDestApRecordStatus == ECmmRecordStatusLoaded )
                {
                iDestApRecordStatus = ECmmRecordStatusExpired;
                }
            }
            break;
        case ECmmDestMetadataRecord:
            {
            if ( iMetadataRecordStatus == ECmmRecordStatusLoaded )
                {
                iMetadataRecordStatus = ECmmRecordStatusExpired;
                }
            }
            break;
        case ECmmDbSnapRecord:
        case ECmmDbBearerPriorityRecord:
        case ECmmDbDefConnRecord:
        // Fallthrough intended.
        default:
            // The record table doesn't affect destinations, just ignore it.
            break;
        }

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_NOTIFYRECORDCHANGE_EXIT );
    }

// ---------------------------------------------------------------------------
// Set the record status for all records.
// ---------------------------------------------------------------------------
//
void CCmmDestinationStruct::SetStatusForAllRecords( const TCmmRecordStatus aStatus )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_SETSTATUSFORALLRECORDS_ENTRY );

    iDestApRecordStatus = aStatus;
    iNetworkRecordStatus = aStatus;
    iMetadataRecordStatus = aStatus;

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_SETSTATUSFORALLRECORDS_EXIT );
    }

// ---------------------------------------------------------------------------
// Copies the data for this destination to a session instance and increments
// the reference counter by one.
// ---------------------------------------------------------------------------
//
void CCmmDestinationStruct::CreateDestinationInstanceL(
        CCmmDestinationInstance& aDestinationInstance )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_CREATESESSIONINSTANCEL_ENTRY );

    // This should work according to status. If status is valid, this
    // destination exists in commsdat, and other sessions can also open a handle
    // to it. Make copies of the records to give to session instance.
    //
    // If the status is unsaved, then this is a new destination that hasn't
    // been saved to commsdat yet. Thus no other session can open a handle to it
    // until it is saved with a call to UpdateL().
    // For a non-valid destination, to save memory, transfer the ownership of
    // record pointers straight to the session and mark the cache pointers as
    // NULL. When update is called, copy the records into the cache also.
    //

    switch ( iStatus )
        {
        case ECmmDestinationStatusNotSaved:
            {
            // Transfer ownership of table records to session. Mark local pointers to NULL.

            aDestinationInstance.SetRecordPointer( ECmmDestApRecord, iDestApRecord ); // iDestApRecord ownership is transfered.
            aDestinationInstance.SetRecordPointer( ECmmDestNetworkRecord, iNetworkRecord ); // iNetworkRecord ownership is transfered.
            aDestinationInstance.SetRecordPointer( ECmmDestMetadataRecord, iMetadataRecord ); // iMetadataRecord ownership is transfered.
            iDestApRecord = NULL;
            iNetworkRecord = NULL;
            iMetadataRecord = NULL;
            iDestApRecordStatus = ECmmRecordStatusBlank;
            iNetworkRecordStatus = ECmmRecordStatusBlank;
            iMetadataRecordStatus = ECmmRecordStatusBlank;

            aDestinationInstance.SetId( iId );
            aDestinationInstance.SetStatus( ECmmDestinationStatusChanged );
            }
            break;
        case ECmmDestinationStatusValid:
            {
            // Reload table records if needed, make copies and transfer those to session.
            LoadRecordL( aDestinationInstance, ECmmDestApRecord );
            LoadRecordL( aDestinationInstance, ECmmDestNetworkRecord );
            LoadRecordL( aDestinationInstance, ECmmDestMetadataRecord );

            aDestinationInstance.SetId( iId );
            aDestinationInstance.SetStatus( ECmmDestinationStatusValid );
            }
            break;
        case ECmmDestinationStatusChanged:
        case ECmmDestinationStatusToBeDeleted:
        default:
            User::Leave( KErrCorrupt ); // Invalid status.
            break;
        }

    // A session handle has been made, increment reference counter.
    iReferenceCounter++;

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_CREATESESSIONINSTANCEL_EXIT );
    }

// ---------------------------------------------------------------------------
// Refresh the data contained in aDestinationInstance. This means reloading
// the data from database if necessary. After this call the contents of
// aDestinationInstance will reflect the current state in the database. 
// ---------------------------------------------------------------------------
//
void CCmmDestinationStruct::RefreshDestinationInstanceL(
        CCmmDestinationInstance& aDestinationInstance )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_REFRESHSESSIONINSTANCEL_ENTRY );

    switch ( iStatus )
        {
        case ECmmDestinationStatusValid:
        case ECmmDestinationStatusToBeDeleted:
            {
            // Reload table records if needed, make copies and transfer those to session.
            LoadRecordL( aDestinationInstance, ECmmDestApRecord );
            LoadRecordL( aDestinationInstance, ECmmDestNetworkRecord );
            LoadRecordL( aDestinationInstance, ECmmDestMetadataRecord );
            }
            break;
        case ECmmDestinationStatusNotSaved: // This was checked before.
        case ECmmDestinationStatusChanged:
        default:
            User::Leave( KErrCorrupt ); // Invalid status.
            break;
        }

    // Add list of currently contained connection methods.
    iCache->GetConnMethodsFromDestinationL(
            aDestinationInstance.GetId(),
            aDestinationInstance.iConnMethodItemArray );

    // Internal state need to be set to the same state as after a successfull update.
    aDestinationInstance.UpdateSuccessful();

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_REFRESHSESSIONINSTANCEL_EXIT );
    }

// ---------------------------------------------------------------------------
// Reloads a destination record if needed and copies the latest version to
// the session instance given as parameter.
// ---------------------------------------------------------------------------
//
void CCmmDestinationStruct::LoadRecordL(
        CCmmDestinationInstance& aDestinationInstance,
        TCmmDbRecords aRecordType )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_LOADRECORDL_ENTRY );

    if ( iStatus != ECmmDestinationStatusValid && iStatus != ECmmDestinationStatusToBeDeleted )
        {
        // Destination must exist in database to be able to load any records.
        User::Leave( KErrCorrupt );
        }

    // Record status:            Action:
    //
    // ECmmRecordStatusBlank     Error
    // ECmmRecordStatusLoaded    Skip load, already loaded
    // ECmmRecordStatusExpired   Load record from database
    // ECmmRecordStatusModified  Error, session side only status
    // ECmmRecordStatusUnsaved   Skip load, not in database

    CommsDat::CCDRecordBase* aRecordPointer;

    switch ( aRecordType )
        {
        case ECmmDestNetworkRecord:
            {
            if ( iNetworkRecordStatus == ECmmRecordStatusExpired )
                {
                iNetworkRecord->LoadL( iTransactionHandler->Session() );
                iNetworkRecordStatus = ECmmRecordStatusLoaded;
                }
            else if ( iNetworkRecordStatus == ECmmRecordStatusBlank ||
                    iNetworkRecordStatus == ECmmRecordStatusModified )
                {
                User::Leave( KErrCorrupt );
                }
            aRecordPointer = CopyRecordL(
                    ECmmDestNetworkRecord,
                    static_cast<CommsDat::CCDRecordBase*>( iNetworkRecord ) );
            aDestinationInstance.SetRecordPointer(
                    ECmmDestNetworkRecord,
                    aRecordPointer ); // Ownership is transfered.
            }
            break;
        case ECmmDestApRecord:
            {
            if ( iDestApRecordStatus == ECmmRecordStatusExpired )
                {
                iDestApRecord->LoadL( iTransactionHandler->Session() );
                iDestApRecordStatus = ECmmRecordStatusLoaded;
                }
            aRecordPointer = CopyRecordL(
                    ECmmDestApRecord,
                    static_cast<CommsDat::CCDRecordBase*>( iDestApRecord ) );
            aDestinationInstance.SetRecordPointer(
                    ECmmDestApRecord,
                    aRecordPointer ); // Ownership is transfered.
            }
            break;
        case ECmmDestMetadataRecord:
            {
            if ( iMetadataRecordStatus == ECmmRecordStatusExpired )
                {
                iMetadataRecord->LoadL( iTransactionHandler->Session() );
                iMetadataRecordStatus = ECmmRecordStatusLoaded;
                }
            aRecordPointer = CopyRecordL(
                    ECmmDestMetadataRecord,
                    static_cast<CommsDat::CCDRecordBase*>( iMetadataRecord ) );
            aDestinationInstance.SetRecordPointer(
                    ECmmDestMetadataRecord,
                    aRecordPointer ); // Ownership is transfered.
            }
            break;
        default:
            {
            User::Leave( KErrArgument );
            }
            break;
        }
    aRecordPointer = NULL;

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_LOADRECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// Save the contents of a session side destination handle into database.
// ---------------------------------------------------------------------------
//
void CCmmDestinationStruct::UpdateL(
        CCmmDestinationInstance& aDestinationInstance,
        CCmmCache* aCache )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_UPDATEL_ENTRY );

    TBool setProtection( EFalse );
    CMManager::TProtectionLevel protLevel( CMManager::EProtLevel0 );

    // Structure:
    // - Use ModifyL() or StoreL() according to own status (new destination or
    //   changes to existing one).
    //
    // - Update (store/modify) records to database using client copies.
    // - Free local record tables.
    // - Copy record tables from client instance into cache.
    // - Update DataMobilitySelectionPolicy-records.
    //

    TInt id( iId );
    switch ( aDestinationInstance.GetStatus() )
        {
        case ECmmDestinationStatusValid:
            {
            if ( iStatus != ECmmDestinationStatusValid )
                {
                // Error, a 'valid'-status on session side implies a 'valid'-status on cache side.
                User::Leave( KErrCorrupt );
                }
            }
            // Fallthrough intended.
        case ECmmDestinationStatusChanged:
            {
            CommsDat::CCDRecordBase* recordPointer;

            // Get pointers to destination records in client instance.
            aDestinationInstance.GetRecordPointer( ECmmDestApRecord, recordPointer );
            CommsDat::CCDAccessPointRecord* instanceDestApRecord =
                    static_cast<CommsDat::CCDAccessPointRecord*>( recordPointer );
            aDestinationInstance.GetRecordPointer( ECmmDestNetworkRecord, recordPointer );
            CommsDat::CCDNetworkRecord* instanceNetworkRecord =
                    static_cast<CommsDat::CCDNetworkRecord*>( recordPointer );
            aDestinationInstance.GetRecordPointer( ECmmDestMetadataRecord, recordPointer );
            CCDSNAPMetadataRecord* instanceMetadataRecord =
                    static_cast<CCDSNAPMetadataRecord*>( recordPointer );

            recordPointer = NULL;

            // Set protections on network record as needed.
            protLevel = aDestinationInstance.CurrentProtectionLevelL();
            if ( protLevel == CMManager::EProtLevel1 ||
                    protLevel == CMManager::EProtLevel2 )
                {
                setProtection = ETrue;
                }
            SetAttribute( instanceNetworkRecord, CommsDat::ECDProtectedWrite, setProtection );

            switch ( iStatus )
                {
                case ECmmDestinationStatusNotSaved:
                    {
                    // This is a new destination that doesn't exist in database yet.

                    // Store network record first, the record ID is needed for
                    // destination access point record.
                    // Record ID for network record is set during construction.
                    instanceNetworkRecord->StoreL( iTransactionHandler->Session() );

                    // If ID is out of valid range, it means this destination was created
                    // without a specific ID. It is a temporary ID and can now be replaced
                    // with the real database ID. This also enables this destination to be
                    // opened by other clients.
                    // Record ID from network record must be used as real ID.
                    if ( iId >= KTemporaryIdCounterStart )
                        {
                        id = instanceNetworkRecord->RecordId() + KCmDefaultDestinationAPTagId;
                        instanceDestApRecord->iRecordTag = id;
                        instanceDestApRecord->iCustomSelectionPolicy =
                                ( id - KCmDefaultDestinationAPTagId );
                        instanceMetadataRecord->iSNAP = id;
                        }
                    instanceDestApRecord->StoreL( iTransactionHandler->Session() );
                    instanceMetadataRecord->StoreL( iTransactionHandler->Session() );
                    }
                    break;

                case ECmmDestinationStatusValid:
                case ECmmDestinationStatusToBeDeleted:
                    {
                    // This destination already exists in database and is beeing modified.

                    instanceNetworkRecord->ModifyL( iTransactionHandler->Session() );
                    instanceDestApRecord->ModifyL( iTransactionHandler->Session() );
                    if ( iMetadataRecordStatus == ECmmRecordStatusUnsaved )
                        {
                        instanceMetadataRecord->StoreL( iTransactionHandler->Session() );
                        }
                    else
                        {
                        instanceMetadataRecord->ModifyL( iTransactionHandler->Session() );
                        }
                    }
                    break;
                case ECmmDestinationStatusChanged:
                default:
                    User::Leave( KErrCorrupt ); // Error, invalid status.
                    break;
                }

            // Copy destination records from client instance into cache.
            delete iDestApRecord;
            delete iNetworkRecord;
            delete iMetadataRecord;
            iDestApRecord = NULL;
            iNetworkRecord = NULL;
            iMetadataRecord = NULL;
            iDestApRecordStatus = ECmmRecordStatusBlank;
            iNetworkRecordStatus = ECmmRecordStatusBlank;
            iMetadataRecordStatus = ECmmRecordStatusBlank;
            iDestApRecord = static_cast<CommsDat::CCDAccessPointRecord*>(
                    CopyRecordL( ECmmDestApRecord, instanceDestApRecord ) );
            iNetworkRecord = static_cast<CommsDat::CCDNetworkRecord*>(
                    CopyRecordL( ECmmDestNetworkRecord, instanceNetworkRecord ) );
            iMetadataRecord = static_cast<CCDSNAPMetadataRecord*>(
                    CopyRecordL( ECmmDestMetadataRecord, instanceMetadataRecord ) );

            instanceDestApRecord = NULL;
            instanceNetworkRecord = NULL;
            instanceMetadataRecord = NULL;



            // Write DataMobilitySelectionPolicy-records into database.
            TInt neededRecordCount( aDestinationInstance.iConnMethodItemArray.Count() ); // +1 for destination itself.
            TInt existingRecordCount( 0 );

            // Create DataMobilitySelectionPolicy-record set.
            CommsDat::CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>* snapRecordSet =
                    new( ELeave ) CommsDat::CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>(
                            iCache->TableId( ECmmDbSnapRecord ) );
            CleanupStack::PushL( snapRecordSet );

            // Create a DataMobilitySelectionPolicy-record to prime the set.
            CCDDataMobilitySelectionPolicyRecord* snapRecordPrime =
                    new( ELeave ) CCDDataMobilitySelectionPolicyRecord(
                            iCache->TableId( ECmmDbSnapRecord ) );
            CleanupStack::PushL( snapRecordPrime );

            snapRecordPrime->iSNAP = id;
            snapRecordSet->iRecords.AppendL( snapRecordPrime );
            CleanupStack::Pop( snapRecordPrime ); // Ownership moved.

            // Find matching DataMobilitySelectionPolicy-records.
            if ( snapRecordSet->FindL( iTransactionHandler->Session() ) )
                {
                existingRecordCount = snapRecordSet->iRecords.Count();
                }

            CCDDataMobilitySelectionPolicyRecord* snapRecord1 =
                    new( ELeave ) CCDDataMobilitySelectionPolicyRecord(
                            iCache->TableId( ECmmDbSnapRecord ) );
            CleanupStack::PushL( snapRecord1 );

            // Current index in snapRecordSet.
            TInt index( 0 );

            // Initial record for destination itself.
            snapRecord1->iSNAP = id;
            snapRecord1->iRecordName.SetL( iNetworkRecord->iRecordName.GetL() );

            // Set protections on snap record.
            SetAttribute( snapRecord1, CommsDat::ECDProtectedWrite, setProtection );

            if ( existingRecordCount > index )
                {
                snapRecord1->SetElementId( snapRecordSet->iRecords[index]->ElementId() );
                snapRecord1->iEmbeddedSNAP = 0;
                snapRecord1->iIAP = 0;
                snapRecord1->iPriority = 0;
                snapRecord1->ModifyL( iTransactionHandler->Session() );
                index++;
                }
            else
                {
                snapRecord1->SetRecordId( KCDNewRecordRequest );
                snapRecord1->StoreL( iTransactionHandler->Session() );
                }
            CleanupStack::PopAndDestroy( snapRecord1 );

            // Records for destination contents.
            for ( TInt i = 0; i < neededRecordCount; i++ )
                {
                // Weird stuff starts to happen in commsdat side if we try to
                // reuse a snap-record, so create new record for each entry.
                CCDDataMobilitySelectionPolicyRecord* snapRecord2 =
                        new( ELeave ) CCDDataMobilitySelectionPolicyRecord(
                                iCache->TableId( ECmmDbSnapRecord ) );
                CleanupStack::PushL( snapRecord2 );

                snapRecord2->iSNAP = id;
                snapRecord2->iRecordName.SetL( iNetworkRecord->iRecordName.GetL() );

                // If the connection method was added to destination while
                // still having a temporary ID, update the ID to real ID.
                TUint32 connMethodRealId( aDestinationInstance.iConnMethodItemArray[i].iId );
                if ( connMethodRealId >= KTemporaryIdCounterStart )
                    {
                    aCache->TranslateTemporaryId(
                            aDestinationInstance.iConnMethodItemArray[i].iId,
                            connMethodRealId );
                    }

                // Set protections on destination contents as needed.
                setProtection = EFalse;
                protLevel = aDestinationInstance.CurrentProtectionLevelL();
                if ( protLevel == CMManager::EProtLevel1 )
                    {
                    setProtection = ETrue;
                    }
                SetAttribute( snapRecord2, CommsDat::ECDProtectedWrite, setProtection );

                if ( aDestinationInstance.iConnMethodItemArray[i].IsEmbedded() )
                    {
                    // Embedded destination.
                    snapRecord2->iEmbeddedSNAP = ( TInt )connMethodRealId;
                    snapRecord2->iIAP = 0;
                    snapRecord2->iPriority =
                            CMManager::KDataMobilitySelectionPolicyPriorityWildCard;
                    aDestinationInstance.iConnMethodItemArray[i].iPriority =
                            CMManager::KDataMobilitySelectionPolicyPriorityWildCard;
                    }
                else
                    {
                    // Normal connection method.
                    snapRecord2->iEmbeddedSNAP = 0;
                    snapRecord2->iIAP = CommsDat::KCDTIdIAPRecord | ( connMethodRealId << 8 );
                    // If the connection method is a virtual IAP that doesn't
                    // link to an IAP, set priority to wildcard.
                    if ( aDestinationInstance.iConnMethodItemArray[i].IsVirtual() &&
                            aDestinationInstance.iConnMethodItemArray[i].LinkedIapId() == 0 )
                        {
                        snapRecord2->iPriority =
                                CMManager::KDataMobilitySelectionPolicyPriorityWildCard;
                        aDestinationInstance.iConnMethodItemArray[i].iPriority =
                                CMManager::KDataMobilitySelectionPolicyPriorityWildCard;
                        }
                    else
                        {
                        snapRecord2->iPriority = i + 1; // Priority values start from 1.
                        aDestinationInstance.iConnMethodItemArray[i].iPriority = i + 1;
                        }
                    }

                if ( existingRecordCount > index )
                    {
                    // An old record can be reused.
                    snapRecord2->SetElementId( snapRecordSet->iRecords[index]->ElementId() );
                    snapRecord2->ModifyL( iTransactionHandler->Session() );
                    index++;
                    }
                else
                    {
                    // A new record must be created.
                    snapRecord2->SetRecordId( KCDNewRecordRequest );
                    snapRecord2->StoreL( iTransactionHandler->Session() );
                    }
                CleanupStack::PopAndDestroy( snapRecord2 );
                }

            // Delete any extra records.
            while ( existingRecordCount > index )
                {
                CCDDataMobilitySelectionPolicyRecord* snapRecord3 =
                        new( ELeave ) CCDDataMobilitySelectionPolicyRecord(
                                iCache->TableId( ECmmDbSnapRecord ) );
                CleanupStack::PushL( snapRecord3 );
                snapRecord3->SetElementId( snapRecordSet->iRecords[index]->ElementId() );
                snapRecord3->DeleteL( iTransactionHandler->Session() );
                CleanupStack::PopAndDestroy( snapRecord3 );
                index++;
                }


            CleanupStack::PopAndDestroy( snapRecordSet );
            }
            break;
        case ECmmDestinationStatusNotSaved:
        case ECmmDestinationStatusToBeDeleted:
        default:
            User::Leave( KErrCorrupt ); // Error, invalid status.
            break;
        }

    // Do not update destination status or record status values yet. Cache
    // will set them after successfully commiting transaction to database.

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_UPDATEL_EXIT );
    }

// ---------------------------------------------------------------------------
// Delete all database records of this destination. Also removes any records
// making this destination an embedded destination in another destination.
// ---------------------------------------------------------------------------
//
void CCmmDestinationStruct::DeleteL()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_DELETEL_ENTRY );

    // Check status for records.
    switch ( iNetworkRecordStatus )
        {
        case ECmmRecordStatusLoaded:
        case ECmmRecordStatusExpired:
            {
            iNetworkRecord->DeleteL( iTransactionHandler->Session() );
            }
            break;
        case ECmmRecordStatusUnsaved:
            // Skip.
            break;
        case ECmmRecordStatusBlank:
        case ECmmRecordStatusModified:
        default:
            ASSERT( 0 ); // Error, invalid status.
            User::Leave( KErrCorrupt );
            break;
        }
    switch ( iDestApRecordStatus )
        {
        case ECmmRecordStatusLoaded:
        case ECmmRecordStatusExpired:
            {
            iDestApRecord->DeleteL( iTransactionHandler->Session() );
            }
            break;
        case ECmmRecordStatusUnsaved:
            // Skip.
            break;
        case ECmmRecordStatusBlank:
        case ECmmRecordStatusModified:
        default:
            ASSERT( 0 ); // Error, invalid status.
            User::Leave( KErrCorrupt );
            break;
        }
    switch ( iMetadataRecordStatus )
        {
        case ECmmRecordStatusLoaded:
        case ECmmRecordStatusExpired:
            {
            iMetadataRecord->DeleteL( iTransactionHandler->Session() );
            }
            break;
        case ECmmRecordStatusUnsaved:
            // Skip.
            break;
        case ECmmRecordStatusBlank:
        case ECmmRecordStatusModified:
        default:
            ASSERT( 0 ); // Error, invalid status.
            User::Leave( KErrCorrupt );
            break;
        }

    // Delete all datamobility selection policy records related to this destination.

    // Create DataMobilitySelectionPolicy-record set.
    CommsDat::CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>* snapRecordSet =
            new( ELeave ) CommsDat::CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>(
                    iCache->TableId( ECmmDbSnapRecord ) );
    CleanupStack::PushL( snapRecordSet );

    // Create a DataMobilitySelectionPolicy-record.
    CCDDataMobilitySelectionPolicyRecord* snapRecord =
            new( ELeave ) CCDDataMobilitySelectionPolicyRecord(
                    iCache->TableId( ECmmDbSnapRecord ) );
    CleanupStack::PushL( snapRecord );

    TRAP_IGNORE( snapRecordSet->LoadL( iTransactionHandler->Session() ) );

    // Loop through all datamobility selection policy records.
    TInt snapRecordCount( snapRecordSet->iRecords.Count() );
    for ( TInt i = 0; i < snapRecordCount; i++ )
        {
        snapRecord->SetElementId( snapRecordSet->iRecords[i]->ElementId() );
        snapRecord->LoadL( iTransactionHandler->Session() );

        TInt snap = snapRecord->iSNAP;
        TInt embeddedSnap = snapRecord->iEmbeddedSNAP;
        if ( snap == iId || embeddedSnap == iId )
            {
            snapRecord->DeleteL( iTransactionHandler->Session() );
            }
        }
    CleanupStack::PopAndDestroy( snapRecord );
    CleanupStack::PopAndDestroy( snapRecordSet );

    OstTraceFunctionExit0( DUP1_CCMMDESTINATIONSTRUCT_DELETEL_EXIT );
    }

// ---------------------------------------------------------------------------
// This should be called when a client session closes a destination handle.
// Reference counter is decremented by one and the remaining number of
// references is returned.
// ---------------------------------------------------------------------------
//
TInt CCmmDestinationStruct::DestinationInstanceClosed()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_SESSIONINSTANCECLOSED_ENTRY );

    iReferenceCounter--;
    if ( iReferenceCounter < 0 )
        {
        // Error, reference counter shouldn't be able to go negative.
        ASSERT( 0 );
        iReferenceCounter = 0;
        }

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_SESSIONINSTANCECLOSED_EXIT );
    return iReferenceCounter;
    }

// -----------------------------------------------------------------------------
// Called after this destination has been updated and database transaction has
// completed successfully. Sets the internal state of this destination
// structure to reflect the new valid state.
// -----------------------------------------------------------------------------
//
void CCmmDestinationStruct::UpdateSuccessful()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_UPDATESUCCESSFUL_ENTRY );

    SetStatus( ECmmDestinationStatusValid );
    SetStatusForAllRecords( ECmmRecordStatusLoaded );

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_UPDATESUCCESSFUL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmDestinationStruct::InitializeDestApRecordL
// Set default values to a new destination access point record.
// -----------------------------------------------------------------------------
//
void CCmmDestinationStruct::InitializeDestApRecordL()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_INITIALIZEDESTAPRECORDL_ENTRY );

    SetDefaultTierManagerL();
    SetDefaultMCprL();
    SetDefaultCprL();
    SetDefaultSCprL();
    SetDefaultProtocolL();

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_INITIALIZEDESTAPRECORDL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmDestinationStruct::SetDefaultTierManagerL
// Sets the default tier manager to destination access point record.
// -----------------------------------------------------------------------------
void CCmmDestinationStruct::SetDefaultTierManagerL()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_SETDEFAULTTIERMANAGERL_ENTRY );

    CommsDat::CCDTierRecord* tierRecord =
            static_cast<CommsDat::CCDTierRecord*>(
                    CommsDat::CCDRecordBase::RecordFactoryL( CommsDat::KCDTIdTierRecord ) );
    CleanupStack::PushL( tierRecord );

    tierRecord->iRecordTag = KDefaultTierManagerTagId;

    if ( tierRecord->FindL( iTransactionHandler->Session() ) )
        {
        iDestApRecord->iTier = tierRecord->ElementId();
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy( tierRecord );

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_SETDEFAULTTIERMANAGERL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmDestinationStruct::SetDefaultMCprL
// Sets the default MCpr to destination access point record.
// -----------------------------------------------------------------------------
void CCmmDestinationStruct::SetDefaultMCprL()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_SETDEFAULTMCPRL_ENTRY );

    CommsDat::CCDMCprRecord* mcprRecord =
            static_cast<CommsDat::CCDMCprRecord*>(
                    CommsDat::CCDRecordBase::RecordFactoryL( CommsDat::KCDTIdMCprRecord ) );
    CleanupStack::PushL( mcprRecord );

    mcprRecord->SetRecordId( 1 );

    mcprRecord->LoadL( iTransactionHandler->Session() );
    iDestApRecord->iMCpr = mcprRecord->ElementId();
    CleanupStack::PopAndDestroy( mcprRecord );

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_SETDEFAULTMCPRL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmDestinationStruct::SetDefaultCprL
// Sets the default Cpr to destination access point record.
// -----------------------------------------------------------------------------
void CCmmDestinationStruct::SetDefaultCprL()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_SETDEFAULTCPRL_ENTRY );

    CommsDat::CCDCprRecord* cprRecord =
            static_cast<CommsDat::CCDCprRecord*>(
                    CommsDat::CCDRecordBase::RecordFactoryL( CommsDat::KCDTIdCprRecord ) );
    CleanupStack::PushL( cprRecord );

    cprRecord->SetRecordId( 1 );

    cprRecord->LoadL( iTransactionHandler->Session() );
    iDestApRecord->iCpr = cprRecord->ElementId();
    CleanupStack::PopAndDestroy( cprRecord );

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_SETDEFAULTCPRL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmDestinationStruct::SetDefaultSCprL
// Sets the default SCpr to destination access point record.
// -----------------------------------------------------------------------------
void CCmmDestinationStruct::SetDefaultSCprL()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_SETDEFAULTSCPRL_ENTRY );

    CommsDat::CCDSCprRecord* scprRecord =
            static_cast<CommsDat::CCDSCprRecord*>(
                    CommsDat::CCDRecordBase::RecordFactoryL( CommsDat::KCDTIdSCprRecord ) );
    CleanupStack::PushL( scprRecord );

    scprRecord->SetRecordId( 1 );

    scprRecord->LoadL( iTransactionHandler->Session() );
    iDestApRecord->iSCpr = scprRecord->ElementId();
    CleanupStack::PopAndDestroy( scprRecord );

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_SETDEFAULTSCPRL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmDestinationStruct::SetDefaultProtocolL
// Sets the default protocol to destination access point record.
// -----------------------------------------------------------------------------
void CCmmDestinationStruct::SetDefaultProtocolL()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_SETDEFAULTPROTOCOLL_ENTRY );

    CommsDat::CCDProtocolRecord* protocolRecord =
            static_cast<CommsDat::CCDProtocolRecord*>(
                    CommsDat::CCDRecordBase::RecordFactoryL( CommsDat::KCDTIdProtocolRecord ) );
    CleanupStack::PushL( protocolRecord );

    protocolRecord->SetRecordId( 1 );

    protocolRecord->LoadL( iTransactionHandler->Session() );
    iDestApRecord->iProtocol = protocolRecord->ElementId();
    CleanupStack::PopAndDestroy( protocolRecord );

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_SETDEFAULTPROTOCOLL_EXIT );
    }

// -----------------------------------------------------------------------------
// Set attribute flag on the given record.
// -----------------------------------------------------------------------------
void CCmmDestinationStruct::SetAttribute(
        CommsDat::CCDRecordBase* aRecord,
        TUint32 aAttribute,
        TBool aSet )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_SETATTRIBUTE_ENTRY );

    if ( aSet )
        {
        aRecord->SetAttributes( aAttribute );
        }
    else
        {
        aRecord->ClearAttributes( aAttribute );
        }

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_SETATTRIBUTE_EXIT );
    }

// -----------------------------------------------------------------------------
// Creates a copy of a record of the given type.
// -----------------------------------------------------------------------------
CommsDat::CCDRecordBase* CCmmDestinationStruct::CopyRecordL(
        TCmmDbRecords aRecordType,
        CommsDat::CCDRecordBase* aSource )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONSTRUCT_COPYRECORDL_ENTRY );

    CommsDat::CCDRecordBase* target = NULL;

    switch ( aRecordType )
        {
        case ECmmDestNetworkRecord:
            {
            // Network record.

            CommsDat::CCDNetworkRecord* source =
                    static_cast<CommsDat::CCDNetworkRecord*>( aSource );
            CommsDat::CCDNetworkRecord* networkRecord =
                    static_cast<CommsDat::CCDNetworkRecord*>(
                            CommsDat::CCDRecordBase::RecordFactoryL(
                                    CommsDat::KCDTIdNetworkRecord ) );
            CleanupStack::PushL( networkRecord );

            if ( !source->iRecordTag.IsNull() )
                {
                networkRecord->iRecordTag.SetL( source->iRecordTag );
                }
            if ( !source->iRecordName.IsNull() )
                {
                networkRecord->iRecordName.SetL( source->iRecordName );
                }
            if ( !source->iHostName.IsNull() )
                {
                networkRecord->iHostName.SetL( source->iHostName );
                }
            networkRecord->SetElementId( source->ElementId() );

            CleanupStack::Pop( networkRecord );
            target = static_cast<CommsDat::CCDRecordBase*>( networkRecord );
            }
            break;
        case ECmmDestApRecord:
            {
            // Access point record.

            CommsDat::CCDAccessPointRecord* source =
                    static_cast<CommsDat::CCDAccessPointRecord*>( aSource );
            CommsDat::CCDAccessPointRecord* destApRecord =
                    static_cast<CommsDat::CCDAccessPointRecord*>(
                            CommsDat::CCDRecordBase::RecordFactoryL(
                                    CommsDat::KCDTIdAccessPointRecord ) );
            CleanupStack::PushL( destApRecord );

            if ( !source->iRecordTag.IsNull() )
                {
                destApRecord->iRecordTag.SetL( source->iRecordTag );
                }
            if ( !source->iRecordName.IsNull() )
                {
                destApRecord->iRecordName.SetL( source->iRecordName );
                }
            if ( !source->iAccessPointGID.IsNull() )
                {
                destApRecord->iAccessPointGID.SetL( source->iAccessPointGID );
                }
            if ( !source->iSelectionPolicy.IsNull() )
                {
                destApRecord->iSelectionPolicy.SetL( source->iSelectionPolicy );
                }

            if ( !source->iTier.IsNull() )
                {
                destApRecord->iTier.SetL( source->iTier );
                }
            if ( !source->iMCpr.IsNull() )
                {
                destApRecord->iMCpr.SetL( source->iMCpr );
                }
            if ( !source->iCpr.IsNull() )
                {
                destApRecord->iCpr.SetL( source->iCpr );
                }
            if ( !source->iSCpr.IsNull() )
                {
                destApRecord->iSCpr.SetL( source->iSCpr );
                }
            if ( !source->iProtocol.IsNull() )
                {
                destApRecord->iProtocol.SetL( source->iProtocol );
                }
            if ( !source->iCprConfig.IsNull() )
                {
                destApRecord->iCprConfig.SetL( source->iCprConfig );
                }
            if ( !source->iAppSID.IsNull() )
                {
                destApRecord->iAppSID.SetL( source->iAppSID );
                }
            if ( !source->iConfigAPIdList.IsNull() )
                {
                destApRecord->iConfigAPIdList.SetL( source->iConfigAPIdList );
                }
            if ( !source->iCustomSelectionPolicy.IsNull() )
                {
                destApRecord->iCustomSelectionPolicy.SetL( source->iCustomSelectionPolicy );
                }
            if ( !source->iPriority.IsNull() )
                {
                destApRecord->iPriority.SetL( source->iPriority );
                }

            destApRecord->SetElementId( aSource->ElementId() );

            CleanupStack::Pop( destApRecord );
            target = static_cast<CommsDat::CCDRecordBase*>( destApRecord );
            }
            break;
        case ECmmDestMetadataRecord:
            {
            // Metadata record.
            CCDSNAPMetadataRecord* source =
                    static_cast<CCDSNAPMetadataRecord*>( aSource );
            CCDSNAPMetadataRecord* metadataRecord =
                    new( ELeave ) CCDSNAPMetadataRecord(
                            iCache->TableId( ECmmDestMetadataRecord ) );
            CleanupStack::PushL( metadataRecord );

            if ( !source->iRecordTag.IsNull() )
                {
                metadataRecord->iRecordTag.SetL( source->iRecordTag );
                }
            if ( !source->iRecordName.IsNull() )
                {
                metadataRecord->iRecordName.SetL( source->iRecordName );
                }
            if ( !source->iSNAP.IsNull() )
                {
                metadataRecord->iSNAP.SetL( source->iSNAP );
                }
            if ( !source->iMetadata.IsNull() )
                {
                metadataRecord->iMetadata.SetL( source->iMetadata );
                }
            if ( !source->iIcon.IsNull() )
                {
                metadataRecord->iIcon.SetL( source->iIcon );
                }
            if ( !source->iIconFileName.IsNull() )
                {
                metadataRecord->iIconFileName.SetL( source->iIconFileName );
                }
            metadataRecord->SetElementId( source->ElementId() );

            CleanupStack::Pop( metadataRecord );
            target = static_cast<CommsDat::CCDRecordBase*>( metadataRecord );
            }
            break;
        default:
            {
            User::Leave( KErrArgument );
            }
            break;
        }

    OstTraceFunctionExit0( CCMMDESTINATIONSTRUCT_COPYRECORDL_EXIT );
    return target;
    }

// End of file
