/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of class MMPMExpiryTimerCallback.
*
*/

#ifndef MPMEXPIRYTIMERCALLBACK_H
#define MPMEXPIRYTIMERCALLBACK_H

// INCLUDES
#include <e32std.h>

/**
* Callback class for timer expiration.
* @since 5.2
*/
class MMPMExpiryTimerCallback
    {
public:
    /**
    * Callback function for handling the timeout.
    * @since 5.2
    */
    virtual void HandleTimedOut() = 0;
    };

#endif // MPMEXPIRYTIMERCALLBACK_H
