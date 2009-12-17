/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: MPM Dual Transfer Mode watcher
*
*/

/**
@file mpmdtmwatcher.h
Mobility Policy Manager Dual Transfer Mode watcher class definitions.
*/

#ifndef MPMDTMWATCHER_H
#define MPMDTMWATCHER_H

//  INCLUDES
#include <etelpckt.h>

// FORWARD DECLARATIONS
class CMPMServer;

// CLASS DECLARATION
/**
*  Mobility Policy Manager Dual Transfer Mode watcher.
*  @lib MPMServer.exe
*  @since 3.1
*/
class CMPMDtmWatcher : public CActive
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @return An initialized instance of this class.
        */
        static CMPMDtmWatcher* NewL( RPacketService& aPacketService );

        /**
        * Destructor.
        */
        ~CMPMDtmWatcher();

    public: // New methods

        /**
        * Checks if phone supports Dual Transfer Mode.
        * @since 3.1
        * @return ETrue phone supports DTM, otherwise EFalse.
        */
        TBool IsInDualMode() const;

    protected: // Functions from base classes

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

    private: // New methods

        /**
        * Constructor.
        */
        CMPMDtmWatcher( RPacketService& aPacketService );

        /**
        * C++ default constructor. Not implemented.
        */
        CMPMDtmWatcher();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Writes the packet class to log.
        * @since 3.1
        */
        void LogPacketClass() const;

    private: // Data

        RPacketService&             iPacketService;
        RPacketService::TMSClass    iMsClass;
        RPacketService::TMSClass    iMaxMsClass;
    };

#endif // MPMDTMWATCHER_H
