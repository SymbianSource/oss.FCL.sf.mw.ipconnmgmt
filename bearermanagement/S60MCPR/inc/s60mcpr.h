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
* Description: S60 MCPR class that represents the MCPR NODE in MeshMachine
*
*/

/**
@file s60mcpr.h
S60 MCPR class that represents the MCPR NODE in MeshMachine
*/

#ifndef S60MCPR_H
#define S60MCPR_H

// MCPR includes 
#include <comms-infras/mobilitymcpr.h>

#include "s60mpmpolicyinterfaces.h" // MMPMPolicyNotificationUser
#include "s60mcprmpmlog.h"

// S60MCPR Panic definitions.
_LIT(KS60MCprPanic, "S60MCprPanic");
static const TInt KPanicNoActivity = 1;
static const TInt KPanicNoContext = 2;
static const TInt KPanicNoServiceProvider = 3;
static const TInt KPanicInvalidCActiveUsage = 4;
static const TInt KPanicUnsupportedMCPRState = 5;
static const TInt KPanicInConsistentMCPRState = 6;
static const TInt KPanicNoDataClient = 7;
static const TInt KPanicPeerMessage = 8;

// Forward declaration
class CMPMPolicyRequests;
class RConnPrefList;

/**
 * Message realm of S60MCPR.
 */
class TCFS60MCPRMessage 
    {         
  public: 
    enum { ERealmId = 0x101F6D4B };
  private: 
          enum 
            {         
            EMPMReselectBestIAPCompletedMsg = 1,
            EMPMIAPConnectionStartedCompletedMsg,
            EMPMIAPConnectionStoppedCompletedMsg,
            EMPMChooseBestIAPCompletedMsg,
            EMPMProcessErrorCompletedMsg,
            EMPMPreferredCarrierAvailableMsg,
            EMPMStartIAPNotificationMsg,
            EMPMErrorNotificationMsg,
            EMPMStartupErrorIgnoredMsg,
            EMPMStopIAPNotificationMsg,
            };
  public: 
    typedef Messages::TMessageSigVoid<EMPMReselectBestIAPCompletedMsg, TCFS60MCPRMessage::ERealmId>     TMPMReselectBestIAPCompletedMsg;
    typedef Messages::TMessageSigVoid<EMPMChooseBestIAPCompletedMsg, TCFS60MCPRMessage::ERealmId>       TMPMChooseBestIAPCompletedMsg;
    typedef Messages::TMessageSigNumber<EMPMProcessErrorCompletedMsg, TCFS60MCPRMessage::ERealmId>      TMPMProcessErrorCompletedMsg;
    typedef Messages::TMessageSigAny<EMPMPreferredCarrierAvailableMsg, TCFS60MCPRMessage::ERealmId>     TMPMPreferredCarrierAvailableMsg;
    typedef Messages::TMessageSigNumber<EMPMStartIAPNotificationMsg, TCFS60MCPRMessage::ERealmId>       TMPMStartIAPNotificationMsg;
    typedef Messages::TMessageSigNumber<EMPMErrorNotificationMsg, TCFS60MCPRMessage::ERealmId>          TMPMErrorNotificationMsg;
    typedef Messages::TMessageSigVoid<EMPMStartupErrorIgnoredMsg, TCFS60MCPRMessage::ERealmId>          TMPMStartupErrorIgnoredMsg;
    typedef Messages::TMessageSigNumber<EMPMStopIAPNotificationMsg, TCFS60MCPRMessage::ERealmId>        TMPMStopIAPNotificationMsg;
    };

/**
 * CS60MetaConnectionProvider
 * 
 * CMobilityMetaConnectionProvider implements Symbian core code for MCPR classes.
 * MMPMPolicyNotificationUser is the callback interface implementation for MPM notifications.
 * 
 * Class manages one Network-level connection using activities and MPM.
 */
NONSHARABLE_CLASS(CS60MetaConnectionProvider)
    :   public CMobilityMetaConnectionProvider, 
        public MMPMPolicyNotificationUser
    {
    friend class CS60MobilityActivity;
public:
    /**
     * Two-phased constructor.
     * @param aFactory reference to CMetaConnectionProviderFactoryBase
     * @param aProviderInfo reference to TProviderInfo
     */
    static CS60MetaConnectionProvider* NewL( ESock::CMetaConnectionProviderFactoryBase& aFactory,
                                             const ESock::TProviderInfo& aProviderInfo );
    /**
     * Copies selection preferences to S60MCPR member variable
     * @param aPrefs reference to selection preferences
     */
    void SetSelectionPrefs( ESock::TSelectionPrefs& aPrefs );
    /**
     * Gets selection preferences from S60MCPR member variable
     * @return reference to original selection preferences
     */
    const ESock::TSelectionPrefs& SelectionPrefs();
    /**
     * Set connection preference list
     * @param aConnPrefList New connection preference list
     */    
    void SetConnPrefList( const ESock::RConnPrefList &aConnPrefList );
    /**
     * Get connection preference list
     * @return reference to original selection preferences
     */    
    ESock::RConnPrefList ConnPrefList() const;
    /**
     * Set sub-session unique id
     * @param aSubSessionUniqueId
     */    
    void SetSubSessionUniqueId( const TSubSessionUniqueId aSubSessionUniqueId);
    /**
     * Get sub-session unique id
     * @return sub session unique id
     */    
    TSubSessionUniqueId SubSessionUniqueId() const;

    /**
     * Copies policy preferences to S60MCPR member variable
     * @param aPrefs reference to policy preferences
     */
    void SetPolicyPrefs( TPolicyConnPref& aPrefs );
    /**
     * Gets policy preferences from S60MCPR member variable
     * @return reference to policy selection preferences
     */
    TPolicyConnPref& PolicyPrefs();
    /**
     * Gets pointer to CMPMPolicyRequests.
     * @return pointer to CMPMPolicyRequests instance. 
     */
    CMPMPolicyRequests* Policy();
    /**
     * Used by DataClientStatusNotification only.
     * Modifies iDataClientStatusStarted.
     * 
     * @return ETrue if allowed to send status notification "started"
     */
    TBool RequestPermissionToSendStarted();
    /**
     * Used by DataClientStatusNotification only.
     * Modifies iDataClientStatusStarted.
     * 
     * @return ETrue if allowed to send status notification "stopped"
     */
    TBool RequestPermissionToSendStopped();
    
    /**
     * S60 MCPR implementation of CMobilityMetaConnectionProvider IsHandshakingNow()
     * 
     * @return ETrue if in the middle of mobility handshake
     */
    TBool IsHandshakingNow() const; //Used by other activities to check the mobility status
    
    /**
     * Sets handshaking flag. 
     */
    void SetHandshakingFlag();
    
    /**
     * Clears handshaking flag. 
     */
    void ClearHandshakingFlag();
    
    /**
     * Re-send last received policy notification if it exists.
     */
    void ReSendPolicyNotification();
    
    /**
     * Clears last received policy notification.
     */
    void ClearPolicyNotification();
    
public: // from MMPMPolicyNotificationUser
    
    /**
    * Called by AO listening MPM notifications.
    * @param aNotification Generic nsotification structure class
    */
    void PolicyNotification( TMpmNotification& aNotification );
    
protected:
    /**
     * C++ default constructor.
     * 
     * @param aFactory reference to CMetaConnectionProviderFactoryBase
     * @param aProviderInfo reference to TProviderInfo
     * @param aActivityMap reference to activities
     */
    CS60MetaConnectionProvider( ESock::CMetaConnectionProviderFactoryBase& aFactory,
                                const ESock::TProviderInfo& aProviderInfo,
                                const MeshMachine::TNodeActivityMap& aActivityMap );
    /**
    * 2nd phase constructor.
    */
    void ConstructL();
    
    /**
    * Destructor.
    */
    virtual ~CS60MetaConnectionProvider();
    
    /**
    * Message receiving function which is implemented in the base class.
    * 
    * @param aSender
    * @param aRecipient
    * @param aMessage
    */
    virtual void ReceivedL( const Messages::TRuntimeCtxId& aSender, 
                            const Messages::TNodeId& aRecipient, 
                            Messages::TSignatureBase& aMessage );
    
    /**
     * Stores the received policy notification.
     */
    void StorePolicyNotification( TMpmNotification& aNotification );
    
private:
    /**
     * Handle to Policy Server interaction implementation
     */
    CMPMPolicyRequests*     iPolicy;
    /**
     * Original selection preferences from IP TierMgr/application
     */
    ESock::TSelectionPrefs  iOrigPrefs;
    /**
     * Selection preferences from MPM. Contains selected AP,
     * IAP & NET for reconnect, startup or for roaming.
     */
    TPolicyConnPref         iPolicyPrefs;
    
    /**
     * In case PolicyServer sends preferred IAP faster than state machine allows,
     * the preferred IAP Id is stored for further 
     */
    TMPMMessageBuffer       iPendingNotification;

    /**
     * Used to filter out extra notifications because MPM only needs one 
     * started/stopped event. Implement as simple FlipFlop -flag.
     */
    TBool                   iDataClientStatusStarted : 1;
    /**
     * Synchronisation flags, private by design, can be modified only by CMobilityActivity
     */
    TBool                   iIsHandshakingNow : 1; 
    
    /**
     * Connection preference list
     */
    ESock::RConnPrefList    iConnPrefList;
        
    /**
     * Sub-session unique id
     */
    TSubSessionUniqueId iSubSessionUniqueId;        
    };

#endif //S60MCPR_H
