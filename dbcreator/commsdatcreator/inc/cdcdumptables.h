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
* Description:   Declaration of the class CDbCreator
*
*/




#ifndef DUMP_TABLES_H
#define DUMP_TABLES_H

// INCLUDE FILES

#include <e32base.h>
#include <s32file.h>

// CLASS DECLARATION

class CMDBSession;

/**
* CDumpTables writes data from CommsDat to CSV format file.
* CDumpTables dumps the CommsDat tables which are not dumped 
* by any other application.
*/
class CDumpTables : public CBase
    {

    public:
    
        /**
         * Two-phase constructor.
         * @return CDumpTables instance.
         */
        static CDumpTables* NewL();

        /**
         * Two-phase constructor.
         * @return CDumpTables instance. Let the instance on the CleanupStack. 
         */
        static CDumpTables* NewLC();
     
        /**
         * Destructor.
         */
        virtual ~CDumpTables();
        
        /**
         * Dumps the unsupported tables to CSV format file.
         * @param outFileName name of the output file with full path. 
         */
        void DoDumpTablesL( const TFileName& aOutFileName );
        
    protected:

        /**
         * Second phase constructor. Leaves on failure.
         */
        void ConstructL();
        
     private:
    
        /**
         * Constructor
         */
        CDumpTables();
        
        /**
         * Template method for dumping one CommsDat table
         * @param class T representing the table which is dumped. 
         */
        template<class T> void DumpTableL( const TDesC* aTableName );

        /**
         * Opens a output file for dumping. It tries to open the file
         * and if it does not exist then a new one created.
         * @param aFileName name of the file. It cannot be NULL.  
         */
        TInt OpenFileL( const TFileName& aFileName );
        
        /**
         * Writes 8 bit descriptor to 16 bit unicode file in CSV format.
         * @param aDes the written text. It gets the ownership.  
         */
        void WriteL( HBufC8* aDes );

        /**
         * Writes 16 bit descriptor to 16 bit unicode file in CSV format.
         * @param aDes the written text. It gets the ownership.  
         */
        void WriteL( HBufC16* aDes );

        /**
         * Transform the parameter field to CSV accepted format.
         * @param aField the field that should be checked and transform
         * to CSV format if it is necessary
         * @return the CSV acceptable format.
         */
        HBufC16* TransformToCSVFormatLC( const HBufC16* aField );
        
        /**
         * Checks the parameter field if transformation to CSV format is
         * necessary.
         * @param aField the field which should be checked.
         * @return ETrue if transformation is necessary else EFalse.
         */
        TBool CheckCSVTransformNeedL( HBufC16* aFiled );


    private:
    
        // Session for CommsDat
        CommsDat::CMDBSession* iSession;
        
        // Fs for file handling
        RFs   iRfs;

        // Output file
        RFileWriteStream iFile;
        
        // Counter for how many double qoutes are in the given text. 
        TInt iNumOfDQuotes;        

    };

#endif DUMP_TABLES_H


