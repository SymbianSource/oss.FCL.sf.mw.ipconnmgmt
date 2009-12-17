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
* Description:  Declaration of CConfirmationQueryNotif
*
*/



#ifndef __CONFIRMATIONQUERYNOTIF_H__
#define __CONFIRMATIONQUERYNOTIF_H__


// INCLUDES
#include "ConnectionDialogsNotifBase.h"
#include "ConnectionUiUtilitiesCommon.h"
#include <etel3rdparty.h>
#include <ConnectionUiUtilities.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CConfirmationQuery;
class CRoamingInfo;

/**
 * OfflineWlanNoteNotif class
 */
NONSHARABLE_CLASS( CConfirmationQueryNotif ) 
                                        : public CConnectionDialogsNotifBase
    {
    public:
    
        /**
        * Constructor.
        */
        CConfirmationQueryNotif();

        /**
        * NewL function
        * @param  -
        * return CConfirmationQueryNotif*
        */
        static CConfirmationQueryNotif* NewL( 
                                        const TBool aResourceFileResponsible );

        /**
         * ~CConfirmationQueryNotif() destructor
         * @param  -
         * return  -
         */
         virtual ~CConfirmationQueryNotif();
                
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
        void StartL( const TDesC8& aBuffer, TInt aReplySlot, 
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
        * Sets the selected choise
        * @param  aChoice TInt 
        * return  -
        */
        void SetSelectedChoiceL ( TInt aChoice );

        void SetVisitorNetwork();
      
    private:
        
        /**
         * Callback is called to launch dialog asynchronously
         * @param  aObject Pointer to notifier object.
         * return  Error code
         */  
        static TInt LaunchDialogL( TAny* aObject );
        
    private:
        TBool iTryNextBest;
        TUint32 iConnMethod;
        CConfirmationQuery* iDialog;   // Pointer to the dialog
        TBool iIsVisitorNetwork;
        TMsgQueryLinkedResults iChoice;
        TConnUiUiDestConnMethodNoteId iNoteInfo;
        CAsyncCallBack *iLaunchDialogAsync; // One shot active object
    };

#endif // __CONFIRMATIONQUERYNOTIF_H__

// End of File
