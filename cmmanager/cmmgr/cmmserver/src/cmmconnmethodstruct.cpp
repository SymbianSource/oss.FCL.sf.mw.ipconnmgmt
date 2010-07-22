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
* Cache side object representing a connection method.
*
*/


#include <e32base.h>
#include <cmpluginbaseeng.h>

#include "cmmconnmethodstruct.h"
#include "cmmconnmethodinstance.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmconnmethodstructTraces.h"
#endif


// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CCmmConnMethodStruct* CCmmConnMethodStruct::NewL(
        const TUint32 aConnMethodId )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODSTRUCT_NEWL_ENTRY );

    CCmmConnMethodStruct* self = CCmmConnMethodStruct::NewLC( aConnMethodId );
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMMCONNMETHODSTRUCT_NEWL_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CCmmConnMethodStruct* CCmmConnMethodStruct::NewLC(
        const TUint32 aConnMethodId )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODSTRUCT_NEWLC_ENTRY );

    CCmmConnMethodStruct* self = new( ELeave ) CCmmConnMethodStruct( aConnMethodId );
    CleanupStack::PushL( self );
    self->ConstructL();

    OstTraceFunctionExit0( CCMMCONNMETHODSTRUCT_NEWLC_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CCmmConnMethodStruct::~CCmmConnMethodStruct()
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODSTRUCT_CCMMCONNMETHODSTRUCT_ENTRY );

    if ( iConnMethodPlugin )
        {
        delete iConnMethodPlugin;
        iConnMethodPlugin = NULL;
        }

    OstTraceFunctionExit0( CCMMCONNMETHODSTRUCT_CCMMCONNMETHODSTRUCT_EXIT );
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CCmmConnMethodStruct::CCmmConnMethodStruct( TUint32 aConnMethodId )
        :
        iConnMethodId( aConnMethodId )
    {
    OstTraceFunctionEntry0( DUP1_CCMMCONNMETHODSTRUCT_CCMMCONNMETHODSTRUCT_ENTRY );

    iConnMethodPlugin = NULL;
    iBearerType = 0;
    iStatus = ECmmConnMethodStatusNotSaved;
    iReferenceCounter = 0;

    OstTraceFunctionExit0( DUP1_CCMMCONNMETHODSTRUCT_CCMMCONNMETHODSTRUCT_EXIT );
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodStruct::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// Return the reference count.
// ---------------------------------------------------------------------------
//
TInt CCmmConnMethodStruct::GetReferenceCounter()
    {
    return iReferenceCounter;
    }

// ---------------------------------------------------------------------------
// Increase the reference counter by one.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodStruct::IncrementReferenceCounter()
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODSTRUCT_INCREMENTREFERENCECOUNTER_ENTRY );
    iReferenceCounter++;
    OstTraceFunctionExit0( CCMMCONNMETHODSTRUCT_INCREMENTREFERENCECOUNTER_EXIT );
    }

// ---------------------------------------------------------------------------
// Return the connection method ID.
// ---------------------------------------------------------------------------
//
TUint32 CCmmConnMethodStruct::GetId() const
    {
    return iConnMethodId;
    }

// ---------------------------------------------------------------------------
// Set the connection method ID.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodStruct::SetId( const TUint32 aConnMethodId )
    {
    iConnMethodId = aConnMethodId;
    }

// ---------------------------------------------------------------------------
// Return bearer type.
// ---------------------------------------------------------------------------
//
TUint32 CCmmConnMethodStruct::GetBearerType() const
    {
    return iBearerType;
    }

// ---------------------------------------------------------------------------
// Gets the current status of this connection method struct.
// ---------------------------------------------------------------------------
//
TCmmConnMethodStatus CCmmConnMethodStruct::GetStatus() const
    {
    return iStatus;
    }


// ---------------------------------------------------------------------------
// Sets a new status value.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodStruct::SetStatus( const TCmmConnMethodStatus& aStatus )
    {
    iStatus = aStatus;
    }

// ---------------------------------------------------------------------------
// Refresh the connection method data in this cache side object to be in synch
// with the database and copy that data back to the session side connection
// method instance given as parameter.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodStruct::RefreshConnMethodInstanceL(
        CCmmConnMethodInstance& aConnMethodInstance )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODSTRUCT_REFRESHCONNMETHODINSTANCEL_ENTRY );

    if ( !iConnMethodPlugin )
        {
        User::Leave( KErrCorrupt );
        }

    switch ( iStatus )
        {
        case ECmmConnMethodStatusValid:
        case ECmmConnMethodStatusToBeDeleted:
            {
            //TODO, add record status check later and only call Reload() if necessary. TCmmRecordStatus
            //if ( !aConnMethodStruct->UpToDate() )
            //    {
                iConnMethodPlugin->ReLoadL();
            //    }
            iConnMethodPlugin->GetPluginDataL( aConnMethodInstance.GetPluginDataInstance() );
            }
            break;
        case ECmmConnMethodStatusNotSaved: // This is checked before.
        case ECmmConnMethodStatusChanged:
        default:
            User::Leave( KErrCorrupt );  // Error, invalid status.
            break;
        }

    // Internal state need to be set to the same state as after a successfull update.
    aConnMethodInstance.UpdateSuccessful();

    OstTraceFunctionExit0( CCMMCONNMETHODSTRUCT_REFRESHCONNMETHODINSTANCEL_EXIT );
    }

// ---------------------------------------------------------------------------
// Decrease the reference counter by one. Return the remaining number of
// references.
// ---------------------------------------------------------------------------
//
TInt CCmmConnMethodStruct::ConnMethodInstanceClosed()
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODSTRUCT_SESSIONINSTANCECLOSED_ENTRY );

    iReferenceCounter--;
    if ( iReferenceCounter < 0 )
        {
        ASSERT( 0 );
        iReferenceCounter = 0;
        }

    OstTraceFunctionExit0( CCMMCONNMETHODSTRUCT_SESSIONINSTANCECLOSED_EXIT );
    return iReferenceCounter;
    }

// ---------------------------------------------------------------------------
// Set the connection method plugin pointer, bearer type and status.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodStruct::SetPlugin(
        CCmPluginBaseEng* aPlugin,
        TUint32 aBearerType,
        TCmmConnMethodStatus aStatus )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODSTRUCT_SETPLUGIN_ENTRY );

    // Only check reference counter, it is incremented by session side instance
    // when data is copied into it.
    if ( iReferenceCounter != 0 )
        {
        ASSERT( 0 ); // Error, wrong internal status.
        }

    iConnMethodPlugin = aPlugin;
    iBearerType = aBearerType;

    switch ( aStatus )
        {
        // Fallthrough intended
        case ECmmConnMethodStatusNotSaved:
        case ECmmConnMethodStatusValid:
            iStatus = aStatus;
            break;
        case ECmmConnMethodStatusChanged:
        case ECmmConnMethodStatusToBeDeleted:
        default:
            iStatus = ECmmConnMethodStatusChanged;
            ASSERT( 0 ); // Error, invalid status as argument.
            break;
        }

    OstTraceFunctionExit0( CCMMCONNMETHODSTRUCT_SETPLUGIN_EXIT );
    }

// ---------------------------------------------------------------------------
// Returns the connection method plugin.
// ---------------------------------------------------------------------------
//
CCmPluginBaseEng* CCmmConnMethodStruct::GetPlugin()
    {
    return iConnMethodPlugin;
    }

// ---------------------------------------------------------------------------
// Called after this connection method has been deleted and database
// transaction has completed successfully. Sets the internal state of this
// connection method structure to reflect the new deleted state.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodStruct::DeleteSuccessful( const TUint32 aNewSecondaryId )
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODSTRUCT_DELETESUCCESSFUL_ENTRY );

    iConnMethodId = aNewSecondaryId;
    iStatus = ( ECmmConnMethodStatusNotSaved );

    OstTraceFunctionExit0( CCMMCONNMETHODSTRUCT_DELETESUCCESSFUL_EXIT );
    }

// ---------------------------------------------------------------------------
// Called after this connection method has been updated and database
// transaction has completed successfully. Sets the internal state of this
// connection method structure to reflect the new valid state.
// ---------------------------------------------------------------------------
//
void CCmmConnMethodStruct::UpdateSuccessful()
    {
    OstTraceFunctionEntry0( CCMMCONNMETHODSTRUCT_UPDATESUCCESSFUL_ENTRY );
    iStatus = ECmmConnMethodStatusValid;
    OstTraceFunctionExit0( CCMMCONNMETHODSTRUCT_UPDATESUCCESSFUL_EXIT );
    }

// End of file
