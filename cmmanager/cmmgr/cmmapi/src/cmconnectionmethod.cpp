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
* Implementation of RCmConnectionMethod.
*
*/


#include <cmconnectionmethod.h>
#include <cmdestination.h>

#include "cmmserverdefs.h"
#include "cmmanagerapi.h"
#include "cmdestinationwrapper.h"
#include "cmconnectionmethodwrapper.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmconnectionmethodTraces.h"
#endif


//-----------------------------------------------------------------------------
//  RCmConnectionMethod::RCmConnectionMethod()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethod::RCmConnectionMethod(
        const RCmConnectionMethod& aConnMethod )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHOD_RCMCONNECTIONMETHOD_ENTRY );

    this->iCmConnectionMethodWrapper = aConnMethod.iCmConnectionMethodWrapper;
    if ( iCmConnectionMethodWrapper )
        {
        iCmConnectionMethodWrapper->IncreaseReferenceCounter();
        }

    OstTraceFunctionExit0( RCMCONNECTIONMETHOD_RCMCONNECTIONMETHOD_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethod::Close()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethod::Close()
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHOD_CLOSE_ENTRY );

    if ( iCmConnectionMethodWrapper )
        {
        if ( iCmConnectionMethodWrapper->DecreaseReferenceCounter() <= 0 )
            {
            delete iCmConnectionMethodWrapper;
            }
        iCmConnectionMethodWrapper = NULL;
        }

    OstTraceFunctionExit0( RCMCONNECTIONMETHOD_CLOSE_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethod::~RCmConnectionMethod()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethod::~RCmConnectionMethod()
    {
    OstTraceFunctionEntry0( DUP1_RCMCONNECTIONMETHOD_RCMCONNECTIONMETHOD_ENTRY );
    Close();
    OstTraceFunctionExit0( DUP1_RCMCONNECTIONMETHOD_RCMCONNECTIONMETHOD_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethod::CreateCopyL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethod RCmConnectionMethod::CreateCopyL()
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHOD_CREATECOPYL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt connMethodHandle = iCmConnectionMethodWrapper->GetHandle();
    CCmConnectionMethodWrapper* cmWrapper = CCmConnectionMethodWrapper::NewLC();

    TInt err = cmWrapper->CreateConnMethodCopy(
            iCmConnectionMethodWrapper->GetMainSession(),
            connMethodHandle );
    User::LeaveIfError( err );
    CleanupStack::Pop( cmWrapper );

    RCmConnectionMethod cm;
    cm.iCmConnectionMethodWrapper = cmWrapper;

    OstTraceFunctionExit0( RCMCONNECTIONMETHOD_CREATECOPYL_EXIT );
    return cm;
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethod::GetIntAttributeL()
//-----------------------------------------------------------------------------
//
EXPORT_C TUint32 RCmConnectionMethod::GetIntAttributeL(
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHOD_GETINTATTRIBUTEL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TUint32 value( 0 );
    TInt err = iCmConnectionMethodWrapper->GetIntAttribute( aAttribute, value );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMCONNECTIONMETHOD_GETINTATTRIBUTEL_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethod::GetBoolAttributeL()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool RCmConnectionMethod::GetBoolAttributeL(
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHOD_GETBOOLATTRIBUTEL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TBool value( 0 );
    TInt err = iCmConnectionMethodWrapper->GetBoolAttribute( aAttribute, value );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMCONNECTIONMETHOD_GETBOOLATTRIBUTEL_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethod::GetStringAttributeL()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC* RCmConnectionMethod::GetStringAttributeL(
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHOD_GETSTRINGATTRIBUTEL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    HBufC* buffer = HBufC::NewLC( KCmmStringLengthMax );
    TInt err = iCmConnectionMethodWrapper->GetStringAttribute( aAttribute, buffer );
    User::LeaveIfError( err );

    if ( buffer->Length() > 0 )
        {
        CleanupStack::Pop( buffer );
        }
    else
        {
        CleanupStack::PopAndDestroy( buffer );
        buffer = NULL;
        buffer = KNullDesC().AllocL();
        }

    OstTraceFunctionExit0( RCMCONNECTIONMETHOD_GETSTRINGATTRIBUTEL_EXIT );
    return buffer;
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethod::GetString8AttributeL()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC8* RCmConnectionMethod::GetString8AttributeL(
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHOD_GETSTRING8ATTRIBUTEL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    HBufC8* buffer8 = HBufC8::NewLC( KCmmStringLengthMax );
    TInt err = iCmConnectionMethodWrapper->GetString8Attribute( aAttribute, buffer8 );
    User::LeaveIfError( err );

    if ( buffer8->Length() > 0 )
        {
        CleanupStack::Pop( buffer8 );
        }
    else
        {
        CleanupStack::PopAndDestroy( buffer8 );
        buffer8 = NULL;
        buffer8 = KNullDesC8().AllocL();
        }

    OstTraceFunctionExit0( RCMCONNECTIONMETHOD_GETSTRING8ATTRIBUTEL_EXIT );
    return buffer8;
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethod::SetIntAttributeL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethod::SetIntAttributeL(
        TUint32 aAttribute,
        TUint32 aValue )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHOD_SETINTATTRIBUTEL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmConnectionMethodWrapper->SetIntAttribute( aAttribute, aValue );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMCONNECTIONMETHOD_SETINTATTRIBUTEL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethod::SetBoolAttributeL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethod::SetBoolAttributeL(
        TUint32 aAttribute,
        TBool aValue )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHOD_SETBOOLATTRIBUTEL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmConnectionMethodWrapper->SetBoolAttribute( aAttribute, aValue );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMCONNECTIONMETHOD_SETBOOLATTRIBUTEL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethod::SetStringAttributeL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethod::SetStringAttributeL(
        TUint32 aAttribute,
        const TDesC16& aValue )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHOD_SETSTRINGATTRIBUTEL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmConnectionMethodWrapper->SetStringAttribute( aAttribute, aValue );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMCONNECTIONMETHOD_SETSTRINGATTRIBUTEL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethod::SetString8AttributeL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethod::SetString8AttributeL(
        TUint32 aAttribute,
        const TDesC8& aValue )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHOD_SETSTRING8ATTRIBUTEL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmConnectionMethodWrapper->SetString8Attribute( aAttribute, aValue );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMCONNECTIONMETHOD_SETSTRING8ATTRIBUTEL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethod::UpdateL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethod::UpdateL()
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHOD_UPDATEL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmConnectionMethodWrapper->Update();
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMCONNECTIONMETHOD_UPDATEL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethod::DeleteL()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool RCmConnectionMethod::DeleteL()
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHOD_DELETEL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TBool result( ETrue );
    TInt err = iCmConnectionMethodWrapper->Delete( result );
    User::LeaveIfError( err );

    // Instead of calling Close(), we need to force this handle closed even if
    // copies have been made. So we delete the destination wrapper directly.
    delete iCmConnectionMethodWrapper;
    iCmConnectionMethodWrapper = NULL;

    OstTraceFunctionExit0( RCMCONNECTIONMETHOD_DELETEL_EXIT );
    return result;
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethod::DestinationL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmDestination RCmConnectionMethod::DestinationL() const
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHOD_DESTINATIONL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    // If client already has a handle open to this destination, give a
    // reference to it.
    CCmDestinationWrapper* destWrap = CCmDestinationWrapper::NewLC();
    TInt connMethodHandle = iCmConnectionMethodWrapper->GetHandle();
    TInt existingHandle( 0 );
    destWrap->GetConnMethodAsEmbeddedDestL(
            iCmConnectionMethodWrapper->GetMainSession(),
            connMethodHandle,
            existingHandle );
    if ( existingHandle )
        {
        CleanupStack::PopAndDestroy( destWrap );
        destWrap = iCmConnectionMethodWrapper->GetMainSession()->GetDestinationWrapperL(
                existingHandle );
        destWrap->IncreaseReferenceCounter();
        }
    else
        {
        CleanupStack::Pop( destWrap );
        }

    RCmDestination dest;
    dest.iCmDestinationWrapper = destWrap;

    OstTraceFunctionExit0( RCMCONNECTIONMETHOD_DESTINATIONL_EXIT );
    return dest;
    }

//--------------------------------------------------------------------------
//  RCmConnectionMethod::RCmConnectionMethod()
//--------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethod::RCmConnectionMethod() : iCmConnectionMethodWrapper( NULL )
    {
    OstTraceFunctionEntry0( DUP2_RCMCONNECTIONMETHOD_RCMCONNECTIONMETHOD_ENTRY );
    OstTraceFunctionExit0( DUP2_RCMCONNECTIONMETHOD_RCMCONNECTIONMETHOD_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethod::operator=()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethod& RCmConnectionMethod::operator=(
        const RCmConnectionMethod& aConnMethod )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHOD_OPERATOR_ENTRY );

    if ( this != &aConnMethod )
        {
        Close();
        this->iCmConnectionMethodWrapper = aConnMethod.iCmConnectionMethodWrapper;
        if ( iCmConnectionMethodWrapper )
            {
            iCmConnectionMethodWrapper->IncreaseReferenceCounter();
            }
        }

    OstTraceFunctionExit0( RCMCONNECTIONMETHOD_OPERATOR_EXIT );
    return *this;
    }

//--------------------------------------------------------------------------
//  RCmConnectionMethod::operator==()
//--------------------------------------------------------------------------
//
EXPORT_C TBool RCmConnectionMethod::operator==( RCmConnectionMethod& aConnMethod ) const
    {
    OstTraceFunctionEntry0( DUP1_RCMCONNECTIONMETHOD_OPERATOR_ENTRY );

    TBool result( EFalse );
    if ( !aConnMethod.iCmConnectionMethodWrapper )
        {
        if ( !iCmConnectionMethodWrapper )
            {
            result = ETrue; // Both empty.
            }
        else
            {
            result = EFalse;
            }
        }
    else if ( !iCmConnectionMethodWrapper )
        {
        result = EFalse;
        }
    else
        {
        // Both connection method objects have wrappers.

        if ( !iCmConnectionMethodWrapper->SessionConnected() ||
                !aConnMethod.iCmConnectionMethodWrapper->SessionConnected() )
            {
            result = EFalse;
            }
        else
            {
            // Both connection method objects are connected to server.

            TInt handle = aConnMethod.iCmConnectionMethodWrapper->GetHandle();
            TInt err = iCmConnectionMethodWrapper->IsEqual( handle, result );

            if ( err )
                {
                result = EFalse;
                }
            }
        }

    OstTraceFunctionExit0( DUP1_RCMCONNECTIONMETHOD_OPERATOR_EXIT );
    return result;
    }

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
//
EXPORT_C TBool RCmConnectionMethod::operator!=( RCmConnectionMethod& aConnMethod ) const
    {
    OstTraceFunctionEntry0( DUP2_RCMCONNECTIONMETHOD_OPERATOR_ENTRY );

    TBool result = !( operator==( aConnMethod ) );

    OstTraceFunctionExit0( DUP2_RCMCONNECTIONMETHOD_OPERATOR_EXIT );
    return result;
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethod::GetIconL()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC* RCmConnectionMethod::GetIconL() const
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHOD_GETICONL_ENTRY );

    User::Leave( KErrNotSupported );

    OstTraceFunctionExit0( RCMCONNECTIONMETHOD_GETICONL_EXIT );
    return NULL;
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethod::SetIconL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethod::SetIconL( const TDesC& /*aIcon*/ )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHOD_SETICONL_ENTRY );

    User::Leave( KErrNotSupported );

    OstTraceFunctionExit0( RCMCONNECTIONMETHOD_SETICONL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethod::RefreshL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethod::RefreshL()
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHOD_REFRESHL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmConnectionMethodWrapper->Refresh();
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMCONNECTIONMETHOD_REFRESHL_EXIT );
    }

// End of file
