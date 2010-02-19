/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "mpmroamingwatcher.h"
#include "mpmlogger.h"

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

// ============================= LOCAL FUNCTIONS ===============================
// None.

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPMRoamingWatcher::CMPMRoamingWatcher
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CMPMRoamingWatcher::CMPMRoamingWatcher( RMobilePhone& aMobilePhone )
    : CActive( CActive::EPriorityStandard ),
      iMobilePhone( aMobilePhone ),
      iRegistrationStatus( RMobilePhone::ERegistrationUnknown ),
      iRoamingStatus( EMPMRoamingStatusUnknown ),
      iCurrentCountryCode( 0 )
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
    // Retrieve current status
    TRequestStatus status( KErrNone );
    RMobilePhone::TMobilePhoneRegistrationStatus registrationStatus ( RMobilePhone::ERegistrationUnknown );
    iMobilePhone.GetNetworkRegistrationStatus( status, registrationStatus );
    User::WaitForRequest( status );
        
    if( status.Int() == KErrNone )
        {
        iRoamingStatus = MapRegistrationStatus( registrationStatus );
        MPMLOGSTRING2( "CMPMRoamingWatcher::ConstructL iRoamingStatus: %d", iRoamingStatus )
        }

    //Start waiting notification    
    iMobilePhone.NotifyNetworkRegistrationStatusChange( iStatus, iRegistrationStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CMPMRoamingWatcher::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPMRoamingWatcher* CMPMRoamingWatcher::NewL( RMobilePhone& aMobilePhone )
    {
    MPMLOGSTRING( "CMPMRoamingWatcher::NewL" )
    CMPMRoamingWatcher* self = new( ELeave ) CMPMRoamingWatcher( aMobilePhone );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMPMRoamingWatcher::~CIpdcRoamingObserver
// Destructor.
// -----------------------------------------------------------------------------
//
CMPMRoamingWatcher::~CMPMRoamingWatcher()
    {
    MPMLOGSTRING( "CMPMRoamingWatcher::~CMPMRoamingWatcher" )
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

    TMPMRoamingStatus roamingStatus = EMPMRoamingStatusUnknown;
    RMobilePhone::TMobilePhoneRegistrationStatus registrationStatus = RMobilePhone::ERegistrationUnknown;

    TInt status = iStatus.Int();

    //store current value if there was no error
    if( status == KErrNone )
        {
        registrationStatus = iRegistrationStatus;
        }

    //Start listening next status change
    iMobilePhone.NotifyNetworkRegistrationStatusChange( iStatus, iRegistrationStatus );
    SetActive();

    //Map current value
    roamingStatus = MapRegistrationStatus( registrationStatus ); 

    if( roamingStatus != iRoamingStatus )
        {
        iRoamingStatus = roamingStatus;
        MPMLOGSTRING2( "CMPMRoamingWatcher::RunL iRoamingStatus: %d", iRoamingStatus )
        }
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
// CMPMRoamingWatcher::MapRegistrationStatus
//
// -----------------------------------------------------------------------------
//
TMPMRoamingStatus CMPMRoamingWatcher::MapRegistrationStatus( RMobilePhone::TMobilePhoneRegistrationStatus aStatus )
    {
    MPMLOGSTRING( "CMPMRoamingWatcher::MapRegistrationStatus" )
    TMPMRoamingStatus roamingStatus = EMPMRoamingStatusUnknown;

    switch (aStatus)
        {
        case RMobilePhone::ERegisteredOnHomeNetwork:
            {
            roamingStatus = EMPMHomenetwork;
            break;
            }
            
        case RMobilePhone::ERegisteredRoaming:
            {
            RMobilePhone::TMobilePhoneNetworkInfoV1 network;
            RMobilePhone::TMobilePhoneNetworkInfoV1Pckg networkPckg( network );

            RMobilePhone::TMobilePhoneNetworkInfoV1 homeNetwork;
            RMobilePhone::TMobilePhoneNetworkInfoV1Pckg homeNetworkPckg( homeNetwork );

            TRequestStatus status( KErrNone );

            iMobilePhone.GetCurrentNetwork(status, networkPckg);
            User::WaitForRequest( status );

            if(status.Int() == KErrNone)
                {
                iMobilePhone.GetHomeNetwork(status, homeNetworkPckg);
                User::WaitForRequest( status );        

                if(status.Int() == KErrNone)
                    {
                    RMobilePhone::TMobilePhoneNetworkCountryCode countryCode = network.iCountryCode;
                    
                    if(countryCode.Compare(homeNetwork.iCountryCode) == 0)
                        {
                        roamingStatus = EMPMNationalRoaming;
                        }
                    else
                        {
                        roamingStatus = EMPMInternationalRoaming;
                        }
                    iCurrentCountryCode = countryCode; 
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
    MPMLOGSTRING2( "CMPMRoamingWatcher::MapRegistrationStatus roamingStatus: %d", roamingStatus )

    return roamingStatus;
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

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None.

// End of File
