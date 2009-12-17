/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of GenConAgentDialogServer
*
*/


// Generic Connection Dialog Client

#ifndef __GENCONAGENTDIALOGSERVER_H__
#define __GENCONAGENTDIALOGSERVER_H__


// INCLUDES
#include <e32std.h>


// ENUMERATIONS

// Reasons for panic
enum TConnDlgClientPanic
    {
    EConnDlgNotifierNotInitialised,             // Notifier was not initialised
    };


// CONSTANTS

// Version of this API
const TUint KConnDlgMajorVersionNumber = 1;     // Major version number
const TUint KConnDlgMinorVersionNumber = 0;     // Minor version number
const TUint KConnDlgBuildVersionNumber = 102;   // Build version number


IMPORT_C TInt ThreadFunction(TAny* aStarted);


#endif

// End of File
