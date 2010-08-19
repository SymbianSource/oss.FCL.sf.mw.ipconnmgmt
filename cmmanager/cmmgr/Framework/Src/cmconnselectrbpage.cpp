/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of plugin base class
*
*/


#include <cmmanager.rsg>
#include <StringLoader.h>

#include "cmconnselectrbpage.h"
#include "cmmanager.hrh"
#include "cmconnsettingsuiimpl.h"
#include "cmlogger.h"

using namespace CMManager;

// ---------------------------------------------------------------------------
// CCMConnSelectRBPage::CCMConnSelectRBPage
// ---------------------------------------------------------------------------
//
CCMConnSelectRBPage::CCMConnSelectRBPage( TInt aDialogResourceId, 
                                    TInt& aCurrentSelectionIndex, 
                                    const MDesCArray* aItemArray, 
                                    CCmManagerImpl& aCmManagerImpl, 
                                    TCmDCSettingSelectionMode& aSelectionMode,
                                    RArray<TUint32>& aDestinations,
                                    TBool areDestinations,
                                    TBool aOpenDestination,
                                    TCoeContextName& aContext )
    : CAknRadioButtonSettingPage( aDialogResourceId, 
                                  aCurrentSelectionIndex, 
                                  aItemArray )
	, iDialogResourceId (aDialogResourceId)
    , iCmManagerImpl( aCmManagerImpl )
    , iSelectionMode( aSelectionMode )
    , iDestinations( aDestinations )
    , iAreDestinations( areDestinations )
    , iOpenDestination( aOpenDestination )
    , iHelpContext ( aContext )
    , iFirstEnter( ETrue )
    {
    CLOG_CREATE;
    iPrevItem = iDestinations[aCurrentSelectionIndex];
    }

// ---------------------------------------------------------------------------
// CCMConnSelectRBPage::~CCMConnSelectRBPage
// ---------------------------------------------------------------------------
//
CCMConnSelectRBPage::~CCMConnSelectRBPage()
    {
    CLOG_CLOSE;    
    }

// ---------------------------------------------------------------------------
// CCMConnSelectRBPage::OkToExitL
// Good to know : EAknSoftkeyCancel is never called, because
// EEikDialogFlagNotifyEsc flag is not set in the resource.
// ---------------------------------------------------------------------------
//
TBool CCMConnSelectRBPage::OkToExitL( TInt aButtonId )
    {
    LOGGER_ENTERFN("CCMConnSelectRBPage::OkToExitL");

    // Convert the button presses into commands for the appui & current
    // view to handle
    TBool retval( EFalse );
    if ( aButtonId ==  EAknSoftkeyOk )
        {
        ProcessCommandL( ECmManagerUiCmdDestSelect );
        retval = ETrue;        
        }
    else 
        {
        if ( aButtonId == EAknSoftkeyOptions )
            {
            //*iSelected = iListbox->CurrentItemUid();
            //retval = ETrue;
            DisplayMenuL();
            retval = EFalse;  
            }
        else
            {
           	retval = ETrue;
            }
        }

    return retval;
    }

// ---------------------------------------------------------------------------
// CCMConnSelectRBPage::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CCMConnSelectRBPage::ProcessCommandL( TInt aCommandId )
    {
    LOGGER_ENTERFN("CCMConnSelectRBPage::ProcessCommandL");
    
    if ( MenuShowing() )
        {
        HideMenu();
        }

    switch ( aCommandId )
        {
        case EAknSoftkeyOptions:
            {
            DisplayMenuL();
            break;
            }

        case EAknSoftkeyCancel:
            {
            AttemptExitL(EFalse);   
            break;
            }

        case EAknSoftkeyOk:                   
        case EAknSoftkeySelect:                   
        case ECmManagerUiCmdDestSelect:
            {
            TUint selectedItem = 
                    iDestinations[ ListBoxControl()->CurrentItemIndex() ];
            if ( selectedItem == KDestItemAlwaysAsk )
                {
                iSelectionMode = EDCAlwaysAsk;
                }
            else if ( selectedItem == KDestItemAskOnce )
                {
                iSelectionMode = EDCAskOnce;
                }
            else if ( selectedItem == KDestItemDefaultConnection )
                {
                iSelectionMode = EDCDefaultConnection;
                }
            else if ( selectedItem == KDestItemUncategorized )
                {
                iSelectionMode = EDCConnectionMethod;
                }
            else 
                {
                if (iAreDestinations)
                    {
                    iSelectionMode = EDCDestination;
                    }
                else
                    {
                    iSelectionMode = EDCConnectionMethod;                        
                    }    
                }
            //iId is to be defined in the caller class   
            SelectCurrentItemL();         
            AttemptExitL(ETrue);
            break;
            }

        case ECmManagerUiCmdCMSelect:
            {
            iSelectionMode = EDCConnectionMethod;
            SelectCurrentItemL();         
            AttemptExitL( ETrue );                   
            break;
            }
        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                                    iEikonEnv->EikAppUi()->AppHelpContextL() );
            }
            break;
        default:
            {
            break;
            }
        }
    }    

// ---------------------------------------------------------------------------
// CCMConnSelectRBPage::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CCMConnSelectRBPage::DynInitMenuPaneL( TInt aResourceId,
                                         CEikMenuPane* aMenuPane )
    {
    LOGGER_ENTERFN("CCMConnSelectRBPage::DynInitMenuPaneL");

    CAknSettingPage::DynInitMenuPaneL( aResourceId, aMenuPane );
    if ( aResourceId == R_SELECT_NETW_CONN_MENU )
        {
    	if (!iCmManagerImpl.IsHelpOn())
            {
            aMenuPane->DeleteMenuItem( EAknCmdHelp );		    
            }
        TUint32 currentItem = 
                    iDestinations[ ListBoxControl()->CurrentItemIndex() ];
        
        // Always Ask ( always the first item )
        if ( (currentItem == KDestItemAlwaysAsk) || 
             (currentItem == KDestItemAskOnce) || 
             (currentItem == KDestItemDefaultConnection) ||
              !iOpenDestination )
            {
            aMenuPane->SetItemDimmed( ECmManagerUiCmdCMSelect, ETrue );
            }        
        else if ( ( currentItem != KDestItemAlwaysAsk ) && 
                  ( currentItem != KDestItemAskOnce ) && 
                  ( currentItem != KDestItemDefaultConnection ) &&
                  ( currentItem != KDestItemUncategorized ))
            {
            CCmDestinationImpl* destImpl = iCmManagerImpl.DestinationL( currentItem );
            
            if ( !destImpl->ConnectionMethodCount() )
                {
                aMenuPane->SetItemDimmed( ECmManagerUiCmdCMSelect, ETrue );                
                }
                
            delete destImpl;
            }
        else
            {
            RArray<TUint32> uncatArray( KCmArraySmallGranularity );
            CleanupClosePushL( uncatArray );
            iCmManagerImpl.ConnectionMethodL( uncatArray );
            
            // Uncategorised - cannot be selected
            if ( currentItem == KDestItemUncategorized )
                {
                aMenuPane->SetItemDimmed( ECmManagerUiCmdDestSelect, ETrue );
                }
            CleanupStack::PopAndDestroy( &uncatArray );
            }
        }
    }


// ---------------------------------------------------------------------------
// CCMConnSelectRBPage::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CCMConnSelectRBPage::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                               TEventCode aType )    
    {
    LOGGER_ENTERFN("CCMConnSelectRBPage::OfferKeyEventL");
    
    TKeyResponse retVal ( EKeyWasNotConsumed );
    // save for the future use
    iPrevItem = iDestinations[ListBoxControl()->CurrentItemIndex()];
    
    TKeyEvent aKeyEventmy = aKeyEvent;
    if ( aKeyEventmy.iCode == EKeyEnter )
        {
        if ( iFirstEnter )
            {//pressing the enter key at the first time will be changed
             //to an up arrow key so the first item in the list will be highlighted
            iFirstEnter = EFalse;
            aKeyEventmy.iScanCode = EStdKeyUpArrow;
            aKeyEventmy.iCode = EKeyUpArrow;
            }
        else
            {//change back to true for the next session
            iFirstEnter=ETrue;
            }
        }
    else if ( aKeyEventmy.iScanCode == EStdKeyDownArrow || aKeyEventmy.iScanCode == EStdKeyUpArrow )
             {
             //we will have highligt so the following enter key should select the item
             iFirstEnter = EFalse;
             }
    
    switch ( aKeyEventmy.iScanCode )
        {
        case EStdKeyDownArrow:
        case EStdKeyUpArrow:
            {
            if ( iOpenDestination ) //only if we are on 'destination level' and we are able to
                //open destinations
                {            
                // If focus is on the 'uncategorised' destination, 
                // change the soft key to 'Open'
                if ( iDestinations[ ListBoxControl()->CurrentItemIndex() ] == 
                                                        KDestItemUncategorized )
                    {
                    HBufC* text = StringLoader::LoadLC( R_QTN_MSK_OPEN );
                    Cba()->SetCommandL( EAknSoftkeySelect, *text );
                    Cba()->DrawNow();
                    CleanupStack::PopAndDestroy( text );                
                    }
                else
                    {
                    HBufC* text = StringLoader::LoadLC( R_QTN_MSK_SELECT );
                    Cba()->SetCommandL( EAknSoftkeySelect, *text );
                    Cba()->DrawNow();
                    CleanupStack::PopAndDestroy( text );
                    }
                }
            break;
            }
        default:
            {
            break;
            }
        }
    retVal = CAknRadioButtonSettingPage::OfferKeyEventL( aKeyEventmy, aType );
    
    return retVal;
    }

// --------------------------------------------------------------------------
// CCMConnSelectRBPage::GetHelpContext
// --------------------------------------------------------------------------
//
void CCMConnSelectRBPage::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    LOGGER_ENTERFN("CCMConnSelectRBPage::GetHelpContext");
        
    aContext.iMajor = KHelpUidPlugin;
    aContext.iContext = iHelpContext; 
    }

// ---------------------------------------------------------------------------
// CCMConnSelectRBPage::HandleListBoxEventL
// ---------------------------------------------------------------------------
//
void CCMConnSelectRBPage::HandleListBoxEventL(CEikListBox* aListBox, 
		MEikListBoxObserver::TListBoxEvent aEventType)
	{
    LOGGER_ENTERFN("CCMConnSelectRBPage::HandleListBoxEventL");
        
    if ( AknLayoutUtils::PenEnabled() )
        {

		if (iDialogResourceId == R_RADIO_BUTTON_SETTING_PAGE_OK_CANCEL)
			{
			// Call base class implementation and return if no options menu
			CAknRadioButtonSettingPage::HandleListBoxEventL(aListBox, aEventType);
		
	    	switch ( aEventType )
    	    	{
        		case EEventEnterKeyPressed:
        		case EEventItemDoubleClicked:
        		case EEventItemSingleClicked:
				case EEventItemClicked:        	
            		{
            		ProcessCommandL(ECmManagerUiCmdDestSelect);
            		break;
            		}

        		default:
	            	{
    	        	break;
            		}
        		}
			return;
			}
		
        switch ( aEventType )
            {
            // All these actions are activating the listbox item
            case MEikListBoxObserver::EEventEnterKeyPressed:
            case MEikListBoxObserver::EEventItemActioned:
                CAknRadioButtonSettingPage::HandleListBoxEventL(aListBox, aEventType);
                break;

            case MEikListBoxObserver::EEventItemClicked:
            case MEikListBoxObserver::EEventItemSingleClicked:
                if (iPrevItem == iDestinations[ListBoxControl()->CurrentItemIndex()])
                    {
                    ProcessCommandL(ECmManagerUiCmdDestSelect);
                    }
                else
                    {
                    // save for the later use
                    iPrevItem = iDestinations[ListBoxControl()->CurrentItemIndex()];
					    
		            if ( iOpenDestination ) //only if we are on 'destination level' and we are able to
        		        //open destinations
                	    {            
                	    // If focus is on the 'uncategorised' destination, 
                	    // change the soft key to 'Open'
                	    if ( iDestinations[ ListBoxControl()->CurrentItemIndex() ] == 
		                                                        KDestItemUncategorized )
        		            {
                	        HBufC* text = StringLoader::LoadLC( R_QTN_MSK_OPEN );
                   		    Cba()->SetCommandL( EAknSoftkeySelect, *text );
                   		    Cba()->DrawNow();
                   		    CleanupStack::PopAndDestroy( text );                
                   		    }
                	    else
                   		    {
                   		    HBufC* text = StringLoader::LoadLC( R_QTN_MSK_SELECT );
                   		    Cba()->SetCommandL( EAknSoftkeySelect, *text );
                   		    Cba()->DrawNow();
                   		    CleanupStack::PopAndDestroy( text );
                   		    }
                         CAknRadioButtonSettingPage::SelectCurrentItemL();
                	     }
                    }
                break;
                
            case MEikListBoxObserver::EEventItemDoubleClicked:
                ProcessCommandL(ECmManagerUiCmdDestSelect);
                break;

            default:
                CAknRadioButtonSettingPage::HandleListBoxEventL(aListBox, aEventType);            
                break;
            }
        }
    else
        {
        CAknRadioButtonSettingPage::HandleListBoxEventL(aListBox, aEventType);
		}		
	}


