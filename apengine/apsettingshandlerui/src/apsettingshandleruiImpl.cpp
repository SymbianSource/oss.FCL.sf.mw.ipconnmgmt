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
*     Defines the main handler, CApSettingsHandlerImpl for 
*       the Access Point settings.
*
*/


// INCLUDE FILES
#include "APSettingsHandlerUIVariant.hrh"

#include <bautils.h>
#include <StringLoader.h>
#include <apsetui.rsg>
#include <ActiveApDb.h>

#include "ApsettingshandleruiImpl.h"
#include "ApSettingsHandlerUI.hrh"
#include "ApSelectorDialog.h"

#include "ApSettingsDlg.h"

#include "TextOverrides.h"
#include "ApSettingsModel.h"
#include "ApSelPopupList.h"
#include "ApSelQueryDialog.h"

#include <aknnotedialog.h>
#include <ApListItemList.h>

#include <f32file.h>

#include "ApSettingsHandlerLogger.h"
#include <ApSettingsHandlerCommons.h>


#include "ApSettingsHandlerUiPrivateCRKeys.h"
#include <centralrepository.h>

#include <sysutil.h>

#include <ApNetworkItem.h>

#include <featmgr.h>
#include <VpnApEngine.h>

#include <data_caging_path_literals.hrh>

#include <shareddataclient.h>

// UID of general settings app, in which help texts are included
const   TUid    KHelpUidApSettingsHandler = { 0x100058EC };


//#define __TEST_PERFORMANCE
/// ROM drive.
_LIT( KApSettingsResDriveZ, "z:" );
/// Access point settings handler UI resource file name.
_LIT( KApSettingsResFileName, "apsetui.rsc" );



/// Estimated FFS overhead for deleting (in bytes).
LOCAL_C const TInt KEstimatedDeleteFfsOverhead = 48 * 1024;



//KFeatureIdIPv6
// #define KFeatureIdIPv6 KOtherFeatureFirst+88

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CApSettingsHandlerImpl::NewLC
// ---------------------------------------------------------
//
CApSettingsHandlerImpl* CApSettingsHandlerImpl::NewLC(
                                         TBool aStartWithSelection,
                                         TSelectionListType aListType,
                                         TSelectionMenuType aSelMenuType,
                                         TInt aIspFilter,
                                         TInt aBearerFilter,
                                         TInt aSortType,
                                         TBool aNoEdit )
    {
    return NULL;
    }




// ---------------------------------------------------------
// CApSettingsHandlerImplImpl::NewLC
// ---------------------------------------------------------
//
CApSettingsHandlerImpl* CApSettingsHandlerImpl::NewLC(
                                            CActiveApDb& aDb,
                                            TBool aStartWithSelection,
                                            TSelectionListType aListType,
                                            TSelectionMenuType aSelMenuType,
                                            TInt aIspFilter,
                                            TInt aBearerFilter,
                                            TInt aSortType,
                                            TBool aNoEdit
                                            )
    {
    return NULL;
    }



// ---------------------------------------------------------
// CApSettingsHandlerImpl::NewLC
// ---------------------------------------------------------
//
CApSettingsHandlerImpl* CApSettingsHandlerImpl::NewLC(
                                         TBool aStartWithSelection,
                                         TSelectionListType aListType,
                                         TSelectionMenuType aSelMenuType,
                                         TInt aIspFilter,
                                         TInt aBearerFilter,
                                         TInt aSortType,
                                         TInt aReqIpvType,
                                         TBool aNoEdit
                                         )
    {
    return NULL;
    }




// ---------------------------------------------------------
// CApSettingsHandlerImpl::NewLC
// ---------------------------------------------------------
//
CApSettingsHandlerImpl* CApSettingsHandlerImpl::NewLC(
                                            CActiveApDb& aDb,
                                            TBool aStartWithSelection,
                                            TSelectionListType aListType,
                                            TSelectionMenuType aSelMenuType,
                                            TInt aIspFilter,
                                            TInt aBearerFilter,
                                            TInt aSortType,
                                            TInt aReqIpvType,
                                            TBool aNoEdit
                                            )
    {
    return NULL;
    }





// ---------------------------------------------------------
// CApSettingsHandlerImpl::NewLC
// ---------------------------------------------------------
//
CApSettingsHandlerImpl* CApSettingsHandlerImpl::NewLC(
                                        TBool aStartWithSelection,
                                        TSelectionListType aListType,
                                        TSelectionMenuType aSelMenuType,
                                        TInt aIspFilter,
                                        TInt aBearerFilter,
                                        TInt aSortType,
                                        TInt aReqIpvType,
                                        TVpnFilterType aVpnFilterType,
                                        TBool aNoEdit
                                            )
    {
    return NULL;
    }


// ---------------------------------------------------------
// CApSettingsHandlerImpl::NewLC
// ---------------------------------------------------------
//
CApSettingsHandlerImpl* CApSettingsHandlerImpl::NewLC(
                                            CActiveApDb& aDb,
                                            TBool aStartWithSelection,
                                            TSelectionListType aListType,
                                            TSelectionMenuType aSelMenuType,
                                            TInt aIspFilter,
                                            TInt aBearerFilter,
                                            TInt aSortType,
                                            TInt aReqIpvType,
                                            TVpnFilterType aVpnFilterType,
                                            TBool aNoEdit
                                            )
    {
    return NULL;
    }






// ---------------------------------------------------------
// CApSettingsHandlerImpl::NewLC
// ---------------------------------------------------------
//
CApSettingsHandlerImpl* CApSettingsHandlerImpl::NewLC(
                                        CActiveApDb& aDb,
                                        TBool aStartWithSelection,
                                        TSelectionListType aListType,
                                        TSelectionMenuType aSelMenuType,
                                        TInt aIspFilter,
                                        TInt aBearerFilter,
                                        TInt aSortType,
                                        TInt aReqIpvType,
                                        TVpnFilterType aVpnFilterType,
                                        TBool aIncludeEasyWlan,
                                        TBool aNoEdit
                                        )
    {
    return NULL;    
    }



// Destructor
CApSettingsHandlerImpl::~CApSettingsHandlerImpl()
    {
    }


// Constructor
CApSettingsHandlerImpl::CApSettingsHandlerImpl( TBool aStartWithSelection,
                                                TSelectionListType aListType,
                                                TSelectionMenuType aSelMenuType,
                                                TBool aNoEdit
                                                )
:iStartWithSelection( aStartWithSelection ),
iListType( aListType ),
iSelMenuType( aSelMenuType ),
iIspFilter( KEApIspTypeAll ),
iBearerFilter( EApBearerTypeAll ),
iReqIpvType( EIPv4 ),
iEventStore( KApUiEventNone ),
iNoEdit( aNoEdit )
    {
    }





// Constructor
CApSettingsHandlerImpl::CApSettingsHandlerImpl( TBool aStartWithSelection,
                                        TSelectionListType aListType,
                                        TSelectionMenuType aSelMenuType,
                                        TInt aReqIpvType,
                                        TBool aNoEdit 
                                        )
:iStartWithSelection( aStartWithSelection ),
iListType( aListType ),
iSelMenuType( aSelMenuType ),
iIspFilter( KEApIspTypeAll ),
iBearerFilter( EApBearerTypeAll ),
iReqIpvType( aReqIpvType ),
iEventStore( KApUiEventNone ),
iNoEdit( aNoEdit )
    {
    }



// ---------------------------------------------------------
// CApSettingsHandlerImpl::ConstructL
// ---------------------------------------------------------
//
void CApSettingsHandlerImpl::ConstructL(   TInt aIspFilter,
                                                TInt aBearerFilter,
                                                TInt aSortType )
    {
    }



// ---------------------------------------------------------
// CApSettingsHandlerImpl::ConstructL
// ---------------------------------------------------------
//
void CApSettingsHandlerImpl::ConstructL(   CActiveApDb& aDb,
                                                TInt aIspFilter,
                                                TInt aBearerFilter,
                                                TInt aSortType )
    {
    }



// ---------------------------------------------------------
// CApSettingsHandlerImpl::ConstructL
// ---------------------------------------------------------
//
void CApSettingsHandlerImpl::ConstructL(   
                                             TInt aIspFilter,
                                             TInt aBearerFilter,
                                             TInt aSortType,
                                             TVpnFilterType aVpnFilterType
                                            )
    {
    }



// ---------------------------------------------------------
// CApSettingsHandlerImpl::ConstructL
// ---------------------------------------------------------
//
void CApSettingsHandlerImpl::ConstructL(
                                             CActiveApDb& aDb,
                                             TInt aIspFilter,
                                             TInt aBearerFilter,
                                             TInt aSortType,
                                             TVpnFilterType aVpnFilterType
                                             )
    {
    }




// ---------------------------------------------------------
// CApSettingsHandlerImpl::ConstructL
// ---------------------------------------------------------
//
void CApSettingsHandlerImpl::ConstructL(
                                        CActiveApDb& aDb,
                                        TInt aIspFilter,
                                        TInt aBearerFilter,
                                        TInt aSortType,
                                        TVpnFilterType aVpnFilterType,
                                        TBool aIncludeEasyWlan
                                        )
    {
    }


// ---------------------------------------------------------
// CApSettingsHandlerImpl::SetFilter
// ---------------------------------------------------------
//
void CApSettingsHandlerImpl::SetFilterL(   TInt aIspFilter,
                                                TInt aBearerFilter,
                                                TInt aSortType )
    {
    }




// ---------------------------------------------------------
// CApSettingsHandlerImpl::SetTextOverrideL
// ---------------------------------------------------------
//
void CApSettingsHandlerImpl::SetTextOverrideL( TTextID aText2Change,
                                                    const TDesC& aNewText )
    {
    }


// ---------------------------------------------------------
// CApSettingsHandlerImpl::TextOverrideL
// ---------------------------------------------------------
//
const TDesC& CApSettingsHandlerImpl::TextOverrideL( TTextID aTextId )
    {
    User::Leave( KErrNotSupported );
    }



// ---------------------------------------------------------
// CApSettingsHandlerImpl::RunSettingsL
// ---------------------------------------------------------
//
TInt CApSettingsHandlerImpl::RunSettingsL( TUint32 aHighLight,
                                                TUint32& aSelected )
    {    
    return 0;
    }



// ---------------------------------------------------------
// CApSettingsHandlerImpl::Cvt()
// ---------------------------------------------------------
//
TInt CApSettingsHandlerImpl::Cvt()
    {
    return KErrNotSupported;
    }


// ---------------------------------------------------------
// CApSettingsHandlerImpl::DoRunSettingsL
// ---------------------------------------------------------
//
TInt CApSettingsHandlerImpl::DoRunSettingsL( TUint32 aHighLight,
                                         TUint32& aSelected )
    {
    return 0;
    }


// ---------------------------------------------------------
// CApSettingsHandlerImpl::RunViewerL
// ---------------------------------------------------------
//
TInt CApSettingsHandlerImpl::RunViewerL( TUint32 aUid )
    {
    return 0;
    }



// ---------------------------------------------------------
// CApSettingsHandlerImpl::DoRunViewerL
// ---------------------------------------------------------
//
TInt CApSettingsHandlerImpl::DoRunViewerL( TUint32 aUid )
    {
    return 0;
    }



//----------------------------------------------------------
// CApSettingsHandlerImpl::HandleApDeleteCmdL
//----------------------------------------------------------
//
void CApSettingsHandlerImpl::HandleApDeleteCmdL( TUint32 aUid, TBool aIsLast )
    {
    }


//----------------------------------------------------------
// CApSettingsHandlerImpl::HandleApBlankNewL
//----------------------------------------------------------
//
TUint32 CApSettingsHandlerImpl::HandleApBlankNewL( TInt aBearers, TInt aIsps )
    {
    return 0;
    }




//----------------------------------------------------------
// CApSettingsHandlerImpl::HandleApUseOldNewL
//----------------------------------------------------------
//
TUint32 CApSettingsHandlerImpl::HandleApUseOldNewL()
    {
    return 0;
    }



//----------------------------------------------------------
// CApSettingsHandlerImpl::EditAfterCreateL
//----------------------------------------------------------
//
TUint32 CApSettingsHandlerImpl::EditAfterCreateL( TUint32 aUid,
                                              CApAccessPointItem* aItem )
    {
    return 0;
    }



//----------------------------------------------------------
// CApSettingsHandlerImpl::DoConstructL
//----------------------------------------------------------
//
void CApSettingsHandlerImpl::DoConstructL( CActiveApDb* aDb,
                                       TInt aIspFilter,
                                       TInt aBearerFilter,
                                       TInt aSortType,
                                       TVpnFilterType aVpnFilterType,
                                       TBool aIncludeEasyWlan )
    {
    }




//----------------------------------------------------------
// CApSettingsHandlerImpl::DoRunSetPageSettingsL
//----------------------------------------------------------
//
void CApSettingsHandlerImpl::DoRunSetPageSettingsL( TUint32 aHighLight,
                                                TUint32& aSelected )
    {
    }



//----------------------------------------------------------
// CApSettingsHandlerImpl::DoRunListSettingsL
//----------------------------------------------------------
//
void CApSettingsHandlerImpl::DoRunListSettingsL( TUint32 aHighLight,
                                             TUint32& aSelected )
    {
    }



//----------------------------------------------------------
// CApSettingsHandlerImpl::DoHandleApUseOldNewL
//----------------------------------------------------------
//
TUint32 CApSettingsHandlerImpl::DoHandleApUseOldNewL( TUint32 aSelected )
    {
    return KErrNotSupported;
    }



//----------------------------------------------------------
// CApSettingsHandlerImpl::SelectApToCopyL
//----------------------------------------------------------
//
TUint32 CApSettingsHandlerImpl::SelectApToCopyL()
    {
    return KErrNotSupported;
    }





//----------------------------------------------------------
// CApSettingsHandlerImpl::DoHandleApBlankNewL
//----------------------------------------------------------
//
TUint32 CApSettingsHandlerImpl::DoHandleApBlankNewL( TInt aBearers, TInt aIsps )
    {
    return 0;
    }




//----------------------------------------------------------
// CApSettingsHandlerImpl::HandleNetworkDeleteCmdL
//----------------------------------------------------------
//
void CApSettingsHandlerImpl::HandleNetworkDeleteCmdL( TUint32 aUid )
    {
    }



//----------------------------------------------------------
// CApSettingsHandlerImpl::GetVariantL
//----------------------------------------------------------
//
TInt CApSettingsHandlerImpl::GetVariantL()
    {
    APSETUILOGGER_ENTERFN( EApImpl,"Impl::GetVariantL")
    
    TInt variant( 0 );
    // Connecting and initialization:
    CRepository* repository = 
            CRepository::NewL( KCRUidApSettingsHandlerUiLV );
    repository->Get( KApSettingsHandlerUiLVFlags, variant );
    delete repository;

    APSETUILOGGER_LEAVEFN( EApImpl,"Impl::GetVariantL")
    return variant;
    }


//----------------------------------------------------------
// CApSettingsHandlerImpl::ConfirmDeleteL
//----------------------------------------------------------
//
TInt CApSettingsHandlerImpl::ConfirmDeleteL( TUint32 aUid, TBool aIsLast )
    {
    return 0;
    }





//----------------------------------------------------------
// CApSettingsHandlerImpl::HandleRunSettingsL
//----------------------------------------------------------
//
void CApSettingsHandlerImpl::HandleRunSettingsL( TUint32 aHighLight, 
                                             TUint32& aSelected )
    {
    }


//----------------------------------------------------------
// CApSettingsHandlerImpl::ChooseBearerTypeL
//----------------------------------------------------------
//
void CApSettingsHandlerImpl::ChooseBearerTypeL( TInt aBearers, 
                                            CApAccessPointItem& aItem )
    {
    }




//----------------------------------------------------------
// CApSettingsHandlerImpl::HandleDuplicateL
//----------------------------------------------------------
//
TUint32 CApSettingsHandlerImpl::HandleDuplicateL( TUint32 aUid )
    {
    return KErrNotSupported;
    }


// End of File

