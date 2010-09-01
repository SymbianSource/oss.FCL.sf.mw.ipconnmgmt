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
*      Implementation of class APControlListPluginLogger.   
*
*/


// INCLUDE FILES

#ifdef _DEBUG

    #include "apcontrollistpluginlogger.h"
    #include <flogger.h>
	#include <eikenv.h>

    // ================= CONSTANTS =======================

    /// apcontrollistplugin logging directory.
    _LIT( KapcontrollistpluginLogDir, "apcontrollistplugin" );
    /// apcontrollistplugin log file name.
    _LIT( KapcontrollistpluginLogFile, "apcontrollistplugin.txt" );


    // ================= MEMBER FUNCTIONS =======================

    // ---------------------------------------------------------
    // APControlListPluginLogger::Write()
    // ---------------------------------------------------------
    //
    void APControlListPluginLogger::Write
    ( TInt32 aMask, TInt aLevel, TRefByValue<const TDesC16> aFmt, ... )
        {
        if( (aMask & APCONTROL_LIST_LOG_MASK) && (aLevel <= APCONTROL_LIST_LOG_LEVEL) )
            {
            VA_LIST list;
            VA_START( list, aFmt );
            RFileLogger::WriteFormat
                (
                KapcontrollistpluginLogDir,
                KapcontrollistpluginLogFile,
                EFileLoggingModeAppend,
                aFmt,
                list
                );
            VA_END( list );
            }
        }


    // ---------------------------------------------------------
    // APControlListPluginLogger::Write()
    // ---------------------------------------------------------
    //
    void APControlListPluginLogger::Write
    ( TInt32 aMask, TInt aLevel, TRefByValue<const TDesC16> aFmt, 
      VA_LIST& aList )
        {
        if( (aMask & APCONTROL_LIST_LOG_MASK) && (aLevel <= APCONTROL_LIST_LOG_LEVEL) )
            {
            RFileLogger::WriteFormat
                (
                KapcontrollistpluginLogDir,
                KapcontrollistpluginLogFile,
                EFileLoggingModeAppend,
                aFmt,
                aList
                );
            }
        }


    // ---------------------------------------------------------
    // APControlListPluginLogger::Write()
    // ---------------------------------------------------------
    //
    void APControlListPluginLogger::Write
    ( TInt32 aMask, TInt aLevel, TRefByValue<const TDesC8> aFmt, ... )
        {
        if( (aMask & APCONTROL_LIST_LOG_MASK) && (aLevel <= APCONTROL_LIST_LOG_LEVEL) )
            {
            VA_LIST list;
            VA_START( list, aFmt );
            RFileLogger::WriteFormat
                (
                KapcontrollistpluginLogDir,
                KapcontrollistpluginLogFile,
                EFileLoggingModeAppend,
                aFmt,
                list
                );
            VA_END( list );
            }
        }


    // ---------------------------------------------------------
    // APControlListPluginLogger::Write()
    // ---------------------------------------------------------
    //
    void APControlListPluginLogger::Write
    ( TInt32 aMask, TInt aLevel, TRefByValue<const TDesC8> aFmt, 
      VA_LIST& aList )
        {
        if( (aMask & APCONTROL_LIST_LOG_MASK) && (aLevel <= APCONTROL_LIST_LOG_LEVEL) )
            {
            RFileLogger::WriteFormat
                (
                KapcontrollistpluginLogDir,
                KapcontrollistpluginLogFile,
                EFileLoggingModeAppend,
                aFmt,
                aList
                );
            }
        }


    // ---------------------------------------------------------
    // APControlListPluginLogger::HexDump()
    // ---------------------------------------------------------
    //
    void APControlListPluginLogger::HexDump
            (
            TInt32 aMask,
            TInt aLevel, 
            const TText* aHeader,
            const TText* aMargin,
            const TUint8* aPtr,
            TInt aLen
            )
        {
        if( (aMask & APCONTROL_LIST_LOG_MASK) && (aLevel <= APCONTROL_LIST_LOG_LEVEL) )
            {
            RFileLogger::HexDump
                (
                KapcontrollistpluginLogDir,
                KapcontrollistpluginLogFile,
                EFileLoggingModeAppend,
                aHeader,
                aMargin,
                aPtr,
                aLen
                );
            }
        }
                
    // ---------------------------------------------------------
    // APControlListPluginLogger::FCreate()
    // ---------------------------------------------------------
    //
	void APControlListPluginLogger::FCreate()
	    {
	    TFileName path(_L("c:\\logs\\"));
	    path.Append(KapcontrollistpluginLogDir);
	    path.Append(_L("\\"));
	    RFs& fs = CEikonEnv::Static()->FsSession();
	    fs.MkDirAll(path);
	    }
#endif // _DEBUG

