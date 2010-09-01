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
* Description:  CConnectionMonitorUiApp application class
*     
*
*/


#ifndef CONNECTIONMONITORUIAPP_H
#define CONNECTIONMONITORUIAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidConnectionMonitorUi = { 0x101F84D0 };

const TInt KErrUnknownConnectionState   = -50000;
const TInt KErrCorruptedBearerType      = -50001;

/**
* Global panic function
*/
extern void Panic( TInt aReason );

// CLASS DECLARATION

/**
* CConnectionMonitorUiApp application class.
* Provides factory to create concrete document object.
* 
*/
class CConnectionMonitorUiApp : public CAknApplication
    {
    public:

    public: // Functions from base classes

    private:

        /**
        * From CApaApplication, creates CConnectionMonitorUiDocument
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID
        * @return The value of KUidConnectionMonitorUi.
        */
        TUid AppDllUid() const;

    };

#endif  // CONNECTIONMONITORUIAPP_H

// End of File

