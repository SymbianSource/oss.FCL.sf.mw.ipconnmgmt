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
* Description:  Implementation of CApSelectorListboxModel.
*
*/


// INCLUDE FILES

#include "ApSelectorListBoxModel.h"
#include "ApListItem.h"
#include <ApSettingsHandlerCommons.h>

#include <featmgr.h>

#include "ApSettingsHandlerLogger.h"

// CONSTANTS
const TInt  KGPRS_ICON_ID        = 0;
// No icon for CDMA (yet?) -> use GPRS icon.
const TInt  KCDMA_ICON_ID        = KGPRS_ICON_ID;
const TInt  KCSD_ICON_ID         = 1;
const TInt  KHSCSD_ICON_ID       = 2;
// const TInt  KSMS_ICON_ID       = 3;   // DEPRECATED
const TInt  KWLAN_ICON_ID        = 4;
// fith icon element is the KEASY_WLAN_ICON_ID  but it is not shown 
// in our UI, but is used by other modules using our 
// resources!!! const TInt  KEASY_WLAN_ICON_ID   = 5;

const TInt  KPROTECTED_ICON_ID   = 6;
// get real icons !!! now using GPRS...
#ifdef __TEST_LAN_BEARER
    const TInt  KLAN_ICON_ID         = KGPRS_ICON_ID;
#endif // __TEST_LAN_BEARER


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CApSelectorListboxModel::CApSelectorListboxModel
// ---------------------------------------------------------
//
CApSelectorListboxModel::CApSelectorListboxModel( TGraphicType aGraphicType )
:CApListItemList(),
iOffset( 0 ),
iFeatureInitialised(EFalse),
iProtectionSupported(EFalse)
    {
    }

// ---------------------------------------------------------
// CApSelectorListboxModel::~CApSelectorListboxModel
// ---------------------------------------------------------
//
CApSelectorListboxModel::~CApSelectorListboxModel()
    {
    }

// ---------------------------------------------------------
// CApSelectorListboxModel::MdcaCount
// ---------------------------------------------------------
//
TInt CApSelectorListboxModel::MdcaCount() const
    {
    return 0;
    }

// ---------------------------------------------------------
// CApSelectorListboxModel::MdcaPoint
// ---------------------------------------------------------
//
TPtrC CApSelectorListboxModel::MdcaPoint( TInt aIndex ) const
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// CApSelectorListboxModel::Item4Uid
// ---------------------------------------------------------
//
TInt CApSelectorListboxModel::Item4Uid( TUint32 aUid, TInt& aErr ) const
    {
    return KErrNotFound;
    }



// ---------------------------------------------------------
// CApSelectorListboxModel::SetOwnIconOffset
// ---------------------------------------------------------
//
void CApSelectorListboxModel::SetOwnIconOffset( TInt aOffset )
    {
    }


// ---------------------------------------------------------
// CApSelectorListboxModel::FormatListboxText
// ---------------------------------------------------------
//
void CApSelectorListboxModel::FormatListboxText( TInt aIndex,
                                                TDes& aBuf ) const
    {
    }

// End of File
