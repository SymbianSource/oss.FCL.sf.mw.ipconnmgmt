/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  MSK key event observer interface
*
*/


#ifndef GSCONNSETTINGSMSKOBSERVER_H
#define GSCONNSETTINGSMSKOBSERVER_H

// INCLUDES
#include <e32base.h>

// Middle Softkey control ID.
const TInt KGSConnSettingsMSKControlID = 3;

/**
 *  Interface for MSK label change observers. 
 *
 *  @lib gsconnsettingsplugin.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( MGSConnSettingsMskObserver )
    {
    public:
        /**
         * Notification to this observer to change MSK label if needed
         */
         virtual void CheckMiddleSoftkeyLabelL() = 0;
    };

#endif //GSCONNSETTINGSMSKOBSERVER_H
