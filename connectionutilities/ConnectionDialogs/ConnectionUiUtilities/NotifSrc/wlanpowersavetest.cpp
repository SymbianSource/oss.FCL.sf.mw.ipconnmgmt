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
* Description:  Dialog that implements WLAN Power Save Testing notification
*
*/


// INCLUDE FILES
#include "wlanpowersavetest.h"
#include "wlanpowersavetestnotif.h"
#include "ConnectionUiUtilitiesCommon.h"
#include "ConnectionDialogsLogger.h"

#include <uikon/eiksrvui.h>
#include <StringLoader.h>
#include <connuiutilsnotif.rsg>

// ================= MEMBER FUNCTIONS =======================
//
// ---------------------------------------------------------
// CWlanPowerSaveQuery::CWlanPowerSaveQuery
// ---------------------------------------------------------
//
CWlanPowerSaveQuery::CWlanPowerSaveQuery( CWlanPowerSaveQueryNotif* aNotif )
: iNotif( aNotif ),
  iButtonGroupPreviouslyChanged( EFalse )
   {
   }
    
    
// ---------------------------------------------------------
// CWlanPowerSaveQuery::~CWlanPowerSaveQuery
// ---------------------------------------------------------
//
CWlanPowerSaveQuery::~CWlanPowerSaveQuery()
    {
    STATIC_CAST( CEikServAppUi*, 
                 CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( EFalse );
    }



// ---------------------------------------------------------
// CWlanPowerSaveQuery::OkToExitL
// ---------------------------------------------------------
//
TBool CWlanPowerSaveQuery::OkToExitL( TInt aButtonId )
    {
    CLOG_ENTERFN( "CWlanPowerSaveQuery::OkToExitL" );
    TInt status = KErrCancel;
    if ( aButtonId == EAknSoftkeyYes || aButtonId == EAknSoftkeySelect || 
         aButtonId == EAknSoftkeyDone || aButtonId == EAknSoftkeyOk )
        {
        status = KErrNone;
        }

    CLOG_WRITEF( _L( "aButtonId = %d" ), aButtonId );
    
    if ( iNotif )
        {
        iNotif->CompleteL( status );	
        }
    
    CLOG_LEAVEFN( "CWlanPowerSaveQuery::OkToExitL" );

    return ETrue;
    }
    
    
// ---------------------------------------------------------
// CWlanPowerSaveQuery::PreLayoutDynInitL()
// ---------------------------------------------------------
//
void CWlanPowerSaveQuery::PreLayoutDynInitL()
    {    
    CAknMessageQueryDialog::PreLayoutDynInitL();

    STATIC_CAST( CEikServAppUi*, 
                 CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( ETrue );
    }


// ---------------------------------------------------------
// CWlanPowerSaveQuery::OfferKeyEventL()
// ---------------------------------------------------------
//
TKeyResponse CWlanPowerSaveQuery::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                 TEventCode aModifiers )
    {
    CLOG_ENTERFN( "CWlanPowerSaveQuery::OfferKeyEventL" );
    TInt code = aKeyEvent.iCode;
        
    switch ( code )
        {    
        case EKeyUpArrow:
        case EKeyDownArrow:
            {
            CAknMessageQueryControl* messageQueryControl = STATIC_CAST( 
                                        CAknMessageQueryControl*, 
                                        Control( EAknMessageQueryContentId ) );
            if ( messageQueryControl )
                {
                TKeyResponse answer = messageQueryControl->OfferKeyEventL( 
                                                    aKeyEvent, aModifiers );
                if ( answer == EKeyWasConsumed )
                    {
                    if ( messageQueryControl->LinkHighLighted() ) 
                        {
                        if ( !iButtonGroupPreviouslyChanged )
                            {
                            CEikButtonGroupContainer& cba = 
                                                        ButtonGroupContainer();

                            ButtonGroupContainer().AddCommandSetToStackL( 
                                                    R_SOFTKEYS_VIEW_EMPTY__VIEW );

                            cba.UpdateCommandObserverL( 
                              CEikButtonGroupContainer::ELeftSoftkeyPosition,
                              *this );

                            cba.UpdateCommandObserverL( 
                              CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                              *this );

                            cba.DrawDeferred();
 
                            iButtonGroupPreviouslyChanged = ETrue;
                           }
                        }
                    else if ( iButtonGroupPreviouslyChanged )
                        {                            
                        CEikButtonGroupContainer& cba = ButtonGroupContainer();

                        cba.RemoveCommandObserver( 
                            CEikButtonGroupContainer::ELeftSoftkeyPosition );

                        cba.RemoveCommandObserver( 
                            CEikButtonGroupContainer::EMiddleSoftkeyPosition );

                        cba.RemoveCommandFromStack( 
                            CEikButtonGroupContainer::ELeftSoftkeyPosition,
                            EAknSoftkeyView );

                        cba.RemoveCommandFromStack( 
                            CEikButtonGroupContainer::ERightSoftkeyPosition, 
                            EAknSoftkeyNo );

                        cba.RemoveCommandFromStack( 
                            CEikButtonGroupContainer::EMiddleSoftkeyPosition, 
                            EAknSoftkeyView );

                        cba.DrawDeferred();

                        iButtonGroupPreviouslyChanged = EFalse;
                        }
                    }
                CLOG_WRITEF( _L( "answer = %d" ), answer );
                CLOG_LEAVEFN( "CWlanPowerSaveQuery::OfferKeyEventL 1" );
                return answer;  
                }
            }
            break;
            
        default:
            break;
        }

    CLOG_LEAVEFN( "CWlanPowerSaveQuery::OfferKeyEventL 2" );
    return CAknMessageQueryDialog::OfferKeyEventL( aKeyEvent, aModifiers );
    }


// ---------------------------------------------------------
// CWlanPowerSaveQuery::TryExitL()
// ---------------------------------------------------------
//
void CWlanPowerSaveQuery::TryExitL( TInt aButtonId )
    {
    CLOG_ENTERFN( "CWlanPowerSaveQuery::TryExitL" );
    CAknMessageQueryDialog::TryExitL( aButtonId );
    CLOG_LEAVEFN( "CWlanPowerSaveQuery::TryExitL" );
    }

// End of File
