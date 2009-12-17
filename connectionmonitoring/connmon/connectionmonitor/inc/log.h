/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Writes log entries for Connection Monitor Server.
*
* To take ConnMon logs, include the debug version of connmon.dll to rom image.
* On the phone, create folder C:\logs\connmon.
*
*/

#ifndef CONNECTIONMONITORLOG_H
#define CONNECTIONMONITORLOG_H

// Log output
// 1 = RDebug
// 2 = File
#define _CONNMON_LOG_OUTPUT 2

#define _CONNMON_ENTEREXIT_LOGGING
#define _CONNMON_FUNCTIONAL_LOGGING
//#define _CONNMON_TIMING_LOGGING
//#define _CONNMON_HEXDUMP_LOGGING // Only for file logging

#include <e32std.h>
#include <e32base.h>

#if ( _CONNMON_LOG_OUTPUT == 1 )
#include <e32debug.h>
#endif

#ifdef _DEBUG

    #define LOG(a) a

    #ifdef _CONNMON_FUNCTIONAL_LOGGING
      #if ( _CONNMON_LOG_OUTPUT == 2 )
        #define LOGIT(a)                    {_LIT(temp, a); Log::Printf(temp);}
        #define LOGIT1(a,a1)                {_LIT(temp, a); Log::Printf(temp,a1);}
        #define LOGIT2(a,a1,a2)             {_LIT(temp, a); Log::Printf(temp,a1,a2);}
        #define LOGIT3(a,a1,a2,a3)          {_LIT(temp, a); Log::Printf(temp,a1,a2,a3);}
        #define LOGIT4(a,a1,a2,a3,a4)       {_LIT(temp, a); Log::Printf(temp,a1,a2,a3,a4);}
        #define LOGIT5(a,a1,a2,a3,a4,a5)    {_LIT(temp, a); Log::Printf(temp,a1,a2,a3,a4,a5);}
        #define LOGIT6(a,a1,a2,a3,a4,a5,a6) {_LIT(temp, a); Log::Printf(temp,a1,a2,a3,a4,a5,a6);}
        #define LOGIT11(a,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11) {_LIT(temp, a); Log::Printf(temp,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11);}
      #else
        #define LOGIT(a)                    {_LIT(temp, a); RDebug::Print(temp);}
        #define LOGIT1(a,a1)                {_LIT(temp, a); RDebug::Print(temp,a1);}
        #define LOGIT2(a,a1,a2)             {_LIT(temp, a); RDebug::Print(temp,a1,a2);}
        #define LOGIT3(a,a1,a2,a3)          {_LIT(temp, a); RDebug::Print(temp,a1,a2,a3);}
        #define LOGIT4(a,a1,a2,a3,a4)       {_LIT(temp, a); RDebug::Print(temp,a1,a2,a3,a4);}
        #define LOGIT5(a,a1,a2,a3,a4,a5)    {_LIT(temp, a); RDebug::Print(temp,a1,a2,a3,a4,a5);}
        #define LOGIT6(a,a1,a2,a3,a4,a5,a6) {_LIT(temp, a); RDebug::Print(temp,a1,a2,a3,a4,a5,a6);}
        #define LOGIT11(a,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11) {_LIT(temp, a); RDebug::Print(temp,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11);}
      #endif
    #else
      #define LOGIT(a)
      #define LOGIT1(a,a1)
      #define LOGIT2(a,a1,a2)
      #define LOGIT3(a,a1,a2,a3)
      #define LOGIT4(a,a1,a2,a3,a4)
      #define LOGIT5(a,a1,a2,a3,a4,a5)
      #define LOGIT6(a,a1,a2,a3,a4,a5,a6)
      #define LOGIT11(a,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11)
    #endif

    #ifdef _CONNMON_ENTEREXIT_LOGGING
      #if ( _CONNMON_LOG_OUTPUT == 2 )
        _LIT(KConnMonLogEnterFn, "-> %S entered");
        _LIT(KConnMonLogExitFn,  "<- %S returned");
        _LIT(KConnMonLogExitFn1, "<- %S returned <%d>");
        #define LOGENTRFN(a)     {_LIT(temp, a); Log::Printf(KConnMonLogEnterFn, &temp);}
        #define LOGEXITFN(a)     {_LIT(temp, a); Log::Printf(KConnMonLogExitFn, &temp);}
        #define LOGEXITFN1(a,a1) {_LIT(temp, a); Log::Printf(KConnMonLogExitFn1, &temp, a1);}
      #else
        _LIT(KConnMonLogEnterFn, "ConnMon: -> %S entered");
        _LIT(KConnMonLogExitFn,  "ConnMon: <- %S returned");
        _LIT(KConnMonLogExitFn1, "ConnMon: <- %S returned <%d>");
        #define LOGENTRFN(a)     {_LIT(temp, a); RDebug::Print(KConnMonLogEnterFn, &temp);}
        #define LOGEXITFN(a)     {_LIT(temp, a); RDebug::Print(KConnMonLogExitFn, &temp);}
        #define LOGEXITFN1(a,a1) {_LIT(temp, a); RDebug::Print(KConnMonLogExitFn1, &temp, a1);}
      #endif
    #else
      #define LOGENTRFN(a)
      #define LOGEXITFN(a)
      #define LOGEXITFN1(a,a1)
    #endif

    #ifdef _CONNMON_TIMING_LOGGING
      #if ( _CONNMON_LOG_OUTPUT == 2 )
        _LIT(KCMLogTimeStart, "TIMING: %S start");
        _LIT(KCMLogTimeEnd, "TIMING: %S took %d ms");
        #define LOGTIMINGSTART(a) \
            {/*_LIT(s, a); Log::Printf(KCMLogTimeStart, &s);*/} \
            TTime _connmon_start; \
            TTime _connmon_end; \
            TTimeIntervalMicroSeconds _connmon_dur; \
            _connmon_start.UniversalTime();

        #define LOGTIMINGEND(a) \
            _connmon_end.UniversalTime(); \
            _connmon_dur = _connmon_end.MicroSecondsFrom( _connmon_start ); \
            {_LIT(s,a); Log::Printf(KCMLogTimeEnd, &s, (TInt)((_connmon_dur.Int64()+500)/1000));}
      #else
        _LIT(KCMLogTimeStart, "ConnMon: TIMING: %S start");
        _LIT(KCMLogTimeEnd, "ConnMon: TIMING: %S took %d ms");
        #define LOGTIMINGSTART(a) \
            {/*_LIT(s, a); RDebug::Print(KCMLogTimeStart, &s);*/} \
            TTime _connmon_start; \
            TTime _connmon_end; \
            TTimeIntervalMicroSeconds _connmon_dur; \
            _connmon_start.UniversalTime();

        #define LOGTIMINGEND(a) \
            _connmon_end.UniversalTime(); \
            _connmon_dur = _connmon_end.MicroSecondsFrom( _connmon_start ); \
            {_LIT(s,a); RDebug::Print(KCMLogTimeEnd, &s, (TInt)((_connmon_dur.Int64()+500)/1000));}
      #endif
    #else
      #define LOGTIMINGSTART(a)
      #define LOGTIMINGEND(a)
    #endif

#else

    #define LOG(a)

    #define LOGIT(a)
    #define LOGIT1(a,a1)
    #define LOGIT2(a,a1,a2)
    #define LOGIT3(a,a1,a2,a3)
    #define LOGIT4(a,a1,a2,a3,a4)
    #define LOGIT5(a,a1,a2,a3,a4,a5)
    #define LOGIT6(a,a1,a2,a3,a4,a5,a6)
    #define LOGIT11(a,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11)

    #define LOGENTRFN(a)
    #define LOGEXITFN(a)
    #define LOGEXITFN1(a,a1)

    #define LOGTIMINGSTART(a)
    #define LOGTIMINGEND(a)

#endif

#ifdef _DEBUG
#if ( _CONNMON_LOG_OUTPUT == 2 )

#include <flogger.h>

_LIT( KLogFolder,"connmon" );
_LIT( KLogFile,"connmon.txt" );

class Log
    {
public:
    static inline void Write( const TDesC& aDes);
    static inline void Printf( TRefByValue<const TDesC> aFmt, ...);
    static inline void HexDump(
            const TText* aHeader,
            const TText* aMargin,
            const TUint8* aPtr,
            TInt aLen );
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

inline void Log::HexDump(
        const TText* aHeader,
        const TText* aMargin,
        const TUint8* aPtr,
        TInt aLen )
    {
    RFileLogger::HexDump(
            KLogFolder(),
            KLogFile(),
            EFileLoggingModeAppend,
            aHeader,
            aMargin,
            aPtr,
            aLen );
    }

#endif // _CONNMON_LOG_OUTPUT
#endif // _DEBUG
#endif // CONNECTIONMONITORLOG_H

// End of file
