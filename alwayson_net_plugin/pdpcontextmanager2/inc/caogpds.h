/*
* Copyright (c) 2004,2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  GPDS interface
*
*/


#ifndef C_CAOGPDS_H
#define C_CAOGPDS_H

// INCLUDE FILES
#include <e32base.h>

#include "maogpds.h"
#include "alwaysonconfig.hrh"

// FORWARD DECLARATIONS
class CPeriodic;
class CActiveSchedulerWait;

class RMmCustomAPI;

/**
*  GPDS interface. Through GPDS interface can be set
*  Always-On mode for TSY.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
NONSHARABLE_CLASS( CAOGpds ):
    public CActive,
    public MAOGpds
    {
public:    // Constructors & destructors

    
    /**
     * Two-phased constructor.
     *
     * @since S60 v3.1
     * @param aCustomAPI: RMCustomApi sends notification to
     *      Always On server from RAU (Routing Area Update)
     *      Always On server sends notification to TSY via RMCustomApi.  
     * @return a pointer to the created instance of CAOGpds.
     */
    static CAOGpds* NewL( RMmCustomAPI& aCustomAPI );


    /**
     * Destructor
     *
     * @since S60 v3.1
     */
    virtual ~CAOGpds();
    
private: // Constructors
    
    /**
     * Constructor.
     *
     * @since S60 v3.1
     * @param aCustomAPI: RMCustomApi sends notification to
     *      Always On server from RAU (Routing Area Update)
     *      Always On server sends notification to TSY via RMCustomApi.  
     */
    CAOGpds( RMmCustomAPI& aCustomAPI );


    /**
     * Default Symbian second phase constructor
     *
     * @since S60 v3.1
     */
    void ConstructL();
    
private: // From base class CActive

    /**
     * From CActive
     * Process the completed request.
     *
     * @since S60 v3.1
     */
    void RunL();
    
    /**
     * From CActive
     * Cancel outstanding request
     *
     * @since S60 v3.1
     */
    void DoCancel();
    
private: // From base class MAOGpds

    /**
     * From MAOGdps
     * Sets always on mode.
     *
     * @since S60 v3.1
     * @param aMode: Current always on mode
     */
    void SetAlwaysOnModeL( TAlwaysOnMode aMode );

private: // Data

    /**
     * Active scheduler wait
     * Own
     */
    CActiveSchedulerWait* iWait;
    

    /**
     * Reference to RMCustomAPI
     */
    RMmCustomAPI& iCustomAPI;
    
    };

#endif // C_CAOGPDS_H
