/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Connection method manager server.
*
*/


#include <cmconnectionmethoddef.h>
#include <cmpluginpacketdatadef.h>
#include <cmpluginwlandef.h>

#include "cmmserver.h"
#include "cmmsession.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmserverTraces.h"
#endif


const TUint CCmmServer::iCmmRangeCount = 12;

const TInt CCmmServer::iCmmRanges[iCmmRangeCount] =
    {
    0,      //   0-10
    11,     //  11-14
    15,     //  15-21
    22,     //  22-99
    100,    // 100-112
    113,    // 113-126
    127,    // 127-199
    200,    // 200-205
    206,    // 206-210
    211,    // 211-214
    215,    // 215-220
    221     // 221-KMaxInt
    };

const TUint8 CCmmServer::iCmmElementIndex[iCmmRangeCount] =
    {
    CPolicyServer::EAlwaysPass,     // 0-10
    CPolicyServer::ECustomCheck,    // 11-14
    CPolicyServer::ECustomCheck,    // 15-21
    CPolicyServer::ENotSupported,   // 22-99
    CPolicyServer::EAlwaysPass,     // 100-112
    CPolicyServer::ECustomCheck,    // 113-126
    CPolicyServer::ENotSupported,   // 127-199
    CPolicyServer::EAlwaysPass,     // 200-205
    CPolicyServer::ECustomCheck,    // 206-210
    CPolicyServer::ECustomCheck,    // 211-214
    CPolicyServer::ECustomCheck,    // 215-220
    CPolicyServer::ENotSupported    // 221->
    };

const CPolicyServer::TPolicyElement CCmmServer::iCmmElements[] =
    {
    {_INIT_SECURITY_POLICY_C1(ECapabilityReadDeviceData), CPolicyServer::EFailClient},
    {_INIT_SECURITY_POLICY_C1(ECapabilityWriteDeviceData), CPolicyServer::EFailClient},
    {_INIT_SECURITY_POLICY_C2(ECapabilityReadDeviceData, ECapabilityWriteDeviceData), 
            CPolicyServer::EFailClient},
    {_INIT_SECURITY_POLICY_C1(ECapabilityNetworkControl), CPolicyServer::EFailClient},
    };

const CPolicyServer::TPolicy CCmmServer::iCmmPolicy = //TODO, check comments
    {
    CPolicyServer::EAlwaysPass, // Specifies all connect attempts should pass
    iCmmRangeCount,     // Count of ranges
    iCmmRanges,         // 0-999, 1000-1008, 1009...
    iCmmElementIndex,   // Only range 1000-1008 are checked
    iCmmElements        // The list of policy elements
    };


// -----------------------------------------------------------------------------
// CCmmServer::CustomSecurityCheckL
// Implements custom security checking for IPCs marked with
// TSpecialCase::ECustomCheck.
// -----------------------------------------------------------------------------
//
CPolicyServer::TCustomResult CCmmServer::CustomSecurityCheckL(
        const RMessage2& aMessage,
        TInt& /*aAction*/,
        TSecurityInfo& /*aMissing*/ )
    {
    switch ( aMessage.Function() )
        {
        // ***********  2nd range: 11-14 *************
        case ECmmGetConnMethodInfoInt:
        case ECmmGetConnMethodInfoBool:
        case ECmmGetConnMethodInfoString:
        case ECmmGetConnMethodInfoString8:
        // ***********  10th range: 211-214 *************
        case ECMGetIntAttribute:
        case ECMGetBoolAttribute:
        case ECMGetStringAttribute:
        case ECMGetString8Attribute:
            {
            switch ( aMessage.Int0() )
                {
                // Private userdata has to be checked.
                // WPA:
                case CMManager::EWlanEnableWpaPsk:
                case CMManager::EWlanWpaPreSharedKey:
                // WEP:
                case CMManager::EWlanWepKey1InHex:
                case CMManager::EWlanWepKey2InHex:
                case CMManager::EWlanWepKey3InHex:
                case CMManager::EWlanWepKey4InHex:
                case CMManager::EWlanWepKey1InAscii:
                case CMManager::EWlanWepKey2InAscii:
                case CMManager::EWlanWepKey3InAscii:
                case CMManager::EWlanWepKey4InAscii:
                case CMManager::EWlanWepKeyIndex:
                // WAPI:
                case CMManager::EWlanWapiPsk:
                case CMManager::EWlanWapiPskFormat:
                // 802.1x:
                case CMManager::EWlan802_1xAllowUnencrypted: //TODO, should this be here?
                // Authentication:
                case CMManager::ECmIFAuthName:
                case CMManager::EPacketDataIFAuthName:
                case CMManager::ECmIFAuthPass:
                case CMManager::EPacketDataIFAuthPass:
                    {
                    // ECapabilityReadDeviceData
                    return ( iCmmElements[0].iPolicy.CheckPolicy( aMessage ) ) ?
                        EPass : EFail;
                    }
                default:
                    // By default reading does not need any capabilities
                    return EPass;
                }
            }
        // ***********  3rd range: 15-21 *************
        case ECmmUpdateBearerPriorityArray:
        case ECmmWriteDefaultConnection:
        case ECmmWriteGeneralConnectionSettings:
            {
            // ECapabilityWriteDeviceData is needed for writing
            return ( iCmmElements[1].iPolicy.CheckPolicy( aMessage ) ) ?
                EPass : EFail;
            }
        case ECmmCopyConnMethod:
        case ECmmMoveConnMethod:
        case ECmmRemoveConnMethod:
        case ECmmRemoveAllReferences:
            {
            // At this phase capability is not checked because of
            // it's too heavy operation...
            // At later phase the protection of the target destination
            // and/or connection method is checked.
            return EPass;
            }
        // ***********  6th range: 113-126 *************
        case EDestCreateDestinationWithName:
        case EDestCreateDestinationWithNameAndId:
            {
            // ECapabilityWriteDeviceData is needed for writing
            return ( iCmmElements[1].iPolicy.CheckPolicy( aMessage ) ) ?
                EPass : EFail;
            }
        case EDestIsConnected:
        case EDestAddConnMethod:
        case EDestAddEmbeddedDestination:
        case EDestDeleteConnMethod:
        case EDestRemoveConnMethod:
        case EDestModifyPriority:
        case EDestSetName:
        case EDestSetMetadata:
        case EDestSetProtection:
        case EDestSetHidden:
        case EDestUpdate:
        case EDestDelete:
            {
            // At this phase capability is not checked because of
            // it's too heavy operation...
            // At Session phase the protection of the destination and/or
            // connection method is checked. If destination/connection method
            // is protected ECapabilityNetworkControl is required from a
            // client.
            return EPass;
            }
        // ***********  9th range: 206-210 *************
        case ECMCreateConnMethod:
        case ECMCreateConnMethodWithId:
            {
            // ECapabilityWriteDeviceData is needed for writing
            return ( iCmmElements[1].iPolicy.CheckPolicy( aMessage ) ) ?
                EPass : EFail;
            }
        case ECMCreateConnMethodToDest:
        case ECMCreateConnMethodToDestWithId:
        case ECMCreateCopyOfExisting:
            {
            // At this phase capability is not checked because of
            // it's too heavy operation...
            // At Session phase the protection of the destination and/or
            // connection method is checked.
            return EPass;
            }
        // ***********  11th range: 215-220 *************
        case ECMSetIntAttribute:
        case ECMSetBoolAttribute:
        case ECMSetStringAttribute:
        case ECMSetString8Attribute:
            {
            switch ( aMessage.Int0() )
                {
                // WPA:
                case CMManager::EWlanEnableWpaPsk:
                case CMManager::EWlanWpaPreSharedKey:
                // WEP:
                case CMManager::EWlanWepKey1InHex:
                case CMManager::EWlanWepKey2InHex:
                case CMManager::EWlanWepKey3InHex:
                case CMManager::EWlanWepKey4InHex:
                case CMManager::EWlanWepKey1InAscii:
                case CMManager::EWlanWepKey2InAscii:
                case CMManager::EWlanWepKey3InAscii:
                case CMManager::EWlanWepKey4InAscii:
                case CMManager::EWlanWepKeyIndex:
                // WAPI:
                case CMManager::EWlanWapiPsk:
                case CMManager::EWlanWapiPskFormat:
                // 802.1x:
                case CMManager::EWlan802_1xAllowUnencrypted: //TODO, should this be here?
                // Authentication:
                case CMManager::ECmIFAuthName:
                case CMManager::EPacketDataIFAuthName:
                case CMManager::ECmIFAuthPass:
                case CMManager::EPacketDataIFAuthPass:
                    {
                    // ECapabilityReadDeviceData
                    return ( iCmmElements[2].iPolicy.CheckPolicy( aMessage ) ) ?
                        EPass : EFail;
                    }
                default:
                    {
                    // ECapabilityWriteDeviceData
                    return ( iCmmElements[1].iPolicy.CheckPolicy( aMessage ) ) ?
                        EPass : EFail;
                    }
                }
            }
        case ECMDelete:
        case ECMUpdate:
            {
            // ECapabilityWriteDeviceData
            return ( iCmmElements[1].iPolicy.CheckPolicy( aMessage ) ) ?
                EPass : EFail;
            }
        default:
            return EPass;
        }
    }

CCmmServer::CCmmServer( TInt aPriority ) : CPolicyServer( aPriority, iCmmPolicy )
    {
    OstTraceFunctionEntry0( CCMMSERVER_CCMMSERVER_ENTRY );

    iSessionCount = 0;
    iCmManager = NULL;
    iContainerIndex = NULL;
    iShutdown = NULL;

    OstTraceFunctionExit0( CCMMSERVER_CCMMSERVER_EXIT );
    }

CCmmServer::~CCmmServer()
    {
    OstTraceFunctionEntry0( DUP1_CCMMSERVER_CCMMSERVER_ENTRY );

    delete iCmManager;
    delete iContainerIndex;
    delete iShutdown;

    OstTraceFunctionExit0( DUP1_CCMMSERVER_CCMMSERVER_EXIT );
    }

CCmmServer* CCmmServer::NewL()
    {
    OstTraceFunctionEntry0( CCMMSERVER_NEWL_ENTRY );

    CCmmServer* server = CCmmServer::NewLC();
    CleanupStack::Pop( server );

    OstTraceFunctionExit0( CCMMSERVER_NEWL_EXIT );
    return server;
    }

CCmmServer* CCmmServer::NewLC()
    {
    OstTraceFunctionEntry0( CCMMSERVER_NEWLC_ENTRY );

    CCmmServer* server = new( ELeave ) CCmmServer( EPriorityNormal );
    CleanupStack::PushL( server );
    server->ConstructL();

    OstTraceFunctionExit0( CCMMSERVER_NEWLC_EXIT );
    return server;
    }

void CCmmServer::ConstructL()
    {
    OstTraceFunctionEntry0( CCMMSERVER_CONSTRUCTL_ENTRY );

    StartL( KCmmServer );

    iCmManager = CCmManagerImpl::NewL();

    // Create the object container index.
    iContainerIndex = CObjectConIx::NewL();

    // Construct shutdown timer.
    iShutdown = new( ELeave ) CCmmDelayedShutdown( this );
    iShutdown->ConstructL();

    OstTraceFunctionExit0( CCMMSERVER_CONSTRUCTL_EXIT );
    }

// -----------------------------------------------------------------------------
// Create a new session.
// -----------------------------------------------------------------------------
//
CSession2* CCmmServer::NewSessionL(
        const TVersion& /*aVersion*/,
        const RMessage2& /*aMessage*/ ) const
    {
    OstTraceFunctionEntry0( CCMMSERVER_NEWSESSIONL_ENTRY );

    return CCmmSession::NewL( *const_cast<CCmmServer*>( this ), iCmManager->Cache() );
    }

// -----------------------------------------------------------------------------
// Increments the session counter. Cancels the shutdown timer if active.
// -----------------------------------------------------------------------------
//
void CCmmServer::IncrementSessions()
    {
    OstTraceFunctionEntry0( CCMMSERVER_INCREMENTSESSIONS_ENTRY );

    iSessionCount++;
    iShutdown->Cancel();

    OstTraceFunctionExit0( CCMMSERVER_INCREMENTSESSIONS_EXIT );
    }

// -----------------------------------------------------------------------------
// Decrements the session counter. Starts the shutdown counter when last
// session closes.
// -----------------------------------------------------------------------------
//
void CCmmServer::DecrementSessions()
    {
    OstTraceFunctionEntry0( CCMMSERVER_DECREMENTSESSIONS_ENTRY );

    iSessionCount--;

    // Terminate the server when there are no clients left.
    if ( iSessionCount <= 0 )
        {
        if ( !iShutdown->IsActive() )
            {
            iShutdown->Start();
            }
        }
    OstTraceFunctionExit0( CCMMSERVER_DECREMENTSESSIONS_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmServer::NewContainerL
// -----------------------------------------------------------------------------
//
CObjectCon* CCmmServer::NewContainerL()
    {
    OstTraceFunctionEntry0( CCMMSERVER_NEWCONTAINERL_ENTRY );

    return iContainerIndex->CreateL();
    }

// -----------------------------------------------------------------------------
// CCmmServer::RemoveContainer
// -----------------------------------------------------------------------------
//
void CCmmServer::RemoveContainer( CObjectCon* aContainer )
    {
    OstTraceFunctionEntry0( CCMMSERVER_REMOVECONTAINER_ENTRY );

    iContainerIndex->Remove( aContainer );

    OstTraceFunctionExit0( CCMMSERVER_REMOVECONTAINER_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmServer::RunError
// -----------------------------------------------------------------------------
//
TInt CCmmServer::RunError( TInt aError )
    {
    OstTraceFunctionEntry0( CCMMSERVER_RUNERROR_ENTRY );

    Message().Complete( aError );

    // The leave will result in an early return from CServer::RunL(),
    // skipping the call to request another message. So we issue the
    // request here in order to keep the server running.
    ReStart();

    OstTraceFunctionExit0( CCMMSERVER_RUNERROR_EXIT );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCmmServer::CmManager
// -----------------------------------------------------------------------------
//
CCmManagerImpl* CCmmServer::CmManager()
    {
    OstTraceFunctionEntry0( CCMMSERVER_CMMANAGER_ENTRY );

    OstTraceFunctionExit0( CCMMSERVER_CMMANAGER_EXIT );

    return iCmManager;
    }

// -----------------------------------------------------------------------------
// Check for restrictions for adding an embedded destination from destination
// instances of all clients.
// - aDestinationId is the ID of the destination where a destination is beeing
//   embedded.
// - aEmbeddedDestinationId is the ID of the destination that is beeing
//   embedded.
// -----------------------------------------------------------------------------
//
TBool CCmmServer::EmbeddedDestinationConflictsFromAllSessions(
        const TUint32& aDestinationId,
        const TUint32& aEmbeddedDestinationId )
    {
    OstTraceFunctionEntry0( CCMMSERVER_EMBEDDEDDESTINATIONCONFLICTSFROMALLSESSIONS_ENTRY );

    TBool result( EFalse );
    CSession2* session( NULL );

    // Iterate through all sessions.
    iSessionIter.SetToFirst();
    for ( session = iSessionIter++; session; session = iSessionIter++ )
        {
        CCmmSession* cmmSession = static_cast<CCmmSession*>( session );
        if ( cmmSession->EmbeddedDestinationConflicts( aDestinationId, aEmbeddedDestinationId ) )
            {
            result = ETrue;
            break;
            }
        }
    OstTraceFunctionExit0( CCMMSERVER_EMBEDDEDDESTINATIONCONFLICTSFROMALLSESSIONS_EXIT );

    return result;
    }

// ---------------------------------------------------------------------------
// Notify each session about a destination/connection method that has been
// updated to, or deleted from, database.
// ---------------------------------------------------------------------------
//
void CCmmServer::RefreshHandlesForAllSessions( const TUint32& aId )
    {
    OstTraceFunctionEntry0( CCMMSERVER_REFRESHHANDLESFORALLSESSIONS_ENTRY );

    CSession2* session( NULL );

    // Iterate through all sessions.
    iSessionIter.SetToFirst();
    for ( session = iSessionIter++; session; session = iSessionIter++ )
        {
        CCmmSession* cmmSession = static_cast<CCmmSession*>( session );
        cmmSession->RefreshHandles( aId );
        }
    OstTraceFunctionExit0( CCMMSERVER_REFRESHHANDLESFORALLSESSIONS_EXIT );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCmmDelayedShutdown::CCmmDelayedShutdown
// -----------------------------------------------------------------------------
//
CCmmDelayedShutdown::CCmmDelayedShutdown( CCmmServer* aServer )
        :
        CActive( 0 ),
        iServer( aServer )
    {
    }

// -----------------------------------------------------------------------------
// CCmmDelayedShutdown::ConstructL
// -----------------------------------------------------------------------------
//
void CCmmDelayedShutdown::ConstructL()
    {
    OstTraceFunctionEntry0( CCMMDELAYEDSHUTDOWN_CONSTRUCTL_ENTRY );

    CActiveScheduler::Add( this );
    User::LeaveIfError( iTimer.CreateLocal() );

    OstTraceFunctionExit0( CCMMDELAYEDSHUTDOWN_CONSTRUCTL_EXIT );
    }

// Destructor
CCmmDelayedShutdown::~CCmmDelayedShutdown()
    {
    OstTraceFunctionEntry0( CCMMDELAYEDSHUTDOWN_CCMMDELAYEDSHUTDOWN_ENTRY );

    Cancel();
    iTimer.Close();
    iServer = NULL;

    OstTraceFunctionExit0( CCMMDELAYEDSHUTDOWN_CCMMDELAYEDSHUTDOWN_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmMonDelayedShutdown::Start
// -----------------------------------------------------------------------------
//
void CCmmDelayedShutdown::Start()
    {
    OstTraceFunctionEntry0( CCMMDELAYEDSHUTDOWN_START_ENTRY );

    if ( IsActive() )
        {
        OstTraceFunctionExit0( CCMMDELAYEDSHUTDOWN_START_EXIT );

        return;
        }

    iTimer.After( iStatus, KCmmShutdownDelay );
    SetActive();

    OstTraceFunctionExit0( DUP1_CCMMDELAYEDSHUTDOWN_START_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmMonDelayedShutdown::DoCancel
// -----------------------------------------------------------------------------
//
void CCmmDelayedShutdown::DoCancel()
    {
    OstTraceFunctionEntry0( CCMMDELAYEDSHUTDOWN_DOCANCEL_ENTRY );

    iTimer.Cancel();

    OstTraceFunctionExit0( CCMMDELAYEDSHUTDOWN_DOCANCEL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmmMonDelayedShutdown::RunL
// -----------------------------------------------------------------------------
//
void CCmmDelayedShutdown::RunL()
    {
    OstTraceFunctionEntry0( CCMMDELAYEDSHUTDOWN_RUNL_ENTRY );

    CActiveScheduler::Stop();

    OstTraceFunctionExit0( CCMMDELAYEDSHUTDOWN_RUNL_EXIT );
    }

// End of file
