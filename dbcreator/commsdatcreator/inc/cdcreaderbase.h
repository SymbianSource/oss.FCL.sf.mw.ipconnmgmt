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
* Description:   Declaration of the class CReaderBase.
*
*/




#ifndef READER_BASE_H
#define READER_BASE_H


// INCLUDE FILES
#include <e32base.h>
#include <f32file.h>
#include <flogger.h>

#include "cdccommon.h"
#include "cdcerrors.h"
#include "cdctagcontainer.h"

// CLASS DECLARATION

/**
* CReaderBase.
* Base class for XML and CSV reader.
*/
class CReaderBase : public CBase
    {
    
    public:
        /**
        * Destructor.
        */
        virtual ~CReaderBase();
        
    protected:

        /**
        * Default constructor.
        * @param TDbCreatorInputFile input file type (csv or xml)
        * @param TDbCreatorFeatures feature type - relevant in case 
        * of csv input as feature is not detected by the reader.
        */
        CReaderBase( TDbCreatorInputFile aFileType, 
                     TDbCreatorFeatures aFeature );
        
        /**
        * Second phase constructor. Leaves on failure.
        * @param aInFileName Name of the input file.
        */
        void ConstructL( const TDesC& aInFileName );
        
    public:
    
        /**
        * Locates the next feature in input file.
        */
        virtual void LocateFeatureL() = 0;
           
        /**
        * Gives back the next line from the given buffer.
        * @return EFalse if the end of buffer is accessed, ETrue otherwise.
        */
        virtual TBool LocateAPL() = 0;
        
        /**
        * Parses a line of the input file.
        */
        virtual void ParseAPL() = 0;
        
        /**
        * Resets the actual file position to the beginning of the file.
        * @return One of the system-wide error codes if reset failed.
        */
        virtual TInt Reset() = 0;
           
        /**
        * Writes log to the log file.
        */
        virtual void ToDoLog() = 0;
        
        /**
        * Returns reference to the TagContainer
        * @return CTagContainer
        */
        CTagContainer& TagContainer();
        
        /**
        * Returns reference to the input file
        * @return RFile
        */   
        RFile& InputFile();
        
        /**
        * Returns reference to the logger
        * @return RFileLogger
        */   
        RFileLogger& FileLogger();
        
        /**
        * Checks if filed ID is read from the input file (otherwise it is 
        * calculated by reader according to the index of the tag)
        * @return TRUE if reader reads the field id
        */
        TBool FieldIDPresent();
        
        /**
        * Returns the feature currently being parsed
        * @return TDbCreatorFeatures
        *    - EFeatureNone    - No feature found
        *    - EFeatureAP      - Internet access point
        *    - EFeatureWLAN    - WLAN access point
        *    - EFeatureVPN     - VPN access point
        */
        TDbCreatorFeatures CurrentFeature();

    protected:
        // @var marks the found feature that is being processed
        TDbCreatorFeatures    iCurrentFeature;
                                            
    private:
    
        // @var contains the values read from input file
        CTagContainer*        iTagContainer;      
        // @var for file handling.                                
        RFs                   iFs;
        // @var Input file.
        RFile                 iInputFile; 
        // @var Log file.
        RFileLogger           iLogFile; 
        // @var Input file type
        TDbCreatorInputFile   iFileType;  
    
    };



#endif // READER_BASE_H

// End of File.
