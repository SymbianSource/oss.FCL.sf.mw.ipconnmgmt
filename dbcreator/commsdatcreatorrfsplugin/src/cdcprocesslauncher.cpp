/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of CCdcProcessLauncher class.
*
*/



#include "cdcprocesslauncher.h"
#include "cdclogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCdcProcessLauncher::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCdcProcessLauncher* CCdcProcessLauncher::NewL()
    {
    CCdcProcessLauncher* self = new( ELeave ) CCdcProcessLauncher();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CCdcProcessLauncher::~CCdcProcessLauncher()
    {
    iProcess.Close();
    Cancel();
    }


// -----------------------------------------------------------------------------
// CCdcProcessLauncher::Launch()
//
// -----------------------------------------------------------------------------
//
void CCdcProcessLauncher::Launch()
    {
    CLOG_WRITE( "CCdcProcessLauncher::Launch" );

    // Exe name and empty parameters
	_LIT( KProcess, "commsdatstartup.exe" );
    _LIT( KEmpty, "" );
    	
    TInt errorCode = iProcess.Create(KProcess, KEmpty); 

    CLOG_WRITE_FORMAT( "CCdcCommsDatCreatorRfsPlugin::process.Create result: %d", errorCode );
	
    if ( errorCode == KErrNone )
        {
        LaunchProcess();
        }
    else
        {
        CLOG_WRITE( "Failed to creating process" );
        }

    CLOG_WRITE( "CCdcProcessLauncher::Launch" );
    }


// -----------------------------------------------------------------------------
// CCdcProcessLauncher::CCdcProcessLauncher
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCdcProcessLauncher::CCdcProcessLauncher()
  : CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CCdcProcessLauncher::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCdcProcessLauncher::ConstructL()
    {
    iProcess.SetHandle( NULL );
    }


// -----------------------------------------------------------------------------
// CCdcProcessLauncher::DoCancel
//
// -----------------------------------------------------------------------------
//
void CCdcProcessLauncher::DoCancel()
    {
    CLOG_WRITE( "CCdcProcessLauncher::DoCancel" );

    if ( iProcess.Handle() )
        {
        iProcess.RendezvousCancel( iStatus );
        iProcess.Close();
        }

    iWait.AsyncStop();

    CLOG_WRITE( "CCdcProcessLauncher::DoCancel" );
    }


// -----------------------------------------------------------------------------
// CCdcProcessLauncher::RunL
//
// -----------------------------------------------------------------------------
//
void CCdcProcessLauncher::RunL()
    {
    CLOG_WRITE( "CCdcProcessLauncher::RunL" );
    ASSERT( iProcess.Handle() );

    if ( iProcess.ExitType() == EExitPanic )
        {
        iProcess.ExitReason();
        }
    else
        {
        iStatus.Int();
        }
    
    iWait.AsyncStop();

    CLOG_WRITE( "CCdcProcessLauncher::RunL" );
    }

// -----------------------------------------------------------------------------
// CCdcProcessLauncher::Launch()
//
// -----------------------------------------------------------------------------
//
void CCdcProcessLauncher::LaunchProcess()
    {
    CLOG_WRITE( "CCdcProcessLauncher::Launch" );

    ASSERT( !IsActive() );
    ASSERT( iProcess.Handle() );

    iProcess.Rendezvous( iStatus );

    if( iStatus == KRequestPending )
        {
        CLOG_WRITE( "CCdcProcessLauncher::Resume" );
        iProcess.Resume();
        CLOG_WRITE( "CCdcProcessLauncher::SetActive" );
        SetActive();
        CLOG_WRITE( "CCdcProcessLauncher::Start waiter" );
        iWait.Start();
        }
    else
        {
        CLOG_WRITE( "Failed to make rendezvous with process, killing it" );
        iProcess.Kill( KErrGeneral );
        }

    CLOG_WRITE( "CCdcProcessLauncher::Launch end" );
    }


