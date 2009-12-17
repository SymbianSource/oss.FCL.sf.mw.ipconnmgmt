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
* Description:  Popup for selecting the underlying connection method or 
*                destination for a VPN connection method
*
*/


// includes
#include <badesca.h> // arrays
#include <gulicon.h> // icons
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>
#include <featmgr.h>
#include <cmpluginvpn.mbg>
#include <cmpluginvpnui.rsg>
#include <cmmanager.rsg>
#include <cmmanager.mbg> 
#include <eikenv.h>
#include <aknlists.h>
#include <eikfrlb.h> // CEikFormattedCellListBox
#include <aknradiobuttonsettingpage.h>
#include <cmpluginbaseeng.h>
#include <cmpluginbase.h>

#include <hlplch.h>
#include <csxhelp/cp.hlp.hrh>

#include "cmlogger.h"
#include "cmdestinationimpl.h"
#include "cmmanagerimpl.h"
#include "cmpvpnnextlayerselectdlg.h"
#include "cmvpncommonconstants.h"
#include <cmcommonconstants.h>
#include <cmpluginvpndef.h>

using namespace CMManager;
// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::NewL
// --------------------------------------------------------------------------
//
CmPluginVpnNextLayerSelectDlg* CmPluginVpnNextLayerSelectDlg::NewL( 
                                        CCmPluginBaseEng& aCmPluginBaseEng,
                                        RArray<TUint32>& aBindableMethods,
                                        TBool& aSnapSelected,
                                        TUint32& aNextLayerId )
    {
    CmPluginVpnNextLayerSelectDlg* self = 
            new ( ELeave ) CmPluginVpnNextLayerSelectDlg( aCmPluginBaseEng,
                                                          aBindableMethods,
                                                          aSnapSelected,
                                                          aNextLayerId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
      
// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::CmPluginVpnNextLayerSelectDlg
// --------------------------------------------------------------------------
//
CmPluginVpnNextLayerSelectDlg::CmPluginVpnNextLayerSelectDlg(
                                        CCmPluginBaseEng& aCmPluginBaseEng,
                                        RArray<TUint32>& aBindableMethods,
                                        TBool& aSnapSelected,
                                        TUint32& aNextLayerId )
    : iMyEikonEnv( *CEikonEnv::Static() )
    , iCmPluginBaseEng( aCmPluginBaseEng )
    , iSnapSelected( aSnapSelected )
    , iNextLayerId( aNextLayerId )
    , iResourceReader( *CCoeEnv::Static() )
    , iDestinations( aBindableMethods )
    , iEasyWlanId(0)  
    , iCmManager( iCmPluginBaseEng.CmMgr() ) 
    , iUncatItems( EFalse )
    {
    ;
    }
    
    
// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::~CmPluginVpnPolicySelectionDlg
// --------------------------------------------------------------------------
//
CmPluginVpnNextLayerSelectDlg::~CmPluginVpnNextLayerSelectDlg()
    {
    iResourceReader.Close();
    
    if ( iMenuBar )
        {
        HideMenu();
        CEikonEnv::Static()->EikAppUi()->RemoveFromStack( iMenuBar );
        delete iMenuBar;
        iMenuBar = NULL;
        }
        
    delete iMyListBox; iMyListBox = NULL;
    }

// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::ConstructL
// --------------------------------------------------------------------------
//
void CmPluginVpnNextLayerSelectDlg::ConstructL()
    {
    LoadResourceL( KPluginVPNResDirAndFileName );
    InitialiseL();
    SetTextsAndIconsL();
    }
    
// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::InitialiseL
// --------------------------------------------------------------------------
// 
void CmPluginVpnNextLayerSelectDlg::InitialiseL()
    {
    LOGGER_ENTERFN( "CmPluginVpnNextLayerSelectDlg::InitialiseL" );
    
    // get the EasyWlan id if it's supported
    if ( FeatureManager::FeatureSupported( KFeatureIdProtocolWlan ) )
        {
        iEasyWlanId = iCmManager.EasyWlanIdL();
        }
    
    // Listbox
    iMyListBox = new ( ELeave ) CAknDoubleLargeGraphicPopupMenuStyleListBox;
    
    CAknPopupList::ConstructL( 
                            iMyListBox, 
                            R_SOFTKEYS_OPTIONS_CANCEL__SELECT, 
                            AknPopupLayouts::EMenuDoubleLargeGraphicWindow );
                               
    TInt flags = 0;
    iMyListBox->ConstructL( this, flags );
    iMyListBox->CreateScrollBarFrameL( ETrue );
    iMyListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
                    CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
        
    // Menu
    CEikMenuBar* newMenuBar = new ( ELeave ) CEikMenuBar();
    CleanupStack::PushL( newMenuBar );
    newMenuBar->ConstructL( this, NULL, R_VPN_REAL_CM_SELECTION_MENUBAR );
    iMyEikonEnv.EikAppUi()->AddToStackL( newMenuBar, 
                                         ECoeStackPriorityMenu, 
                                         ECoeStackFlagRefusesFocus );
    iMenuBar = newMenuBar;
    CleanupStack::Pop( newMenuBar ); // ownership taken by 'this'
    }
    
// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::SetTextsAndIconsL
// --------------------------------------------------------------------------
//
void CmPluginVpnNextLayerSelectDlg::SetTextsAndIconsL()
    {
    LOGGER_ENTERFN( "CmPluginVpnNextLayerSelectDlg::SetTextsAndIconsL" );
    // Title
    SetTitleL( *StringLoader::LoadLC( 
                            R_QTN_NETW_CONSET_PRMPT_VPN_REAL_CONNECTION ) );
    CleanupStack::PopAndDestroy(); // header
    
    // Item text array
    CDesCArray* itemArray =
                new ( ELeave ) CDesCArrayFlat( KCmArrayMediumGranularity );
    CleanupStack::PushL( itemArray );
    iMyListBox->Model()->SetItemTextArray( itemArray ); // ownership taken
    iMyListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( itemArray ); // as ownership is taken
                    
    // Icon array
    CArrayPtr<CGulIcon>* icons =
         new ( ELeave ) CArrayPtrFlat<CGulIcon>( KCmArrayMediumGranularity );
    CleanupStack::PushL( icons );
    iMyListBox->ItemDrawer()->FormattedCellData()->SetIconArrayL( icons );
    // ownership taken by SetIconArrayL!
    CleanupStack::Pop( icons ); // as ownership is taken
    
    // Step through the Destinations - do not show:
    // Parent Destinations or Destinations with no non-virtual CMs
    // Show Uncategorised if valid and EasyWlan if supported
    TInt destCount = iDestinations.Count();
    for ( TInt i = 0; i < destCount; i++ )
        {        
        switch ( iDestinations[i] )
            {
            case KDestItemUncategorized:
                {
                // Append 'Uncategorised' if necessary
                AppendUncatDestinationL( *itemArray, *icons );
                break;
                }
            case KDestItemEasyWlan:
                {
                // Append 'EasyWlan' if supported
                TRAPD( err, AppendEasyWlanL( *itemArray, *icons ) );
                if ( err != KErrNotSupported )
                    {
                    User::LeaveIfError( err );
                    }
                break;
                }
            // normal destination
            default:
                {
                CCmDestinationImpl* dest = 
                iCmManager.DestinationL( iDestinations[i] );
                CleanupStack::PushL( dest );
                TInt numCms = dest->ConnectionMethodCount();        
                AppendDestinationTextsL( *dest,numCms, *itemArray, *icons );                
                CleanupStack::PopAndDestroy( dest );
                break;
                }                    
            }
        }
    }


// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::AppendDestinationsTextsL
// --------------------------------------------------------------------------
//
void CmPluginVpnNextLayerSelectDlg::AppendDestinationTextsL(
                                            CCmDestinationImpl& aDestination,
                                            TInt aNumCms,
                                            CDesCArray& aItems,
                                            CArrayPtr<CGulIcon>& aIcons )
    {
    LOGGER_ENTERFN( "CmPluginVpnNextLayerSelectDlg::AppendDestinationL" );
    // icon
    CGulIcon* icon = ( CGulIcon* ) aDestination.IconL();
    CleanupStack::PushL( icon );
    aIcons.AppendL( icon );
    TInt iconIndex = aIcons.Count() - 1;
    CleanupStack::Pop( icon ); // icon array took over ownership
    
    // compile the list item
    HBufC* firstLine = aDestination.NameLC();
    HBufC* secondLine;
    
    if ( aNumCms == 1 )
        {
        secondLine = StringLoader::LoadLC(
                        R_CMMANAGERUI_CONN_METHODS_AMOUNT_ONE );
        }
    else
        {
        secondLine = StringLoader::LoadLC( 
                        R_CMMANAGERUI_CONN_METHODS_AMOUNT_MANY,
                        aNumCms );
        }
        
    HBufC* item = FormatListItemTextsLC( *firstLine, 
                                         *secondLine,
                                         iconIndex );
    aItems.AppendL( *item );

    CleanupStack::PopAndDestroy( item );
    CleanupStack::PopAndDestroy( secondLine );
    CleanupStack::PopAndDestroy( firstLine );
    }

// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::AppendUncatDestinationL
// --------------------------------------------------------------------------
//
void CmPluginVpnNextLayerSelectDlg::AppendUncatDestinationL( 
                                                CDesCArray& aItems,
                                                CArrayPtr<CGulIcon>& aIcons )
    {
    LOGGER_ENTERFN( "CmPluginVpnNextLayerSelectDlg::AppendUncatDestinationL" );

    TParse mbmFile;
    User::LeaveIfError( mbmFile.Set( KManagerIconFilename, 
                        &KDC_APP_BITMAP_DIR, 
                        NULL ) );
    
    // icon
    CGulIcon* icon = AknsUtils::CreateGulIconL(
                AknsUtils::SkinInstance(), 
                KAknsIIDQgnPropSetConnDestUncategorized,
                mbmFile.FullName(), 
                EMbmCmmanagerQgn_prop_set_conn_dest_uncategorized, 
                EMbmCmmanagerQgn_prop_set_conn_dest_uncategorized_mask );
    CleanupStack::PushL( icon );
    aIcons.AppendL( icon );
    CleanupStack::Pop( icon );  // icon array took over ownership
    TInt iconIndex = aIcons.Count() - 1;
    
    // remove virtual CMs
    RArray<TUint32> uncatCms ( KCmArraySmallGranularity );    
    iCmManager.ConnectionMethodL( uncatCms );
    CleanupClosePushL( uncatCms );
    
    for ( TInt i = 0; i < uncatCms.Count(); i++ )
        {
        TInt bearerType = iCmManager.GetConnectionMethodInfoIntL( 
                                        uncatCms[i], ECmBearerType );
        
        if ( iCmManager.GetBearerInfoBoolL( bearerType, ECmVirtual ) )
            {
            uncatCms.Remove(i);
            i--;
            }        
        }
    
    // second line text
    HBufC* secondLine;            
    if ( uncatCms.Count() == 1 )
        {
        secondLine = StringLoader::LoadLC(
                            R_CMMANAGERUI_CONN_METHODS_AMOUNT_ONE );
        }
    else
        {
        secondLine = StringLoader::LoadLC( 
                            R_CMMANAGERUI_CONN_METHODS_AMOUNT_MANY,
                            uncatCms.Count() );
        }    

    // compile the texts
    HBufC* item = FormatListItemTextsLC( R_CMMANAGERUI_DEST_UNCATEGORIZED,
                                         *secondLine,
                                         iconIndex );            
    aItems.AppendL( *item );

    CleanupStack::PopAndDestroy( item );
    CleanupStack::PopAndDestroy( secondLine );
    CleanupStack::PopAndDestroy( &uncatCms );
    }
                                                 
// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::AppendEasyWlanL
// --------------------------------------------------------------------------
//
void CmPluginVpnNextLayerSelectDlg::AppendEasyWlanL( 
                                                CDesCArray& aItems,
                                                CArrayPtr<CGulIcon>& aIcons )
    {
    LOGGER_ENTERFN( "CmPluginVpnNextLayerSelectDlg::AppendEasyWlanL" );

    // icon
    CCmPluginBase* cm = iCmManager.GetConnectionMethodL( iEasyWlanId );
    CleanupStack::PushL( cm );
    CGulIcon* icon = ( CGulIcon* ) cm->GetIntAttributeL( ECmBearerIcon );
    CleanupStack::PushL( icon );
    aIcons.AppendL( icon );  // ownership passed to array
    TInt iconIndex = aIcons.Count() - 1;
    CleanupStack::Pop( icon );
    CleanupStack::PopAndDestroy( cm );
    
    // compile the texts
    HBufC* easyWlan = FormatListItemTextsLC( 
                                R_QTN_NETW_CONSET_VPN_EASY_WLAN,
                                R_QTN_NETW_CONSET_VPN_EASY_WLAN_PROMPT,
                                iconIndex );
    aItems.AppendL( *easyWlan );
    
    CleanupStack::PopAndDestroy( easyWlan );
    }
    
// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CmPluginVpnNextLayerSelectDlg::DynInitMenuPaneL( 
                                TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if ( aResourceId == R_VPN_REAL_CM_SELECTION_MENU )
        {
    	if (!iCmManager.IsHelpOn())
            {
            aMenuPane->DeleteMenuItem( EAknCmdHelp );		    
            }
        TUint32 highlighted = iDestinations[ iListBox->CurrentItemIndex() ];
        
        switch ( highlighted )
            {
            case KDestItemUncategorized:
                {
                aMenuPane->SetItemDimmed( ECmManagerUiCmdDestSelect, ETrue );
                break;                
                }
            case KDestItemEasyWlan:
                {
                aMenuPane->SetItemDimmed( ECmManagerUiCmdCMSelect, ETrue );
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::ProcessCommandL
// --------------------------------------------------------------------------
//
void CmPluginVpnNextLayerSelectDlg::ProcessCommandL( TInt aCommandId )
    {
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
        case ECmManagerUiCmdDestSelect:
            {
            TUint32 destId = iDestinations[ iListBox->CurrentItemIndex() ];
            
            if ( destId != KDestItemUncategorized )
                {
                TUint32 attribute = ECmNextLayerSNAPId;
                
                // EasyWlan was selected
                if ( destId == KDestItemEasyWlan )
                    {
                    attribute = ECmNextLayerIapId;
                    destId = iEasyWlanId;
                    }
                iCmPluginBaseEng.SetIntAttributeL( attribute, destId );
                AttemptExitL( ETrue );
                break;
                }
            // For uncategorised destinations, flow through to ECmManagerUiCmdCMSelect
            }
        case ECmManagerUiCmdCMSelect:
            {
            // select a connection method from the highlighted destination
            TUint32 destId = iDestinations[ iListBox->CurrentItemIndex() ];
            TUint32 cmId (0);
            if ( ShowCMSelectionDlgL( destId, cmId ) )
                {
                iCmPluginBaseEng.SetIntAttributeL( ECmNextLayerIapId, cmId );
                AttemptExitL( ETrue );
                }
            else
                {
                CancelPopup();
                }
            break;
            }
        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                                    iEikonEnv->EikAppUi()->AppHelpContextL() );
            }
            break;
        case EAknSoftkeyBack:
            {
            CancelPopup();
            break;
            } 
        default:
            {
            CAknPopupList::ProcessCommandL( aCommandId );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CmPluginVpnNextLayerSelectDlg::OfferKeyEventL( 
                                                const TKeyEvent& aKeyEvent, 
                                                TEventCode aType )    
    {
    TKeyResponse retVal ( EKeyWasNotConsumed );
    TUint highlighted = iDestinations[ iListBox->CurrentItemIndex() ];
    
    switch ( aKeyEvent.iScanCode )
        {
        case EStdKeyDownArrow:
        case EStdKeyUpArrow:
            {
            // If focus is on the 'uncategorised' destination, 
            // change the soft key to 'Open'
            if ( highlighted == KDestItemUncategorized )
                {
                ButtonGroupContainer()->SetCommandL( 
                                ECmManagerUiCmdDestSelect, 
                                *( StringLoader::LoadLC( R_QTN_MSK_OPEN ) ) 
                                                    );
                ButtonGroupContainer()->DrawDeferred();
                CleanupStack::PopAndDestroy();
                }
            else
                {
                ButtonGroupContainer()->SetCommandL( 
                               ECmManagerUiCmdDestSelect, 
                               *( StringLoader::LoadLC( R_QTN_MSK_SELECT ) ) 
                                                    );
                ButtonGroupContainer()->DrawDeferred();
                CleanupStack::PopAndDestroy();
                }
            break;
            }
        case EStdKeyNo:
            {
            CancelPopup();
            return EKeyWasConsumed;
            }
        default:
            {
            // If focus is on the 'uncategorised' destination, 
            // change the soft key to 'Open'
            if ( highlighted == KDestItemUncategorized )
                {
                ButtonGroupContainer()->SetCommandL( 
                                ECmManagerUiCmdDestSelect, 
                                *( StringLoader::LoadLC( R_QTN_MSK_OPEN ) ) 
                                                    );
                ButtonGroupContainer()->DrawDeferred();
                CleanupStack::PopAndDestroy();
                }
                
            if ( aKeyEvent.iCode == EKeyEscape )
                {
                CancelPopup();
                return EKeyWasConsumed;
                }
            }
        }
    retVal = iListBox->OfferKeyEventL( aKeyEvent, aType );
    
    return retVal;
    }
    
// ---------------------------------------------------------------------------
// CDestDlg::HandleListBoxEventL
// ---------------------------------------------------------------------------
void CmPluginVpnNextLayerSelectDlg::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                    TListBoxEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemDoubleClicked:
            {
            ProcessCommandL(ECmManagerUiCmdDestSelect);
            break;
            }
        default:
            {
            break;
            };
        };
    }

// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::SetEmphasis
// --------------------------------------------------------------------------
//
void CmPluginVpnNextLayerSelectDlg::SetEmphasis( CCoeControl* aMenuControl, 
                                                 TBool aEmphasis )
    {
	CEikAppUi* appUi = iMyEikonEnv.EikAppUi();
	appUi->RemoveFromStack( aMenuControl );
	
	TRAP_IGNORE( appUi->AddToStackL ( 
	                            aMenuControl, 
                                aEmphasis ? ECoeStackPriorityDialog : 
                                ECoeStackPriorityMenu ) );
                                
	appUi->UpdateStackedControlFlags( 
	                            aMenuControl, 
                                aEmphasis ? 0 : ECoeStackFlagRefusesFocus,
                                ECoeStackFlagRefusesFocus );
	appUi->HandleStackChanged();
    }
    
// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::LoadResourceL
// --------------------------------------------------------------------------
//
void CmPluginVpnNextLayerSelectDlg::LoadResourceL( 
                                                const TDesC& aResFileName )
    {
    LOGGER_ENTERFN( "CmPluginVpnNextLayerSelectDlg::LoadResourceL" );
    TInt err ( KErrNone );
    
    // Add resource file.
    TParse* fp = new ( ELeave ) TParse();
    err = fp->Set( aResFileName, 
                        &KDC_RESOURCE_FILES_DIR, 
                        NULL ); 
    if ( err )
        {
        User::Leave( err );
        }   

    TFileName resourceFileNameBuf = fp->FullName();
    delete fp;

    iResourceReader.OpenL( resourceFileNameBuf );
    }


// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::FormatListItemTextsLC
// --------------------------------------------------------------------------
//
HBufC* CmPluginVpnNextLayerSelectDlg::FormatListItemTextsLC( 
                                                TInt aFirstLineResId,
                                                TInt aSecondLineResId,
                                                TInt aIconIndex )
    {
    HBufC* firstLine = StringLoader::LoadLC( aFirstLineResId );
    HBufC* secondLine = StringLoader::LoadLC( aSecondLineResId );
    
    HBufC* retVal = FormatListItemTextsL( *firstLine,
                                          *secondLine,
                                          aIconIndex );
    CleanupStack::PopAndDestroy( secondLine );
    CleanupStack::PopAndDestroy( firstLine );
    CleanupStack::PushL( retVal );
    return retVal;
    }
    
// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::FormatListItemTextsLC
// --------------------------------------------------------------------------
//
HBufC* CmPluginVpnNextLayerSelectDlg::FormatListItemTextsLC( 
                                                const TDesC& aFirstLine,
                                                const TDesC& aSecondLine,
                                                TInt aIconIndex )
    {
    HBufC* retVal = FormatListItemTextsL( aFirstLine,
                                          aSecondLine,
                                          aIconIndex );
    CleanupStack::PushL( retVal );
    return retVal;
    }

// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::FormatListItemTextsLC
// --------------------------------------------------------------------------
//
HBufC* CmPluginVpnNextLayerSelectDlg::FormatListItemTextsLC( 
                                                TInt aFirstLineResId,
                                                const TDesC& aSecondLine,
                                                TInt aIconIndex )
    {
    // Load the first string
    HBufC* firstLine = StringLoader::LoadLC( aFirstLineResId );
    
    HBufC* retVal = FormatListItemTextsL( *firstLine,
                                          aSecondLine,
                                          aIconIndex );
    CleanupStack::PopAndDestroy( firstLine );
    CleanupStack::PushL( retVal );
    return retVal;
    }

// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::FormatListItemTextsL
// --------------------------------------------------------------------------
//
HBufC* CmPluginVpnNextLayerSelectDlg::FormatListItemTextsL( 
                                                const TDesC& aFirstLine,
                                                const TDesC& aSecondLine,
                                                TInt aIconIndex )
    {
    HBufC* retVal = HBufC::NewL( aFirstLine.Length() + 
                           aSecondLine.Length() + 
                           KNumberOfStaticCharsIn2LineFormattedString );
    
    TPtr ptr ( retVal->Des() );
    ptr.AppendNum( aIconIndex );
    ptr.Append( TChar( KSettingsListBoxItemPrefix ) );
    ptr.Append( aFirstLine );
    ptr.Append( TChar( KSettingsListBoxItemPrefix ) );
    ptr.Append( aSecondLine );
    
    return retVal;
    }

// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::ShowCMSelectionDlgL
// --------------------------------------------------------------------------
//
TBool CmPluginVpnNextLayerSelectDlg::ShowCMSelectionDlgL( 
                                                    TUint32 aDestinationId,
                                                    TUint32& aCmId )
    {
    LOGGER_ENTERFN( "CmPluginVpnNextLayerSelectDlg::ShowCMSelectionDlgL" );
    TBool retVal ( EFalse );
        
    // Collect item texts
    CDesCArrayFlat* items = 
            new ( ELeave ) CDesCArrayFlat( KCmArraySmallGranularity );
    CleanupStack::PushL( items );
    
    RPointerArray<CCmPluginBase> cmDataArray ( KCmArrayMediumGranularity );
    iCmManager.CreateFlatCMListLC( aDestinationId , cmDataArray );
    CleanupStack::Pop( &cmDataArray );
    CCmManagerImpl::CleanupResetAndDestroyPushL(cmDataArray);
    iCmManager.FilterOutVirtualsL( cmDataArray );


    // after it, get the names for each element
    // and finally make up the real items array.
    
    // Get the names of the connection methods
    // virtuals are already filtered out, 
    TInt count = cmDataArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        HBufC* cmName = 
            iCmManager.GetConnectionMethodInfoStringL( cmDataArray[i]->GetIntAttributeL( ECmId ),
                                                       ECmName );
        CleanupStack::PushL( cmName );
            
        // append the name to the string
        items->AppendL( *cmName );
            
        CleanupStack::PopAndDestroy( cmName );
        }
        
    // Display the radio button page
    TInt selected ( 0 );
    TInt resId( R_VPN_REAL_CM_RADIO_BUTTON_SETTING_PAGE );
        
    CAknRadioButtonSettingPage* dlg = 
                            new ( ELeave ) CAknRadioButtonSettingPage(
                                    resId,
                                    selected, 
                                    items );
    CleanupStack::PushL( dlg );
    dlg->SetSettingTextL(
                *StringLoader::LoadLC( R_CMWIZARD_SELECT_CONN_METHOD ) );
    CleanupStack::PopAndDestroy();            
    CleanupStack::Pop( dlg );

    MakeVisible( EFalse );

    TInt result = 0;
    TRAPD( err, result = dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) );

    MakeVisible( ETrue );
    User::LeaveIfError( err );

    if ( result )

        {
        // store result
        aCmId = cmDataArray[ selected ]->GetIntAttributeL( ECmId );
        retVal = ETrue;
        }
    
    CleanupStack::PopAndDestroy( &cmDataArray );
    CleanupStack::PopAndDestroy( items );
    return retVal;
    }
    
// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::DisplayMenuL
// --------------------------------------------------------------------------
//
void CmPluginVpnNextLayerSelectDlg::DisplayMenuL()
    {
    iMenuBar->TryDisplayMenuBarL();
    }

// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::HideMenu
// --------------------------------------------------------------------------
//
void CmPluginVpnNextLayerSelectDlg::HideMenu()
    {
    iMenuBar->StopDisplayingMenuBar();
    }

// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::MenuShowing
// --------------------------------------------------------------------------
//
TBool CmPluginVpnNextLayerSelectDlg::MenuShowing() const
    {
    return iMenuBar->IsDisplayed();
    }

// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::GetHelpContext
// --------------------------------------------------------------------------
//
void CmPluginVpnNextLayerSelectDlg::GetHelpContext( 
                                            TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KHelpUidPlugin;
    aContext.iContext = KSET_HLP_VPN_DEST_ASSOC_VIEW;
    }

// --------------------------------------------------------------------------
// CmPluginVpnNextLayerSelectDlg::MakeVisible
// --------------------------------------------------------------------------
//
void CmPluginVpnNextLayerSelectDlg::MakeVisible( TBool aVisible )
    {
    CAknPopupList::MakeVisible( aVisible );

    // Necessary implementation.
    CAknPopupList::FadeBehindPopup( IsVisible() );
    }




