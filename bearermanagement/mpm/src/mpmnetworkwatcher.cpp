/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observes network status.
*
*/


// INCLUDE FILES
#include "mpmlogger.h"
#include "mpmroamingwatcher.h"
#include "mpmnetworkwatcher.h"

// EXTERNAL DATA STRUCTURES
// None.

// EXTERNAL FUNCTION PROTOTYPES  
// None.

// CONSTANTS

// MACROS
// None.

// LOCAL CONSTANTS AND MACROS
// None.

// MODULE DATA STRUCTURES
// None.

// LOCAL FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
// None.

// -----------------------------------------------------------------------------
// CMPMNetworkWatcher::CMPMNetworkWatcher
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CMPMNetworkWatcher::CMPMNetworkWatcher( RMobilePhone& aMobilePhone, CMPMRoamingWatcher* aRoamingWatcher  )
    : CActive( CActive::EPriorityStandard ),
      iMobilePhone( aMobilePhone ),
      iNetworkInfo(),
      iNetworkInfoPckg( iNetworkInfo ),
      iRoamingWatcher( aRoamingWatcher )
    {
    MPMLOGSTRING( "CMPMNetworkWatcher::CMPMNetworkWatcher" )

    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CMPMNetworkWatcher::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMPMNetworkWatcher::ConstructL()
    {
    MPMLOGSTRING( "CMPMNetworkWatcher::ConstructL" )

    // Retrieve current status
    iMobilePhone.GetCurrentNetwork( iStatus, iNetworkInfoPckg );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CMPMNetworkWatcher::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPMNetworkWatcher* CMPMNetworkWatcher::NewL( RMobilePhone& aMobilePhone, CMPMRoamingWatcher* aRoamingWatcher )
    {
    MPMLOGSTRING( "CMPMNetworkWatcher::NewL" )

    CMPMNetworkWatcher* self = new( ELeave ) CMPMNetworkWatcher( aMobilePhone, aRoamingWatcher );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CMPMNetworkWatcher::~CMPMNetworkWatcher
// Destructor.
// -----------------------------------------------------------------------------
//
CMPMNetworkWatcher::~CMPMNetworkWatcher()
    {
    MPMLOGSTRING( "CMPMNetworkWatcher::~CMPMNetworkWatcher" )

    Cancel();
    }

// -----------------------------------------------------------------------------
// CMPMNetworkWatcher::RunL
//
// -----------------------------------------------------------------------------
//
void CMPMNetworkWatcher::RunL()
    {
    MPMLOGSTRING( "CMPMNetworkWatcher::RunL" )

    if (iStatus.Int() == KErrNone)
        {
        // Updated network status should now be found in iNetworkInfo.
        MPMLOGSTRING( "CMPMNetworkWatcher::RunL: Network event was notified." )
        iRoamingWatcher->UpdateRoamingStatus();
        }

    // Start to listen for next status change
    iMobilePhone.NotifyCurrentNetworkChange( iStatus, iNetworkInfoPckg );
    SetActive();
    }


// -----------------------------------------------------------------------------
// CMPMNetworkWatcher::RunError
//
// -----------------------------------------------------------------------------
//
TInt CMPMNetworkWatcher::RunError( TInt /*aError*/ )
    {
    MPMLOGSTRING( "CMPMNetworkWatcher::RunError" )
            
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMPMNetworkWatcher::DoCancel
//
// -----------------------------------------------------------------------------
//
void CMPMNetworkWatcher::DoCancel()
    {
    MPMLOGSTRING( "CMPMNetworkWatcher::DoCancel" )
            
    iMobilePhone.CancelAsyncRequest( EMobilePhoneNotifyCurrentNetworkChange );
    }

// -----------------------------------------------------------------------------
// CMPMNetworkWatcher::NetworkStatus
//
// -----------------------------------------------------------------------------
//
RMobilePhone::TMobilePhoneNetworkInfoV1 CMPMNetworkWatcher::NetworkStatus() const
    {
    MPMLOGSTRING( "CMPMNetworkWatcher::NetworkStatus" )
            
    return iNetworkInfo;
    }

// End of File
