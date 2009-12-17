/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: S60 MCPR implementation
*
*/


/**
@file s60mcpr.cpp
S60 MCPR implementation
*/

#include <comms-infras/ss_msgintercept.h>

#include "s60mcpr.h"
#include "s60mcprstates.h"
#include "s60mcpractivities.h"

#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

#include <comms-infras/ss_protopt.h>
#include "s60tcprecvwin.h"

#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

using namespace Messages;
using namespace MeshMachine;
using namespace ESock;
using namespace MCprActivities;

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::NewL
// -----------------------------------------------------------------------------
//
CS60MetaConnectionProvider* CS60MetaConnectionProvider::NewL( CMetaConnectionProviderFactoryBase& aFactory, 
                                                              const TProviderInfo& aProviderInfo )
    {
    S60MCPRLOGSTRING("S60MCPR::NewL()");

    CS60MetaConnectionProvider* self =
        new (ELeave) CS60MetaConnectionProvider( aFactory,
                                                 aProviderInfo,
                                                 S60MCprActivities::S60MCprActivityMap::Self());
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::CS60MetaConnectionProvider
// -----------------------------------------------------------------------------
//
CS60MetaConnectionProvider::CS60MetaConnectionProvider( CMetaConnectionProviderFactoryBase& aFactory,
                                                        const TProviderInfo& aProviderInfo, 
                                                        const MeshMachine::TNodeActivityMap& aActivityMap )
    :   CMobilityMetaConnectionProvider( aFactory, aProviderInfo, aActivityMap ),
        iDataClientStatusStarted( EFalse )
    {
    LOG_NODE_CREATE(KS60MCprTag, CS60MetaConnectionProvider);
    }

// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::ConstructL
// -----------------------------------------------------------------------------
//
void CS60MetaConnectionProvider::ConstructL()
    {
    CCoreMetaConnectionProvider::ConstructL();
    iPolicy = CMPMPolicyRequests::NewL( *(MMPMPolicyNotificationUser*)this );
    S60MCPRLOGSTRING2("S60MCPR<%x>::ConstructL() iPolicy=%x",(TInt*)this, (TInt*)iPolicy);
    
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    
    RMetaExtensionContainer mec;
    mec.Open(iAccessPointConfig);
    CleanupClosePushL(mec);
 
    //Append pointer to lookup table which holds the various TCP receive window sizes for different bearer types.
    CTCPReceiveWindowSize* receiveWindow;
    receiveWindow = new (ELeave)CS60TCPReceiveWindowSize();
        
    CleanupStack::PushL(receiveWindow);
    mec.AppendExtensionL(receiveWindow);
    CleanupStack::Pop(receiveWindow);
    
    //Append the pointer of CSAPSetOpt which provides generic SetOpt( ) implementation
    CSAPSetOpt* protoOption = new (ELeave)CSAPSetOpt();
    CleanupStack::PushL(protoOption);
    mec.AppendExtensionL(protoOption);
    CleanupStack::Pop(protoOption);

    iAccessPointConfig.Close();
    iAccessPointConfig.Open(mec);
    CleanupStack::PopAndDestroy(&mec);
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    }

// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::~CS60MetaConnectionProvider
// -----------------------------------------------------------------------------
//
CS60MetaConnectionProvider::~CS60MetaConnectionProvider()
    {
    if ( iPolicy )
        {
        iPolicy->RequestDelete();
        }
    S60MCPRLOGSTRING1("S60MCPR<%x>::~CS60MetaConnectionProvider",(TInt*)this);
    LOG_NODE_DESTROY(KS60MCprTag, CS60MetaConnectionProvider);
    }

// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::ReceivedL
// -----------------------------------------------------------------------------
//
void CS60MetaConnectionProvider::ReceivedL( const TRuntimeCtxId& aSender, 
                                            const TNodeId& aRecipient, 
                                            TSignatureBase& aMessage )
    {
    S60MCPRLOGSTRING2("S60MCPR<%x>::ReceivedL() aMessage=%d",(TInt*)this, aMessage.MessageId().MessageId());
    ESOCK_DEBUG_MESSAGE_INTERCEPT(aSender, aMessage, aRecipient);

    TNodeContext<CS60MetaConnectionProvider> ctx( *this, aMessage, aSender, aRecipient );
    CMobilityMetaConnectionProvider::Received( ctx );
    User::LeaveIfError( ctx.iReturn );
    }

// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::Policy
// -----------------------------------------------------------------------------
//
CMPMPolicyRequests* CS60MetaConnectionProvider::Policy()
    {
    return iPolicy;
    }

// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::RequestPermissionToSendStarted
// -----------------------------------------------------------------------------
//
TBool CS60MetaConnectionProvider::RequestPermissionToSendStarted()
    {
    if ( !iDataClientStatusStarted )
        {
        iDataClientStatusStarted = ETrue;
        return ETrue;
        }
    else
        {
        return EFalse; 
        }
    }

// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::RequestPermissionToSendStopped
// -----------------------------------------------------------------------------
//
TBool CS60MetaConnectionProvider::RequestPermissionToSendStopped()
    {
    if ( iDataClientStatusStarted )
        {
        iDataClientStatusStarted = EFalse;
        return ETrue;
        }
    else
        {
        return EFalse; 
        }
    }

// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::PolicyPrefs
// -----------------------------------------------------------------------------
//
TPolicyConnPref& CS60MetaConnectionProvider::PolicyPrefs()
    {
    return iPolicyPrefs;
    }

// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::SetPolicyPrefs
// -----------------------------------------------------------------------------
//
void CS60MetaConnectionProvider::SetPolicyPrefs( TPolicyConnPref& aPrefs )
    {
    iPolicyPrefs = aPrefs;
    }

// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::SetSelectionPrefs
// -----------------------------------------------------------------------------
//
void CS60MetaConnectionProvider::SetSelectionPrefs( TSelectionPrefs& aPrefs )
    {
    iOrigPrefs = aPrefs;
    }

// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::SelectionPrefs
// -----------------------------------------------------------------------------
//
const TSelectionPrefs& CS60MetaConnectionProvider::SelectionPrefs()
    { 
    return iOrigPrefs; 
    }

// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::SetConnPrefList
// -----------------------------------------------------------------------------
//
void CS60MetaConnectionProvider::SetConnPrefList( const ESock::RConnPrefList &aConnPrefList )
    {
    iConnPrefList = aConnPrefList;
    }

// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::ConnPrefList
// -----------------------------------------------------------------------------
//
ESock::RConnPrefList CS60MetaConnectionProvider::ConnPrefList() const
    {
    return iConnPrefList;
    }

// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::SetSubSessionUniqueId
// -----------------------------------------------------------------------------
//
void CS60MetaConnectionProvider::SetSubSessionUniqueId( const TSubSessionUniqueId aSubSessionUniqueId)
    {
    iSubSessionUniqueId = aSubSessionUniqueId;
    }

// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::SubSessionUniqueId
// -----------------------------------------------------------------------------
//
TSubSessionUniqueId CS60MetaConnectionProvider::SubSessionUniqueId() const
    {
    return iSubSessionUniqueId;
    }

// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::IsHandshakingNow
// -----------------------------------------------------------------------------
//
TBool CS60MetaConnectionProvider::IsHandshakingNow() const
    {
    return iIsHandshakingNow;
    }


// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::SetHandshakingFlag
// -----------------------------------------------------------------------------
//
void CS60MetaConnectionProvider::SetHandshakingFlag()
    {
    iIsHandshakingNow = ETrue;
    }


// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::ClearHandshakingFlag
// -----------------------------------------------------------------------------
//
void CS60MetaConnectionProvider::ClearHandshakingFlag()
    {
    iIsHandshakingNow = EFalse;
    }


// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::PolicyNotification
// -----------------------------------------------------------------------------
//
void CS60MetaConnectionProvider::PolicyNotification( TMpmNotification& aNotification )
    {
    switch ( aNotification.iMPMNotificationType )
        {
        case EMPMPreferredIAPAvailable:
            {
            TMpmNotificationPrefIAPAvailable& notification = 
                static_cast<TMpmNotificationPrefIAPAvailable&>(
                             const_cast<TMpmNotification&>( aNotification ) );

            //HandlePreferredIAPAvailable( const_cast<TMpmNotificationPrefIAPAvailable&>( notification ) );

            // Check if the TPrefIAPNotifInfo contains new or old IAP. 
            //
            if ( ServiceProvider() &&
                 ((RMetaServiceProviderInterface*)ServiceProvider())->ProviderInfo().APId() != notification.iNewIapId )
                {
                S60MCPRLOGSTRING2("S60MCPR<%x>::PolicyNotification() EMPMPreferredIAPAvailable IAP %d",(TInt*)this,notification.iNewIapId);

                // Store PolicyNotification
                // This could happen if PolicyServer sends notification too early.
                //
                StorePolicyNotification( aNotification );
                
                // Send preferred carrier message into meshmachine.
                //
                RNodeInterface ni;
                ni.OpenPostMessageClose( NodeId(), 
                                         NodeId(), 
                                         TCFS60MCPRMessage::TMPMPreferredCarrierAvailableMsg( (TAny*)&notification ).CRef() );
                }
#ifdef _DEBUG
            else
                {
                S60MCPRLOGSTRING2("S60MCPR<%x>::PolicyNotification() EMPMPreferredIAPAvailable SAME IAP %d",(TInt*)this,notification.iNewIapId );
                }
#endif
            break;
            }
        case EMPMMobilityErrorNotification:
            {
            TMpmNotificationError& notification = 
                static_cast<TMpmNotificationError&>(
                             const_cast<TMpmNotification&>( aNotification ) );
            
                S60MCPRLOGSTRING2("S60MCPR<%x>::PolicyNotification() EMPMMobilityErrorNotification %d",(TInt*)this,notification.iError);

            // Store PolicyNotification
            // This could happen if PolicyServer sends notification too early.
            //
            StorePolicyNotification( aNotification );

            // Send error notification into meshmachine.
            //
            RNodeInterface ni;
            ni.OpenPostMessageClose( NodeId(), 
                                     NodeId(), 
                                     TCFS60MCPRMessage::TMPMErrorNotificationMsg( notification.iError ).CRef() );
            break;
            }

        case EMPMClientErrorNotification:
            {
            TMpmNotificationError& notification = 
                static_cast<TMpmNotificationError&>(
                             const_cast<TMpmNotification&>( aNotification ) );
                        
            S60MCPRLOGSTRING2("S60MCPR<%x>::PolicyNotification() EMPMClientErrorNotification %d",(TInt*)this,notification.iError);

            PostToClients<TDefaultClientMatchPolicy>( TNodeCtxId( 0, Id() ),
                                                      TCFDataClient::TStop( notification.iError ).CRef(),
                                                      TClientType( TCFClientType::EData) );
            break;
            }

        case EMPMStartIAPNotification:
            {
            TMpmNotificationStartIAP& notification = 
                static_cast<TMpmNotificationStartIAP&>(
                             const_cast<TMpmNotification&>( aNotification ) );

            const TStartIAPNotifInfo& info = notification.iInfo;

            S60MCPRLOGSTRING2("S60MCPR<%x>::PolicyNotification() EMPMStartIAPNotification IAP %d",(TInt*)this,info.iIap);

            // Store PolicyNotification
            // This could happen if PolicyServer sends notification too early.
            //
            StorePolicyNotification( aNotification );
            
            // The TMPMStartIAPNotificationMsg only has effect if there's an activity waiting for it.
            //
            RNodeInterface ni;
            ni.OpenPostMessageClose( NodeId(), 
                                     NodeId(), 
                                     TCFS60MCPRMessage::TMPMStartIAPNotificationMsg( info.iIap ).CRef() ); 
            break;
            }
        case EMPMStopIAPNotification:
            {
            TMpmNotificationStopIAP& notification = 
                static_cast<TMpmNotificationStopIAP&>( const_cast<TMpmNotification&>( aNotification ) );

            // Allow TStop message only when ServiceProvider exists and the IAP matches or is unspecified. 
            // 
            if ( ServiceProvider() && 
                    (((RMetaServiceProviderInterface*)ServiceProvider())->ProviderInfo().APId() == notification.iInfo.iIap ||
                    notification.iInfo.iIap == 0 )) 
                {
                S60MCPRLOGSTRING2("S60MCPR<%x>::PolicyNotification() EMPMStopIAPNotification IAP %d",(TInt*)this,notification.iInfo.iIap);

                //TODO migrate to use MCPR's TStop and vertical msg down. Not supported at the moment.
                /*PostToClients<TDefaultClientMatchPolicy>( TNodeCtxId( 0, Id() ),
                                                          TCFServiceProvider::TStop( KErrCancel ).CRef(),
                                                          TClientType( TCFClientType::EData) );*/
                }
#ifdef _DEBUG
            else
                {
                S60MCPRLOGSTRING2("S60MCPR<%x>::PolicyNotification() EMPMStopIAPNotification NO MATCH! IAP %d",(TInt*)this,notification.iInfo.iIap);
                }
#endif
            break;
            }
        default:
            {
#ifdef _DEBUG
            // In debug builds, panic
            _LIT( KPanicMsg,
                  "S60MCPR::NotificationReceived: Invalid case" );
            User::Panic( KPanicMsg, KErrNotSupported );
#endif
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::ReSendPolicyNotification
// -----------------------------------------------------------------------------
//
void CS60MetaConnectionProvider::ReSendPolicyNotification()
    {
    if ( iPendingNotification.Length() > 0 )
        {
        // Typically deliver the notification and resubmit a request for new one.
        TUint8* msgBufferPtr = const_cast<TUint8*>( iPendingNotification.Ptr() );
        TMpmNotification* notification = reinterpret_cast<TMpmNotification*>( msgBufferPtr );

        S60MCPRLOGSTRING1("S60MCPR<%x>::ReSendPolicyNotification()",(TInt*)this);
        
        PolicyNotification( *const_cast<TMpmNotification*>( notification ) );
        }
    }


// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::ClearPolicyNotification
// -----------------------------------------------------------------------------
//
void CS60MetaConnectionProvider::ClearPolicyNotification()
    {
    iPendingNotification.Zero();
    }


// -----------------------------------------------------------------------------
// CS60MetaConnectionProvider::StorePolicyNotification
// -----------------------------------------------------------------------------
//
void CS60MetaConnectionProvider::StorePolicyNotification( TMpmNotification& aNotification )
    {
    // Store PolicyNotification
    // This could happen if PolicyServer sends notification too early.
    //
    if ( iPendingNotification.Length() == 0 )
        {
        ASSERT(Mem::Copy((TAny*)iPendingNotification.Ptr(), &aNotification, aNotification.Length()));
        iPendingNotification.SetLength( aNotification.Length() );
        }
#ifdef _DEBUG
    else
        {
        S60MCPRLOGSTRING2("S60MCPR<%x>::StorePolicyNotification() Notification already exists",
                  (TInt*)this, reinterpret_cast<TMpmNotification*>( &iPendingNotification )->iMPMNotificationType);
        }
#endif
    }
	
// End of file

