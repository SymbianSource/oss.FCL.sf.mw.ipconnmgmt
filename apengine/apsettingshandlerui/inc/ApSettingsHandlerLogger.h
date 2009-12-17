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
* Description: 
*       Implemented logger functionality of the module
*
*/


#ifndef APSETTINGSHANDLER_LOGGER_H
#define APSETTINGSHANDLER_LOGGER_H



// INCLUDE FILES

#include <flogger.h>
#include <eikenv.h>


// CONSTANTS

const TInt KApSetUIMajorVersion = 4;
const TInt KApSetUIMinorVersion = 0;
const TInt KApSetUIBuildVersion = 16;

_LIT( KApSetUILogMessageAppBanner,   "ApSetUI: (%d.%d.%d) started" );
_LIT( KApSetUILogEnterFn,            "ApSetUI: -> %S" );
_LIT( KApSetUILogLeaveFn,            "ApSetUI: <- %S" );
_LIT( KApSetUILogExit,               "ApSetUI: exit" );
_LIT( KApSetUILogTimeFormatString,   "%H:%T:%S:%*C3" );

// NOTE! We're depending on FLogger DLL, because it assumes the existence
// of C:\LOGS directory.
_LIT( KApSetUIFLoggerDependency,        "c:\\logs\\" );
_LIT( KApSetUIDoubleBackSlash,      "\\" );
_LIT( KApSetUILogDir,                "data" );
_LIT( KApSetUILogFile,               "apui.log" );


    enum TApSetUiLogMask                ///< Log mask bits.
        {
        ELogOff         = 0x00000000,   ///< Don't log.
        EDelete         = 0x00000001,   ///< Log delete activity.
        ESettings       = 0x00000002,   ///< Log Settings
        EListbox        = 0x00000004,   ///< Log listbox activity.
        EModel          = 0x00000008,   ///< Log Model activity.
        EAPI            = 0x00000010,   ///< Log API activity.
        EApImpl         = 0x00000020,   ///< Log Implementation activity.
        EDialog         = 0x00000040,   ///< Log dialog activity.
        EOther          = 0x00000080,   ///< Log other activity.
        /*
        EApItem         = 0x00000040,   ///< Log ApItem activity.
        ECommons        = 0x00000080,   ///< Log transaction activity.
        EActiveDb       = 0x000000F0,   ///< Log active db & notification 
                                        ///< activity.
        */
        ESpecial        = 0x0FFFF000,   ///< Log special, temp stuff only
        ELogAll         = 0xFFFFFFFF    ///< Log all.
        };

    // MACROS

    /// Determines what to log. Construct this from TApSetUiLogMask values.
//    #define APSETUI_LOG_MASK ELogAll
//    #define APSETUI_LOG_MASK    EDelete + EModel + EListbox
//    #define APSETUI_LOG_MASK    EDelete + EModel
// #define APSETUI_LOG_MASK ESettings +EListbox +EModel +EAPI +EApImpl +EDialog +EOther
    #define APSETUI_LOG_MASK    EModel
    

#ifdef __TEST_LOGGING
    #define APSETUILOGGER_CREATE { \
                TFileName path( KApSetUIFLoggerDependency );    \
                path.Append( KApSetUILogDir );                  \
                path.Append( KApSetUIDoubleBackSlash );         \
                RFs& fs = CEikonEnv::Static()->FsSession();     \
                fs.MkDirAll( path );                            \
                RFileLogger::WriteFormat( KApSetUILogDir, KApSetUILogFile,  \
                                          EFileLoggingModeOverwrite,        \
                                          KApSetUILogMessageAppBanner,      \
                                          KApSetUIMajorVersion,             \
                                          KApSetUIMinorVersion,             \
                                          KApSetUIBuildVersion ); }         
    #define APSETUILOGGER_DELETE {                          \
                RFileLogger::Write( KApSetUILogDir,         \
                                    KApSetUILogFile,        \
                                    EFileLoggingModeAppend, \
                                    KApSetUILogExit ); }
    #define APSETUILOGGER_ENTERFN( mask, a ) {                              \
                _LIT( temp, a );                                            \
                if (mask & APSETUI_LOG_MASK)                                \
                    {                                                       \
                    RFileLogger::WriteFormat( KApSetUILogDir,               \
                                              KApSetUILogFile,              \
                                              EFileLoggingModeAppend,       \
                                              KApSetUILogEnterFn, &temp );  \
                    } }
    #define APSETUILOGGER_LEAVEFN( mask, a ) {                              \
                _LIT( temp, a );                                            \
                if (mask & APSETUI_LOG_MASK)                                \
                    {                                                       \
                    RFileLogger::WriteFormat( KApSetUILogDir,               \
                                              KApSetUILogFile,              \
                                              EFileLoggingModeAppend,       \
                                              KApSetUILogLeaveFn, &temp );  \
                    } }
    #define APSETUILOGGER_WRITE( mask, a ) {                                \
                _LIT( temp, a );                                            \
                if (mask & APSETUI_LOG_MASK)                                \
                    {                                                       \
                    RFileLogger::Write( KApSetUILogDir,                     \
                                        KApSetUILogFile,                    \
                                    EFileLoggingModeAppend, temp );         \
                    } }
    #define APSETUILOGGER_WRITE_FORMAT( mask, a, b ) {                      \
                _LIT( temp, a );                                            \
                if (mask & APSETUI_LOG_MASK)                                \
                    {                                                       \
                    RFileLogger::WriteFormat( KApSetUILogDir,               \
                                              KApSetUILogFile,              \
                                              EFileLoggingModeAppend,       \
                                              temp, b );                    \
                    } }
    #define APSETUILOGGER_WRITE_TIMESTAMP( mask, a ) {                      \
                _LIT( temp, a );                                            \
                if (mask & APSETUI_LOG_MASK)                                \
                    {                                                       \
                    TTime time; TBuf<48> buf; time.HomeTime();              \
                    time.FormatL( buf, KApSetUILogTimeFormatString );       \
                    buf.Insert( 0, temp );                                  \
                    RFileLogger::Write( KApSetUILogDir, KApSetUILogFile,    \
                    EFileLoggingModeAppend, buf);                           \
                    } }
    #define APSETUILOGGER_WRITE_BUF( mask, a ) {                            \
                if (mask & APSETUI_LOG_MASK)                                \
                    {                                                       \
                    RFileLogger::Write( KApSetUILogDir, KApSetUILogFile,    \
                                        EFileLoggingModeAppend, a);         \
                    } }
#else
    #define APSETUILOGGER_CREATE {}
    #define APSETUILOGGER_DELETE {}
    #define APSETUILOGGER_ENTERFN( mask, a ) {}
    #define APSETUILOGGER_LEAVEFN( mask, a ) {}
    #define APSETUILOGGER_WRITE( mask, a ) {}
    #define APSETUILOGGER_WRITE_FORMAT( mask, a, b ) {}
    #define APSETUILOGGER_WRITE_TIMESTAMP( mask, a ) {}
    #define APSETUILOGGER_WRITE_BUF( mask, a ) {}
#endif // __TEST_LOGGING

#endif // APSETTINGSHANDLER_LOGGER_H

// End of file
