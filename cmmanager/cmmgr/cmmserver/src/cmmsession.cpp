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
* Handles client requests.
*
*/


#include <cmconnectionmethoddef.h>
#include <cmpluginembdestinationdef.h>
#include <cmdefconnvalues.h>

#include "cmmserver.h"
#include "cmmserverdefs.h"
#include "cmmsession.h"
#include "cmmdestinationinstance.h"
#include "cmmconnmethodinstance.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmsessionTraces.h"
#endif


CCmmSession* CCmmSession::NewL( CCmmServer& aServer, CCmmCache& aCache )
    {
    OstTraceFunctionEntry0( CCMMSESSION_NEWL_ENTRY );

    CCmmSession* self = CCmmSession::NewLC( aServer, aCache );
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMMSESSION_NEWL_EXIT );
    return self;
    }

CCmmSession* CCmmSession::NewLC( CCmmServer& aServer, CCmmCache& aCache )
    {
    OstTraceFunctionEntry0( CCMMSESSION_NEWLC_ENTRY );

    CCmmSession* self = new( ELeave ) CCmmSession( aServer, aCache );
    CleanupStack::PushL( self ) ;
    self->ConstructL() ;

    OstTraceFunctionExit0( CCMMSESSION_NEWLC_EXIT );
    return self ;
    }

// -----------------------------------------------------------------------------
// Constructor.
// -----------------------------------------------------------------------------
//
CCmmSession::CCmmSession( CCmmServer& aServer, CCmmCache& aCache )
        :
        iServer( aServer ),
        iCache( aCache )
    {
    OstTraceFunctionEntry0( CCMMSESSION_CCMMSESSION_ENTRY );

    iDestinationContainer = NULL;
    iDestinationObjects = NULL;
    iConnMethodContainer = NULL;
    iConnMethodObjects = NULL;

    OstTraceFunctionExit0( CCMMSESSION_CCMMSESSION_EXIT );
    }

// -----------------------------------------------------------------------------
// The second phase of two phase construction.
// -----------------------------------------------------------------------------
//
void CCmmSession::ConstructL()
    {
    OstTraceFunctionEntry0( CCMMSESSION_CONSTRUCTL_ENTRY );

    iServer.IncrementSessions();

    // Create a new object index (it stores the destination objects owned by
    // this session).
    iDestinationObjects = CObjectIx::NewL();

    // Initialize the object container using the object container index in the
    // server (Object container provides unique ids for the objects owned by
    // this session).
    iDestinationContainer = iServer.NewContainerL();

    // The same for connection method subsessions.
    iConnMethodObjects = CObjectIx::NewL();
    iConnMethodContainer = iServer.NewContainerL();

    OstTraceFunctionExit0( CCMMSESSION_CONSTRUCTL_EXIT );
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CCmmSession::~CCmmSession()
    {
    OstTraceFunctionEntry0( DUP1_CCMMSESSION_CCMMSESSION_ENTRY );

    iServer.DecrementSessions();

    delete iDestinationObjects;
    iDestinationObjects = NULL;

    if ( iDestinationContainer != 0 )
        {
        iServer.RemoveContainer( iDestinationContainer );
        iDestinationContainer = NULL;
        }

    delete iConnMethodObjects;
    iConnMethodObjects = NULL;

    if ( iConnMethodContainer != 0 )
        {
        iServer.RemoveContainer( iConnMethodContainer );
        iConnMethodContainer = NULL;
        }
    OstTraceFunctionExit0( DUP1_CCMMSESSION_CCMMSESSION_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::ServiceL
// -----------------------------------------------------------------------------
//
void CCmmSession::ServiceL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_SERVICEL_ENTRY );

    TInt result( KErrNone );
    switch ( aMessage.Function() )
        {
        case ECmmGetBearerInfoInt:
            {
            GetBearerInfoIntL( aMessage );
            }
            break;
        case ECmmGetBearerInfoBool:
            {
            GetBearerInfoBoolL( aMessage );
            }
            break;
        case ECmmGetBearerInfoString:
            {
            GetBearerInfoStringL( aMessage );
            }
            break;
        case ECmmGetBearerInfoString8:
            {
            GetBearerInfoString8L( aMessage );
            }
            break;
        case ECmmGetConnMethodInfoInt:
            {
            GetConnMethodInfoIntL( aMessage );
            }
            break;
        case ECmmGetConnMethodInfoBool:
            {
            GetConnMethodInfoBoolL( aMessage );
            }
            break;
        case ECmmGetConnMethodInfoString:
            {
            GetConnMethodInfoStringL( aMessage );
            }
            break;
        case ECmmGetConnMethodInfoString8:
            {
            GetConnMethodInfoString8L( aMessage );
            }
            break;
        case ECmmGetConnMethodArray:
            {
            GetConnMethodArrayL( aMessage );
            }
            break;
        case ECmmGetAllDestinations:
            {
            GetAllDestinationsL( aMessage );
            }
            break;
        case ECmmGetEasyWlanId:
            {
            GetEasyWLANIdL( aMessage );
            }
            break;
        case ECmmRemoveAllReferences: // CM becomes uncategorized.
            {
            RemoveAllReferencesL( aMessage );
            }
            break;
        case ECmmGetSupportedBearers:
            {
            GetSupportedBearersL( aMessage );
            }
            break;
        case ECmmGetUncategorizedIcon:
            {
            GetUncategorizedIconL( aMessage );
            }
            break;
        case ECmmReadDefaultConnection:
            {
            ReadDefaultConnectionL( aMessage );
            }
            break;
        case ECmmWriteDefaultConnection:
            {
            // Default connection is now just internet snap.
            result = KErrNotSupported;
            }
            break;
        case ECmmReadGeneralConnectionSettings:
            {
            ReadGenConnSettingsL( aMessage );
            }
            break;
        case ECmmWriteGeneralConnectionSettings:
            {
            WriteGenConnSettingsL( aMessage );
            }
            break;
        case ECmmGetBearerPriorityArray:
            {
            GetBearerPriorityArrayL( aMessage );
            }
            break;
        case ECmmUpdateBearerPriorityArray:
            {
            UpdateBearerPriorityArrayL( aMessage );
            }
            break;
        case ECmmCopyConnMethod:
            {
            CopyConnMethodL( aMessage );
            }
            break;
        case ECmmMoveConnMethod:
            {
            MoveConnMethodL( aMessage );
            }
            break;
        case ECmmRemoveConnMethod:
            {
            RemoveConnMethodL( aMessage );
            }
            break;

        case EDestGetDestination:
        case EDestRefresh:
        case EDestCreateDestinationWithName:
        case EDestCreateDestinationWithNameAndId:
        case EDestCloseDestination:
        case EDestGetConnMethodCount:
        case EDestGetConnMethodPriority:
        case EDestGetName:
        case EDestGetId:
        case EDestGetElementId:
        case EDestMetadata:
        case EDestGetProtectionLevel:
        case EDestIsConnected:
        case EDestIsHidden:
        case EDestIsEqual:
        case EDestGetIcon:
        case EDestAddConnMethod:
        case EDestAddEmbeddedDestination:
        case EDestDeleteConnMethod:
        case EDestRemoveConnMethod:
        case EDestModifyPriority:
        case EDestSetName:
        case EDestSetMetadata:
        case EDestSetProtection:
        case EDestSetHidden:
        case EDestUpdate:
        case EDestDelete:
        case EDestSetIcon:
            {
            ServiceDestinationL( aMessage );
            }
            break;

        case ECMGetConnMethodWithId:
        case ECMRefresh:
        case ECMCreateConnMethod:
        case ECMCreateConnMethodWithId:
        case ECMGetConnMethodFromDestWithIndex:
        case ECMGetConnMethodFromDestWithId:
        case ECMCreateConnMethodToDest:
        case ECMCreateConnMethodToDestWithId:
        case ECMCreateCopyOfExisting:
        case ECMCloseConnMethod:
        case ECMGetIntAttribute:
        case ECMGetBoolAttribute:
        case ECMGetStringAttribute:
        case ECMGetString8Attribute:
        case ECMIsEqual:
        case ECMSetIntAttribute:
        case ECMSetBoolAttribute:
        case ECMSetStringAttribute:
        case ECMSetString8Attribute:
        case ECMDelete:
        case ECMUpdate:
        case EDestGetEmbeddedDestination:
            {
            ServiceConnMethodL( aMessage );
            }
            break;
        default:
            {
            result = KErrNotSupported;
            }
            break;
        }

    aMessage.Complete( result );

    OstTraceFunctionExit0( CCMMSESSION_SERVICEL_EXIT );
    }

// -----------------------------------------------------------------------------
// Finds a connection method instance that belongs to this session and matches
// the provided ID. Return NULL if no match is found.
// -----------------------------------------------------------------------------
//
CCmmConnMethodInstance* CCmmSession::FindConnMethodInstanceById(
        const TUint32& aConnMethodId )
    {
    OstTraceFunctionEntry0( CCMMSESSION_FINDCONNMETHODINSTANCEBYID_ENTRY );

    CCmmConnMethodInstance* wantedConnMethodInstance( NULL );

    for ( TInt i = 0; i < iConnMethodObjects->Count(); i++ )
        {
        CCmmConnMethodInstance* connMethodInstance =
                ( CCmmConnMethodInstance* )( ( *iConnMethodObjects )[i] );
        if ( connMethodInstance && connMethodInstance->GetId() == aConnMethodId )
            {
            wantedConnMethodInstance = connMethodInstance;
            break;
            }
        }

    OstTraceFunctionExit0( CCMMSESSION_FINDCONNMETHODINSTANCEBYID_EXIT );
    return wantedConnMethodInstance;
    }

// -----------------------------------------------------------------------------
// Finds a destination instance that belongs to this session and matches
// the provided handle.
// -----------------------------------------------------------------------------
//
CCmmDestinationInstance* CCmmSession::FindDestinationInstanceByHandleL(
        const TInt& aDestinationHandle )
    {
    OstTraceFunctionEntry0( CCMMSESSION_FINDDESTINATIONINSTANCEBYHANDLEL_ENTRY );

    return (CCmmDestinationInstance*)iDestinationObjects->AtL( aDestinationHandle );
    }

// -----------------------------------------------------------------------------
// Finds a destination instance that belongs to this session and matches
// the provided ID. Return NULL if no match is found.
// -----------------------------------------------------------------------------
//
CCmmDestinationInstance* CCmmSession::FindDestinationInstanceById(
        const TUint32& aDestinationId )
    {
    OstTraceFunctionEntry0( CCMMSESSION_FINDDESTINATIONINSTANCEBYID_ENTRY );

    CCmmDestinationInstance* wantedDestinationInstance( NULL );

    for ( TInt i = 0; i < iDestinationObjects->Count(); i++ )
        {
        CCmmDestinationInstance* destinationInstance =
                ( CCmmDestinationInstance* )( ( *iDestinationObjects )[i] );
        if ( destinationInstance && destinationInstance->GetId() == aDestinationId )
            {
            wantedDestinationInstance = destinationInstance;
            break;
            }
        }

    OstTraceFunctionExit0( CCMMSESSION_FINDDESTINATIONINSTANCEBYID_EXIT );
    return wantedDestinationInstance;
    }

// -----------------------------------------------------------------------------
// Check from all open destination handles in this session if the given
// connection method is inside any of them. The given destination is skipped.
// -----------------------------------------------------------------------------
//
TBool CCmmSession::ConnMethodInOtherDestination(
        const TUint32& aConnMethodId,
        const TUint32& aDestinationId )
    {
    OstTraceFunctionEntry0( CCMMSESSION_CONNMETHODINOTHERDESTINATION_ENTRY );

    for ( TInt i = 0; i < iDestinationObjects->Count(); i++ )
        {
        CCmmDestinationInstance* destinationInstance =
                ( CCmmDestinationInstance* )( ( *iDestinationObjects )[i] );
        if ( destinationInstance && destinationInstance->GetId() != aDestinationId )
            {
            for ( TInt j = 0; j < destinationInstance->iConnMethodItemArray.Count(); j++ )
                {
                if ( destinationInstance->iConnMethodItemArray[i].iId == aConnMethodId )
                    {
                    OstTraceFunctionExit0( CCMMSESSION_CONNMETHODINOTHERDESTINATION_EXIT );
                    return ETrue;
                    }
                }
            }
        }

    OstTraceFunctionExit0( DUP1_CCMMSESSION_CONNMETHODINOTHERDESTINATION_EXIT );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// Check for restrictions for adding an embedded destination from destination
// instances of all clients.
// - aDestinationId is the ID of the destination where a destination is beeing
//   embedded.
// - aEmbeddedDestinationId is the ID of the destination that is beeing
//   embedded.
// -----------------------------------------------------------------------------
//
TBool CCmmSession::EmbeddedDestinationConflictsFromAllSessions(
        const TUint32& aDestinationId,
        const TUint32& aEmbeddedDestinationId )
    {
    OstTraceFunctionEntry0( CCMMSESSION_EMBEDDEDDESTINATIONCONFLICTSFROMALLSESSIONS_ENTRY );

    return iServer.EmbeddedDestinationConflictsFromAllSessions( aDestinationId, aEmbeddedDestinationId );
    }

// -----------------------------------------------------------------------------
// Check for restrictions for adding an embedded destination from destination
// instances of this session.
// - aDestinationId is the ID of the destination where a destination is beeing
//   embedded.
// - aEmbeddedDestinationId is the ID of the destination that is beeing
//   embedded.
//
// - Check that any destination instance does not contain aDestinationId as
//   embedded destination.
// - Check that any destination instance for aEmbeddedDestinationId does not
//   contain an embedded destination.
// -----------------------------------------------------------------------------
//
TBool CCmmSession::EmbeddedDestinationConflicts(
        const TUint32& aDestinationId,
        const TUint32& aEmbeddedDestinationId )
    {
    OstTraceFunctionEntry0( CCMMSESSION_EMBEDDEDDESTINATIONCONFLICTS_ENTRY );

    TBool result( EFalse );

    for ( TInt i = 0; i < iDestinationObjects->Count(); i++ )
        {
        CCmmDestinationInstance* destinationInstance =
                ( CCmmDestinationInstance* )( ( *iDestinationObjects )[i] );
        if ( destinationInstance )
            {
            if ( destinationInstance->HasEmbeddedWithId( aDestinationId ) ||
                    ( destinationInstance->GetId() == aEmbeddedDestinationId &&
                            destinationInstance->HasEmbedded() ) )
                {
                result = ETrue;
                break;
                }
            }
        }

    OstTraceFunctionExit0( CCMMSESSION_EMBEDDEDDESTINATIONCONFLICTS_EXIT );
    return result;
    }

// ---------------------------------------------------------------------------
// After update to database, refresh temporary ID to real ID if necessary and
// refresh status information for any related handles for all client sessions.
// ---------------------------------------------------------------------------
//
void CCmmSession::RefreshHandlesForAllSessions( const TCmmIdStruct& aIdStruct )
    {
    OstTraceFunctionEntry0( CCMMSESSION_REFRESHHANDLESFORALLSESSIONS_ENTRY );

    // If the ID structure contains a temporary ID, we need to update the
    // temporary ID to the real ID for all relevant handles in this session and
    // cache.
    if ( aIdStruct.iTemporaryId )
        {
        // Real ID tells if this is a destination or connection method.

        if ( aIdStruct.iRealId < KCmmConnMethodIdIntervalMax )
            {
            // Connection method. Need to iterate through all destination and
            // connection method handles.

            // Update the ID of the connection method on session side.
            for ( TInt i = 0; i < iConnMethodObjects->Count(); i++ )
                {
                CCmmConnMethodInstance* connMethodInstance =
                        ( CCmmConnMethodInstance* )( ( *iConnMethodObjects )[i] );
                if ( connMethodInstance &&
                        connMethodInstance->GetId() == aIdStruct.iTemporaryId )
                    {
                    connMethodInstance->SetId( aIdStruct.iRealId ); //TODO, need to set ID inside records?
                    break; // Can only be 1 match.
                    }
                }
            // Update the ID of the connection method in cache side also.
            iCache.RefreshConnMethodId( aIdStruct );

            // Iterate all destinations. If the connection method is in them,
            // update the ID to real ID.
            for ( TInt i = 0; i < iDestinationObjects->Count(); i++ )
                {
                CCmmDestinationInstance* destinationInstance =
                        ( CCmmDestinationInstance* )( ( *iDestinationObjects )[i] );
                if ( destinationInstance )
                    {
                    destinationInstance->RefreshConnMethodId( aIdStruct );
                    }
                }
            }
        else
            {
            // Destination. Need to iterate through all destination handles.
            for ( TInt i = 0; i < iDestinationObjects->Count(); i++ )
                {
                CCmmDestinationInstance* destinationInstance =
                        ( CCmmDestinationInstance* )( ( *iDestinationObjects )[i] );
                if ( destinationInstance &&
                        destinationInstance->GetId() == aIdStruct.iTemporaryId )
                    {
                    destinationInstance->SetId( aIdStruct.iRealId );
                    break; // Can only be 1 match.
                    }
                }
            // Update the ID of the destination in cache side also.
            iCache.RefreshDestinationId( aIdStruct );
            }
        }

    // Update status information for all related destination/connection method
    // handles in all client sessions.
    iServer.RefreshHandlesForAllSessions( aIdStruct.iRealId );

    OstTraceFunctionExit0( CCMMSESSION_REFRESHHANDLESFORALLSESSIONS_EXIT );
    }

// ---------------------------------------------------------------------------
// Removes a connection method from any open destination handle in this
// session.
// ---------------------------------------------------------------------------
//
void CCmmSession::RemoveConnMethodFromDestinationHandles(
        const TUint32& aConnMethodId )
    {
    OstTraceFunctionEntry0( CCMMSESSION_REMOVECONNMETHODFROMDESTINATIONHANDLES_ENTRY );

    for ( TInt i = 0; i < iDestinationObjects->Count(); i++ )
        {
        CCmmDestinationInstance* destinationInstance =
                ( CCmmDestinationInstance* )( ( *iDestinationObjects )[i] );
        if ( destinationInstance )
            {
            for ( TInt j = 0; j < destinationInstance->iConnMethodItemArray.Count(); j++ )
                {
                if ( destinationInstance->iConnMethodItemArray[j].iId == aConnMethodId )
                    {
                    destinationInstance->iConnMethodItemArray.Remove( j );
                    break;
                    }
                }
            }
        }

    OstTraceFunctionExit0( CCMMSESSION_REMOVECONNMETHODFROMDESTINATIONHANDLES_EXIT );
    }

// ---------------------------------------------------------------------------
// Notify this session destination/connection method handles about an
// updated/deleted destination/connection method.
// ---------------------------------------------------------------------------
//
void CCmmSession::RefreshHandles( const TUint32& aId ) const
    {
    OstTraceFunctionEntry0( CCMMSESSION_REFRESHHANDLES_ENTRY );

    // Destination or connection method.
    if ( aId < KCmmConnMethodIdIntervalMax )
        {
        // Connection method.
        for ( TInt i = 0; i < iConnMethodObjects->Count(); i++ )
            {
            CCmmConnMethodInstance* connMethodInstance =
                    ( CCmmConnMethodInstance* )( ( *iConnMethodObjects )[i] );
            if ( connMethodInstance && connMethodInstance->GetId() == aId )
                {
                connMethodInstance->SetStatus( ECmmConnMethodStatusChanged );
                }
            }
        }
    else
        {
        // Destination.
        for ( TInt i = 0; i < iDestinationObjects->Count(); i++ )
            {
            CCmmDestinationInstance* destinationInstance =
                    ( CCmmDestinationInstance* )( ( *iDestinationObjects )[i] );
            if ( destinationInstance && destinationInstance->GetId() == aId )
                {
                destinationInstance->SetStatus( ECmmDestinationStatusChanged ); //TODO, any record status need be set?
                }
            }
        }

    OstTraceFunctionExit0( CCMMSESSION_REFRESHHANDLES_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetBearerInfoIntL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetBearerInfoIntL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETBEARERINFOINTL_ENTRY );

    TUint32 bearerType( aMessage.Int0() );
    TUint32 attribute( aMessage.Int1() );

    TUint32 result = iCache.GetBearerInfoIntL( bearerType, attribute );
    TPckg<TUint32> resultPckg( result );
    aMessage.WriteL( 2, resultPckg );

    OstTraceFunctionExit0( CCMMSESSION_GETBEARERINFOINTL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetBearerInfoBoolL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetBearerInfoBoolL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETBEARERINFOBOOLL_ENTRY );

    TUint32 bearerType( aMessage.Int0() );
    TUint32 attribute( aMessage.Int1() );

    TBool result = iCache.GetBearerInfoBoolL( bearerType, attribute );
    TPckg<TBool> resultPckg( result );
    aMessage.WriteL( 2, resultPckg );

    OstTraceFunctionExit0( CCMMSESSION_GETBEARERINFOBOOLL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetBearerInfoStringL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetBearerInfoStringL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETBEARERINFOSTRINGL_ENTRY );

    TUint32 bearerType( aMessage.Int0() );
    TUint32 attribute( aMessage.Int1() );

    HBufC* result = iCache.GetBearerInfoStringL( bearerType, attribute );
    if ( !result )
        {
        OstTraceFunctionExit0( CCMMSESSION_GETBEARERINFOSTRINGL_EXIT );
        return;
        }
    CleanupStack::PushL( result );

    TInt bufferLen = aMessage.GetDesMaxLength( 2 );
    if ( result->Length() > bufferLen )
        {
        User::Leave( KErrArgument );
        }

    TPtrC resultPtr = result->Des();
    aMessage.WriteL( 2, resultPtr );
    CleanupStack::PopAndDestroy( result );

    OstTraceFunctionExit0( DUP1_CCMMSESSION_GETBEARERINFOSTRINGL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetBearerInfoString8L
// -----------------------------------------------------------------------------
//
void CCmmSession::GetBearerInfoString8L( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETBEARERINFOSTRING8L_ENTRY );

    TUint32 bearerType( aMessage.Int0() );
    TUint32 attribute( aMessage.Int1() );

    HBufC8* result = iCache.GetBearerInfoString8L( bearerType, attribute );
    if ( !result )
        {
        OstTraceFunctionExit0( CCMMSESSION_GETBEARERINFOSTRING8L_EXIT );
        return;
        }
    CleanupStack::PushL( result );

    TInt bufferLen = aMessage.GetDesMaxLength( 2 );
    if ( result->Length() > bufferLen )
        {
        User::Leave( KErrArgument );
        }

    TPtrC8 resultPtr = result->Des();
    aMessage.WriteL( 2, resultPtr );
    CleanupStack::PopAndDestroy( result );

    OstTraceFunctionExit0( DUP1_CCMMSESSION_GETBEARERINFOSTRING8L_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetConnMethodInfoIntL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetConnMethodInfoIntL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETCONNMETHODINFOINTL_ENTRY );

    TUint32 cmId( aMessage.Int0() );
    TUint32 attribute( aMessage.Int1() );

    TUint32 result = iCache.GetConnectionMethodInfoIntL( cmId, attribute );
    TPckg<TUint32> resultPckg( result );
    aMessage.WriteL( 2, resultPckg );

    OstTraceFunctionExit0( CCMMSESSION_GETCONNMETHODINFOINTL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetConnMethodInfoBoolL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetConnMethodInfoBoolL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETCONNMETHODINFOBOOLL_ENTRY );

    TUint32 cmId( aMessage.Int0() );
    TUint32 attribute( aMessage.Int1() );

    TBool result = iCache.GetConnectionMethodInfoBoolL( cmId, attribute );
    TPckg<TBool> resultPckg( result );
    aMessage.WriteL( 2, resultPckg );

    OstTraceFunctionExit0( CCMMSESSION_GETCONNMETHODINFOBOOLL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetConnMethodInfoStringL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetConnMethodInfoStringL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETCONNMETHODINFOSTRINGL_ENTRY );

    TUint32 cmId( aMessage.Int0() );
    TUint32 attribute( aMessage.Int1() );

    HBufC* result = iCache.GetConnectionMethodInfoStringL( cmId, attribute );
    if ( !result )
        {
        OstTraceFunctionExit0( CCMMSESSION_GETCONNMETHODINFOSTRINGL_EXIT );
        return;
        }
    CleanupStack::PushL( result );

    TInt bufferLen = aMessage.GetDesMaxLength( 2 );
    if ( result->Length() > bufferLen )
        {
        User::Leave( KErrArgument );
        }

    TPtrC resultPtr = result->Des();
    aMessage.WriteL( 2, resultPtr );
    CleanupStack::PopAndDestroy( result );

    OstTraceFunctionExit0( DUP1_CCMMSESSION_GETCONNMETHODINFOSTRINGL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetConnMethodInfoString8L
// -----------------------------------------------------------------------------
//
void CCmmSession::GetConnMethodInfoString8L( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETCONNMETHODINFOSTRING8L_ENTRY );

    TUint32 cmId( aMessage.Int0() );
    TUint32 attribute( aMessage.Int1() );

    HBufC8* result = iCache.GetConnectionMethodInfoString8L( cmId, attribute );
    if ( !result )
        {
        OstTraceFunctionExit0( CCMMSESSION_GETCONNMETHODINFOSTRING8L_EXIT );
        return;
        }
    CleanupStack::PushL( result );

    TInt bufferLen = aMessage.GetDesMaxLength( 2 );
    if ( result->Length() > bufferLen )
        {
        User::Leave( KErrArgument );
        }

    TPtrC8 resultPtr = result->Des();
    aMessage.WriteL( 2, resultPtr );
    CleanupStack::PopAndDestroy( result );

    OstTraceFunctionExit0( DUP1_CCMMSESSION_GETCONNMETHODINFOSTRING8L_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetConnMethodArrayL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetConnMethodArrayL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETCONNMETHODARRAYL_ENTRY );

    // Read attribute flags from client request.
    TPckgBuf<TCmmIpcStructGetConnMethods> parametersPckg;
    aMessage.ReadL( 0, parametersPckg );
    TCmmIpcStructGetConnMethods attributes = parametersPckg();

    // Create connection method ID array.
    RArray<TUint32> cmArray;
    CleanupClosePushL( cmArray );
    iCache.GetAllConnMethodsL( cmArray, attributes.iCheckBearerType );

    // Check if only legacy connection methods are needed.
    if ( attributes.iLegacyOnly )
        {
        // Remove all referenced connection methods.
        for ( TInt i = cmArray.Count() - 1; i >= 0; i-- )
            {
            if ( iCache.DestinationsContainingConnMethod( cmArray[i] ) )
                {
                cmArray.Remove( i );
                }
            }
        }

    // EasyWLAN handling.
    if ( !attributes.iEasyWlan )
        {
        // If there is an EasyWLAN IAP, the ID needs to be removed from the array.
        TUint32 easyWlanId = iCache.EasyWlanIdL();
        if ( easyWlanId != 0 )
            {
            TInt index = cmArray.Find( easyWlanId );
            if ( index != KErrNotFound )
                {
                cmArray.Remove( index );
                }
            }
        }

    // Check connection method ID count.
    TInt connMethodCount = cmArray.Count();
    if ( connMethodCount > aMessage.GetDesMaxLengthL( 2 ) )
        {
        // Client buffer is too small to contain all the informarmation. Return
        // without any answer and the client side will ask again using a bigger
        // buffer.
        CleanupStack::PopAndDestroy( &cmArray );
        return;
        }

    TPckg<TInt> countPckg( connMethodCount );
    aMessage.WriteL( 1, countPckg );

    if ( connMethodCount == 0 )
        {
        CleanupStack::PopAndDestroy( &cmArray );
        return;
        }

    // Write connection method IDs to client.
    HBufC8* idBuf = HBufC8::NewLC( connMethodCount );
    TPtr8 bufPtr( idBuf->Des() );
    for ( TInt i = 0; i < connMethodCount; i++ )
        {
        bufPtr.Append( (TUint8)( cmArray[i] & 0x000000FF ) );
        }
    aMessage.WriteL( 2, bufPtr );

    CleanupStack::PopAndDestroy( idBuf );
    CleanupStack::PopAndDestroy( &cmArray );

    OstTraceFunctionExit0( DUP1_CCMMSESSION_GETCONNMETHODARRAYL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetAllDestinationsL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetAllDestinationsL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETALLDESTINATIONSL_ENTRY );

    TInt destCount = iCache.GetDestinationCount();
    if ( destCount > aMessage.GetDesMaxLengthL( 1 ) )
        {
        // Client buffer is too small to contain all the informarmation. Return
        // without any answer and the client side will ask again using a bigger
        // buffer.
        return;
        }

    // Write the destination count to client.
    TPckg<TInt> countPckg( destCount );
    aMessage.WriteL( 0, countPckg );

    if ( destCount == 0 )
        {
        return;
        }

    // Get the destination IDs from database cache.
    RArray<TUint32> destArray;
    CleanupClosePushL( destArray );
    iCache.GetDestinationsL( destArray );

    HBufC16* idBuf = HBufC16::NewLC( destCount );
    TPtr16 bufPtr( idBuf->Des() );
    for ( TInt i = 0; i < destCount; i++ )
        {
        bufPtr.Append( destArray[i] );
        }

    // Write the destination IDs to client.
    aMessage.WriteL( 1, bufPtr );

    CleanupStack::PopAndDestroy( idBuf );
    CleanupStack::PopAndDestroy( &destArray );

    OstTraceFunctionExit0( DUP1_CCMMSESSION_GETALLDESTINATIONSL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetEasyWLANIdL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetEasyWLANIdL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETEASYWLANIDL_ENTRY );

    TUint32 id = iCache.EasyWlanIdL();
    TPckg<TUint32> idPckg( id );
    aMessage.WriteL( 0, idPckg );

    OstTraceFunctionExit0( CCMMSESSION_GETEASYWLANIDL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetSupportedBearersL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetSupportedBearersL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETSUPPORTEDBEARERSL_ENTRY );

    // Get the supported bearer types.
    RArray<TUint32> bearerArray;
    CleanupClosePushL( bearerArray );
    iServer.CmManager()->SupportedBearersL( bearerArray );

    // Write bearer count to client.
    TInt bearerCount( bearerArray.Count() );
    TInt neededBufferSize( bearerCount * sizeof( TUint32 ) );
    TPckg<TInt> countPckg( bearerCount );
    aMessage.WriteL( 0, countPckg );

    // Check the client buffer size.
    if ( neededBufferSize > aMessage.GetDesMaxLengthL( 1 ) || bearerCount == 0 )
        {
        // Client buffer is too small to contain all the information. Return
        // with only the bearer count information and the client side will ask
        // again using a big enough buffer.
        CleanupStack::PopAndDestroy( &bearerArray );
        return;
        }

    // Add the bearer types into a buffer.
    HBufC8* bearerBuf = HBufC8::NewLC( neededBufferSize );
    TPtr8 ptr( bearerBuf->Des() );
    for ( TInt i = 0; i < bearerCount; i++ )
        {
        TUint32 a( bearerArray[i] );
        ptr.Append( (TUint8*)&a, 4 );
        }

    // Write the bearer types to client.
    aMessage.WriteL( 1, ptr );

    CleanupStack::PopAndDestroy( bearerBuf );
    CleanupStack::PopAndDestroy( &bearerArray );

    OstTraceFunctionExit0( DUP1_CCMMSESSION_GETSUPPORTEDBEARERSL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetUncategorizedIconL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetUncategorizedIconL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETUNCATEGORIZEDICONL_ENTRY );

    HBufC* result = KNullDesC().AllocLC(); //TODO, Is there an uncategorized icon?

    TInt bufferLen = aMessage.GetDesMaxLength( 0 );
    if ( result->Length() > bufferLen )
        {
        User::Leave( KErrArgument );
        }

    TPtrC resultPtr = result->Des();
    aMessage.WriteL( 0, resultPtr );
    CleanupStack::PopAndDestroy( result );

    OstTraceFunctionExit0( CCMMSESSION_GETUNCATEGORIZEDICONL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::ReadDefaultConnectionL
// -----------------------------------------------------------------------------
//
void CCmmSession::ReadDefaultConnectionL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_READDEFAULTCONNECTIONL_ENTRY );

    // Default connection is now simply the internet destination.
    TCmDefConnValue defaultConnection;
    defaultConnection.iType = ECmDefConnDestination;
    defaultConnection.iId = 0;

    iCache.InternetDestinationIdL( defaultConnection.iId );
    if ( defaultConnection.iId == 0 )
        {
        User::Leave( KErrNotFound );
        }

    TPckgBuf<TCmDefConnValue> dcPckgBuf( defaultConnection );
    aMessage.WriteL( 0, dcPckgBuf );

    OstTraceFunctionExit0( CCMMSESSION_READDEFAULTCONNECTIONL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::ReadGenConnSettingsL
// -----------------------------------------------------------------------------
//
void CCmmSession::ReadGenConnSettingsL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_READGENCONNSETTINGSL_ENTRY );

    TCmGenConnSettings genConnSettings;
    iCache.ReadGenConnSettingsL( genConnSettings );

    TPckgBuf<TCmGenConnSettings> genConnSettingsPckgBuf( genConnSettings );
    aMessage.WriteL( 0, genConnSettingsPckgBuf );

    OstTraceFunctionExit0( CCMMSESSION_READGENCONNSETTINGSL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::WriteGenConnSettingsL
// -----------------------------------------------------------------------------
//
void CCmmSession::WriteGenConnSettingsL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_WRITEGENCONNSETTINGSL_ENTRY );

    // Read data from client request.
    TPckgBuf<TCmGenConnSettings> genConnSettingsPckgBuf;
    aMessage.ReadL( 0, genConnSettingsPckgBuf );
    TCmGenConnSettings genConnSettings = genConnSettingsPckgBuf();

    iCache.WriteGenConnSettingsL( genConnSettings );

    OstTraceFunctionExit0( CCMMSESSION_WRITEGENCONNSETTINGSL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetBearerPriorityArrayL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetBearerPriorityArrayL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETBEARERPRIORITYARRAYL_ENTRY );

    const TInt maxLength( aMessage.GetDesMaxLengthL( 0 ) );
    if ( maxLength < 2 )
        {
        // Minimum length of 2 is needed to store count and needed length.
        User::Leave( KErrArgument );
        }

    RPointerArray<CCmmBearerPriority> bearerPriorityArray;
    CmmCleanupResetAndDestroyPushL( bearerPriorityArray );

    iCache.CopyBearerPriorityArrayL( bearerPriorityArray );
    const TInt bearerCountInArray( bearerPriorityArray.Count() );
    TInt bearerCount( bearerCountInArray );

    TInt maxBufLen( 2 );
    // Check the length needed for serializing.
    for ( TInt i = 0; i < bearerCountInArray; i++ )
        {
        // Skip if service type is not valid.
        if ( bearerPriorityArray[i]->ServiceType() && bearerPriorityArray[i]->ServiceType()->Length() > 0 )
            {
            maxBufLen += KCmmBearerPriorityHeaderLength; // Priorities and servicetype length.
            maxBufLen += bearerPriorityArray[i]->ServiceType()->Length();
            }
        else
            {
            bearerCount--;
            }
        }

    // If given buffer is shorter than needed, just write count and needed length.
    if ( maxBufLen > maxLength )
        {
        HBufC* buffer = HBufC::NewLC( KCmmDefaultBearerPriorityArraySize );
        TPtr bufferPtr( buffer->Des() );

        bufferPtr.Append( bearerCount );
        bufferPtr.Append( maxBufLen );
        aMessage.WriteL( 0, bufferPtr );

        CleanupStack::PopAndDestroy( buffer );
        CleanupStack::PopAndDestroy( &bearerPriorityArray );
        OstTraceFunctionExit0( CCMMSESSION_GETBEARERPRIORITYARRAYL_EXIT );
        return;
        }

    // Add needed buffer + space for bearerCount and maxlength.
    HBufC* buffer = HBufC::NewLC( maxBufLen );
    TPtr bufferPtr( buffer->Des() );

    // Write count and bufmaxLen.
    bufferPtr.Append( bearerCount );
    bufferPtr.Append( maxBufLen );

    for ( TInt i = 0; i < bearerCountInArray; i++ )
        {
        // Skip if service type is not valid.
        if ( bearerPriorityArray[i]->ServiceType() && bearerPriorityArray[i]->ServiceType()->Length() > 0 )
            {
            TUint32 priority = bearerPriorityArray[i]->Priority();
            TUint32 uiPriority = bearerPriorityArray[i]->UiPriority();
            const HBufC* serviceType = bearerPriorityArray[i]->ServiceType();

            bufferPtr.Append( priority >> KBitsInTwoBytes );
            bufferPtr.Append( priority & 0x0000FFFF );
            bufferPtr.Append( uiPriority >> KBitsInTwoBytes );
            bufferPtr.Append( uiPriority & 0x0000FFFF );

            const TInt stringLength = serviceType->Length();
            bufferPtr.Append( stringLength );
            bufferPtr.Append( *serviceType );
            }
        }
    aMessage.WriteL( 0, bufferPtr );

    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PopAndDestroy( &bearerPriorityArray );

    OstTraceFunctionExit0( DUP1_CCMMSESSION_GETBEARERPRIORITYARRAYL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::UpdateBearerPriorityArrayL
// -----------------------------------------------------------------------------
//
void CCmmSession::UpdateBearerPriorityArrayL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_UPDATEBEARERPRIORITYARRAYL_ENTRY );

    HBufC* bearerPriorityBuf = HBufC::NewLC( aMessage.GetDesMaxLengthL( 0 ) );
    TPtr bearerPriorityBufPtr( bearerPriorityBuf->Des() );

    aMessage.ReadL( 0, bearerPriorityBufPtr );

    const TInt bearerCount = bearerPriorityBufPtr[0];
    if ( ( bearerCount < 1 ) || ( aMessage.GetDesMaxLengthL( 0 ) < 2 ) )
        {
        User::Leave( KErrArgument );
        }

    RPointerArray<CCmmBearerPriority> bearerPriorityArray;
    CmmCleanupResetAndDestroyPushL( bearerPriorityArray );

    TInt position( 2 ); // Start of first priority item.
    for ( TInt i = 0; i < bearerCount; i++ )
        {
        TUint32 priority;
        TUint32 uiPriority;
        priority = bearerPriorityBufPtr[position] << KBitsInTwoBytes;
        position++;
        priority += bearerPriorityBufPtr[position];
        position++;
        uiPriority = bearerPriorityBufPtr[position] << KBitsInTwoBytes;
        position++;
        uiPriority += bearerPriorityBufPtr[position];
        position++;
        const TInt stringLength = bearerPriorityBufPtr[position];
        position++;
        if ( stringLength <= 0 )
            {
            User::Leave( KErrArgument );
            }
        else
            {
            HBufC* serviceName = HBufC::NewLC( stringLength );
            serviceName->Des().Append( &( bearerPriorityBufPtr[position] ), stringLength );
            position += stringLength;

            TPtrC tempServiceType( serviceName->Des() );
            CCmmBearerPriority* item = CCmmBearerPriority::NewLC( tempServiceType, priority, uiPriority );
            bearerPriorityArray.AppendL( item );
            CleanupStack::Pop( item );

            CleanupStack::PopAndDestroy( serviceName );
            }
        }

    // Update bearer priority array
    iCache.UpdateBearerPriorityArrayL( bearerPriorityArray );

    CleanupStack::PopAndDestroy( &bearerPriorityArray );
    CleanupStack::PopAndDestroy( bearerPriorityBuf );

    OstTraceFunctionExit0( CCMMSESSION_UPDATEBEARERPRIORITYARRAYL_EXIT );
    }

// -----------------------------------------------------------------------------
// Copies a connection method into a destination. If the connection method is
// in any other destination, it becomes shared. Calls update on the target
// destination.
// -----------------------------------------------------------------------------
//
void CCmmSession::CopyConnMethodL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_COPYCONNMETHODL_ENTRY );

    CCmmDestinationInstance* destination = ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int0() );
    CCmmConnMethodInstance* connMethod = ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int1() );

    // Can't add an embedded destination this way.
    if ( connMethod->IsEmbeddedDestination() )
        {
        User::Leave( KErrArgument );
        }

    // Check the protection level of the destination.
    // And based on that check the needed capabilities from the client.
    CMManager::TProtectionLevel protLevel( CMManager::EProtLevel0 );
    destination->GetProtectionL( protLevel );
    CPolicyServer::TCustomResult capabilities( CPolicyServer::EPass );
    if ( protLevel == CMManager::EProtLevel1 )
        {
        capabilities = iServer.CapabilityCheckWithProtection( aMessage );
        }

    if ( capabilities == CPolicyServer::EFail )
        {
        User::Leave( KErrPermissionDenied );
        }

    // Add connection method into destination.
    TInt index = destination->AddConnMethodL( *connMethod );

    // Update destination into database.
    destination->UpdateL();

    // Write the connection method index (priority) inside the destination to client.
    TPckg<TInt> indexPckg( index );
    aMessage.WriteL( 2, indexPckg );

    OstTraceFunctionExit0( CCMMSESSION_COPYCONNMETHODL_EXIT );
    }

// -----------------------------------------------------------------------------
// Moves a connection method from one destination to another. Calls update on
// both the source and target destinations (which also updates the connection
// method).
// -----------------------------------------------------------------------------
//
void CCmmSession::MoveConnMethodL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_MOVECONNMETHODL_ENTRY );

    // Read data from client request.
    TPckgBuf<TCmmIpcStructMoveConnMethod> attributesPckg;
    aMessage.ReadL( 0, attributesPckg );
    TCmmIpcStructMoveConnMethod attributes = attributesPckg();

    CCmmDestinationInstance* sourceDestination =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( attributes.iSourceDestHandle );
    CCmmDestinationInstance* targetDestination =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( attributes.iTargetDestHandle );
    CCmmConnMethodInstance* connMethod =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( attributes.iConnMethodHandle );
    TUint32 connMethodId( connMethod->GetId() );

    // Can't add an embedded destination this way.
    if ( connMethod->IsEmbeddedDestination() )
        {
        User::Leave( KErrArgument );
        }
    // Check if the connection method is in the source destination.
    if ( !sourceDestination->ValidConnMethodIdInDestinationIncludeEmbedded( connMethodId ) )
        {
        User::Leave( KErrNotFound );
        }
    // Check if the connection method is already in the target destination.
    if ( targetDestination->ValidConnMethodIdInDestinationIncludeEmbedded( connMethodId ) )
        {
        User::Leave( KErrAlreadyExists );
        }
    // Check if the connection method can be removed from the source destination.
    if ( sourceDestination->ConnMethodInDestinationButLocked( connMethodId ) )
        {
        User::Leave( KErrLocked );
        }
    // Check that the connection method is not in use by an active connection.
    if ( iCache.CheckIfCmConnected( connMethodId ) )
        {
        User::Leave( KErrInUse );
        }

    // Check the protection level of the source and target destination.
    // And based on those check the needed capabilities from the client.
    TBool protectionExists( EFalse );
    CMManager::TProtectionLevel protLevelSource( CMManager::EProtLevel0 );
    sourceDestination->GetProtectionL( protLevelSource );
    if ( protLevelSource == CMManager::EProtLevel1 
            || protLevelSource == CMManager::EProtLevel3 )
        {
        protectionExists = ETrue;
        }

    // If source destination is not protected check the target destination.
    if ( !protectionExists )
        {
        CMManager::TProtectionLevel protLevelTarget( CMManager::EProtLevel0 );
        targetDestination->GetProtectionL( protLevelTarget );
        if ( protLevelTarget == CMManager::EProtLevel1 )
            {
            protectionExists = ETrue;
            }
        }

    CPolicyServer::TCustomResult capabilities( CPolicyServer::EPass );
    if ( protectionExists )
        {
        capabilities = iServer.CapabilityCheckWithProtection( aMessage );
        }

    if ( capabilities == CPolicyServer::EFail )
        {
        User::Leave( KErrPermissionDenied );
        }

    // Add connection method into target destination and update it.
    attributesPckg().iIndex = targetDestination->AddConnMethodL( *connMethod );
    targetDestination->UpdateL();

    // Remove connection method from source destination and update it.
    sourceDestination->RemoveConnMethodFromDestinationL( *connMethod, EFalse ); // EFalse to not check connected state again.
    sourceDestination->UpdateL();

    // Write the index (priority) back to client.
    aMessage.WriteL( 0, attributesPckg );

    OstTraceFunctionExit0( CCMMSESSION_MOVECONNMETHODL_EXIT );
    }

// -----------------------------------------------------------------------------
// Remove a connection method froma destination. Does not call update on the
// affected destination.
// -----------------------------------------------------------------------------
//
void CCmmSession::RemoveConnMethodL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_REMOVECONNMETHODL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int0() );
    CCmmConnMethodInstance* connMethodInstance =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int1() );

    // Check the protection level of the destination.
    // And based on that check the needed capabilities from the client.
    CMManager::TProtectionLevel protLevel( CMManager::EProtLevel0 );
    destinationInstance->GetProtectionL( protLevel );
    CPolicyServer::TCustomResult capabilities( CPolicyServer::EPass );
    if ( protLevel == CMManager::EProtLevel1 || protLevel == CMManager::EProtLevel3 )
        {
        capabilities = iServer.CapabilityCheckWithProtection( aMessage );
        }

    if ( capabilities == CPolicyServer::EFail )
        {
        User::Leave( KErrPermissionDenied );
        }

    destinationInstance->RemoveConnMethodFromDestinationL( *connMethodInstance );
    destinationInstance->UpdateL();

    OstTraceFunctionExit0( CCMMSESSION_REMOVECONNMETHODL_EXIT );
    }

// -----------------------------------------------------------------------------
// Removes a connection method from every destination and make it uncategorized.
// Updates any destination that contains the connection method.
// -----------------------------------------------------------------------------
//
void CCmmSession::RemoveAllReferencesL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_REMOVEALLREFERENCESL_ENTRY );

    CCmmConnMethodInstance* connMethodInstance =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int0() );

    TBool capabilityCheckNeeded( EFalse );
    iCache.CheckIfConnMethodBelongsToProtectedDestinationL(
            *connMethodInstance,
            capabilityCheckNeeded );
    if ( capabilityCheckNeeded )
        {
        CPolicyServer::TCustomResult capabilities( CPolicyServer::EPass );
        capabilities = iServer.CapabilityCheckWithProtection( aMessage );
        if ( capabilities == CPolicyServer::EFail )
            {
            User::Leave( KErrPermissionDenied );
            }
        }

    iCache.CheckIfConnMethodReferencesCanBeRemovedL( *connMethodInstance );
    iCache.RemoveAllReferencesToConnMethodL( *connMethodInstance );

    OstTraceFunctionExit0( CCMMSESSION_REMOVEALLREFERENCESL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::ServiceDestinationL
// -----------------------------------------------------------------------------
//
void CCmmSession::ServiceDestinationL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_SERVICEDESTINATIONL_ENTRY );

    switch ( aMessage.Function() )
        {
        case EDestGetDestination:
            {
            GetDestinationL( aMessage );
            }
            break;
        case EDestRefresh:
            {
            RefreshDestinationL( aMessage );
            }
            break;
        case EDestCreateDestinationWithName:
            {
            CreateDestinationWithNameL( aMessage );
            }
            break;
        case EDestCreateDestinationWithNameAndId:
            {
            CreateDestinationWithNameAndIdL( aMessage );
            }
            break;
        case EDestCloseDestination:
            {
            CloseDestination( aMessage );
            }
            break;
        case EDestGetConnMethodCount:
            {
            GetConnectionMehodCountL( aMessage );
            }
            break;
        case EDestGetConnMethodPriority:
            {
            GetConnMethodPriorityL( aMessage );
            }
            break;
        case EDestGetName:
            {
            GetDestinationNameL( aMessage );
            }
            break;
        case EDestGetId:
            {
            GetDestinationIdL( aMessage );
            }
            break;
        case EDestGetElementId:
            {
            GetDestinationElementIdL( aMessage );
            }
            break;
        case EDestMetadata:
            {
            GetDestinationMetadataL( aMessage );
            }
            break;
        case EDestGetProtectionLevel:
            {
            GetDestinationProtectionLevelL( aMessage );
            }
            break;
        case EDestIsConnected:
            {
            IsDestinationConnectedL( aMessage );
            }
            break;
        case EDestIsHidden:
            {
            IsDestinationHiddenL( aMessage );
            }
            break;
        case EDestIsEqual:
            {
            DestinationIsEqualL( aMessage );
            }
            break;
        case EDestGetIcon:
            {
            GetDestinationIconL( aMessage );
            }
            break;
        case EDestAddConnMethod:
            {
            DestAddConnMethodL( aMessage );
            }
            break;
        case EDestAddEmbeddedDestination:
            {
            DestAddEmbeddedDestinationL( aMessage );
            }
            break;
        case EDestDeleteConnMethod:
            {
            DestDeleteConnMethodL( aMessage );
            }
            break;
        case EDestRemoveConnMethod:
            {
            DestRemoveConnMethodL( aMessage );
            }
            break;
        case EDestModifyPriority:
            {
            ModifyConnMethodPriorityL( aMessage );
            }
            break;
        case EDestSetName:
            {
            SetDestinationNameL( aMessage );
            }
            break;
        case EDestSetMetadata:
            {
            SetDestinationMetadataL( aMessage );
            }
            break;
        case EDestSetProtection:
            {
            SetDestinationProtectionL( aMessage );
            }
            break;
        case EDestSetHidden:
            {
            SetDestinationHiddenL( aMessage );
            }
            break;
        case EDestUpdate:
            {
            UpdateDestinationL( aMessage );
            }
            break;
        case EDestDelete:
            {
            DeleteDestinationL( aMessage );
            }
            break;
        case EDestSetIcon:
            {
            SetDestinationIconL( aMessage );
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMMSESSION_SERVICEDESTINATIONL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetDestinationL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetDestinationL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETDESTINATIONL_ENTRY );

    // Structure:
    // - Check arguments.
    // - Create session instance.
    // - Ask cache to open/create target and put data to session instance.
    // - Create subsession and complete message.

    // API side checks that ID is between 0x1000 - 0x1100, and converts if necessary.
    TUint32 destinationId( aMessage.Int0() );

    // Check that the ID is in valid range.
    if ( destinationId <= KCmDefaultDestinationAPTagId ||
            destinationId >= KCmMaxDestinationAPTagId )
        {
        User::Leave( KErrArgument );
        }

    // Check if a destination with given ID exists.
    if ( !iCache.DestinationExistsWithId( destinationId ) )
        {
        User::Leave( KErrNotFound );
        }

    // If this session already has a destination with matching ID open, provide
    // the client with a reference to the already opened handle.
    CCmmDestinationInstance* destinationInstance = FindDestinationInstanceById( destinationId );
    if ( destinationInstance )
        {
        TPckg<TInt> existingHandlePckg( destinationInstance->GetHandle() );
        aMessage.WriteL( 1, existingHandlePckg );
        User::Leave( KErrAlreadyExists );
        }
    destinationInstance = NULL;
    TPckg<TInt> existingHandlePckg( 0 );
    aMessage.WriteL( 1, existingHandlePckg );

    destinationInstance = CCmmDestinationInstance::NewLC( this, &iCache );

    // Cache will open a handle to the destination if not already open, and
    // copy relevant data into this destination instance.
    iCache.OpenDestinationL( *destinationInstance, destinationId );

    iDestinationContainer->AddL( ( CObject* ) destinationInstance );
    TInt handle = iDestinationObjects->AddL( ( CObject* ) destinationInstance );
    destinationInstance->SetHandle( handle );
    CleanupStack::Pop( destinationInstance );

    TPckg<TInt> handlePckg( handle );
    TInt err = aMessage.Write( 3, handlePckg );
    if ( err )
        {
        // Removes from object index and destroys the object.
        iDestinationObjects->Remove( handle );
        User::Leave( err );
        }

    OstTraceFunctionExit0( CCMMSESSION_GETDESTINATIONL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::RefreshDestinationL
// -----------------------------------------------------------------------------
//
void CCmmSession::RefreshDestinationL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_REFRESHDESTINATIONL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    iCache.RefreshDestinationL( *destinationInstance );

    OstTraceFunctionExit0( CCMMSESSION_REFRESHDESTINATIONL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::CreateDestinationWithNameL
// -----------------------------------------------------------------------------
//
void CCmmSession::CreateDestinationWithNameL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_CREATEDESTINATIONWITHNAMEL_ENTRY );

    // Structure:
    // - Check arguments.
    // - Create session instance.
    // - Ask cache to open/create target and put data to session instance.
    // - Create subsession and complete message.

    // Load and check name.
    TInt destNameLength = aMessage.GetDesLength( 0 );
    if ( destNameLength <= 0 )
        {
        User::Leave( KErrArgument );
        }

    HBufC* destinationName = HBufC::NewLC( destNameLength );
    TPtr ptrDestinationName = destinationName->Des();
    aMessage.ReadL( 0, ptrDestinationName );

    // Check if a destination with given name exists (or is already created but not yet saved).
    if ( iCache.DestinationExistsWithNameL( *destinationName, 0 ) ||
            iCache.NotSavedDestinationOpenWithNameL( *destinationName, 0 ) )
        {
        User::Leave( KErrAlreadyExists );
        }

    CCmmDestinationInstance* destinationInstance = CCmmDestinationInstance::NewLC( this, &iCache );

    // Create a temporary ID to be used until a real one is obtained from the database.
    TUint32 temporaryId = iCache.NextFreeTemporaryId();

    // Cache will create the destination data structure and copy the relevant
    // data to the session instance.
    iCache.CreateDestinationL( *destinationInstance, *destinationName, temporaryId );

    iDestinationContainer->AddL( ( CObject* ) destinationInstance );
    TInt handle = iDestinationObjects->AddL( ( CObject* ) destinationInstance );
    destinationInstance->SetHandle( handle );
    CleanupStack::Pop( destinationInstance );
    CleanupStack::PopAndDestroy( destinationName );

    TPckg<TInt> handlePckg( handle );
    TInt err = aMessage.Write( 3, handlePckg );
    if ( err )
        {
        // Removes from object index and destroys the object.
        iDestinationObjects->Remove( handle );
        User::Leave( err );
        }

    OstTraceFunctionExit0( CCMMSESSION_CREATEDESTINATIONWITHNAMEL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::CreateDestinationWithNameAndIdL
// -----------------------------------------------------------------------------
//
void CCmmSession::CreateDestinationWithNameAndIdL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_CREATEDESTINATIONWITHNAMEANDIDL_ENTRY );

    // Structure:
    // - Check arguments.
    // - Create session instance.
    // - Ask cache to open/create target and put data to session instance.
    // - Create subsession and complete message.

    // API side checks that ID is between 0x1000 - 0x1100, and converts if necessary.
    TUint32 destinationId( (TUint32)aMessage.Int1() );

    // Check that the ID is in valid range.
    if ( destinationId <= KCmDefaultDestinationAPTagId ||
            destinationId >= KCmMaxDestinationAPTagId )
        {
        User::Leave( KErrArgument );
        }

    // Check if a destination with given ID exists (or is already created but not saved).
    if ( iCache.DestinationExistsWithId( destinationId ) ||
            iCache.DestinationOpenWithId( destinationId ) )
        {
        User::Leave( KErrAlreadyExists );
        }

    // Load and check name.
    TInt destNameLength = aMessage.GetDesLength( 0 );
    if ( destNameLength <= 0 )
        {
        User::Leave( KErrArgument );
        }

    HBufC* destinationName = HBufC::NewLC( destNameLength );

    TPtr ptrDestinationName = destinationName->Des();
    aMessage.ReadL( 0, ptrDestinationName );

    // Check if a destination with given name exists (or is already created but not yet saved).
    if ( iCache.DestinationExistsWithNameL( *destinationName, 0 ) ||
            iCache.NotSavedDestinationOpenWithNameL( *destinationName, 0 ) )
        {
        User::Leave( KErrAlreadyExists );
        }

    CCmmDestinationInstance* destinationInstance = CCmmDestinationInstance::NewLC( this, &iCache );

    // Cache will create the destination data structure and copy the relevant
    // data to the session instance.
    iCache.CreateDestinationL( *destinationInstance, *destinationName, destinationId );

    iDestinationContainer->AddL( ( CObject* ) destinationInstance );
    TInt handle = iDestinationObjects->AddL( ( CObject* ) destinationInstance );
    destinationInstance->SetHandle( handle );
    CleanupStack::Pop( destinationInstance );
    CleanupStack::PopAndDestroy( destinationName );

    TPckg<TInt> handlePckg( handle );
    TInt err = aMessage.Write( 3, handlePckg );
    if ( err )
        {
        // Removes from object index and destroys the object.
        iDestinationObjects->Remove( handle );
        User::Leave( err );
        }

    OstTraceFunctionExit0( CCMMSESSION_CREATEDESTINATIONWITHNAMEANDIDL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::CloseDestinationL
// -----------------------------------------------------------------------------
//
void CCmmSession::CloseDestination( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_CLOSEDESTINATION_ENTRY );

    // Check first that the destination instance exists.
    CObject* destinationObject = iDestinationObjects->At( aMessage.Int3() );
    if ( destinationObject )
        {
        // Destination instance destructor will notify cache.
        iDestinationObjects->Remove( aMessage.Int3() );
        }

    OstTraceFunctionExit0( CCMMSESSION_CLOSEDESTINATION_EXIT );
    }

// -----------------------------------------------------------------------------
// Return the connection method count from the provided destination.
// Embedded destinations are included.
// -----------------------------------------------------------------------------
//
void CCmmSession::GetConnectionMehodCountL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETCONNECTIONMEHODCOUNTL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    TInt connMethodCount( destinationInstance->iConnMethodItemArray.Count() ); // Includes embedded destinations.

    TPckg<TInt> countPckg( connMethodCount );
    aMessage.WriteL( 0, countPckg );

    OstTraceFunctionExit0( CCMMSESSION_GETCONNECTIONMEHODCOUNTL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetConnMethodPriorityL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetConnMethodPriorityL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETCONNMETHODPRIORITYL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    CCmmConnMethodInstance* connMethodInstance =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int0() );
    TUint32 connMethodId = connMethodInstance->GetId();

    TInt index( KErrNotFound );
    for ( TInt i = 0; i < destinationInstance->iConnMethodItemArray.Count(); i++ )
        {
        if ( destinationInstance->iConnMethodItemArray[i].iId == connMethodId )
            {
            // Index in the connection method array is the connection method
            // priority. But priority starts from 1, so adjust by +1.
            index = i + 1;
            break;
            }
        }

    // Leave if given connection method is not inside this destination.
    User::LeaveIfError( index );

    // Check if the connection method is an embedded destination.
    if ( connMethodInstance->GetBearerType() == KUidEmbeddedDestination )
        {
        index = CMManager::KDataMobilitySelectionPolicyPriorityWildCard;
        }

    //TODO, what if CM is virtual?

    TUint priority = ( TUint )index;
    TPckg<TUint> priorityPckg( priority );
    aMessage.WriteL( 1, priorityPckg );

    OstTraceFunctionExit0( CCMMSESSION_GETCONNMETHODPRIORITYL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetDestinationNameL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetDestinationNameL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETDESTINATIONNAMEL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    HBufC* name = destinationInstance->GetLocalisedDestinationNameL();
    CleanupStack::PushL( name );
    TPtrC namePtr( name->Des() );

    TInt bufferLen = aMessage.GetDesMaxLength( 0 );
    if ( namePtr.Length() > bufferLen )
        {
        User::Leave( KErrArgument );
        }

    aMessage.WriteL( 0, namePtr );

    CleanupStack::PopAndDestroy( name );

    OstTraceFunctionExit0( CCMMSESSION_GETDESTINATIONNAMEL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetDestinationIconL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetDestinationIconL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETDESTINATIONICONL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    HBufC* icon = destinationInstance->GetDestinationIconL();
    CleanupStack::PushL( icon );
    TPtrC iconPtr( icon->Des() );

    TInt bufferLen = aMessage.GetDesMaxLength( 0 );
    if ( iconPtr.Length() > bufferLen )
        {
        User::Leave( KErrArgument );
        }

    aMessage.WriteL( 0, iconPtr );
    CleanupStack::PopAndDestroy( icon );

    OstTraceFunctionExit0( CCMMSESSION_GETDESTINATIONICONL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetDestinationIdL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetDestinationIdL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETDESTINATIONIDL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    TUint32 id = destinationInstance->GetRecordTagFromApRecordL();
    TPckg<TUint32> idPckg( id );
    aMessage.WriteL( 0, idPckg );

    OstTraceFunctionExit0( CCMMSESSION_GETDESTINATIONIDL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetDestinationElementIdL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetDestinationElementIdL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETDESTINATIONELEMENTIDL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    TUint32 elementId = destinationInstance->GetElementIdL();
    TPckg<TUint32> elementIdPckg( elementId );
    aMessage.WriteL( 0, elementIdPckg );

    OstTraceFunctionExit0( CCMMSESSION_GETDESTINATIONELEMENTIDL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetDestinationMetadataL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetDestinationMetadataL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETDESTINATIONMETADATAL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );
    CMManager::TSnapMetadataField metadataField =
            ( CMManager::TSnapMetadataField )aMessage.Int0();

    TUint32 metadata( 0 );
    destinationInstance->GetMetadataL( metadataField, metadata );

    TPckg<TUint32> metadataPckg( metadata );
    aMessage.WriteL( 1, metadataPckg );

    OstTraceFunctionExit0( CCMMSESSION_GETDESTINATIONMETADATAL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetDestinationProtectionLevelL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetDestinationProtectionLevelL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETDESTINATIONPROTECTIONLEVELL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    CMManager::TProtectionLevel protLevel( CMManager::EProtLevel0 );
    destinationInstance->GetProtectionL( protLevel );

    TPckg<TInt> protLevelPckg( protLevel );
    aMessage.WriteL( 0, protLevelPckg );

    OstTraceFunctionExit0( CCMMSESSION_GETDESTINATIONPROTECTIONLEVELL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::IsDestinationConnectedL
// -----------------------------------------------------------------------------
//
void CCmmSession::IsDestinationConnectedL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_ISDESTINATIONCONNECTEDL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    if ( !iCache.DestinationExistsWithId( destinationInstance->GetId() ) )
        {
        User::Leave( KErrNotFound );
        }

    TBool isConnected = iCache.DestinationConnectedL( destinationInstance->GetId() );
    TPckg<TBool> isConnectedPckg( isConnected );
    aMessage.WriteL( 0, isConnectedPckg );

    OstTraceFunctionExit0( CCMMSESSION_ISDESTINATIONCONNECTEDL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::IsDestinationHiddenL
// -----------------------------------------------------------------------------
//
void CCmmSession::IsDestinationHiddenL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_ISDESTINATIONHIDDENL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    TUint32 isHiddenMetadata( 0 );
    destinationInstance->GetMetadataL( CMManager::ESnapMetadataHiddenAgent, isHiddenMetadata );

    TBool isHidden( EFalse );
    if ( isHiddenMetadata )
        {
        isHidden = ETrue;
        }

    TPckg<TBool> isHiddenPckg( isHidden );
    aMessage.WriteL( 0, isHiddenPckg );

    OstTraceFunctionExit0( CCMMSESSION_ISDESTINATIONHIDDENL_EXIT );
    }

// -----------------------------------------------------------------------------
// Checks if the destination given as parameter is equal to this destination
// -----------------------------------------------------------------------------
//
void CCmmSession::DestinationIsEqualL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_DESTINATIONISEQUALL_ENTRY );

    CCmmDestinationInstance* dest =
        ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    CCmmDestinationInstance* destToBeCompared =
        ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int0() );

    TBool equal( EFalse );
    if ( dest->GetId() == destToBeCompared->GetId() )
        {
        equal = ETrue;
        }

    TPckg<TBool> equalPckg( equal );

    aMessage.WriteL( 1, equalPckg );

    OstTraceFunctionExit0( CCMMSESSION_DESTINATIONISEQUALL_EXIT );
    }


// -----------------------------------------------------------------------------
// Adds a connection method into a destination. Does not add embedded
// destinations.
// -----------------------------------------------------------------------------
//
void CCmmSession::DestAddConnMethodL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_DESTADDCONNMETHODL_ENTRY );

    CCmmDestinationInstance* destination =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    // Check the protection and if protected check the needed capabilities.
    CMManager::TProtectionLevel protLevel =
            destination->CurrentProtectionLevel();
    if ( protLevel == CMManager::EProtLevel1 )
        {
        CPolicyServer::TCustomResult capabilities( CPolicyServer::EPass );
        capabilities = iServer.CapabilityCheckWithProtection( aMessage );
        if ( capabilities == CPolicyServer::EFail )
            {
            User::Leave( KErrPermissionDenied );
            }
        }

    CCmmConnMethodInstance* connMethod =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int0() );

    if ( connMethod->IsEmbeddedDestination() )
        {
        User::Leave( KErrArgument );
        }

    TInt index = destination->AddConnMethodL( *connMethod );
    TPckg<TInt> indexPckg( index );
    aMessage.WriteL( 1, indexPckg );

    OstTraceFunctionExit0( CCMMSESSION_DESTADDCONNMETHODL_EXIT );
    }

// -----------------------------------------------------------------------------
// Adds a destination into a destination as an embedded destination.
// -----------------------------------------------------------------------------
//
void CCmmSession::DestAddEmbeddedDestinationL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_DESTADDEMBEDDEDDESTINATIONL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    // Check the protection and if protected check the needed capabilities.
    CMManager::TProtectionLevel protLevel = 
            destinationInstance->CurrentProtectionLevel();
    if ( protLevel == CMManager::EProtLevel1 )
        {
        CPolicyServer::TCustomResult capabilities( CPolicyServer::EPass );
        capabilities = iServer.CapabilityCheckWithProtection( aMessage );
        if ( capabilities == CPolicyServer::EFail )
            {
            User::Leave( KErrPermissionDenied );
            }
        }

    CCmmDestinationInstance* embeddedDestination =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int0() );

    TInt index = destinationInstance->AddEmbeddedDestinationL( *embeddedDestination );
    TPckg<TInt> indexPckg( index );
    aMessage.WriteL( 1, indexPckg );

    OstTraceFunctionExit0( CCMMSESSION_DESTADDEMBEDDEDDESTINATIONL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::DestDeleteConnMethodL
// -----------------------------------------------------------------------------
//
void CCmmSession::DestDeleteConnMethodL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_DESTDELETECONNMETHODL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );
    CCmmConnMethodInstance* connMethodInstance =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int0() );
    
    // Check the protection of destination and if protected check the needed
    // capabilities.
    CMManager::TProtectionLevel protLevel = 
            destinationInstance->CurrentProtectionLevel();
    if ( protLevel == CMManager::EProtLevel1 ||
            protLevel == CMManager::EProtLevel3 )
        {
        CPolicyServer::TCustomResult capabilities( CPolicyServer::EPass );
        capabilities = iServer.CapabilityCheckWithProtection( aMessage );
        if ( capabilities == CPolicyServer::EFail )
            {
            User::Leave( KErrPermissionDenied );
            }
        }

    // Check the protection of CM and if protected check the needed
    // capabilities.
    CheckCapabilitiesForProtectedCML( aMessage, connMethodInstance );

    destinationInstance->DeleteConnMethodFromDestinationL( *connMethodInstance );

    OstTraceFunctionExit0( CCMMSESSION_DESTDELETECONNMETHODL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::DestRemoveConnMethodL
// -----------------------------------------------------------------------------
//
void CCmmSession::DestRemoveConnMethodL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_DESTREMOVECONNMETHODL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );
    CCmmConnMethodInstance* connMethodInstance =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int0() );

    // Check the protection of destination and if protected check the needed
    // capabilities.
    CMManager::TProtectionLevel protLevel = 
            destinationInstance->CurrentProtectionLevel();
    if ( protLevel == CMManager::EProtLevel1 ||
            protLevel == CMManager::EProtLevel3 )
        {
        CPolicyServer::TCustomResult capabilities( CPolicyServer::EPass );
        capabilities = iServer.CapabilityCheckWithProtection( aMessage );
        if ( capabilities == CPolicyServer::EFail )
            {
            User::Leave( KErrPermissionDenied );
            }
        }

    destinationInstance->RemoveConnMethodFromDestinationL( *connMethodInstance );

    OstTraceFunctionExit0( CCMMSESSION_DESTREMOVECONNMETHODL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::ModifyConnMethodPriorityL
// -----------------------------------------------------------------------------
//
void CCmmSession::ModifyConnMethodPriorityL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_MODIFYCONNMETHODPRIORITYL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    // Check the protection of destination and if protected check the needed
    // capabilities.
    CMManager::TProtectionLevel protLevel = 
            destinationInstance->CurrentProtectionLevel();
    if ( protLevel == CMManager::EProtLevel1 ||
            protLevel == CMManager::EProtLevel3 )
        {
        CPolicyServer::TCustomResult capabilities( CPolicyServer::EPass );
        capabilities = iServer.CapabilityCheckWithProtection( aMessage );
        if ( capabilities == CPolicyServer::EFail )
            {
            User::Leave( KErrPermissionDenied );
            }
        }

    CCmmConnMethodInstance* connMethodInstance =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int0() );

    // Index values start from 0 (0 meaning highest priority).
    TUint index( ( TUint )aMessage.Int1() );

    destinationInstance->ModifyConnMethodPriorityL( *connMethodInstance, index );

    OstTraceFunctionExit0( CCMMSESSION_MODIFYCONNMETHODPRIORITYL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::SetDestinationNameL
// -----------------------------------------------------------------------------
//
void CCmmSession::SetDestinationNameL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_SETDESTINATIONNAMEL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    // Check that client does not try to change the name of the Internet Destination.
    TUint32 metadata( 0 );
    destinationInstance->GetMetadataL( CMManager::ESnapMetadataPurpose, metadata );
    if ( metadata == CMManager::ESnapPurposeInternet )
        {
        User::Leave( KErrPermissionDenied );
        }

    // Check the protection and if protected check the needed capabilities.
    CMManager::TProtectionLevel protLevel =
            destinationInstance->CurrentProtectionLevel();
    if ( protLevel == CMManager::EProtLevel1 ||
            protLevel == CMManager::EProtLevel2 )
        {
        CPolicyServer::TCustomResult capabilities( CPolicyServer::EPass );
        capabilities = iServer.CapabilityCheckWithProtection( aMessage );
        if ( capabilities == CPolicyServer::EFail )
            {
            User::Leave( KErrPermissionDenied );
            }
        }

    // Load and check name.
    TInt destNameLength = aMessage.GetDesLength( 0 );
    if ( destNameLength <= 0 )
        {
        User::Leave( KErrArgument ); //TODO, check also max length?
        }

    HBufC* newName = HBufC::NewLC( destNameLength );
    TPtr ptrNewName = newName->Des();
    aMessage.ReadL( 0, ptrNewName );

    // Check if a destination with given name exists (or is already created but not yet saved).
    if ( iCache.DestinationExistsWithNameL( *newName, destinationInstance->GetId() ) ||
            iCache.NotSavedDestinationOpenWithNameL( *newName, destinationInstance->GetId() ) )
        {
        User::Leave( KErrAlreadyExists );
        }

    destinationInstance->SetDestinationNameL( *newName );

    CleanupStack::PopAndDestroy( newName );

    OstTraceFunctionExit0( CCMMSESSION_SETDESTINATIONNAMEL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::SetDestinationIconL
// -----------------------------------------------------------------------------
//
void CCmmSession::SetDestinationIconL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_SETDESTINATIONICONL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    // Check the protection and if protected check the needed capabilities.
    CMManager::TProtectionLevel protLevel =
            destinationInstance->CurrentProtectionLevel(); //TODO, check the current level can't be altered without proper capas.
    if ( protLevel == CMManager::EProtLevel1 ||
            protLevel == CMManager::EProtLevel2 )
        {
        CPolicyServer::TCustomResult capabilities( CPolicyServer::EPass );
        capabilities = iServer.CapabilityCheckWithProtection( aMessage );
        if ( capabilities == CPolicyServer::EFail )
            {
            User::Leave( KErrPermissionDenied );
            }
        }

    // Load and check name.
    TInt destIconNameLength = aMessage.GetDesLength( 0 );
    if ( destIconNameLength < 0 || destIconNameLength > KCmmStringLengthMax )
        {
        User::Leave( KErrArgument );
        }

    HBufC* newIconName = HBufC::NewLC( destIconNameLength );
    TPtr ptrNewIconName = newIconName->Des();
    aMessage.ReadL( 0, ptrNewIconName );

    destinationInstance->SetDestinationIconL( *newIconName );
    CleanupStack::PopAndDestroy( newIconName );

    OstTraceFunctionExit0( CCMMSESSION_SETDESTINATIONICONL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::SetDestinationMetadataL
// -----------------------------------------------------------------------------
//
void CCmmSession::SetDestinationMetadataL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_SETDESTINATIONMETADATAL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    // Check the protection and if protected check the needed capabilities.
    CMManager::TProtectionLevel protLevel =
            destinationInstance->CurrentProtectionLevel();
    if ( protLevel == CMManager::EProtLevel1 ||
            protLevel == CMManager::EProtLevel2 )
        {
        CPolicyServer::TCustomResult capabilities( CPolicyServer::EPass );
        capabilities = iServer.CapabilityCheckWithProtection( aMessage );
        if ( capabilities == CPolicyServer::EFail )
            {
            User::Leave( KErrPermissionDenied );
            }
        }

    CMManager::TSnapMetadataField metadataField =
            ( CMManager::TSnapMetadataField )aMessage.Int0();
    TUint32 metadata = aMessage.Int1();

    destinationInstance->SetMetadataL( metadataField, metadata );

    OstTraceFunctionExit0( CCMMSESSION_SETDESTINATIONMETADATAL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::SetDestinationProtectionL
// -----------------------------------------------------------------------------
//
void CCmmSession::SetDestinationProtectionL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_SETDESTINATIONPROTECTIONL_ENTRY );

    // Check the needed capabilities.
    CPolicyServer::TCustomResult capabilities( CPolicyServer::EPass );
    capabilities = iServer.CapabilityCheckWithProtection( aMessage );
    if ( capabilities == CPolicyServer::EFail )
        {
        User::Leave( KErrPermissionDenied );
        }

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );
    CMManager::TProtectionLevel protLevel =
            ( CMManager::TProtectionLevel )aMessage.Int0();
    destinationInstance->SetProtectionL( protLevel );

    OstTraceFunctionExit0( CCMMSESSION_SETDESTINATIONPROTECTIONL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::SetDestinationHiddenL
// -----------------------------------------------------------------------------
//
void CCmmSession::SetDestinationHiddenL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_SETDESTINATIONHIDDENL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    // Check the protection and if protected check the needed capabilities.
    CMManager::TProtectionLevel protLevel =
            destinationInstance->CurrentProtectionLevel();
    if ( protLevel == CMManager::EProtLevel1 ||
            protLevel == CMManager::EProtLevel2 )
        {
        CPolicyServer::TCustomResult capabilities( CPolicyServer::EPass );
        capabilities = iServer.CapabilityCheckWithProtection( aMessage );
        if ( capabilities == CPolicyServer::EFail )
            {
            User::Leave( KErrPermissionDenied );
            }
        }

    TBool hidden = aMessage.Int0();
    destinationInstance->SetMetadataL( CMManager::ESnapMetadataHiddenAgent, hidden );

    OstTraceFunctionExit0( CCMMSESSION_SETDESTINATIONHIDDENL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::UpdateDestinationL
// -----------------------------------------------------------------------------
//
void CCmmSession::UpdateDestinationL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_UPDATEDESTINATIONL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    // Check the protection and if protected check the needed capabilities
    CMManager::TProtectionLevel protLevel =
            destinationInstance->CurrentProtectionLevel();
    if ( protLevel == CMManager::EProtLevel1 ||
         protLevel == CMManager::EProtLevel2 ||
         protLevel == CMManager::EProtLevel3 )
        {
        CPolicyServer::TCustomResult capabilities( CPolicyServer::EPass );
        capabilities = iServer.CapabilityCheckWithProtection( aMessage );
        if ( capabilities == CPolicyServer::EFail )
            {
            User::Leave( KErrPermissionDenied );
            }
        }

    destinationInstance->UpdateL();

    OstTraceFunctionExit0( CCMMSESSION_UPDATEDESTINATIONL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::DeleteDestinationL
// -----------------------------------------------------------------------------
//
void CCmmSession::DeleteDestinationL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_DELETEDESTINATIONL_ENTRY );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( aMessage.Int3() );

    // Check the protection of destination and if protected check the needed
    // capabilities.
    CMManager::TProtectionLevel protLevel =
            destinationInstance->CurrentProtectionLevel();
    if ( protLevel == CMManager::EProtLevel1 ||
         protLevel == CMManager::EProtLevel2 ||
         protLevel == CMManager::EProtLevel3 )
        {
        CPolicyServer::TCustomResult capabilities( CPolicyServer::EPass );
        capabilities = iServer.CapabilityCheckWithProtection( aMessage );
        if ( capabilities == CPolicyServer::EFail )
            {
            User::Leave( KErrPermissionDenied );
            }
        }

    iCache.CheckIfDestinationCanBeDeletedL( *destinationInstance );
    iCache.DeleteDestinationL( *destinationInstance );

    // Close the destination handle. Destination instance destructor will
    // notify cache. If the reference count for the cache side handle goes to
    // zero, it will be deleted.
    iDestinationObjects->Remove( aMessage.Int3() );

    OstTraceFunctionExit0( CCMMSESSION_DELETEDESTINATIONL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::ServiceConnMethodL
// -----------------------------------------------------------------------------
//
void CCmmSession::ServiceConnMethodL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_SERVICECONNMETHODL_ENTRY );

    switch ( aMessage.Function() )
        {
        case ECMCreateConnMethod:
            {
            CreateConnMethodL( aMessage );
            }
            break;
        case ECMUpdate:
            {
            UpdateConnMethodL( aMessage );
            }
            break;
        case ECMCloseConnMethod:
            {
            CloseConnMethod( aMessage ); // Can't leave.
            }
            break;
        case ECMDelete:
            {
            DeleteConnMethodL( aMessage );
            }
            break;
        case ECMGetConnMethodWithId:
            {
            GetConnMethodWithIdL( aMessage );
            }
            break;
        case ECMRefresh:
            {
            RefreshConnMethodL( aMessage );
            }
            break;
        case ECMGetIntAttribute:
            {
            GetIntAttributeL( aMessage );
            }
            break;
        case ECMGetBoolAttribute:
            {
            GetBoolAttributeL( aMessage );
            }
            break;
        case ECMGetStringAttribute:
            {
            GetStringAttributeL( aMessage );
            }
            break;
        case ECMGetString8Attribute:
            {
            GetString8AttributeL( aMessage );
            }
            break;
        case ECMSetIntAttribute:
            {
            SetIntAttributeL( aMessage );
            }
            break;
        case ECMSetBoolAttribute:
            {
            SetBoolAttributeL( aMessage );
            }
            break;
        case ECMSetStringAttribute:
            {
            SetStringAttributeL( aMessage );
            }
            break;
        case ECMSetString8Attribute:
            {
            SetString8AttributeL( aMessage );
            }
            break;
        case ECMCreateConnMethodWithId:
            {
            CreateConnMethodWithIdL( aMessage );
            }
            break;
        case ECMGetConnMethodFromDestWithIndex:
            {
            GetConnMethodFromDestWithIndexL( aMessage );
            }
            break;
        case ECMGetConnMethodFromDestWithId:
            {
            GetConnMethodFromDestWithIdL( aMessage );
            }
            break;
        case ECMCreateConnMethodToDest:
            {
            CreateConnMethodToDestL( aMessage );
            }
            break;
        case ECMCreateConnMethodToDestWithId:
            {
            CreateConnMethodToDestWithIdL( aMessage );
            }
            break;
        case ECMIsEqual:
            {
            CmIsEqualL( aMessage );
            }
            break;
        case ECMCreateCopyOfExisting:
            {
            CreateCopyOfExistingL( aMessage );
            }
            break;
        case EDestGetEmbeddedDestination:
            {
            GetEmbeddedDestinationL( aMessage );
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
        }

    OstTraceFunctionExit0( CCMMSESSION_SERVICECONNMETHODL_EXIT );
    }

// -----------------------------------------------------------------------------
// Create a new connection method (not embedded destination).
// -----------------------------------------------------------------------------
//
void CCmmSession::CreateConnMethodL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_CREATECONNMETHODL_ENTRY );

    TUint32 bearerType( aMessage.Int0() );

    CCmmConnMethodInstance* connMethodInstance =
            CCmmConnMethodInstance::NewLC( this, &iCache );
    iCache.CreateConnMethodL( *connMethodInstance, NULL, bearerType, 0 );

    iConnMethodContainer->AddL( ( CObject* ) connMethodInstance );
    TInt handle = iConnMethodObjects->AddL( ( CObject* ) connMethodInstance );
    connMethodInstance->SetHandle( handle );
    CleanupStack::Pop( connMethodInstance );

    TPckg<TInt> handlePckg( handle );
    TInt error = aMessage.Write( 3, handlePckg );
    if ( error )
        {
        // Remove from object index and destroy the object.
        iConnMethodObjects->Remove( handle );
        User::Leave( error );
        }

    OstTraceFunctionExit0( CCMMSESSION_CREATECONNMETHODL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::ConnMethodUpdateL
// -----------------------------------------------------------------------------
//
void CCmmSession::UpdateConnMethodL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_UPDATECONNMETHODL_ENTRY );

    CCmmConnMethodInstance* connMethodInstance =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int3() );

    // Check if the Client has capabilities to delete this CM.
    CheckCapabilitiesForProtectedCML( aMessage, connMethodInstance );

    connMethodInstance->UpdateL();

    OstTraceFunctionExit0( CCMMSESSION_UPDATECONNMETHODL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::CloseConnMethod
// -----------------------------------------------------------------------------
//
void CCmmSession::CloseConnMethod( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_CLOSECONNMETHOD_ENTRY );

    // Check first that the connection method instance exists.
    CObject* connMethodObject = iConnMethodObjects->At( aMessage.Int3() );
    if ( connMethodObject )
        {
        // Connection method instance destructor will notify cache.
        iConnMethodObjects->Remove( aMessage.Int3() );
        }

    OstTraceFunctionExit0( CCMMSESSION_CLOSECONNMETHOD_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::DeleteConnMethodL
// -----------------------------------------------------------------------------
//
void CCmmSession::DeleteConnMethodL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_DELETECONNMETHODL_ENTRY );

    CCmmConnMethodInstance* connMethodInstance =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int3() );

    // Check if the Client has capabilities to delete this CM.
    CheckCapabilitiesForProtectedCML( aMessage, connMethodInstance );

    iCache.CheckIfConnMethodCanBeDeletedL( *connMethodInstance );
    iCache.DeleteConnMethodL( *connMethodInstance );
    // Ignore the boolean return value. It is always true, or the
    // DeleteL()-call leaves.

    // Close the connection method handle. Connection method instance
    // destructor will notify cache. If the reference count for the cache side
    // handle goes to zero, it will be deleted.
    iConnMethodObjects->Remove( aMessage.Int3() );

    OstTraceFunctionExit0( CCMMSESSION_DELETECONNMETHODL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetConnMethodWithIdL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetConnMethodWithIdL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETCONNMETHODWITHIDL_ENTRY );

    TUint32 connMethodId = aMessage.Int0();

    // If this session already has a connection method with matching id open,
    // provide the client with a reference to the already opened handle.
    CCmmConnMethodInstance* cmInstance = FindConnMethodInstanceById( connMethodId );
    if ( cmInstance )
        {
        // Also check that the connection method exists in database.
        if ( iCache.ConnMethodExistsWithId( connMethodId ) )
            {
            TPckg<TInt> existingHandlePckg( cmInstance->GetHandle() );
            aMessage.WriteL( 1, existingHandlePckg );
            User::Leave( KErrAlreadyExists );
            }
        }
    cmInstance = NULL;
    TPckg<TInt> existingHandlePckg( 0 );
    aMessage.WriteL( 1, existingHandlePckg );

    cmInstance = CCmmConnMethodInstance::NewLC( this, &iCache );
    // Will check if connection method ID is valid.
    iCache.OpenConnMethodL( *cmInstance, NULL, connMethodId );

    iConnMethodContainer->AddL( ( CObject* ) cmInstance );
    TInt handle = iConnMethodObjects->AddL( ( CObject* ) cmInstance );
    cmInstance->SetHandle( handle );
    CleanupStack::Pop( cmInstance );

    TPckg<TInt> handlePckg( handle );
    TInt error = aMessage.Write( 3, handlePckg );
    if ( error )
        {
        // Remove from object index and destroy the object.
        iConnMethodObjects->Remove( handle );
        User::Leave( error );
        }

    OstTraceFunctionExit0( CCMMSESSION_GETCONNMETHODWITHIDL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::RefreshConnMethodL
// -----------------------------------------------------------------------------
//
void CCmmSession::RefreshConnMethodL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_REFRESHCONNMETHODL_ENTRY );

    CCmmConnMethodInstance* connMethodInstance =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int3() );

    iCache.RefreshConnMethodL( *connMethodInstance );

    OstTraceFunctionExit0( CCMMSESSION_REFRESHCONNMETHODL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetIntAttributeL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetIntAttributeL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETINTATTRIBUTEL_ENTRY );

    CCmmConnMethodInstance* cm =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int3() );

    TUint32 attribute( aMessage.Int0() );
    TUint32 value = cm->GetIntAttributeL( attribute );

    TPckg<TUint32> valuePckg( value );
    aMessage.WriteL( 1, valuePckg );

    OstTraceFunctionExit0( CCMMSESSION_GETINTATTRIBUTEL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetBoolAttributeL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetBoolAttributeL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETBOOLATTRIBUTEL_ENTRY );

    CCmmConnMethodInstance* cm =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int3() );

    TUint32 attribute( aMessage.Int0() );
    TBool value = cm->GetBoolAttributeL( attribute );

    TPckg<TBool> valuePckg( value );
    aMessage.WriteL( 1, valuePckg );

    OstTraceFunctionExit0( CCMMSESSION_GETBOOLATTRIBUTEL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetStringAttributeL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetStringAttributeL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETSTRINGATTRIBUTEL_ENTRY );

    CCmmConnMethodInstance* cm =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int3() );

    TUint32 attribute( aMessage.Int0() );
    HBufC* value = cm->GetStringAttributeL( attribute );
    if ( !value )
        {
        OstTraceFunctionExit0( CCMMSESSION_GETSTRINGATTRIBUTEL_EXIT );
        return;
        }
    CleanupStack::PushL( value );
    TPtrC valuePtr = value->Des();

    // Check the buffer length of the given buffer.
    TInt bufferLen = aMessage.GetDesMaxLength( 1 );
    if ( valuePtr.Length() > bufferLen )
        {
        User::Leave( KErrArgument );
        }

    aMessage.WriteL( 1, valuePtr );
    CleanupStack::PopAndDestroy( value );

    OstTraceFunctionExit0( DUP1_CCMMSESSION_GETSTRINGATTRIBUTEL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::GetString8AttributeL
// -----------------------------------------------------------------------------
//
void CCmmSession::GetString8AttributeL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETSTRING8ATTRIBUTEL_ENTRY );

    CCmmConnMethodInstance* cm =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int3() );

    TUint32 attribute( aMessage.Int0() );
    HBufC8* value = cm->GetString8AttributeL( attribute );
    if ( !value )
        {
        OstTraceFunctionExit0( CCMMSESSION_GETSTRING8ATTRIBUTEL_EXIT );
        return;
        }
    CleanupStack::PushL( value );
    TPtrC8 valuePtr = value->Des();

    // Check the buffer length of the given buffer.
    TInt bufferLen = aMessage.GetDesMaxLength( 1 );
    if ( valuePtr.Length() > bufferLen )
        {
        User::Leave( KErrArgument );
        }

    aMessage.WriteL( 1, valuePtr );
    CleanupStack::PopAndDestroy( value );

    OstTraceFunctionExit0( DUP1_CCMMSESSION_GETSTRING8ATTRIBUTEL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::SetIntAttributeL
// -----------------------------------------------------------------------------
//
void CCmmSession::SetIntAttributeL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_SETINTATTRIBUTEL_ENTRY );

    CCmmConnMethodInstance* cm =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int3() );

    // Check the capability needed if protected CM
    // ECapabilityWriteDeviceData is checked earlier already.
    CheckCapabilitiesForProtectedCML( aMessage, cm );

    TUint32 attribute( aMessage.Int0() );
    TUint32 value( aMessage.Int1() );

    cm->SetIntAttributeL( attribute, value );

    OstTraceFunctionExit0( CCMMSESSION_SETINTATTRIBUTEL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::SetBoolAttributeL
// -----------------------------------------------------------------------------
//
void CCmmSession::SetBoolAttributeL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_SETBOOLATTRIBUTEL_ENTRY );

    CCmmConnMethodInstance* cm =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int3() );

    // Check the capability needed if protected CM
    // ECapabilityWriteDeviceData is checked earlier already.
    CheckCapabilitiesForProtectedCML( aMessage, cm );

    TUint32 attribute( aMessage.Int0() );
    TBool value( aMessage.Int1() );

    cm->SetBoolAttributeL( attribute, value );

    OstTraceFunctionExit0( CCMMSESSION_SETBOOLATTRIBUTEL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::SetStringAttributeL
// -----------------------------------------------------------------------------
//
void CCmmSession::SetStringAttributeL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_SETSTRINGATTRIBUTEL_ENTRY );

    CCmmConnMethodInstance* cm =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int3() );

    // Check the capability needed if protected CM
    // ECapabilityWriteDeviceData is checked earlier already.
    CheckCapabilitiesForProtectedCML( aMessage, cm );

    TUint32 attribute( aMessage.Int0() );

    HBufC* value = HBufC::NewLC( aMessage.GetDesLengthL( 1 ) );
    TPtr valuePtr( value->Des() );
    aMessage.ReadL( 1, valuePtr );

    cm->SetStringAttributeL( attribute, valuePtr );

    CleanupStack::PopAndDestroy( value );

    OstTraceFunctionExit0( CCMMSESSION_SETSTRINGATTRIBUTEL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmSession::SetString8AttributeL
// -----------------------------------------------------------------------------
//
void CCmmSession::SetString8AttributeL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_SETSTRING8ATTRIBUTEL_ENTRY );

    CCmmConnMethodInstance* cm =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int3() );

    // Check the capability needed if protected CM
    // ECapabilityWriteDeviceData is checked earlier already.
    CheckCapabilitiesForProtectedCML( aMessage, cm );

    TUint32 attribute( aMessage.Int0() );

    HBufC8* value = HBufC8::NewLC( aMessage.GetDesLengthL( 1 ) );
    TPtr8 valuePtr( value->Des() );
    aMessage.ReadL( 1, valuePtr );

    cm->SetString8AttributeL( attribute, valuePtr );

    CleanupStack::PopAndDestroy( value );

    OstTraceFunctionExit0( CCMMSESSION_SETSTRING8ATTRIBUTEL_EXIT );
    }

// -----------------------------------------------------------------------------
// Create a new connection method (not embedded destination) with specific ID.
// -----------------------------------------------------------------------------
//
void CCmmSession::CreateConnMethodWithIdL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_CREATECONNMETHODWITHIDL_ENTRY );

    TUint32 bearerType( aMessage.Int0() );
    TUint32 connMethodId = ( aMessage.Int1() );

    CCmmConnMethodInstance* connMethod =
            CCmmConnMethodInstance::NewLC( this, &iCache );
    // Will check if ID is available.
    iCache.CreateConnMethodL( *connMethod, NULL, bearerType, connMethodId );

    iConnMethodContainer->AddL( ( CObject* ) connMethod );
    TInt handle = iConnMethodObjects->AddL( ( CObject* ) connMethod );
    connMethod->SetHandle( handle );
    CleanupStack::Pop( connMethod );

    TPckg<TInt> handlePckg( handle );
    TInt error = aMessage.Write( 3, handlePckg );
    if ( error )
        {
        // Remove from object index and destroy the object.
        iConnMethodObjects->Remove( handle );
        User::Leave( error );
        }

    OstTraceFunctionExit0( CCMMSESSION_CREATECONNMETHODWITHIDL_EXIT );
    }

// -----------------------------------------------------------------------------
// Gets a Connection Method from a Destination with index number.
// -----------------------------------------------------------------------------
//
void CCmmSession::GetConnMethodFromDestWithIndexL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETCONNMETHODFROMDESTWITHINDEXL_ENTRY );

    TInt destinationHandle( aMessage.Int0() );
    TInt connMethodIndex( aMessage.Int1() );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( destinationHandle );

    // Check index is in range.
    if ( connMethodIndex < 0 ||
         connMethodIndex >= destinationInstance->iConnMethodItemArray.Count() )
        {
        User::Leave( KErrArgument );
        }
    TInt connMethodId( destinationInstance->iConnMethodItemArray[connMethodIndex].iId );

    // If this session already has a connection method with matching id open
    // and that connection method correctly belongs to the provided destination,
    // provide the client with a reference to the already opened handle.
    CCmmConnMethodInstance* cmInstance = FindConnMethodInstanceById( connMethodId );
    if ( cmInstance &&
            destinationInstance->ValidConnMethodIdInDestinationIncludeEmbedded( connMethodId ) )
        {
        // Also check that the connection method exists in database.
        if ( iCache.ConnMethodExistsWithId( connMethodId ) ||
                iCache.DestinationExistsWithId( connMethodId ) )
            {
            TPckg<TInt> existingHandlePckg( cmInstance->GetHandle() );
            aMessage.WriteL( 2, existingHandlePckg );
            User::Leave( KErrAlreadyExists );
            }
        }
    cmInstance = NULL;
    TPckg<TInt> existingHandlePckg( 0 );
    aMessage.WriteL( 2, existingHandlePckg );

    cmInstance = CCmmConnMethodInstance::NewLC( this, &iCache );
    // Will check if connection method ID is valid.
    iCache.OpenConnMethodL( *cmInstance, destinationInstance, connMethodId );

    iConnMethodContainer->AddL( ( CObject* ) cmInstance );
    TInt handle = iConnMethodObjects->AddL( ( CObject* ) cmInstance );
    cmInstance->SetHandle( handle );
    CleanupStack::Pop( cmInstance );

    TPckg<TInt> handlePckg( handle );
    TInt error = aMessage.Write( 3, handlePckg );
    if ( error )
        {
        // Remove from object index and destroy the object.
        iConnMethodObjects->Remove( handle );
        User::Leave( error );
        }

    OstTraceFunctionExit0( CCMMSESSION_GETCONNMETHODFROMDESTWITHINDEXL_EXIT );
    }

// -----------------------------------------------------------------------------
// Gets a Connection Method from a Destination with Id.
// -----------------------------------------------------------------------------
//
void CCmmSession::GetConnMethodFromDestWithIdL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETCONNMETHODFROMDESTWITHIDL_ENTRY );

    TInt destinationHandle( aMessage.Int0() );
    TUint32 connMethodId( aMessage.Int1() );

    CCmmDestinationInstance* destinationInstance =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( destinationHandle );

    // If this session already has a connection method with matching id open
    // and that connection method correctly belongs to the provided destination,
    // provide the client with a reference to the already opened handle.
    CCmmConnMethodInstance* cmInstance = FindConnMethodInstanceById( connMethodId );
    if ( cmInstance &&
            destinationInstance->ValidConnMethodIdInDestinationIncludeEmbedded( connMethodId ) )
        {
        // Also check that the connection method exists in database.
        if ( iCache.ConnMethodExistsWithId( connMethodId ) ||
                iCache.DestinationExistsWithId( connMethodId ) )
            {
            TPckg<TInt> existingHandlePckg( cmInstance->GetHandle() );
            aMessage.WriteL( 2, existingHandlePckg );
            User::Leave( KErrAlreadyExists );
            }
        }
    cmInstance = NULL;
    TPckg<TInt> existingHandlePckg( 0 );
    aMessage.WriteL( 2, existingHandlePckg );

    cmInstance = CCmmConnMethodInstance::NewLC( this, &iCache );
    // Will check if connection method ID is valid.
    iCache.OpenConnMethodL( *cmInstance, destinationInstance, connMethodId );

    iConnMethodContainer->AddL( ( CObject* ) cmInstance );
    TInt handle = iConnMethodObjects->AddL( ( CObject* ) cmInstance );
    cmInstance->SetHandle( handle );
    CleanupStack::Pop( cmInstance );

    TPckg<TInt> handlePckg( handle );
    TInt error = aMessage.Write( 3, handlePckg );
    if ( error )
        {
        // Remove from object index and destroy the object.
        iConnMethodObjects->Remove( handle );
        User::Leave( error );
        }

    OstTraceFunctionExit0( CCMMSESSION_GETCONNMETHODFROMDESTWITHIDL_EXIT );
    }

// -----------------------------------------------------------------------------
// Create a new connection method (not embedded destination) into a destination.
// -----------------------------------------------------------------------------
//
void CCmmSession::CreateConnMethodToDestL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_CREATECONNMETHODTODESTL_ENTRY );

    TInt destinationHandle( aMessage.Int0() );
    TUint32 bearerType( aMessage.Int1() );

    CCmmDestinationInstance* destination =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( destinationHandle );

    // Check the protection and if protected check the needed capabilities.
    CMManager::TProtectionLevel protLevel =
            destination->CurrentProtectionLevel();
    if ( protLevel == CMManager::EProtLevel1 )
        {
        CPolicyServer::TCustomResult capabilities( CPolicyServer::EPass );
        capabilities = iServer.CapabilityCheckWithProtection( aMessage );
        if ( capabilities == CPolicyServer::EFail )
            {
            User::Leave( KErrPermissionDenied );
            }
        }

    CCmmConnMethodInstance* connMethod = CCmmConnMethodInstance::NewLC( this, &iCache );
    iCache.CreateConnMethodL( *connMethod, destination, bearerType, 0 );

    iConnMethodContainer->AddL( ( CObject* ) connMethod );
    TInt handle = iConnMethodObjects->AddL( ( CObject* ) connMethod );
    connMethod->SetHandle( handle );
    CleanupStack::Pop( connMethod );

    TPckg<TInt> handlePckg( handle );
    TInt error = aMessage.Write( 3, handlePckg );
    if ( error )
        {
        // Remove from object index and destroy the object.
        iConnMethodObjects->Remove( handle );
        User::Leave( error );
        }

    OstTraceFunctionExit0( CCMMSESSION_CREATECONNMETHODTODESTL_EXIT );
    }

// -----------------------------------------------------------------------------
// Create a new connection method (not embedded destination) with specific ID
// into a destination.
// -----------------------------------------------------------------------------
//
void CCmmSession::CreateConnMethodToDestWithIdL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_CREATECONNMETHODTODESTWITHIDL_ENTRY );

    TInt destinationHandle( aMessage.Int0() );
    TUint32 bearerType( aMessage.Int1() );
    TUint32 connMethodId( aMessage.Int2() );

    CCmmDestinationInstance* destination =
            ( CCmmDestinationInstance* )iDestinationObjects->AtL( destinationHandle );

    // Check the protection and if protected check the needed capabilities.
    CMManager::TProtectionLevel protLevel =
            destination->CurrentProtectionLevel();
    if ( protLevel == CMManager::EProtLevel1 )
        {
        CPolicyServer::TCustomResult capabilities( CPolicyServer::EPass );
        capabilities = iServer.CapabilityCheckWithProtection( aMessage );
        if ( capabilities == CPolicyServer::EFail )
            {
            User::Leave( KErrPermissionDenied );
            }
        }

    CCmmConnMethodInstance* connMethod =
            CCmmConnMethodInstance::NewLC( this, &iCache );
    // Will check if ID is available.
    iCache.CreateConnMethodL( *connMethod, destination, bearerType, connMethodId );

    iConnMethodContainer->AddL( ( CObject* ) connMethod );
    TInt handle = iConnMethodObjects->AddL( ( CObject* ) connMethod );
    connMethod->SetHandle( handle );
    CleanupStack::Pop( connMethod );

    TPckg<TInt> handlePckg( handle );
    TInt error = aMessage.Write( 3, handlePckg );
    if ( error )
        {
        // Remove from object index and destroy the object.
        iConnMethodObjects->Remove( handle );
        User::Leave( error );
        }

    OstTraceFunctionExit0( CCMMSESSION_CREATECONNMETHODTODESTWITHIDL_EXIT );
    }

// -----------------------------------------------------------------------------
// Checks if the CM given as parameter is equal to this CM
// -----------------------------------------------------------------------------
//
void CCmmSession::CmIsEqualL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_CMISEQUALL_ENTRY );

    CCmmConnMethodInstance* cm =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int3() );

    CCmmConnMethodInstance* cmToBeCompared =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int0() );

    TBool equal( EFalse );
    if ( cm->GetId() == cmToBeCompared->GetId() )
        {
        equal = ETrue;
        }

    TPckg<TBool> equalPckg( equal );
    aMessage.WriteL( 1, equalPckg );

    OstTraceFunctionExit0( CCMMSESSION_CMISEQUALL_EXIT );
    }

// -----------------------------------------------------------------------------
// Creates a copy of an existing connection method and opens a handle to it.
// -----------------------------------------------------------------------------
//
void CCmmSession::CreateCopyOfExistingL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_CREATECOPYOFEXISTINGL_ENTRY );

    CCmmConnMethodInstance* sourceCm =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int0() );
    TUint32 cmId = sourceCm->GetId();

    CCmmConnMethodInstance* targetCm =
            CCmmConnMethodInstance::NewLC( this, &iCache );
    iCache.CreateCopyOfConnMethodL( *targetCm, *sourceCm );

    iConnMethodContainer->AddL( ( CObject* ) targetCm );
    TInt handle = iConnMethodObjects->AddL( ( CObject* ) targetCm );
    targetCm->SetHandle( handle );
    CleanupStack::Pop( targetCm );

    TPckg<TInt> handlePckg( handle );
    TInt error = aMessage.Write( 3, handlePckg );
    if ( error )
        {
        // Remove from object index and destroy the object.
        iConnMethodObjects->Remove( handle );
        User::Leave( error );
        }

    OstTraceFunctionExit0( CCMMSESSION_CREATECOPYOFEXISTINGL_EXIT );
    }

// -----------------------------------------------------------------------------
// Creates a copy of an existing connection method and opens a handle to it.
// -----------------------------------------------------------------------------
//
void CCmmSession::GetEmbeddedDestinationL( const RMessage2& aMessage )
    {
    OstTraceFunctionEntry0( CCMMSESSION_GETEMBEDDEDDESTINATIONL_ENTRY );

    CCmmConnMethodInstance* connMethodInstance =
            ( CCmmConnMethodInstance* )iConnMethodObjects->AtL( aMessage.Int3() );

    if ( connMethodInstance->GetBearerType() != KUidEmbeddedDestination )
        {
        User::Leave( KErrNotSupported );
        }

    TUint32 destinationId = connMethodInstance->GetId();

    // Check that the ID is in valid range.
    if ( destinationId <= KCmDefaultDestinationAPTagId ||
            destinationId >= KCmMaxDestinationAPTagId )
        {
        User::Leave( KErrCorrupt );
        }

    // If this session already has a destination with matching ID open, provide
    // the client with a reference to the already opened handle.
    CCmmDestinationInstance* destinationInstance = FindDestinationInstanceById( destinationId );
    if ( destinationInstance )
        {
        if ( iCache.DestinationExistsWithId( destinationId ) )
            {
            TPckg<TInt> existingHandlePckg( destinationInstance->GetHandle() );
            aMessage.WriteL( 1, existingHandlePckg );
            User::Leave( KErrAlreadyExists );
            }
        }
    destinationInstance = NULL;
    TPckg<TInt> existingHandlePckg( 0 );
    aMessage.WriteL( 1, existingHandlePckg );

    destinationInstance = CCmmDestinationInstance::NewLC( this, &iCache );

    // Cache will open a handle to the destination if not already open, and
    // copy relevant data into this destination instance.
    iCache.OpenDestinationL( *destinationInstance, destinationId ); // Checks ID is valid.

    iDestinationContainer->AddL( ( CObject* ) destinationInstance );
    TInt handle = iDestinationObjects->AddL( ( CObject* ) destinationInstance );
    destinationInstance->SetHandle( handle );
    CleanupStack::Pop( destinationInstance );

    TPckg<TInt> handlePckg( handle );
    TInt error = aMessage.Write( 3, handlePckg );
    if ( error )
        {
        // Removes from object index and destroys the object.
        iDestinationObjects->Remove( handle );
        User::Leave( error );
        }
    OstTraceFunctionExit0( CCMMSESSION_GETEMBEDDEDDESTINATIONL_EXIT );
    }

// -----------------------------------------------------------------------------
// Check if CM is protected and if so then check the needed capabilities.
// -----------------------------------------------------------------------------
//
void CCmmSession::CheckCapabilitiesForProtectedCML(
        const RMessage2& aMessage,
        CCmmConnMethodInstance* aConnectionMethodInstance )
    {
    OstTraceFunctionEntry0( CCMMSESSION_CHECKCAPABILITIESFORPROTECTEDCML_ENTRY );

    TBool prot = aConnectionMethodInstance->GetBoolAttributeL( CMManager::ECmProtected );
    if ( prot )
        {
        CPolicyServer::TCustomResult capabilities( CPolicyServer::EPass );
        capabilities = iServer.CapabilityCheckWithProtection( aMessage );
        if ( capabilities == CPolicyServer::EFail )
            {
            User::Leave( KErrPermissionDenied );
            }
        }

    OstTraceFunctionExit0( CCMMSESSION_CHECKCAPABILITIESFORPROTECTEDCML_EXIT );
    }

// End of file
