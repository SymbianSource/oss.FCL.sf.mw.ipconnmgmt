/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Dialog for setting the gateway. It's used temporarily.
*
*/

#include <avkon.hrh>
#include <eikdef.h>
#include <eikenv.h>
#include <eikedwin.h>
#include <gulicon.h>
#include <cmmanager.rsg>
#include <data_caging_path_literals.hrh>
#include <cmmanager.mbg> 
#include <badesca.h>        // CDesCArray
#include <aknnavide.h>
#include <akntitle.h>
#include <eikmenup.h>
#include <StringLoader.h>
#include <AknIconArray.h>
#include <aknnavilabel.h>
#include <textresolver.h>

#include <hlplch.h>
#include <csxhelp/cp.hlp.hrh>

#include <cmcommonconstants.h>
#include <cmdestination.h>
#include <cmcommonui.h>
#include <cmpluginbaseeng.h>
#include <cmpluginbase.h>

#include "cmmanager.hrh"
#include "destlistitem.h"
#include "destlistitemlist.h"
#include "destdlg.h"
#include "cmwizard.h"
#include "uncatdlg.h"
#include "cmdlg.h"    
#include "cmdesticondialog.h"
#include "cmlistboxmodel.h"
#include "cmlistitem.h"
#include <mcmdexec.h>
#include "ccmdexec.h"
#include "cmlogger.h"

using namespace CMManager;
using namespace CommsDat;

// ---------------------------------------------------------------------------
// CDestDlg::ConstructAndRunLD
// Constructs the dialog and runs it.
// ---------------------------------------------------------------------------
//
TInt CDestDlg::ConstructAndRunLD( CCmManagerImpl* aCmManagerImpl,  
                                  TUint32 aHighlight,
                                  TUint32& aSelected,
                                  TBool& aExiting )
    {
    iHighlight = aHighlight;
    iSelected = &aSelected;
    iCmManagerImpl = aCmManagerImpl;
    iExiting = &aExiting;
    *iExiting = EFalse;
    
    iCmdExec = new (ELeave) CCmdExec(*this);

    ConstructL( R_DEST_MENUBAR );
    PrepareLC( R_DEST_DIALOG );    
    iModel = new( ELeave )CDestListboxModel();
    iConnSettingsImpl = CCmConnSettingsUiImpl::NewL( iCmManagerImpl );

    // CommsDat notifier should be started with this first UI view for
    // it will create active object
    iCmManagerImpl->StartCommsDatNotifierL();
    iCmManagerImpl->WatcherRegisterL( this );
    
    return RunLD();
    }
    
// ---------------------------------------------------------------------------
// CDestDlg::CDestDlg()
// ---------------------------------------------------------------------------
//
CDestDlg::CDestDlg() 
    : iModelPassed( EFalse )
    , iExitReason( KDialogUserExit )
    , iEscapeArrived( EFalse )
    {
    CLOG_CREATE;
    }
    
// ---------------------------------------------------------------------------
// CDestDlg::~CApSelectorDialog
// Destructor
// ---------------------------------------------------------------------------
//
CDestDlg::~CDestDlg()
    {
    // iModel is deleted by the ListBox because LB owns the model...
    // of course only after we passed ownership...
    if ( !iModelPassed )
        {
        delete iModel;
        }
    if ( iNaviDecorator )
        {
        delete iNaviDecorator;
        }
    if ( iTitlePane )
        {
        // set old text back, if we have it and if we are going back to
        // Connectivity Settings view
        if ( iOldTitleText )
            {
            if ( iExitReason == KDialogUserBack )
                {
                TRAP_IGNORE( iTitlePane->SetTextL( *iOldTitleText ) );
                }
            delete iOldTitleText;
            }
        }
    if ( iConnSettingsImpl )
        {
        delete iConnSettingsImpl;            
        }

    if( iCmWizard )
        {
        delete iCmWizard;
        }
    
    delete iCmdExec;
    
    CLOG_CLOSE;
    }

// ---------------------------------------------------------------------------
// CDestDlg::PreLayoutDynInitL();
// called by framework before dialog is shown
// ---------------------------------------------------------------------------
//
void CDestDlg::PreLayoutDynInitL()
    {
    iListbox =
        STATIC_CAST( CDestListbox*, Control( KDestListboxId ) );
    iListbox->CreateScrollBarFrameL( ETrue );
    iListbox->ScrollBarFrame()->SetScrollBarVisibilityL
        ( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    iListbox->SetListBoxObserver( this );

    iListbox->Model()->SetItemTextArray( iModel );
    iModelPassed = ETrue;
    InitTextsL();    
    HandleListboxDataChangeL();
    iListbox->HandleItemAdditionL();
    }

// ----------------------------------------------------------------------------
// CDestDlg::DynInitMenuPaneL
// ----------------------------------------------------------------------------
//
void CDestDlg::DynInitMenuPaneL( TInt aResourceId,
                                        CEikMenuPane* aMenuPane )
    {
    switch ( aResourceId )
        {
        case R_DEST_MENU:
            {            
            // Check if it is the 'New connection' list item 
            // rather than a real destination
            TUint32 listItemUid = iListbox->CurrentItemUid();
           if ( listItemUid == KDestItemNewConnection ) 
                {
                aMenuPane->SetItemDimmed( ECmManagerUiCmdDestEdit, ETrue );
                aMenuPane->SetItemDimmed( ECmManagerUiCmdDestRename, ETrue );                
                aMenuPane->SetItemDimmed( ECmManagerUiCmdDestDelete, ETrue );                
                aMenuPane->SetItemDimmed( ECmManagerUiCmdDestChangeIcon, ETrue );   
                } 
            if ( !iCmManagerImpl->IsDefConnSupported() )
                {
                aMenuPane->SetItemDimmed( ECmManagerUiCmdDestDefaultConnection, ETrue );
                }
        	if (!iCmManagerImpl->IsHelpOn())
                {
                aMenuPane->DeleteMenuItem( EAknCmdHelp );		    
                }
            break;
            }
        default:
            break;
        }
    }
    
// ---------------------------------------------------------------------------
// CDestDlg::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CDestDlg::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                       TEventCode aType )    
    {
    LOGGER_ENTERFN( "CDestDlg::OfferKeyEventL" );
    CLOG_WRITE_1("Key iCode: %d", aKeyEvent.iCode );
    
    TKeyResponse retVal ( EKeyWasNotConsumed );
    
    TBool down( EFalse );
    
    switch ( aKeyEvent.iCode )
        {
        case EKeyEscape:
            {
            // Handling application close needs special care 
            // because of iCmWizard 
            if ( iCmWizard )
                {
                // if wizard alives then escape should do after wizard has ended.
                CLOG_WRITE( "CDestDlg::OfferKeyEventL: Wizard in long process" );
                iEscapeArrived = ETrue;
                retVal = EKeyWasConsumed;
                }
            else    
               {
                CLOG_WRITE( "CDestDlg::OfferKeyEventL:Escape" );
                retVal = CAknDialog::OfferKeyEventL( aKeyEvent, aType );
               }
            break;
            }
        case EKeyBackspace:
            {
            ProcessCommandL( ECmManagerUiCmdDestDelete );
            retVal = EKeyWasConsumed;
            break;
            }
        case EKeyDownArrow:
            {
            down = ETrue;
            // intended flow-through
            }
        case EKeyUpArrow:
            {
            // When moving from the 'New connection' (first) item, MSK should be 'Open'
            if ( iListbox->CurrentItemUid() == KDestItemNewConnection )
                {
                SetMskL( R_QTN_MSK_OPEN );
                }                    
            
            // When moving to the 'New connection' (first item)
            else if ( ( iListbox->CurrentItemIndex() == 1 && !down ) || 
                      ( iListbox->CurrentItemIndex() + 1 == 
                        iModel->MdcaCount() && down ) )
                {
                SetMskL( R_QTN_MSK_SELECT );
                }    
            // flow through to 'default' intended
            }
        default:
            {
            retVal = iListbox->OfferKeyEventL(aKeyEvent, aType);
            break;
            }
        }
    return retVal;
    }

// ----------------------------------------------------------------------------
// CDestDlg::ShowDefaultSetNoteL
// ----------------------------------------------------------------------------
//
void CDestDlg::ShowDefaultSetNoteL( TCmDefConnValue aSelection )
    {
    switch ( aSelection.iType )
        {
        case ECmDefConnDestination:
        case ECmDefConnConnectionMethod:
            {
            HBufC* connName =  iConnSettingsImpl->CreateDefaultConnectionNameL( aSelection );
            CleanupStack::PushL(connName);
            TCmCommonUi::ShowNoteL( R_CMMANAGERUI_DEFAULT_CONNECTION_SET_TO,
                                                *connName,
                                                TCmCommonUi::ECmOkNote );
            CleanupStack::PopAndDestroy( connName );
            }
        }
    }


// ----------------------------------------------------------------------------
// CDestDlg::SetDefaultNaviL
// ----------------------------------------------------------------------------
//
void CDestDlg::SetDefaultNaviL( TCmDefConnValue aSelection )    
    {
    HBufC* title = NULL;
    
    switch ( aSelection.iType )
        {
        case ECmDefConnDestination:
        case ECmDefConnConnectionMethod:
            {
            HBufC* connName =  iConnSettingsImpl->CreateDefaultConnectionNameL( aSelection );
            CleanupStack::PushL(connName);
            title = StringLoader::LoadL( R_CMMANAGERUI_NAVI_DEFAULT, *connName );
            CleanupStack::PopAndDestroy( connName );
            CleanupStack::PushL( title );
            break;
            }
        }    
    
    if ( iNaviDecorator )
        {
        if ( iNaviDecorator->ControlType() == 
             CAknNavigationDecorator::ENaviLabel )
            {
            CAknNaviLabel* naviLabel = 
                (CAknNaviLabel*)iNaviDecorator->DecoratedControl();
            if ( title )
                {
                naviLabel->SetTextL(*title);
                }
            else
                {
                naviLabel->SetTextL( KNullDesC );
                }
            
            iNaviPane->PushL( *iNaviDecorator );
            }
        }
        
    if ( title )
        {
        CleanupStack::PopAndDestroy( title );
        }
    }
    
// ----------------------------------------------------------------------------
// CDestDlg::ProcessCommandL
// ----------------------------------------------------------------------------
//
void CDestDlg::ProcessCommandL( TInt aCommandId )
    {
    LOGGER_ENTERFN( "CDestDlg::ProcessCommandL" );
    CLOG_WRITE_1("Command: %d", aCommandId );

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
        case ECmManagerUiCmdDestUserExit:
            {
            CLOG_WRITE( "CDestDlg::ProcessCommandL command: exit");
            iExitReason = KDialogUserExit;
            iCmManagerImpl->WatcherUnRegister();
            TryExitL( iExitReason );
            break;            
            }
        case EAknSoftkeyBack:
            {
            iExitReason = KDialogUserBack;
            iCmManagerImpl->WatcherUnRegister();
            TryExitL( iExitReason );
            break;
            }
        case ECmManagerUiCmdDestEdit:
            {
            OnCommandDestinationEditL();
            break;
            }            
        case ECmManagerUiCmdDestDefaultConnection:
            {
            TCmDCSettingSelection selection;
            TCmDefConnValue defConnValue;
            iCmManagerImpl->ReadDefConnL( defConnValue );
            switch (defConnValue.iType)
                {
                case ECmDefConnConnectionMethod:
                    {
                    selection.iResult = EDCConnectionMethod;
                    selection.iId = defConnValue.iId;
                    break;
                    }
                case ECmDefConnDestination:
                    {
                    selection.iResult = EDCDestination;
                    selection.iId = defConnValue.iId;
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            if ( iConnSettingsImpl->RunDefaultConnecitonRBPageL( selection ) )
                {
                defConnValue = selection.ConvertToDefConn();                
                
                iCmManagerImpl->WriteDefConnL( defConnValue );
                HandleListboxDataChangeL();
                }
            break;
            }
            
        case ECmManagerUiCmdDestChangeIcon:
            {
            // Check if this is UnCat
            if ( iListbox->CurrentItemUid() == KDestItemUncategorized )
                {
                TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                                        TCmCommonUi::ECmErrorNote );
                break;
                }

            // Check that destination is not protected
            CCmDestinationImpl* dest =
                       iCmManagerImpl->DestinationL( iListbox->CurrentItemUid() );
            CleanupStack::PushL( dest );
            if ( dest->ProtectionLevel() )
                {
                TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                                        TCmCommonUi::ECmErrorNote );

                CleanupStack::PopAndDestroy( dest );
                break;
                }

            TInt iconSelected = 0;
            CCmDestinationIconDialog* popup = 
                          new (ELeave) CCmDestinationIconDialog( iconSelected );  
            if ( popup->ExecuteLD() )
                {
                dest->SetIconL( iconSelected ); // subclass implements
                dest->UpdateL();
                CleanupStack::PopAndDestroy( dest ); 
                
                HandleListboxDataChangeL();
                break;
                }

            CleanupStack::PopAndDestroy( dest );

            break;
            
            }

            
       case ECmManagerUiCmdDestAdd:
            {
            AddDestinationL();
            break;           
            }
            
        case EAknSoftkeyClear:
        case ECmManagerUiCmdDestDelete:            
            {
            OnCommandDestinationDeleteL();
            break;
            
            }            
            
        case ECmManagerUiCmdDestRename:
            {
            OnCommandDestinationRenameL();
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
// CDestDlg::InitTextsL
// called before the dialog is shown
// to initialize localized textual data
// ---------------------------------------------------------------------------
//
void CDestDlg::InitTextsL()
    {
    // set pane text if neccessary...
    // pane text needed if not pop-up...
    if ( iEikonEnv )
        {
        iStatusPane = iEikonEnv->AppUiFactory()->StatusPane();
        iTitlePane =
            ( CAknTitlePane* )iStatusPane->ControlL(
                                TUid::Uid( EEikStatusPaneUidTitle ) );

        iOldTitleText = iTitlePane->Text()->AllocL();
        iTitlePane->SetTextL( 
                        *(StringLoader::LoadLC( R_CMMANAGERUI_NAVI )) );

        CleanupStack::PopAndDestroy(); // resource R_CMMANAGERUI_NAVI
        iNaviPane = ( CAknNavigationControlContainer* ) 
                        iStatusPane->ControlL( 
                                TUid::Uid( EEikStatusPaneUidNavi ) );
        iNaviDecorator = iNaviPane->CreateNavigationLabelL( KNullDesC );
        iNaviPane->PushL( *iNaviDecorator );
        if ( iCmManagerImpl->IsDefConnSupported() )
            {
            TCmDefConnValue selection;
            selection.iType = ECmDefConnAlwaysAsk;
            iCmManagerImpl->ReadDefConnL( selection );
            SetDefaultNaviL( selection );
            }
        }
    }

// ---------------------------------------------------------------------------
// CDestDlg::SetNoOfDestsL
// ---------------------------------------------------------------------------
//
void CDestDlg::SetNoOfDestsL( TInt aCount )
    {
    HBufC* title = NULL;
    
    if ( aCount == 1 )
        {
        title = StringLoader::LoadLC( R_QTN_NETW_CONSET_NAVI_ONE_DESTINATION );
        }
    else
        {
        title = StringLoader::LoadLC( R_QTN_NETW_CONSET_NAVI_NOF_DESTINATIONS,
                                         aCount );
        }
    if (iNaviDecorator)
        {
        if ( iNaviDecorator->ControlType() == 
             CAknNavigationDecorator::ENaviLabel )
            {
            CAknNaviLabel* naviLabel = 
                            (CAknNaviLabel*)iNaviDecorator->DecoratedControl();
            naviLabel->SetTextL(*title);
            iNaviPane->PushL( *iNaviDecorator );
            }
        }
        
      CleanupStack::PopAndDestroy( title );
    }

// ---------------------------------------------------------------------------
// CDestDlg::CreateCustomControlL
// ---------------------------------------------------------------------------
//
SEikControlInfo CDestDlg::CreateCustomControlL
( TInt aControlType )
    {
    SEikControlInfo controlInfo;
    controlInfo.iTrailerTextId = 0;
    controlInfo.iFlags = 0;
    if ( aControlType == KDestListboxType )
        {
        controlInfo.iControl = new ( ELeave ) CDestListbox;
        }
    else
        {
        controlInfo.iControl = NULL;
        }
    return controlInfo;
    }

// ---------------------------------------------------------------------------
// CDestDlg::OkToExitL
// Good to know : EAknSoftkeyCancel is never called, because
// EEikDialogFlagNotifyEsc flag is not set in the resource.
// ---------------------------------------------------------------------------
//
TBool CDestDlg::OkToExitL( TInt aButtonId )
    {
    CLOG_WRITE_1( "CDestDlg::OkToExitL buttonId: %d", aButtonId );
    
    // Translate the button presses into commands for the appui & current
    // view to handle
    TBool retval( EFalse );
    switch ( aButtonId )
        {
        case EAknSoftkeyOk:
        case EAknSoftkeySelect:
        case ECmManagerUiCmdCmAdd:
            {
            if ( !iProcessing )
                {
                iProcessing = ETrue;
                ProcessCommandL( ECmManagerUiCmdDestEdit );
                iProcessing = EFalse;
                }
            break;
            }
        case EAknSoftkeyBack:
            {
            if (!iProcessing)
                {
                *iExiting = EFalse;
                iExitReason = KDialogUserBack;
                retval = ETrue;                
                }
            break;
            }
        case EAknSoftkeyOptions:
            {
            if ( !iProcessing )
                {
                DisplayMenuL();
                }
            break;
            }
        default:
            {

            CLOG_WRITE_1( "CDestDlg::OkToExitL: High level stop %d", aButtonId );

            *iExiting = ETrue;
            retval = ETrue;
            break;
            }
        }
    return retval;
    }

// ---------------------------------------------------------------------------
// CDestDlg::HandleListBoxEventL
// ---------------------------------------------------------------------------
void CDestDlg::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                    TListBoxEvent aEventType )
    {
    LOGGER_ENTERFN( "CDestDlg::HandleListBoxEventL" );

    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            {
            if ( !iProcessing )
                {
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
            };
        };
    }

// ----------------------------------------------------------------------------
// CDestDlg::HandleListboxDataChangeL
// called before the dialog is shown to initialize listbox data
// ----------------------------------------------------------------------------
//
void CDestDlg::HandleListboxDataChangeL()
    {
    LOGGER_ENTERFN( "CDestDlg::HandleListboxDataChangeL" );

    iCmManagerImpl->OpenTransactionLC();
    
    iModel->ResetAndDestroy();
    RArray<TUint32> destIdArray = RArray<TUint32>( KCmArrayBigGranularity );

    CArrayPtr< CGulIcon >* icons = new( ELeave ) CAknIconArray( KGranularity );
    CleanupStack::PushL( icons );

    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
    
    iCmManagerImpl->AllDestinationsL(destIdArray);
    
    // Append the destinations to the list
    CleanupClosePushL( destIdArray ); // 1
    
   TCmDefConnValue defConnSel;
   defConnSel.iType = ECmDefConnAlwaysAsk;//default value
   if ( iCmManagerImpl->IsDefConnSupported() )
       {
        //refresh default connection in navi pane
        iCmManagerImpl->ReadDefConnL(defConnSel);
        SetDefaultNaviL( defConnSel );                                        
       }
   else        
        {
        SetNoOfDestsL( destIdArray.Count() );
        }
    ///!!!TEMP, no icon for add connection yet
    TParse mbmFile;
    User::LeaveIfError( mbmFile.Set( KManagerIconFilename, 
                                     &KDC_APP_BITMAP_DIR, 
                                     NULL ) );

    CGulIcon* icon = AknsUtils::CreateGulIconL( //second, KDestReservedIconIndexProtected 
                        skinInstance, 
                        KAknsIIDQgnPropSetConnDestAdd,
                        mbmFile.FullName(), 
                        EMbmCmmanagerQgn_prop_set_conn_dest_add, 
                        EMbmCmmanagerQgn_prop_set_conn_dest_add_mask ); 
    CleanupStack::PushL( icon );
    icons->AppendL( icon );
    CleanupStack::Pop( icon );
    
   icon = AknsUtils::CreateGulIconL( //second, KDestReservedIconIndexProtected 
                        skinInstance, 
                        KAknsIIDQgnIndiSettProtectedAdd,
                        mbmFile.FullName(), 
                        EMbmCmmanagerQgn_indi_sett_protected_add, 
                        EMbmCmmanagerQgn_indi_sett_protected_add_mask );
   

// Previous must me changed to following if any problems with theme colours
/*    AknsUtils::CreateColorIconLC( skinInstance,
                                  KAknsIIDQgnIndiSettProtectedAdd,
                                  KAknsIIDQsnIconColors,
                                  EAknsCIQsnIconColorsCG13,
                                  bitmap,
                                  mask,
                                  mbmFile.FullName(),
                                  EMbmCmmanagerQgn_indi_sett_protected_add,
                                  EMbmCmmanagerQgn_indi_sett_protected_add_mask,
                                  AKN_LAF_COLOR( 215 ) );

    icon = CGulIcon::NewL( bitmap, mask ); // Ownership transferred
    CleanupStack::Pop( 2 ); // bitmap, mask*/
                        
    CleanupStack::PushL( icon );
    icons->AppendL( icon );
    CleanupStack::Pop( icon );
    
    //third, uncategorized KDestReservedIconIndexUncategorized
    icons->AppendL( iCmManagerImpl->UncategorizedIconL() );
    
    //0..3 are reserved indexes!
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    
    AknsUtils::CreateColorIconLC( skinInstance,
                                  KAknsIIDQgnIndiDefaultConnAdd,
                                  KAknsIIDQsnIconColors,
                                  EAknsCIQsnIconColorsCG13,
                                  bitmap,
                                  mask,
                                  mbmFile.FullName(),
                                  EMbmCmmanagerQgn_indi_default_conn_add,
                                  EMbmCmmanagerQgn_indi_default_conn_add_mask,
                                  AKN_LAF_COLOR( 215 ) );

    icon = CGulIcon::NewL( bitmap, mask ); // Ownership transferred
    CleanupStack::Pop( 2 ); // bitmap, mask

    CleanupStack::PushL( icon );
    icons->AppendL( icon );
    CleanupStack::Pop( icon );
       
    // Add the New Connection Item
    HBufC* newConnection = StringLoader::LoadLC( 
                                    R_CMMANAGERUI_NEW_CONNECTION ); // 2
        
    CDestListItem* newConnItem = 
                    CDestListItem::NewLC( 
                            KDestItemNewConnection,
                            newConnection,
                            0,
                            KDestReservedIconIndexNewConn, EProtLevel0, EFalse  ); // 3
    iModel->AppendL( newConnItem ); // ownership relinquished
    CleanupStack::Pop( newConnItem );
    CleanupStack::Pop( newConnection );
    
    TTime destTime;

//refresh default connection icon in list - no effect if feature flag is off (defConnSel
// is ECmDefConnAlwaysAsk)
    TInt defUid = KErrNotFound;
    if ( defConnSel.iType == ECmDefConnDestination )
        {
        defUid = defConnSel.iId;
        }

    
    for (TInt i = 0; i < destIdArray.Count(); i++)
        {
        // Get the destinations from their IDs
        CCmDestinationImpl* dest = NULL;
        
        
        TRAPD( err, dest = iCmManagerImpl->DestinationL( destIdArray[i] ) );
        if( err )
            {
            continue;
            }
        CleanupStack::PushL( dest ); 

        // Check whether the MMS SNAP is in question. 
        // It should not be shown on the UI in the destinations list
        TInt snapMetadata = 0;
        TRAPD(metaErr, snapMetadata = dest->MetadataL( CMManager::ESnapMetadataPurpose ));
        if ( metaErr == KErrNone && snapMetadata == CMManager::ESnapPurposeMMS )
            {
            // This is the MMS SNAP, skip this item
            if( dest )
                {
                CleanupStack::PopAndDestroy( dest );
                dest = NULL;
                }  
            continue;
            }

        // Also hidden destinations are shown (TSW id ERUN-79KFAK)
        icons->AppendL( dest->IconL() );            
        HBufC* name =  dest->NameLC(); // 3
        TBool def = EFalse;
        if ( defUid == dest->Id() ) //this is the default destination, indicate it!
            {
            def = ETrue;
            }
        
        // The connection methods bound to this destination may have to be checked with
        // validity. Some connection methods might be invalid.
        TInt numOfCms = dest->ConnectionMethodCount();
        if (numOfCms > 0)
            {
            RArray<TUint32> cmIds;
            dest->ConnectMethodIdArrayL( cmIds );
            CleanupClosePushL( cmIds );

            for ( TInt j = 0; j < cmIds.Count(); j++ )
                {
                TUint recId = cmIds[j];

                TRAP( err, TUint32 bearerType = iCmManagerImpl->BearerTypeFromCmIdL( recId ) );
                if( err == KErrNotSupported )
                    {
                    CLOG_WRITE_1( "CDestDlg::HandleListboxDataChangeL, IAP(%d) unsupported", recId );
                    numOfCms = numOfCms -1;
                    }
                }

            CleanupStack::PopAndDestroy( &cmIds );
            }
        
        CDestListItem* item = CDestListItem::NewLC( 
                                    dest->Id(),
                                    name,
                                    numOfCms,
                                    icons->Count()-1, dest->ProtectionLevel(), def ); // 4
        iModel->AppendL( item );        // ownership passed
        CleanupStack::Pop( item );
        CleanupStack::Pop( name );
        if( dest )
            {
            CleanupStack::PopAndDestroy( dest );
            dest = NULL;
            }    
        }
        
    // Add the Uncategorised Item
    RArray<TUint32> uncatArray(KCmArraySmallGranularity);
    CleanupClosePushL(uncatArray);
    
    iCmManagerImpl->ConnectionMethodL( uncatArray, ETrue );
    TUint32 numberOfCMs = NumberOfCMsL( uncatArray );
    if ( numberOfCMs )
        {
        icon = AknsUtils::CreateGulIconL(
                        skinInstance, 
                        KAknsIIDQgnPropSetConnDestUncategorized,
                        mbmFile.FullName(), 
                        EMbmCmmanagerQgn_prop_set_conn_dest_uncategorized, 
                        EMbmCmmanagerQgn_prop_set_conn_dest_uncategorized_mask ); 
        CleanupStack::PushL( icon );
        icons->AppendL( icon );
        CleanupStack::Pop( icon );

        HBufC* uncat = StringLoader::LoadLC( 
                                    R_CMMANAGERUI_DEST_UNCATEGORIZED ); // 2
        
        // Check the hidden connection methods
        for ( TInt k = 0; k < uncatArray.Count(); k++ )
            {
            TUint recId = uncatArray[k];
            if ( iCmManagerImpl->GetConnectionMethodInfoBoolL( recId, ECmHidden ) )
                {
                numberOfCMs = numberOfCMs -1;
                }
            }
        
        CDestListItem* uncatItem = 
                        CDestListItem::NewLC( 
                                KDestItemUncategorized,
                                uncat,
                                numberOfCMs,
                                icons->Count()-1, EProtLevel0, EFalse ); // 3
        iModel->AppendL( uncatItem ); // ownership relinquished
        CleanupStack::Pop( uncatItem );
        CleanupStack::Pop( uncat );
        }

    CleanupStack::PopAndDestroy( 2, &destIdArray ); // destIdArray
        
    CArrayPtr<CGulIcon>* oldIcons = 
                            iListbox->ItemDrawer()->ColumnData()->IconArray();
    if( oldIcons )
        {
        oldIcons->ResetAndDestroy();
        delete oldIcons;
        }
    
    iListbox->ItemDrawer()->ColumnData()->SetIconArray( icons );
    iListbox->HandleItemAdditionL();
    iListbox->UpdateScrollBarsL();

    if ( !iListbox->CurrentItemUid() && !numberOfCMs )
        {
        // selection is out of range (e.g. Uncategorized deleted)
        iListbox->SetCurrentItemIndexAndDraw( 0 );
        }

    CleanupStack::Pop( icons );
    
    iCmManagerImpl->RollbackTransaction();   
    }
    
// ----------------------------------------------------------------------------
// CDestDlg::NumberOfCMsL
// ----------------------------------------------------------------------------
//
TUint32 CDestDlg::NumberOfCMsL(RArray<TUint32> cmArray)
    {
    LOGGER_ENTERFN( "CDestDlg::NumberOfCMsL" );

    TUint32 cmCount = cmArray.Count();
    TUint32 retValue = cmArray.Count();
    CCDIAPRecord* iapRecord = NULL;
    for ( TInt i = 0; i < cmCount; i++ )
        {
        if ( cmArray[i] <= 255 )// not embedded destination 
            {
            iapRecord = static_cast<CCDIAPRecord *>
                          (CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
            CleanupStack::PushL( iapRecord );
            
            iapRecord->SetRecordId( cmArray[i] );
            iapRecord->LoadL( iCmManagerImpl->Session() );
            
            TRAPD( err, iCmManagerImpl->BearerTypeFromIapRecordL( iapRecord ) );
            
            if( err == KErrNotSupported )
                // This is unsupported connection method -> don't display
                {
                retValue--;
                }
            else
                {
                User::LeaveIfError( err );
                }
            CleanupStack::PopAndDestroy( iapRecord );
            }
        }
    return retValue;
    }

// ----------------------------------------------------------------------------
// CDestDlg::AddDestinationL
// ----------------------------------------------------------------------------
//
void CDestDlg::AddDestinationL()
    {
    LOGGER_ENTERFN( "CDestDlg::AddDestinationL" );

    TBuf<KDestinationNameMaxLength> destName;    
    
    TBool okToAdd = EFalse;
    TBool nameAccepted = ETrue;
    do 
        {
        okToAdd = TCmCommonUi::ShowConfirmationQueryWithInputL(
                                           R_CMMANAGERUI_PRMPT_DESTINATION_NAME,
                                           destName );
        if ( okToAdd )
            {
            CCmDestinationImpl* dest = NULL;
            
            TRAPD(err, dest = iCmManagerImpl->CreateDestinationL( destName ) );
            if ( err == KErrAlreadyExists )
                {
                nameAccepted = EFalse;
                TCmCommonUi::ShowNoteL
                    ( R_CMMANAGERUI_INFO_DEST_ALREADY_IN_USE,
                    destName,
                    TCmCommonUi::ECmErrorNote );                        
                }
            else if ( err == KErrDiskFull )
                {
                CTextResolver* iTextResolver = CTextResolver::NewLC(*iCoeEnv); 
                okToAdd = EFalse;
                TPtrC buf;
                buf.Set(iTextResolver->ResolveErrorString(err)); 
                TCmCommonUi::ShowNoteL( buf, TCmCommonUi::ECmErrorNote );
                CleanupStack::PopAndDestroy( iTextResolver );
                }
            else if ( err == KErrNone )
                {
                CleanupStack::PushL( dest );
                nameAccepted = ETrue;
                TInt index = 0;
                TInt iconSelected = 0;
                CCmDestinationIconDialog* popup = 
                      new (ELeave) CCmDestinationIconDialog( iconSelected );  
                
                // Show the icon dialogs
                if ( popup->ExecuteLD() )
                    {
                    index = iconSelected;
                    
                    dest->SetIconL( index ); // subclass implements
                    //dest->UpdateL();
                    TRAPD( err, dest->UpdateL() );
                    if( err )
                        {
                        TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                                TCmCommonUi::ECmErrorNote );
                        }
                    else
                        {
                        TRAP( err, HandleListboxDataChangeL() );
                        if ( err )
                            {
                            TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                                    TCmCommonUi::ECmErrorNote );
                            }
                        }
                    
                    CDestListboxModel* lbmodel = 
                              STATIC_CAST( CDestListboxModel*,
                                           iListbox->Model()->ItemTextArray() );
                    CDestListItem* lbitem = 
                                        lbmodel->At( lbmodel->MdcaCount() - 1 );

                    //returns null if item is not present                                        
                    if ( lbitem->Uid() == KDestItemUncategorized  )
                        {
                        //last item is uncategorized
                        iListbox->ScrollToMakeItemVisible( 
                                                    lbmodel->MdcaCount() - 2 );
                        //last item is uncategorized
                        iListbox->SetCurrentItemIndexAndDraw( 
                                                    lbmodel->MdcaCount() - 2 );
                        }
                     else
                        {
                        iListbox->ScrollToMakeItemVisible( 
                                                    lbmodel->MdcaCount() - 1 );
                        iListbox->SetCurrentItemIndexAndDraw( 
                                                    lbmodel->MdcaCount() - 1 );
                        }                    

                    iListbox->HandleItemAdditionL();
               
                    // Sets the mittle soft key text to "Open"                        
                    SetMskL( R_QTN_MSK_OPEN );
                                            
                    }
                if(dest)
                    {
                    CleanupStack::PopAndDestroy( dest );
                    dest = NULL;
                    }    
                }
            }
        } while ( !nameAccepted && okToAdd );
    }

// ----------------------------------------------------------------------------
// CDestDlg::OnCommandDestinationEdit
// ----------------------------------------------------------------------------
//
void CDestDlg::OnCommandDestinationEditL()
    {
    LOGGER_ENTERFN( "CDestDlg::OnCommandDestinationEditL" );

    // Check if memory full
    TBool ret ( EFalse );
    ret = iCmManagerImpl->IsMemoryLow();
    if( ret )
        {
        CLOG_WRITE( "CDestDlg::OnCommandDestinationEditL: Memory Full, return!" );
        return;
        }

    if ( iListbox->CurrentItemUid() == KDestItemNewConnection )
        {
        // If there's at least one uprotected destination available
        if ( iCmManagerImpl->DestinationCountL() )
            {
            if ( iCmManagerImpl->HasUnprotectedDestinationsL() )
                {
                // Opens a Connection method creation wizard
                CLOG_WRITE( "CDestDlg::OnCommandDestinationEditL iCmWizard constructing" );
                iCmWizard = new (ELeave) CCmWizard( *iCmManagerImpl );
                TInt ret( KErrNone );
                TRAPD( err, ret = iCmWizard->CreateConnectionMethodL() );
                if ( err == KErrDiskFull )
                    {
                    CTextResolver* iTextResolver = CTextResolver::NewLC(*iCoeEnv); 
                    ret = KErrCancel;
                    TPtrC buf;
                    buf.Set(iTextResolver->ResolveErrorString(err)); 
                    TCmCommonUi::ShowNoteL( buf, TCmCommonUi::ECmErrorNote );
                    CleanupStack::PopAndDestroy( iTextResolver );
                    }
                else if ( err )
                    {
                    TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                            TCmCommonUi::ECmErrorNote );
                    ret = KErrCancel;
                    }
                delete iCmWizard;
                iCmWizard = NULL;
                CLOG_WRITE_1( "CDestDlg::iCmWizard ret %d", ret );

                // If application wanted to be closed before then RunAppShutter()
                // should be called here                
                if( iEscapeArrived )
                    {
                    CLOG_WRITE_1( "CDestDlg::iEscapeArrived %d", iEscapeArrived );
                    ( ( CAknAppUi* )iEikonEnv->EikAppUi() )->RunAppShutter();
                    }
                else
                    {
                    if( ret != KErrCancel )
                        {
                        TRAP( err, HandleListboxDataChangeL() );
                        if ( err )
                            {
                            TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                                    TCmCommonUi::ECmErrorNote );
                            return;
                            }
                        }
                    }

                }
            else
                {
                // Show a warning here
                TCmCommonUi::ShowNoteL( R_CMWIZARD_NO_UNPROTECTED_DESTINATION,
                                        TCmCommonUi::ECmWarningNote );
                }
            }
        else
            {
            // ... otherwise show a warning.
            TCmCommonUi::ShowNoteL( R_CMWIZARD_NO_DESTINATION,
                                    TCmCommonUi::ECmWarningNote );
            }
        }
    else
        {
        if ( iListbox->CurrentItemUid() == KDestItemUncategorized )                
            {
            CUncatDlg* uncatDlg = CUncatDlg::NewL( iCmManagerImpl );

            TUint32 selected = 0;
            if ( uncatDlg->ConstructAndRunLD( 0, selected ) == 
                 KDialogUserExit)
                {
                TryExitL(ETrue);
                }
            else
                {
                HandleListboxDataChangeL();                
                }
            }
        else
            {
            CCmDlg* cmDlg = CCmDlg::NewL( iCmManagerImpl , 
                iListbox->CurrentItemUid(), this );

            TUint32 selected = 0;
            if ( cmDlg->ConstructAndRunLD( 0, selected ) == 
                 KDialogUserExit )
                {
                TryExitL(ETrue);
                }
            else
                {
                HandleListboxDataChangeL();                
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// CDestDlg::OnCommandDestinationDelete
// ----------------------------------------------------------------------------
//
void CDestDlg::OnCommandDestinationDeleteL()
    {
    LOGGER_ENTERFN( "CDestDlg::OnCommandDestinationDeleteL" );

    TCmDefConnValue oldConn;
    if (iCmManagerImpl->IsDefConnSupported())
        {    
        iCmManagerImpl->ReadDefConnL( oldConn );
        }
    TUint32 listItemUid = iListbox->CurrentItemUid();
    
    if ( listItemUid == KDestItemNewConnection )
        {
        // ignore this command - it's not a destination
        return;
        }            
    
    if ( listItemUid == KDestItemUncategorized )
        {
        // Cannot delete Uncategoried Destination
        TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                                TCmCommonUi::ECmErrorNote );
                                
        return;
        }

    TBool carryOn( ETrue );
    // Cannot delete if the destination is embedded somewhere                                
    CCmDestinationImpl* dest = 
         iCmManagerImpl->DestinationL( iListbox->CurrentItemUid() );
    CleanupStack::PushL( dest );
    // We need this trap only because this function is called from a trap
    TRAPD( err, dest->CheckIfEmbeddedL( dest->Id() ) );
    if ( err == KErrNotSupported )
        {
        CleanupStack::PopAndDestroy( dest );
        TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                                TCmCommonUi::ECmErrorNote );
        return;
        }
    else if ( err )
        {
        CleanupStack::PopAndDestroy( dest );
        User::Leave(err);
        }
    
    // Show an error message to the user
    if ( dest->ProtectionLevel() )
        {
        TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                                TCmCommonUi::ECmErrorNote );
        carryOn = EFalse;
        }
        
    if( carryOn )
        {
        if( dest->IsConnectedL() )
            {
            TCmCommonUi::ShowNoteL( 
                       R_CMMANAGERUI_INFO_DEST_IN_USE_CANNOT_DELETE,
                       TCmCommonUi::ECmErrorNote );
            carryOn = EFalse;
            }
        }
    
    // check if it is linked by anybody
    
    if ( carryOn )
        {
        // for each IAP in CM manager
        //   1. check if it is virtual
        //      if not => goto 1.
        //      if yes:
        //      2. check if it links to the destination to be deleted
        //         if yes => carryOn = EFalse, ERROR
        //         if not:
        //         3. check if it links to any of the CMs in this destination
        //            if not => goto 1.
        //            if yes:
        //            4. check if it is also in this destination
        //               if not => carryOn = EFalse, ERROR
        //               if yes => goto 1.
        // As we need different notes in different cases:
        TUint32 noteresId = R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED;
        CommsDat::CMDBRecordSet<CommsDat::CCDIAPRecord>* iaps = 
                                                    iCmManagerImpl->AllIapsL();
        
        CleanupStack::PushL( iaps );
        
        // for each IAP in CM manager
        for ( TInt i = 0; carryOn && i < iaps->iRecords.Count(); ++i )
            {
            CommsDat::CCDIAPRecord* rec = (*iaps)[i];
            TUint32 bearerType = 0;
            
            TRAP_IGNORE( bearerType = 
                             iCmManagerImpl->BearerTypeFromIapRecordL( rec ) );
            if ( !bearerType )
                {
                continue;
                }
                
            // check if it is virtual
            if ( iCmManagerImpl->GetBearerInfoBoolL( bearerType, ECmVirtual ) )
                {
                // check if it links to the destination to be deleted
                CCmPluginBase* plugin = NULL;            
                TRAP_IGNORE( plugin = iCmManagerImpl->GetConnectionMethodL( 
                                                           rec->RecordId() ) );

                if ( !plugin )
                    {
                    continue;
                    }
                
                CleanupStack::PushL( plugin );

                if ( plugin->IsLinkedToSnap( dest->Id() ) )
                    {
                    // the CM links to this destination, deletion not allowed
                    carryOn = EFalse;
                    noteresId = R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED;
                    }
                else
                    {
                    
                    // check if the CM links to any of the CMs in this destination
                    for ( TInt j = 0; j < dest->ConnectionMethodCount(); ++j )
                        {
                        CCmPluginBase* destPlugin = NULL;
                        
                        TRAP_IGNORE( destPlugin = dest->GetConnectionMethodL( j ) );
                        if ( !destPlugin )
                            {
                            continue;
                            }
                        
                        if ( destPlugin->GetIntAttributeL( ECmId ) == 
                                                               rec->RecordId() )
                            {
                            // the CM shouldn't be compared to itself
                            continue;
                            }
                            
                        if ( plugin->IsLinkedToIap( 
                                    destPlugin->GetIntAttributeL( ECmId ) ) )
                            {
                            // the CM links to at least one CM in this destination
                            carryOn = EFalse; 
                            noteresId = R_QTN_NETW_CONSET_INFO_CANNOT_DELETE_VIRTUAL_REF;
                            break;
                            }
                        }

                    }

                CleanupStack::PopAndDestroy( plugin );                    
                }
                
                    
            }
            
        CleanupStack::PopAndDestroy( iaps );

        if ( !carryOn )
            {
            TCmCommonUi::ShowNoteL(
                       noteresId,
                       TCmCommonUi::ECmErrorNote );
            }
        }
    
        
    // check if it has protected method
    if( carryOn )
        {
        if( IsThereProtectedMethodL( dest ) )
            {
            TCmCommonUi::ShowNoteL( 
                       R_QTN_NETW_CONSET_INFO_CANNOT_DELETE_DEST_PROT_CM,
                       TCmCommonUi::ECmErrorNote );
            carryOn = EFalse;
            }
        }
        
    if( carryOn )
        {
        HBufC* destName = dest->NameLC(); // 2
        if ( TCmCommonUi::ShowConfirmationQueryL(
                              R_CMMANAGERUI_QUEST_DEST_DELETE, *destName ) )
            {
            CleanupStack::PopAndDestroy( destName );   
            TRAPD( err, dest->DeleteLD() );
            switch ( err )
                {
                case KErrInUse:
                    {
                    TCmCommonUi::ShowNoteL( 
                               R_CMMANAGERUI_INFO_DEST_IN_USE_CANNOT_DELETE,
                               TCmCommonUi::ECmErrorNote );
                    }
                    break;

                case KErrLocked:
                    {
                    TCmCommonUi::ShowNoteL
                        ( R_QTN_NETW_CONSET_INFO_CANNOT_DELETE_VIRTUAL_REF,
                          TCmCommonUi::ECmErrorNote );
                    }
                    break;
                    
                case KErrNone:
                    {
                    if (iCmManagerImpl->IsDefConnSupported())
                        {         
                        ShowDefaultConnectionNoteL(oldConn);
                        } 
                    CleanupStack::Pop( dest ); 
                    dest = NULL;  
                    
                    TInt selected = iListbox->CurrentItemIndex();

                    HandleListboxDataChangeL();

                    if ( iListbox->Model()->NumberOfItems() )
                        {
                        if( selected == iListbox->Model()->NumberOfItems() )
                            {
                            --selected;
                            }

                        //first item cannot be deleted
                        iListbox->ScrollToMakeItemVisible( selected);
                        iListbox->SetCurrentItemIndexAndDraw( selected );                            
                        }
                    }
                    break;
                    
                default:
                    {
                    }
                }                          
            }
        else
            {
            CleanupStack::PopAndDestroy( destName );   
            }                          
        }

    if( dest )
        {
        CleanupStack::PopAndDestroy( dest );
        }

    }

   
// ----------------------------------------------------------------------------
// CDestDlg::OnCommandDestinationRenameL
// ----------------------------------------------------------------------------
//
void CDestDlg::OnCommandDestinationRenameL()
    {
    LOGGER_ENTERFN( "CDestDlg::OnCommandDestinationRenameL" );

    // Check if this is UnCat
    if ( iListbox->CurrentItemUid() == KDestItemUncategorized )
        {
        TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                                TCmCommonUi::ECmErrorNote );
        return;
        }
    
    // Get currently selected destination.
    CCmDestinationImpl* dest =
          iCmManagerImpl->DestinationL( iListbox->CurrentItemUid() );
    CleanupStack::PushL( dest );

    // Show an error message to the user
    if ( dest->ProtectionLevel() )
        {
        TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                                TCmCommonUi::ECmErrorNote );

        CleanupStack::PopAndDestroy( dest );
        return;
        }

    // Allocate max length for the buffer, since we don't know how long
    // the new name will be. Copy the actual name of the destination
    // into the buffer.
    TBuf< KDestinationNameMaxLength > destName;
    destName.Copy( *( dest->NameLC() ) );
    
    TBool okToRename = EFalse;
    TBool nameAccepted = ETrue;
    do 
        {
        okToRename = TCmCommonUi::ShowConfirmationQueryWithInputL(
                                           R_CMMANAGERUI_PRMPT_DESTINATION_NAME,
                                           destName );
        if ( okToRename )
            {
            TRAPD(err, dest->SetNameL( destName ));
            if ( err == KErrAlreadyExists )
                {
                nameAccepted = EFalse;
                TCmCommonUi::ShowNoteL( R_CMMANAGERUI_INFO_DEST_ALREADY_IN_USE,
                                        destName,
                                        TCmCommonUi::ECmErrorNote );                        
                }
             else if ( err == KErrNone )
                {
                nameAccepted = ETrue;
                dest->UpdateL();
                HandleListboxDataChangeL();
                }
            }
        } while ( !nameAccepted && okToRename );

    // dest->NameLC()
    CleanupStack::PopAndDestroy( );
    CleanupStack::PopAndDestroy( dest );
    }

// ----------------------------------------------------------------------------
// CDestDlg::IsThereProtectedMethodL
// ----------------------------------------------------------------------------
//
TBool CDestDlg::IsThereProtectedMethodL( CCmDestinationImpl* aDest )
    {
    LOGGER_ENTERFN( "CDestDlg::IsThereProtectedMethodL" );

    TInt count( aDest->ConnectionMethodCount() );

    for( TInt i = 0; i < count; ++i )
        {
        CCmPluginBase* cm = aDest->GetConnectionMethodL( i );
        
        if( cm->GetBoolAttributeL( ECmProtected ) )
            {
            return ETrue;
            }
        }
        
    return EFalse;
    }


// ----------------------------------------------------------------------------
// CDestDlg::SetMskL
// ----------------------------------------------------------------------------
//
void CDestDlg::SetMskL( TInt aResourceId )
    {
    HBufC* text = StringLoader::LoadLC( aResourceId );
    ButtonGroupContainer().SetCommandL( EAknSoftkeySelect, *text );
    ButtonGroupContainer().DrawDeferred();
    CleanupStack::PopAndDestroy( text );
                
    }

// --------------------------------------------------------------------------
// CDestDlg::GetHelpContext
// --------------------------------------------------------------------------
//
void CDestDlg::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KHelpUidPlugin;
    aContext.iContext = KSET_HLP_CONN_DEST_VIEW;
    }

// --------------------------------------------------------------------------
// CDestDlg::Execute
// --------------------------------------------------------------------------
//
void CDestDlg::Execute()
    {
    LOGGER_ENTERFN( "CDestDlg::Execute" );
    iProcessing = ETrue;
    
    TRAPD( err, ProcessCommandL(ECmManagerUiCmdDestEdit) );
    if ( err )
        {
        HandleLeaveError( err );
        }

    iProcessing = EFalse;
    }

// --------------------------------------------------------------------------
// CDestDlg::HandleResourceChange
// --------------------------------------------------------------------------
//

void CDestDlg::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType ); 

    if( aType == KEikDynamicLayoutVariantSwitch )
        {
        DrawNow(); 
        }
        
    CAknDialog::HandleResourceChange(aType);        
    }

// --------------------------------------------------------------------------
// CDestDlg::HandleLeaveError
// --------------------------------------------------------------------------
//
void CDestDlg::HandleLeaveError( TInt aError )
    {
    LOGGER_ENTERFN( "CDestDlg::HandleLeaveError" );

    switch( aError )
        {
            case KLeaveWithoutAlert:
            case KErrNoMemory:
                EnforcedResetCDestDlg();
                break;
            default:
                // More leave errors may be handled if necessary
                // Now other leave errors are ignored here
                break;
        }
    }

// --------------------------------------------------------------------------
// CDestDlg::EnforcedResetCDestDlg
// --------------------------------------------------------------------------
//
void CDestDlg::EnforcedResetCDestDlg()
    {
    LOGGER_ENTERFN( "CDestDlg::EnforcedResetCDestDlg" );

    if( iCmWizard != NULL )
        {
        delete iCmWizard;
        iCmWizard = NULL;
        }

    if( iProcessing )
        {
        iProcessing = EFalse;
        }
}

// --------------------------------------------------------------------------
// CDestDlg::ShowDefaultConnectionNoteL
// --------------------------------------------------------------------------
//
void CDestDlg::ShowDefaultConnectionNoteL(TCmDefConnValue aOldDefConn)
    {
    LOGGER_ENTERFN( "CDestDlg::ShowDefaultConnectionNoteL" );
    TCmDefConnValue newConn;
        
    iCmManagerImpl->ReadDefConnL( newConn );

    if ( newConn != aOldDefConn )
        {
        ShowDefaultSetNoteL( newConn );
        }
    }

// --------------------------------------------------------------------------
// CDestDlg::CommsDatChanges
// --------------------------------------------------------------------------
//
void CDestDlg::CommsDatChangesL()
    {
    // Update list box
    HandleListboxDataChangeL();
    }
