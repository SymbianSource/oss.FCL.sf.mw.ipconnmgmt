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
* Description:  Defines MAORAUManagerObserver interface
*
*/


#ifndef M_MAORAUMANAGEROBSERVER_H
#define M_MAORAUMANAGEROBSERVER_H

// INCLUDE FILES

// FORWARD DECLARATIONS

/**
*  RAU manager observer interface.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
class MAORAUManagerObserver
    {
public: // Data types

    // RAU type
    enum TRAUType
        {
        ENormal,
        EPeriodical
        };

public: // New methods

    /**
     * Succesfull RAU is notified through this method.
     *
     * @since S60 v3.1
     * @param aType: RAU type
     */
    virtual void HandleSuccesfulRAUL( TRAUType aType ) = 0;

protected:

    /**
     * Prohibit destruction
     *
     * @since S60 v3.1
     */
    virtual ~MAORAUManagerObserver(){};
    };

#endif // M_MAORAUMANAGEROBSERVER_H
