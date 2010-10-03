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
*     Defines the main handler, CApSettingsHandler and public API for 
*       the Access Point settings.
*
*/


// INCLUDE FILES
#include <ActiveApDb.h>

#include "ApSettingsHandlerUI.h"
#include "ApsettingshandleruiImpl.h"
#include "ApSettingsHandlerLogger.h"
#include <data_caging_path_literals.hrh>


// UID of general settings app, in which help texts are included
const   TUid    KHelpUidApSettingsHandler = { 0x100058EC };


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CApSettingsHandler::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApSettingsHandler* CApSettingsHandler::NewLC(
                                         TBool aStartWithSelection,
                                         TSelectionListType aListType,
                                         TSelectionMenuType aSelMenuType,
                                         TInt aIspFilter,
                                         TInt aBearerFilter,
                                         TInt aSortType )
    {
    return NULL;
    }




// ---------------------------------------------------------
// CApSettingsHandler::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApSettingsHandler* CApSettingsHandler::NewLC(
                                            CActiveApDb& aDb,
                                            TBool aStartWithSelection,
                                            TSelectionListType aListType,
                                            TSelectionMenuType aSelMenuType,
                                            TInt aIspFilter,
                                            TInt aBearerFilter,
                                            TInt aSortType
                                            )
    {
    return NULL;
    }



// ---------------------------------------------------------
// CApSettingsHandler::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApSettingsHandler* CApSettingsHandler::NewLC(
                                         TBool aStartWithSelection,
                                         TSelectionListType aListType,
                                         TSelectionMenuType aSelMenuType,
                                         TInt aIspFilter,
                                         TInt aBearerFilter,
                                         TInt aSortType,
                                         TInt aReqIpvType
                                         )
    {
    return NULL;
    }




// ---------------------------------------------------------
// CApSettingsHandler::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApSettingsHandler* CApSettingsHandler::NewLC(
                                            CActiveApDb& aDb,
                                            TBool aStartWithSelection,
                                            TSelectionListType aListType,
                                            TSelectionMenuType aSelMenuType,
                                            TInt aIspFilter,
                                            TInt aBearerFilter,
                                            TInt aSortType,
                                            TInt aReqIpvType
                                            )
    {
    return NULL;
    }





// ---------------------------------------------------------
// CApSettingsHandler::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApSettingsHandler* CApSettingsHandler::NewLC(
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


// ---------------------------------------------------------
// CApSettingsHandler::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApSettingsHandler* CApSettingsHandler::NewLC(
                                            CActiveApDb& aDb,
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






// ---------------------------------------------------------
// CApSettingsHandler::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApSettingsHandler* CApSettingsHandler::NewLC(
                                        CActiveApDb& aDb,
                                        TBool aStartWithSelection,
                                        TSelectionListType aListType,
                                        TSelectionMenuType aSelMenuType,
                                        TInt aIspFilter,
                                        TInt aBearerFilter,
                                        TInt aSortType,
                                        TInt aReqIpvType,
                                        TVpnFilterType aVpnFilterType,
                                        TBool aIncludeEasyWlan
                                        )
    {
    return NULL;    
    }


// Destructor
EXPORT_C CApSettingsHandler::~CApSettingsHandler()
    {
    }



// Constructor
EXPORT_C CApSettingsHandler::CApSettingsHandler( TBool aStartWithSelection,
                                                TSelectionListType aListType,
                                                TSelectionMenuType aSelMenuType
                                                )
:iStartWithSelection( aStartWithSelection ),
iListType( aListType ),
iSelMenuType( aSelMenuType ),
iIspFilter( KEApIspTypeAll ),
iBearerFilter( EApBearerTypeAll ),
iReqIpvType( EIPv4 ),
iEventStore( KApUiEventNone )
    {
    }





// Constructor
CApSettingsHandler::CApSettingsHandler( TBool aStartWithSelection,
                                        TSelectionListType aListType,
                                        TSelectionMenuType aSelMenuType,
                                        TInt aReqIpvType
                                        )
:iStartWithSelection( aStartWithSelection ),
iListType( aListType ),
iSelMenuType( aSelMenuType ),
iIspFilter( KEApIspTypeAll ),
iBearerFilter( EApBearerTypeAll ),
iReqIpvType( aReqIpvType ),
iEventStore( KApUiEventNone )
    {
    }



// ---------------------------------------------------------
// CApSettingsHandler::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApSettingsHandler::ConstructL(   TInt aIspFilter,
                                                TInt aBearerFilter,
                                                TInt aSortType )
    {
    }



// ---------------------------------------------------------
// CApSettingsHandler::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApSettingsHandler::ConstructL(   CActiveApDb& aDb,
                                                TInt aIspFilter,
                                                TInt aBearerFilter,
                                                TInt aSortType )
    {
    }



// ---------------------------------------------------------
// CApSettingsHandler::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApSettingsHandler::ConstructL(   
                                             TInt aIspFilter,
                                             TInt aBearerFilter,
                                             TInt aSortType,
                                             TVpnFilterType aVpnFilterType
                                            )
    {
    }



// ---------------------------------------------------------
// CApSettingsHandler::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApSettingsHandler::ConstructL(
                                             CActiveApDb& aDb,
                                             TInt aIspFilter,
                                             TInt aBearerFilter,
                                             TInt aSortType,
                                             TVpnFilterType aVpnFilterType
                                             )
    {
    }




// ---------------------------------------------------------
// CApSettingsHandler::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApSettingsHandler::ConstructL(
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
// CApSettingsHandler::SetFilter
// ---------------------------------------------------------
//
EXPORT_C void CApSettingsHandler::SetFilterL(   TInt aIspFilter,
                                                TInt aBearerFilter,
                                                TInt aSortType )
    {
    }




// ---------------------------------------------------------
// CApSettingsHandler::SetTextOverrideL
// ---------------------------------------------------------
//
EXPORT_C void CApSettingsHandler::SetTextOverrideL( TTextID aText2Change,
                                                    const TDesC& aNewText )
    {
    }


// ---------------------------------------------------------
// CApSettingsHandler::TextOverrideL
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CApSettingsHandler::TextOverrideL( TTextID aTextId )
    {
    User::Leave( KErrNotSupported );
    }



// ---------------------------------------------------------
// CApSettingsHandler::RunSettingsL
// ---------------------------------------------------------
//
EXPORT_C TInt CApSettingsHandler::RunSettingsL( TUint32 aHighLight,
                                                TUint32& aSelected )
    {
    return 0;
    }



// ---------------------------------------------------------
// CApSettingsHandler::Cvt()
// ---------------------------------------------------------
//
EXPORT_C TInt CApSettingsHandler::Cvt()
    {
    return KErrNotSupported;
    }


// ---------------------------------------------------------
// CApSettingsHandler::DoRunSettingsL
// ---------------------------------------------------------
//
TInt CApSettingsHandler::DoRunSettingsL( TUint32 /*aHighLight*/,
                                         TUint32& /*aSelected*/ )
    {
    User::Leave(KErrNotSupported);
    return KErrNotSupported;
    }


// ---------------------------------------------------------
// CApSettingsHandler::RunViewerL
// ---------------------------------------------------------
//
EXPORT_C TInt CApSettingsHandler::RunViewerL( TUint32 aUid )
    {
    return 0;
    }



// ---------------------------------------------------------
// CApSettingsHandler::DoRunViewerL
// ---------------------------------------------------------
//
TInt CApSettingsHandler::DoRunViewerL( TUint32 /*aUid*/ )
    {
    User::Leave(KErrNotSupported);
    return KErrNotSupported;
    }



//----------------------------------------------------------
// CApSettingsHandler::HandleApDeleteCmdL
//----------------------------------------------------------
//
void CApSettingsHandler::HandleApDeleteCmdL( TUint32 /*aUid*/, 
                                             TBool /*aIsLast*/ )
    {
    User::Leave(KErrNotSupported);
    }


//----------------------------------------------------------
// CApSettingsHandler::HandleApBlankNewL
//----------------------------------------------------------
//
TUint32 CApSettingsHandler::HandleApBlankNewL( TInt /*aBearers*/, 
                                               TInt /*aIsps*/ )
    {
    User::Leave(KErrNotSupported);
    return KErrNotSupported;
    }



//----------------------------------------------------------
// CApSettingsHandler::HandleApUseOldNewL
//----------------------------------------------------------
//
TUint32 CApSettingsHandler::HandleApUseOldNewL()
    {
    User::Leave(KErrNotSupported);
    return KErrNone;
    }



//----------------------------------------------------------
// CApSettingsHandler::EditAfterCreateL
//----------------------------------------------------------
//
TUint32 CApSettingsHandler::EditAfterCreateL( TUint32 /*aUid*/,
                                              CApAccessPointItem* /*aItem*/ )
    {
    User::Leave(KErrNotSupported);
    return KErrNotSupported;  
    }



//----------------------------------------------------------
// CApSettingsHandler::DoConstructL
//----------------------------------------------------------
//
void CApSettingsHandler::DoConstructL( CActiveApDb* aDb,
                                       TInt aIspFilter,
                                       TInt aBearerFilter,
                                       TInt aSortType,
                                       TVpnFilterType aVpnFilterType,
                                       TBool aIncludeEasyWlan )
    {
    }




//----------------------------------------------------------
// CApSettingsHandler::DoRunSetPageSettingsL
//----------------------------------------------------------
//
void CApSettingsHandler::DoRunSetPageSettingsL( TUint32 /*aHighLight*/,
                                                TUint32& /*aSelected*/ )
    {
    User::Leave(KErrNotSupported);
    }



//----------------------------------------------------------
// CApSettingsHandler::DoRunListSettingsL
//----------------------------------------------------------
//
void CApSettingsHandler::DoRunListSettingsL( TUint32 /*aHighLight*/,
                                             TUint32& /*aSelected*/ )
    {
    User::Leave(KErrNotSupported);
    }



//----------------------------------------------------------
// CApSettingsHandler::DoHandleApUseOldNewL
//----------------------------------------------------------
//
TUint32 CApSettingsHandler::DoHandleApUseOldNewL( TUint32 /*aSelected*/ )
    {
    User::Leave(KErrNotSupported);
    return KErrNotSupported;
    }



//----------------------------------------------------------
// CApSettingsHandler::SelectApToCopyL
//----------------------------------------------------------
//
TUint32 CApSettingsHandler::SelectApToCopyL()
    {
    User::Leave(KErrNotSupported);
    return KErrNotSupported;
    }



//----------------------------------------------------------
// CApSettingsHandler::DoHandleApBlankNewL
//----------------------------------------------------------
//
TUint32 CApSettingsHandler::DoHandleApBlankNewL( TInt /*aBearers*/, TInt /*aIsps*/ )
    {
    User::Leave(KErrNotSupported);
    return KErrNotSupported;
    }




//----------------------------------------------------------
// CApSettingsHandler::HandleNetworkDeleteCmdL
//----------------------------------------------------------
//
void CApSettingsHandler::HandleNetworkDeleteCmdL( TUint32 /*aUid*/ )
    {
    User::Leave(KErrNotSupported);
    }



//----------------------------------------------------------
// CApSettingsHandler::GetVariantL
//----------------------------------------------------------
//
TInt CApSettingsHandler::GetVariantL()
    {
    User::Leave(KErrNotSupported);
    return KErrNotSupported;
    }


//----------------------------------------------------------
// CApSettingsHandler::ConfirmDeleteL
//----------------------------------------------------------
//
TInt CApSettingsHandler::ConfirmDeleteL( TUint32 /*aUid*/, TBool /*aIsLast*/ )
    {
    User::Leave(KErrNotSupported);
    return KErrNotSupported;
    }



//----------------------------------------------------------
// CApSettingsHandler::HandleRunSettingsL
//----------------------------------------------------------
//
void CApSettingsHandler::HandleRunSettingsL( TUint32 /*aHighLight*/, 
                                             TUint32& /*aSelected*/ )
    {
    User::Leave(KErrNotSupported);
    }



//----------------------------------------------------------
// CApSettingsHandler::ChooseBearerTypeL
//----------------------------------------------------------
//
void CApSettingsHandler::ChooseBearerTypeL( TInt /*aBearers*/, 
                                            CApAccessPointItem& /*aItem*/ )
    {
    User::Leave(KErrNotSupported);
    }




//----------------------------------------------------------
// CApSettingsHandler::HandleDuplicateL
//----------------------------------------------------------
//
TUint32 CApSettingsHandler::HandleDuplicateL( TUint32 /*aUid*/ )
    {
    User::Leave(KErrNotSupported);
    return KErrNotSupported;
    }


// End of File

