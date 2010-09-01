/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This header defines logging macros for MPM
*
*/

/**
@file mpmlogger.h
Mobility Policy Manager debug logging definitions.
*/

#ifndef MPMLOGGER_H
#define MPMLOGGER_H

// MACROS
#ifndef _DEBUG

// UREL BUILD:
#define MPM_LOGGING_METHOD  0   // No logging in UREL builds

#else

// UDEB BUILD:
#define MPM_LOGGING_METHOD  1  // 0 = No logging, 
                               // 1 = Flogger, 
                               // 2 = RDebug
#endif // _DEBUG

// Macro usage examples
// Example: MPMLOGTEXT(own_desc) ;
// Example: MPMLOGSTRING("Test") ;
// Example: MPMLOGSTRING2("Test %i", aValue) ;
// Example: MPMLOGSTRING3("Test %i %i", aValue1, aValue2) ;

#if MPM_LOGGING_METHOD == 1      // Flogger

#include <flogger.h>
_LIT(KMPMLogFolder,"MPM");
_LIT(KMPMLogFile,"MPMLOG.TXT");

#define MPMLOGTEXT(x)\
    {\
    RFileLogger::Write(KMPMLogFolder(),KMPMLogFile(),EFileLoggingModeAppend,\
    x);\
    }

#define MPMLOGSTRING(x)\
    {\
    _LIT(tempLogDes,x);\
    RFileLogger::Write(KMPMLogFolder(),KMPMLogFile(),EFileLoggingModeAppend,\
    tempLogDes());\
    }

#define MPMLOGSTRING2(x,y)\
    { _LIT(tempLogDes,x);\
    RFileLogger::WriteFormat(KMPMLogFolder(),\
                             KMPMLogFile(),\
                             EFileLoggingModeAppend,\
    TRefByValue<const TDesC>(tempLogDes()),y);\
    }

#define MPMLOG8STRING2(x,y)\
    { _LIT8(tempLogDes,x);\
    RFileLogger::WriteFormat(KMPMLogFolder(),\
                             KMPMLogFile(),\
                             EFileLoggingModeAppend,\
    TRefByValue<const TDesC8>(tempLogDes()),y);\
    }

#define MPMLOGSSID(x,y,z)\
    { _LIT8(tempLogDes,x);\
    RFileLogger::WriteFormat(KMPMLogFolder(),\
                             KMPMLogFile(),\
                             EFileLoggingModeAppend,\
    TRefByValue<const TDesC8>(tempLogDes()),y,z);\
    }

#define MPMLOGSTRING3(x,y,z)\
    { _LIT(tempLogDes,x);\
    RFileLogger::WriteFormat(KMPMLogFolder(),\
                             KMPMLogFile(),\
                             EFileLoggingModeAppend,\
    TRefByValue<const TDesC>(tempLogDes()),y,z);\
    }

#define MPMLOGSTRING4(w,x,y,z)\
    { _LIT(tempLogDes,w);\
    RFileLogger::WriteFormat(KMPMLogFolder(),\
                             KMPMLogFile(),\
                             EFileLoggingModeAppend,\
    TRefByValue<const TDesC>(tempLogDes()),x,y,z);\
    }

#define MPMLOGSTRING5(w,x,y,z,s)\
    { _LIT(tempLogDes,w);\
    RFileLogger::WriteFormat(KMPMLogFolder(),\
                             KMPMLogFile(),\
                             EFileLoggingModeAppend,\
    TRefByValue<const TDesC>(tempLogDes()),x,y,z,s);\
    }

#elif MPM_LOGGING_METHOD == 2    // RDebug

#include <e32svr.h>

#define MPMLOGTEXT(x) RDebug::Print(x);
#define MPMLOGSTRING(x) RDebug::Print(_L(x));
#define MPMLOGSTRING2(x,y) RDebug::Print(_L(x),(y));
#define MPMLOG8STRING2(x,y) RDebug::Print(_L8(x),(y));
#define MPMLOGSSID(x,y,z) RDebug::Print(_L(x),(y),(z));
#define MPMLOGSTRING3(x,y,z) RDebug::Print(_L(x),(y),(z));
#define MPMLOGSTRING4(w,x,y,z) RDebug::Print(_L(w),(x),(y),(z));
#define MPMLOGSTRING5(w,x,y,z,s) RDebug::Print(_L(w),(x),(y),(z),(s));

#else // MPM_LOGGING_METHOD == 0 or invalid

#define MPMLOGTEXT(x)
#define MPMLOGSTRING(x)
#define MPMLOGSTRING2(x,y)
#define MPMLOG8STRING2(x,y)
#define MPMLOGSSID(x,y,z)
#define MPMLOGSTRING3(x,y,z)
#define MPMLOGSTRING4(w,x,y,z)
#define MPMLOGSTRING5(w,x,y,z,s)

#endif // MPM_LOGGING_METHOD

#endif // MPMLOGGER_H

// End of File
