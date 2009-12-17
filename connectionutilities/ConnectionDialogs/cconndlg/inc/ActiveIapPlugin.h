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
* Description:  Declaration of Active Iap 
*
*/



#ifndef __ACTIVEIAPPLUGIN_H__
#define __ACTIVEIAPPLUGIN_H__


// INCLUDES
#include "ConnDlgPlugin.h"
#include "ActiveIAPListing.h"


// FORWARD DECLARATION
class CIapDialog;


// CLASS DECLARATION

/**
 * CActiveCConnDlgIapPlugin class,
 * an active object for CConnDlgIapPlugin
 */     
NONSHARABLE_CLASS( CActiveCConnDlgIapPlugin ) : public CActiveIAPListing
    {
    public:
        /**
        * NewL function
        * @param  aPlugin a pointer to notifier plugin
        * @param aIAPId iap id
        * @param aPrefs preferences pointer    
        */
        static CActiveCConnDlgIapPlugin* NewL( CConnDlgIapPlugin* aPlugin,                            
                                           const TConnectionPrefs& aPrefs );
    
        /**
        * StartSearchIAPsL setactive and starts active object
        */    
        void StartSearchIAPsL();

        /**
        * Destructor
        */    
        virtual ~CActiveCConnDlgIapPlugin();
    
        /**
        * Cancels the active object
        */
        void Cancel();

              
    protected:
        /**
        * ConstructL 
        */
        void ConstructL();
    
        /**
        * Constructor
        * @param  aPlugin a pointer to notifier plugin    
        * @param aIAPId iap id
        * @param aPrefs preferences pointer        
        */    
        CActiveCConnDlgIapPlugin( CConnDlgIapPlugin* aPlugin,
                                  const TConnectionPrefs& aPrefs );
    
        /**
        * DoCancel from CActive
        */    
        virtual void DoCancel();

        /**
        * RunL from CActive
        */
        virtual void RunL();

        /**
        * Gives back, refresh interval of scan for network
        */
        TInt GetRefreshInterval();


    protected:
    
        // a pointer to notifer, not owned
        CConnDlgIapPlugin* iPlugin;
    
        ///< Pointer to the dialog, not owned 
        CIapDialog* iDialog;
    
        TBool isDialogDeleting; 
     
    };     


#endif

// End of File
