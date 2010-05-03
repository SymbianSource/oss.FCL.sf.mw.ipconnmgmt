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
* Implementation of RCmManagerExt.
*
*/


#include <cmmanagerext.h>
#include <cmconnectionmethodext.h>
#include <cmdestinationext.h>

#include "cmmserverdefs.h"
#include "cmmanagerapi.h"
#include "cmconnectionmethodwrapper.h"
#include "cmdestinationwrapper.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmanagerextTraces.h"
#endif


//-----------------------------------------------------------------------------
//  RCmManagerExt::OpenL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::OpenL()
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_OPENL_ENTRY );

    OpenLC();
    CleanupStack::Pop( this );

    OstTraceFunctionExit0( RCMMANAGEREXT_OPENL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::OpenLC()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::OpenLC()
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_OPENLC_ENTRY );

    if ( iCmManagerApi )
        {
        User::Leave( KErrAlreadyExists );
        }

    CleanupClosePushL( *this );
    iCmManagerApi = new (ELeave) RCmManagerApi();
    User::LeaveIfError( iCmManagerApi->Connect() );

    OstTraceFunctionExit0( RCMMANAGEREXT_OPENLC_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::Close()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::Close()
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_CLOSE_ENTRY );

    if ( iCmManagerApi )
        {
        iCmManagerApi->Close();
        delete iCmManagerApi;
        iCmManagerApi = NULL;
        }

    OstTraceFunctionExit0( RCMMANAGEREXT_CLOSE_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::CreateTablesAndOpenL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::CreateTablesAndOpenL()
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_CREATETABLESANDOPENL_ENTRY );
    OpenL();
    OstTraceFunctionExit0( RCMMANAGEREXT_CREATETABLESANDOPENL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::ConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethodExt RCmManagerExt::ConnectionMethodL(
        TUint32 aConnectionMethodId )
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_CONNECTIONMETHODL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    if ( !( ( aConnectionMethodId > KCmmConnMethodIdIntervalMin ) &&
            ( aConnectionMethodId < KCmmConnMethodIdIntervalMax ) ) )
        {
        User::Leave( KErrArgument );
        }

    // If client already has a handle open to this connection method, give a
    // reference to it.
    CCmConnectionMethodWrapper* cmWrapper = CCmConnectionMethodWrapper::NewLC();
    TInt existingHandle( 0 );
    cmWrapper->GetConnMethodWithIdL( iCmManagerApi, aConnectionMethodId, existingHandle );
    if ( existingHandle )
        {
        CleanupStack::PopAndDestroy( cmWrapper );
        cmWrapper = iCmManagerApi->GetConnMethodWrapperL( existingHandle );
        cmWrapper->IncreaseReferenceCounter();
        }
    else
        {
        CleanupStack::Pop( cmWrapper );
        }

    RCmConnectionMethodExt cm;
    cm.iCmConnectionMethodWrapper = cmWrapper;

    OstTraceFunctionExit0( RCMMANAGEREXT_CONNECTIONMETHODL_EXIT );
    return cm;
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::GetBearerInfoIntL()
//-----------------------------------------------------------------------------
//
EXPORT_C TUint32 RCmManagerExt::GetBearerInfoIntL(
        TUint32 aBearerType,
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_GETBEARERINFOINTL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    TUint32 value( 0 );
    TInt err = iCmManagerApi->GetBearerInfoInt( aBearerType, aAttribute, value );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMMANAGEREXT_GETBEARERINFOINTL_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::GetBearerInfoBoolL()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool RCmManagerExt::GetBearerInfoBoolL(
        TUint32 aBearerType,
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_GETBEARERINFOBOOLL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    TBool value( 0 );
    TInt err = iCmManagerApi->GetBearerInfoBool( aBearerType, aAttribute, value );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMMANAGEREXT_GETBEARERINFOBOOLL_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::GetBearerInfoStringL()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC* RCmManagerExt::GetBearerInfoStringL(
        TUint32 aBearerType,
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_GETBEARERINFOSTRINGL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    HBufC* buffer = HBufC::NewLC( KCmmStringLengthMax );
    TInt err = iCmManagerApi->GetBearerInfoString( aBearerType, aAttribute, buffer );
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

    OstTraceFunctionExit0( RCMMANAGEREXT_GETBEARERINFOSTRINGL_EXIT );
    return buffer;
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::GetBearerInfoString8L()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC8* RCmManagerExt::GetBearerInfoString8L(
        TUint32 aBearerType,
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_GETBEARERINFOSTRING8L_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    HBufC8* buffer8 = HBufC8::NewLC( KCmmStringLengthMax );
    TInt err = iCmManagerApi->GetBearerInfoString8( aBearerType, aAttribute, buffer8 );
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

    OstTraceFunctionExit0( RCMMANAGEREXT_GETBEARERINFOSTRING8L_EXIT );
    return buffer8;
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::GetConnectionMethodInfoIntL()
//-----------------------------------------------------------------------------
//
EXPORT_C TUint32 RCmManagerExt::GetConnectionMethodInfoIntL(
        TUint32 aIapId,
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_GETCONNECTIONMETHODINFOINTL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    TUint32 value( 0 );
    TInt err = iCmManagerApi->GetConnectionMethodInfoInt( aIapId, aAttribute, value );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMMANAGEREXT_GETCONNECTIONMETHODINFOINTL_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::GetConnectionMethodInfoBoolL()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool RCmManagerExt::GetConnectionMethodInfoBoolL(
        TUint32 aIapId,
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_GETCONNECTIONMETHODINFOBOOLL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    TBool value( 0 );
    TInt err = iCmManagerApi->GetConnectionMethodInfoBool( aIapId, aAttribute, value );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMMANAGEREXT_GETCONNECTIONMETHODINFOBOOLL_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::GetConnectionMethodInfoStringL()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC* RCmManagerExt::GetConnectionMethodInfoStringL(
        TUint32 aIapId,
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_GETCONNECTIONMETHODINFOSTRINGL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    HBufC* buffer = HBufC::NewLC( KCmmStringLengthMax );
    TInt err = iCmManagerApi->GetConnectionMethodInfoString( aIapId, aAttribute, buffer );
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

    OstTraceFunctionExit0( RCMMANAGEREXT_GETCONNECTIONMETHODINFOSTRINGL_EXIT );
    return buffer;
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::GetConnectionMethodInfoString8L()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC8* RCmManagerExt::GetConnectionMethodInfoString8L(
        TUint32 aIapId,
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_GETCONNECTIONMETHODINFOSTRING8L_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    HBufC8* buffer8 = HBufC8::NewLC( KCmmStringLengthMax );
    TInt err = iCmManagerApi->GetConnectionMethodInfoString8( aIapId, aAttribute, buffer8 );
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

    OstTraceFunctionExit0( RCMMANAGEREXT_GETCONNECTIONMETHODINFOSTRING8L_EXIT );
    return buffer8;
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::CreateDestinationL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmDestinationExt RCmManagerExt::CreateDestinationL(
        const TDesC& aName )
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_CREATEDESTINATIONL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    if ( aName.Length() == 0 )
        {
        User::Leave( KErrArgument );
        }

    CCmDestinationWrapper* destWrap = CCmDestinationWrapper::NewLC();
    destWrap->CreateDestByNameL( iCmManagerApi, aName );
    CleanupStack::Pop( destWrap );

    RCmDestinationExt dest;
    dest.iCmDestinationWrapper = destWrap;

    OstTraceFunctionExit0( RCMMANAGEREXT_CREATEDESTINATIONL_EXIT );
    return dest;
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::CreateConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethodExt RCmManagerExt::CreateConnectionMethodL(
        TUint32 aBearerType )
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_CREATECONNECTIONMETHODL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    CCmConnectionMethodWrapper* cmWrapper = CCmConnectionMethodWrapper::NewLC();
    cmWrapper->CreateConnMethodL( iCmManagerApi, aBearerType );
    CleanupStack::Pop( cmWrapper );

    RCmConnectionMethodExt cm;
    cm.iCmConnectionMethodWrapper = cmWrapper;

    OstTraceFunctionExit0( RCMMANAGEREXT_CREATECONNECTIONMETHODL_EXIT );
    return cm;
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::CopyConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C TInt RCmManagerExt::CopyConnectionMethodL(
        RCmDestinationExt& aTargetDestination,
        RCmConnectionMethodExt& aConnectionMethod )
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_COPYCONNECTIONMETHODL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    if ( !aTargetDestination.iCmDestinationWrapper ||
            !aConnectionMethod.iCmConnectionMethodWrapper )
        {
        User::Leave( KErrArgument );
        }
    if ( !aTargetDestination.iCmDestinationWrapper->SessionConnected() ||
            !aConnectionMethod.iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrArgument );
        }

    TInt index( 0 );
    TInt targetDestHandle = aTargetDestination.iCmDestinationWrapper->GetHandle();
    TInt connMethodHandle = aConnectionMethod.iCmConnectionMethodWrapper->GetHandle();
    TInt err = iCmManagerApi->CopyConnectionMethod( targetDestHandle, connMethodHandle, index );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMMANAGEREXT_COPYCONNECTIONMETHODL_EXIT );
    return index;
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::MoveConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C TInt RCmManagerExt::MoveConnectionMethodL(
        RCmDestinationExt& aSourceDestination,
        RCmDestinationExt& aTargetDestination,
        RCmConnectionMethodExt& aConnectionMethod )
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_MOVECONNECTIONMETHODL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    if ( !aSourceDestination.iCmDestinationWrapper ||
            !aTargetDestination.iCmDestinationWrapper ||
            !aConnectionMethod.iCmConnectionMethodWrapper )
        {
        User::Leave( KErrArgument );
        }
    if ( !aSourceDestination.iCmDestinationWrapper->SessionConnected() ||
            !aTargetDestination.iCmDestinationWrapper->SessionConnected() ||
            !aConnectionMethod.iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrArgument );
        }

    TCmmIpcStructMoveConnMethod pkgData;
    pkgData.iSourceDestHandle = aSourceDestination.iCmDestinationWrapper->GetHandle();
    pkgData.iTargetDestHandle = aTargetDestination.iCmDestinationWrapper->GetHandle();
    pkgData.iConnMethodHandle = aConnectionMethod.iCmConnectionMethodWrapper->GetHandle();
    pkgData.iIndex = 0;

    TInt err = iCmManagerApi->MoveConnectionMethod( pkgData );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMMANAGEREXT_MOVECONNECTIONMETHODL_EXIT );
    return pkgData.iIndex;
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::RemoveConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::RemoveConnectionMethodL(
        RCmDestinationExt& aDestination,
        RCmConnectionMethodExt& aConnectionMethod )
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_REMOVECONNECTIONMETHODL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    if ( !aDestination.iCmDestinationWrapper ||
            !aConnectionMethod.iCmConnectionMethodWrapper )
        {
        User::Leave( KErrArgument );
        }
    if ( !aDestination.iCmDestinationWrapper->SessionConnected() ||
            !aConnectionMethod.iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrArgument );
        }

    TInt targetDestHandle = aDestination.iCmDestinationWrapper->GetHandle();
    TInt connMethodHandle = aConnectionMethod.iCmConnectionMethodWrapper->GetHandle();
    TInt err = iCmManagerApi->RemoveConnectionMethod( targetDestHandle, connMethodHandle );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMMANAGEREXT_REMOVECONNECTIONMETHODL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::RemoveAllReferencesL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::RemoveAllReferencesL(
        RCmConnectionMethodExt& aConnectionMethod )
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_REMOVEALLREFERENCESL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    if ( !aConnectionMethod.iCmConnectionMethodWrapper ||
            !aConnectionMethod.iCmConnectionMethodWrapper->SessionConnected() )
        {
        User::Leave( KErrArgument );
        }

    TInt connMethodHandle = aConnectionMethod.iCmConnectionMethodWrapper->GetHandle();
    TInt err = iCmManagerApi->RemoveAllReferences( connMethodHandle );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMMANAGEREXT_REMOVEALLREFERENCESL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::ConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::ConnectionMethodL(
        RArray<TUint32>& aCmArray,
        TBool aCheckBearerType,
        TBool aLegacyOnly,
        TBool aEasyWlan )
    {
    OstTraceFunctionEntry0( DUP1_RCMMANAGEREXT_CONNECTIONMETHODL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    TCmmIpcStructGetConnMethods parameters;
    parameters.iCheckBearerType = aCheckBearerType;
    parameters.iLegacyOnly = aLegacyOnly;
    parameters.iEasyWlan = aEasyWlan;

    iCmManagerApi->GetConnMethodsL( parameters, aCmArray );

    OstTraceFunctionExit0( DUP1_RCMMANAGEREXT_CONNECTIONMETHODL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::AllDestinationsL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::AllDestinationsL( RArray<TUint32>& aDestArray )
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_ALLDESTINATIONSL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    iCmManagerApi->GetAllDestinationsL( aDestArray );

    OstTraceFunctionExit0( RCMMANAGEREXT_ALLDESTINATIONSL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::DestinationL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmDestinationExt RCmManagerExt::DestinationL( TUint32 aDestinationId )
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_DESTINATIONL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    // If client already has a handle open to this destination, give a
    // reference to it.
    CCmDestinationWrapper* destWrap = CCmDestinationWrapper::NewLC();
    TInt existingHandle( 0 );
    destWrap->GetDestWithIdL( iCmManagerApi, aDestinationId, existingHandle );
    if ( existingHandle )
        {
        CleanupStack::PopAndDestroy( destWrap );
        destWrap = iCmManagerApi->GetDestinationWrapperL( existingHandle );
        destWrap->IncreaseReferenceCounter();
        }
    else
        {
        CleanupStack::Pop( destWrap );
        }

    RCmDestinationExt dest;
    dest.iCmDestinationWrapper = destWrap;

    OstTraceFunctionExit0( RCMMANAGEREXT_DESTINATIONL_EXIT );
    return dest;
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::BearerPriorityArrayL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::BearerPriorityArrayL(
        RArray<TBearerPriority>& aArray ) const
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_BEARERPRIORITYARRAYL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    iCmManagerApi->GetBearerPriorityArrayL( aArray );

    OstTraceFunctionExit0( RCMMANAGEREXT_BEARERPRIORITYARRAYL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::UpdateBearerPriorityArrayL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::UpdateBearerPriorityArrayL(
        const RArray<TBearerPriority>& aArray )
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_UPDATEBEARERPRIORITYARRAYL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    iCmManagerApi->SetBearerPriorityArrayL( aArray );

    OstTraceFunctionExit0( RCMMANAGEREXT_UPDATEBEARERPRIORITYARRAYL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::CleanupGlobalPriorityArray()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::CleanupGlobalPriorityArray(
        RArray<TBearerPriority>& aArray ) const
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_CLEANUPGLOBALPRIORITYARRAY_ENTRY );

    for ( TInt i = 0; i < aArray.Count(); i++ )
        {
        delete aArray[i].iServiceType;
        }
    aArray.Reset();
    aArray.Close();

    OstTraceFunctionExit0( RCMMANAGEREXT_CLEANUPGLOBALPRIORITYARRAY_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::SupportedBearersL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::SupportedBearersL( RArray<TUint32>& aArray ) const
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_SUPPORTEDBEARERSL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }
    iCmManagerApi->GetSupportedBearersL( aArray );

    OstTraceFunctionExit0( RCMMANAGEREXT_SUPPORTEDBEARERSL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::EasyWlanIdL()
//-----------------------------------------------------------------------------
//
EXPORT_C TUint32 RCmManagerExt::EasyWlanIdL()
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_EASYWLANIDL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    TUint32 value( 0 );
    TInt err = iCmManagerApi->GetEasyWlanId( value );
    User::LeaveIfError( err );
    OstTraceFunctionExit0( RCMMANAGEREXT_EASYWLANIDL_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::ReadDefConnL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::ReadDefConnL( TCmDefConnValue& aDefConnSetting )
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_READDEFCONNL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmManagerApi->ReadDefaultConnection( aDefConnSetting );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMMANAGEREXT_READDEFCONNL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::WriteDefConnL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::WriteDefConnL( const TCmDefConnValue& aDefConnSetting )
    {
    OstTraceFunctionEntry0( RCMMANAGEREXT_WRITEDEFCONNL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmManagerApi->WriteDefaultConnection( aDefConnSetting );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMMANAGEREXT_WRITEDEFCONNL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::CreateDestinationL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmDestinationExt RCmManagerExt::CreateDestinationL(
        const TDesC& aName,
        TUint32 aDestId )
    {
    OstTraceFunctionEntry0( DUP1_RCMMANAGEREXT_CREATEDESTINATIONL_ENTRY );

    if ( aDestId == 0 )
        {
        return CreateDestinationL( aName );
        }

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    if ( aName.Length() == 0 )
        {
        User::Leave( KErrArgument );
        }

    TUint32 adjustedDestId( 0 );
    if ( aDestId > KCmmDestIdIntervalLegacyMin && aDestId < KCmmDestIdIntervalLegacyMax )
        {
        adjustedDestId = aDestId + KCmDefaultDestinationAPTagId;
        }
    else if ( aDestId > KCmmDestIdIntervalMin && aDestId < KCmmDestIdIntervalMax )
        {
        adjustedDestId = aDestId;
        }
    else
        {
        User::Leave( KErrArgument );
        }

    CCmDestinationWrapper* destWrap = CCmDestinationWrapper::NewLC();
    destWrap->CreateDestByNameAndIdL( iCmManagerApi, aName, adjustedDestId );
    CleanupStack::Pop( destWrap );

    RCmDestinationExt dest;
    dest.iCmDestinationWrapper = destWrap;

    OstTraceFunctionExit0( DUP1_RCMMANAGEREXT_CREATEDESTINATIONL_EXIT );
    return dest;
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::CreateConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethodExt RCmManagerExt::CreateConnectionMethodL(
        TUint32 aBearerType,
        TUint32 aConnMethodId )
    {
    OstTraceFunctionEntry0( DUP1_RCMMANAGEREXT_CREATECONNECTIONMETHODL_ENTRY );

    if ( aConnMethodId == 0 )
        {
        return CreateConnectionMethodL( aBearerType );
        }

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }
    if ( !( aConnMethodId > KCmmConnMethodIdIntervalMin &&
            aConnMethodId < KCmmConnMethodIdIntervalMax ) )
        {
        User::Leave( KErrArgument );
        }

    CCmConnectionMethodWrapper* cmWrapper = CCmConnectionMethodWrapper::NewLC();
    cmWrapper->CreateConnMethodWithIdL( iCmManagerApi, aBearerType, aConnMethodId );
    CleanupStack::Pop( cmWrapper );

    RCmConnectionMethodExt cm;
    cm.iCmConnectionMethodWrapper = cmWrapper;

    OstTraceFunctionExit0( DUP1_RCMMANAGEREXT_CREATECONNECTIONMETHODL_EXIT );
    return cm;
    }

// End of file
