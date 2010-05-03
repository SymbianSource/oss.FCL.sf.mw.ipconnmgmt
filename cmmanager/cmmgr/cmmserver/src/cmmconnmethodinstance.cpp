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
CCmmConnMethodInstance* CCmmConnMethodInstance::NewL( CCmmSession* aCmmSession, CCmmCache* aCache )
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
CCmmConnMethodInstance* CCmmConnMethodInstance::NewLC( CCmmSession* aCmmSession, CCmmCache* aCache )
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
CCmmConnMethodInstance::CCmmConnMethodInstance( CCmmSession* aCmmSession, CCmmCache* aCache )
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
// Set the plugin base pointer.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::SetPlugin( CCmPluginBaseEng* aPlugin )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_SETPLUGIN_ENTRY );

    iPlugin = aPlugin;

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_SETPLUGIN_EXIT );
    }

// ---------------------------------------------------------------------------
// Get the plugin base pointer.
// ---------------------------------------------------------------------------
//
CCmPluginBaseEng* CCmmConnMethodInstance::GetPlugin() const
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_GETPLUGIN_ENTRY );
    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_GETPLUGIN_EXIT );
    return iPlugin;
    }

// ---------------------------------------------------------------------------
// Get pointer to the plugin data container.
// ---------------------------------------------------------------------------
//
CCmClientPluginInstance* CCmmConnMethodInstance::GetPluginDataInstance() const
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_GETPLUGINDATAINSTANCE_ENTRY );
    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_GETPLUGINDATAINSTANCE_EXIT );
    return iPluginDataInstance;
    }

// ---------------------------------------------------------------------------
// Get connection method ID.
// ---------------------------------------------------------------------------
//
TUint32 CCmmConnMethodInstance::GetId() const
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_GETID_ENTRY );
    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_GETID_EXIT );

    return iConnMethodId;
    }

// ---------------------------------------------------------------------------
// Set connection method ID.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::SetId( const TUint32& aConnMethodId )
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
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_GETBEARERTYPE_ENTRY );
    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_GETBEARERTYPE_EXIT );

    return iBearerType;
    }

// ---------------------------------------------------------------------------
// Get bearer type.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::SetBearerType( const TUint32& aBearerType )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_SETBEARERTYPE_ENTRY );

    iBearerType = aBearerType;

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_SETBEARERTYPE_EXIT );
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
void CCmmConnMethodInstance::SetHandle( const TInt& aHandle )
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

    iPlugin->GetPluginDataL( iPluginDataInstance ); //TODO, check deletes for iPluginDataInstance
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
// Refresh the relevant connection method data in cache side object to be in
// synch with database and copy that data back to this session side connection
// method object.
// ---------------------------------------------------------------------------
//
//TODO, cleanup
/*
void CCmmConnMethodInstance::RefreshDataL( CCmmConnMethodStruct* aConnMethodStruct ) //TODO, remove
    {

    //TODO
    //if ( !aConnMethodStruct )
        //{
        //User::Leave( KErrCorrupt );
        //}
    if ( !iPlugin )
        {
        User::Leave( KErrCorrupt );
        }

    switch ( aConnMethodStruct->GetStatus() )
        {
        case ECmmConnMethodStatusValid:
        case ECmmConnMethodStatusToBeDeleted:
            {
            //TODO, add record status check later and only call Reload() if necessary. TCmmRecordStatus
            //if ( !aConnMethodStruct->UpToDate() )
            //    {
                iPlugin->ReLoadL();
            //    }
            iPlugin->GetPluginDataL( iPluginDataInstance );
            }
            break;
        case ECmmConnMethodStatusNotSaved: // This is checked before.
        case ECmmConnMethodStatusChanged:
        default:
            User::Leave( KErrCorrupt );  // Error, invalid status.
            break;
        }

    // Internal state need to be set to the same state as after a successfull update.
    UpdateSuccessful();

    }*/

// ---------------------------------------------------------------------------
// CCmmConnMethodInstance::GetIntAttributeL
// ---------------------------------------------------------------------------
//
TUint32 CCmmConnMethodInstance::GetIntAttributeL( const TUint32& aAttribute )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_GETINTATTRIBUTEL_ENTRY );

    __ASSERT_DEBUG( iPlugin != NULL, User::Leave( KErrNotFound ) );

    return iPlugin->GetIntAttributeL( aAttribute, iPluginDataInstance );
    }

// ---------------------------------------------------------------------------
// CCmmConnMethodInstance::GetBoolAttributeL
// ---------------------------------------------------------------------------
//
TBool CCmmConnMethodInstance::GetBoolAttributeL( const TUint32& aAttribute )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_GETBOOLATTRIBUTEL_ENTRY );

    __ASSERT_DEBUG( iPlugin != NULL, User::Leave( KErrNotFound ) );

    TBool retVal( EFalse );

    switch ( aAttribute )
        {
        case CMManager::ECmConnected:
            {
            if ( GetId() > 0 )
                {
                if ( iCache )
                    {
                    retVal = iCache->CheckIfCmConnected( GetId() );
                    }
                }
            }
            break;
        case CMManager::ECmIsLinked:
            {
            //TODO
            }
            break;
        default:
            {
            retVal = iPlugin->GetBoolAttributeL( aAttribute, iPluginDataInstance );
            }
        }

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_GETBOOLATTRIBUTEL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmmConnMethodInstance::GetStringAttributeL
// ---------------------------------------------------------------------------
//
HBufC* CCmmConnMethodInstance::GetStringAttributeL( const TUint32& aAttribute )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_GETSTRINGATTRIBUTEL_ENTRY );

    __ASSERT_DEBUG( iPlugin != NULL, User::Leave( KErrNotFound ) );

    return iPlugin->GetStringAttributeL( aAttribute, iPluginDataInstance );
    }

// ---------------------------------------------------------------------------
// CCmmConnMethodInstance::GetString8AttributeL
// ---------------------------------------------------------------------------
//
HBufC8* CCmmConnMethodInstance::GetString8AttributeL( const TUint32& aAttribute )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_GETSTRING8ATTRIBUTEL_ENTRY );

    __ASSERT_DEBUG( iPlugin != NULL, User::Leave( KErrNotFound ) );

    return iPlugin->GetString8AttributeL( aAttribute, iPluginDataInstance );
    }

// ---------------------------------------------------------------------------
// CCmmConnMethodInstance::SetIntAttributeL
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::SetIntAttributeL(
        const TUint32& aAttribute,
        const TUint32& aValue )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_SETINTATTRIBUTEL_ENTRY );

    __ASSERT_DEBUG( iPlugin != NULL, User::Leave( KErrNotFound ) );

    iPlugin->SetIntAttributeL( aAttribute, aValue, iPluginDataInstance );

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_SETINTATTRIBUTEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmmConnMethodInstance::SetBoolAttributeL
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::SetBoolAttributeL(
        const TUint32& aAttribute,
        const TBool& aValue )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_SETBOOLATTRIBUTEL_ENTRY );

    __ASSERT_DEBUG( iPlugin != NULL, User::Leave( KErrNotFound ) );

    iPlugin->SetBoolAttributeL( aAttribute, aValue, iPluginDataInstance );

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_SETBOOLATTRIBUTEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmmConnMethodInstance::SetStringAttributeL
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::SetStringAttributeL(
        const TUint32& aAttribute,
        const TDesC16& aValue )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_SETSTRINGATTRIBUTEL_ENTRY );

    __ASSERT_DEBUG( iPlugin != NULL, User::Leave( KErrNotFound ) );

    iPlugin->SetStringAttributeL( aAttribute, aValue, iPluginDataInstance );

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_SETSTRINGATTRIBUTEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmmConnMethodInstance::SetString8AttributeL
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::SetString8AttributeL(
        const TUint32& aAttribute,
        const TDesC8& aValue )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_SETSTRING8ATTRIBUTEL_ENTRY );

    __ASSERT_DEBUG( iPlugin != NULL, User::Leave( KErrNotFound ) );

    iPlugin->SetString8AttributeL( aAttribute, aValue, iPluginDataInstance );

    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_SETSTRING8ATTRIBUTEL_EXIT );
    }

// ---------------------------------------------------------------------------
// Gets the current status of this connection method instance.
// ---------------------------------------------------------------------------
//
TCmmConnMethodStatus CCmmConnMethodInstance::GetStatus() const
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODINSTANCE_GETSTATUS_ENTRY );
    OstTraceFunctionExit0( CCMMCONNMETHODINSTANCE_GETSTATUS_EXIT );
    return iStatus;
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
    SetStatus( ECmmConnMethodStatusValid );
    }

// ---------------------------------------------------------------------------
// Called after this connection method has been deleted and database
// transaction has completed successfully. Sets the internal state of this
// connection method structure to reflect the new deleted state.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodInstance::DeleteSuccessful( const TUint32& aNewSecondaryId )
    {
    SetStatus( ECmmConnMethodStatusChanged );
    SetId( aNewSecondaryId );
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
        const TUint32& aConnMethodId )
    {
    if ( iCmmSession )
        {
        iCmmSession->RemoveConnMethodFromDestinationHandles( aConnMethodId );
        }
    }

// End of file
