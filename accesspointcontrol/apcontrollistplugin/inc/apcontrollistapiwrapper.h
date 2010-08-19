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
* Description:  
* 		 Wrapper class for asyncron calls in RMyPhone and RMyCustomAPI.
*
*/

#ifndef APCONTROLLISTAPIWRAPPER_H
#define APCONTROLLISTAPIWRAPPER_H

// INCLUDES
#include <e32base.h>
#include <rmmcustomapi.h>

// FORWARD DECLARATIONS
class CAPControlListPlugin;

// CLASS DECLARATION
/**
*  Wrapper class for asyncron calls in RMyPhone and RMyCustomAPI.
*/
class CAPControlListAPIWrapper : public CActive
	{
	private: 
		// Enum for the states checked in RunL()
		enum TRequestType { EGetACLStatus, EAddAPN, EAddAPNSecurity, 
		                    ERemoveAPN, ERemoveAPNSecurity, EActivateACL, 
							EActivateACLSecurity, EDeactivateACL, 
							EDeactivateACLSecurity, EReadData, 
							EEnumerateAPN, ENoRequest};

    public:  
    	// Constructor and destructor
    	    			
        /**
        * Constructor.
        * @param aPlugin reference to the view
        * @param aPriority priority for CActive
        */
		CAPControlListAPIWrapper(CAPControlListPlugin& aPlugin,
									TInt aPriority = EPriorityStandard );
        /**
        * Destructor.
        */
		~CAPControlListAPIWrapper();
		
        // New functions     
          
        /**
        * Initialises iPhone and iCustomAPI members. 
        * Uses User::WaitForRequest
        * because we need to know the ACL status before Visible() is called.
        */
		void ConstructL();
		
		// Primary functions for the API calls
		// These are the functions for which security check is necessary.
		// If the security was already checked they call the correspondig
		// secondary function. If the security wasn't already checked they
		// call the asyncron security check and let RunL call the correspondig
		// secondary function when the security check is finished.
        /**
        * Add APN to the ACL list
        */
		void AddAPNL();		
        /**
        * Remove APN from the ACL list which is currently selected in the
        * listbox.
        */
		void RemoveAPN();		
        /**
        * Activates the ACL list
        */
		void ActivateACL();
        /**
        * Deactivates the ACL list
        */
		void DeActivateACL();
				
        /**
        * Gets the ACL status then reads the ACL list.
        * If this call is succesfull then RunL calls EnumerateAPN 
        * then it calls DoReadData
        */
		void ReadData();
		
        /**
        * Method for checking if plugin should be visible and used in 
        * the Connection settings.
        * @return ETrue if plugin should be visible.
        * @return EFalse if plugin should not be visible.
        */
		TBool Visible() const;
		
        /**
        * Method for checking if the ACL list is enabled or not. 
        * @return ETrue if the ACL list is enabled.
        * @return EFalse if the ACL list is not enabled.
        */
		TBool Enabled() const;
		
        /**
      	* Calls RMyPhone::NotifyIccAccessCapsChange
        * Used by CAPControlListCapsChange class. This function is nescessary
        * since iPhone is a private member.
        */
		void NotifyIccAccessCapsChange(TRequestStatus& aReqStatus, 
														TUint32& aCaps);				
        /**
      	* Calls RTelSubSessionBase::CancelAsyncRequest
        * Used by CAPControlListCapsChange class. This function is nescessary
        * since iPhone is a private member.
        */
        void CancelNotify(); 
				
        // Functions from CActive
        /**
        * Closes API in case of Cancel() 
        */
		void DoCancel();		
        /**
        * Handles asynchronous function completion 
        */
		void RunL(); 
		/** 
		* Shows SIM card error note in error cases
		*/
		void ShowSimCardErrorNoteL();
		
	private:
        // New functions
        
        /**
		* Called from one of the primary functions for the API calls
        * Calls RMmCustomAPI::CheckSecurityCode asyncron call. This functon is
        * called only once.
        * @param aRequest We need this to set the state so we'll know in RunL
        * which primary function called it.
        */
		void SecurityCheck( TRequestType aRequest ); 
		
		// Secondary functions for the API calls		
        /**
        * Secondary function for AddAPNL().Makes the asyncron call to 
        * add an APN to the ACL list.
        * If the security was already checked it is called from AddAPNL.
        * If the security was not checked it is called from the RunL()
        * when RunL() is called for RMmCustomAPI::CheckSecurityCode()
        */
		void DoAddAPNL();		
        /**
        * Secondary function for RemoveAPN().Makes the asyncron call to 
        * remove an APN from the ACL list.
        * If the security was already checked it is called from AddAPNL.
        * If the security was not checked it is called from the RunL()
        * when RunL() is called for RMmCustomAPI::CheckSecurityCode()
        */
		void DoRemoveAPN();
        /**
        * Secondary function for ActivateACL() and DeActivateACL().Makes the 
        * asyncron call to SetACLStatus either EAclStatusEnabled or
        * EAclStatusDisabled.
        * If the security was already checked it is called from ActivateACL()
        * or DeActivateACL().
        * If the security was not checked it is called from the RunL()
        * when RunL() is called for RMmCustomAPI::CheckSecurityCode()
        * @param aAclStatus the status we want to set.
        */
		void SetACLStatus( const RMobilePhone::TAPNControlListServiceStatus 
		                                        aAPNControlListServiceStatus );
		
        /**
        * Reads the ACL list. 
        * Gets the number of items with an asyncron call. The actual reading
        * of the APNs happens in DoReadData()
        */
		void EnumerateAPN();
		
        /**
        * Reads an APN entry from the ACL list and puts it in the listbox. 
        * Called from RunL() when RunL() after ReadData() was called. It is
        * called iSize times. 
        */
		void DoReadData();

        /**
        * Case EReadData in RunL 
        * Code removed from RunL to make it's size smaller
        */
		void FinishReadDataL();
		
	    /**
	     * From CActive, handles leaves from RunL.
	     * @param aLeaveCode The leave code.
	     */
	    TInt RunError( TInt aLeaveCode );
		
	private: // Data
		
        //@var reference to the view
		CAPControlListPlugin&			iPlugin;
	
		// API objects
        //@var RTelServer object for the ACL API
		RTelServer 						iServer;	
        //@var RMobilePhone object for the ACL API
		RMobilePhone 					iPhone;
        //@var RMmCustomAPI object for the ACL API
		RMmCustomAPI 					iCustomAPI;

        //@var state to store which is the current request
		TRequestType iRequest;
		
        //@var  ACL status
		RMobilePhone::TAPNControlListServiceStatus		iAclStatus;   
		
        //@var phone is online
		TBool 							iConnAllowed;
		
        //@var security checked
		TBool 							iSecurityChecked;
		
        //@var AP Name from the user input ( AddAPN2L() )
		HBufC* 							iAPN;

		//members for reading data ( ReadData() ReadData2() )
        //@var AP Name in the form expected by the API
		RMobilePhone::TAPNEntryV3 		iApn;
        //@var iApn packaged
		RMobilePhone::TAPNEntryV3Pckg 	iApnPKG;
        //@var size of the ACL list
		TUint32 						iSize;
        //@var index of item currently read from the ACL list 
		TUint32 						iIndex;
		
		TBool                           iSimCardError;
	} ;

#endif

// End of file
