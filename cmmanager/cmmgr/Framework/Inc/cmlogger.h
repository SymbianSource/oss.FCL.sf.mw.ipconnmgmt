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
* Description:  Declaration of logger
*
*/

#ifndef CONNECTIONMETHODLOGGER_H
#define CONNECTIONMETHODLOGGER_H

// INCLUDE FILES
#include <e32svr.h>

#ifdef _DEBUG

#define __CONNECTION_METHOD_LOG__
#include <flogger.h>

// CONSTANTS

class CFunctionEntryExit;

//Logging to file in THUMB UDEB
class TLogger
    {
    public: // methods

        static void CreateLogger( TAny* aOwner );
        static void CloseLogger( TAny* aOwner );

        static void AttachLogger( const TAny* aOwner, 
                                   const TAny* aAttach1,
                                   const TAny* aAttach2 );
       
        static void NameIt( const TAny* aOwner, 
                            TRefByValue<const TDesC> aNameFmt, 
                            ... );
        
        static void LogIt( const TAny* aOwner, 
                           TRefByValue<const TDesC> aFmt, 
                           ... );
        static void LogIt( const TAny* aOwner, 
                           TRefByValue<const TDesC8> aFmt, 
                           ... );
        static void LogIt( const TAny* aOwner, 
                           TRefByValue<const TDesC> aFmt, 
                           VA_LIST aList );
        static void LogIt( const TAny* aOwner, 
                           TRefByValue<const TDesC8> aFmt, 
                           VA_LIST aList );

    private:       

        TBuf<2048>      iLogBuf;
        TBuf8<2048>     iLogBuf8;
        TInt            iRefCounter;
        TAny*           iOwner;
    };

NONSHARABLE_CLASS( CFunctionEntryExit ): public CBase
    {
    public:

        CFunctionEntryExit( const TAny* aOwner, 
                            TRefByValue<const TDesC> aFunc );
        ~CFunctionEntryExit();

    public:
    
        TPtrC       iFunc;  // function name
        const TAny* iOwner;
        TTime       iEntryTime;
    };

#define LOGGER_ENTERFN( a )     CFunctionEntryExit entryExit( this, _L(a) );
#define CLOG_ENTERFN_PTR( a )   CFunctionEntryExit entryExit( NULL, _L(a) );

#define CLOG_CREATE             TLogger::CreateLogger( this );
#define CLOG_CREATE_PTR         TLogger::CreateLogger( NULL );
#define CLOG_ATTACH( attach1, attach2 )  TLogger::AttachLogger( this, \
                                                                   attach1, \
                                                                   attach2 );
#define CLOG_NAME( a )           TLogger::NameIt( this, a );
#define CLOG_NAME_1( a, b )      TLogger::NameIt( this, a, b );
#define CLOG_NAME_2( a, b, c )   TLogger::NameIt( this, a, b, c );
#define CLOG_NAME_3( a, b, c, d )   TLogger::NameIt( this, a, b, c, d );
#define CLOG_CLOSE               TLogger::CloseLogger( this );
#define CLOG_WRITE( a )          TLogger::LogIt( this, _L(a) );
#define CLOG_WRITE_FORMAT( a, b )   TLogger::LogIt( this, _L(a), b );
#define CLOG_WRITE_1( a, b )     TLogger::LogIt( this, _L(a), b );
#define CLOG_WRITE_2( a, b, c )  TLogger::LogIt( this, _L(a), b, c );
#define CLOG_WRITE_3( a, b, c, d )  TLogger::LogIt( this, _L(a), b, c, d );
#define CLOG_WRITE_4( a, b, c, d, e )   TLogger::LogIt( this, \
                                                          _L(a), b, c, d, e );
#define CLOG_WRITE8( a )         TLogger::LogIt( this, _L8(a) );
#define CLOG_WRITE8_1( a, b )    TLogger::LogIt( this, _L8(a), b );
#define CLOG_WRITE8_2( a, b, c ) TLogger::LogIt( this, _L8(a), b, c );
#define CLOG_WRITE8_3( a, b, c, d ) TLogger::LogIt( this, _L8(a), b, c, d );
#define CLOG_WRITE8_4( a, b, c, d, e )  TLogger::LogIt( this, \
                                                          _L8(a), b, c, d, e );

#define CLOG_NAME_PTR( ptr, a )                     TLogger::NameIt( ptr, a );
#define CLOG_ATTACH_PTR( ptr, attach1, attach2 )    TLogger::AttachLogger( \
                                                        ptr, attach1, attach2 );
#define CLOG_WRITE_PTR( ptr, a )                    TLogger::LogIt( ptr, \
                                                                      _L(a) );
#define CLOG_WRITE_1_PTR( ptr, a, b )               TLogger::LogIt( ptr, \
                                                                    _L(a), b );
#define CLOG_WRITE_2_PTR( ptr, a, b, c )            TLogger::LogIt( ptr, \
                                                                  _L(a), b, c );
#define CLOG_WRITE_3_PTR( ptr, a, b, c, d )         TLogger::LogIt( ptr, \
                                                               _L(a), b, c, d );
#define CLOG_WRITE_4_PTR( ptr, a, b, c, d, e )      TLogger::LogIt( ptr, \
                                                            _L(a), b, c, d, e );
#define CLOG_WRITE_FORMAT_PTR( ptr, a, b )          TLogger::LogIt( ptr, \
                                                                    _L(a), b );
#define CLOG_CLOSE_PTR( ptr )   TLogger::CloseLogger( ptr );

#else   // _DEBUG

#define LOGGER_ENTERFN( a )
#define CLOG_ENTERFN_PTR( a )

#define CLOG_CREATE
#define CLOG_CREATE_PTR
#define CLOG_ATTACH( attach1, attach2 )
#define CLOG_NAME( a )
#define CLOG_NAME_1( a, b )
#define CLOG_NAME_2( a, b, c )
#define CLOG_NAME_3( a, b, c, d )
#define CLOG_CLOSE
#define CLOG_WRITE( a )                   
#define CLOG_WRITE_FORMAT( a, b )              
#define CLOG_WRITE_1( a, b )
#define CLOG_WRITE_2( a, b, c )           
#define CLOG_WRITE_3( a, b, c, d )        
#define CLOG_WRITE_4( a, b, c, d, e )     
#define CLOG_WRITE8( a )                  
#define CLOG_WRITE8_1( a, b )             
#define CLOG_WRITE8_2( a, b, c )          
#define CLOG_WRITE8_3( a, b, c, d )       
#define CLOG_WRITE8_4( a, b, c, d, e )    

#define CLOG_NAME_PTR( ptr, a )
#define CLOG_ATTACH_PTR( ptr, attach1, attach2 )
#define CLOG_WRITE_PTR( ptr, a )
#define CLOG_WRITE_1_PTR( ptr, a, b )
#define CLOG_WRITE_2_PTR( ptr, a, b, c )
#define CLOG_WRITE_3_PTR( ptr, a, b, c, d )
#define CLOG_WRITE_4_PTR( ptr, a, b, c, d, e )
#define CLOG_WRITE_FORMAT_PTR( ptr, a, b )
#define CLOG_CLOSE_PTR( ptr )

#endif // _DEBUG
#endif // CONNECTIONMETHODLOGGER_H
