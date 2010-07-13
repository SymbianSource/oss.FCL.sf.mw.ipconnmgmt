/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: S60 MCPR simple select activity implementation
*
*/

/**
@file s60mcprsimpleselectactivity.cpp
S60 MCPR simple select activity implementation
*/

#include <comms-infras/ss_api_ext.h>
#include <comms-infras/esock_params_internal.h>
#include <comms-infras/esock_params.h>
#include <commdbconnpref.h>

#include <extendedconnpref.h>

#include "s60mcpractivities.h"
#include "s60mcprstates.h"

using namespace Messages;
using namespace MeshMachine;
using namespace ESock;
using namespace MCprActivities;
using namespace S60MCprActivities;

// -----------------------------------------------------------------------------
// S60MCprSelectActivity
// -----------------------------------------------------------------------------
//
namespace S60MCprSelectActivity
    {
    // Overrides CNodeActivityBase method implementation
    void CS60SelectActivity::Cancel(MeshMachine::TNodeContextBase& aContext)
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::CS60SelectActivity::Cancel()",(TInt*)&aContext.Node())        
        __ASSERT_DEBUG(aContext.iNodeActivity, User::Panic(KS60MCprPanic, KPanicNoActivity));
        CSelectNextLayerActivity& ac = 
            static_cast<CSelectNextLayerActivity&>( *aContext.iNodeActivity );
        
        if (!ac.iTierManager.IsNull() && ac.iTierManager == PostedToNodeId())
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::CS60SelectActivity::Cancel() Forwarding TCancel to TM",(TInt*)&aContext.Node())    
            RClientInterface::OpenPostMessageClose(TNodeCtxId(ActivityId(), iNode.Id()), PostedToNodeId(), TEBase::TCancel().CRef());
            SetError( KErrCancel );
            }
        else
            {
            CSelectNextLayerActivity::Cancel(aContext);
            }
        }
    
    MeshMachine::CNodeActivityBase* CS60SelectActivity::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
        {
        return new(ELeave) CS60SelectActivity( aActivitySig, aNode );
        }
    
    CS60SelectActivity::CS60SelectActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
        : CSelectNextLayerActivity(aActivitySig, aNode)
        {
        }

    // -----------------------------------------------------------------------------
    // TAwaitingSelectNextLayer
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT(TAwaitingSelectNextLayer, NetStateMachine::MState, TContext)
    TBool TAwaitingSelectNextLayer::Accept()
        {
		if ( !iContext.iMessage.IsMessage<TCFSelector::TSelect>() )
            {
            return EFalse;
            }

        __ASSERT_DEBUG( iContext.iPeer || iContext.iSender == iContext.NodeId(),
                        User::Panic(KS60MCprPanic, KPanicPeerMessage) );

        // Get MCPR
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();

        // Next find out if our selection needed update. 
        
        // Get's the active service provider.
        RMetaServiceProviderInterface* sp = (RMetaServiceProviderInterface*) node.ServiceProvider();
        // Initially NULL.
        if ( sp && sp->ProviderInfo().APId() != node.PolicyPrefs().IapId() )
            {
            // Active sp exists but it differs from the required.
            // Do not select complete yet.
            S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingSelectNextLayer::Accept() Active ServiceProvider but not the correct one.",(TInt*)&iContext.Node())
            return ETrue;
            }

        // If this node has any service providers (has already received some SelectComplete messages)
        // we forward these to the requesting client. The service providers forwarded here may include
        // all (selection has completed) or just a subset (selection still in progress) of this node's
        // possible service providers.
        TClientIter<TDefaultClientMatchPolicy> iter = 
            iContext.Node().GetClientIter<TDefaultClientMatchPolicy>( TClientType(TCFClientType::EServProvider) );
        RMetaServiceProviderInterface* serviceProvider = static_cast<RMetaServiceProviderInterface*>( iter++ );
        TCFSelector::TSelectComplete msg( TNodeId::NullId(), TProviderInfo() );
        while ( serviceProvider )
            {
            msg.iNodeId = serviceProvider->RecipientId();
            msg.iProviderInfo = serviceProvider->ProviderInfo();
            iContext.PostToSender( msg ); //We do not expect any answer so do not remember the peers TNodeId (SetPostedTo()).
            serviceProvider = static_cast<RMetaServiceProviderInterface*>( iter++ );
            }

        //If this node has already completed selection, we send a final SelectComplete(NULL).
        //if ( iContext.Node().IsSelectionCompleted() ) // private
        if ( sp )  // If active sp exists, the selection is completed
            {
            iContext.PostToSender( TCFSelector::TSelectComplete( TNodeId::NullId(),
                                                                 TProviderInfo()).CRef() );
            //We do not expect any answer so do not remember the peers TNodeId (SetPostedTo()).
            iContext.iMessage.ClearMessageId();
            return EFalse;
            }

        //Otherwise we perform the selection (start the activity or add the originator to the list).
        S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingSelectNextLayer::Accept()",(TInt*)&iContext.Node())
        return ETrue;
        }    
    
    // -----------------------------------------------------------------------------
    // Post ChooseBestIAP to MPM.
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT(TSimpleSelect, NetStateMachine::MStateTransition, TContext)
    void TSimpleSelect::DoL()
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TSimpleSelect::DoL()",(TInt*)&iContext.Node())
        
        // Send simple select to self
        iContext.iNodeActivity->PostRequestTo( iContext.NodeId(),
                                               TCFSelector::TSimpleSelect().CRef() );
        }

    // -----------------------------------------------------------------------------
    // Accepts state completion.
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT(TAwaitingSimpleSelectComplete, NetStateMachine::MState, TContext)
    TBool TAwaitingSimpleSelectComplete::Accept()
        {
        return EFalse;
        }

    // -----------------------------------------------------------------------------
    // S60 Selection logic in MCPR instead of TM!
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT(TSelectNextLayer, NetStateMachine::MStateTransition, TContext)
    void TSelectNextLayer::DoL()
        {
        __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KS60MCprPanic, KPanicNoActivity));
        CSelectNextLayerActivity& ac = 
            static_cast<CSelectNextLayerActivity&>( *iContext.iNodeActivity );

        // Get MCPR
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();

        // SNAP Attach
#ifdef _DEBUG    
        if ( node.SelectionPrefs().Prefs().ExtensionId() == TConnPref::EConnPrefProviderInfo &&
             node.SelectionPrefs().Scope()&TSelectionPrefs::ESelectFromExisting )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TSelectNextLayer::DoL() SNAP Attach problem!!!!",(TInt*)&iContext.Node())
            __ASSERT_DEBUG(EFalse, User::Panic(KS60MCprPanic, KPanicUnsupportedMCPRState));
            }
#endif //_DEBUG

		TSelectionPrefs prefs;

        ESock::RConnPrefList::TIter<TConnCSRPref> iterCSR = node.ConnPrefList().getIter<TConnCSRPref>();
        //There should be one and only one CSR pref
        ASSERT(iterCSR[0] != NULL && iterCSR[1] == NULL);
        TConnCSRPref* csrprefs = iterCSR[0];
		prefs.SetSubSessionUniqueId(node.SubSessionUniqueId());
		prefs.SetFlags(csrprefs->Flags());
		prefs.SetScope(csrprefs->Scope());

		/*** HACK ALERT!

		 */
		TConnAPPref* nextAP = TConnAPPref::NewL(node.PolicyPrefs().IapId());
		CleanupStack::PushL(nextAP);
		node.ConnPrefList().AppendL(nextAP);
		CleanupStack::Pop();
		/* END OF HACK */	
        // Attach. 399 attach shouldn't be visible here.
        if ( prefs.Scope()&TSelectionPrefs::ESelectFromExisting )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TSelectNextLayer::DoL() Attach ",(TInt*)&iContext.Node())
            TCFSelector::TSimpleSelect msg( prefs );
            iContext.iNodeActivity->PostRequestTo( ac.iTierManager, msg );
            }
        // Start
        else
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TSelectNextLayer::DoL() Start",(TInt*)&iContext.Node())

            TCommDbConnPref ippprefs;
            ippprefs.SetIapId( node.PolicyPrefs().IapId() );
            ippprefs.SetNetId( node.PolicyPrefs().NetId() );
            //node.MapPolicyPrefsToConnPrefL( node.PolicyPrefs(), tmppref );
            // don't use reference we want to preserve the original TSelectionPrefs.
            TSelectionPrefs newPrefs;
            newPrefs = prefs;
            newPrefs.SetPrefs( ippprefs );
            TCFSelector::TSimpleSelect msg(newPrefs);
            iContext.iNodeActivity->PostRequestTo(ac.iTierManager, msg);
            }     
        }
    }

// -----------------------------------------------------------------------------
// S60MCprSimpleSelectActivity
// -----------------------------------------------------------------------------
//
namespace S60MCprSimpleSelectActivity
    {
    // -----------------------------------------------------------------------------
    // TAwaitingSelectNextLayer
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT(TAwaitingSelectNextLayer, NetStateMachine::MState, TContext)
    TBool TAwaitingSelectNextLayer::Accept()
        {
        if ( !iContext.iMessage.IsMessage<TCFSelector::TSimpleSelect>() )
            {
            return EFalse;
            }

        __ASSERT_DEBUG( iContext.iPeer || iContext.iSender == iContext.NodeId(),
                        User::Panic(KS60MCprPanic, KPanicPeerMessage) );

        // Get MCPR
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();

        // Next find out if our selection needed update. 
        
        // Get's the active service provider.
        RMetaServiceProviderInterface* sp = (RMetaServiceProviderInterface*) node.ServiceProvider();
        // Initially NULL.
        if ( sp && sp->ProviderInfo().APId() != node.PolicyPrefs().IapId() )
            {
            // Active sp exists but it differs from the required.
            // Do not select complete yet.
            S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingSelectNextLayer::Accept() Active ServiceProvider but not the correct one.",(TInt*)&iContext.Node())
            return ETrue;
            }

        // If this node has any service providers (has already received some SelectComplete messages)
        // we forward these to the requesting client. The service providers forwarded here may include
        // all (selection has completed) or just a subset (selection still in progress) of this node's
        // possible service providers.
        TClientIter<TDefaultClientMatchPolicy> iter = 
            iContext.Node().GetClientIter<TDefaultClientMatchPolicy>( TClientType(TCFClientType::EServProvider) );
        RMetaServiceProviderInterface* serviceProvider = static_cast<RMetaServiceProviderInterface*>( iter++ );
        TCFSelector::TSelectComplete msg( TNodeId::NullId(), TProviderInfo() );
        while ( serviceProvider )
            {
            msg.iNodeId = serviceProvider->RecipientId();
            msg.iProviderInfo = serviceProvider->ProviderInfo();
            iContext.PostToSender( msg ); //We do not expect any answer so do not remember the peers TNodeId (SetPostedTo()).
            serviceProvider = static_cast<RMetaServiceProviderInterface*>( iter++ );
            }

        //If this node has already completed selection, we send a final SelectComplete(NULL).
        //if ( iContext.Node().IsSelectionCompleted() ) // private
        if ( sp )  // If active sp exists, the selection is completed
            {
            iContext.PostToSender( TCFSelector::TSelectComplete( TNodeId::NullId(),
                                                                 TProviderInfo()).CRef() );
            //We do not expect any answer so do not remember the peers TNodeId (SetPostedTo()).
            iContext.iMessage.ClearMessageId();
            return EFalse;
            }

        //Otherwise we perform the selection (start the activity or add the originator to the list).
        S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingSelectNextLayer::Accept()",(TInt*)&iContext.Node())
        return ETrue;
        }

    // -----------------------------------------------------------------------------
    // S60 Selection logic in MCPR instead of TM!
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT(TSelectNextLayer, NetStateMachine::MStateTransition, TContext)
    void TSelectNextLayer::DoL()
        {
        __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KS60MCprPanic, KPanicNoActivity));
        CSelectNextLayerActivity& ac = 
            static_cast<CSelectNextLayerActivity&>( *iContext.iNodeActivity );

        // Get MCPR
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();

        // SNAP Attach
#ifdef _DEBUG    
        if ( node.SelectionPrefs().Prefs().ExtensionId() == TConnPref::EConnPrefProviderInfo &&
             node.SelectionPrefs().Scope()&TSelectionPrefs::ESelectFromExisting )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TSelectNextLayer::DoL() SNAP Attach problem!!!!",(TInt*)&iContext.Node())
            __ASSERT_DEBUG(EFalse, User::Panic(KS60MCprPanic, KPanicUnsupportedMCPRState));
            }
#endif //_DEBUG

        TSelectionPrefs newPrefs;
        newPrefs = node.SelectionPrefs();
        newPrefs.SetSubSessionUniqueId( node.SubSessionUniqueId() );
        
        // Attach. 399 attach shouldn't be visible here.
        if ( node.SelectionPrefs().Scope()&TSelectionPrefs::ESelectFromExisting )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TSelectNextLayer::DoL() Attach ",(TInt*)&iContext.Node())
            TCFSelector::TSimpleSelect msg( newPrefs );
            iContext.iNodeActivity->PostRequestTo( ac.iTierManager, msg );
            }
        // Start
        else
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TSelectNextLayer::DoL() Start",(TInt*)&iContext.Node())
            TCommDbConnPref ippprefs;
            ippprefs.SetIapId( node.PolicyPrefs().IapId() );
            ippprefs.SetNetId( node.PolicyPrefs().NetId() );
            //node.MapPolicyPrefsToConnPrefL( node.PolicyPrefs(), tmppref );
            // don't use reference we want to preserve the original TSelectionPrefs.
            newPrefs.SetPrefs( ippprefs );
            TCFSelector::TSimpleSelect msg(newPrefs);
            iContext.iNodeActivity->PostRequestTo(ac.iTierManager, msg);
            }
        }

    // -----------------------------------------------------------------------------
    // Post ChooseBestIAP to MPM.
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT(TChooseBestIAP, NetStateMachine::MStateTransition, TContext)
    void TChooseBestIAP::DoL()
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TChooseBestIAP::DoL()",(TInt*)&iContext.Node())
        
        // Get MCPR
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();

        // Build up the PolicyRequest.
        TConnPref* pref = new ( ELeave ) TMpmConnPref();
        CleanupStack::PushL( pref );
        TConnPref* polpref = (TConnPref*)new ( ELeave ) TPolicyConnPref();
        CleanupStack::PushL( polpref );

        // Is it Ok to store references of both node and activity?
        // It should be ok since this is the activity we want to response for.
        CChooseBestIAPCb* cb = new( ELeave ) CChooseBestIAPCb( node, iContext.iNodeActivity );

        CleanupStack::PushL( cb );

        // Resolve application uid.
        TSubSessionUniqueId subSessionUniqueId = node.SubSessionUniqueId();

        TSecureId secureId( 0 );
        
        // Note that we use Symbian internal API to resolve the uid. 
        AIPCClientInfo clientinfo;
        TInt ret = clientinfo.ClientSid( subSessionUniqueId, secureId );
        if ( ret != KErrNone )
            {
            S60MCPRLOGSTRING2("S60MCPR<%x>::TChooseBestIAP::DoL() Failed to resolve Application UID %d",(TInt*)&iContext.Node(),ret)
            // Application UID may not exist. Use null id.
            secureId = KNullUid;
            }
        
        TUid id = TUid::Uid( secureId );
        
        S60MCPRLOGSTRING2("S60MCPR<%x>::TChooseBestIAP::DoL() Application UID=%x",(TInt*)&iContext.Node(),id);        

        // Copy original TConnPref.
        if (!node.ConnPrefList().Count())
            {
            *pref = node.SelectionPrefs().Prefs();
            }
        else
            {
            ESock::RConnPrefList::TIter<TExtendedConnPref> iterAP = node.ConnPrefList().getIter<TExtendedConnPref>();
            TExtendedConnPref *ptr = &(*iterAP);
            static_cast<TMpmConnPref*>(pref)->SetSnapPurpose ( ptr->SnapPurpose() );
            static_cast<TMpmConnPref*>(pref)->SetSnapId ( ptr->SnapId() );
            static_cast<TMpmConnPref*>(pref)->SetIapId ( ptr->IapId() );
            static_cast<TMpmConnPref*>(pref)->SetBearerSet ( ptr->BearerSet() );
            static_cast<TMpmConnPref*>(pref)->SetNoteBehaviour ( ptr->NoteBehaviour() );
            static_cast<TMpmConnPref*>(pref)->SetDisconnectDialog ( ptr->DisconnectDialog() );
            static_cast<TMpmConnPref*>(pref)->SetConnSelectionDialog ( ptr->ConnSelectionDialog() );
            static_cast<TMpmConnPref*>(pref)->SetForcedRoaming ( ptr->ForcedRoaming() );
            }

        // Send request
        node.Policy()->ChooseBestIAPL( pref, id, polpref, cb );

        // Pop three objects.
        CleanupStack::Pop( cb );
        CleanupStack::Pop( polpref );
        CleanupStack::Pop( pref );
        }

    // -----------------------------------------------------------------------------
    // Accepts state completion.
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT(TAwaitingChooseBestIAP, NetStateMachine::MState, TContext)
    TBool TAwaitingChooseBestIAP::Accept()
        {
        if ( iContext.iMessage.IsMessage<TCFS60MCPRMessage::TMPMChooseBestIAPCompletedMsg>() )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingChooseBestIAP::Accept() TMPMChooseBestIAPCompletedMsg Accepted", (TInt*)&iContext.Node())

            // MPM ChooseBestIAP completed.
            return ETrue;
            }
        else if ( iContext.iMessage.IsMessage<TEBase::TError>() )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingChooseBestIAP::Accept() NOT Accept TError", (TInt*)&iContext.Node())
            // Some error possibly sent by the MPM. We can let the core handle this.
            return EFalse;
            }
        else if ( iContext.iMessage.IsMessage<TEBase::TCancel>() )
            {
            // Cancellation from the RConnection::Start(). 
            // Cancel MPM asynch.
            S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingChooseBestIAP::Accept() NOT Accept TCancel, Cancelling ChooseBestIAPL()", (TInt*)&iContext.Node())

            // Get the MCPR
            CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();

            node.Policy()->CancelChooseBestIAPL();

            return EFalse;
            }
        else
            {
            // Else
            return EFalse;
            }
        }

    // -----------------------------------------------------------------------------
    // Decides if ChooseBestIAP is required.
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT(TChooseBestIAPRequiredOrNoTagOrSetChosenApActive, NetStateMachine::MStateFork, TContext)
    TInt TChooseBestIAPRequiredOrNoTagOrSetChosenApActive::TransitionTag()
        {
        __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KS60MCprPanic, KPanicNoActivity));

        // Initialize
		CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();
        TInt iapid = node.PolicyPrefs().IapId();
        TInt result = MeshMachine::KNoTag;
        RMetaServiceProviderInterface* sp = NULL;
        TBool boundButNotActiveMCPRfound = EFalse;

        // Search desired&bound MCPRs and check that EActive flag is not set.
        TClientIter<TDefaultClientMatchPolicy> iter = 
            node.GetClientIter<TDefaultClientMatchPolicy>(TCFClientType::EServProvider);
        for ( TInt i = 0; iter[i]; i++ )
            {
            sp = static_cast<RMetaServiceProviderInterface*>( iter[i] );
            // Make sure sp is not active and it exists
            if ( !( sp->Flags() & TCFClientType::EActive ) && 
                 sp->ProviderInfo().APId() == iapid )
                {
                boundButNotActiveMCPRfound = ETrue;
                break;
                }
            }
        
        // Set service provider to point to active service provider.
        sp = (RMetaServiceProviderInterface*) node.ServiceProvider();
        
        // If we are attaching to existing, we can use the passthrough preferences
        //
        if ( node.SelectionPrefs().Scope()&TSelectionPrefs::ESelectFromExisting )
            {
            S60MCPRLOGSTRING2("S60MCPR<%x>::TChooseBestIAPRequiredOrNoTagOrSetChosenApActive<%x>::TransitionTag() Selecting from existing",
                      (TInt*)&iContext.Node(),(TInt*)this);
            result = MeshMachine::KNoTag;
            }
        // If our policy selection doesn't match with the existing selection,
        // but we have one ( i.e. we're roaming or recovering from error ) 
        // We just need to select our IP Proto MCPRs.
        // Preferences should be in place and choose best iap is not required.
        //
        else if ( sp && sp->ProviderInfo().APId() != iapid )
            {
            S60MCPRLOGSTRING2("S60MCPR<%x>::TChooseBestIAPRequiredOrNoTagOrSetChosenApActive<%x>::TransitionTag() Selecting with existing preferences",(TInt*)&iContext.Node(),(TInt*)this);
            result = MeshMachine::KNoTag;
            }
        // If MCPR can be found but it's not active we can
        // just jump into setting it active.
        //
        else if ( boundButNotActiveMCPRfound )
            {
            S60MCPRLOGSTRING2("S60MCPR<%x>::TChooseBestIAPRequiredOrNoTagOrSetChosenApActive::TransitionTag() IP Proto MCPR of AP %d found but not Active",(TInt*)&iContext.Node(),iapid)
            result = S60MCprStates::KSetChosenApActive;
            }
        // We should execute the ChooseBestIAP query.
        //
        else
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TChooseBestIAPRequiredOrNoTagOrSetChosenApActive::TransitionTag() Selection from MPM required",(TInt*)&iContext.Node())
            result = S60MCprStates::KChooseBestIAPRequired;
            }
        
//  In other words:
//         return ( ( passThrough || justSelect ) ? 
//                    MeshMachine::KNoTag 
//                  : justSetActive ? 
//                          S60MCprStates::KSetChosenApActive
//                        : S60MCprStates::KChooseBestIAPRequired ) 
//               | NetStateMachine::EForward;

        return result | NetStateMachine::EForward;
        }

    // -----------------------------------------------------------------------------
    // TSetChosenApActiveOrSelectedProviderBackward
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT(TSetChosenApActiveOrSelectedProviderBackward, NetStateMachine::MStateFork, TContext)
    TInt TSetChosenApActiveOrSelectedProviderBackward::TransitionTag()
        {
        __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KS60MCprPanic, KPanicNoActivity));
        CSelectNextLayerActivity& ac = static_cast<CSelectNextLayerActivity&>( *iContext.iNodeActivity );
        if ( ac.SelectionChoices().Count() > ( ac.iCurrentIndex + 1 ) )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TSetChosenApActiveOrSelectedProviderBackward::TransitionTag() KSelectedProvider",(TInt*)&iContext.Node())
            return MCprStates::KSelectedProvider | NetStateMachine::EBackward;
            }
        S60MCPRLOGSTRING1("S60MCPR<%x>::TSetChosenApActiveOrSelectedProviderBackward::TransitionTag() KSetChosenApActive",(TInt*)&iContext.Node())
        return S60MCprStates::KSetChosenApActive | NetStateMachine::EForward;
        }

    // -----------------------------------------------------------------------------
    // Sets the chosen AP as active.
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT(TSetChosenApActive, NetStateMachine::MStateTransition, TContext)
    void TSetChosenApActive::DoL()
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TSetChosenApActive::DoL()",(TInt*)&iContext.Node())

        // Get the MCPR
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();

        // Get the policy server's selection result 
        TInt iapid = node.PolicyPrefs().IapId();
        // Get iterator from the base class.
        TClientIter<TDefaultClientMatchPolicy> iter = node.GetClientIter<TDefaultClientMatchPolicy>(TCFClientType::EServProvider);
        // Introduce handle to IPProtoMCPR
        RMetaServiceProviderInterface* sp = NULL;
        // Iterate and set the EActive flag when needed
        for ( TInt i = 0; iter[i]; i++ )
            {
            sp = static_cast<RMetaServiceProviderInterface*>(iter[i]);
            if ( sp->ProviderInfo().APId() == iapid ) // In IPProto the runtime APId is the IapId.
                {
                S60MCPRLOGSTRING2("S60MCPR<%x>::TSetChosenApActive::DoL() IP Proto MCPR of AP %d set Active",(TInt*)&iContext.Node(),iapid)
                sp->SetFlags( TCFClientType::EActive );
                break; // Leave sp selected
                }
            else if ( node.SelectionPrefs().Scope()&TSelectionPrefs::ESelectFromExisting &&
                      iapid == 0 ) // Not selected by MPM. Legacy Attach.
                {
                // Set IapId.
                node.PolicyPrefs().SetIapId( sp->ProviderInfo().APId() );
                // Inform MPM of used AP.
                node.Policy()->ApplicationJoinsConnectionL( node.PolicyPrefs().IapId() );
                break;
                }
#ifdef _DEBUG // allow urel use any in case of error. The missing TSetChosenApActive -log above indicates a problem.
            else
                {
                sp = NULL;
                }
#endif
            }
        // At this point we're not doing reconnect or anything like that. 
        // Therefore the Selected provider must be the same as the one picked by MPM and must be found.
        __ASSERT_DEBUG(sp, User::Panic(KS60MCprPanic, KPanicInConsistentMCPRState));
        }

    // -----------------------------------------------------------------------------
    // CChooseBestIAPCb::PolicyResponse
    // -----------------------------------------------------------------------------
    //
    void CChooseBestIAPCb::PolicyResponse( PolicyRequest& aCompletedRequest )
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::CChooseBestIAPCb::PolicyResponse()",(TInt*)&iNode)

        // Request to MPM failed for some reason
        if( aCompletedRequest.iStatus != KErrNone )
            {
            S60MCPRLOGSTRING2("S60MCPR<%x>::CChooseBestIAPCb::PolicyResponse() MPM Error %d",(TInt*)&iNode,aCompletedRequest.iStatus)

            TEBase::TError msg( TCFS60MCPRMessage::TMPMChooseBestIAPCompletedMsg::Id(), 
                                aCompletedRequest.iStatus );
            if ( iLastRequestOriginator.IsOpen() )
                {
                iLastRequestOriginator.ReplyTo( iNode.Id(), msg );
                iLastRequestOriginator.Close();
                }
            return;
            }

        // We only accept TPolicyConnPref from MPM.
        ASSERT( aCompletedRequest.iPolicyPref->ExtensionId() == TPolicyConnPref::EConnS60PolicyPref );

        TPolicyConnPref* policypref = (TPolicyConnPref*)aCompletedRequest.iPolicyPref;

        iNode.SetPolicyPrefs( *policypref );

        S60MCPRLOGSTRING3("S60MCPR<%x>::CChooseBestIAPCb::PolicyResponse() IAP %d NET %d",
                  (TInt*)&iNode,policypref->IapId(),policypref->NetId())

        TCFS60MCPRMessage::TMPMChooseBestIAPCompletedMsg req;
        if ( iLastRequestOriginator.IsOpen() )
            {
            iLastRequestOriginator.ReplyTo( iNode.Id(), req );
            iLastRequestOriginator.Close();
            }
        }
    } // namespace S60MCprSimpleSelectActivity

//  End of File
