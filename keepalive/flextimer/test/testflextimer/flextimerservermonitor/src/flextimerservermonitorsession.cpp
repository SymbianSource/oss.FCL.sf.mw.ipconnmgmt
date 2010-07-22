/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *      Session implementation for FlexTimerServerMonitor server
 *
 */
/*
 * %version: 1 %
 */

// System include files

// User include files go here:
#include <rflextimer.h>
#include "flextimerservermonitorsession.h"
#include "flextimerservermonitorcommon.h"
#include "flextimerservermonitorserver.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "flextimerservermonitorsessionTraces.h"
#endif

// Constants

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------
//
CFlexTimerServerMonitorSession::CFlexTimerServerMonitorSession()
    {
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CFlexTimerServerMonitorSession::~CFlexTimerServerMonitorSession()
    {
    }

// --------------------------------------------------------------------------
// 2nd phase constructor
// --------------------------------------------------------------------------
//
void CFlexTimerServerMonitorSession::ConstructL()
    {
    }

// --------------------------------------------------------------------------
// Two-phased constructor
// --------------------------------------------------------------------------
//
CFlexTimerServerMonitorSession* CFlexTimerServerMonitorSession::NewL()
    {
    CFlexTimerServerMonitorSession* self =
            new (ELeave) CFlexTimerServerMonitorSession();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// Handle clients' requests. Due the request implementations are simple
// the actions are done in ServiceL and not dispatched to separate functions.
// --------------------------------------------------------------------------
//
void CFlexTimerServerMonitorSession::ServiceL( const RMessage2& aMessage )
    {
    // This ain't the most elegant solution but here we go.
    //
    // Passing the server's reference as a parameter during session creation 
    // would be another solution.
    CFlexTimerServerMonitorServer* server = 
        static_cast<CFlexTimerServerMonitorServer*>( 
            const_cast<CServer2*>( Server() ) );

    // Dispatch and execute the client's request
    switch ( aMessage.Function() )
        {
        case EFlexTimerServerMonitorStartMonitoring:
            {
            aMessage.Complete( server->StartMonitoring() );
            break;
            }
        case EFlexTimerServerMonitorHasServerCrashed:
            {
            TBool hasCrashed;

            TInt ret = server->GetStatus( hasCrashed );

            TPckgBuf<TBool> pckg( hasCrashed );
            aMessage.WriteL( 0, pckg );
            aMessage.Complete( ret );
            break;
            }
        case EFlexTimerServerMonitorStopMonitoring:
            {
            aMessage.Complete( server->StopMonitoring() );
            break;
            }
        default:
            aMessage.Complete( KErrNotSupported );
        }
    }
