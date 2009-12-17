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
* Description: This header defines logging macros for S60MCPR
*
*/


/**
@file s60mcprlog.h
This header defines logging macros for S60MCPR.
*/

#ifndef S60MCPRLOG_H
#define S60MCPRLOG_H

// Logging tag
#if defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
#include <comms-infras/ss_logext.h>
#define KS60MCprTag KESockMetaConnectionTag
_LIT8(KS60MCprSubTag, "netmcpr");
#endif

#ifndef _DEBUG

// UREL BUILD:
#define S60MCPR_LOGGING_METHOD  0   // No logging in UREL builds

#else

// UDEB BUILD:
#define S60MCPR_LOGGING_METHOD  1     // 0 = No logging,
                                      // 1 = CDU
                                      // 2 = RDebug
#endif // _DEBUG

#if S60MCPR_LOGGING_METHOD == 1 // Flogger

#include <cflog.h>

#define S60MCPRLOGTEXT(x)\
    {\
    RFileLogger::Write(KBMLogFolder(),KBMLogFile(),EFileLoggingModeAppend,\
    x);\
    }

#define S60MCPRLOGSTRING(x)\
    {\
    _LIT(tempLogDes,x);\
    __CFLOG_0(KS60MCprTag,KS60MCprSubTag, tempLogDes());\
    }

#define S60MCPRLOGSTRING1(x,y)\
    { _LIT(tempLogDes,x);\
    __CFLOG_1(KS60MCprTag,KS60MCprSubTag, tempLogDes(), y);\
    }

#define S60MCPRLOGSTRING2(x,y,z)\
    { _LIT(tempLogDes,x);\
    __CFLOG_2(KS60MCprTag,KS60MCprSubTag, tempLogDes(), y, z);\
    }

#define S60MCPRLOGSTRING3(w,x,y,z)\
    { _LIT(tempLogDes,w);\
    __CFLOG_3(KS60MCprTag,KS60MCprSubTag, tempLogDes(), x, y, z);\
    }

#define S60MCPRLOGSTRING4(w,x,y,z,v)\
    { _LIT(tempLogDes,w);\
    __CFLOG_4(KS60MCprTag,KS60MCprSubTag, tempLogDes(), x, y, z, v);\
    }

#define S60MCPRLOGTEXT(x)\
    {\
    RFileLogger::Write(KBMLogFolder(),KBMLogFile(),EFileLoggingModeAppend,\
    x);\
    }

#elif S60MCPR_LOGGING_METHOD == 2    // RDebug

#include <e32svr.h>

#define S60MCPRLOGTEXT(x) RDebug::Print(x);
#define S60MCPRLOGSTRING(x) RDebug::Print(_L(x));
#define S60MCPRLOGSTRING1(x,y) RDebug::Print(_L(x),y);
#define S60MCPRLOGSTRING2(x,y,z) RDebug::Print(_L(x),y,z);
#define S60MCPRLOGSTRING3(w,x,y,z) RDebug::Print(_L(w),x,y,z);
#define S60MCPRLOGSTRING4(w,x,y,z,v) RDebug::Print(_L(w),x,y,z,v);

#else // S60MCPR_LOGGING_METHOD == 0 or invalid

#define S60MCPRLOGTEXT(x)
#define S60MCPRLOGSTRING(x)
#define S60MCPRLOGSTRING1(x,y)
#define S60MCPRLOGSTRING2(x,y,z)
#define S60MCPRLOGSTRING3(w,x,y,z)
#define S60MCPRLOGSTRING4(w,x,y,z,v)

#endif // S60MCPR_LOGGING_METHOD

#endif  // S60MCPRLOG_H

// End of File

