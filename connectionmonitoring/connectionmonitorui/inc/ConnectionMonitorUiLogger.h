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
* Description:  Logging support for R&D purposes
*
*
*/


#ifndef CONNECTIONMONITORUILOGGER_H_INCLUDED
#define CONNECTIONMONITORUILOGGER_H_INCLUDED

// ========== INCLUDE FILES ================================

#include <eikenv.h>
#include <flogger.h>

// ========== CONSTANTS ====================================

#ifdef _DEBUG

_LIT( KCMUILogDir,              "CMUI");
_LIT( KCMUILogFile,             "CMUI.txt");
_LIT(KCMUILogBanner,            "CMUI 0.1.0");
_LIT(KCMUILogEnterFn,           "CMUI: -> %S");
_LIT(KCMUILogLeaveFn,           "CMUI: <- %S");
_LIT(KCMUILogExit,              "CMUI: Application exit");
_LIT( KCMUILogTimeFormatString, "%H:%T:%S:%*C2");

// ========== MACROS =======================================

#define CMUILOGGER_CREATE             {FCreate();}
#define CMUILOGGER_DELETE             {RFileLogger::Write(KCMUILogDir, KCMUILogFile, EFileLoggingModeAppend, KCMUILogExit);}
#define CMUILOGGER_ENTERFN(a)         {_LIT(temp, a); RFileLogger::WriteFormat(KCMUILogDir, KCMUILogFile, EFileLoggingModeAppend, KCMUILogEnterFn, &temp);}
#define CMUILOGGER_LEAVEFN(a)         {_LIT(temp, a); RFileLogger::WriteFormat(KCMUILogDir, KCMUILogFile, EFileLoggingModeAppend, KCMUILogLeaveFn, &temp);}
#define CMUILOGGER_WRITE(a)           {_LIT(temp, a); RFileLogger::Write(KCMUILogDir, KCMUILogFile, EFileLoggingModeAppend, temp);}
#define CMUILOGGER_WRITE_F(a, b)      {_LIT(temp, a); RFileLogger::WriteFormat(KCMUILogDir, KCMUILogFile, EFileLoggingModeAppend, temp, b);}
#define CMUILOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KCMUILogTimeFormatString ); buffer.Insert(0, temp); RFileLogger::Write(KCMUILogDir, KCMUILogFile, EFileLoggingModeAppend, buffer); }
#define CMUILOGGER_WRITEF             FPrint


inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KCMUILogDir, KCMUILogFile, EFileLoggingModeAppend, aFmt, list);
    }

inline void FPrint(const TDesC& aDes)
    {
    RFileLogger::WriteFormat(KCMUILogDir, KCMUILogFile, EFileLoggingModeAppend, aDes);
    }

inline void FHex(const TUint8* aPtr, TInt aLen)
    {
    RFileLogger::HexDump(KCMUILogDir, KCMUILogFile, EFileLoggingModeAppend, 0, 0, aPtr, aLen);
    }

inline void FHex(const TDesC8& aDes)
    {
    FHex(aDes.Ptr(), aDes.Length());
    }

inline void FCreate()
    {
    TFileName path(_L("c:\\logs\\"));
    path.Append(KCMUILogDir);
    path.Append(_L("\\"));
    RFs& fs = CEikonEnv::Static()->FsSession();
    fs.MkDirAll(path);
    RFileLogger::WriteFormat(KCMUILogDir, KCMUILogFile, EFileLoggingModeOverwrite, KCMUILogBanner);
    }

#else // ! _DEBUG

inline void FPrint(const TRefByValue<const TDesC> /*aFmt*/, ...) { };

#define CMUILOGGER_CREATE
#define CMUILOGGER_DELETE
#define CMUILOGGER_ENTERFN(a)
#define CMUILOGGER_LEAVEFN(a)
#define CMUILOGGER_WRITE(a)
#define CMUILOGGER_WRITE_F(a, b)
#define CMUILOGGER_WRITEF   1 ? ((void)0) : FPrint
#define CMUILOGGER_WRITE_TIMESTAMP(a)

#endif // _DEBUG

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

// ========== CLASS DECLARATION ============================

#endif // CONNECTIONMONITORUILOGGER_H_INCLUDED