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


#include <cmmanagerdef.h>

#include "cmmserverdefs.h"
#include "cmmanagerapi.h"
#include "cmdestinationwrapper.h"
#include "cmconnectionmethodwrapper.h"
#include "cmmclistatic.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmanagerapiTraces.h"
#endif


RCmManagerApi::RCmManagerApi() : RSessionBase()
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_RCMMANAGERAPI_ENTRY );

    iConnected = EFalse;
    iSmallBufsForDestinations = ETrue;
    iSmallBufsForConnMethods = ETrue;

    OstTraceFunctionExit0( RCMMANAGERAPI_RCMMANAGERAPI_EXIT );
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::Connect()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::Connect()
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_CONNECT_ENTRY );

    if ( !iConnected )
        {
        TInt retry = 2;
        for (;;)
            {
            TInt err = CreateSession( KCmmServer, TVersion( 0, 0, 0 ), KCmmDefaultMessageSlots );

            if ( err != KErrNotFound && err != KErrServerTerminated )
                {
                if ( !err )
                    {
                    iConnected = ETrue;
                    }
                OstTraceFunctionExit0( RCMMANAGERAPI_CONNECT_EXIT );
                return err;
                }
            // KErrNotFound and KErrServerTerminated will go through.
            if ( --retry <= 0 )
                {
                OstTraceFunctionExit0( DUP1_RCMMANAGERAPI_CONNECT_EXIT );
                return err;
                }
            err = TCmManagerLauncher::LaunchServer(
                    KCmmServerFile,
                    KCmmUid3 );

            if ( err != KErrNone && err != KErrAlreadyExists )
                {
                // Failed to launch server, and it wasn't already running.
                OstTraceFunctionExit0( DUP2_RCMMANAGERAPI_CONNECT_EXIT );
                return err;
                }
            }
        }

    OstTraceFunctionExit0( DUP3_RCMMANAGERAPI_CONNECT_EXIT );
    return KErrNone;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::Close()
//-----------------------------------------------------------------------------
//
void RCmManagerApi::Close()
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_CLOSE_ENTRY );

    if ( iConnected )
        {
        // Close all subsessions first.
        //
        // Don't call subsession destructors (wrappers), since they will try to
        // remove themselves from these pointer arrays. Here we can just close
        // the arrays at the end.
        for ( TInt i = 0; i < iConnMethods.Count(); i++ )
            {
            iConnMethods[i]->CloseSession();
            }
        iConnMethods.Close();
        for ( TInt i = 0; i < iDestinations.Count(); i++ )
            {
            iDestinations[i]->CloseSession();
            }
        iDestinations.Close();

        RSessionBase::Close();
        iConnected = EFalse;
        }

    OstTraceFunctionExit0( RCMMANAGERAPI_CLOSE_EXIT );
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::StoreDestinationWrapper()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::StoreDestinationWrapper(
        CCmDestinationWrapper* aDestinationWrapper )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_STOREDESTINATIONWRAPPER_ENTRY );

    TInt err = iDestinations.InsertInAddressOrder( aDestinationWrapper );

    OstTraceFunctionExit0( RCMMANAGERAPI_STOREDESTINATIONWRAPPER_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::RemoveDestinationWrapper()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::RemoveDestinationWrapper(
        CCmDestinationWrapper* aDestinationWrapper )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_REMOVEDESTINATIONPOINTER_ENTRY );

    TInt index = iDestinations.FindInAddressOrder( aDestinationWrapper );
    if ( index >= 0 )
        {
        iDestinations.Remove( index );
        index = KErrNone;
        }

    OstTraceFunctionExit0( RCMMANAGERAPI_REMOVEDESTINATIONPOINTER_EXIT );
    return index;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::GetDestinationWrapperL()
//-----------------------------------------------------------------------------
//
CCmDestinationWrapper* RCmManagerApi::GetDestinationWrapperL( const TInt aHandle )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_GETDESTINATIONWRAPPER_ENTRY );

    CCmDestinationWrapper* result( NULL );
    if ( aHandle )
        {
        for ( TInt i = 0; i < iDestinations.Count(); i++ )
            {
            if ( iDestinations[i]->GetHandle() == aHandle )
                {
                result = iDestinations[i];
                break;
                }
            }
        }
    if ( !result )
        {
        User::Leave( KErrCorrupt );
        }

    OstTraceFunctionExit0( RCMMANAGERAPI_GETDESTINATIONWRAPPER_EXIT );
    return result;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::StoreConnMethodWrapper()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::StoreConnMethodWrapper(
        CCmConnectionMethodWrapper* aConnMethodWrapper )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_STORECONNMETHODWRAPPER_ENTRY );

    TInt err = iConnMethods.InsertInAddressOrder( aConnMethodWrapper );

    OstTraceFunctionExit0( RCMMANAGERAPI_STORECONNMETHODWRAPPER_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::RemoveConnMethodWrapper()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::RemoveConnMethodWrapper(
        CCmConnectionMethodWrapper* aConnMethodWrapper )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_REMOVECONNMETHODPOINTER_ENTRY );

    TInt index = iConnMethods.FindInAddressOrder( aConnMethodWrapper );
    if ( index >= 0 )
        {
        iConnMethods.Remove( index );
        index = KErrNone;
        }

    OstTraceFunctionExit0( RCMMANAGERAPI_REMOVECONNMETHODPOINTER_EXIT );
    return index;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::GetConnMethodWrapperL()
//-----------------------------------------------------------------------------
//
CCmConnectionMethodWrapper* RCmManagerApi::GetConnMethodWrapperL( const TInt aHandle )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_GETCONNMETHODWRAPPER_ENTRY );

    CCmConnectionMethodWrapper* result( NULL );
    if ( aHandle )
        {
        for ( TInt i = 0; i < iConnMethods.Count(); i++ )
            {
            if ( iConnMethods[i]->GetHandle() == aHandle )
                {
                result = iConnMethods[i];
                break;
                }
            }
        }
    if ( !result )
        {
        User::Leave( KErrCorrupt );
        }

    OstTraceFunctionExit0( RCMMANAGERAPI_GETCONNMETHODWRAPPER_EXIT );
    return result;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::GetBearerInfoInt()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::GetBearerInfoInt(
        const TUint32 aBearerType,
        const TUint32 aAttribute,
        TUint32& aValue )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_GETBEARERINFOINT_ENTRY );

    TPckg<TUint32> pckg( aValue );
    TIpcArgs args( aBearerType, aAttribute, &pckg );
    TInt err = SendReceive( ECmmGetBearerInfoInt, args );

    OstTraceFunctionExit0( RCMMANAGERAPI_GETBEARERINFOINT_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::GetBearerInfoBool()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::GetBearerInfoBool(
        const TUint32 aBearerType,
        const TUint32 aAttribute,
        TBool& aValue )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_GETBEARERINFOBOOL_ENTRY );

    TPckg<TBool> pckg( aValue );
    TIpcArgs args( aBearerType, aAttribute, &pckg );
    TInt err = SendReceive( ECmmGetBearerInfoBool, args );

    OstTraceFunctionExit0( RCMMANAGERAPI_GETBEARERINFOBOOL_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::GetBearerInfoString()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::GetBearerInfoString(
        const TUint32 aBearerType,
        const TUint32 aAttribute,
        HBufC* aBuffer )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_GETBEARERINFOSTRING_ENTRY );

    TPtr descriptor( aBuffer->Des() );
    TIpcArgs args( aBearerType, aAttribute, &descriptor );
    TInt err = SendReceive( ECmmGetBearerInfoString, args );

    OstTraceFunctionExit0( RCMMANAGERAPI_GETBEARERINFOSTRING_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::GetBearerInfoString8()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::GetBearerInfoString8(
        const TUint32 aBearerType,
        const TUint32 aAttribute,
        HBufC8* aBuffer8 )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_GETBEARERINFOSTRING8_ENTRY );

    TPtr8 descriptor( aBuffer8->Des() );
    TIpcArgs args( aBearerType, aAttribute, &descriptor );
    TInt err = SendReceive( ECmmGetBearerInfoString8, args );

    OstTraceFunctionExit0( RCMMANAGERAPI_GETBEARERINFOSTRING8_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::GetConnectionMethodInfoInt()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::GetConnectionMethodInfoInt(
        const TUint32 aIapId,
        const TUint32 aAttribute,
        TUint32& aValue )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_GETCONNECTIONMETHODINFOINT_ENTRY );

    TPckg<TUint32> pckg( aValue );
    TIpcArgs args( aIapId, aAttribute, &pckg );
    TInt err = SendReceive( ECmmGetConnMethodInfoInt, args );

    OstTraceFunctionExit0( RCMMANAGERAPI_GETCONNECTIONMETHODINFOINT_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::GetConnectionMethodInfoBool()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::GetConnectionMethodInfoBool(
        const TUint32 aIapId,
        const TUint32 aAttribute,
        TBool& aValue )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_GETCONNECTIONMETHODINFOBOOL_ENTRY );

    TPckg<TUint32> pckg( aValue );
    TIpcArgs args( aIapId, aAttribute, &pckg );
    TInt err = SendReceive( ECmmGetConnMethodInfoBool, args );

    OstTraceFunctionExit0( RCMMANAGERAPI_GETCONNECTIONMETHODINFOBOOL_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::GetConnectionMethodInfoString()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::GetConnectionMethodInfoString(
        const TUint32 aIapId,
        const TUint32 aAttribute,
        HBufC* aBuffer )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_GETCONNECTIONMETHODINFOSTRING_ENTRY );

    TPtr descriptor( aBuffer->Des() );
    TIpcArgs args( aIapId, aAttribute, &descriptor );
    TInt err = SendReceive( ECmmGetConnMethodInfoString, args );

    OstTraceFunctionExit0( RCMMANAGERAPI_GETCONNECTIONMETHODINFOSTRING_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::GetConnectionMethodInfoString8()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::GetConnectionMethodInfoString8(
        const TUint32 aIapId,
        const TUint32 aAttribute,
        HBufC8* aBuffer8 )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_GETCONNECTIONMETHODINFOSTRING8_ENTRY );

    TPtr8 descriptor( aBuffer8->Des() );
    TIpcArgs args( aIapId, aAttribute, &descriptor );
    TInt err = SendReceive( ECmmGetConnMethodInfoString8, args );

    OstTraceFunctionExit0( RCMMANAGERAPI_GETCONNECTIONMETHODINFOSTRING8_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::GetConnMethodsL(
//-----------------------------------------------------------------------------
//
void RCmManagerApi::GetConnMethodsL(
        const TCmmIpcStructGetConnMethods& aParameters,
        RArray<TUint32>& aCmArray )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_GETCONNMETHODSL_ENTRY );

    TInt err( KErrNone );
    TInt count( KMaxTInt );
    TPckg<TInt> countPckg( count );
    TPckgBuf<TCmmIpcStructGetConnMethods> parametersPckg( aParameters );

    HBufC8* smallBuf( NULL );
    HBufC8* bigBuf( NULL );

    if ( iSmallBufsForConnMethods )
        {
        smallBuf = HBufC8::NewLC( KCmmConnMethodAmountNormal );
        TPtr8 smallBufPtr( smallBuf->Des() );

        TIpcArgs args( &parametersPckg, &countPckg, &smallBufPtr );
        err = SendReceive( ECmmGetConnMethodArray, args );
        User::LeaveIfError( err );
        }

    if ( countPckg() <= KCmmConnMethodAmountNormal )
        {
        aCmArray.Reset();
        aCmArray.ReserveL( countPckg() );
        for ( TInt i = 0; i < countPckg(); i++ )
            {
            aCmArray.AppendL( (TUint32)(*smallBuf)[i] );
            }
        }
    else if ( countPckg() == KMaxTInt )
        {
        // All connection methods didn't fit into small array, ask again with big array.
        iSmallBufsForConnMethods = EFalse; // Switch small buffer mode off.
        if ( smallBuf )
            {
            CleanupStack::PopAndDestroy( smallBuf );
            smallBuf = NULL;
            }

        bigBuf = HBufC8::NewLC( KCmmConnMethodAmountMax );
        TPtr8 bigBufPtr( bigBuf->Des() );

        TIpcArgs args( &parametersPckg, &countPckg, &bigBufPtr );
        err = SendReceive( ECmmGetConnMethodArray, args );
        User::LeaveIfError( err );

        if ( countPckg() <= KCmmConnMethodAmountMax )
            {
            // Don't switch back to small buffer mode if client was only asking
            // for legacy connection methods (likely a small list).
            if ( countPckg() < KCmmConnMethodAmountNormal && aParameters.iLegacyOnly == EFalse )
                {
                iSmallBufsForConnMethods = ETrue; // Switch small buffer mode back on.
                }

            aCmArray.Reset();
            aCmArray.ReserveL( countPckg() );
            for ( TInt i = 0; i < countPckg(); i++ )
                {
                aCmArray.AppendL( (TUint32)(*smallBuf)[i] );
                }
            }
        else
            {
            User::Leave( KErrUnknown );
            }
        CleanupStack::PopAndDestroy( bigBuf );
        }
    else
        {
        User::Leave( KErrUnknown );
        }

    if ( smallBuf )
        {
        CleanupStack::PopAndDestroy( smallBuf );
        }

    OstTraceFunctionExit0( RCMMANAGERAPI_GETCONNMETHODSL_EXIT );
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::GetAllDestinationsL()
//-----------------------------------------------------------------------------
//
void RCmManagerApi::GetAllDestinationsL( RArray<TUint32>& aDestArray )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_GETALLDESTINATIONSL_ENTRY );

    TInt err( KErrNone );
    TInt count( KMaxTInt );
    TPckg<TInt> countPckg( count );

    HBufC16* smallBuf( NULL );
    HBufC16* bigBuf( NULL );

    if ( iSmallBufsForDestinations )
        {
        smallBuf = HBufC16::NewLC( KCmmDestAmountNormal );
        TPtr16 smallBufPtr( smallBuf->Des() );

        TIpcArgs args( &countPckg, &smallBufPtr );
        err = SendReceive( ECmmGetAllDestinations, args );
        User::LeaveIfError( err );
        }

    if ( countPckg() <= KCmmDestAmountNormal )
        {
        aDestArray.Reset();
        aDestArray.ReserveL( countPckg() );
        for ( TInt i = 0; i < countPckg(); i++ )
            {
            aDestArray.AppendL( (TUint32)(*smallBuf)[i] );
            }
        }
    else if ( countPckg() == KMaxTInt )
        {
        // All destinations didn't fit into small array, ask again with big array.
        iSmallBufsForDestinations = EFalse; // Switch small buffer mode off.
        if ( smallBuf )
            {
            CleanupStack::PopAndDestroy( smallBuf );
            smallBuf = NULL;
            }

        bigBuf = HBufC16::NewLC( KCmmDestAmountMax );
        TPtr16 bigBufPtr( bigBuf->Des() );

        TIpcArgs args( &countPckg, &bigBufPtr );
        err = SendReceive( ECmmGetAllDestinations, args );
        User::LeaveIfError( err );

        if ( countPckg() <= KCmmDestAmountMax )
            {
            if ( countPckg() < KCmmDestAmountNormal )
                {
                iSmallBufsForDestinations = ETrue; // Switch small buffer mode back on.
                }

            aDestArray.Reset();
            aDestArray.ReserveL( countPckg() );
            for ( TInt i = 0; i < countPckg(); i++ )
                {
                aDestArray.AppendL( (TUint32)(*smallBuf)[i] );
                }
            }
        else
            {
            User::Leave( KErrUnknown );
            }
        CleanupStack::PopAndDestroy( bigBuf );
        }
    else
        {
        User::Leave( KErrUnknown );
        }

    if ( smallBuf )
        {
        CleanupStack::PopAndDestroy( smallBuf );
        }

    OstTraceFunctionExit0( RCMMANAGERAPI_GETALLDESTINATIONSL_EXIT );
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::GetBearerPriorityArrayL()
//-----------------------------------------------------------------------------
//
void RCmManagerApi::GetBearerPriorityArrayL( RArray<TBearerPriority>& aArray )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_GETBEARERPRIORITYARRAYL_ENTRY );

    HBufC* buffer = HBufC::NewLC( KCmmDefaultBearerPriorityArraySize );
    TPtr bufferPtr( buffer->Des() );

    TIpcArgs args( &bufferPtr );
    TInt err = SendReceive( ECmmGetBearerPriorityArray, args );
    User::LeaveIfError( err );

    // bufferPtr[1] should now contain the needed buffer length.
    if ( bufferPtr[1] > bufferPtr.Length() )
        {
        // First buffer wasn't big enough. Try again with correct size buffer.
        CleanupStack::PopAndDestroy( buffer );
        buffer = HBufC::NewLC( bufferPtr[1] );
        bufferPtr.Set( buffer->Des() );

        TIpcArgs args2( &bufferPtr );
        err = SendReceive( ECmmGetBearerPriorityArray, args2 );
        User::LeaveIfError( err );

        if ( bufferPtr[1] > bufferPtr.Length() )
            {
            User::Leave( KErrAbort );
            }
        }

    const TInt bearerCount = bufferPtr[0];

    // Cleanup old array. There's a pointer in that T-class...
    for ( TInt i = 0; i < aArray.Count(); i++ )
        {
        if ( aArray[i].iServiceType )
            {
            delete aArray[i].iServiceType;
            aArray[i].iServiceType = NULL;
            }
        }
    aArray.Reset();
    aArray.ReserveL( bearerCount );

    TInt position( 2 ); // Start of first priority item
    for ( TInt i = 0; i < bearerCount; i++ )
        {
        TBearerPriority bearerPriority;
        bearerPriority.iPriority = bufferPtr[position++] << KBitsInTwoBytes;
        bearerPriority.iPriority += bufferPtr[position++];
        bearerPriority.iUIPriority = bufferPtr[position++] << KBitsInTwoBytes;
        bearerPriority.iUIPriority += bufferPtr[position++];
        bearerPriority.iServiceType = NULL;

        const TInt stringLength = bufferPtr[position++];
        if ( stringLength )
            {
            HBufC* serviceName = HBufC::NewL( stringLength );
            serviceName->Des().Append( &(bufferPtr[position]), stringLength );
            position += stringLength;
            bearerPriority.iServiceType = serviceName;
            // Ownership moved into array.
            }
        aArray.AppendL( bearerPriority );
        }

    CleanupStack::PopAndDestroy( buffer );

    OstTraceFunctionExit0( RCMMANAGERAPI_GETBEARERPRIORITYARRAYL_EXIT );
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::SetBearerPriorityArrayL()
//-----------------------------------------------------------------------------
//
void RCmManagerApi::SetBearerPriorityArrayL( const RArray<TBearerPriority>& aArray )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_SETBEARERPRIORITYARRAYL_ENTRY );

    // iPriority:           2 16-bit values,
    // iUIPriority:         2 16-bit values,
    // iServiceType length: 1 16-bit value.
    const TInt KCmmBearerPriorityHeaderLength = 5;

    // Buffer starts with legth of 2, to contain both the total amount of
    // TBearerPriority objects, and the size of the buffer (number of 16-bit
    // values) needed to contain the whole structure including all headers.
    TInt bufferLength( 2 );

    const TInt bearerCount = aArray.Count();
    for ( TInt i = 0; i < bearerCount; i++ )
        {
        bufferLength += KCmmBearerPriorityHeaderLength;
        const TBearerPriority& bearerPriority = aArray[i];
        if ( !bearerPriority.iServiceType )
            {
            User::Leave( KErrArgument );
            }
        // Get length of service type name and add it to needed buffer length.
        bufferLength += bearerPriority.iServiceType->Length();
        }

    HBufC* buffer = HBufC::NewLC( bufferLength );
    TPtr bufferPtr( buffer->Des() );

    bufferPtr.Append( bearerCount );
    bufferPtr.Append( bufferLength );

    TInt position = bufferPtr.Length();
    for ( TInt i = 0; i < bearerCount; i++ )
        {
        const TBearerPriority& bearerPriority = aArray[i];

        bufferPtr.Append( ( bearerPriority.iPriority & 0xFFFF0000 ) >> KBitsInTwoBytes );
        bufferPtr.Append( ( bearerPriority.iPriority & 0x0000FFFF ) );
        bufferPtr.Append( ( bearerPriority.iUIPriority & 0xFFFF0000 ) >> KBitsInTwoBytes );
        bufferPtr.Append( ( bearerPriority.iUIPriority & 0x0000FFFF ) );

        position += KCmmBearerPriorityHeaderLength;
        if ( !bearerPriority.iServiceType )
            {
            User::Leave( KErrArgument );
            }
        const TInt stringLength = bearerPriority.iServiceType->Length();
        bufferPtr.Append( stringLength );
        bufferPtr.Insert( position, *(bearerPriority.iServiceType) );
        position += stringLength;
        }

    TIpcArgs args( &bufferPtr );
    TInt err = SendReceive( ECmmUpdateBearerPriorityArray, args );
    User::LeaveIfError( err );
    CleanupStack::PopAndDestroy( buffer );

    OstTraceFunctionExit0( RCMMANAGERAPI_SETBEARERPRIORITYARRAYL_EXIT );
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::GetSupportedBearersL()
//-----------------------------------------------------------------------------
//
void RCmManagerApi::GetSupportedBearersL( RArray<TUint32>& aArray )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_GETSUPPORTEDBEARERSL_ENTRY );

    TInt err( KErrNone );
    TInt count( KMaxTInt );
    TPckg<TInt> countPckg( count );

    HBufC8* smallBuf( NULL );
    HBufC8* bigBuf( NULL );

    smallBuf = HBufC8::NewLC( KCmmBearerAmountNormal * sizeof( TUint32 ) );
    TPtr8 smallBufPtr( smallBuf->Des() );

    TIpcArgs args( &countPckg, &smallBufPtr );
    err = SendReceive( ECmmGetSupportedBearers, args );
    User::LeaveIfError( err );

    if ( countPckg() <= KCmmBearerAmountNormal )
        {
        aArray.Reset();
        aArray.ReserveL( countPckg() );

        TUint32 a( 0 );
        TPtr8 ptr = smallBuf->Des();
        for ( TInt i = 0; i < countPckg(); i++ )
            {
            a = 0;
            a += (TUint32)ptr[0 + ( i * sizeof( TUint32 ) )];
            a += (TUint32)ptr[1 + ( i * sizeof( TUint32 ) )] << KBitsInOneByte;
            a += (TUint32)ptr[2 + ( i * sizeof( TUint32 ) )] << KBitsInTwoBytes;
            a += (TUint32)ptr[3 + ( i * sizeof( TUint32 ) )] << KBitsInThreeBytes;
            aArray.AppendL( a );
            }
        }
    else
        {
        TInt storedCount = countPckg();

        bigBuf = HBufC8::NewLC( countPckg() * sizeof( TUint32 ) );
        TPtr8 bigBufPtr( bigBuf->Des() );

        TIpcArgs args( &countPckg, &bigBufPtr );
        err = SendReceive( ECmmGetSupportedBearers, args );
        User::LeaveIfError( err );

        if ( countPckg() <= storedCount )
            {
            aArray.Reset();
            aArray.ReserveL( countPckg() );

            TUint32 a( 0 );
            TPtr8 ptr = bigBuf->Des();
            for ( TInt i = 0; i < countPckg(); i++ )
                {
                a = 0;
                a += (TUint32)ptr[0 + ( i * sizeof( TUint32 ) )];
                a += (TUint32)ptr[1 + ( i * sizeof( TUint32 ) )] << KBitsInOneByte;
                a += (TUint32)ptr[2 + ( i * sizeof( TUint32 ) )] << KBitsInTwoBytes;
                a += (TUint32)ptr[3 + ( i * sizeof( TUint32 ) )] << KBitsInThreeBytes;
                aArray.AppendL( a );
                }
            }
        else
            {
            User::Leave( KErrUnknown );
            }

        CleanupStack::PopAndDestroy( bigBuf );
        }
    CleanupStack::PopAndDestroy( smallBuf );

    OstTraceFunctionExit0( RCMMANAGERAPI_GETSUPPORTEDBEARERSL_EXIT );
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::GetEasyWlanId()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::GetEasyWlanId( TUint32& aValue )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_GETEASYWLANID_ENTRY );

    TPckg<TUint32> pckg( aValue );
    TIpcArgs args( &pckg );
    TInt err = SendReceive( ECmmGetEasyWlanId, args );

    OstTraceFunctionExit0( RCMMANAGERAPI_GETEASYWLANID_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::ReadDefaultConnection()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::ReadDefaultConnection(
        TCmDefConnValue& aDefConnSetting )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_READDEFAULTCONNECTION_ENTRY );

    TPckgBuf<TCmDefConnValue> pkg( aDefConnSetting );
    TIpcArgs args( &pkg );
    TInt err = SendReceive( ECmmReadDefaultConnection, args );
    aDefConnSetting = pkg();

    OstTraceFunctionExit0( RCMMANAGERAPI_READDEFAULTCONNECTION_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::WriteDefaultConnection()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::WriteDefaultConnection(
        const TCmDefConnValue& aDefConnSetting )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_WRITEDEFAULTCONNECTION_ENTRY );

    TPckgBuf<TCmDefConnValue> pkg( aDefConnSetting );
    TIpcArgs args( &pkg );
    TInt err = SendReceive( ECmmWriteDefaultConnection, args );

    OstTraceFunctionExit0( RCMMANAGERAPI_WRITEDEFAULTCONNECTION_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::ReadGeneralConnectionSettings()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::ReadGeneralConnectionSettings(
        TCmGenConnSettings& aGenConnSettings )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_READGENERALCONNECTIONSETTINGS_ENTRY );

    TPckgBuf<TCmGenConnSettings> pkg( aGenConnSettings );
    TIpcArgs args( &pkg );
    TInt err = SendReceive( ECmmReadGeneralConnectionSettings, args );
    aGenConnSettings = pkg();

    OstTraceFunctionExit0( RCMMANAGERAPI_READGENERALCONNECTIONSETTINGS_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::WriteGeneralConnectionSettings()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::WriteGeneralConnectionSettings(
        const TCmGenConnSettings& aGenConnSettings )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_WRITEGENERALCONNECTIONSETTINGS_ENTRY );

    TPckgBuf<TCmGenConnSettings> pkg( aGenConnSettings );
    TIpcArgs args( &pkg );
    TInt err = SendReceive( ECmmWriteGeneralConnectionSettings, args );

    OstTraceFunctionExit0( RCMMANAGERAPI_WRITEGENERALCONNECTIONSETTINGS_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::CopyConnectionMethod()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::CopyConnectionMethod(
        const TInt aTargetDestHandle,
        const TInt aConnMethodHandle,
        TInt& aIndex )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_COPYCONNECTIONMETHOD_ENTRY );

    TPckg<TInt> pkg( aIndex );
    TIpcArgs args( aTargetDestHandle, aConnMethodHandle, &pkg );
    TInt err = SendReceive( ECmmCopyConnMethod, args );

    OstTraceFunctionExit0( RCMMANAGERAPI_COPYCONNECTIONMETHOD_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::MoveConnectionMethod()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::MoveConnectionMethod(
        TCmmIpcStructMoveConnMethod& aPkgData )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_MOVECONNECTIONMETHOD_ENTRY );

    TPckgBuf<TCmmIpcStructMoveConnMethod> pkg( aPkgData );
    TIpcArgs args( &pkg );
    TInt err = SendReceive( ECmmMoveConnMethod, args );
    aPkgData = pkg();

    OstTraceFunctionExit0( RCMMANAGERAPI_MOVECONNECTIONMETHOD_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::RemoveConnectionMethod()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::RemoveConnectionMethod(
        const TInt aTargetDestHandle,
        const TInt aConnMethodHandle )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_REMOVECONNECTIONMETHOD_ENTRY );

    TIpcArgs args( aTargetDestHandle, aConnMethodHandle );
    TInt err = SendReceive( ECmmRemoveConnMethod, args );

    OstTraceFunctionExit0( RCMMANAGERAPI_REMOVECONNECTIONMETHOD_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::RemoveAllReferences()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::RemoveAllReferences(
        const TInt aConnMethodHandle )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_REMOVEALLREFERENCES_ENTRY );

    TIpcArgs args( aConnMethodHandle );
    TInt err = SendReceive( ECmmRemoveAllReferences, args );

    OstTraceFunctionExit0( RCMMANAGERAPI_REMOVEALLREFERENCES_EXIT );
    return err;
    }

//-----------------------------------------------------------------------------
// RCmManagerApi::GetUncategorizedIcon()
//-----------------------------------------------------------------------------
//
TInt RCmManagerApi::GetUncategorizedIcon( HBufC* aBuffer )
    {
    OstTraceFunctionEntry0( RCMMANAGERAPI_GETUNCATEGORIZEDICON_ENTRY );

    TPtr descriptor( aBuffer->Des() );
    TIpcArgs args( &descriptor );
    TInt err = SendReceive( ECmmGetUncategorizedIcon, args );

    OstTraceFunctionExit0( RCMMANAGERAPI_GETUNCATEGORIZEDICON_EXIT );
    return err;
    }

// End of file
