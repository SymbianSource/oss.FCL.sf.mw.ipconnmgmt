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
* Description:  Implementation of CCmPluginWlanActiveWaiter.
*
*/


// INCLUDE FILES
#include "cmpwlanactivewaiter.h"



// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCmPluginWlanActiveWaiter* CCmPluginWlanActiveWaiter::NewL()
// ---------------------------------------------------------
//
CCmPluginWlanActiveWaiter* CCmPluginWlanActiveWaiter::NewL( )
    {
    CCmPluginWlanActiveWaiter* self = 
                         new ( ELeave ) CCmPluginWlanActiveWaiter( );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;        
    }

// ---------------------------------------------------------
// CCmPluginWlanActiveWaiter::ConstructL()
// ---------------------------------------------------------
//
void CCmPluginWlanActiveWaiter::ConstructL()
    {     
    CActiveScheduler::Add( this );            
    }


// ---------------------------------------------------------
// CCmPluginWlanActiveWaiter::CCmPluginWlanActiveWaiter()
// ---------------------------------------------------------
//
CCmPluginWlanActiveWaiter::CCmPluginWlanActiveWaiter( )
: CActive( KErrNone )
    {
    }
    

// ---------------------------------------------------------
// CCmPluginWlanActiveWaiter::~CCmPluginWlanActiveWaiter()
// ---------------------------------------------------------
//    
CCmPluginWlanActiveWaiter::~CCmPluginWlanActiveWaiter()
    {
    Cancel();
    }


// ---------------------------------------------------------
// CCmPluginWlanActiveWaiter::DoCancel()
// ---------------------------------------------------------
// 
void CCmPluginWlanActiveWaiter::DoCancel()
    {
    }
     
 
// ---------------------------------------------------------
// CCmPluginWlanActiveWaiter::RunL()
// ---------------------------------------------------------
//     
void CCmPluginWlanActiveWaiter::RunL()
    {
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }

// ---------------------------------------------------------
// CCmPluginWlanActiveWaiter::WaitForRequest()
// ---------------------------------------------------------
// 
TInt CCmPluginWlanActiveWaiter::WaitForRequest()
    {
    SetActive(); 
    iWait.Start();
    return iStatus.Int();
    }    
// End of File

