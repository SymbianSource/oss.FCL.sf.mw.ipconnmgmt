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
*      Panic function and codes.   
*      
*
*/


#ifndef CONNECTIONUIUTILITIESPANIC_H
#define CONNECTIONUIUTILITIESPANIC_H

// TYPES

/**
* Panic reasons for Connection Ui Utilities.
*/
enum TConnectionUiUtilitiesPanicCodes
    {
    EUnknownCase,
    };


// FUNCTION DECLARATIONS

/**
* Panic the thread.
* @param aReason Reason for the panic.
*/
void Panic( TConnectionUiUtilitiesPanicCodes aPanic );

#endif
