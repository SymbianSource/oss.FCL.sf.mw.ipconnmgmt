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
* Description:  APControlListPlugin class.
*
*/


// INCLUDE FILES

// System includes
#include <hlplch.h>

#include <aknlists.h>        //for CListBox
#include <f32file.h>
#include <aknnotewrappers.h> //for CAknInformationNote
#include <AknQueryDialog.h>
#include <StringLoader.h>
#include <apcontrollistpluginrsc.rsg>
#include <data_caging_path_literals.hrh>
#include <apcontrollistplugin.mbg>
#include <featmgr.h>

// User includes
#include "apcontrollistplugin.hrh"
#include "apcontrollistplugin.h"
#include "apcontrollistplugincontainer.h"
#include "apcontrollistpluginlogger.h"
#include "apcontrollistapiwrapper.h"
#include "apcontrollistcapschange.h"

// CONSTANTS
// Resource file location constant
_LIT( KResourceFileName, "APControlListPluginRsc.rsc" );

// ROM folder
_LIT( KDriveZ, "z:" );

// Name of the MBM file containing icons
_LIT( KFileIcons, "apcontrollistplugin.mbm");

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CAPControlListPlugin::NewL
// First phase constructor
// ---------------------------------------------------------
CAPControlListPlugin* CAPControlListPlugin::NewL( TAny* /*aInitParams*/ )
    {
	CLOG_CREATE()
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::NewL" ) ) );
    CAPControlListPlugin* self = new ( ELeave ) CAPControlListPlugin();
    CleanupStack::PushL( self );
    
    self->ConstructL( );
                      
    CleanupStack::Pop( self );
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::NewL" ) ) );
    return self;     
    }

// ---------------------------------------------------------
// CAPControlListPlugin::NewL
// Destructor
// ---------------------------------------------------------
CAPControlListPlugin::~CAPControlListPlugin()
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::~CAPControlListPlugin" ) ) );
    if( iContainer )
    {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
    }
    if (iAPIWrapper)
    {
    	delete iAPIWrapper;
    }
    if (iChangeNotifier)
    {
    	delete iChangeNotifier;
    }
    iResources.Close();
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::~CAPControlListPlugin" ) ) );
    }

// ---------------------------------------------------------
// CAPControlListPlugin::Id
// ---------------------------------------------------------
TUid CAPControlListPlugin::Id() const
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::Id" ) ) );
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::Id" ) ) );
    return KAPControlListPluginUid;
    }
    
// ---------------------------------------------------------
// CAPControlListPlugin::HandleCommandL( TInt aCommand )
// ---------------------------------------------------------
//
void CAPControlListPlugin::HandleCommandL( TInt aCommand )
    {   
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::HandleCommandL" ) ) );
    switch ( aCommand )
        {
        case EAknSoftkeyBack :
            {
            // Activate the view ( from GS ) that we
            // are supposed to return to
            AppUi()->ActivateLocalViewL( iPrevViewId.iViewUid );
            break;
            }
        case EAPControlListCmdActivate :
            {
            iAPIWrapper->ActivateACL();
            break;
            }
        case EAPControlListCmdDeactivate :
            {
            iAPIWrapper->DeActivateACL();
            break;
            }
        case EAPControlListCmdAdd :
            {
            iAPIWrapper->AddAPNL();
            break;
            }
        case EAPControlListCmdRemove :
            {
            iAPIWrapper->RemoveAPN();
            break;
            }
        case EAknCmdHelp : 
            {
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), 
                                                 AppUi()->AppHelpContextL() );
            break;
            }
        case EEikCmdExit :
        default :
            {            
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::HandleCommandL" ) ) );
    }
    
// ---------------------------------------------------------
// CAPControlListPlugin::ExitOnUSIMRemovalL
// ---------------------------------------------------------
void CAPControlListPlugin::ExitOnUSIMRemovalL() 
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::ExitOnUSIMRemovalL" ) ) );
    iVisible = EFalse;
	AppUi()->ActivateLocalViewL( iPrevViewId.iViewUid );
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::ExitOnUSIMRemovalL" ) ) );
    }
    
// ---------------------------------------------------------
// CAPControlListPlugin::DoActivateL
// ---------------------------------------------------------
void CAPControlListPlugin::DoActivateL( const TVwsViewId& aPrevViewId, 
                                      TUid /*aCustomMessageId*/, 
                                      const TDesC8& /*aCustomMessage*/ )
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::DoActivateL" ) ) );
    if ( iPrevViewId.iViewUid.iUid == 0 )
        {
        //must be prepared to application switch, so it stores
        //only the parent view (General Settings Main View)
        iPrevViewId = aPrevViewId;
        }

    if ( !iContainer )
        {
        iContainer = new( ELeave ) CAPControlListPluginContainer;
        iContainer->SetMopParent( this );
        
        TRAPD( error, iContainer->ConstructL( ClientRect(), this ) );
        if ( error )
            {
            delete iContainer;
            iContainer = NULL;
            User::Leave( error );
            }
            
        AppUi()->AddToViewStackL( *this, iContainer );
        iContainer->HandleResourceChange(KEikDynamicLayoutVariantSwitch);
        
        if (!iAPIWrapper)  
            {
            iAPIWrapper = new( ELeave ) CAPControlListAPIWrapper(*this);
            iAPIWrapper->ConstructL();
            }
        if (!iChangeNotifier)  
            {
            iChangeNotifier = new( ELeave ) CAPControlListCapsChange(*this);
            iChangeNotifier->NotifyCapsChange();
            }
	    				
        ShowInfoNoteL(R_QTN_ACL_WAIT_READING_SIM);
        iAPIWrapper->ReadData();  
        iContainer->HandleResourceChange(KEikDynamicLayoutVariantSwitch);
        }
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::DoActivateL" ) ) );
    }

// ---------------------------------------------------------
// CAPControlListPlugin::PluginUid
// ---------------------------------------------------------
TUid CAPControlListPlugin::PluginUid() const
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::PluginUid" ) ) );
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::PluginUid" ) ) );
    return KAPControlListPluginUid;
    }

// ---------------------------------------------------------
// CAPControlListPlugin::GetCaption
// ---------------------------------------------------------
void CAPControlListPlugin::GetCaptionL( TDes& aCaption ) const
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::GetCaptionL" ) ) );
    StringLoader::Load( aCaption, R_QTN_ACL_MENU_ITEM );
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::GetCaptionL" ) ) );
    }

// ---------------------------------------------------------
// CAPControlListPlugin::HasBitmap
// ---------------------------------------------------------
TBool CAPControlListPlugin::HasBitmap() const
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::HasBitmap" ) ) );
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::HasBitmap" ) ) );
    return EFalse;
    }

// ---------------------------------------------------------
// CAPControlListPlugin::GetBitmapL
// ---------------------------------------------------------
void CAPControlListPlugin::GetBitmapL( CFbsBitmap* /*aBitmap*/, 
                                     CFbsBitmap* /*aMask*/ ) const
    {
    //no bitmap
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::GetBitmapL" ) ) );
    User::Leave( KErrNotFound );
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::GetBitmapL" ) ) );
    }
    
// ---------------------------------------------------------
// CAPControlListPlugin::PluginProviderCategory
// ---------------------------------------------------------
TInt CAPControlListPlugin::PluginProviderCategory() const
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::PluginProviderCategory" ) ) );
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::PluginProviderCategory" ) ) );
    return EGSPluginProviderOEM;
    }

// ---------------------------------------------------------
// CApSettingsPlugin::CreateIconL
// ---------------------------------------------------------    
CGulIcon* CAPControlListPlugin::CreateIconL( const TUid aIconType )
    {
    TFileName iconsFileName;

    iconsFileName.Append( KDriveZ );
    iconsFileName.Append( KDC_APP_BITMAP_DIR );
    iconsFileName.Append( KFileIcons );
    
	CGulIcon* icon;
       
    if( aIconType == KGSIconTypeLbxItem )
        {
        icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(), 
        KAknsIIDQgnPropSetConnAccp, 
        iconsFileName,
        EMbmApcontrollistpluginQgn_prop_set_conn_acl,
        EMbmApcontrollistpluginQgn_prop_set_conn_acl_mask );
        }
     else
        {
        icon = CGSPluginInterface::CreateIconL( aIconType );
        }
	
	return icon;
    }    
    
// ---------------------------------------------------------
// CAPControlListPlugin::Visible
// ---------------------------------------------------------
TBool CAPControlListPlugin::Visible() const
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::Visible" ) ) );
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::Visible" ) ) );
    return iVisible;
    }
    
// ---------------------------------------------------------
// CAPControlListPlugin::CAPControlListPlugin
// ---------------------------------------------------------
CAPControlListPlugin::CAPControlListPlugin():
    iPrevViewId(),
    iResources( *CCoeEnv::Static() ),
    iVisible(EFalse)
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::CAPControlListPlugin" ) ) );
    // no implementation required
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::CAPControlListPlugin" ) ) );
    }

// ---------------------------------------------------------
// CAPControlListPlugin::ConstructL
// ---------------------------------------------------------
TAny CAPControlListPlugin::ConstructL()
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::ConstructL" ) ) );
   
    TFileName fileName;
    TDriveUnit drive( EDriveZ );
    
    fileName.Append( drive.Name() );
    fileName.Append( KDC_RESOURCE_FILES_DIR );
    fileName.Append( KResourceFileName );    
    iResources.OpenL( fileName );

    BaseConstructL( R_APCLPLUGIN_MAIN_VIEW );
     
	// get Visible   
    iAPIWrapper = new( ELeave ) CAPControlListAPIWrapper(*this);
    iAPIWrapper->ConstructL();
    iVisible = iAPIWrapper->Visible();
    iAPIWrapper->Cancel();
    delete iAPIWrapper;
    iAPIWrapper = NULL;
        
    FeatureManager::InitializeLibL();
    iHelp = FeatureManager::FeatureSupported( KFeatureIdHelp ); 
    FeatureManager::UnInitializeLib();
    
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::ConstructL" ) ) );
    }
    
    
// ---------------------------------------------------------
// CAPControlListPlugin::DynInitMenuPaneL
// ---------------------------------------------------------
void CAPControlListPlugin::DynInitMenuPaneL( TInt aResourceId,
                                         CEikMenuPane* aMenuPane )
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::DynInitMenuPaneL" ) ) );
    CAknView::DynInitMenuPaneL( aResourceId, aMenuPane );
    if ( aResourceId == R_APCLPLUGIN_MAIN_MENU )
    {    
		if ( iAPIWrapper->Enabled() )
		{
    		aMenuPane->DeleteMenuItem( EAPControlListCmdActivate );
		}
		else
		{
    		aMenuPane->DeleteMenuItem( EAPControlListCmdDeactivate );
		}
		if ( !iContainer->NumberOfItems() )
		{
    		aMenuPane->DeleteMenuItem( EAPControlListCmdRemove );
		}
		if (!iHelp)
		{
    		aMenuPane->DeleteMenuItem( EAknCmdHelp );		    
		}
    }
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::DynInitMenuPaneL" ) ) );
    }
        
// ---------------------------------------------------------
// CAPControlListPlugin::DoDeactivate
// ---------------------------------------------------------
void CAPControlListPlugin::DoDeactivate()
    {        
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::DoDeactivate" ) ) );
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
    iContainer = NULL; 
    if (iChangeNotifier)
    {
    	delete iChangeNotifier;
		iChangeNotifier = NULL; 
    }
    if (iAPIWrapper)  
	    {
	    delete iAPIWrapper;
	    iAPIWrapper = NULL;    	
	    }
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::DoDeactivate" ) ) );
    }
    
// ---------------------------------------------------------
// CAPControlListPlugin::UiComplete( TInt aUirEvent )
// ---------------------------------------------------------
//
void CAPControlListPlugin::UiComplete( TInt /*aUirEvent*/ )
    {    
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::UiComplete" ) ) );
    // Activate this view as we are coming back
    // from some other view in the UI
    TRAP_IGNORE( 
        AppUi()->ActivateLocalViewL( KAPControlListPluginUid ) )
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::UiComplete" ) ) );
    }
    
// ---------------------------------------------------------
//  CAPControlListPlugin::HandleListBoxEventL
// ---------------------------------------------------------
void CAPControlListPlugin::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                              TListBoxEvent aEventType )
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::HandleListBoxEventL" ) ) );
    switch ( aEventType )
        {
        case EEventEnterKeyPressed :
        case EEventItemClicked :
            {
            break;
            }
        default :
           break;
        }
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::HandleListBoxEventL" ) ) );
    }
    
// ----------------------------------------------------------
// CAPControlListPlugin::ShowInfoNoteL( TInt aResourceId )
// ----------------------------------------------------------
//
void CAPControlListPlugin::ShowInfoNoteL( TInt aResourceId )
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::ShowInfoNoteL" ) ) );
    HBufC* text = iCoeEnv->AllocReadResourceLC( aResourceId );
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::ShowInfoNoteL" ) ) );
    }
    
// ----------------------------------------------------------
// CAPControlListPlugin::ShowConfirmNoteL( TInt aResourceId )
// ----------------------------------------------------------
//

void CAPControlListPlugin::ShowConfirmNoteL( TInt aResourceId )
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::ShowConfirmNoteL" ) ) );
    HBufC* text = iCoeEnv->AllocReadResourceLC( aResourceId );
    CAknConfirmationNote* note = new ( ELeave ) CAknConfirmationNote( ETrue );
    note->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::ShowConfirmNoteL" ) ) );
    }
    
// ----------------------------------------------------------
// CAPControlListPlugin::ShowConfirmNoteL( TInt aResourceId )
// ----------------------------------------------------------
//
void CAPControlListPlugin::ShowConfirmNoteAPNL( TInt aResourceId, TPtrC& aAPN )
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPlugin::ShowConfirmNoteL" ) ) );
    HBufC* text = StringLoader::LoadLC ( aResourceId, aAPN);
    CAknConfirmationNote* note = new ( ELeave ) CAknConfirmationNote( ETrue );
    note->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPlugin::ShowConfirmNoteL" ) ) );
    }
    
// ----------------------------------------------------------
// CAPControlListPlugin::Container()
// ----------------------------------------------------------
//
CAPControlListPluginContainer* CAPControlListPlugin::Container()
    {
    return iContainer;
    }
    
// ----------------------------------------------------------
// CAPControlListPlugin::APIWrapper()
// ----------------------------------------------------------
//
CAPControlListAPIWrapper* CAPControlListPlugin::APIWrapper()
    {
    	return iAPIWrapper;
    }
//End of file
