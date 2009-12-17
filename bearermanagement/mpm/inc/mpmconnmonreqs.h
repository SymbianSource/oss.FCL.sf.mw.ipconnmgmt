/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: MPM class used for ConnMon requests
*
*/

/**
@file mpmconnmonreqs.h
MPM class used for ConnMon requests.
*/

#ifndef MPMCONNMONREQS_H
#define MPMCONNMONREQS_H

//  INCLUDES
#include <e32base.h>
#include <rconnmon.h>
#include "mpmconnmonevents.h"

// CLASS DECLARATION
/**
*  MPM class definition for ConnMon requests.
*
*  @lib MPMServer.exe
*  @since 3.1
*/
class CMPMConnMonReqs : public CActive
    {

    public:    // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMPMConnMonReqs* NewL( CMPMConnMonEvents& aParent,
                                      RConnectionMonitor& aConnMon,
                                      TUint aConnId,
                                      CMPMServerSession& aSession );

        /**
        * Destructor.
        */
        virtual ~CMPMConnMonReqs();

    public: // New functions
        
        /**
        * Starts IapAvailability request towards Connection monitor. 
        * Calls active sceduler wait to wait until the request completes.
        * This is used when first session is starting and MPMServer registers 
        * to ConnMonitor services. In this case the information 
        * will not be accurate, unless the session creation waits for the 
        * first update from ConnMon. 
        *
        * If registering to ConnMon is moved to MPMServer start, 
        * remove this function.
        *
        * @since 3.1
        */
        void AvailableIapsSync();
        
        /**
        * Requests available IAPs from Connection monitor.
        * This also includes scanning available WLAN networks.
        * @param aCallback Type of callback to use after completing 
        * @param aForceRefreshIntervalSeconds
        * @since 3.2
        */
        void RefreshAvailableIAPs( TWlanScanCallback aCallback, TInt aForceRefreshIntervalSeconds );

        /**
        * Compare request object connection ids
        * @since 3.1
        * @param aReq1
        * @param aReq2
        * @return ETrue if ids are equal, otherwise EFalse
        */
        static TBool CompareConnIds( const CMPMConnMonReqs& aReq1,
                                     const CMPMConnMonReqs& aReq2 );

        /**
        * Marks the request as obsolete but allows it run to its completion
        * @since 3.2
        */
        void MyCancel();

    private:
        /**
        * Completes the error case and handles cancellation.
        * @since 3.2
        */
        void ErrorCallbackL( TInt aStatus );
            
    public: // Functions from base classes

        /**
        * From CActive. Cancels request.
        * @since 3.1
        */
        void DoCancel();

        /**
        * From CActive. Run when request completes.
        * @since 3.1
        */
        void RunL();

        /**
        * From CActive. Run if RunL leaves.
        * @since 3.1
        * @param aError Error that caused leave.
        * @return KErrNone
        */
        TInt RunError( TInt aError );

    private: // New methods

        /**
        * C++ default constructor.
        */
        CMPMConnMonReqs( CMPMConnMonEvents&  aParent,
                         RConnectionMonitor& aConnMon,
                         TUint aConnId, 
                         CMPMServerSession&  aSession );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // Data

        // Internal states types for this object
        enum TRequestState
            {
            EGetIapAvailState, 
            EScanWLANNetworksState,
            EScanWLANNetworksStateCached,
            EGetConnectionCountState, 
            EObsolete
            };

        // Internal state
        TRequestState iNextState;

        // ConnMon request cancel code
        TInt iLastCancelCode;

        // Parent object reference
        CMPMConnMonEvents& iParent;

        // Reference to the Connection Monitor If
        RConnectionMonitor& iConnMon;

        // Connection identifier
        TUint iConnId;

        // Iap availability info buffer
        TConnMonIapInfoBuf iIapBuf;

        // MPM server session which requested the service
        // After service completes callback function is called through 
        // the reference.
        CMPMServerSession& iSession;

        // Used for first session start to wait until necessary data available
        CActiveSchedulerWait iActiveSchedulerWait;        

        // Callback type to be used when WLAN scan has been completed
        TWlanScanCallback iWlanScanCallback;

        // Keeps track of the number of connections
        TUint iConnectionCount; 
    };

#endif // MPMCONNMONREQS_H

// End of file
