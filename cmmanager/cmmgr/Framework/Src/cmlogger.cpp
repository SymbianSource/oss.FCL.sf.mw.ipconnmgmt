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
* Description:  Implementation of TLogger
*
*/

#include    "cmlogger.h"

#ifdef __CONNECTION_METHOD_LOG__

_LIT( KDeclareLoggerName, "<DECLARE_LOGGER_NAME>< %S >" );
_LIT( KAttachLogger, "<ATTACH_LOGGER><%x>to<%x>" );
_LIT( KOwnerLog, "<%x> -\t" );
_LIT8( KOwnerLog8, "<%x> -\t" );
_LIT( KLoggerCreated, "<LOGGER_CREATED>" );
_LIT( KLoggerClosed, "<LOGGER_CLOSED>" );
_LIT( KLoggerName, "[CMManager]" );
_LIT8( KLoggerName8, "[CMManager]" );

// ---------------------------------------------------------
// CFunctionEntryExit::CFunctionEntryExit()
// ---------------------------------------------------------
//
CFunctionEntryExit::CFunctionEntryExit( const TAny* aOwner, 
                                        TRefByValue<const TDesC> aFunc )
    : iOwner( aOwner )
    {
    iEntryTime.UniversalTime();

    iFunc.Set( aFunc );
        
    TLogger::LogIt( aOwner, _L("-> <%S>"), &iFunc );
    }
    
// ---------------------------------------------------------
// CFunctionEntryExit::~CFunctionEntryExit()
// ---------------------------------------------------------
//
CFunctionEntryExit::~CFunctionEntryExit() 
    {
    TTime exitTime;
    
    exitTime.UniversalTime();
    TLogger::LogIt( iOwner, _L("<- <%S><%d>"), 
                      &iFunc, 
                      I64LOW( exitTime.MicroSecondsFrom( iEntryTime ).Int64()));
    }

// ---------------------------------------------------------
// TLogger::CreateFlogger()
// ---------------------------------------------------------
//
void TLogger::CreateLogger( TAny* aOwner )
    {
    LogIt( aOwner, KLoggerCreated );
    }
    
// ---------------------------------------------------------
// TLogger::CloseLogger()
// ---------------------------------------------------------
//
void TLogger::CloseLogger( TAny* aOwner )
    {
    LogIt( aOwner, KLoggerClosed );
    }

// ---------------------------------------------------------
// TLogger::AttachFLogger()
// ---------------------------------------------------------
//
void TLogger::AttachLogger( const TAny* aOwner,
                               const TAny* aAttach1, 
                               const TAny* aAttach2 )
    {
    LogIt( aOwner, KAttachLogger, aAttach1, aAttach2 );
    }
    
// ---------------------------------------------------------
// TLogger::NameIt()
// ---------------------------------------------------------
//
void TLogger::NameIt( const TAny* aOwner, TRefByValue<const TDesC> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    
    TBuf<KLogBufferSize> nameBuf;
    TBuf<KLogBufferSize> logBuf;
    
    nameBuf.FormatList( aFmt, list );
    logBuf.Format( KDeclareLoggerName, &nameBuf );

    VA_END(list);
    
    TBuf<32> tempBuf;
    
#ifdef _LOG_MICRO_TIME_
    TTime timeNull( 0 );
    TTime timeNow;
    
    timeNow.UniversalTime();
    
    tempBuf.Format( _L(" - %Li - "), timeNow.MicroSecondsFrom(timeNull));
    
    logBuf.Insert( 0, timeBuf );
#endif    

    tempBuf.Format( KOwnerLog, aOwner );
    logBuf.Insert( 0, tempBuf );
    logBuf.Insert( 0, KLoggerName );

    RDebug::Print( logBuf );
    }
    
// ---------------------------------------------------------
// TLogger::LogIt()
// ---------------------------------------------------------
//
void TLogger::LogIt( const TAny* aOwner, 
                       TRefByValue<const TDesC> aFmt, 
                       VA_LIST aList )
    {
    TBuf<KLogBufferSize> logBuf;

    logBuf.FormatList( aFmt, aList );

    TBuf<32> tempBuf;

#ifdef _LOG_MICRO_TIME_
    TTime timeNull( 0 );
    TTime timeNow;
    
    timeNow.UniversalTime();
    
    tempBuf.Format( _L(" - %Li - "), timeNow.MicroSecondsFrom(timeNull));
    
    logBuf.Insert( 0, timeBuf );
#endif    

    tempBuf.Format( KOwnerLog, aOwner );
    logBuf.Insert( 0, tempBuf );
    logBuf.Insert( 0, KLoggerName );

    RDebug::Print( logBuf );
    }
    
// ---------------------------------------------------------
// TLogger::LogIt()
// ---------------------------------------------------------
//
void TLogger::LogIt( const TAny* aOwner, 
                     TRefByValue<const TDesC8> aFmt, 
                     VA_LIST aList )
    {
    TBuf8<KLogBufferSize> logBuf;
    
    logBuf.FormatList( aFmt, aList );

    TBuf8<32> tempBuf8;

#ifdef _LOG_MICRO_TIME_
    TTime timeNull( 0 );
    TTime timeNow;
    
    timeNow.UniversalTime();
    
    
    tempBuf8.Format( _L8(" - %Li - "), timeNow.MicroSecondsFrom(timeNull));
    
    logBuf.Insert( 0, tempBuf );
#endif    

    tempBuf8.Format( KOwnerLog8, aOwner );
    logBuf.Insert( 0, tempBuf8 );
    logBuf.Insert( 0, KLoggerName8 );

    RDebug::RawPrint( logBuf );
    }

// ---------------------------------------------------------
// TLogger::LogIt()
// ---------------------------------------------------------
//
void TLogger::LogIt( const TAny* aOwner, TRefByValue<const TDesC> aFmt, ...)

    {
    VA_LIST list;
    VA_START(list,aFmt);
    
    TLogger::LogIt( aOwner, aFmt, list );

    VA_END(list);
    }

// ---------------------------------------------------------
// TLogger::LogIt()
// ---------------------------------------------------------
//
void TLogger::LogIt( const TAny* aOwner, TRefByValue<const TDesC8> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    
    TLogger::LogIt( aOwner, aFmt, list );

    VA_END(list);
    }

#endif  // __CONNECTION_METHOD_LOG__
