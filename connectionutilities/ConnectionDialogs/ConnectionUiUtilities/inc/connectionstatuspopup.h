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
* Description:  Declaration of CConnectionStatusPopup
*
*/
#ifndef __CCONNECTIONSTATUSPOPUP_H_
#define __CCONNECTIONSTATUSPOPUP_H_

// INCLUDES
#include <e32base.h>
#include "HbDeviceNotificationDialogSymbian.h"

/**
 * CConnectionStatusPopup
 * Handler for connection status popups
 */
NONSHARABLE_CLASS( CConnectionStatusPopup ) : public CBase, 
    public MHbDeviceNotificationDialogObserver
    {
public: // Constructors and destructor

    /**
    * Two-phased constructor. Leaves on failure.
    * @return The constructed CConnectionStatusPopup object.
    */
    static CConnectionStatusPopup* NewL();
    
    /**
     * Destructor
     */
    virtual ~CConnectionStatusPopup();

public:
    
    /**
     * Shows "Connecting" discreet popup.
     */
    void ConnectingViaDiscreetPopup( );

    /**
     * Shows "Connecting via '%U'" discreet popup.
     */
    void ConnectingViaDiscreetPopup( TUint32 aIAPId, 
            TBool aConnectionAlreadyActive );
    /**
     * Cancels the "Connecting via '%U'" discreet popup.
     */
    void CancelConnectingViaDiscreetPopup();

    /**
     * Shows connection error discreet popup.
     */
    void ConnectionErrorDiscreetPopup( TInt aErrorCode );
    
public: // MHbDeviceNotificationDialogObserver

    void NotificationDialogActivated(
            const CHbDeviceNotificationDialogSymbian* aDialog );
    
    void NotificationDialogClosed(
            const CHbDeviceNotificationDialogSymbian* aDialog,
            TInt aCompletionCode );
    
private:
    
    /**
     * Defines long enough timeout for the popup to be considered shown
     * "forever".
     */
    const static TInt KForeverTimeout = 3*60*1000*1000;

    /**
     * The state of the popup
     */
    enum TPopupState
        {
        /**
         *  Not shown
         */
        EPopupClosed,
        /**
         *  "Connecting" one-liner
         */
        EPopupConnecting,
        /**
         *  "Connecting" with iap name on second row
         */
        EPopupConnectingIap,
        /**
         *  "Connection failed/error"
         */
        EPopupError
        };
    
    /**
     * Constructor
     */
    CConnectionStatusPopup();

    /**
     * Second-phase constructor
     */
    void ConstructL();
    
    /**
     * Sets the new state for the popup.
     */
    void SetState( TPopupState aNewState );

    /**
     * Formats iap info for Connecting via -popup.
     */
    void FormatIapInfoL( TUint32 aIapId );

    /**
     * Prepares popup for error code.
     * @return ETrue when popup should be shown.
     */
    TBool ResolveErrorCodeL( TInt aErrorCode );

private:

    // Popup dialog
    CHbDeviceNotificationDialogSymbian* iPopup;
    
    // The state of the popup
    TPopupState iPopupState;
    };

#endif /* __CCONNECTIONSTATUSPOPUP_H_ */
