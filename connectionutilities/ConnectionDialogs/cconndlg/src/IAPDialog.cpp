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
* Description:  Implementation of class CIapDialog.
*
*/


// INCLUDE FILES
#include "IAPDialog.h"
#include "ConnectionInfo.h"
#include "ConnectionInfoArray.h"
#include "ConnDlgPlugin.h"
#include "ConnectionDialogsLogger.h"
#include "ExpiryTimer.h"

#include <AknIconArray.h>
#include <AknsUtils.h>
#include <uikon/eiksrvui.h>

#include <data_caging_path_literals.hrh>
#include <apsettings.mbg>


// CONST DECLARATIONS

// ROM folder
_LIT( KDriveZ, "z:" );


// Name of the MBM file containing icons
_LIT( KApsettingsMBMFile, "ApSettings.mbm" );

#if defined(_DEBUG)
_LIT( KErrNullPointer, "NULL pointer" );
#endif

// CONSTANTS

LOCAL_D const TInt KIconsGranularity = 4;


// ================= MEMBER FUNCTIONS =======================
//
// ---------------------------------------------------------
// CIapDialog::CIapDialog
// ---------------------------------------------------------
//
CIapDialog::CIapDialog( CConnDlgIapPlugin* aPlugin )
: CAknListQueryDialog( &iDummy ),
  iPlugin( aPlugin ),
  iFromOkToExit( EFalse )
    {
    }


// ---------------------------------------------------------
// CIapDialog::NewL
// ---------------------------------------------------------
//
CIapDialog* CIapDialog::NewL( CConnDlgIapPlugin* aPlugin )
    {
    CIapDialog* self = new( ELeave ) CIapDialog( aPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// ---------------------------------------------------------
// CIapDialog::ConstructL()
// ---------------------------------------------------------
//
void CIapDialog::ConstructL()
    {
    }


// ---------------------------------------------------------
// CIapDialog::~CIapDialog()
// ---------------------------------------------------------
//
CIapDialog::~CIapDialog()
    {
    CLOG_ENTERFN( "CIapDialog::~CIapDialog " );
    iIAPIds.Close();
    delete iExpiryTimer;
    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( EFalse );
    CLOG_LEAVEFN( "CIapDialog::~CIapDialog " );
    }


// ---------------------------------------------------------
// CIapDialog::PreLayoutDynInitL()
// ---------------------------------------------------------
//
void CIapDialog::PreLayoutDynInitL()
    {
    CLOG_ENTERFN( "CIapDialog::PreLayoutDynInitL " );  
    
    CAknListQueryDialog::PreLayoutDynInitL();

    SetOwnershipType( ELbmDoesNotOwnItemArray );
    SetIconsL();

    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( ETrue );

    iExpiryTimer = CExpiryTimer::NewL( *this );
    iExpiryTimer->Start();
    
    CLOG_LEAVEFN( "CIapDialog::PreLayoutDynInitL " );      
    }


// ---------------------------------------------------------
// CIapDialog::SetIconsL()
// ---------------------------------------------------------
//
void CIapDialog::SetIconsL()
    {
    CLOG_ENTERFN( "CIapDialog::SetIconsL " );  
    
    CArrayPtr< CGulIcon >* icons = new( ELeave ) CAknIconArray( 
                                                        KIconsGranularity );
    CleanupStack::PushL( icons );

    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();

    TFileName iconsFileName;

    iconsFileName.Append( KDriveZ );
    iconsFileName.Append( KDC_APP_BITMAP_DIR );
    iconsFileName.Append( KApsettingsMBMFile );

    icons->AppendL( AknsUtils::CreateGulIconL( skinInstance, 
                            KAknsIIDQgnPropWmlGprs,
                            iconsFileName, 
                            EMbmApsettingsQgn_prop_wml_gprs, 
                            EMbmApsettingsQgn_prop_wml_gprs_mask ) );

    icons->AppendL( AknsUtils::CreateGulIconL( skinInstance, 
                            KAknsIIDQgnPropWmlCsd,
                            iconsFileName, 
                            EMbmApsettingsQgn_prop_wml_csd, 
                            EMbmApsettingsQgn_prop_wml_csd_mask ) );

    icons->AppendL( AknsUtils::CreateGulIconL( skinInstance, 
                            KAknsIIDQgnPropWmlHscsd,
                            iconsFileName, 
                            EMbmApsettingsQgn_prop_wml_hscsd, 
                            EMbmApsettingsQgn_prop_wml_hscsd_mask ) );

    icons->AppendL( AknsUtils::CreateGulIconL( skinInstance, 
                        KAknsIIDQgnPropWlanEasy,
                        iconsFileName, 
                        EMbmApsettingsQgn_prop_wlan_easy, 
                        EMbmApsettingsQgn_prop_wlan_easy_mask ) );

    icons->AppendL( AknsUtils::CreateGulIconL( skinInstance, 
                        KAknsIIDQgnPropWlanBearer,
                        iconsFileName, 
                        EMbmApsettingsQgn_prop_wlan_bearer, 
                        EMbmApsettingsQgn_prop_wlan_bearer_mask ) );
    
    SetIconArrayL( icons );

    CleanupStack::Pop( icons );
    CLOG_LEAVEFN( "CIapDialog::SetIconsL " );      
    }


// ---------------------------------------------------------
// CIapDialog::OkToExitL
// ---------------------------------------------------------
//
TBool CIapDialog::OkToExitL( TInt aButtonId )
    {
    CLOG_ENTERFN( "CIapDialog::OkToExitL " );      
    
    TBool result( EFalse );
    TInt completeCode( KErrNone );
    
    if ( aButtonId == EAknSoftkeySelect || aButtonId == EAknSoftkeyOk ||
         aButtonId == EAknSoftkeyDone )
        {
        iPlugin->SetPreferredIapIdL( iIAPIds[ListBox()->CurrentItemIndex()] );
        result = ETrue;
        }
    else if ( aButtonId == EAknSoftkeyCancel )
        {
        completeCode = KErrCancel;
        result = ETrue;
        }
        
    if ( result )
        {
        iFromOkToExit = ETrue;        
        
        __ASSERT_DEBUG( iPlugin, User::Panic( KErrNullPointer, KErrNone ) );
        iPlugin->CompleteL( completeCode );
        }

    CLOG_LEAVEFN( "CIapDialog::OkToExitL " );      

    return result;
    }


// ---------------------------------------------------------
// CIapDialog::RefreshDialogL
// ---------------------------------------------------------
//    
void CIapDialog::RefreshDialogL( CConnectionInfoArray* aIAP, 
                                 TBool aIsReallyRefreshing )
    {  
    CLOG_ENTERFN( "CIapDialog::RefreshDialogL " );  
     
    SetItemTextArray( aIAP );

    iIAPIds.Reset();
    
    TInt numAPs = aIAP->Count();
    for( TInt ii = 0; ii < numAPs; ii++ )
        {
        TUint32 id = aIAP->At( ii )->Id();
        iIAPIds.AppendL( id );        
        }
    
    ListBox()->SetCurrentItemIndex( 0 );

    ListBox()->HandleItemAdditionL();

    if ( aIsReallyRefreshing )
        {   // If we are really making a refresh, we need to recalculate the 
            // layout, because the number of items could be different and the
            // dialog should be resized accordingly
        Layout();   
        SizeChanged();
        DrawNow();
        }

    CLOG_LEAVEFN( "CIapDialog::RefreshDialogL " );  
    }

// ---------------------------------------------------------
// CIapDialog::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CIapDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                         TEventCode aType)
    {
    if( aType == EEventKey && aKeyEvent.iCode == EKeyPhoneSend )
        {
        // Let's not obscure the Dialer in the background
        if ( iExpiryTimer )
            {
            iExpiryTimer->Cancel();
            iExpiryTimer->StartShort();    
            }
        }
        
    return CAknListQueryDialog::OfferKeyEventL( aKeyEvent,aType ); 
    }

void CIapDialog::HandleTimedOut()
    {
    TRAP_IGNORE( TryExitL(EAknSoftkeyCancel) );
    }



// End of File
