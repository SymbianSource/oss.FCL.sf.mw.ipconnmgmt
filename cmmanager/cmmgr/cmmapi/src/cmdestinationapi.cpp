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
#include "cmdestinationapi.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmdestinationapiTraces.h"
#endif


RCmDestinationApi::RCmDestinationApi()
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_RCMDESTINATIONAPI_ENTRY );
    iSessionConnected = EFalse;
    OstTraceFunctionExit0( RCMDESTINATIONAPI_RCMDESTINATIONAPI_EXIT );
    }

TBool RCmDestinationApi::SessionConnected()
    {
    return iSessionConnected;
    }

void RCmDestinationApi::ConnectAndGetDestWithIdL(
        const RSessionBase& aSession,
        const TUint32 aId,
        TInt& aExistingHandle )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_CONNECTANDGETDESTWITHIDL_ENTRY );

    if ( iSessionConnected )
        {
        User::Leave( KErrAlreadyExists );
        }

    aExistingHandle = 0;
    TPckg<TInt> pckg( aExistingHandle );
    TIpcArgs args( aId, &pckg );
    TInt result = CreateSubSession( aSession, EDestGetDestination, args );
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

    OstTraceFunctionExit0( RCMDESTINATIONAPI_CONNECTANDGETDESTWITHIDL_EXIT );
    }

void RCmDestinationApi::ConnectAndCreateDestWithNameL(
        const RSessionBase& aSession,
        const TDesC& aName )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_CONNECTANDCREATEDESTWITHNAMEL_ENTRY );

    if ( iSessionConnected )
        {
        User::Leave( KErrAlreadyExists );
        }
    TIpcArgs args( &aName );
    User::LeaveIfError( CreateSubSession( aSession, EDestCreateDestinationWithName, args ) );
    iSessionConnected = ETrue;

    OstTraceFunctionExit0( RCMDESTINATIONAPI_CONNECTANDCREATEDESTWITHNAMEL_EXIT );
    }

void RCmDestinationApi::ConnectAndCreateDestWithNameAndIdL(
        const RSessionBase& aSession,
        const TDesC& aName,
        const TUint32 aId )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_CONNECTANDCREATEDESTWITHNAMEANDIDL_ENTRY );

    if ( iSessionConnected )
        {
        User::Leave( KErrAlreadyExists );
        }
    TIpcArgs args( &aName, aId );
    User::LeaveIfError( CreateSubSession( aSession, EDestCreateDestinationWithNameAndId, args ) );
    iSessionConnected = ETrue;

    OstTraceFunctionExit0( RCMDESTINATIONAPI_CONNECTANDCREATEDESTWITHNAMEANDIDL_EXIT );
    }

void RCmDestinationApi::ConnectAndGetConnMethodAsEmbeddedDestL(
        const RSessionBase& aSession,
        const TInt aConnMethodHandle,
        TInt& aExistingHandle )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_CONNECTANDGETCONNMETHODASEMBEDDEDDESTL_ENTRY );

    if ( iSessionConnected )
        {
        User::Leave( KErrAlreadyExists );
        }

    aExistingHandle = 0;
    TPckg<TInt> pckg( aExistingHandle );
    TIpcArgs args( aConnMethodHandle, &pckg );
    TInt result = CreateSubSession( aSession, EDestGetEmbeddedDestination, args );
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

    OstTraceFunctionExit0( RCMDESTINATIONAPI_CONNECTANDGETCONNMETHODASEMBEDDEDDESTL_EXIT );
    }

void RCmDestinationApi::Close()
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_CLOSE_ENTRY );

    if ( iSessionConnected )
        {
        CloseSubSession( EDestCloseDestination );
        iSessionConnected = EFalse;
        }

    OstTraceFunctionExit0( RCMDESTINATIONAPI_CLOSE_EXIT );
    }

TInt RCmDestinationApi::ConnectionMethodCount( TInt& aCount )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_CONNECTIONMETHODCOUNT_ENTRY );

    TPckg<TInt> pckg( aCount );
    TIpcArgs args( &pckg );
    TInt err = SendReceive( EDestGetConnMethodCount, args );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_CONNECTIONMETHODCOUNT_EXIT );
    return err;
    }

TInt RCmDestinationApi::Priority( const TInt aHandle, TUint& aPriority )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_PRIORITY_ENTRY );

    TPckg<TUint> pckg( aPriority );
    TIpcArgs args( aHandle, &pckg );
    TInt err = SendReceive( EDestGetConnMethodPriority, args );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_PRIORITY_EXIT );
    return err;
    }

TInt RCmDestinationApi::GetName( HBufC* aBuffer )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_GETNAME_ENTRY );

    TPtr descriptor( aBuffer->Des() );
    TIpcArgs args( &descriptor );
    TInt err = SendReceive( EDestGetName, args );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_GETNAME_EXIT );
    return err;
    }

TInt RCmDestinationApi::GetId( TUint32& aId )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_GETID_ENTRY );

    TPckg<TUint32> pckg( aId );
    TIpcArgs args( &pckg );
    TInt err = SendReceive( EDestGetId, args );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_GETID_EXIT );
    return err;
    }

TInt RCmDestinationApi::GetElementId( TUint32& aElementId )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_GETELEMENTID_ENTRY );

    TPckg<TUint32> pckg( aElementId );
    TIpcArgs args( &pckg );
    TInt err = SendReceive( EDestGetElementId, args );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_GETELEMENTID_EXIT );
    return err;
    }

TInt RCmDestinationApi::GetMetadata(
        const TUint32 aMetadataField,
        TUint32& aValue )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_GETMETADATA_ENTRY );

    TPckg<TUint32> pckg( aValue );
    TIpcArgs args( aMetadataField, &pckg );
    TInt err = SendReceive( EDestMetadata, args );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_GETMETADATA_EXIT );
    return err;
    }

TInt RCmDestinationApi::GetProtectionLevel( TInt& aProtectionLevel )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_GETPROTECTIONLEVEL_ENTRY );

    TPckg<TInt> pckg( aProtectionLevel );
    TIpcArgs args( &pckg );
    TInt err = SendReceive( EDestGetProtectionLevel, args );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_GETPROTECTIONLEVEL_EXIT );
    return err;
    }

TInt RCmDestinationApi::AddConnectionMethod(
        const TInt aConnMethodHandle,
        TInt& aIndex )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_ADDCONNECTIONMETHOD_ENTRY );

    TPckg<TInt> pckg( aIndex );
    TIpcArgs args( aConnMethodHandle, &pckg );
    TInt err = SendReceive( EDestAddConnMethod, args );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_ADDCONNECTIONMETHOD_EXIT );
    return err;
    }

TInt RCmDestinationApi::AddEmbeddedDestination(
        const TInt aDestHandle,
        TInt& aIndex )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_ADDEMBEDDEDDESTINATION_ENTRY );

    TPckg<TInt> pckg( aIndex );
    TIpcArgs args( aDestHandle, &pckg );
    TInt err = SendReceive( EDestAddEmbeddedDestination, args );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_ADDEMBEDDEDDESTINATION_EXIT );
    return err;
    }

TInt RCmDestinationApi::DeleteConnectionMethod(
        const TInt aConnMethodHandle )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_DELETECONNECTIONMETHOD_ENTRY );

    TIpcArgs args( aConnMethodHandle );
    TInt err = SendReceive( EDestDeleteConnMethod, args );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_DELETECONNECTIONMETHOD_EXIT );
    return err;
    }

TInt RCmDestinationApi::RemoveConnectionMethod(
        const TInt aConnMethodHandle )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_REMOVECONNECTIONMETHOD_ENTRY );

    TIpcArgs args( aConnMethodHandle );
    TInt err = SendReceive( EDestRemoveConnMethod, args );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_REMOVECONNECTIONMETHOD_EXIT );
    return err;
    }

TInt RCmDestinationApi::ModifyPriority(
        const TInt aConnMethodHandle,
        const TUint aIndex )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_MODIFYPRIORITY_ENTRY );

    TIpcArgs args( aConnMethodHandle, aIndex );
    TInt err = SendReceive( EDestModifyPriority, args );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_MODIFYPRIORITY_EXIT );
    return err;
    }

TInt RCmDestinationApi::SetName( const TDesC& aValue )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_SETNAME_ENTRY );

    TIpcArgs args( &aValue );
    TInt err = SendReceive( EDestSetName, args );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_SETNAME_EXIT );
    return err;
    }

TInt RCmDestinationApi::SetMetadata(
        const TUint32 aMetaField,
        const TUint32 aValue )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_SETMETADATA_ENTRY );

    TIpcArgs args( aMetaField, aValue );
    TInt err = SendReceive( EDestSetMetadata, args );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_SETMETADATA_EXIT );
    return err;
    }

TInt RCmDestinationApi::SetProtection( const TInt aValue )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_SETPROTECTION_ENTRY );

    TIpcArgs args( aValue );
    TInt err = SendReceive( EDestSetProtection, args );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_SETPROTECTION_EXIT );
    return err;
    }

TInt RCmDestinationApi::SetHidden( const TBool aHidden )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_SETHIDDEN_ENTRY );

    TIpcArgs args( aHidden );
    TInt err = SendReceive( EDestSetHidden, args );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_SETHIDDEN_EXIT );
    return err;
    }

TInt RCmDestinationApi::Update()
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_UPDATE_ENTRY );

    TInt err = SendReceive( EDestUpdate );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_UPDATE_EXIT );
    return err;
    }

TInt RCmDestinationApi::Refresh()
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_REFRESH_ENTRY );

    TInt err = SendReceive( EDestRefresh );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_REFRESH_EXIT );
    return err;
    }

TInt RCmDestinationApi::Delete()
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_DELETE_ENTRY );

    TInt err = SendReceive( EDestDelete );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_DELETE_EXIT );
    return err;
    }

TInt RCmDestinationApi::IsConnected( TBool& aResult )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_ISCONNECTED_ENTRY );

    TPckg<TBool> pckg( aResult );
    TIpcArgs args( &pckg );
    TInt err = SendReceive( EDestIsConnected, args );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_ISCONNECTED_EXIT );
    return err;
    }

TInt RCmDestinationApi::IsHidden( TBool& aResult )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_ISHIDDEN_ENTRY );

    TPckg<TBool> pckg( aResult );
    TIpcArgs args( &pckg );
    TInt err = SendReceive( EDestIsHidden, args );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_ISHIDDEN_EXIT );
    return err;
    }

TInt RCmDestinationApi::IsEqual( const TInt aHandle, TBool& aResult )
    {
    OstTraceFunctionEntry0( RCMDESTINATIONAPI_ISEQUAL_ENTRY );

    TPckg<TBool> pckg( aResult );
    TIpcArgs args( aHandle, &pckg );
    TInt err = SendReceive( EDestIsEqual, args );

    OstTraceFunctionExit0( RCMDESTINATIONAPI_ISEQUAL_EXIT );
    return err;
    }

// End of file
