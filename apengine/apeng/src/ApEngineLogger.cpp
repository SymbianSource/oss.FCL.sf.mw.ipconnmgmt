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
*      Implementation of class ApEngineLogger.   
*      
*
*/


// INCLUDE FILES

#ifdef __TEST_APENGINE_LOG__

    #include "ApEngineLogger.h"
    #include <flogger.h>

    // ================= CONSTANTS =======================

    /// ApEngine logging directory.
    _LIT( KApEngineLogDir, "ApEngine" );
    /// ApEngine log file name.
    _LIT( KApEngineLogFile, "ApEngine.txt" );


    // ================= MEMBER FUNCTIONS =======================

    // ---------------------------------------------------------
    // ApEngineLogger::Write()
    // ---------------------------------------------------------
    //
    void ApEngineLogger::Write
    ( TInt32 aMask, TInt aLevel, TRefByValue<const TDesC16> aFmt, ... )
        {
        }


    // ---------------------------------------------------------
    // ApEngineLogger::Write()
    // ---------------------------------------------------------
    //
    void ApEngineLogger::Write
    ( TInt32 aMask, TInt aLevel, 
      TRefByValue<const TDesC16> aFmt, 
      VA_LIST& aList )
        {
        }


    // ---------------------------------------------------------
    // ApEngineLogger::Write()
    // ---------------------------------------------------------
    //
    void ApEngineLogger::Write
    ( TInt32 aMask, TInt aLevel, TRefByValue<const TDesC8> aFmt, ... )
        {
        }


    // ---------------------------------------------------------
    // ApEngineLogger::Write()
    // ---------------------------------------------------------
    //
    void ApEngineLogger::Write
    ( TInt32 aMask, TInt aLevel, 
      TRefByValue<const TDesC8> aFmt, VA_LIST& aList )
        {
        }


    // ---------------------------------------------------------
    // ApEngineLogger::HexDump()
    // ---------------------------------------------------------
    //
    void ApEngineLogger::HexDump
            (
            TInt32 aMask,
            TInt aLevel, 
            const TText* aHeader,
            const TText* aMargin,
            const TUint8* aPtr,
            TInt aLen
            )
        {
        }
#endif // __TEST_APENGINE_LOG__
