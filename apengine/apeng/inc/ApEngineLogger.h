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
*      Logging macros for Access Point Engine
*      
*
*/


#ifndef APENGINE_LOGGER_H
#define APENGINE_LOGGER_H

// INCLUDES

#ifdef __TEST_APENGINE_LOG__
    #include <e32std.h>
    #include <e32def.h>
    #include <flogger.h>

    // TYPES

    enum TApEngineLogMask             ///< Log mask bits.
        {
        ELogOff         = 0x00000000,   ///< Don't log.
        ETransaction    = 0x00000001,   ///< Log transaction activity.
        ESelect         = 0x00000002,   ///< Log ApSelect activity.
        EHandler        = 0x00000004,   ///< Log ApDataHandler activity.
        EUtil           = 0x00000008,   ///< Log ApUtils activity.
        ENetworks       = 0x0000000F,   ///< Log Networks activity.
        EApList         = 0x00000010,   ///< Log ApList activity.
        EProtection     = 0x00000020,   ///< Log protection activity.
        EApItem         = 0x00000040,   ///< Log ApItem activity.
        ECommons        = 0x00000080,   ///< Log transaction activity.
        EActiveDb       = 0x000000F0,   ///< Log active db & notification 
                                        ///< activity.

        ESpecial        = 0x0FFFF000,   ///< Log special, temp stuff only
        ELogAll         = 0xFFFFFFFF    ///< Log all.
        };

    // MACROS

    /// Determines what to log. Construct this from TApEngineLogMask values.
    #define APENGINE_LOG_MASK ELogAll

    /// Determines log detail (0==basic level).
    #define APENGINE_LOG_LEVEL 4

    // CLASS DECLARATION

    /**
    * Logger class.
    */
    NONSHARABLE_CLASS( ApEngineLogger )
        {
        public:     // new methods

        /**
        * Write formatted log.
        * @param aMask Log mask.
        * @param aLevel Log level.
        * @param aFmt Format string.
        */
        static void Write
            ( TInt32 aMask, TInt aLevel, 
              TRefByValue<const TDesC16> aFmt, ... );

        /**
        * Write formatted log.
        * @param aMask Log mask.
        * @param aLevel Log level.
        * @param aFmt Format string.
        * @param aList Variable argument list.
        */
        static void Write
            (
            TInt32 aMask,
            TInt aLevel,
            TRefByValue<const TDesC16> aFmt,
            VA_LIST& aList
            );

        /**
        * Write formatted log.
        * @param aMask Log mask.
        * @param aLevel Log level.
        * @param aFmt Format string.
        */
        static void Write
            ( TInt32 aMask, TInt aLevel, TRefByValue<const TDesC8> aFmt, ... );

        /**
        * Write formatted log.
        * @param aMask Log mask.
        * @param aLevel Log level.
        * @param aFmt Format string.
        * @param aList Variable argument list.
        */
        static void Write
            (
            TInt32 aMask,
            TInt aLevel,
            TRefByValue<const TDesC8> aFmt,
            VA_LIST& aList
            );

        /**
        * Write hex dump.
        * @param aMask Log mask.
        * @param aLevel Log level.
        * @param aHeader Header string.
        * @param aMargin Margin.
        * @param aPtr Data.
        * @param aLen Data length.
        */
        static void HexDump
            (
            TInt32 aMask,
            TInt aLevel, 
            const TText* aHeader,
            const TText* aMargin,
            const TUint8* aPtr,
            TInt aLen
            );
        };


    /// Write formatted to log.
    #define CLOG( body ) ApEngineLogger::Write body
    /// Write hex dump.
    #define CDUMP( body ) ApEngineLogger::HexDump body

#else /* not defined __TEST_APENGINE_LOG__ */

    /// Do nothing (log disabled).
    #define CLOG( body )
    /// Do nothing (log disabled).
    #define CDUMP( body )

#endif /* def __TEST_APENGINE_LOG__ */

#endif /* def APENGINE_LOGGER_H */
