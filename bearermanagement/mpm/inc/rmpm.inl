/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Inline functions for RMPM class
*
*/


// -----------------------------------------------------------------------------
// TMpmSnapBuffer::Reset
// -----------------------------------------------------------------------------
//
inline void TMpmSnapBuffer::Reset()
    {
    iCount = 0;
    for ( TUint i( 0 ); i < KMpmMaxIAPCount; i++ )
        {
        iIapId[i] = 0;
        }
    }

// -----------------------------------------------------------------------------
// TMpmSnapBuffer::Count
// -----------------------------------------------------------------------------
//
inline const TInt TMpmSnapBuffer::Count() const
    {
    return iCount;
    }
    
// -----------------------------------------------------------------------------
// TMpmNotificationPrefIAPAvailable::Length
// -----------------------------------------------------------------------------
//
inline TInt TMpmNotificationPrefIAPAvailable::Length()
    {
    return sizeof( TMpmNotificationPrefIAPAvailable );
    }

// -----------------------------------------------------------------------------
// TMpmNotificationError::Length
// -----------------------------------------------------------------------------
//
inline TInt TMpmNotificationError::Length()
    {
    return sizeof( TMpmNotificationError );
    }

// -----------------------------------------------------------------------------
// TMpmNotificationStartIAP::Length
// -----------------------------------------------------------------------------
//
inline TInt TMpmNotificationStartIAP::Length()
    {
    return sizeof( TMpmNotificationStartIAP );
    }

    
// -----------------------------------------------------------------------------
// TMpmNotificationStopIAP::Length
// -----------------------------------------------------------------------------
//
inline TInt TMpmNotificationStopIAP::Length()
    {
    return sizeof( TMpmNotificationStopIAP );
    }

#ifdef _DEBUG
// -----------------------------------------------------------------------------
// RMPM::SendReq
// -----------------------------------------------------------------------------
//
inline void RMPM::SendReq( TMpmAsynchCodes aReq, TInt aValue )
    {
    //TPckg<TInt> intPckg( aValue );
    TInt err = SendReceive( aReq, 
                            TIpcArgs( aValue ) );
    ASSERT( err == KErrNone );
    }
#endif //_DEBUG

