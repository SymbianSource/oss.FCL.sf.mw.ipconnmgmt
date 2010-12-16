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
* Description:  Observes terminals roaming status.
*
*/


// INCLUDE FILES

#include <cmgenconnsettings.h>

#include "mpmserver.h"
#include "mpmlogger.h"
#include "mpmnetworkwatcher.h"
#include "mpmroamingwatcher.h"

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
// CMPMRoamingWatcher::CMPMRoamingWatcher
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CMPMRoamingWatcher::CMPMRoamingWatcher( RMobilePhone& aMobilePhone,
                                        CMPMServer* aServer )
    : CActive( CActive::EPriorityStandard ),
      iMobilePhone( aMobilePhone ),
      iServer( aServer ),
      iRegistrationStatus( RMobilePhone::ERegistrationUnknown ),
      iRoamingStatus( EMPMRoamingStatusUnknown ),
      iPreviousValidRoamingStatus( EMPMRoamingStatusUnknown )
    {
    MPMLOGSTRING( "CMPMRoamingWatcher::CMPMRoamingWatcher" )

    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CMPMRoamingWatcher::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMPMRoamingWatcher::ConstructL()
    {
    MPMLOGSTRING( "CMPMRoamingWatcher::ConstructL" )

    iNetworkWatcher = CMPMNetworkWatcher::NewL( iMobilePhone, this );
            
    // Retrieve current status
    iMobilePhone.GetNetworkRegistrationStatus( iStatus, iRegistrationStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CMPMRoamingWatcher::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPMRoamingWatcher* CMPMRoamingWatcher::NewL( RMobilePhone& aMobilePhone,
                                              CMPMServer* aServer )
    {
    MPMLOGSTRING( "CMPMRoamingWatcher::NewL" )
    CMPMRoamingWatcher* self = new( ELeave ) CMPMRoamingWatcher( aMobilePhone, aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// CMPMRoamingWatcher::~CMPMRoamingWatcher
// Destructor.
// -----------------------------------------------------------------------------
//
CMPMRoamingWatcher::~CMPMRoamingWatcher()
    {
    MPMLOGSTRING( "CMPMRoamingWatcher::~CMPMRoamingWatcher" )

    delete iNetworkWatcher;
    Cancel();
    }

// -----------------------------------------------------------------------------
// CMPMRoamingWatcher::RunL
//
// -----------------------------------------------------------------------------
//
void CMPMRoamingWatcher::RunL()
    {
    MPMLOGSTRING( "CMPMRoamingWatcher::RunL" )

    // iRegistrationStatus has been updated when coming to this RunL().
            
    if (iStatus.Int() != KErrNone)
        {
        MPMLOGSTRING2( "CMPMRoamingWatcher::RunL: Erroneous registration event, iStatus: %d", iStatus.Int() )
        iRegistrationStatus = RMobilePhone::ERegistrationUnknown;
        }
    
    // Update iRoamingStatus with current registration status value.
    UpdateRoamingStatus(); 

    // Start listening for next status change
    iMobilePhone.NotifyNetworkRegistrationStatusChange( iStatus, iRegistrationStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CMPMRoamingWatcher::RunError
//
// -----------------------------------------------------------------------------
//
TInt CMPMRoamingWatcher::RunError( TInt /*aError*/ )
    {
    MPMLOGSTRING( "CMPMRoamingWatcher::RunError" )

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMPMRoamingWatcher::DoCancel
//
// -----------------------------------------------------------------------------
//
void CMPMRoamingWatcher::DoCancel()
    {
    MPMLOGSTRING( "CMPMRoamingWatcher::DoCancel" )

    iMobilePhone.CancelAsyncRequest( EMobilePhoneNotifyNetworkRegistrationStatusChange );
    }

// -----------------------------------------------------------------------------
// CMPMRoamingWatcher::UpdateRoamingStatus
// Updates the class variable iRoamingStatus.
// -----------------------------------------------------------------------------
//
void CMPMRoamingWatcher::UpdateRoamingStatus()
    {
    MPMLOGSTRING2( "CMPMRoamingWatcher::UpdateRoamingStatus: iRoamingStatus: %d", iRoamingStatus )
    MPMLOGSTRING2( "CMPMRoamingWatcher::UpdateRoamingStatus: iPreviousValidRoamingStatus: %d",
                   iPreviousValidRoamingStatus )            
    MPMLOGSTRING2( "CMPMRoamingWatcher::UpdateRoamingStatus: iRegistrationStatus: %d", iRegistrationStatus )
    
    TMPMRoamingStatus roamingStatus = EMPMRoamingStatusUnknown;

    switch (iRegistrationStatus)
        {
        case RMobilePhone::ERegisteredOnHomeNetwork:
            {
            roamingStatus = EMPMHomenetwork;
            break;
            }
            
        case RMobilePhone::ERegisteredRoaming:
            {
            RMobilePhone::TMobilePhoneNetworkInfoV1 currentNetwork;

            RMobilePhone::TMobilePhoneNetworkInfoV1 homeNetwork;
            RMobilePhone::TMobilePhoneNetworkInfoV1Pckg homeNetworkPckg( homeNetwork );

            currentNetwork = iNetworkWatcher->NetworkStatus();

            TRequestStatus status( KErrNone );
            iMobilePhone.GetHomeNetwork( status, homeNetworkPckg );
            User::WaitForRequest( status );

            if (status.Int() == KErrNone)
                {
                RMobilePhone::TMobilePhoneNetworkCountryCode countryCode =
                        currentNetwork.iCountryCode;
                if (countryCode.Compare( homeNetwork.iCountryCode ) == 0)
                    {
                    roamingStatus = EMPMNationalRoaming;
                    }
                else
                    {
                    roamingStatus = EMPMInternationalRoaming;
                    }
                }                
            break;
            }
            
        default:
            {
            roamingStatus = EMPMRoamingStatusUnknown;
            break;
            }            
        }

    // Stop cellular connections if the roaming status has really changed
    if( ( roamingStatus != EMPMRoamingStatusUnknown ) &&
        ( roamingStatus != iPreviousValidRoamingStatus ) &&
        ( iPreviousValidRoamingStatus != EMPMRoamingStatusUnknown ) )
        {
        TUint32 currentDataUsage = iServer->DataUsageWatcher()->CellularDataUsage();

        // All cellular connections are stopped when roaming has occurred and cellular
        // data usage needs to be confirmed from user or it is disabled 
        if ( ( currentDataUsage == ECmCellularDataUsageConfirm ) ||
             ( currentDataUsage == ECmCellularDataUsageDisabled ) )
            {
            iServer->StopCellularConns();
            }
        }

    // Store the previous roaming status if it is a valid roaming status
    if ( roamingStatus != EMPMRoamingStatusUnknown )
        {
        iPreviousValidRoamingStatus = roamingStatus;
        }

    iRoamingStatus = roamingStatus;
    MPMLOGSTRING2( "CMPMRoamingWatcher::UpdateRoamingStatus: iRoamingStatus: %d", iRoamingStatus )
    }

// -----------------------------------------------------------------------------
// CMPMRoamingWatcher::RoamingStatus
//
// -----------------------------------------------------------------------------
//
TMPMRoamingStatus CMPMRoamingWatcher::RoamingStatus() const
    {
    MPMLOGSTRING2( "CMPMRoamingWatcher::RoamingStatus iRoamingStatus: %d", iRoamingStatus )

    return iRoamingStatus;
    }

// End of File
