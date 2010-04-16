/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Declaration of cdccommsdatcreator class.
*
*/




#ifndef COMMSDAT_CREATOR_H
#define COMMSDAT_CREATOR_H

// INCLUDE FILES
#include <e32base.h>
#include <bacline.h>
#include <f32file.h>

#include <cmmanagerext.h>


// CONSTANTS


// DATA TYPES


// FORWARD DECLARATIONS
class RCmConnectionMethodExt;
class RCmDestinationExt;
class CReaderBase;
class CProcessorBase;
class CEapTypeElement;
class CLingerElement;

// CLASS DECLARATION


/**
* Creates CommsDat database from the input XML file.
* Writes the content of the CommsDat to CSV output file.
*/
class CCdcCommsDatCreator: public CBase
    {

    public:

        /** Epoc constructors */
        IMPORT_C static CCdcCommsDatCreator* NewL();
        IMPORT_C static CCdcCommsDatCreator* NewLC();
    
        IMPORT_C void CreateCommsDatL( const TFileName& aInFileName );

        IMPORT_C void DumpCommsDatL( const TFileName& aOutFileName );

        IMPORT_C void CreateTablesL();


        /** Destructor */
        virtual ~CCdcCommsDatCreator();
        
    private:
    
        /** First stage constructor */
        CCdcCommsDatCreator();
    
        /** Epoc constructor */
        void ConstructL(); // second-phase constructor
        
        /**
        * InputFileType
        * @return TDbCreatorInputFile
        *    EFileUnknown     ///< Unrecognized
        *    EFileCSV         ///< CSV file
        *    EFileXML         ///< XML file
        */
        TUint InputFileType ( const TFileName& aInFileName );

        void LogIapIdsL();
                      
        void UpdateUncatCmsL( RArray< TBool >& aCmInDest );
                      
        template<class T> void ProcessPluginL();
        
        void ProcessDNL( RArray< TBool >& aCmInDest, TBool aFirstRound );

        void SetUnderlyingIapL( RPointerArray< HBufC >& aUnderLying );

        void SetDefaultConnectionL( RPointerArray< HBufC >& aDefCon );
        
        void SetWlanSecurityL( RPointerArray< HBufC >& aWlanSecurity );
        
        void SetLingerSettingsL( RPointerArray< CLingerElement >& aLingerSettings );
                      
        TBool iIsWlanS;
        
        RPointerArray< RCmDestinationExt > iDestArray;
        RPointerArray< HBufC > iDestNames;
        RPointerArray< RCmConnectionMethodExt > iPluginArray;
        RPointerArray< HBufC > iPluginNames;
        RPointerArray< CEapTypeElement > iEapSettings;
		RPointerArray< HBufC > iUnderLying;
    	RPointerArray< HBufC > iDefCon;
    	RPointerArray< HBufC > iWlanSecurity;
    	RPointerArray< CLingerElement > iLingerSettings;
    	RArray< TBool > iCmInDest;
    
        RCmManagerExt iCmManager;
        
        CReaderBase* iReader;

        CProcessorBase* iFileProcessor;

    };

#endif   // COMMSDAT_CREATOR_H
            
