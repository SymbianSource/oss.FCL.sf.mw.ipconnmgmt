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
* Description:  Implementation of CUncatDlg.
*
*/

#include <avkon.hrh>
#include <eikdef.h>
#include <eikenv.h>
#include <eikedwin.h>
#include <cmmanager.rsg>
#include <aknnavide.h>
#include <akntitle.h>
#include <eikmenup.h>
#include <data_caging_path_literals.hrh>
#include <StringLoader.h>
#include <AknIconArray.h>

#include "cmmanager.hrh"
#include "uncatdlg.h"
#include "cmlistitem.h"
#include "cmlistitemlist.h"
#include <cmpluginbaseeng.h>
#include <cmpluginbase.h>
#include "cmdestinationimpl.h"
#include <cmcommonui.h>
#include <cmcommonconstants.h>    
#include <cmpbasesettingsdlg.h>
#include "cmlistboxmodel.h"
#include "selectdestinationdlg.h"
#include "cmlogger.h"

using namespace CMManager;

// ---------------------------------------------------------------------------
// CUncatDlg::ConstructAndRunLD
// Constructs the dialog and runs it.
// ---------------------------------------------------------------------------
//
TInt CUncatDlg::ConstructAndRunLD( TUint32 aHighlight,
                                   TUint32& aSelected )
    {
    CleanupStack::PushL( this );
    iHighlight = aHighlight;
    iSelected = &aSelected;
    ConstructL( R_CM_MENUBAR );
    PrepareLC(R_CM_DIALOG);
    CleanupStack::Pop( this );  // it will be PushL-d by ExecuteLD...
    iModel = new( ELeave ) CCmListboxModel();
    //destructed in base class destructor
    iInfoPopupNoteController = CAknInfoPopupNoteController::NewL();

    // Trace changes in CommsDat
    iCmManager->WatcherRegisterL( this );
    
    return RunLD();
    }
    
// ---------------------------------------------------------------------------
// CUncatDlg::NewL()
// Two-phase dconstructor, second phase is ConstructAndRunLD
// ---------------------------------------------------------------------------
//
CUncatDlg* CUncatDlg::NewL(CCmManagerImpl* aCmManager )
    {
    CUncatDlg* self = new (ELeave) CUncatDlg( aCmManager );
    return self;
    }

// ---------------------------------------------------------------------------
// CUncatDlg::CUncatDlg()
// ---------------------------------------------------------------------------
//
CUncatDlg::CUncatDlg( CCmManagerImpl* aCmManager )
    : CCmDlg( aCmManager, NULL, NULL )
    , iCmUncatItems(KCmArraySmallGranularity)
    {
    CLOG_CREATE;
    CLOG_ATTACH( this, iCmManager );
    }
    
// ---------------------------------------------------------------------------
// CUncatDlg::~CUncatDlg
// Destructor
// ---------------------------------------------------------------------------
//
CUncatDlg::~CUncatDlg()
    {
    CLOG_WRITE( "CUncatDlg::~CUncatDlg" );
    CleanupUncatArray();   
    CLOG_CLOSE;
    }

// ---------------------------------------------------------------------------
// CUncatDlg::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CUncatDlg::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    TBool hideAdd ( EFalse );
    TBool hideMove ( EFalse );
    TBool hideCopy( EFalse );
    TBool hidePrioritise( EFalse );
    
    CCmDlg::DynInitMenuPaneL( aResourceId, aMenuPane );
    if ( aResourceId == R_CM_MENU )
        {
        // There are no destinatons to move to OR
        // the highlighted connection method is in use
        if ( !iCmManager->DestinationCountL() )
            {
            hideMove = ETrue;
            }

        // No priorities in Uncategorized -> always disabled
        hidePrioritise = ETrue;

        aMenuPane->SetItemDimmed( ECmManagerUiCmdCmAdd, hideAdd );
        aMenuPane->SetItemDimmed( ECmManagerUiCmdCmPrioritise, hidePrioritise  );
        aMenuPane->SetItemDimmed( ECmManagerUiCmdCmCopyToOtherDestination, hideCopy );
        aMenuPane->SetItemDimmed( ECmManagerUiCmdCmCopyToOtherDestination, hideMove );
        }
    }
    
// ---------------------------------------------------------------------------
// CUncatDlg::InitTextsL
// called before the dialog is shown
// to initialize localized textual data
// ---------------------------------------------------------------------------
//
void CUncatDlg::InitTextsL()
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
    iTitlePane =
        ( CAknTitlePane* )iStatusPane->ControlL(
                            TUid::Uid( EEikStatusPaneUidTitle ) );

    iOldTitleText = iTitlePane->Text()->AllocL();
    HBufC* name = StringLoader::LoadLC( R_CMMANAGERUI_DEST_UNCATEGORIZED );
    iTitlePane->SetTextL( *name );
    CleanupStack::PopAndDestroy( name );
    iNaviPane = ( CAknNavigationControlContainer* ) 
                    iStatusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) );
    iNaviDecorator = iNaviPane->CreateNavigationLabelL( KNullDesC );
    iNaviPane->PushL( *iNaviDecorator );
    }

// ---------------------------------------------------------------------------
// CUncatDlg::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CUncatDlg::ProcessCommandL( TInt aCommandId )
    {
    if ( MenuShowing() )
        {
        HideMenu();
        }

    switch ( aCommandId )
        {
        case ECmManagerUiCmdCmMoveToOtherDestination:
            {
            if ( CurrentCML()->GetBoolAttributeL( ECmConnected ) )
                    {
                TCmCommonUi::ShowNoteL( R_QTN_SET_NOTE_AP_IN_USE_EDIT,
                                    TCmCommonUi::ECmErrorNote );
                }
            else
                {
                // The selected item will be at the same position
                TInt selected = iListbox->CurrentItemIndex();
                TInt noi = iListbox->Model()->NumberOfItems();
                // If it is the last then the previous will be selected.
                if( selected == noi-1 )
                    {
                    selected--;
                    }
                
                TRAPD( err, CCmDlg::ProcessCommandL(
                                ECmManagerUiCmdCmCopyToOtherDestination ) );
                if ( err == KErrCancel )
                    {
                    break;
                    }
                else
                    {
                    User::LeaveIfError( err );
                    }
                    
                if ( iListbox->Model()->NumberOfItems() )
                    {
                    //first item cannot be deleted
                    iListbox->ScrollToMakeItemVisible( selected);
                    iListbox->SetCurrentItemIndexAndDraw( selected );                            
                    }
                else
                    {
                    TCmCommonUi::ShowNoteL( R_QTN_NETW_CONSET_INFO_UNCAT_EMPTY,
                                            TCmCommonUi::ECmInfoNote );
                    TryExitL( KDialogUserBack );                                
                    }
                }
            }
            break;
            
        case EAknSoftkeyClear:
        case ECmManagerUiCmdCmDelete:            
            {
            CCmPluginBase* cm = CurrentCML();
            
            if ( cm->GetBoolAttributeL( ECmProtected ) )
                {
                TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                                        TCmCommonUi::ECmErrorNote );
                                        
                break;
                }
           
            if ( cm->GetBoolAttributeL( ECmIsLinked ) )//same check as KErrLocked below
                {
                TCmCommonUi::ShowNoteL( R_QTN_NETW_CONSET_INFO_CANNOT_DELETE_VIRTUAL_REF,
                                        TCmCommonUi::ECmErrorNote );
                
                break;
                }
           
            if ( cm->GetBoolAttributeL( ECmConnected ) )//same check as KErrInUse below
                {
                TCmCommonUi::ShowNoteL( R_CMMANAGERUI_INFO_CM_IN_USE_CANNOT_DELETE,
                                        TCmCommonUi::ECmErrorNote );
                
                break;
                }

            HBufC* cmName = cm->GetStringAttributeL(ECmName);
            CleanupStack::PushL(cmName);
            
            if ( TCmCommonUi::ShowConfirmationQueryL( 
                                    R_CMMANAGERUI_QUEST_CM_DELETE, *cmName ) )
                {
                TRAPD( err, CurrentCML()->DeleteL( ETrue ) );

                switch ( err )
                    {
                    case KErrInUse:
                        {
                        TCmCommonUi::ShowNoteL
                            ( R_CMMANAGERUI_INFO_CM_IN_USE_CANNOT_DELETE,
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
                        HandleListboxDataChangeL();

                        if ( !iListbox->Model()->NumberOfItems() )
                            {
                            TCmCommonUi::ShowNoteL( R_QTN_NETW_CONSET_INFO_UNCAT_EMPTY,
                                                    TCmCommonUi::ECmInfoNote );
                            TryExitL( KDialogUserBack );                                
                            }
                        }
                        break;

                    default:
                        {
                        }
                    }

                }
            CleanupStack::PopAndDestroy( cmName );         
         
            }
            break;
            
        case ECmManagerUiCmdCmAdd:
        case ECmManagerUiCmdCmCopyToOtherDestination:
        case ECmManagerUiCmdCmPrioritise:
            {
            TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                                    TCmCommonUi::ECmErrorNote );
            }
            break;
            
        default:
            {
            CCmDlg::ProcessCommandL(aCommandId);
            }        
        }        
    }
    
// ---------------------------------------------------------------------------
// CUncatDlg::CurrentCML
// called before the dialog is shown to initialize listbox data
// ---------------------------------------------------------------------------
//
CCmPluginBase* CUncatDlg::CurrentCML()
    {
    return CMByIndexL( iListbox->CurrentItemIndex() );
    }    
    
// ---------------------------------------------------------------------------
// CUncatDlg::CMByIndexL
// a connection method in the list
// ---------------------------------------------------------------------------
//
CCmPluginBase* CUncatDlg::CMByIndexL( TInt aIndex )
    {
    TInt anIndex = iItemIndex[aIndex];    
    if( !iCmUncatItems[anIndex].iPlugin )
        {
        iCmUncatItems[anIndex].iPlugin = 
                                iCmManager->GetConnectionMethodL( 
                                                iCmUncatItems[anIndex].iCmId );
        }
    return iCmUncatItems[anIndex].iPlugin;
    }

// ---------------------------------------------------------------------------
// CUncatDlg::CMCount
// number of cms in the list
// ---------------------------------------------------------------------------
//
TInt CUncatDlg::CMCount()
    {
    return iCmUncatItems.Count();
    }

// --------------------------------------------------------------------------
// CUncatDlg::ConstructCMArrayL
// --------------------------------------------------------------------------
//
void CUncatDlg::ConstructCMArrayL( RArray<TUint32>& aCmIds )
    {
    // empty the array - reuse
    CleanupUncatArray( ETrue );
    iCmManager->ConnectionMethodL( aCmIds, EFalse );
    CleanupClosePushL( aCmIds );
    
    for( TInt i = 0; i < aCmIds.Count(); ++i )
        {
        TUncatItem item;
        
        item.iPlugin = NULL;
        item.iCmId = aCmIds[i];
        
        User::LeaveIfError( iCmUncatItems.Append( item ) );
        }
    
    CleanupStack::Pop( &aCmIds );
    }

// --------------------------------------------------------------------------
// CCmDlg::ClearHiddenCMsFromArrayL
// --------------------------------------------------------------------------
//
void CUncatDlg::ClearHiddenCMsFromArrayL( RArray<TUint32>& aCmIds )
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
            // Remove the same item from iCmUncatItems array
            for( TInt i = 0; i < iCmUncatItems.Count(); i++ )
                {
                if( iCmUncatItems[i].iCmId == recId )
                    {
                    iCmUncatItems.Remove( i );
                    break;
                    }
                }
            }
        }
    }


// --------------------------------------------------------------------------
// CUncatDlg::CleanupUncatArray
// --------------------------------------------------------------------------
//
void CUncatDlg::CleanupUncatArray( TBool aReuseArray )
    {
    CLOG_WRITE( "CUncatDlg::CleanupUncatArray" );
    for ( TInt i = 0; i < iCmUncatItems.Count(); ++i )
        {
        CCmPluginBase* cm = iCmUncatItems[i].iPlugin;
        delete cm;
        }
    
    if ( aReuseArray )
        {
        iCmUncatItems.Reset();
        }
    else
        {
        iCmUncatItems.Close();
        }
    }

// --------------------------------------------------------------------------
// CUncatDlg::CommsDatChangesL
// --------------------------------------------------------------------------
//
void CUncatDlg::CommsDatChangesL()
    {
    // Update list box
    HandleListboxDataChangeL();
    }
