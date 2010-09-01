/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ConnectionMonitorPlugin implementation.
*
*/


// User includes
#include    "connectionmonitorplugin.h"
#include    <gsparentplugin.h>
#include    <gscommon.hrh>
#include    <connectionmonitorpluginrsc.rsg> // GUI Resource
#include    <connectionmonitorplugin.mbg>
#include    <gsprivatepluginproviderids.h>
#include    <gsfwviewuids.h>

// System includes
#include    <AknNullService.h>
#include    <bautils.h>
#include    <StringLoader.h>
#include    <AknGlobalNote.h>
#include    <ErrorUI.h>

// Constants

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CConnectionMonitorPlugin::CConnectionMonitorPlugin()
// Constructor
// ---------------------------------------------------------------------------
//
CConnectionMonitorPlugin::CConnectionMonitorPlugin()
    : iResources( *iCoeEnv ),
      iNullService(NULL)
    {
    }


// ---------------------------------------------------------------------------
// CConnectionMonitorPlugin::~CConnectionMonitorPlugin()
// Destructor
// ---------------------------------------------------------------------------
//
CConnectionMonitorPlugin::~CConnectionMonitorPlugin()
    {
    iResources.Close();

    if ( iNullService )
        {
        delete iNullService;
        }
    }


// ---------------------------------------------------------------------------
// CConnectionMonitorPlugin::ConstructL()
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
void CConnectionMonitorPlugin::ConstructL()
    {
    BaseConstructL();
    
    OpenLocalizedResourceFileL( KConnectionMonitorPluginResourceFileName,
                                iResources );
    }


// ---------------------------------------------------------------------------
// CConnectionMonitorPlugin::NewL()
// Static constructor
// ---------------------------------------------------------------------------
//
CConnectionMonitorPlugin* CConnectionMonitorPlugin::NewL( 
                                                    TAny* /*aInitParams*/ )
    {
    CConnectionMonitorPlugin* self = 
                                new( ELeave ) CConnectionMonitorPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CConnectionMonitorPlugin::Id
// -----------------------------------------------------------------------------
//
TUid CConnectionMonitorPlugin::Id() const
    {
    return KConnectionMonitorPluginUID;
    }


// -----------------------------------------------------------------------------
// CConnectionMonitorPlugin::DoActivateL
// -----------------------------------------------------------------------------
//
void CConnectionMonitorPlugin::DoActivateL( 
                                  const TVwsViewId& /*aPrevViewId*/,
                                  TUid /*aCustomMessageId*/,
                                  const TDesC8& /*aCustomMessage*/ )
    {
    }


// -----------------------------------------------------------------------------
// CConnectionMonitorPlugin::DoDeactivate
// -----------------------------------------------------------------------------
//
void CConnectionMonitorPlugin::DoDeactivate()
    {
    }


// ========================= From CGSPluginInterface ==================


// -----------------------------------------------------------------------------
// CConnectionMonitorPlugin::GetCaptionL
// -----------------------------------------------------------------------------
//
void CConnectionMonitorPlugin::GetCaptionL( TDes& aCaption ) const
    {
    HBufC* result = StringLoader::LoadL( 
                                    R_QTN_CP_FOLDER_CONNECTION_MANAGEMENT );
    aCaption.Copy( *result );
    delete result;
    }


// -----------------------------------------------------------------------------
// CConnectionMonitorPlugin::PluginProviderCategory
// -----------------------------------------------------------------------------
//
TInt CConnectionMonitorPlugin::PluginProviderCategory() const
    {
    return KGSPluginProviderInternal;
    }


// -----------------------------------------------------------------------------
// CConnectionMonitorPlugin::ItemType()
// -----------------------------------------------------------------------------
//
TGSListboxItemTypes CConnectionMonitorPlugin::ItemType()
    {
    return EGSItemTypeSettingDialog;
    }


// -----------------------------------------------------------------------------
// CConnectionMonitorPlugin::GetValue()
// -----------------------------------------------------------------------------
//
void CConnectionMonitorPlugin::GetValue( const TGSPluginValueKeys /*aKey*/,
                                      TDes& /*aValue*/ )
    {
    }


// -----------------------------------------------------------------------------
// CConnectionMonitorPlugin::HandleSelection()
// -----------------------------------------------------------------------------
//
void CConnectionMonitorPlugin::HandleSelection(
    const TGSSelectionTypes /*aSelectionType*/ )
    {
    TRAPD( error, LaunchConnectionMonitorAppL() );
    if( error != KErrNone )
        {
        ShowErrorNote( error );
        }
    }

// ---------------------------------------------------------------------------
// CConnectionMonitorPlugin::CreateIconL
// ---------------------------------------------------------------------------
//
CGulIcon* CConnectionMonitorPlugin::CreateIconL( const TUid aIconType )
    {
    //EMbm<Mbm_file_name><Bitmap_name>
    CGulIcon* icon;
    TParse* fp = new( ELeave ) TParse();
    CleanupStack::PushL( fp );
    fp->Set( KConnectionMonitorPluginIconDirAndName, &KDC_BITMAP_DIR, NULL );

    if ( aIconType == KGSIconTypeLbxItem )
        {
        icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(),
        KAknsIIDQgnPropCpConnMan,
        fp->FullName(),
        EMbmConnectionmonitorpluginQgn_prop_cp_conn_man,
        EMbmConnectionmonitorpluginQgn_prop_cp_conn_man_mask );
        }    
    else
        {
        icon = CGSPluginInterface::CreateIconL( aIconType );
        }

    CleanupStack::PopAndDestroy( fp );

    return icon;
    }


// -----------------------------------------------------------------------------
// CConnectionMonitorPlugin::OpenLocalizedResourceFileL()
// -----------------------------------------------------------------------------
//
void CConnectionMonitorPlugin::OpenLocalizedResourceFileL(
    const TDesC& aResourceFileName,
    RConeResourceLoader& aResourceLoader )
    {
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() );

    // Find the resource file:
    TParse parse;
    parse.Set( aResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
    TFileName fileName( parse.FullName() );

    // Get language of resource file:
    BaflUtils::NearestLanguageFile( fsSession, fileName );

    // Open resource file:
    aResourceLoader.OpenL( fileName );

    // If leave occurs before this, close is called automatically when the
    // thread exits.
    fsSession.Close();
    }


// -----------------------------------------------------------------------------
// CConnectionMonitorPlugin::LaunchConnectionMonitorAppL()
// -----------------------------------------------------------------------------
//
void CConnectionMonitorPlugin::LaunchConnectionMonitorAppL()
    {
    // Launch app
    if ( iNullService )
        {
        delete iNullService;
        iNullService = NULL;
        }
    iNullService = CAknNullService::NewL( KConnectionMonitorAppUid, this );
    }

// ---------------------------------------------------------
// CWlanPlugin::ShowErrorNote
// ---------------------------------------------------------
//
void CConnectionMonitorPlugin::ShowErrorNote(TInt aErrorCode)
    {
    TBool errorNoteShowed = EFalse;
        
    TRAPD(error, errorNoteShowed = ShowErrorNoteL(aErrorCode));
            
    if(error || !errorNoteShowed) 
        {
        TRAP_IGNORE( ShowGeneralErrorNoteL() );
        }    
    }

// ---------------------------------------------------------
// CWlanPlugin::ShowErrorNoteL
// ---------------------------------------------------------
//
TBool CConnectionMonitorPlugin::ShowErrorNoteL(TInt aErrorCode)
    {
    CErrorUI* errorUi = CErrorUI::NewLC();
    
    TBool errorNoteShowed = errorUi->ShowGlobalErrorNoteL( aErrorCode );
    
    CleanupStack::PopAndDestroy( errorUi );
        
    return errorNoteShowed;
    }    

// ---------------------------------------------------------
// CWlanPlugin::ShowGeneralErrorNoteL
// ---------------------------------------------------------
//
void CConnectionMonitorPlugin::ShowGeneralErrorNoteL()
    {
    HBufC *msg = HBufC::NewL( R_QTN_ERR_OS_GENERAL );
    
	msg->Des().Copy( _L( "" ) );
    CAknGlobalNote *note = CAknGlobalNote::NewLC();
    
    
    note->ShowNoteL( EAknGlobalErrorNote, *msg );


    CleanupStack::PopAndDestroy( note );
    CleanupStack::PopAndDestroy( msg );
    }
   
// End of file
