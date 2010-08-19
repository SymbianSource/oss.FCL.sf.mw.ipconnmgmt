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
#include <AknQueryDialog.h>
#include <rmmcustomapi.h>
#include <etelmm.h>
#include <apcontrollistpluginrsc.rsg>
#include <errorres.rsg>
#include <StringLoader.h>
#include <aknnotewrappers.h> 

#include "apcontrollistapiwrapper.h"
#include "apcontrollistpluginlogger.h"
#include "apcontrollistplugincontainer.h"
#include "apcontrollistplugin.h"
#include "apcontrollistbox.h"
#include "centralrepository.h"
#include "CoreApplicationUIsSDKCRKeys.h"

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------- 
// CAPControlListAPIWrapper::CAPControlListAPIWrapper
// Description: CAPControlListAPIWrapper constructer 
// ----------------------------------------------------------------------------- 
//
CAPControlListAPIWrapper::CAPControlListAPIWrapper(
							CAPControlListPlugin& aPlugin, TInt aPriority ) : 
											CActive( aPriority ), 
											iPlugin(aPlugin),
											iApnPKG(iApn),
											iSimCardError( EFalse )
	{
	CActiveScheduler::Add( this ); 
	}

//
//----------------------------------------------------------------------------- 
// CAPControlListAPIWrapper:: CAPControlListAPIWrapper
// Description: CAPControlListAPIWrapper Destructer
// ----------------------------------------------------------------------------- 
// 
CAPControlListAPIWrapper::~CAPControlListAPIWrapper()
	{
    CLOG( ( ESelector, 0, _L( 
    			"-> CAPControlListAPIWrapper::~CAPControlListAPIWrapper" ) ) );
	Cancel();
	iCustomAPI.Close();
	iPhone.Close();
	iServer.Close();
	delete iAPN;
    CLOG( ( ESelector, 0, _L( 
    			"<- CAPControlListAPIWrapper::~CAPControlListAPIWrapper" ) ) );
	}

//
//----------------------------------------------------------------------------- 
// CAPControlListAPIWrapper::RequestFunction
// Description: Request Function for CAPControlListAPIWrapper 
// ---------------------------------------------------------------------------- 
//
void CAPControlListAPIWrapper::SecurityCheck(TRequestType aRequest) 
	{
    CLOG( ( ESelector, 0, _L( 
    			"-> CAPControlListAPIWrapper::SecurityCheck" ) ) );
	if( !IsActive() )
		{
		iCustomAPI.CheckSecurityCode (iStatus, 
								RMmCustomAPI::ESecurityCodePin2 ); 	 	
		iRequest = aRequest;	
		SetActive();
		}
    CLOG( ( ESelector, 0, _L( 
    			"<- CAPControlListAPIWrapper::SecurityCheck" ) ) );
	}


//
//----------------------------------------------------------------------------- 
// CAPControlListAPIWrapper::DoCancel
// Description: This is called from CActive's Cancel 
//----------------------------------------------------------------------------- 
//
void CAPControlListAPIWrapper::DoCancel()
	{
    CLOG( ( ESelector, 0, _L( 
    			"-> CAPControlListAPIWrapper::DoCancel" ) ) );
	CLOG( ( ESelector, 0, _L( "Current request: %d" ), iRequest ) );
	switch(iRequest)
	{
	case EEnumerateAPN: 
        iPhone.CancelAsyncRequest(EMobilePhoneEnumerateAPNEntries);
	    break; 
	case EReadData: 
        iPhone.CancelAsyncRequest(EMobilePhoneGetAPNname);
	    break;
	    
	// Since there are no enums for these, can't cancel  
	case EAddAPNSecurity:
	case EAddAPN:
	case ERemoveAPNSecurity:
	case ERemoveAPN: 
	case EActivateACLSecurity:
	case EActivateACL: 
	case EDeactivateACLSecurity:
	case EDeactivateACL:
	    break; 	
	 
	case ENoRequest: // no active request
	    break; 	
	default:
		// This could be only in case of programming error
		CLOG( ( ESelector, 0, _L( "State Error: %d" ), iRequest ) );
	    break;
	}
    CLOG( ( ESelector, 0, _L( 
    			"<- CAPControlListAPIWrapper::DoCancel" ) ) );
	}

//
//----------------------------------------------------------------------------- 
// CAPControlListAPIWrapper::FinishReadDataL
// Description: Case EReadData in RunL  
//----------------------------------------------------------------------------- 
//
void CAPControlListAPIWrapper::FinishReadDataL()
	{
    CLOG( ( ESelector, 0, _L( 
    			"-> CAPControlListAPIWrapper::FinishReadDataL" ) ) );
	if (iIndex > 0)
		{
		iApn = iApnPKG();
		// lablel lengthes must be replaced with dots and  
	    // removed from the beginnig
	    if ( iApn.iApn.Length() )
    	    {
    	    TUint8 lablelLength	= (TUint8)(iApn.iApn[0]);
    	    TUint8 wholeLength	= lablelLength;
    	    iApn.iApn.Delete(0,1);
    	    while (iApn.iApn.Length() > wholeLength)
    	        {
    	        wholeLength = wholeLength + 
    	            (TUint8)(iApn.iApn[lablelLength]) + 1;
    	        iApn.iApn[lablelLength] = '.';
    	        lablelLength = wholeLength;//place of next dot
    	        }	        
		    HBufC* buf = HBufC::NewLC( RMobilePhone::KMaxApnName );
		    buf->Des().Copy( iApn.iApn );
		    TPtrC itemPtr(buf->Des());
			iPlugin.Container()->AddToListBoxL(itemPtr);			
		    CleanupStack::PopAndDestroy( buf );
    	    }
	    else
	        {
			// Empty APN means network provided APN
    		HBufC* buf = StringLoader::LoadLC ( 
    							R_QTN_ACL_NETWORK_PROVIDED_APN );
    		TPtrC nPtr(buf->Des());
			iPlugin.Container()->AddToListBoxL(nPtr);			
		    CleanupStack::PopAndDestroy( buf );
	        }
		}
	if (iIndex < iSize)
		{
		DoReadData();
		iIndex++;
		}
	else
		{
		iRequest = ENoRequest;			
		if ( iSize > 0 )
			{
			iPlugin.Container()->SetCurrentItemIndex(0);    		
			}				
		}
    CLOG( ( ESelector, 0, _L( 
    			"<- CAPControlListAPIWrapper::FinishReadDataL" ) ) );
	}

//
//----------------------------------------------------------------------------- 
// CAPControlListAPIWrapper::RunL 
// Description: CActive::RunL implementation which will either stop the
// Scheduler or increment the count
//----------------------------------------------------------------------------- 
//
void CAPControlListAPIWrapper::RunL()
	{
    CLOG( ( ESelector, 0, _L( "RequestStatus: %d for request: %d" ), 
                                                iStatus.Int(), iRequest) );     

    // If there happens an error show error note and set this sw to error
    // state --> does not accept any operations but closing.
	if ( iStatus.Int() != KErrNone )
		{
        ShowSimCardErrorNoteL();

        iSimCardError = ETrue;
	    iRequest = ENoRequest;  

		return;
		}
	
	iSimCardError = EFalse;
	
	switch(iRequest)
	{
	case EGetACLStatus:
	    {
		CLOG( ( ESelector, 0, _L( "EGetACLStatus OK" ) ) );
#ifdef _DEBUG
    	if (iAclStatus == RMobilePhone::EAPNControlListServiceEnabled)
    		{		
        	CLOG( ( ESelector, 0, _L( "iAclStatus == EAclStatusEnabled" ) ) );
    		}
    	else if (iAclStatus == RMobilePhone::EAPNControlListServiceDisabled)
    		{		
        	CLOG( ( ESelector, 0, _L( "iAclStatus == EAclStatusDisabled" ) ) );
    		}
    	else
    		{		
        	CLOG( ( ESelector, 0, _L( "iAclStatus is bad!!!" ) ) );
    		}
#endif 		    
		if ( Enabled() )
			{
		    iPlugin.ShowInfoNoteL(R_QTN_ACL_INFO_ACTIVE);
		    iPlugin.Container()->WriteToNaviPaneL(R_QTN_ACL_NAVI_ACTIVE);
			}
		else
			{
		    iPlugin.ShowInfoNoteL(R_QTN_ACL_INFO_NOT_ACTIVE);
		    iPlugin.Container()->WriteToNaviPaneL(R_QTN_ACL_NAVI_NOT_ACTIVE);
			}
        EnumerateAPN();
	    break; 	        
	    }
	case EAddAPNSecurity:
		CLOG( ( ESelector, 0, _L( "EAddAPNSecurity OK" ) ) );
		iSecurityChecked = ETrue;
		DoAddAPNL();
	    break; 
	case EAddAPN:
	    {
		iRequest = ENoRequest;	
		CLOG( ( ESelector, 0, _L( "EAddAPN OK" ) ) );
        TPtrC tptrc(iAPN->Des());
		if (tptrc.Length())
			{
	    	iPlugin.Container()->AddToListBoxL(tptrc);        
			}
		else
			{
			HBufC* buf = StringLoader::LoadLC ( 
						R_QTN_ACL_NETWORK_PROVIDED_APN );
			TPtrC itemPtr(buf->Des());
			iPlugin.Container()->AddToListBoxL(itemPtr);			
			CleanupStack::PopAndDestroy( buf );
			}
	    break; 	        
	    } 
	case ERemoveAPNSecurity:
		CLOG( ( ESelector, 0, _L( "ERemoveAPNSecurity OK" ) ) );
		iSecurityChecked = ETrue;
		DoRemoveAPN();
	    break; 
	case ERemoveAPN: 
		iRequest = ENoRequest;	
		CLOG( ( ESelector, 0, _L( "ERemoveAPN OK" ) ) );		
		iPlugin.Container()->RemoveFromListBoxL();    			
	    break; 
	case EActivateACLSecurity:
		CLOG( ( ESelector, 0, _L( "EActivateACLSecurity OK" ) ) );
		iSecurityChecked = ETrue;
		SetACLStatus( RMobilePhone::EAPNControlListServiceEnabled );
	    break; 
	case EActivateACL: 
		iRequest = ENoRequest;	
		CLOG( ( ESelector, 0, _L( "EActivateACL OK" ) ) );
		iAclStatus = RMobilePhone::EAPNControlListServiceEnabled;
		iPlugin.ShowConfirmNoteL(R_QTN_ACL_CONF_CONTROL_ACTIVATED);	
		iPlugin.Container()->WriteToNaviPaneL(R_QTN_ACL_NAVI_ACTIVE);
	    break; 
	case EDeactivateACLSecurity:
		CLOG( ( ESelector, 0, _L( "EDeactivateACLSecurity OK" ) ) );
		iSecurityChecked = ETrue;
		SetACLStatus( RMobilePhone::EAPNControlListServiceDisabled );
	    break; 
	case EDeactivateACL: 
		iRequest = ENoRequest;	
		CLOG( ( ESelector, 0, _L( "EDeactivateACL OK" ) ) );
		iAclStatus = RMobilePhone::EAPNControlListServiceDisabled;
		iPlugin.ShowConfirmNoteL(R_QTN_ACL_CONF_CONTROL_DEACTIVATED);	
		iPlugin.Container()->WriteToNaviPaneL(R_QTN_ACL_NAVI_NOT_ACTIVE);
	    break; 
	case EEnumerateAPN: 
		CLOG( ( ESelector, 0, _L( "EEnumerateAPN OK" ) ) );
		iRequest = EReadData;	
		FinishReadDataL();
	    break; 
	case EReadData: 
		CLOG( ( ESelector, 0, _L( "EReadData OK" ) ) );
		FinishReadDataL();
	    break; 
	default:
		{
		// This could be only in case of programming error
		CLOG( ( ESelector, 0, _L( "State Error: %d" ), iRequest ) );
		User::Leave(KErrGeneral);			
		}
	    break;
	}
	}
	

// ---------------------------------------------------------
// CAPControlListAPIWrapper::DoReadData
// ---------------------------------------------------------
void CAPControlListAPIWrapper::DoReadData() 
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListAPIWrapper::ReadData2" ) ) );
	if( !IsActive() )
		{
		iRequest = EReadData;			
		iApn.iApn.Zero();
		iApn.iApn.SetMax();
		iPhone.GetAPNname(iStatus, iIndex, iApnPKG);
		SetActive();
		}    
    CLOG( ( ESelector, 0, _L( "<- CAPControlListAPIWrapper::ReadData2" ) ) );
    }
// ---------------------------------------------------------
// CAPControlListAPIWrapper::ReadData
// ---------------------------------------------------------
void CAPControlListAPIWrapper::ReadData() 
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListAPIWrapper::ReadData" ) ) );
	if( !IsActive() )
		{
		iRequest = EGetACLStatus;			
	    iPhone.GetAPNControlListServiceStatus( iStatus, iAclStatus );
		SetActive();
		}    
    CLOG( ( ESelector, 0, _L( "<- CAPControlListAPIWrapper::ReadData" ) ) );
    }
// ---------------------------------------------------------
// CAPControlListAPIWrapper::ReadData
// ---------------------------------------------------------
void CAPControlListAPIWrapper::EnumerateAPN() 
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListAPIWrapper::EnumerateAPN" ) ) );
	if( !IsActive() )
		{
		iRequest = EEnumerateAPN;			
		iIndex = 0;
		iPhone.EnumerateAPNEntries(iStatus, iSize);
		SetActive();
		}    
    CLOG( ( ESelector, 0, _L( "<- CAPControlListAPIWrapper::EnumerateAPN" ) ) );
    }
    
// ---------------------------------------------------------
// CAPControlListAPIWrapper::DeActivateACLL()
// ---------------------------------------------------------
//
void CAPControlListAPIWrapper::DeActivateACL()
    {   	
    CLOG( ( ESelector, 0, _L( 
    			"-> CAPControlListAPIWrapper::DeActivateACLL" ) ) );
    
    // Check if in error state
    if ( iSimCardError )
        {
        ShowSimCardErrorNoteL();
        return;
        }
	if (iSecurityChecked)
		{
		SetACLStatus( RMobilePhone::EAPNControlListServiceDisabled );
		}
	else
		{
		SecurityCheck(EDeactivateACLSecurity);
		}
    
    CLOG( ( ESelector, 0, _L( 
    			"<- CAPControlListAPIWrapper::DeActivateACLL" ) ) );
    }

// ---------------------------------------------------------
// CAPControlListAPIWrapper::ActivateACLL()
// ---------------------------------------------------------
//
void CAPControlListAPIWrapper::ActivateACL()
    {   	
    CLOG( ( ESelector, 0, _L( 
    			"-> CAPControlListAPIWrapper::ActivateACLL" ) ) );

    // Check if in error state
    if ( iSimCardError )
        {
        ShowSimCardErrorNoteL();
        return;
        }
	if (iSecurityChecked)
		{
		SetACLStatus( RMobilePhone::EAPNControlListServiceEnabled );
		}
	else
		{
		SecurityCheck(EActivateACLSecurity);
		}
    
    CLOG( ( ESelector, 0, _L( 
    			"<- CAPControlListAPIWrapper::ActivateACLL" ) ) );
    }

// ---------------------------------------------------------
// CAPControlListAPIWrapper::SetACLStatus()
// ---------------------------------------------------------
//
void CAPControlListAPIWrapper::SetACLStatus( const 
    RMobilePhone::TAPNControlListServiceStatus aAPNControlListServiceStatus )
    {   	
    CLOG( ( ESelector, 0, 
    				_L( "-> CAPControlListAPIWrapper::SetACLStatus" ) ) );
    
    // Check if in error state
    if ( iSimCardError )
        {
        ShowSimCardErrorNoteL();
        return;
        }

	if( !IsActive() )
		{
   		iPhone.SetAPNControlListServiceStatus( iStatus, 
   		                    aAPNControlListServiceStatus );
		if ( aAPNControlListServiceStatus == 
		        RMobilePhone::EAPNControlListServiceEnabled )
			{
			iRequest = EActivateACL;			
			}
		else
			{
			iRequest = EDeactivateACL;			
			}
		SetActive();
		}    
    CLOG( ( ESelector, 0, _L( 
    					"<- CAPControlListAPIWrapper::SetACLStatus" ) ) );
    }


// ---------------------------------------------------------
// CAPControlListAPIWrapper::RemoveAPN()
// ---------------------------------------------------------
//
void CAPControlListAPIWrapper::RemoveAPN()
    {   	
    CLOG( ( ESelector, 0, _L( "-> CAPControlListAPIWrapper::RemoveAPNL" ) ) );
    
    // Check if in error state
    if ( iSimCardError )
        {
        ShowSimCardErrorNoteL();
        return;
        }

    if( iPlugin.Container()->NumberOfItems() > 0)
        {
    	if (iSecurityChecked)
    		{
    		DoRemoveAPN();
    		}
    	else
    		{
    		SecurityCheck(ERemoveAPNSecurity);
    		}
        }
    CLOG( ( ESelector, 0, _L( "<- CAPControlListAPIWrapper::RemoveAPNL" ) ) );
    }

// ---------------------------------------------------------
// CAPControlListAPIWrapper::DoRemoveAPN()
// ---------------------------------------------------------
//
void CAPControlListAPIWrapper::DoRemoveAPN()
    {   	
    CLOG( ( ESelector, 0, _L( "-> CAPControlListAPIWrapper::RemoveAPN2L" ) ) );
    
    // Check if in error state
    if ( iSimCardError )
        {
        ShowSimCardErrorNoteL();
        return;
        }

	if( !IsActive() )
		{
		TInt index = iPlugin.Container()->CurrentItemIndex();
		iPhone.DeleteAPNName( iStatus, index );
		iRequest = ERemoveAPN;
		SetActive();
		}    
    CLOG( ( ESelector, 0, _L( "<- CAPControlListAPIWrapper::RemoveAPN2L" ) ) );
    }

// ---------------------------------------------------------
// CAPControlListAPIWrapper::AddAPNL()
// ---------------------------------------------------------
//
void CAPControlListAPIWrapper::AddAPNL()
    {   	
    CLOG( ( ESelector, 0, _L( "-> CAPControlListAPIWrapper::AddAPNL" ) ) );
    
    // Check if in error state
    if ( iSimCardError )
        {
        ShowSimCardErrorNoteL();
        return;
        }
	if (iSecurityChecked)
		{
		DoAddAPNL();
		}
	else
		{
		SecurityCheck(EAddAPNSecurity);
		}
    
    CLOG( ( ESelector, 0, _L( "<- CAPControlListAPIWrapper::AddAPNL" ) ) );
    }

// ---------------------------------------------------------
// CAPControlListAPIWrapper::DoAddAPNL()
// ---------------------------------------------------------
//
void CAPControlListAPIWrapper::DoAddAPNL()
    {   	
    CLOG( ( ESelector, 0, _L( "-> CAPControlListAPIWrapper::DoAddAPNL" ) ) );
	if( !IsActive() )
		{
		delete iAPN;
		iAPN = NULL;
		iAPN = HBufC::NewL( RMobilePhone::KMaxApnName );
	    TPtr16 ptr( iAPN->Des() );		    
	    CAknQueryDialog* dlg = 
	                CAknQueryDialog::NewL( ptr, CAknQueryDialog::ENoTone );
	       
	    TBool notCanceled = EFalse;
	    
	    // Ask the user until either the input is valid or canceled
	    while ( dlg->ExecuteLD( R_APN_NAME_QUERY ) )
	        {
	        
	        TBool validInput = ETrue;
	        
            for (TInt index = 0; index < ptr.Length(); index++)
                {
                TChar inputchar = ptr[index];
                // For ACL only 0-9, A-Z, a-z, '-' and '.' are valid
                if (! ( ('0' <= inputchar && inputchar <= '9') ||
                        ('A' <= inputchar && inputchar <= 'Z') ||
                        ('a' <= inputchar && inputchar <= 'z') ||
                        inputchar == '-' || inputchar == '.' ) )
                    {
                    validInput = EFalse;
                    break;
                    }
                }
            
	        if ( validInput )
		        {
		        notCanceled =  ETrue;
		        break;
		        }
		    else
    		    {
                HBufC* text = StringLoader::LoadLC ( 
                    R_QTN_ACL_ERR_INVALID_INPUT );
                CAknErrorNote* note = new ( ELeave ) CAknErrorNote(
                     ETrue );
                note->ExecuteLD( *text );

                CleanupStack::PopAndDestroy( text );
                
                dlg = 
	                CAknQueryDialog::NewL( ptr, CAknQueryDialog::ENoTone );
	            
    		    }
	        }
	        
	    if ( notCanceled )
		    {
		   
		    // lablel lengthes must be added in place of dots and at the 
		    // beginnig
            _LIT8(KFormat,"%c%S"); 
		    iApn.iApn.SetMax();
		    iApn.iApn.Zero();		    
            if ( iAPN->Length() )
                {
    	    	TBufC8<RMobilePhone::KMaxApnName> buffer;
    	    	TBufC8<RMobilePhone::KMaxApnName> buffer2;
    	    	buffer.Des().Copy( iAPN->Des() );
    	    	TInt index = buffer.Des().Find(_L8("."));
    	    	
    	    	while (index != KErrNotFound)
        	    	{
       	    	    buffer2.Des().Copy( buffer.Des().Left( index ) );
                    TPtr8 tmp = buffer2.Des();
    			    iApn.iApn.AppendFormat(KFormat, index, &tmp);
    			    buffer.Des().Delete(0,index+1);
    			    index = buffer.Des().Find(_L8("."));
        	    	}
       	        TPtr8 tmp = buffer.Des();
    			iApn.iApn.AppendFormat(KFormat, buffer.Length(), &tmp);               
                }
            else
                {
		        _LIT8(KEmpty8,""); 
		        iApn.iApn.Copy(KEmpty8);                    
                }
            
	        CLOG( ( ESelector, 0, _L( "Size: %d" ), iApn.iApn.Size() ) );
	    	iPhone.AppendAPNName( iStatus, iApnPKG );
	    	
			iRequest = EAddAPN;
			SetActive();
		    }		
		}    
    CLOG( ( ESelector, 0, _L( "<- CAPControlListAPIWrapper::DoAddAPNL" ) ) );
    }

// ---------------------------------------------------------
// CAPControlListAPIWrapper::Visible
// ---------------------------------------------------------
TBool CAPControlListAPIWrapper::Visible() const
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListAPIWrapper::Visible" ) ) );    
    if (!iConnAllowed)
        {
		return EFalse; // phone is in off-line mode            
        }
	TUint32 aCaps;	  
	iPhone.GetIccAccessCaps(aCaps);
	CLOG( ( ESelector, 0, _L( "%d" ),  aCaps) );
	if (aCaps & RMobilePhone::KCapsUSimAccessSupported)
		{		
		CLOG( ( ESelector, 0, _L( 
						"<- CAPControlListAPIWrapper::Visible 2" ) ) );
		return ETrue;
		}
	else
		{			
		CLOG( ( ESelector, 0, _L( 
						"<- CAPControlListAPIWrapper::Visible 3" ) ) );
		return EFalse;
		}
    }
// ---------------------------------------------------------
// CAPControlListAPIWrapper::Enabled
// ---------------------------------------------------------
TBool CAPControlListAPIWrapper::Enabled() const
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListAPIWrapper::Enabled" ) ) );
    CLOG( ( ESelector, 0, _L( "<- CAPControlListAPIWrapper::Enabled" ) ) );
	if ( iAclStatus == RMobilePhone::EAPNControlListServiceEnabled )
		{
		return ETrue;		
		}
	else
		{
		return EFalse;
		}		
    }
	
// ---------------------------------------------------------
// CAPControlListAPIWrapper::ConstructL()
// ---------------------------------------------------------
//
void CAPControlListAPIWrapper::ConstructL()
    {   	
    CLOG( ( ESelector, 0, _L( "-> CAPControlListAPIWrapper::ConstructL" ) ) );
    // init RMobilePhone and RMmCustomAPI
	_LIT (KTsyName,"phonetsy.tsy");
	RTelServer::TPhoneInfo info;
	RPhone::TLineInfo lineInfo;
	User::LeaveIfError( iServer.Connect() );
	User::LeaveIfError( iServer.LoadPhoneModule( KTsyName ) );
	User::LeaveIfError( iServer.GetPhoneInfo( 0, info ) );
	User::LeaveIfError( iPhone.Open(iServer, info.iName) );	  
	User::LeaveIfError( iPhone.Initialise() );
	User::LeaveIfError( iCustomAPI.Open(iPhone) );
	           		
	CRepository* aRepository = CRepository::NewL( KCRUidCoreApplicationUIs );
    aRepository->Get( KCoreAppUIsNetworkConnectionAllowed, iConnAllowed);
    delete aRepository;

    if (!iAPN)
		{
		iAPN = HBufC::NewL( RMobilePhone::KMaxApnName );
		}
    CLOG( ( ESelector, 0, _L( "<- CAPControlListAPIWrapper::ConstructL" ) ) );
    }
    
// ----------------------------------------------------------
// CAPControlListPlugin::NotifyIccAccessCapsChange()
// ----------------------------------------------------------
//
void CAPControlListAPIWrapper::NotifyIccAccessCapsChange(
							TRequestStatus& aReqStatus, TUint32& aCaps)
    {
    CLOG( ( ESelector, 0, 
        _L( "-> CAPControlListAPIWrapper::NotifyIccAccessCapsChange" ) ) );
    iPhone.NotifyIccAccessCapsChange( aReqStatus, aCaps );
    CLOG( ( ESelector, 0, 
        _L( "<- CAPControlListAPIWrapper::NotifyIccAccessCapsChange" ) ) );
    }
// ----------------------------------------------------------
// CAPControlListPlugin::CancelNotify()
// ----------------------------------------------------------
//
void CAPControlListAPIWrapper::CancelNotify()
    {
    CLOG( ( ESelector, 0, 
        _L( "-> CAPControlListAPIWrapper::CancelNotify" ) ) );
    iPhone.CancelAsyncRequest(EMobilePhoneNotifyIccAccessCapsChange);
    CLOG( ( ESelector, 0, 
        _L( "<- CAPControlListAPIWrapper::CancelNotify" ) ) );
    }
// ----------------------------------------------------------
// CAPControlListPlugin::ShowSimCardErrorNoteL()
// ----------------------------------------------------------
//
void CAPControlListAPIWrapper::ShowSimCardErrorNoteL()
    {
    HBufC* text = StringLoader::LoadLC ( 
        R_QTN_ACL_ERR_SIM_CARD );
    CAknErrorNote* note = new ( ELeave ) CAknErrorNote(
         ETrue );
    note->ExecuteLD( *text );

    CleanupStack::PopAndDestroy( text );
    }
// -----------------------------------------------------------------------------
// Handles the leave from the RunL()
// -----------------------------------------------------------------------------
//
TInt CAPControlListAPIWrapper::RunError( TInt /*aLeaveCode*/ )
    {
    iSimCardError = ETrue;
    iRequest = ENoRequest;  

    return KErrNone;
    }


