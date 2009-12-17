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
* Description:  Defines MAOGpdsObserver interface
*
*/


#ifndef M_MAOGPDSOBSERVER_H
#define M_MAOGPDSOBSERVER_H

// INCLUDE FILES

// FORWARD DECLARATIONS

/**
*  Interface through succesful always on mode detting to TSY is
*  notified.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.x not used yet
*/
class MAOGpdsObserver
    {
public: // New methods

    /**
     * Handles always on mode set notification.
     *
     * @since S60 v3.x not used yet
     * @param aError: Success code
     */
    virtual void HandleAlwaysOnModeSetL( TInt aError ) = 0;

protected:

    /**
     * Destructor
     *
     * @since S60 v3.x not used yet
     */
    virtual ~MAOGpdsObserver(){};
    };

#endif // M_MAOGPDSOBSERVER_H
