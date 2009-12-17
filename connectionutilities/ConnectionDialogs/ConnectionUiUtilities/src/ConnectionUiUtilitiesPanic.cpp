/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*      Implementation of panic function.   
*      
*
*/


// INCLUDE FILES

#include <e32std.h>
#include "ConnectionUiUtilitiesPanic.h"


// ================= LOCAL FUNCTIONS =======================

// ---------------------------------------------------------
// Panic()
// ---------------------------------------------------------
//
void Panic( TConnectionUiUtilitiesPanicCodes aPanic )
    {
    _LIT( KConnUiUtilsSet, "ConnectionUiUtilities" );
    User::Panic( KConnUiUtilsSet, aPanic );
    }


