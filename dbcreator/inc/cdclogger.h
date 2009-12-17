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
* Description:   contains logger macros
*
*/




#ifndef LOGGER_H
#define LOGGER_H


// INCLUDE FILES

#include <e32std.h>
#include <e32def.h>


#ifdef _DEBUG

#include <flogger.h>

#include "cdclogfilename.h"


#define CREATE_LOG                              \
    {                                           \
    User::LeaveIfError( iLogFile.Connect() );   \
    iLogFile.CreateLog( KLogDir, KLogFile , EFileLoggingModeAppend ); \
    }                                           \

#define CLOSE_LOG       \
    {                   \
    iLogFile.Close();   \
    }                   \


/// Writes to the log file the string 'a'.
#define CLOG_WRITE( a )             \
    {                               \
    _LIT16( temp, a );              \
    RFileLogger::Write              \
        (                           \
        KLogDir,                    \
        KLogFile,                   \
        EFileLoggingModeAppend,     \
        temp                        \
        );                          \
    }


/// Writes to the log file formatted in unicode.
#define CLOG_WRITE_FORMAT( a, b )   \
    {                               \
    _LIT( temp, a );                \
    RFileLogger::WriteFormat        \
        (                           \
        KLogDir,                    \
        KLogFile,                   \
        EFileLoggingModeAppend,     \
        temp,                       \
        b                           \
        );                          \
    }

                                                    
#else /* not defined __DEBUG__*/

/// Empty definition (disable log).
#define CLOG_WRITE( a )

/// Empty definition (disable log).
#define CLOG_WRITE_FORMAT( a, b )

#define CREATE_LOG                              \

#define CLOSE_LOG                              \


#endif /* def _DEBUG */

#endif /* def LOGGER_H */


// End of File.