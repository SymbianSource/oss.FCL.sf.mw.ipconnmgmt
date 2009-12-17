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
* Description:  Defines MAOStateContext interface
*
*/


#ifndef M_MAOSTATECONTEXT_H
#define M_MAOSTATECONTEXT_H

// INCLUDE FILES
#include <e32std.h>

// FORWARD DECLARATIONS
class MAOConnectionManager;
class MAOTimer;
class MAORAUManager;
class MAOSettings;
class MAOGpds;

/**
*  State context interface.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
class MAOStateContext
    {
public:

    /**
     * Returns connection manager interface.
     *
     * @since 3.1
     * @return MAOConnectionManager&
     */
    virtual MAOConnectionManager& ConnectionManager() const = 0;

    /**
      * Returns retry timer interface.
     *
     * @since 3.1
     * @return MAOTimer&
     */
    virtual MAOTimer& Timer() const = 0;
    
    /**
      * Returns RAU manager interface.
     *
     * @since 3.1
     * @return MAORAUManager&
     */
    virtual MAORAUManager& RAUManager() const = 0;
    
    /**
     * Returns settings interface.
     *
     * @since 3.1
     * @return MAOSettings&
     */
    virtual MAOSettings& Settings() const = 0;
    
    /**
     * Returns GPDS interface.
     *
     * @since 3.1
     * @return MAOGpds&
     */
    virtual MAOGpds& Gpds() const = 0;
    
protected:

    /**
     * Prohibit destruction
     *
     * @since 3.1
     */
    virtual ~MAOStateContext(){};
    };

#endif // M_MAOSTATECONTEXT_H
