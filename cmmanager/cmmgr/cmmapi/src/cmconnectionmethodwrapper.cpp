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
* Collects API calls from RCmConnectionMethod- and
* RCmConnectionMethodExt-classes.
*
*/


#include "cmconnectionmethodwrapper.h"
#include "cmmanagerapi.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmconnectionmethodwrapperTraces.h"
#endif


CCmConnectionMethodWrapper* CCmConnectionMethodWrapper::NewL()
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_NEWL_ENTRY );

    CCmConnectionMethodWrapper* self = CCmConnectionMethodWrapper::NewLC();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_NEWL_EXIT );
    return self;
    }

CCmConnectionMethodWrapper* CCmConnectionMethodWrapper::NewLC()
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_NEWLC_ENTRY );

    CCmConnectionMethodWrapper* self = new (ELeave) CCmConnectionMethodWrapper;
    CleanupStack::PushL( self );

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_NEWLC_EXIT );
    return self;
    }

CCmConnectionMethodWrapper::~CCmConnectionMethodWrapper()
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_CCMCONNECTIONMETHODWRAPPER_ENTRY );

    iCmConnectionMethodApi.Close();

    if ( iCmManagerApi )
        {
        iCmManagerApi->RemoveConnMethodWrapper( this );
        iCmManagerApi = NULL;
        }

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_CCMCONNECTIONMETHODWRAPPER_EXIT );
    }

CCmConnectionMethodWrapper::CCmConnectionMethodWrapper()
    {
    OstTraceFunctionEntry0( DUP1_CCMCONNECTIONMETHODWRAPPER_CCMCONNECTIONMETHODWRAPPER_ENTRY );

    iCmManagerApi = NULL;
    iReferenceCounter = 1;

    OstTraceFunctionExit0( DUP1_CCMCONNECTIONMETHODWRAPPER_CCMCONNECTIONMETHODWRAPPER_EXIT );
    }

void CCmConnectionMethodWrapper::GetConnMethodWithIdL(
        RCmManagerApi* aCmManagerApi,
        const TUint32 aIapId,
        TInt& aExistingHandle )
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_GETCONNMETHODWITHIDL_ENTRY );

    if ( iCmManagerApi || iCmConnectionMethodApi.SessionConnected() )
        {
        User::Leave( KErrAlreadyExists );
        }

    iCmManagerApi = aCmManagerApi;
    iCmConnectionMethodApi.ConnectAndGetConnMethodWithIdL(
            *aCmManagerApi,
            aIapId,
            aExistingHandle );
    if ( !aExistingHandle )
        {
        iCmManagerApi->StoreConnMethodWrapper( this );
        }

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_GETCONNMETHODWITHIDL_EXIT );
    }

void CCmConnectionMethodWrapper::GetConnMethodFromDestWithIndexL(
        RCmManagerApi* aCmManagerApi,
        const TInt aDestHandle,
        const TInt aIndex,
        TInt& aExistingHandle )
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_GETCONNMETHODFROMDESTWITHINDEXL_ENTRY );

    if ( iCmManagerApi || iCmConnectionMethodApi.SessionConnected() )
        {
        User::Leave( KErrAlreadyExists );
        }

    iCmManagerApi = aCmManagerApi;
    iCmConnectionMethodApi.ConnectAndGetConnMethodFromDestWithIndexL(
            *aCmManagerApi,
            aDestHandle,
            aIndex,
            aExistingHandle );
    if ( !aExistingHandle )
        {
        iCmManagerApi->StoreConnMethodWrapper( this );
        }

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_GETCONNMETHODFROMDESTWITHINDEXL_EXIT );
    }

void CCmConnectionMethodWrapper::GetConnMethodFromDestWithIdL(
        RCmManagerApi* aCmManagerApi,
        const TInt aDestHandle,
        const TInt aIapId,
        TInt& aExistingHandle )
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_GETCONNMETHODFROMDESTWITHIDL_ENTRY );

    if ( iCmManagerApi || iCmConnectionMethodApi.SessionConnected() )
        {
        User::Leave( KErrAlreadyExists );
        }

    iCmManagerApi = aCmManagerApi;
    iCmConnectionMethodApi.ConnectAndGetConnMethodFromDestWithIdL(
            *aCmManagerApi,
            aDestHandle,
            aIapId,
            aExistingHandle );
    if ( !aExistingHandle )
        {
        iCmManagerApi->StoreConnMethodWrapper( this );
        }

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_GETCONNMETHODFROMDESTWITHIDL_EXIT );
    }

void CCmConnectionMethodWrapper::CreateConnMethodL(
        RCmManagerApi* aCmManagerApi,
        const TUint32 aBearerType )
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_CREATECONNMETHODL_ENTRY );

    if ( iCmManagerApi || iCmConnectionMethodApi.SessionConnected() )
        {
        User::Leave( KErrAlreadyExists );
        }

    iCmManagerApi = aCmManagerApi;
    iCmConnectionMethodApi.ConnectAndCreateConnMethodL(
            *aCmManagerApi,
            aBearerType );
    iCmManagerApi->StoreConnMethodWrapper( this );

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_CREATECONNMETHODL_EXIT );
    }

void CCmConnectionMethodWrapper::CreateConnMethodWithIdL(
        RCmManagerApi* aCmManagerApi,
        const TUint32 aBearerType,
        const TUint32 aIapId )
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_CREATECONNMETHODWITHIDL_ENTRY );

    if ( iCmManagerApi || iCmConnectionMethodApi.SessionConnected() )
        {
        User::Leave( KErrAlreadyExists );
        }

    iCmManagerApi = aCmManagerApi;
    iCmConnectionMethodApi.ConnectAndCreateConnMethodWithIdL(
            *aCmManagerApi,
            aBearerType,
            aIapId );
    iCmManagerApi->StoreConnMethodWrapper( this );

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_CREATECONNMETHODWITHIDL_EXIT );
    }

TInt CCmConnectionMethodWrapper::CreateConnMethodCopy(
        RCmManagerApi* aCmManagerApi,
        const TInt aConnMethodHandle )
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_CREATECONNMETHODCOPY_ENTRY );

    TInt err( KErrNone );

    if ( iCmManagerApi || iCmConnectionMethodApi.SessionConnected() )
        {
        err = KErrAlreadyExists;
        }
    else
        {
        err = iCmConnectionMethodApi.ConnectAndCreateConnMethodCopy(
                *aCmManagerApi,
                aConnMethodHandle );
        if ( !err )
            {
            iCmManagerApi = aCmManagerApi;
            iCmManagerApi->StoreConnMethodWrapper( this );
            }
        }

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_CREATECONNMETHODCOPY_EXIT );
    return err;
    }

void CCmConnectionMethodWrapper::CreateConnMethodToDestL(
        RCmManagerApi* aCmManagerApi,
        const TInt aDestHandle,
        const TUint32 aBearerType )
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_CREATECONNMETHODFROMDESTL_ENTRY );

    if ( iCmManagerApi || iCmConnectionMethodApi.SessionConnected() )
        {
        User::Leave( KErrAlreadyExists );
        }

    iCmManagerApi = aCmManagerApi;
    iCmConnectionMethodApi.ConnectAndCreateConnMethodToDestL(
            *aCmManagerApi,
            aDestHandle,
            aBearerType );
    iCmManagerApi->StoreConnMethodWrapper( this );

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_CREATECONNMETHODFROMDESTL_EXIT );
    }

void CCmConnectionMethodWrapper::CreateConnMethodToDestWithIdL(
        RCmManagerApi* aCmManagerApi,
        const TInt aDestHandle,
        const TUint32 aBearerType,
        const TUint32 aIapId )
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_CREATECONNMETHODFROMDESTWITHIDL_ENTRY );

    if ( iCmManagerApi || iCmConnectionMethodApi.SessionConnected() )
        {
        User::Leave( KErrAlreadyExists );
        }

    iCmManagerApi = aCmManagerApi;
    iCmConnectionMethodApi.ConnectAndCreateConnMethodToDestWithIdL(
            *aCmManagerApi,
            aDestHandle,
            aBearerType,
            aIapId );
    iCmManagerApi->StoreConnMethodWrapper( this );

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_CREATECONNMETHODFROMDESTWITHIDL_EXIT );
    }

TInt CCmConnectionMethodWrapper::GetIntAttribute(
        const TUint32 aAttribute,
        TUint32& aValue )
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_GETINTATTRIBUTE_ENTRY );

    TInt err = iCmConnectionMethodApi.GetIntAttribute( aAttribute, aValue );

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_GETINTATTRIBUTE_EXIT );
    return err;
    }

TInt CCmConnectionMethodWrapper::GetBoolAttribute(
        const TUint32 aAttribute,
        TBool& aValue )
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_GETBOOLATTRIBUTE_ENTRY );

    TInt err = iCmConnectionMethodApi.GetBoolAttribute( aAttribute, aValue );

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_GETBOOLATTRIBUTE_EXIT );
    return err;
    }

TInt CCmConnectionMethodWrapper::GetStringAttribute(
        const TUint32 aAttribute,
        HBufC* aBuffer )
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_GETSTRINGATTRIBUTE_ENTRY );

    TInt err = iCmConnectionMethodApi.GetStringAttribute( aAttribute, aBuffer );

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_GETSTRINGATTRIBUTE_EXIT );
    return err;
    }

TInt CCmConnectionMethodWrapper::GetString8Attribute(
        const TUint32 aAttribute,
        HBufC8* aBuffer8 )
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_GETSTRING8ATTRIBUTE_ENTRY );

    TInt err = iCmConnectionMethodApi.GetString8Attribute( aAttribute, aBuffer8 );

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_GETSTRING8ATTRIBUTE_EXIT );
    return err;
    }

TInt CCmConnectionMethodWrapper::SetIntAttribute(
        const TUint32 aAttribute,
        const TUint32 aValue )
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_SETINTATTRIBUTE_ENTRY );

    TInt err = iCmConnectionMethodApi.SetIntAttribute( aAttribute, aValue );

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_SETINTATTRIBUTE_EXIT );
    return err;
    }

TInt CCmConnectionMethodWrapper::SetBoolAttribute(
        const TUint32 aAttribute,
        const TBool aValue )
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_SETBOOLATTRIBUTE_ENTRY );

    TInt err = iCmConnectionMethodApi.SetBoolAttribute( aAttribute, aValue );

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_SETBOOLATTRIBUTE_EXIT );
    return err;
    }

TInt CCmConnectionMethodWrapper::SetStringAttribute(
        const TUint32 aAttribute,
        const TDesC16& aValue )
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_SETSTRINGATTRIBUTE_ENTRY );

    TInt err = iCmConnectionMethodApi.SetStringAttribute( aAttribute, aValue );

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_SETSTRINGATTRIBUTE_EXIT );
    return err;
    }

TInt CCmConnectionMethodWrapper::SetString8Attribute(
        const TUint32 aAttribute,
        const TDesC8& aValue )
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_SETSTRING8ATTRIBUTE_ENTRY );

    TInt err = iCmConnectionMethodApi.SetString8Attribute( aAttribute, aValue );

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_SETSTRING8ATTRIBUTE_EXIT );
    return err;
    }

TInt CCmConnectionMethodWrapper::Update()
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_UPDATE_ENTRY );

    TInt result = iCmConnectionMethodApi.Update();

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_UPDATE_EXIT );
    return result;
    }

TInt CCmConnectionMethodWrapper::Refresh()
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_REFRESH_ENTRY );

    TInt result = iCmConnectionMethodApi.Refresh();

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_REFRESH_EXIT );
    return result;
    }

TInt CCmConnectionMethodWrapper::Delete( TBool& aResult )
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_DELETE_ENTRY );

    TInt err = iCmConnectionMethodApi.Delete( aResult );

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_DELETE_EXIT );
    return err;
    }

TInt CCmConnectionMethodWrapper::IsEqual( const TInt aHandle, TBool& aResult )
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_ISEQUAL_ENTRY );

    TInt err = iCmConnectionMethodApi.IsEqual( aHandle, aResult );

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_ISEQUAL_EXIT );
    return err;
    }

TBool CCmConnectionMethodWrapper::SessionConnected() const
    {
    return iCmConnectionMethodApi.SessionConnected();
    }

void CCmConnectionMethodWrapper::CloseSession()
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_CLOSESESSION_ENTRY );
    iCmConnectionMethodApi.Close();
    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_CLOSESESSION_EXIT );
    }

TInt CCmConnectionMethodWrapper::GetHandle() const
    {
    return iCmConnectionMethodApi.SubSessionHandle();
    }

RCmManagerApi* CCmConnectionMethodWrapper::GetMainSession() const
    {
    return iCmManagerApi;
    }

void CCmConnectionMethodWrapper::IncreaseReferenceCounter()
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_INCREASEREFERENCECOUNTER_ENTRY );
    iReferenceCounter++;
    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_INCREASEREFERENCECOUNTER_EXIT );
    }

TInt CCmConnectionMethodWrapper::DecreaseReferenceCounter()
    {
    OstTraceFunctionEntry0( CCMCONNECTIONMETHODWRAPPER_DECREASEREFERENCECOUNTER_ENTRY );

    iReferenceCounter--;
    if ( iReferenceCounter < 0 )
        {
        iReferenceCounter = 0;
        }

    OstTraceFunctionExit0( CCMCONNECTIONMETHODWRAPPER_DECREASEREFERENCECOUNTER_EXIT );
    return iReferenceCounter;
    }

// End of file
