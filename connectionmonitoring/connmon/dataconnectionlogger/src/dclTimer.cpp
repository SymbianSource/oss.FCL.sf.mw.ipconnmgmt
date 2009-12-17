/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Timer active object needed in logging data in timer mode.
*
*/

#include <e32base.h>

#include "dclTimer.h"
#include "dcl_log.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDclTimerAO::CDclTimerAO
// -----------------------------------------------------------------------------
//
CDclTimerAO::CDclTimerAO( CEngine* aEngine, TInt aTimerInterval )
    :
    CActive( CActive::EPriorityStandard ),
    iEngine( aEngine ),
    iTimerInterval( aTimerInterval )
    {
    }

// -----------------------------------------------------------------------------
// CDclTimerAO::ConstructL
// -----------------------------------------------------------------------------
//
void CDclTimerAO::ConstructL()
    {
    CActiveScheduler::Add( this );

    User::LeaveIfError( iTimer.CreateLocal() );

    iConnectionIds.Reset();
    }

// Destructor
CDclTimerAO::~CDclTimerAO()
    {
    Cancel();
    iTimer.Close();
    iConnectionIds.Close();
    }

// -----------------------------------------------------------------------------
// CDclTimerAO::Add
// -----------------------------------------------------------------------------
//
void CDclTimerAO::Add( const TUint& aConnectionId )
    {
    iConnectionIds.Append( aConnectionId );

    NextTimerAfter();
    }

// -----------------------------------------------------------------------------
// CDclTimerAO::Remove
// -----------------------------------------------------------------------------
//
void CDclTimerAO::Remove( const TUint& aConnectionId )
    {
    for ( TInt i=0; i < iConnectionIds.Count(); i++ )
        {
        if ( aConnectionId == iConnectionIds[ i ] )
            {
            // A connection has switched to threshold mode
            iConnectionIds.Remove( i );
            return;
            }
        }
    }

// -----------------------------------------------------------------------------
// CDclTimerAO::NextTimerAfter
// -----------------------------------------------------------------------------
//
void CDclTimerAO::NextTimerAfter()
    {
    if ( !IsActive() )
        {
        iTimer.After( iStatus, iTimerInterval * 1000000 );
        SetActive();
        }
    }


// -----------------------------------------------------------------------------
// CDclTimerAO::DoCancel
// -----------------------------------------------------------------------------
//
void CDclTimerAO::DoCancel()
    {
    if ( IsActive() )
        {
        iTimer.Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CDclTimerAO::RunL
// -----------------------------------------------------------------------------
//
void CDclTimerAO::RunL()
    {
    TInt count( 0 );

    if ( iStatus.Int() == KErrNone )
        {
        LOG( Log::Printf( _L("Timer EVENT\n")));

        // Log all connections that are in timer mode
        count = iConnectionIds.Count();

        for ( TInt i = ( count - 1 ); i >= 0; i-- )
            {
            TInt ret = iEngine->LogDataInTimerModeL( iConnectionIds[ i ] );

            if ( ret != KErrNone )
                {
                // A connection has switched to threshold mode
                iConnectionIds.Remove( i );
                }
            }

        // Go on with the timer if there are still connections in timer mode
        if ( iConnectionIds.Count() > 0 )
            {
            NextTimerAfter();
            }
        }
    else
        {
        // Switch all connections back to threshold mode beacause timer failed
        LOG( Log::Printf( _L("Timer FAILED: %d.\n"), iStatus.Int()));

        count = iConnectionIds.Count();

        for ( TInt i = ( count - 1 ); i >= 0; i-- )
            {
            iEngine->SwitchToThresholdMode( iConnectionIds[ i ] );
            iConnectionIds.Remove( i );
            }
        }
    }

// End-of-file
