/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: WLAN bearer availability check
*
*/

/**
@file mpmwlanavailability.cpp
WLAN bearer availability check
*/

// INCLUDE FILES
#include "mpmwlanavailability.h"
#include "mpmlogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWlanAvailability::NewL
// -----------------------------------------------------------------------------
//
CWlanAvailability* CWlanAvailability::NewL( CMPMConnMonEvents*  aConnMonEvents,
                                            RConnectionMonitor& aConnMon )
    {
    CWlanAvailability* self = new ( ELeave ) CWlanAvailability( aConnMonEvents,
                                                                aConnMon );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CWlanAvailability::CWlanAvailability
// -----------------------------------------------------------------------------
//
CWlanAvailability::CWlanAvailability( CMPMConnMonEvents*  aConnMonEvents,
                                      RConnectionMonitor& aConnMon )
    : CActive( CActive::EPriorityStandard ),
      iConnMonEvents( aConnMonEvents ),
      iConnMon( aConnMon )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CWlanAvailability::ConstructL
// -----------------------------------------------------------------------------
//
void CWlanAvailability::ConstructL()
    {
    iObserver.Reset();
    }

// -----------------------------------------------------------------------------
// CWlanAvailability::Start
// -----------------------------------------------------------------------------
//
void CWlanAvailability::Start( MWlanAvailability* aObserver )
    {
    MPMLOGSTRING( "CWlanAvailability::Start" )

    iObserver.Append( aObserver );

    if ( !IsActive() )
        {
        iConnMon.GetBoolAttribute( EBearerIdWLAN, 
                                   0, 
                                   KBearerAvailability,
                                   iResult,
                                   iStatus );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CWlanAvailability::CancelObserver
// -----------------------------------------------------------------------------
//
void CWlanAvailability::CancelObserver( MWlanAvailability* aObserver )
    {
    MPMLOGSTRING( "CWlanAvailability::CancelObserver" )
    
    for ( TInt i=0; i < iObserver.Count(); i++ )
        {
        if ( iObserver[ i ] == aObserver )
            {
            iObserver.Remove( i );        
            }
        }
    }

// -----------------------------------------------------------------------------
// CWlanAvailability::~CWlanAvailability
// -----------------------------------------------------------------------------
//
CWlanAvailability::~CWlanAvailability()
    {
    MPMLOGSTRING( "CWlanAvailability::~CWlanAvailability" )

    Cancel();

    iObserver.Close();
    }

// -----------------------------------------------------------------------------
// CWlanAvailability::RunL
// -----------------------------------------------------------------------------
//
void CWlanAvailability::RunL()
    {
    if ( iStatus.Int() != KErrNone )
        {
        MPMLOGSTRING2( "CWlanAvailability::RunL status %d", iStatus.Int() )
        }

    for ( TInt i=0; i < iObserver.Count(); i++ )
        {
        iObserver[ i ]->WlanAvailabilityResponse( iStatus.Int(), iResult );
        }

    iObserver.Reset();

    // we are done
    iConnMonEvents->SetWlanAvailabilityPtrNull();
    delete this;
    }

// -----------------------------------------------------------------------------
// CWlanAvailability::DoCancel
// -----------------------------------------------------------------------------
//
void CWlanAvailability::DoCancel()
    {
    MPMLOGSTRING( "CWlanAvailability::DoCancel" )

    iConnMon.CancelAsyncRequest( KBearerAvailability );

    for ( TInt i=0; i < iObserver.Count(); i++ )
        {
        iObserver[ i ]->WlanAvailabilityResponse( KErrCancel, iResult );
        }
    }

// End of file
