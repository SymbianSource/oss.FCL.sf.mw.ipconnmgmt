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
* Description:  Declaration of the CApNetworks class.
*
*/


// INCLUDE FILES
#include <cdbcols.h>
#include <commdb.h>
#include <eikdef.h>
#include <d32dbms.h>
#include <ApNetworkItem.h>
#include <ApNetworkItemList.h>
#include <ApEngineConsts.h>

#include <comms-infras/commdb/protection/protectdb.h>

#include "ApNetworks.h"
#include "ApEngineLogger.h"
#include "ApEngineCommons.h"




// LOCAL CONSTANTS AND MACROS

// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
// ---------------------------------------------------------
// CApNetworks::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApNetworks* CApNetworks::NewLC( CCommsDatabase& aDb )
    {
    return NULL;
    }


// Destructor
// ---------------------------------------------------------
// CApNetworks::~CApNetworks
// ---------------------------------------------------------
//
EXPORT_C CApNetworks::~CApNetworks()
    {
    }


// C++ default constructor can NOT contain any code that
// might leave.
//
// ---------------------------------------------------------
// CApNetworks::CApNetworks
// ---------------------------------------------------------
//
EXPORT_C CApNetworks::CApNetworks()
:iCount( 0 )
    {
    }


// Symbian OS default constructor can leave.
// ---------------------------------------------------------
// CApNetworks::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApNetworks::ConstructL( CCommsDatabase& aDb )

    {
    }



// ---------------------------------------------------------
// CApNetworks::Count
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApNetworks::Count() const
    {
    return 0;
    }


// ---------------------------------------------------------
// CApNetworks::Name
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CApNetworks::NameL( TUint32 aUid ) const
    {
    User::Leave( KErrNotSupported );
    }






// ---------------------------------------------------------
// CApNetworks::AllListItemDataL
// ---------------------------------------------------------
//
EXPORT_C TInt CApNetworks::AllListItemDataL( CApNetworkItemList& aList )
    {
    return 0;
    }




// ================= OTHER EXPORTED FUNCTIONS ==============

// ================== OTHER MEMBER FUNCTIONS ===============



// ---------------------------------------------------------
// CApNetworks::DoUpdate
// ---------------------------------------------------------
//
TInt CApNetworks::DoUpdateL()
    {
    return KErrNotFound;
    }



// End of File
