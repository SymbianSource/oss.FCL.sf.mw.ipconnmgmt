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

#include "mpmcommsdataccess.h"

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
    EOffline
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

private:

        /**
         * Propagates wlan network selection as an initial value for the dialog,
         * Used if there are several dialogs queued.
         *
         * @param aDialogStatus  Status of the dialog when destroyed.
         * @since 3.2
         */
        void OfferInformation( TInt aDialogStatus );

        CMPMWlanQueryDialog( CMPMIapSelection&  aIapSelection,
                             TUint32            aSelectedWlanIap );

        void ConstructL();

private: // data

        // Reference to iap selection object
        CMPMIapSelection& iIapSelection;

        // Stores data for offline note
        TPckgBuf<TBool> iOfflineReply;
        
        // Stores data for wlan network guery
        TPckgBuf<TMpmWlanNetworkPrefs> iNetworkPrefs;
        
        //Interfce to Notifier
        RNotifier iNotifier;

        //State of querying Wlan settings
        TWlanQueryState iWlanQueryState;
            
        // Connection Ui Utilities pointer
        CConnectionUiUtilities* iConnUiUtils;
        
        // Wlan iap
        TUint32 iWlanIapId;
        
        // Flag that can be used to cancel the dialog startup based on earlier dialog
        TInt iOverrideStatus;
    };

#endif // MPMWLANQUERYDIALOG_H
