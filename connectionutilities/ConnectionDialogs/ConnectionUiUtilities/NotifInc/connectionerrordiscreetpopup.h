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
 * Description:  Declaration of CConnectingViaDiscreetPopup
 *
 */

#ifndef CCONNECTIONERRORDISCREETPOPUP_H
#define CCONNECTIONERRORDISCREETPOPUP_H

// INCLUDES
#include <e32base.h>	// For CActive, link against: euser.lib
#include <cmmanager.h>

#include "connectionerrordiscreetpopupnotif.h"
 
/**
 * CConnectingViaDiscreetPopup class
 */
NONSHARABLE_CLASS( CConnectionErrorDiscreetPopup ) : public CActive
    {
public:
    /**
     * Destructor
     * Cancel and destroy
     */
    ~CConnectionErrorDiscreetPopup();

    /**
     * NewL function 
     * Two-phased constructor.
     * @param  aNotif a pointer to notifier plugin
     */
    static CConnectionErrorDiscreetPopup* NewL(
            CConnectionErrorDiscreetPopupNotif* aNotif );

    /**
     * NewL function 
     * Two-phased constructor.
     * @param  aNotif a pointer to notifier plugin
     */
    static CConnectionErrorDiscreetPopup* NewLC(
            CConnectionErrorDiscreetPopupNotif* aNotif );

public:
    /**
     * StartL Function for making the initial request
     */
    void StartL( TUint32 aErrCode );

private:
    /**
     * C++ constructor
     */
    CConnectionErrorDiscreetPopup( CConnectionErrorDiscreetPopupNotif* aNotif );

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
    TInt RunError( TInt aError );
    
    /**
     * Resolves error code saved in iErrorCode.
     * @return EFalse when resolve failed.
     */
    TBool ResolveErrorCode( TInt& aText1, TInt& aText2,
            TInt& aBitmap, TInt& aMask );

private:

    // Used for complete runl, not owned 
    TRequestStatus* iClientStatus;

    // Pointer to notifer, not owned
    CConnectionErrorDiscreetPopupNotif* iNotif;
    
    // Error code to be shown
    TInt iErrorCode;
    
    // Handle to cmmanager interface
    RCmManager icmManager;

    };

#endif // CCONNECTIONERRORDISCREETPOPUP_H
