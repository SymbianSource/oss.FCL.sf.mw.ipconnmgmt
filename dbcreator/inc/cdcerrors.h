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
* Description:   Declaration of the error codes of the DbCreator.
*
*/




#ifndef DBCREATOR_ERRORS_H
#define DBCREATOR_ERRORS_H

// INCLUDE FILES

#include <e32base.h>

// The potential errors.
const TInt  KErrApIsNotCreated         = -600;
const TInt  KErrGivenRealIdNotExists   = -601;
const TInt  KErrGivenRealNameNotExists = -602;
const TInt  KErrTooMuchTag             = -603;
const TInt  KErrInvaildPolicyId        = -604;
const TInt  KErrComment                = -605;
const TInt  KErrInvalidContent         = -606;
const TInt  KErrAssertionFailed        = -607;
const TInt  KErrFileCannotBeOpened     = -608;
const TInt  KErrWLANTablesMissing      = -609;
const TInt  KErrInvalidParameter       = -610;
const TInt  KErrInvalidExtension       = -611;

//Panic codes
const TInt  KErrInvalidCase            = -700;
const TInt  KErrOutOfRange             = -701;

//Panic texts
_LIT16( KPanicInvalidCase,         "Invalid case" );
_LIT16( KPanicOutOfRange,          "Value is out of range" );


#endif // DBCREATOR_ERRORS_H


// End of File.