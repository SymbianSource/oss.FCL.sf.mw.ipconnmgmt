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

#ifndef __DEVICEDIALOGOBSERVER_H__
#define __DEVICEDIALOGOBSERVER_H__

#include "CellularDataConfirmation.h"

NONSHARABLE_CLASS ( CDeviceDialogObserver ) : public CBase, public MHbDeviceDialogObserver
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CDeviceDialogObserver* NewL( CCellularDataConfirmation* aNotifier );
        
        /**
        * Destructor
        */
        ~CDeviceDialogObserver();
        
        /**
        * Handles the user input received from the dialog
        * @param    aData CHbSymbianVariantMap&
        */
        void DataReceived(CHbSymbianVariantMap& aData);
        
        /**
        * Handles the closing of the dialog
        * @param    aCompletionCode TInt
        */
        void DeviceDialogClosed(TInt /*aCompletionCode*/);
        /**
        * Sets the available choices for the query into the class variable
        * @param    aChoices RArray<TInt>
        */
        void SetChoices( RArray<TMsgQueryLinkedResults>& aChoices );
    private:
        /**
        * Constructor
        */
        CDeviceDialogObserver( CCellularDataConfirmation* aNotifier );
        
    private:
        /* Pointer to the object that triggered the dialog opening,
         * needed to update the information about the user input.
         */
        CCellularDataConfirmation* iNotifier;
        /* The order of the buttons in the dialog, in order to map the
         * data received from the dialog into the actual selected choice.
         */
        RArray<TMsgQueryLinkedResults> iChoiceIds;
    };

#endif /* __DEVICEDIALOGOBSERVER_H__ */
