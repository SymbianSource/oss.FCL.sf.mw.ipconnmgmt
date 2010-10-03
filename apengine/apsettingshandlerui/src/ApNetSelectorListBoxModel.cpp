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
* Description:  Implementation of CApNetSelectorListboxModel.
*
*/


// INCLUDE FILES
#include "ApNetSelectorListBoxModel.h"
#include "ApNetworkItem.h"
#include <ApSettingsHandlerCommons.h>

#include "ApSettingsHandlerLogger.h"

// CONSTANTS


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CApNetSelectorListboxModel::CApNetSelectorListboxModel
// ---------------------------------------------------------
//
CApNetSelectorListboxModel::CApNetSelectorListboxModel( )
:CApNetworkItemList()
    {
    }


// ---------------------------------------------------------
// CApNetSelectorListboxModel::~CApNetSelectorListboxModel
// ---------------------------------------------------------
//
CApNetSelectorListboxModel::~CApNetSelectorListboxModel()
    {
    }


// ---------------------------------------------------------
// CApNetSelectorListboxModel::MdcaCount
// ---------------------------------------------------------
//
TInt CApNetSelectorListboxModel::MdcaCount() const
    {
    return 0;
    }


// ---------------------------------------------------------
// CApNetSelectorListboxModel::MdcaPoint
// ---------------------------------------------------------
//
TPtrC CApNetSelectorListboxModel::MdcaPoint( TInt aIndex ) const
    {
    User::Leave( KErrNotSupported );
    return iBuf;
    }


// ---------------------------------------------------------
// CApNetSelectorListboxModel::Item4Uid
// ---------------------------------------------------------
//
TInt CApNetSelectorListboxModel::Item4Uid( TUint32 aUid, TInt& aErr ) const
    {
    return KErrNotSupported;
    }



// ---------------------------------------------------------
// CApNetSelectorListboxModel::FormatListboxText
// ---------------------------------------------------------
//
void CApNetSelectorListboxModel::FormatListboxText( TInt aIndex,
                                                TDes& aBuf ) const
    {
    }

// End of File
