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
* Description:  application class
*     
*
*/


#ifndef CONNECTIONMONITORUIDOCUMENT_H
#define CONNECTIONMONITORUIDOCUMENT_H

// INCLUDES
#include <AknDoc.h>
   
// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*  CConnectionMonitorUiDocument application class.
*/
class CConnectionMonitorUiDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aApp Eikon Application
        */
        static CConnectionMonitorUiDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CConnectionMonitorUiDocument();

    public: // New functions

    private:

        /**
        * EPOC default constructor.
        * @param aApp Eikon Application
        */
        CConnectionMonitorUiDocument(CEikApplication& aApp);
        void ConstructL();

    private:

        /**
        * From CEikDocument, create CConnectionMonitorUiAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();
    };

#endif // CONNECTIONMONITORUIDOCUMENT_H

// End of File

