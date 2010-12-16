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


#ifndef MPMNETWORKWATCHER_H
#define MPMNETWORKWATCHER_H

//  INCLUDES
#include <e32base.h>
#include <etel.h>
#include <etelmm.h>

// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
class CMPMRoamingWatcher;

// CLASS DECLARATION
/**
*  Mobility Policy Manager network status watcher.
*  @lib MPMServer.exe
*  @since 5.2
*/
class CMPMNetworkWatcher : public CActive
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aMobilePhone RMobilePhone object
        * @param aRoamingWatcher CMPMRoamingWatcher object, for remapping
        * @return Watcher object.
        */
        static CMPMNetworkWatcher* NewL( RMobilePhone& aMobilePhone, CMPMRoamingWatcher* aRoamingWatcher );
        
        /**
        * Destructor.
        */
        ~CMPMNetworkWatcher();
    
    public: // New functions
        
        /**
        * Network status.
        * @return Current network's info
        */
        RMobilePhone::TMobilePhoneNetworkInfoV1 CMPMNetworkWatcher::NetworkStatus() const;
        
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
        * Handles a possible leave occurring in RunL (currently nothing leaves).
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
        * Constructor.
        * @param aMobilePhone Reference to RMobilePhone object
        * @param aRoamingWatcher Reference to CMPMRoamingWatcher object
        */
        CMPMNetworkWatcher( RMobilePhone& aMobilePhone, CMPMRoamingWatcher* aRoamingWatcher );
            
        // data

        /**
        * Reference to ETEL's RMobilePhone object
        */
        RMobilePhone& iMobilePhone;

        /**
        * Used to store the notified current network's status
        */
        RMobilePhone::TMobilePhoneNetworkInfoV2 iNetworkInfo;

        /**
        * Used to store the notified current network's status
        */
        RMobilePhone::TMobilePhoneNetworkInfoV2Pckg iNetworkInfoPckg;

        /**
        * Reference to CMPMRoamingWatcher object
        */
        CMPMRoamingWatcher* iRoamingWatcher;
    };

#endif // MPMNETWORKWATCHER_H

// End of File
