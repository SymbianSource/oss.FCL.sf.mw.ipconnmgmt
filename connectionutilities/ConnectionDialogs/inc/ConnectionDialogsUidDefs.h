/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of the UIDs used by Connection Dialogs
*
*/



#ifndef __CONNECTIONDIALOGSUIDDEFS_H__
#define __CONNECTIONDIALOGSUIDDEFS_H__


// INCLUDES
#include <e32std.h>


// CONSTANTS

// UIDs for CConnDlg plugins

// ID of IAP dialog
const TUid KUidCConnDlgIap = { 0x101FD675 };

// ID of New IAP dialog
const TUid KUidCConnDlgNewIap = { 0x101FD676 };

// ID of New ISP dialog
const TUid KUidCConnDlgNewIsp = { 0x101FD677 };

// ID of Authentication dialog
const TUid KUidCConnDlgAuthentication = { 0x101FD678 }; 

// ID of Reconnect dialog
const TUid KUidCConnDlgReconnect = { 0x101FD679 };

// ID of QOS dialog
const TUid KUidCConnDlgQos = { 0x101FD67A };

// ID of Modem and Location dialog
const TUid KUidCConnDlgModemAndLocation = { 0x101FD67B };

// ID of Select connection dialog
const TUid KUidCConnDlgSelectConn = { 0x10281BB3 };


// Diisconnect dialog uid
const TUid KUidDisconnectDlg = { 0x101F85F1 };

// ID of Connecting via discreet popup
// Deprecated. Left here for documentation purpose.
// const TUid KUidConnectingViaDiscreetPopup = { 0x20026FC5 };

// ID of Connection error discreet popup
// Deprecated. Left here for documentation purpose.
// const TUid KUidConnectionErrorDiscreetPopup = { 0x20026FEF };

// WLAN uids

// ID of SelectWLan dialog
const TUid KUidSelectWLanDlg = { 0x10207373 };

// ID of OfflineWlanNote dialog
const TUid KUidCOfflineWlanNoteDlg = { 0x101FD671 };

// ID of OfflineWlanDisabledNote dialog
const TUid KUidCOfflineWlanDisabledNoteDlg = { 0x101FD672 };

// ID of Easy Wep dialog
const TUid KUidEasyWepDlg = { 0x101FD673 };

// ID of Easy Wpa dialog
const TUid KUidEasyWpaDlg = { 0x101FD674 };

// ID of WLANNetworkUnavailableNote dialog
const TUid KUidWLANNetworkUnavailableNoteDlg = { 0x101FD67F };
// ALR uids

// ID of confirmation queries
const TUid KUidConfirmationQuery = { 0x10281BAF };

// ID of two info notes
const TUid KUidConnectViaNote = { 0x10281BB0 };

// ID of WLAN Power Save Test query
const TUid KUidWlanPowerSaveTestNote = { 0x101F6D4F };

// ID of Easy Wapi dialog
const TUid KUidEasyWapiDlg = { 0x101F6D51 };

// ID of No WLAN networks are available at the moment -note
// Deprecated. Left here for documentation purpose.
// const TUid KUidNoWLANNetworksAvailableNote = { 0x20016A83 };

#endif  // __CONNECTIONDIALOGSUIDDEFS_H__

// End of File
