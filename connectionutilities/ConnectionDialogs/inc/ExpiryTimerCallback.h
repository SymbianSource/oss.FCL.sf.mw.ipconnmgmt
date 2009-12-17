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
* Description:  Declaration of class CExpiryTimerCallback 
*
*/


#ifndef CEXPIRYTIMERCALLBACK_H
#define CEXPIRYTIMERCALLBACK_H

// INCLUDES
#include <e32std.h>

// CLASS DECLARATION
/**
 * Callback class for timer expiration
 */

class MExpiryTimerCallback
    {
public:
    virtual void HandleTimedOut() = 0;
    };

#endif // CEXPIRYTIMERCALLBACK_H
