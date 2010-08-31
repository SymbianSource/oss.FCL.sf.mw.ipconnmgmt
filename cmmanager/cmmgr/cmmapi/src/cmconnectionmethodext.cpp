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
* Implementation of RCmConnectionMethodExt.
*
*/


#include <cmconnectionmethodext.h>
#include <cmdestinationext.h>

#include "cmmserverdefs.h"
#include "cmmanagerapi.h"
#include "cmdestinationwrapper.h"
#include "cmconnectionmethodwrapper.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmconnectionmethodextTraces.h"
#endif


//-----------------------------------------------------------------------------
//  RCmConnectionMethodExt::RCmConnectionMethodExt()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethodExt::RCmConnectionMethodExt(
        const RCmConnectionMethodExt& aConnMethod )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODEXT_RCMCONNECTIONMETHODEXT_ENTRY );

    this->iCmConnectionMethodWrapper = aConnMethod.iCmConnectionMethodWrapper;
    if ( iCmConnectionMethodWrapper )
        {
        iCmConnectionMethodWrapper->IncreaseReferenceCounter();
        }

    OstTraceFunctionExit0( RCMCONNECTIONMETHODEXT_RCMCONNECTIONMETHODEXT_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethodExt::Close()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethodExt::Close()
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODEXT_CLOSE_ENTRY );

    if ( iCmConnectionMethodWrapper )
        {
        if ( iCmConnectionMethodWrapper->DecreaseReferenceCounter() <= 0 )
            {
            delete iCmConnectionMethodWrapper;
            }
        iCmConnectionMethodWrapper = NULL;
        }

    OstTraceFunctionExit0( RCMCONNECTIONMETHODEXT_CLOSE_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethodExt::~RCmConnectionMethodExt()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethodExt::~RCmConnectionMethodExt()
    {
    OstTraceFunctionEntry0( DUP2_RCMCONNECTIONMETHODEXT_RCMCONNECTIONMETHODEXT_ENTRY );
    Close();
    OstTraceFunctionExit0( DUP2_RCMCONNECTIONMETHODEXT_RCMCONNECTIONMETHODEXT_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethodExt::CreateCopyL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethodExt RCmConnectionMethodExt::CreateCopyL()
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODEXT_CREATECOPYL_ENTRY );

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

    RCmConnectionMethodExt cm;
    cm.iCmConnectionMethodWrapper = cmWrapper;

    OstTraceFunctionExit0( RCMCONNECTIONMETHODEXT_CREATECOPYL_EXIT );
    return cm;
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethodExt::GetIntAttributeL()
//-----------------------------------------------------------------------------
//
EXPORT_C TUint32 RCmConnectionMethodExt::GetIntAttributeL(
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODEXT_GETINTATTRIBUTEL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TUint32 value( 0 );
    TInt err = iCmConnectionMethodWrapper->GetIntAttribute( aAttribute, value );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMCONNECTIONMETHODEXT_GETINTATTRIBUTEL_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethodExt::GetBoolAttributeL()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool RCmConnectionMethodExt::GetBoolAttributeL(
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODEXT_GETBOOLATTRIBUTEL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TBool value( 0 );
    TInt err = iCmConnectionMethodWrapper->GetBoolAttribute( aAttribute, value );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMCONNECTIONMETHODEXT_GETBOOLATTRIBUTEL_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethodExt::GetStringAttributeL()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC* RCmConnectionMethodExt::GetStringAttributeL(
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODEXT_GETSTRINGATTRIBUTEL_ENTRY );

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

    OstTraceFunctionExit0( RCMCONNECTIONMETHODEXT_GETSTRINGATTRIBUTEL_EXIT );
    return buffer;
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethodExt::GetString8AttributeL()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC8* RCmConnectionMethodExt::GetString8AttributeL(
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODEXT_GETSTRING8ATTRIBUTEL_ENTRY );

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

    OstTraceFunctionExit0( RCMCONNECTIONMETHODEXT_GETSTRING8ATTRIBUTEL_EXIT );
    return buffer8;
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethodExt::SetIntAttributeL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethodExt::SetIntAttributeL(
        TUint32 aAttribute,
        TUint32 aValue )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODEXT_SETINTATTRIBUTEL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmConnectionMethodWrapper->SetIntAttribute( aAttribute, aValue );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMCONNECTIONMETHODEXT_SETINTATTRIBUTEL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethodExt::SetBoolAttributeL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethodExt::SetBoolAttributeL(
        TUint32 aAttribute,
        TBool aValue )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODEXT_SETBOOLATTRIBUTEL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmConnectionMethodWrapper->SetBoolAttribute( aAttribute, aValue );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMCONNECTIONMETHODEXT_SETBOOLATTRIBUTEL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethodExt::SetStringAttributeL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethodExt::SetStringAttributeL(
        TUint32 aAttribute,
        const TDesC16& aValue )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODEXT_SETSTRINGATTRIBUTEL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmConnectionMethodWrapper->SetStringAttribute( aAttribute, aValue );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMCONNECTIONMETHODEXT_SETSTRINGATTRIBUTEL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethodExt::SetString8AttributeL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethodExt::SetString8AttributeL(
        TUint32 aAttribute,
        const TDesC8& aValue )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODEXT_SETSTRING8ATTRIBUTEL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmConnectionMethodWrapper->SetString8Attribute( aAttribute, aValue );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMCONNECTIONMETHODEXT_SETSTRING8ATTRIBUTEL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethodExt::UpdateL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethodExt::UpdateL()
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODEXT_UPDATEL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmConnectionMethodWrapper->Update();
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMCONNECTIONMETHODEXT_UPDATEL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethodExt::DeleteL()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool RCmConnectionMethodExt::DeleteL()
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODEXT_DELETEL_ENTRY );

    if ( !iCmConnectionMethodWrapper || !iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrBadHandle );
        }

    TBool result( ETrue );
    TInt err = iCmConnectionMethodWrapper->Delete( result );
    User::LeaveIfError( err );

    delete iCmConnectionMethodWrapper;
    iCmConnectionMethodWrapper = NULL;

    OstTraceFunctionExit0( RCMCONNECTIONMETHODEXT_DELETEL_EXIT );
    return result;
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethodExt::DestinationL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmDestinationExt RCmConnectionMethodExt::DestinationL() const
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODEXT_DESTINATIONL_ENTRY );

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

    RCmDestinationExt dest;
    dest.iCmDestinationWrapper = destWrap;

    OstTraceFunctionExit0( RCMCONNECTIONMETHODEXT_DESTINATIONL_EXIT );
    return dest;
    }

//--------------------------------------------------------------------------
//  RCmConnectionMethodExt::RCmConnectionMethodExt()
//--------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethodExt::RCmConnectionMethodExt() : iCmConnectionMethodWrapper( NULL )
    {
    OstTraceFunctionEntry0( DUP1_RCMCONNECTIONMETHODEXT_RCMCONNECTIONMETHODEXT_ENTRY );
    OstTraceFunctionExit0( DUP1_RCMCONNECTIONMETHODEXT_RCMCONNECTIONMETHODEXT_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmConnectionMethodExt::operator=()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethodExt& RCmConnectionMethodExt::operator=(
        const RCmConnectionMethodExt& aConnMethod )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODEXT_OPERATOR_ENTRY );

    if ( this != &aConnMethod )
        {
        Close();
        this->iCmConnectionMethodWrapper = aConnMethod.iCmConnectionMethodWrapper;
        if ( iCmConnectionMethodWrapper )
            {
            iCmConnectionMethodWrapper->IncreaseReferenceCounter();
            }
        }

    OstTraceFunctionExit0( RCMCONNECTIONMETHODEXT_OPERATOR_EXIT );
    return *this;
    }

//--------------------------------------------------------------------------
//  RCmConnectionMethodExt::operator==()
//--------------------------------------------------------------------------
//
EXPORT_C TBool RCmConnectionMethodExt::operator==( RCmConnectionMethodExt& aConnMethod ) const
    {
    OstTraceFunctionEntry0( DUP1_RCMCONNECTIONMETHODEXT_OPERATOR_ENTRY );

    TBool result( EFalse );
    if ( !aConnMethod.iCmConnectionMethodWrapper )
        {
        if ( !iCmConnectionMethodWrapper )
            {
            result = ETrue; // Both empty
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

    OstTraceFunctionExit0( DUP1_RCMCONNECTIONMETHODEXT_OPERATOR_EXIT );
    return result;
    }

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
//
EXPORT_C TBool RCmConnectionMethodExt::operator!=( RCmConnectionMethodExt& aConnMethod ) const
    {
    OstTraceFunctionEntry0( DUP2_RCMCONNECTIONMETHODEXT_OPERATOR_ENTRY );

    TBool result = !( operator==( aConnMethod ) );

    OstTraceFunctionExit0( DUP2_RCMCONNECTIONMETHODEXT_OPERATOR_EXIT );
    return result;
    }

// End of file
