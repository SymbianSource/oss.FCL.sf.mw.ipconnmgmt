/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Declaration of CCdcProcessLauncher class.
*
*/



#ifndef C_CCDCPROCESSLAUNCHER_H
#define C_CCDCPROCESSLAUNCHER_H

#include <e32base.h>

/**
*  Launches a single executable and waits for its rendezvous for a given time.
*
*  Launches a single executable
*
*  @lib None
*  @since S60 3.2
*/
class CCdcProcessLauncher : public CActive
    {

public:

    /**
    * Two-phased constructor.
    */
    static CCdcProcessLauncher* NewL();

    /**
    * Destructor.
    */
    virtual ~CCdcProcessLauncher();

    /**
    * Launch the executable and wait for its rendezvous if needed.
    *
    */
    void Launch();

private:

    /**
    * C++ default constructor.
    */
    CCdcProcessLauncher();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

    /**
    * From CActive.
    * Implements cancellation of an outstanding request.
    */
    void DoCancel();

    /**
    * From CActive.
    * Handles an active object’s request completion event.
    */
    void RunL();

    /**
    * Launch the executable and wait for its rendezvous if needed.
    */
    void LaunchProcess();

private: // data

    // THandle to the executable being launched.
    RProcess iProcess;

    // CActiveSchedulerWait to force caller waiting it    
    CActiveSchedulerWait iWait;    

    };

#endif // C_CCDCPROCESSLAUNCHER_H
