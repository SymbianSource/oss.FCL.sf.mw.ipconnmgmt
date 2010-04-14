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
* Description:  Wizard used for adding connection methods
*
*/

#include <AknWaitDialog.h>
#include <cmmanager.rsg>
#include <StringLoader.h>
#include <avkon.hrh>
#include "cmwizard.h"
#include <cmcommonconstants.h>
#include <cmcommonui.h>
#include "cmdestinationimpl.h"
#include "cmlogger.h"
#include "cmmanagerimpl.h"
#include <cmpluginbaseeng.h>
#include <cmpluginbase.h>
#include "selectdestinationdlg.h"

using namespace CMManager;

// ---------------------------------------------------------------------------
// CCmWizard::CCmWizard
// ---------------------------------------------------------------------------
//
CCmWizard::CCmWizard( CCmManagerImpl& aCmManager ) 
    : iCmManager( aCmManager )
    {
    CLOG_CREATE;
    }
    
// ---------------------------------------------------------------------------
// CCmWizard::~CCmWizard
// ---------------------------------------------------------------------------
//
CCmWizard::~CCmWizard()
    {
    if( iCheckConnDlg )
        {
        delete iCheckConnDlg;
        }
    CLOG_CLOSE;
    }
    
// ---------------------------------------------------------------------------
// CCmWizard::CreateConnectionMethodL
// ---------------------------------------------------------------------------
//
TInt CCmWizard::CreateConnectionMethodL
                                    ( CCmDestinationImpl* aTargetDestination )
    {
    TInt index( KErrCancel );
    
    LOGGER_ENTERFN( "CCmWizard::CreateConnectionMethodL" );
    TBool wizardCancelled = EFalse; // indicates if the wizard has got cancelled
    TUint32 bearerType = 0;
    TBool manuallyConfigure ( EFalse );
    
    if ( AutoCheckForCmsL() )
        {
        RArray<TUint32> availableBearers( KCmArrayMediumGranularity );
        CleanupClosePushL( availableBearers );

        CLOG_WRITE( "CCmWizard::Starting CheckForCms" );
        CheckForCmsL( availableBearers );

        // If user has cancelled the dialog, then (s)he wanted to terminate
        // the whole procedure.
        if ( iCheckConnDlgCancelled )
            {
            CLOG_WRITE( "CCmWizard::wizardCancelled" );
            wizardCancelled = ETrue;
            }
        else
            {
            // If there's coverage for at least one of the bearer types
            if ( availableBearers.Count() )
                {
                CLOG_WRITE_1( "CCmWizard::CreateConnectionMethodL av. bearers: [%d]",
                               availableBearers.Count() );
                // Show available conn methods in a list from which user has to
                // select.
                bearerType = TCmCommonUi::ShowAvailableMethodsL
                                                ( availableBearers,
                                                  iCmManager );
                if ( !bearerType )
                    {
                    wizardCancelled = ETrue;
                    CLOG_WRITE( "CCmWizard::wizardCancelled" );
                    }
                }
            else    // no coverage at all
                {
                CLOG_WRITE( "CCmWizard::CreateConnectionMethodL no available bearers, manual selection." );
                // "Do you want to configure conn methods manually?"
                if ( ManualCheckForCmsL() )
                    {
                    // Yes, select a bearer manually
                    bearerType = TCmCommonUi::SelectBearerL( iCmManager );
                    if ( !bearerType )
                        {
                        // Cancelled dialog
                        wizardCancelled = ETrue;
                        }
                    else
                        {
                        manuallyConfigure = ETrue;
                        }
                    }
                else
                    {
                    // No, user has cancelled the dialog
                    wizardCancelled = ETrue;
                    }
                }
            }

        CleanupStack::PopAndDestroy( &availableBearers );
        }
    else    // "No" for checking conn methods automatically
        {
        CLOG_WRITE( "CCmWizard::AutoCheckForCmsL returned FALSE" );
        // Manual bearer selection
        bearerType = TCmCommonUi::SelectBearerL( iCmManager );
        if ( !bearerType )
            {
            // Cancelled dialog
            wizardCancelled = ETrue;
            }
        else
            {
            manuallyConfigure = ETrue;
            }
        }

    if ( !wizardCancelled )
        {
        CCmPluginBase* plugin = iCmManager.CreateConnectionMethodL(
                                            bearerType, aTargetDestination );
        CleanupStack::PushL( plugin );

        CLOG_WRITE( "CCmWizard::CreateConnectionMethodL plugin created, starting initialization" );

        // Bearer-specific UI-supported initialization is done by plug-ins
	    TBool ret = EFalse;
	    TRAPD(err, ret = plugin->InitializeWithUiL( manuallyConfigure ) );
        if ( err == KErrNone && ret )
            {
            CCmDestinationImpl* targetDest = aTargetDestination;
            // If target destination is not yet specified...
            if ( !aTargetDestination )
                {
                // For virtual bearers: eliminated the underlying destination 
                // from the destination selection dialog
                TUint32 targetDestId( 0 );
                
                RArray<TUint32> destArray( iCmManager.DestinationCountL() );
                CleanupClosePushL( destArray );
                iCmManager.AllDestinationsL( destArray );
                // now do the filtering
                plugin->FilterPossibleParentsL( destArray );
                
                if ( !destArray.Count() )
                    {
                    // there are no destinations to put the CM into
                    TCmCommonUi::ShowNoteL( R_CMWIZARD_NO_VPN_STACKING,
                                            TCmCommonUi::ECmErrorNote );
        
                    CleanupStack::PopAndDestroy( &destArray );
                    CleanupStack::PopAndDestroy( plugin );
        
                    CLOG_WRITE_1( "CCmWizard::CreateConnectionMethodL no valid parents after filtering, index: [%d]", index );
                    return index;
                    }
                    
                // ... then let the user select a destination.
                CSelectDestinationDlg* dlg = CSelectDestinationDlg::NewL( targetDestId,
                                                                          iCmManager,
                                                                          &destArray );
                dlg->ExecuteLD( R_CMMANAGER_LIST_QUERY_WITH_MSGBOX );
        
                CleanupStack::PopAndDestroy( &destArray );
        
                if ( targetDestId )
                    {
                    // CREATE an object based on an ID.
                    targetDest = iCmManager.DestinationL( targetDestId );
                    CleanupStack::PushL( targetDest );
                    }
                }

            CLOG_WRITE_1( "CCmWizard::CreateConnectionMethodL target dest: [0x%x]",
                           targetDest );
    
            // If the target destination is still unspecified at this point,
            // then the only reason could be that the previous dialog was
            // cancelled ==> do something only if we have the target 
            // destination
            if ( targetDest )
                {
                plugin->UpdateL();  // commit changes
    
                index = iCmManager.CopyConnectionMethodL( *targetDest,
                                                                    *plugin );

                RArray<TUint32> additionalCms;
                CleanupClosePushL (additionalCms);
                plugin->GetAdditionalUids( additionalCms );
                if ( additionalCms.Count() )
                    {//if there are additional cms created, move them to the target destination as well
                    for ( TInt i = 0; i<additionalCms.Count(); i++)
                        {
                        CCmPluginBase* cm = iCmManager.GetConnectionMethodL( additionalCms[i] );
                        CleanupStack::PushL(cm);
                        iCmManager.CopyConnectionMethodL( *targetDest,
                                                                    *cm );
                        CleanupStack::PopAndDestroy( cm );
                        }
                    }
                CleanupStack::PopAndDestroy( &additionalCms );

                HBufC* destName = targetDest->NameLC();
    
                CleanupStack::PopAndDestroy( destName );
    
                // Here's a tricky one: if 'aTargetDestination' was NULL, then
                // I had to create 'targetDest' myself (with
                // iCmManager.DestinationL), consequently I have to
                // free it up, too.
                if ( !aTargetDestination )
                    {
                    CleanupStack::PopAndDestroy( targetDest );
                    }
                }
            else // Wi-Fi may have set IAP in database. So, delete it.
                {
                CLOG_WRITE( "CCmWizard::CreateConnectionMethodL, handling cancel add destination -- start" );                                   
                RArray<TUint32> additionalCms;
                CleanupClosePushL (additionalCms);
    
                plugin->GetAdditionalUids( additionalCms );
                CLOG_WRITE_1( "CCmWizard::CreateConnectionMethodL, addtionalCms count [%d]", additionalCms.Count() );
                for ( TInt i = 0; i<additionalCms.Count(); i++)
                    {
                    CCmPluginBase* cm = iCmManager.GetConnectionMethodL( additionalCms[i] );
                    CleanupStack::PushL(cm);
                    cm->DeleteL( ETrue );
                    CleanupStack::PopAndDestroy( cm );
                    }
                // If plugin hasn't got Iap Id, it's "normal" CM which has not been saved
                // into CommsDat yet. Otherwise it's Wifi CM and has to be deleted by using
                // DeleteL() method. 
                if( plugin->GetIntAttributeL( ECmIapId ) )
                    {
                    plugin->DeleteL( ETrue );
                    }
    
                CleanupStack::PopAndDestroy( &additionalCms );
                CLOG_WRITE( "CCmWizard::CreateConnectionMethodL, handling cancel add destination -- end" );
                }
    		    }
        CleanupStack::PopAndDestroy( plugin );
        }
       
    return index;
    }

// ---------------------------------------------------------------------------
// CCmWizard::DialogDismissedL
// ---------------------------------------------------------------------------
//
void CCmWizard::DialogDismissedL( TInt aButtonId )
    {
    LOGGER_ENTERFN( "CCmWizard::DialogDismissedL" );

    CLOG_WRITE_1( "CCmWizard::DialogDismissedL, aButtonId: [%d]", aButtonId );
    if ( aButtonId == EAknSoftkeyCancel )
        {
        if ( iCheckConnDlg != NULL )
            {
            CLOG_WRITE( "CCmWizard::CheckConnDlg is not NULL" );
            iCheckConnDlgCancelled = ETrue;
            }
        }
    
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    iCheckConnDlg = NULL;
    }

// ---------------------------------------------------------------------------
// CCmWizard::AutoCheckForCmsL
// ---------------------------------------------------------------------------
//
TBool CCmWizard::AutoCheckForCmsL() const
    {
    LOGGER_ENTERFN( "CCmWizard::AutoCheckForCmsL" );
    // Automatically check for available conn methods?
    TInt retval =
            TCmCommonUi::ShowConfirmationQueryL( R_CMWIZARD_AUTOMATIC_CHECK );

    return ( retval == EAknSoftkeyYes || retval == EAknSoftkeyOk )
            ? ETrue : EFalse;
    }

// ---------------------------------------------------------------------------
// CCmWizard::ManualCheckForCmsL
// ---------------------------------------------------------------------------
//
TBool CCmWizard::ManualCheckForCmsL() const
    {
    LOGGER_ENTERFN( "CCmWizard::ManualCheckForCmsL" );
    // Manually configure CMs?
    TInt retval =
            TCmCommonUi::ShowConfirmationQueryL(    
                                             R_CMWIZARD_MANUAL_CONFIGURATION );

    return ( retval == EAknSoftkeyYes || retval == EAknSoftkeyOk )
            ? ETrue : EFalse;
    }

// ---------------------------------------------------------------------------
// CCmWizard::CheckForCmsL
// ---------------------------------------------------------------------------
//
void CCmWizard::CheckForCmsL( RArray<TUint32>& aCoverageArray )
    {
    LOGGER_ENTERFN( "CCmWizard::CheckForCmsL" );
    // Show a wait dialog
    iCheckConnDlgCancelled = FALSE;
    iCheckConnDlg = new (ELeave) CAknWaitDialog(
                          ( REINTERPRET_CAST( CEikDialog**, &iCheckConnDlg )) );
    
    iCheckConnDlg->SetCallback( this );
    
    iCheckConnDlg->SetTone( CAknNoteDialog::EConfirmationTone );

    HBufC* text = StringLoader::LoadLC( R_CMWIZARD_CHECKING_METHODS );
    iCheckConnDlg->SetTextL( *text );
    CleanupStack::PopAndDestroy( text );

    iCheckConnDlg->ExecuteLD( R_CMMANAGER_WAIT_NOTE );

    RArray<TUint32> bearers( KCmArrayMediumGranularity );
    CleanupClosePushL( bearers );

    iCmManager.SupportedBearersL( bearers );

    for ( TInt i = 0; i < bearers.Count() && !iCheckConnDlgCancelled; i++ )
        {
        TBool avail( EFalse );
        
        TRAPD( err, avail = iCmManager.GetBearerInfoBoolL( bearers[i], ECmCoverage ) );

        CLOG_WRITE_2( "CCmWizard::CheckForCms, bearer [%d] coverage error [%d]", i, err );
        
        if( err )
            {
            continue;
            }
            

        if ( avail &&
             iCmManager.GetBearerInfoBoolL( bearers[i], ECmAddToAvailableList ) )
            {
            aCoverageArray.AppendL( bearers[i] );
            CLOG_WRITE_1( "CCmWizard::CheckForCms, bearer [%d] appended", i );
            }
        }

    CleanupStack::PopAndDestroy( &bearers );

    // Dismiss wait dialog
    if( !iCheckConnDlgCancelled )
        {
        CLOG_WRITE( "CCmWizard::CheckForCms, cancelling wait dlg" );
        iCheckConnDlg->ProcessFinishedL();
        
        // Wait here in case wait note has not yet been dismissed.
        // This happens in hw when Themes/effects is enabled.
        if ( iCheckConnDlg != NULL )
            {
            iWait.Start();
            }
        CLOG_WRITE( "CCmWizard::CheckForCms, wait dialog deleted" );
        }
    else
        {
        CLOG_WRITE( "CCmWizard::CheckForCms, wait dlg was already cancelled" );
        }

    // Set the member to NULL every case
    iCheckConnDlg = NULL;
    }



// ---------------------------------------------------------------------------
// CCmWizard::FindTargetDestinationL
// ---------------------------------------------------------------------------
//
TUint32 CCmWizard::FindTargetDestinationL( CCmPluginBase* aPlugin )
    {
    LOGGER_ENTERFN( "CCmWizard::FindTargetDestinationL" );

    // Tries to read the destination id
    TUint32 targetDestId (0); 
    TRAPD( err, targetDestId = 
                aPlugin->GetIntAttributeL( ECmNextLayerSNAPId ) );
                
    // only supported by virtual bearers, don't leave        
    if ( err && err != KErrNotSupported )
       {
       User::Leave( err );
       }
                
    // targetDestId == 0 means that not destination but a connection 
    // method have been selected
    if ( targetDestId == 0 )
        {
        // Now the destination containing the connection method 
        // should be obtained
        TUint32 targetCmId (0); 
        TRAPD( err, targetCmId = 
                    aPlugin->GetIntAttributeL( ECmNextLayerIapId ) );
        
        // only supported by virtual bearers, don't leave        
        if ( err && err != KErrNotSupported )
            {
            User::Leave( err );
            }
            
        targetDestId = FindParentDestinationL( targetCmId );   
        }
        
    return targetDestId;    
    }
 
// ---------------------------------------------------------------------------
// CCmWizard::FindParentDestinationL
// ---------------------------------------------------------------------------
//
TUint32 CCmWizard::FindParentDestinationL( TUint32 aCmId )
    {
    LOGGER_ENTERFN( "CCmWizard::FindParentDestinationL" );

    TBool found = EFalse;
    TUint32 targetDestId (0); 
    
    RArray<TUint32> destinations;
   
    iCmManager.AllDestinationsL( destinations );
    CleanupClosePushL( destinations );
    
    RArray<TUint32> cms;
    
    for ( TInt i = 0; i < destinations.Count() && !found; i++ )
        {            
        CCmDestinationImpl* dest = 
                          iCmManager.DestinationL( destinations[i] );
        CleanupStack::PushL( dest );
        dest->ConnectMethodIdArrayL( cms );
        CleanupClosePushL( cms );

        for ( TInt j = 0; j < cms.Count() && !found; j++ )
            {
            if( cms[j] == aCmId)
                {
                targetDestId = destinations[i];
                found = ETrue;
                }
            }
        CleanupStack::PopAndDestroy( &cms );
        CleanupStack::PopAndDestroy( dest );
        }

    CleanupStack::PopAndDestroy( &destinations );

    return targetDestId;    
    }
