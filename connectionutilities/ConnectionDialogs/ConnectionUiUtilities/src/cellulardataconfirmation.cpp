/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Prompt Dialog implementation
*
*/


// INCLUDE FILES
#include <StringLoader.h>
#include <connuiutilsnotif.rsg>
#include <hb/hbcore/hbsymbianvariant.h>
#include "ConnectionDialogsLogger.h"
#include "cellulardataconfirmation.h"
#include "ConnectionDialogsNotifBase.h"
#include "ConnectionDialogsUidDefs.h"
#include "devicedialogobserver.h"


// CONSTANTS
_LIT( KHomenetwork, "homenetwork" );
_LIT( KType, "com.nokia.ipconnmgmt.promptdialog/1.0" );

// ---------------------------------------------------------
// CCellularDataConfirmation::CCellularDataConfirmation()
// ---------------------------------------------------------
//
CCellularDataConfirmation::CCellularDataConfirmation():
iDialog( NULL ), iObserver( NULL ), iCompleted( EFalse ),
iStatus( NULL ), iResult( NULL )
    {
    }

// ---------------------------------------------------------
// CCellularDataConfirmation::~CCellularDataConfirmation()
// ---------------------------------------------------------
//
CCellularDataConfirmation::~CCellularDataConfirmation()
    {
    CLOG_ENTERFN( "CCellularDataConfirmation::~CCellularDataConfirmation" );
    delete iDialog;
    delete iObserver;
    CLOG_LEAVEFN( "CCellularDataConfirmation::~CCellularDataConfirmation" );
    }

// ---------------------------------------------------------
// CCellularDataConfirmation* CCellularDataConfirmation::NewL
// ---------------------------------------------------------
//
CCellularDataConfirmation* CCellularDataConfirmation::NewL( )
    {
    CLOG_ENTERFN( "CCellularDataConfirmation::NewL" );
    CCellularDataConfirmation* self = new ( ELeave ) CCellularDataConfirmation();
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop();
    CLOG_LEAVEFN( "CCellularDataConfirmation::NewL" );
    return self;
    }

void CCellularDataConfirmation::ConstructL()
    {
    }

// ---------------------------------------------------------
// void CCellularDataConfirmation::StartL
// ---------------------------------------------------------
//
void CCellularDataConfirmation::StartL( TMsgQueryLinkedResults& aResult,
        const TBool aHomeNetwork,
        TRequestStatus& aStatus )
    {
    CLOG_ENTERFN( "CCellularDataConfirmation::StartL" );
    aResult = EMsgQueryCancelled;
    iResult = &aResult;
    aStatus = KRequestPending;
    iStatus = &aStatus;

    // Create the list box items
    RArray<TMsgQueryLinkedResults> choices;           
    
    // The variant map is needed to construct the dialog correctly,
    // information whether we are in home network is needed
    if ( aHomeNetwork ) 
        {
        choices.AppendL( EMsgQueryAutomatically );
        choices.AppendL( EMsgQueryThisTime );
        choices.AppendL( EMsgQueryCancelled );
        }
    else
        {
        choices.AppendL( EMsgQueryThisTime );
        choices.AppendL( EMsgQueryAutomatically );
        choices.AppendL( EMsgQueryCancelled );
        }

    // Observer is needed in order to get the user inputs
    iObserver = CDeviceDialogObserver::NewL( this );
    iDialog = CHbDeviceDialogSymbian::NewL();
    
    // Set the choice order for the observer so that it knows which button press
    // means which choice
    iObserver->SetChoices( choices );
    
    // Create the variant data information for the plugin
    CHbSymbianVariantMap* map = CHbSymbianVariantMap::NewL();
    CleanupStack::PushL( map );
    CHbSymbianVariant *variant
      = CHbSymbianVariant::NewL( &aHomeNetwork, CHbSymbianVariant::EBool );
    CleanupStack::PushL( variant );
    User::LeaveIfError( map->Add( KHomenetwork, variant ) );
    CleanupStack::Pop( variant ); // map's cleanup sequence handles variant.
    
    // Show the dialog.
    User::LeaveIfError( iDialog->Show( KType, *map, iObserver ) );
    
    CleanupStack::PopAndDestroy( map ); 
    CLOG_LEAVEFN( "CCellularDataConfirmation::StartL" );
    }

// ---------------------------------------------------------
// void CConfirmationQueryNotif::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CCellularDataConfirmation::CompleteL( TInt aStatus )
    {   
    CLOG_ENTERFN( "CCellularDataConfirmation::CompleteL" );
    if ( !iCompleted )
        {
        iCompleted = ETrue;
        User::RequestComplete( iStatus, aStatus );
        }
    CLOG_LEAVEFN( "CCellularDataConfirmation::CompleteL" );
    }

// ---------------------------------------------------------------
// void CConfirmationQueryNotif::SetSelectedChoiceL( TInt aChoice )
// ----------------------------------------------------------------
//
void CCellularDataConfirmation::SetSelectedChoice ( TInt aChoice )
    {
    CLOG_ENTERFN( "CCellularDataConfirmation::SetSelectedChoice" );
    *iResult = (TMsgQueryLinkedResults)aChoice;
    CLOG_LEAVEFN( "CCellularDataConfirmation::SetSelectedChoice" );
    }

// ---------------------------------------------------------------
// void CCellularDataConfirmation::Cancel()
// ----------------------------------------------------------------
//
void CCellularDataConfirmation::Cancel()
    {
    CLOG_ENTERFN( "CCellularDataConfirmation::Cancel" );
    if ( !iCompleted )
        {
        iDialog->Cancel(); 
        User::RequestComplete( iStatus, KErrCancel );
        }
    CLOG_LEAVEFN( "CCellularDataConfirmation::Cancel" );
    }

