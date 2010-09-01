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
* Description:  Class for asyncron calls.
*
*/


// INCLUDE FILES
#include <rmmcustomapi.h>

#include "apcontrollistcapschange.h"
#include "apcontrollistplugin.h"
#include "apcontrollistapiwrapper.h"
#include "apcontrollistpluginlogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------- 
// CAPControlListCapsChange::CAPControlListCapsChange
// Description: CAPControlListCapsChange constructer 
// ----------------------------------------------------------------------------- 
//
CAPControlListCapsChange::CAPControlListCapsChange(
					CAPControlListPlugin& aPlugin, TInt aPriority ) : 
													CActive( aPriority ), 
													iPlugin(aPlugin) 
	{
	CActiveScheduler::Add( this ); 
	}

//
//----------------------------------------------------------------------------- 
// CAPControlListCapsChange:: CAPControlListCapsChange
// Description: CAPControlListCapsChange Destructer
// ----------------------------------------------------------------------------- 
// 
CAPControlListCapsChange::~CAPControlListCapsChange()
	{
	Cancel();
	}



//
//----------------------------------------------------------------------------- 
// CAPControlListCapsChange::DoCancel
// Description: This calls CActive's Cancel 
//----------------------------------------------------------------------------- 
//
void CAPControlListCapsChange::DoCancel()
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListCapsChange::DoCancel" ) ) );
    iPlugin.APIWrapper()->CancelNotify();   
    CLOG( ( ESelector, 0, _L( "<- CAPControlListCapsChange::DoCancel" ) ) );
    }

//
//----------------------------------------------------------------------------- 
// CAPControlListCapsChange::RunL 
// Description: CActive::RunL implementation which will either stop the
// Scheduler or increment the count
//----------------------------------------------------------------------------- 
//
void CAPControlListCapsChange::RunL()
	{
    CLOG( ( ESelector, 0, _L( "-> CAPControlListCapsChange::RunL" ) ) );
    CLOG( ( ESelector, 0, _L( "iStatus.Int(): %d"), iStatus.Int() ) );
    CLOG( ( ESelector, 0, _L( "iCaps: %d"), iCaps ) );
	if ( !(iCaps & RMobilePhone::KCapsUSimAccessSupported) )
		{
		iPlugin.ExitOnUSIMRemovalL();
		}
	else
		{
    	iPlugin.APIWrapper()->NotifyIccAccessCapsChange( iStatus, iCaps );
		SetActive();			
		}
    CLOG( ( ESelector, 0, _L( "<- CAPControlListCapsChange::RunL" ) ) );
	}
	


// ---------------------------------------------------------
// CAPControlListCapsChange::NotifyCapsChange()
// ---------------------------------------------------------
//
void CAPControlListCapsChange::NotifyCapsChange()
    {   	
    CLOG( ( ESelector, 0, _L( 
    	"-> CAPControlListCapsChange::NotifyCapsChange" ) ) );
	if( !IsActive() )
		{
    	iPlugin.APIWrapper()->NotifyIccAccessCapsChange( iStatus, iCaps );
		SetActive();	
		}
    CLOG( ( ESelector, 0, _L( 
    	"<- CAPControlListCapsChange::NotifyCapsChange" ) ) );
    }

