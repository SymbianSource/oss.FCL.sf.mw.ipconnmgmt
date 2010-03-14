/*
 * Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:  Declaration of CConnectingViaDiscreetPopup
 *
 */

#ifndef CCONNECTINGVIADISCREETPOPUP_H
#define CCONNECTINGVIADISCREETPOPUP_H

// INCLUDES
#include <e32base.h>	// For CActive, link against: euser.lib
#include <cmmanager.h>

#include "connectingviadiscreetpopupnotif.h"

/**
 * CConnectingViaDiscreetPopup class
 */
NONSHARABLE_CLASS( CConnectingViaDiscreetPopup ) : public CActive
    {
public:
    /**
     * Destructor
     * Cancel and destroy
     */
    ~CConnectingViaDiscreetPopup();

    /**
     * NewL function 
     * Two-phased constructor.
     * @param  aNotif a pointer to notifier plugin
     */
    static CConnectingViaDiscreetPopup* NewL(
            CConnectingViaDiscreetPopupNotif* aNotif);

    /**
     * NewL function 
     * Two-phased constructor.
     * @param  aNotif a pointer to notifier plugin
     */
    static CConnectingViaDiscreetPopup* NewLC(
            CConnectingViaDiscreetPopupNotif* aNotif);

public:
    /**
     * StartL Function for making the initial request
     */
    void StartL(TUint32 aIapId );

private:
    /**
     * C++ constructor
     */
    CConnectingViaDiscreetPopup(CConnectingViaDiscreetPopupNotif* aNotif);

    /**
     * Second-phase constructor
     */
    void ConstructL();

private:
    /**
     * From CActive
     */
    void RunL();

    /**
     * From CActive 
     */
    void DoCancel();

    /**
     * From CActive 
     */
    TInt RunError(TInt aError);

private:

    // Used for complete runl, not owned 
    TRequestStatus* iClientStatus;

    // Pointer to notifer, not owned
    CConnectingViaDiscreetPopupNotif* iNotif;

    // Handle to cmmanager interface
    RCmManager icmManager;

    // Bearer type from cmmanager
    TUint iBearerType;

    // Connection name - owned
    HBufC* iConnectionName;

    // Popup flags
    TInt iPopupFlags;

    };

#endif // CCONNECTINGVIADISCREETPOPUP_H
