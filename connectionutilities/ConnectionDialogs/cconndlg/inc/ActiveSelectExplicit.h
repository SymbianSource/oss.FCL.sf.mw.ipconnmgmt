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



#ifndef __ACTIVESELECTEXPLICIT_H__
#define __ACTIVESELECTEXPLICIT_H__


// INCLUDES
#include "ConnDlgPlugin.h"
#include "ActiveIAPListing.h"
#include "SelectConnectionDialog.h"
#include "ActiveBase.h"
#include <cmmanagerext.h>
#include <rmpm.h>


// FORWARD DECLARATION
class CSelectExplicitDialog;


// CLASS DECLARATION

/**
 * CActiveSelectExplicit class,
 * an active object for SelectExplicit
 */     
NONSHARABLE_CLASS( CActiveSelectExplicit ) : public CActiveBase
    {
    public:
        /**
        * NewL function
        * @param  aPlugin a pointer to notifier plugin
        * @param aIAPId iap id
        * @param aPrefs preferences pointer    
        */
        static CActiveSelectExplicit* NewL( 
                                        CSelectConnectionDialog* aCallerDialog,
                                        TUint aDestId, 
                                        TInt aRefreshInterval,
                                        TUint32 aBearerSet, 
                                        TBool aIsWLANFeatureSupported, 
                                        TUint aDefaultCMId );

        /**
        * Setactive and starts active object
        */    
        void ShowSelectExplicitL();

        /**
        * Destructor
        */    
        virtual ~CActiveSelectExplicit();
    
        /**
        * Cancels the active object
        */
        void Cancel();

        /**
        * Gives back the id of preferred iap.
        * @return the id of iap
        */
        TUint32 GetPreferredIapIdL();
    
    
        /**
        * Sets the preferred iap into db.
        * @param aIAPId id of the preferred iap
        */
        void SetPreferredIapIdL( TUint32 aIAPId );

        
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
        CActiveSelectExplicit( CSelectConnectionDialog* aCallerDialog,
                               TUint aDestId, 
                               TInt aRefreshInterval,
                               TUint32 aBearerSet, 
                               TBool aIsWLANFeatureSupported, 
                               TUint aDefaultCMId );


        /**
        * DoCancel from CActive
        */    
        virtual void DoCancel();

        /**
        * RunL from CActive
        */
        virtual void RunL();

        /**
        * BuildConnectionListL
        * @param    -
        */
        void BuildConnectionListL();

        void AddConnectionMethodItemL( TUint32 aId,
                                       TBool aCheckForEasyWlan, 
                                       TBool& aEasyWlanIsInDb, 
                                       TBool aDefaultCM );
                                       
        void AddAdhocWlanIapsL();                               


    protected:
        // contains all iaps from db, owned
        CConnectionInfoArray *iItems;   

        CSelectConnectionDialog* iCallerDialog;

        TUint iDestId;

        ///< Pointer to the dialog, not owned 
        CSelectExplicitDialog* iDialog;
    
        TInt iRefreshInterval;

        CArrayPtr< CGulIcon >* iIcons;

        RMPM iMPM;
        RCmManagerExt iCmManagerExt;

        TUint32 iBearerSet;
        TBool iIsWLANFeatureSupported;
        TUint iDefaultCMId;
    };     


#endif

// End of File
