/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Defines dialog for access point view/edit.
*
*/


// INCLUDE FILES
#include "APSettingsHandlerUIVariant.hrh"


#include <aknnavide.h>
#include <akntitle.h>
#include <AknQueryDialog.h>
#include <akntextsettingpage.h>
#include <aknpopupsettingpage.h>
#include <aknradiobuttonsettingpage.h>
#include <aknpasswordsettingpage.h>
#include <aknmfnesettingpage.h>

#include <ApAccessPointItem.h>
#include <utf.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <nifvar_internal.h>
#endif


#include "ApSettingsLookups.h"
#include "ApSettingsDlg.h"
#include "ApSettingsModel.h"
#include <apsetui.rsg>

#include "ApSettingsHandlerUI.hrh"
#include "ApsettingshandleruiImpl.h"
#include "ApSettingsHandlerConsts.h"
#include <ApSettingsHandlerCommons.h>

#include "ApSettingsHandlerLogger.h"

#include <commonphoneparser.h>

#include "ApNetSelPopupList.h"

#include "AccessPointTextSettingPage.h"


#include <featmgr.h>

#include "FeatureManagerWrapper.h"
#include "ApMultiLineDataQueryDialog.h"

#include <ConnectionUiUtilities.h>
#include <wlanmgmtcommon.h>
#include <centralrepository.h>

// CONSTANTS
// General Settings UID, Hardcoded
// const TUid KGeneralSettingsUid={0X100058EC};
//        const TInt KOptionalDefaultValue    = 1;
        


const TInt KIspCallAnalogue         = 0;
const TInt KIspCallIsdn110          = 1;
const TInt KIspCallIsdn120          = 2;

const TInt KBearerListGprs          = 0;
const TInt KBearerListWLAN          = 1;
const TInt KBearerListCSD           = 2;
const TInt KBearerListHSCSD         = 3;



// They provide the indexes.
const TInt KBearerArrayCSD           = 0;
const TInt KBearerArrayGPRS          = 1;
const TInt KBearerArrayHSCSD         = 2;
const TInt KBearerArrayWLAN          = 3;

const TInt KBoolMaxNumber           = 1;

_LIT( KEmptyText, "" );
_LIT( KTxtListItemFormat, " \t%S\t\t%S" );
_LIT( KTxtMenuListItemFormat, " \t%S\t\t" );
_LIT( KTxtCompulsory, "\t*" );

_LIT( KTxtNumber, "%d" );


_LIT( KKnownNameServer1, "fec0:000:0000:ffff::1" );
_LIT( KKnownNameServer2, "fec0:000:0000:ffff::2" );


const TInt KStaredPasswordLength = 4;

const TUint32 KDefAdhocChannel = 7;


const TInt  KItemIndex0 = 0;
const TInt  KItemIndex1 = 1;
const TInt  KItemIndex2 = 2;
const TInt  KItemIndex3 = 3;

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CApSettingsDlg::ConstructAndRunLD
// Constructs the dialog and runs it.
// ---------------------------------------------------------
//
TInt CApSettingsDlg::ConstructAndRunLD( CApSettingsModel& aModel,
                                        CApSettingsHandlerImpl& aHandler,
                                        TUint32& aUid,
                                        CApAccessPointItem* aItem )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::ConstructAndRunLD")
    
    CleanupStack::PushL( this );

    iFeatureManagerWrapper = CFeatureManagerWrapper::NewL();
    
    iHelpSupported = FeatureManager::FeatureSupported( KFeatureIdHelp );

#ifdef __TEST_OOMDEBUG
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL<RFs>( fs );
    TUint att;
    if ( fs.Att( KTestFileName, att ) == KErrNone )
        {
        iMemTestOn = ETrue;
        }
    else
        {
        iMemTestOn = EFalse;
        }
    CleanupStack::PopAndDestroy(); // fs, will also close it
#endif // __TEST_OOMDEBUG


    iUid = &aUid;

    iDataModel = &aModel;
    iHandler = &aHandler;
    iVariant = iHandler->iExt->iVariant;


    CApAccessPointItem* ap = CApAccessPointItem::NewLC();

    iBearerItemArray = 
            new( ELeave )CArrayPtrFlat<CApAccessPointItem>( KBearers );
    iBearerItemArray->ResetAndDestroy();
    for ( TInt i=0; i<KBearers; i++ )
        {
        iBearerItemArray->AppendL( NULL );
        }
    TApBearerType ab = ap->BearerTypeL();
    iBearerItemArray->At( BearerToArrayIndexL( ab ) ) = ap;
    // Now iApItem is part of array-> destructed member var, pop it.
    CleanupStack::Pop( ap );

    iApItem = ap;
    
    if ( !aItem )
        {
        iDataModel->AccessPointDataL( *iUid, *iApItem );
        }
    else
        {
        iApItem->CopyFromL( *aItem );
        }
    iDataModel->ClearWEPAndWPASettings();

    iOldApItem = CApAccessPointItem::NewLC();
    CleanupStack::Pop(); // member var

    iOldApItem->CopyFromL( *iApItem );

    iOriginalBearerType = iApItem->BearerTypeL();

    ConstructL( R_APSETTINGS_MENUBAR );

    SetBearerTypeL( iApItem->BearerTypeL() );
    // ExecuteLD will PushL( this ), so we have to Pop it...
    CleanupStack::Pop(); // this


    APSETUILOGGER_LEAVEFN( ESettings,"Settings::ConstructAndRunLD")
    return ExecuteLD( R_APSETTINGS_DIALOG );
    }



// Destructor
CApSettingsDlg::~CApSettingsDlg()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::~CApSettingsDlg")
    
    delete iFeatureManagerWrapper;

    if ( iDataModel )
        {// Remove ourselves from observers...
        iDataModel->Database()->RemoveObserver( this );
        }

    if ( iOldApItem )
        {
        delete iOldApItem;
        }

    if ( iNaviDecorator )
        {
        delete iNaviDecorator;
        }

    if ( iTitlePane )
        {
        // set old text back, if we have it...
        if ( iOldTitleText )
            {
            if ( iExitReason != EShutDown )
                {
                TRAP_IGNORE( iTitlePane->SetTextL( *iOldTitleText ) );
                }
            delete iOldTitleText;
            }
        }

    if ( iBearerItemArray )
        {
        iBearerItemArray->ResetAndDestroy();
        delete iBearerItemArray;
        }
    
    if (iBackupApItem)
        {
        delete iBackupApItem;
        }

    APSETUILOGGER_LEAVEFN( ESettings,"Settings::~CApSettingsDlg")
    }



// ---------------------------------------------------------
// CApSettingsDlg::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
//                                 TEventCode aType )
//  Handles key events
// ---------------------------------------------------------
//
TKeyResponse CApSettingsDlg::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                             TEventCode aType )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::OfferKeyEventL")
    
    TKeyResponse retval( EKeyWasNotConsumed );
    // Only interested in standard key events
    if ( aType == EEventKey )
        {
        // If a menu is showing offer key events to it.
        if ( CAknDialog::MenuShowing() )
            {
            retval = CAknDialog::OfferKeyEventL( aKeyEvent, aType );
            }
        else
            {
            if ( iList )
                {
                // as list IS consuming, must handle because it 
                // IS the SHUTDOWN...
                // or, a view switch is shutting us down...
                if ( aKeyEvent.iCode == EKeyEscape )
                    {
                    ProcessCommandL( EEikCmdExit );
                    retval = EKeyWasConsumed;
                    }
                else
                    {
                    retval = iList->OfferKeyEventL( aKeyEvent, aType );
                    if ( iSecSettingsExitReason )
                        {                        
                        if ( iSecSettingsExitReason 
                             & KApUiEventShutDownRequested )
                            {            
                            ProcessCommandL( EAknCmdExit );
                            }
                        else
                            {                        
                            if ( iSecSettingsExitReason 
                                 & KApUiEventExitRequested )
                                {            
                                ProcessCommandL( EApSelCmdExit );
                                }
                            }
                        }
                    }
                }
            else
                {
                if ( aKeyEvent.iCode == EKeyOK )
                    {
                    ProcessCommandL( EApSelCmdChange );
                    retval = EKeyWasConsumed;
                    }
                }
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::OfferKeyEventL")
    return retval;
    }



// ---------------------------------------------------------
// CApSettingsDlg::NewL
// Two-phase dconstructor, second phase is ConstructAndRunLD
// ---------------------------------------------------------
//
CApSettingsDlg* CApSettingsDlg::NewL( TInt aIspFilter,
                                      TInt aBearerFilter,
                                      TInt aSortType,
                                      TUint32& aEventStore,
                                      TInt aReqIpvType
                                      )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::NewL")
    
    CApSettingsDlg* db = 
        new ( ELeave )CApSettingsDlg( aIspFilter, 
                                      aBearerFilter, 
                                      aSortType, 
                                      aEventStore,
                                      aReqIpvType
                                      );
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::NewL")
    return db;

    }




// Constructor
CApSettingsDlg::CApSettingsDlg( TInt aIspFilter, TInt aBearerFilter, 
                                TInt aSortType, TUint32& aEventStore,
                                TInt aReqIpvType )
:
iLevel( 0 ),
iOldIndex( 0 ),
iOldTopIndex( 0 ),
iAdvIndex( 0 ),
iBearerType( EApBearerTypeAll ),
iIspFilter( aIspFilter ),
iBearerFilter( aBearerFilter ),
iSortType( aSortType ),
iEventStore( &aEventStore ),
iFieldCount( 0 ),
iAdvancedFieldCount( 0 ),
iExitReason( EExitNone ),
iReqIpvType( aReqIpvType ),
iL2Ipv4( ETrue ),
iSecSettingsExitReason(0)
    {
    iBackupApItem = NULL;
    }


// ---------------------------------------------------------
// CApSettingsDlg::OkToExitL( TInt aButtonId)
// called by framework when the OK button is pressed
// ---------------------------------------------------------
//
TBool CApSettingsDlg::OkToExitL( TInt aButtonId )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::OkToExitL")
    
    // Translate the button presses into commands for the appui & current
    // view to handle
    TBool retval( EFalse );
    if ( aButtonId == EAknSoftkeyOptions )
        {
        DisplayMenuL() ;
        }
    else 
        {
        if ( (aButtonId == EApSelCmdChange) 
            || (aButtonId == EApSelCmdQuickChange))
            {
            ProcessCommandL(aButtonId);
            if ( iSecSettingsExitReason )
                {                        
                if ( iSecSettingsExitReason 
                     & KApUiEventShutDownRequested )
                    {            
                    ProcessCommandL( EAknCmdExit );
//                    retval = ETrue; // exit the dialog
                    }
                else
                    {                        
                    if ( iSecSettingsExitReason 
                         & KApUiEventExitRequested )
                        {            
                        ProcessCommandL( EApSelCmdExit );
//                        retval = ETrue; // exit the dialog
                        }
                    }
                }
            }
        else
            {
            if ( aButtonId == EAknSoftkeyBack )
                {
                if ( iLevel )
                    {
                    TSaveAction cansave( EApNoAction );
                    if ( iLevel == 2 )
                        {// coming back from Ipv4 settings
                        // check if Wlan and has compulsory fields
                        if (iBearerType == EApBearerTypeWLAN)
                            { // WLAN
                            TApMember member;
                            cansave = CanSaveWlanCompulsoriesL( member );                        
                            switch (cansave)
                                {
                                case EApCanSave:
                                    {
                                    // nothing to do in this case, simply go on.
                                    iLevel --;
                                    break;
                                    }
                                case EApMustDelete:
                                    {
                                    // user said quit without save,
                                    // restore from iBackupApItem
                                    RestoreIpv4SettingsL();
                                    iLevel --;
                                    break;
                                    }
                                case EApShallGoBack:
                                    {
                                    // user wants to re-edit, do not come back...
                                    // no change in iLevel
                                    
                                    // Set correct item to edit
                                    switch ( member )
                                        {
                                        case EApWlanIpNetMask:
                                        case EApIspIPNetMask:
                                            {
                                            if ( IsWlanSupported() )
                                                {                    
                                                SelectItem( KListIndexWLANSubnetMask );
                                                retval = EFalse;
                                                }
                                            else
                                                {
                                                User::Leave( KErrNotSupported );
                                                }
                                            break;
                                            }
                                        case EApIspIPGateway:
                                        case EApWlanIpGateway:
                                            {
                                            if ( IsWlanSupported() )
                                                {                    
                                                SelectItem( KListIndexWLANDefaultGateway );
                                                retval = EFalse;
                                                }
                                            else
                                                {
                                                User::Leave( KErrNotSupported );
                                                }
                                            break;
                                            }
                                        default:
                                            {// otherwise, nothing to do...
                                            __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
                                            break;
                                            }
                                        }
                                    break;
                                    }
                                case EApNoAction:
                                default:
                                    {
                                    // this is not a valid case as it always one from 
                                    // the above 3, 
                                    // nothing to do,
                                    iLevel --;
                                    break;
                                    }
                                }
                            }
                        else
                            {
                            iLevel--;
                            }                        
                        }
                    else
                        {
                        iLevel--;
                        }
                    HandleListboxDataChangeL( EFalse );
                    if ( iOldIndex >=0 )
                        {
                        if ( cansave != EApShallGoBack )
                            {
                            iList->SetTopItemIndex( iOldTopIndex );
                            iList->SetCurrentItemIndexAndDraw( iOldIndex );                        
                            }
                        if ( *iEventStore & KApUiEventExitRequested )
                            {
                            ProcessCommandL( EApSelCmdExit );
                            }
                        }
                    }
                else
                    {
                    if ( iExitReason == EShutDown )
                        {
                        retval = ETrue;
                        }
                    else
                        {
                        if ( !(*iEventStore & KApUiEventShutDownRequested ) )
                            {
                            retval = SaveDataL( );
                            }
                        else
                            {
                            retval = ETrue;
                            }
                        }
                    }
                }
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::OkToExitL")
    return retval;
    }




// ---------------------------------------------------------
// CApSettingsDlg::InitTextsL
// called before the dialog is shown
// to initialize localized textual data
// ---------------------------------------------------------
//
void CApSettingsDlg::InitTextsL()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::InitTextsL")
    
    // set pane texts
    // first get StatusPane
    iStatusPane = iEikonEnv->AppUiFactory()->StatusPane();
    // then get TitlePane
    iTitlePane = ( CAknTitlePane* ) iStatusPane->ControlL( 
                                    TUid::Uid( EEikStatusPaneUidTitle ) );
    // if not already stored, store it for restoring
    if ( !iOldTitleText )
        {
        iOldTitleText = iTitlePane->Text()->AllocL();
        }
    // set new titlepane text
    iTitlePane->SetTextL( iApItem->ConnectionName() );
    // if not already done, clear NaviPane with adding en empty one
    if ( !iNaviDecorator )
        {
        iNaviPane = ( CAknNavigationControlContainer* ) 
                        iStatusPane->ControlL( 
                            TUid::Uid( EEikStatusPaneUidNavi ) );
        iNaviDecorator = iNaviPane->CreateNavigationLabelL( KEmpty );
        iNaviPane->PushL( *iNaviDecorator );
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::InitTextsL")
    }




// ---------------------------------------------------------
// CApSettingsDlg::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CApSettingsDlg::DynInitMenuPaneL
( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::DynInitMenuPaneL")
    
    CAknDialog::DynInitMenuPaneL( aResourceId, aMenuPane );
    if ( aResourceId == R_APSETTINGS_MENU )
        {
        if ( iLevel )
            {
            // Disallow Advanced settings item if already in advanced 
            // or in SMS.
            aMenuPane->DeleteMenuItem( EApSelCmdAdvanced );
            }
		if ( !iHelpSupported )
			{
			aMenuPane->DeleteMenuItem( EAknCmdHelp );	
			}
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::DynInitMenuPaneL")
    }



// ---------------------------------------------------------
// CApSettingsDlg::HandleListboxDataChangeL
// called before the dialog is shown to initialize listbox data
// ---------------------------------------------------------
//
void CApSettingsDlg::HandleListboxDataChangeL( TBool aReRead )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::HandleListboxDataChangeL")
    
    if ( aReRead && ( *iUid != KApNoneUID ) )
        { // reload the whole ApItem !
        // if it does not yet exists, no need to do so...
        iDataModel->AccessPointDataL( *iUid, *iApItem );
        iDataModel->ClearWEPAndWPASettings();
        }

    iBearerType = iApItem->BearerTypeL();

    // fill up our new list with data
    CDesCArrayFlat* ItemArray = new ( ELeave ) CDesCArrayFlat( 4 );
    CleanupStack::PushL( ItemArray );
    switch ( iLevel )
        {
        case 0:
            {
            FillListWithDataL( *ItemArray, *iField, iFieldCount,
                               iTitles, iCompulsory );
            break;
            }
        case 1:
            {
            FillListWithDataL( *ItemArray, *iFieldAdvanced, 
                               iAdvancedFieldCount, iTitlesAdvanced, 
                               iCompulsoryAdvanced );
            break;
            }
        case 2:
            {
            FillListWithDataL( *ItemArray, *iFieldL2, iFieldCountL2,
                               iTitlesL2, iCompulsoryL2 );
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
            break;
            }
        }


    // if not already stored, store it for restoring
    if ( !iOldTitleText )
        {
        iOldTitleText = iTitlePane->Text()->AllocL();
        }
    // set new title text
    if ( iLevel == 2 )
        {
        HBufC* value;
        if ( iL2Ipv4 )
            {
            value = iEikonEnv->AllocReadResourceLC( 
                            R_APUI_VIEW_IPV4_SETTINGS );
            }
        else
            {                
            value = iEikonEnv->AllocReadResourceLC( 
                            R_APUI_VIEW_IPV6_SETTINGS );
            }
        iTitlePane->SetTextL( *value );
        CleanupStack::PopAndDestroy( value );
        }
    else
        {            
        iTitlePane->SetTextL( iApItem->ConnectionName() );
        }

    iList->Model()->SetItemTextArray( ItemArray );
    // now it is owned by the LB, so pop it
    CleanupStack::Pop(); // do not destroy !
    iItemArray = ItemArray;

    iList->HandleItemAdditionL();
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::HandleListboxDataChangeL")
    }



// ---------------------------------------------------------
// CApSettingsDlg::SetTextOverrides
// ---------------------------------------------------------
//
void CApSettingsDlg::SetTextOverrides( CTextOverrides* aOverrides )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::SetTextOverrides<->")
    
    __ASSERT_DEBUG( aOverrides, Panic( ENullPointer ) );
    iTextOverrides = aOverrides;
    }



// ---------------------------------------------------------
// CApSettingsDlg::HandleApDbEventL
// called by the active access point framework
// ---------------------------------------------------------
//
void CApSettingsDlg::HandleApDbEventL( TEvent anEvent )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::HandleApDbEventL")
    
    switch ( anEvent )
        {
        case EDbChanged:
            {
            HandleListboxDataChangeL( ETrue );
            break;
            }
        case EDbClosing:
            {
            break;
            }
        case EDbAvailable:
            {
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
            break;
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::HandleApDbEventL")
    }



// ---------------------------------------------------------
// CApSettingsDlg::GetHelpContext()
// ---------------------------------------------------------
//
void CApSettingsDlg::GetHelpContext(TCoeHelpContext& aContext) const
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::GetHelpContext")
    
    aContext.iMajor = iHandler->iHelpMajor;
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::GetHelpContext")
    }


// ---------------------------------------------------------
// CApSettingsDlg::PreLayoutDynInitL();
// called by framework before dialog is shown
// ---------------------------------------------------------
//
void CApSettingsDlg::PreLayoutDynInitL()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::PreLayoutDynInitL")
    
    InitTextsL();

    iList = STATIC_CAST( CAknSettingStyleListBox*, 
                         Control( KApSettingsListboxId ) );

    iList->CreateScrollBarFrameL( ETrue );
    iList->ScrollBarFrame()->SetScrollBarVisibilityL
        ( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    HandleListboxDataChangeL( EFalse );
    iList->SetCurrentItemIndex( 0 );
    iList->SetListBoxObserver( this );

    iDataModel->Database()->AddObserverL( this );

    APSETUILOGGER_LEAVEFN( ESettings,"Settings::PreLayoutDynInitL")
    }




// From MEikCommandObserver
void CApSettingsDlg::ProcessCommandL( TInt aCommandId )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::ProcessCommandL")
    
    if ( MenuShowing() )
        {
        HideMenu();
        }

    switch ( aCommandId )
        {
        case EApSelCmdQuickChange:
            {
            ChangeSettingsL( ETrue );
            break;
            }
        case EApSelCmdChange:
            {
            ChangeSettingsL( EFalse );
            break;
            }
        case EApSelCmdAdvanced:
            {
            if ( iLevel == 0 )
                {
                iLevel ++;
                iOldIndex = iList->CurrentItemIndex();
                iOldTopIndex = iList->TopItemIndex();
                iList->SetCurrentItemIndex( 0 );
                TRAPD( err, HandleListboxDataChangeL( EFalse ) );
                if ( err )
                    {
                    iLevel--;
                    User::Leave( err );
                    }
                }
            break;
            }
        case EAknCmdHelp:
            {
            if ( iHelpSupported )
            	{
            	iDataModel->LaunchHelpL();
            	}
            break;
            }
//        case EAknSoftkeyBack:
        case EApSelCmdExit:
            {
            iExitReason = EExit;
            *iEventStore |= KApUiEventExitRequested;
            TryExitL( EAknSoftkeyBack );
            break;
            }
        case EAknCmdExit:
        case EEikCmdExit:
            {
            *iEventStore |= KApUiEventShutDownRequested;
            iExitReason = EShutDown;
            TryExitL( EAknSoftkeyBack );
            break;
            }
        default:
            {
            // silently ignore it
            break;
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::ProcessCommandL")
    }


// From MEikListBoxObserver
void CApSettingsDlg::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                          TListBoxEvent aEventType )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::HandleListBoxEventL")
    
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
            // both handled in the same way for now...
        case EEventItemDoubleClicked:
        case EEventItemSingleClicked:
            {
            ChangeSettingsL( ETrue );
            break;
            }
        case EEventItemClicked:
            {
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
//            __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
            };
        };
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::HandleListBoxEventL")
    }




//----------------------------------------------------------
// CApSettingsDlg::FillListWithDataL
//----------------------------------------------------------
//
void CApSettingsDlg::FillListWithDataL( CDesCArrayFlat& aItemArray,
                                        const TApMember& arr, TInt aLength,
                                        const TInt* aRes,
                                        const TInt* aCompulsory )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::FillListWithDataL")
    
    TInt i;
    TApMember* sgd = MUTABLE_CAST( TApMember*, &arr );
    for( i=0; i<aLength; i++ )
        {
        switch ( *sgd )
            {
            case EApWapGatewayAddress:
//            case EApIspIPAddr:
            case EApGprsIPNameServer1:
            case EApIspIPNameServer1:
            case EApGprsIPNameServer2:
            case EApIspIPNameServer2:
            case EApWapAccessPointName:
            case EApWapStartPage:
            case EApGprsAccessPointName:
            case EApIspLoginName:
            case EApIspLoginPass:
            case EApIspIfAuthName:
            case EApIspIfAuthPass:
            case EApIspDefaultTelNumber:
            case EApIspInitString:
            case EApIspIfCallbackInfo:
            case EApIspLoginScript:
            case EApProxyServerAddress:
            case EApProxyPortNumber:
            case EApNetworkName:
            case EApWlanNetworkName:
//            case EApIspIPNetMask:
//            case EApWlanIpGateway:
                {
                AddTextualListBoxItemL( aItemArray, *sgd, 
                                        *aRes, *aCompulsory );
                break;
                }
            case EApWlanIpNetMask:
            case EApIspIPNetMask:
            case EApIspIPGateway:
            case EApWlanIpGateway:
                { // check if they do really needed to be displayed...
                if ( IsNetMaskAndGatewayVisibleL() )
                    {
                    AddTextualListBoxItemL( aItemArray, *sgd, 
                                        *aRes, *aCompulsory );
                    }
                break;
                }                
            case EApIspIPAddr:
                { // GPRS, IPv4 or CSD, IPv4 settings
                if ( iApItem->BearerTypeL() == EApBearerTypeGPRS )
                    {
                    TUint32 tint;
                    iApItem->ReadUint( EApGprsPdpType, tint );
                    if ( tint != EIPv6 )
                        {
                        AddTextualListBoxItemL( aItemArray, *sgd, 
                                                *aRes, *aCompulsory );
                        }
                    }
                else
                    { // CSD
                    if ( iL2Ipv4 )
                        {
                        AddTextualListBoxItemL( aItemArray, *sgd, 
                                                *aRes, *aCompulsory );
                        }
                    }
                break;
                }
            case EApWapCurrentBearer:
            case EApWapSecurity:
            case EApWapWspOption:
            case EApIspPromptForLogin:
            case EApGprsDisablePlainTextAuth:
            case EApIspDisablePlainTextAuth:
            case EApIspBearerCallTypeIsdn:
            case EApIspIfCallbackEnabled:
            case EApIspIfCallbackType:
            case EApIspEnableSWCompression:
            case EApGprsHeaderCompression:
            case EApIspUseLoginScript:
            case EApIspBearerSpeed:
            case EApGprsPdpType:
            case EApIP6DNSAddrFromServer:
            case EApWlanNetworkMode:
            case EApWlanSecurityMode:
            case EApWlanScanSSID:
                {
                AddOnOffListBoxItemL( aItemArray, *sgd, *aRes, *aCompulsory );
                break;
                }
            case EApWlanChannelId:
                {
                if ( IsAdhocChannelVisible() )
                    {                    
                    AddOnOffListBoxItemL( aItemArray, *sgd, *aRes, 
                                          *aCompulsory );
                    }
                break;
                }                
            case EApIpv4Settings:
            case EApIpv6Settings:
                {
                AddMenuListBoxItemL( aItemArray, *sgd, *aRes, *aCompulsory );
                break;
                }
            case EApWlanSecuritySettings:
                {
                AddMenuListBoxItemL( aItemArray, *sgd, *aRes, *aCompulsory );
                break;
                }
            default:
                {
                __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
                break;
                }
            }
        sgd++;
        aRes++;
        aCompulsory++;
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::FillListWithDataL")
    }


//----------------------------------------------------------
// CApSettingsDlg::AddTextualListBoxItemL
//----------------------------------------------------------
//
void CApSettingsDlg::AddTextualListBoxItemL( CDesCArrayFlat& aItemArray,
                                             TApMember aMember, TInt aRes,
                                             TInt aCompulsory )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::AddTextualListBoxItemL")
    
    HBufC* sgd = CreateTextualListBoxItemL( aMember, aRes, aCompulsory );
    CleanupStack::PushL( sgd );
    aItemArray.AppendL( sgd->Des() );
    CleanupStack::PopAndDestroy( sgd );
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::AddTextualListBoxItemL")
    }


//----------------------------------------------------------
// CApSettingsDlg::AddOnOffListBoxItem
//----------------------------------------------------------
//
void CApSettingsDlg::AddOnOffListBoxItemL( CDesCArrayFlat& aItemArray,
                                           TApMember aMember, TInt aRes,
                                           TInt aCompulsory )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::AddOnOffListBoxItemL")
    
    // Read up title text from resource
    HBufC* sgd = CreateOnOffListBoxItemL( aMember, aRes, aCompulsory );
    CleanupStack::PushL( sgd );
    aItemArray.AppendL( sgd->Des() );
    CleanupStack::PopAndDestroy( sgd );
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::AddOnOffListBoxItemL")
    }


//----------------------------------------------------------
// CApSettingsDlg::UpdateOnOffListBoxItemL
//----------------------------------------------------------
//
void CApSettingsDlg::UpdateOnOffListBoxItemL( TApMember aMember, TInt aRes, 
                                              TInt aPos, TInt aCompulsory )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::UpdateOnOffListBoxItemL")
    
    // Read up title text from resource
    HBufC* sgd = CreateOnOffListBoxItemL( aMember, aRes, aCompulsory );
    CleanupStack::PushL( sgd );
    // first try to add, if Leaves, list will be untouched
    iItemArray->InsertL( aPos, sgd->Des() );
    // if successful, previous item is scrolled up with one,
    // so delete that one...
    if ( ++aPos < iItemArray->MdcaCount() )
        {
        iItemArray->Delete( aPos );
        }
    CleanupStack::PopAndDestroy( sgd );
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::UpdateOnOffListBoxItemL")
    }


//----------------------------------------------------------
// CApSettingsDlg::TextualListBoxItemL
//----------------------------------------------------------
//
void CApSettingsDlg::UpdateTextualListBoxItemL( TApMember aMember, TInt aRes,
                                                TInt aPos, TInt aCompulsory  )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::UpdateTextualListBoxItemL")

    HBufC* sgd = CreateTextualListBoxItemL( aMember, aRes, aCompulsory );
    CleanupStack::PushL( sgd );
    // first try to add, if Leaves, list will be untouched
    iItemArray->InsertL( aPos, sgd->Des() );
    // if successful, previous item is scrolled up with one,
    // so delete that one...
    if ( ++aPos < iItemArray->MdcaCount() )
        {
        iItemArray->Delete( aPos );
        }
    CleanupStack::PopAndDestroy( sgd );
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::UpdateTextualListBoxItemL")
    }


//----------------------------------------------------------
// CApSettingsDlg::CreateTextualListBoxItem
//----------------------------------------------------------
//
HBufC*  CApSettingsDlg::CreateTextualListBoxItemL( TApMember aMember, 
                                                   TInt aRes, 
                                                   TInt aCompulsory )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::CreateTextualListBoxItemL")
    
    // both variables needed independently of the following 
    // conditions so I must declare them here...
    HBufC16* value;
    TBool Pushed( EFalse );
    if ( ( aMember == EApIspLoginScript ) || ( aMember == EApWapStartPage ) 
         || ( aMember == EApGprsAccessPointName ) 
         || ( aMember == EApProxyServerAddress ) )
        {
        // allocates as much as needed...
        value = iApItem->ReadConstLongTextL( aMember )->AllocLC();
        }
    else
        {
        value = HBufC::NewLC( KModifiableTextLength );
        // handle different 'types' (8 bit or 16 bit) descriptors
        // and bring them to common base (16 bit...)
        if ( ( aMember == EApIspIfCallbackInfo ) 
            || ( aMember == EApIspInitString ) )
            {
            HBufC8* tmpValue8 = HBufC8::NewLC( KModifiableTextLength );
            TPtr8 ptr( tmpValue8->Des() );
            iApItem->ReadTextL( aMember, ptr );
            value->Des().Copy( *tmpValue8 );
            CleanupStack::PopAndDestroy(); // tmpValue8
            }
        else
            {
            if ( aMember == EApProxyPortNumber )
                {
                TUint32 tempint;
                iApItem->ReadUint( aMember, tempint );
                TPtr ptr( value->Des() );
                ptr.Format( KTxtNumber, tempint );
                }
            else
                {
                TPtr16 ptr( value->Des() );
                if ( ( aMember == EApIspIfAuthPass ) 
                    || ( aMember == EApIspLoginPass ) )
                    {
                    ptr.SetLength( KStaredPasswordLength );
                    ptr.Fill( '*' );
                    }
                else
                    {
                    iApItem->ReadTextL( aMember, ptr );
                    }
                }
            }
        switch ( aMember )
            {
            case EApIspIPAddr:
                {
                if ( ( value->Compare( KDynIpAddress ) == 0 )
                    || ( value->Compare( KEmptyText ) == 0 ) )
                    {
                    value = iEikonEnv->AllocReadResourceLC( 
                                R_APUI_VIEW_IP_ADDR_DYNAMIC );
                    Pushed = ETrue;
                    }
                break;
                }
            case EApIspIPNetMask:
            case EApIspIPGateway:
            case EApWlanIpNetMask:
            case EApWlanIpGateway:
                {
                if ( ( value->Compare( KDynIpAddress ) == 0 )
                    || ( value->Compare( KEmptyText ) == 0 ) )
                    {
                    value = iEikonEnv->AllocReadResourceLC( 
                                R_APUI_VIEW_COMPULSORY );
                    Pushed = ETrue;
                    }
                break;
                }
            case EApGprsIPNameServer1:
            case EApIspIPNameServer1:
            case EApGprsIPNameServer2:
            case EApIspIPNameServer2:
                {
                if ( ( value->Compare( KDynIpAddress ) == 0 ) 
                     || ( value->Compare( KEmptyText ) == 0 ) )
                    {
                    value = iEikonEnv->AllocReadResourceLC( 
                                R_APUI_VIEW_NEW_AP_DNS_SERVER_AUTOMATIC );
                    Pushed = ETrue;
                    }
                break;
                }
            case EApIspDefaultTelNumber:
            case EApWlanNetworkName:
                {
                if ( value->Compare( KEmptyText ) == 0 )
                    {
                    value = iEikonEnv->AllocReadResourceLC( 
                                R_APUI_VIEW_AVKON_COMPULSORY );
                    Pushed = ETrue;
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    // Define a heap descriptor to hold all the item text
    // HBufC is non-modifiable
    HBufC* title = iEikonEnv->AllocReadResourceLC( aRes );

    HBufC* value1 = ReplaceWhiteSpaceCharactersLC( *value );

    if ( value1->CompareC( KEmpty ) == 0 )
        { // empty, get 'None' as value
        HBufC* temp = NULL;
        switch ( aMember )
            {
            case EApWapStartPage:
                { // get special None for homepage
                temp = iEikonEnv->AllocReadResourceLC( 
                                R_APUI_SETT_HOME_PAGE_NONE );
                break;
                }
            case EApIspIfAuthName:
                { // user name none
                temp = iEikonEnv->AllocReadResourceLC( 
                                R_APUI_VIEW_NEW_AP_USER_NAME_NONE );
                break;
                }
            case EApProxyServerAddress:
                { // proxy server address none
                temp = iEikonEnv->AllocReadResourceLC( 
                                R_APUI_VIEW_NEW_AP_PROXY_SERVER_ADDR_NONE );
                break;
                }
            case EApIspIfCallbackInfo:
                { // callback number none
                temp = iEikonEnv->AllocReadResourceLC( 
                                R_APUI_VIEW_NEW_AP_CB_NUMBER_NONE );
                break;
                }
            case EApIspLoginScript:
                { // login script none
                temp = iEikonEnv->AllocReadResourceLC( 
                                R_APUI_VIEW_NEW_AP_LOGIN_SCRIPT_NONE );
                break;
                }
            case EApIspInitString:
                { // modem init string none
                temp = iEikonEnv->AllocReadResourceLC( 
                                R_APUI_VIEW_NEW_AP_MODEM_INIT_NONE );
                break;
                }
            default:
                { // get 'normal' None for others
                temp = 
                    iEikonEnv->AllocReadResourceLC( 
                                R_APUI_SETT_VAL_FIELD_NONE );
                break;
                }
            }
        CleanupStack::Pop(); // temp
        CleanupStack::PopAndDestroy( value1 );
        CleanupStack::PushL( temp );
        value1 = temp;
        }

    // Define a heap descriptor to hold all the item text
    TInt length = title->Length() + value1->Length() + 10;
    if ( aCompulsory )
        {
        length += 3;
        }
    HBufC* itemText = HBufC::NewLC( length );

    // handle exotic language number conversion here
    TPtr t(title->Des());
    TPtr v(value1->Des());
    AknTextUtils::LanguageSpecificNumberConversion( t );
    AknTextUtils::LanguageSpecificNumberConversion( v );
    
    // Define a modifiable pointer descriptor to be able to append text to the
    // non-modifiable heap descriptor itemText
    TPtr itemTextPtr = itemText->Des();
    itemTextPtr.Format( KTxtListItemFormat, title, value1 );
    if ( aCompulsory )
        {
        itemTextPtr.Append( KTxtCompulsory );
        }
    CleanupStack::Pop();    // itemtext,
    if ( Pushed )
        {
        // "Dynamic" text from resource if needed
        CleanupStack::PopAndDestroy();
        }
    // title, value, value1.  !value is allocated with LC in both cases...!
    CleanupStack::PopAndDestroy( 3 );
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::CreateTextualListBoxItemL")
    return itemText;
    }



//----------------------------------------------------------
// CApSettingsDlg::CreateOnOffListBoxItemL
//----------------------------------------------------------
//
HBufC*  CApSettingsDlg::CreateOnOffListBoxItemL( TApMember aMember, TInt aRes,
                                                 TInt aCompulsory )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::CreateOnOffListBoxItemL")
    
    // Read up title text from resource
    HBufC* title = iEikonEnv->AllocReadResourceLC( aRes );

    TUint32 aValueResourceID = 0;
    TBool bvar;

    switch ( aMember )
        {
        case EApIspBearerCallTypeIsdn:
            {// Data call type "Analogue"/"ISDN"
            TUint32 tempint;
            iApItem->ReadUint( EApIspBearerCallTypeIsdn, tempint );
            switch ( tempint )
                {
                case ECallTypeISDNv110:
                    {
                    aValueResourceID = R_APUI_VIEW_DATACALL_ISDN_110;
                    break;
                    }
                case ECallTypeISDNv120:
                    {
                    aValueResourceID = R_APUI_VIEW_DATACALL_ISDN_120;
                    break;
                    }
                case ECallTypeAnalogue:
                default:
                    {
                    aValueResourceID = R_APUI_VIEW_DATACALL_ANALOLGUE;
                    break;
                    }
                }
            break;
            }
        case EApWapWspOption:
            { // "Continuous"/"Temporary"
            TUint32 tval;
            iApItem->ReadUint( aMember, tval );
            if ( tval ==  EWapWspOptionConnectionOriented )
                {
                aValueResourceID = R_APUI_VIEW_CONN_TYPE_CONTIN;
                }
            else
                {
                aValueResourceID = R_APUI_VIEW_CONN_TYPE_TEMP;
                }
            break;
            }
        case EApIspIfCallbackType:
            { // "Use server number"/"Use other number"
            TUint32 tval;
            iApItem->ReadUint( aMember, tval );
            if ( tval == ECallbackActionMSCBCPAcceptServerSpecifiedNumber )
                {
                aValueResourceID = R_APUI_VIEW_CB_USE_SERVER_NUMBER;
                }
            else
                {
                if ( tval == 
                        ECallbackActionMSCBCPRequireClientSpecifiedNumber )
                    {
                    aValueResourceID = R_APUI_VIEW_CB_USE_OTHER_NUMBER;
                    }
                }
            break;
            }
        case EApWapSecurity:
            { // on/off
            iApItem->ReadBool( aMember, bvar );
            if ( bvar )
                {
                aValueResourceID = R_APUI_VIEW_WTLS_SECURITY_ON;
                }
            else
                {
                aValueResourceID = R_APUI_VIEW_WTLS_SECURITY_OFF;
                }
            break;
            }
        case EApIspUseLoginScript:
            {
            iApItem->ReadBool( aMember, bvar );
            if ( bvar )
                {
                aValueResourceID = R_APUI_VIEW_USE_LOGINS_YES;
                }
            else
                {
                aValueResourceID = R_APUI_VIEW_USE_LOGINS_NO;
                }
            break;
            }

        case EApIspEnableSWCompression:
        case EApGprsHeaderCompression:
            {
            iApItem->ReadBool( aMember, bvar );
            if ( bvar )
                {
                aValueResourceID = R_APUI_VIEW_PPP_COMP_YES;
                }
            else
                {
                aValueResourceID = R_APUI_VIEW_PPP_COMP_NO;
                }
            break;
            }
        case EApIspPromptForLogin:
            {
            iApItem->ReadBool( aMember, bvar );
            if ( bvar )
                {
                aValueResourceID = R_APUI_VIEW_PROMPT_PASSWD_YES;
                }
            else
                {
                aValueResourceID = R_APUI_VIEW_PROMPT_PASSWD_NO;
                }
            break;
            }
        case EApIspIfCallbackEnabled:
            { // yes/no
            iApItem->ReadBool( aMember, bvar );
            if ( bvar )
                {
                aValueResourceID = R_APUI_VIEW_USE_CB_YES;
                }
            else
                {
                aValueResourceID = R_APUI_VIEW_USE_CB_NO;
                }
            break;
            }
        case EApWapCurrentBearer:
            { // "Packet data"/"SMS"/"Data call"
            if ( iBearerType == EApBearerTypeCSD )
                {
                aValueResourceID = R_APUI_VIEW_BEARER_TYPE_CSD;
                }
            else
                {
                if ( iBearerType == EApBearerTypeHSCSD )
                    {
                    aValueResourceID = R_APUI_VIEW_BEARER_TYPE_HSCSD;
                    }
                else
                    {
                    if ( iBearerType == EApBearerTypeGPRS )
                        {
                        aValueResourceID = R_APUI_VIEW_BEARER_TYPE_GPRS;
                        }
                    else
                        {
                        if ( IsWlanSupported() )
                            {
                            if ( iBearerType == EApBearerTypeWLAN )
                                {
                                aValueResourceID = 
                                            R_APUI_VIEW_BEARER_TYPE_WLAN;
                                }
                            else
                                {
                                User::Leave( KErrInvalidBearerType );
                                }
                            }
                        else
                            {    
                            User::Leave( KErrInvalidBearerType );
                            }
                        }
                    }
                }
            break;
            }
        case EApGprsDisablePlainTextAuth:
        case EApIspDisablePlainTextAuth:
            { // "Normal"/"Secure"
            iApItem->ReadBool( aMember, bvar );
            if ( bvar )
                {
                aValueResourceID = R_APUI_VIEW_PASSWORD_AUTH_SECURE;
                }
            else
                {
                aValueResourceID = R_APUI_VIEW_PASSWORD_AUTH_NORMAL;
                }
            break;
            }
        case EApIspBearerSpeed:
            {
            TUint32 uvar;
            iApItem->ReadUint( aMember, uvar );
            switch ( TApCallSpeed( uvar ) )
                {
                case KSpeedAutobaud:
                    {
                    aValueResourceID = R_APUI_VIEW_SPEED_AUTODETECT;
                    break;
                    }
                case KSpeed9600:
                    {
                    aValueResourceID = R_APUI_VIEW_SPEED_9600;
                    break;
                    }
                case KSpeed14400:
                    {
                    aValueResourceID = R_APUI_VIEW_SPEED_14400;
                    break;
                    }
                case KSpeed19200:
                    {
                    aValueResourceID = R_APUI_VIEW_SPEED_19200;
                    break;
                    }
                case KSpeed28800:
                    {
                    aValueResourceID = R_APUI_VIEW_SPEED_28800;
                    break;
                    }
                case KSpeed38400:
                    {
                    aValueResourceID = R_APUI_VIEW_SPEED_38400;
                    break;
                    }
                case KSpeed43200:
                    {
                    aValueResourceID = R_APUI_VIEW_SPEED_43200;
                    break;
                    }
                case KSpeed56000: 
                    {
                    aValueResourceID = R_APUI_VIEW_SPEED_56000;
                    break;
                    }
                default:
                    {
                    // auto-repair corrupted value
                    // use 9600 if previous data was not valid...
                    aValueResourceID = R_APUI_VIEW_SPEED_9600;
                    }
                }

            break;
            }
        case EApGprsPdpType:
            {
            TUint32 uvar( 0 );
            iApItem->ReadUint( aMember, uvar );
            if ( uvar == EIPv6 )
                {
                aValueResourceID = R_APUI_VIEW_PDP_TYPE_IPV6;
                }
            else
                {
                aValueResourceID = R_APUI_VIEW_PDP_TYPE_IPV4;
                }
            break;
            }
        case EApIP6DNSAddrFromServer:
            {
            // Change the 'magic numbers'
            // Add the three possibilities here
            iApItem->ReadBool( aMember, bvar );
            TInt stype = GetDomainNameServersTypeL();

            switch ( stype )
                {
                case 0:
                    {
                    aValueResourceID = 
                        R_APUI_VIEW_NEW_AP_DNS_SERVERS_AUTOMATIC;
                    break;
                    }
                case 1:
                    {
                    aValueResourceID = R_APUI_VIEW_SET_IP_WELL_KNOWN;
                    break;
                    }
                case 2:
                    {
                    aValueResourceID = R_APUI_VIEW_SET_IP_USER_DEFINED;
                    break;
                    }
                default:
                    {
                    __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
                    break;
                    }
                }
            break;
            }
        case EApWlanNetworkMode:
            {
            TUint32 netmode( 0 );
            iApItem->ReadUint( aMember, netmode );
            if ( netmode == EAdhoc )
                {
                aValueResourceID = R_APUI_VIEW_WLAN_NETWORK_MODE_ADHOC;
                }
            else
                {
                aValueResourceID = R_APUI_VIEW_WLAN_NETWORK_MODE_INFRA;
                }
            break;
            }
        case EApWlanSecurityMode:
            {
            TUint32 secmode( 0 );
            iApItem->ReadUint( aMember, secmode );
            switch ( secmode )
                {
                case EOpen:
                    {
                    aValueResourceID = R_APUI_VIEW_WLAN_SECURITY_MODE_OPEN;
                    break;
                    }
                case EWep:
                    {
                    aValueResourceID = R_APUI_VIEW_WLAN_SECURITY_MODE_WEP;
                    break;
                    }
                case E802_1x:
                    {
                    aValueResourceID = R_APUI_VIEW_WLAN_SECURITY_MODE_802_1X;
                    break;
                    }
                case EWpa:
                case EWpa2:
                    {
                    aValueResourceID = R_APUI_VIEW_WLAN_SECURITY_MODE_WPA;
                    break;
                    }
                default:
                    {
                    aValueResourceID = R_APUI_VIEW_WLAN_SECURITY_MODE_OPEN;
                    break;
                    }
                }

            break;
            }
        case EApWlanScanSSID:
            { // yes/no
            iApItem->ReadBool( aMember, bvar );
            if ( bvar )
                {
                aValueResourceID = R_APUI_VIEW_HIDDEN_NETWORK_YES;
                }
            else
                {
                aValueResourceID = R_APUI_VIEW_HIDDEN_NETWORK_NO;
                }
            break;
            }
        case EApWlanChannelId:
            { // ad-hoc channel auto / user defined
            TUint32 channel(0);
            iApItem->ReadUint( aMember, channel );
            if ( channel )
                {
                aValueResourceID = R_APUI_VIEW_AD_HOC_CHANNEL_USER_DEFINED;
                }
            else
                {
                aValueResourceID = R_APUI_VIEW_AD_HOC_CHANNEL_AUTO;
                }
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
            break;
            }
        }

    HBufC* value;
    if ( aValueResourceID )
        {
        // Read up value text from resource
        value = iEikonEnv->AllocReadResourceLC( aValueResourceID );
        }
    else
        {
        value = HBufC::NewLC( 0 );
        }


    // Define a heap descriptor to hold all the item text
    TInt length = title->Length() + value->Length() + 10 ;
    if ( aCompulsory )
        {
        length += 3;
        }

    HBufC* itemText = HBufC::NewLC( length);

    // Define a modifiable pointer descriptor to be able to append text to the
    // non-modifiable heap descriptor itemText
    TPtr itemTextPtr = itemText->Des();
    itemTextPtr.Format( KTxtListItemFormat, title, value);
    if ( aCompulsory )
        {
        itemTextPtr.Append( KTxtCompulsory );
        }

    CleanupStack::Pop();    // itemText
    CleanupStack::PopAndDestroy( 2 ); // value, title
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::CreateOnOffListBoxItemL")
    return itemText;
    }



//----------------------------------------------------------
// CApSettingsDlg::ChangeSettingsL
//----------------------------------------------------------
//
void CApSettingsDlg::ChangeSettingsL( TBool aQuick )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::ChangeSettingsL")
    
#ifdef __TEST_OOMDEBUG
    if ( iMemTestOn )
        {
        TInt leavecode( KErrNoMemory );

        TInt ts( GetTestStateL() );

        if ( ts == KEditingTest )
            {
            TBuf<48> buf;
            APSETUILOGGER_WRITE_BUF( _L("Starting Editing memtest") );

            TUint32 rate( 0 );
            TUint32 retrycount( KOOMRetryCount );
            TUint32 retryoffset( KOOMRetryOffset );
            for (
                rate = 1;
                ((rate < 50000) && (leavecode == KErrNoMemory)) || retrycount;
                rate++
                )
                {
                __UHEAP_SETFAIL( RHeap::EDeterministic, rate );
                TRAP( leavecode, DoChangeSettingsL( aQuick ) );
                __UHEAP_RESET;
                if ( !leavecode )
                    {
                    retrycount--;
                    rate += retryoffset;
                    }
                }
            User::LeaveIfError( leavecode );
            buf.Format( _L("\t Memtest finished, max. rate was: %d )"), rate );
            APSETUILOGGER_WRITE_BUF( buf );
            }
        else
            {
            DoChangeSettingsL( aQuick );
            }
        }
    else
        {
        DoChangeSettingsL( aQuick );
        }
#else
    DoChangeSettingsL( aQuick );
#endif // __TEST_OOMDEBUG
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::ChangeSettingsL")
    }



//----------------------------------------------------------
// CApSettingsDlg::SetBearerTypeL
//----------------------------------------------------------
//
void CApSettingsDlg::SetBearerTypeL( TApBearerType aBearer )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::SetBearerTypeL")
    
    iBearerType = aBearer;

    HandleListStructureChangeL();

    APSETUILOGGER_LEAVEFN( ESettings,"Settings::SetBearerTypeL")
    }



//----------------------------------------------------------
// CApSettingsDlg::InvertSettingsL
//----------------------------------------------------------
//
void CApSettingsDlg::InvertSettingsL( TApMember aDataMember )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::InvertSettingsL")
    
    switch ( aDataMember )
        {
        case EApIspIfCallbackType:
            {
            TUint32 ctype( ECallbackActionMSCBCPRequireClientSpecifiedNumber );
            iApItem->ReadUint( EApIspIfCallbackType, ctype );
            if ( ctype == ECallbackActionMSCBCPRequireClientSpecifiedNumber )
                {
                iApItem->WriteUint( EApIspIfCallbackType, 
                         ECallbackActionMSCBCPAcceptServerSpecifiedNumber );
                }
            else
                {
                iApItem->WriteUint( EApIspIfCallbackType, 
                         ECallbackActionMSCBCPRequireClientSpecifiedNumber );
                }            
            break;
            }
        case EApGprsPdpType:
            {
            TUint32 ipnettype( 0 );
            iApItem->ReadUint( EApGprsPdpType, ipnettype );
            if ( ipnettype == EIPv6 )
                {
                iApItem->WriteUint( EApGprsPdpType, EIPv4 );
                }
            else
                {
                iApItem->WriteUint( EApGprsPdpType, EIPv6 );                
                }
            break;
            }
        case EApWlanNetworkMode:
            {
            TUint32 netmode( 0 );
            iApItem->ReadUint( EApWlanNetworkMode, netmode );
            if ( netmode == EAdhoc )
                {
                iApItem->WriteUint( EApWlanNetworkMode, EInfra );
                }
             else
                {
                iApItem->WriteUint( EApWlanNetworkMode, EAdhoc );
                LimitSecMode();
                }            
            break;
            }
        case EApWapWspOption:
            {
            TUint32 intvar;
            if ( iApItem->ReadUint( aDataMember, intvar ) == KErrNone )
                {
                if ( intvar == EWapWspOptionConnectionless )
                    {
                    intvar = EWapWspOptionConnectionOriented;
                    }
                else
                    {
                    intvar = EWapWspOptionConnectionless;
                    }
                iApItem->WriteUint( aDataMember, intvar );
                }
            break;
            }
        default:
            {
            TBool bvar;
            if ( iApItem->ReadBool( aDataMember, bvar ) == KErrNone )
                {
                bvar = !bvar;
                iApItem->WriteBool( aDataMember, bvar );
                }
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::InvertSettingsL")
    }


//----------------------------------------------------------
// CApSettingsDlg::ShowPopupSettingPageL
//----------------------------------------------------------
//
TBool CApSettingsDlg::ShowPopupSettingPageL( TApMember aData )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::ShowPopupSettingPageL")
    
    TInt currvalue( 0 );
    TBool retval( EFalse );
    TBool mustrepeat( EFalse );

    CDesCArrayFlat* items = FillPopupSettingPageLC( aData,  currvalue );

    TInt attr_resid( 0 );
    GetResId( aData, attr_resid );
    
    do
        {
        HBufC* titlebuf;
        CAknRadioButtonSettingPage* dlg;
        if ( attr_resid )
            {
            titlebuf = iEikonEnv->AllocReadResourceLC( attr_resid );
            dlg = new ( ELeave )CAknRadioButtonSettingPage(
                                            R_RADIO_BUTTON_SETTING_PAGE, 
                                            currvalue, 
                                            items );
            // must push 'cause SetSettingTextL can leave...
            CleanupStack::PushL( dlg ); 
            TPtrC ptr( titlebuf->Des() );
            dlg->SetSettingTextL( ptr );
            CleanupStack::Pop(); // dlg
            }
        else
            {
            dlg = new ( ELeave )CAknRadioButtonSettingPage( 
                                            R_RADIO_BUTTON_SETTING_PAGE, 
                                            currvalue, 
                                            items );
            }
        if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted ) )
            {
            if( AskQueryIfWcdmaL( aData , currvalue) )
                {
                UpdateFromPopupSettingPageL( aData, currvalue );
                retval = ETrue;
                mustrepeat = EFalse;
                }
            else
                {
                mustrepeat = ETrue;
                }
            }
        else
            {
            mustrepeat = EFalse;
            }
        if ( attr_resid )
            {
            // titlebuf, text title readed from resource...
            CleanupStack::PopAndDestroy();
            }
        }while( mustrepeat );


    // items, will also delete all elements in the array!
    CleanupStack::PopAndDestroy( items );
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::ShowPopupSettingPageL")
    return retval;
    }







//----------------------------------------------------------
// CApSettingsDlg::ShowPopupTextSettingPageL
//----------------------------------------------------------
//
TBool CApSettingsDlg::ShowPopupTextSettingPageL( TApMember aData )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::ShowPopupTextSettingPageL")
    
    TBool retval( EFalse );

    // must be prepared for the worst case...
    TBuf<KModifiableLongTextLength> textToChange;

    HBufC* aBuf1 = GetTextLC( aData );

    HBufC* aBuf = ConvertPerNToDelimLC( aBuf1->Des() );

    TPtr16 ptr( aBuf->Des() );
    textToChange.Append( ptr );

    TInt page_resid( 0 );
    TInt TextSettingPageFlags( EAknSettingPageNoOrdinalDisplayed );
    GetTextResId( aData, page_resid, TextSettingPageFlags );


    TBool isLatin ( aData == EApWapStartPage );

    CAccessPointTextSettingPage* dlg =
        new( ELeave )CAccessPointTextSettingPage( page_resid, textToChange,
                                                  TextSettingPageFlags,
                                                  isLatin );

    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted ) )
        {
        switch ( aData )
            {
            case EApWapStartPage:
            case EApGprsAccessPointName:
            case EApIspLoginScript:
            case EApIspInitString:
            case EApWapAccessPointName:
            case EApIspLoginName:
            case EApIspLoginPass:
            case EApIspIfAuthName:
            case EApIspIfAuthPass:
                {
                HBufC* tmp = ConvertDelimToPerNLC( textToChange );
                textToChange.Zero();
                textToChange.Append( tmp->Des() );
                CleanupStack::PopAndDestroy( tmp );
                break;
                }
            case EApProxyServerAddress:
                {
                textToChange.Trim();
                break;
                }
            default:
                { // no need for conversion
                break;
                }
            }
        if ( ( aData == EApIspLoginScript ) || ( aData == EApWapStartPage ) 
            || ( aData == EApGprsAccessPointName ) 
            || ( aData == EApProxyServerAddress ) )
            {
            iApItem->WriteLongTextL( aData, textToChange );
            }
        else
            {
            if ( aData == EApProxyPortNumber )
                {
                // first remove non-number chars
                HBufC* num = StripNonNumberLC( textToChange );
                textToChange.Zero();
                textToChange.Append( num->Des() );
                CleanupStack::PopAndDestroy( num );
                // and now get real numberic value
                TInt tempint;
                TLex lex;
                lex.Assign( textToChange );
                if ( lex.Val( tempint ) )
                    { // this means some error, set it to 0
                    tempint = 0;
                    }
                iApItem->WriteUint( EApProxyPortNumber, tempint );
                }
            else
                {
                if ( ( aData == EApIspInitString ) )
                    {
                    // must be prepared for the worst case...
                    TBuf8<KModifiableLongTextLength> textToChange8;
                    textToChange8.Copy( textToChange );
                    iApItem->WriteTextL( aData, textToChange8 );
                    }
                else
                    {
                    if ( aData == EApWapAccessPointName )
                        {
                        iApItem->SetNamesL( textToChange );
                        }
                    else
                        {
                        iApItem->WriteTextL( aData, textToChange );
                        }
                    }
                }
            }
        retval = ETrue;
        }
    *iEventStore |= KApUiEventEdited;
    CleanupStack::PopAndDestroy( 2 ); // aBuf, aBuf1
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::ShowPopupTextSettingPageL")
    return retval;
    }



//----------------------------------------------------------
// CApSettingsDlg::GetTextLC
//----------------------------------------------------------
//
HBufC* CApSettingsDlg::GetTextLC( TApMember aData )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::GetTextLC")
    
    HBufC* ret;
    switch ( aData )
        {
        case EApWapStartPage:
            { // Long text !!!
            if ( iApItem->ReadTextLengthL( aData ) )
                {
                // allocates as much as needed...
                ret = iApItem->ReadConstLongTextL( aData )->AllocLC();
                }
            else
                {
                ret = iEikonEnv->AllocReadResourceLC(
                                        R_APUI_VIEW_WAP_START_PAGE_DEFAULT );
                }
            break;
            }
        case EApGprsAccessPointName:
            {
            // allocates as much as needed...
            ret = iApItem->ReadConstLongTextL( aData )->AllocLC();
            break;
            }
        case EApIspLoginScript:
            {
            // allocates as much as needed...
            ret = iApItem->ReadConstLongTextL( aData )->AllocLC();
            break;
            }
        case EApProxyServerAddress:
            {
            // allocates as much as needed...
            ret = iApItem->ReadConstLongTextL( aData )->AllocLC();
            break;
            }
        case EApWapGatewayAddress:
        case EApIspIPAddr:
        case EApIspIPNetMask:
        case EApWlanIpNetMask:
        case EApIspIPGateway:
        case EApWlanIpGateway:
        case EApGprsIPNameServer1:
        case EApIspIPNameServer1:
        case EApGprsIPNameServer2:
        case EApIspIPNameServer2:
            {
            ret = HBufC::NewLC( KModifiableTextLength );
            TPtr16 ptr( ret->Des() );
            iApItem->ReadTextL( aData, ptr );
            if ( ptr.Compare( KEmptyText ) == 0 )
                {
                *ret = KDynIpAddress;
                }
            break;
            }
        case EApIspInitString:
            {
            ret = HBufC::NewLC( KModifiableTextLength );
            HBufC8* tmpValue8 = HBufC8::NewLC( KModifiableTextLength );
            TPtr8 ptr( tmpValue8->Des() );
            iApItem->ReadTextL( aData, ptr );
            ret->Des().Copy( ptr );
            CleanupStack::PopAndDestroy(); // tmpValue8
            break;
            }
        case EApProxyPortNumber:
            {
            TUint32 tempint;
            iApItem->ReadUint( EApProxyPortNumber, tempint );
            ret = HBufC::NewLC( KMaxProxyPortNumberLength );
            TPtr itemTextPtr = ret->Des();
            itemTextPtr.Format( KTxtNumber, tempint );
            break;
            }
        default:
            {
            ret = HBufC::NewLC( KModifiableTextLength );
            TPtr16 ptr( ret->Des() );
            iApItem->ReadTextL( aData, ptr );
            break;
            }
        }

    switch ( aData )
        {
        case EApWapStartPage:
        case EApGprsAccessPointName:
        case EApIspLoginScript:
        case EApIspInitString:
        case EApWapAccessPointName:
        case EApIspLoginName:
        case EApIspLoginPass:
        case EApIspIfAuthName:
        case EApIspIfAuthPass:
            {
            break;
            }
        default:
            {
            // nothing to do
            break;
            }
        }


    APSETUILOGGER_LEAVEFN( ESettings,"Settings::GetTextLC")
    return ret;
    }


//----------------------------------------------------------
// CApSettingsDlg::GetTextResId
//----------------------------------------------------------
//
void CApSettingsDlg::GetTextResId( TApMember aData, TInt& apage, 
                                   TInt& aTextSettingPageFlags )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::GetTextResId")
    
    if ( ( aData != EApWapAccessPointName )
         && ( aData != EApIspDefaultTelNumber ) )
        {
        aTextSettingPageFlags |= CAknTextSettingPage::EZeroLengthAllowed;
        }
    else
        {
        aTextSettingPageFlags &= (~CAknTextSettingPage::EZeroLengthAllowed);
        }

    switch ( aData )
        {
        case EApIspInitString:
            {
            apage = R_TEXT_SETTING_PAGE_MODEM_INIT_STRING;
            break;
            }
        case EApWapAccessPointName:
            {
            apage = R_TEXT_SETTING_PAGE_CONN_NAME;
            break;
            }
        case EApGprsAccessPointName:
            {
            apage = R_TEXT_SETTING_PAGE_GPRSAP_NAME;
            break;
            }
        case EApIspLoginScript:// use EDWIN for script.
            {
            apage = R_TEXT_SETTING_PAGE_LOGIN_SCRIPT;
            break;
            }
        case EApIspLoginName:
        case EApIspIfAuthName:
            {
            apage = R_TEXT_SETTING_PAGE_LOGIN_NAME;
            break;
            }
        case EApWapStartPage:
            {
            apage = R_TEXT_SETTING_PAGE_STARTING_PAGE;
            break;
            }
        // use EDWINS as no such thing as PhoneNumber editor...
        case EApIspDefaultTelNumber:
            {
            apage = R_TEXT_SETTING_PAGE_ACCESS_NUM;
            break;
            }
        case EApIspIfCallbackInfo:
            {
            apage = R_TEXT_SETTING_PAGE_CB_NUM;
            break;
            }
        case EApWapGatewayAddress:
            {
            apage = R_TEXT_SETTING_PAGE_GW_ADDR;
            break;
            }
        case EApProxyServerAddress:
            {
            apage = R_TEXT_SETTING_PAGE_PROXY_SERVER_ADDR;
            break;
            }
        case EApProxyPortNumber:
            {
            apage = R_TEXT_SETTING_PAGE_PROXY_PORT_NUMBER;
            break;
            }
        case EApIspIPAddr:
            {
            apage = R_TEXT_SETTING_PAGE_ISPIP_ADDR;
            break;
            }
        case EApIspIPNetMask:
        case EApWlanIpNetMask:
            {
            apage = R_TEXT_SETTING_PAGE_ISP_NET_MASK;
            break;
            }
        case EApIspIPGateway:
        case EApWlanIpGateway:
            {
            apage = R_TEXT_SETTING_PAGE_ISP_GATEWAY;
            break;
            }            
        case EApGprsIPNameServer1:
        case EApIspIPNameServer1:
            {
            apage = R_TEXT_SETTING_PAGE_NAME_SERV1;
            break;
            }
        case EApGprsIPNameServer2:
        case EApIspIPNameServer2:
            {
            apage = R_TEXT_SETTING_PAGE_NAME_SERV2;
            break;
            }
        case EApIspLoginPass:
        case EApIspIfAuthPass:
            { // use EEikCtSecretEd
            apage = R_TEXT_SETTING_PAGE_LOGIN_PASSWD;
            break;
            }

        case EApIspUseLoginScript:
            {// use EDWIN:
            apage = R_SETTING_APP_EDWIN_LOGIN_SCRIPT;
            break;
            }
        case EApWlanNetworkName:
            {
            apage = R_TEXT_SETTING_PAGE_NETW_NAME;
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
            apage = 0;
            break;
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::GetTextResId")
    }



//----------------------------------------------------------
// CApSettingsDlg::GetResId
//----------------------------------------------------------
//
void CApSettingsDlg::GetResId( TApMember aData, TInt& aattr )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::GetResId")
    
    // not text based ones:
    switch ( aData )
        {
        case EApWapCurrentBearer:
            {
            aattr = R_APUI_VIEW_BEARER_TYPE;
            break;
            }
        case EApWapSecurity:
            {
            aattr = R_APUI_VIEW_WTLS_SECURITY;
            break;
            }
        case EApWapWspOption:
            {
            aattr = R_APUI_VIEW_CONNECTION_TYPE;
            break;
            }
        case EApIspPromptForLogin:
            {
            aattr = R_APUI_VIEW_PROMPT_PASSWORD;
            break;
            }
        case EApGprsDisablePlainTextAuth:
        case EApIspDisablePlainTextAuth:
            {
            aattr = R_APUI_VIEW_PASSWORD_AUTH;
            break;
            }
        case EApIspBearerCallTypeIsdn: // used for data call type
            {
            aattr = R_APUI_VIEW_DATA_CALL_TYPE;
            break;
            }
        case EApIspBearerSpeed:
            {
            aattr = R_APUI_VIEW_MAX_CONN_SPEED;
            break;
            }
        case EApIspIfCallbackEnabled:
            {
            aattr = R_APUI_VIEW_USE_CALL_BACK;
            break;
            }
        case EApIspIfCallbackType:
            {
            aattr = R_APUI_VIEW_CALL_BACK_TYPE;
            break;
            }
        case EApIspEnableSWCompression:
        case EApGprsHeaderCompression:
            {
            aattr = R_APUI_VIEW_ENABLE_PPP_COMP;
            break;
            }
        case EApIspInitString:
            {
            aattr = R_APUI_VIEW_MODEM_INIT_STRING;
            break;
            }
        case EApIspUseLoginScript:
            {
            aattr = R_APUI_VIEW_USE_LOGIN_SCRIPT;
            break;
            }
        case EApGprsPdpType:
            {
            aattr = R_APUI_VIEW_PDP_TYPE;
            break;
            }
        case EApIP6DNSAddrFromServer:
            {
            aattr = R_APUI_VIEW_SET_DNS_SERVERS_IP;
            break;
            }
        case EApWlanNetworkName:
            {
            aattr = R_APUI_VIEW_WLAN_NETWORK_NAME;
            break;
            }
        case EApWlanNetworkMode:
            {
            aattr = R_APUI_VIEW_WLAN_NETWORK_MODE;
            break;
            }
        case EApWlanSecurityMode:
            {
            aattr = R_APUI_VIEW_WLAN_SECURITY_MODE;
            break;
            }
        case EApWlanScanSSID:
            {
            aattr = R_APUI_VIEW_WLAN_HIDDEN_NETWORK;
            break;
            }
        case EApWlanChannelId:
            {
            aattr = R_APUI_VIEW_AD_HOC;
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
            aattr = 0;
            break;
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::GetResId")
    }




// ---------------------------------------------------------
// CApSettingsDlg::FillPopupSettingPageL
// ---------------------------------------------------------
//
CDesCArrayFlat* CApSettingsDlg::FillPopupSettingPageLC( TApMember aData, 
                                                        TInt& aCurrvalue )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::FillPopupSettingPageLC")
    
    CDesCArrayFlat* items = new( ELeave)CDesCArrayFlat( 1 );
    CleanupStack::PushL( items );

    TUint32 tval( 0 );

    switch ( aData )
        {
        case EApIspPromptForLogin:
            {
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_PROMPT_PASSWD_YES ) );
            CleanupStack::PopAndDestroy();
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_PROMPT_PASSWD_NO ) );
            CleanupStack::PopAndDestroy();
            GetBoolDataValue( aData, ETrue, aCurrvalue );
            break;
            }
        case EApWapSecurity:
            {
            items->AppendL( *iEikonEnv->AllocReadResourceLC(
                                        R_APUI_VIEW_WTLS_SECURITY_ON ) );
            CleanupStack::PopAndDestroy();
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_WTLS_SECURITY_OFF ) );
            CleanupStack::PopAndDestroy();
            GetBoolDataValue( aData, ETrue, aCurrvalue );
            break;
            }
        case EApIspIfCallbackEnabled:
            {
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_USE_CB_YES ) );
            CleanupStack::PopAndDestroy();
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_USE_CB_NO ) );
            CleanupStack::PopAndDestroy();
            GetBoolDataValue( aData, ETrue, aCurrvalue );
            break;
            }
        case EApIspEnableSWCompression:
        case EApGprsHeaderCompression:
            {
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_PPP_COMP_YES  )  );
            CleanupStack::PopAndDestroy();
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_PPP_COMP_NO ) );
            CleanupStack::PopAndDestroy();
            GetBoolDataValue( aData, ETrue, aCurrvalue );
            break;
            }
        case EApWapCurrentBearer:
            {
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_BEARER_TYPE_GPRS ) );
            CleanupStack::PopAndDestroy();
            
            if ( IsWlanSupported() )
                {
                items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                            R_APUI_VIEW_BEARER_TYPE_WLAN ) );
                CleanupStack::PopAndDestroy();
                }

            TBool isCsdSupported = 
                  FeatureManager::FeatureSupported( KFeatureIdAppCsdSupport );
#ifdef __TEST_CSD_SUPPORT
            isCsdSupported = ETrue;
#endif // __TEST_CSD_SUPPORT

            if ( isCsdSupported )
                {
                items->AppendL( *iEikonEnv->AllocReadResourceLC(
                                            R_APUI_VIEW_BEARER_TYPE_CSD ) );
                CleanupStack::PopAndDestroy();
                
                if ( IsHSCSDEnabledL() )
                    {
                    items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                            R_APUI_VIEW_BEARER_TYPE_HSCSD ) );
                    CleanupStack::PopAndDestroy();
                    }
                }
            aCurrvalue = BearerType2BearerItemPosL( iBearerType );
            break;
            }
        case EApGprsDisablePlainTextAuth:
        case EApIspDisablePlainTextAuth:
            {
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_PASSWORD_AUTH_NORMAL ) );
            CleanupStack::PopAndDestroy();
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_PASSWORD_AUTH_SECURE ) );
            CleanupStack::PopAndDestroy();
            GetBoolDataValue( aData, EFalse, aCurrvalue );
            // needs to be inverted?
            break;
            }
        case EApWapWspOption:
            {
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_CONN_TYPE_CONTIN ) );
            CleanupStack::PopAndDestroy();
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_CONN_TYPE_TEMP ) );
            CleanupStack::PopAndDestroy();
            iApItem->ReadUint( aData, tval );
            aCurrvalue = Min( 1, tval );
            aCurrvalue = !aCurrvalue;
            break;
            }
        case EApIspBearerCallTypeIsdn: // used for data call type
            {
            items->AppendL( *iEikonEnv->AllocReadResourceLC(
                                        R_APUI_VIEW_DATACALL_ANALOLGUE ) );
            CleanupStack::PopAndDestroy();
            items->AppendL( *iEikonEnv->AllocReadResourceLC(
                                        R_APUI_VIEW_DATACALL_ISDN_110 ) );
            CleanupStack::PopAndDestroy();
            items->AppendL( *iEikonEnv->AllocReadResourceLC(
                                        R_APUI_VIEW_DATACALL_ISDN_120 ) );
            CleanupStack::PopAndDestroy();
            iApItem->ReadUint( EApIspBearerCallTypeIsdn, tval );
            switch ( tval )
                {
                case ECallTypeISDNv110:
                    {
                    aCurrvalue = KIspCallIsdn110;
                    break;
                    }
                case ECallTypeISDNv120:
                    {
                    aCurrvalue = KIspCallIsdn120;
                    break;
                    }
                default:
                case ECallTypeAnalogue:
                    {
                    aCurrvalue = KIspCallAnalogue;
                    break;
                    }
                }
            break;
            }
        case EApIspBearerSpeed:
            {
            FillUpBearerSpeedValuesL( *items, aCurrvalue );
            break;
            }
        case EApIspIfCallbackType:
            {
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_CB_USE_SERVER_NUMBER ) );
            CleanupStack::PopAndDestroy();
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_CB_USE_OTHER_NUMBER ) );
            CleanupStack::PopAndDestroy();
            iApItem->ReadUint( aData, tval );
            if ( tval == ECallbackActionMSCBCPRequireClientSpecifiedNumber )
                {
                aCurrvalue = 1;
                }
            else
                {
                aCurrvalue = 0;
                }
            break;
            }
        case EApIspUseLoginScript:
            {
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_USE_LOGINS_YES ) );
            CleanupStack::PopAndDestroy();
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_USE_LOGINS_NO ) );
            CleanupStack::PopAndDestroy();
            GetBoolDataValue( aData, ETrue, aCurrvalue );
            break;
            }
        case EApGprsPdpType:
            {
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_PDP_TYPE_IPV4 ) );
            CleanupStack::PopAndDestroy();
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_PDP_TYPE_IPV6 ) );
            CleanupStack::PopAndDestroy();
            iApItem->ReadUint( aData, tval );
            if ( tval == EIPv6 )
                {
                aCurrvalue = 1;
                }
            else
                {
                aCurrvalue = 0;
                }
            break;
            }
        case EApIP6DNSAddrFromServer:
            {
            TApBearerType b = iApItem->BearerTypeL();
            TBool hasAuto = ( b != EApBearerTypeCSD ) 
                            && ( b != EApBearerTypeHSCSD ) ;
            if ( hasAuto )
                {                
                items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                R_APUI_VIEW_NEW_AP_DNS_SERVERS_AUTOMATIC ) );
                CleanupStack::PopAndDestroy();
                }
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_SET_IP_WELL_KNOWN ) );
            CleanupStack::PopAndDestroy();
            items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_SET_IP_USER_DEFINED ) );
            CleanupStack::PopAndDestroy();
            TBool bval;
            iApItem->ReadBool( aData, bval );
            if ( !bval )
                {
                aCurrvalue = GetIPv6DNSTypeL();
                if ( !hasAuto )
                    { // no Auto item, dec. currvalue
                    aCurrvalue--;
                    }
                }
            else
                {
                aCurrvalue = 0;
                }
            break;
            }
        case EApWlanNetworkName:
            {
            if ( IsWlanSupported() )
                {            
                items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_WLAN_NETWORK_NAME_USER ) );
                CleanupStack::PopAndDestroy();
                items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                        R_APUI_VIEW_WLAN_NETWORK_NAME_SCAN ) );
                CleanupStack::PopAndDestroy();
                // set current value as user defined: if not yet given, 
                // it is true, if it had been already specified, 
                // we could not know if it was the user or was it scanned
                aCurrvalue = 0;
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
        case EApWlanNetworkMode:
            {
            if ( IsWlanSupported() )
                {
                items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                    R_APUI_VIEW_WLAN_NETWORK_MODE_INFRA ) );
                CleanupStack::PopAndDestroy();
                items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                    R_APUI_VIEW_WLAN_NETWORK_MODE_ADHOC ) );
                CleanupStack::PopAndDestroy();
                iApItem->ReadUint( aData, tval );
                if ( tval == EAdhoc )
                    {
                    aCurrvalue = 1;
                    }
                else
                    {
                    aCurrvalue = 0;
                    }
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
        case EApWlanScanSSID:
            {
            if ( IsWlanSupported() )
                {
                TBool bval(EFalse);
                items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                    R_APUI_VIEW_HIDDEN_NETWORK_NO ) );
                CleanupStack::PopAndDestroy();
                items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                    R_APUI_VIEW_HIDDEN_NETWORK_YES ) );
                CleanupStack::PopAndDestroy();
                iApItem->ReadBool( aData, bval );
                if ( bval )
                    {
                    aCurrvalue = 1;
                    }
                else
                    {
                    aCurrvalue = 0;
                    }
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
        case EApWlanChannelId:
            {
            if ( IsWlanSupported() )
                {
                TUint32 channel(0);
                items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                R_APUI_VIEW_AD_HOC_CHANNEL_AUTO ) );
                CleanupStack::PopAndDestroy();
                items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                R_APUI_VIEW_AD_HOC_CHANNEL_USER_DEFINED ) );
                CleanupStack::PopAndDestroy();
                iApItem->ReadUint( aData, channel );
                if ( channel )
                    {
                    aCurrvalue = 1;
                    }
                else
                    {
                    aCurrvalue = 0;
                    }
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
        case EApWlanSecurityMode:
            {
            if ( IsWlanSupported() )
                {
                TUint32 netmode(0);
                
                items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                    R_APUI_VIEW_WLAN_SECURITY_MODE_OPEN ) );
                CleanupStack::PopAndDestroy();
                items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                    R_APUI_VIEW_WLAN_SECURITY_MODE_WEP ) );
                CleanupStack::PopAndDestroy();

                iApItem->ReadUint( EApWlanNetworkMode, netmode );
                if ( netmode == EInfra )
                    {
                    items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                    R_APUI_VIEW_WLAN_SECURITY_MODE_802_1X ) );
                    CleanupStack::PopAndDestroy();
                    items->AppendL( *iEikonEnv->AllocReadResourceLC( 
                                    R_APUI_VIEW_WLAN_SECURITY_MODE_WPA ) );
                    CleanupStack::PopAndDestroy();
                    }
                    
                iApItem->ReadUint( aData, tval );
                switch ( tval )
                    {
                    case EOpen:
                        {
                        aCurrvalue = 0;
                        break;
                        }
                    case EWep:
                        {
                        aCurrvalue = 1;
                        break;
                        }
                    case E802_1x:
                        {
                        aCurrvalue = 2;
                        if ( netmode != EInfra )
                            {
                            aCurrvalue--;
                            }
                        break;
                        }
                    case EWpa:
                    case EWpa2:
                        {
                        aCurrvalue = 3;
                        if ( netmode != EInfra )
                            {
                            aCurrvalue-= 2;
                            }
                        break;
                        }
                    default:
                        {
                        aCurrvalue = 0;
                        break;
                        }
                    }
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic ( EUnknownCase ) );
            User::Leave( KErrInvalidColumn );
            break;
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::FillPopupSettingPageLC")
    return items;
    }




// ---------------------------------------------------------
// CApSettingsDlg::UpdateFromPopupSettingPageL
// ---------------------------------------------------------
//
void CApSettingsDlg::UpdateFromPopupSettingPageL( TApMember aData, 
                                                  TInt aCurrvalue )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::UpdateFromPopupSettingPageL")
    
    switch ( aData )
        {
        case EApWapSecurity:
        case EApIspPromptForLogin:
        case EApIspIfCallbackEnabled:
        case EApIspEnableSWCompression:
        case EApGprsHeaderCompression:
        case EApIspUseLoginScript:
            { // INVERTED!
            iApItem->WriteBool( aData, !( TBool( aCurrvalue ) ) );
            break;
            }
        case EApWlanScanSSID:
        case EApGprsDisablePlainTextAuth:
        case EApIspDisablePlainTextAuth:
            { // NOT INVERTED!
            iApItem->WriteBool( aData, ( TBool( aCurrvalue ) ) );
            break;
            }
        case EApWapWspOption:
            {
            if ( aCurrvalue )
                {
                iApItem->WriteUint( EApWapWspOption, 
                                    EWapWspOptionConnectionless );
                }
            else
                {
                iApItem->WriteUint( EApWapWspOption, 
                                    EWapWspOptionConnectionOriented );
                }
            break;
            }
        case EApWapCurrentBearer:
            {
            DoBearerChangeL( aCurrvalue );
            break;
            }
        case EApIspBearerSpeed:
            {
            iApItem->WriteUint( EApIspBearerSpeed,
                                GetBearerSpeedForSelection( aCurrvalue )
                              );
            break;
            }
        case EApIspIfCallbackType:
            { // UINT
            if ( aCurrvalue )
                {
                iApItem->WriteUint( EApIspIfCallbackType, 
                        ECallbackActionMSCBCPRequireClientSpecifiedNumber );
                }
            else
                {
                iApItem->WriteUint( EApIspIfCallbackType, 
                        ECallbackActionMSCBCPAcceptServerSpecifiedNumber );
                }
            break;
            }
        case EApIspBearerCallTypeIsdn: // used for data call type
            {
            // as order is the same in UI and in enum, simply write it
            iApItem->WriteUint( EApIspBearerCallTypeIsdn, aCurrvalue );
            LimitSpeed();
            break;
            }
        case EApGprsPdpType:
            {
            // get value to write
            if ( aCurrvalue )
                {
                aCurrvalue = EIPv6;
                }
            else
                {
                aCurrvalue = EIPv4;
                }
            iApItem->WriteUint( EApGprsPdpType, aCurrvalue );
            break;
            }
        case EApIP6DNSAddrFromServer:
            {
            // if it is CSD or HSCSD, there is NO AUTOMATIC value, 
            // just the next two: Well-known & User defined...
            // So in those cases, aCurrvalue must be 0 & 1, which
            // corresponds to the normal 1, 2 so it should be incremented
            TInt realvalue = aCurrvalue;
            TApBearerType b = iApItem->BearerTypeL();
            if ( ( b == EApBearerTypeCSD ) || ( b == EApBearerTypeHSCSD ) )
                {
                realvalue++;
                }

            if ( realvalue == 2 )
                { // user defined!!!
                TBool isip6( EFalse );
                if ( iApItem->BearerTypeL() == EApBearerTypeGPRS )
                    {
                    TUint32 ival(0);
                    iApItem->ReadUint( EApGprsPdpType, ival );
                    if ( ival == EIPv6 )
                        {
                        isip6 = ETrue;
                        }
                    }
                else
                    {
                    isip6 = !iL2Ipv4;
                    }
                GetNameServersL( isip6 );
                }
            else
                {
                if ( realvalue == 1 )
                    { // well-known
                    // write default well known name server values
                    iApItem->WriteTextL( EApIP6NameServer1, 
                                         KKnownNameServer1 );
                    iApItem->WriteTextL( EApIP6NameServer2, 
                                         KKnownNameServer2 );
                    }
                else
                    {
                    iApItem->WriteTextL( EApIP6NameServer1, KDynIpv6Address );
                    iApItem->WriteTextL( EApIP6NameServer2, KDynIpv6Address );
                    }
                }
            break;
            }
        case EApWlanNetworkMode:
            {
            if ( IsWlanSupported() )
                {
                if ( aCurrvalue )
                    {
                    iApItem->WriteUint( EApWlanNetworkMode, EAdhoc );
                    LimitSecMode();
                    }
                else
                    {
                    iApItem->WriteUint( EApWlanNetworkMode, EInfra );
                    }
                
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
        case EApWlanSecurityMode:
            {
            if ( IsWlanSupported() )
                {
                TInt secmode(EOpen);
                switch ( aCurrvalue  )
                    {
                    case KItemIndex0:
                        {
                        secmode = EOpen;
                        break;
                        }
                    case KItemIndex1:
                        {
                        secmode = EWep;
                        break;
                        }
                    case KItemIndex2:
                        {
                        secmode = E802_1x;
                        break;
                        }
                    case KItemIndex3:
                        {
                        secmode = EWpa;
                        break;
                        }
                    default:
                        {
                        secmode = EOpen;
                        break;
                        }
                    }
                iApItem->WriteUint( EApWlanSecurityMode, secmode );
                }
            else
                {
                User::Leave( KErrNotSupported );
                }                
            break;
            }
        case EApWlanChannelId:
            {
            if ( IsWlanSupported() )
                {
                if ( aCurrvalue )
                    { // user defined, ask number from user, offer 7 as default
                    TUint32 channel = GetAdHocChannelL();
                    iApItem->WriteUint( EApWlanChannelId, channel );
                    }
                else
                    { // auto, write 0
                    iApItem->WriteUint( EApWlanChannelId, aCurrvalue );
                    }
                }
            else
                {
                User::Leave( KErrNotSupported );
                }                
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
            User::Leave( KErrInvalidColumn );
            break;
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::UpdateFromPopupSettingPageL")
    }



// ---------------------------------------------------------
// CApSettingsDlg::CanSaveL( )
// ---------------------------------------------------------
//
CApSettingsDlg::TSaveAction CApSettingsDlg::CanSaveL( TApMember& aDataNeeded )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::CanSaveL")
    
    TSaveAction retval( EApCanSave );

    // it is used in two cases, less code by doing it here
    HBufC *sgd = HBufC::NewLC( KModifiableTextLength );
    TPtr16 ptr( sgd->Des() );

    switch ( iBearerType )
        {
        case EApBearerTypeCSD:
        case EApBearerTypeHSCSD:
            {
            iApItem->ReadTextL( EApIspDefaultTelNumber, ptr );
            if ( *sgd == KEmptyText ) 
                {
                retval = EApMustDelete;
                if ( !( *iEventStore & KApUiEventExitRequested ) )
                    {
                    if ( ! AskQueryL( R_APUI_AP_ACCESS_NUMBER_NOT ) )
                        {
                        retval = EApShallGoBack;
                        aDataNeeded = EApIspDefaultTelNumber;
                        }
                    }
                }
            break;
            }
        case EApBearerTypeGPRS:
            {
            //  nothing to do...
            break;
            }
        case EApBearerTypeWLAN:
            {
            if ( IsWlanSupported() )
                {
                iApItem->ReadTextL( EApWlanNetworkName, ptr );
                if ( *sgd == KEmptyText ) 
                    {
                    retval = EApShallGoBack;
                    aDataNeeded = EApWlanNetworkName;
                    if ( !( *iEventStore & KApUiEventExitRequested ) )
                        {
                        if ( AskQueryL( R_APUI_AP_WLAN_NAME_NOT_DEFINED ) )
                            {
                            retval = EApMustDelete;
                            }
                        }
                    }
                if ( (retval != EApShallGoBack) && (retval != EApMustDelete) 
                     && IsNetMaskAndGatewayVisibleL() )
                    { // now check compulsory fields
                    iApItem->ReadTextL( EApWlanIpNetMask, ptr );
                    if ( ( ptr.Compare( KDynIpAddress ) == 0 ) 
                        || ( ptr.Compare( KEmptyText ) == 0 ) )
                        {
                        retval = EApShallGoBack;
                        aDataNeeded = EApWlanIpNetMask;
                        if ( AskQueryL( 
                            R_APUI_VIEW_NETW_QUEST_IAP_INCOMPLETE_DELETE ) )
                            {
                            retval = EApMustDelete;
                            }                                                
                        }
                    else
                        {                        
                        iApItem->ReadTextL( EApIspIPGateway, ptr );
                        if ( ( sgd->Compare( KDynIpAddress ) == 0 ) 
                            || ( sgd->Compare( KEmptyText ) == 0 ) )
                            {
                            retval = EApShallGoBack;
                            aDataNeeded = EApIspIPGateway;
                            if ( AskQueryL( 
                                R_APUI_VIEW_NETW_QUEST_IAP_INCOMPLETE_DELETE )
                               )
                                {
                                retval = EApMustDelete;
                                }                            
                            }
                        }
                    }
                if ( (retval != EApShallGoBack) && (retval != EApMustDelete) 
                     && (!HasSecuritySettingsFilledL()) )
                    {
                    retval = EApShallGoBack;
                    aDataNeeded = EApWlanSecuritySettings;
                    if ( !( *iEventStore & KApUiEventExitRequested ) )
                        {
                        if ( AskQueryL( R_APUI_AP_WLAN_SEC_SETT_EMPTY ) )
                            {
                            retval = EApMustDelete;
                            }
                        }
                    }
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;            
            }
        default:
            { // in release, leave, in debug, PANIC
            __ASSERT_DEBUG( EFalse, Panic( EInvalidBearerType ) );
            User::Leave( KErrInvalidBearerType );
            break;
            }
        }
    CleanupStack::PopAndDestroy( sgd );  // sgd

    if ( ( retval == EApShallGoBack ) && 
         ( *iEventStore & KApUiEventExitRequested ) )
        {
        retval = EApNoAction;
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::CanSaveL")
    return retval;
    }






//----------------------------------------------------------
// CApSettingsDlg::ShowPopupIpAddrSettingPageL
//----------------------------------------------------------
//
void CApSettingsDlg::ShowPopupIpAddrSettingPageL( TApMember aData )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::ShowPopupIpAddrSettingPageL")
    
    HBufC* aBuf = HBufC::NewLC( KModifiableTextLength );
    TPtr16 ptr( aBuf->Des() );

    iApItem->ReadTextL( aData, ptr );

    TBuf<KModifiableTextLength> textToChange;

    textToChange.Append( ptr );

    TInt page_resid( 0 );
    TInt TextSettingPageFlags( EAknSettingPageNoOrdinalDisplayed );
    GetTextResId( aData, page_resid, TextSettingPageFlags  );

    TInetAddr AddrToChange;
    AddrToChange.Input( textToChange );
    CAknIpFieldSettingPage* dlg = 
            new( ELeave )CAknIpFieldSettingPage( page_resid, AddrToChange );
    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted ) )
        {
        // first need to check phone number validity, if not, 
        // display error msg,
        AddrToChange.Output( textToChange );
        iApItem->WriteTextL( aData, textToChange );
        }
    CleanupStack::PopAndDestroy(); // aBuf
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::ShowPopupIpAddrSettingPageL")
    }






//----------------------------------------------------------
// CApSettingsDlg::ShowPopupPhoneNumSettingPageL
//----------------------------------------------------------
//
void CApSettingsDlg::ShowPopupPhoneNumSettingPageL( TApMember aData )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::ShowPopupPhoneNumSettingPageL")
    
    HBufC* aBuf = HBufC::NewLC( KModifiableTextLength );
    TPtr16 ptr( aBuf->Des() );

    if ( aData == EApIspIfCallbackInfo )
        { // 8 bit text
        HBufC8* tmpValue8 = HBufC8::NewLC( KModifiableTextLength );
        TPtr8 ptr8( tmpValue8->Des() );
        iApItem->ReadTextL( aData, ptr8 );
        ptr.Copy( ptr8 );
        CleanupStack::PopAndDestroy(); // tmpvalue8
        }
    else
        {
        iApItem->ReadTextL( aData, ptr );
        }

    TBuf<KModifiableTextLength> textToChange;

    textToChange.Append( ptr );

    TInt page_resid( 0 );
    TInt TextSettingPageFlags( EAknSettingPageNoOrdinalDisplayed );
    GetTextResId( aData, page_resid, TextSettingPageFlags );
    TBool mustgoon( ETrue );
    do
        {
        CAknSettingPage* dlg = 
                new( ELeave )CAknTextSettingPage( page_resid, 
                                                  textToChange, 
                                                  TextSettingPageFlags );
        if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted ) )
            {
            // first need to check phone number validity,
            // if not, dispaly error msg,
            // bearing in mind that if empty is not allowed,
            // we can not got empty text from editor, so IF it is not a valid
            // phone number, but it is empty, thna it is allowed and
            // it is the user's wish, so go on as valid...
            // MNAN-6XBFNB: Digits that are not western should also be accepted
            // Convert digits to western and check the phone number like this.
            TBuf<KModifiableTextLength> temp;
            temp = textToChange;
            AknTextUtils::ConvertDigitsTo( temp, EDigitTypeWestern );
            
            if ( CommonPhoneParser::IsValidPhoneNumber
                    ( temp, CommonPhoneParser::EContactCardNumber )                   
                    || ( temp.Compare( KEmpty ) == 0) )
                {
                if ( aData == EApIspIfCallbackInfo )
                    { // 8 bit text
                    HBufC8* tmpValue8 = HBufC8::NewLC( KModifiableTextLength );
                    TPtr8 ptr8( tmpValue8->Des() );
                    iApItem->ReadTextL( aData, ptr8 );
                    ptr8.Copy( textToChange );
                    iApItem->WriteTextL( aData, ptr8 );
                    CleanupStack::PopAndDestroy(); // tmpvalue8
                    }
                else
                    {
                    iApItem->WriteTextL( aData, textToChange );
                    }
                mustgoon = EFalse;
                }
            else
                { // give invalid number message
                ShowNoteL( R_APUI_VIEW_INVALID_NUMBER );
                }
            }
        else
            {
            mustgoon = EFalse;
            }
        }
        while( mustgoon );
    CleanupStack::PopAndDestroy(); // aBuf
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::ShowPopupPhoneNumSettingPageL")
    }




//----------------------------------------------------------
// CApSettingsDlg::ShowPopupPasswordPageL
//----------------------------------------------------------
//
void CApSettingsDlg::ShowPopupPasswordPageL( TApMember aData )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::ShowPopupPasswordPageL")
    
    // must be prepared for the worst case...
    TBuf<KMaxLoginPasswordLength> newpass;

    TUint32 txtLength = iApItem->ReadTextLengthL( aData );
    // allocates as much as needed...
    HBufC* oldpass = HBufC::NewLC( txtLength );
    TPtr16 ptr( oldpass->Des() );
    iApItem->ReadTextL( aData, ptr );

    CAknAlphaPasswordSettingPage* dlg = 
            new( ELeave )CAknAlphaPasswordSettingPage(
                            R_TEXT_SETTING_PAGE_LOGIN_PASSWD, 
                            newpass, ptr );
    CleanupStack::PushL(dlg);
    // As password is asked only once...
    dlg->SetMaxPasswordLength( KMaxLoginPasswordLength );
    CleanupStack::Pop();
    if ( dlg->ExecuteLD(CAknSettingPage::EUpdateWhenChanged) )
        {
        iApItem->WriteTextL( aData, newpass );
        }
    CleanupStack::PopAndDestroy(); // oldpass
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::ShowPopupPasswordPageL")
    }



//----------------------------------------------------------
// CApSettingsDlg::GetBoolDataValue
//----------------------------------------------------------
//
void CApSettingsDlg::GetBoolDataValue( TApMember aData, TBool aInvert, 
                                       TInt& aCurrvalue)
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::GetBoolDataValue")
    
    iApItem->ReadBool( aData, aCurrvalue);
    // just to be on the sure side...
    aCurrvalue = Min( KBoolMaxNumber, aCurrvalue);
    if ( aInvert)
        {
        aCurrvalue = !aCurrvalue;
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::GetBoolDataValue")
    }


//----------------------------------------------------------
// CApSettingsDlg::DoBearerChangeL
//----------------------------------------------------------
//
void CApSettingsDlg::DoBearerChangeL( TInt aCurrvalue)
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::DoBearerChangeL")
    
    TApBearerType newbearer = BearerItemPos2BearerTypeL( aCurrvalue );
    HandleBearerChangeL( newbearer );
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::DoBearerChangeL")
    }



//----------------------------------------------------------
// CApSettingsDlg::HandleBearerChangeL
//----------------------------------------------------------
//
void CApSettingsDlg::HandleBearerChangeL( TApBearerType aBearer)
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::HandleBearerChangeL")
    
    if ( iBearerItemArray->At( BearerToArrayIndexL( aBearer)) == NULL)
        {
        CApAccessPointItem* ap = CApAccessPointItem::NewLC();
        ap->CopyFromL( *iApItem);
        ap->WriteUint( EApWapAccessPointID, iApItem->WapUid());
        iBearerItemArray->At( BearerToArrayIndexL( aBearer)) = ap;
        iApItem = ap;
        CleanupStack::Pop(); // ap
        }
    else
        {
        iApItem = iBearerItemArray->At( BearerToArrayIndexL( aBearer));
        }
    iApItem->SetBearerTypeL( aBearer );
    SetBearerTypeL( aBearer );
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::HandleBearerChangeL")
    }


//----------------------------------------------------------
// CApSettingsDlg::BearerToArrayIndexL
//----------------------------------------------------------
//
TInt32 CApSettingsDlg::BearerToArrayIndexL( TApBearerType aBearer)
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::BearerToArrayIndexL")
    
    TInt32 retval( KBearerArrayGPRS );
    switch( aBearer )
        {
        case EApBearerTypeCSD:
            {
            retval = KBearerArrayCSD;
            break;
            }
        case EApBearerTypeGPRS:
            {
            retval = KBearerArrayGPRS;
            break;
            }
        case EApBearerTypeHSCSD:
            {
            retval = KBearerArrayHSCSD;
            break;
            }
        case EApBearerTypeWLAN:
            {
            if ( IsWlanSupported() )
                {
                retval = KBearerArrayWLAN;
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EInvalidBearerType ) );
            User::Leave( KErrInvalidBearerType );
            break;
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::BearerToArrayIndexL")
    return retval;
    }

/*
//----------------------------------------------------------
// CApSettingsDlg::ArrayIndexToBearer
//----------------------------------------------------------
//
TApBearerType CApSettingsDlg::ArrayIndexToBearer( TInt32 aIndex)
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::ArrayIndexToBearer")
    
    TApBearerType retval( EApBearerTypeGPRS );
    switch( aIndex )
        {
        case KBearerArrayCSD:
            {
            retval = EApBearerTypeCSD;
            break;
            }
        case KBearerArrayGPRS:
            {
            retval = EApBearerTypeGPRS;
            break;
            }
        case KBearerArrayHSCSD:
            {
            retval = EApBearerTypeHSCSD;
            break;
            }
        case KBearerArrayWLAN:
            {
            if ( IsWlanSupported() )
                {
                retval = EApBearerTypeWLAN;
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EInvalidBearerType ) );
            break;
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::ArrayIndexToBearer")
    return retval;
    }

*/


//----------------------------------------------------------
// CApSettingsDlg::FillUpBearerSpeedValuesL
//----------------------------------------------------------
//
void CApSettingsDlg::FillUpBearerSpeedValuesL( CDesCArrayFlat& aItems, 
                                               TInt& aCurrvalue )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::FillUpBearerSpeedValuesL")
    
    TUint32 tval( 0 );
    TUint32 bval( 0 );

    iApItem->ReadUint( EApIspBearerSpeed, tval );
    iApItem->ReadUint( EApIspBearerCallTypeIsdn, bval );
    TApBearerType btype = iApItem->BearerTypeL();
    const TInt* actResSpeeds = NULL;
    const TInt* actSpeeds = NULL;

    TBool isWcdma = 
            FeatureManager::FeatureSupported( KFeatureIdProtocolWcdma );

    switch ( bval )
        {
        case ECallTypeAnalogue:
            { // CSD/HSCSD, Analogue
           if ( btype == EApBearerTypeHSCSD )
                {
                actResSpeeds = KResHcsdAnalogue;
                actSpeeds = KHcsdAnalogue;
                }
            else
                {
                actResSpeeds = KResCsdAnalogue;
                actSpeeds = KCsdAnalogue;
                if ( isWcdma )
                    {
                    actResSpeeds = KResCsdAnalogueWcdma;
                    actSpeeds = KCsdAnalogueWcdma;
                    }
                }
            break;
            }
        case ECallTypeISDNv110:
            { // CSD/HSCSD, ISDN v110:
            if ( btype == EApBearerTypeHSCSD )
                {
                actResSpeeds = KResHcsdIsdn110;
                actSpeeds = KHcsdIsdn110;
                }
            else
                {
                actResSpeeds = KResCsdIsdn110;
                actSpeeds = KCsdIsdn110;
                if ( isWcdma )
                    {
                    actResSpeeds = KResCsdIsdn110Wcdma;
                    actSpeeds = KCsdIsdn110Wcdma;
                    }
                }
            break;
            }
        case ECallTypeISDNv120:
            {
           if ( btype == EApBearerTypeHSCSD )
                {
                actResSpeeds = KResHcsdIsdn120;
                actSpeeds = KHcsdIsdn120;
                }
            else
                {
                actResSpeeds = KResCsdIsdn120;
                actSpeeds = KCsdIsdn120;
                if ( isWcdma )
                    {
                    actResSpeeds = KResCsdIsdn120Wcdma;
                    actSpeeds = KCsdIsdn120Wcdma;
                    }
                }
            break;
            }
        default:
            {
            break;
            }
        }

    if ( actResSpeeds )
        {
        while ( *actResSpeeds != KEndOfArray )
            {
            aItems.AppendL( *iEikonEnv->AllocReadResourceLC( *actResSpeeds ) );
            CleanupStack::PopAndDestroy();
            ++actResSpeeds;
            }
        }

    if( actSpeeds )
        {
        aCurrvalue = 0;
        TInt nextSpeed = *( actSpeeds + aCurrvalue + 1) ;
        while ( ( nextSpeed != KEndOfArray ) 
                && ( *( actSpeeds + aCurrvalue) < ( TInt )tval ) 
                && ( nextSpeed <= ( TInt )tval ) )
            {
            ++aCurrvalue;
            nextSpeed = *( actSpeeds + aCurrvalue + 1) ;
            }
        }

    APSETUILOGGER_LEAVEFN( ESettings,"Settings::FillUpBearerSpeedValuesL")
    }



//----------------------------------------------------------
// CApSettingsDlg::SaveDataL
//----------------------------------------------------------
//
TBool CApSettingsDlg::SaveDataL( )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::SaveDataL")
    
    TApMember aDataNeeded;
    TSaveAction action(EApCanSave);
    TBool retval( ETrue );
    TInt err (KErrNone );
    
    // first handle ALL db stuff because Symbian DB does handles 
    // consequtive transactions wrongly...
    TTransactionResult ownTransaction = 
            iDataModel->StartTransactionLC(
                                /*aWrite*/EFalse,
                                /*aShowNote*/ETrue,
                                /*aRetry*/ETrue );
    if ( ownTransaction != EFailedWithLocked )
        {
        action = CanSaveL(aDataNeeded);
        switch ( action )
            {
            case EApCanSave:
#ifdef __WINS__ 
            case EApMustDelete:
#endif // __WINS__
                { // save data
                if ( ( *iOldApItem != *iApItem ) || ( *iUid == KApNoneUID ) 
                    || iDataModel->iWepSecSettings 
                    || iDataModel->iWpaSecSettings )
                    { // save if different or NEW BLANK
                    *iEventStore |= KApUiEventEdited;
                    if ( *iUid == KApNoneUID )
                        {
                        HBufC* buf = HBufC::NewLC( KApMaxConnNameLength );
                        // now check if name is unique, if it is, 
                        // save and commit, if it is not, show note
                        // and no save at all, Rollback!
                        TPtr16 ptr( buf->Des() );
                        iApItem->ReadTextL( EApWapAccessPointName, ptr );
                        TBool b(EFalse);
                        TRAP( err, b = 
                              iDataModel->ApUtils()->IsValidNameL( ptr ) );
                        CleanupStack::PopAndDestroy( buf );
                        if ( b )
                            {
                            TRAP( err, 
                                  iDataModel->CreateFromDataL( *iApItem ) );
                            if ( err == KErrNone )
                                {
                                iApItem->ReadUint( EApWapAccessPointID, 
                                                   *iUid );
                                *iEventStore |= KApUiEventCreatedBlank;
                                }
                            }
                        else
                            {
                            err = KErrAlreadyExists;
                            // no DB call after this point, as transaction must be rolled back, 
                            // but we can do it only at the end
                            if ( ownTransaction == EOwnTransaction )
                                {
                                CleanupStack::PopAndDestroy(); //Rollback
                                // set as if no need to commit...
                                ownTransaction = EUsingAlreadyStarted;
                                }                            
                            }
                        }
                    else
                        {
                        // If the name is not unique, 
                        // it will leave with KErrAlreadyExists
                        TRAP( err, 
                              iDataModel->UpdateAccessPointDataL( *iApItem ) );
                        }
                    }
                break;
                }
#ifndef __WINS__  
            case EApMustDelete:
                {
                if ( *iUid != KApNoneUID )
                    {
                    TUint32 WapUid( 0 );
                    iApItem->ReadUint( EApWapAccessPointID, WapUid );
                    iDataModel->RemoveApL( WapUid );
                    }
                break;
                }
#endif // __WINS__
            case EApShallGoBack:
                {
                switch ( aDataNeeded )
                    {
                    case EApIspDefaultTelNumber:
                        {
                        SelectItem( KListIndexCSDAccessNumber );
                        retval = EFalse;
                        break;
                        }
                    case EApWlanNetworkName:
                        {
                        if ( IsWlanSupported() )
                            {                    
                            SelectItem( KListIndexWLANNetworkName );
                            retval = EFalse;
                            }
                        else
                            {
                            User::Leave( KErrNotSupported );
                            }
                        break;
                        }
                    case EApWlanIpNetMask:
                    case EApIspIPNetMask:
                        {
                        if ( IsWlanSupported() )
                            {                    
                            SelectItem( KListIndexWLANSubnetMask );
                            retval = EFalse;
                            }
                        else
                            {
                            User::Leave( KErrNotSupported );
                            }
                        break;
                        }
                    case EApIspIPGateway:
                    case EApWlanIpGateway:
                        {
                        if ( IsWlanSupported() )
                            {                    
                            SelectItem( KListIndexWLANDefaultGateway );
                            retval = EFalse;
                            }
                        else
                            {
                            User::Leave( KErrNotSupported );
                            }
                        break;
                        }
                    case EApWlanSecuritySettings:
                        {
                        if ( IsWlanSupported() )
                            {                    
                            SelectItem( KListIndexWLANSecuritySettings );
                            retval = EFalse;
                            ChangeSettingsL( EFalse );
                            }
                        else
                            {
                            User::Leave( KErrNotSupported );
                            }                    
                        break;
                        }
                    default:
                        {// otherwise, nothing to do...
                        __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
                        break;
                        }
                    }
                break;
                } 
            case EApNoAction:
                { // can not save, do not go back: Exit, do nothing but ignore...
                break;
                }            
            default:
                {
                __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
                break;
                }
            }

        if ( ownTransaction == EOwnTransaction )
            {
            err = iDataModel->Database()->Database()->CommitTransaction();
            User::LeaveIfError( err );
            CleanupStack::Pop(); // RollbackTransaction
            }
        }
    else
        {
        User::Leave( KErrLocked );
        }
    // end transaction stuff
    // and handle possible other stuff like
    // displaying notifications, redirecting the user, etc...
    
    if ( err == KErrAlreadyExists )
        {
        // Check if exit and if it is, Exit, 
        // if it is not, then show note 
        // and move user back to editor...
        if ( !( *iEventStore & KApUiEventExitRequested ) )
            {                        
            HBufC* buf = HBufC::NewLC( KApMaxConnNameLength );
            TPtr16 ptr( buf->Des() );
            iApItem->ReadTextL( EApWapAccessPointName, ptr );
            ShowNoteL( R_APUI_IAP_NAME_ALREADY_EXISTS, buf );
            CleanupStack::PopAndDestroy( buf ); // buf
            SelectItem( KListIndexConnectionName );
            retval = EFalse;
            ChangeSettingsL( EFalse );
            }
        }
    else
        {
        if ( err == KErrInvalidName )
            {
            ShowNoteL( R_APUI_VIEW_INVALID_CONN_NAME );
            SelectItem( KListIndexConnectionName );
            retval = EFalse;
            }
        else
            {
            if ( !( *iEventStore & KApUiEventExitRequested ) )
                {
                User::LeaveIfError( err );
                }                            
            iOldApItem->CopyFromL( *iApItem );
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::SaveDataL")
    return retval;
    }



//----------------------------------------------------------
// CApSettingsDlg::HandleListStructureChangeL
//----------------------------------------------------------
//
void CApSettingsDlg::HandleListStructureChangeL()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::HandleListStructureChangeL")
    
    TUint32 l = iApItem->ReadTextLengthL( EApWapGatewayAddress );
    HBufC* buf = HBufC::NewLC( l );
    TPtr16 ptr( buf->Des() );
    iApItem->ReadTextL( EApWapGatewayAddress, ptr );

    if ( ( iBearerType == EApBearerTypeCSD ) 
        || ( iBearerType == EApBearerTypeHSCSD ) )
        {
        GetCSDSettingArrays( iVariant );
        }
    else
        {
        if ( iBearerType == EApBearerTypeGPRS )
            {
            GetGPRSSettingArrays( iVariant );
            }
        else
            {
            if ( IsWlanSupported() )
                {
                if ( iBearerType == EApBearerTypeWLAN )
                    {
                    GetWLANSettingArraysL( iVariant );
                    }
                else
                    {
                    User::Leave( KErrInvalidBearer );
                    }
                }
            else
                {
                User::Leave( KErrInvalidBearer );
                }
            }
        }
    CleanupStack::PopAndDestroy( buf );
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::HandleListStructureChangeL")
    }



//----------------------------------------------------------
// CApSettingsDlg::LimitSpeed
//----------------------------------------------------------
//
void CApSettingsDlg::LimitSpeed()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::LimitSpeed")
    
    // now 'limit' the speeds
    TUint32 speed( 0 );
    iApItem->ReadUint( EApIspBearerSpeed, speed );
    TUint32 calltype( EFalse );
    iApItem->ReadUint( EApIspBearerCallTypeIsdn, calltype );
    const TInt* actSpeeds = NULL;
    TBool isWcdma = 
            FeatureManager::FeatureSupported( KFeatureIdProtocolWcdma ) ;
    switch ( iBearerType )
        {
        case EApBearerTypeCSD:
            {
            switch ( calltype )
                {
                case ECallTypeISDNv110:
                    {
                    actSpeeds = isWcdma ? KCsdIsdn110Wcdma : KCsdIsdn110;
                    break;
                    }
                case ECallTypeISDNv120:
                    {
                    actSpeeds = isWcdma ? KCsdIsdn120Wcdma : KCsdIsdn120;
                    break;
                    }
                case ECallTypeAnalogue:
                default:
                    {
                    actSpeeds = isWcdma ? KCsdAnalogueWcdma : KCsdAnalogue;
                    }
                }
            break;
            }
        case EApBearerTypeHSCSD:
            {
            switch ( calltype )
                {
                case ECallTypeISDNv110:
                    {
                    actSpeeds = KHcsdIsdn110;
                    break;
                    }
                case ECallTypeISDNv120:
                    {
                    actSpeeds = KHcsdIsdn120;
                    break;
                    }
                case ECallTypeAnalogue:
                default:
                    {
                    actSpeeds = KHcsdAnalogue;
                    }
                }
            break;
            }
        default:
            {
            break;
            }
        }

    if( actSpeeds )
        {
        TInt tempSpeed = *actSpeeds;
        TInt nextSpeed = *( actSpeeds + 1 );
        while ( ( nextSpeed != KEndOfArray ) 
                && ( tempSpeed < ( TInt )speed )
                && ( nextSpeed <= ( TInt )speed ) )
            {
            tempSpeed = *( ++actSpeeds );
            nextSpeed = *( actSpeeds + 1 );
            }
        speed = tempSpeed ;
        }


    iApItem->WriteUint( EApIspBearerSpeed, speed );
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::LimitSpeed")
    }


//----------------------------------------------------------
// CApSettingsDlg::SelectItem
//----------------------------------------------------------
//
void CApSettingsDlg::SelectItem( TInt aItemIndex )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::SelectItem")
    
    iList->ScrollToMakeItemVisible( aItemIndex );
    iList->SetCurrentItemIndexAndDraw( aItemIndex );
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::SelectItem")
    }





// Converting EParagraphDelimiters to \ns
// ---------------------------------------------------------
// CApSettingsDlg::ConvertDelimToPerNLC
// ---------------------------------------------------------
//
HBufC* CApSettingsDlg::ConvertDelimToPerNLC( const TDesC& aInText )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::ConvertDelimToPerNLC")
    
    TInt size = aInText.Length();
    HBufC* outText = HBufC::NewLC( size );
    TPtr ptr = outText->Des();

    for ( TInt ii=0; ii<size; ++ii )
        {
        TText ch = aInText[ii];
        if ( ch == CEditableText::ELineBreak ||
             ch == CEditableText::EParagraphDelimiter )
            {
            ch = '\n';
            }
        ptr.Append( ch );
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::ConvertDelimToPerNLC")
    return outText;
    }



// Converting \ns to EParagraphDelimiters
// ---------------------------------------------------------
// CApSettingsDlg::ConvertPerNToDelimLC
// ---------------------------------------------------------
//

HBufC* CApSettingsDlg::ConvertPerNToDelimLC( const TDesC& aInText )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::ConvertPerNToDelimLC")
    
    TInt size = aInText.Length();
    HBufC* outText = HBufC::NewLC( size );
    TPtr ptr = outText->Des();

    TText ch;
    for (TInt ii=0; ii<size; ii++)
        {
        ch = TText( aInText[ii] );
        switch (ch)
            {
            case '\r':
                // ignore
                ptr.Append(ch);
                break;
            case '\n':
                ptr.Append( CEditableText::EParagraphDelimiter );
                break;
            default:
                ptr.Append(ch);
                break;
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::ConvertPerNToDelimLC")
    return outText;
    }


//----------------------------------------------------------
// CApSettingsDlg::DoChangeSettingsL
//----------------------------------------------------------
//
void CApSettingsDlg::DoChangeSettingsL( TBool aQuick )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::DoChangeSettingsL")
    
    TInt itemnum = ( Max( iList->CurrentItemIndex(), 0 ) );
    TApMember* ptr;
    TInt*   tptr;
    TInt*   compulsptr;
    switch ( iLevel )
        {
        case 0:
            {
            ptr = iField;
            tptr = iTitles;
            compulsptr = iCompulsory;
            break;
            }
        case 1:
            {
            ptr = iFieldAdvanced;
            tptr = iTitlesAdvanced;
            compulsptr = iCompulsoryAdvanced;
            break;
            }
        case 2:
            {
            ptr = iFieldL2;
            tptr = iTitlesL2;
            compulsptr = iCompulsoryL2;
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
            return; // just to suppress warnings about ptr's 
                    // beeing used uninitialised
            }
        }



    ptr += itemnum;
    tptr += itemnum;
    compulsptr += itemnum;
    TInt topitem( iList->TopItemIndex() );
    
    TApBearerType bearer = iApItem->BearerTypeL();
    if ( bearer == EApBearerTypeGPRS )
        {
        TUint32 tint;
        iApItem->ReadUint( EApGprsPdpType, tint );
        if ( tint == EIPv6 )
            {
            if ( iLevel == 1 )
                {
                if ( itemnum )
                    {
                    ptr++;
                    tptr++;
                    compulsptr++;
                    }
                }
            }
        }
    else
        { // others
        if ( bearer == EApBearerTypeWLAN )
            {
            // if we are in iL2Ipv4, AND 
            // EApIspIPNetMask and EApIspIPGateway fields are not added 
            // skip those items...
            if ( !IsNetMaskAndGatewayVisibleL() )
                {                
                if ( *ptr == EApIspIPNetMask ) 
                    {
                    ptr++;
                    tptr++;
                    compulsptr++;
                    }
                if ( *ptr == EApIspIPGateway ) 
                    {
                    ptr++;
                    tptr++;
                    compulsptr++;
                    }
                }
            if ( !IsAdhocChannelVisible() )
                {
                if ( ( *ptr == EApWlanChannelId ) 
                    || ( *ptr == EApProxyServerAddress ) )
                    {
                    ptr++;
                    tptr++;
                    compulsptr++;
                    }
                }
            }
        else
            {            
            // CSD
            if ( *ptr == EApIspIPAddr ) 
                {
                if ( !iL2Ipv4 )
                    {
                    ptr++;
                    tptr++;
                    compulsptr++;
                    }
                }
            }
        }


    switch ( *ptr  )
        {
        case EApIspPromptForLogin:
        case EApWapSecurity:
        case EApIspIfCallbackEnabled:
        case EApIspEnableSWCompression:
        case EApGprsHeaderCompression:
        case EApIspUseLoginScript:
        case EApGprsDisablePlainTextAuth:
        case EApIspDisablePlainTextAuth:
        case EApWapWspOption:
        case EApIspIfCallbackType:
        case EApGprsPdpType:
        case EApWlanNetworkMode:
        case EApWlanScanSSID:
            { // type 4 setting item with two available values
            TBool changed( ETrue );
            if ( aQuick )
                {
                InvertSettingsL( *ptr );
                }
            else
                {
                changed = ShowPopupSettingPageL( *ptr );
                }
            if ( changed )
                {
                if ( *ptr == EApGprsPdpType )
                    {
                    HandleListboxDataChangeL( EFalse );
                    }
                UpdateOnOffListBoxItemL( *ptr, *tptr, itemnum, *compulsptr );
                if ( *ptr == EApWlanNetworkMode )
                    {
                    UpdateOnOffListBoxItemL( *( ptr+1 ), *( tptr+1 ),
                                             itemnum+1, 
                                             *( compulsptr + 1 ) );
                    if ( (itemnum+1) <= iList->BottomItemIndex() )
                        {
                        iList->DrawItem( itemnum+1 );
                        }
                    }
                }
            break;
            }
        case EApIspDefaultTelNumber:
        case EApIspIfCallbackInfo:
            { // type 1, Phone number editor
            ShowPopupPhoneNumSettingPageL( *ptr );
            UpdateTextualListBoxItemL( *ptr, *tptr, itemnum, *compulsptr );
            break;
            }
        case EApWapGatewayAddress:
        case EApIspIPAddr:
        case EApIspIPNetMask:
        case EApWlanIpNetMask:
        case EApIspIPGateway:
        case EApWlanIpGateway:
        case EApGprsIPNameServer1:
        case EApIspIPNameServer1:
        case EApGprsIPNameServer2:
        case EApIspIPNameServer2:
            {// above ones are type 1, IP Address editors!
            ShowPopupIpAddrSettingPageL( *ptr );
            UpdateTextualListBoxItemL( *ptr, *tptr, itemnum, *compulsptr );
            if ( *ptr == EApWapGatewayAddress )
                {
                HandleListStructureChangeL();
                HandleListboxDataChangeL( EFalse );
                }
            if ( *ptr == EApIspIPAddr )
                {
                HandleListboxDataChangeL( EFalse );
                }
            break;
            }
        case EApWapAccessPointName:
        case EApGprsAccessPointName:
        case EApIspLoginName:
        case EApIspIfAuthName:
        case EApIspLoginScript:
        case EApWapStartPage:
        case EApIspInitString:
        case EApProxyServerAddress:
        case EApProxyPortNumber:
            { // type 1, Text setting item
            if ( ShowPopupTextSettingPageL( *ptr ) )
                {
                UpdateTextualListBoxItemL( *ptr, *tptr, itemnum, *compulsptr );
                if ( *ptr == EApWapAccessPointName )
                    {
                    iTitlePane->SetTextL( iApItem->ConnectionName() );
                    }
                }
            break;
            }
        case EApIspLoginPass:
        case EApIspIfAuthPass:

            { // re-confirm, secreted, etc,
            ShowPopupPasswordPageL( *ptr );
            UpdateTextualListBoxItemL( *ptr, *tptr, itemnum, *compulsptr );
            break;
            }
        case EApWapCurrentBearer:
        case EApIspBearerCallTypeIsdn: // used for data call type
        case EApIspBearerSpeed:
        case EApWlanChannelId:
            { // type 2, Pop-up setting item
            if ( ShowPopupSettingPageL( *ptr ) )
                {
                if ( *ptr == EApWapCurrentBearer ) 
                    {
                    // if ==, the whole list needs to be updated !
                    SetBearerTypeL( iApItem->BearerTypeL() );
                    LimitSpeed();
                    HandleListboxDataChangeL( EFalse );
                    iList->SetTopItemIndex( topitem );
                    iList->DrawNow();
                    }
                else
                    {
                    if ( *ptr == EApIspBearerCallTypeIsdn )
                        { // first limit the speed if neccessary
                        LimitSpeed();
                        // need to redraw EApIspBearerSpeed, too
                        // because it is possible that it had been changed 
                        // to keep constrains simply update next field,
                        // too, 'cause they are next to each other...
                        UpdateOnOffListBoxItemL( *( ptr+1 ), *( tptr+1 ),
                                                 itemnum+1, 
                                                 *( compulsptr + 1 ) );
                        iList->DrawItem( itemnum+1 );
                        }
                    UpdateOnOffListBoxItemL( *ptr, *tptr, 
                                             itemnum, *compulsptr );
                    }
                }
            break;
            }
        case EApWlanSecurityMode:
            { // type 2, Pop-up setting item
            if ( ShowPopupSettingPageL( *ptr ) )
                {
                // security mode has changed, re-create security settings!
                UpdateOnOffListBoxItemL( *ptr, *tptr, 
                                         itemnum, *compulsptr );
                iHandler->iModel->UpdateSecurityModeL( *iApItem );
                }
            break;
            }
        case EApIP6DNSAddrFromServer:
            { 
            // if GPRS, PDP type Ipv4, DNS data query
            // if GPRS, PDP type Ipv6, type 2, Pop-up setting item
            TUint32 ival(0);
            iApItem->ReadUint( EApGprsPdpType, ival );
            TBool changed( EFalse );
            if ( iApItem->BearerTypeL() == EApBearerTypeGPRS )
                {
                if ( ival == EIPv6 )
                    {
                    // GPRS, PDP type Ipv6, DNS pop-up setting item
                    changed = ShowPopupSettingPageL( *ptr );
                    }
                else
                    {
                    // GPRS, PDP type Ipv4, DNS data query
                    changed = GetNameServersL( EFalse );
                    }
                }
            else
                { // CSD/HSCSD
                if ( iL2Ipv4 )
                    {
                    changed = GetNameServersL( EFalse );
                    }
                else
                    {
                    changed = ShowPopupSettingPageL( *ptr );
                    }
                }
            if ( changed )
                {
                UpdateOnOffListBoxItemL( *ptr, *tptr, itemnum, *compulsptr );
                }
            break;
            }
        case EApNetworkName:
            {
            ChangeNetworkGroupL();
            UpdateTextualListBoxItemL( *ptr, *tptr, itemnum, *compulsptr );
            break;
            }
        case EApIpv4Settings:
        case EApIpv6Settings:
            {
            iL2Ipv4 = ( *ptr == EApIpv4Settings );
            if ( iLevel )
                {
                // enter Ipv4 or Ipv6 settings
                if ( iL2Ipv4 )
                    {
                    // save current state so we can roll back to it
                    // if needed, e.g. if user fills but discards values...
                    if (!iBackupApItem )
                        {
                        iBackupApItem  = CApAccessPointItem::NewLC();
                        CleanupStack::Pop(); // member var                        
                        }
                    iBackupApItem->CopyFromL( *iApItem );
                    }
                iLevel++;
                HandleListStructureChangeL();
                iOldIndex = iList->CurrentItemIndex();
                iOldTopIndex = iList->TopItemIndex();
                iList->SetCurrentItemIndex( 0 );
                HandleListboxDataChangeL( EFalse );
                itemnum = 0;
                }
            break;
            }
        case EApWlanNetworkName:
            {
            if ( IsWlanSupported() )
                {
                // Temporarily use plain text editor to input 
                // network name...
                ChangeWlanNetworkNameL();
                // as it sets connection mode and security mode, 
                // we must update the full list...
                HandleListboxDataChangeL( EFalse );
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
        case EApWlanSecuritySettings:
            {
            if ( IsWlanSupported() )
                {
                iSecSettingsExitReason = ChangeWlanSecuritySettingsL();
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
/*
        case EApWlanChannelId:
            {
            breaaak; 
            }
*/            
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
            break;
            }
        }
    SelectItem( itemnum );
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::DoChangeSettingsL")
    }




//----------------------------------------------------------
// CApSettingsDlg::GetSpeedForSelection
//----------------------------------------------------------
//
TApCallSpeed CApSettingsDlg::GetBearerSpeedForSelection( TInt aCurrvalue )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::GetBearerSpeedForSelection")
    
    TApCallSpeed sp( KSpeed9600 );


    TUint32 tval( 0 );
    iApItem->ReadUint( EApIspBearerCallTypeIsdn, tval );

    const TInt* actSpeeds = NULL;
    TBool isWcdma = 
            FeatureManager::FeatureSupported( KFeatureIdProtocolWcdma );

    switch ( tval )
        {
        case ECallTypeAnalogue:
            { // CSD/HSCSD, Analogue
            switch ( iBearerType )
                {
                case EApBearerTypeCSD:
                    {
                    actSpeeds = isWcdma ? KCsdAnalogueWcdma : KCsdAnalogue;
                    break;
                    }
                case EApBearerTypeHSCSD:
                    {
                    actSpeeds = KHcsdAnalogue;
                    break;
                    }
                default:
                    {
                    // this can only mean programmer error as no other
                    // bearer type is supperted with speed settings
                    __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
                    break;
                    }
                }
            break;
            }
        case ECallTypeISDNv110:
            {
            switch ( iBearerType )
                {
                case EApBearerTypeCSD:
                    {
                    actSpeeds = isWcdma ? KCsdIsdn110Wcdma : KCsdIsdn110;
                    break;
                    }
                case EApBearerTypeHSCSD:
                    {
                    actSpeeds = KHcsdIsdn110;
                    break;
                    }
                default:
                    {
                    // this can only mean programmer error as no other
                    // bearer type is supperted with speed settings
                    __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
                    break;
                    }
                }
            break;
            }
        case ECallTypeISDNv120:
            {
            switch ( iBearerType )
                {
                case EApBearerTypeCSD:
                    {
                    actSpeeds = isWcdma ? KCsdIsdn120Wcdma : KCsdIsdn120;
                    break;
                    }
                case EApBearerTypeHSCSD:
                    {
                    actSpeeds = KHcsdIsdn120;
                    break;
                    }
                default:
                    {
                    // this can only mean programmer error as no other
                    // bearer type is supperted with speed settings
                    __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
                    break;
                    }
                }
            break;
            }
        default:
            {
            // this can only mean programmer error as no other
            // bearer type is supperted with speed settings
            __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
            break;
            }
        }

    sp = (TApCallSpeed) *(actSpeeds + aCurrvalue);

    APSETUILOGGER_LEAVEFN( ESettings,"Settings::GetBearerSpeedForSelection")
    return sp;
    }



//----------------------------------------------------------
// CApSettingsDlg::GetCurrIndexFromBearerSpeed
//----------------------------------------------------------
//
TInt CApSettingsDlg::GetCurrIndexFromBearerSpeed( )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::GetCurrIndexFromBearerSpeed")
    
    TUint32 tval( 0 );
    TUint32 bval( 0 );
    TInt retval( 0 );

    iApItem->ReadUint( EApIspBearerSpeed, tval );
    iApItem->ReadUint( EApIspBearerCallTypeIsdn, bval );

    TBool isWcdma = 
            FeatureManager::FeatureSupported( KFeatureIdProtocolWcdma );

    const TInt* actSpeeds = NULL;

    switch ( bval )
        {
        case ECallTypeAnalogue:
            { // CSD/HSCSD, Analogue
            switch ( iBearerType )
                {
                case EApBearerTypeCSD:
                    {
                    actSpeeds = isWcdma ? KCsdAnalogueWcdma : KCsdAnalogue;
                    break;
                    }
                case EApBearerTypeHSCSD:
                    {
                    actSpeeds = KHcsdAnalogue;
                    break;
                    }
                default:
                    {
                    // this can only mean programmer error as no other
                    // bearer type is supperted with speed settings
                    __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
                    break;
                    }
                }
            break;
            }
        case ECallTypeISDNv110:
            {
            switch ( iBearerType )
                {
                case EApBearerTypeCSD:
                    {
                    actSpeeds = isWcdma ? KCsdIsdn110Wcdma : KCsdIsdn110;
                    break;
                    }
                case EApBearerTypeHSCSD:
                    {
                    actSpeeds = KHcsdIsdn110;
                    break;
                    }
                default:
                    {
                    // this can only mean programmer error as no other
                    // bearer type is supperted with speed settings
                    __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
                    break;
                    }
                }
            break;
            }
        case ECallTypeISDNv120:
            {
            switch ( iBearerType )
                {
                case EApBearerTypeCSD:
                    {
                    actSpeeds = isWcdma ? KCsdIsdn120Wcdma : KCsdIsdn120;
                    break;
                    }
                case EApBearerTypeHSCSD:
                    {
                    actSpeeds = KHcsdIsdn120;
                    break;
                    }
                default:
                    {
                    // this can only mean programmer error as no other
                    // bearer type is supperted with speed settings
                    __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
                    break;
                    }
                }
            break;
            }
        default:
            {
            // this can only mean programmer error as no other
            // bearer type is supperted with speed settings
            __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
            break;
            }
        }

    if( actSpeeds )
        {
        retval = 0;
        while ( ( *( actSpeeds + retval + 1) != KEndOfArray ) 
                && ( *( actSpeeds + retval) < ( TInt )tval ) 
                && ( *( actSpeeds + retval + 1) <= ( TInt )tval ) )
            {
            ++retval;
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::GetCurrIndexFromBearerSpeed")
    return retval;
    }


//----------------------------------------------------------
// CApSettingsDlg::ReplaceNonPrintingCharactersLC
//----------------------------------------------------------
//
HBufC* CApSettingsDlg::ReplaceWhiteSpaceCharactersLC(  const TDesC& aInText )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::ReplaceWhiteSpaceCharactersLC")
    
    TInt size = aInText.Length();
    HBufC* aOutText = HBufC::NewLC( size );

    TPtr ptr = aOutText->Des();

    for ( TInt ii=0; ii<size; ++ii )
        {
        TChar ch = aInText[ii];
        if ( ch.IsSpace() )
            {
            ch = ' ';
            }
        ptr.Append( ch );
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::ReplaceWhiteSpaceCharactersLC")
    return aOutText;
    }



//----------------------------------------------------------
// CApSettingsDlg::ChangeNetworkGroupL
//----------------------------------------------------------
//
void CApSettingsDlg::ChangeNetworkGroupL()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::ChangeNetworkGroupL")
    
    HBufC* titlebuf;

    titlebuf = iHandler->iModel->EikEnv()->AllocReadResourceLC
                            (
                            R_APUI_VIEW_NETWORK_GROUP
                            );

    // needs this Dummy array to pass it to CAknSettingPage's constructor...
    CDesCArrayFlat* aItemArray = new ( ELeave ) CDesCArrayFlat( 1 );
    CleanupStack::PushL( aItemArray );
    TUint32 prefUid( 0 );
    TInt currsel( -1 );
    iApItem->ReadUint( EApNetworkID, prefUid );

    TInt prefInt( prefUid );
    CApNetSelPopupList* dlg = 
        CApNetSelPopupList::NewL( *iDataModel, *iHandler, currsel, 
                                  prefInt, *iEventStore, 
                                  aItemArray, ETrue );

    CleanupStack::PushL( dlg );
    TPtrC ptr( titlebuf->Des() );
    dlg->SetSettingTextL( ptr );
    // dlg must be Pop-ed because ExecuteLD starts with PushL()...
    CleanupStack::Pop();
    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
        {
        // update value in iApItem
        // get networkitem and add it to item...
        iApItem->WriteUint( EApNetworkID, prefInt );
        // update network data according to new ID
        iDataModel->DataHandler()->ReadNetworkPartL( *iApItem );
        }
    CleanupStack::PopAndDestroy( aItemArray ); // aItemArray
    CleanupStack::PopAndDestroy( titlebuf ); // titlebuf
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::ChangeNetworkGroupL")
    }




//----------------------------------------------------------
// CApSettingsDlg::IsHSCSDEnabledL
//----------------------------------------------------------
//
TInt CApSettingsDlg::IsHSCSDEnabledL()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::IsHSCSDEnabledL")
    
    TInt EnableHSCSD( ETrue );

/*
    // Connecting and initialization:
    TInt err( KErrNone );
    CRepository* repository;
    TRAP( err, repository = CRepository::NewL( KGeneralSettingsUid ) );
//    repository->Get( KGSHSCSDAccessPoints, EnableHSCSD );
    delete repository;
*/

#ifdef __TEST_NO_HSCSD
    EnableHSCSD = EFalse;
#endif
#ifdef __TEST_HSCSD_SUPPORT
    EnableHSCSD = ETrue;
#endif // __TEST_HSCSD_SUPPORT

    APSETUILOGGER_LEAVEFN( ESettings,"Settings::IsHSCSDEnabledL")
    return EnableHSCSD;
    }







//----------------------------------------------------------
// CApSettingsDlg::GetCSDSettingArrays
//----------------------------------------------------------
//
void CApSettingsDlg::GetCSDSettingArrays( TInt variant )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::GetCSDSettingArrays")

    // If IPv6 support requested, is the only place where L2 level needed, 
    // so it is the only place when it is variated...
    if ( iReqIpvType & EIPv6 )
        {
        if ( iL2Ipv4 )
            {
            iFieldL2 = 
                (TApMember*) &Fields_CSD_L2_IPv4[KApMember][0];
            iFieldCountL2 = 
                sizeof( Fields_CSD_L2_IPv4[KApMember] )/sizeof( TApMember );
            iTitlesL2 = 
                MUTABLE_CAST( TInt*, 
                              &Fields_CSD_L2_IPv4[KResourceID][0] );
            iCompulsoryL2 = 
                MUTABLE_CAST( TInt*, 
                              &Fields_CSD_L2_IPv4[KCompulsory][0] );
            }
        else
            {
            iFieldL2 = 
                (TApMember*) &Fields_CSD_L2_IPv6[KApMember][0];
            iFieldCountL2 = 
                sizeof( Fields_CSD_L2_IPv6[KApMember] )/sizeof( TApMember );
            iTitlesL2 = 
                MUTABLE_CAST( TInt*, 
                              &Fields_CSD_L2_IPv6[KResourceID][0] );
            iCompulsoryL2 = 
                MUTABLE_CAST( TInt*, 
                              &Fields_CSD_L2_IPv6[KCompulsory][0] );
            }
        }



    if ( variant & KWSPSettingsVisible )
        {
        if ( iReqIpvType & EIPv6 )
            {
            iField = (TApMember*) &Fields_CSD_WSP_IPv6[KApMember][0];
            iFieldCount = 
                sizeof( Fields_CSD_WSP_IPv6[KApMember] )/sizeof( TApMember );

            iFieldAdvanced = 
                (TApMember*) &Fields_CSD_Advanced_WSP_IPv6[KApMember][0];
            iAdvancedFieldCount = 
                sizeof( Fields_CSD_Advanced_WSP_IPv6[KApMember] )
                    / sizeof( TApMember );

            iTitles = 
                MUTABLE_CAST( TInt*, &Fields_CSD_WSP_IPv6[KResourceID][0] );
            iTitlesAdvanced = 
                MUTABLE_CAST( TInt*, 
                              &Fields_CSD_Advanced_WSP_IPv6[KResourceID][0] );

            iCompulsory = 
                MUTABLE_CAST( TInt*, 
                              &Fields_CSD_WSP_IPv6[KCompulsory][0] );
            iCompulsoryAdvanced = 
                MUTABLE_CAST( TInt*, 
                              &Fields_CSD_Advanced_WSP_IPv6[KCompulsory][0] );

            }
        else
            {
            iField = (TApMember*) &Fields_CSD_WSP[KApMember][0];
            iFieldCount = 
                sizeof( Fields_CSD_WSP[KApMember] )/sizeof( TApMember );

            iFieldAdvanced = 
                (TApMember*) &Fields_CSD_Advanced_WSP[KApMember][0];
            iAdvancedFieldCount = 
                sizeof( Fields_CSD_Advanced_WSP[KApMember] )
                    / sizeof( TApMember );

            iTitles = 
                MUTABLE_CAST( TInt*, &Fields_CSD_WSP[KResourceID][0] );
            iTitlesAdvanced = 
                MUTABLE_CAST( TInt*, 
                              &Fields_CSD_Advanced_WSP[KResourceID][0] );

            iCompulsory = 
                MUTABLE_CAST( TInt*, &Fields_CSD_WSP[KCompulsory][0] );
            iCompulsoryAdvanced = 
                MUTABLE_CAST( TInt*, 
                              &Fields_CSD_Advanced_WSP[KCompulsory][0] );
            }
        }
    else
        {
        if ( iReqIpvType & EIPv6 )
            {
            iField = (TApMember*) &Fields_CSD_IPv6[KApMember][0];
            iFieldCount = 
                sizeof( Fields_CSD_IPv6[KApMember] )/sizeof( TApMember );

            iFieldAdvanced = 
                (TApMember*) &Fields_CSD_Advanced_IPv6[KApMember][0];
            iAdvancedFieldCount = 
                sizeof( Fields_CSD_Advanced_IPv6[KApMember] )
                    / sizeof( TApMember );

            iTitles = 
                MUTABLE_CAST( TInt*, 
                              &Fields_CSD_IPv6[KResourceID][0] );
            iTitlesAdvanced = 
                MUTABLE_CAST( TInt*, 
                              &Fields_CSD_Advanced_IPv6[KResourceID][0] );

            iCompulsory = 
                MUTABLE_CAST( TInt*, 
                              &Fields_CSD_IPv6[KCompulsory][0] );
            iCompulsoryAdvanced = 
                MUTABLE_CAST( TInt*, 
                              &Fields_CSD_Advanced_IPv6[KCompulsory][0] );
            }
        else
            {
            iField = (TApMember*) &Fields_CSD[KApMember][0];
            iFieldCount = sizeof( Fields_CSD[KApMember] )/sizeof( TApMember );

            iFieldAdvanced = (TApMember*) &Fields_CSD_Advanced[KApMember][0];
            iAdvancedFieldCount = 
                sizeof( Fields_CSD_Advanced[KApMember] )/sizeof( TApMember );

            iTitles = MUTABLE_CAST( TInt*, &Fields_CSD[KResourceID][0] );
            iTitlesAdvanced = 
                MUTABLE_CAST( TInt*, &Fields_CSD_Advanced[KResourceID][0] );

            iCompulsory = MUTABLE_CAST( TInt*, &Fields_CSD[KCompulsory][0] );
            iCompulsoryAdvanced = 
                MUTABLE_CAST( TInt*, &Fields_CSD_Advanced[KCompulsory][0] );
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::GetCSDSettingArrays")
    }



//----------------------------------------------------------
// CApSettingsDlg::GetGPRSSettingArrays
//----------------------------------------------------------
//
void CApSettingsDlg::GetGPRSSettingArrays( TInt variant )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::GetGPRSSettingArrays")
    
    if ( variant & KWSPSettingsVisible )
        {
        if ( iReqIpvType & EIPv6 )
            {
            iField = ( TApMember* ) &Fields_GPRS_WSP_IPv6[ KApMember ][0];
            iFieldCount = sizeof( Fields_GPRS_WSP_IPv6[0] )
                            / sizeof( TApMember );
            iFieldAdvanced = 
                ( TApMember* ) &Fields_GPRS_Advanced_WSP_IPv6[ KApMember ][0];
            iAdvancedFieldCount = 
                sizeof( Fields_GPRS_Advanced_WSP_IPv6[ KApMember ])
                    / sizeof( TApMember );
            iTitles = 
                MUTABLE_CAST( TInt*, &Fields_GPRS_WSP_IPv6[ KResourceID ][0] );
            iTitlesAdvanced = 
                MUTABLE_CAST( 
                        TInt*, 
                        &Fields_GPRS_Advanced_WSP_IPv6[ KResourceID ][0] );
            iCompulsory = 
                MUTABLE_CAST( TInt*, &Fields_GPRS_WSP_IPv6[ KCompulsory ][0] );
            iCompulsoryAdvanced = 
                MUTABLE_CAST( 
                        TInt*, 
                        &Fields_GPRS_Advanced_WSP_IPv6[ KCompulsory ][0] );
            }
        else
            {
            iField = ( TApMember* ) &Fields_GPRS_WSP[ KApMember ][0];
            iFieldCount = sizeof( Fields_GPRS_WSP[0] )/sizeof( TApMember );
            iFieldAdvanced = 
                ( TApMember* ) &Fields_GPRS_Advanced_WSP[ KApMember ][0];
            iAdvancedFieldCount = 
                sizeof( Fields_GPRS_Advanced_WSP[ KApMember ])
                    / sizeof( TApMember );
            iTitles = 
                MUTABLE_CAST( TInt*, &Fields_GPRS_WSP[ KResourceID ][0] );
            iTitlesAdvanced = 
                MUTABLE_CAST( 
                        TInt*, 
                        &Fields_GPRS_Advanced_WSP[ KResourceID ][0] );
            iCompulsory = 
                MUTABLE_CAST( TInt*, &Fields_GPRS_WSP[ KCompulsory ][0] );
            iCompulsoryAdvanced = 
                MUTABLE_CAST( 
                        TInt*, 
                        &Fields_GPRS_Advanced_WSP[ KCompulsory ][0] );
            }
        }
    else
        {
        if ( iReqIpvType & EIPv6 )
            {
            iField = ( TApMember* ) &Fields_GPRS_IPv6[ KApMember ][0];
            iFieldCount = sizeof( Fields_GPRS_IPv6[0] )/sizeof( TApMember );
            iFieldAdvanced = 
                ( TApMember* ) &Fields_GPRS_Advanced_IPv6[ KApMember ][0];
            iAdvancedFieldCount = 
                sizeof( Fields_GPRS_Advanced_IPv6[ KApMember ])
                    /   sizeof( TApMember );
            iTitles = 
                MUTABLE_CAST( TInt*, &Fields_GPRS_IPv6[ KResourceID ][0] );
            iTitlesAdvanced = 
                MUTABLE_CAST( TInt*, 
                              &Fields_GPRS_Advanced_IPv6[ KResourceID ][0] );
            iCompulsory = 
                MUTABLE_CAST( TInt*, &Fields_GPRS_IPv6[ KCompulsory ][0] );
            iCompulsoryAdvanced = 
                MUTABLE_CAST( TInt*, 
                              &Fields_GPRS_Advanced_IPv6[ KCompulsory ][0] );
            }
        else
            {
            iField = ( TApMember* ) &Fields_GPRS[ KApMember ][0];
            iFieldCount = sizeof( Fields_GPRS[0] )/sizeof( TApMember );
            iFieldAdvanced = 
                ( TApMember* ) &Fields_GPRS_Advanced[ KApMember ][0];
            iAdvancedFieldCount = 
                sizeof( Fields_GPRS_Advanced[ KApMember ])/sizeof( TApMember );
            iTitles = 
                MUTABLE_CAST( TInt*, &Fields_GPRS[ KResourceID ][0] );
            iTitlesAdvanced = 
                MUTABLE_CAST( TInt*, &Fields_GPRS_Advanced[ KResourceID ][0] );
            iCompulsory = 
                MUTABLE_CAST( TInt*, &Fields_GPRS[ KCompulsory ][0] );
            iCompulsoryAdvanced = 
                MUTABLE_CAST( TInt*, &Fields_GPRS_Advanced[ KCompulsory ][0] );
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::GetGPRSSettingArrays")
    }




//----------------------------------------------------------
// CApSettingsDlg::GetWLANSettingArraysL
//----------------------------------------------------------
//
void CApSettingsDlg::GetWLANSettingArraysL( TInt /*aVariant*/ )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::GetWLANSettingArraysL")
    
    if ( IsWlanSupported() )
        {        
        // If IPv6 support requested, is the only place where L2 level needed, 
        // so it is the only place when it is variated...
        if ( iReqIpvType & EIPv6 )
            {
            if ( iL2Ipv4 )
                {
                iFieldL2 = 
                    (TApMember*) &Fields_WLAN_L2_IPv4[KApMember][0];
                iFieldCountL2 = 
                    sizeof( Fields_WLAN_L2_IPv4[KApMember] )
                        / sizeof( TApMember );
                iTitlesL2 = 
                    MUTABLE_CAST( TInt*, 
                                  &Fields_WLAN_L2_IPv4[KResourceID][0] );
                iCompulsoryL2 = 
                    MUTABLE_CAST( TInt*, 
                                  &Fields_WLAN_L2_IPv4[KCompulsory][0] );
                }
            else
                {
                iFieldL2 = 
                    (TApMember*) &Fields_CSD_L2_IPv6[KApMember][0];
                iFieldCountL2 = 
                    sizeof( Fields_CSD_L2_IPv6[KApMember] )
                        / sizeof( TApMember );
                iTitlesL2 = 
                    MUTABLE_CAST( TInt*, 
                                  &Fields_CSD_L2_IPv6[KResourceID][0] );
                iCompulsoryL2 = 
                    MUTABLE_CAST( TInt*, 
                                  &Fields_CSD_L2_IPv6[KCompulsory][0] );
                }
            }


        if ( iReqIpvType & EIPv6 )
            {
            iField = ( TApMember* ) &Fields_WLAN_IPv6[ KApMember ][0];
            iFieldCount = sizeof( Fields_WLAN_IPv6[0] )/sizeof( TApMember );
            iFieldAdvanced = 
                ( TApMember* ) &Fields_WLAN_Advanced_IPv6[ KApMember ][0];
            iAdvancedFieldCount = 
                sizeof( Fields_WLAN_Advanced_IPv6[ KApMember ])
                    / sizeof( TApMember );
            iTitles = 
                MUTABLE_CAST( TInt*, &Fields_WLAN_IPv6[ KResourceID ][0] );
            iTitlesAdvanced = 
                MUTABLE_CAST( TInt*, 
                              &Fields_WLAN_Advanced_IPv6[ KResourceID ][0] );
            iCompulsory = 
                MUTABLE_CAST( TInt*, &Fields_WLAN_IPv6[ KCompulsory ][0] );
            iCompulsoryAdvanced = 
                MUTABLE_CAST( TInt*, 
                              &Fields_WLAN_Advanced_IPv6[ KCompulsory ][0] );
            }
        else
            {
            iField = ( TApMember* ) &Fields_WLAN_IPv4[ KApMember ][0];
            iFieldCount = sizeof( Fields_WLAN_IPv4[0] )/sizeof( TApMember );
            iFieldAdvanced = 
                ( TApMember* ) &Fields_WLAN_Advanced_IPv4[ KApMember ][0];
            iAdvancedFieldCount = 
                sizeof( Fields_WLAN_Advanced_IPv4[ KApMember ])
                    / sizeof( TApMember );
            iTitles = 
                MUTABLE_CAST( TInt*, &Fields_WLAN_IPv4[ KResourceID ][0] );
            iTitlesAdvanced = 
                MUTABLE_CAST( TInt*, 
                              &Fields_WLAN_Advanced_IPv4[ KResourceID ][0] );
            iCompulsory = 
                MUTABLE_CAST( TInt*, &Fields_WLAN_IPv4[ KCompulsory ][0] );
            iCompulsoryAdvanced = 
                MUTABLE_CAST( TInt*, 
                              &Fields_WLAN_Advanced_IPv4[ KCompulsory ][0] );
            }
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::GetWLANSettingArraysL")
    }



//----------------------------------------------------------
// CApSettingsDlg::GetNameServersL
//----------------------------------------------------------
//
TInt CApSettingsDlg::GetNameServersL( TBool aIp6 )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::GetNameServersL")
    
    TInt retval( 0 );

    if ( (iReqIpvType & EIPv6) && aIp6 )
        {
        retval = GetIpv6NameServerL();
        }
    else
        {
        retval = GetIpv4NameServerL();
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::GetNameServersL")
    return retval;
    }






//----------------------------------------------------------
// CApSettingsDlg::AddMenuListBoxItemL
//----------------------------------------------------------
//
void CApSettingsDlg::AddMenuListBoxItemL( CDesCArrayFlat& aItemArray,
                                          TApMember /*aMember*/, TInt aRes,
                                          TInt aCompulsory )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::AddMenuListBoxItemL")
    
    // Define a heap descriptor to hold all the item text
    // HBufC is non-modifiable
    HBufC* title = iEikonEnv->AllocReadResourceLC( aRes );

    // Define a heap descriptor to hold all the item text
    TInt length = title->Length() + 10;
    if ( aCompulsory )
        {
        length += 3;
        }
    HBufC* itemText = HBufC::NewLC( length );


    // Define a modifiable pointer descriptor to be able to append text to the
    // non-modifiable heap descriptor itemText
    TPtr itemTextPtr = itemText->Des();
    itemTextPtr.Format( KTxtMenuListItemFormat, title );
    if ( aCompulsory )
        {
        itemTextPtr.Append( KTxtCompulsory );
        }

    aItemArray.AppendL( *itemText );
    CleanupStack::PopAndDestroy( itemText );
    CleanupStack::PopAndDestroy( title ); // title
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::AddMenuListBoxItemL")
    }


//----------------------------------------------------------
// CApSettingsDlg::AskQueryIfWcdmaL
//----------------------------------------------------------
//
TBool CApSettingsDlg::AskQueryIfWcdmaL( TApMember aMember, TInt aCurrvalue  )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::AskQueryIfWcdmaL")
    
    TBool accept = ETrue ;
    TBool isWcdma = 
            FeatureManager::FeatureSupported( KFeatureIdProtocolWcdma );
    if ( isWcdma && iBearerType == EApBearerTypeCSD && 
         aMember == EApIspBearerSpeed )
        {
        TApCallSpeed newSpeed = GetBearerSpeedForSelection( aCurrvalue );
        switch ( newSpeed)
            {
            case KSpeed28800:
            case KSpeed38400:
            case KSpeed56000: 
                {
                accept = AskQueryL( R_APUI_IAP_CONF_MAX_SPEED_WCDMA );
                break;
                }
            default:
                {
                break;
                }

            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::AskQueryIfWcdmaL")
    return accept;
    }



//----------------------------------------------------------
// CApSettingsDlg::GetDomainNameServersTypeL
//----------------------------------------------------------
//
TInt CApSettingsDlg::GetDomainNameServersTypeL()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::GetDomainNameServersTypeL")
    
    TInt retval( 0 );
    // GPRS, net type 4:
    //      if IspNameServ <> 0.0.0.0 => User defined
    // GPRS, net type 6: 
    //      if IspIPv6NameServ <> 0.0.0.0 => User defined OR well-known
    // CSD: 
    //      if in IPv4settings: 
    //          if IspNameServ <> 0.0.0.0 => User defined
    // CSD: 
    //      if in IPv6settings: 
    //          if IspIPv6NameServ <> 0.0.0.0 => User defined OR well-known

    if ( iApItem->BearerTypeL() == EApBearerTypeGPRS )
        {
        TUint32 ipnettype( 0 );
        iApItem->ReadUint( EApGprsPdpType, ipnettype );
        if ( ipnettype == EIPv6 )
            {
            retval = GetIPv6DNSTypeL();
            }
        else
            {
            retval = GetIPv4DNSTypeL();
            }
        }
    else
        { // CSD
        // IPv4settings: IspNameServ <> 0.0.0.0 => User defined
        // IPv6settings: 
        //      IspIPv6NameServ <> 0.0.0.0 => User defined OR well-known
        if ( iL2Ipv4 )
            { // IPv4 settings
            retval = GetIPv4DNSTypeL();
            }
        else
            { // IPv6 settings
            retval = GetIPv6DNSTypeL();
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::GetDomainNameServersTypeL")
    return retval;
    }



//----------------------------------------------------------
// CApSettingsDlg::GetIpv4NameServerL
//----------------------------------------------------------
//
TInt CApSettingsDlg::GetIpv4NameServerL()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::GetIpv4NameServerL")
    
    HBufC* dns1 = HBufC::NewLC( KModifiableTextLength );
    HBufC* dns2 = HBufC::NewLC( KModifiableTextLength );
    TPtr dns1ptr( dns1->Des() );
    TPtr dns2ptr( dns2->Des() );
    
    TInt ResId( 0 );

    iApItem->ReadTextL( EApGprsIPNameServer1, dns1ptr );
    iApItem->ReadTextL( EApGprsIPNameServer2, dns2ptr );
    ResId = R_DNSIP4_QUERY;

    TInetAddr dns1Addr;
    TInetAddr dns2Addr;
    dns1Addr.Input( dns1ptr );
    dns2Addr.Input( dns2ptr );

    CAknMultiLineIpQueryDialog* ipquery = 
        CAknMultiLineIpQueryDialog::NewL( dns1Addr, dns2Addr );

    TInt retval = ipquery->ExecuteLD( ResId );
    if ( retval )
        {
        dns1Addr.Output( dns1ptr );
        dns2Addr.Output( dns2ptr );
        if ( dns1ptr.Compare( KDynIpAddress )  == 0 )
            { // move 2nd to first, set second to Dynamic...
            iApItem->WriteTextL( EApGprsIPNameServer1, dns2ptr );
            iApItem->WriteTextL( EApGprsIPNameServer2, KDynIpAddress );
            }
        else
            {
            iApItem->WriteTextL( EApGprsIPNameServer1, dns1ptr );
            iApItem->WriteTextL( EApGprsIPNameServer2, dns2ptr );
            }
        }   
    CleanupStack::PopAndDestroy( 2 ); // dns1, dns2
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::GetIpv4NameServerL")
    return retval;
    }


//----------------------------------------------------------
// CApSettingsDlg::GetIpv6NameServerL
//----------------------------------------------------------
//
TInt CApSettingsDlg::GetIpv6NameServerL()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::GetIpv6NameServerL")
    
    HBufC* dns1 = HBufC::NewLC( KModifiableTextLength );
    HBufC* dns2 = HBufC::NewLC( KModifiableTextLength );
    TPtr dns1ptr( dns1->Des() );
    TPtr dns2ptr( dns2->Des() );
    
    TInt ResId( 0 );

    iApItem->ReadTextL( EApIP6NameServer1, dns1ptr );
    iApItem->ReadTextL( EApIP6NameServer2, dns2ptr );
    ResId = R_DNSIP6_QUERY;

    if ( ( dns1ptr.Compare( KKnownNameServer1 ) == 0 )
        && ( dns2ptr.Compare( KKnownNameServer2 ) == 0 ) )
        { // special value, well-known
        // in this case 'dynamic' string must be offered so 
        // temp data must be changed to 'dynamic'
        dns1ptr = KDynIpv6Address;
        dns2ptr = KDynIpv6Address;
        }

    CApMultiLineDataQueryDialog* ipquery = 
        CApMultiLineDataQueryDialog::NewL( dns1ptr, dns2ptr );

    TInt retval = ipquery->ExecuteLD( ResId );
    if ( retval )
        {
        TInetAddr dns1Addr;
        TInetAddr dns2Addr;
        TInt err1( KErrNone );
        TInt err2( KErrNone );
        if ( dns1ptr.Compare( KEmpty )  != 0 )
            {
            err1 = dns1Addr.Input( dns1ptr );
            }
        if ( dns2ptr.Compare( KEmpty )  != 0 )
            {
            err2 = dns2Addr.Input( dns2ptr );
            }
        if ( err1 || err2 )
            {
            ShowNoteL( R_APUI_VIEW_SET_INFO_INVALID_IP );
            GetIpv6NameServerL();
            }
        else
            {
            if ( 
                ( dns1ptr.Compare( KDynIpv6Address )  == 0 ) ||
                ( dns1ptr.Compare( KEmpty )  == 0 ) 
               )
                { // swap them: move 2nd to first, set second to Dynamic...
                iApItem->WriteTextL( EApIP6NameServer1, dns2ptr );
                iApItem->WriteTextL( EApIP6NameServer2, KDynIpv6Address );
                }
            else
                { // keep the original order
                iApItem->WriteTextL( EApIP6NameServer1, dns1ptr );
                iApItem->WriteTextL( EApIP6NameServer2, dns2ptr );
                }
            }
        }   
    CleanupStack::PopAndDestroy( 2 ); // dns1, dns2
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::GetIpv6NameServerL")
    return retval;
    }



//----------------------------------------------------------
// CApSettingsDlg::GetIPv6DNSTypeL
//----------------------------------------------------------
//
TInt CApSettingsDlg::GetIPv6DNSTypeL()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::GetIPv6DNSTypeL")
    
    TInt retval( 0 );

    HBufC* buf1 = HBufC::NewLC( KModifiableTextLength );
    TPtr16 ptr1( buf1->Des() );

    HBufC* buf2 = HBufC::NewLC( KModifiableTextLength );
    TPtr16 ptr2( buf2->Des() );

    // net type 6: if IspIPv6NameServ <> 0.0.0.0 
    // => User defined OR well-known
    iApItem->ReadTextL( EApIP6NameServer1, ptr1 );
    iApItem->ReadTextL( EApIP6NameServer2, ptr2 );

    if ( ( ptr1.Compare( KKnownNameServer1 ) == 0 )
        && ( ptr2.Compare( KKnownNameServer2 ) == 0 ) )
        { // special value, well-known
        retval = 1;
        }
    else
        { 
        // dynamic, if 0:0:...
        // OR EMPTY text...
        if ( ( ( ptr1.Compare( KDynIpv6Address ) == 0 )
               || ( ptr1.Compare( KEmpty ) == 0 ) )
           &&( ( ptr2.Compare( KDynIpv6Address ) == 0 ) 
               ||( ptr2.Compare( KEmpty ) == 0 ) ) )
            { // dynamic, 0:0:...
            // OR in case of CSD, HSCSD, it might be user defined, 
            // as for those bearers Dynamic is not possible...
            TApBearerType bt = iApItem->BearerTypeL();
            switch (bt) 
                {
                case EApBearerTypeCSD:
                case EApBearerTypeHSCSD:
                    {
                    retval = 2; // consider it user defined
                    break;
                    }
                default:
                    {
                    retval = 0;
                    break;
                    }
                }
            }
        else
            { // other value, user defined
            retval = 2;
            }
        }

    CleanupStack::PopAndDestroy( 2 );

    APSETUILOGGER_LEAVEFN( ESettings,"Settings::GetIPv6DNSTypeL")
    return retval;
    }





//----------------------------------------------------------
// CApSettingsDlg::GetIPv4DNSTypeL
//----------------------------------------------------------
//
TInt CApSettingsDlg::GetIPv4DNSTypeL()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::GetIPv4DNSTypeL")
    
    TInt retval( 0 );

    HBufC* buf1 = HBufC::NewLC( KModifiableTextLength );
    TPtr16 ptr1( buf1->Des() );

    HBufC* buf2 = HBufC::NewLC( KModifiableTextLength );
    TPtr16 ptr2( buf2->Des() );

    // net type 4: if IspNameServ <> 0.0.0.0 => User defined
    iApItem->ReadTextL( EApGprsIPNameServer1, ptr1 );
    iApItem->ReadTextL( EApGprsIPNameServer2, ptr2 );

    if ( ( ptr1.Compare( KDynIpAddress ) == 0 )
    && ( ptr2.Compare( KDynIpAddress ) == 0 ) )
        { // dynamic, 0:0:...
        retval = 0;
        }
    else
        { // other value, user defined
        retval = 2;
        }

    CleanupStack::PopAndDestroy( 2 );

    APSETUILOGGER_LEAVEFN( ESettings,"Settings::GetIPv4DNSTypeL")
    return retval;
    }





// ---------------------------------------------------------
// CApSettingsDlg::StripNonNumberLC
// ---------------------------------------------------------
//
HBufC* CApSettingsDlg::StripNonNumberLC( const TDesC16& aInText )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::StripNonNumberLC")
    
//    CLOG( ( EApItem, 0, _L( "-> CApSettingsDlg::StripNonNumberLC" ) ) );

    TInt size = aInText.Length();
    HBufC* aOutText = HBufC::NewLC( size );
    TPtr ptr = aOutText->Des();

    for ( TInt ii=0; ii<size; ++ii )
        {
        // TText ch = aInText[ii];
        TChar ch = aInText[ii];
        if ( ch.IsDigit() )
            {
            ptr.Append( ch );
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::StripNonNumberLC")
    return aOutText;
    }
    

//----------------------------------------------------------
// CApSettingsDlg::BearerType2BearerItemPosL
//----------------------------------------------------------
//
TInt CApSettingsDlg::BearerType2BearerItemPosL( TApBearerType aBearer )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::BearerType2BearerItemPosL")
    
    TInt pos(0);
    switch ( aBearer )
        {
        case EApBearerTypeGPRS:
            {
            pos = KBearerListGprs;
            break;
            }
        case EApBearerTypeWLAN:
            {
            if ( IsWlanSupported() )
                {                
                pos = KBearerListWLAN;
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }            
        case EApBearerTypeCSD:
            {
            TBool isCsdSupported = 
                  FeatureManager::FeatureSupported( KFeatureIdAppCsdSupport );
            if ( isCsdSupported )
                {
                pos = KBearerListCSD;
	            if ( !IsWlanSupported() )
	                {
	                pos--;
	                }
                }
            else
                { // To be determined, what is correct
                }
            break;
            }
        case EApBearerTypeHSCSD:
            {
            if ( IsHSCSDEnabledL() )
                {
                pos = KBearerListHSCSD;
				if ( !IsWlanSupported() )
                	{
                	pos--;
                	}                
                }
            else
                { // should not be possible, but be defensive, 
                // correct it to CSD, if possible...
                TBool isCsdSupported = 
                      FeatureManager::FeatureSupported( 
                                        KFeatureIdAppCsdSupport );
                if ( isCsdSupported )
                    {
                    pos = KBearerListCSD;
					if ( !IsWlanSupported() )
	                	{
	                	pos--;
	                	}
                    }
                else
                    { // To be determined, what is correct
                    }
                }
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EInvalidBearerType ) );
            User::Leave( KErrInvalidBearerType );
            break;
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::BearerType2BearerItemPosL")
    return pos;
    }


//----------------------------------------------------------
// CApSettingsDlg::BearerItemPos2BearerTypeL
//----------------------------------------------------------
//
TApBearerType CApSettingsDlg::BearerItemPos2BearerTypeL( TInt aPos )
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::BearerItemPos2BearerTypeL")
    
    TApBearerType bearer;
    CArrayFixFlat<TApBearerType>* bearerlist = 
                new(ELeave) CArrayFixFlat<TApBearerType>(4);
    CleanupStack::PushL( bearerlist );

    bearerlist->AppendL( EApBearerTypeGPRS );
  
    if ( IsWlanSupported() )
        {
        bearerlist->AppendL( EApBearerTypeWLAN );
        }
    
    TBool isCsdSupported = 
          FeatureManager::FeatureSupported( KFeatureIdAppCsdSupport );
    if ( isCsdSupported )
        {
        bearerlist->AppendL( EApBearerTypeCSD );
        }
    if ( IsHSCSDEnabledL() && isCsdSupported )
        {
        bearerlist->AppendL( EApBearerTypeHSCSD );
        }

    __ASSERT_DEBUG( ( aPos < bearerlist->Count() ), \
                      Panic( EInvalidBearerType ) );
    __ASSERT_DEBUG( ( aPos >= 0 ), Panic( EInvalidBearerType ) );
    
    aPos = Max( aPos, 0 );
    aPos = Min( aPos, ( bearerlist->Count() - 1 ) );

    bearer = bearerlist->At( aPos );

    CleanupStack::PopAndDestroy( bearerlist );

    APSETUILOGGER_LEAVEFN( ESettings,"Settings::BearerItemPos2BearerTypeL")
    return bearer;
    }



//----------------------------------------------------------
// CApSettingsDlg::ChangeWlanNetworkNameL
//----------------------------------------------------------
//
void CApSettingsDlg::ChangeWlanNetworkNameL()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::ChangeWlanNetworkNameL")
    
    // Implement WLAN network name change 
    TInt currvalue( 0 );
    CDesCArrayFlat* items = FillPopupSettingPageLC( EApWlanNetworkName,  
                                                    currvalue );

    TInt attr_resid( 0 );
    GetResId( EApWlanNetworkName, attr_resid );
    HBufC* titlebuf;
    CAknRadioButtonSettingPage* dlg;
    if ( attr_resid )
        {
        titlebuf = iEikonEnv->AllocReadResourceLC( attr_resid );
        dlg = new ( ELeave )CAknRadioButtonSettingPage(
                                R_RADIO_BUTTON_SETTING_PAGE, 
                                currvalue, items );
        // must push 'cause SetSettingTextL can leave...
        CleanupStack::PushL( dlg );
        TPtrC ptr( titlebuf->Des() );
        dlg->SetSettingTextL( ptr );
        CleanupStack::Pop(); // dlg
        }
    else
        {
        dlg = new ( ELeave )CAknRadioButtonSettingPage(
                            R_RADIO_BUTTON_SETTING_PAGE, currvalue, items );
        }
    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted ) )
        {
        if ( !currvalue )
            {// user defined
            QueryWlanNetworkNameL();
            }
        else
            { // scan for, select one!
            CConnectionUiUtilities* connUiUtils = 
                CConnectionUiUtilities::NewL();
            CleanupStack::PushL( connUiUtils );

            TWlanSsid ssid;
            TWlanConnectionMode connmode;
            TWlanConnectionSecurityMode secmode;
            TBool ret = connUiUtils->SearchWLANNetwork( ssid, 
                                                        connmode, 
                                                        secmode );
            if ( ret )
                {
                HBufC* tmp = HBufC::NewLC( KCommsDbSvrMaxFieldLength );
                TPtr16 tmpPtr16 = tmp->Des();
                CnvUtfConverter::ConvertToUnicodeFromUtf8( tmpPtr16, ssid);
                
                iApItem->WriteTextL( EApWlanNetworkName, *tmp );
                CleanupStack::PopAndDestroy( tmp );
                // now handle connmode & secmode settings
                if ( connmode == EWlanConnectionModeInfrastructure )
                    {
                    iApItem->WriteUint( EApWlanNetworkMode, EInfra );
                    }
                else
                    {
                    if ( connmode == EWlanConnectionModeAdhoc )
                        {
                        iApItem->WriteUint( EApWlanNetworkMode, EAdhoc );
                        }
                    else
                        {
                        User::Leave( KErrNotSupported );
                        }                    
                    }
                switch (secmode)
                    {
                    case EWlanConnectionSecurityOpen:
                        {
                        iApItem->WriteUint( EApWlanSecurityMode, EOpen );
                        break;
                        }
                    case EWlanConnectionSecurityWep:
                        {
                        iApItem->WriteUint( EApWlanSecurityMode, EWep );
                        break;
                        }
                    case EWlanConnectionSecurity802d1x:
                        {
                        iApItem->WriteUint( EApWlanSecurityMode, E802_1x );
                        break;
                        }
                    case EWlanConnectionSecurityWpa:
                        {
                        iApItem->WriteUint( EApWlanSecurityMode, EWpa );
                        break;
                        }
                    case EWlanConnectionSecurityWpaPsk:
                        {
                        iApItem->WriteUint( EApWlanSecurityMode, EWpa );
                        break;
                        }
                    default:
                        {
                        break;
                        }                            
                    }
             
              }
            CleanupStack::PopAndDestroy( connUiUtils );
            }
        }
    if ( attr_resid )
        {
        // titlebuf, text title readed from resource...
        CleanupStack::PopAndDestroy();
        }
    // items, will also delete all elements in the array!
    CleanupStack::PopAndDestroy( items ); 
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::ChangeWlanNetworkNameL")
    }


//----------------------------------------------------------
// CApSettingsDlg::ChangeWlanSecuritySettings
//----------------------------------------------------------
//
TInt CApSettingsDlg::ChangeWlanSecuritySettingsL()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::ChangeWlanSecuritySettingsL")
    
    TInt retval(0);
    TUint32 secmode( 0 );
    iApItem->ReadUint( EApWlanSecurityMode, secmode );

    TBool need2changeid( EFalse );
    TUint32 origServiceId(0);
    iApItem->ReadUint( EApIapServiceId, origServiceId );
    if ( iOriginalBearerType != iApItem->BearerTypeL() )
        {
        need2changeid = ETrue;
        iApItem->WriteUint( EApIapServiceId, 0 );        
        }
        
    switch ( secmode )
        {
        case EOpen:
            {
            ShowNoteL( R_APUI_VIEW_NO_SECURITY_SETTINGS );
            break;
            }
        case EWep:
            {
            retval = ChangeWepSecuritySettingsL();
            break;
            }
        case E802_1x:
            {
            retval = Change8021xSecuritySettingsL();
            break;
            }
        case EWpa:
        case EWpa2:
            {
            retval = ChangeWpaSecuritySettingsL();
            break;
            }
        default:
            {
            // some weird error, repair it...
            __ASSERT_DEBUG( EFalse, Panic( EInvalidBearerType ) );
            break;
            }
        }
    if ( need2changeid )
        {
        iApItem->WriteUint( EApIapServiceId, origServiceId );
        }

    APSETUILOGGER_LEAVEFN( ESettings,"Settings::ChangeWlanSecuritySettingsL")
    return retval;
    }


//----------------------------------------------------------
// CApSettingsDlg::ChangeWepSecuritySettingsL
//----------------------------------------------------------
//
TInt CApSettingsDlg::ChangeWepSecuritySettingsL()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::ChangeWepSecuritySettingsL<->")
    
    return iHandler->iModel->ChangeWepSettingsL( iApItem );
    }



//----------------------------------------------------------
// CApSettingsDlg::ChangeWpaSecuritySettingsL
//----------------------------------------------------------
//
TInt CApSettingsDlg::ChangeWpaSecuritySettingsL()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::ChangeWpaSecuritySettingsL<->")
    
    return iHandler->iModel->ChangeWpaSettingsL( iApItem );
    }


//----------------------------------------------------------
// CApSettingsDlg::Change8021xSecuritySettingsL
//----------------------------------------------------------
//
TInt CApSettingsDlg::Change8021xSecuritySettingsL()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::Change8021xSecuritySettingsL<->")
    
    return iHandler->iModel->Change8021xSettingsL( iApItem );
    }




//----------------------------------------------------------
// CApSettingsDlg::IsWlanSupported
//----------------------------------------------------------
//
TBool CApSettingsDlg::IsWlanSupported()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::IsWlanSupported")
    
    TBool retval = FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );
#ifdef __TEST_WLAN_SUPPORT
    retval = ETrue;
#endif // __TEST_WLAN_SUPPORT
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::IsWlanSupported")
    return retval;
    }
    
    

//----------------------------------------------------------
// CApSettingsDlg::QueryWlanNetworkNameL
//----------------------------------------------------------
//
TBool CApSettingsDlg::QueryWlanNetworkNameL()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::QueryWlanNetworkNameL")
    
    TBool retval( EFalse );
    
    HBufC* buff = HBufC::NewLC( KModifiableTextLength );
    TPtr16 ptr( buff->Des() );
    iApItem->ReadTextL( EApWlanNetworkName, ptr );   

    CAknQueryDialog* dlg = 
                CAknQueryDialog::NewL( ptr, CAknQueryDialog::ENoTone );
    
    if ( dlg->ExecuteLD( R_APUI_WLAN_NAME_QUERY ) )
        {
        iApItem->WriteTextL( EApWlanNetworkName, ptr );
        retval = ETrue;
        }
    CleanupStack::PopAndDestroy( buff );
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::QueryWlanNetworkNameL")
    return retval;
    }


//----------------------------------------------------------
// CApSettingsDlg::IsNetMaskAndGatewayVisible
//----------------------------------------------------------
//
TBool CApSettingsDlg::IsNetMaskAndGatewayVisibleL()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::IsNetMaskAndGatewayVisibleL")
    
    TBool retval(EFalse);
    // display only if:
    // Phone IP Addr. <> Automatic (0.0.0.0)
    HBufC* tmpValue = HBufC::NewLC( KModifiableTextLength );
    TPtr ptr( tmpValue->Des() );
    iApItem->ReadTextL( EApIspIPAddr, ptr );
    if ( ( tmpValue->Compare( KDynIpAddress ) != 0 ) 
        && ( tmpValue->Compare( KEmptyText ) != 0 ) )    
        {
        retval = ETrue;
        }
    CleanupStack::PopAndDestroy( tmpValue );
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::IsNetMaskAndGatewayVisibleL")
    return retval;
    }



//----------------------------------------------------------
// CApSettingsDlg::GetAdHocChannelL
//----------------------------------------------------------
//
TUint32 CApSettingsDlg::GetAdHocChannelL()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::GetAdHocChannelL")
    
    // Get real ad-hoc channel
    // Default: KDefAdhocChannel
    TInt channel( KDefAdhocChannel );
    TUint32 retval(0);
    iApItem->ReadUint( EApWlanChannelId, retval );
    channel = TInt(retval);
    if ( !channel )
        {
        channel = KDefAdhocChannel;
        }

    CAknNumberQueryDialog* dlg = 
            CAknNumberQueryDialog::NewL( channel, CAknQueryDialog::ENoTone );
    
    if ( dlg->ExecuteLD( R_APUI_WLAN_ADHOC_CHANNEL_QUERY ) )
        {
        retval = channel;
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::GetAdHocChannelL")
    return retval;
    }
    
    
    
//----------------------------------------------------------
// CApSettingsDlg::IsAdhocChannelVisible
//----------------------------------------------------------
//
TBool CApSettingsDlg::IsAdhocChannelVisible() 
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::IsAdhocChannelVisible")
    
    TBool retval(EFalse);
    TUint32 netmode( 0 );
    iApItem->ReadUint( EApWlanNetworkMode, netmode );
    if ( netmode == EAdhoc )
        {
        retval = ETrue;
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::IsAdhocChannelVisible")
    return retval;
    }
    
    

//----------------------------------------------------------
// CApSettingsDlg::HasSecuritySettingsFilledL
//----------------------------------------------------------
//
TBool CApSettingsDlg::HasSecuritySettingsFilledL()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::HasSecuritySettingsFilledL<->")
    
    return iDataModel->HasWlanSecSettingsFilledL( *iApItem );
    }



//----------------------------------------------------------
// CApSettingsDlg::LimitSecMode
//----------------------------------------------------------
//
void CApSettingsDlg::LimitSecMode()
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::LimitSecMode")
    
    TUint32 secmode( 0 );
    iApItem->ReadUint( EApWlanSecurityMode, secmode );
    switch ( secmode )
        {
        case EOpen:
        case EWep:
        // nothing to do in this case 
            {
            break;
            }
        case E802_1x:
        case EWpa:
        case EWpa2:
        default:
            {
            iApItem->WriteUint( EApWlanSecurityMode, EOpen );
            break;
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::LimitSecMode")
    }




// ---------------------------------------------------------
// CApSettingsDlg::CanSaveWlanCompulsoriesL( )
// ---------------------------------------------------------
//
CApSettingsDlg::TSaveAction CApSettingsDlg::CanSaveWlanCompulsoriesL( 
                                                TApMember& aDataNeeded )
    {
    TSaveAction retval( EApCanSave );

    HBufC *sgd = HBufC::NewLC( KModifiableTextLength );
    TPtr16 ptr( sgd->Des() );

    if ( (retval != EApShallGoBack) && (retval != EApMustDelete) 
         && IsNetMaskAndGatewayVisibleL() )
        { // now check compulsory fields
        iApItem->ReadTextL( EApWlanIpNetMask, ptr );
        if ( ( ptr.Compare( KDynIpAddress ) == 0 ) 
            || ( ptr.Compare( KEmptyText ) == 0 ) )
            {
            retval = EApShallGoBack;
            aDataNeeded = EApWlanIpNetMask;
            if ( AskQueryL( 
                R_APUI_VIEW_NETW_QUEST_IAP_INCOMPLETE_DELETE ) )
                {
                retval = EApMustDelete;
                }                                                
            }
        else
            {                        
            iApItem->ReadTextL( EApIspIPGateway, ptr );
            if ( ( sgd->Compare( KDynIpAddress ) == 0 ) 
                || ( sgd->Compare( KEmptyText ) == 0 ) )
                {
                retval = EApShallGoBack;
                aDataNeeded = EApIspIPGateway;
                if ( AskQueryL( 
                    R_APUI_VIEW_NETW_QUEST_IAP_INCOMPLETE_DELETE )
                   )
                    {
                    retval = EApMustDelete;
                    }                            
                }
            }
        }
    CleanupStack::PopAndDestroy( sgd );  // sgd
        
    return retval;
    }


// ---------------------------------------------------------
// CApSettingsDlg::RestoreIpv4SettingsL()
// ---------------------------------------------------------
//
void CApSettingsDlg::RestoreIpv4SettingsL()
    {
    HBufC* buf = HBufC::NewLC( KModifiableTextLength );
    TPtr ptr( buf->Des() );
    
    iBackupApItem->ReadTextL( EApIspIPAddr, ptr );
    iApItem->WriteTextL( EApIspIPAddr, ptr );
    
    iBackupApItem->ReadTextL( EApIspIPNetMask, ptr );
    iApItem->WriteTextL( EApIspIPNetMask, ptr );
    
    iBackupApItem->ReadTextL( EApIspIPGateway, ptr );
    iApItem->WriteTextL( EApIspIPGateway, ptr );

    iBackupApItem->ReadTextL( EApIspIPNameServer1, ptr );
    iApItem->WriteTextL( EApIspIPNameServer1, ptr );

    iBackupApItem->ReadTextL( EApIspIPNameServer2, ptr );
    iApItem->WriteTextL( EApIspIPNameServer2, ptr );

    CleanupStack::PopAndDestroy( buf );
    
    }


    
// End of File
