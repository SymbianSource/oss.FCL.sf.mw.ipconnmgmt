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
* Description:  Declaration of WLAN data holder array for WLAN plugin
*
*/

#ifndef WLAN_DATA_ARRAY_H
#define WLAN_DATA_ARRAY_H

// INCLUDES
#include    <e32base.h>

class CCmPluginWlanData;

/**
 *  WLAN Plugin data holder class
 *  A list of CCmPluginWlanData-s pointers; items are owned.
 *
 *  ?more_complete_description
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CCmPluginWlanDataArray ) : public CArrayPtrFlat<CCmPluginWlanData>
    {

    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CCmPluginWlanDataArray();

        /**
        * Destructor. Items in the list are destroyed.
        */
        virtual ~CCmPluginWlanDataArray();

	};

#endif // WLAN_DATA_ARRAY_H
