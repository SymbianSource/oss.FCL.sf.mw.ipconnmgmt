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
*      Inline method definitions of the class CWlanNetworkDataProvider.
*
*
*/



#ifndef WLAN_NETWORK_DATA_PROVIDER_NOTIF_INL
#define WLAN_NETWORK_DATA_PROVIDER_NOTIF_INL


// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CWlanNetworkDataProvider::GetNetworkInfoArray
// ---------------------------------------------------------
//    
CNetworkInfoArray** CWlanNetworkDataProvider::GetNetworkInfoArray()
    {
    return &iNetworkInfoArray;
    }    
    
    
#ifndef __WINS__
// ---------------------------------------------------------
// CWlanNetworkDataProvider::RefreshScanInfo
// ---------------------------------------------------------
//    
void CWlanNetworkDataProvider::RefreshScanInfo( TRequestStatus& aStatus )
    {
    iWlanMgmt->GetScanResults( aStatus, *iScanInfo ); 
    }
#endif // ! __WINS__           
    
#endif // WLAN_NETWORK_DATA_PROVIDER_NOTIF_INL

// End of File