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


#ifndef MPMROAMINGWATCHER_H
#define MPMROAMINGWATCHER_H

//  INCLUDES
#include <e32base.h>
#include <etel.h>
#include <etelmm.h>

// Possible roaming status values
enum TMPMRoamingStatus 
    { 
    EMPMHomenetwork = 0, 
    EMPMNationalRoaming, 
    EMPMInternationalRoaming,
    EMPMRoamingStatusUnknown
    };

// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
// None.

// CLASS DECLARATION
/**
*  Mobility Policy Manager network roaming status watcher.
*  @lib MPMServer.exe
*  @since 5.2
*/
class CMPMRoamingWatcher : public CActive
	{
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aMobilePhone Reference to RMobilePhone object
        */
        static CMPMRoamingWatcher* NewL( RMobilePhone& aMobilePhone );
        
        /**
        * Destructor.
        */
        ~CMPMRoamingWatcher();
    
    public: // New functions
        
        /**
        * Returns the current roaming status.
        * @return The current roaming status        
        */
        TMPMRoamingStatus RoamingStatus() const;
        
        TBool AskCellularDataUsageAbroad() const;
        
        void SetAskCellularDataUsageAbroad(TBool aAskUsage);
        
        
    protected: // Constructors and destructor
        	    
	    /**
        * Second-phase constructor.
        */
	    void ConstructL();
    
    protected: // from CActive
    
        /**
        * Handles completions of async requests.
        */
        void RunL();
        
        /**
        * Handles a leave occurring in RunL.
        * @param aError Leave code of RunL method.
        * @return KErrNone
        */
        TInt RunError( TInt aError );
        
        /**
        * Cancels async requests.
        */
        void DoCancel();
        
    private: // Data
        
        /**
        * C++ default constructor.  Not implemented.
        */
        CMPMRoamingWatcher();
        
        /**
        * Constructor.
        */
        CMPMRoamingWatcher( RMobilePhone& aMobilePhone );
        
        /**
        * Maps registration value received from ETEL.
        * @param aStatus Registration value to be mapped
        * @return Roaming status
        */
        TMPMRoamingStatus MapRegistrationStatus(  RMobilePhone::TMobilePhoneRegistrationStatus aStatus );
    
        //data

        /**
        * Reference to ETEL object
        */
        RMobilePhone& iMobilePhone;

        /**
        * This variable is used to store registration status from async request to ETEL
        */
        RMobilePhone::TMobilePhoneRegistrationStatus iRegistrationStatus;   	            

        /**
        * Current roaming status
        */
        TMPMRoamingStatus iRoamingStatus;

        /**
        * Current country
        */
        RMobilePhone::TMobilePhoneNetworkCountryCode iCurrentCountryCode;
        
        /**
         * This variable indicates wheather cellular data usage abroad must be confirmed from the user
         * This variable is set to TRUE when roaming to visitor network happens and it is set to FALSE when
         * Confirm Dialog is presented first time in current foreign country. It is also set to FALSE every
         * time when mobile comes back to home network or moves to other foreign country
         */
        TBool iAskCellularDataUsageAbroad;
	};

#endif // MPMROAMINGWATCHER_H

// End of File
