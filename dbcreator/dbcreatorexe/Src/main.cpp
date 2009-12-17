/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Entry point of DbCreator 
*
*/



// INCLUDE FILES
#include <e32test.h>
#include <e32base.h>
#include <bacline.h>
#include <f32file.h>

#include "cdcdbcreator.h"
#include "cdcerrors.h"
#include "cdclogger.h"



/**
* Entry point for the test program. Runs test under a TRAP harness.
*/
GLDEF_C TInt E32Main()
    {
    TInt result( KErrNone );
    CDbCreator* dbc = new CDbCreator;
    if (dbc == NULL)
        {
        result = KErrNoMemory;
        return result;
        }
    
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TRAP( result, dbc->DoStartL() );
    delete cleanup;
               
    CLOG_WRITE_FORMAT( "\t result: %d \n", result )
    switch ( result )
        {
        case KErrNone: 
            CLOG_WRITE( "No error" )
            break;
        case KErrNotSupported:
        	CLOG_WRITE( "! Error : Not supported feature found" )
        	break;
        case KErrAlreadyExists:
            CLOG_WRITE( "! Error : Access point already exists" )
            break;
        case KErrNotFound:
            CLOG_WRITE( "! Error : File not found" )
            break;
        case KErrInvalidContent:
            CLOG_WRITE( "! Error : Invalid content found" )
            break;
        case KErrAssertionFailed:
            CLOG_WRITE( "! Error : Assertion failed" )
            break;
        case KErrFileCannotBeOpened:
            CLOG_WRITE( "! Error : Cannot open file\n" )
            break;
        case KErrWLANTablesMissing:
            CLOG_WRITE( "! Error : WLAN tables are missing\n" )
            break;
        case KErrInvalidParameter:
            CLOG_WRITE( "! Error : Invalid parameter\n" )
            break;
        case KErrInvalidExtension:
            CLOG_WRITE( "! Error : File extension must be .csv or .xml\n" );
            break;
        default:
            CLOG_WRITE( "! Error : Unknown error" )
            break;
        }
    delete dbc;
    return result;
    }
