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
* Description:  Implementation of WLan network data holder class
*
*/


// INCLUDE FILES

#include "cmpluginwlandata.h"
#include "cmpluginwlandataarray.h"


// CONSTANTS

LOCAL_D const TInt KGranularity = 4;    ///< Granularity of the list.

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
// ---------------------------------------------------------
// CCmPluginWlanDataArray::CCmPluginWlanDataArray
// ---------------------------------------------------------
//
CCmPluginWlanDataArray::CCmPluginWlanDataArray()
: CArrayPtrFlat<CCmPluginWlanData>( KGranularity )
    {
    }

// Destructor
// ---------------------------------------------------------
// CCmPluginWlanDataArray::~CCmPluginWlanDataArray
// ---------------------------------------------------------
//
CCmPluginWlanDataArray::~CCmPluginWlanDataArray()
    {

    ResetAndDestroy();

    }


//  End of File
