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
* Description:   Declaration of the class CDataMobilityTables.
*
*/




#ifndef DATAMOBILITYTABLESHANDLER_H
#define DATAMOBILITYTABLESHANDLER_H


// INCLUDES
#include <e32base.h>
#include <metadatabase.h>

using namespace CommsDat;

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* CDataMobilityTables
* Capable of checking the existence of certain Data Mobility specific 
* tables and - if necessary create them.
*/
class CDataMobilityTables : public CBase
    {   
    public:

        /**
        * Static constructor
        */
        static CDataMobilityTables* NewL();

        /**
        * Destructor
        */
        virtual ~CDataMobilityTables();

        /**
        * Checks if Data Mobility ALR tables exist. Creates them if not.
        */
        void CheckAndCreateAlrTablesL();
        
        /**
        * Checks if Data Mobility MIP tables exist. Creates them if not.
        */
        void CheckAndCreateMipTablesL();


    private:

        /**
        * C++ default constructor.
        */
        CDataMobilityTables();

        /**
        * By default Symbian second phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Check if Global Bearer Type Priorization Table exists in CommsDat;
        * create if not.
        */
        void CreateGlobalBearerTypeTableL();
        
        /**
        * Check if Data Mobility Selection Policy Table exists in CommsDat;
        * create if not.
        */
        void CreateDataMobilitySelectionPolicyTableL();
        
        /**
        * Check if Virtual IAP Next Layer Table exists in CommsDat;
        * create if not.
        */
        void CreateVirtualIAPNextLayerTableL();

        /**
        * Check if MIP Service Table exists in CommsDat; create if not.
        */
        void CreateMIPServiceTableL();
        
        /**
        * Check if MIP Service Table exists in CommsDat; create if not.
        */
        void CreateMIP4TableL();
        
        /**
        * Check if MIP Service Table exists in CommsDat; create if not.
        */
        void CreateMIP6TableL();
        
        /**
        * Check if SNAP Metadata Table exists in CommsDat; create if not.
        */
        void CreateSNAPMetadataTableL();
        
        /**
        * Check if IAP Metadata Table exists in CommsDat; create if not.
        */
        void CreateIAPMetadataTableL();
        
        /**
        * Check if WLAN Destination network Table exists in CommsDat; 
        * create if not.
        */
        void CreateDestNetworkTableL();
        
        /**
        * Populates the Global Bearer Priorization table.
        */
        void PopulateGlobalBearerTypeTableL();
        
    private:    // Data
    
        //@var session - Owned
        CMDBSession* iSession;
        
    };

#endif      // DATAMOBILITYTABLESHANDLER_H

// End of File.
