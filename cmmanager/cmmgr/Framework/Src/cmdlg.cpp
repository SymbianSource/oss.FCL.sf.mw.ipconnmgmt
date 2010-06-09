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
* Description:  Implementation of CCmDlg
*
*/

#include <avkon.hrh>
#include <eikdef.h>
#include <eikenv.h>
#include <eikedwin.h>
#include <cmmanager.rsg>
#include <data_caging_path_literals.hrh>
#include <aknnavide.h>
#include <akntitle.h>
#include <eikmenup.h>
#include <StringLoader.h>
#include <AknIconArray.h>
#include <aknnavilabel.h>
#include <cmmanager.mbg>
#include <cmpluginembdestinationdef.h>
#include <featmgr.h>

#include <hlplch.h>
#include <csxhelp/cp.hlp.hrh>

#include "cmdlg.h"
#include "cmmanager.hrh"
#include "cmlistitem.h"
#include "cmlistitemlist.h"
#include <cmpluginbaseeng.h>
#include <cmpluginbase.h>
#include "cmdestinationimpl.h"
#include <cmcommonui.h>
#include <cmcommonconstants.h>
#include <cmpbasesettingsdlg.h>
#include "cmwizard.h"
#include "selectdestinationdlg.h"
#include <cmmanagerdef.h>
#include <mcmdexec.h>
#include "ccmdexec.h"
#include "destdlg.h"
#include "cmlogger.h"
#include "cmconnsettingsuiimpl.h"

using namespace CMManager;
using namespace CommsDat;

// ---------------------------------------------------------------------------
// CCmDlg::ConstructAndRunLD
// Constructs the dialog and runs it.
// ---------------------------------------------------------------------------
//
TInt CCmDlg::ConstructAndRunLD(  TUint32 aHighlight,
                                          TUint32& aSelected )
    {
    CleanupStack::PushL( this );

    iHighlight = aHighlight;
    iSelected = &aSelected;
    
    ConstructL( R_CM_MENUBAR );
    PrepareLC( R_CM_DIALOG );
    CleanupStack::Pop( this );

    iModel = new( ELeave )CCmListboxModel();
    
    iCmDestinationImpl = iCmManager->DestinationL( iDestUid );
    
    iInfoPopupNoteController = CAknInfoPopupNoteController::NewL();
    
    // Trace changes in CommsDat
    iCmManager->WatcherRegisterL( this );
        
    return RunLD();
    }
    
// ---------------------------------------------------------------------------
// CCmDlg::NewL()
// Two-phase dconstructor, second phase is ConstructAndRunLD
// ---------------------------------------------------------------------------
//
CCmDlg* CCmDlg::NewL(CCmManagerImpl* aCmManager, TUint32 aDestUid,
                     CDestDlg* aDestDlg)
    {
    CCmDlg* self = new (ELeave) CCmDlg( aCmManager, aDestUid, aDestDlg );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CCmDlg::CCmDlg()
// ---------------------------------------------------------------------------
//
CCmDlg::CCmDlg( CCmManagerImpl* aCmManager, TUint32 aDestUid,
                CDestDlg* aDestDlg )
    : iModelPassed( EFalse )
    , iCmManager( aCmManager )
    , iDestUid ( aDestUid )
    , iPrioritising ( EFalse )
    , iExitReason( KDialogUserExit )
    , iEscapeArrived( EFalse )
    , iExitduringProcessing( EFalse )
    , iBackduringProcessing( EFalse )
    , iDestDlg( aDestDlg )
    , iPluginExitExceptionally( EFalse )
    {
    }
    
// ---------------------------------------------------------------------------
// CCmDlg::~CCmDlg
// Destructor
// ---------------------------------------------------------------------------
//
CCmDlg::~CCmDlg()
    {
    // iModel is deleted by the ListBox because LB owns the model...
    // of course only after we passed ownership...
    if ( !iModelPassed )
        {
        delete iModel;
        }
        
    delete iNaviDecorator;
    
    if ( iTitlePane )
        {
        // set old text back, if we have it...
        if ( iOldTitleText )
            {
            if ( iExitReason == KDialogUserBack || iPluginExitExceptionally  )
                {
                TRAP_IGNORE( iTitlePane->SetTextL( *iOldTitleText ) );
                }
            delete iOldTitleText; iOldTitleText = NULL;
            }
        }    
        
    delete iOldTitleText;
    
    if(iCmDestinationImpl)
        {
        delete iCmDestinationImpl;
        iCmDestinationImpl = NULL;
        }    
    
    if ( iInfoPopupNoteController )
        {
        delete iInfoPopupNoteController;
        }
        
    if( iCmWizard )
        {
        delete iCmWizard;
        }
    iItemIndex.Close();
    
    delete iCmdExec;
    iCmdExec = NULL;
    }

// ---------------------------------------------------------------------------
// CCmDlg::PreLayoutDynInitL();
// called by framework before dialog is shown
// ---------------------------------------------------------------------------
//
void CCmDlg::PreLayoutDynInitL()
    {
    iListbox = STATIC_CAST( CCmListbox*, Control( KCmListboxId ) );
    iListbox->CreateScrollBarFrameL( ETrue );
    iListbox->ScrollBarFrame()->SetScrollBarVisibilityL
                        ( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    iListbox->SetListBoxObserver( this );

    iListbox->Model()->SetItemTextArray( iModel );
    iModelPassed = ETrue;
    InitTextsL();
    HandleListboxDataChangeL();
    iListbox->HandleItemAdditionL();
    
    if ( iHighlight )
        {
        iListbox->ScrollToMakeItemVisible( iHighlight );
        iListbox->SetCurrentItemIndex( iHighlight );
        }    
    }
    
// --------------------------------------------------------------------------
// CCmDlg::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CCmDlg::DynInitMenuPaneL( TInt aResourceId,
                                        CEikMenuPane* aMenuPane )
    {
    CAknDialog::DynInitMenuPaneL( aResourceId, aMenuPane );        
    
    if ( aResourceId == R_CM_MENU )
        {
    	if ( !iCmManager->IsHelpOn() )
            {
            aMenuPane->DeleteMenuItem( EAknCmdHelp );		    
            }
        // Menu items
    	TBool hideEdit      ( EFalse );
        TBool hideAdd       ( EFalse );
        TBool hideRename    ( EFalse );
        TBool hideDelete    ( EFalse );
        TBool hideCopy      ( EFalse );
        TBool hideMove      ( EFalse );
        TBool hidePrioritise( EFalse );
           
        // Edit, Rename, Delete and Organise menu items are hidden if:
        // 1. CM List is empty
        // 2. Embedded Destinations
        if ( !( iListbox->Model()->ItemTextArray()->MdcaCount() ) )
            {
            hidePrioritise = ETrue;
            hideEdit = ETrue;
            hideRename  = ETrue;
            hideDelete  = ETrue;
            hideCopy = ETrue;
            hideMove = ETrue;
            }
        // Embedded destinations can be deleted but not edited
        else if ( ( CurrentCML()->GetBoolAttributeL(ECmDestination) ) )
            {
            hideRename  = ETrue;
            }
        // Only one CM in the list OR highlighted CM is a virtual CM
        else if ( ( iListbox->Model()->ItemTextArray()->MdcaCount() == 1 ) )
            {
            hidePrioritise = ETrue;
            }            
        // virtual CMs pointing to destinations
        else if ( CurrentCML()->GetBoolAttributeL( ECmVirtual ) )
            {
            if ( CurrentCML()->GetIntAttributeL( ECmNextLayerSNAPId ) )
                {
                hidePrioritise = ETrue;
                }
            }
 
        // All item specific options need to be dimmed if in prioritising view
        if ( iPrioritising )
            {
            hideEdit = ETrue;
            hideAdd = ETrue;
            hideRename = ETrue;
            hideDelete = ETrue;
            hideCopy = ETrue;
            hideMove = ETrue;
            hidePrioritise = ETrue;           
            }

        // Show or hide menu items
        aMenuPane->SetItemDimmed( ECmManagerUiCmdCmEdit,    hideEdit );
        aMenuPane->SetItemDimmed( ECmManagerUiCmdCmAdd,     hideAdd );
        aMenuPane->SetItemDimmed( ECmManagerUiCmdCmRename,  hideRename );            
        aMenuPane->SetItemDimmed( ECmManagerUiCmdCmDelete,  hideDelete );    
        aMenuPane->SetItemDimmed( ECmManagerUiCmdCmPrioritise, hidePrioritise );
        aMenuPane->SetItemDimmed( ECmManagerUiCmdCmMoveToOtherDestination, hideMove );
        aMenuPane->SetItemDimmed( ECmManagerUiCmdCmCopyToOtherDestination, hideCopy ); 
        }   
    }
    
// ---------------------------------------------------------------------------
// CCmDlg::InitTextsL
// called before the dialog is shown
// to initialize localized textual data
// ---------------------------------------------------------------------------
//
void CCmDlg::InitTextsL()
    {
    // set pane text if neccessary...
    // pane text needed if not pop-up...
    HBufC* primary = 
              iEikonEnv->AllocReadResourceLC( R_CMMANAGERUI_EMPTY_METHOD_VIEW_PRIMARY );
    HBufC* secondary = 
              iEikonEnv->AllocReadResourceLC( R_CMMANAGERUI_EMPTY_METHOD_VIEW_SECONDARY );
    CDesCArrayFlat* items = new (ELeave) CDesCArrayFlat(2);
    CleanupStack::PushL(items);
    items->AppendL(primary->Des()); 
    items->AppendL(secondary->Des());      
    HBufC* emptyText = 
          StringLoader::LoadLC( R_TWO_STRING_FOR_EMPTY_VIEW , *items);              
    iListbox->View()->SetListEmptyTextL( *emptyText );    
    CleanupStack::PopAndDestroy( emptyText );
    CleanupStack::PopAndDestroy( items );
    CleanupStack::PopAndDestroy( secondary );
    CleanupStack::PopAndDestroy( primary );

    iStatusPane = iEikonEnv->AppUiFactory()->StatusPane();
    iTitlePane = ( CAknTitlePane* )iStatusPane->ControlL(
                                          TUid::Uid( EEikStatusPaneUidTitle ) );

    iOldTitleText = iTitlePane->Text()->AllocL();
    HBufC* name = iCmDestinationImpl->NameLC();
    iTitlePane->SetTextL( *name );
    CleanupStack::PopAndDestroy( name );

    iNaviPane = ( CAknNavigationControlContainer* ) 
                    iStatusPane->ControlL( 
                            TUid::Uid( EEikStatusPaneUidNavi ) );
    iNaviDecorator = iNaviPane->CreateNavigationLabelL( KNullDesC );
    iNaviPane->PushL( *iNaviDecorator );
    }

// ---------------------------------------------------------------------------
// CCmDlg::SetNoOfCMsL
// ---------------------------------------------------------------------------
//

void CCmDlg::SetNoOfCMsL(TInt aCount)
    {
    if ( iNaviDecorator )
        {
        if ( iNaviDecorator->ControlType() == 
             CAknNavigationDecorator::ENaviLabel)
            {
            CAknNaviLabel* naviLabel = 
                            (CAknNaviLabel*)iNaviDecorator->DecoratedControl();
            if (aCount)
                {
                HBufC* title = NULL;
                
                if (aCount == 1)
                    {
                    title = StringLoader::LoadLC( 
                                           R_CMMANAGERUI_NAVI_ONE_CONN_METHOD );
                    }
                else
                    {
                    title = StringLoader::LoadLC( 
                                           R_CMMANAGERUI_NAVI_MANY_CONN_METHODS, 
                                           aCount );
                    }
                    
                naviLabel->SetTextL(*title);
                  CleanupStack::PopAndDestroy( title );
                }
            else
                {
                naviLabel->SetTextL(KNullDesC);
                }
                
            iNaviPane->PushL( *iNaviDecorator );
            }
        }
      
    }

//----------------------------------------------------------
// CCmDlg::CreateCustomControlL
//----------------------------------------------------------
//
SEikControlInfo CCmDlg::CreateCustomControlL( TInt aControlType )
    {
    SEikControlInfo controlInfo;
    controlInfo.iTrailerTextId = 0;
    controlInfo.iFlags = 0;
    if ( aControlType == KCmListboxType )
        {
        controlInfo.iControl = new ( ELeave ) CCmListbox;
        }
    else
        {
        controlInfo.iControl = NULL;
        }
    return controlInfo;
    }

// ---------------------------------------------------------------------------
// CCmDlg::OkToExitL
// Good to know : EAknSoftkeyCancel is never called, because
// EEikDialogFlagNotifyEsc flag is not set in the resource.
// ---------------------------------------------------------------------------
//
TBool CCmDlg::OkToExitL( TInt aButtonId )
    {
    LOGGER_ENTERFN( "CCmDlg::OkToExitL" );

    // Translate the button presses into commands for the appui & current
    // view to handle
    // Dialog should not exit
    TBool retVal( EFalse );
    
    switch ( aButtonId )
        {
        case EAknSoftkeyOk:
        case ECmManagerUiCmdCmEdit:
            {
            if ( !iProcessing )
                {
                if ( CMCount() )
                    {
                    iProcessing = ETrue;
                    ProcessCommandL( aButtonId );
                    iProcessing = EFalse;
                    }
                else
                    {
                    iProcessing = ETrue;
                    AddConnectionMethodL();
                    iProcessing = EFalse;
                    }
                }
            break;
            }
        case EAknSoftkeyClear:
            {
            ProcessCommandL( aButtonId );
            break;
            }
        case ECmManagerUiCmdCmPrioOk:
            {
            if ( iPrioritising )
                {
                FinishPriorizingL( ETrue );
                }
                
            break;
            }               
       case ECmManagerUiCmdCmPrioCancel:            
            {
            if ( !MenuShowing() )            
                {
                if ( iPrioritising )
                    {
                    FinishPriorizingL( EFalse );
                    }
                }            
            break;
            }           
        case ECmManagerUiCmdCmUserExit:
            {
            iCmManager->WatcherUnRegister();
                        
            iExitReason = KDialogUserExit;
            break;
            }
        case EAknSoftkeyBack:
            {
            iCmManager->WatcherUnRegister();
            iExitReason = KDialogUserBack;
            
            if ( !iProcessing )
                {
                *iSelected = 0;
                TryExitL( iExitReason );
                }
            else
                {
                iBackduringProcessing = ETrue;
                }
            break;
            }
        default:
            {
            if ( !iProcessing )
                {
                if ( aButtonId == EAknSoftkeyOptions )
                    {
                    DisplayMenuL();
                    }
                else
                    {
                    retVal = ETrue;
                    }
                }
            if ( iProcessing )
                {
                iExitduringProcessing = ETrue;
                if( aButtonId == KDialogUserExit )
                    {
                    retVal = ETrue;
                    } 
                
                if ( iPluginExitExceptionally )
                    {
                    retVal = ETrue;
                    }
                }
            break;
            }
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmDlg::HandleListBoxEventL
// ---------------------------------------------------------------------------
//
void CCmDlg::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                  TListBoxEvent aEventType )
    {
    LOGGER_ENTERFN( "CCmDlg::HandleListBoxEventL" );

    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
            {
            if ( iPrioritising )
                {
                FinishPriorizingL( ETrue );
                }
            else
                {
                if ( !iProcessing )
                    {
                    delete iCmdExec;
                    iCmdExec = NULL;
                    iCmdExec = new (ELeave) CCmdExec(*this);
                    iCmdExec->Execute();
                    }
                }           
            break;            
            }
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
            };
        };
    }
       
    
// ---------------------------------------------------------------------------
// CCmDlg::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CCmDlg::ProcessCommandL( TInt aCommandId )
    {
    LOGGER_ENTERFN( "CCmDlg::ProcessCommandL" );

    if ( MenuShowing() )
        {
        HideMenu();
        }

    switch ( aCommandId )
        {        
        // Connection Method Modification Commands
        case ECmManagerUiCmdCmAdd:
            {
            if ( !iProcessing )
                {
                iProcessing = ETrue;
                TRAPD( err, AddConnectionMethodL() );
                // Memory full
                if (err == KLeaveWithoutAlert || err == KErrNoMemory)
                    {
                    iProcessing = EFalse;
                    return;
                    }
                iProcessing = EFalse;

                if ( iBackduringProcessing )
                    {
                    iBackduringProcessing = EFalse;
                    TryExitL( KDialogUserBack );
                    }
                }
            break;
            }

        case ECmManagerUiCmdCmRename:            
            {
            RenameConnectionMethodL();
            break;
            }
            
        case EAknSoftkeyClear:
        case ECmManagerUiCmdCmDelete:            
            {
            if ( iPrioritising )
                {
                // Delete cannot be done if priorising has started.
                break;
                }
            DeleteCurrentCmL();         
            break;
            }
            
        case ECmManagerUiCmdCmPrioritise:            
            {
            PrioritiseConnectionMethodL();
            break;
            }
            
        case ECmManagerUiCmdCmCopyToOtherDestination:            
            {
            CopyConnectionMethodL();            
            break;
            }
            
        case ECmManagerUiCmdCmMoveToOtherDestination:            
            {            
            MoveConnectionMethodL();
            break;
            }
            
        case EAknSoftkeyOk:
        case ECmManagerUiCmdCmEdit:
            {
            EditConnectionMethodL();
            break;
            }   

        // Button Selection commands
        case EAknSoftkeyOptions:
            {
            DisplayMenuL();
            break;
            }
            
        case ECmManagerUiCmdCmUserExit:
            {
            if (!iProcessing)
                {
                *iSelected = 0;
                iExitReason = KDialogUserExit;
                TryExitL( iExitReason );
                }
            break;
            }
            
        case EAknSoftkeyBack:
            {
            if (!iProcessing)
                {
                *iSelected = 0;
                iExitReason = KDialogUserBack;
                TryExitL( iExitReason );
                }
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
// CCmDlg::DeleteCurrentCmL
// ---------------------------------------------------------------------------
//
void CCmDlg::DeleteCurrentCmL( )
    {
    LOGGER_ENTERFN( "CCmDlg::DeleteCurrentCmL" );

    TCmDefConnValue oldConn;
    if (iCmManager->IsDefConnSupported())
        {
        iCmManager->ReadDefConnL( oldConn );
        }
    CCmPluginBase* cm = CurrentCML();
    if ( !cm )
    	{
    	return;
    	}

    if ( cm->GetBoolAttributeL( ECmProtected ) )
        {
        TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                                TCmCommonUi::ECmErrorNote );
                                
        return;
        }
   
    if ( cm->GetBoolAttributeL( ECmIsLinked ) )//same check as KErrLocked below
        {
        TCmCommonUi::ShowNoteL( 
                            R_QTN_NETW_CONSET_INFO_CANNOT_DELETE_VIRTUAL_REF,
                            TCmCommonUi::ECmErrorNote );
        
        return;
        }
   
    if ( cm->GetBoolAttributeL( ECmConnected ) )//same check as KErrInUse below
        {
        TCmCommonUi::ShowNoteL( R_CMMANAGERUI_INFO_CM_IN_USE_CANNOT_DELETE,
                                TCmCommonUi::ECmErrorNote );
        
        return;
        }

    // EKOA-74KC3L: check if it is the last CM in a destination being referenced
    // by a VPN
    if ( iCmDestinationImpl &&
         iCmDestinationImpl->ConnectionMethodCount() == 1 )
        {
        // it is definitely the last, but is it referenced?

        // for each IAP in CM manager
        //   1. check if it is virtual
        //      if not => goto 1.
        //      if yes:
        //      2. check if it links to the destination of this CM
        //         if yes => carryOn = EFalse, ERROR
        //         if not: carryOn = ETrue

        CommsDat::CMDBRecordSet<CommsDat::CCDIAPRecord>* iaps = 
                                                    iCmManager->AllIapsL();
        
        CleanupStack::PushL( iaps );
        
        TBool carryOn = ETrue;
        TUint32 destId = iCmDestinationImpl->Id();

        // for each IAP in CM manager
        for ( TInt i = 0; carryOn && i < iaps->iRecords.Count(); ++i )
            {
            CommsDat::CCDIAPRecord* rec = (*iaps)[i];
            TUint32 bearerType = 0;
            
            TRAP_IGNORE( bearerType = 
                             iCmManager->BearerTypeFromIapRecordL( rec ) );
            if ( !bearerType )
                {
                continue;
                }
                
            // check if it is virtual
            if ( iCmManager->GetBearerInfoBoolL( bearerType, ECmVirtual ) )
                {
                // check if it links to the current destination
                CCmPluginBase* plugin = NULL;            
                TRAP_IGNORE( plugin = iCmManager->GetConnectionMethodL( 
                                                           rec->RecordId() ) );

                if ( !plugin )
                    {
                    continue;
                    }
                
                if ( plugin->IsLinkedToSnap( destId ) )
                    {
                    // the CM links to this destination, deletion not allowed
                    carryOn = EFalse;
                    }

                delete plugin;
                }
                    
            }
            
        CleanupStack::PopAndDestroy( iaps );
        
        if ( !carryOn )
            {
            TCmCommonUi::ShowNoteL( R_QTN_NETW_CONSET_INFO_CANNOT_DELETE_VIRTUAL_REF,
                                TCmCommonUi::ECmErrorNote );
            return;            
            }
        
        }
    

    // If it's an embedded destination, show a warning
    TBool showCmDeleteQuery( ETrue );
    if ( cm->GetBoolAttributeL( ECmDestination ) )
        {
        if ( TCmCommonUi::ShowConfirmationQueryL(
                            R_CMMANAGERUI_QUEST_EMBEDDED_DEST_DELETE ) )
            {
            // user wants to delete, set flag to avoid 2nd query message
            showCmDeleteQuery = EFalse;
            }
        else
            {
            return; // user cancelled delete request
            }
        }

    HBufC* cmName = cm->GetStringAttributeL( ECmName );
    CleanupStack::PushL( cmName );
    
    // Query for CM deletion - which will be skipped in the case of embedded 
    // destintations
    // Protected CM's cannot be deleted either
    if ( !showCmDeleteQuery || iAlreadyConfirmedDelete ||
         TCmCommonUi::ShowConfirmationQueryL( 
                                    R_CMMANAGERUI_QUEST_CM_DELETE,
                                    *cmName ) )
        {
        TInt err = KErrNone;
        if ( iCmDestinationImpl )
            {
            TRAP( err, iCmDestinationImpl->DeleteConnectionMethodL( *cm ) );
            }
        else
            {
            TRAP( err, (void)cm->DeleteL( ETrue ) );
            }
        
        switch ( err )
            {
            case KErrInUse: //shouldn't get here, already checked above
                {
                TCmCommonUi::ShowNoteL
                    ( R_CMMANAGERUI_INFO_CM_IN_USE_CANNOT_DELETE,
                    TCmCommonUi::ECmErrorNote );
                }
                break;
                
            case KErrLocked://shouldn't get here, already checked above
                {
                TCmCommonUi::ShowNoteL
                    ( R_QTN_NETW_CONSET_INFO_CANNOT_DELETE_VIRTUAL_REF,
                      TCmCommonUi::ECmErrorNote );
                }
                break;
                
            case KErrNone:
                {
                if ( iCmDestinationImpl )
                    {
                    iCmDestinationImpl->UpdateL();
                    }
                if (iCmManager->IsDefConnSupported())
                    {
                    iDestDlg->ShowDefaultConnectionNoteL( oldConn );
                    }
                HandleListboxDataChangeL();
                iListbox->HandleItemRemovalL();
                }
                break;

            default:
                {
                }
                break;
            }
        }

    CleanupStack::PopAndDestroy( cmName );
    }

// --------------------------------------------------------------------------
// CCmDlg::RenameConnectionMethodL    
// --------------------------------------------------------------------------
//
void CCmDlg::RenameConnectionMethodL()
    {
    LOGGER_ENTERFN( "CCmDlg::RenameConnectionMethodL" );

    CCmPluginBase* cm = CurrentCML();
    if ( !cm )
        {
        return;
        }

    if ( cm->GetBoolAttributeL( ECmProtected ) )
        {
        TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                                TCmCommonUi::ECmErrorNote );
        return;
        }

    if ( CurrentCmInUseL() )
        {
        return;
        } 
           
    TBuf<KConnMethodNameMaxLength> buf;
    
    HBufC* cmName = cm->GetStringAttributeL(ECmName);
    CleanupStack::PushL(cmName);
    buf.Copy(*cmName);
    
    // Saves the recent naming method and set it to "Not accept"
    TUint32 savedNM = cm->GetIntAttributeL( ECmNamingMethod );
    cm->SetIntAttributeL( ECmNamingMethod, ENamingNotAccept );

    // Asks the new name
    TBool go = ETrue;
    TInt ret = TCmCommonUi::ShowConfirmationQueryWithInputL(
                                                R_CMMANAGERUI_PRMPT_CM_MAME,
                                                buf );
    
    // While the name is not unique or cancel pressed
    while( ret && go )
        {
        // Tries to save the nem name. If it is not unique
        // then ret == KErrArgument
        TRAPD( err, cm->SetStringAttributeL( ECmName, buf ) );

        if( KErrArgument == err )
            {
            // Opens a info note
            HBufC* noteLine = StringLoader::LoadLC(
                    R_CMMANAGERUI_INFO_CM_ALREADY_IN_USE, buf );

            TCmCommonUi::ShowNoteL( *noteLine, TCmCommonUi::ECmInfoNote );

            CleanupStack::PopAndDestroy( noteLine );

            // Asks the new name again
            ret = TCmCommonUi::ShowConfirmationQueryWithInputL(
                                                R_CMMANAGERUI_PRMPT_CM_MAME,
                                                buf );
            }
        else
            {
            // New name is unique loop can stop
            go = EFalse;
            }    
        }

    // Sets back the original naming method
    cm->SetIntAttributeL( ECmNamingMethod, savedNM );

    // If not cancel pressed the new name is set
    if( ret )
        {
        cm->UpdateL();
        
        HandleListboxDataChangeL();
        }
   
    CleanupStack::PopAndDestroy( cmName );
    }

// --------------------------------------------------------------------------
// CCmDlg::PrioritiseConnectionMethodL    
// --------------------------------------------------------------------------
//
void CCmDlg::PrioritiseConnectionMethodL()
    {
    LOGGER_ENTERFN( "CCmDlg::PrioritiseConnectionMethodL" );
    
    ButtonGroupContainer().SetCommandSetL( R_SOFTKEYS_PRIO_OK_CANCEL__OK );
    iListbox->View()->SelectItemL( iListbox->CurrentItemIndex() );
    iListbox->View()->DrawItem( iListbox->CurrentItemIndex() );
    ButtonGroupContainer().DrawNow();
    iPrioritising = ETrue;
    iCmToPrioritise = iListbox->CurrentItemIndex();
    }

// --------------------------------------------------------------------------
// CCmDlg::AddConnectionMethodL    
// --------------------------------------------------------------------------
//
void CCmDlg::AddConnectionMethodL()
    {
    LOGGER_ENTERFN( "CCmDlg::AddConnectionMethodL" );

    // Check first if parent destination is protected
    if ( iCmDestinationImpl && 
         iCmDestinationImpl->ProtectionLevel() == CMManager::EProtLevel1 )
        {
        TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                                TCmCommonUi::ECmErrorNote );
        return;
        }

    // Opens a Connection method creation wizard
    iCmWizard = new (ELeave) CCmWizard( *iCmManager );

    TInt itemstemp = iListbox->Model()->NumberOfItems();

    TInt ret( KErrNone );
    TRAPD( err, ret = iCmWizard->CreateConnectionMethodL( iCmDestinationImpl ) );
    if ( err )
        {
        TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                TCmCommonUi::ECmErrorNote );
        ret = KErrCancel;
        }
    delete iCmWizard;
    iCmWizard = NULL;


    // If application wanted to be closed before then RunAppShutter()
    // should be called here                
    if( iEscapeArrived )
        {
        ( ( CAknAppUi* )iEikonEnv->EikAppUi() )->RunAppShutter();
        }
    
    if( ret != KErrCancel )
        {
        iHighlight = ret;


        // Update the middle soft key if this is the first cm added
        if ( CMCount() == 1 )
            {
            HBufC* text = StringLoader::LoadLC( R_QTN_MSK_EDIT );
            ButtonGroupContainer().SetCommandL( ECmManagerUiCmdCmEdit, 
                                                    *text );
            CleanupStack::PopAndDestroy( text );
            }
        
        //HandleListboxDataChangeL();
        TRAP( err, HandleListboxDataChangeL() );
        if ( err )
            {
            TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                    TCmCommonUi::ECmErrorNote );
            return;
            }
        iListbox->HandleItemAdditionL();
    
        if (itemstemp < iListbox->Model()->NumberOfItems())
            {
            //first item cannot be deleted
            iListbox->ScrollToMakeItemVisible( 
                                    iListbox->Model()->NumberOfItems() -1 );
            iListbox->SetCurrentItemIndex( iHighlight );
            }
        }
    }

// --------------------------------------------------------------------------
// CCmDlg::CopyConnectionMethodL    
// --------------------------------------------------------------------------
//
void CCmDlg::CopyConnectionMethodL()
    {
    LOGGER_ENTERFN( "CCmDlg::CopyConnectionMethodL" );

    CCmPluginBase* cm = CurrentCML();
    if ( !cm )
        {
        return;
        }
    
    if ( cm->GetBoolAttributeL( ECmProtected ) ||
        ( iCmDestinationImpl && 
         iCmDestinationImpl->ProtectionLevel() == CMManager::EProtLevel1 ) )
        {
        TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                                TCmCommonUi::ECmErrorNote );
        return;
        }

    TUint32 destId( 0 );
    
    // build an array of possible parent destinations
    TInt destCount=0;
    TRAPD( retVal, destCount = iCmManager->DestinationCountL() );
    if( retVal != KErrNone || destCount < 1 )
        {
        TCmCommonUi::ShowNoteL( R_CMWIZARD_NO_DESTINATION,
                                TCmCommonUi::ECmErrorNote );
        return;
        }
        
    RArray<TUint32> destArray( iCmManager->DestinationCountL() );
    CleanupClosePushL( destArray );
    
    iCmManager->AllDestinationsL( destArray );
    
    // drop current destination (don't ask why)
    if ( iCmDestinationImpl ) 
        {
        destId = iCmDestinationImpl->Id();
        
        TInt index = destArray.Find( destId );
        if ( index != KErrNotFound )
            {
            destArray.Remove( index );
            }
        }
    
    
    // now filter the destinations
    CurrentCML()->FilterPossibleParentsL( destArray );

    // check if there are any remaining destinations
    if ( !destArray.Count() )
        {
        TCmCommonUi::ShowNoteL( R_CMWIZARD_NO_VPN_STACKING,
                                TCmCommonUi::ECmErrorNote );        
        CleanupStack::PopAndDestroy( &destArray );
        return;
        }

    // create a copy of the current connection method
    CCmPluginBase* cmCopy = cm->CreateCopyL();
    CleanupStack::PushL( cmCopy );

    // ok, there is at least 1, pop up the dialog
    CSelectDestinationDlg* dlg = CSelectDestinationDlg::NewL( destId, 
                                                              *iCmManager, 
                                                              &destArray );

    if ( dlg->ExecuteLD( R_CMMANAGER_LIST_QUERY_WITH_MSGBOX ) )
        {
        CCmDestinationImpl* dest = iCmManager->DestinationL( destId );
        CleanupStack::PushL( dest ); 
        if ( iCmDestinationImpl )
            {
            // store it in commsdat
            cmCopy->UpdateL();
            dest->AddConnectionMethodL( *cmCopy );
            }
        else
            {
            iCmManager->CopyConnectionMethodL( *dest, *( cm ) );
            }

        // Rare but UpdateL can leave that cause crash
        TRAPD( err, dest->UpdateL() );
        if( err < 0 )
            {
            CLOG_WRITE_1( "CCmDlg::CopyConnectionMethodL UpdateL: %d", err );
            }
    
        CleanupStack::PopAndDestroy( dest );
        HandleListboxDataChangeL();                             
        }
    CleanupStack::PopAndDestroy( cmCopy );
    CleanupStack::PopAndDestroy( &destArray );
    }

// --------------------------------------------------------------------------
// CCmDlg::MoveConnectionMethodL
// --------------------------------------------------------------------------
//
void CCmDlg::MoveConnectionMethodL()
    {
    LOGGER_ENTERFN( "CCmDlg::MoveConnectionMethodL" );

    CCmPluginBase* cm = CurrentCML();
    if ( !cm )
        {
        return;
        }

    if ( cm->GetBoolAttributeL( ECmProtected ) ||
         iCmDestinationImpl->ProtectionLevel() == CMManager::EProtLevel1 )
        {
        TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                                TCmCommonUi::ECmErrorNote );
        return;
        }

    if ( CurrentCmInUseL() )
        {
        return;
        }

    // build an array of possible parent destinations
    RArray<TUint32> destArray( iCmManager->DestinationCountL() );
    CleanupClosePushL( destArray );
    
    iCmManager->AllDestinationsL( destArray );
    
    TUint32 destId( 0 );

    // first remove the current parent (makes no sense moving a CM to its
    // original destination)
    if ( iCmDestinationImpl ) 
        {
        destId = iCmDestinationImpl->Id();
        
        TInt index = destArray.Find( destId );
        if ( index != KErrNotFound )
            {
            destArray.Remove( index );
            }
        }
    
    // now filter the destinations
    CurrentCML()->FilterPossibleParentsL( destArray );

    // check if there are any remaining destinations
    if ( !destArray.Count() )
        {
        TCmCommonUi::ShowNoteL( R_CMWIZARD_NO_VPN_STACKING,
                                TCmCommonUi::ECmErrorNote );        
        CleanupStack::PopAndDestroy( &destArray );
        return;
        }

        
    CSelectDestinationDlg* dlg = CSelectDestinationDlg::NewL( destId, 
                                                              *iCmManager, 
                                                              &destArray );
                    
    if ( dlg->ExecuteLD( R_CMMANAGER_LIST_QUERY_WITH_MSGBOX ) )
        {
        CCmDestinationImpl* dest = iCmManager->DestinationL( destId );
        CleanupStack::PushL( dest );
        iCmManager->MoveConnectionMethodL( *iCmDestinationImpl,
                                            *dest,
                                            *( CurrentCML() ) );
        CleanupStack::PopAndDestroy( dest );
        iCmDestinationImpl->UpdateL();                            

        HandleListboxDataChangeL();
        iListbox->HandleItemRemovalL();
        }

    CleanupStack::PopAndDestroy( &destArray );
    }

// --------------------------------------------------------------------------
// CCmDlg::EditConnectionMethodL
// --------------------------------------------------------------------------
//
void CCmDlg::EditConnectionMethodL()
    {
    LOGGER_ENTERFN( "CCmDlg::EditConnectionMethodL" );

    CCmPluginBase* cm = NULL;
    
    TRAPD( err, cm = CurrentCML() );
    
    if( err == KErrNotSupported )
        {
        return;
        }
    else
        {
        User::LeaveIfError( err );
        }

    if ( !cm )
        {
        return;
        }

    // Must reload here in case another app changed the data (CurrentCML loads
    // only if not already loaded)   
    cm->ReLoadL();
    
    if ( cm->GetBoolAttributeL( ECmProtected ) ||
         cm->GetBoolAttributeL( ECmDestination ) )
        {
        TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                                TCmCommonUi::ECmErrorNote );        
        }
    else if ( cm->GetBoolAttributeL( ECmBearerHasUi ) )
        {
        // check if the cm is in use
        if ( !CurrentCmInUseL() )
            {
            TInt ret = cm->RunSettingsL();
            if ( ret == KDialogUserExit )
                {
                if( IsExceptionExitL() )
                    {
                    iPluginExitExceptionally = ETrue;
                    TryExitL( KDialogUserBack );
                    }
                else
                    {
                    TryExitL( KDialogUserExit );
                    }
                }
            else
                {
                if ( ret == KDialogUserDeleteConfirmed )
                    {
                    iAlreadyConfirmedDelete = ETrue;
                    DeleteCurrentCmL();
                    iAlreadyConfirmedDelete = EFalse;
                    }
                if ( ret == KDialogUserDelete )
                    {
                    DeleteCurrentCmL();
                    }
                
                // Reorder according to priorities, if needed
                ReOrderCMsL();
                
                HandleListboxDataChangeL();
                }
            }
        }
    }
    
// ---------------------------------------------------------------------------
// CCmDlg::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CCmDlg::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                     TEventCode aType )    
    {
    LOGGER_ENTERFN( "CCmDlg::OfferKeyEventL" );
    CLOG_WRITE_1("Key iCode: %d", aKeyEvent.iCode );

    TKeyResponse retVal ( EKeyWasNotConsumed );

    if ( aKeyEvent.iCode == EKeyBackspace )
        {
        // While priorising delete item is disabled
        if( !iPrioritising )
            {
            ProcessCommandL( ECmManagerUiCmdCmDelete );
            }
        retVal = EKeyWasConsumed;
        }
    else if ( aKeyEvent.iCode == EKeyEscape )
        {
        // Handling application close needs special care 
        // because of iCmWizard 
        if ( iCmWizard )
            {
            // if wizard alives then escape should do after wizard has ended.
            CLOG_WRITE( "CCmDlg::OfferKeyEventL: Wizard in long process" );
            iEscapeArrived = ETrue;
            retVal = EKeyWasConsumed;
            }
        else    
            {
            CLOG_WRITE( "CCmDlg::OfferKeyEventL:Escape" );
            retVal = CAknDialog::OfferKeyEventL( aKeyEvent, aType );
            }
        }
    else
        {
        retVal = iListbox->OfferKeyEventL(aKeyEvent, aType);       
        }

    return retVal;
    }
// ---------------------------------------------------------------------------
// CCmDlg::HandleInfoPopupNoteEvent
// called before the dialog is shown to initialize listbox data
// ---------------------------------------------------------------------------
//    
void CCmDlg::HandleInfoPopupNoteEvent(
                                CAknInfoPopupNoteController* /*aController*/,
                                TAknInfoPopupNoteEvent /*aEvent*/ )
    {
    }

// ---------------------------------------------------------------------------
// CCmDlg::CurrentCML
// Currently selected connection method in the list
// ---------------------------------------------------------------------------
//
CCmPluginBase* CCmDlg::CurrentCML()
    {
    if ( iItemIndex.Count() == 0 )
    	{
    	return NULL;
    	}
    return iCmDestinationImpl->GetConnectionMethodL( 
        iItemIndex[iListbox->CurrentItemIndex()]);
    }

// ---------------------------------------------------------------------------
// CCmDlg::CMByIndexL
// a connection method in the list
// ---------------------------------------------------------------------------
//
CCmPluginBase* CCmDlg::CMByIndexL( TInt aIndex )
    {
    return iCmDestinationImpl->GetConnectionMethodL( aIndex );
    }

// ---------------------------------------------------------------------------
// CCmDlg::CMCount
// number of cms in the list
// ---------------------------------------------------------------------------
//
TInt CCmDlg::CMCount()
    {
    return iCmDestinationImpl->ConnectionMethodCount();
    }
    
// ---------------------------------------------------------------------------
// CCmDlg::HandleListboxDataChangeL
// called before the dialog is shown to initialize listbox data
// ---------------------------------------------------------------------------
//
void CCmDlg::HandleListboxDataChangeL()
    {
    TInt selected = 0;
    if ( iListbox->CurrentItemIndex() )
        {
        selected = iListbox->CurrentItemIndex();
        }
    
    LOGGER_ENTERFN( "CCmDlg::HandleListboxDataChangeL" );
    iCmManager->OpenTransactionLC();

    RArray<TUint32> cmIds;
    ConstructCMArrayL( cmIds );
    CleanupClosePushL( cmIds );
    
    // Remove hidden Connection Methods from the array if uncat folder
    if ( !iCmDestinationImpl )
        {
        ClearHiddenCMsFromArrayL( cmIds );
        }
    
    /// Using test values for now.
    CArrayPtr< CGulIcon >* icons = new( ELeave ) CAknIconArray( KCMGranularity );
    CleanupStack::PushL( icons );
    iModel->ResetAndDestroy();
    
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();        
    TParse mbmFile;
    User::LeaveIfError( mbmFile.Set( KManagerIconFilename, 
                                     &KDC_APP_BITMAP_DIR, 
                                     NULL ) );        

    
    icons->AppendL( AknsUtils::CreateGulIconL( 
                              skinInstance, 
                              KAknsIIDQgnIndiSettProtectedAdd,
                              mbmFile.FullName(), 
                              EMbmCmmanagerQgn_indi_sett_protected_add, 
                              EMbmCmmanagerQgn_indi_sett_protected_add_mask ) );  

    //0..1 are reserved indexes!
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

    CGulIcon* icon = CGulIcon::NewL( bitmap, mask ); // Ownership transferred
    CleanupStack::Pop( 2 ); // bitmap, mask

    CleanupStack::PushL( icon );
    icons->AppendL( icon );
    CleanupStack::Pop( icon );

    RArray<TUint> bearerIdArray = RArray<TUint>( KCmArrayBigGranularity );
    CleanupClosePushL( bearerIdArray );    
    
    TCmDefConnValue sel;
    TInt defUid = KErrNotFound;
    if ( iCmManager->IsDefConnSupported() )
        {
        iCmManager->ReadDefConnL(sel);
        if ( sel.iType == EDCConnectionMethod )
            {
            defUid = sel.iId;
            }
        }
    iItemIndex.Reset();
    for ( TInt i = 0; i < cmIds.Count(); i++ )
        {
        TInt iconId = KErrNotFound;
        CCDAccessPointRecord* destAPRecord = NULL;
        CCDIAPRecord* iapRecord = NULL;
        HBufC* name = NULL;
        TBool isDest( cmIds[i] > KCmDefaultDestinationAPTagId );
        TUint recId( 0 );
        TBool protect;
        TUint32 bearerType( 0 );
        
        if( isDest )
            {
            destAPRecord = static_cast<CCDAccessPointRecord *>(
                           CCDRecordBase::RecordFactoryL( KCDTIdAccessPointRecord ) );
            CleanupStack::PushL( destAPRecord );
            destAPRecord->iRecordTag = cmIds[i];
            if ( !destAPRecord->FindL( iCmManager->Session() ) )
                {
                User::Leave( KErrNotFound );
                }
            
            name = TPtrC( destAPRecord->iRecordName ).AllocLC();

            TProtectionLevel protLev = CCmDestinationImpl::ProtectionLevelL( *iCmManager, cmIds[i] );
            
            protect = (protLev == EProtLevel1) || (protLev == EProtLevel2);

            HBufC* destName = StringLoader::LoadL( R_QTN_NETW_CONSET_EMBEDDED_DEST,
                                                   *name );
                                                   
            CleanupStack::PopAndDestroy( name );            
            name = destName;            
            CleanupStack::PushL( name );
            
            bearerType = KUidEmbeddedDestination;
            }
        else
            {
            recId = cmIds[i];
            iapRecord = static_cast<CCDIAPRecord *>
                          (CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
            CleanupStack::PushL( iapRecord );
            
            iapRecord->SetRecordId( recId );
            iapRecord->LoadL( iCmManager->Session() );
            
            TRAPD( err, bearerType = iCmManager->BearerTypeFromIapRecordL( iapRecord ) );
            
            if( err == KErrNotSupported )
                // This is unsupported connection method -> don't add it to the list
                {
                CleanupStack::PopAndDestroy( iapRecord );
                continue;
                }
            else
                {
                User::LeaveIfError( err );
                }
            name = TPtrC( iapRecord->iRecordName ).AllocLC();
            protect = iapRecord->Attributes() & ECDProtectedWrite;

            if ( bearerIdArray.Count() )
                {
                for ( TInt j = 0; j<bearerIdArray.Count(); j++ )
                    {
                    if ( bearerType == bearerIdArray[j] )
                        {
                        iconId = j+KCmReservedIconIndexCount;
                        }
                    }
                }
            }

        if ( iconId == KErrNotFound )
            {
            TInt iconInt( 0 );
            if( isDest )
                // Embedded destinations has not constant icon.
                {
                iconInt = (TInt)CCmDestinationImpl::IconIdL( *iCmManager,
                                                             cmIds[i] );
                }
            else
                {
                iconInt = iCmManager->GetBearerInfoIntL( bearerType, 
                                                         ECmBearerIcon );
                }

            if (iconInt) 
                {
                if ( isDest )
                    {
                    // append zero - assuming that bearer type values differ from zero
                    bearerIdArray.Append( 0 ); 
                    }
                else
                    {
                    bearerIdArray.Append( bearerType );
                    }

                icons->AppendL( (CGulIcon*)iconInt );
                iconId = bearerIdArray.Count()-1+KCmReservedIconIndexCount;
                }
            else 
                {
                iconId = KCmReservedIconIndexCount;
                }
            }
        TBool def = EFalse;
        if ( defUid == recId ) 
            {
            def = ETrue;
            }
        
        TInt nextLayerSnapId = 0;
        if( !isDest ) 
            {
            // Check if it is VPN over destination    
            // If yes the list item should have single line 
            CCmPluginBase* plugin = iCmManager->GetConnectionMethodL( cmIds[i] );
            CleanupStack::PushL( plugin );
                   
            if( plugin->GetBoolAttributeL( ECmVirtual ) )
                {
                nextLayerSnapId = plugin->GetIntAttributeL( ECmNextLayerSNAPId );
                }
                
            CleanupStack::PopAndDestroy( plugin );
            }

        CCmListItem* item = 
                CCmListItem::NewLC( 
                            recId, 
                            name,
                            i, 
                            iconId,
                            protect,
                            (nextLayerSnapId ? ETrue : EFalse),
                            // reasons for single line
                            ( isDest || !iCmDestinationImpl), 
                            def );
        CleanupStack::Pop( item );
        CleanupStack::Pop( name );
        CleanupStack::PopAndDestroy(); // iapRecord or nwRecord
        CleanupStack::PushL( item );
        iModel->AppendL( item );
        CleanupStack::Pop( item );
        iItemIndex.Append(i);
        }
    
    TInt cmCount = iItemIndex.Count();
    SetNoOfCMsL( cmCount );
    
    if ( !cmCount && iCmDestinationImpl )
        {
        HBufC* buf = 0;
        // rare case probably only caused by bad configuration
        if ( iCmDestinationImpl->ProtectionLevel() == EProtLevel1 )
            {
            buf = StringLoader::LoadLC( R_TEXT_SOFTKEY_OPTION );
            }
        else
            {    
            buf = StringLoader::LoadLC( R_CMMANAGERUI_MSK_ADD_METHOD );
            }
        ButtonGroupContainer().SetCommandL( ECmManagerUiCmdCmEdit, *buf );
        ButtonGroupContainer().DrawDeferred();                                                    
        CleanupStack::PopAndDestroy( buf );
        }

    CleanupStack::PopAndDestroy( &bearerIdArray );
    
    CArrayPtr<CGulIcon>* oldIcons = 
                            iListbox->ItemDrawer()->ColumnData()->IconArray();
    if ( oldIcons )
        {
        oldIcons->ResetAndDestroy();
        delete oldIcons;
        }
    
    iListbox->ItemDrawer()->ColumnData()->SetIconArray( icons );

    CleanupStack::Pop( icons );
    CleanupStack::PopAndDestroy( &cmIds ); 
    
    iCmManager->CommitTransactionL( KErrNone );

    // Fix for the added IAP not showing in the list.
    iListbox->HandleItemAdditionL();

    iListbox->DrawNow();
    iListbox->UpdateScrollBarsL();
    
    TInt count = iListbox->Model()->NumberOfItems();
    if ( count )
        {
        if ( count <= selected )
            {
            selected = count - 1; // index, so --
            }
        if ( selected >= 0 )
            {
            iListbox->ScrollToMakeItemVisible( selected);
            iListbox->SetCurrentItemIndex( selected );
            }
        }
    }
    

// --------------------------------------------------------------------------
// CCmDlg::CurrentCmInUseL
// --------------------------------------------------------------------------
//
TBool CCmDlg::CurrentCmInUseL()
    {
    TBool retVal = CurrentCML()->GetBoolAttributeL( ECmConnected );
    if ( retVal )
        {
        TCmCommonUi::ShowNoteL( R_QTN_SET_NOTE_AP_IN_USE_EDIT,
                                TCmCommonUi::ECmErrorNote );
        retVal = ETrue;
        }
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmDlg::ConstructCMArrayL
// --------------------------------------------------------------------------
//
void CCmDlg::ConstructCMArrayL( RArray<TUint32>& aCmIds )
    {
    iCmDestinationImpl->ReLoadConnectionMethodsL();
    iCmDestinationImpl->ConnectMethodIdArrayL( aCmIds );
    }

// --------------------------------------------------------------------------
// CCmDlg::ClearHiddenCMsFromArrayL
// --------------------------------------------------------------------------
//
void CCmDlg::ClearHiddenCMsFromArrayL( RArray<TUint32>& aCmIds )
    {
    TBool hidden( EFalse );
    TInt err( KErrNone );
    for ( TInt index = 0; index < aCmIds.Count(); index++ )
        {
        TUint recId = aCmIds[index];
        TRAP( err, hidden = iCmManager->GetConnectionMethodInfoBoolL( recId, ECmHidden ) );
        if ( err || hidden )
            {
            aCmIds.Remove( index );
            index--;
            }
        }
    }

// --------------------------------------------------------------------------
// CCmDlg::GetHelpContext
// --------------------------------------------------------------------------
//
void CCmDlg::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KHelpUidPlugin;
    aContext.iContext = KSET_HLP_CONN_METHODS_VIEW;
    }
    
// --------------------------------------------------------------------------
// CCmDlg::FinishPriorizingL
// --------------------------------------------------------------------------
//
void CCmDlg::FinishPriorizingL( TBool aOkPushed )
    {
    LOGGER_ENTERFN( "CCmDlg::FinishPriorizing" );

    iListbox->ClearSelection();
    iPrioritising = EFalse;
    
    if( aOkPushed )
        {
        if ( CurrentCML()->GetBoolAttributeL( ECmDestination ) ||
             (CurrentCML()->GetBoolAttributeL( ECmVirtual ) &&
              CurrentCML()->GetIntAttributeL( ECmNextLayerSNAPId )) )
            {
            // Priorising not possible
            TCmCommonUi::ShowNoteL( R_CMMANAGERUI_INFO_PRIO_NOT_POSSIBLE,
                                    TCmCommonUi::ECmInfoNote );
            }
        else
            {
            CCmPluginBase* cm = CMByIndexL(iCmToPrioritise);
            iCmDestinationImpl->ModifyPriorityL( 
                                *cm, 
                                iListbox->CurrentItemIndex());
            iCmDestinationImpl->UpdateL();
            HandleListboxDataChangeL();
            }
        }
     // Set the softkeys back    
     ButtonGroupContainer().SetCommandSetL(
                                    R_SOFTKEYS_OPTIONS_BACK__EDIT );
     ButtonGroupContainer().DrawDeferred();
     }


// --------------------------------------------------------------------------
// CCmDlg::Execute
// --------------------------------------------------------------------------
//
void CCmDlg::Execute()
    {
    LOGGER_ENTERFN( "CCmDlg::Execute" );

    iInfoPopupNoteController->HideInfoPopupNote();
    
    if(iPrioritising)
    	{
    	TRAPD( err, FinishPriorizingL( ETrue ) );
    	if ( err )
    	    {
    	    HandleLeaveError( err );
    	    }
    	}
    else
    	{
        iProcessing = ETrue;
        
        TRAPD( err, ProcessCommandL(ECmManagerUiCmdCmEdit) );
    	if ( err )
    	    {
    	    HandleLeaveError( err );
    	    }

        iProcessing = EFalse;
    	}
    }

// --------------------------------------------------------------------------
// CCmDlg::HandleLeaveError
// --------------------------------------------------------------------------
//
void CCmDlg::HandleLeaveError( TInt /*aError*/ )
    {
    if(iProcessing)
        {
        iProcessing = EFalse;
        }
    }

// --------------------------------------------------------------------------
// CCmDlg::HandleResourceChange
// --------------------------------------------------------------------------
//
void CCmDlg::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType ); 

    if( aType == KEikDynamicLayoutVariantSwitch )
        {
        DrawNow(); 
        }
        
    CAknDialog::HandleResourceChange(aType);        
    }

// --------------------------------------------------------------------------
// CCmDlg::GetInsertIndexL
// --------------------------------------------------------------------------
//
TInt CCmDlg::GetInsertIndexL( TInt aCount, TUint32 aPriority )
    {
    TInt ret = -1;
    TUint32 priority;
    
    for ( TInt index = 0; index < aCount; index++ )
        {
        CCmPluginBase* cm = iCmDestinationImpl->GetConnectionMethodL( iItemIndex[index] );
        
        if ( cm->GetBoolAttributeL( ECmVirtual ) &&
             !cm->GetIntAttributeL( ECmNextLayerSNAPId ) )
             {
             TUint32 underlying = cm->GetIntAttributeL( ECmNextLayerIapId );
             if ( underlying != 0 )
                {
                // Find the index that this VPN Iap should be inserted
                TUint32 underlyingBearer = iCmManager->BearerTypeFromCmIdL( underlying );
                priority = iCmManager->GetBearerInfoIntL( underlyingBearer, ECmDefaultPriority );
                }
             else
                {
                priority = KDataMobilitySelectionPolicyPriorityWildCard;
                }
             }
        else
            {
            priority = cm->Plugin()->GetIntAttributeL( ECmDefaultPriority );
            }
        
        if (priority > aPriority)
            {
            ret = index;
            break;
            }
        }
    
    return ret;
    }

// --------------------------------------------------------------------------
// CCmDlg::ReBuildItemIndexArrayL
// --------------------------------------------------------------------------
//
void CCmDlg::ReBuildItemIndexArrayL()
    {
    iCmManager->OpenTransactionLC();

    RArray<TUint32> cmIds;
    ConstructCMArrayL( cmIds );
    CleanupClosePushL( cmIds );
    
    // Remove hidden Connection Methods from the array if uncat folder
    if ( !iCmDestinationImpl )
        {
        ClearHiddenCMsFromArrayL( cmIds );
        }

    iItemIndex.Reset();
    for ( TInt i = 0; i < cmIds.Count(); i++ )
        {
        iItemIndex.Append(i);
        }

    CleanupStack::PopAndDestroy( &cmIds ); 
    
    iCmManager->CommitTransactionL( KErrNone );
    }

// --------------------------------------------------------------------------
// CCmDlg::ReOrderCMsL
// --------------------------------------------------------------------------
//
void CCmDlg::ReOrderCMsL()
    {
    ReBuildItemIndexArrayL();
    
    TBool prioAllowed = ETrue;
    TInt firstNotAllowedEntry = 0;
    TInt index = 0;
    TInt count = iItemIndex.Count();
    
    if (!iCmDestinationImpl)
        {
        return;
        }
   
    // This part is to move a SANP-referred VPN Iap to the end of list if the VPN Iap is changed from
    // IAP-referred to SNAP-referred. 
    for ( index = 0; index < count; index++ )
        {
        CCmPluginBase* cm = iCmDestinationImpl->GetConnectionMethodL( iItemIndex[index] );
        
        if ( cm->GetBoolAttributeL( ECmDestination ) ||
             (cm->GetBoolAttributeL( ECmVirtual ) &&
              cm->GetIntAttributeL( ECmNextLayerSNAPId )) )
            {
            prioAllowed = EFalse;
            firstNotAllowedEntry = index;
            }
        else
            {
            if ( !prioAllowed )
                {
                iCmDestinationImpl->ModifyPriorityL(*cm, firstNotAllowedEntry);
                iCmDestinationImpl->UpdateL();
                // start from the beginning
                index = 0;
                prioAllowed = ETrue;
                continue;
                }
            }
        }

    // This part is to move a IAP-referred VPN Iap to a proper location of priority in list
    // if the VAP Iap is changed from SNAP-referred to IAP-referred.
    for ( index = 0; index < count; index++ )
        {
        CCmPluginBase* cm = iCmDestinationImpl->GetConnectionMethodL( iItemIndex[index] );

        if ( cm->GetBoolAttributeL( ECmVirtual ) &&
             !cm->GetIntAttributeL( ECmNextLayerSNAPId ) )
            {
            TUint32 underlying = cm->GetIntAttributeL( ECmNextLayerIapId );
            if ( underlying != 0 )
                {
                // Find the index that this VPN Iap should be inserted
                TUint32 underlyingBearer = iCmManager->BearerTypeFromCmIdL( underlying );
                TUint32 underlyingPrio = iCmManager->GetBearerInfoIntL( underlyingBearer, ECmDefaultPriority );
                TInt insertAt = GetInsertIndexL( index, underlyingPrio );
                
                // Modify the priority of this VPN Iap
                if (insertAt != -1)
                    {
                    iCmDestinationImpl->ModifyPriorityL( *cm, insertAt );
                    iCmDestinationImpl->UpdateL();
                    break;
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CCmDlg::CommsDatChanges
// --------------------------------------------------------------------------
//
void CCmDlg::CommsDatChangesL()
    {
    if (iCmDestinationImpl)
        {
        // If the destination that is currently working on disappears
        // with some reason then go back to parent view
        if( !iCmManager->DestinationStillExistedL( iCmDestinationImpl ) )
            {
            iCmManager->WatcherUnRegister();
            iExitReason = KDialogUserBack;
            TryExitL( iExitReason );
            return;
            }
        }
    
    // Update list box
    HandleListboxDataChangeL();
    }

// --------------------------------------------------------------------------
// CCmDlg::IsExceptionExitL
// --------------------------------------------------------------------------
//
TBool CCmDlg::IsExceptionExitL()
    {
    TBool retV( EFalse );
 
    if (iCmDestinationImpl)
        {
        // If the destination that is currently working on disappears
        // with some reason then go back to parent view
        if( !iCmManager->DestinationStillExistedL( iCmDestinationImpl ) )
            {
            retV = ETrue;
            }
        }
    
    return retV;
    }
