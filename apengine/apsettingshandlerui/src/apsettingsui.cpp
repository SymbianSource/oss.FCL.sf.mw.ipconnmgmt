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
* Description: 
*     Defines the handler, CApSettingsUi and oem API for 
*       the Access Point settings.
*
*/


// INCLUDE FILES

#include <e32base.h>

#include <ApSettingsHandlerCommons.h>
#include "apsettingsui.h"
#include "ApsettingshandleruiImpl.h"

#include "ApSettingsHandlerLogger.h"
#include <data_caging_path_literals.hrh>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CApSettingsUi::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApSettingsUi* CApSettingsUi::NewLC(
                                        TBool aStartWithSelection,
                                        TSelectionListType aListType,
                                        TSelectionMenuType aSelMenuType,
                                        TInt aIspFilter,
                                        TInt aBearerFilter,
                                        TInt aSortType,
                                        TInt aReqIpvType,
                                        TVpnFilterType aVpnFilterType
                                            )
    {
    return NULL;
    }



// Destructor
EXPORT_C CApSettingsUi::~CApSettingsUi()
    {
    }




// Constructor
CApSettingsUi::CApSettingsUi( TBool aStartWithSelection,
                            TSelectionListType aListType,
                            TSelectionMenuType aSelMenuType,
                            TInt aReqIpvType )
:iStartWithSelection( aStartWithSelection ),
iListType( aListType ),
iSelMenuType( aSelMenuType ),
iReqIpvType( aReqIpvType )
    {
    }



// ---------------------------------------------------------
// CApSettingsUi::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApSettingsUi::ConstructL( TInt aIspFilter,
                                         TInt aBearerFilter,
                                         TInt aSortType,
                                         TVpnFilterType aVpnFilterType
                                        )
    {
    }



// ---------------------------------------------------------
// CApSettingsUi::RunSettingsL
// ---------------------------------------------------------
//
EXPORT_C TInt CApSettingsUi::RunSettingsL( TUint32 aHighLight,
                                                TUint32& aSelected )
    {
    return 0;
    }



// End of File

