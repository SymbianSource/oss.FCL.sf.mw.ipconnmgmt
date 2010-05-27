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
* Collects API calls from RCmDestination- and RCmDestinationExt-classes.
*
*/


#include "cmdestinationwrapper.h"
#include "cmmanagerapi.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmdestinationwrapperTraces.h"
#endif


CCmDestinationWrapper* CCmDestinationWrapper::NewL()
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_NEWL_ENTRY );

    CCmDestinationWrapper* self = CCmDestinationWrapper::NewLC();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_NEWL_EXIT );
    return self;
    }

CCmDestinationWrapper* CCmDestinationWrapper::NewLC()
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_NEWLC_ENTRY );

    CCmDestinationWrapper* self = new (ELeave) CCmDestinationWrapper;
    CleanupStack::PushL( self );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_NEWLC_EXIT );
    return self;
    }

CCmDestinationWrapper::~CCmDestinationWrapper()
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_CCMDESTINATIONWRAPPER_ENTRY );

    iCmDestinationApi.Close();

    if ( iCmManagerApi )
        {
        iCmManagerApi->RemoveDestinationWrapper( this );
        iCmManagerApi = NULL;
        }

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_CCMDESTINATIONWRAPPER_EXIT );
    }

CCmDestinationWrapper::CCmDestinationWrapper()
    {
    OstTraceFunctionEntry0( DUP1_CCMDESTINATIONWRAPPER_CCMDESTINATIONWRAPPER_ENTRY );

    iCmManagerApi = NULL;
    iReferenceCounter = 1;

    OstTraceFunctionExit0( DUP1_CCMDESTINATIONWRAPPER_CCMDESTINATIONWRAPPER_EXIT );
    }

void CCmDestinationWrapper::GetDestWithIdL(
        RCmManagerApi* aCmManagerApi,
        const TUint32 aId,
        TInt& aExistingHandle )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_GETDESTWITHIDL_ENTRY );

    if ( iCmManagerApi || iCmDestinationApi.SessionConnected() )
        {
        User::Leave( KErrAlreadyExists );
        }

    iCmManagerApi = aCmManagerApi;
    iCmDestinationApi.ConnectAndGetDestWithIdL( *aCmManagerApi, aId, aExistingHandle );
    if ( !aExistingHandle )
        {
        iCmManagerApi->StoreDestinationWrapper( this );
        }

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_GETDESTWITHIDL_EXIT );
    }

void CCmDestinationWrapper::CreateDestByNameL(
        RCmManagerApi* aCmManagerApi,
        const TDesC& aName )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_CREATEDESTBYNAMEL_ENTRY );

    if ( iCmManagerApi || iCmDestinationApi.SessionConnected() )
        {
        User::Leave( KErrAlreadyExists );
        }

    iCmManagerApi = aCmManagerApi;
    iCmDestinationApi.ConnectAndCreateDestWithNameL( *aCmManagerApi, aName );
    iCmManagerApi->StoreDestinationWrapper( this );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_CREATEDESTBYNAMEL_EXIT );
    }

void CCmDestinationWrapper::CreateDestByNameAndIdL(
        RCmManagerApi* aCmManagerApi,
        const TDesC& aName,
        const TUint32 aId )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_CREATEDESTBYNAMEANDIDL_ENTRY );

    if ( iCmManagerApi || iCmDestinationApi.SessionConnected() )
        {
        User::Leave( KErrAlreadyExists );
        }

    iCmManagerApi = aCmManagerApi;
    iCmDestinationApi.ConnectAndCreateDestWithNameAndIdL( *aCmManagerApi, aName, aId );
    iCmManagerApi->StoreDestinationWrapper( this );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_CREATEDESTBYNAMEANDIDL_EXIT );
    }

void CCmDestinationWrapper::GetConnMethodAsEmbeddedDestL(
        RCmManagerApi* aCmManagerApi,
        const TInt aConnMethodHandle,
        TInt& aExistingHandle )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_GETCONNMETHODASEMBEDDEDDESTL_ENTRY );

    if ( iCmManagerApi || iCmDestinationApi.SessionConnected() )
        {
        User::Leave( KErrAlreadyExists );
        }

    iCmManagerApi = aCmManagerApi;
    iCmDestinationApi.ConnectAndGetConnMethodAsEmbeddedDestL( *aCmManagerApi, aConnMethodHandle, aExistingHandle );
    if ( !aExistingHandle )
        {
        iCmManagerApi->StoreDestinationWrapper( this );
        }

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_GETCONNMETHODASEMBEDDEDDESTL_EXIT );
    }

TInt CCmDestinationWrapper::ConnectionMethodCount( TInt& aCount )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_CONNECTIONMETHODCOUNT_ENTRY );

    TInt err = iCmDestinationApi.ConnectionMethodCount( aCount );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_CONNECTIONMETHODCOUNT_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::Priority( const TInt aHandle, TUint& aPriority )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_PRIORITY_ENTRY );

    TInt err = iCmDestinationApi.Priority( aHandle, aPriority );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_PRIORITY_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::GetName( HBufC* aBuffer )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_GETNAME_ENTRY );

    TInt err = iCmDestinationApi.GetName( aBuffer );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_GETNAME_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::GetId( TUint32& aId )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_GETID_ENTRY );

    TInt err = iCmDestinationApi.GetId( aId );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_GETID_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::GetElementId( TUint32& aElementId )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_GETELEMENTID_ENTRY );

    TInt err = iCmDestinationApi.GetElementId( aElementId );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_GETELEMENTID_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::GetMetadata(
        const TUint32 aMetadataField,
        TUint32& aValue )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_GETMETADATA_ENTRY );

    TInt err = iCmDestinationApi.GetMetadata( aMetadataField, aValue );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_GETMETADATA_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::GetProtectionLevel( TInt& aProtectionLevel )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_GETPROTECTIONLEVEL_ENTRY );

    TInt err = iCmDestinationApi.GetProtectionLevel( aProtectionLevel );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_GETPROTECTIONLEVEL_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::AddConnectionMethod(
        const TInt aConnMethodHandle,
        TInt& aIndex )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_ADDCONNECTIONMETHOD_ENTRY );

    TInt err = iCmDestinationApi.AddConnectionMethod( aConnMethodHandle, aIndex );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_ADDCONNECTIONMETHOD_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::AddEmbeddedDestination(
        const TInt aDestHandle,
        TInt& aIndex )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_ADDEMBEDDEDDESTINATION_ENTRY );

    TInt err = iCmDestinationApi.AddEmbeddedDestination( aDestHandle, aIndex );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_ADDEMBEDDEDDESTINATION_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::DeleteConnectionMethod(
        const TInt aConnMethodHandle )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_DELETECONNECTIONMETHOD_ENTRY );

    TInt err = iCmDestinationApi.DeleteConnectionMethod( aConnMethodHandle );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_DELETECONNECTIONMETHOD_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::RemoveConnectionMethod(
        const TInt aConnMethodHandle )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_REMOVECONNECTIONMETHOD_ENTRY );

    TInt err = iCmDestinationApi.RemoveConnectionMethod( aConnMethodHandle );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_REMOVECONNECTIONMETHOD_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::ModifyPriority(
        const TInt aConnMethodHandle,
        const TUint aIndex )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_MODIFYPRIORITY_ENTRY );

    TInt err = iCmDestinationApi.ModifyPriority( aConnMethodHandle, aIndex );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_MODIFYPRIORITY_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::SetName( const TDesC& aValue )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_SETNAME_ENTRY );

    TInt err = iCmDestinationApi.SetName( aValue );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_SETNAME_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::SetMetadata(
        const TUint32 aMetaField,
        const TUint32 aValue )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_SETMETADATA_ENTRY );

    TInt err = iCmDestinationApi.SetMetadata( aMetaField, aValue );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_SETMETADATA_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::SetProtection( const TInt aValue )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_SETPROTECTION_ENTRY );

    TInt err = iCmDestinationApi.SetProtection( aValue );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_SETPROTECTION_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::SetHidden( const TBool aHidden )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_SETHIDDEN_ENTRY );

    TInt err = iCmDestinationApi.SetHidden( aHidden );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_SETHIDDEN_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::Update()
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_UPDATE_ENTRY );

    TInt err = iCmDestinationApi.Update();

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_UPDATE_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::Refresh()
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_REFRESH_ENTRY );

    TInt err = iCmDestinationApi.Refresh();

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_REFRESH_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::Delete()
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_DELETE_ENTRY );

    TInt err = iCmDestinationApi.Delete();

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_DELETE_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::IsConnected( TBool& aResult )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_ISCONNECTED_ENTRY );

    TInt err = iCmDestinationApi.IsConnected( aResult );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_ISCONNECTED_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::IsHidden( TBool& aResult )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_ISHIDDEN_ENTRY );

    TInt err = iCmDestinationApi.IsHidden( aResult );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_ISHIDDEN_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::IsEqual( const TInt aHandle, TBool& aResult )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_ISEQUAL_ENTRY );

    TInt err = iCmDestinationApi.IsEqual( aHandle, aResult );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_ISEQUAL_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::GetIcon( HBufC* aBuffer )
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_GETICON_ENTRY );

    TInt err = iCmDestinationApi.GetIcon( aBuffer );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_GETICON_EXIT );
    return err;
    }

TInt CCmDestinationWrapper::SetIcon( const TDesC& aValue)
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_SETICON_ENTRY );

    TInt err = iCmDestinationApi.SetIcon( aValue );

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_SETICON_EXIT );
    return err;
    }

TBool CCmDestinationWrapper::SessionConnected()
    {
    return iCmDestinationApi.SessionConnected();
    }

void CCmDestinationWrapper::CloseSession()
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_CLOSESESSION_ENTRY );
    iCmDestinationApi.Close();
    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_CLOSESESSION_EXIT );
    }

TInt CCmDestinationWrapper::GetHandle() const
    {
    return iCmDestinationApi.SubSessionHandle();
    }

RCmManagerApi* CCmDestinationWrapper::GetMainSession() const
    {
    return iCmManagerApi;
    }

void CCmDestinationWrapper::IncreaseReferenceCounter()
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_INCREASEREFERENCECOUNTER_ENTRY );
    iReferenceCounter++;
    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_INCREASEREFERENCECOUNTER_EXIT );
    }

TInt CCmDestinationWrapper::DecreaseReferenceCounter()
    {
    OstTraceFunctionEntry0( CCMDESTINATIONWRAPPER_DECREASEREFERENCECOUNTER_ENTRY );

    iReferenceCounter--;
    if ( iReferenceCounter < 0 )
        {
        iReferenceCounter = 0;
        }

    OstTraceFunctionExit0( CCMDESTINATIONWRAPPER_DECREASEREFERENCECOUNTER_EXIT );
    return iReferenceCounter;
    }

// End of file
