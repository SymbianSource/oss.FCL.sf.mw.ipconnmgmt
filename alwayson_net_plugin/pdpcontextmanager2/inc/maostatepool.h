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
* Description:  Defines MAOStatePool interface
*
*/


#ifndef M_MAOSTATEPOOL_H
#define M_MAOSTATEPOOL_H

// INCLUDE FILES
#include <e32base.h>
#include "taostate.h"

// FORWARD DECLARATIONS

/**
*  State pool.
*  State pool provides possibility to change Always-On state.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
class MAOStatePool
    {
public:

    /**
      * Changes to defined state.
     * New state is returned as a parameter.
     *
     * @since 3.1
     * @param aState: New state
     * @return TAOState*
     */
    virtual TAOState* ChangeState( TAOState::TAOStateName aState ) = 0;
    
    /**
      * Returns current state.
     *
     * @since 3.1
     * @return TAOState* The current state
     */
    virtual TAOState* CurrentState() const = 0;

protected:
    
    /**
     * Prohibit destruction
     *
     * @since 3.1
     */
    virtual ~MAOStatePool(){};
    };

#endif // M_MAOSTATEPOOL_H
