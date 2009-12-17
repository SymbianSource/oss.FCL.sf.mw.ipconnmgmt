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
* Description:  Declaration of CNoWLANNetworksAvailableNotif
*
*/



#ifndef __NO_WLAN_NETWORKS_AVAILABLE_NOTE_H__
#define __NO_WLAN_NETWORKS_AVAILABLE_NOTE_H__

// INCLUDES
#include "ConnectionDialogsNotifBase.h"

//FORWARD DECLARATION
class CActiveWLANNetworkUnavailableNote;

/**
 * CNoWLANNetworksAvailableNotif class
 */
NONSHARABLE_CLASS( CNoWLANNetworksAvailableNotif ) 
                                        : public CConnectionDialogsNotifBase
    {
    public:
        /**
        * NewL function
        * @param  -
        * return CNoWLANNetworksAvailableNotif*
        */
        static CNoWLANNetworksAvailableNotif* NewL( 
                                        const TBool aResourceFileResponsible );
        
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
    
        /**
        * CompleteL the notifier is complete
        * @param  aStatus status
        * return  -
        */
        void CompleteL( TInt aStatus );
                     
    protected:    
        /**
        * Constructor
        */
        CNoWLANNetworksAvailableNotif();    
    
    protected:
        // pointer to active object, owned
        CActiveWLANNetworkUnavailableNote* iActiveNote;

    };    

#endif //__NO_WLAN_NETWORKS_AVAILABLE_NOTE_H__

// End of File
    
