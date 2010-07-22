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
* Session side object representing a connection method.
*
*/


#include <cmpluginbaseeng.h>
#include <cmconnectionmethoddef.h>
#include <cmpluginembdestinationdef.h>

#include "cmmconnmethodinstance.h"
#include "cmmsession.h"
#include "cmmcache.h"
#include "cmmconnmethodstruct.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmconnmethodinstanceTraces.h"
#endif


// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CCmmConnMethodInstance* CCmmConnMethodInstance::NewL(
        CCmmSession* aCmmSession,
        CCmmCache* aCache )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_NEWL_ENTRY );

    CCmmConnMethodInstance* self = CCmmConnMethodInstance::NewLC( aCmmSession, aCache );
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_NEWL_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CCmmConnMethodInstance* CCmmConnMethodInstance::NewLC(
        CCmmSession* aCmmSession,
        CCmmCache* aCache )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_NEWLC_ENTRY );

    CCmmConnMethodInstance* self = new( ELeave ) CCmmConnMethodInstance( aCmmSession, aCache );
    CleanupClosePushL( *self );
    self->ConstructL();

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_NEWLC_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CCmmConnMethodInstance::~CCmmConnMethodInstance()
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_CCMMCONNMETHODINSTANCE_ENTRY );

    // Tell cache this handle is beeing closed. Cache will update own bookkeeping.
    if ( iCache )
        {
        iCache->CloseConnMethod( *this );
        }

    delete iPluginDataInstance;

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_CCMMCONNMETHODINSTANCE_EXIT );
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CCmmConnMethodInstance::CCmmConnMethodInstance(
        CCmmSession* aCmmSession,
        CCmmCache* aCache )
        :
        iCmmSession( aCmmSession ),
        iCache( aCache )
    {
    OstTraceFunctionEntry0( DUP1_CCMMCONNMETHODINSTANCE_CCMMCONNMETHODINSTANCE_ENTRY );

    iPlugin = NULL;
    iPluginDataInstance = NULL;
    iConnMethodId = 0;
    iBearerType = 0;
    iStatus = ECmmConnMethodStatusChanged;
    iHandle = 0;

    OstTraceFunctionExit0( DUP1_CCMMCONNMETHODINSTANCE_CCMMCONNMETHODINSTANCE_EXIT );
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::ConstructL()
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_CONSTRUCTL_ENTRY );

    iPluginDataInstance = CCmClientPluginInstance::NewL();

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------
// Get pointer to the plugin data container.
// ---------------------------------------------------------------------------
//
CCmClientPluginInstance* CCmmConnMethodInstance::GetPluginDataInstance() const
    {
    return iPluginDataInstance;
    }

// ---------------------------------------------------------------------------
// Get connection method ID.
// ---------------------------------------------------------------------------
//
TUint32 CCmmConnMethodInstance::GetId() const
    {
    return iConnMethodId;
    }

// ---------------------------------------------------------------------------
// Set connection method ID.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::SetId( const TUint32 aConnMethodId )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_SETID_ENTRY );

    iConnMethodId = aConnMethodId;

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_SETID_EXIT );
    }

// ---------------------------------------------------------------------------
// Return bearer type.
// ---------------------------------------------------------------------------
//
TUint32 CCmmConnMethodInstance::GetBearerType() const
    {
    return iBearerType;
    }

// ---------------------------------------------------------------------------
// Get handle ID.
// ---------------------------------------------------------------------------
//
TInt CCmmConnMethodInstance::GetHandle() const
    {
    return iHandle;
    }

// ---------------------------------------------------------------------------
// Set handle ID.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::SetHandle( const TInt aHandle )
    {
    iHandle = aHandle;
    }

// ---------------------------------------------------------------------------
// Return ETrue, if this connection method represents an embedded destination.
// ---------------------------------------------------------------------------
//
TBool CCmmConnMethodInstance::IsEmbeddedDestination() const
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_ISEMBEDDEDDESTINATION_ENTRY );

    TBool result( EFalse );
    if ( iBearerType == KUidEmbeddedDestination )
        {
        result = ETrue;
        }

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_ISEMBEDDEDDESTINATION_EXIT );
    return result;
    }

// ---------------------------------------------------------------------------
// Save the contents of this connection method into database. The attribute
// aTemporaryHandle must be true if this connection instance was temporarily
// created for the duration of this update operation only. 
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::UpdateL( TBool aTemporaryHandle )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_UPDATEL_ENTRY );

    if ( iCache )
        {
        iCache->UpdateConnMethodL( *this, aTemporaryHandle );
        }
    else
        {
        User::Leave( KErrBadHandle );
        }

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_UPDATEL_EXIT );
    }

// ---------------------------------------------------------------------------
// Copy the relevant connection method information from cache side object.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::CopyDataL( CCmmConnMethodStruct* aConnMethodStruct )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_COPYDATAL_ENTRY );

    if ( !aConnMethodStruct )
        {
        User::Leave( KErrCorrupt );
        }

    iConnMethodId = aConnMethodStruct->GetId();
    iBearerType = aConnMethodStruct->GetBearerType();

    iPlugin = aConnMethodStruct->GetPlugin();
    if ( !iPlugin )
        {
        User::Leave( KErrCorrupt );
        }

    // Embedded destination does not have any data.
    if ( !IsEmbeddedDestination() )
        {
        iPlugin->GetPluginDataL( iPluginDataInstance );
        }

    aConnMethodStruct->IncrementReferenceCounter();

    switch ( aConnMethodStruct->GetStatus() )
        {
        case ECmmConnMethodStatusNotSaved:
        case ECmmConnMethodStatusToBeDeleted:
            {
            iStatus = ECmmConnMethodStatusChanged;
            }
            break;
        case ECmmConnMethodStatusValid:
            {
            iStatus = ECmmConnMethodStatusValid;
            }
            break;
        case ECmmConnMethodStatusChanged:
        default:
            User::Leave( KErrCorrupt );  // Error, invalid status.
            break;
        }

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_COPYDATAL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmmConnMethodInstance::GetIntAttributeL
// ---------------------------------------------------------------------------
//
TUint32 CCmmConnMethodInstance::GetIntAttributeL( const TUint32 aAttribute )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_GETINTATTRIBUTEL_ENTRY );

    if ( !iPlugin )
        {
        User::Leave( KErrCorrupt );
        }

    // Embedded destination has only limited set of attributes.
    if ( IsEmbeddedDestination() )
        {
        switch ( aAttribute )
            {
            case CMManager::ECmId:
            case CMManager::ECmBearerType:
            case CMManager::ECmDefaultPriority:
            case CMManager::ECmDefaultUiPriority:
            case ECmExtensionLevel:
                {
                break;
                }
            default:
                {
                User::Leave( KErrNotSupported );
                }
            }
        }

    TUint32 result = iPlugin->GetIntAttributeL( aAttribute, iPluginDataInstance );

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_GETINTATTRIBUTEL_EXIT );
    return result;
    }

// ---------------------------------------------------------------------------
// CCmmConnMethodInstance::GetBoolAttributeL
// ---------------------------------------------------------------------------
//
TBool CCmmConnMethodInstance::GetBoolAttributeL( const TUint32 aAttribute )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_GETBOOLATTRIBUTEL_ENTRY );

    if ( !iPlugin )
        {
        User::Leave( KErrCorrupt );
        }

    // Embedded destination has only limited set of attributes.
    if ( IsEmbeddedDestination() )
        {
        switch ( aAttribute )
            {
            case CMManager::ECmCoverage:
            case CMManager::ECmDestination:
            case CMManager::ECmProtected:
            case CMManager::ECmHidden:
            case CMManager::ECmBearerHasUi:
            case CMManager::ECmVirtual:
                {
                break;
                }
            default:
                {
                User::Leave( KErrNotSupported );
                }
            }
        }

    TBool retVal( EFalse );
    switch ( aAttribute )
        {
        case CMManager::ECmConnected:
            {
            if ( GetId() > 0 )
                {
                // Check first if this is embedded destination.
                retVal = iPlugin->GetBoolAttributeL( CMManager::ECmDestination,
                        iPluginDataInstance );
                if ( retVal )
                    {
                    retVal = iCache->DestinationConnectedL( GetId() );
                    }
                else
                    {
                    if ( iCache )
                        {
                        retVal = iCache->CheckIfCmConnected( GetId() );
                        }
                    }
                }
            }
            break;
        case CMManager::ECmIsLinked:
            {
            // Does any virtual iap point to this connection method.
            // Check first if this connection method is an embedded destination.
            if ( GetId() > 0 )
                {
                retVal = iPlugin->GetBoolAttributeL( CMManager::ECmDestination,
                        iPluginDataInstance );
                if ( retVal )
                    {
                    retVal = iCache->DestinationPointedToByVirtualIap( GetId() );
                    }
                else
                    {
                    retVal = iCache->ConnMethodPointedToByVirtualIap( GetId() );
                    }
                }
            }
            break;
        default:
            {
            retVal = iPlugin->GetBoolAttributeL( aAttribute, iPluginDataInstance );
            }
            break;
        }

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_GETBOOLATTRIBUTEL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmmConnMethodInstance::GetStringAttributeL
// ---------------------------------------------------------------------------
//
HBufC* CCmmConnMethodInstance::GetStringAttributeL( const TUint32 aAttribute )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_GETSTRINGATTRIBUTEL_ENTRY );

    if ( !iPlugin )
        {
        User::Leave( KErrCorrupt );
        }

    // Embedded destination has only limited set of attributes.
    if ( IsEmbeddedDestination() )
        {
        User::Leave( KErrNotSupported );
        }

    HBufC* result = iPlugin->GetStringAttributeL( aAttribute, iPluginDataInstance );

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_GETSTRINGATTRIBUTEL_EXIT );
    return result;
    }

// ---------------------------------------------------------------------------
// CCmmConnMethodInstance::GetString8AttributeL
// ---------------------------------------------------------------------------
//
HBufC8* CCmmConnMethodInstance::GetString8AttributeL( const TUint32 aAttribute )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_GETSTRING8ATTRIBUTEL_ENTRY );

    if ( !iPlugin )
        {
        User::Leave( KErrCorrupt );
        }

    // Embedded destination has only limited set of attributes.
    if ( IsEmbeddedDestination() )
        {
        User::Leave( KErrNotSupported );
        }

    HBufC8* result = iPlugin->GetString8AttributeL( aAttribute, iPluginDataInstance );

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_GETSTRING8ATTRIBUTEL_EXIT );
    return result;
    }

// ---------------------------------------------------------------------------
// CCmmConnMethodInstance::SetIntAttributeL
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::SetIntAttributeL(
        const TUint32 aAttribute,
        const TUint32 aValue )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_SETINTATTRIBUTEL_ENTRY );

    if ( !iPlugin )
        {
        User::Leave( KErrCorrupt );
        }

    iPlugin->SetIntAttributeL( aAttribute, aValue, iPluginDataInstance );

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_SETINTATTRIBUTEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmmConnMethodInstance::SetBoolAttributeL
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::SetBoolAttributeL(
        const TUint32 aAttribute,
        const TBool aValue )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_SETBOOLATTRIBUTEL_ENTRY );

    if ( !iPlugin )
        {
        User::Leave( KErrCorrupt );
        }

    iPlugin->SetBoolAttributeL( aAttribute, aValue, iPluginDataInstance );

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_SETBOOLATTRIBUTEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmmConnMethodInstance::SetStringAttributeL
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::SetStringAttributeL(
        const TUint32 aAttribute,
        const TDesC16& aValue )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_SETSTRINGATTRIBUTEL_ENTRY );

    if ( !iPlugin )
        {
        User::Leave( KErrCorrupt );
        }

    iPlugin->SetStringAttributeL( aAttribute, aValue, iPluginDataInstance );

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_SETSTRINGATTRIBUTEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmmConnMethodInstance::SetString8AttributeL
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::SetString8AttributeL(
        const TUint32 aAttribute,
        const TDesC8& aValue )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_SETSTRING8ATTRIBUTEL_ENTRY );

    if ( !iPlugin )
        {
        User::Leave( KErrCorrupt );
        }

    iPlugin->SetString8AttributeL( aAttribute, aValue, iPluginDataInstance );

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_SETSTRING8ATTRIBUTEL_EXIT );
    }

// ---------------------------------------------------------------------------
// Sets a new status value.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::SetStatus( const TCmmConnMethodStatus& aStatus )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_SETSTATUS_ENTRY );

    iStatus = aStatus;

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_SETSTATUS_EXIT );
    }

// ---------------------------------------------------------------------------
// Called after this connection method has been updated and database
// transaction has completed successfully. Sets the internal state of this
// connection method instance to reflect the new valid state.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::UpdateSuccessful()
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_UPDATESUCCESSFUL_ENTRY );

    SetStatus( ECmmConnMethodStatusValid );

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_UPDATESUCCESSFUL_EXIT );
    }

// ---------------------------------------------------------------------------
// Called after this connection method has been deleted and database
// transaction has completed successfully. Sets the internal state of this
// connection method structure to reflect the new deleted state.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::DeleteSuccessful( const TUint32 aNewSecondaryId )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_DELETESUCCESSFUL_ENTRY );

    SetStatus( ECmmConnMethodStatusChanged );
    SetId( aNewSecondaryId );

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_DELETESUCCESSFUL_EXIT );
    }

// ---------------------------------------------------------------------------
// After update/delete to database, refresh temporary ID to real ID if
// necessary and refresh status information for any related handles for
// all client sessions.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::RefreshHandlesForAllSessions(
        const TCmmIdStruct& aIdStruct )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_REFRESHHANDLESFORALLSESSIONS_ENTRY );

    if ( iCmmSession )
        {
        iCmmSession->RefreshHandlesForAllSessions( aIdStruct );
        }

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_REFRESHHANDLESFORALLSESSIONS_EXIT );
    }

// ---------------------------------------------------------------------------
// Removes a connection method from any open destination handle in the same
// session.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::RemoveConnMethodFromSessionDestinationHandles(
        const TUint32 aConnMethodId )
    {
    if ( iCmmSession )
        {
        iCmmSession->RemoveConnMethodFromDestinationHandles( aConnMethodId );
        }
    }

// End of file
