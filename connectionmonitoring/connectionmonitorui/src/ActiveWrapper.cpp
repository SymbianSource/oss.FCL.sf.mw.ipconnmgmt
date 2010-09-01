/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CActiveWrapper implementation file
*
*/


#include <e32base.h>

#include "ActiveWrapper.h"
#include "ConnectionMonitorUiLogger.h"

// LOCAL CONSTANTS AND MACROS

// ---------------------------------------------------------
// CActiveWrapper::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CActiveWrapper* CActiveWrapper::NewL()
    {
    CMUILOGGER_ENTERFN( "CActiveWrapper::NewL()" );
    
    CActiveWrapper* self = new ( ELeave ) CActiveWrapper();
    CleanupStack::PushL( self );

    self->ConstructL();
    
    CleanupStack::Pop( self );

    CMUILOGGER_LEAVEFN( "CActiveWrapper::NewL()" );
    return self;
    }

// ---------------------------------------------------------
// CActiveWrapper::ConstructL
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CActiveWrapper::ConstructL()
    {
    CMUILOGGER_ENTERFN( "CActiveWrapper::ConstructL()" );

    CActiveScheduler::Add( this );

    CMUILOGGER_LEAVEFN( "CActiveWrapper::ConstructL()" );
    }


// ---------------------------------------------------------
// CActiveWrapper::CActiveWrapper
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CActiveWrapper::CActiveWrapper() : CActive( CActive::EPriorityStandard )
    {
    }


// ---------------------------------------------------------
// CActiveWrapper::~CActiveWrapper
//
// Destructor
// ---------------------------------------------------------
//
CActiveWrapper::~CActiveWrapper()
    {
    CMUILOGGER_ENTERFN( "CActiveWrapper::~CActiveWrapper()" );
    
    Cancel();
    
    CMUILOGGER_WRITE( "CActiveWrapper::~CActiveWrapper Canceled" );
    
    CMUILOGGER_LEAVEFN( "CActiveWrapper::~CActiveWrapper()" );
    }


// ---------------------------------------------------------
// CActiveWrapper::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CActiveWrapper::RunL() 
    {
    CMUILOGGER_ENTERFN( "CActiveWrapper::RunL()" );

    if ( iWait.IsStarted() )
        {
        CMUILOGGER_WRITE( "AsyncStop" );

        iWait.AsyncStop();
        }

    CMUILOGGER_LEAVEFN( "CActiveWrapper::RunL()" );
    }


// ---------------------------------------------------------
// CActiveWrapper::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CActiveWrapper::DoCancel()
    {
    CMUILOGGER_ENTERFN( "CActiveWrapper::DoCancel()" );

    if ( iWait.IsStarted() )
        {
        CMUILOGGER_WRITE( "AsyncStop" );

        iWait.AsyncStop();
        }

    CMUILOGGER_LEAVEFN( "CActiveWrapper::DoCancel()" );
    }
    

// ---------------------------------------------------------
// CActiveWrapper::StartGetStringAttribute
//
// Starts the active object
// ---------------------------------------------------------
//
void CActiveWrapper::StartGetStringAttribute( TInt const aConnectionId, 
                            RConnectionMonitor* const aConnectionMonitor,
                            TUint const aAttribute,
                            TDes& aValue )
    {
    CMUILOGGER_ENTERFN( "CActiveWrapper::StartGetStringAttribute()" );

    if ( IsActive() == EFalse )
        {
        CMUILOGGER_WRITE( "NOT active" );
        aConnectionMonitor->GetStringAttribute( aConnectionId, 0, aAttribute,
                                                aValue, iStatus );
        SetActive();
        iWait.Start();
        }
    else
        {
        CMUILOGGER_WRITE( "Already active" );
        }
        
    CMUILOGGER_LEAVEFN( "CActiveWrapper::StartGetStringAttribute()" );
    }


// ---------------------------------------------------------
// CActiveWrapper::StartGetIntAttribute
//
// Starts the active object
// ---------------------------------------------------------
//
void CActiveWrapper::StartGetIntAttribute( TInt const aConnectionId,
                            RConnectionMonitor* const aConnectionMonitor,
                            TUint const aAttribute,
                            TInt& aValue )
    {
    CMUILOGGER_ENTERFN( "CActiveWrapper::StartGetIntAttribute()" );

    if ( IsActive() == EFalse )
        {
        CMUILOGGER_WRITE( "NOT active" );
        aConnectionMonitor->GetIntAttribute( aConnectionId, 
                                             0,
                                             aAttribute,
                                             aValue,
                                             iStatus );
        SetActive();
        iWait.Start();
        }
    else
        {
        CMUILOGGER_WRITE( "Already active" );
        }
        
    CMUILOGGER_LEAVEFN( "CActiveWrapper::StartGetIntAttribute()" );
    }


// ---------------------------------------------------------
// CActiveWrapper::StartGetUintAttribute
//
// Starts the active object
// ---------------------------------------------------------
//
void CActiveWrapper::StartGetUintAttribute( TInt const aConnectionId,
                            RConnectionMonitor* const aConnectionMonitor,
                            TUint const aAttribute,
                            TUint& aValue )
    {
    CMUILOGGER_ENTERFN( "CActiveWrapper::StartGetUintAttribute()" );

    if ( IsActive() == EFalse )
        {
        CMUILOGGER_WRITE( "NOT active" );
        aConnectionMonitor->GetUintAttribute( aConnectionId,
                                              0,
                                              aAttribute,
                                              aValue,
                                              iStatus );
        SetActive();
        iWait.Start();
        }
    else
        {
        CMUILOGGER_WRITE( "Already active" );
        }
        
    CMUILOGGER_LEAVEFN( "CActiveWrapper::StartGetUintAttribute()" );
    }


// ---------------------------------------------------------
// CActiveWrapper::StartGetConnSharings
//
// Starts the active object
// ---------------------------------------------------------
//
void CActiveWrapper::StartGetConnSharings( TInt const aConnectionId,
                            RConnectionMonitor* const aConnectionMonitor,
                            TConnMonClientEnumBuf& aValue )
    {
    CMUILOGGER_ENTERFN( "CActiveWrapper::StartGetConnSharings()" );

    if ( IsActive() == EFalse )
        {
        CMUILOGGER_WRITE( "NOT active" );   
        aConnectionMonitor->GetPckgAttribute( aConnectionId,
                                              0,
                                              KClientInfo,
                                              aValue,
                                              iStatus );
        SetActive();
        iWait.Start();
        }
    else
        {
        CMUILOGGER_WRITE( "Already active" );
        }
        
    CMUILOGGER_LEAVEFN( "CActiveWrapper::StartGetConnSharings()" );
    }


// ---------------------------------------------------------
// CActiveWrapper::StartGetConnTime
//
// Starts the active object
// ---------------------------------------------------------
//
void CActiveWrapper::StartGetConnTime( TInt const aConnectionId,
                            RConnectionMonitor* const aConnectionMonitor,
                            TConnMonTimeBuf& aValue )
    {
    CMUILOGGER_ENTERFN( "CActiveWrapper::StartGetConnTime()" );

    if ( IsActive() == EFalse )
        {
        CMUILOGGER_WRITE( "NOT active" );   
        aConnectionMonitor->GetPckgAttribute( aConnectionId,
                                              0,
                                              KStartTime,
                                              aValue,
                                              iStatus );
        SetActive();
        iWait.Start();
        }
    else
        {
        CMUILOGGER_WRITE( "Already active" );
        }
        
    CMUILOGGER_LEAVEFN( "CActiveWrapper::StartGetConnTime()" );
    }


// ---------------------------------------------------------
// CActiveWrapper::StartGetBearerType
//
// Starts the active object
// ---------------------------------------------------------
//
void CActiveWrapper::StartGetBearerType( TInt const aConnectionId,
                            RConnectionMonitor& aConnectionMonitor,
                            TUint const aAttribute,
                            TInt& aValue )
    {
    CMUILOGGER_ENTERFN( "CActiveWrapper::StartGetBearerType()" );

    if ( IsActive() == EFalse )
        {
        CMUILOGGER_WRITE( "NOT active" );
        aConnectionMonitor.GetIntAttribute( aConnectionId, 
                                            0,
                                            aAttribute,
                                            aValue,
                                            iStatus );
        SetActive();
        iWait.Start();
        }
    else
        {
        CMUILOGGER_WRITE( "Already active" );
        }
        
    CMUILOGGER_LEAVEFN( "CActiveWrapper::StartGetBearerType()" );
    }


// ---------------------------------------------------------
// CActiveWrapper::StartGetConnectionCount
//
// Starts the active object
// ---------------------------------------------------------
//
void CActiveWrapper::StartGetConnectionCount( TUint& aConnectionCount,
                            RConnectionMonitor& aConnectionMonitor )
    {
    CMUILOGGER_ENTERFN( "CActiveWrapper::StartGetConnectionCount()" );

    if ( IsActive() == EFalse )
        {
        CMUILOGGER_WRITE( "NOT active" );
        aConnectionMonitor.GetConnectionCount( aConnectionCount, iStatus );
        SetActive();
        iWait.Start();
        }
    else
        {
        CMUILOGGER_WRITE( "Already active" );
        }
        
    CMUILOGGER_LEAVEFN( "CActiveWrapper::StartGetConnectionCount()" );
    }


// End of File
