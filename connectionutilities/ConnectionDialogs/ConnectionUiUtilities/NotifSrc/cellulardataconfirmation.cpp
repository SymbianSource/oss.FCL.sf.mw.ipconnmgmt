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
#include <ConnUiUtilsNotif.rsg>
#include <hb/hbcore/hbsymbianvariant.h>
#include "ConnectionDialogsLogger.h"
#include "cellulardataconfirmation.h"
#include "ConnectionDialogsNotifBase.h"
#include "ConnectionDialogsUidDefs.h"
#include "ConnUiUtilsNotif.h"
#include "devicedialogobserver.h"


// CONSTANTS
const TInt KVariableLength = 30;
_LIT( KHomenetwork, "homenetwork");
_LIT(KType, "com.nokia.ipconnmgmt.promptdialog/1.0");

// ---------------------------------------------------------
// CCellularDataConfirmation::CCellularDataConfirmation()
// ---------------------------------------------------------
//
CCellularDataConfirmation::CCellularDataConfirmation()
    {
    iCompleted = EFalse;
    iChoice = EMsgQueryCancelled;
    iObserver = NULL;
    iDialog = NULL;
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
CCellularDataConfirmation* CCellularDataConfirmation::NewL( 
                                        const TBool aResourceFileResponsible )
    {
    CLOG_ENTERFN( "CCellularDataConfirmation::NewL" );
    CCellularDataConfirmation* self = new ( ELeave ) CCellularDataConfirmation();
    CleanupStack::PushL( self );
    self->ConstructL( KResourceFileName, aResourceFileResponsible );
    CleanupStack::Pop();
    CLOG_LEAVEFN( "CCellularDataConfirmation::NewL" );
    return self;
    }

// ---------------------------------------------------------
// void CCellularDataConfirmation::StartL
// ---------------------------------------------------------
//
void CCellularDataConfirmation::StartL( const TDesC8& aBuffer, 
                                        TInt aReplySlot, 
                                        const RMessagePtr2& aMessage )
    {
    CLOG_ENTERFN( "CCellularDataConfirmation::StartL" );
    iChoice = EMsgQueryCancelled; 
    TPckgBuf<TConnUiUiDestConnMethodNoteId> input;
    TConnUiUiDestConnMethodNoteId noteInfo;
    input.Copy( aBuffer );
    noteInfo = input();   
    iReplySlot = aReplySlot;
    iMessage = aMessage;
    
    // Create the list box items
    RArray<TMsgQueryLinkedResults> choices;           
    
    // Observer is needed in order to get the user inputs
    iObserver = CDeviceDialogObserver::NewL( this );
    iDialog = CHbDeviceDialogSymbian::NewL();
    
    // The variant map is needed to construct the dialog correctly,
    // information whether we are in home network is needed
    TBuf<KVariableLength> key(KHomenetwork);
    TBool homeNetwork;
    if ( noteInfo.iNoteId == EConfirmMethodUsageQueryInHomeNetwork ) 
        {
        homeNetwork = ETrue;
        choices.Append(EMsgQueryAutomatically);
        choices.Append(EMsgQueryThisTime);
        choices.Append(EMsgQueryCancelled);
        }
    else
        {
        homeNetwork = EFalse;
        choices.Append(EMsgQueryThisTime);
        choices.Append(EMsgQueryAutomatically);
        choices.Append(EMsgQueryCancelled);
        }
    // Set the choice order for the observer so that it knows which button press
    // means which choice
    iObserver->SetChoices(choices);
    
    // Create the variant data information for the plugin
    CHbSymbianVariantMap* map = CHbSymbianVariantMap::NewL();
    CleanupStack::PushL( map );
    CHbSymbianVariant *variant =  CHbSymbianVariant::NewL ( &homeNetwork, CHbSymbianVariant::EBool );
    CleanupStack::PushL( variant );
    TInt error = map->Add( key, variant);
    User::LeaveIfError( error );
    CleanupStack::Pop( variant ); // map's cleanup sequence handles variant.
    
    // Show the dialog.
    error = iDialog->Show( KType, *map, iObserver );
    User::LeaveIfError( error );
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
        if ( !iMessage.IsNull() )
            {  
            iMessage.WriteL( iReplySlot, TPckg<TMsgQueryLinkedResults>( iChoice ) );
            iMessage.Complete( aStatus );
            iCompleted = ETrue;
            } 
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
    iChoice = (TMsgQueryLinkedResults)aChoice;
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
        TRAP_IGNORE( iMessage.WriteL( iReplySlot, 
            TPckg<TMsgQueryLinkedResults>( EMsgQueryCancelled ) ));
        iMessage.Complete( KErrCancel ); 
        }
    CLOG_LEAVEFN( "CCellularDataConfirmation::Cancel" );
    }

// ---------------------------------------------------------------
// CCellularDataConfirmation::TNotifierInfo 
// CCellularDataConfirmation::RegisterL()
// ----------------------------------------------------------------
//
CCellularDataConfirmation::TNotifierInfo CCellularDataConfirmation::RegisterL()
    {
    CLOG_ENTERFN( "CCellularDataConfirmation::RegisterL" );
    iInfo.iUid = KUidConfirmationQuery;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel = KUidConfirmationQuery;
    CLOG_LEAVEFN( "CCellularDataConfirmation::RegisterL" );
    return iInfo;
    }
