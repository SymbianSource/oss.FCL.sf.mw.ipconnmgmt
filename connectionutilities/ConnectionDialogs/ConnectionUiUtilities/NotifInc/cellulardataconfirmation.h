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
#include <hb/hbcore/hbsymbiandevicedialog.h>
#include "ConnectionDialogsNotifBase.h"
#include "ConnectionUiUtilitiesCommon.h"

class CDeviceDialogObserver;


NONSHARABLE_CLASS ( CCellularDataConfirmation ) : public CConnectionDialogsNotifBase
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CCellularDataConfirmation* NewL( 
                                const TBool aResourceFileResponsible );
        
        /**
        * Destructor
        */
        ~CCellularDataConfirmation();
        
        /**
        * Start the Notifier
        * @param  aBuffer    Buffer
        * @param  aReplySlot Identifies which message argument to use for the 
        *                    reply. This message argument will refer to a 
        *                    modifiable descriptor, a TDes8 type, into which
        *                    data can be returned. 
        * @param  aMessage   Message
        * return -
        */
        void StartL( const TDesC8& aBuffer, 
                    TInt aReplySlot, 
                    const RMessagePtr2& aMessage );
        
        /**
        * Cancel() the notifier
        * @param  -
        * return -
        */
        void Cancel();
        
        /**
        * RegisterL register the client notifier function
        * @param  -
        * return TNotifierInfo
        */
        TNotifierInfo RegisterL();
        
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
    
    private:
        /** Pointer to the device dialog interface for handling the dialog */
        CHbDeviceDialog* iDialog;
        /** The observer to handle the data received from the orbit dialog */
        CDeviceDialogObserver* iObserver;
        /* The choice selected by the user from the dialog */
        TMsgQueryLinkedResults iChoice;
        /* Information if request was already completed, in case the
         * observer receives the data signal and the signal about closing the
         * dialog.
         */
        TBool iCompleted;
        
    
    };

#endif //__CELLULARDATACONFIRMATION_H__
