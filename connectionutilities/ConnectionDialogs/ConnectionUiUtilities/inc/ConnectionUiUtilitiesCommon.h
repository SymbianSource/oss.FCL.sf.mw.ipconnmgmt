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
* Description: 
*     Declares the common used constants and types for
*     Connection Ui Utilities.
*
*/


#ifndef CONNECTIONUIUTILITIES_COMMON_H
#define CONNECTIONUIUTILITIES_COMMON_H

// INCLUDES
#include <wlanmgmtcommon.h>
#include "ConnectionDialogsUidDefs.h"


// CONSTANTS
const TInt KEasyWepQuery256BitMaxLength = 58;
const TInt KEasyWepQueryNon256BitMaxLength = 26;
const TInt KEasyWpaQueryMaxLength = 64;
const TInt KEasyWapiQueryMaxLength = 64;

// Note and Query Ids
enum TConnUINoteAndQueryIds
    {
    EReconnectToConnMethodConfirmationQuery = 1,
    ERoamingToConnMethodConfirmationQuery,
    EConnectedViaDestConnMethodConfirmationNote,
    EConnectingToConnMethodInfoNote,
    EConfirmMethodUsageQuery,
    EConnectedViaConnMethodConfirmationNote,
    EConfirmMethodUsageQueryInHomeNetwork,
    EConfirmMethodUsageQueryInForeignNetwork,
    EConfirmMethodUsageQueryNationalRoaming
    };

/**
 * Defines preferences of the WlanNetwork
 */
class TConnUiUiWlanNetworkPrefs
    {
public:
    TWlanSsid                           iSsId;        ///< Ssid of Wlan network
    TWlanConnectionMode                 iNetworkMode; ///< mode of Wlan network
    TWlanConnectionSecurityMode         iSecMode;     ///< Security mode of Wlan network
    TWlanConnectionExtentedSecurityMode iExtSecMode;  ///< Extended security mode of Wlan network
    TBool                               iProtectedSetupSupported; 
                                        ///< Wlan network supports Protected Setup
    };
    
/**
*
*/
class TWepKeyData
    {
public:
    TBuf< KEasyWepQuery256BitMaxLength > iKey;
    TBool iHex;
    };

/**
*
*/
class TConnUiUiDestConnMethodNoteId
    {
public:
    TUint32   iDestination;       ///< Destination of connection
    TUint32   iConnectionMethod;  ///< Connection method of connection
    TInt      iNoteId;            ///< Which note will be used
    TBool     iNextBestExists;    ///< If it exists another method to try
   	TWlanSsid iSsid;              ///< SSID of the connected EasyWlan

    };

class TConnUiConnectingViaDiscreetPopup
    {
public:
    TUint32   iIapId;
    };

#endif  // CONNECTIONUIUTILITIES_COMMON_H


// End of File
