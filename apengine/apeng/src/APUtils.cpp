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
* Description:  Declaration of the class CApUtils
*     Contains utility functions for access point handling
*
*/


// INCLUDE FILES
#include "APEngineVariant.hrh"

#include "ApUtils.h"
#include "ApAccessPointItem.h"
#include "ApDataHandler.h"
#include "ApEngineCommons.h"
#include "ApEngineLogger.h"
#include <e32des16.h>
#include <es_sock.h>
#include <es_enum.h>

#include    <featmgr.h>

#include <WlanCdbCols.h>


// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES


// ================= MEMBER FUNCTIONS =======================

// construct/destruct/copy
// ---------------------------------------------------------
// CApUtils::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApUtils* CApUtils::NewLC( CCommsDatabase& aDb )
    {
    return NULL;
    }



// Destructor
// ---------------------------------------------------------
// CApUtils::~CApUtils
// ---------------------------------------------------------
//
EXPORT_C CApUtils::~CApUtils()
    {
    }


// C++ default constructor can NOT contain any code that
// might leave.
//
// ---------------------------------------------------------
// CApUtils::CApUtils
// ---------------------------------------------------------
//
EXPORT_C CApUtils::CApUtils()
    {
    }


// Symbian OS default constructor can leave.
// ---------------------------------------------------------
// CApUtils::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApUtils::ConstructL( CCommsDatabase& aDb )
    {
    }





// utilities

// ---------------------------------------------------------
// CApUtils::IsAPInUseL
// ---------------------------------------------------------
//
EXPORT_C TBool CApUtils::IsAPInUseL( TUint32 aUid )
    {
    return EFalse;
    }




// ---------------------------------------------------------
// CApUtils::WapBearerTypeL
// ---------------------------------------------------------
//
EXPORT_C void CApUtils::WapBearerTypeL( TUint32 aUid, TDes& aBearer )
    {
    }


// ---------------------------------------------------------
// CApUtils::BearerTypeL
// ---------------------------------------------------------
//
EXPORT_C TApBearerType CApUtils::BearerTypeL( TUint32 aUid )
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// CApUtils::ISPTypeL
// ---------------------------------------------------------
//
EXPORT_C void CApUtils::ISPTypeL( TUint32 aUid, TCommsDbIspType& aIspType )
    {
    }



// ---------------------------------------------------------
// CApUtils::IsValidNameL
// ---------------------------------------------------------
//
EXPORT_C TBool CApUtils::IsValidNameL( const TDesC& aNameText )
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApUtils::MakeValidName
// ---------------------------------------------------------
//
EXPORT_C TBool CApUtils::MakeValidNameL( TDes& aNameText )
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApUtils::NameL
// ---------------------------------------------------------
//
EXPORT_C void CApUtils::NameL( TUint32 aUid, TDes& aName )
    {
    }



// ---------------------------------------------------------
// CApUtils::SetNameL
// ---------------------------------------------------------
//
EXPORT_C void CApUtils::SetNameL( TDes& aName, TUint32 aUid )
    {
    }



// ---------------------------------------------------------
// CApUtils::SetISPTypeL
// ---------------------------------------------------------
//
EXPORT_C void CApUtils::SetISPTypeL( TUint32 aUid, TCommsDbIspType aIspType )
    {
    }



// ---------------------------------------------------------
// CApUtils::IapIdFromWapIdL
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApUtils::IapIdFromWapIdL( TUint32 aUid )
    {
    return 0;
    }


// ---------------------------------------------------------
// CApUtils::WapIdFromIapIdL
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApUtils::WapIdFromIapIdL( TUint32 aUid )
    {
    return 0;
    }


// ---------------------------------------------------------
// CApUtils::HasMandatoryFieldsFilledL
// ---------------------------------------------------------
//
EXPORT_C TBool CApUtils::HasMandatoryFieldsFilledL( TUint32 aUid )
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApUtils::IAPExistsL
// ---------------------------------------------------------
//
EXPORT_C TBool CApUtils::IAPExistsL( TUint32 aUid )
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApUtils::WapApExistsL
// ---------------------------------------------------------
//
EXPORT_C TBool CApUtils::WapApExistsL( TUint32 aUid )
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApUtils::DoHasMandatoryFieldsFilledL
// ---------------------------------------------------------
//
EXPORT_C TBool CApUtils::DoHasMandatoryFieldsFilledL(
                                                  TUint32 aUid,
                                                  CCommsDbTableView* aWapt,
                                                  TApBearerType aBearertype )
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApUtils::IsValidNameL
// ---------------------------------------------------------
//
EXPORT_C TBool CApUtils::IsValidNameL( const TDesC& aNameText,
                                       TUint32 aAcceptUid )
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApUtils::IsReadOnlyL
// ---------------------------------------------------------
//
EXPORT_C TBool CApUtils::IsReadOnlyL( TUint32 aUid )
    {
    return EFalse;
    }




// ---------------------------------------------------------
// CApUtils::IsWlanL
// ---------------------------------------------------------
//
TBool CApUtils::IsWlanL( TUint32 aLanCommdbId )
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApUtils::HasWlanSSIDL
// ---------------------------------------------------------
//
TBool CApUtils::HasWlanSSIDL( TUint32 aLanCommdbId )
    {
    return EFalse;
    }


// ==== OTHER PRIVATE/protected, NON-EXPORTED FUNCTIONS ====

// ---------------------------------------------------------
// CApUtils::DoWapBearerTypeL
// ---------------------------------------------------------
//
void CApUtils::DoWapBearerTypeL( TUint32 aUid, TDes& aBearer )
    {
    }



// ---------------------------------------------------------
// CApUtils::DoBearerTypeL
// ---------------------------------------------------------
//
TApBearerType CApUtils::DoBearerTypeL( TUint32 aUid )
    {
    User::Leave( KErrNotSupported );
    }



// ---------------------------------------------------------
// CApUtils::DoISPTypeL
// ---------------------------------------------------------
//
void CApUtils::DoISPTypeL( TUint32 aUid, TCommsDbIspType& aIspType )
    {
    }


// ---------------------------------------------------------
// CApUtils::DoNameL
// ---------------------------------------------------------
//
void CApUtils::DoNameL( TUint32 aUid, TDes& aName )
    {
    }


// ---------------------------------------------------------
// CApUtils::DoSetNameL
// ---------------------------------------------------------
//
void CApUtils::DoSetNameL( TDes& aName, TUint32 aUid )
    {
    }


// ---------------------------------------------------------
// CApUtils::DoSetISPTypeL
// ---------------------------------------------------------
//
void CApUtils::DoSetISPTypeL( TUint32 aUid, TCommsDbIspType aIspType )
    {
    }


// ---------------------------------------------------------
// CApUtils::DoesExistsL
// ---------------------------------------------------------
//
TBool CApUtils::DoesExistsL( TPtrC aTable, TUint32 aUid )
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApUtils::DoIsWlanL
// ---------------------------------------------------------
//
TBool CApUtils::DoIsWlanL( TUint32 aLanCommdbId )
    {
    return EFalse;
    }

//  End of File
