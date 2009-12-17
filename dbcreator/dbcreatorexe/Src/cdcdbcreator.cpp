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
* Description:   Module for creating CommsDat from an input file
*
*/




// INCLUDE FILES
#include <e32def.h>
#include <e32base.h>
#include <s32strm.h>
#include <s32file.h>

#include "cdcdbcreator.h"
#include "cdcerrors.h"
#include "cdccommsdatcreator.h"


// CONSTANTS

// Length of the characters "f:"
const TUint KFParamLength = 2;

// Default input file for db creating
_LIT16( KInFile, "C:\\Data\\DbCreator\\VariantData.xml" );
// Default output file for dumping
_LIT16( KOutFile, "C:\\Data\\DbCreator\\DumpFile.csv" );


void CDbCreator::DoStartL()
    {
    //process command line arguments
    //------------------------------
    TBool extensionOnly( EFalse );           //Only extend CommsDat
    TBool dump( EFalse );                    //Dump is needed not creation
    
    CCommandLineArguments* args = CCommandLineArguments::NewLC();

    TFileName fileName;
    ParseCommandLineLC( args, fileName, extensionOnly, dump );


    CCdcCommsDatCreator* cdc = CCdcCommsDatCreator::NewLC();
                                            
    //checks filename argument
    if ( fileName.Length() == 0 )
        {
        if( dump )
            {
            fileName.Copy( KOutFile );
            }
        else
            {
            fileName.Copy( KInFile );
            }
        }
                                            
    //calls the appropriate function.
    if( dump )
        {
        cdc->DumpCommsDatL( fileName );
        }
    else if( extensionOnly ) 
        {
        cdc->CreateTablesL();    
        }
    else
        {
        cdc->CreateTablesL();
        cdc->CreateCommsDatL( fileName );    
        }
        
    CleanupStack::PopAndDestroy( 2, args);  // args, cdc
    }



//------------------------------------------------
// CDbCreator::ParseCommandLineLC
//------------------------------------------------
//

void CDbCreator::ParseCommandLineLC( CCommandLineArguments* aArgs,
                                     TFileName& aFileName,   
                                     TBool& aExt,
                                     TBool& aDump )
    {
    TPtrC arg;

    if ( aArgs->Count() > 1 )
        {

        for ( TInt i = 1; i < aArgs->Count(); i++ )
            {
            arg.Set( aArgs->Arg( i ) );
            
            if( arg.Size() > 0 )
                {
                switch( arg[0] )
                    {
                    case 'd':
                    case 'D'://fall-through 
                        // D means "dump" needed
                        aDump = ETrue;
                        break;
                    case 'f':
                    case 'F'://fall-through 
                        {
                        _LIT( KHeader,  "f:" );
                        if ( 0 == arg.FindF( KHeader ) )
                            {
                            //f: must be in the head position of file location
                            aFileName.Copy( arg.Right( arg.Length() - KFParamLength ) );
                            }
                        else
                            {
                            User::Leave( KErrInvalidParameter );    
                            }
                        
                        break;
                        }
                    // Only the CommsDat will be extended
                    case 'E':
                    case 'e'://fall-through 
                        {    
                        aExt = ETrue;    
                        break;
                        }
                    default:
                        {
                        User::Leave( KErrInvalidParameter );
                        }
                    }//swich
                }//if
            }//for
        }//if

    }         
// End of File.
