/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Handles displaying wlan related dialogs
*
*/


#ifndef MPMWLANQUERYDIALOG_H
#define MPMWLANQUERYDIALOG_H

#include <e32std.h>
#include <wdbifwlansettings.h> //for struct
#include <ConnectionUiUtilities.h>
#include <wifiprotuiclient.h>
#include <wifiprotreturn.h>

#include "mpmiapselection.h" // for TOfflineNoteResponse

// ID of Easy Wep dialog
const TUid KUidEasyWepDlg = { 0x101FD673 };

// ID of Easy Wpa dialog
const TUid KUidEasyWpaDlg = { 0x101FD674 };

// ID of OfflineWlanNote dialog
const TUid KUidCOfflineWlanNoteDlg = { 0x101FD671 };

/**
 * This structure is copied from ConnectionUiUtilitiesCommon.h,
 * but given here a different name.
 * Defines preferences of the Wlan Network
 */
class TMpmWlanNetworkPrefs
    {
public:
    TWlanSsid                           iSsId;        ///< Ssid of Wlan network
    TWlanConnectionMode                 iNetworkMode; ///< mode of Wlan network
    TWlanConnectionSecurityMode         iSecMode;     ///< Security mode of Wlan network
    TBool                               iProtectedSetupSupported; 
                                        ///< Wlan network supports Protected Setup
    };

const TInt KEasyWepQuery256BitMaxLength = 58;

class TWlanAgtWepKeyData
    {
public:
    TBuf< KEasyWepQuery256BitMaxLength > iKey;
    TBool iHex;
    };

class CMPMIapSelection;
/**
 *  Handles displaying wlan related dialogs
 *
 *  @lib mpmserver.exe
 *  @since S60 v3.2
 */
class CMPMWlanQueryDialog : public CActive
    {

enum TWlanQueryState
    {
    EOffline,
    EWlanNetwork,
    EWepSettings,
    EWpaSettings, 
    EWPS,
    ERetrySettingsStorage
    };

public:

    static CMPMWlanQueryDialog* NewL( CMPMIapSelection&  aSession,
                                      TUint32            aSelectedWlanIap );

    /**
    * Destructor.
    */
    virtual ~CMPMWlanQueryDialog();

public: // Functions from base classes

        /**
        * From CActive. Cancels UI dialogs.
        * @since 3.2
        */        
        void DoCancel();
        
        /**
        * From CActive. Run when UI dialog finished.
        * @since 3.2
        */               
        void RunL();
        
        /**
        * From CActive. Run if RunL leaves.
        * @since 3.2
        * @param aError Error that caused leave.
        * @return KErrNone
        */
        TInt RunError( TInt aError );

public:

        /**
        * Starts displaying wlan dialogs if necessary
        * @since 3.2
        */
        void StartWlanQueryL();
        
        /**
        * Stores user defined easy wlan parameters to commsdat
        * if easy wlan has been selected.
        *
        * @since 3.2
        */
        void StoreEasyWlanSelectionL();

private:

        /**
         * Propagates wlan network selection as an initial value for the dialog,
         * Used if there are several dialogs queued.
         *
         * @param aWlanIapId     Selected WLAN IAP
         * @param aOfflineStatus Offline note response
         * @param aDialogStatus  Status of the dialog when destroyed.
         * @since 3.2
         */
        void OfferInformation( TUint32 aWlanIapId, 
                               TOfflineNoteResponse aOfflineStatus,
                               TInt aDialogStatus );

        CMPMWlanQueryDialog( CMPMIapSelection&  aSession,
                             TUint32            aSelectedWlanIap );

        void ConstructL();

        /**
        * Gets Easy Wlan data from member variables and stores 
        * it to arguments
        *
        * @since 9.1
        * @param aSsid Wlan iap ssid, stored in used ssid field
        * @param aSecMode Wlan iap security mode
        * @param aConnMode Wlan iap connection mode
        * @param aWepKeyData Wlan iap wep key data
        * @param aEnableWpaPsk Wlan iap enable psk mode
        * @param aWpaPsk Wlan iap wpa psk
        * @param aWpaKeyLen Wlan iap wpa key length
        */
        void GetEasyWlanDataL( TWlanSsid&    aSsid,
                               TUint&        aSecMode,
                               TWlanNetMode& aConnMode,
                               TWepKeyData&  aWepKeyData, 
                               TUint&        aEnableWpaPsk,
                               TDes8&        aWpaPsk,
                               TUint&        aWpaKeyLen  );

        /**
        * Maps TWlanConnectionSecurityMode to security mode stored in CommsDat
        *
        * @since 9.1
        * @param aConnSecmode Security mode in TWlanConnectionSecurityMode format
        * @param aCommsdatSecMode returned security mode value in CommsDat format
        */
        TInt ConnSecModeToCommsDatSecMode( TWlanConnectionSecurityMode aConnSecmode,
                                           TUint& aCommsdatSecMode ) const;

        /**
        * Maps TWlanIapSecurityMode to security mode stored in CommsDat
        *
        * @since 9.1
        * @param aConnSecmode Security mode in TWlanIapSecurityMode format
        * @param aCommsdatSecMode returned security mode value in CommsDat format
        */
        TInt IapSecModeToCommsDatSecMode( TWlanIapSecurityMode aConnSecmode,
                                          TUint& aCommsdatSecMode ) const;

        /**
        * Gets Easy Wlan data from member variables when WPS has been used 
        * and stores it to arguments
        *
        * @since 9.1
        * @param aSsid Wlan iap ssid, stored in used ssid field
        * @param aSecMode Wlan iap security mode
        * @param aConnMode Wlan iap connection mode
        * @param aWepKeyData WEP key data
        * @param aEnableWpaPsk Wlan iap enable psk mode
        * @param aWpaPsk Wlan iap wpa psk
        * @param aWpaKeyLen Wlan iap wpa key length
        */
        void GetEasyWlanDataForWpsL( TWlanSsid&    aSsid,
                                     TUint&        aSecMode,
                                     TWlanNetMode& aConnMode,
                                     TWepKeyData&  aWepKeyData, 
                                     TUint&        aEnableWpaPsk,
                                     TDes8&        aWpaPsk,
                                     TUint&        aWpaKeyLen  );

        /**
        * Starts WPS dialog and sets the active object active.
        *
        * @since 9.1
        */
        void StartWpsDlgL();

        /**
        * Determines whether WEP key is hex format based on the length
        *
        * @since 9.1
        * @param aLength 
        * @return ETrue if format is hex, otherwise EFalse
        */
        TBool IsWepFormatHexL( TInt aLength ) const;

        /**
        * Get the network preferences using ConnectionUiUtilities API.
        *
        * @since 5.1
        */
        void GetNetworkPrefs();

private: // data

        // Reference to iap selection object
        CMPMIapSelection& iIapSelection;

        // Stores data for offline note
        TPckgBuf<TBool> iOfflineReply;
        
        // Stores data for wlan network guery
        TPckgBuf<TMpmWlanNetworkPrefs> iNetworkPrefs;
        
        // Stores data for wep query
        TPckgBuf<TWlanAgtWepKeyData> iNotifWep;
        
        // Stores data for wpa query
        TPckgBuf< TBuf< KWLMMaxWpaPskLength > > iNotifWpaKey;

        //Interfce to Notifier
        RNotifier iNotifier;

        //State of querying Wlan settings
        TWlanQueryState iWlanQueryState;
            
        // selected wlan iap id
        TUint32 iWlanIapId;
            
        // ETrue if user has selected easy wlan
        TBool iEasyWlanSelected;
            
        // Flag that can be used to cancel the dialog startup based on earlier dialog
        TInt iOverrideStatus;
            
        // Connection Ui Utilities pointer
        CConnectionUiUtilities* iConnUiUtils;

        // Contains WLAN settings received through WPS
        TWlanProtectedSetupCredentialAttribute iWpsAttribute;

        // WPS client pointer for displaying WPS dialogs
        CWiFiProtUiClient* iWps;

        // Contains WPS return value
        WiFiProt::TWiFiReturn iWpsReturn;

        // True if wps has been completed. 
        TBool iWpsCompleted;
        
        // Retry count
        TUint32 iRetryCount;
        
        // Retry timer
        RTimer iTimer;
    };

#endif // MPMWLANQUERYDIALOG_H
