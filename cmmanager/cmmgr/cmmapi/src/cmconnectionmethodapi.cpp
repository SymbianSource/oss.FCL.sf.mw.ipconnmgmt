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
* Client side IPC handling for Connection Method Manager server.
*
*/


#include "cmmserverdefs.h"
#include "cmconnectionmethodapi.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmconnectionmethodapiTraces.h"
#endif


RCmConnectionMethodApi::RCmConnectionMethodApi()
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_RCMCONNECTIONMETHODAPI_ENTRY );
    iSessionConnected = EFalse;
    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_RCMCONNECTIONMETHODAPI_EXIT );
    }

TBool RCmConnectionMethodApi::SessionConnected() const
    {
    return iSessionConnected;
    }

void RCmConnectionMethodApi::ConnectAndGetConnMethodWithIdL(
        const RSessionBase& aSession,
        const TUint32 aIapId,
        TInt& aExistingHandle )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_CONNECTANDGETCONNMETHODWITHIDL_ENTRY );

    if ( iSessionConnected )
        {
        User::Leave( KErrAlreadyExists );
        }

    aExistingHandle = 0;
    TPckg<TInt> pckg( aExistingHandle );
    TIpcArgs args( aIapId, &pckg );
    TInt result = CreateSubSession( aSession, ECMGetConnMethodWithId, args );
    if ( !aExistingHandle )
        {
        User::LeaveIfError( result );
        iSessionConnected = ETrue;
        }
    else
        {
        if ( result != KErrAlreadyExists )
            {
            User::Leave( result );
            }
        }

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_CONNECTANDGETCONNMETHODWITHIDL_EXIT );
    }

void RCmConnectionMethodApi::ConnectAndGetConnMethodFromDestWithIndexL(
        const RSessionBase& aSession,
        const TInt aDestHandle,
        const TInt aIndex,
        TInt& aExistingHandle )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_CONNECTANDGETCONNMETHODFROMDESTWITHINDEXL_ENTRY );

    if ( iSessionConnected )
        {
        User::Leave( KErrAlreadyExists );
        }

    aExistingHandle = 0;
    TPckg<TInt> pckg( aExistingHandle );
    TIpcArgs args( aDestHandle, aIndex, &pckg );
    TInt result = CreateSubSession( aSession, ECMGetConnMethodFromDestWithIndex, args );
    if ( !aExistingHandle )
        {
        User::LeaveIfError( result );
        iSessionConnected = ETrue;
        }
    else
        {
        if ( result != KErrAlreadyExists )
            {
            User::Leave( result );
            }
        }

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_CONNECTANDGETCONNMETHODFROMDESTWITHINDEXL_EXIT );
    }

void RCmConnectionMethodApi::ConnectAndGetConnMethodFromDestWithIdL(
        const RSessionBase& aSession,
        const TInt aDestHandle,
        const TInt aIapId,
        TInt& aExistingHandle )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_CONNECTANDGETCONNMETHODFROMDESTWITHIDL_ENTRY );

    if ( iSessionConnected )
        {
        User::Leave( KErrAlreadyExists );
        }

    aExistingHandle = 0;
    TPckg<TInt> pckg( aExistingHandle );
    TIpcArgs args( aDestHandle, aIapId, &pckg );
    TInt result = CreateSubSession( aSession, ECMGetConnMethodFromDestWithId, args );
    if ( !aExistingHandle )
        {
        User::LeaveIfError( result );
        iSessionConnected = ETrue;
        }
    else
        {
        if ( result != KErrAlreadyExists )
            {
            User::Leave( result );
            }
        }

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_CONNECTANDGETCONNMETHODFROMDESTWITHIDL_EXIT );
    }

void RCmConnectionMethodApi::ConnectAndCreateConnMethodL(
        const RSessionBase& aSession,
        const TUint32 aBearerType )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_CONNECTANDCREATECONNMETHODL_ENTRY );

    if ( iSessionConnected )
        {
        User::Leave( KErrAlreadyExists );
        }

    TIpcArgs args( aBearerType );
    User::LeaveIfError( CreateSubSession( aSession, ECMCreateConnMethod, args ) );
    iSessionConnected = ETrue;

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_CONNECTANDCREATECONNMETHODL_EXIT );
    }

void RCmConnectionMethodApi::ConnectAndCreateConnMethodWithIdL(
        const RSessionBase& aSession,
        const TUint32 aBearerType,
        const TUint32 aIapId )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_CONNECTANDCREATECONNMETHODWITHIDL_ENTRY );

    if ( iSessionConnected )
        {
        User::Leave( KErrAlreadyExists );
        }

    TIpcArgs args( aBearerType, aIapId );
    User::LeaveIfError( CreateSubSession( aSession, ECMCreateConnMethodWithId, args ) );
    iSessionConnected = ETrue;

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_CONNECTANDCREATECONNMETHODWITHIDL_EXIT );
    }

TInt RCmConnectionMethodApi::ConnectAndCreateConnMethodCopy(
        const RSessionBase& aSession,
        const TInt aConnMethodHandle )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_CONNECTANDCREATECONNMETHODCOPY_ENTRY );

    TInt err( KErrNone );
    if ( iSessionConnected )
        {
        err = KErrAlreadyExists;
        }
    else
        {
        TIpcArgs args( aConnMethodHandle );
        err = CreateSubSession( aSession, ECMCreateCopyOfExisting, args );
        if ( !err )
            {
            iSessionConnected = ETrue;
            }
        }

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_CONNECTANDCREATECONNMETHODCOPY_EXIT );
    return err;
    }

void RCmConnectionMethodApi::ConnectAndCreateConnMethodToDestL(
        const RSessionBase& aSession,
        const TInt aDestHandle,
        const TUint32 aBearerType )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_CONNECTANDCREATECONNMETHODFROMDESTL_ENTRY );

    if ( iSessionConnected )
        {
        User::Leave( KErrAlreadyExists );
        }

    TIpcArgs args( aDestHandle, aBearerType );
    User::LeaveIfError( CreateSubSession( aSession, ECMCreateConnMethodToDest, args ) );
    iSessionConnected = ETrue;

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_CONNECTANDCREATECONNMETHODFROMDESTL_EXIT );
    }

void RCmConnectionMethodApi::ConnectAndCreateConnMethodToDestWithIdL(
        const RSessionBase& aSession,
        const TInt aDestHandle,
        const TUint32 aBearerType,
        const TUint32 aIapId )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_CONNECTANDCREATECONNMETHODFROMDESTWITHIDL_ENTRY );

    if ( iSessionConnected )
        {
        User::Leave( KErrAlreadyExists );
        }

    TIpcArgs args( aDestHandle, aBearerType, aIapId );
    User::LeaveIfError( CreateSubSession( aSession, ECMCreateConnMethodToDestWithId, args ) );
    iSessionConnected = ETrue;

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_CONNECTANDCREATECONNMETHODFROMDESTWITHIDL_EXIT );
    }

void RCmConnectionMethodApi::Close()
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_CLOSE_ENTRY );

    if ( iSessionConnected )
        {
        CloseSubSession( ECMCloseConnMethod );
        iSessionConnected = EFalse;
        }

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_CLOSE_EXIT );
    }

TInt RCmConnectionMethodApi::GetIntAttribute(
        const TUint32 aAttribute,
        TUint32& aValue )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_GETINTATTRIBUTE_ENTRY );

    TPckg<TUint32> pckg( aValue );
    TIpcArgs args( aAttribute, &pckg );
    TInt err = SendReceive( ECMGetIntAttribute, args );

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_GETINTATTRIBUTE_EXIT );
    return err;
    }

TInt RCmConnectionMethodApi::GetBoolAttribute(
        const TUint32 aAttribute,
        TBool& aValue )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_GETBOOLATTRIBUTE_ENTRY );

    TPckg<TBool> pckg( aValue );
    TIpcArgs args( aAttribute, &pckg );
    TInt err = SendReceive( ECMGetBoolAttribute, args );

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_GETBOOLATTRIBUTE_EXIT );
    return err;
    }

TInt RCmConnectionMethodApi::GetStringAttribute(
        const TUint32 aAttribute,
        HBufC* aBuffer )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_GETSTRINGATTRIBUTE_ENTRY );

    TPtr descriptor( aBuffer->Des() );
    TIpcArgs args( aAttribute, &descriptor );
    TInt err = SendReceive( ECMGetStringAttribute, args );

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_GETSTRINGATTRIBUTE_EXIT );
    return err;
    }

TInt RCmConnectionMethodApi::GetString8Attribute(
        const TUint32 aAttribute,
        HBufC8* aBuffer8 )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_GETSTRING8ATTRIBUTE_ENTRY );

    TPtr8 descriptor( aBuffer8->Des() );
    TIpcArgs args( aAttribute, &descriptor );
    TInt err = SendReceive( ECMGetString8Attribute, args );

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_GETSTRING8ATTRIBUTE_EXIT );
    return err;
    }

TInt RCmConnectionMethodApi::SetIntAttribute(
        const TUint32 aAttribute,
        const TUint32 aValue )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_SETINTATTRIBUTE_ENTRY );

    TIpcArgs args( aAttribute, aValue );
    TInt err = SendReceive( ECMSetIntAttribute, args );

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_SETINTATTRIBUTE_EXIT );
    return err;
    }

TInt RCmConnectionMethodApi::SetBoolAttribute(
        const TUint32 aAttribute,
        const TBool aValue )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_SETBOOLATTRIBUTE_ENTRY );

    TIpcArgs args( aAttribute, aValue );
    TInt err = SendReceive( ECMSetBoolAttribute, args );

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_SETBOOLATTRIBUTE_EXIT );
    return err;
    }

TInt RCmConnectionMethodApi::SetStringAttribute(
        const TUint32 aAttribute,
        const TDesC16& aValue )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_SETSTRINGATTRIBUTE_ENTRY );

    TIpcArgs args( aAttribute, &aValue );
    TInt err = SendReceive( ECMSetStringAttribute, args );

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_SETSTRINGATTRIBUTE_EXIT );
    return err;
    }

TInt RCmConnectionMethodApi::SetString8Attribute(
        const TUint32 aAttribute,
        const TDesC8& aValue )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_SETSTRING8ATTRIBUTE_ENTRY );

    TIpcArgs args( aAttribute, &aValue );
    TInt err = SendReceive( ECMSetString8Attribute, args );

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_SETSTRING8ATTRIBUTE_EXIT );
    return err;
    }

TInt RCmConnectionMethodApi::Update()
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_UPDATE_ENTRY );

    TInt err = SendReceive( ECMUpdate );

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_UPDATE_EXIT );
    return err;
    }

TInt RCmConnectionMethodApi::Refresh()
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_REFRESH_ENTRY );

    TInt err = SendReceive( ECMRefresh );

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_REFRESH_EXIT );
    return err;
    }

TInt RCmConnectionMethodApi::Delete( TBool& aResult )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_DELETE_ENTRY );

    TPckg<TBool> pckg( aResult );
    TIpcArgs args( &pckg );
    TInt err = SendReceive( ECMDelete, args );

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_DELETE_EXIT );
    return err;
    }

TInt RCmConnectionMethodApi::IsEqual( const TInt aHandle, TBool& aResult )
    {
    OstTraceFunctionEntry0( RCMCONNECTIONMETHODAPI_ISEQUAL_ENTRY );

    TPckg<TBool> pckg( aResult );
    TIpcArgs args( aHandle, &pckg );
    TInt err = SendReceive( ECMIsEqual, args );

    OstTraceFunctionExit0( RCMCONNECTIONMETHODAPI_ISEQUAL_EXIT );
    return err;
    }

// End of file
