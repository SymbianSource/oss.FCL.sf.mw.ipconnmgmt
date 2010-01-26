/*
* Copyright (c) 2004,2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Always-On Settings
*
*/


#ifndef C_CAOSETTINGS_H
#define C_CAOSETTINGS_H

// INCLUDE FILES
#include <e32base.h>
#include <cenrepnotifyhandler.h>

#include "maosettings.h"

// FORWARD DECLARATIONS
class MAOSettingsObserver;
class CRepository;

// CONSTANTS

/**
*  TLingerSetting encapsulates iap id and corresponding.
*  linger timer interval.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.2
*/
class TLingerSetting
    { 
    public: // Data
        TInt iIap;
        TInt iInterval;
    };


/**
*  Settings encapsulates all Always-On related settings.
*  Settings also notifies if Always-On settings are
*  updated OTA (Over The Air).
*  Settings changes are notified through MAOSettingsObserver interface.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
NONSHARABLE_CLASS( CAOSettings ):
    public CActive,
    public MAOSettings,
    public MCenRepNotifyHandlerCallback
    {
public:    // Constructors & destructors

    /**
     * Two-phased constructor.
     *
     * @since S60 v3.1
     * @param aObserver observer for setting changes
     * @return pointer to the created CAOSettings object
     */
    static CAOSettings* NewL( MAOSettingsObserver& aObserver );

    /**
     * Destructor
     *
     * @since S60 v3.1
     */
    virtual ~CAOSettings();
    
public: // New methods

    /**
     * Fetches asynchronously current settings.
     * When settings have been received notification
     * is broadcasted through MAOSettingsObserver interface.
     * If parameter '0' is used then all settings are fetched.
     *
     * @since S60 v3.1
     * @param aId of the required settings
     */
    void FetchSettings( TUint32 aId = 0 );
    
private: // From base class CActive

    /**
     * From CActive
     * Process the completed request.
     *
     * @since S60 v3.1
     */
    void RunL();

    /**
     * From CActive
     * Cancel outstanding request
     *
     * @since S60 v3.1
     */
    void DoCancel();

    /**
     * From CActive
     * Handle a leave occurred in the RunL().
     * Note: If RunError function pointer is not given in construction,
     * the leave error is to be propagated back to the active scheduler!
     *
     * @since S60 v3.1
     * @param aError the error code of leave
     */
    TInt RunError( TInt aError );

public: // From base class MAOSettings
        // Allow Always-On Server access settings
    
    /**
     * From MAOSettings
     * Gets current acces point id.
     *
     * @since S60 v3.1
     */
    TInt AccessPointId() const;

    /**
     * From MAOSettings
     * Gets current retry timer value in seconds.
     *
     * @since S60 v3.1
     */
    TInt RetryTimerValue() const;
            
    /**
     * From MAOSettings
     * Gets Always-On support in HPLMN (home network).
     *
     * @since S60 v3.1
     * @return ETrue if supported
     */
    TBool AlwaysOnSupportedInHPLMN() const;
    
    /**
     * From MAOSettings
     * Gets Always-On support in VPLMN (visitor network).
     *
     * @since S60 v3.1
     * @return ETrue if supported.
     */
    TBool AlwaysOnSupportedInVPLMN() const;
    
    /**
     * From MAOSettings
     * Gets current connection timer value in seconds.
     *
     * @since S60 v3.1
     * @return TInt the connection timer value in seconds.
     */
    TInt ConnectionTimerValue() const;
        
    /**
     * Gets current unconnect timer value in seconds.
     *
     * @since 3.0
     * @return TInt
     */
    TInt UnconnectTimerValue() const;
    
     /**
     * From MAOSettings
     * Gets current linger timer value in seconds.
     *
     * @param aIapId Iap id of the connection.
     * @since S60 v3.2
     */
    TInt LingerTimerValue( const TUint aIapId ) const;
    
    /**
     * Checks whether user allows cellular connections
     * or not.
     *
     * @since 5.1
     * @return ETrue if cellular connections are allowed,
     *         EFalse otherwise.
     */
    TBool IsCellularAllowedByUser() const;
    
private: // From base class MAOSettings
    
    /** 
     * From MAOSettings
     * This callback method is used to notify the client about
     * changes in keys when the whole repository is listened for.
     *
     * Note: It is not guaranteed that a notification will be received
     *       for all keys, if multiple keys are changed in rapid succession
     *       by multiple threads or when the whole repository is reset,
     *       therefore only listen for whole repository if this is not an 
     *       issue.
     *
     * @param aId Id of the key that has changed.If multiple keys were changed
     *        by whole repository reset, value will be KInvalidNotificationId.
     */
    void HandleNotifyGeneric( TUint32 aId );
    
private: // New methods

    /**
     * Constructor.
     *
     * @since S60 v3.1
     * @param aObserver observer for setting changes
     */
    CAOSettings( MAOSettingsObserver& aObserver );

    /**
     * Default Symbian 2nd-phase constructor
     *
     * @since S60 v3.1
     */
    void ConstructL();

private: // New methods

    /**
     * Gets current settings
     * 0 stands for all
     *
     * @since S60 v3.1
     * @param
     */
    void SetupSettings( TUint32 aId = 0 );
    
    /**
     * Completes self
     *
     * @since S60 v3.1
     * @param aStatus TRequestStatus to coplete
     * @param aReason reason of the completion
     */
    void CompleteSelf( TRequestStatus& aStatus, TInt aReason );
    
    /**
     * Update IAP setting.
     * If new setting cannot be retrieved old remains
     *
     * @since S60 v3.1
     */
    void UpdateIAPSetting();
    
    /**
     * Update retry timer setting.
     * If new setting cannot be retrieved old remains
     *
     * @since S60 v3.1
     */
    void UpdateRetryTimerSetting();

    /**
     * Update connection timer setting.
     * If new setting cannot be retrieved old remains
     *
     * @since S60 v3.1
     */
    void UpdateConnectionTimerSetting();
    
    /**
     * Update unconnect timer setting.
     * If new setting cannot be retrieved old remains
     *
     * @since S60 v3.1
     */
    void UpdateUnconnectTimerSetting();

    /**
     * Update HPLMN setting.
     * If new setting cannot be retrieved old remains
     *
     * @since S60 v3.1
     */
    void UpdateHPLMNSetting();
    
    /**
     * Update VPLMN setting.
     * If new setting cannot be retrieved old remains
     *
     * @since S60 v3.1
     */
    void UpdateVPLMNSetting();

    /**
     * Update linger timer setting.
     * If new setting cannot be retrieved old remains
     *
     * @since S60 v3.2
     */
    void UpdateLingerTimerSetting();
    
    /**
     * Updates the setting
     *
     * @since S60 v3.1
     * @param aId id os the setting to update
     * @param aValue new value of the setting
     */
    void UpdateSetting( TUint32 aId, TInt& aValue );
    
private: // Data

    /** 
     * Settings observer
     */
    MAOSettingsObserver& iObserver;
    
    /** 
     * Central repository
     * Own
     */
    CRepository* iRepository;
    
    /** 
     * Listener for settings
     * Own
     */
    CCenRepNotifyHandler* iCenRepNotifyHandler;
    
    /** 
     * Access point ID
     */
    TInt iIAP;
    
    /** 
     * Retry timer value in seconds
     */
    TInt iRetryTimerValue;
    
    /** 
     * Always-On mode supported in HPLMN
     */
    TBool iSupportedInHPLMN;

    /** 
     * Always-On mode supported in VPLMN
     */
    TBool iSupportedInVPLMN;
    
    /** 
     * Setting to fetch
     */
    TUint32 iSettingsToFetch;
    
    /** 
     * Connection timer value
     */
    TInt iConnectionTimerValue;
        
    /**
     * Own: Unconnect timer value
     */
    TInt iUnconnectTimerValue;
    
    /** 
     * Linger timer value in seconds
     */
    TInt iLingerTimerValue;
    
    /** 
     * Array of iap id and linger interval pairs
     */
    RArray< TLingerSetting > iLingerSettings;
    };
    
#endif // C_CAOSETTINGS_H
