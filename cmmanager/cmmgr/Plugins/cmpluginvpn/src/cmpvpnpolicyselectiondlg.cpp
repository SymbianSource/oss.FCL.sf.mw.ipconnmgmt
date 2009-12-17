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
* Description:  Displays a radio button selection list of VPN policies
*
*/


// includes
#include <aknPopup.h>
// #include <vpnapidefs.h>
#include <aknlists.h>
#include <cmmanager.rsg>
#include <cmpluginvpnui.rsg>
#include <cmcommonui.h>
#include <cmpluginbaseeng.h>
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>
#include <aknradiobuttonsettingpage.h>

#include "cmlogger.h"
#include "cmpvpnpolicyselectiondlg.h"
#include "cmvpncommonconstants.h"
#include <cmpluginvpndef.h>
#include <cmcommonconstants.h>

using namespace CMManager;

// ----------------------------------------------------------------------------
// CmPluginVpnPolicySelectionDlg::NewL
// ----------------------------------------------------------------------------
//
CmPluginVpnPolicySelectionDlg* CmPluginVpnPolicySelectionDlg::NewL( 
                                        CCmPluginBaseEng& aCmPluginBaseEng )
    {
    CmPluginVpnPolicySelectionDlg* self = 
            new ( ELeave ) CmPluginVpnPolicySelectionDlg( aCmPluginBaseEng );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CmPluginVpnPolicySelectionDlg::~CmPluginVpnPolicySelectionDlg
// ----------------------------------------------------------------------------
//
CmPluginVpnPolicySelectionDlg::~CmPluginVpnPolicySelectionDlg()
    {

    iVpnServ.Close();

    delete iPolicyInfoList;

    iResourceReader.Close();
    }
    
// ----------------------------------------------------------------------------
// CmPluginVpnPolicySelectionDlg::CmPluginVpnPolicySelectionDlg
// ----------------------------------------------------------------------------
//
CmPluginVpnPolicySelectionDlg::CmPluginVpnPolicySelectionDlg(
                        CCmPluginBaseEng& aCmPluginBaseEng )
    : iCmPluginBaseEng( aCmPluginBaseEng )
    , iResourceReader( *CCoeEnv::Static() )
    , iVpnClientUnaccessible ( EFalse )
    {
    }
    
// ----------------------------------------------------------------------------
// CmPluginVpnPolicySelectionDlg::ConstructL
// ----------------------------------------------------------------------------
//
void CmPluginVpnPolicySelectionDlg::ConstructL()
    {
    LoadResourceL( KPluginVPNResDirAndFileName );
    
    // Get the policies
    iPolicyInfoList = 
        new (ELeave) CArrayFixFlat<TVpnPolicyInfo>( KCmArrayMediumGranularity );
    
    TInt err( iVpnServ.Connect() );
    
    // Vpn client is inaccessible
    if ( err )
        {
        VpnClientInaccessibleL();
        }
    else
        {
        iVpnServ.GetPolicyInfoList( iPolicyInfoList );        
        iVpnServ.EnumeratePolicies( iNumPolicies );
        }
    }

// ----------------------------------------------------------------------------
// CmPluginVpnPolicySelectionDlg::LoadResourceL
// ----------------------------------------------------------------------------
//
void CmPluginVpnPolicySelectionDlg::LoadResourceL( const TDesC& aResFileName )
    {
    TInt err( KErrNone );
    
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

// ----------------------------------------------------------------------------
// CmPluginVpnPolicySelectionDlg::ShowPolicySelectionListL
// ----------------------------------------------------------------------------
//
TBool CmPluginVpnPolicySelectionDlg::ShowPolicySelectionListL()
    {
    LOGGER_ENTERFN( "CmPluginVpnPolicySelectionDlg::ShowPolicySelectionListL" );
    TBool retVal ( EFalse );
    
    CEikTextListBox* list = new ( ELeave ) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL( list );
             
    // Create popup list
    CAknPopupList* popupList = CAknPopupList::NewL(
                                       list, 
                                       R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
                                       AknPopupLayouts::EMenuWindow);
    CleanupStack::PushL( popupList );

    // initialise listbox.
    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
    list->CreateScrollBarFrameL( ETrue );
    list->ScrollBarFrame()->SetScrollBarVisibilityL(
                               CEikScrollBarFrame::EOff,
                               CEikScrollBarFrame::EAuto);


    MDesCArray* itemList = list->Model()->ItemTextArray();
	CDesCArray* items = ( CDesCArray* ) itemList;	
	
    // collect the item texts    
    if ( iNumPolicies )
        {
        for ( TInt i = 0; i < iNumPolicies; i++ )
            {
            HBufC* polName = ( iPolicyInfoList->At(i) ).iName.AllocLC();
            items->AppendL( *polName );
            CleanupStack::PopAndDestroy( polName );
            }
        
        // title
        HBufC* title = StringLoader::LoadLC( R_QTN_NETW_CONSET_PRMPT_VPN_POLICY );
        popupList->SetTitleL( *title );
        CleanupStack::PopAndDestroy( title );
        
        // dislay dialog
        CleanupStack::Pop( popupList );
        if ( popupList->ExecuteLD() )
            {
            TInt ix = list->CurrentItemIndex();
            
            // Set VPN Policy to be used
            HBufC* polName = ( iPolicyInfoList->At(ix) ).iName.AllocLC();
            HBufC* polId = ( iPolicyInfoList->At(ix) ).iId.AllocLC();
            
            CLOG_WRITE_2( "New policy settings:[%S], [%S]", polName, polId );

            iCmPluginBaseEng.SetStringAttributeL( EVpnServicePolicyName, *polName );
            iCmPluginBaseEng.SetStringAttributeL( EVpnServicePolicy, *polId );
            CleanupStack::PopAndDestroy( polId );
            CleanupStack::PopAndDestroy( polName );
            
            retVal = ETrue;
            }
        }
    else
        {
        if (!iVpnClientUnaccessible)
        	{
        	// display error note
        	HBufC* text = StringLoader::LoadLC( R_QTN_VPN_INFO_NO_POLICIES_INSTALLED );
        	TCmCommonUi::ShowNoteL( *text, TCmCommonUi::ECmInfoNote );
        	CleanupStack::PopAndDestroy( text );
        	}
        CleanupStack::PopAndDestroy( popupList );
        }


	CleanupStack::PopAndDestroy( list );
    return retVal;
    }


// ----------------------------------------------------------------------------
// CmPluginVpnPolicySelectionDlg::ShowPolicySelectionDlgL
// ----------------------------------------------------------------------------
//
TBool CmPluginVpnPolicySelectionDlg::ShowPolicySelectionDlgL()
    {
    LOGGER_ENTERFN( "CmPluginVpnPolicySelectionDlg::ShowPolicySelectionDlgL" );
    TBool retVal ( EFalse );
    
    TInt selection(0);
    
    // Compile the list items
    CDesCArrayFlat* items = 
            new ( ELeave ) CDesCArrayFlat( KCmArraySmallGranularity );
    CleanupStack::PushL( items );
    
    TInt vpnerr;
    vpnerr = iVpnServ.EnumeratePolicies( iNumPolicies );
    
    if ( vpnerr )
        {
        VpnClientInaccessibleL();
        }
    else
        {        
        if ( iNumPolicies )
            {
            vpnerr = iVpnServ.GetPolicyInfoList( iPolicyInfoList );
            if ( vpnerr == 0 )
                {
                for ( TInt i = 0; i < iNumPolicies; i++ )
                    {
                    HBufC* policyName = 
                            ( iPolicyInfoList->At(i) ).iName.AllocLC();
                    
                            
                    // indicate if this policy is the one in the settings
                    HBufC* val = iCmPluginBaseEng.GetStringAttributeL( 
                                                        EVpnServicePolicyName );
                    
                    if ( !( val->Compare( policyName->Des() ) ) )
                        {
                        selection = i;
                        }
                    delete val;
                    
                    items->AppendL( *policyName );
                    CleanupStack::PopAndDestroy( policyName );
                    }
                // Create the dialog
                CAknRadioButtonSettingPage* dlg = 
                                    new ( ELeave ) CAknRadioButtonSettingPage(
                                            R_RADIO_BUTTON_SETTING_PAGE_OK_CANCEL,
                                            selection, 
                                            items );
                CleanupStack::PushL( dlg );
                
                // set title
                HBufC* title = StringLoader::LoadLC( 
                                        R_QTN_VPN_SETT_VPN_IAP_POLICY_REFERENCE );
                dlg->SetSettingTextL( *title );
                CleanupStack::PopAndDestroy( title );

                // display dialog
                CleanupStack::Pop();
                if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
                    {
                    HBufC* polId = 
                            ( iPolicyInfoList->At( selection ) ).iId.AllocLC();
                    HBufC* polName = 
                            ( iPolicyInfoList->At( selection ) ).iName.AllocLC();
                    iCmPluginBaseEng.SetStringAttributeL( EVpnServicePolicyName,
                                                          *polName );
                    
                    CLOG_WRITE_2( "New policy settings:[%S], [%S]", polName, polId );
                    
                    iCmPluginBaseEng.SetStringAttributeL( EVpnServicePolicy, 
                                                          *polId );
                    CleanupStack::PopAndDestroy( polName );
                    CleanupStack::PopAndDestroy( polId );
                    retVal = ETrue;
                    }
                }
            }
        else
            {
            // note: no VPN policies installed, reset policy to compulsory
            CLOG_WRITE("No policy installed, reset policies");
            
            HBufC* text = StringLoader::LoadLC( 
                                        R_QTN_VPN_INFO_NO_POLICIES_INSTALLED );
            TCmCommonUi::ShowNoteL( *text, TCmCommonUi::ECmInfoNote );
            CleanupStack::PopAndDestroy( text );
            vpnerr = -1;
            }
        }

    if ( vpnerr )
        {
        // note: no VPN policies installed or any other vpn error,
        //  reset policy to compulsory
        CLOG_WRITE("Reset policies");
        
        iCmPluginBaseEng.SetStringAttributeL( EVpnServicePolicyName,
                                              KNullDesC() );
        iCmPluginBaseEng.SetStringAttributeL( EVpnServicePolicy, 
                                              KNullDesC() );
        
        }

    CleanupStack::PopAndDestroy( items );
    return retVal;
    }
    
    
// ----------------------------------------------------------------------------
// CmPluginVpnPolicySelectionDlg::VpnClientInaccessibleL
// ----------------------------------------------------------------------------
//
void CmPluginVpnPolicySelectionDlg::VpnClientInaccessibleL()
    {
    LOGGER_ENTERFN( "CmPluginVpnPolicySelectionDlg::VpnClientInaccessibleL" );
    HBufC* text = StringLoader::LoadLC( R_QTN_VPN_INFO_MGMT_UI_NOT_ACCESSIBLE );
    TCmCommonUi::ShowNoteL( *text, TCmCommonUi::ECmInfoNote );
    CleanupStack::PopAndDestroy( text );
    iVpnClientUnaccessible = ETrue;
    }
