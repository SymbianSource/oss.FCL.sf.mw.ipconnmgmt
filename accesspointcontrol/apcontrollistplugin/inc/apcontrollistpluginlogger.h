/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
*      Logging macros for APControl List Plugin
*
*/


#ifndef APCONROL_LIST_PLUGIN_LOGGER_H
#define APCONROL_LIST_PLUGIN_LOGGER_H

// INCLUDES

#ifdef _DEBUG
    #include <e32std.h>
    #include <e32def.h>
    #include <flogger.h>

    // TYPES

    enum TAPControlListPluginLogMask             ///< Log mask bits.
        {
        ELogOff         = 0x00000000,   ///< Don't log.
        EApUi           = 0x00000001,   ///< Log transaction activity.
        ESelector       = 0x00000002,   ///< Log Selection activity.
        EEditing        = 0x00000004,   ///< Log editing activity.
        EModel          = 0x00000008,   ///< Log model activity.

        EProtection     = 0x00000020,   ///< Log protection activity.
        EApItem         = 0x00000040,   ///< Log ApItem activity.
        ECommons        = 0x00000080,   ///< Log transaction activity.
        EActiveDb       = 0x000000F0,   ///< Log active db&notific. activity.
        ELogAll         = 0xFFFFFFFF    ///< Log all.
        };

    // MACROS

    /// Determines what to log. Construct this from TApEngineLogMask values.
    #define APCONTROL_LIST_LOG_MASK ELogAll
    /// Determines log detail (0==basic level).
    #define APCONTROL_LIST_LOG_LEVEL 4

    // CLASS DECLARATION

    /**
    * Logger class.
    */
NONSHARABLE_CLASS( APControlListPluginLogger )
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
            ( TInt32 aMask, TInt aLevel, 
              TRefByValue<const TDesC8> aFmt, ... );

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

        /**
        * Create log dir.
        */
		static void FCreate();
        };


	#define CLOG_CREATE()  {APControlListPluginLogger::FCreate();}
    /// Write formatted to log.
    #define CLOG( body ) APControlListPluginLogger::Write body
    /// Write hex dump.
    #define CDUMP( body ) APControlListPluginLogger::HexDump body

#else /* not defined _DEBUG */

    /// Do nothing (log disabled).
    #define CLOG( body )
    /// Do nothing (log disabled).
    #define CDUMP( body )
	#define CLOG_CREATE()

#endif /* def _DEBUG */

#endif /* def APCONROL_LIST_PLUGIN_LOGGER_H */
