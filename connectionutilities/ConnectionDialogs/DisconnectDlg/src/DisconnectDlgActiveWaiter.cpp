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
* Description:  Implementation of DisconnectDlgActiveWaiter.
*
*/


// INCLUDE FILES
#include "DisconnectDlgActiveWaiter.h"
#include "ConnectionDialogsLogger.h"



// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDisconnectDlgActiveWaiter* CDisconnectDlgActiveWaiter::NewL()
// ---------------------------------------------------------
//
CDisconnectDlgActiveWaiter* CDisconnectDlgActiveWaiter::NewL( )
    {
    CLOG_ENTERFN( "CDisconnectDlgActiveWaiter::NewL " );  
    
    CDisconnectDlgActiveWaiter* self = 
                         new ( ELeave ) CDisconnectDlgActiveWaiter( );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    CLOG_LEAVEFN( "CDisconnectDlgActiveWaiter::NewL " );      

    return self;        
    }

// ---------------------------------------------------------
// CDisconnectDlgActiveWaiter::ConstructL()
// ---------------------------------------------------------
//
void CDisconnectDlgActiveWaiter::ConstructL()
    {     
    CActiveScheduler::Add( this );            
    }


// ---------------------------------------------------------
// CDisconnectDlgActiveWaiter::CDisconnectDlgActiveWaiter()
// ---------------------------------------------------------
//
CDisconnectDlgActiveWaiter::CDisconnectDlgActiveWaiter( )
: CActive( EPriorityNormal )
    {
    }
    

// ---------------------------------------------------------
// CDisconnectDlgActiveWaiter::~CDisconnectDlgActiveWaiter()
// ---------------------------------------------------------
//    
CDisconnectDlgActiveWaiter::~CDisconnectDlgActiveWaiter()
    {
    Cancel();
    }


// ---------------------------------------------------------
// CDisconnectDlgActiveWaiter::DoCancel()
// ---------------------------------------------------------
// 
void CDisconnectDlgActiveWaiter::DoCancel()
    {
    }
     
 
// ---------------------------------------------------------
// CDisconnectDlgActiveWaiter::RunL()
// ---------------------------------------------------------
//     
void CDisconnectDlgActiveWaiter::RunL()
    {
    CLOG_ENTERFN( "CDisconnectDlgActiveWaiter::RunL " );  
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    CLOG_LEAVEFN( "CDisconnectDlgActiveWaiter::RunL " );
    }

// ---------------------------------------------------------
// CDisconnectDlgActiveWaiter::WaitForRequest()
// ---------------------------------------------------------
// 
TInt CDisconnectDlgActiveWaiter::WaitForRequest()
    {
    SetActive(); 
    iWait.Start();
    return iStatus.Int();
    }    
// End of File

