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
* Description:  Declaration of class CApDataHandler
*
*/


// INCLUDE FILES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <nifvar_internal.h>
#endif
#include "APEngineVariant.hrh"

#include <cdbcols.h>
#include <commdb.h>
#include <cdbpreftable.h>
#include <etelpckt.h>
#include <featmgr.h>
#ifdef __TEST_USE_SHARED_DATA
    #include <SharedDataClient.h>
    #include <sharedDataValues.h>
#endif //__TEST_USE_SHARED_DATA
#include <WlanCdbCols.h>

#include "ApDataHandler.h"
#include "ApAccessPointItem.h"
#include "ApEngineCommons.h"
#include "ApUtils.h"
#include "ApEngineVer.h"
#include "ApEngineLogger.h" 
#include "ApNetworkItem.h"
#include "APItemExtra.h"
#include "APItemCdmaData.h"
#include "APItemWlanData.h"
#include "ApSpeedLookup.h"

// CONSTANTS
#if defined(_DEBUG)
    _LIT( KErrInvalidIntendedType, "Invalid intended default type" );
#endif // (_DEBUG)
_LIT( KErrNoSuchCase, "No such case" );


// Agreed GPRS_QOS_WARNING_TIMEOUT Value
const TInt KGprsOsTimeout = -1;

/**
* General Settings UID
*/
#ifdef __TEST_USE_SHARED_DATA
    LOCAL_D const TUid KGeneralSettingsUid = { 0X100058EC };
#endif // __TEST_USE_SHARED_DATA






_LIT( KWlanBearerName, "WLANBearer" );
_LIT( KWlanBearerAgent, "wlanagt.agt" );
_LIT( KWlanBearerNif, "wlannif" );

_LIT( KWlanLDDName, "not used" );
_LIT( KWlanPDDName, "not used" );

const TInt KWlanLastSocketActivityTimeout = -1;
const TInt KWlanLastSessionClosedTimeout = 1;
const TInt KWlanLastSocketClosedTimeout = -1;






// MACROS


// LOCAL FUNCTION PROTOTYPES


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
// ---------------------------------------------------------
// CApDataHandler::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApDataHandler* CApDataHandler::NewLC( CCommsDatabase& aDb )
    {
    return NULL;
    }



// ---------------------------------------------------------
// CApDataHandler::~CApDataHandler
// ---------------------------------------------------------
//
EXPORT_C CApDataHandler::~CApDataHandler()
    {
    }


// ---------------------------------------------------------
// CApDataHandler::CApDataHandler
// ---------------------------------------------------------
//
EXPORT_C CApDataHandler::CApDataHandler( )
    {
    }


// ---------------------------------------------------------
// CApDataHandler::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::ConstructL( CCommsDatabase& aDb )
    {
    }


// query
// ---------------------------------------------------------
// CApDataHandler::AccessPointDataL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::AccessPointDataL( TUint32 aUid,
                                              CApAccessPointItem& aApItem )
    {
    }



// update
// ---------------------------------------------------------
// CApDataHandler::UpdateAccessPointDataL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::UpdateAccessPointDataL
                                                (
                                                CApAccessPointItem& aApItem,
                                                TBool& aNameChanged
                                                )
    {
    }



// Creating new AP
// ---------------------------------------------------------
// CApDataHandler::CreateCopyFromL
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApDataHandler::CreateCopyFromL( TUint32 aBaseId )
    {
    User::Leave( KErrNotSupported );
    }



// ---------------------------------------------------------
// CApDataHandler::CreateFromDataL
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApDataHandler::CreateFromDataL( CApAccessPointItem& aApItem )
    {
    User::Leave( KErrNotSupported );
    }



// Remove
// ---------------------------------------------------------
// CApDataHandler::RemoveAP
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::RemoveAPL( TUint32 aUid )
    {
    User::Leave( KErrNotSupported );
    }


// Getting default access point
// ---------------------------------------------------------
// CApDataHandler::DefaultL
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApDataHandler::DefaultL( TBool aIsWap ) const
    {
    return 0;
    }



// Setting as default
// ---------------------------------------------------------
// CApDataHandler::SetAsDefaultL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::SetAsDefaultL( TUint32 aUid,
                                            TCommsDbIspType aIntendedType )
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// ---------------------------------------------------------
// Connection Preferene Handling
// ---------------------------------------------------------
// ---------------------------------------------------------


// ---------------------------------------------------------
// CApDataHandler::SetPreferredIfDbIapTypeL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::SetPreferredIfDbIapTypeL
                                    (
                                    TUint32 aRank,
                                    TCommDbConnectionDirection aDirection,
                                    TUint32 aBearers,
                                    TCommDbDialogPref aPrompt,
                                    TUint32 aIAP,
                                    TBool aOverwrite
                                    )
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// CApDataHandler::SetPreferredIfDbIspTypeL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::SetPreferredIfDbIspTypeL
                                    (
                                    TUint32 aRank,
                                    TCommDbConnectionDirection aDirection,
                                    TUint32 aBearers,
                                    TCommDbDialogPref aPrompt,
                                    TUint32 aISP,
                                    TUint32 aChargeCard,
                                    const TDesC& aServiceType,
                                    TBool aOverwrite
                                    )
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// CApDataHandler::GetPreferredIfDbIapTypeL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::GetPreferredIfDbIapTypeL
                                    (
                                    TUint32 aRank,
                                    TCommDbConnectionDirection aDirection,
                                    TUint32& aBearers,
                                    TUint32& aPrompt,
                                    TUint32& aIAP
                                    )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::GetPreferredIfDbIspTypeL
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CApDataHandler::GetPreferredIfDbIspTypeL
                                    (
                                    TUint32 /*aRank*/,
                                    TCommDbConnectionDirection /*aDirection*/,
                                    TUint32& /*aBearers*/,
                                    TUint32& /*aPrompt*/,
                                    TUint32& /*aISP*/,
                                    TUint32& /*aChargeCard*/
                                    )
    {
    User::Leave( KErrNotSupported );
    }



// ---------------------------------------------------------
// CApDataHandler::GetPreferredIfDbIspTypeL
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApDataHandler::DefaultL( TBool aIsWap,
                                                 TBool& aReadOnly ) const
    {
    return 0;
    }




// ---------------------------------------------------------
// CApDataHandler::RemoveNetworkL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::RemoveNetworkL( TUint32 aUid )
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// CApDataHandler::CreateNetworkL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::CreateNetworkL( CApNetworkItem& aNetwork )
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// CApDataHandler::ReadNetworkPartL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::ReadNetworkPartL( CApAccessPointItem& aItem )
    {
    }


// ---------------------------------------------------------
// CApDataHandler::ReadNetworkL
// ---------------------------------------------------------
//
EXPORT_C void CApDataHandler::ReadNetworkL( TUint32 aUid, 
                                            CApNetworkItem& aNetwork )
    {
    }


// ================= OTHER EXPORTED FUNCTIONS ==============


// ================= PROTECTED FUNCTIONS ==============

// ---------------------------------------------------------
// CApDataHandler::ReadWapDataL
// ---------------------------------------------------------
//
void CApDataHandler::ReadWapDataL( TUint32 aWapId,
                                  CApAccessPointItem& aApItem )
    {
    }


// ---------------------------------------------------------
// CApDataHandler::ReadIapDataL
// ---------------------------------------------------------
//
void CApDataHandler::ReadIapDataL( TUint32 aIapId,
                                   CApAccessPointItem& aApItem )
    {
    }





// ---------------------------------------------------------
// CApDataHandler::ReadIspDataL
// ---------------------------------------------------------
//
void CApDataHandler::ReadIspDataL( TUint32 aIspId, 
                                   CApAccessPointItem& aApItem )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::ReadGprsDataL
// ---------------------------------------------------------
//
void CApDataHandler::ReadGprsDataL( TUint32 aIspId,
                                    CApAccessPointItem& aApItem )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::ReadLanDataL
// ---------------------------------------------------------
//
void CApDataHandler::ReadLanDataL( TUint32 aIspId,
                                   CApAccessPointItem& aApItem )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::ReadDialInOutDataL
// ---------------------------------------------------------
//
void CApDataHandler::ReadDialInOutDataL( TUint32 aIspId,
                                         CApAccessPointItem& aApItem )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::ReadVpnInfoL
// ---------------------------------------------------------
//
void CApDataHandler::ReadVpnInfoL( TUint32 aIspId, 
                                   CApAccessPointItem& aApItem)
    {
    }



// ---------------------------------------------------------
// CApDataHandler::WriteWapBearerDataL
// ---------------------------------------------------------
//
void CApDataHandler::WriteWapBearerDataL( TBool aIsNew, TUint32 aWapId,
                                   CApAccessPointItem& aApItem )
    {
    }




// ---------------------------------------------------------
// CApDataHandler::WriteWapApDataL
// ---------------------------------------------------------
//
void CApDataHandler::WriteWapApDataL( TBool aIsNew, TUint32 aWapId,
                                   CApAccessPointItem& aApItem )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::WriteIapDataL
// ---------------------------------------------------------
//
void CApDataHandler::WriteIapDataL( TBool aIsNew, TUint32 aIapId,
                                   CApAccessPointItem& aApItem )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::WriteIspDataL
// ---------------------------------------------------------
//
void CApDataHandler::WriteIspDataL( TBool aIsNew, TUint32 aIspId,
                                   CApAccessPointItem& aApItem )
    {
    }




// ---------------------------------------------------------
// CApDataHandler::WriteGprsDataL
// ---------------------------------------------------------
//
void CApDataHandler::WriteGprsDataL( TBool aIsNew,
                                    CApAccessPointItem& aApItem )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::WriteLanDataL
// ---------------------------------------------------------
//
void CApDataHandler::WriteLanDataL( TBool aIsNew,
                                    CApAccessPointItem& aApItem )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::DoUpdateAccessPointDataL
// ---------------------------------------------------------
//
TUint32 CApDataHandler::DoUpdateAccessPointDataL
                                        (
                                        CApAccessPointItem& aApItem,
                                        TBool aIsNew,
                                        TBool& aNameChanged
                                        )
    {
    return 0;
    }



// ---------------------------------------------------------
// CApDataHandler::SetDefaultIapIspL
// ---------------------------------------------------------
//
void CApDataHandler::SetDefaultIapIspL( TUint32 aUid )
    {
    }





// ---------------------------------------------------------
// CApDataHandler::RemoveWapBearerApL
// ---------------------------------------------------------
//
void CApDataHandler::RemoveWapBearerApL( TUint32 aUid )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::RemoveWapApL
// ---------------------------------------------------------
//
void CApDataHandler::RemoveWapApL( TUint32 aUid )
    {
    }


// ---------------------------------------------------------
// CApDataHandler::RemoveIapL
// ---------------------------------------------------------
//
void CApDataHandler::RemoveIapL( TUint32 aUid )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::RemoveIspL
// ---------------------------------------------------------
//
void CApDataHandler::RemoveIspL( TUint32 aUid, TBool aOut )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::RemoveGprsL
// ---------------------------------------------------------
//
void CApDataHandler::RemoveGprsL( TUint32 aUid, TBool aOut )
    {
    }


// ---------------------------------------------------------
// CApDataHandler::RemoveLanL
// ---------------------------------------------------------
//
void CApDataHandler::RemoveLanL( TUint32 aUid )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::RemoveWlanL
// ---------------------------------------------------------
//
void CApDataHandler::RemoveWlanL( TUint32 aUid )
    {
    User::Leave( KErrNotSupported );
    }



// ---------------------------------------------------------
// CApDataHandler::HandleBearerChangeIfNeededL
// ---------------------------------------------------------
//
TBool CApDataHandler::HandleBearerChangeIfNeededL
                                    ( CApAccessPointItem& aApItem )
    {
    return EFalse;
    }





// ---------------------------------------------------------
// CApDataHandler::DoAccessPointDataL
// ---------------------------------------------------------
//
void CApDataHandler::DoAccessPointDataL( TUint32 aUid,
                                         CApAccessPointItem& aApItem )
    {
    }


// ---------------------------------------------------------
// CApDataHandler::DoRemoveAPL
// ---------------------------------------------------------
//
void CApDataHandler::DoRemoveAPL( TUint32 aUid )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::DoSetAsDefaultL
// ---------------------------------------------------------
//
void CApDataHandler::DoSetAsDefaultL( TUint32 aUid,
                                      TCommsDbIspType aIntendedType )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::DoSetPreferredIfDbIapTypeL
// ---------------------------------------------------------
//
void CApDataHandler::DoSetPreferredIfDbIapTypeL(
                                 TUint32 aRank,
                                 TCommDbConnectionDirection aDirection,
                                 TUint32 aBearers,
                                 TCommDbDialogPref aPrompt,
                                 TUint32 aIAP,
                                 TBool aOverwrite
                                    )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::DoSetPreferredIfDbIspTypeL
// ---------------------------------------------------------
//
void CApDataHandler::DoSetPreferredIfDbIspTypeL(
                                 TUint32 /*aRank*/,
                                 TCommDbConnectionDirection /*aDirection*/,
                                 TUint32 /*aBearers*/,
                                 TCommDbDialogPref /*aPrompt*/,
                                 TUint32 /*aISP*/,
                                 TUint32 /*aChargeCard*/,
                                 const TDesC& /*aServiceType*/,
                                 TBool /*aOverwrite*/
                                    )
    {
    User::Leave( KErrNotSupported );
    }



// ---------------------------------------------------------
// CApDataHandler::ApSpeed2EtelL
// ---------------------------------------------------------
//
void CApDataHandler::ApSpeed2EtelL( CApAccessPointItem* aItem )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::EtelSpeed2ApL
// ---------------------------------------------------------
//
void CApDataHandler::EtelSpeed2ApL( CApAccessPointItem& aItem )
    {
    }



// Getting default access point
// ---------------------------------------------------------
// CApDataHandler::DoGetDefaultL
// ---------------------------------------------------------
//
TUint32 CApDataHandler::DoGetDefaultL( TBool aIsWap,
                                             TBool* aReadOnly ) const
    { // client MUST initiate a transaction, we will NOT !
    return KErrNotSupported;
    }


// ---------------------------------------------------------
// CApDataHandler::WriteProxyDataL
// ---------------------------------------------------------
//
void CApDataHandler::WriteProxyDataL( TBool aIsNew, 
                                      const CApAccessPointItem& aApItem )
    {
    }


// ---------------------------------------------------------
// CApDataHandler::ReadProxyDataL
// ---------------------------------------------------------
//
void CApDataHandler::ReadProxyDataL( CApAccessPointItem& aApItem )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::RemoveProxyDataL
// ---------------------------------------------------------
//
void CApDataHandler::RemoveProxyDataL( HBufC* aServiceType, TUint32 aIspId )
    {
    }




// ---------------------------------------------------------
// CApDataHandler::WriteNetworkDataL
// ---------------------------------------------------------
//
void CApDataHandler::WriteNetworkDataL( TBool aIsNew, 
                                        CApAccessPointItem& aApItem )
    {
    }


// ---------------------------------------------------------
// CApDataHandler::ReadNetworkDataL
// ---------------------------------------------------------
//
void CApDataHandler::ReadNetworkDataL( CApAccessPointItem& aApItem )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::RemoveNetworkDataL
// ---------------------------------------------------------
//
void CApDataHandler::RemoveNetworkDataL( TUint32 aIspId )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::GetLocationIdL
// ---------------------------------------------------------
//
TUint32 CApDataHandler::GetLocationIdL()
    {
    return 0;    
    }



// ---------------------------------------------------------
// CApDataHandler::GetModemBearerIDL
// ---------------------------------------------------------
//
TUint32 CApDataHandler::GetModemBearerIDL( CApAccessPointItem& aApItem )
    {
  	User::Leave( KErrInvalidBearerType );
    }




// ---------------------------------------------------------
// CApDataHandler::ReadServiceL
// ---------------------------------------------------------
//
void CApDataHandler::ReadServiceL( CCommsDbTableView& ispt,
                                   CApAccessPointItem& aApItem )
    {
    }


// ---------------------------------------------------------
// CApDataHandler::ReadServiceIp6L
// ---------------------------------------------------------
//
void CApDataHandler::ReadServiceIp6L( CCommsDbTableView& ispt,
                                   CApAccessPointItem& aApItem )
    {
    }




// ---------------------------------------------------------
// CApDataHandler::WriteServiceL
// ---------------------------------------------------------
//
void CApDataHandler::WriteServiceL( CCommsDbTableView& aTable,
                                    CApAccessPointItem& aApItem )
    {
    }


// ---------------------------------------------------------
// CApDataHandler::WriteServiceIp6L
// ---------------------------------------------------------
//
void CApDataHandler::WriteServiceIp6L( CCommsDbTableView& ispt,
                                       CApAccessPointItem& aApItem )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::WriteCsdDataL
// ---------------------------------------------------------
//
void CApDataHandler::WriteCsdDataL( TBool aIsNew,
                                    CApAccessPointItem& aApItem )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::ReadWlanL
// ---------------------------------------------------------
//
void CApDataHandler::ReadWlanL( CCommsDbTableView& lant,
                                CApAccessPointItem& aApItem )
    {
    }



// ---------------------------------------------------------
// CApDataHandler::WriteWlanL
// ---------------------------------------------------------
//
void CApDataHandler::WriteWlanL( CCommsDbTableView& aTable,
                                 CApAccessPointItem& aApItem )
    {
    }




// ---------------------------------------------------------
// CApDataHandler::CreateLanBearerIfNeededL
// ---------------------------------------------------------
//
TUint32 CApDataHandler::CreateLanBearerIfNeededL()
    {
    return KErrNotFound;
    }






// ---------------------------------------------------------
// CApDataHandler::ReadModemBearerNameL
// ---------------------------------------------------------
//
void CApDataHandler::ReadModemBearerNameL( CApAccessPointItem& aApItem )
    {
    }

//  End of File
