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
* Description:  Implementation of CApSettingsModel.
*
*/


// INCLUDE FILES
#include "APSettingsHandlerUIVariant.hrh"

#include <ActiveApDb.h>
#include <ApListItemList.h>
#include <ApListItem.h>
#include <ApSelect.h>
#include <apsetui.rsg>
#include <VpnApEngine.h>
#include <hlplch.h>
#include <eikenv.h>
#include <eikappui.h>
#include <StringLoader.h>
#include <featmgr.h>

#include "ApSettingsHandlerUI.hrh"

#include "ApSettingsModel.h"
#include <ApSettingsHandlerCommons.h>
#include "Apsettingsdeleteoperation.h"

#include <sysutil.h>
#include <ErrorUI.h>


#include "ApNetworks.h"
#include "FeatureManagerWrapper.h"

#include <ApAccessPointItem.h>

#include "ApSettingsHandlerLogger.h"

//#define __TEST_PERFORMANCE

// CONSTANTS




/**
* Estimated number of bytes in FFS for one writing operation. This amount
* is checked to be available before each transaction which places write-lock.
* Exact size is not known (because what happens in a transaction is unknown).
* A typical access point is estimated to be less than ~2000 bytes; 
* so assuming the typical operation of adding one access point 
* (with database transaction overhead and safety) is estimated 
* to be 2000 bytes. This should be a safe guess.
*/
LOCAL_C const TInt KEstimatedFfsUsage = 2000;



// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CApSettingsModel::NewL
// ---------------------------------------------------------
//
CApSettingsModel* CApSettingsModel::NewL( TInt aIspFilter, TInt aBearerFilter,
                                          TInt aSortType, TInt aReqIpvType,
                                          TVpnFilterType aVpnFilterType,
                                          TInt aVariant,
                                          TBool aIncludeEasyWlan,
                                          TBool aNoEdit )
    {
    return NULL;
    }



// ---------------------------------------------------------
// CApSettingsModel::NewL
// ---------------------------------------------------------
//
CApSettingsModel* CApSettingsModel::NewL( CActiveApDb& aDb, TInt aIspFilter,
                                          TInt aBearerFilter, TInt aSortType,
                                          TInt aReqIpvType, 
                                          TVpnFilterType aVpnFilterType,
                                          TInt aVariant,
                                          TBool aIncludeEasyWlan,
                                          TBool aNoEdit )
    {
    return NULL;
    }


/**
* Destructor.
*/
CApSettingsModel::~CApSettingsModel( )
    {
    }


/**
* Constructor
*/
CApSettingsModel::CApSettingsModel( TInt aIspFilter, TInt aBearerFilter, 
                                    TInt aSortType, TInt aReqIpvType,
                                    TInt aVariant,
                                    TVpnFilterType aVpnFilterType,
                                    TBool aIncludeEasyWlan,
                                    TBool aNoEdit )

    {
    }



// ---------------------------------------------------------
// CApSettingsModel::ConstructL
// ---------------------------------------------------------
//
void CApSettingsModel::ConstructL( CActiveApDb* aDb )
    {
    }



// ---------------------------------------------------------
// CApSettingsModel::Database
// ---------------------------------------------------------
//
CActiveApDb* CApSettingsModel::Database()
    {
    return NULL;
    }




// ---------------------------------------------------------
// CApSettingsModel::AllListItemDataL
// ---------------------------------------------------------
//
TInt CApSettingsModel::AllListItemDataL( TBool& aFailedLocked, 
                                         CApListItemList& aList, 
                                         TInt aIspFilter, TInt aBearerFilter,
                                         TInt aSortType, TInt aReqIpvType )
    {
    return 0;
    }



// ---------------------------------------------------------
// CApSettingsModel::AllListItemDataL
// ---------------------------------------------------------
//
TInt CApSettingsModel::AllListItemDataL( 
                                        TBool& aFailedLocked,
                                        CApListItemList& aList,
                                        TInt aIspFilter,
                                        TInt aBearerFilter,
                                        TInt aSortType,
                                        TInt aReqIpvType,
                                        TVpnFilterType aVpnFilterType,
                                        TBool aIncludeEasyWlan
                                        )
    {
    return 0;
    }



// ---------------------------------------------------------
// CApSettingsModel::AllListItemDataL
// ---------------------------------------------------------
//
TInt CApSettingsModel::AllListItemDataL( TBool& aFailedLocked, 
                                         CApListItemList& aList )
    {
    return 0;
    }



// ---------------------------------------------------------
// CApSettingsModel::AccessPointDataL
// ---------------------------------------------------------
//
void CApSettingsModel::AccessPointDataL( TUint32 aUid, 
                                         CApAccessPointItem& aApItem )
    {
    }



// ---------------------------------------------------------
// CApSettingsModel::UpdateAccessPointDataL
// ---------------------------------------------------------
//
TBool CApSettingsModel::UpdateAccessPointDataL( CApAccessPointItem& aApItem )
    {
    return EFalse;
    }




// ---------------------------------------------------------
// CApSettingsModel::DataHandler
// ---------------------------------------------------------
//
CApDataHandler* CApSettingsModel::DataHandler()
    {
    return NULL;
    }



// ---------------------------------------------------------
// CApSettingsModel::ApUtils
// ---------------------------------------------------------
//
CApUtils* CApSettingsModel::ApUtils()
    {
    return NULL;
    }


// ---------------------------------------------------------
// CApSettingsModel::SetParams
// ---------------------------------------------------------
//
void CApSettingsModel::SetParams( TApSettingsParams aParams )
    {
    }


// ---------------------------------------------------------
// CApSettingsModel::Params
// ---------------------------------------------------------
//
TApSettingsParams CApSettingsModel::Params()
    {
    User::Leave( KErrNotSupported );
    return iParams;
    }



// ---------------------------------------------------------
// CApSettingsModel::VpnEngine
// ---------------------------------------------------------
//
CVpnApEngine* CApSettingsModel::VpnEngine()
    {
    return NULL;
    }


// ---------------------------------------------------------
// CApSettingsModel::LaunchHelpL
// ---------------------------------------------------------
//
void CApSettingsModel::LaunchHelpL()
    {
    }


// ---------------------------------------------------------
// CApSettingsModel::RemoveApL
// ---------------------------------------------------------
//
void CApSettingsModel::RemoveApL( TUint32 aUid )
    {
    }




// ---------------------------------------------------------
// CApSettingsModel::StarTransactionLC
// ---------------------------------------------------------
//
TTransactionResult CApSettingsModel::StartTransactionLC(
                        TBool aWrite, TBool aShowNote,
                        TBool aRetry, TBool aLFFSChecking /* ETrue */ )
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// CApSettingsModel::GetWriteLockL
// ---------------------------------------------------------
//
void CApSettingsModel::GetWriteLockL( )
    {
    }

// ---------------------------------------------------------
// CApSettingsModel::RollbackTransactionOnLeave
// ---------------------------------------------------------
//
void CApSettingsModel::RollbackTransactionOnLeave( TAny* aDb )
    {
    }



// ---------------------------------------------------------
// CApSettingsModel::CommitTransaction
// ---------------------------------------------------------
//
TInt CApSettingsModel::CommitTransaction()
    {
    return KErrNotSupported;
    }



// ---------------------------------------------------------
// CApSettingsModel::FFSSpaceBelowCriticalLevelL
// ---------------------------------------------------------
//
TBool CApSettingsModel::FFSSpaceBelowCriticalLevelL
        ( TBool aShowErrorNote, TInt aBytesToWrite /*=0*/ )
    {
    return EFalse;
    }




// ---------------------------------------------------------
// CApSettingsModel::NetWorkListDataL
// ---------------------------------------------------------
//
TInt CApSettingsModel::NetWorkListDataL( TBool& aFailedLocked, 
                                         CApNetworkItemList& aList )
    {
    return KErrNotSupported;
    }



// ---------------------------------------------------------
// CApSettingsModel::RequestedIPvType
// ---------------------------------------------------------
//
TInt CApSettingsModel::RequestedIPvType( )
    {
    return 0;
    }




// ---------------------------------------------------------
// CApSettingsModel::Need2DeleteSelect
// called when menu needs to be displayed
// ---------------------------------------------------------
//
TBool CApSettingsModel::Need2DeleteSelect(  TInt aResourceId, TInt aCount )
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApSettingsModel::Need2DeleteOpen
// called when menu needs to be displayed
// ---------------------------------------------------------
//
TBool CApSettingsModel::Need2DeleteOpen(  TInt aResourceId, TInt aCount, 
                                          TUint32 aUid, TBool aNeedsNone )
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApSettingsModel::Need2DeleteDeleteL
// called when menu needs to be displayed
// ---------------------------------------------------------
//
TBool CApSettingsModel::Need2DeleteDeleteL(  TInt aResourceId, TInt aCount, 
                                             TUint32 aUid, TBool aNeedsNone )
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApSettingsModel::Need2DeleteNewUseExisting
// called when menu needs to be displayed
// ---------------------------------------------------------
//
TBool CApSettingsModel::Need2DeleteNewUseExisting(  
                                            TInt aResourceId, 
                                            TInt aCount )
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApSettingsModel::Need2DeleteNewL
// called when menu needs to be displayed
// ---------------------------------------------------------
//
TBool CApSettingsModel::Need2DeleteNewL(  TInt aResourceId )
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApSettingsModel::Need2DeleteHelp
// called when menu needs to be displayed
// ---------------------------------------------------------
//
TBool CApSettingsModel::Need2DeleteHelp(  TInt aResourceId )
    {
    return EFalse;
    }









// ---------------------------------------------------------
// CApSettingsModel::GetLinkedVpnAccessPointLC
// ---------------------------------------------------------
//
HBufC* CApSettingsModel::GetLinkedVpnAccessPointLC( TUint32 aUid )
    {
    return NULL;
    }



// ---------------------------------------------------------
// CApSettingsModel::EikEnv
// ---------------------------------------------------------
//
CEikonEnv* CApSettingsModel::EikEnv()
    {
    return NULL;
    }



// ---------------------------------------------------------
// CApSettingsModel::CreateFromDataL
// ---------------------------------------------------------
//
TUint32 CApSettingsModel::CreateFromDataL( CApAccessPointItem& aApItem )
    {
    return 0;
    }





// ---------------------------------------------------------
// CApSettingsModel::ChangeWepSettingsL
// ---------------------------------------------------------
//
TInt CApSettingsModel::ChangeWepSettingsL( CApAccessPointItem* aApItem )
    {
    return KErrNotSupported;
    }



// ---------------------------------------------------------
// CApSettingsModel::ChangeWpaSettingsL
// ---------------------------------------------------------
//
TInt CApSettingsModel::ChangeWpaSettingsL( CApAccessPointItem* aApItem )
    {
    return KErrNotSupported;
    }



// ---------------------------------------------------------
// CApSettingsModel::Change8021xSettingsL
// ---------------------------------------------------------
//
TInt CApSettingsModel::Change8021xSettingsL( CApAccessPointItem* aApItem )
    {
    return KErrNotSupported;
    }



// ---------------------------------------------------------
// CApSettingsModel::ClearWEPAndWPASettings
// ---------------------------------------------------------
//
void CApSettingsModel::ClearWEPAndWPASettings()
    {
    }


// ---------------------------------------------------------
// CApSettingsModel::WriteWlanL
// ---------------------------------------------------------
//
void CApSettingsModel::WriteWlanL( CApAccessPointItem& aApItem,
                                   TBool aIsNew )
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// CApSettingsModel::LoadWlanL
// ---------------------------------------------------------
//
void CApSettingsModel::LoadWlanL( CApAccessPointItem& aApItem )
    {
    User::Leave( KErrNotSupported );
    }


//----------------------------------------------------------
// CApSettingsModel::HasWlanSecSettingsFilledL
//----------------------------------------------------------
//
TBool CApSettingsModel::HasWlanSecSettingsFilledL( CApAccessPointItem& aApItem )
    {
    return EFalse;
    }


//----------------------------------------------------------
// CApSettingsModel::UpdateSecurityModeL
//----------------------------------------------------------
//
void CApSettingsModel::UpdateSecurityModeL( CApAccessPointItem& aApItem )
    {
    User::Leave( KErrNotSupported );
    }

// End of File
