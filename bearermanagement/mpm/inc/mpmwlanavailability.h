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
* Description: WLAN availability check.
*
*/

/**
@file mpmwlanavailability.h
WLAN availability check definition.
*/

#ifndef MPMWLANAVAILABILITY_H
#define MPMWLANAVAILABILITY_H

// INCLUDES
#include <e32base.h>
#include <rconnmon.h>
#include "mpmconnmonevents.h"

// FORWARD DECLARATIONS
class CMPMConnMonEvents;

// CLASS DECLARATION
/**
*  WLAN availability check
*
*  @lib MPMServer.exe
*  @since 5.2
*/
class MWlanAvailability
    {
    public:
        /**
        * WLAN availability check response
        * @since 5.2
        * @param aError generic Symbian error response code
        * @param aResult Result of WLAN availability check
        * @return void
        */
        virtual void WlanAvailabilityResponse( const TInt  aError, 
                                               const TBool aResult ) = 0;
    };

// CLASS DECLARATION
/**
*  WLAN availability check implementation
*
*  @lib MPMServer.exe
*  @since 5.2
*/
NONSHARABLE_CLASS( CWlanAvailability ) : public CActive
    {
    public:

        /**
        * Two-phased constructor.
        */
        static CWlanAvailability* NewL( CMPMConnMonEvents*  aConnMonEvents,
                                        RConnectionMonitor& aConnMon );

        /**
        * Destructor.
        */
        virtual ~CWlanAvailability();

        /**
        * For starting the active object.
        * @since 5.2
        */
        void Start( MWlanAvailability* aObserver );
        
        /**
         * For cancelling the request for an observer.
         * @since 5.2
         */
        void CancelObserver( MWlanAvailability* aObserver );
        
    private:

        /**
        * C++ default constructor.
        */
        CWlanAvailability( CMPMConnMonEvents*  aConnMonEvents,
                           RConnectionMonitor& aConnMon );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // from CActive

        /**
        * Active Object RunL.
        */
        void RunL();

        /**
        * Active Object DoCancel.
        */
        void DoCancel();

    private:
        
        CMPMConnMonEvents*                  iConnMonEvents;
        RConnectionMonitor&                 iConnMon;
        RPointerArray< MWlanAvailability >  iObserver;
        TRequestStatus                      iRequestStatus;
        TBool                               iResult;
    };

#endif // MPMWLANAVAILABILITY_H
