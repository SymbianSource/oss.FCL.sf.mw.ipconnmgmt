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
* Description:   Declaration of the class CWlanTables.
*
*/




#ifndef WLANTABLESHANDLER_H
#define WLANTABLESHANDLER_H


// INCLUDES
#include <e32base.h>
#include <metadatabase.h>
#include <WlanCdbCols.h>
#include <commsdattypesv1_1.h>

using namespace CommsDat;

// FORWARD DECLARATIONS

// CONSTANTS

// CLASS DECLARATION

/**
* CWlanTables
* Capable of checking the existence of certain WLAN specific 
* tables and - if necessry create them.
*/
class CWlanTables : public CBase
    {   
    public:

        /**
        * Static constructor
        */
        static CWlanTables* NewL();

        /**
        * Destructor
        */
        virtual ~CWlanTables();

        /**
        * Checks if WLAN tables exist. Creates them if not.
        */
        void CheckAndCreateTablesL();


    private:

        /**
        * C++ default constructor.
        */
        CWlanTables();

        /**
        * By default Symbian second phase constructor is private.
        */
        void ConstructL();  

        /**
        * Check if WLAN Service Table exists in CommsDat; create if not.
        */
        void CreateWlanServiceTableL();

        /**
        * Check if WLAN Device Table exists in CommsDat; create if not.
        */
        void CreateWlanDeviceSettingsTableL();

        /**
        * Check if WLAN Secondary SSID Table exists in CommsDat; create if not.
        */
        void CreateWlanSecSSIDTableL();

        /**
        * Check if WLAN bearer record exists in LANBearer table in CommsDat; create if not.
        */	
		void CreateWlanBearerRecordL();



    private:    // Data
    
        //@var session - Owned
        CMDBSession* iSession;
    };

#endif      // WLANTABLESHANDLER_H

// End of File.
