/*
* Copyright (c) 2004,2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Logging macros
*
*/


#ifndef LOGGER_H
#define LOGGER_H

// INCLUDE FILES
#include <e32std.h>
#include <flogger.h>
#include "alwaysonconfig.hrh"

// CONSTANTS
_LIT( KPath, "pdpcm2" );
_LIT( KFile, "AlwaysOn.txt" );

#ifdef LOGGING_ENABLED

#ifdef LOGGING_MODE_FILE

#define LOG_1( a )\
    RFileLogger::Write( KPath, KFile, EFileLoggingModeAppend, a )
#define LOG_2( a, b )\
    RFileLogger::WriteFormat( KPath, KFile, EFileLoggingModeAppend, a, b )
#define LOG_3( a, b, c )\
    RFileLogger::WriteFormat( KPath, KFile, EFileLoggingModeAppend, a, b, c )
    
#else
    
#define LOG_1( a ) RDebug::Print( a )
#define LOG_2( a, b ) RDebug::Print( a, b )
#define LOG_3( a, b, c ) RDebug::Print( a, b, c )
    
#endif // LOGGING_MODE_FILE

#else

#define LOG_1( a )
#define LOG_2( a, b )
#define LOG_3( a, b, c )

#endif // LOGGING_ENABLED

#endif // LOGGER_H
