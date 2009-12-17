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
*      Inline method definitions of the class CConnectionInfo
*
*
*/



#ifndef __CONNECTIONINFO_INL__
#define __CONNECTIONINFO_INL__


// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CConnectionInfo::Id
// ---------------------------------------------------------
//
const TUint32 CConnectionInfo::Id() const
    {
    return iId;
    }


// ---------------------------------------------------------
// CConnectionInfo::Name()
// ---------------------------------------------------------
//
const TDesC& CConnectionInfo::Name() const
    { 
    return *iName; 
    }


// ---------------------------------------------------------
// CConnectionInfo::ListItem()
// ---------------------------------------------------------
//
const TDesC& CConnectionInfo::ListItem() const
    { 
    return *iListItem; 
    }


// ---------------------------------------------------------
// CConnectionInfo::VPNIapId()
// ---------------------------------------------------------
//
const TUint32 CConnectionInfo::VPNIapId() const
    {
    return iVPN_iap_id;
    }


// ---------------------------------------------------------
// CConnectionInfo::DestinationId()
// ---------------------------------------------------------
//
const TUint32 CConnectionInfo::DestinationId() const
    {
    return iDestinationId;
    }


// ---------------------------------------------------------
// CConnectionInfo::VPNIapId()
// ---------------------------------------------------------
//
const TBool CConnectionInfo::IsVPN() const
    {
    return iIsVPN;
    }


#endif // __CONNECTINGNOTENOTIF_INL__

// End of File
