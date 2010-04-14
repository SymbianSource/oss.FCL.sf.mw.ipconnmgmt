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
* Description:  base class for connection method plugin dialogs
*
*/


// INCLUDE FILES

// System
#include <cmmanager.rsg>
#include <aknnavide.h>
#include <StringLoader.h>
#include <akntitle.h>
#include <aknradiobuttonsettingpage.h>
#include <data_caging_path_literals.hrh>
#include <akntextsettingpage.h>
#include <aknpasswordsettingpage.h>
#include <in_sock.h>
#include <aknmfnesettingpage.h>
#include <AknQueryDialog.h>
#include <featmgr.h>

#include <hlplch.h>

// User
#include <mcmdexec.h>
#include <cmpbasesettingsdlg.h>
#include <cmmanager.h>
#include <cmpluginbaseeng.h>
#include "cmmanagerimpl.h"
#include "cmmanager.hrh"
#include <cmcommonui.h>
#include <cmcommonconstants.h>
#include <cmpsettingsconsts.h>
#include "cmpluginmultilinedialog.h"
#include "cmlogger.h"
#include "ccmdexec.h"

using namespace CMManager;

// Maximum speed mapping
struct TIPv6DNSMapping
    {
    TInt iResId;
    TInt iSettingValue;
    };

// Mapping for IPv6 DNS types
const TIPv6DNSMapping KTIPv6DNSItems[] =
    {
    { R_QTN_SET_DNS_SERVERS_AUTOMATIC, EIPv6Unspecified },
    { R_QTN_SET_IP_WELL_KNOWN,         EIPv6WellKnown },
    { R_QTN_SET_IP_USER_DEFINED,       EIPv6UserDefined },
    { 0, 0 }
    };
    
const TUint32 KMaxInt32Length = 10;

// ================= MEMBER FUNCTIONS =======================================

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::ConstructAndRunLD
// Constructs the dialog and runs it.
// --------------------------------------------------------------------------
//
EXPORT_C TInt CmPluginBaseSettingsDlg::ConstructAndRunLD( )
    {
    CleanupStack::PushL( this );
    FeatureManager::InitializeLibL();
    iHelp = FeatureManager::FeatureSupported( KFeatureIdHelp ); 
    FeatureManager::UnInitializeLib();
    ConstructL( R_PLUGIN_BASE_MENUBAR );
    PrepareLC( R_PLUGIN_BASE_DIALOG );
    CleanupStack::Pop( this );
    iSettingIndex = new ( ELeave ) CArrayFixFlat<TUint>( 4 );

    if (!iHasSettingChanged)
	    {
        iHasSettingChanged = ETrue;
        }

    return RunLD();
    }
    
    
// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::CmPluginBaseSettingsDlg()
// --------------------------------------------------------------------------
//
EXPORT_C  CmPluginBaseSettingsDlg::CmPluginBaseSettingsDlg( 
                                          CCmPluginBaseEng& aCmPluginBase ) :
        iCmPluginBaseEng( aCmPluginBase ),
        iExitReason( KDialogUserBack ),
        iResourceReader( *CCoeEnv::Static() ),
        iIsPossibleToSaveSetting( ETrue ),
        iHasSettingChanged( EFalse )
    {
    }
   
// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::~CApSelectorDialog
// --------------------------------------------------------------------------
//
EXPORT_C CmPluginBaseSettingsDlg::~CmPluginBaseSettingsDlg()
    {
    // iModel is deleted by the ListBox because LB owns the model...
    // of course only after we passed ownership...
    if ( iNaviDecorator )
        {
        delete iNaviDecorator;
        }
    if ( iTitlePane )
        {
        // set old text back, if we have it...
        if ( iOldTitleText )
            {
            TRAP_IGNORE( iTitlePane->SetTextL( *iOldTitleText ) );
            delete iOldTitleText;
            }
        }       
    if ( iSettingIndex )
        {
        delete iSettingIndex;
        }
    iResourceReader.Close();        

    delete iCmdExec;
    iCmdExec = NULL;
    }

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::PreLayoutDynInitL();
// called by framework before dialog is shown
// --------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlg::PreLayoutDynInitL()
    {
    iListbox = STATIC_CAST( CAknSettingStyleListBox*, 
                                           Control( KPluginBaseListboxId ) );
    iListbox->CreateScrollBarFrameL( ETrue );
    iListbox->ScrollBarFrame()->SetScrollBarVisibilityL
                    ( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    iListbox->SetListBoxObserver( this );

    HandleListboxDataChangeL();
    iListbox->HandleItemAdditionL();
    InitTextsL();
    }

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::UpdateListBoxContentL
// --------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlg::UpdateListBoxContentL()
    {        
    // Must save listbox indexes because content will be deleted
    TInt lastListBoxPos = iListbox->CurrentItemIndex();
    TInt lastListBoxTopPos = iListbox->TopItemIndex();

    // deleting listbox content
    MDesCArray* itemList = iListbox->Model()->ItemTextArray();
    CDesCArray* itemArray = (CDesCArray*)itemList;
    
    while ( itemArray->Count() )
        {
        itemArray->Delete( 0 );
        }
    while ( iSettingIndex->Count() )
        {
        iSettingIndex->Delete( 0 );
        }

    // CONNECTION NAME
    AppendSettingTextsL( *itemArray, ECmName );

    // BEARER SPECIFIC SETTINGS
    UpdateListBoxContentBearerSpecificL( *itemArray );

    //-----------------------------------------------------------------------
    // Handle content changes
    iListbox->Reset();
    iListbox->HandleItemAdditionL();
    
    // Listbox might not have index
    if ( lastListBoxPos != -1 )
        {
        iListbox->SetCurrentItemIndex( lastListBoxPos );
        iListbox->SetTopItemIndex( lastListBoxTopPos );
        }
    iListbox->DrawNow();
    }

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::AppendSettingTextsL
// --------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlg::AppendSettingTextsL( 
                                                    CDesCArray& aItemArray,
                                                    const TInt aSettingItemId,
                                                    const TInt aValueResId )
    {
    HBufC* value = StringLoader::LoadLC( aValueResId );

    AppendSettingTextsL( aItemArray, aSettingItemId, value );
    
    CleanupStack::PopAndDestroy( value );
    }
    
// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::AppendSettingTextsL
// --------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlg::AppendSettingTextsL( 
                                            CDesCArray& aItemArray,
                                            const TInt aSettingItemId,
                                            const HBufC* aValueString )
    {
    const TCmAttribConvTable* convItem = 
                        iCmPluginBaseEng.ConvTableItem( aSettingItemId );
    HBufC* value = NULL;
    
    if ( aValueString )
        {
        value = (HBufC*)aValueString;
        }
    else
        {
        if ( convItem->iAttribFlags & EConvNumber )
            {
            TUint32 num = iCmPluginBaseEng.GetIntAttributeL( aSettingItemId );
            
            if( !num && convItem->iDefSettingResId )
                {
                value = StringLoader::LoadL( convItem->iDefSettingResId );
                }
            else
                {
                value = HBufC::NewL( KMaxInt32Length );
                value->Des().AppendNum( num );

                TPtr number( value->Des() );
                AknTextUtils::LanguageSpecificNumberConversion( number );
                }
            }
        else
            {
            if( convItem->iAttribFlags & EConv8Bits )
                // make 16bits copy of this 8bits buffer
                {
                HBufC8* value8 = iCmPluginBaseEng.GetString8AttributeL( aSettingItemId );
                CleanupStack::PushL( value8 );
                value = HBufC::NewL( value8->Length() );
                value->Des().Copy( *value8 );
                CleanupStack::PopAndDestroy( value8 );
                }
            else
                {   
                value = iCmPluginBaseEng.GetStringAttributeL( aSettingItemId );
                }
                
            if ( convItem->iDefSettingResId )
                // We have default value in resource.
                // Check if we have use it, instead of current value.
                {
                if ( convItem->iAttribFlags & EConvIPv4 )
                    // This is an IPv4 text attribute
                    {
                    if ( IsUnspecifiedIPv4Address( *value ) )
                        // Use default value from resource instead of
                        // displaying unspecified IP address
                        {
                        delete value; value = NULL;
                        }
                    }
                else if ( convItem->iAttribFlags & EConvIPv6 )
                    // This is an IPv4 text attribute
                    {
                    }
                else if( convItem->iAttribFlags & EConvPassword )
                    // In case of password, we can show only the resource 
                    // string.
                    {
                    delete value; value = NULL;
                    }
                else if ( *value == KNullDesC && convItem->iDefSettingResId )
                    // empty string, but it has a default value in resource
                    // Use the resource value.
                    {
                    delete value; value = NULL;
                    }
                    
                if ( !value )
                    // use default value from resource.
                    {
                    value = StringLoader::LoadL( convItem->iDefSettingResId );
                    }
                }
            }

        CleanupStack::PushL( value );
        }

    HBufC* title = StringLoader::LoadLC( convItem->iTitleId );

    _LIT( KSettingListFormat, "\t%S\t\t%S" );
    HBufC* itemTextBuf = HBufC::NewLC( KSettingListFormat().Length() + 
                                       title->Length() +
                                       value->Length() + 
                                       KTxtCompulsory().Length() );
                                       
    TPtr itemText = itemTextBuf->Des();

    itemText.Format( KSettingListFormat, title, value ); 
    
    // indicator for compulsory settings
    if ( convItem->iAttribFlags & EConvCompulsory )
        {
        itemText.Append( KTxtCompulsory );        
        }
        
    aItemArray.AppendL( itemText );
    CleanupStack::PopAndDestroy( 2, title ); // itemTextBuf, title

    if ( !aValueString )
        {
        CleanupStack::PopAndDestroy( (HBufC*)value );
        }
        
    iSettingIndex->AppendL( aSettingItemId );
    }

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::AppendSettingTextsL
// --------------------------------------------------------------------------
//

EXPORT_C void CmPluginBaseSettingsDlg::AppendSettingTextsL(   
                                              CDesCArray& aItemArray,
                                              const TInt aSettingItemId,
                                              const TInt aTitleResId,
                                              const TInt aValueResId,
                                              const TBool aCompulsory ) const
    {
    HBufC* value = NULL;
    if ( aValueResId )
        {
        value = StringLoader::LoadLC( aValueResId );
        }
    else
        {   
        value = KNullDesC().AllocL();
        CleanupStack::PushL( value );
        }        
    TPtr ptrValue = value->Des();
    
    AppendSettingTextsL( aItemArray, aSettingItemId, 
                                        aTitleResId, ptrValue, aCompulsory );
    
    CleanupStack::PopAndDestroy( value );
    }
    
// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::AppendSettingTextsL
// --------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlg::AppendSettingTextsL(   
                                              CDesCArray& aItemArray,
                                              const TInt aSettingItemId,
                                              const TInt aTitleResId,
                                              const TDesC& aValueString,
                                              const TBool aCompulsory ) const
    {
    // Title
    HBufC* title = StringLoader::LoadLC( aTitleResId );

    _LIT( KSettingListFormat, "\t%S\t\t%S" );
    HBufC* itemTextBuf = HBufC::NewLC( KSettingListFormat().Length() + 
                                       title->Length() +
                                       aValueString.Length() + 
                                       KTxtCompulsory().Length() );
                                       
    TPtr itemText = itemTextBuf->Des();

    itemText.Format( KSettingListFormat, title, &aValueString ); 
    
    // indicator for compulsory settings
    if ( aCompulsory )
        {
        itemText.Append( KTxtCompulsory );        
        }
        
    aItemArray.AppendL( itemText );
    CleanupStack::PopAndDestroy( 2, title ); // itemTextBuf, title
    iSettingIndex->AppendL( aSettingItemId );
    }

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlg::DynInitMenuPaneL( 
                                               TInt aResourceId,
                                               CEikMenuPane* aMenuPane  )
    {
    CAknDialog::DynInitMenuPaneL( aResourceId, aMenuPane );
        
    if ( aResourceId == R_PLUGIN_BASE_MENU )
        {
    	if (!iHelp)
            {
            aMenuPane->DeleteMenuItem( EAknCmdHelp );		    
            }
        TInt currentItem = iSettingIndex->At( iListbox->CurrentItemIndex() );
        if ( currentItem == ECmBearerSettingName )
            {
            
            aMenuPane->SetItemDimmed( EPluginBaseCmdChange, ETrue );
            }
        }
    }
    
// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::ProcessCommandL
// --------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlg::ProcessCommandL( TInt aCommandId )
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
        case EPluginBaseCmdExit:
            {
            iExitReason = KDialogUserExit;
            if( iIsPossibleToSaveSetting && iHasSettingChanged )
            {
                iCmPluginBaseEng.UpdateL();
                iHasSettingChanged = EFalse;
            }

            TryExitL( iExitReason );
            break;
            }
        case EAknSoftkeyOk:
        case EAknSoftkeyChange:
            {
            // Function CheckSpaceBelowCriticalLevelL may User::Leave with KLeaveWithoutAlert
            // if no space available to save changes. The User::Leave message will be caught
            // by iCmdExec. So,it is not caught here.
            iCmPluginBaseEng.CheckSpaceBelowCriticalLevelL();

            ShowPopupSettingPageL( 
                    iSettingIndex->At( iListbox->CurrentItemIndex() ), 
                                        aCommandId );

            break;            
            }
        case EAknCmdHelp:
            {
            // Help context (the default value) has been set so this value is used.
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

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::InitTextsL
// called before the dialog is shown
// to initialize localized textual data
// --------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlg::InitTextsL()
    {
    // set pane text if neccessary...
    // pane text needed if not pop-up...
    if ( iEikonEnv )
        {
        iStatusPane = iEikonEnv->AppUiFactory()->StatusPane();
        iTitlePane = ( CAknTitlePane* )iStatusPane->ControlL(
                                TUid::Uid( EEikStatusPaneUidTitle ) );

        iOldTitleText = iTitlePane->Text()->AllocL();
        HBufC* pdConnName = iCmPluginBaseEng.GetStringAttributeL( ECmName );
        CleanupStack::PushL( pdConnName ); // 1
                
        if ( pdConnName )
            {
            iTitlePane->SetTextL( *pdConnName );
            }
        else
            {
            iTitlePane->SetTextL( KNullDesC );
            }        
        CleanupStack::PopAndDestroy( pdConnName ); // 0


        iNaviPane = ( CAknNavigationControlContainer* )iStatusPane->ControlL(
                                        TUid::Uid( EEikStatusPaneUidNavi ) );
        iNaviDecorator = iNaviPane->CreateNavigationLabelL( KNullDesC );
        iNaviPane->PushL( *iNaviDecorator );
        }
    }

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::OkToExitL
// Good to know : EAknSoftkeyCancel is never called, because
// EEikDialogFlagNotifyEsc flag is not set in the resource.
// --------------------------------------------------------------------------
//
EXPORT_C TBool CmPluginBaseSettingsDlg::OkToExitL( TInt aButtonId )
    {
    // Translate the button presses into commands for the appui & current
    // view to handle
    TBool retval( EFalse );
    
    switch ( aButtonId )
        {
        case EAknSoftkeyOk:
        case EAknSoftkeyChange:
            {
            ProcessCommandL( aButtonId );
            break;
            }        
        case EAknSoftkeyBack:
        case EAknSoftkeyExit:
        case KDialogUserExit:
            {
            
            if ( iIsPossibleToSaveSetting && iHasSettingChanged )
            {
                iCmPluginBaseEng.UpdateL();
                iHasSettingChanged = EFalse;
            }
            retval = ETrue;
            break;
            }
        case EAknSoftkeyOptions:
            {
            DisplayMenuL();
            break;
            }
        default:
            {
            retval = ETrue;
            break;
            }
        }

    return retval;
    }

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::HandleListBoxEventL
// --------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlg::HandleListBoxEventL( 
                        CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            {
            if ( !iProcessing )
                {
                delete iCmdExec;
                iCmdExec = NULL;
                iCmdExec = new (ELeave) CCmdExec(*this);
                iCmdExec->Execute();
                }
            break;
            }
        case EEventEditingStarted:
            {
            break;
            }
        case EEventEditingStopped:
            {
            break;
            }
        default:
            {
            break;
//            User::Leave( KErrNotSupported );
            }
        }
    }

// ---------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::OfferKeyEventL
// ---------------------------------------------------------------------------
//
EXPORT_C TKeyResponse CmPluginBaseSettingsDlg::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                       TEventCode aType )    
    {
    TKeyResponse retVal ( EKeyWasNotConsumed );
    switch( aKeyEvent.iCode )
        {
        // Add processing for case EKeyEscape to support status pane event in IAP-related editing 
        // views (Dlg view, AdvDlg view and IPv4 and IPv6 views and so on)
        case EKeyEscape:
            TryExitL( iExitReason );
            retVal = EKeyWasConsumed;
            break;
        default:
            retVal = iListbox->OfferKeyEventL( aKeyEvent, aType ); 
            break;
        }
        
    return retVal;
    }

//---------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::LoadResourceL
//---------------------------------------------------------------------------
//
EXPORT_C TInt CmPluginBaseSettingsDlg::LoadResourceL ( 
                                                const TDesC& aResFileName )
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
    return err;
    }

//---------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::HandleListboxDataChangeL
// called before the dialog is shown to initialize listbox data
//---------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlg::HandleListboxDataChangeL()
    {
    UpdateListBoxContentL();
    iListbox->HandleItemAdditionL();
    }

//---------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::ShowPopupSettingPageL
//---------------------------------------------------------------------------
//
EXPORT_C TBool CmPluginBaseSettingsDlg::ShowPopupSettingPageL( 
                                           TUint32 aAttribute, TInt /*aCommandId*/ )
    {
    TBool retval( EFalse );
    
    switch ( aAttribute )
        {
        case ECmBearerSettingName:
            {
            TCmCommonUi::ShowNoteL ( R_PLUGIN_BASE_INFO_BEARER_NOT_EDITABLE,
                                                  TCmCommonUi::ECmInfoNote );
            break;
            }
        case ECmName:
        default:
            {
            retval = ShowPopupTextSettingPageL( aAttribute );
            break;
            }
        }
    return retval;
    }
    
//---------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::SelectItem
//---------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlg::SelectItem( TInt aItemIndex )
    {
    iListbox->ScrollToMakeItemVisible( aItemIndex );
    iListbox->SetCurrentItemIndexAndDraw( aItemIndex );
    }
    
//---------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::StringAttributeWithDefaultL
//---------------------------------------------------------------------------
//
EXPORT_C HBufC* CmPluginBaseSettingsDlg::StringAttributeWithDefaultL( 
                                        TUint32 aAttribute, TUint32 aDefRes )
    {
    HBufC* value = iCmPluginBaseEng.GetStringAttributeL( aAttribute );
    if ( *value == KNullDesC )
        {
        delete value;
        value = NULL;
        value = StringLoader::LoadL( aDefRes );
        }

    return value;
    }
   
// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::GetHelpContext
// --------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlg::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KHelpUidPlugin;
    aContext.iContext = iHelpContext;
    }

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::ShowPopupIpSettingPageL
// --------------------------------------------------------------------------
//
EXPORT_C TBool CmPluginBaseSettingsDlg::ShowPopupIpSettingPageL( 
                                                            TUint32 aAttribute,
                                                            TUint32 aTitleId )
    
    {
    TBool retval = EFalse;
    const TCmAttribConvTable* convItem = 
                        iCmPluginBaseEng.ConvTableItem( aAttribute );
    
    HBufC* servName = HBufC::NewLC( convItem->iMaxLength );
    TPtr ptrServName ( servName->Des() );
     
    HBufC* orig = iCmPluginBaseEng.GetStringAttributeL( aAttribute );    
    CleanupStack::PushL( orig );
    ptrServName.Copy( *orig );
    
    TInetAddr addr;
    addr.Input( ptrServName );
    
    TInt resId = EditorId( convItem->iEditorResId, R_TEXT_SETTING_PAGE_IP );
    
    CAknIpFieldSettingPage* dlg = 
                    new ( ELeave ) CAknIpFieldSettingPage( 
                                       resId, 
                                       addr );

    CleanupStack::PushL( dlg );
    SetTitleL( dlg, convItem->iTitleId, aTitleId );
    CleanupStack::Pop( dlg );

    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted ) )
        {
        addr.Output( ptrServName );
        
        if ( orig->Des().CompareF( ptrServName ) )
            // User modified IP address
            {
            iCmPluginBaseEng.SetStringAttributeL( aAttribute, ptrServName );
            retval = ETrue;  
            UpdateListBoxContentL();                                                                                     
            }
        }

    CleanupStack::PopAndDestroy( 2, servName ); // orig, servName
    return retval;
    }
    
// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::ShowRadioButtonSettingPageL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CmPluginBaseSettingsDlg::ShowRadioButtonSettingPageL( 
                                                const TUint32* aResIds, 
                                                TInt& aCurrentItem,
                                                TUint32 aTitleId )
    {
    CDesCArrayFlat* items = 
                   new ( ELeave ) CDesCArrayFlat( KCmArraySmallGranularity );
            
    CleanupStack::PushL( items );
    TInt count( 0 );       
    for( ; aResIds[count]; ++count )
        {
        HBufC* item = StringLoader::LoadLC( aResIds[count] );
        items->AppendL( *item );
        }
        
    TUint32 resId = EditorId( 0, R_RADIO_BUTTON_SETTING_PAGE_OK_CANCEL );
    CAknRadioButtonSettingPage* dlg = 
                            new ( ELeave ) CAknRadioButtonSettingPage(
                                    resId,
                                    aCurrentItem, 
                                    items );
    CleanupStack::PushL( dlg );
    SetTitleL( dlg, 0, aTitleId );
    CleanupStack::Pop( dlg );

    TBool retVal = dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged );    
    
    CleanupStack::PopAndDestroy( count );   // selection items
    CleanupStack::PopAndDestroy( items );
    
    return retVal;
    }
    
// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::ShowRadioButtonSettingPageL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CmPluginBaseSettingsDlg::ShowRadioButtonSettingPageL( 
                                                CDesCArrayFlat& aItems, 
                                                TInt& aCurrentItem,
                                                TUint32 aTitleId )
    {
    TUint32 resId = EditorId( 0, R_RADIO_BUTTON_SETTING_PAGE_OK_CANCEL );
    CAknRadioButtonSettingPage* dlg = 
                            new ( ELeave ) CAknRadioButtonSettingPage(
                                    resId,
                                    aCurrentItem, 
                                    &aItems );
    CleanupStack::PushL( dlg );
    SetTitleL( dlg, 0, aTitleId );
    CleanupStack::Pop( dlg );

    TBool retVal = dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged );    

    return retVal;
    }
    
//---------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::ShowPopupTextSettingPageL
//---------------------------------------------------------------------------
//
EXPORT_C TBool CmPluginBaseSettingsDlg::ShowPopupTextSettingPageL( 
                                                            TUint32 aAttribute,
                                                            TUint32 aTitleId )
    {    
    TBool retval( EFalse );
    const TCmAttribConvTable* convItem = 
                        iCmPluginBaseEng.ConvTableItem( aAttribute );
    
    HBufC* textToChange = HBufC::NewLC( convItem->iMaxLength );
    TPtr ptrTextToChange( textToChange->Des() );

    HBufC* value = HBufC::NewLC( convItem->iMaxLength );
    if ( convItem->iAttribFlags & EConv8Bits )
        // convert 8bits attribute to 16bits
        {
        HBufC8* stringFromCm = 
                iCmPluginBaseEng.GetString8AttributeL( aAttribute );
        value->Des().Copy( *stringFromCm );

        delete stringFromCm; stringFromCm = NULL;
        }
    else
        {
        HBufC* stringFromCm = 
                iCmPluginBaseEng.GetStringAttributeL( aAttribute );
        value->Des().Copy( *stringFromCm );

        delete stringFromCm; stringFromCm = NULL;
        }
        
    if( *value == KNullDesC && convItem->iDefValueResId )
        // empty string is replaced with the default value from resource
        {
        HBufC* stringFromCm = StringLoader::LoadL( convItem->iDefValueResId );
                
        value->Des().Copy( *stringFromCm );

        delete stringFromCm; stringFromCm = NULL;
        }
   
    TPtr ptrValue( value->Des() );
    ptrTextToChange.Copy( ptrValue );
    
    TInt textSettingPageFlags( EAknSettingPageNoOrdinalDisplayed );
    
    if ( !(convItem->iAttribFlags & EConvNoZeroLength) )
        {
        textSettingPageFlags = CAknTextSettingPage::EZeroLengthAllowed;
        }

    TUint32 resId = EditorId( convItem->iEditorResId, 0 );

    CAknTextSettingPage* dlg = new ( ELeave ) CAknTextSettingPage( 
                                    resId, 
                                    ptrTextToChange, 
                                    textSettingPageFlags );

    CleanupStack::PushL( dlg );
    SetTitleL( dlg, convItem->iTitleId, aTitleId );
    CleanupStack::Pop( dlg );

    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted ) )
        {
        if ( aAttribute == ECmName )
            {
            iTitlePane->SetTextL( ptrTextToChange );                                                                          
            }
        if ( ptrTextToChange.Compare( ptrValue ) )
            // user modified the value
            {
            if ( convItem->iAttribFlags & EConv8Bits )
                // convert back from 16bits to 8bits
                {
                HBufC8* stringTo = HBufC8::NewLC( ptrTextToChange.Length() );
                
                stringTo->Des().Copy( ptrTextToChange );
                iCmPluginBaseEng.SetString8AttributeL( aAttribute, *stringTo );
                                                                   
                CleanupStack::PopAndDestroy( stringTo );
                }
            else
                {
                if( aAttribute == ECmName )
                    {
                    SetUniqueNameL( ptrTextToChange );
                    }
                else
                    {
                    iCmPluginBaseEng.SetStringAttributeL( aAttribute, 
                                                      ptrTextToChange );
                    }    
                }
                
            retval = ETrue;
            UpdateListBoxContentL();
            }
        }

    CleanupStack::PopAndDestroy( 2, textToChange );
    return retval;
    }


void CmPluginBaseSettingsDlg::SetUniqueNameL( TPtr aPtrTextToChange )
    {
     // Saves the recent naming method and set it to "Not accept"
    TUint32 savedNM = iCmPluginBaseEng.GetIntAttributeL( ECmNamingMethod );
    iCmPluginBaseEng.SetIntAttributeL( ECmNamingMethod, ENamingNotAccept );

    // Tries to save the nem name. If it is not unique
    // then ret == KErrArgument
    TRAPD( err, iCmPluginBaseEng.SetStringAttributeL( ECmName, aPtrTextToChange ) );

    if( KErrArgument == err )
        {
        // Opens a info note
        HBufC* noteLine = StringLoader::LoadLC(
                    R_CMMANAGERUI_INFO_DEST_ALREADY_IN_USE, aPtrTextToChange );

        TCmCommonUi::ShowNoteL( *noteLine, TCmCommonUi::ECmInfoNote );

        CleanupStack::PopAndDestroy( noteLine );
        }

    // Sets back the original naming method
    iCmPluginBaseEng.SetIntAttributeL( ECmNamingMethod, savedNM );
    }



//---------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::ShowPopupPasswordSettingPageL
//---------------------------------------------------------------------------
//
EXPORT_C TBool CmPluginBaseSettingsDlg::ShowPopupPasswordSettingPageL( 
                                                        TUint32 aAttribute,
                                                        TUint32 aTitleId )
    {
    TBool retVal( EFalse );

    const TCmAttribConvTable* convItem = 
                        iCmPluginBaseEng.ConvTableItem( aAttribute );
    HBufC* newPassword = HBufC::NewLC( KCmMaxPasswordLength );
    TPtr16 ptrNewPassword = newPassword->Des();
    
    HBufC* oldPassword = NULL;
    oldPassword = iCmPluginBaseEng.GetStringAttributeL( aAttribute );
    CleanupStack::PushL( oldPassword );   
    
    TUint32 resId = EditorId( convItem->iEditorResId, 
                              R_TEXT_SETTING_PAGE_PASSWD );
    CAknAlphaPasswordSettingPage* dlg = 
                    new ( ELeave ) CAknAlphaPasswordSettingPage(
                                            resId,
                                            ptrNewPassword,
                                            *oldPassword );
    CleanupStack::PushL( dlg );
    SetTitleL( dlg, convItem->iTitleId, aTitleId );
    CleanupStack::Pop( dlg );

    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
        {
        iCmPluginBaseEng.SetStringAttributeL( aAttribute, newPassword->Des() );
        UpdateListBoxContentL();

        retVal = ETrue;
        }
    CleanupStack::PopAndDestroy( 2, newPassword );
    
    return retVal;
    }

//---------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::ShowNumSettingPageL
//---------------------------------------------------------------------------
//
EXPORT_C TBool CmPluginBaseSettingsDlg::ShowPopupPortNumSettingPageL( 
                                    TUint32 aAttribute, TUint32 aTitleId )
    {
    TBool retVal( EFalse );
    const TCmAttribConvTable* convItem = 
                        iCmPluginBaseEng.ConvTableItem( aAttribute );

    TInt value( 0 );
    value = iCmPluginBaseEng.GetIntAttributeL( aAttribute );

    TInt origVal = value;
    
    HBufC* buf = HBufC::NewLC( convItem->iMaxLength );
    TPtr ptr( buf->Des() );
    
    ptr.Num( value );
    
    TInt flags = EAknSettingPageNoOrdinalDisplayed;
    
    if ( !(convItem->iAttribFlags & EConvNoZeroLength) )
        {
        flags |= CAknTextSettingPage::EZeroLengthAllowed;
        }
    
    TUint32 resId = EditorId( convItem->iEditorResId, 
                              R_PORT_NUMBER_SETTING_PAGE );
    CAknTextSettingPage* dlg = new ( ELeave ) CAknTextSettingPage( 
                                    resId,
                                    ptr, 
                                    flags );

    CleanupStack::PushL( dlg );
    SetTitleL( dlg, convItem->iTitleId, aTitleId );
    CleanupStack::Pop( dlg );

    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted ) )
        {
        TLex lex ( buf->Des() );

        // An empty string should be treated as default        
        if ( buf->Des() == KNullDesC )
            {
            value = 0;
            }
        else
            {
            lex.Val( value );
            }
        if ( value != origVal )
            {
            iCmPluginBaseEng.SetIntAttributeL( aAttribute, value );
            retVal = ETrue;
            UpdateListBoxContentL();
            }
        }
        
    CleanupStack::PopAndDestroy( buf );
    return retVal;
    }

//---------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::ShowPopupIPv4DNSEditorL
//---------------------------------------------------------------------------
//
EXPORT_C TBool CmPluginBaseSettingsDlg::ShowPopupIPv4DNSEditorL(
                                            TUint32 aDNSUsageAttribute,
                                            TUint32 aDNS1ServAttribute,
                                            TUint32 aDNS2ServAttribute,
                                            TUint32 aTitleId )
    {
    // In default the return value is 'Automatic'
    TBool retVal( EFalse );
    // In default DNS setting is automatic.
    const TCmAttribConvTable* convItem = 
                        iCmPluginBaseEng.ConvTableItem( aDNSUsageAttribute );

    HBufC* primServ = iCmPluginBaseEng.GetStringAttributeL( aDNS1ServAttribute );
    CleanupStack::PushL( primServ );
    HBufC* secServ = iCmPluginBaseEng.GetStringAttributeL( aDNS2ServAttribute );
    CleanupStack::PushL( secServ );
    
    TBuf<KIpAddressWidth> newPrim;
    TBuf<KIpAddressWidth> newSec;
    
    newPrim.Copy( *primServ );
    newSec.Copy( *secServ );
    
    TInetAddr dns1Addr;
    TInetAddr dns2Addr;
    
    dns1Addr.Input( newPrim );
    dns2Addr.Input( newSec );
    
    CAknMultiLineIpQueryDialog* dlg = 
            CAknMultiLineIpQueryDialog::NewL( dns1Addr, 
                                              dns2Addr );
    
    CleanupStack::PushL( dlg );

    TUint32 editorId = EditorId( convItem->iEditorResId, R_DNSIP4_QUERY );
    dlg->PrepareLC( editorId );

    TUint32 titleId = TitleId( convItem->iTitleId, aTitleId );
    if ( titleId )
        {
        dlg->SetTitleL( titleId );
        }
        
    CleanupStack::Pop( dlg );
    
    if ( dlg->RunLD() )
        {
        dns1Addr.Output( newPrim );
        dns2Addr.Output( newSec );
        
        if ( newPrim.CompareF( *primServ ) ||
             newSec.CompareF( *secServ ) )
            // User modified the DNS setting
            {
            iCmPluginBaseEng.SetDNSServerAddressL( 
                                aDNS1ServAttribute,
                                newPrim,
                                aDNS2ServAttribute, 
                                newSec,
                                aDNSUsageAttribute,
                                EFalse );
                
            UpdateListBoxContentL();
            
            retVal = ETrue;
            }
        }
    
    CleanupStack::PopAndDestroy( 2, primServ );
    
    return retVal;
    }

//---------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::ShowPopupIPv6DNSEditorL
//---------------------------------------------------------------------------
//
EXPORT_C TInt CmPluginBaseSettingsDlg::ShowPopupIPv6DNSEditorL(
                                            const TUint32* aSelectionItems,
                                            TUint32 aDNSUsageAttribute,
                                            TUint32 aDNS1ServAttribute,
                                            TUint32 aDNS2ServAttribute )
    {
    TBool update( EFalse );
    TInt selected = GetIPv6DNSTypeL( aDNS1ServAttribute, aDNS2ServAttribute );
    
    // convert the current setting value to the selection index
    TInt resId = KTIPv6DNSItems[selected].iResId;    
    TInt i = 0;
    while ( aSelectionItems[i] )
        {
        if ( resId == aSelectionItems[i] )
            {
            selected = i;
            break;
            }
        i++;
        }
    
    if ( ShowRadioButtonSettingPageL( aSelectionItems, 
                                      selected, 
                                      R_QTN_SET_DNS_SERVERS_IP ) )
        {
        // convert the selection to the setting value
        resId = aSelectionItems[selected];
        TInt j = 0;
        
        while ( KTIPv6DNSItems[j].iResId )
            {
            if ( resId == KTIPv6DNSItems[j].iResId )
                {
                selected = KTIPv6DNSItems[j].iSettingValue;
                break;
                }
            j++;
            }
        
        switch ( selected )
            {
            case EIPv6WellKnown: 
                {
                iCmPluginBaseEng.SetStringAttributeL( 
                        aDNS1ServAttribute, KKnownIp6NameServer1 );
                iCmPluginBaseEng.SetStringAttributeL( 
                        aDNS2ServAttribute, KKnownIp6NameServer2 );
                        
                iCmPluginBaseEng.SetBoolAttributeL( 
                        aDNSUsageAttribute, EFalse );
                        
                update = ETrue;
                }
                break;

            case EIPv6Unspecified: 
                {
                iCmPluginBaseEng.SetStringAttributeL( 
                        aDNS1ServAttribute, KDynamicIpv6Address );
                iCmPluginBaseEng.SetStringAttributeL( 
                        aDNS2ServAttribute, KDynamicIpv6Address );               
                iCmPluginBaseEng.SetBoolAttributeL( 
                        aDNSUsageAttribute, ETrue );
                        
                update = ETrue;
                }
                break;

            case EIPv6UserDefined:                 
                {
                TInt err1;
                TInt err2;
                TBool cancelled(EFalse);
                do
                    {
                    // These has to be reset to KErrNone in case of
                    // inputting an invalid address (DLAN-7FJD68). 
                    err1 = KErrNone;
                    err2 = KErrNone;
             
                    HBufC* dns1 = HBufC::NewLC( KMaxIPv6NameServerLength );
                    HBufC* dns2 = HBufC::NewLC( KMaxIPv6NameServerLength );
                    
                    TPtr dns1ptr( dns1->Des() );
                    TPtr dns2ptr( dns2->Des() );
                    
                    HBufC* temp = iCmPluginBaseEng.GetStringAttributeL(
                                                aDNS1ServAttribute );
                    dns1ptr.Copy( *temp );
                    delete temp; temp = NULL;
                    
                    temp = iCmPluginBaseEng.GetStringAttributeL( 
                                                aDNS2ServAttribute );
                    dns2ptr.Copy( *temp );
                    delete temp; temp = NULL;

                    // KDynamicIpv6Address is offered instead of empty string 
                    if ( dns1ptr.Length() == 0 )
                        { 
                        dns1ptr = KDynamicIpv6Address;
                        }
                    if ( dns2ptr.Length() == 0 )
                        { 
                        dns2ptr = KDynamicIpv6Address;
                        }

                        
                    if ( ( dns1ptr.Compare( KKnownIp6NameServer1 ) == 0 ) && 
                         ( dns2ptr.Compare( KKnownIp6NameServer2 ) == 0 ) )
                        { // special value, well-known
                        // in this case 'dynamic' string must be offered so 
                        // temp data must be changed to 'dynamic'
                        dns1ptr = KDynamicIpv6Address;
                        dns2ptr = KDynamicIpv6Address;
                        }

                    CCmPluginMultiLineDialog* ipquery = 
                        CCmPluginMultiLineDialog::NewL( dns1ptr, 
                                                        dns2ptr );

                    if ( ipquery->ExecuteLD( R_DNSIP6_QUERY ) )
                        {
                        TInetAddr dns1Addr;
                        TInetAddr dns2Addr;

                        if ( dns1ptr.Compare( KNullDesC ) )
                            {
                            err1 = dns1Addr.Input( dns1ptr );
                            }
                        if ( dns2ptr.Compare( KNullDesC ) )
                            {
                            err2 = dns2Addr.Input( dns2ptr );
                            }
                        if ( err1 || err2 )
                            {
                            TCmCommonUi::ShowNoteL
                                ( R_QTN_SET_INFO_INVALID_IP,
                                TCmCommonUi::ECmErrorNote );
                            }
                        else
                            {
                            iCmPluginBaseEng.SetDNSServerAddressL( 
                                                aDNS1ServAttribute,
                                                dns1ptr,
                                                aDNS2ServAttribute, 
                                                dns2ptr,
                                                aDNSUsageAttribute,
                                                ETrue );

                            update = ETrue;
                            }
                        }
                    else
                        {
                        cancelled = ETrue;
                        }
                    CleanupStack::PopAndDestroy( dns2 );
                    CleanupStack::PopAndDestroy( dns1 );                            
                    } while ( (err1 || err2) && (!cancelled) );
                break;
                }
            default:
                {
                User::Leave( KErrNotSupported );
                }
            }
        }

    if( update )
        {
        UpdateListBoxContentL();
        }
                
    return selected;
    }

//---------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::TitleId
//---------------------------------------------------------------------------
//
TUint32 CmPluginBaseSettingsDlg::TitleId( TUint32 aConvTitleId,
                                          TUint32 aTitleId ) const
    {
    TUint32 titleId( 0 );
    if ( aTitleId )
        {
        titleId = aTitleId;
        }
    else if ( !titleId )
        {
        titleId = aConvTitleId;
        }
        
    return titleId;
    }
    
//---------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::SetTitleL
//---------------------------------------------------------------------------
//
void CmPluginBaseSettingsDlg::SetTitleL( CAknSettingPage* aDlg,
                                         TUint32 aConvTitleId,
                                         TUint32 aTitleId ) const
    {
    TUint32 titleId( TitleId( aConvTitleId, aTitleId ) );
        
    if ( titleId )
        {
        HBufC* title = StringLoader::LoadLC( titleId );
        aDlg->SetSettingTextL( *title );
        CleanupStack::PopAndDestroy( title );
        }
    }
    
//---------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::EditorId
//---------------------------------------------------------------------------
//
TUint32 CmPluginBaseSettingsDlg::EditorId( TUint32 aConvEditorId,
                                           TUint32 aEditorId ) const
    {
    TUint32 resId( 0 );
    if ( aConvEditorId )
        {
        resId = aConvEditorId;
        }
    if ( !resId )
        {
        resId = aEditorId;
        }

    return resId;
    }
    
// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::GetIPv6DNSTypeL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CmPluginBaseSettingsDlg::GetIPv6DNSTypeL( 
                                                TUint32 aDNS1ServAttribute,
                                                TUint32 aDNS2ServAttribute )
    {
    TIPv6Types retval( EIPv6UserDefined );

    HBufC* priServ = NULL;
    priServ = iCmPluginBaseEng.GetStringAttributeL( aDNS1ServAttribute );
    CleanupStack::PushL( priServ );
    TPtr16 ptr1( priServ->Des() );
       
    HBufC* secServ = NULL;
    secServ = iCmPluginBaseEng.GetStringAttributeL( aDNS2ServAttribute );
    CleanupStack::PushL( secServ );
    TPtr16 ptr2( secServ->Des() );
    
    // net type 6: if IspIPv6NameServ <> 0.0.0.0 
    // => User defined OR well-known
    
    if ( ( !ptr1.Compare( KKnownIp6NameServer1 ) )
        && ( !ptr2.Compare( KKnownIp6NameServer2 ) ) )
        { // special value, well-known
        retval = EIPv6WellKnown;
        }
    else
        { 
        // dynamic, if 0:0:...
        // OR EMPTY text...
        if ( ( ( !ptr1.Compare( KDynamicIpv6Address ) )
               || ( !ptr1.Compare( KNullDesC ) ) )
           &&( ( !ptr2.Compare( KDynamicIpv6Address ) ) 
               ||( !ptr2.Compare( KDynamicIpv6Address ) ) ) )
            { // dynamic, 0:0:...
            retval = EIPv6Unspecified;
            }
        else
            { // other value, user defined
            retval = EIPv6UserDefined;
            }
        }

    CleanupStack::PopAndDestroy( 2, priServ );

    return retval;
    }
    
// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::HandleResourceChange
// --------------------------------------------------------------------------
//

EXPORT_C void CmPluginBaseSettingsDlg::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType ); 

    CAknDialog::HandleResourceChange(aType);

    if( aType == KEikDynamicLayoutVariantSwitch )
        {
        DrawNow(); 
        }
    }

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::Execute
// --------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlg::Execute()
    {
    iProcessing = ETrue;
    
    TRAPD( err, ProcessCommandL( EAknSoftkeyChange ) );
    if ( err )
        {
        HandleLeaveError( err );
        }
    iProcessing = EFalse;
    }

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::HandleLeaveError
// --------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlg::HandleLeaveError( TInt aError )
{
    switch( aError )
    {
        case KLeaveWithoutAlert:
        case KErrNoMemory:
            // Indicate no memory to save setting changes
            iIsPossibleToSaveSetting = EFalse;

            if (iProcessing)
            {
                // Release processing protection so that the next processing can be done
                iProcessing = EFalse;
            }
            break;
        default:
            // More leave errors may be handled if necessary
            // Now other leave errors are ignored here
            break;
    }
}
