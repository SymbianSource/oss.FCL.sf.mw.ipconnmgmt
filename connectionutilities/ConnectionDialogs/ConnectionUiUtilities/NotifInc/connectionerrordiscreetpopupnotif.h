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
 * Description:  Declaration of Generic Connection Ui Utilities Notifier
 *
 */

#ifndef __CONNECTIONERRORDISCREETPOPUPNOTIF_H__
#define __CONNECTIONERRORDISCREETPOPUPNOTIF_H__

// INCLUDES
#include "ConnectionDialogsNotifBase.h"

//FORWARD DECLARATION
class CConnectionErrorDiscreetPopup;

/**
 * CConnectionErrorDiscreetPopupNotif class
 */
NONSHARABLE_CLASS( CConnectionErrorDiscreetPopupNotif ) : 
public CConnectionDialogsNotifBase
    {
public:
    /**
     * NewL function
     * @param  -
     * return CConnectionErrorDiscreetPopupNotif*
     */
    static CConnectionErrorDiscreetPopupNotif* NewL(
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
    CConnectionErrorDiscreetPopupNotif();

protected:

    // pointer to active object, owned
    CConnectionErrorDiscreetPopup* iActiveNote;

    };

#endif //__CONNECTINGVIADISCREETPOPUPNOTIF_H__
// End of File

