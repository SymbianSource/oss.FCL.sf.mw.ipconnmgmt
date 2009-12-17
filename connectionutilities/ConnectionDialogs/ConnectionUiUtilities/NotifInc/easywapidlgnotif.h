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
* Description:  Notifier part for WAPI pre-shared key query dialog
*
*/



#ifndef __EASY_WAPI_DLG_NOTIF_H__
#define __EASY_WAPI_DLG_NOTIF_H__


// INCLUDES
#include "ConnectionDialogsNotifBase.h"
#include "WepWpaQueryDlg.h"
#include "ConnectionUiUtilitiesCommon.h"

/**
 * CEasyWapiDlgNotif class
 */
NONSHARABLE_CLASS( CEasyWapiDlgNotif ) : public CConnectionDialogsNotifBase
    {
public:
    

   /**
    * NewL function
    * @param  -
    * return CEasyWapiDlgNotif*
    */
    static CEasyWapiDlgNotif* NewL( const TBool aResourceFileResponsible );

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
    *                    modifiable descriptor, a TDes8 type, into which data
    *                    can be returned. 
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

private:
 
    // Pointer to the dialog
    CWepWpaQueryDlg*                            iDialog;
    
    // To read/write WAPI PSK from/to client
    TPckgBuf< TBuf< KEasyWapiQueryMaxLength > > iKey;
    
    TBool iHex; // not used
    };

#endif // __EASY_WAPI_DLG_NOTIF_H__

// End of File