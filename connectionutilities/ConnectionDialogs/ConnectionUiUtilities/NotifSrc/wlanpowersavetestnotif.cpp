/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Notier part for WLAN Power Save Testing notification dialog
*
*/



// INCLUDE FILES
#include "wlanpowersavetestnotif.h"
#include "wlanpowersavetest.h"
#include "ConnectionUiUtilitiesCommon.h"
#include "ConnUiUtilsNotif.h"
#include "ConnectionDialogsLogger.h"

#include <ConnUiUtilsNotif.rsg>
#include <StringLoader.h>

#include <ConnectionUiUtilities.h>



// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CWlanPowerSaveQueryNotif::CWlanPowerSaveQueryNotif()
// ---------------------------------------------------------
//
CWlanPowerSaveQueryNotif::CWlanPowerSaveQueryNotif()
    :
    iDisable( EFalse )
    {
    }


// ---------------------------------------------------------
// CWlanPowerSaveQueryNotif* CWlanPowerSaveQueryNotif::NewL()
// ---------------------------------------------------------
//
CWlanPowerSaveQueryNotif* CWlanPowerSaveQueryNotif::NewL( 
                                        const TBool aResourceFileResponsible )
    {
    CWlanPowerSaveQueryNotif* self = new ( ELeave ) CWlanPowerSaveQueryNotif();
    CleanupStack::PushL( self );
    self->ConstructL( KResourceFileName, aResourceFileResponsible );
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------
// CWlanPowerSaveQueryNotif::~CWlanPowerSaveQueryNotif()
// ---------------------------------------------------------
//
CWlanPowerSaveQueryNotif::~CWlanPowerSaveQueryNotif()
    {   
    if ( iExitAsync )
        {
    	delete iExitAsync;
    	iExitAsync = NULL;
        }
    }

// ---------------------------------------------------------
// CWlanPowerSaveQueryNotif::RegisterL()
// ---------------------------------------------------------
//
CWlanPowerSaveQueryNotif::TNotifierInfo CWlanPowerSaveQueryNotif::RegisterL()
    {
    iInfo.iUid      = KUidWlanPowerSaveTestNote;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel  = KUidWlanPowerSaveTestNote;

    return iInfo;
    }

// ---------------------------------------------------------
// void CWlanPowerSaveQueryNotif::StartL
// ---------------------------------------------------------
//
void CWlanPowerSaveQueryNotif::StartL( const TDesC8& /*aBuffer*/, 
                                       TInt aReplySlot,
                                       const RMessagePtr2& aMessage )
    {
    if ( iDialog )
        {
        // Dialog is already on the display
        aMessage.Complete( KErrServerBusy );
        return;
        }
            
    iReplySlot = aReplySlot;
    iMessage   = aMessage;
    iCancelled = EFalse;    

    iDialog = new ( ELeave ) CWlanPowerSaveQuery( this );
    iDialog->PrepareLC( R_POWERSAVE_QUERY );
    
    HBufC* heading  = NULL;
    HBufC* message  = NULL;
    TInt resourceId = 0;
    
    resourceId = R_QTN_WLAN_POWER_SAVE_TEST_DLG_TEXT;
    heading    = StringLoader::LoadLC( 
                     R_QTN_WLAN_POWER_SAVE_TEST_DLG_TITLE );
    
    CDesCArrayFlat* strings = new( ELeave ) CDesCArrayFlat( 2 );
    CleanupStack::PushL( strings );
    
    HBufC* messageBase = StringLoader::LoadL( resourceId, *strings );

    CleanupStack::PopAndDestroy( strings );

    CleanupStack::PushL( messageBase );

    _LIT( KLinkStartTag, "\n<AknMessageQuery Link>" );
    _LIT( KLinkEndTag, "</AknMessageQuery Link>" );

    HBufC* linkString1 = StringLoader::LoadLC( 
                            R_QTN_WLAN_POWER_SAVE_TEST_DLG_LINK );

    TInt lenMsg = messageBase->Des().Length()+
                  linkString1->Des().Length()+
                  KLinkStartTag().Length()+
                  KLinkEndTag().Length();


    message = HBufC::NewL( lenMsg );
    TPtr messagePtr = message->Des();

    messagePtr.Append( messageBase->Des() ); 

    messagePtr.Append( KLinkStartTag ); 
    messagePtr.Append( linkString1->Des() );
    messagePtr.Append( KLinkEndTag );

    CleanupStack::PopAndDestroy( linkString1 );
    CleanupStack::PopAndDestroy( messageBase );

    CleanupStack::PushL( message );

    iDialog->SetMessageTextL( *message );
    CleanupStack::PopAndDestroy( message );

    iDialog->QueryHeading()->SetTextL( *heading );
    CleanupStack::PopAndDestroy( heading );

    // Set callback for the link
    TCallBack callBackFirstLink( DisableTestLinkL, this );
    iDialog->SetLink( callBackFirstLink );

    iDialog->RunLD();
    }

// ---------------------------------------------------------
// void CWlanPowerSaveQueryNotif::Cancel()
// ---------------------------------------------------------
//
void CWlanPowerSaveQueryNotif::Cancel()
    {
    CLOG_ENTERFN( "CWlanPowerSaveQueryNotif::Cancel" );
    if ( !iCancelled )
        {
        iCancelled = ETrue;
        if ( !iMessage.IsNull() )
            {
            iMessage.Complete( KErrCancel );
            }
        
        delete iDialog;
        iDialog = NULL;
        }
            
    CLOG_LEAVEFN( "CWlanPowerSaveQueryNotif::Cancel" );
    }


// ---------------------------------------------------------
// void CWlanPowerSaveQueryNotif::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CWlanPowerSaveQueryNotif::CompleteL( TInt aStatus )
    {  
    CLOG_ENTERFN( "CWlanPowerSaveQueryNotif::CompleteL" );
    if ( !iMessage.IsNull() )
        {        
        iMessage.WriteL( iReplySlot, TPckg< TBool >( iDisable ) );
        iMessage.Complete( aStatus );
        }
    
    iCancelled = ETrue;
    iDialog    = NULL; //deleted by AVKON when OkToExitL completes
    
    CLOG_LEAVEFN( "CWlanPowerSaveQueryNotif::CompleteL" );
    }


// ---------------------------------------------------------
// TInt CWlanPowerSaveQueryNotif::DisableTestLinkL()
// ---------------------------------------------------------
//
TInt CWlanPowerSaveQueryNotif::DisableTestLinkL( TAny* aObject )
    {
    CLOG_ENTERFN( "CWlanPowerSaveQueryNotif::DisableTestLinkL" );
    CWlanPowerSaveQueryNotif* myself =
                            static_cast<CWlanPowerSaveQueryNotif*>( aObject );
    
    if ( !myself->iExitAsync )
        {
        // Can't exit now. This call could be triggered by a pointer
        // event. AVKON would crash if exit is done now.
        // Trigger a one shot active object to delay exit. 
        TCallBack cb( myself->DisableTestLinkL, myself );
	    myself->iExitAsync = new( ELeave ) CAsyncCallBack( cb, CActive::EPriorityHigh );
	    myself->iExitAsync->CallBack();
	    
	    CLOG_LEAVEFN( "CWlanPowerSaveQueryNotif::DisableTestLinkL" );
	    return 1;
        }
    else
        {
        // We can exit now. Called by one shot active object.
        myself->DoDisableTestLinkL();
        
        CLOG_LEAVEFN( "CWlanPowerSaveQueryNotif::DisableTestLinkL" );
        return 0;	
        }
    }


// ---------------------------------------------------------
// void CWlanPowerSaveQueryNotif::DoDisableTestLinkL()
// ---------------------------------------------------------
//
void CWlanPowerSaveQueryNotif::DoDisableTestLinkL()
    {
    CLOG_ENTERFN( "CWlanPowerSaveQueryNotif::DoDisableTestLinkL" );

    iDisable = ETrue;
    
    iDialog->TryExitL( EAknSoftkeySelect );
    
    CLOG_LEAVEFN( "CWlanPowerSaveQueryNotif::DoDisableTestLinkL" );
    }

// End of File
