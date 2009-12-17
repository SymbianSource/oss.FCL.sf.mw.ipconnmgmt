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
* Description:  Defines MAOGpds interface
*
*/


#ifndef M_MAOGPDS_H
#define M_MAOGPDS_H

// INCLUDE FILES

// FORWARD DECLARATIONS

/**
*  GPDS interface.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
class MAOGpds
    {
public: // Data types

    /** Always-On mode enums */
    enum TAlwaysOnMode
        {
        EHPLMN,
        EVPLMN,
        EBoth,
        ENeither
        };

public: // New methods

    /**
     * Sets always on mode.
     *
     * @since S60 v3.1
     * @param aMode: Current always on mode
     */
    virtual void SetAlwaysOnModeL( TAlwaysOnMode aMode ) = 0;

    };

#endif // M_MAOGPDS_H
