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




#ifndef C_CDCDBCREATOR_H
#define C_CDCDBCREATOR_H

// INCLUDE FILES
#include <e32base.h>
#include <bacline.h>
#include <f32file.h>

/**
* CsvProcessor creates data to the CommsDb from the CSV file.
*/
class CDbCreator : public CBase
    {

    public:
    
        /**
        * Creates the internet access points and creates output file.
        */
        void DoStartL();
     
          
    private:
    
        /**
        * ParseCommandLineLC
        * Gets parameters from command line. If some parameters are not
        * supplied in the command line, they are untouched.
        * @param aArgs, command line arguments.
        * @param aFileName, xml file name.
        * @param aExt, will the CommsDat be extended or not.
        * @param aDump, dump or not.
        */
        
        void ParseCommandLineLC( CCommandLineArguments* aArgs,
                                         TFileName& aFileName,   
                                         TBool& aExt,
                                         TBool& aDump );

    };


#endif   // C_CDCDBCREATOR_H
            
