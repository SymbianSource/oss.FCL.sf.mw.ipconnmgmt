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
* Description:  Writes log entries for Connection Monitor Extension.
*
*/

#ifndef CONNECTIONMONITOREXTLOG_H
#define CONNECTIONMONITOREXTLOG_H

#include <e32std.h>
#include <e32base.h>

#ifdef _DEBUG
    #define LOG(a) a
#else
    #define LOG(a)
#endif

#ifdef _DEBUG

#include <flogger.h>

_LIT( KLogFolder,"ConnMonExt" );
_LIT( KLogFile,"ConnMonExt.txt" );

class Log
    {
public:
    static inline void Write( const TDesC& aDes);
    static inline void Printf( TRefByValue<const TDesC> aFmt, ...);
    static inline void Printf( TRefByValue<const TDesC8> aFmt, ...);
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

inline void Log::Printf( TRefByValue<const TDesC8> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list,aFmt );
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

#endif // _DEBUG
#endif // CONNECTIONMONITOREXTLOG_H

// End-of-file
