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
* Description:
* Listens for changes in dial-up PDP context override P&S-key.
*
*/

#ifndef CONNMONDIALUPOVERRIDENOTIFIER_H
#define CONNMONDIALUPOVERRIDENOTIFIER_H

class CConnMonServer;

/**
 * Stop requesting new notifications after this many consecutive errors.
 */
const TInt KConnMonPsEventErrorThreshold = 80;

/**
 * Maximum time that dial-up PDP context override is allowed to stay on.
 */
const TUint KConnMonDialUpOverrideInterval = 45000000; // 45sec

/**
 * Read policy for dial-up PDP context override key.
 */
_LIT_SECURITY_POLICY_PASS( KDialUpOverrideReadC0 );

/**
 * Write policy for dial-up PDP context override key.
 */
_LIT_SECURITY_POLICY_C2( KDialUpOverrideWriteC2, ECapabilityNetworkControl, ECapabilityNetworkServices );

/**
 * An active object to listen for changes in the dial-up PDP context override
 * P&S-key. Only used if the feature is enabled by the CenRep key
 * KDialUpOverride in CmManager repository (KCRUidCmManager).
 */
NONSHARABLE_CLASS( CConnMonDialUpOverrideNotifier ) : public CActive
    {
public:
    static CConnMonDialUpOverrideNotifier* NewL( CConnMonServer* aServer );
    static CConnMonDialUpOverrideNotifier* NewLC( CConnMonServer* aServer );
    virtual ~CConnMonDialUpOverrideNotifier();

    /**
     * Set the KDialUpConnection-property in Connection Monitor RProperty
     * category to the initial state of EConnMonReady.
     */
    TInt ResetStatus();

private:
    CConnMonDialUpOverrideNotifier( CConnMonServer* aServer );
    void ConstructL();
    void RequestNotifications();

private: // Methods from base class
    void DoCancel();
    void RunL();

private:
    // Pointer back to ConnMon server class.
    CConnMonServer* iServer;

    // Counter for notification errors.
    TInt iErrorCounter;

    // Dial-up PDP context override property.
    // Used to signal an incoming dial-up connection and the completion of
    // preparatory actions by ConnMon (Closing down internal PDP contexts).
    RProperty iDialUpProperty;
    };

#endif // CONNMONDIALUPOVERRIDENOTIFIER_H

// End of file
