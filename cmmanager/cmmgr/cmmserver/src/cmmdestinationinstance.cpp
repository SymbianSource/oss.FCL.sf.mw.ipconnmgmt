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
* Session side object representing a destination.
*
*/


#include <cmpluginbaseeng.h>
#include <datamobilitycommsdattypes.h>
#include <cmpluginembdestinationdef.h>

#include "cmmdestinationinstance.h"
#include "cmmsession.h"
#include "cmmcache.h"
#include "cmmconnmethodinstance.h"
#include "cmmconnmethoditem.h"
#include "cmmanagertextresolver.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmdestinationinstanceTraces.h"
#endif


// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CCmmDestinationInstance* CCmmDestinationInstance::NewL(
        CCmmSession* aCmmSession,
        CCmmCache* aCache )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_NEWL_ENTRY );

    CCmmDestinationInstance* self = CCmmDestinationInstance::NewLC( aCmmSession, aCache );
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_NEWL_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CCmmDestinationInstance* CCmmDestinationInstance::NewLC(
        CCmmSession* aCmmSession,
        CCmmCache* aCache )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_NEWLC_ENTRY );

    CCmmDestinationInstance* self = new( ELeave ) CCmmDestinationInstance( aCmmSession, aCache );
    CleanupClosePushL( *self );
    self->ConstructL();

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_NEWLC_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CCmmDestinationInstance::~CCmmDestinationInstance()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_CCMMDESTINATIONINSTANCE_ENTRY );

    // Tell cache this handle is beeing closed. Cache will update it's own
    // bookkeeping accordingly.
    if ( iCache )
        {
        iCache->CloseDestination( *this );
        }
    delete iNetworkRecord;
    delete iDestApRecord;
    delete iMetadataRecord;

    iConnMethodItemArray.Close();
    iConnMethodsToBeDeleted.Close();

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_CCMMDESTINATIONINSTANCE_EXIT );
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CCmmDestinationInstance::CCmmDestinationInstance(
        CCmmSession* aCmmSession,
        CCmmCache* aCache )
        :
        iCmmSession( aCmmSession ),
        iCache( aCache )
    {
    OstTraceFunctionEntry0( DUP1_CCMMDESTINATIONINSTANCE_CCMMDESTINATIONINSTANCE_ENTRY );

    iNetworkRecord = NULL;
    iDestApRecord = NULL;
    iMetadataRecord = NULL;

    iNetworkRecordStatus = ECmmRecordStatusBlank;
    iDestApRecordStatus = ECmmRecordStatusBlank;
    iMetadataRecordStatus = ECmmRecordStatusBlank;

    iStatus = ECmmDestinationStatusChanged;
    iId = 0;
    iHandle = 0;

    iProtectionChanged = EFalse;
    iLastProtectionLevel = CMManager::EProtLevel0;
    iCurrentProtectionLevel = CMManager::EProtLevel0;

    OstTraceFunctionExit0( DUP1_CCMMDESTINATIONINSTANCE_CCMMDESTINATIONINSTANCE_EXIT );
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::ConstructL()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_CONSTRUCTL_ENTRY );
    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------
// Set the destination ID.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::SetId( const TUint32 aId )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_SETID_ENTRY );

    iId = aId;

    // If this is the real ID (not the temporary ID of a new unsaved destination),
    // write it also into network record.
    if ( aId > KCmmDestIdIntervalMin && aId < KCmmDestIdIntervalMax )
        {
        iNetworkRecord->SetRecordId( aId - KCmDefaultDestinationAPTagId );
        iDestApRecord->iRecordTag = aId;
        iDestApRecord->iCustomSelectionPolicy = ( aId - KCmDefaultDestinationAPTagId );
        iMetadataRecord->iSNAP = aId;
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_SETID_EXIT );
    }

// ---------------------------------------------------------------------------
// Get the destination ID.
// ---------------------------------------------------------------------------
//
TUint32 CCmmDestinationInstance::GetId() const
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_GETID_ENTRY );
    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_GETID_EXIT );
    return iId;
    }

// ---------------------------------------------------------------------------
// Set the current status of this destination.
// ECmmDestinationStatusValid: Contents in sync with database.
// ECmmDestinationStatusChanged: Contents differ from database contents.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::SetStatus( const TCmmDestinationStatus& aStatus )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_SETSTATUS_ENTRY );

    switch ( aStatus )
        {
        // Fallthrough intended
        case ECmmDestinationStatusValid:
        case ECmmDestinationStatusChanged:
            iStatus = aStatus;
            break;
        default:
            ASSERT(0); // Error, invalid status.
            break;
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_SETSTATUS_EXIT );
    }

// ---------------------------------------------------------------------------
// Gets the current status of this destination instance.
// ---------------------------------------------------------------------------
//
TCmmDestinationStatus CCmmDestinationInstance::GetStatus() const
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_GETSTATUS_ENTRY );
    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_GETSTATUS_EXIT );
    return iStatus;
    }

// ---------------------------------------------------------------------------
// Get handle ID.
// ---------------------------------------------------------------------------
//
TInt CCmmDestinationInstance::GetHandle() const
    {
    return iHandle;
    }

// ---------------------------------------------------------------------------
// Set handle ID.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::SetHandle( const TInt aHandle )
    {
    iHandle = aHandle;
    }

// ---------------------------------------------------------------------------
// Set the record status for all records.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::SetStatusForAllRecords( const TCmmRecordStatus& aStatus )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_SETSTATUSFORALLRECORDS_ENTRY );

    iDestApRecordStatus = aStatus;
    iNetworkRecordStatus = aStatus;
    iMetadataRecordStatus = aStatus;

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_SETSTATUSFORALLRECORDS_EXIT );
    }

// ---------------------------------------------------------------------------
// Get the destination element ID.
// ---------------------------------------------------------------------------
//
TUint32 CCmmDestinationInstance::GetElementIdL()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_GETELEMENTIDL_ENTRY );

    LoadRecordIfMissingL( ECmmDestNetworkRecord );

    if ( !iNetworkRecord )
        {
        User::Leave( KErrCorrupt );
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_GETELEMENTIDL_EXIT );
    return iNetworkRecord->ElementId() & KCDMaskShowField;
    }

// ---------------------------------------------------------------------------
// Get the destination access point record's record tag (destination ID).
// ---------------------------------------------------------------------------
//
TUint32 CCmmDestinationInstance::GetRecordTagFromApRecordL()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_GETRECORDTAGFROMAPRECORDL_ENTRY );

    LoadRecordIfMissingL( ECmmDestApRecord );

    if ( !iDestApRecord )
        {
        User::Leave( KErrCorrupt );
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_GETRECORDTAGFROMAPRECORDL_EXIT );
    return iDestApRecord->iRecordTag;
    }

// ---------------------------------------------------------------------------
// Sets a record pointer to the one given as parameter, Ownership is
// transfered. Also deletes the previous pointer and sets the record status
// to 'ECmmRecordStatusLoaded'.
// ---------------------------------------------------------------------------
//
TInt CCmmDestinationInstance::SetRecordPointer(
        const TCmmDbRecords& aDestRecord,
        CommsDat::CCDRecordBase* aRecordPointer )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_SETRECORDPOINTER_ENTRY );

    TInt ret( KErrNone );
    switch ( aDestRecord )
        {
        case ECmmDestNetworkRecord:
            {
            delete iNetworkRecord;
            iNetworkRecord = static_cast<CommsDat::CCDNetworkRecord*>( aRecordPointer );
            iNetworkRecordStatus = ECmmRecordStatusLoaded;
            }
            break;
        case ECmmDestApRecord:
            {
            delete iDestApRecord;
            iDestApRecord = static_cast<CommsDat::CCDAccessPointRecord*>( aRecordPointer );
            iDestApRecordStatus = ECmmRecordStatusLoaded;
            }
            break;
        case ECmmDestMetadataRecord:
            {
            delete iMetadataRecord;
            iMetadataRecord = static_cast<CCDSNAPMetadataRecord*>( aRecordPointer );
            iMetadataRecordStatus = ECmmRecordStatusLoaded;
            }
            break;
        default:
            {
            ret = KErrNotFound;
            }
            break;
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_SETRECORDPOINTER_EXIT );
    return ret;
    }

// ---------------------------------------------------------------------------
// Gets a reference to the requested record pointer.
// ---------------------------------------------------------------------------
//
TInt CCmmDestinationInstance::GetRecordPointer(
        const TCmmDbRecords& aDestRecord,
        CommsDat::CCDRecordBase*& aRecordPointer )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_GETRECORDPOINTER_ENTRY );

    TInt ret( KErrNone );
    aRecordPointer = NULL;
    switch ( aDestRecord )
        {
        case ECmmDestNetworkRecord:
            {
            aRecordPointer = static_cast<CommsDat::CCDRecordBase*>( iNetworkRecord );
            }
            break;
        case ECmmDestApRecord:
            {
            aRecordPointer = static_cast<CommsDat::CCDRecordBase*>( iDestApRecord );
            }
            break;
        case ECmmDestMetadataRecord:
            {
            aRecordPointer = static_cast<CommsDat::CCDRecordBase*>( iMetadataRecord );
            }
            break;
        default:
            {
            ASSERT( 0 );
            ret = KErrNotFound;
            }
            break;
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_GETRECORDPOINTER_EXIT );
    return ret;
    }

// ---------------------------------------------------------------------------
// Save the contents of this destination into database. All connection methods
// including embedded destination, that the client has open handles to, are
// also updated.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::UpdateL()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_UPDATEL_ENTRY );

    if ( AllMandatoryRecordsContainData() )
        {
        // Check for metadata conflicts.
        if ( ConflictingMetadataFoundL() )
            {
            OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_UPDATEL_EXIT );

            User::Leave( KErrAlreadyExists );
            }

        // Check that the destination name is still valid and not in use already.
        if ( iCache->DestinationExistsWithNameL( iNetworkRecord->iRecordName.GetL(), iId ) )
            {
            OstTraceFunctionExit0( DUP1_CCMMDESTINATIONINSTANCE_UPDATEL_EXIT );

            User::Leave( KErrAlreadyExists );
            }

        iCache->UpdateDestinationL( *this );
        }
    else
        {
        User::Leave( KErrCorrupt );
        }

    OstTraceFunctionExit0( DUP2_CCMMDESTINATIONINSTANCE_UPDATEL_EXIT );
    }

// ---------------------------------------------------------------------------
// Get the destination name.
// ---------------------------------------------------------------------------
//
TDesC& CCmmDestinationInstance::GetDestinationNameL()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_GETDESTINATIONNAMEL_ENTRY );

    LoadRecordIfMissingL( ECmmDestNetworkRecord );

    if ( !iNetworkRecord )
        {
        OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_GETDESTINATIONNAMEL_EXIT );

        User::Leave( KErrCorrupt );
        }

    OstTraceFunctionExit0( DUP1_CCMMDESTINATIONINSTANCE_GETDESTINATIONNAMEL_EXIT );
    return iNetworkRecord->iRecordName.GetL();
    }

// ---------------------------------------------------------------------------
// Get the localised destination name.
// ---------------------------------------------------------------------------
//
HBufC* CCmmDestinationInstance::GetLocalisedDestinationNameL()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_GETLOCALISEDDESTINATIONNAMEL_ENTRY );

    LoadRecordIfMissingL( ECmmDestNetworkRecord );

    if ( !iNetworkRecord )
        {
        User::Leave( KErrCorrupt );
        }

    TUint32 purpose( 0 );
    GetMetadataL( CMManager::ESnapMetadataPurpose, purpose );
    HBufC* resolvedText( NULL );
    TBool isLocalised( EFalse );
    switch ( purpose )
        {
        case CMManager::ESnapPurposeInternet:
            {
            _LIT( KDestinationTextId, "txt_occ_dblist_internet");
            resolvedText = CCmManagerTextResolver::ResolveTextL( KDestinationTextId );
            isLocalised = ETrue;
            }
            break;
        case CMManager::ESnapPurposeOperator:
            {
            _LIT( KDestinationTextId, "txt_occ_dblist_operator_services");
            resolvedText = CCmManagerTextResolver::ResolveTextL( KDestinationTextId );
            isLocalised = ETrue;
            }
            break;
        case CMManager::ESnapPurposeIntranet:
            {
            // Intranet is handled in a special way. If localised metadata is OFF
            // destination's network record name is returned
            TUint32 localized( 0 );
            GetMetadataL( CMManager::ESnapMetadataDestinationIsLocalised, localized );
            if ( localized )
                {
                _LIT( KDestinationTextId, "txt_occ_dblist_intranet");
                resolvedText = CCmManagerTextResolver::ResolveTextL( KDestinationTextId );
                isLocalised = ETrue;
                }
            }
            break;
        default:
            break;
        }

    // Not Internet, operator or intranet, or something went wrong.
    if ( !isLocalised || ( isLocalised && !resolvedText ) )
        {
        resolvedText = TPtrC( iNetworkRecord->iRecordName ).AllocL();
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_GETLOCALISEDDESTINATIONNAMEL_EXIT );
    return resolvedText;
    }

// ---------------------------------------------------------------------------
// Set the destination name.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::SetDestinationNameL( const TDesC& aDestinationName )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_SETDESTINATIONNAMEL_ENTRY );

    // Write action, load all records.
    LoadAllRecordsL();

    iNetworkRecord->iRecordName.SetL( aDestinationName );
    iDestApRecord->iRecordName.SetL( aDestinationName );

    iNetworkRecordStatus = ECmmRecordStatusModified;
    iDestApRecordStatus = ECmmRecordStatusModified;
    iStatus = ECmmDestinationStatusChanged;

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_SETDESTINATIONNAMEL_EXIT );
    }

// ---------------------------------------------------------------------------
// Get the destination icon.
// ---------------------------------------------------------------------------
//
HBufC* CCmmDestinationInstance::GetDestinationIconL()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_GETDESTINATIONICONL_ENTRY );

    LoadRecordIfMissingL( ECmmDestMetadataRecord );

    if ( !iMetadataRecord )
        {
        User::Leave( KErrCorrupt );
        }

    HBufC* icon( NULL );
    icon = TPtrC( iMetadataRecord->iIconFileName ).AllocL();

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_GETDESTINATIONICONL_EXIT );
    return icon;
    }

// ---------------------------------------------------------------------------
// Set the destination icon.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::SetDestinationIconL( const TDesC& aDestinationIcon )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_SETDESTINATIONICONL_ENTRY );

    // Write action, load all records.
    LoadAllRecordsL();

    iMetadataRecord->iIconFileName.SetL( aDestinationIcon );

    iMetadataRecordStatus = ECmmRecordStatusModified;
    iStatus = ECmmDestinationStatusChanged;

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_SETDESTINATIONICONL_EXIT );
    }

// ---------------------------------------------------------------------------
// Get metadata of specified type.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::GetMetadataL(
        const CMManager::TSnapMetadataField& aMetadataField,
        TUint32& aMetadata )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_GETMETADATAL_ENTRY );

    LoadRecordIfMissingL( ECmmDestMetadataRecord );

    if ( !iMetadataRecord )
        {
        User::Leave( KErrCorrupt );
        }

    switch ( aMetadataField )
        {
        case CMManager::ESnapMetadataInternet:
            aMetadata = ( TUint32 )( iMetadataRecord->iMetadata ) &
                    CMManager::ESnapMetadataInternet;
            break;
        case CMManager::ESnapMetadataHighlight:
            aMetadata = ( TUint32 )( iMetadataRecord->iMetadata ) &
                    CMManager::ESnapMetadataHighlight;
            break;
        case CMManager::ESnapMetadataHiddenAgent:
            aMetadata = ( TUint32 )( iMetadataRecord->iMetadata ) &
                    CMManager::ESnapMetadataHiddenAgent;
            break;
        case CMManager::ESnapMetadataDestinationIsLocalised:
            aMetadata = ( ( TUint32 )( iMetadataRecord->iMetadata ) &
                    CMManager::ESnapMetadataDestinationIsLocalised ) >> 4;
            break;
        case CMManager::ESnapMetadataPurpose:
            aMetadata = ( ( TUint32 )( iMetadataRecord->iMetadata ) &
                    CMManager::ESnapMetadataPurpose ) >> 8;
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_GETMETADATAL_EXIT );
    }

// ---------------------------------------------------------------------------
// Set metadata of specified type.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::SetMetadataL(
        const CMManager::TSnapMetadataField& aMetadataField,
        const TUint32 aMetadata )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_SETMETADATAL_ENTRY );

    // Write action, load all records.
    LoadAllRecordsL();

    switch ( aMetadataField )
        {
        case CMManager::ESnapMetadataInternet:
            SetMetadataInternetL( aMetadata );
            break;
        case CMManager::ESnapMetadataHighlight:
            SetMetadataHighlight( aMetadata );
            break;
        case CMManager::ESnapMetadataHiddenAgent:
            SetMetadataHiddenAgentL( aMetadata );
            break;
        case CMManager::ESnapMetadataDestinationIsLocalised:
            SetMetadataLocalizationL( aMetadata );
            break;
        case CMManager::ESnapMetadataPurpose:
            SetMetadataPurposeL( aMetadata );
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }

    iMetadataRecordStatus = ECmmRecordStatusModified;
    iStatus = ECmmDestinationStatusChanged;

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_SETMETADATAL_EXIT );
    }

// ---------------------------------------------------------------------------
// Get destination protection level.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::GetProtectionL( CMManager::TProtectionLevel& aProtectionLevel )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_GETPROTECTIONL_ENTRY );

    LoadRecordIfMissingL( ECmmDestMetadataRecord );

    if ( !iMetadataRecord )
        {
        User::Leave( KErrCorrupt );
        }

    TUint32 value = ( ( TUint32 )( iMetadataRecord->iMetadata ) &
            KDestProtectionLevelMask ) >> KBitsToShiftDestProtectionLevel;
    switch ( value )
        {
        case CMManager::EProtLevel0:
            aProtectionLevel = CMManager::EProtLevel0;
            break;
        case CMManager::EProtLevel1:
            aProtectionLevel = CMManager::EProtLevel1;
            break;
        case CMManager::EProtLevel2:
            aProtectionLevel = CMManager::EProtLevel2;
            break;
        case CMManager::EProtLevel3:
            aProtectionLevel = CMManager::EProtLevel3;
            break;
        default:
            User::Leave( KErrCorrupt );
            break;
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_GETPROTECTIONL_EXIT );
    }

// ---------------------------------------------------------------------------
// Set destination protection level.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::SetProtectionL(
        CMManager::TProtectionLevel aProtectionLevel )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_SETPROTECTIONL_ENTRY );

    // Write action, load all records.
    LoadAllRecordsL();

    // Protection level is saved to CommsDat during update-call, so we need to
    // keep track of both the original value and the currently set new value.
    iCurrentProtectionLevel = aProtectionLevel;
    if ( !iProtectionChanged )
        {
        GetProtectionL( iLastProtectionLevel );
        }
    if ( aProtectionLevel == iLastProtectionLevel )
        {
        iProtectionChanged = EFalse;
        }
    else
        {
        iProtectionChanged = ETrue;
        }

    // Read current metadata without protection bits.
    TUint32 currentMetadata =
            ( ( TUint32 )( iMetadataRecord->iMetadata ) & ( ~KDestProtectionLevelMask ) );
    // Add new protection bits.
    iMetadataRecord->iMetadata =
            currentMetadata | ( aProtectionLevel << KBitsToShiftDestProtectionLevel );

    iMetadataRecordStatus = ECmmRecordStatusModified;
    iStatus = ECmmDestinationStatusChanged;

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_SETPROTECTIONL_EXIT );
    }

// ---------------------------------------------------------------------------
// Return true if protection level has been changed.
// ---------------------------------------------------------------------------
//
TBool CCmmDestinationInstance::ProtectionChanged()
    {
    return iProtectionChanged;
    }

// ---------------------------------------------------------------------------
// Return the previous valid protection level of this connection method
// instance.
// ---------------------------------------------------------------------------
//
CMManager::TProtectionLevel CCmmDestinationInstance::LastProtectionLevel()
    {
    return iLastProtectionLevel;
    }

// ---------------------------------------------------------------------------
// Return the protection level currently set into this destination instance.
// ---------------------------------------------------------------------------
//
CMManager::TProtectionLevel CCmmDestinationInstance::CurrentProtectionLevelL()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_CURRENTPROTECTIONLEVELL_ENTRY );

    CMManager::TProtectionLevel protLevel( iCurrentProtectionLevel );
    if ( !iProtectionChanged )
        {
        GetProtectionL( protLevel );
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_CURRENTPROTECTIONLEVELL_EXIT );
    return protLevel;
    }

// ---------------------------------------------------------------------------
// Adds a connection method into this destination. Returns the index (priority)
// in the connection method array where the connection method was added.
// Embedded destinations can not be added with this method, use
// AddEmbeddedDestinationL() instead.
// ---------------------------------------------------------------------------
//
TInt CCmmDestinationInstance::AddConnMethodL(
        CCmmConnMethodInstance& aConnMethodInstance )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_ADDCONNMETHODL_ENTRY );

    TUint32 connMethodId( aConnMethodInstance.GetId() );

    // Check that the connection method is not an embedded destination.
    if ( aConnMethodInstance.IsEmbeddedDestination() )
        {
        User::Leave( KErrArgument );
        }

    // Check that the connection method is not already inside this destination.
    for ( TInt i = 0; i < iConnMethodItemArray.Count(); i++ )
        {
        if ( connMethodId == iConnMethodItemArray[i].iId )
            {
            User::Leave( KErrAlreadyExists );
            }
        }

    // Remove the connection method from delete list, if found there.
    for ( TInt i = 0; i < iConnMethodsToBeDeleted.Count(); i++ )
        {
        if ( connMethodId == iConnMethodsToBeDeleted[i] )
            {
            iConnMethodsToBeDeleted.Remove( i );
            break;
            }
        }

    // Get the priority value of this connection method according to bearer.
    TUint32 bearerType( aConnMethodInstance.GetBearerType() );
    TUint bearerPriority( 0 );
    iCache->BearerPriorityFromIapRecordL(
            ( CommsDat::CCDIAPRecord* )aConnMethodInstance.GetPluginDataInstance()
                    ->iGenRecordArray[KIapRecordIndex],
            bearerPriority );
    TCmmConnMethodItem item( connMethodId, bearerType, bearerPriority, 0 );

    // Find out the correct position for the connection method inside this
    // destination.

    // If the destination contains 1 or more connection methods of the same
    // bearer type, position the new connection method after the last of these
    // connection methods.
    // If not, position the connection method before the first connection
    // method found that has an equal or greater bearer based priority.

    // The relevant connection method item count is the number of connection
    // methods excluding a possible embedded destination.
    TInt relevantCount = iConnMethodItemArray.Count();
    if ( relevantCount > 0 && iConnMethodItemArray[ relevantCount - 1 ].IsEmbedded() )
        {
        relevantCount--;
        }
    // The final position will be between 0 and relevantCount.
    TInt index( relevantCount );

    TBool positionFound( EFalse );
    TBool connMethodIsVirtual = aConnMethodInstance.GetBoolAttributeL( CMManager::ECmVirtual );
    if ( connMethodIsVirtual )
        {
        // Ask link information only if this is a virtual IAP.
        TUint32 linkedIapId = aConnMethodInstance.GetIntAttributeL( CMManager::ECmNextLayerIapId );
        if ( linkedIapId == 0 )
            {
            // If this is a virtual IAP that doesn't point to an IAP, position it at
            // the end of the list.
            item.iPriority = CMManager::KDataMobilitySelectionPolicyPriorityWildCard;
            index = relevantCount;
            positionFound = ETrue;
            }
        else
            {
            // If this is a virtual IAP that links to another IAP, position it
            // as if it was that IAP.
            TCmmConnMethodItem linkedItem;
            TInt err = iCache->GetConnMethodItem( linkedIapId, linkedItem );
            if ( err )
                {
                User::Leave( KErrArgument );
                }
            if ( linkedItem.IsVirtual() )
                {
                // This is a virtual IAP linking to another virtual IAP.
                index = relevantCount;
                positionFound = ETrue;
                }
            bearerType = linkedItem.iBearerType;
            bearerPriority = linkedItem.iBearerPriority;
            }
        }

    if ( !positionFound )
        {
        // Search for any connection methods with the same bearer type.
        for ( TInt i = 0; i < relevantCount; i++ )
            {
            if ( iConnMethodItemArray[i].iBearerType == bearerType )
                {
                index = i + 1;
                positionFound = ETrue;
                // Don't break, need find last item.
                }
            }
        }
    if ( !positionFound )
        {
        // No connection method found with the same bearer type. Position
        // the connection method according to bearer priority.
        for ( TInt i = 0; i < relevantCount; i++ )
            {
            if ( iConnMethodItemArray[i].iBearerPriority >= bearerPriority )
                {
                index = i;
                break;
                }
            }
        }

    // Add the connection method item into this destination at the correct
    // position (priority).
    iConnMethodItemArray.InsertL( item, index );

    SetStatus( ECmmDestinationStatusChanged );

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_ADDCONNMETHODL_EXIT );
    return index;
    }

// ---------------------------------------------------------------------------
// Adds an embedded destination into this destination. Returns the index in
// the connection method array where the connection method was added.
// ---------------------------------------------------------------------------
//
TInt CCmmDestinationInstance::AddEmbeddedDestinationL(
        const CCmmDestinationInstance& aEmbeddedDestinationInstance )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_ADDEMBEDDEDDESTINATIONL_ENTRY );

    TUint32 embeddedDestinationId( aEmbeddedDestinationInstance.GetId() );

    // Check that not adding itself.
    if ( embeddedDestinationId == iId )
        {
        User::Leave( KErrArgument );
        }

    // Check that the embedded destination is not already an embedded
    // destination in this destination.
    if ( HasEmbeddedWithId( embeddedDestinationId ) )
        {
        User::Leave( KErrAlreadyExists );
        }

    // A destination can not be added into a destination that is itself an
    // embedded destination.
    if ( iCache->DestinationIsEmbedded( iId ) )
        {
        User::Leave( KErrNotSupported );
        }

    // A destination can not be added into a destination that already has an
    // embedded destination.
    if ( HasEmbedded() || iCache->DestinationHasEmbedded( iId ) )
        {
        User::Leave( KErrNotSupported );
        }

    // A destination that already has an embedded destination can not be added
    // into a destination.
    if ( aEmbeddedDestinationInstance.HasEmbedded() ||
            iCache->DestinationHasEmbedded( embeddedDestinationId ) )
        {
        User::Leave( KErrNotSupported );
        }

    // Check session side destination instances for all clients.
    // - Check that any destination instance does not contain aDestinationId as
    //   embedded destination.
    // - Check that any destination instance for aEmbeddedDestinationId does
    //   not contain an embedded destination.
    if ( iCmmSession->EmbeddedDestinationConflictsFromAllSessions( iId, embeddedDestinationId ) )
        {
        User::Leave( KErrAlreadyExists );
        }

    // Add the embedded destination at the end of the connection method item array.
    TInt index = iConnMethodItemArray.Count();
    TCmmConnMethodItem item(
            embeddedDestinationId,
            KUidEmbeddedDestination,
            CMManager::KDataMobilitySelectionPolicyPriorityWildCard,
            CMManager::KDataMobilitySelectionPolicyPriorityWildCard );
    iConnMethodItemArray.AppendL( item );

    SetStatus( ECmmDestinationStatusChanged );

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_ADDEMBEDDEDDESTINATIONL_EXIT );
    return index;
    }

// ---------------------------------------------------------------------------
// Delete a connection method that belongs to this destination. If the
// connection method belongs to any other destination, it is only removed from
// this destination and not deleted. Does not call update.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::DeleteConnMethodFromDestinationL(
        const CCmmConnMethodInstance& aConnMethodInstance )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_DELETECONNMETHODFROMDESTINATIONL_ENTRY );

    // Check that the connection method is inside this destination.
    TUint32 connMethodId( aConnMethodInstance.GetId() );
    TInt index( KErrNotFound );
    for ( TInt i = 0; i < iConnMethodItemArray.Count(); i++ )
        {
        if ( connMethodId == iConnMethodItemArray[i].iId )
            {
            index = i;
            break;
            }
        }
    if ( index < 0 )
        {
        User::Leave( KErrArgument );
        }

    // Check if the connection method removal is blocked by any virtual IAPs.
    if ( ConnMethodInDestinationButLocked( connMethodId ) )
        {
        User::Leave( KErrLocked );
        }

    // Check if any virtual IAP points to this connection method, thus blocking
    // the delete.
    if ( iCache->ConnMethodPointedToByVirtualIap( connMethodId ) )
        {
        User::Leave( KErrLocked );
        }

    // Check that the connection method is not in use by an active connection.
    if ( iCache->CheckIfCmConnected( connMethodId ) )
        {
        User::Leave( KErrInUse );
        }

    // This will also remove an embedded destination.
    iConnMethodItemArray.Remove( index );
    SetStatus( ECmmDestinationStatusChanged );

    // If this connection method is an embedded destination, then removing it
    // from this destination is enough. The destination is not deleted.
    if ( !aConnMethodInstance.IsEmbeddedDestination() )
        {
        // Add connection method to delete list only if not referenced from any
        // other destination (in database or in any destination handles for the
        // client currently beeing served).
        if ( !iCache->ConnMethodInOtherDestination( connMethodId, iId ) &&
                !iCmmSession->ConnMethodInOtherDestination( connMethodId, iId ) )
            {
            if ( iConnMethodsToBeDeleted.Find( connMethodId ) == KErrNotFound )
                {
                iConnMethodsToBeDeleted.AppendL( connMethodId );
                }
            }
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_DELETECONNMETHODFROMDESTINATIONL_EXIT );
    }

// ---------------------------------------------------------------------------
// Remove a connection method from this destination. If the second attribute
// aTestIfConnected is false, the test to check if connection method is in use
// is skipped. In this case this should have been already tested by the caller.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::RemoveConnMethodFromDestinationL(
        const CCmmConnMethodInstance& aConnMethodInstance,
        const TBool aTestIfConnected )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_REMOVECONNMETHODFROMDESTINATIONL_ENTRY );

    // Check that the connection method is inside this destination.
    TUint32 connMethodId( aConnMethodInstance.GetId() );
    TInt index( KErrNotFound );
    for ( TInt i = 0; i < iConnMethodItemArray.Count(); i++ )
        {
        if ( connMethodId == iConnMethodItemArray[i].iId )
            {
            index = i;
            break;
            }
        }
    if ( index < 0 )
        {
        User::Leave( KErrArgument );
        }

    // Check if the connection method removal is blocked by any virtual IAPs.
    if ( ConnMethodInDestinationButLocked( connMethodId ) )
        {
        User::Leave( KErrLocked );
        }

    if ( aTestIfConnected )
        {
        // Check that the connection method is not in use by an active connection.
        if ( iCache->CheckIfCmConnected( connMethodId ) )
            {
            User::Leave( KErrInUse );
            }
        }

    // This will also remove an embedded destination.
    iConnMethodItemArray.Remove( index );
    SetStatus( ECmmDestinationStatusChanged );

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_REMOVECONNMETHODFROMDESTINATIONL_EXIT );
    }

// ---------------------------------------------------------------------------
// Modifies the priority of a connection method inside this destination.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::ModifyConnMethodPriorityL(
        CCmmConnMethodInstance& aConnMethodInstance,
        TUint aIndex )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_MODIFYCONNMETHODPRIORITYL_ENTRY );

    // Check index range.
    if ( aIndex >= iConnMethodItemArray.Count() )
        {
        User::Leave( KErrArgument );
        }

    // Check if the connection method is in this destination. Also store the
    // current index of the connection method.
    TInt oldIndex( KErrNotFound );
    for ( TInt i = 0; i < iConnMethodItemArray.Count(); i++ )
        {
        if ( iConnMethodItemArray[i].iId == aConnMethodInstance.GetId() )
            {
            oldIndex = i;
            break;
            }
        }
    if ( oldIndex == KErrNotFound )
        {
        User::Leave( KErrArgument );
        }

    // If connection method is an embedded destination, just return silently.
    if ( aConnMethodInstance.IsEmbeddedDestination() )
        {
        OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_MODIFYCONNMETHODPRIORITYL_EXIT );
        return;
        }

    // If connection method is a virtual IAP linking to a destination, just return silently.
    if ( aConnMethodInstance.GetBoolAttributeL( CMManager::ECmVirtual ) &&
            aConnMethodInstance.GetIntAttributeL( CMManager::ECmNextLayerIapId ) == 0 )
        {
        OstTraceFunctionExit0( DUP1_CCMMDESTINATIONINSTANCE_MODIFYCONNMETHODPRIORITYL_EXIT );
        return;
        }

    // If the given index points to any wildcard priority connection method,
    // adjust it so it points to the last priority position.
    TUint numberOfConnMethodsWithPriority( NumberOfConnMethodsWithPriority() );
    if ( aIndex >= numberOfConnMethodsWithPriority )
        {
        if ( numberOfConnMethodsWithPriority == 0 )
            {
            // Can't be 0, since aConnMethodInstance is not a wildcard IAP and
            // is in the array.
            ASSERT( 0 );
            User::Leave( KErrCorrupt );
            }
        aIndex = numberOfConnMethodsWithPriority - 1;
        }

    // Check if the priority is changing from current.
    if ( oldIndex == aIndex )
        {
        OstTraceFunctionExit0( DUP2_CCMMDESTINATIONINSTANCE_MODIFYCONNMETHODPRIORITYL_EXIT );
        return;
        }

    // Change the location of the connection method to reflect the new priority.
    TCmmConnMethodItem item = iConnMethodItemArray[oldIndex];
    iConnMethodItemArray.Remove( oldIndex );
    iConnMethodItemArray.InsertL( item, ( TInt )aIndex );

    SetStatus( ECmmDestinationStatusChanged );

    OstTraceFunctionExit0( DUP3_CCMMDESTINATIONINSTANCE_MODIFYCONNMETHODPRIORITYL_EXIT );
    }

// ---------------------------------------------------------------------------
// Finds a connection method instance that belongs to the same session and
// matches the provided ID. Return NULL if no match is found.
// ---------------------------------------------------------------------------
//
CCmmConnMethodInstance* CCmmDestinationInstance::FindConnMethodInstanceFromSessionById(
        const TUint32 aConnMethodId ) const
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_FINDCONNMETHODINSTANCEFROMSESSIONBYID_ENTRY );

    CCmmConnMethodInstance* connMethodInstance = iCmmSession->
            FindConnMethodInstanceById( aConnMethodId );

    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_FINDCONNMETHODINSTANCEFROMSESSIONBYID_EXIT );
    return connMethodInstance;
    }

// ---------------------------------------------------------------------------
// Finds a destination instance that belongs to the same session and matches
// the provided ID. Return NULL if no match is found.
// ---------------------------------------------------------------------------
//
CCmmDestinationInstance* CCmmDestinationInstance::FindDestinationInstanceFromSessionById(
        const TUint32 aDestinationId ) const
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_FINDDESTINATIONINSTANCEFROMSESSIONBYID_ENTRY );

    CCmmDestinationInstance* destInstance =
            iCmmSession->FindDestinationInstanceById( aDestinationId );

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_FINDDESTINATIONINSTANCEFROMSESSIONBYID_EXIT );
    return destInstance;
    }

// ---------------------------------------------------------------------------
// Check from all open destination handles in the same session if the given
// connection method is inside any of them. The given destination is skipped.
// ---------------------------------------------------------------------------
//
TBool CCmmDestinationInstance::ConnMethodInOtherDestinationInSession(
        const TUint32 aConnMethodId,
        const TUint32 aDestinationId ) const
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_CONNMETHODINOTHERDESTINATIONINSESSION_ENTRY );

    TBool inOtherSession = iCmmSession->
            ConnMethodInOtherDestination( aConnMethodId, aDestinationId );

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_CONNMETHODINOTHERDESTINATIONINSESSION_EXIT );
    return inOtherSession;
    }

// ---------------------------------------------------------------------------
// Check if the given connection method is in this destination. Include
// embedded destinations.
// ---------------------------------------------------------------------------
//
TBool CCmmDestinationInstance::ValidConnMethodIdInDestinationIncludeEmbedded(
        const TUint32 aConnMethodId ) const
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_VALIDCONNMETHODIDINDESTINATIONINCLUDEEMBEDDED_ENTRY );

    TBool validity( EFalse );
    if ( aConnMethodId > 0 )
        {
        for ( TInt i = 0; i < iConnMethodItemArray.Count(); i++ )
            {
            if ( iConnMethodItemArray[i].iId == aConnMethodId )
                {
                validity = ETrue;
                break;
                }
            }
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_VALIDCONNMETHODIDINDESTINATIONINCLUDEEMBEDDED_EXIT );
    return validity;
    }

// ---------------------------------------------------------------------------
// Check if the given connection method is inside this destination and if it
// is in a 'locked' status. A locked connection method can't be deleted nor
// removed from the destination. A connection method becomes locked if it is
// the only remaining connection method in a destination and a virtual IAP
// points into that destination.
// ---------------------------------------------------------------------------
//
TBool CCmmDestinationInstance::ConnMethodInDestinationButLocked(
        const TUint32 aConnMethodId ) const
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_CONNMETHODINDESTINATIONBUTLOCKED_ENTRY );

    TBool result( EFalse );
    if ( iConnMethodItemArray.Count() == 1 )
        {
        if ( iConnMethodItemArray[0].iId == aConnMethodId )
            {
            if ( iCache->DestinationPointedToByVirtualIap( iId ) )
                {
                result = ETrue;
                }
            }
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_CONNMETHODINDESTINATIONBUTLOCKED_EXIT );
    return result;
    }

// ---------------------------------------------------------------------------
// Counts how many connection methods that require priority information are
// currently in this destination. This excludes any virtual IAPs that point to
// a destination or any embedded destinations from the count. Assumes the
// connection method array is in order so all wildcard priority IAPs are at the
// end of the list.
// ---------------------------------------------------------------------------
//
TUint CCmmDestinationInstance::NumberOfConnMethodsWithPriority() const
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_NUMBEROFCONNMETHODSWITHPRIORITY_ENTRY );

    TUint count( iConnMethodItemArray.Count() );

    for ( TUint i = 0; i < iConnMethodItemArray.Count(); i++ )
        {
        if ( iConnMethodItemArray[i].IsEmbedded() || iConnMethodItemArray[i].iPriority ==
                CMManager::KDataMobilitySelectionPolicyPriorityWildCard )
            {
            count--;
            }
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_NUMBEROFCONNMETHODSWITHPRIORITY_EXIT );
    return count;
    }

// ---------------------------------------------------------------------------
// Check if this destination has an embedded destination. Checks the current
// (possibly unsaved) status of this destination handle, not the current status
// in database.
// ---------------------------------------------------------------------------
//
TBool CCmmDestinationInstance::HasEmbedded() const
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_HASEMBEDDED_ENTRY );

    TBool result( EFalse );

    // If there is an embedded destination, it will be at the very last
    // position in the connection method item array.
    TInt index = iConnMethodItemArray.Count() - 1;
    if ( index >= 0 )
        {
        if ( iConnMethodItemArray[index].IsEmbedded() )
            {
            result = ETrue;
            }
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_HASEMBEDDED_EXIT );
    return result;
    }

// ---------------------------------------------------------------------------
// Check if this destination has an embedded destination with the specified ID.
// Checks the current (possibly unsaved) status of this destination handle, not
// the current status in database.
// ---------------------------------------------------------------------------
//
TBool CCmmDestinationInstance::HasEmbeddedWithId( const TUint32 aDestinationId ) const
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_HASEMBEDDEDWITHID_ENTRY );

    TBool result( EFalse );

    // If there is an embedded destination, it will be at the very last
    // position in the connection method item array.
    TInt index = iConnMethodItemArray.Count() - 1;
    if ( index >= 0 )
        {
        // No need to check bearer, since ID will not match anyway if the last
        // item is not an embedded destination.
        if ( iConnMethodItemArray[index].iId == aDestinationId )
            {
            result = ETrue;
            }
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_HASEMBEDDEDWITHID_EXIT );
    return result;
    }

// ---------------------------------------------------------------------------
// After update/delete to database, refresh temporary ID to real ID if
// necessary and refresh status information for any related handles for
// all client sessions.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::RefreshHandlesForAllSessions( const TCmmIdStruct& aIdStruct )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_REFRESHHANDLESFORALLSESSIONS_ENTRY );

    iCmmSession->RefreshHandlesForAllSessions( aIdStruct );

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_REFRESHHANDLESFORALLSESSIONS_EXIT );
    }

// ---------------------------------------------------------------------------
// If the destination contains a connection method with matching temporary ID,
// updates the ID to real ID. This is done after the connection method has been
// successfully saved into the database.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::RefreshConnMethodId( const TCmmIdStruct& aIdStruct )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_REFRESHCONNMETHODID_ENTRY );

    for ( TInt i = 0; i < iConnMethodItemArray.Count(); i++ )
        {
        if ( iConnMethodItemArray[i].iId == aIdStruct.iTemporaryId )
            {
            iConnMethodItemArray[i].iId = aIdStruct.iRealId;
            break;
            }
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_REFRESHCONNMETHODID_EXIT );
    }

// ---------------------------------------------------------------------------
// Called after this destination has been updated and database transaction has
// completed successfully. Sets the internal state of this destination instance
// to reflect the new valid state.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::UpdateSuccessful()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_UPDATESUCCESSFUL_ENTRY );

    SetStatus( ECmmDestinationStatusValid );
    SetStatusForAllRecords( ECmmRecordStatusLoaded );
    iConnMethodsToBeDeleted.Reset();
    iProtectionChanged = EFalse;

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_UPDATESUCCESSFUL_EXIT );
    }

// ---------------------------------------------------------------------------
// Checks if all mandatory records contain data and are ready to be saved into
// database.
// ---------------------------------------------------------------------------
//
TBool CCmmDestinationInstance::AllMandatoryRecordsContainData() const
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_ALLMANDATORYRECORDSCONTAINDATA_ENTRY );

    TBool result( ETrue );

    if ( iNetworkRecordStatus != ECmmRecordStatusLoaded &&
            iNetworkRecordStatus != ECmmRecordStatusModified )
        {
        result = EFalse;
        }
    if ( iDestApRecordStatus != ECmmRecordStatusLoaded &&
            iDestApRecordStatus != ECmmRecordStatusModified )
        {
        result = EFalse;
        }
    if ( iMetadataRecordStatus != ECmmRecordStatusLoaded &&
            iMetadataRecordStatus != ECmmRecordStatusModified )
        {
        result = EFalse;
        }
    if ( !iNetworkRecord || !iDestApRecord || !iMetadataRecord )
        {
        result = EFalse;
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_ALLMANDATORYRECORDSCONTAINDATA_EXIT );
    return result;
    }

// ---------------------------------------------------------------------------
// Loads a requested type of record from database if it is not yet loaded.
//
// Currently all records are loaded when client opens a handle to a
// destination, but if future optimizations are added and records are only
// loaded when needed, the functionality of this method becomes essential.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::LoadRecordIfMissingL( TCmmDbRecords aRecordType )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_REFRESHRECORDL_ENTRY );

    TCmmRecordStatus status( ECmmRecordStatusBlank );
    switch ( aRecordType )
        {
        case ECmmDestNetworkRecord:
            status = iNetworkRecordStatus;
            break;
        case ECmmDestApRecord:
            status = iDestApRecordStatus;
            break;
        case ECmmDestMetadataRecord:
            status = iMetadataRecordStatus;
            break;
        default:
            User::Leave( KErrCorrupt ); // Error, unknown type.
            break;
        }

    switch ( status )
        {
        // Fallthrough intended
        case ECmmRecordStatusBlank:
            iCache->LoadDestinationRecordL( *this, aRecordType );
            break;
        case ECmmRecordStatusLoaded:
        case ECmmRecordStatusModified:
            // Record is up-to-date.
            break;
        case ECmmRecordStatusExpired:
        case ECmmRecordStatusUnsaved:
        default:
            User::Leave( KErrCorrupt ); // Error, invalid or unknown status.
            break;
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_REFRESHRECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// Loads all records from database that are not up-to-date.
// Guarantees the record pointers are valid or leaves.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::LoadAllRecordsL()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_LOADALLRECORDSL_ENTRY );

    LoadRecordIfMissingL( ECmmDestNetworkRecord );
    LoadRecordIfMissingL( ECmmDestApRecord );
    LoadRecordIfMissingL( ECmmDestMetadataRecord );

    if ( !iNetworkRecord || !iDestApRecord || !iMetadataRecord )
        {
        User::Leave( KErrCorrupt );
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_LOADALLRECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// Set metadata of type ESnapMetadataInternet.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::SetMetadataInternetL( const TUint32 aMetadata )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_SETMETADATAINTERNETL_ENTRY );

    if ( aMetadata )
        {
        if ( iCache->DestinationExistsWithMetadataLocalizedL(
                *this,
                CMManager::ELocalisedDestInternet ) )
            {
            User::Leave( KErrAlreadyExists );
            }
        // Clear ESnapMetadataHiddenAgent-bit.
        TUint32 value = iMetadataRecord->iMetadata & ( ~CMManager::ESnapMetadataHiddenAgent );

        // Set old ESnapMetadataInternet-bit.
        value = value | CMManager::ESnapMetadataInternet;

        // Clear ESnapMetadataDestinationIsLocalised-bits.
        value = value & ( ~CMManager::ESnapMetadataDestinationIsLocalised );
        // Set ELocalisedDestInternet-value.
        iMetadataRecord->iMetadata = value | ( CMManager::ELocalisedDestInternet << 4 );
        value = value | ( CMManager::ELocalisedDestInternet << 4 );

        // Clear ESnapMetadataPurpose-bits.
        value = value & ( ~CMManager::ESnapMetadataPurpose );
        // Set ESnapPurposeInternet-value.
        iMetadataRecord->iMetadata = value | ( CMManager::ESnapPurposeInternet << 8 );
        }
    else
        {
        // Clear old ESnapMetadataInternet-bit,
        // ESnapMetadataDestinationIsLocalised-bits and CMManager::ESnapMetadataPurpose-bits.
        TUint32 temp = ~( CMManager::ESnapMetadataInternet |
                CMManager::ESnapMetadataDestinationIsLocalised |
                CMManager::ESnapMetadataPurpose );
        iMetadataRecord->iMetadata = iMetadataRecord->iMetadata & temp;
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_SETMETADATAINTERNETL_EXIT );
    }

// ---------------------------------------------------------------------------
// Set metadata of type ESnapMetadataHighlight.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::SetMetadataHighlight( const TUint32 aMetadata )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_SETMETADATAHIGHLIGHTL_ENTRY );

    // TODO: Old CmManager does not perform any duplicate checks when setting highlight.
    // Now with single click this is not used anymore anyway. So I suggest we don't add any checks here.
    if ( aMetadata )
        {
        // Set ESnapMetadataHighlight-bit.
        iMetadataRecord->iMetadata =
                iMetadataRecord->iMetadata | CMManager::ESnapMetadataHighlight;
        }
    else
        {
        // Clear ESnapMetadataHighlight-bit.
        iMetadataRecord->iMetadata =
                iMetadataRecord->iMetadata & ( ~CMManager::ESnapMetadataHighlight );
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_SETMETADATAHIGHLIGHTL_EXIT );
    }

// ---------------------------------------------------------------------------
// Set metadata of type ESnapMetadataHiddenAgent.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::SetMetadataHiddenAgentL( const TUint32 aMetadata )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_SETMETADATAHIDDENAGENTL_ENTRY );

    if ( aMetadata )
        {
        TUint32 metadataInternet( iMetadataRecord->iMetadata &
                CMManager::ESnapMetadataInternet );
        TUint32 metadataLocalized( ( iMetadataRecord->iMetadata &
                CMManager::ESnapMetadataDestinationIsLocalised ) >> 4 );
        TUint32 metadataPurpose( ( iMetadataRecord->iMetadata &
                CMManager::ESnapMetadataPurpose ) >> 8 );
        if ( metadataInternet ||
                ( metadataLocalized == CMManager::ELocalisedDestInternet ) ||
                ( metadataPurpose == CMManager::ESnapPurposeInternet ) )
            {
            // Not allowed to set ESnapMetadataHiddenAgent if destination is
            // localized or has purpose set.
            User::Leave( KErrArgument );
            }
        // Set ESnapMetadataHiddenAgent-bit.
        iMetadataRecord->iMetadata =
                iMetadataRecord->iMetadata | CMManager::ESnapMetadataHiddenAgent;
        }
    else
        {
        // Clear ESnapMetadataHiddenAgent-bit.
        iMetadataRecord->iMetadata =
                iMetadataRecord->iMetadata & ( ~CMManager::ESnapMetadataHiddenAgent );
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_SETMETADATAHIDDENAGENTL_EXIT );
    }

// ---------------------------------------------------------------------------
// Set metadata of type ESnapMetadataDestinationIsLocalised.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::SetMetadataLocalizationL( const TUint32 aMetadata )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_SETMETADATALOCALIZATIONL_ENTRY );

    // Check that aMetadata is within valid range.
    if ( aMetadata > CMManager::ELocalisedDestIntranet )
        {
        User::Leave( KErrArgument );
        }

    if ( aMetadata == CMManager::ENotLocalisedDest )
        {
        // Clear old ESnapMetadataDestinationIsLocalised-bits.
        iMetadataRecord->iMetadata = iMetadataRecord->iMetadata & ~( CMManager::ESnapMetadataDestinationIsLocalised );
        }
    else
        {
        if ( iCache->DestinationExistsWithMetadataLocalizedL( *this, aMetadata ) )
            {
            User::Leave( KErrAlreadyExists );
            }

        TUint32 temp = ~(
                CMManager::ESnapMetadataInternet |
                CMManager::ESnapMetadataDestinationIsLocalised |
                CMManager::ESnapMetadataPurpose );

        // Clear old ESnapMetadataInternet-bit,
        // ESnapMetadataDestinationIsLocalised-bits and ESnapMetadataPurpose-bits.
        TUint32 value = iMetadataRecord->iMetadata & temp;
        switch ( aMetadata )
            {
            case CMManager::ELocalisedDestInternet:
                {
                // Clear ESnapMetadataHiddenAgent-bit.
                value = value & ( ~CMManager::ESnapMetadataHiddenAgent );
                // Set old ESnapMetadataInternet-bit.
                value = value | CMManager::ESnapMetadataInternet;
                // Set ESnapPurposeInternet-value.
                value = value | ( CMManager::ESnapPurposeInternet << 8 );
                }
                break;
            case CMManager::ELocalisedDestWap:
                {
                // Set ESnapPurposeOperator-value.
                value = value | ( CMManager::ESnapPurposeOperator << 8 );
                }
                break;
            case CMManager::ELocalisedDestMMS:
                {
                // Set ESnapMetadataHiddenAgent-bit.
                value = value | CMManager::ESnapMetadataHiddenAgent;
                // Set ESnapPurposeMMS-value.
                value = value | ( CMManager::ESnapPurposeMMS << 8 );
                }
                break;
            case CMManager::ELocalisedDestIntranet:
                {
                // Set ESnapPurposeIntranet-value.
                value = value | ( CMManager::ESnapPurposeIntranet << 8 );
                }
                break;
            default:
                User::Leave( KErrArgument );
                break;
            }
        // Set ESnapMetadataDestinationIsLocalised-value.
        iMetadataRecord->iMetadata = value | ( aMetadata << 4 );
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_SETMETADATALOCALIZATIONL_EXIT );
    }

// ---------------------------------------------------------------------------
// Set metadata of type ESnapMetadataPurpose.
// ---------------------------------------------------------------------------
//
void CCmmDestinationInstance::SetMetadataPurposeL( const TUint32 aMetadata )
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_SETMETADATAPURPOSEL_ENTRY );

    // Check that aMetadata is within valid range.
    if ( aMetadata > CMManager::ESnapPurposeIntranet )
        {
        User::Leave( KErrArgument );
        }

    TUint32 temp = ~(
            CMManager::ESnapMetadataInternet |
            CMManager::ESnapMetadataDestinationIsLocalised |
            CMManager::ESnapMetadataPurpose );
    if ( aMetadata == CMManager::ESnapPurposeUnknown )
        {
        // Clear old ESnapMetadataInternet-bit,
        // ESnapMetadataDestinationIsLocalised-bits and ESnapMetadataPurpose-bits.
        iMetadataRecord->iMetadata = iMetadataRecord->iMetadata & temp;
        }
    else
        {
        if ( iCache->DestinationExistsWithMetadataPurposeL( *this, aMetadata ) )
            {
            User::Leave( KErrAlreadyExists );
            }

        // Clear old ESnapMetadataInternet-bit,
        // ESnapMetadataDestinationIsLocalised-bits and ESnapMetadataPurpose-bits.
        TUint32 value = iMetadataRecord->iMetadata & temp;

        switch ( aMetadata )
            {
            case CMManager::ESnapPurposeInternet:
                {
                // Clear ESnapMetadataHiddenAgent-bit.
                value = value & ( ~CMManager::ESnapMetadataHiddenAgent );
                // Set old ESnapMetadataInternet-bit.
                value = value | CMManager::ESnapMetadataInternet;
                // Set ELocalisedDestInternet-value.
                value = value | ( CMManager::ELocalisedDestInternet << 4 );
                }
                break;
            case CMManager::ESnapPurposeOperator:
                {
                // Set ELocalisedDestWap-value.
                value = value | ( CMManager::ELocalisedDestWap << 4 );
                }
                break;
            case CMManager::ESnapPurposeMMS:
                {
                // Set ESnapMetadataHiddenAgent-bit.
                value = value | CMManager::ESnapMetadataHiddenAgent;
                // Set ELocalisedDestMMS-value.
                value = value | ( CMManager::ELocalisedDestMMS << 4 );
                }
                break;
            case CMManager::ESnapPurposeIntranet:
                {
                // Set ELocalisedDestIntranet-value.
                value = value | ( CMManager::ELocalisedDestIntranet << 4 );
                }
                break;
            default:
                User::Leave( KErrArgument );
                break;
            }
        // Set ESnapMetadataPurpose-value.
        iMetadataRecord->iMetadata = value | ( aMetadata << 8 );
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_SETMETADATAPURPOSEL_EXIT );
    }

// ---------------------------------------------------------------------------
// Checks the current metadata for this destination, and verifies there is no
// other conflicting destinations in the database.
// ---------------------------------------------------------------------------
//
TBool CCmmDestinationInstance::ConflictingMetadataFoundL()
    {
    OstTraceFunctionEntry0( CCMMDESTINATIONINSTANCE_CONFLICTINGMETADATAFOUNDL_ENTRY );

    TBool result( EFalse );
    TUint32 metadataInternet( iMetadataRecord->iMetadata &
            CMManager::ESnapMetadataInternet );
    TUint32 metadataLocalized( ( iMetadataRecord->iMetadata &
            CMManager::ESnapMetadataDestinationIsLocalised ) >> 4 );
    TUint32 metadataPurpose( ( iMetadataRecord->iMetadata &
            CMManager::ESnapMetadataPurpose ) >> 8 );

    if ( metadataPurpose )
        {
        // Some metadata is set, need to check conflicts in database.
        switch ( metadataPurpose )
            {
            // Fallthrough intended.
            case CMManager::ESnapPurposeInternet:
            case CMManager::ESnapPurposeOperator:
            case CMManager::ESnapPurposeMMS:
            case CMManager::ESnapPurposeIntranet:
                result = iCache->DestinationExistsWithMetadataPurposeL(
                        *this,
                        metadataPurpose );
                break;
            default:
                User::Leave( KErrCorrupt ); // Invalid metadata.
                break;
            }
        }
    else if ( metadataLocalized )
        {
        // Error, metadataPurpose was not set. Continue anyway.
        switch ( metadataLocalized )
            {
            // Fallthrough intended.
            case CMManager::ELocalisedDestInternet:
            case CMManager::ELocalisedDestWap:
            case CMManager::ELocalisedDestMMS:
            case CMManager::ELocalisedDestIntranet:
                result = iCache->DestinationExistsWithMetadataLocalizedL(
                        *this,
                        metadataLocalized );
                break;
            default:
                User::Leave( KErrCorrupt ); // Invalid metadata.
                break;
            }
        }
    else if ( metadataInternet )
        {
        // Error, metadataPurpose and metadataLocalized was not set. Continue anyway.
        result = iCache->DestinationExistsWithMetadataLocalizedL(
                *this,
                CMManager::ELocalisedDestInternet );
        }

    OstTraceFunctionExit0( CCMMDESTINATIONINSTANCE_CONFLICTINGMETADATAFOUNDL_EXIT );
    return result;
    }

// End of file
