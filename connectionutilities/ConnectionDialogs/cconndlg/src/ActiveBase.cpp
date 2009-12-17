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
* Description:  Implementation of base CActive class with a timer
*
*/


// INCLUDE FILES
#include "ActiveBase.h"
#include "ConnectionDialogsLogger.h"

/*#include <FeatMgr.h>
#include <StringLoader.h>
#include <AknIconArray.h>
#include <AknsUtils.h>
#include <data_caging_path_literals.hrh>

#include <cmdestinationext.h>
#include <CmConnectionMethodDef.h>
#include <cmpluginWlanDef.h>

#include <cmmanager.mbg>
#include <CConnDlgPlugin.rsg>
*/



///////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------
// CActiveBase::CActiveBase()
// ---------------------------------------------------------
//
CActiveBase::CActiveBase()
: CActive( EPriorityUserInput )
    {
    }

    
// ---------------------------------------------------------
// CActiveBase* CActiveBase::NewL()
// ---------------------------------------------------------
//
CActiveBase* CActiveBase::NewL()
    {
    CLOG_ENTERFN( "CActiveBase::NewL " );  
    
    CActiveBase* self = new ( ELeave ) CActiveBase();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    CLOG_LEAVEFN( "CActiveBase::NewL " );      

    return self;        
    }


// ---------------------------------------------------------
// CActiveBase::ConstructL()
// ---------------------------------------------------------
//
void CActiveBase::ConstructL()
    {
    CActiveScheduler::Add( this );            
    iPeriodic = CPeriodic::NewL( CActive::EPriorityStandard );
    }


// ---------------------------------------------------------
// CActiveBase::~CActiveBase()
// ---------------------------------------------------------
//    
CActiveBase::~CActiveBase()
    {
    Cancel();
    }


// ---------------------------------------------------------
// CActiveBase::Cancel()
// ---------------------------------------------------------
// 
void CActiveBase::Cancel()
    {
    StopTimer();
    CActive::Cancel();
    }


// ---------------------------------------------------------
// CActiveBase::DoCancel()
// ---------------------------------------------------------
// 
void CActiveBase::DoCancel()
    {
    }

    
// ---------------------------------------------------------
// CActiveBase::RunL()
// ---------------------------------------------------------
//     
void CActiveBase::RunL()
    {
    }

    
// ---------------------------------------------------------
// CActiveBase::StartTimerL
// ---------------------------------------------------------
//    
void CActiveBase::StartTimerL( TInt aTickInterval )
    {    
    CLOG_ENTERFN( "CActiveBase::StartTimerL " );
    
    if( !iPeriodic )
        {
        iPeriodic = CPeriodic::NewL( CActive::EPriorityStandard ); 
        }
        
    iPeriodic->Start( aTickInterval, aTickInterval, TCallBack( Tick, this ) );
    
    CLOG_LEAVEFN( "CActiveBase::StartTimerL " );
    }


// ---------------------------------------------------------
// CActiveBase::StopTimer
// ---------------------------------------------------------
//
void CActiveBase::StopTimer()
    {
    CLOG_ENTERFN( "CActiveBase::StopTimer " );  
    
    if( iPeriodic )
        {
        CLOG_WRITE( "iPeriodic" );
        iPeriodic->Cancel();
        CLOG_WRITE( "Cancel" );
        
        delete iPeriodic;
        CLOG_WRITE( "delete" );
        
        iPeriodic = NULL;
        }        
    
    CLOG_LEAVEFN( "CActiveBase::StopTimer " );
    }

    
// ---------------------------------------------------------
// CActiveBase::Tick
// ---------------------------------------------------------
//
TInt CActiveBase::Tick( TAny* aObject )
    {
    CLOG_ENTERFN( "CActiveBase::Tick " );  
    
    CActiveBase* myself = static_cast<CActiveBase*>( aObject );

    if ( myself->iStatus == KErrCancel )
        {
        CLOG_WRITE( "iStatus == KErrCancel" );        
        myself->StopTimer();            
        }
    else
        {
        CLOG_WRITE( "iStatus != KErrCancel" );    
                                                              
        myself->SetActive();
        myself->iClientStatus = &( myself->iStatus );  
        *( myself->iClientStatus ) = KRequestPending;  

        User::RequestComplete( myself->iClientStatus, KErrNone );         
        }
    
    CLOG_LEAVEFN( "CActiveBase::Tick " );      
    
    return 1;
    }        


// End of File
