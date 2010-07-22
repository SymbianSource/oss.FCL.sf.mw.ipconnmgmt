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
* Implementation of RCmManager.
*
*/


#include <cmmanager.h>
#include <cmconnectionmethod.h>
#include <cmdestination.h>

#include "cmmserverdefs.h"
#include "cmmanagerapi.h"
#include "cmconnectionmethodwrapper.h"
#include "cmdestinationwrapper.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmanagerTraces.h"
#endif


//-----------------------------------------------------------------------------
//  RCmManager::OpenL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::OpenL()
    {
    OstTraceFunctionEntry0( RCMMANAGER_OPENL_ENTRY );

    OpenLC();
    CleanupStack::Pop( this );

    OstTraceFunctionExit0( RCMMANAGER_OPENL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManager::OpenLC()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::OpenLC()
    {
    OstTraceFunctionEntry0( RCMMANAGER_OPENLC_ENTRY );

    if ( iCmManagerApi )
        {
        User::Leave( KErrAlreadyExists );
        }

    CleanupClosePushL( *this );
    iCmManagerApi = new (ELeave) RCmManagerApi();
    User::LeaveIfError( iCmManagerApi->Connect() );

    OstTraceFunctionExit0( RCMMANAGER_OPENLC_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManager::Close()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::Close()
    {
    OstTraceFunctionEntry0( RCMMANAGER_CLOSE_ENTRY );

    if ( iCmManagerApi )
        {
        iCmManagerApi->Close();
        delete iCmManagerApi;
        iCmManagerApi = NULL;
        }

    OstTraceFunctionExit0( RCMMANAGER_CLOSE_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManager::CreateTablesAndOpenL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::CreateTablesAndOpenL()
    {
    OstTraceFunctionEntry0( RCMMANAGER_CREATETABLESANDOPENL_ENTRY );
    OpenL();
    OstTraceFunctionExit0( RCMMANAGER_CREATETABLESANDOPENL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManager::ConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethod RCmManager::ConnectionMethodL(
        TUint32 aConnectionMethodId ) const
    {
    OstTraceFunctionEntry0( RCMMANAGER_CONNECTIONMETHODL_ENTRY );

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

    RCmConnectionMethod cm;
    cm.iCmConnectionMethodWrapper = cmWrapper;

    OstTraceFunctionExit0( DUP1_RCMMANAGER_CONNECTIONMETHODL_EXIT );
    return cm;
    }

//-----------------------------------------------------------------------------
//  RCmManager::GetBearerInfoIntL()
//-----------------------------------------------------------------------------
//
EXPORT_C TUint32 RCmManager::GetBearerInfoIntL(
        TUint32 aBearerType,
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMMANAGER_GETBEARERINFOINTL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    TUint32 value( 0 );
    TInt err = iCmManagerApi->GetBearerInfoInt( aBearerType, aAttribute, value );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMMANAGER_GETBEARERINFOINTL_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmManager::GetBearerInfoBoolL()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool RCmManager::GetBearerInfoBoolL(
        TUint32 aBearerType,
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMMANAGER_GETBEARERINFOBOOLL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    TBool value( 0 );
    TInt err = iCmManagerApi->GetBearerInfoBool( aBearerType, aAttribute, value );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMMANAGER_GETBEARERINFOBOOLL_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmManager::GetBearerInfoStringL()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC* RCmManager::GetBearerInfoStringL(
        TUint32 aBearerType,
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMMANAGER_GETBEARERINFOSTRINGL_ENTRY );

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
        buffer = KNullDesC().AllocL();
        }

    OstTraceFunctionExit0( RCMMANAGER_GETBEARERINFOSTRINGL_EXIT );
    return buffer;
    }

//-----------------------------------------------------------------------------
//  RCmManager::GetBearerInfoString8L()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC8* RCmManager::GetBearerInfoString8L(
        TUint32 aBearerType,
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMMANAGER_GETBEARERINFOSTRING8L_ENTRY );

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
        buffer8 = KNullDesC8().AllocL();
        }

    OstTraceFunctionExit0( RCMMANAGER_GETBEARERINFOSTRING8L_EXIT );
    return buffer8;
    }

//-----------------------------------------------------------------------------
//  RCmManager::GetConnectionMethodInfoIntL()
//-----------------------------------------------------------------------------
//
EXPORT_C TUint32 RCmManager::GetConnectionMethodInfoIntL(
        TUint32 aIapId,
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMMANAGER_GETCONNECTIONMETHODINFOINTL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    TUint32 value( 0 );
    TInt err = iCmManagerApi->GetConnectionMethodInfoInt( aIapId, aAttribute, value );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMMANAGER_GETCONNECTIONMETHODINFOINTL_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmManager::GetConnectionMethodInfoBoolL()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool RCmManager::GetConnectionMethodInfoBoolL(
        TUint32 aIapId,
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMMANAGER_GETCONNECTIONMETHODINFOBOOLL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    TBool value( 0 );
    TInt err = iCmManagerApi->GetConnectionMethodInfoBool( aIapId, aAttribute, value );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMMANAGER_GETCONNECTIONMETHODINFOBOOLL_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmManager::GetConnectionMethodInfoStringL()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC* RCmManager::GetConnectionMethodInfoStringL(
        TUint32 aIapId,
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMMANAGER_GETCONNECTIONMETHODINFOSTRINGL_ENTRY );

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
        buffer = KNullDesC().AllocL();
        }

    OstTraceFunctionExit0( RCMMANAGER_GETCONNECTIONMETHODINFOSTRINGL_EXIT );
    return buffer;
    }

//-----------------------------------------------------------------------------
//  RCmManager::GetConnectionMethodInfoString8L()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC8* RCmManager::GetConnectionMethodInfoString8L(
        TUint32 aIapId,
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( RCMMANAGER_GETCONNECTIONMETHODINFOSTRING8L_ENTRY );

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
        buffer8 = KNullDesC8().AllocL();
        }

    OstTraceFunctionExit0( RCMMANAGER_GETCONNECTIONMETHODINFOSTRING8L_EXIT );
    return buffer8;
    }

//-----------------------------------------------------------------------------
//  RCmManager::CreateDestinationL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmDestination RCmManager::CreateDestinationL(
        const TDesC& aName )
    {
    OstTraceFunctionEntry0( RCMMANAGER_CREATEDESTINATIONL_ENTRY );

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

    RCmDestination dest;
    dest.iCmDestinationWrapper = destWrap;

    OstTraceFunctionExit0( RCMMANAGER_CREATEDESTINATIONL_EXIT );
    return dest;
    }

//-----------------------------------------------------------------------------
//  RCmManager::CreateConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethod RCmManager::CreateConnectionMethodL(
        TUint32 aBearerType )
    {
    OstTraceFunctionEntry0( RCMMANAGER_CREATECONNECTIONMETHODL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    CCmConnectionMethodWrapper* cmWrapper = CCmConnectionMethodWrapper::NewLC();
    cmWrapper->CreateConnMethodL( iCmManagerApi, aBearerType );
    CleanupStack::Pop( cmWrapper );

    RCmConnectionMethod cm;
    cm.iCmConnectionMethodWrapper = cmWrapper;

    OstTraceFunctionExit0( RCMMANAGER_CREATECONNECTIONMETHODL_EXIT );
    return cm;
    }

//-----------------------------------------------------------------------------
//  RCmManager::CopyConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C TInt RCmManager::CopyConnectionMethodL(
        RCmDestination& aTargetDestination,
        RCmConnectionMethod& aConnectionMethod )
    {
    OstTraceFunctionEntry0( RCMMANAGER_COPYCONNECTIONMETHODL_ENTRY );

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

    OstTraceFunctionExit0( RCMMANAGER_COPYCONNECTIONMETHODL_EXIT );
    return index;
    }

//-----------------------------------------------------------------------------
//  RCmManager::MoveConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C TInt RCmManager::MoveConnectionMethodL(
        RCmDestination& aSourceDestination,
        RCmDestination& aTargetDestination,
        RCmConnectionMethod& aConnectionMethod )
    {
    OstTraceFunctionEntry0( RCMMANAGER_MOVECONNECTIONMETHODL_ENTRY );

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

    OstTraceFunctionExit0( RCMMANAGER_MOVECONNECTIONMETHODL_EXIT );
    return pkgData.iIndex;
    }

//-----------------------------------------------------------------------------
//  RCmManager::RemoveConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::RemoveConnectionMethodL(
        RCmDestination& aDestination,
        RCmConnectionMethod& aConnectionMethod )
    {
    OstTraceFunctionEntry0( RCMMANAGER_REMOVECONNECTIONMETHODL_ENTRY );

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

    OstTraceFunctionExit0( RCMMANAGER_REMOVECONNECTIONMETHODL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManager::RemoveAllReferencesL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::RemoveAllReferencesL(
        RCmConnectionMethod& aConnectionMethod )
    {
    OstTraceFunctionEntry0( RCMMANAGER_REMOVEALLREFERENCESL_ENTRY );

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

    OstTraceFunctionExit0( RCMMANAGER_REMOVEALLREFERENCESL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManager::ConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::ConnectionMethodL(
        RArray<TUint32>& aCmArray,
        TBool aCheckBearerType,
        TBool aLegacyOnly,
        TBool aEasyWlan ) const
    {
    OstTraceFunctionEntry0( DUP1_RCMMANAGER_CONNECTIONMETHODL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    TCmmIpcStructGetConnMethods parameters;
    parameters.iCheckBearerType = aCheckBearerType;
    parameters.iLegacyOnly = aLegacyOnly;
    parameters.iEasyWlan = aEasyWlan;

    iCmManagerApi->GetConnMethodsL( parameters, aCmArray );

    OstTraceFunctionExit0( RCMMANAGER_CONNECTIONMETHODL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManager::AllDestinationsL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::AllDestinationsL( RArray<TUint32>& aDestArray ) const
    {
    OstTraceFunctionEntry0( RCMMANAGER_ALLDESTINATIONSL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    iCmManagerApi->GetAllDestinationsL( aDestArray );

    OstTraceFunctionExit0( RCMMANAGER_ALLDESTINATIONSL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManager::DestinationL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmDestination RCmManager::DestinationL( TUint32 aDestinationId ) const
    {
    OstTraceFunctionEntry0( RCMMANAGER_DESTINATIONL_ENTRY );

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

    RCmDestination dest;
    dest.iCmDestinationWrapper = destWrap;

    return dest;
    }

//-----------------------------------------------------------------------------
//  RCmManager::BearerPriorityArrayL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::BearerPriorityArrayL(
        RArray<TBearerPriority>& aArray ) const
    {
    OstTraceFunctionEntry0( RCMMANAGER_BEARERPRIORITYARRAYL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    iCmManagerApi->GetBearerPriorityArrayL( aArray );

    OstTraceFunctionExit0( RCMMANAGER_BEARERPRIORITYARRAYL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManager::UpdateBearerPriorityArrayL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::UpdateBearerPriorityArrayL(
        const RArray<TBearerPriority>& aArray )
    {
    OstTraceFunctionEntry0( RCMMANAGER_UPDATEBEARERPRIORITYARRAYL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    iCmManagerApi->SetBearerPriorityArrayL( aArray );

    OstTraceFunctionExit0( RCMMANAGER_UPDATEBEARERPRIORITYARRAYL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManager::CleanupGlobalPriorityArray()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::CleanupGlobalPriorityArray(
        RArray<TBearerPriority>& aArray ) const
    {
    OstTraceFunctionEntry0( RCMMANAGER_CLEANUPGLOBALPRIORITYARRAY_ENTRY );

    for ( TInt i = 0; i < aArray.Count(); i++ )
        {
        delete aArray[i].iServiceType;
        }
    aArray.Reset();
    aArray.Close();

    OstTraceFunctionExit0( RCMMANAGER_CLEANUPGLOBALPRIORITYARRAY_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManager::SupportedBearersL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::SupportedBearersL( RArray<TUint32>& aArray ) const
    {
    OstTraceFunctionEntry0( RCMMANAGER_SUPPORTEDBEARERSL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }
    iCmManagerApi->GetSupportedBearersL( aArray );

    OstTraceFunctionExit0( RCMMANAGER_SUPPORTEDBEARERSL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManager::EasyWlanIdL()
//-----------------------------------------------------------------------------
//
EXPORT_C TUint32 RCmManager::EasyWlanIdL() const
    {
    OstTraceFunctionEntry0( RCMMANAGER_EASYWLANIDL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    TUint32 value( 0 );
    TInt err = iCmManagerApi->GetEasyWlanId( value );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMMANAGER_EASYWLANIDL_EXIT );
    return value;
    }

//-----------------------------------------------------------------------------
//  RCmManager::ReadDefConnL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::ReadDefConnL(
        TCmDefConnValue& aDefConnSetting )
    {
    OstTraceFunctionEntry0( RCMMANAGER_READDEFCONNL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmManagerApi->ReadDefaultConnection( aDefConnSetting );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMMANAGER_READDEFCONNL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManager::WriteDefConnL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::WriteDefConnL(
        const TCmDefConnValue& aDefConnSetting )
    {
    OstTraceFunctionEntry0( RCMMANAGER_WRITEDEFCONNL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmManagerApi->WriteDefaultConnection( aDefConnSetting );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMMANAGER_WRITEDEFCONNL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManager::ReadGenConnSettingsL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::ReadGenConnSettingsL(
        TCmGenConnSettings& aGenConnSettings )
    {
    OstTraceFunctionEntry0( RCMMANAGER_READGENCONNSETTINGSL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmManagerApi->ReadGeneralConnectionSettings( aGenConnSettings );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMMANAGER_READGENCONNSETTINGSL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManager::WriteGenConnSettingsL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::WriteGenConnSettingsL(
        const TCmGenConnSettings& aGenConnSettings )
    {
    OstTraceFunctionEntry0( RCMMANAGER_WRITEGENCONNSETTINGSL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    TInt err = iCmManagerApi->WriteGeneralConnectionSettings( aGenConnSettings );
    User::LeaveIfError( err );

    OstTraceFunctionExit0( RCMMANAGER_WRITEGENCONNSETTINGSL_EXIT );
    }

//-----------------------------------------------------------------------------
//  RCmManager::CreateDestinationL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmDestination RCmManager::CreateDestinationL(
        const TDesC& aName,
        TUint32 aDestId )
    {
    OstTraceFunctionEntry0( DUP1_RCMMANAGER_CREATEDESTINATIONL_ENTRY );

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

    RCmDestination dest;
    dest.iCmDestinationWrapper = destWrap;

    OstTraceFunctionExit0( DUP1_RCMMANAGER_CREATEDESTINATIONL_EXIT );
    return dest;
    }

//-----------------------------------------------------------------------------
//  RCmManager::CreateConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethod RCmManager::CreateConnectionMethodL(
        TUint32 aBearerType,
        TUint32 aConnMethodId )
    {
    OstTraceFunctionEntry0( DUP1_RCMMANAGER_CREATECONNECTIONMETHODL_ENTRY );

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

    RCmConnectionMethod cm;
    cm.iCmConnectionMethodWrapper = cmWrapper;

    OstTraceFunctionExit0( DUP1_RCMMANAGER_CREATECONNECTIONMETHODL_EXIT );
    return cm;
    }

//-----------------------------------------------------------------------------
//  RCmManager::GetUncategorizedIconL()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC* RCmManager::GetUncategorizedIconL() const
    {
    OstTraceFunctionEntry0( RCMMANAGER_GETUNCATEGORIZEDICONL_ENTRY );

    if ( !iCmManagerApi )
        {
        User::Leave( KErrBadHandle );
        }

    HBufC* buffer = HBufC::NewLC( KCmmStringLengthMax );
    TInt err = iCmManagerApi->GetUncategorizedIcon( buffer );
    User::LeaveIfError( err );

    if ( buffer->Length() > 0 )
        {
        CleanupStack::Pop( buffer );
        }
    else
        {
        CleanupStack::PopAndDestroy( buffer );
        buffer = KNullDesC().AllocL();
        }

    OstTraceFunctionExit0( RCMMANAGER_GETUNCATEGORIZEDICONL_EXIT );
    return buffer;
    }

// End of file
