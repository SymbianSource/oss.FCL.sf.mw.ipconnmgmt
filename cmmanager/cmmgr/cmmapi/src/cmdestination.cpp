/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation of RCmDestination.
*
*/


#include <cmdestination.h>
#include <cmconnectionmethod.h>

#include "cmmserverdefs.h"
#include "cmmanagerapi.h"
#include "cmdestinationwrapper.h"
#include "cmconnectionmethodwrapper.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmdestinationTraces.h"
#endif


//-----------------------------------------------------------------------------
//  RCmDestination::Close()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestination::Close()
    {
    OstTraceFunctionEntry0( RCMDESTINATION_CLOSE_ENTRY );

    if ( iCmDestinationWrapper )
        {
        if ( iCmDestinationWrapper->DecreaseReferenceCounter() <= 0 )
            {
            delete iCmDestinationWrapper;
            }
        iCmDestinationWrapper = NULL;
        }

    OstTraceFunctionExit0( RCMDESTINATION_CLOSE_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestination::~RCmDestination()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmDestination::~RCmDestination()
    {
    OstTraceFunctionEntry0( RCMDESTINATION_RCMDESTINATION_ENTRY );
    Close();
    OstTraceFunctionExit0( RCMDESTINATION_RCMDESTINATION_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestination::RCmDestination()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmDestination::RCmDestination(
        const RCmDestination& aDestination )
    {
    OstTraceFunctionEntry0( DUP1_RCMDESTINATION_RCMDESTINATION_ENTRY );

    this->iCmDestinationWrapper = aDestination.iCmDestinationWrapper;
    if ( iCmDestinationWrapper )
        {
        iCmDestinationWrapper->IncreaseReferenceCounter();
        }

    OstTraceFunctionExit0( DUP1_RCMDESTINATION_RCMDESTINATION_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestination::operator=()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmDestination& RCmDestination::operator=(
        const RCmDestination& aDestination )
    {
    OstTraceFunctionEntry0( RCMDESTINATION_OPERATOR_ENTRY );

    if ( this != &aDestination )
        {
        Close();
        this->iCmDestinationWrapper = aDestination.iCmDestinationWrapper;
        if ( iCmDestinationWrapper )
            {
            iCmDestinationWrapper->IncreaseReferenceCounter();
            }
        }

    OstTraceFunctionExit0( RCMDESTINATION_OPERATOR_EXIT );
    return *this;
    }

//-----------------------------------------------------------------------------
//  RCmDestination::ConnectionMethodCount()
//-----------------------------------------------------------------------------
//
EXPORT_C TInt RCmDestination::ConnectionMethodCount() const
    {
    OstTraceFunctionEntry0( RCMDESTINATION_CONNECTIONMETHODCOUNT_ENTRY );

    __ASSERT_ALWAYS( iCmDestinationWrapper && iCmDestinationWrapper->SessionConnected(),
            User::Panic( KCmmPanicCategoryApi, EDestNotConnectedToServer ) );

    TInt value( 0 );
    TInt err = iCmDestinationWrapper->ConnectionMethodCount( value );

    if ( err )
        {
        value = 0;
        }

    OstTraceFunctionExit0( RCMDESTINATION_CONNECTIONMETHODCOUNT_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmDestination::ConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethod RCmDestination::ConnectionMethodL(
        TInt aIndex ) const
    {
    OstTraceFunctionEntry0( RCMDESTINATION_CONNECTIONMETHODL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    CCmConnectionMethodWrapper* cmWrapper = CCmConnectionMethodWrapper::NewLC();
    TInt destHandle = iCmDestinationWrapper->GetHandle();
    TInt existingHandle( 0 );
    cmWrapper->GetConnMethodFromDestWithIndexL(
            iCmDestinationWrapper->GetMainSession(),
            destHandle,
            aIndex,
            existingHandle );
    if ( existingHandle )
        {
        CleanupStack::PopAndDestroy( cmWrapper );
        cmWrapper = iCmDestinationWrapper->GetMainSession()->GetConnMethodWrapperL( existingHandle );
        cmWrapper->IncreaseReferenceCounter();
        }
    else
        {
        CleanupStack::Pop( cmWrapper );
        }

    RCmConnectionMethod cm;
    cm.iCmConnectionMethodWrapper = cmWrapper;

    OstTraceFunctionExit0( RCMDESTINATION_CONNECTIONMETHODL_EXIT );
    return cm;
    }

//-----------------------------------------------------------------------------
//  RCmDestination::ConnectionMethodByIDL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethod RCmDestination::ConnectionMethodByIDL(
        TInt aConnMethodId ) const
    {
    OstTraceFunctionEntry0( RCMDESTINATION_CONNECTIONMETHODBYIDL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    CCmConnectionMethodWrapper* cmWrapper = CCmConnectionMethodWrapper::NewLC();
    TInt destHandle = iCmDestinationWrapper->GetHandle();
    TInt existingHandle( 0 );
    cmWrapper->GetConnMethodFromDestWithIdL(
            iCmDestinationWrapper->GetMainSession(),
            destHandle,
            aConnMethodId,
            existingHandle );
    if ( existingHandle )
        {
        CleanupStack::PopAndDestroy( cmWrapper );
        cmWrapper = iCmDestinationWrapper->GetMainSession()->GetConnMethodWrapperL(
                existingHandle );
        cmWrapper->IncreaseReferenceCounter();
        }
    else
        {
        CleanupStack::Pop( cmWrapper );
        }

    RCmConnectionMethod cm;
    cm.iCmConnectionMethodWrapper = cmWrapper;

    OstTraceFunctionExit0( RCMDESTINATION_CONNECTIONMETHODBYIDL_EXIT );
    return cm;
    }

//-----------------------------------------------------------------------------
//  RCmDestination::PriorityL()
//-----------------------------------------------------------------------------
//
EXPORT_C TUint RCmDestination::PriorityL(
        const RCmConnectionMethod& aConnectionMethod ) const
    {
    OstTraceFunctionEntry0( RCMDESTINATION_PRIORITYL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }
    if ( !aConnectionMethod.iCmConnectionMethodWrapper ||
            !aConnectionMethod.iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrArgument );
        }

    TUint priority( 0 );
    TInt handle = aConnectionMethod.iCmConnectionMethodWrapper->GetHandle();
    TInt err = iCmDestinationWrapper->Priority( handle, priority );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATION_PRIORITYL_EXIT );
    return priority;
    }

//-----------------------------------------------------------------------------
//  RCmDestination::NameLC()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC* RCmDestination::NameLC() const
    {
    OstTraceFunctionEntry0( RCMDESTINATION_NAMELC_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    HBufC* buffer = HBufC::NewLC( KCmmStringLengthMax );
    TInt err = iCmDestinationWrapper->GetName( buffer );
    User::LeaveIfError( err );
    // Leave buffer in cleanup stack.

    OstTraceFunctionExit0( RCMDESTINATION_NAMELC_EXIT );
    return buffer;
    }

//-----------------------------------------------------------------------------
//  RCmDestination::Id()
//-----------------------------------------------------------------------------
//
EXPORT_C TUint32 RCmDestination::Id() const
    {
    OstTraceFunctionEntry0( RCMDESTINATION_ID_ENTRY );

    __ASSERT_ALWAYS( iCmDestinationWrapper && iCmDestinationWrapper->SessionConnected(),
            User::Panic( KCmmPanicCategoryApi, EDestNotConnectedToServer ) );

    TUint32 id( 0 );
    TInt err = iCmDestinationWrapper->GetId( id );

    if ( err )
        {
        id = 0;
        }

    OstTraceFunctionExit0( RCMDESTINATION_ID_EXIT );
    return id;
    }

//-----------------------------------------------------------------------------
//  RCmDestination::ElementId()
//-----------------------------------------------------------------------------
//
EXPORT_C TUint32 RCmDestination::ElementId() const
    {
    OstTraceFunctionEntry0( RCMDESTINATION_ELEMENTID_ENTRY );

    __ASSERT_ALWAYS( iCmDestinationWrapper && iCmDestinationWrapper->SessionConnected(),
            User::Panic( KCmmPanicCategoryApi, EDestNotConnectedToServer ) );

    TUint32 elementId( 0 );
    TInt err = iCmDestinationWrapper->GetElementId( elementId );

    if ( err )
        {
        elementId = 0;
        }

    OstTraceFunctionExit0( RCMDESTINATION_ELEMENTID_EXIT );
    return elementId;
    }

//-----------------------------------------------------------------------------
//  RCmDestination::MetadataL()
//-----------------------------------------------------------------------------
//
EXPORT_C TUint32 RCmDestination::MetadataL(
        CMManager::TSnapMetadataField aMetadataField ) const
    {
    OstTraceFunctionEntry0( RCMDESTINATION_METADATAL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TUint32 value( 0 );
    TInt err = iCmDestinationWrapper->GetMetadata( aMetadataField, value );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATION_METADATAL_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmDestination::ProtectionLevel()
//-----------------------------------------------------------------------------
//
EXPORT_C CMManager::TProtectionLevel RCmDestination::ProtectionLevel() const
    {
    OstTraceFunctionEntry0( RCMDESTINATION_PROTECTIONLEVEL_ENTRY );

    __ASSERT_ALWAYS( iCmDestinationWrapper && iCmDestinationWrapper->SessionConnected(),
            User::Panic( KCmmPanicCategoryApi, EDestNotConnectedToServer ) );

    TInt protectionLevel( CMManager::EProtLevel0 );
    TInt err = iCmDestinationWrapper->GetProtectionLevel( protectionLevel );

    if ( err )
        {
        protectionLevel = CMManager::EProtLevel0;
        }

    OstTraceFunctionExit0( RCMDESTINATION_PROTECTIONLEVEL_EXIT );
    return ( CMManager::TProtectionLevel )protectionLevel;
    }

//-----------------------------------------------------------------------------
//  RCmDestination::IsHidden()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool RCmDestination::IsHidden() const
    {
    OstTraceFunctionEntry0( RCMDESTINATION_ISHIDDEN_ENTRY );

    __ASSERT_ALWAYS( iCmDestinationWrapper && iCmDestinationWrapper->SessionConnected(),
            User::Panic( KCmmPanicCategoryApi, EDestNotConnectedToServer ) );

    TBool value( EFalse );
    TInt err = iCmDestinationWrapper->IsHidden( value );

    if ( err )
        {
        value = EFalse;
        }

    OstTraceFunctionExit0( RCMDESTINATION_ISHIDDEN_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmDestination::CreateConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethod RCmDestination::CreateConnectionMethodL(
        TUint32 aBearerType )
    {
    OstTraceFunctionEntry0( RCMDESTINATION_CREATECONNECTIONMETHODL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    CCmConnectionMethodWrapper* cmWrapper = CCmConnectionMethodWrapper::NewLC();
    TInt destHandle = iCmDestinationWrapper->GetHandle();
    cmWrapper->CreateConnMethodToDestL(
            iCmDestinationWrapper->GetMainSession(),
            destHandle,
            aBearerType );
    CleanupStack::Pop( cmWrapper );

    RCmConnectionMethod cm;
    cm.iCmConnectionMethodWrapper = cmWrapper;

    OstTraceFunctionExit0( RCMDESTINATION_CREATECONNECTIONMETHODL_EXIT );
    return cm;
    }

//-----------------------------------------------------------------------------
//  RCmDestination::CreateConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethod RCmDestination::CreateConnectionMethodL(
        TUint32 aBearerType,
        TUint32 aConnMethodId )
    {
    OstTraceFunctionEntry0( DUP1_RCMDESTINATION_CREATECONNECTIONMETHODL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    CCmConnectionMethodWrapper* cmWrapper = CCmConnectionMethodWrapper::NewLC();
    TInt destHandle = iCmDestinationWrapper->GetHandle();
    cmWrapper->CreateConnMethodToDestWithIdL(
            iCmDestinationWrapper->GetMainSession(),
            destHandle,
            aBearerType,
            aConnMethodId );
    CleanupStack::Pop( cmWrapper );

    RCmConnectionMethod cm;
    cm.iCmConnectionMethodWrapper = cmWrapper;

    OstTraceFunctionExit0( DUP1_RCMDESTINATION_CREATECONNECTIONMETHODL_EXIT );
    return cm;
    }

//-----------------------------------------------------------------------------
//  RCmDestination::AddConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C TInt RCmDestination::AddConnectionMethodL(
        RCmConnectionMethod aConnectionMethod )
    {
    OstTraceFunctionEntry0( RCMDESTINATION_ADDCONNECTIONMETHODL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }
    if ( !aConnectionMethod.iCmConnectionMethodWrapper ||
            !aConnectionMethod.iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrArgument );
        }

    TInt index( 0 );
    TInt connMethodHandle = aConnectionMethod.iCmConnectionMethodWrapper->GetHandle();
    TInt err = iCmDestinationWrapper->AddConnectionMethod( connMethodHandle, index );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATION_ADDCONNECTIONMETHODL_EXIT );
    return index;
    }

//-----------------------------------------------------------------------------
//  RCmDestination::AddEmbeddedDestinationL()
//-----------------------------------------------------------------------------
//
EXPORT_C TInt RCmDestination::AddEmbeddedDestinationL(
        const RCmDestination& aDestination )
    {
    OstTraceFunctionEntry0( RCMDESTINATION_ADDEMBEDDEDDESTINATIONL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }
    if ( iCmDestinationWrapper == aDestination.iCmDestinationWrapper )
        {
        User::Leave( KErrArgument );
        }
    if ( !aDestination.iCmDestinationWrapper ||
            !aDestination.iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrArgument );
        }

    TInt index( 0 );
    TInt destinationHandle = aDestination.iCmDestinationWrapper->GetHandle();
    TInt err = iCmDestinationWrapper->AddEmbeddedDestination( destinationHandle, index );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATION_ADDEMBEDDEDDESTINATIONL_EXIT );
    return index;
    }

//-----------------------------------------------------------------------------
//  RCmDestination::DeleteConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestination::DeleteConnectionMethodL(
        RCmConnectionMethod& aConnectionMethod )
    {
    OstTraceFunctionEntry0( RCMDESTINATION_DELETECONNECTIONMETHODL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }
    if ( !aConnectionMethod.iCmConnectionMethodWrapper ||
            !aConnectionMethod.iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrArgument );
        }

    TInt connMethodHandle = aConnectionMethod.iCmConnectionMethodWrapper->GetHandle();
    TInt err = iCmDestinationWrapper->DeleteConnectionMethod( connMethodHandle );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATION_DELETECONNECTIONMETHODL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestination::RemoveConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestination::RemoveConnectionMethodL(
        RCmConnectionMethod aConnectionMethod )
    {
    OstTraceFunctionEntry0( RCMDESTINATION_REMOVECONNECTIONMETHODL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }
    if ( !aConnectionMethod.iCmConnectionMethodWrapper ||
            !aConnectionMethod.iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrArgument );
        }

    TInt connMethodHandle = aConnectionMethod.iCmConnectionMethodWrapper->GetHandle();
    TInt err = iCmDestinationWrapper->RemoveConnectionMethod( connMethodHandle );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATION_REMOVECONNECTIONMETHODL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestination::ModifyPriorityL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestination::ModifyPriorityL(
        RCmConnectionMethod& aConnectionMethod,
        TUint aIndex )
    {
    OstTraceFunctionEntry0( RCMDESTINATION_MODIFYPRIORITYL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }
    if ( !aConnectionMethod.iCmConnectionMethodWrapper ||
            !aConnectionMethod.iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrArgument );
        }

    TInt connMethodHandle = aConnectionMethod.iCmConnectionMethodWrapper->GetHandle();
    TInt err = iCmDestinationWrapper->ModifyPriority( connMethodHandle, aIndex );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATION_MODIFYPRIORITYL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestination::SetNameL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestination::SetNameL( const TDesC& aName )
    {
    OstTraceFunctionEntry0( RCMDESTINATION_SETNAMEL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmDestinationWrapper->SetName( aName );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATION_SETNAMEL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestination::SetMetadataL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestination::SetMetadataL(
        CMManager::TSnapMetadataField aMetadataField,
        TUint32 aValue )
    {
    OstTraceFunctionEntry0( RCMDESTINATION_SETMETADATAL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmDestinationWrapper->SetMetadata( aMetadataField, aValue );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATION_SETMETADATAL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestination::SetProtectionL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestination::SetProtectionL( CMManager::TProtectionLevel aProtLevel )
    {
    OstTraceFunctionEntry0( RCMDESTINATION_SETPROTECTIONL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmDestinationWrapper->SetProtection( aProtLevel );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATION_SETPROTECTIONL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestination::SetHiddenL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestination::SetHiddenL( TBool aHidden )
    {
    OstTraceFunctionEntry0( RCMDESTINATION_SETHIDDENL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmDestinationWrapper->SetHidden( aHidden );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATION_SETHIDDENL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestination::UpdateL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestination::UpdateL()
    {
    OstTraceFunctionEntry0( RCMDESTINATION_UPDATEL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmDestinationWrapper->Update();
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATION_UPDATEL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestination::DeleteLD()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestination::DeleteLD()
    {
    OstTraceFunctionEntry0( RCMDESTINATION_DELETELD_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmDestinationWrapper->Delete();
    User::LeaveIfError( err );

    // Instead of calling Close(), we need to force this handle closed even if
    // copies have been made. So we delete the destination wrapper directly.
    delete iCmDestinationWrapper;
    iCmDestinationWrapper = NULL;

    OstTraceFunctionExit0( RCMDESTINATION_DELETELD_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestination::IsConnectedL()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool RCmDestination::IsConnectedL() const
    {
    OstTraceFunctionEntry0( RCMDESTINATION_ISCONNECTEDL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TBool value( 0 );
    TInt err = iCmDestinationWrapper->IsConnected( value );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATION_ISCONNECTEDL_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmDestination::RCmDestination()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmDestination::RCmDestination() : iCmDestinationWrapper( NULL )
    {
    OstTraceFunctionEntry0( DUP2_RCMDESTINATION_RCMDESTINATION_ENTRY );
    OstTraceFunctionExit0( DUP2_RCMDESTINATION_RCMDESTINATION_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestination::operator==()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool RCmDestination::operator==(
        const RCmDestination& aDestination ) const
    {
    OstTraceFunctionEntry0( DUP1_RCMDESTINATION_OPERATOR_ENTRY );

    TBool result( EFalse );
    if ( !aDestination.iCmDestinationWrapper )
        {
        if ( !iCmDestinationWrapper )
            {
            result = ETrue; // Both empty.
            }
        else
            {
            result = EFalse;
            }
        }
    else if ( !iCmDestinationWrapper )
        {
        result = EFalse;
        }
    else
        {
        // Both destination objects have wrappers.

        if ( !iCmDestinationWrapper->SessionConnected() ||
                !aDestination.iCmDestinationWrapper->SessionConnected() )
            {
            result = EFalse;
            }
        else
            {
            // Both destination objects are connected to server.

            TInt handle = aDestination.iCmDestinationWrapper->GetHandle();
            TInt err = iCmDestinationWrapper->IsEqual( handle, result );
            if ( err )
                {
                result = EFalse;
                }
            }
        }

    OstTraceFunctionExit0( DUP1_RCMDESTINATION_OPERATOR_EXIT );
    return result;
    }

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//
EXPORT_C TBool RCmDestination::operator!=(
        const RCmDestination& aDestination ) const
    {
    OstTraceFunctionEntry0( DUP2_RCMDESTINATION_OPERATOR_ENTRY );

    TBool result = !( operator==( aDestination ) );

    OstTraceFunctionExit0( DUP2_RCMDESTINATION_OPERATOR_EXIT );
    return result;
    }

//-----------------------------------------------------------------------------
//  RCmDestination::GetIconL()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC* RCmDestination::GetIconL() const
    {
    OstTraceFunctionEntry0( RCMDESTINATION_GETICONL_ENTRY );

    User::Leave( KErrNotSupported );

    OstTraceFunctionExit0( RCMDESTINATION_GETICONL_EXIT );
    return NULL;
    }

//-----------------------------------------------------------------------------
//  RCmDestination::SetIconL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestination::SetIconL( const TDesC& /*aIcon*/ )
    {
    OstTraceFunctionEntry0( RCMDESTINATION_SETICONL_ENTRY );

    User::Leave( KErrNotSupported );

    OstTraceFunctionExit0( RCMDESTINATION_SETICONL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestination::RefreshL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestination::RefreshL()
    {
    OstTraceFunctionEntry0( RCMDESTINATION_REFRESHL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmDestinationWrapper->Refresh();
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATION_REFRESHL_EXIT );
    }

// End of file
