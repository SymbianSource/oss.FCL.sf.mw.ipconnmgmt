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

#ifndef __CELLULARDATACONFIRMATION_H__
#define __CELLULARDATACONFIRMATION_H__

#include <e32base.h>
#include <ConnectionUiUtilities.h>
#include <hb/hbcore/hbdevicedialogsymbian.h>
#include "ConnectionUiUtilitiesCommon.h"

class CDeviceDialogObserver;


NONSHARABLE_CLASS ( CCellularDataConfirmation ) : public CBase
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CCellularDataConfirmation* NewL( );
        
        /**
        * Destructor
        */
        ~CCellularDataConfirmation();
        
        /**
        * Start the Notifier
        * @param aResult Result of user selection
        * @param aHomeNetwork Home or foreign network.
        * @param aStatus Status object of notifier
        */
        void StartL( TMsgQueryLinkedResults& aResult,
                const TBool aHomeNetwork,
                TRequestStatus& aStatus );
        
        /**
        * Cancel() the notifier
        * @param  -
        * return -
        */
        void Cancel();
        
        /**
        * CompleteL the notifier is complete
        * @param  aStatus status
        * return  -
        */
        void CompleteL( TInt aStatus );
        /**
        * Sets the selected choice of the presented dialog
        * @param  aChoice TInt 
        * return  -
        */
        void SetSelectedChoice( TInt aChoice );
       
    private:
        /**
        * Constructor
        */
        CCellularDataConfirmation();
    
        /**
         * Second-phase constructor
         */
        void ConstructL();

        
    private:
        /** Pointer to the device dialog interface for handling the dialog */
        CHbDeviceDialogSymbian* iDialog;
        /** The observer to handle the data received from the orbit dialog */
        CDeviceDialogObserver* iObserver;

        /* Information if request was already completed, in case the
         * observer receives the data signal and the signal about closing the
         * dialog.
         */
        TBool iCompleted;
        
        /** Request status. Not owned. */
        TRequestStatus* iStatus;
        
        /** Result of query. Not owned. */
        TMsgQueryLinkedResults* iResult;
    
    };

#endif //__CELLULARDATACONFIRMATION_H__
