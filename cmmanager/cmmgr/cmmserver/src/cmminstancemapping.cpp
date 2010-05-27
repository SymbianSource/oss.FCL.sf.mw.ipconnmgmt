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
* Keeps track of the general destination/connection method structure in
* database.
*
*/


#include <metadatabase.h>
#include <commsdattypesv1_1.h>
#include <datamobilitycommsdattypes.h>
#include <cmpluginembdestinationdef.h>
#include <wlancontainer.h>

#include "cmminstancemapping.h"
#include "cmpluginbaseeng.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmminstancemappingTraces.h"
#endif


// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CDestination* CDestination::NewL()
    {
    OstTraceFunctionEntry0( CDESTINATION_NEWL_ENTRY );

    CDestination* self = CDestination::NewLC();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CDESTINATION_NEWL_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CDestination* CDestination::NewLC()
    {
    OstTraceFunctionEntry0( CDESTINATION_NEWLC_ENTRY );

    CDestination* self = new( ELeave ) CDestination();
    CleanupStack::PushL( self );
    self->ConstructL();

    OstTraceFunctionExit0( CDESTINATION_NEWLC_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CDestination::~CDestination()
    {
    OstTraceFunctionEntry0( CDESTINATION_CDESTINATION_ENTRY );

    iConnMethodItemArray.Close();
    iUnsupportedConnMethods.Close();

    OstTraceFunctionExit0( CDESTINATION_CDESTINATION_EXIT );
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CDestination::CDestination()
    {
    OstTraceFunctionEntry0( DUP1_CDESTINATION_CDESTINATION_ENTRY );

    iId = 0;

    OstTraceFunctionExit0( DUP1_CDESTINATION_CDESTINATION_EXIT );
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CDestination::ConstructL()
    {
    OstTraceFunctionEntry0( CDESTINATION_CONSTRUCTL_ENTRY );
    OstTraceFunctionExit0( CDESTINATION_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CCmmInstanceMapping* CCmmInstanceMapping::NewL( CCmmCache& aCache )
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_NEWL_ENTRY );

    CCmmInstanceMapping* self = CCmmInstanceMapping::NewLC( aCache );
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_NEWL_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CCmmInstanceMapping* CCmmInstanceMapping::NewLC( CCmmCache& aCache )
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_NEWLC_ENTRY );

    CCmmInstanceMapping* self = new( ELeave ) CCmmInstanceMapping( aCache );
    CleanupStack::PushL( self );
    self->ConstructL();

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_NEWLC_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CCmmInstanceMapping::~CCmmInstanceMapping()
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_CCMMINSTANCEMAPPING_ENTRY );

    iConnMethodItemArray.Close();
    iUnsupportedConnMethods.Close();
    iDestinations.ResetAndDestroy();
    iDeletedConnMethods.Close();
    iDeletedDestinations.Close();

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_CCMMINSTANCEMAPPING_EXIT );
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CCmmInstanceMapping::CCmmInstanceMapping( CCmmCache& aCache ) : iCache( aCache )
    {
    OstTraceFunctionEntry0( DUP1_CCMMINSTANCEMAPPING_CCMMINSTANCEMAPPING_ENTRY );
    iEasyWlanId = 0;
    OstTraceFunctionExit0( DUP1_CCMMINSTANCEMAPPING_CCMMINSTANCEMAPPING_EXIT );
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CCmmInstanceMapping::ConstructL()
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_CONSTRUCTL_ENTRY );

    // Read connection methods from database and find the supported ones.
    ReadAndValidateConnMethodsL();
    // Read destinations from database.
    ReadAndValidateDestinationsL();

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------
// Resets arrays and reads CMs and Connection Methods
// ---------------------------------------------------------------------------
//
void CCmmInstanceMapping::RefreshL()
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_REFRESHL_ENTRY );

    ReadAndValidateConnMethodsL();
    ReadAndValidateDestinationsL();

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_REFRESHL_EXIT );
    }

// ---------------------------------------------------------------------------
// Check if the given ID is a valid existing destination ID.
// ---------------------------------------------------------------------------
//
TBool CCmmInstanceMapping::ValidDestinationId( const TUint32& aId ) const
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_VALIDDESTINATIONID_ENTRY );

    TBool validity( EFalse );

    if ( aId > 0 )
        {
        for ( TInt i = 0; i < iDestinations.Count(); i++ )
            {
            if ( iDestinations[i]->iId == aId )
                {
                validity = ETrue;
                break;
                }
            }
        }

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_VALIDDESTINATIONID_EXIT );
    return validity;
    }

// ---------------------------------------------------------------------------
// Check if the given ID is a valid existing connection method ID.
// ---------------------------------------------------------------------------
//
TBool CCmmInstanceMapping::ValidConnMethodId( const TUint32& aId ) const
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_VALIDCONNMETHODID_ENTRY );

    TBool validity( EFalse );

    if ( aId > 0 )
        {
        for ( TInt i = 0; i < iConnMethodItemArray.Count(); i++ )
            {
            if ( iConnMethodItemArray[i].iId == aId )
                {
                validity = ETrue;
                break;
                }
            }
        }

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_VALIDCONNMETHODID_EXIT );
    return validity;
    }

// ---------------------------------------------------------------------------
// Check from database if the given destination is an embedded destination in
// any other destination.
// ---------------------------------------------------------------------------
//
TBool CCmmInstanceMapping::DestinationIsEmbedded( const TUint32& aDestinationId ) const
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_DESTINATIONISEMBEDDED_ENTRY );

    TBool result( EFalse );
    TInt index( 0 );

    // Iterate through all destinations.
    for ( TInt i = 0; i < iDestinations.Count(); i++ )
        {
        // An embedded destination is always at the very last position in
        // the connection method item array.
        index = iDestinations[i]->iConnMethodItemArray.Count() - 1;
        if ( index >= 0 )
            {
            // We can skip bearer type check. If the last item isn't an
            // embedded destination, the ID will not match anyway.
            if ( iDestinations[i]->iConnMethodItemArray[index].iId == aDestinationId )
                {
                result = ETrue;
                break;
                }
            }
        }

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_DESTINATIONISEMBEDDED_EXIT );
    return result;
    }

// ---------------------------------------------------------------------------
// Check from database if the given destination has an embedded destination.
// ---------------------------------------------------------------------------
//
TBool CCmmInstanceMapping::DestinationHasEmbedded( const TUint32& aDestinationId ) const
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_DESTINATIONHASEMBEDDED_ENTRY );

    TBool result( EFalse );

    for ( TInt i = 0; i < iDestinations.Count(); i++ )
        {
        // Find the correct destination.
        if ( iDestinations[i]->iId == aDestinationId )
            {
            // An embedded destination is always at the very last position in
            // the connection method item array.
            TInt index = iDestinations[i]->iConnMethodItemArray.Count() - 1;
            if ( index >= 0 )
                {
                if ( iDestinations[i]->iConnMethodItemArray[index].iBearerType == KUidEmbeddedDestination )
                    {
                    result = ETrue;
                    }
                }
            break;
            }
        }

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_DESTINATIONHASEMBEDDED_EXIT );
    return result;
    }

// ---------------------------------------------------------------------------
// Check from database if the given destination is pointed to by any virtual
// IAP.
// ---------------------------------------------------------------------------
//
TBool CCmmInstanceMapping::DestinationPointedToByVirtualIap(
        const TUint32& /*aDestinationId*/ ) const
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_DESTINATIONPOINTEDTOBYVIRTUALIAP_ENTRY );
    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_DESTINATIONPOINTEDTOBYVIRTUALIAP_EXIT );

    return EFalse; //TODO, virtual IAPs are not yet supported.
    }

// ---------------------------------------------------------------------------
// Check from database if the given connection method is pointed to by any
// virtual IAP.
// ---------------------------------------------------------------------------
//
TBool CCmmInstanceMapping::ConnMethodPointedToByVirtualIap(
        const TUint32& /*aConnMethodId*/ ) const
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_CONNMETHODPOINTEDTOBYVIRTUALIAP_ENTRY );
    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_CONNMETHODPOINTEDTOBYVIRTUALIAP_EXIT );

    return EFalse; //TODO, virtual IAPs are not yet supported.
    }

// ---------------------------------------------------------------------------
// Check if the given connection method is the only connection method in the
// given destination and if a virtual IAP points to that destination.
// ---------------------------------------------------------------------------
//
TBool CCmmInstanceMapping::ConnMethodInDestinationButLocked(
        const TUint32& /*aConnMethodId*/,
        const TUint32& /*aDestinationId*/ ) const
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_CONNMETHODINDESTINATIONBUTLOCKED_ENTRY );

    // - Find correct destination.
    // - Check if it only has 1 CM.
    // - Check if the CM is the one given.
    // - Call DestinationPointedToByVirtualIap( aDestinationId ).
    //
    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_CONNMETHODINDESTINATIONBUTLOCKED_EXIT );

    return EFalse; //TODO, virtual IAPs are not yet supported.
    }

// ---------------------------------------------------------------------------
// Get bearer type of connection method matching given ID.
// Return KErrNone if ID is found, KErrNotFound if not.
// ---------------------------------------------------------------------------
//
TInt CCmmInstanceMapping::GetConnMethodBearerType(
        const TUint32& aConnMethodId,
        TUint32& bearerType ) const
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_GETCONNMETHODBEARERTYPE_ENTRY );

    TInt result( KErrNotFound );

    for ( TInt i = 0; i < iConnMethodItemArray.Count(); i++ )
        {
        if ( iConnMethodItemArray[i].iId == aConnMethodId )
            {
            bearerType = iConnMethodItemArray[i].iBearerType;
            result = KErrNone;
            break;
            }
        }
    if ( result == KErrNotFound )
        {
        // Embedded destinations are not in connection method array, need check from ID range.
        if ( aConnMethodId > KCmmDestIdIntervalMin && aConnMethodId < KCmmDestIdIntervalMax )
            {
            bearerType = KUidEmbeddedDestination;
            result = KErrNone;
            }
        }

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_GETCONNMETHODBEARERTYPE_EXIT );
    return result;
    }

// ---------------------------------------------------------------------------
// Returns the number of destinations the provided connection method belongs
// to. Zero is returned if the connection method belongs to no destination
// (legacy IAP) or does not exist.
// ---------------------------------------------------------------------------
//
TInt CCmmInstanceMapping::DestinationsContainingConnMethod(
        const TUint32& aConnMethodId ) const
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_CONNMETHODREFERENCECOUNT_ENTRY );

    TInt referenceCount( 0 );
    TInt count( 0 );

    // Go through each destination.
    for ( TInt i = 0; i < iDestinations.Count(); i++ )
        {
        TBool foundInThisDestination( EFalse );

        // Loop through all connection methods in this destination.
        count = iDestinations[i]->iConnMethodItemArray.Count();
        for ( TInt j = 0; j < count; j++ )
            {
            if ( iDestinations[i]->iConnMethodItemArray[j].iId == aConnMethodId )
                {
                foundInThisDestination = ETrue;
                referenceCount++;
                break;
                }
            }

        // Check unsupported connection methods also.
        if ( !foundInThisDestination )
            {
            count = iDestinations[i]->iUnsupportedConnMethods.Count();
            for ( TInt j = 0; j < count; j++ )
                {
                if ( iDestinations[i]->iUnsupportedConnMethods[j] == aConnMethodId )
                    {
                    referenceCount++;
                    break;
                    }
                }
            }
        }

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_CONNMETHODREFERENCECOUNT_EXIT );
    return referenceCount;
    }

// ---------------------------------------------------------------------------
// Returns the destination IDs containing the connection method given as
// parameter.
// ---------------------------------------------------------------------------
//
void CCmmInstanceMapping::DestinationsContainingConnMethodL(
        const TUint32 aConnMethodId,
        RArray<TUint32>& aDestinationIds ) const
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_DESTINATIONSCONTAININGCONNMETHODL_ENTRY );
    
    aDestinationIds.Reset();
    TInt count( 0 );
    
    // Go through each destination.
    for ( TInt i = 0; i < iDestinations.Count(); i++ )
        {
        TBool foundInThisDestination( EFalse );

        // Loop through all connection methods in this destination.
        count = iDestinations[i]->iConnMethodItemArray.Count();
        for ( TInt j = 0; j < count; j++ )
            {
            if ( iDestinations[i]->iConnMethodItemArray[j].iId == aConnMethodId )
                {
                foundInThisDestination = ETrue;
                aDestinationIds.AppendL( iDestinations[i]->iId );
                break;
                }
            }

        // Check unsupported connection methods also.
        if ( !foundInThisDestination )
            {
            count = iDestinations[i]->iUnsupportedConnMethods.Count();
            for ( TInt j = 0; j < count; j++ )
                {
                if ( iDestinations[i]->iUnsupportedConnMethods[j] == aConnMethodId )
                    {
                    aDestinationIds.AppendL( iDestinations[i]->iId );
                    break;
                    }
                }
            }
        }
    
    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_DESTINATIONSCONTAININGCONNMETHODL_EXIT );
    }

// ---------------------------------------------------------------------------
// Get database session.
// ---------------------------------------------------------------------------
//
CommsDat::CMDBSession& CCmmInstanceMapping::Session() const
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_SESSION_ENTRY );

    return iCache.Session();
    }

// ---------------------------------------------------------------------------
// Discovers all connection methods from database. Unsupported connection
// methods are kept separately.
// ---------------------------------------------------------------------------
//
void CCmmInstanceMapping::ReadAndValidateConnMethodsL()
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_READANDVALIDATECONNMETHODSL_ENTRY );

    CMDBRecordSet<CCDIAPRecord>* iapRecordSet =
            new( ELeave ) CMDBRecordSet<CCDIAPRecord>( KCDTIdIAPRecord );
    CleanupStack::PushL( iapRecordSet );

    CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>( CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
    CleanupStack::PushL( iapRecord );

    TRAP_IGNORE( iapRecordSet->LoadL( Session() ) );

    //TODO, check from commsdat dump if all high-range IDs are available, or was some records
    //TODO  used up by something. will this possibly affect cm/destination creation with predef ID? (Keijo)

    iConnMethodItemArray.Reset();
    iUnsupportedConnMethods.Reset();

    TInt iapRecordCount( iapRecordSet->iRecords.Count() );
    TInt err( KErrNone );
    TUint32 bearerType( 0 );
    TUint bearerPriority( CMManager::KDataMobilitySelectionPolicyPriorityWildCard );

    for ( TInt i = 0; i < iapRecordCount; i++ )
        {
        TUint32 connMethodId( iapRecordSet->iRecords[i]->RecordId() );

        // Check the connection method is not on the deleted list waiting to be deleted from database.
        TInt indexInDeletedList = iDeletedConnMethods.FindInOrder( ( TUint )connMethodId );
        if ( indexInDeletedList == KErrNotFound )
            {
            // Check the bearer type of the iap. Leaves if iap is unsupported.
            iapRecord->SetRecordId( connMethodId );
            TRAP( err, iCache.BearerInfoFromIapRecordL( iapRecord, bearerType, bearerPriority ) ); //TODO
            if ( !err )
                {
                TCmmConnMethodItem item( connMethodId, bearerType, bearerPriority, 0 );
                iConnMethodItemArray.Append( item ); // Ignore errors.
                }
            else if ( err == KErrNotSupported )
                {
                iUnsupportedConnMethods.Append( connMethodId ); // Ignore errors.
                }
            else if ( err == KErrNoMemory )
                {
                User::Leave( err ); //TODO, check what this will cause.
                }
            }
        }

    CleanupStack::PopAndDestroy( iapRecord );
    CleanupStack::PopAndDestroy( iapRecordSet );

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_READANDVALIDATECONNMETHODSL_EXIT );
    }

// ---------------------------------------------------------------------------
// Discovers all destinations and the connection methods inside them.
// Unsupported connection methods are kept separately.
// ---------------------------------------------------------------------------
//
void CCmmInstanceMapping::ReadAndValidateDestinationsL()
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_READANDVALIDATEDESTINATIONSL_ENTRY );

    // Make 2 loops, first add destinations and then connection methods / embedded destinations.
    iDestinations.ResetAndDestroy();

    // Explicitly build a TLinearOrder<class>. Used as parameter to RArray::InsertInOrder().
    TLinearOrder<TCmmConnMethodItem> connMethodItemOrderingLogic( TCmmConnMethodItem::Compare );

    CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>* snapRecordSet =
            new( ELeave ) CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>(
                    iCache.TableId( ECmmDbSnapRecord ) );
    CleanupStack::PushL( snapRecordSet );

    CCDDataMobilitySelectionPolicyRecord* snapRecord =
            new( ELeave ) CCDDataMobilitySelectionPolicyRecord(
                    iCache.TableId( ECmmDbSnapRecord ) );
    CleanupStack::PushL( snapRecord );

    TRAP_IGNORE( snapRecordSet->LoadL( Session() ) );

    // Read snap ID, connection method ID and embedded destination ID.
    const TInt snapRecordCount( snapRecordSet->iRecords.Count() );
    for ( TInt i = 0; i < snapRecordCount; i++ )
        {
        snapRecord->SetElementId( snapRecordSet->iRecords[i]->ElementId() );
        snapRecord->LoadL( Session() );

        TUint32 destinationId = (TUint32)( snapRecord->iSNAP );
        TInt indexInDeletedList = iDeletedDestinations.FindInOrder( ( TUint )destinationId );
        if ( indexInDeletedList == KErrNotFound )
            {
            TUint32 connMethodId = ( snapRecord->iIAP & KCDMaskShowRecordId ) >> 8;
            TUint32 embeddedDestinationId = (TUint32)( snapRecord->iEmbeddedSNAP );

            // If connMethodId and embeddedDestinationId are 0 this is a destination.
            if ( connMethodId == 0 && embeddedDestinationId == 0 )
                {
                TBool destAlreadyExists( EFalse );
                for ( TInt j = 0; j < iDestinations.Count(); j++ )
                    {
                    if ( destinationId == iDestinations[j]->iId )
                        {
                        destAlreadyExists = ETrue;
                        break;
                        }
                    }

                if ( !destAlreadyExists )
                    {
                    CDestination* dest = CDestination::NewL();
                    dest->iId = destinationId;

                    CCDSNAPMetadataRecord* metadataRecord = new( ELeave ) CCDSNAPMetadataRecord( 
                            iCache.TableId( ECmmDestMetadataRecord ) );
                    CleanupStack::PushL( metadataRecord );

                    // Add destination metadata.
                    metadataRecord->iSNAP.SetL( destinationId );
                    if ( metadataRecord->FindL( Session() ) )
                        {
                        metadataRecord->LoadL( Session() );
                        dest->iMetadata = metadataRecord->iMetadata;
                        iDestinations.AppendL( dest );
                        }
                    CleanupStack::PopAndDestroy( metadataRecord );
                    metadataRecord = NULL;
                    }
                }
            }
        }

    // Read snap ID, connection method ID and embedded destination ID.
    for ( TInt i = 0; i < snapRecordCount; i++ )
        {
        snapRecord->SetElementId( snapRecordSet->iRecords[i]->ElementId() );
        snapRecord->LoadL( Session() );

        TUint32 destinationId = (TUint32)( snapRecord->iSNAP );
        TUint32 connMethodId = ( snapRecord->iIAP & KCDMaskShowRecordId ) >> 8;
        TUint32 embeddedDestinationId = (TUint32)( snapRecord->iEmbeddedSNAP );

        // If connMethodId or embeddedDestinationId differs from 0 this is a connection method object.
        if ( connMethodId > 0 || embeddedDestinationId > 0 )
            {
            // Find destination.
            CDestination* destination( NULL );
            for ( TInt j = 0; j < iDestinations.Count(); j++ )
                {
                if ( destinationId == iDestinations[j]->iId )
                    {
                    destination = iDestinations[j];
                    break;
                    }
                }

            if ( destination )
                {
                if ( connMethodId )
                    {
                    // Connection method, not embedded destination.
                    TBool found( EFalse );
                    TCmmConnMethodItem item;

                    // Find connection method.
                    for ( TInt j = 0; j < iConnMethodItemArray.Count(); j++ )
                        {
                        if ( iConnMethodItemArray[j].iId == connMethodId )
                            {
                            item = iConnMethodItemArray[j];
                            found = ETrue;
                            break;
                            }
                        }
                    if ( found )
                        {
                        // Make sure no duplicate entries are allowed. Any
                        // duplicate would imply a corrupted CommsDat.
                        TInt index = destination->iConnMethodItemArray.Find<TUint32>(
                                connMethodId,
                                TCmmConnMethodItem::FindCompare );
                        if ( index == KErrNotFound )
                            {
                            item.iPriority = snapRecord->iPriority;
                            destination->iConnMethodItemArray.InsertInOrderAllowRepeats( //TODO, if prio is 256, bearer type used? embeded should always be last.
                                    item,
                                    connMethodItemOrderingLogic ); // Ignore errors.
                            }
                        }
                    else
                        {
                        // Check if the connection method is unsupported instead.
                        for ( TInt j = 0; j < iUnsupportedConnMethods.Count(); j++ )
                            {
                            if ( iUnsupportedConnMethods[j] == connMethodId )
                                {
                                found = ETrue;
                                break;
                                }
                            }
                        if ( found )
                            {
                            destination->iUnsupportedConnMethods.Append( connMethodId ); // Ignore errors. //TODO, allow repeats?
                            }
                        }
                    }

                else
                    {
                    // Embedded destination, not IAP.
                    // Prevent destinations from embedding themselves.
                    if ( embeddedDestinationId != destinationId )
                        {
                        // Check embedded destination ID is valid.
                        TBool found( EFalse );
                        for ( TInt j = 0; j < iDestinations.Count(); j++ )
                            {
                            if ( embeddedDestinationId == iDestinations[j]->iId )
                                {
                                found = ETrue;
                                break;
                                }
                            }
                        if ( found )
                            {
                            TCmmConnMethodItem item(
                                    embeddedDestinationId,
                                    KUidEmbeddedDestination,
                                    CMManager::KDataMobilitySelectionPolicyPriorityWildCard,
                                    CMManager::KDataMobilitySelectionPolicyPriorityWildCard );
                            destination->iConnMethodItemArray.InsertInOrderAllowRepeats(
                                    item,
                                    connMethodItemOrderingLogic );
                            }
                        }
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy( snapRecord );
    CleanupStack::PopAndDestroy( snapRecordSet );

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_READANDVALIDATEDESTINATIONSL_EXIT );
    }

// ---------------------------------------------------------------------------
// Returns all conenction method IDs. Unsupported connection methods are
// included if aCheckBearerType is set to EFalse.
// ---------------------------------------------------------------------------
//
void CCmmInstanceMapping::GetAllConnMethodsL(
        RArray<TUint32>& aConnMethodArray,
        TBool aCheckBearerType ) const
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_GETCONNMETHODSL_ENTRY );

    TInt connMethodCount = iConnMethodItemArray.Count();
    aConnMethodArray.Reset();

    if ( aCheckBearerType )
        {
        aConnMethodArray.ReserveL( connMethodCount );
        }
    else
        {
        aConnMethodArray.ReserveL( connMethodCount + iUnsupportedConnMethods.Count() );
        }

    for ( TInt i = 0; i < connMethodCount; i++ )
        {
        aConnMethodArray.AppendL( iConnMethodItemArray[i].iId );
        }
    if ( !aCheckBearerType )
        {
        // Include unsupported connection methods also.
        for ( TInt i = 0; i < iUnsupportedConnMethods.Count(); i++ )
            {
            aConnMethodArray.AppendL( iUnsupportedConnMethods[i] );
            }
        }

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_GETCONNMETHODSL_EXIT );
    }

// ---------------------------------------------------------------------------
// Return the number of destinations in database.
// ---------------------------------------------------------------------------
//
TInt CCmmInstanceMapping::GetDestinationCount() const
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_GETDESTINATIONCOUNT_ENTRY );

    return iDestinations.Count();
    }

// ---------------------------------------------------------------------------
// Return an array containing all destination IDs.
// ---------------------------------------------------------------------------
//
void CCmmInstanceMapping::GetDestinationsL( RArray<TUint32>& aDestinationArray ) const
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_GETDESTINATIONSL_ENTRY );

    aDestinationArray.Reset();
    aDestinationArray.ReserveL( iDestinations.Count() ); // Re-allocates only if more is needed.

    for ( TInt i = 0; i < iDestinations.Count(); i++ )
        {
        aDestinationArray.AppendL( iDestinations[i]->iId );
        }

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_GETDESTINATIONSL_EXIT );
    }

// ---------------------------------------------------------------------------
// Inserts all the valid connection methods inside the given destination into
// the provided connection method item array. The array is reset first.
// ---------------------------------------------------------------------------
//
void CCmmInstanceMapping::GetConnMethodsFromDestinationL(
        const TUint32& aDestinationId,
        RArray<TCmmConnMethodItem>& aConnMethodArray ) const
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_GETCONNMETHODSFROMDESTINATIONL_ENTRY );

    TBool destinationFound( EFalse );

    for ( TInt i = 0; i < iDestinations.Count(); i++ )
        {
        if ( iDestinations[i]->iId == aDestinationId )
            {
            destinationFound = ETrue;
            aConnMethodArray.Reset();
            aConnMethodArray.ReserveL( iDestinations[i]->iConnMethodItemArray.Count() );

            for ( TInt j = 0; j < iDestinations[i]->iConnMethodItemArray.Count(); j++ )
                {
                aConnMethodArray.AppendL( iDestinations[i]->iConnMethodItemArray[j] );
                }
            break;
            }
        }
    if ( !destinationFound )
        {
        User::Leave( KErrNotFound );
        }

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_GETCONNMETHODSFROMDESTINATIONL_EXIT );
    }

// ---------------------------------------------------------------------------
// Check if the given connection method belongs to any other destination than
// the one provided.
// ---------------------------------------------------------------------------
//
TBool CCmmInstanceMapping::ConnMethodInOtherDestination(
        const TUint32& aConnMethodId,
        const TUint32& aDestinationId )
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_CONNMETHODINOTHERDESTINATION_ENTRY );

    // Loop through all destinations.
    for ( TInt i = 0; i < iDestinations.Count(); i++ )
        {
        // Skip the specified destination.
        if ( iDestinations[i]->iId != aDestinationId )
            {
            // Loop through all connection methods in the current destination.
            for ( TInt j = 0; j < iDestinations[i]->iConnMethodItemArray.Count(); j++ )
                {
                if ( iDestinations[i]->iConnMethodItemArray[j].iId == aConnMethodId )
                    {
                    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_CONNMETHODINOTHERDESTINATION_EXIT );
                    return ETrue;
                    }
                }
            }
        }

    OstTraceFunctionExit0( DUP1_CCMMINSTANCEMAPPING_CONNMETHODINOTHERDESTINATION_EXIT );
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Return the EasyWLAN IAP ID, zero if not found or WLAN not supported.
// ---------------------------------------------------------------------------
//
TUint32 CCmmInstanceMapping::EasyWlanIdL()
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_EASYWLANIDL_ENTRY );

    iEasyWlanId = 0;

    // Check WLAN support.
    if ( iCache.WlanSupported() )
        {
        CMDBRecordSet<CCDIAPRecord>* iapRecordSet =
                new( ELeave ) CMDBRecordSet<CCDIAPRecord>( KCDTIdIAPRecord );
        CleanupStack::PushL( iapRecordSet );
        TRAP_IGNORE( iapRecordSet->LoadL( Session() ) );

        TInt iapRecordCount( iapRecordSet->iRecords.Count() );
        for ( TInt i = 0; i < iapRecordCount; i++ )
            {
            CCDIAPRecord* iapRecord = ( CCDIAPRecord* )iapRecordSet->iRecords[i];

            if ( TPtrC( iapRecord->iServiceType ) == TPtrC( KCDTypeNameLANService ) &&
                    TPtrC( iapRecord->iBearerType ) == TPtrC( KCDTypeNameLANBearer ) )
                {
                TUint32 serviceId = iapRecord->iService;

                CCDWlanServiceRecord* wlanServ =
                        new( ELeave ) CCDWlanServiceRecord(
                                CCDWlanServiceRecord::TableIdL( Session() ) );
                CleanupStack::PushL( wlanServ );

                wlanServ->iWlanServiceId.SetL( serviceId );
                if ( wlanServ->FindL( Session() ) )
                    {
                    wlanServ->LoadL( Session() );
                    // Only EasyWLAN IAP has NULL in SSID field.
                    if ( wlanServ->iWLanSSID.IsNull() ||
                            !( TPtrC( wlanServ->iWLanSSID ).Compare( KNullDesC ) ) )
                        {
                        iEasyWlanId = iapRecord->RecordId();
                        }
                    }
                CleanupStack::PopAndDestroy( wlanServ );
                }
            if ( iEasyWlanId != 0 )
                {
                break;
                }
            }
        CleanupStack::PopAndDestroy( iapRecordSet );
        }

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_EASYWLANIDL_EXIT );
    return iEasyWlanId;
    }

// ---------------------------------------------------------------------------
// Find out the internet destination ID. ID is set to 0 if not found.
// ---------------------------------------------------------------------------
//
void CCmmInstanceMapping::InternetDestinationIdL( TUint& aInternetDestinationId )
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_INTERNETDESTINATIONIDL_ENTRY );

    // Set to zero in case the destination is not found.
    aInternetDestinationId = 0;

    CommsDat::CMDBRecordSet<CCDSNAPMetadataRecord>* metaSet =
            new( ELeave )CommsDat::CMDBRecordSet<CCDSNAPMetadataRecord>(
                    iCache.TableId( ECmmDestMetadataRecord ) );
    CleanupStack::PushL( metaSet );

    TRAP_IGNORE( metaSet->LoadL( Session() ) );

    CCDSNAPMetadataRecord* metadataRecord =
            new( ELeave ) CCDSNAPMetadataRecord(
                    iCache.TableId( ECmmDestMetadataRecord ) );
    CleanupStack::PushL( metadataRecord );

    for ( TInt i = 0; i < metaSet->iRecords.Count(); i++ )
        {
        metadataRecord->SetRecordId( metaSet->iRecords[i]->RecordId() );
        metadataRecord->LoadL( Session() );

        TUint32 metadata = metadataRecord->iMetadata;

        TUint32 internet = metadata & CMManager::ESnapMetadataInternet;
        TUint32 localizationValue = ( metadata & CMManager::ESnapMetadataDestinationIsLocalised ) >> 4;
        TUint32 purposeValue = ( metadata & CMManager::ESnapMetadataPurpose ) >> 8;

        // The first record that has a matching value in any of the 3 metadata
        // fields will be taken as the internet snap.
        if ( internet ||
                ( localizationValue == CMManager::ELocalisedDestInternet ) ||
                ( purposeValue == CMManager::ESnapPurposeInternet ) )
            {
            TInt id = metadataRecord->iSNAP;
            if ( ValidDestinationId( ( TUint32 )id ) )
                {
                aInternetDestinationId = ( TUint )id;
                break;
                }
            }
        }
    CleanupStack::PopAndDestroy( metadataRecord );
    CleanupStack::PopAndDestroy( metaSet );

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_INTERNETDESTINATIONIDL_EXIT );
    }

// ---------------------------------------------------------------------------
// Add a connection method ID to deleted list. Ignores any duplicates. Also
// removes the connection method from destination/connection method structures
// so Refresh()-call is not needed. Use this method if the connection method
// has not been removed from database yet.
// ---------------------------------------------------------------------------
//
void CCmmInstanceMapping::AddConnMethodToDeletedListL( const TUint& aConnMethodId )
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_ADDCONNMETHODTODELETEDLISTL_ENTRY );

    iDeletedConnMethods.InsertInOrderL( aConnMethodId );

    // Remove the connection method from current destination/connection method structures.
    RemoveConnMethod( aConnMethodId );

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_ADDCONNMETHODTODELETEDLISTL_EXIT );
    }

// ---------------------------------------------------------------------------
// Remove a connection method ID from deleted list. Nothing happens if ID is
// not found from the list.
// ---------------------------------------------------------------------------
//
void CCmmInstanceMapping::RemoveConnMethodFromDeletedList( const TUint& aConnMethodId )
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_REMOVECONNMETHODFROMDELETEDLIST_ENTRY );

    TInt index = iDeletedConnMethods.FindInOrder( aConnMethodId );
    if ( index != KErrNotFound )
        {
        iDeletedConnMethods.Remove( index );
        }

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_REMOVECONNMETHODFROMDELETEDLIST_EXIT );
    }

// ---------------------------------------------------------------------------
// Add a destination ID to deleted list. Ignores any duplicates. Also removes
// the destination from destination/connection method structures so
// Refresh()-call is not needed. Use this method if the connection method has
// not been removed from database yet.
// ---------------------------------------------------------------------------
//
void CCmmInstanceMapping::AddDestinationToDeletedListL( const TUint& aDestinationId )
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_ADDDESTINATIONTODELETEDLISTL_ENTRY );

    iDeletedDestinations.InsertInOrderL( aDestinationId );

    // Remove the destination from current destination/connection method structures.
    RemoveDestination( aDestinationId );

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_ADDDESTINATIONTODELETEDLISTL_EXIT );
    }

// ---------------------------------------------------------------------------
// Remove a destination ID from deleted list. Nothing happens if ID is not
// found from the list.
// ---------------------------------------------------------------------------
//
void CCmmInstanceMapping::RemoveDestinationFromDeletedList( const TUint& aDestinationId ) //TODO, check removal is called in all necessary places.
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_REMOVEDESTINATIONFROMDELETEDLIST_ENTRY );

    TInt index = iDeletedDestinations.FindInOrder( aDestinationId );
    if ( index != KErrNotFound )
        {
        iDeletedDestinations.Remove( index );
        }

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_REMOVEDESTINATIONFROMDELETEDLIST_EXIT );
    }

// ---------------------------------------------------------------------------
// Remove the connection method from current destination/connection method
// structures. This is a lot faster than calling Refresh(). Use this method if
// the connection method has been removed from database.
// ---------------------------------------------------------------------------
//
void CCmmInstanceMapping::RemoveConnMethod( const TUint32& aConnMethodId ) //TODO, check where this is used and if it would be better to use version with array instead?
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_REMOVECONNMETHOD_ENTRY );

    // Remove from list of connection methods.
    for ( TInt i = 0; i < iConnMethodItemArray.Count(); i++ )
        {
        if ( iConnMethodItemArray[i].iId == aConnMethodId )
            {
            iConnMethodItemArray.Remove( i );
            break;
            }
        }

    // Remove connection method from all destinations.
    for ( TInt i = 0; i < iDestinations.Count(); i++ )
        {
        for ( TInt j = 0; j < iDestinations[i]->iConnMethodItemArray.Count(); j++ )
            {
            if ( iDestinations[i]->iConnMethodItemArray[j].iId == aConnMethodId )
                {
                iDestinations[i]->iConnMethodItemArray.Remove( j );
                break;
                }
            }
        }

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_REMOVECONNMETHOD_EXIT );
    }

// ---------------------------------------------------------------------------
// Remove the connection method from current destination/connection method
// structures. This is a lot faster than calling Refresh(). Use this method if
// the connection method has been removed from database.
// ---------------------------------------------------------------------------
//
void CCmmInstanceMapping::RemoveConnMethod(
        const TUint32& aConnMethodId,
        RArray<TUint32>& aChangedDestinations )
    {
    OstTraceFunctionEntry0( DUP1_CCMMINSTANCEMAPPING_REMOVECONNMETHOD_ENTRY );

    // Remove from list of connection methods.
    for ( TInt i = 0; i < iConnMethodItemArray.Count(); i++ )
        {
        if ( iConnMethodItemArray[i].iId == aConnMethodId )
            {
            iConnMethodItemArray.Remove( i );
            break;
            }
        }

    // Remove connection method from all destinations.
    for ( TInt i = 0; i < iDestinations.Count(); i++ )
        {
        for ( TInt j = 0; j < iDestinations[i]->iConnMethodItemArray.Count(); j++ )
            {
            if ( iDestinations[i]->iConnMethodItemArray[j].iId == aConnMethodId )
                {
                iDestinations[i]->iConnMethodItemArray.Remove( j );
                // Add ID of changed destination into array. Ignore any error.
                aChangedDestinations.Append( iDestinations[i]->iId );
                break;
                }
            }
        }
    
    OstTraceFunctionExit0( DUP1_CCMMINSTANCEMAPPING_REMOVECONNMETHOD_EXIT );
    }

// ---------------------------------------------------------------------------
// Remove the destination from current destination/connection method
// structures. This is a lot faster than calling Refresh(). Use this method if
// the connection method has been removed from database.
// ---------------------------------------------------------------------------
//
void CCmmInstanceMapping::RemoveDestination( const TUint32& aDestinationId )
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_REMOVEDESTINATION_ENTRY );

    for ( TInt i = 0; i < iDestinations.Count(); i++ )
        {
        if ( iDestinations[i]->iId == aDestinationId )
            {
            delete iDestinations[i];
            iDestinations.Remove( i );
            i--;
            }
        else
            {
            // Check also if the destination is an embedded destination
            // anywhere, and remove that reference too.
            TInt cmCount( iDestinations[i]->iConnMethodItemArray.Count() );
            if ( cmCount )
                {
                if ( iDestinations[i]->iConnMethodItemArray[cmCount - 1].iId == aDestinationId )
                    {
                    iDestinations[i]->iConnMethodItemArray.Remove( cmCount - 1 );
                    }
                }
            }
        }

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_REMOVEDESTINATION_EXIT );
    }

// ---------------------------------------------------------------------------
// Remove the connection method from all destinations in the current
// destination/connection method structures. This is a lot faster than calling
// Refresh(). The ID of any changed destination is added to the
// aChangedDestinations-array.
// ---------------------------------------------------------------------------
//
void CCmmInstanceMapping::RemoveConnMethodFromDestinations(
        const TUint32& aConnMethodId,
        RArray<TUint32>& aChangedDestinations )
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_REMOVECONNMETHODFROMDESTINATIONS_ENTRY );

    // Remove given connection method from all destinations.
    for ( TInt i = 0; i < iDestinations.Count(); i++ )
        {
        for ( TInt j = 0; j < iDestinations[i]->iConnMethodItemArray.Count(); j++ )
            {
            if ( iDestinations[i]->iConnMethodItemArray[j].iId == aConnMethodId )
                {
                iDestinations[i]->iConnMethodItemArray.Remove( j );
                // Add ID of changed destination into array. Ignore any error.
                aChangedDestinations.Append( iDestinations[i]->iId );
                break;
                }
            }
        }

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_REMOVECONNMETHODFROMDESTINATIONS_EXIT );
    }

// ---------------------------------------------------------------------------
// Iterate through destinations to find the one identified with parameter
// and returns its metadata mask.
// ---------------------------------------------------------------------------
//
TUint32 CCmmInstanceMapping::DestinationMetadata( 
        const TUint32 aDestinationId ) const
    {
    OstTraceFunctionEntry0( CCMMINSTANCEMAPPING_DESTINATIONMETADATA_ENTRY );

    TUint32 metadata( 0 );
    for ( TInt i = 0; i < iDestinations.Count(); i++ )
        {
        if ( iDestinations[i]->iId == aDestinationId )
            {
            metadata = iDestinations[i]->iMetadata;
            break;
            }
        }

    OstTraceFunctionExit0( CCMMINSTANCEMAPPING_DESTINATIONMETADATA_EXIT );
    return metadata;
    }

// End of file
