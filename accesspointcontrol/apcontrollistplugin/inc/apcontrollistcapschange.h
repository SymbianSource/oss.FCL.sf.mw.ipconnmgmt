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
* Description:  Class for asyncron API call NotifyIccAccessCapsChange.
*
*/

#ifndef APCONTROLLISTCAPSCHANGE_H
#define APCONTROLLISTCAPSCHANGE_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CAPControlListPlugin;

// CLASS DECLARATION
/**
*  Wrapper class for asyncron syncron API call NotifyIccAccessCapsChange.
*/
class CAPControlListCapsChange : public CActive
	{
	public: 
    	// Constructor and destructor
    	    			
        /**
        * Constructor.
        * @param aPlugin reference to the view
        * @param aPriority priority for CActive
        */
		CAPControlListCapsChange(CAPControlListPlugin& aPlugin,
									TInt aPriority = EPriorityStandard );
        /**
        * Destructor.
        */
		~CAPControlListCapsChange();
		
        // New functions     
        /**
        * Makes asyncron API call RMobilePhone::NotifyIccAccessCapsChange
        */
		void NotifyCapsChange();
		 
				
        // Functions from CActive
        /**
        * Called by Cancel() 
        */
		void DoCancel();		
        /**
        * Handles asynchronous function completion 
        */
		void RunL();
		
	private:
        //@var reference to the view
		CAPControlListPlugin&	iPlugin;
		
        //@var Caps set by NotifyIccAccessCapsChange
		TUint32 				iCaps;
	} ;

#endif

// End of file
