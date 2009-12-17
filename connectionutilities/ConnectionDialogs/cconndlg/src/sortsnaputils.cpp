/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class TSortSnapUtils.
*
*/


// INCLUDE FILES
#include "sortsnaputils.h"
#include "ConnectionDialogsLogger.h"

// ================= MEMBER FUNCTIONS =======================
//
// ---------------------------------------------------------
// TSortSnapUtils::TSortSnapUtils
// ---------------------------------------------------------
//
TSortSnapUtils::TSortSnapUtils( RMPM& aMPM ):
iSnapCount( 0 ),
iSSUState( EUninitialised ),
iMPM( aMPM )
    {
    CLOG_ENTERFN( "TSortSnapUtils::TSortSnapUtils" );
    CLOG_LEAVEFN( "TSortSnapUtils::TSortSnapUtils" );
    }


// ---------------------------------------------------------
// TSortSnapUtils::~TSortSnapUtils
// ---------------------------------------------------------
//
TSortSnapUtils::~TSortSnapUtils()
    {
    CLOG_ENTERFN( "TSortSnapUtils::~TSortSnapUtils" );
    CLOG_LEAVEFN( "TSortSnapUtils::~TSortSnapUtils" );
    }

// ---------------------------------------------------------
// TSortSnapUtils::InitDestIdArray
// ---------------------------------------------------------
//
void TSortSnapUtils::InitDestIdArray()
    {
    CLOG_ENTERFN( "TSortSnapUtils::InitDestIdArray" );
    switch (iSSUState)
        {
        case EUseMPM:
            {
            // we already know that MPM is used
            CLOG_WRITE( "using MPM, no need to re-read from P&S " );
            break;
            }
        case EUninitialised:
        case EUsePubSub:
        default:
            {
            iSnapCount = 0;
            TMpmSnapPubSubFormat snapData;
            TPtr8 e( reinterpret_cast< TUint8* >( &snapData ), 
                     sizeof( snapData ) );
            for (TInt i = 0 ; i < KMPMSortSnapMaxKey; i++ )
                {
                TInt err = RProperty::Get( KMPMSortSnapCategory, 
                                           i, 
                                           e );
               CLOG_WRITEF( _L( "RProperty::Get returned: %d" ), err );
               CLOG_WRITEF( _L( "Snap Id: %d" ), snapData.iSnap );
               CLOG_WRITEF( _L( "Snap Number: %d" ), iSnapCount );

                if (err == KErrNone)                            
                    {
                    iSnapIdArray[iSnapCount] = snapData.iSnap;
                    iSnapCount++;
                    }
                }
            //determine what method to use:
            // we always have at least one snap element
            // in P&S
            // if we are called from MPM                
            if (iSnapCount)
                {
                CLOG_WRITE( "iSSUState = EUsePubSub " );
                iSSUState = EUsePubSub;       
                }
             else
                {
                CLOG_WRITE( "iSSUState = EUseMPM " );
                iSSUState = EUseMPM;       
                }
            }
        }
    CLOG_LEAVEFN( "TSortSnapUtils::InitDestIdArray" );
    }

// ---------------------------------------------------------
// TSortSnapUtils::GetDestIdArrayL
// ---------------------------------------------------------
//
void TSortSnapUtils::GetDestIdArrayL( RCmManagerExt& aCmManagerExt, RArray<TUint32>& aDestArray )
    {
    CLOG_ENTERFN( "TSortSnapUtils::GetDestIdArrayL" );
    aDestArray.Reset();
    InitDestIdArray(); // re-read the values, set iSSUState
    switch (iSSUState)
        {
        case EUsePubSub:
            {
            CLOG_WRITE( "Using P&S " );
            // append from 1, do not append uncategorised destination!
            for ( TInt i = 1 ; i < iSnapCount; i++ )
                {
                aDestArray.Append( iSnapIdArray[i] );
                CLOG_WRITEF( _L( "Appending to array: Snap Id: %d" ), iSnapIdArray[i] );                
                }
            break;
            }
        case EUseMPM:
            {
            CLOG_WRITE( "Using MPM " );
            aCmManagerExt.AllDestinationsL( aDestArray );
            break;
            }
        case EUninitialised:
        default:
            {
            //should not ever happen, as we initialise at the beginning of the 
            // function
            User::Leave( KErrNotReady );
            }
        }
    CLOG_LEAVEFN( "TSortSnapUtils::GetDestIdArrayL" );
    }

// ---------------------------------------------------------
// TSortSnapUtils::GetSortSnapDataL()
// ---------------------------------------------------------
//
TInt TSortSnapUtils::GetSortSnapData( TUint32 aSnapId, TMpmSnapBuffer& aSnapBuffer )
    {
    CLOG_ENTERFN( "TSortSnapUtils::GetSortSnapDataL" );
    
    if (iSSUState ==  EUninitialised)
        {
        InitDestIdArray();
        }
    
    switch (iSSUState)
        {
        case EUsePubSub:
            {
            CLOG_WRITE( "Using P&S " );
            TMpmSnapPubSubFormat snapData;
            TPtr8 e( reinterpret_cast< TUint8* >( &snapData ), 
            sizeof( snapData ) );
            // initialize snap to invalid value
            snapData.iSnap = (TUint32) KErrNotFound;
            for ( TInt i = 0 ; i < iSnapCount; i++ )
                {
                if ( aSnapId == iSnapIdArray[i] )
                    {
                    
                    TInt err = RProperty::Get( KMPMSortSnapCategory, 
                                               i, 
                                               e );
                    CLOG_WRITEF( _L( "RProperty::Get returned: %d" ), err );

                    if (err == KErrNone)                            
                        {
                        if ( aSnapId == snapData.iSnap )
                            {
                            aSnapBuffer = snapData.iSortedIaps;
                            
                            CLOG_WRITEF( _L( "Found: Snap id: %d" ), snapData.iSnap );
                            CLOG_WRITEF( _L( "Iap count: %d" ), snapData.iSortedIaps.iCount );
                            
                            CLOG_LEAVEFN( "TSortSnapUtils::GetSortSnapDataL" );
                            return KErrNone;
                            }
                        }
                    else
                        {
                        CLOG_LEAVEFN( "TSortSnapUtils::GetSortSnapDataL" );
                        return err;
                        }
                    }
                }
            break;    
            }
        case EUseMPM:
            {
            CLOG_WRITE( "Using MPM " );
            TInt errConnect = iMPM.Connect();
            CLOG_WRITEF( _L( "errConnect = %d" ), errConnect );
            if ( errConnect == KErrNone )
                {
                TInt err = iMPM.SortSNAP( aSnapId, aSnapBuffer ); 
                iMPM.Close();
                CLOG_LEAVEFN( "TSortSnapUtils::GetSortSnapDataL" );
                return err;       
                }
            else
                {
                CLOG_LEAVEFN( "TSortSnapUtils::GetSortSnapDataL" );
                return errConnect;
                }
            }
        case EUninitialised:
        default:
            {
            //should not ever happen, as we initialise at the beginning of the 
            // function            
            CLOG_LEAVEFN( "TSortSnapUtils::GetSortSnapDataL" );
            return KErrNotReady;
            }
        }
    return KErrNone;
    }

