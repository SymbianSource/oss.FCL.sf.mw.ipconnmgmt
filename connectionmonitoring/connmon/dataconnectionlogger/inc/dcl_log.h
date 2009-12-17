/*
* Copyright (c) 2003-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Writes log entries for Data Connection Logger.
*
*/

#ifndef DATACONNECTIONLOGGERLOG_H
#define DATACONNECTIONLOGGERLOG_H

#include <e32std.h>
#include <e32base.h>

#define _DCL_ENTEREXIT_LOGGING
#define _DCL_FUNCTIONAL_LOGGING

#ifdef _DEBUG

    #define LOG(a) a

    #ifdef _DCL_FUNCTIONAL_LOGGING
        #define LOGIT0(s)          {_LIT(temp, s); Log::Printf(temp);}
        #define LOGIT1(s, a1)      {_LIT(temp, s); Log::Printf(temp, a1);}
        #define LOGIT2(s, a1, a2)  {_LIT(temp, s); Log::Printf(temp, a1, a2);}
    #else
        #define LOGIT0(s)
        #define LOGIT1(s, a1)
        #define LOGIT2(s, a1, a2)
    #endif

    #ifdef _DCL_ENTEREXIT_LOGGING
        _LIT(KDclLogEnterFn, "-> %S entered");
        _LIT(KDclLogExitFn,  "<- %S returned");
        _LIT(KDclLogExitFn1, "<- %S returned with <%d>");
        #define LOGENTRFN(s)       {_LIT(temp, s); Log::Printf(KDclLogEnterFn, &temp);}
        #define LOGEXITFN(s)       {_LIT(temp, s); Log::Printf(KDclLogExitFn,  &temp);}
        #define LOGEXITFN1(s, a1)  {_LIT(temp, s); Log::Printf(KDclLogExitFn1, &temp, a1);}
    #else
        #define LOGENTRFN(s)
        #define LOGEXITFN(s)
        #define LOGEXITFN1(s, a1)
    #endif

#else

    #define LOG(a)

    #define LOGIT0(s)
    #define LOGIT1(s, a1)
    #define LOGIT2(s, a1, a2)

    #define LOGENTRFN(s)
    #define LOGEXITFN(s)
    #define LOGEXITFN1(s, a1)

#endif // _DEBUG

#ifdef _DEBUG

#include <flogger.h>

_LIT( KLogFolder,"dcl" );
_LIT( KLogFile,"dcl.txt" );

class Log
    {
public:
    static inline void Write( const TDesC& aDes );
    static inline void Printf( TRefByValue<const TDesC> aFmt, ... );
    };

inline void Log::Write( const TDesC& aDes )
    {
    RFileLogger::Write( KLogFolder(), KLogFile(), EFileLoggingModeAppend, aDes );
    }

inline void Log::Printf( TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat( KLogFolder(), KLogFile(), EFileLoggingModeAppend, aFmt, list );
    }

#endif // _DEBUG
#endif // DATACONNECTIONLOGGERLOG_H

// End of file
