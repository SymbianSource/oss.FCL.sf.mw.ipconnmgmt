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
* Description:  Declaration of Active SelectConnection 
*
*/



#ifndef __ACTIVESELECTCONNECTIONPLUGIN_H__
#define __ACTIVESELECTCONNECTIONPLUGIN_H__


// INCLUDES
#include <e32base.h>
#include <agentdialog.h>
#include <cmmanagerext.h>
#include <rmpm.h>
#include "ActiveBase.h"


// FORWARD DECLARATION
class CSelectConnectionDialog;
class CConnectionInfoArray;
class MEikSrvNotifierBase2;
class CGulIcon;
class TSortSnapUtils;

// CLASS DECLARATION

/**
 * CActiveSelectConnectionPlugin class,
 * an active object for selection plugin
 */     
NONSHARABLE_CLASS( CActiveSelectConnectionPlugin ) : public CActiveBase
    {
    public:
        /**
        * NewL function
        * @param  aPlugin a pointer to notifier plugin
        * @param aIAPId iap id
        * @param aPrefs preferences pointer    
        */
        static CActiveSelectConnectionPlugin* NewL( 
                                                MEikSrvNotifierBase2* aPlugin,
                                                const TUint32 aElementId,
                                                TUint32 aBearerSet );
    
        /**
        * Setactive and starts active object
        */    
        void ShowSelectConnectionL();

        /**
        * Destructor
        */    
        virtual ~CActiveSelectConnectionPlugin();
    
        /**
        * Cancels the active object
        */
        void Cancel();

        /**
        * Sets the preferred iap into db.
        * calls iActivePlugin    
        * @param aIAPId id of the preferred iap
        * @param aDestinationId id of the preferred Destination
        */
        TUint32 GetElementIDL( TUint32 aIAPId, TUint32 aDestinationId );


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
        CActiveSelectConnectionPlugin( MEikSrvNotifierBase2* aPlugin,
                                       const TUint32 aElementId,
                                       TUint32 aBearerSet );
    
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

        void AddSearchForWLanItemL();

        void AddDestinationItemsL();
        void AddDestinationItemL( RCmDestinationExt aDestination, 
                                  TUint32 aDestId, TBool aFirstItem, 
                                  TBool aDefaultDest,
                                  TSortSnapUtils& aSortSnapUtils );

        void AddUncategorizedItemL( TSortSnapUtils& aSortSnapUtils );
        
        /**
        * Gives back, refresh interval of scan for network
        */
        TInt GetRefreshIntervalL();
        
        /**
        * Counts the number of ad-hoc WLAN iaps
        */
        TInt CountAdhocWlanIapsL();

    protected:
        // contains all iaps from db, owned
        CConnectionInfoArray *iItems;   

        // Selected preferences
        TUint32 iBearerSet;

        // Requested bearer set preference
        TUint32 iReqBearerSet;
        
        // Selected preferences
        TUint32 iElementId;

        // a pointer to notifer, not owned
        MEikSrvNotifierBase2* iPlugin;
    
        ///< Pointer to the dialog, not owned 
        CSelectConnectionDialog* iDialogConn;
    
        TBool iIsWLANFeatureSupported;

        TInt iRefreshInterval;

        TInt iHighlightedItem;

        CArrayPtr< CGulIcon >* iIcons;

        RMPM iMPM;

        RCmManagerExt iCmManagerExt;

        TUint iDefaultDestId;

        TUint iDefaultCMId;
    };     


#endif

// End of File
