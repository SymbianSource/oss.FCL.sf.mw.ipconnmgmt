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
* Description: MPM Dual Transfer Mode watcher
*
*/

/**
@file mpmdtmwatcher.cpp
Mobility Policy Manager Dual Transfer Mode watcher.
*/

// INCLUDE FILES
#include "mpmdtmwatcher.h"
#include "mpmserver.h"
#include "mpmlogger.h"
#include <pcktcs.h> // This header has to be the last in the include file list 
                    // in order to avoid this error: 
                    // \EPOC32\include\pcktcs.h:26: undefined identifier 'TLitC'

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPMDtmWatcher::NewL
// -----------------------------------------------------------------------------
//
CMPMDtmWatcher* CMPMDtmWatcher::NewL( RPacketService&   aPacketService )
    {
    CMPMDtmWatcher* self = new( ELeave ) CMPMDtmWatcher( aPacketService );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CMPMDtmWatcher::CMPMDtmWatcher
// -----------------------------------------------------------------------------
//
CMPMDtmWatcher::CMPMDtmWatcher( RPacketService& aPacketService )
    : CActive( CActive::EPriorityStandard ),
      iPacketService( aPacketService ),
      iMsClass( RPacketService::EMSClassUnknown ),
      iMaxMsClass( RPacketService::EMSClassUnknown )
    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CMPMDtmWatcher::~CMPMDtmWatcher
// -----------------------------------------------------------------------------
//
CMPMDtmWatcher::~CMPMDtmWatcher()
    {
    MPMLOGSTRING( "CMPMDtmWatcher::~CMPMDtmWatcher" )
    Cancel();
    }


// -----------------------------------------------------------------------------
// CMPMDtmWatcher::ConstructL
// -----------------------------------------------------------------------------
//
void CMPMDtmWatcher::ConstructL()
    {
    MPMLOGSTRING( "CMPMDtmWatcher::ConstructL" )

    // Request current MS class from ETel
    //
    TRequestStatus status;
    MPMLOGSTRING( "CMPMDtmWatcher::ConstructL GetMSClass" )
    iPacketService.GetMSClass( status, iMsClass, iMaxMsClass );
    MPMLOGSTRING( "CMPMDtmWatcher::ConstructL WaitForRequest" )
    // This information is needed before the server is started.
    // 
    User::WaitForRequest( status );
    MPMLOGSTRING2( "CMPMDtmWatcher::ConstructL LeaveIfError: %d", status.Int() )
    User::LeaveIfError( status.Int() );

    LogPacketClass();

    // Request notification from ETel about MS class change
    iPacketService.NotifyMSClassChange( iStatus, iMsClass );
    SetActive();
    }


// -----------------------------------------------------------------------------
// CMPMDtmWatcher::IsInDualMode
// -----------------------------------------------------------------------------
//
TBool CMPMDtmWatcher::IsInDualMode() const
    {
    MPMLOGSTRING( "CMPMDtmWatcher::IsInDualMode" )

    LogPacketClass();

    return ( iMsClass == RPacketService::EMSClassDualMode );
    }


// -----------------------------------------------------------------------------
// CMPMDtmWatcher::RunL
// -----------------------------------------------------------------------------
//
void CMPMDtmWatcher::RunL()
    {
    MPMLOGSTRING( "CMPMDtmWatcher::RunL" )

    LogPacketClass();

    // Request notification from ETel about MS class change
    iPacketService.NotifyMSClassChange( iStatus, iMsClass );
    SetActive();
    }


// -----------------------------------------------------------------------------
// CMPMDtmWatcher::DoCancel
// -----------------------------------------------------------------------------
//
void CMPMDtmWatcher::DoCancel()
    {
    MPMLOGSTRING( "CMPMDtmWatcher::DoCancel" )

    iPacketService.CancelAsyncRequest( EPacketNotifyMSClassChange );
    }


// -----------------------------------------------------------------------------
// CMPMDtmWatcher::LogPacketClass
// -----------------------------------------------------------------------------
//    
void CMPMDtmWatcher::LogPacketClass() const
    {
#ifdef _DEBUG
    switch ( iMsClass )
        {
        // Active simultaneous PS and CS calls supported (Class A)
        case RPacketService::EMSClassDualMode:
            {
            MPMLOGSTRING( "CMPMDtmWatcher::LogPacketClass EMSClassDualMode" )
            break;
            }
        // Active CS and Suspended PS simultaneous calls supported (Class B)
        case RPacketService::EMSClassSuspensionRequired:
            {
            MPMLOGSTRING(
                "CMPMDtmWatcher::LogPacketClass EMSClassSuspensionRequired" )
            break;
            }
        // Active CS or Active PS only call supported (Class C)
        case RPacketService::EMSClassAlternateMode:
            {
            MPMLOGSTRING(
                "CMPMDtmWatcher::LogPacketClass EMSClassAlternateMode" )
            break;
            }
        // Active CS only call supported (Class C)
        case RPacketService::EMSClassCircuitSwitchedOnly:
            {
            MPMLOGSTRING(
                "CMPMDtmWatcher::LogPacketClass EMSClassCircuitSwitchedOnly" )
            break;
            }
        // Active PS only call supported (Class C)
        case RPacketService::EMSClassPacketSwitchedOnly:
            {
            MPMLOGSTRING(
                "CMPMDtmWatcher::LogPacketClass EMSClassPacketSwitchedOnly" )
            break;
            }
        // Unknown what configuration is supported
        case RPacketService::EMSClassUnknown:
            {
            MPMLOGSTRING( "CMPMDtmWatcher::LogPacketClass EMSClassUnknown" )
            break;
            }
        default:
            {
            MPMLOGSTRING(
                "CMPMDtmWatcher::LogPacketClass Packet class unspecified" )
            break;
            }
        }
#endif // _DEBUG

    return;
    }

//  End of File
