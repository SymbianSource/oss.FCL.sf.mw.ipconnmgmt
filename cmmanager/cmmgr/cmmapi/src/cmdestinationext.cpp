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
* Implementation of RCmDestinationExt.
*
*/


#include <cmdestinationext.h>
#include <cmconnectionmethodext.h>

#include "cmmserverdefs.h"
#include "cmmanagerapi.h"
#include "cmdestinationwrapper.h"
#include "cmconnectionmethodwrapper.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmdestinationextTraces.h"
#endif


//-----------------------------------------------------------------------------
//  RCmDestinationExt::Close()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestinationExt::Close()
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_CLOSE_ENTRY );

    if ( iCmDestinationWrapper )
        {
        if ( iCmDestinationWrapper->DecreaseReferenceCounter() <= 0 )
            {
            delete iCmDestinationWrapper;
            }
        iCmDestinationWrapper = NULL;
        }

    OstTraceFunctionExit0( RCMDESTINATIONEXT_CLOSE_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::~RCmDestinationExt()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmDestinationExt::~RCmDestinationExt()
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_RCMDESTINATIONEXT_ENTRY );
    Close();
    OstTraceFunctionExit0( RCMDESTINATIONEXT_RCMDESTINATIONEXT_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::RCmDestinationExt()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmDestinationExt::RCmDestinationExt(
        const RCmDestinationExt& aDestination )
    {
    OstTraceFunctionEntry0( DUP1_RCMDESTINATIONEXT_RCMDESTINATIONEXT_ENTRY );

    this->iCmDestinationWrapper = aDestination.iCmDestinationWrapper;
    if ( iCmDestinationWrapper )
        {
        iCmDestinationWrapper->IncreaseReferenceCounter();
        }

    OstTraceFunctionExit0( DUP1_RCMDESTINATIONEXT_RCMDESTINATIONEXT_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::operator=()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmDestinationExt& RCmDestinationExt::operator=(
        const RCmDestinationExt& aDestination )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_OPERATOR_ENTRY );

    if ( this != &aDestination )
        {
        Close();
        this->iCmDestinationWrapper = aDestination.iCmDestinationWrapper;
        if ( iCmDestinationWrapper )
            {
            iCmDestinationWrapper->IncreaseReferenceCounter();
            }
        }

    OstTraceFunctionExit0( RCMDESTINATIONEXT_OPERATOR_EXIT );
    return *this;
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::ConnectionMethodCount()
//-----------------------------------------------------------------------------
//
EXPORT_C TInt RCmDestinationExt::ConnectionMethodCount()
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_CONNECTIONMETHODCOUNT_ENTRY );

    __ASSERT_ALWAYS( iCmDestinationWrapper && iCmDestinationWrapper->SessionConnected(),
            User::Panic( KCmmPanicCategoryApi, EDestNotConnectedToServer ) );

    TInt value( 0 );
    TInt err = iCmDestinationWrapper->ConnectionMethodCount( value );

    if ( err )
        {
        value = 0;
        }

    OstTraceFunctionExit0( RCMDESTINATIONEXT_CONNECTIONMETHODCOUNT_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::ConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethodExt RCmDestinationExt::ConnectionMethodL(
        TInt aIndex )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_CONNECTIONMETHODL_ENTRY );

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

    RCmConnectionMethodExt cm;
    cm.iCmConnectionMethodWrapper = cmWrapper;

    OstTraceFunctionExit0( RCMDESTINATIONEXT_CONNECTIONMETHODL_EXIT );
    return cm;
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::ConnectionMethodByIDL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethodExt RCmDestinationExt::ConnectionMethodByIDL(
        TInt aConnMethodId )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_CONNECTIONMETHODBYIDL_ENTRY );

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
        cmWrapper = iCmDestinationWrapper->GetMainSession()->GetConnMethodWrapperL( existingHandle );
        cmWrapper->IncreaseReferenceCounter();
        }
    else
        {
        CleanupStack::Pop( cmWrapper );
        }

    RCmConnectionMethodExt cm;
    cm.iCmConnectionMethodWrapper = cmWrapper;

    OstTraceFunctionExit0( RCMDESTINATIONEXT_CONNECTIONMETHODBYIDL_EXIT );
    return cm;
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::PriorityL()
//-----------------------------------------------------------------------------
//
EXPORT_C TUint RCmDestinationExt::PriorityL(
        const RCmConnectionMethodExt& aConnectionMethod )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_PRIORITYL_ENTRY );

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

    OstTraceFunctionExit0( RCMDESTINATIONEXT_PRIORITYL_EXIT );
    return priority;
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::NameLC()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC* RCmDestinationExt::NameLC()
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_NAMELC_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    HBufC* buffer = HBufC::NewLC( KCmmStringLengthMax );
    TInt err = iCmDestinationWrapper->GetName( buffer );
    User::LeaveIfError( err );
    // Leave buffer in cleanup stack.

    OstTraceFunctionExit0( RCMDESTINATIONEXT_NAMELC_EXIT );
    return buffer;
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::Id()
//-----------------------------------------------------------------------------
//
EXPORT_C TUint32 RCmDestinationExt::Id()
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_ID_ENTRY );

    __ASSERT_ALWAYS( iCmDestinationWrapper && iCmDestinationWrapper->SessionConnected(),
            User::Panic( KCmmPanicCategoryApi, EDestNotConnectedToServer ) );

    TUint32 id( 0 );
    TInt err = iCmDestinationWrapper->GetId( id );

    if ( err )
        {
        id = 0;
        }

    OstTraceFunctionExit0( RCMDESTINATIONEXT_ID_EXIT );
    return id;
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::ElementId()
//-----------------------------------------------------------------------------
//
EXPORT_C TUint32 RCmDestinationExt::ElementId()
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_ELEMENTID_ENTRY );

    __ASSERT_ALWAYS( iCmDestinationWrapper && iCmDestinationWrapper->SessionConnected(),
            User::Panic( KCmmPanicCategoryApi, EDestNotConnectedToServer ) );

    TUint32 elementId( 0 );
    TInt err = iCmDestinationWrapper->GetElementId( elementId );

    if ( err )
        {
        elementId = 0;
        }

    OstTraceFunctionExit0( RCMDESTINATIONEXT_ELEMENTID_EXIT );
    return elementId;
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::MetadataL()
//-----------------------------------------------------------------------------
//
EXPORT_C TUint32 RCmDestinationExt::MetadataL(
        CMManager::TSnapMetadataField aMetadataField ) const
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_METADATAL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TUint32 value( 0 );
    TInt err = iCmDestinationWrapper->GetMetadata( aMetadataField, value );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATIONEXT_METADATAL_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::ProtectionLevel()
//-----------------------------------------------------------------------------
//
EXPORT_C CMManager::TProtectionLevel RCmDestinationExt::ProtectionLevel()
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_PROTECTIONLEVEL_ENTRY );

    __ASSERT_ALWAYS( iCmDestinationWrapper && iCmDestinationWrapper->SessionConnected(),
            User::Panic( KCmmPanicCategoryApi, EDestNotConnectedToServer ) );

    TInt protectionLevel( CMManager::EProtLevel0 );
    TInt err = iCmDestinationWrapper->GetProtectionLevel( protectionLevel );

    if ( err )
        {
        protectionLevel = CMManager::EProtLevel0;
        }

    OstTraceFunctionExit0( RCMDESTINATIONEXT_PROTECTIONLEVEL_EXIT );
    return ( CMManager::TProtectionLevel )protectionLevel;
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::IsHidden()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool RCmDestinationExt::IsHidden()
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_ISHIDDEN_ENTRY );

    __ASSERT_ALWAYS( iCmDestinationWrapper && iCmDestinationWrapper->SessionConnected(),
            User::Panic( KCmmPanicCategoryApi, EDestNotConnectedToServer ) );

    TBool value( EFalse );
    TInt err = iCmDestinationWrapper->IsHidden( value );

    if ( err )
        {
        value = EFalse;
        }

    OstTraceFunctionExit0( RCMDESTINATIONEXT_ISHIDDEN_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::CreateConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethodExt RCmDestinationExt::CreateConnectionMethodL(
        TUint32 aBearerType )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_CREATECONNECTIONMETHODL_ENTRY );

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

    RCmConnectionMethodExt cm;
    cm.iCmConnectionMethodWrapper = cmWrapper;

    OstTraceFunctionExit0( RCMDESTINATIONEXT_CREATECONNECTIONMETHODL_EXIT );
    return cm;
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::CreateConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethodExt RCmDestinationExt::CreateConnectionMethodL(
        TUint32 aBearerType,
        TUint32 aConnMethodId )
    {
    OstTraceFunctionEntry0( DUP1_RCMDESTINATIONEXT_CREATECONNECTIONMETHODL_ENTRY );

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

    RCmConnectionMethodExt cm;
    cm.iCmConnectionMethodWrapper = cmWrapper;

    OstTraceFunctionExit0( DUP1_RCMDESTINATIONEXT_CREATECONNECTIONMETHODL_EXIT );
    return cm;
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::AddConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C TInt RCmDestinationExt::AddConnectionMethodL(
        RCmConnectionMethodExt aConnectionMethod )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_ADDCONNECTIONMETHODL_ENTRY );

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

    OstTraceFunctionExit0( RCMDESTINATIONEXT_ADDCONNECTIONMETHODL_EXIT );
    return index;
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::AddEmbeddedDestinationL()
//-----------------------------------------------------------------------------
//
EXPORT_C TInt RCmDestinationExt::AddEmbeddedDestinationL(
        const RCmDestinationExt& aDestination )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_ADDEMBEDDEDDESTINATIONL_ENTRY );

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

    OstTraceFunctionExit0( RCMDESTINATIONEXT_ADDEMBEDDEDDESTINATIONL_EXIT );
    return index;
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::DeleteConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestinationExt::DeleteConnectionMethodL(
        RCmConnectionMethodExt& aConnectionMethod )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_DELETECONNECTIONMETHODL_ENTRY );

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

    OstTraceFunctionExit0( RCMDESTINATIONEXT_DELETECONNECTIONMETHODL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::RemoveConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestinationExt::RemoveConnectionMethodL(
        RCmConnectionMethodExt aConnectionMethod )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_REMOVECONNECTIONMETHODL_ENTRY );

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

    OstTraceFunctionExit0( RCMDESTINATIONEXT_REMOVECONNECTIONMETHODL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::ModifyPriorityL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestinationExt::ModifyPriorityL(
        RCmConnectionMethodExt& aConnectionMethod,
        TUint aIndex )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_MODIFYPRIORITYL_ENTRY );

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

    OstTraceFunctionExit0( RCMDESTINATIONEXT_MODIFYPRIORITYL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::SetNameL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestinationExt::SetNameL( const TDesC& aName )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_SETNAMEL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmDestinationWrapper->SetName( aName );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATIONEXT_SETNAMEL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::SetMetadataL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestinationExt::SetMetadataL(
        CMManager::TSnapMetadataField aMetadataField,
        TUint32 aValue )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_SETMETADATAL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmDestinationWrapper->SetMetadata( aMetadataField, aValue );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATIONEXT_SETMETADATAL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::SetProtectionL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestinationExt::SetProtectionL( CMManager::TProtectionLevel aProtLevel )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_SETPROTECTIONL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmDestinationWrapper->SetProtection( aProtLevel );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATIONEXT_SETPROTECTIONL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::SetHiddenL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestinationExt::SetHiddenL( TBool aHidden )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_SETHIDDENL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmDestinationWrapper->SetHidden( aHidden );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATIONEXT_SETHIDDENL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::UpdateL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestinationExt::UpdateL()
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_UPDATEL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmDestinationWrapper->Update();
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATIONEXT_UPDATEL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::DeleteLD()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmDestinationExt::DeleteLD()
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_DELETELD_ENTRY );

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

    OstTraceFunctionExit0( RCMDESTINATIONEXT_DELETELD_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::IsConnectedL()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool RCmDestinationExt::IsConnectedL() const
    {
    OstTraceFunctionEntry0( RCMDESTINATIONEXT_ISCONNECTEDL_ENTRY );

    if ( !iCmDestinationWrapper || !iCmDestinationWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TBool value( 0 );
    TInt err = iCmDestinationWrapper->IsConnected( value );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMDESTINATIONEXT_ISCONNECTEDL_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::RCmDestinationExt()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmDestinationExt::RCmDestinationExt() : iCmDestinationWrapper( NULL )
    {
    OstTraceFunctionEntry0( DUP2_RCMDESTINATIONEXT_RCMDESTINATIONEXT_ENTRY );
    OstTraceFunctionExit0( DUP2_RCMDESTINATIONEXT_RCMDESTINATIONEXT_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmDestinationExt::operator==()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool RCmDestinationExt::operator==(
        const RCmDestinationExt& aDestination ) const
    {
    OstTraceFunctionEntry0( DUP1_RCMDESTINATIONEXT_OPERATOR_ENTRY );

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

    OstTraceFunctionExit0( DUP1_RCMDESTINATIONEXT_OPERATOR_EXIT );
    return result;
    }

//-----------------------------------------------------------------------------
//RCmDestinationExt::operator!=()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool RCmDestinationExt::operator!=(
        const RCmDestinationExt& aDestination ) const
    {
    OstTraceFunctionEntry0( DUP2_RCMDESTINATIONEXT_OPERATOR_ENTRY );

    TBool result = !( operator==( aDestination ) );

    OstTraceFunctionExit0( DUP2_RCMDESTINATIONEXT_OPERATOR_EXIT );
    return result;
    }

// End of file
