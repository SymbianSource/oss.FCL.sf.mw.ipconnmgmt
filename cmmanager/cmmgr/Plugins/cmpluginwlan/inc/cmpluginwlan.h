/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of WLAN interface implementation for WLAN plugin
*
*/

#ifndef WLAN_PLUGIN_H
#define WLAN_PLUGIN_H

// INCLUDES
#include <cmpluginlanbase.h>
#include <cmpluginwlandef.h>
#include <e32base.h>
#include <eikdoc.h>
#include <apparc.h>
#include <eikenv.h>
#include <wlanmgmtcommon.h>
#include <wifiprotreturn.h>

// FORWARD DECLARATION
class CCDWlanServiceRecord;
class CWEPSecuritySettings;
class CWEPSecuritySettingsUi;
class CWPASecuritySettings;
class CWPASecuritySettingsUi;
class RConeResourceLoader;
class CConnectionUiUtilities;
class CCmPluginWlanDataArray;
class CAknWaitDialog;
 
class CWAPISecuritySettings;
class CWAPISecuritySettingsUi;

/** 
 * WPA Mode
 */
enum TWpaMode
    {
    EWpaModeUndefined = -1,
    EWpaModeEap,
    EWpaModePreSharedKey
    };

/**
 *  WLAN Plugin IF implementation class
 *
 *  ?more_complete_description
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CCmPluginWlan ) : public CCmPluginLanBase
    {

    public: // Constructors and destructor

        /**
        * Two phased constructor
        *
        * @since S60 3.2
        * @param aInitParam
        * @return The created object.
        */      
		static CCmPluginWlan* NewL( TCmPluginInitParam* aInitParam );

        /**
        * Destructor.
        */      
        virtual ~CCmPluginWlan();
        
        virtual CCmPluginBaseEng* CreateInstanceL( TCmPluginInitParam& aInitParam ) const;
        
    public: // from base class
        
		/**
		* From CCmPluginBaseEng
		* Gets the value for a TUint32 attribute
		*
		* @since S60 3.2
		* @param aAttribute Identifies the attribute to be retrived
		* @return the requested TUint32 value
		*/
		TUint32 GetIntAttributeL( const TUint32 aAttribute ) const;

		/**
		* From CCmPluginBaseEng
		* Gets the value for a TBool attribute
		*
		* @since S60 3.2
		* @param aAttribute Identifies the attribute to be retrived
        * @return the requested TBool value
		*/
		TBool GetBoolAttributeL( const TUint32 aAttribute ) const;

		/**
		* From CCmPluginBaseEng
		* Gets the value for a TDes16 attribute
		*
		* @since S60 3.2
		* @param aAttribute Identifies the attribute to be retrived
        * @return the requested string value
		*/
		HBufC* GetStringAttributeL( const TUint32 aAttribute ) const;

		/**
		* From CCmPluginBaseEng
		* Sets the value for a TInt attribute
		*
		* @since S60 3.2
		* @param aAttribute Identifies the attribute to be set.
		* @param aValue The value to be set
        */
		void SetIntAttributeL( const TUint32 aAttribute, TUint32 aValue );

		/**
		* * From CCmPluginBaseEng
		* Sets the value for a TBool attribute
		*
		* @since S60 3.2
		* @param aAttribute Identifies the attribute to be set.
		* @param aValue The value to be set
        */
		void SetBoolAttributeL( const TUint32 aAttribute, TBool aValue );

		/**
		* From CCmPluginBaseEng
		* Sets the value for a TDesC attribute
		*
		* @since S60 3.2
		* @param aAttribute Identifies the attribute to be set.
		* @param aValue The value to be set
        */
		void SetStringAttributeL( const TUint32 aAttribute, const TDesC16& aValue );
      
        /**
		* From CCmPluginBaseEng
		* Checks if the plug-in can handle the given AP.
		*
		* @since S60 3.2
		* @param aIapId
		* @return ETrue if plug-in can handle the IAP
        */        
        TBool CanHandleIapIdL( TUint32 aIapId ) const;
        
        /**
		* From CCmPluginBaseEng
        * Checks if the plug-in can handle the given AP
        *
        * @since S60 3.2
        * @param aIapRecord IAP record to be checked
        * @return ETrue if plug-in can handle the IAP
        */
        TBool CanHandleIapIdL( CommsDat::CCDIAPRecord *aIapRecord ) const;

        /**
        * From CCmPluginBaseEng
        * Launches the WLAN settings dialog
        *
        * @since S60 3.2
        * @return soft key selection
        */
        TInt RunSettingsL();
        
        /**
        * From CCmPluginBaseEng
        * see base class definition
        *
        * @since S60 3.2
        */
	    TBool InitializeWithUiL( TBool aManuallyConfigure );

        /**
        * From CCmPluginBaseEng
        * see base class definition
        *
        * @since S60 3.2
        */      
	    void LoadServiceSettingL();
	    
	    /**
        * From CCmPluginBaseEng
        * see base class definition
        *
        * @since S60 3.2
        */
	    void AdditionalReset();
	    
    public: // New functions
	    /**
	    * Edits the security settings for the current connection method
	    *
        * @since S60 3.2
        * @param aEikonEnv Eikon environment to use
        * @return exit reason
	    */	  
	    TInt EditSecuritySettingsL( CEikonEnv& aEikonEnv );
	    
	    /**
	    * Returns the security settings validity for the current connection method
	    *
	    * @since S60 3.2
	    * @return Validity of the current security settings
	    */	    
        TBool AreSecuritySettingsValidL();

	    /**
	    * Loads the security settings for the current connection method
	    *
	    * @since S60 3.2
	    */
	    void LoadSecuritySettingsL();

    protected:
    
        /**
        * From CCmPluginBaseEng
        * see base class definition
        *
        * @since S60 3.2
        */
	    virtual void BearerRecordIdLC( HBufC* &aBearerName, 
                                   TUint32& aRecordId );


        /**
        * Copy data of the connection method that is not 
        * added to the conversion table. (e.g. bearer specific
        * flags)
        *
        * @param aDestInst copy attributes into this plugin instance
        */    
        void CopyAdditionalDataL( CCmPluginBaseEng& aDestInst ) const;
        
        
	private: // construction

        /**
        * Constructor
        *
        * @since S60 3.2
        */      
		CCmPluginWlan( TCmPluginInitParam* aInitParam );

        /**
        * Second phase constructor
        *
        * @since S60 3.2
        */      
		void ConstructL();

    private: // from base class
    
        /**
        * From CCmPluginBaseEng
        * Updates additional records, in this case, the WLAN records.
        *
        * @since S60 3.2
        */
        void UpdateAdditionalRecordsL();

        /**
        * From CCmPluginBaseEng
        * Deletes additional records, in this case, the WLAN records
        *
        * @since S60 3.2
        */
        void DeleteAdditionalRecordsL();

        /**
        * From CCmPluginBaseEng
        * Loads additional records, in this case, the WLAN records
        *
        * @since S60 3.2
        */
        void LoadAdditionalRecordsL();
    
	    /**
        * From CCmPluginBaseEng
        * Creates a new service record for this bearer
        *
        * @since S60 3.2
        */
	    void CreateNewServiceRecordL();
	    
	    /**
        * From CCmPluginBaseEng
        * Creates the WLAN service record used by this WLAN
        *
        * @since S60 3.2
        */

	    void CreateWlanServiceRecordL();
	    
        /**
        * From CCmPluginBaseEng
        */
        virtual void PrepareToUpdateRecordsL();

        /**
        * Appends uids of connection methods - which were also created during connection method
        * creation - to the array given as parameter 
        * @param aIapIds the array the additonal cm uids are appended to
        */        
        virtual void GetAdditionalUids( RArray<TUint32>& aIapIds );
	    
    private: // new functions
    	    
        /**
        * Updates the security settings for the current WLAN connection method
        *
        * @since S60 3.2
        */
        void UpdateSecuritySettingsL();

        /**
        * Allocates a string specified by aResId and replaces %N in the
        * resource with aInt
        *
        * @since S60 3.2
        * @param aResId Resource ID to be read from resource file.
        * @param aInt Integer that will replace %N in the string.
        * @return String read from resource file, specified by aResId,
        * containing aInt.
        */
        HBufC* ReadResourceL( TInt aResId, TInt aInt ) const;

        /**
        * The UI for manually configuring a WLAN connection method
        * 
        * @since S60 3.2
        * @return ETrue if the configuration was successful
        */
        TBool ManuallyConfigureL();
        
        /**
        * Continuing with Manual configuration from step 4
        * called from ManuallyConfigureL or AutomaticallyConfigureL in case the
        * specicfied network was not found (reverting to fully manual setup)
        * 
        * @since S60 3.2
        * @param TBool aAskNWModeAndSec if set to ETrue, NW mode and Security 
        * mode dialogs are shown. If EFalse, these values are taken from
        * the corresponding parameters of this function
        * @param TWlanNetMode& aNwMode NW mode used if aAskNWModeAndSec is EFalse
        * @param TWlanConnectionExtentedSecurityMode& aSecurityMode 
        * Security mode used if aAskNWModeAndSec is EFalse
        * @param TWpaMode& aWpaMode WPA mode
        * @return ETrue if the configuration was successful
        */
        TBool ProceedWithManualL( TBool aAskNWModeAndSec,
                                  CMManager::TWlanNetMode aNwMode = CMManager::EInfra, 
                                  TWlanConnectionExtentedSecurityMode aSecurityMode = EWlanConnectionExtentedSecurityModeOpen,
                                  TWpaMode aWpaMode = EWpaModeUndefined );
        /**
        * The UI for automatically configuring a WLAN connection method
        * 
        * @since S60 3.2
        * @return ETrue if the configuration was successful
        */
        TBool AutomaticallyConfigureL();

        /*
        * Loads a specific resource, leaves the resource loader on the 
        * cleanup stack
        * 
        * @since S60 3.2
        * @param aResourceLoader
        */
        void LoadResourceLC( RConeResourceLoader& aResourceLoader ) const;
        
        /*
        * Prompt for the Network Name of the new WLAN connection method
        * sets the network name and SSID settings 
        *
        * @since S60 3.2
        * @param 
        * @return ETrue if a selection was made, EFalse if cancelled
        */
        TBool ShowNWNameQueryL( TDes& aName );
        
        /*
        * Scans for a WLAN network by a given SSID
        * initially makes a broadcast scan and if not found a direct scan.
        * Sets the scan ssid setting
        *
        * @since S60 3.2
        * @param aSsid the SSID to scan for
        * @return ETrue if a network has been found with the SSID
        */
        TBool ScanForWlanNwL( TWlanSsid& aSsid, 
                              CMManager::TWlanNetMode& aNetworkMode,
                              TWlanConnectionExtentedSecurityMode& aSecurityMode,
                              TBool& aProtectedSetupSupported  );

        /*
        * Selection list for Network Mode
        *
        * @since S60 3.2
        * @param aNWMode (return value) network mode selected
        * @return ETrue if a selection was made, EFalse if cancelled
        */
        TBool SelectNWModeL( CMManager::TWlanNetMode& aNWMode );
        
        /*
        * Displays a user selection dialog of Security Modes
        * 
        * @since S60 3.2
        * @param aNwMode which network mode was selected
        * @param aSecurityMode returns the selection
        * @return ETrue if a selection was made, EFalse if cancelled
        */
        TBool ShowNWSecurityDlgL( CMManager::TWlanNetMode aNwMode, 
                                  TWlanConnectionExtentedSecurityMode& aSecurityMode );
        
        /*
        * Triggers the WEP key input query  
        *
        * @since S60 3.2
        * @return ETrue if a selection was made, EFalse if cancelled
        */
        TBool EnterWEPKeyL();
        
        /*
        * Triggers the pre-shared key for WPA WLAN query
        *
        * @since S60 3.2
        * @return ETrue if a selection was made, EFalse if cancelled
        */
        TBool EnterWpaPreSharedKeyL();
        
        /*
        * Shows the WPA/WPA2 mode dialog
        * 
        * @since S60 3.2
        * @param aWpaMode return value
        * @return ETrue if selection made
        */
        TBool SelectWpaModeDialogL( TWpaMode& aWpaMode );
        
        /**
        * Reloads internal security settings modules according to the changes
        * @since S60 3.2
        */
        void UpdateSecurityModeL();
        
    
        /**
        * Called from another CM-s CopyAdditionalDataL, on the passed 
        * copied CM, and marks this as a copy of the original CM which 
        * has an IAP ID of aBaseIapID
        *
        * @since S60 3.2
        */
        void SetAsCopyOf( TUint32 aBaseIapID );

        /**
        * Deletes the security settings related to this connection method.
        *
        * @since S60 3.2
        */
        void DeleteSecuritySettingsL();

        
        /**
        * Set the EWlanIpDNSAddrFromServer according to the 
        * current name server values.
        *
        * @since S60 3.2
        */
        void SetDnsIpFromServerL();


        /**
        * Set the EWlanIp6DNSAddrFromServer according to the 
        * current name server values.
        *
        * @since S60 3.2
        */
        void SetDns6IpFromServerL();


	    /**
	    * Checks for coveradge and number of WLAN connections available
        * @since S60 3.2
        * @return The number of available WLAN network connections
        */
        TInt CheckNetworkCoverageL() const;


	    /**
	    * Converts security mode from TWlanConnectionSecurityMode to 
        * TWlanSecMode
        * @since S60 3.2
        * @return The security mode as TWlanSecMode
        */
        CMManager::TWlanSecMode ConvertConnectionSecurityModeToSecModeL(
                                    TWlanConnectionExtentedSecurityMode aSecurityMode );


	    /**
	    * Converts security mode from TWlanSecMode to 
        * TWlanConnectionSecurityMode
        * @since S60 3.2
        * @return The security mode as TWlanConnectionSecurityMode
        */
        TWlanConnectionExtentedSecurityMode ConvertSecModeToConnectionSecurityModeL(
                                    CMManager::TWlanSecMode aSecMode );

        /**
        * Destroy wait dialog 
        */
        void DestroyWaitDialog();


        /**
        * Sets the daemon name. Starts DHCP if set.
        *
        * @since S60 3.2
        */
        void SetDaemonNameL();


        /**
        * Converts the possible WLAN connection states from TWlanConnectionMode
        * to TWlanNetMode.
        * @since S60 3.2
        * @return The WLAN connection mode as TWlanNetMode
        */
        CMManager::TWlanNetMode ConvertWLANConnectionStatesL( 
                                    TWlanConnectionMode aConnectionMode );

        WiFiProt::TWiFiReturn ProtectedSetupL(TWlanSsid& aSsid );

        /*
        * Triggers the pre-shared key for WAPI WLAN query
        *
        * @since S60 5.1
        * @return ETrue if a selection was made, EFalse if cancelled
        */
        TBool EnterWAPIPreSharedKeyL();
        
	private: // Data

        /**
         * WLan service record
         * Own
         */         
        CCDWlanServiceRecord* iWlanServiceRecord; // owned
        
        /**
         * WLAN table id
         */
        CommsDat::TMDBElementId iWlanTableId;

        /**
         * WEP settings data
         * Own
         */
        CWEPSecuritySettings* iWepSecSettings;
        
        /**
         * WPA settings data
         * Own
         */
        CWPASecuritySettings* iWpaSecSettings;
        
        /**
         * Number of networks
         */
        TInt iWlanNetworks;
        
        /**
         * Connection UI utilities pointer
         * Own
         */
        CConnectionUiUtilities* iConnUiUtils; // owned

        /**
        * Signals that this connection method is a copy of another WLAN CM.
        */
        TBool iIsCopy;
        
        /**
        * The IAP ID of the CM from which this copy had been made. 
        */
        TUint32 iCopiedFromIapId;

        /**
        * "Searching WLAN networks" wait dialog.
        */
        CAknWaitDialog* iWaitDialog;
        
        RArray<TUint32> iAdditionalCmsCreated;

        /**
         * Asked from the Feature manager at the constructor phase
         */
        TBool iIsWAPISupported;
        
        /**
        * WAPI settings data
        * Own
        */
        CWAPISecuritySettings* iWAPISecSettings;
	};

#endif // WLAN_PLUGIN_H
