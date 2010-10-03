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
* Description:  Declaration of the CApSelect class.
*
*/


// INCLUDE FILES
#include <cdbcols.h>
#include <commdb.h>
#include <eikdef.h>
#include <d32dbms.h>
#include <featmgr.h>
#include <comms-infras/commdb/protection/protectdb.h>

#include <etelpckt.h>
#ifdef __TEST_USE_SHARED_DATA
    #include <sharedDataValues.h>
#endif // __TEST_USE_SHARED_DATA

#include "ApListItem.h"
#include "ApListItemList.h"
#include "ApSelect.h"
#include "ApUtils.h"
#include "ApEngineCommons.h"
#include <ApEngineConsts.h>
#include "ApEngineLogger.h"
#include "ApListItemKey.h"




// CONSTANTS
const TInt KCursorNullPos = 0;
const TInt KItemCountNull = 0;

#ifdef __TEST_USE_SHARED_DATA
    // General Settings UID, Hardcoded
    const TUid KGeneralSettingsUid={0X100058EC};
#endif // __TEST_USE_SHARED_DATA


const TInt KDIALOUT     = 0;
const TInt KGPRSIN      = 1;
const TInt KGPRSOUT     = 2;
const TInt KCDMA_ISP    = 3;
const TInt KVPN         = 4;


const TInt  KUID        = 0;
const TInt  KISPTYPE    = 1;
const TInt  KBEARERTYPE = 2;



// LOCAL CONSTANTS AND MACROS
#define VPN_SERVICE_SNAP    _S("HomeSNAP")

// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
// ---------------------------------------------------------
// CApSelect::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApSelect* CApSelect::NewLC( CCommsDatabase& aDb,
                                     TInt aIspFilter,
                                     TInt aBearerFilter,
                                     TInt aSortType )
    {
    return NULL;
    }


// Destructor
// ---------------------------------------------------------
// CApSelect::~CApSelect
// ---------------------------------------------------------
//
EXPORT_C CApSelect::~CApSelect()
    {
    }


// C++ default constructor can NOT contain any code that
// might leave.
//
// ---------------------------------------------------------
// CApSelect::CApSelect
// ---------------------------------------------------------
//
EXPORT_C CApSelect::CApSelect()
:iCursor( KCursorNullPos ),
iCount( KItemCountNull )
    {
    }


// Symbian OS default constructor can leave.
// ---------------------------------------------------------
// CApSelect::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApSelect::ConstructL( CCommsDatabase& aDb,
                                 TInt aIspFilter,
                                 TInt aBearerFilter,
                                 TInt aSortType )

    {
    }



// ---------------------------------------------------------
// CApSelect::SetFilters
// ---------------------------------------------------------
//
EXPORT_C void CApSelect::SetFiltersL( TInt aIspFilter,
                                    TInt aBearerFilter,
                                    TInt aSortType )
    {
    }


// ---------------------------------------------------------
// CApSelect::IspTypeFilter
// ---------------------------------------------------------
//
EXPORT_C TInt CApSelect::IspTypeFilter()
    {
    return 0;
    }


// ---------------------------------------------------------
// CApSelect::BearerFilter
// ---------------------------------------------------------
//
EXPORT_C TInt CApSelect::BearerFilter()
    {
    return 0;
    }


// ---------------------------------------------------------
// CApSelect::SortOrder
// ---------------------------------------------------------
//
EXPORT_C TInt CApSelect::SortOrder()
    {
    return 0;
    }



// ---------------------------------------------------------
// CApSelect::MoveToFirst
// ---------------------------------------------------------
//
EXPORT_C TBool CApSelect::MoveToFirst()
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApSelect::MoveNext
// ---------------------------------------------------------
//
EXPORT_C TBool CApSelect::MoveNext()
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApSelect::MoveLast
// ---------------------------------------------------------
//
EXPORT_C TBool CApSelect::MoveLast()
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApSelect::MovePrev
// ---------------------------------------------------------
//
EXPORT_C TBool CApSelect::MovePrev()
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApSelect::Count
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApSelect::Count()
    {
    return 0;
    }


// ---------------------------------------------------------
// CApSelect::Name
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CApSelect::Name()
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// CApSelect::Uid
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApSelect::Uid()
    {
    return 0;
    }


// ---------------------------------------------------------
// CApSelect::Type
// ---------------------------------------------------------
//
EXPORT_C TCommsDbIspType CApSelect::Type()
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// CApSelect::BearerType
// ---------------------------------------------------------
//
EXPORT_C TApBearerType CApSelect::BearerType()
    {
    User::Leave( KErrNotSupported );
    }




// ---------------------------------------------------------
// CApSelect::AllListItemDataL
// ---------------------------------------------------------
//
EXPORT_C TInt CApSelect::AllListItemDataL( CApListItemList& aList )
    {
    return 0;
    }



// ---------------------------------------------------------
// CApSelect::IsReadOnly
// ---------------------------------------------------------
//
EXPORT_C TBool CApSelect::IsReadOnly() const
    {
    return EFalse;
    }



// ================= OTHER EXPORTED FUNCTIONS ==============

// ================== OTHER MEMBER FUNCTIONS ===============



// ---------------------------------------------------------
// CApSelect::DoUpdate
// ---------------------------------------------------------
//
TInt CApSelect::DoUpdateL()
    {
    return KErrNotSupported;
    }





// ---------------------------------------------------------
// CApSelect::GetFilteredSortedWapTableViewLC
// ---------------------------------------------------------
//
CCommsDbTableView* CApSelect::GetFilteredSortedWapTableViewLC()
    {
    return NULL;
    }





// ---------------------------------------------------------
// CApSelect::GetFilteredSortedIpBearerTableViewLC()
// ---------------------------------------------------------
//
CCommsDbTableView* CApSelect::GetFilteredSortedIpBearerTableViewLC()
    {
    return NULL;
    }



// ---------------------------------------------------------
// CApSelect::GetFilteredSortedIspOutTableViewLC()
// ---------------------------------------------------------
//
CCommsDbTableView* CApSelect::GetFilteredSortedIspOutTableViewLC()
    {
    return NULL;
    }



// ---------------------------------------------------------
// CApSelect::GetFilteredSortedGprsTableViewLC()
// ---------------------------------------------------------
//
CCommsDbTableView* CApSelect::GetFilteredSortedGprsTableViewLC( TBool aIn )
    {
    return NULL;
    }



// ---------------------------------------------------------
// CApSelect::GetUintValuesL
// ---------------------------------------------------------
//
void CApSelect::GetUintValuesL( CCommsDbTableView* aTable,
                                const TDesC& aField,
                                CArrayFixFlat<TUint32>& aArray )
    {
    }



// ---------------------------------------------------------
// CApSelect::GetProtValuesL
// ---------------------------------------------------------
//
void CApSelect::GetProtValuesL( CCommsDbTableView* aTable,
                                CArrayFixFlat<TUint32>& aArray )
    {    
    }




// ---------------------------------------------------------
// CApSelect::GetTextValuesL
// ---------------------------------------------------------
//
void CApSelect::GetTextValuesL( CCommsDbTableView* aTable,
                                const TDesC& aField,
                                CArrayPtrFlat<HBufC>& aArray )
    {
    }



// ---------------------------------------------------------
// CApSelect::GetLongTextValuesL
// ---------------------------------------------------------
//
void CApSelect::GetLongTextValuesL( CCommsDbTableView* aTable,
                                    const TDesC& aField,
                                    CArrayPtrFlat<HBufC>& aArray )
    {
    }



// ---------------------------------------------------------
// CApSelect::IsInArray
// ---------------------------------------------------------
//
TBool CApSelect::IsInArray( CArrayFixFlat<TUint32>* aArray, TUint32 aValue )
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApSelect::PosInArray
// ---------------------------------------------------------
//
TInt CApSelect::PosInArray( CArrayFixFlat<TUint32>* aArray, TUint32 aValue )
    {
    return KErrNotFound;
    }


// ---------------------------------------------------------
// CApSelect::GetItemPos
// ---------------------------------------------------------
//
TInt CApSelect::GetItemPos( CApListItemList* aList, TUint32 aId )
    {
    return KErrNotFound;
    }



// ---------------------------------------------------------
// CApSelect::OpenViewMatchingTextL
// ---------------------------------------------------------
//
CCommsDbTableView* CApSelect::OpenViewMatchingTextL(
                                             const TDesC& aTableName,
                                             const TDesC& aColumnToMatch,
                                             const TDesC16& aValueToMatch )
    {
    return NULL;
    }



// ---------------------------------------------------------
// CApSelect::GetIspValuesL
// ---------------------------------------------------------
//
void CApSelect::GetIspValuesL( CCommsDbTableView* aTable,
                               CArrayFixFlat<TUint32[3]>* aArray,
                               const TDesC& aColName )
    {
    }




// ---------------------------------------------------------
// CApSelect::GetIapPos
// ---------------------------------------------------------
//
TInt CApSelect::GetIapPos( CArrayFixFlat<TUint32[3]>* aArray, TUint32 aValue )
    {
    return KErrNotFound;
    }



// ---------------------------------------------------------
// CApSelect::SortLocalisedNameL()
// ---------------------------------------------------------
//
void CApSelect::SortLocalisedNameL( TBool aAscending )
    {
    }




// ---------------------------------------------------------
// CApSelect::SortUidL()
// ---------------------------------------------------------
//
void CApSelect::SortUidL( TBool aAscending )
    {
    }

// ---------------------------------------------------------
// CApSelect::NewLC()
// ---------------------------------------------------------
//
EXPORT_C CApSelect* CApSelect::NewLC( CCommsDatabase& aDb,
                                      TInt aIspFilter,
                                      TInt aBearerFilter,
                                      TInt aSortType,
                                      TInt aReqIpvType
                                      )
    {
    return NULL;
    }



// ---------------------------------------------------------
// CApSelect::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApSelect::ConstructL( CCommsDatabase& aDb,
                                 TInt aIspFilter,
                                 TInt aBearerFilter,
                                 TInt aSortType,
                                 TInt aReqIpvType )

    {
    }




// ---------------------------------------------------------
// CApSelect::NewLC()
// ---------------------------------------------------------
//
EXPORT_C CApSelect* CApSelect::NewLC( CCommsDatabase& aDb,
                                      TInt aIspFilter,
                                      TInt aBearerFilter,
                                      TInt aSortType,
                                      TInt aReqIpvType,
                                      TVpnFilterType aVpnFilterType
                                      )
    {
    return NULL;
    }



// ---------------------------------------------------------
// CApSelect::NewLC()
// ---------------------------------------------------------
//
EXPORT_C CApSelect* CApSelect::NewLC( CCommsDatabase& aDb,
                                      TInt aIspFilter,
                                      TInt aBearerFilter,
                                      TInt aSortType,
                                      TInt aReqIpvType,
                                      TVpnFilterType aVpnFilterType,
                                      TBool aIncludeEasyWlan )
    {
    return NULL;    
    }


// ---------------------------------------------------------
// CApSelect::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApSelect::ConstructL( CCommsDatabase& aDb,
                                 TInt aIspFilter,
                                 TInt aBearerFilter,
                                 TInt aSortType,
                                 TInt aReqIpvType,
                                 TVpnFilterType aVpnFilterType )
    {
    }





// ---------------------------------------------------------
// CApSelect::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApSelect::ConstructL( CCommsDatabase& aDb,
                                 TInt aIspFilter,
                                 TInt aBearerFilter,
                                 TInt aSortType,
                                 TInt aReqIpvType,
                                 TVpnFilterType aVpnFilterType,
                                 TBool aIncludeEasyWlan )
    {
    }



// ---------------------------------------------------------
// CApSelect::SetFiltersL
// ---------------------------------------------------------
//
void CApSelect::SetFiltersL( TInt aIspFilter, TInt aBearerFilter,
                             TInt aSortType, TInt aReqIpvType,
                             TInt aVpnFilterType )
    {
    }



// ---------------------------------------------------------
// CApSelect::GetVpnTableViewLC()
// ---------------------------------------------------------
//
CCommsDbTableView* CApSelect::GetVpnTableViewLC( )
    {
    return NULL;
    }



// ---------------------------------------------------------
// CApSelect::GetVpnValuesL
// ---------------------------------------------------------
//
void CApSelect::GetVpnValuesL( CCommsDbTableView* aTable,
                               CArrayFixFlat<TVpnData>* aVpnArray )
    { // get the VPN values
    }





// ---------------------------------------------------------
// CApSelect::AddLanL
// ---------------------------------------------------------
//
void CApSelect::AddLanL( TBool aWlan )
    {
    }


// ---------------------------------------------------------
// CApSelect::GetSupportedBearerSet
// ---------------------------------------------------------
//
TInt CApSelect::GetSupportedBearerSet( TInt aBearerFilter )
    {
    return 0;
    }


// End of File
