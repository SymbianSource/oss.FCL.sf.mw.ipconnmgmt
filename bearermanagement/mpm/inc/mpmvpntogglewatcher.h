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
* Description: Listen VPN toggle key changes in central repository.
*
*/

#ifndef MPMVPNTOGGLEWATCHER_H
#define MPMVPNTOGGLEWATCHER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CRepository;

// Stop after this many consecutive central repository errors.
const TInt KMpmVpnToggleWatcherCenRepErrorThreshold = 80;

/**
* Class that specifies the functions for handling notify messages about
* VPN toggle key changes in central repository.
*/
class MMpmVpnToggleWatcherNotify
{
public:
    
    /**
     * Sets values for VPN toggle after VPN toggle key changes in central
     * repository.
     * @param aVpnPreferred Informs if VPN connection is preferred
     *                      connection
     * @param aVpnIapId VPN IAP Id, which is used for VPN connection, when
     *                  VPN connection is preferred                           
     * @param aSnapId SNAP Id SNAP Id, which is used for VPN connection,
     *                when VPN connection is preferred
     */
     virtual void SetVpnToggleValuesL( const TBool aVpnPreferred,
                                       const TUint32 aVpnIapId,
                                       const TUint32 aSnapId ) = 0;
};


/**
 *  Class for monitoring VPN toggle key changes in central repository.
 *  Follows KMpmVpnToggleCenRepUid key in central repository.
 */
class CMpmVpnToggleWatcher : public CActive
    {
public:

    /**
    * Two-phased constructor.
    * @param aNotify Handler for notify messages.
    */    
    static CMpmVpnToggleWatcher* NewL( MMpmVpnToggleWatcherNotify& aNotify );

    /**
    * Destructor.
    */
    virtual ~CMpmVpnToggleWatcher();
    
    /**
     * Call this when you want to start listen event.
     */
    void StartL();    
        
    /**
     * Informs if VPN connection is preferred.
     * @return True if VPN connection is preferred.
     */
    TBool IsVpnConnectionPreferred() const;
    
    /**
     * Returns VPN IAP Id, which is used for VPN connection, when VPN
     * connection is preferred.
     * @return VPN IAP Id
     */
    TUint32 VpnIapId() const;
    
    /**
     * Returns SNAP Id, which is used for VPN connection, when VPN
     * connection is preferred.
     * @return SNAP Id
     */
    TUint32 SnapId() const;    
    
    /**
     * Resets VPN toggle values.
     */    
    void ResetVpnToggleValues();
    
private:

    CMpmVpnToggleWatcher( MMpmVpnToggleWatcherNotify& aNotify );

    void ConstructL();
    
    /**
     * Request for notifications.
     * @return Error value
     */
    TInt RequestNotifications();
    
    /**
     * Gets VPN toggle values.
     * @return Error value
     */
    TInt GetVpnToggleValues();

// from base class CActive

    void RunL();

    void DoCancel();
    
private: // data
    
    /**
     * Central repository handle
     * Own.
     */
    CRepository* iRepository;    
    
    /**
     * Informs if VPN connection is preferred connection
     * Own.
     */
    TBool iVpnConnectionPreferred;
    
    /**
     * VPN IAP Id which is used, when VPN connection is preferred
     * Own.
     */
    TUint32 iVpnIapId;
    
    /**
     * SNAP Id which is used, when VPN connection is preferred
     * Own.
     */
    TUint32 iSnapId;
    
    /**
     * Error counter.
     */
    TUint iErrorCounter;

    /**
     * Handler for notify messages.
     * Not own.
     */
    MMpmVpnToggleWatcherNotify& iNotify;
    
    };

#endif // MPMVPNTOGGLEWATCHER_H
