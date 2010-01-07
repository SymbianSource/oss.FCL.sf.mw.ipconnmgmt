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
 * Description:  Declaration of CNoWlansDiscreetPopup
 *
 */

#ifndef CNOWLANSDISCREETPOPUP_H
#define CNOWLANSDISCREETPOPUP_H

// INCLUDES
#include <e32base.h>	// For CActive, link against: euser.lib

#include "ConnectionDialogsNotifBase.h"
 
/**
 * CConnectingViaDiscreetPopup class
 */
NONSHARABLE_CLASS( CNoWlansDiscreetPopup ) : public CActive
    {
public:
    /**
     * Destructor
     * Cancel and destroy
     */
    virtual ~CNoWlansDiscreetPopup();

    /**
     * NewL function 
     * Two-phased constructor.
     * @param  aNotif a pointer to notifier plugin
     */
    static CNoWlansDiscreetPopup* NewL(
            CConnectionDialogsNotifBase* aNotif );

    /**
     * NewL function 
     * Two-phased constructor.
     * @param  aNotif a pointer to notifier plugin
     */
    static CNoWlansDiscreetPopup* NewLC(
            CConnectionDialogsNotifBase* aNotif );

public:
    /**
     * StartL Function for making the initial request
     */
    void StartL();

private:
    /**
     * C++ constructor
     */
    CNoWlansDiscreetPopup( CConnectionDialogsNotifBase* aNotif );

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
    
private:
    // Used for complete runl, not owned 
    TRequestStatus* iClientStatus;

    // Pointer to notifer, not owned
    CConnectionDialogsNotifBase* iNotif;
    };

#endif // CNOWLANSDISCREETPOPUP_H
