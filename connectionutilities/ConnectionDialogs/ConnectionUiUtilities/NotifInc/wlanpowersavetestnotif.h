/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Notifier part for WLAN Power Save Testing notification dialog
*
*/

#ifndef __POWERSAVETESTQUERYNOTIF_H__
#define __POWERSAVETESTQUERYNOTIF_H__


// INCLUDES
#include <e32base.h>

#include "ConnectionDialogsNotifBase.h"

// FORWARD DECLARATIONS
class CWlanPowerSaveQuery;

/**
 * OfflineWlanNoteNotif class
 */
NONSHARABLE_CLASS( CWlanPowerSaveQueryNotif ) 
                                        : public CConnectionDialogsNotifBase
    {
    public:
    
        /**
        * NewL function
        * @param  -
        * return CWlanPowerSaveQueryNotif*
        */
        static CWlanPowerSaveQueryNotif* NewL( 
                                        const TBool aResourceFileResponsible );
                                        
        /**
        * ~CWlanPowerSaveQueryNotif() destructor
        * @param  -
        * return  -
        */
        virtual ~CWlanPowerSaveQueryNotif();
                                

        /**
        * RegisterL register the client notifier function
        * @param  -
        * return TNotifierInfo
        */
        TNotifierInfo RegisterL();

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
        void StartL( const TDesC8&       aBuffer, 
                     TInt                aReplySlot, 
                     const RMessagePtr2& aMessage );

        /**
        * Cancel() the notifier
        * @param  -
        * return -
        */
        void Cancel();

    public:

        /**
        * CompleteL the notifier is complete
        * @param  aStatus status
        * return  -
        */
        void CompleteL( TInt aStatus );

         /**
        * Callback is called when user selects dialog's link
        * @param  aObject Pointer to notifier object.
        * return  Error code
        */  
        static TInt DisableTestLinkL( TAny* aObject );
        
    private:        

        /**
        * Constructor.
        */
        CWlanPowerSaveQueryNotif();
        
        /**
        * Exits the dialog.
        */
        void DoDisableTestLinkL();
        
    private:
        CWlanPowerSaveQuery* iDialog;    // Pointer to the dialog
        TBool                iDisable;   // Disable notification in the future
        CAsyncCallBack*      iExitAsync; // One shot active object
    };


#endif // __POWERSAVETESTQUERYNOTIF_H__

// End of File
