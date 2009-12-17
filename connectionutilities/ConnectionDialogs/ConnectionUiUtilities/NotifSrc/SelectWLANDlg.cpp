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
*     Defines dialog CSelectWLANDlg from Connection Ui Utilities
*
*/


// INCLUDE FILES

#include <AknIconArray.h>
#include <AknsUtils.h>
#include <AknIconUtils.h>
#include <AknWaitDialog.h>
#include <aknnotewrappers.h>
#include <aknnotedialog.h>
#include <connectionuiutilities.mbg>
#include <ConnUiUtilsNotif.rsg>
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>
#include <uikon/eiksrvui.h>
#include <utf.h>
#include <AknGlobalNote.h>

#include "ConnUiUtilsNotif.hrh"
#include "SelectWLANDlg.h"
#include "SelectWLanDlgPlugin.h"
#include "ConnectionDialogsLogger.h"
#include "NetworkInfoArray.h"
#include "wlancoveragecheck.h"
#include "ExpiryTimer.h"

// CONSTANTS

#if defined(_DEBUG)
_LIT( KErrNullPointer, "NULL pointer" );
#endif

// ROM folder
_LIT( KDriveZ, "z:" );

// Name of the MBM file containing icons
_LIT( KFileIcons, "ConnectionUiUtilities.mbm" );


LOCAL_D const TInt KIconsGranularity = 4;


// ================= MEMBER FUNCTIONS =======================

// Constructor
CSelectWLANDlg::CSelectWLANDlg( CSelectWLanDlgPlugin* aPlugin, TInt aIndex,
                                CNetworkInfoArray* *const aNetworkArray )
: CAknListQueryDialog( &aIndex ),
  iPlugin( aPlugin ),
  iNetworkArray( aNetworkArray ),
  iFromOkToExit( EFalse )
    {
    }


// Destructor
CSelectWLANDlg::~CSelectWLANDlg()
    {
    CLOG_ENTERFN( "CSelectWLANDlg::~CSelectWLANDlg " ); 
    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( EFalse );
    CLOG_LEAVEFN( "CSelectWLANDlg::~CSelectWLANDlg " );
    delete iExpiryTimer;
    }


// ---------------------------------------------------------
// CSelectWLANDlg::NewL
// ---------------------------------------------------------
//
CSelectWLANDlg* CSelectWLANDlg::NewL( CSelectWLanDlgPlugin* aPlugin, 
                                      CNetworkInfoArray* *const aNetworkArray )
    {
    CLOG_ENTERFN( "CSelectWLANDlg::NewL " );  
    
    CSelectWLANDlg* self = new ( ELeave )CSelectWLANDlg( aPlugin, 0, 
                                                        aNetworkArray );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    CLOG_LEAVEFN( "CSelectWLANDlg::NewL " );  

    return self;
    }


// ---------------------------------------------------------
// CSelectWLANDlg::ConstructL
// ---------------------------------------------------------
//
void CSelectWLANDlg::ConstructL()
    {
    }
    
// ---------------------------------------------------------
// CSelectWLANDlg::PreLayoutDynInitL()
// called by framework before dialog is shown
// ---------------------------------------------------------
//
void CSelectWLANDlg::PreLayoutDynInitL()
    {
    CLOG_ENTERFN( "CSelectWLANDlg::PreLayoutDynInitL " );  
    
    CAknListQueryDialog::PreLayoutDynInitL();

    SetIconsL();
    
    CLOG_WRITEF( _L( "iNetworkArray : %d" ),  *iNetworkArray );
    
    SetOwnershipType( ELbmOwnsItemArray );    
    SetItemTextArray( *iNetworkArray );
    SetOwnershipType( ELbmDoesNotOwnItemArray );

    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( ETrue );
    
    iExpiryTimer = CExpiryTimer::NewL( *this );
    iExpiryTimer->Start();
    
    CLOG_LEAVEFN( "CSelectWLANDlg::PreLayoutDynInitL " );      
    }    
    
    
// ---------------------------------------------------------
// CSelectWLANDlg::RefreshDialogL
// ---------------------------------------------------------
//    
void CSelectWLANDlg::RefreshDialogL()
    {  
    CLOG_ENTERFN( "CSelectWLANDlg::RefreshDialogL " );  
     
    SetItemTextArray( *iNetworkArray );
    ListBox()->HandleItemAdditionL();
    
    TInt current = ListBox()->CurrentItemIndex(); 
    
    if ( current == KErrNotFound )
        {
        // Highligt has been lost since the new list is
        // shorter than the previous one. Set highligt to bottom.
        TInt bottom = ListBox()->Model()->NumberOfItems()- 1;
    	ListBox()->SetCurrentItemIndexAndDraw( bottom );
        }  
    
    Layout();   
    SizeChanged();
    DrawNow();
    
    CLOG_LEAVEFN( "CSelectWLANDlg::RefreshDialogL " );  
              
    }

// ---------------------------------------------------------
// CSelectWLANDlg::OkToExitL( TInt aButtonId)
// called by framework when the OK button is pressed
// ---------------------------------------------------------
//
TBool CSelectWLANDlg::OkToExitL( TInt aButtonId )
    {
    CLOG_ENTERFN( "CSelectWLANDlg::OkToExitL " );  
    
    TBool result( EFalse );
    if ( aButtonId == EAknSoftkeySelect || aButtonId == EAknSoftkeyOk )
        {
        __ASSERT_DEBUG( iPlugin, User::Panic( KErrNullPointer, KErrNone ) );
        
        iFromOkToExit = ETrue;
        
        TInt index = ListBox()->CurrentItemIndex();
        CNetworkInfoBase* tempNetworkItem = ( **iNetworkArray )[index ];
                
        TWlanSsid sSID;
        if ( tempNetworkItem->OthersEntry() )
            {
            CLOG_WRITE( "tempNetworkItem->OthersEntry()" );

            iPlugin->StopTimer();
            
            HBufC* buff = HBufC::NewLC( KWlanMaxSsidLength );
            TPtr16 ptr( buff->Des() );

            CAknQueryDialog* dlg = 
                        CAknQueryDialog::NewL( ptr, CAknQueryDialog::ENoTone );
    
            if ( dlg->ExecuteLD( R_WLAN_NAME_QUERY ) )
                {
                CLOG_WRITE( "dlg->ExecuteLD( R_WLAN_NAME_QUERY )" );

                TWlanConnectionMode nMode;
                TWlanConnectionSecurityMode sMode;
                TWlanConnectionExtentedSecurityMode sExtMode;

                sSID.Copy( buff->Des() );       
                CleanupStack::PopAndDestroy( buff );

                iPlugin->SetEasyWlanToHiddenL( ETrue ); 

                TBool protSetupSupp;
                if ( ScanForWlanNwL( sSID, nMode, sMode, sExtMode, protSetupSupp ) )
                    {
                    CLOG_WRITEF( _L( "Protected Setup in CSelectWLANDlg::OkToExitL: %d" ), protSetupSupp );
                    CLOG_WRITE( "Setting prefs and completing" );

                    iPlugin->SetNetworkPrefsL( sSID, nMode, sMode, 
                                               sExtMode, protSetupSupp );
                    iPlugin->CompleteL( KErrNone );
                    }
                else
                    {
                    HBufC* stringLabel = StringLoader::LoadLC( 
                                                    R_INFO_NO_NETWORKS_FOUND );

                    //RAknUiServer* globalNote = CAknSgcClient::AknSrv();
                    //if ( globalNote->Handle() )
                    //    {
                    //    globalNote->ShowGlobalNoteL( *stringLabel, 
                    //                            EAknGlobalInformationNote );
                    //    }
                        
                    CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
                    globalNote->ShowNoteL( EAknGlobalInformationNote, *stringLabel );
                    CleanupStack::PopAndDestroy( globalNote );
                        
                    CleanupStack::PopAndDestroy( stringLabel );
                    // set network prefs because we still need ssid

                    iPlugin->SetNetworkPrefsL( sSID, nMode, sMode, 
                                               sExtMode, protSetupSupp );
                    iPlugin->CompleteL( KErrNotFound );
                    }
                }
            else
                {
                CleanupStack::PopAndDestroy( buff );
                iPlugin->CompleteL( KErrCancel );
                }
            }
        else
            {
            iPlugin->SetEasyWlanToHiddenL( EFalse );
  
            //sSID.Copy( tempNetworkItem->NetworkName() );
            CnvUtfConverter::ConvertFromUnicodeToUtf8( sSID, tempNetworkItem->NetworkName() );
        
            TWlanConnectionMode nMode = 
                            ( TWlanConnectionMode )tempNetworkItem->NetworkMode();
            TWlanConnectionSecurityMode sMode = tempNetworkItem->SecurityMode();
            TWlanConnectionExtentedSecurityMode sExtMode = tempNetworkItem->ExtendedSecurityMode();
            
            // ADHOC is supported only with security modes: open and WEP.
            if ( nMode == EWlanConnectionModeAdhoc && 
                 ( sMode != EWlanConnectionSecurityOpen &&
                   sMode != EWlanConnectionSecurityWep) )
                {
                ShowInfoNoteL( R_CONNUIUTILITIES_NOT_SUPPORTED );
                CLOG_LEAVEFN( "CSelectWLANDlg::OkToExitL " );  
                return EFalse;
                }
                
            iPlugin->StopTimer();    

            TBool protSetupSupp = tempNetworkItem->ProtectedSetupSupported();
     
            CLOG_WRITEF( _L( "Protected Setup in CSelectWLANDlg::OkToExitL: %d" ), protSetupSupp );
            iPlugin->SetNetworkPrefsL( sSID, nMode, sMode, sExtMode, protSetupSupp );
            iPlugin->CompleteL( KErrNone );
            }
        
        result =  ETrue;
        }
    else if ( aButtonId == EAknSoftkeyCancel )
        {
        if ( iFromOkToExit )
            {
            // ssid query dialog is open
            result =  EFalse; 
            }
        else
            {
            iFromOkToExit = ETrue;
        
            iPlugin->StopTimer();
            iPlugin->CompleteL( KErrCancel );

            result =  ETrue;    
            }
        }

    CLOG_LEAVEFN( "CSelectWLANDlg::OkToExitL " );  

    return result;
    }


// ---------------------------------------------------------
// CSelectWLANDlg::SetIconsL()
// ---------------------------------------------------------
//
void CSelectWLANDlg::SetIconsL()
    {
    // Don't change the order of the icons, or if anything should be 
    // added/removed, then also enum TIconType in ConnUiUtilsNotif.hrh should 
    // be updated!

    CLOG_ENTERFN( "CSelectWLANDlg::SetIconsL " ); 
    
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;         
    
    CArrayPtr< CGulIcon >* icons = new( ELeave ) CAknIconArray( 
                                                        KIconsGranularity );
    CleanupStack::PushL( icons );

    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();

    TFileName iconsFileName;

    iconsFileName.Append( KDriveZ );

    iconsFileName.Append( KDC_APP_BITMAP_DIR );

    iconsFileName.Append( KFileIcons );
    
    CLOG_WRITEF( _L( "iconsFileName : %S" ),  &iconsFileName );
    
    CGulIcon* icon = CGulIcon::NewLC();
    
    AknsUtils::CreateColorIconL( skinInstance, 
                    KAknsIIDQgnIndiWlanSignalLowAdd, 
                    KAknsIIDQsnIconColors, EAknsCIQsnIconColorsCG13, 
                    bitmap, 
                    mask, 
                    iconsFileName,
                    EMbmConnectionuiutilitiesQgn_indi_wlan_signal_low_add, 
                    EMbmConnectionuiutilitiesQgn_indi_wlan_signal_low_add_mask,
                    KRgbBlack );
    icon->SetBitmap( bitmap );
    icon->SetMask( mask );    
    icons->AppendL( icon );
                
    CleanupStack::Pop( icon ); 

    icon = CGulIcon::NewLC();
    
    AknsUtils::CreateColorIconL( skinInstance, 
                    KAknsIIDQgnIndiWlanSignalMedAdd, 
                    KAknsIIDQsnIconColors, EAknsCIQsnIconColorsCG13, 
                    bitmap, 
                    mask, 
                    iconsFileName,
                    EMbmConnectionuiutilitiesQgn_indi_wlan_signal_med_add, 
                    EMbmConnectionuiutilitiesQgn_indi_wlan_signal_med_add_mask,
                    KRgbBlack );
    icon->SetBitmap( bitmap );
    icon->SetMask( mask );    
    icons->AppendL( icon );
                
    CleanupStack::Pop( icon );
    
    icon = CGulIcon::NewLC();
    
    AknsUtils::CreateColorIconL( skinInstance, 
                KAknsIIDQgnIndiWlanSignalGoodAdd, 
                KAknsIIDQsnIconColors, EAknsCIQsnIconColorsCG13, 
                bitmap, 
                mask, 
                iconsFileName,
                EMbmConnectionuiutilitiesQgn_indi_wlan_signal_good_add, 
                EMbmConnectionuiutilitiesQgn_indi_wlan_signal_good_add_mask,
                KRgbBlack );
    icon->SetBitmap( bitmap );
    icon->SetMask( mask );    
    icons->AppendL( icon );
                
    CleanupStack::Pop( icon );     
        
    icon = CGulIcon::NewLC();
    
    AknsUtils::CreateColorIconL( skinInstance, 
                KAknsIIDQgnIndiWlanSecureNetworkAdd, 
                KAknsIIDQsnIconColors, EAknsCIQsnIconColorsCG13, 
                bitmap, 
                mask, 
                iconsFileName,
                EMbmConnectionuiutilitiesQgn_indi_wlan_secure_network_add, 
                EMbmConnectionuiutilitiesQgn_indi_wlan_secure_network_add_mask,
                KRgbBlack );
    icon->SetBitmap( bitmap );
    icon->SetMask( mask );    
    icons->AppendL( icon );
                
    CleanupStack::Pop( icon );    
       
    AknIconUtils::CreateIconL( bitmap, mask, iconsFileName,
                    EMbmConnectionuiutilitiesQgn_prop_snote_wlan_adhoc, 
                    EMbmConnectionuiutilitiesQgn_prop_snote_wlan_adhoc_mask );
                               
    TInt err( KErrNone );
    TRAP( err, icons->AppendL( CGulIcon::NewL( bitmap, mask ) ) );
    if ( err )
        {
        delete bitmap;
        delete mask;
        User::Leave( err );
        }
    
    
    AknIconUtils::CreateIconL( bitmap, mask, iconsFileName,
                    EMbmConnectionuiutilitiesQgn_prop_snote_wlan_infra, 
                    EMbmConnectionuiutilitiesQgn_prop_snote_wlan_infra_mask );
                               
    TRAP( err, icons->AppendL( CGulIcon::NewL( bitmap, mask ) ) );
    if ( err )
        {
        delete bitmap;
        delete mask;
        User::Leave( err );
        }
    
    icon = CGulIcon::NewLC();
    
    AknsUtils::CreateColorIconL( skinInstance, 
                KAknsIIDQgnIndiWifiProtectedAdd, 
                KAknsIIDQsnIconColors, EAknsCIQsnIconColorsCG13, 
                bitmap, 
                mask, 
                iconsFileName,
                EMbmConnectionuiutilitiesQgn_indi_wifi_protected_add, 
                EMbmConnectionuiutilitiesQgn_indi_wifi_protected_add,
                KRgbBlack );
    icon->SetBitmap( bitmap );
    icon->SetMask( mask );    
    icons->AppendL( icon );
                
    CleanupStack::Pop( icon );
    
    SetIconArrayL( icons );      

    CleanupStack::Pop( icons );

    CLOG_LEAVEFN( "CSelectWLANDlg::SetIconsL " );  
    }

// ----------------------------------------------------------------------------
// void CSelectWLANDlg::HandleResourceChange( TInt aType )
// Handle resource change events. 
// ----------------------------------------------------------------------------
//
void CSelectWLANDlg::HandleResourceChange( TInt aType )
    {
    CLOG_ENTERFN( "CSelectWLANDlg::HandleResourceChange " );  
    
    if ( aType == KAknsMessageSkinChange )
        {
        CAknListQueryDialog::HandleResourceChange( aType );

        TRAP_IGNORE( SetIconsL() );
        SizeChanged();
        }
    else
        {
        if ( aType == KEikDynamicLayoutVariantSwitch )
            {
            TRect mainPaneRect;
            AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                                               mainPaneRect );

            TAknLayoutRect layoutRect;
            layoutRect.LayoutRect( TRect( TPoint( 0, 0 ), 
                                   mainPaneRect.Size() ),
                                   AKN_LAYOUT_WINDOW_list_gen_pane( 0 ) );

            ListBox()->SetRect( layoutRect.Rect() );
            }

        // Base call
        CAknListQueryDialog::HandleResourceChange( aType );
        }

    CLOG_LEAVEFN( "CSelectWLANDlg::HandleResourceChange " );  
    }
         

// --------------------------------------------------------------------------
// CSelectWLANDlg::ScanForWlanNwL
// --------------------------------------------------------------------------
//
TBool CSelectWLANDlg::ScanForWlanNwL( 
#ifdef __WINS__
                         TWlanSsid& /*aSsid*/, 
                         TWlanConnectionMode& /*aConnectionMode*/,
                         TWlanConnectionSecurityMode& /*aSecurityMode*/,
                         TWlanConnectionExtentedSecurityMode& /*aExtSecurityMode*/,
                         TBool& /*aProtectedSetupSupported*/ )
#else
                         TWlanSsid& aSsid, 
                         TWlanConnectionMode& aConnectionMode,
                         TWlanConnectionSecurityMode& aSecurityMode,
                         TWlanConnectionExtentedSecurityMode& aExtSecurityMode,
                         TBool& aProtectedSetupSupported )
#endif  // __WINS__
    {
    CLOG_ENTERFN( "CSelectWLANDlg::ScanForWlanNwL" ); 
    
    TBool retVal ( EFalse );

    DestroyWaitDialog();

    CLOG_WRITE( "Creating WaitDialog..." );

    iWaitDialog = new( ELeave )CAknWaitDialog
                    ( REINTERPRET_CAST( CEikDialog**, &iWaitDialog ), ETrue );
    iWaitDialog->ExecuteLD( R_SEARCHING_WLAN_WAIT_NOTE ); 

#ifndef __WINS__
    CWlanCoverageCheck* coverage = new( ELeave )CWlanCoverageCheck;
    CleanupStack::PushL( coverage );

    CLOG_WRITE( "Coverage created, calling ScanForPromptedSsidL, broadcast" );

    retVal = coverage->ScanForPromptedSsidL( aSsid, ETrue, aConnectionMode, 
                                             aSecurityMode,
                                             aExtSecurityMode, 
                                             aProtectedSetupSupported );
    CLOG_WRITEF( _L( "Protected Setup in CSelectWLANDlg::ScanForWlanNwL_1: %d" ), aProtectedSetupSupported );
    CLOG_WRITE( "ScanForPromptedSsidL returned" );
    
    if ( !retVal )
        {
        CLOG_WRITE( "Calling ScanForPromptedSsidL, direct" );
        retVal = coverage->ScanForPromptedSsidL( aSsid, EFalse, 
                                                 aConnectionMode, 
                                                 aSecurityMode,
                                                 aExtSecurityMode,
                                                 aProtectedSetupSupported );
    CLOG_WRITEF( _L( "Protected Setup in CSelectWLANDlg::ScanForWlanNwL_2: %d" ), aProtectedSetupSupported );
    CLOG_WRITE( "ScanForPromptedSsidL returned" );
        }

    CleanupStack::PopAndDestroy( coverage );

#endif  // __WINS

    CLOG_WRITE( "Destroying WaitDialog..." );

    DestroyWaitDialog();

    return retVal;
    }


// ---------------------------------------------------------
// CSelectWLANDlg::DestroyWaitDialog()
// ---------------------------------------------------------
//
void CSelectWLANDlg::DestroyWaitDialog()
    {
    CLOG_ENTERFN( "CSelectWLANDlg::DestroyWaitDialog " );

    delete iWaitDialog;
    iWaitDialog = NULL;
    }

// ---------------------------------------------------------
// CSelectWLANDlg::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CSelectWLANDlg::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                             TEventCode aType)
    {
    if( NeedToDismissQueryL(aKeyEvent) )
        {
        return EKeyWasConsumed;
        }
 	
    return CAknListQueryDialog::OfferKeyEventL(aKeyEvent,aType);
    }
    
// ---------------------------------------------------------
// CSelectWLANDlg::NeedToDismissQueryL
// ---------------------------------------------------------
//
TBool CSelectWLANDlg::NeedToDismissQueryL(const TKeyEvent& aKeyEvent)
    {
    if (aKeyEvent.iCode == EKeyPhoneSend)
        {
        TryExitL(EEikBidCancel);
        return ETrue;
        }
        
    return EFalse;
    }

// ---------------------------------------------------------
// CSelectWLANDlg::ShowInfoNoteL
// ---------------------------------------------------------
//
void CSelectWLANDlg::ShowInfoNoteL( TInt aResId )
    {
    HBufC* stringLabel = StringLoader::LoadL( aResId, iEikonEnv );
    CleanupStack::PushL( stringLabel );

    CAknInformationNote* dialog = new ( ELeave )CAknInformationNote( ETrue );
    dialog->ExecuteLD( *stringLabel );

    CleanupStack::PopAndDestroy( stringLabel );   // stringLabel
    }

void CSelectWLANDlg::HandleTimedOut()
    {
    TRAP_IGNORE( TryExitL(EAknSoftkeyCancel) );
    }

// End of File
