/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
*       This file containes the Access Point Engine speed - ETel speed 
*       lookup-information for the Access Point Engine 
*
*/



#ifndef APSPEED_LOOKUP_H
#define APSPEED_LOOKUP_H

// INCLUDE FILES
#include <ApEngineConsts.h>


// CONSTANTS

const TInt  CSD     = 0;
const TInt  HSCSD   = 1;

const TInt  Analogue    = 0;
const TInt  V110        = 1;
const TInt  V120        = 2;


const TInt SpeedData[27][13] =
    {
        {
        CSD,  Analogue,  KSpeedAutobaud,  0,  1,  2,  0,  0,  0,  0,  0,  1,  0
        },
        {
        CSD,  Analogue,  KSpeed9600,      0,  4,  2,  0,  0,  0,  0,  0,  1,  2
        },
        {
        CSD,  Analogue,  KSpeed14400,     0,  5,  2,  0,  0,  0,  0,  0,  1,  3
        },        
        {
        CSD,  Analogue,  KSpeed28800,     0,  7,  2,  0,  0,  0,  0,  0,  1,  3
        },
        {
        CSD,  V110,  KSpeed9600,      0,  4,  2,  0,  0,  0,  0,  0,  1,  4
        },
        {
        CSD,  V110,  KSpeed14400,     0,  5,  2,  0,  0,  0,  0,  0,  1,  4
        },        
        {
        CSD,  V110,  KSpeed28800,     0,  7,  2,  0,  0,  0,  0,  0,  1,  4
        },
        {
        CSD,  V110,  KSpeed38400,     0, 10,  2,  0,  0,  0,  0,  0,  1,  4
        },
        {
        CSD,  V120,  KSpeed9600,      0,  4,  2,  0,  0,  0,  0,  0,  1,  5
        },
        {
        CSD,  V120,  KSpeed14400,     0,  5,  2,  0,  0,  0,  0,  0,  1,  5
        },        
        {
        CSD,  V120,  KSpeed28800,     0,  7,  2,  0,  0,  0,  0,  0,  1,  5
        },
        {
        CSD,  V120,  KSpeed56000,     0, 13,  2,  0,  0,  0,  0,  0,  1,  5
        },
        {
        HSCSD, Analogue, KSpeedAutobaud,  1,  7,  2,  1, 12,  0,  0,  2,  1,  3
        },
        {
        HSCSD, Analogue, KSpeed9600,      1,  7,  2,  1,  4,  0,  1,  1,  1,  3
        },
        {
        HSCSD, Analogue, KSpeed14400,     1,  7,  2,  1, 12,  0,  1,  1,  1,  3
        },        
        {
        HSCSD, Analogue, KSpeed19200,     1,  7,  2,  1,  4,  0,  2,  2,  1,  3
        },
        {
        HSCSD, Analogue, KSpeed28800,     1,  7,  2,  1, 12,  0,  2,  2,  1,  3
        },
        {
        HSCSD, V110, KSpeed9600,      1, 10,  2,  1,  4,  0,  1,  1,  1,  4
        },
        {
        HSCSD, V110, KSpeed14400,     1, 10,  2,  1, 12,  0,  1,  1,  1,  4
        },        
        {
        HSCSD, V110, KSpeed19200,     1, 10,  2,  1,  4,  0,  2,  2,  1,  4
        },
        {
        HSCSD, V110, KSpeed28800,     1, 10,  2,  1, 12,  0,  2,  2,  1,  4
        },
        {
        HSCSD, V110, KSpeed38400,     1, 10,  2,  1, 12,  0,  3,  3,  1,  4
        },
        {
        HSCSD, V120, KSpeed9600,      1, 13,  2,  1,  4,  0,  1,  1,  1,  5
        },
        {
        HSCSD, V120, KSpeed14400,     1, 13,  2,  1, 12,  0,  1,  1,  1,  5
        },        
        {
        HSCSD, V120, KSpeed19200,     1, 13,  2,  1,  4,  0,  2,  2,  1,  5
        },        
        {
        HSCSD, V120, KSpeed28800,     1, 13,  2,  1, 12,  0,  2,  2,  1,  5
        },
        {
        HSCSD, V120, KSpeed43200,     1, 13,  2,  1, 12,  0,  3,  3,  1,  5
        }
    };
        



#endif

// End of File
