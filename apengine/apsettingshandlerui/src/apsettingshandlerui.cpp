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
    APSETUILOGGER_CREATE;
    APSETUILOGGER_ENTERFN( EAPI,"Handler::NewLC")
    
    CApSettingsHandler* db = 
            new( ELeave ) CApSettingsHandler( aStartWithSelection,
                                              aListType,
                                              aSelMenuType );
    CleanupStack::PushL( db );

    

    db->ConstructL( aIspFilter, aBearerFilter, aSortType );
    
    APSETUILOGGER_LEAVEFN( EAPI,"Handler::NewLC")
    
    return db;
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
    APSETUILOGGER_CREATE;
    APSETUILOGGER_ENTERFN( EAPI,"Handler::NewLC2")
    
    CApSettingsHandler* db = 
            new( ELeave ) CApSettingsHandler( aStartWithSelection,
                                              aListType,
                                              aSelMenuType );
    CleanupStack::PushL( db );

    db->ConstructL( aDb, aIspFilter, aBearerFilter, aSortType );
    
    APSETUILOGGER_LEAVEFN( EAPI,"Handler::NewLC2")
    
    return db;
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
    APSETUILOGGER_CREATE;
    APSETUILOGGER_ENTERFN( EAPI,"Handler::NewLC3")
    
    CApSettingsHandler* db = 
        new( ELeave ) CApSettingsHandler( aStartWithSelection,
                                          aListType,
                                          aSelMenuType,
                                          aReqIpvType
                                          );
    CleanupStack::PushL( db );

    db->ConstructL( aIspFilter, aBearerFilter, aSortType );
    
    APSETUILOGGER_LEAVEFN( EAPI,"Handler::NewLC3")
    
    return db;
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
    APSETUILOGGER_CREATE;
    APSETUILOGGER_ENTERFN( EAPI,"Handler::NewLC4")
    
    CApSettingsHandler* db = 
        new( ELeave ) CApSettingsHandler( aStartWithSelection,
                                          aListType,
                                          aSelMenuType,
                                          aReqIpvType
                                          );
    CleanupStack::PushL( db );

    db->ConstructL( aDb, aIspFilter, aBearerFilter, aSortType );
    
    APSETUILOGGER_LEAVEFN( EAPI,"Handler::NewLC4")
    
    return db;
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
    APSETUILOGGER_CREATE;
    APSETUILOGGER_ENTERFN( EAPI,"Handler::NewLC5")

    CApSettingsHandler* db = 
        new( ELeave ) CApSettingsHandler( aStartWithSelection,
                                          aListType,
                                          aSelMenuType,
                                          aReqIpvType
                                          );
    CleanupStack::PushL( db );

    db->ConstructL( aIspFilter, aBearerFilter, 
                    aSortType, aVpnFilterType );
    
    APSETUILOGGER_LEAVEFN( EAPI,"Handler::NewLC5")
    
    return db;
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
    APSETUILOGGER_CREATE;
    APSETUILOGGER_ENTERFN( EAPI,"Handler::NewLC6")
    
    CApSettingsHandler* db = 
        new( ELeave ) CApSettingsHandler( aStartWithSelection,
                                          aListType,
                                          aSelMenuType,
                                          aReqIpvType
                                          );
    CleanupStack::PushL( db );
    
    db->ConstructL( aDb, aIspFilter, aBearerFilter, 
                    aSortType, aVpnFilterType );
    
    APSETUILOGGER_LEAVEFN( EAPI,"Handler::NewLC6")
    
    return db;
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
    APSETUILOGGER_CREATE;
    APSETUILOGGER_ENTERFN( EAPI,"Handler::NewLC7")
    
    CApSettingsHandler* db = 
        new( ELeave ) CApSettingsHandler( aStartWithSelection,
                                          aListType,
                                          aSelMenuType,
                                          aReqIpvType
                                          );
    CleanupStack::PushL( db );
    
    db->ConstructL( aDb, aIspFilter, aBearerFilter, 
                    aSortType, aVpnFilterType, aIncludeEasyWlan );
    
    APSETUILOGGER_LEAVEFN( EAPI,"Handler::NewLC7")
    return db;    
    }


// Destructor
EXPORT_C CApSettingsHandler::~CApSettingsHandler()
    {
    APSETUILOGGER_DELETE;

    APSETUILOGGER_WRITE( EAPI,"Handler:: deleting implementation");
    
    delete iImpl;
    
    APSETUILOGGER_WRITE( EAPI,"Handler::~finished");
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
    iHelpMajor = KHelpUidApSettingsHandler;
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
    iHelpMajor = KHelpUidApSettingsHandler;
    }



// ---------------------------------------------------------
// CApSettingsHandler::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApSettingsHandler::ConstructL(   TInt aIspFilter,
                                                TInt aBearerFilter,
                                                TInt aSortType )
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::ConstructL")
    
    DoConstructL( NULL, aIspFilter, aBearerFilter, 
                  aSortType, EVpnFilterBoth, EFalse );
    
    APSETUILOGGER_LEAVEFN( EAPI,"Handler::ConstructL")
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
    APSETUILOGGER_ENTERFN( EAPI,"Handler::ConstructL2")
    
    DoConstructL( &aDb, aIspFilter, aBearerFilter, 
                  aSortType, EVpnFilterBoth, EFalse );
    
    APSETUILOGGER_LEAVEFN( EAPI,"Handler::ConstructL2")
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
    APSETUILOGGER_ENTERFN( EAPI,"Handler::ConstructL3")
    
    DoConstructL( NULL, aIspFilter, aBearerFilter, 
                  aSortType, aVpnFilterType, EFalse );
    
    APSETUILOGGER_LEAVEFN( EAPI,"Handler::ConstructL3")
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
    APSETUILOGGER_ENTERFN( EAPI,"Handler::ConstructL4")
    
    DoConstructL( &aDb, aIspFilter, aBearerFilter, 
                  aSortType, aVpnFilterType, EFalse );
    
    APSETUILOGGER_LEAVEFN( EAPI,"Handler::ConstructL4")
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
    APSETUILOGGER_ENTERFN( EAPI,"Handler::ConstructL5")
    
    DoConstructL( &aDb, aIspFilter, aBearerFilter, 
                  aSortType, aVpnFilterType, aIncludeEasyWlan );
    
    APSETUILOGGER_LEAVEFN( EAPI,"Handler::ConstructL5")
    }


// ---------------------------------------------------------
// CApSettingsHandler::SetFilter
// ---------------------------------------------------------
//
EXPORT_C void CApSettingsHandler::SetFilterL(   TInt aIspFilter,
                                                TInt aBearerFilter,
                                                TInt aSortType )
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::SetFilterL")
    
    iImpl->SetFilterL( aIspFilter, aBearerFilter, aSortType ); 
    
    APSETUILOGGER_LEAVEFN( EAPI,"Handler::SetFilterL")   
    }




// ---------------------------------------------------------
// CApSettingsHandler::SetTextOverrideL
// ---------------------------------------------------------
//
EXPORT_C void CApSettingsHandler::SetTextOverrideL( TTextID aText2Change,
                                                    const TDesC& aNewText )
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::SetTextOverrideL")
    
    __ASSERT_DEBUG( iImpl, Panic( ENullPointer ) );
    iImpl->SetTextOverrideL( aText2Change, aNewText );
    
    APSETUILOGGER_LEAVEFN( EAPI,"Handler::SetTextOverrideL")
    }


// ---------------------------------------------------------
// CApSettingsHandler::TextOverrideL
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CApSettingsHandler::TextOverrideL( TTextID aTextId )
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::TextOverrideL<->")
    
    __ASSERT_DEBUG( iImpl, Panic( ENullPointer ) );
    return iImpl->TextOverrideL( aTextId );
    }



// ---------------------------------------------------------
// CApSettingsHandler::RunSettingsL
// ---------------------------------------------------------
//
EXPORT_C TInt CApSettingsHandler::RunSettingsL( TUint32 aHighLight,
                                                TUint32& aSelected )
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::RunSettingsL")
    
    iImpl->RunSettingsL( aHighLight, aSelected );
    
    APSETUILOGGER_LEAVEFN( EAPI,"Handler::RunSettingsL")
    
    return iImpl->EventStore();
    }



// ---------------------------------------------------------
// CApSettingsHandler::Cvt()
// ---------------------------------------------------------
//
EXPORT_C TInt CApSettingsHandler::Cvt()
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::Cvt<->")
    return KErrNone;
    }


// ---------------------------------------------------------
// CApSettingsHandler::DoRunSettingsL
// ---------------------------------------------------------
//
TInt CApSettingsHandler::DoRunSettingsL( TUint32 /*aHighLight*/,
                                         TUint32& /*aSelected*/ )
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::DoRunSettingsL@Deprecated")
    
    //  Deprecated as moved to iImpl
    __ASSERT_DEBUG( EFalse, Panic( ENullPointer ) );
    User::Leave(KErrNotSupported);
    return KErrNone;
    }


// ---------------------------------------------------------
// CApSettingsHandler::RunViewerL
// ---------------------------------------------------------
//
EXPORT_C TInt CApSettingsHandler::RunViewerL( TUint32 aUid )
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::RunViewerL")
    
    iImpl->RunViewerL( aUid );    
    
    APSETUILOGGER_LEAVEFN( EAPI,"Handler::RunViewerL")
    
    return iImpl->EventStore();
    }



// ---------------------------------------------------------
// CApSettingsHandler::DoRunViewerL
// ---------------------------------------------------------
//
TInt CApSettingsHandler::DoRunViewerL( TUint32 /*aUid*/ )
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::DoRunViewerL@Deprecated")
    
    //  Deprecated as moved to iImpl
    __ASSERT_DEBUG( EFalse, Panic( ENullPointer ) );
    User::Leave(KErrNotSupported);
    return KErrNone;
    }



//----------------------------------------------------------
// CApSettingsHandler::HandleApDeleteCmdL
//----------------------------------------------------------
//
void CApSettingsHandler::HandleApDeleteCmdL( TUint32 /*aUid*/, 
                                             TBool /*aIsLast*/ )
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::HandleApDeleteCmdL@Deprecated")
    
    //  Deprecated as moved to iImpl
    __ASSERT_DEBUG( EFalse, Panic( ENullPointer ) );
    User::Leave(KErrNotSupported);
    }


//----------------------------------------------------------
// CApSettingsHandler::HandleApBlankNewL
//----------------------------------------------------------
//
TUint32 CApSettingsHandler::HandleApBlankNewL( TInt /*aBearers*/, 
                                               TInt /*aIsps*/ )
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::HandleApBlankNewL@Deprecated")
    
    //  Deprecated as moved to iImpl
    __ASSERT_DEBUG( EFalse, Panic( ENullPointer ) );
    User::Leave(KErrNotSupported);
    return KErrNone;
    }



//----------------------------------------------------------
// CApSettingsHandler::HandleApUseOldNewL
//----------------------------------------------------------
//
TUint32 CApSettingsHandler::HandleApUseOldNewL()
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::HandleApUseOldNewL@Deprecated")
    
    //  Deprecated as moved to iImpl
    __ASSERT_DEBUG( EFalse, Panic( ENullPointer ) );
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
    APSETUILOGGER_ENTERFN( EAPI,"Handler::EditAfterCreateL@Deprecated")
    
    //  Deprecated as moved to iImpl
    __ASSERT_DEBUG( EFalse, Panic( ENullPointer ) );
    User::Leave(KErrNotSupported);
    return KErrNone;  
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
    APSETUILOGGER_ENTERFN( EAPI,"Handler::DoConstructL")
    
    // must place it to a temp variable, otherwise CodeScanner reports 
    // it as a bad practice putting up member to Cleanupstack, but 
    // we have NO NewL so we must use this.
    CApSettingsHandlerImpl* tmp = CApSettingsHandlerImpl::NewLC( 
                                           *aDb, iStartWithSelection,
                                           iListType, iSelMenuType,
                                           aIspFilter, aBearerFilter, 
                                           aSortType, iReqIpvType,
                                           aVpnFilterType, aIncludeEasyWlan,
                                           ETrue );
    CleanupStack::Pop(tmp);   // will soon be a member, need to pop
    iImpl = tmp;                                           
    
    APSETUILOGGER_LEAVEFN( EAPI,"Handler::DoConstructL")
    }




//----------------------------------------------------------
// CApSettingsHandler::DoRunSetPageSettingsL
//----------------------------------------------------------
//
void CApSettingsHandler::DoRunSetPageSettingsL( TUint32 /*aHighLight*/,
                                                TUint32& /*aSelected*/ )
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::DoRunSetPageSettingsL@Deprecated")
    
    //  Deprecated as moved to iImpl
    __ASSERT_DEBUG( EFalse, Panic( ENullPointer ) );
    User::Leave(KErrNotSupported);
    }



//----------------------------------------------------------
// CApSettingsHandler::DoRunListSettingsL
//----------------------------------------------------------
//
void CApSettingsHandler::DoRunListSettingsL( TUint32 /*aHighLight*/,
                                             TUint32& /*aSelected*/ )
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::DoRunListSettingsL@Deprecated")
    
    //  Deprecated as moved to iImpl
    __ASSERT_DEBUG( EFalse, Panic( ENullPointer ) );
    User::Leave(KErrNotSupported);
    }



//----------------------------------------------------------
// CApSettingsHandler::DoHandleApUseOldNewL
//----------------------------------------------------------
//
TUint32 CApSettingsHandler::DoHandleApUseOldNewL( TUint32 /*aSelected*/ )
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::DoHandleApUseOldNewL@Deprecated")
    
    //  Deprecated as moved to iImpl
    __ASSERT_DEBUG( EFalse, Panic( ENullPointer ) );
    User::Leave(KErrNotSupported);
    return KErrNone;
    }



//----------------------------------------------------------
// CApSettingsHandler::SelectApToCopyL
//----------------------------------------------------------
//
TUint32 CApSettingsHandler::SelectApToCopyL()
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::SelectApToCopyL@Deprecated")
    
    //  Deprecated as moved to iImpl
    __ASSERT_DEBUG( EFalse, Panic( ENullPointer ) );
    User::Leave(KErrNotSupported);
    return KErrNone;
    }



//----------------------------------------------------------
// CApSettingsHandler::DoHandleApBlankNewL
//----------------------------------------------------------
//
TUint32 CApSettingsHandler::DoHandleApBlankNewL( TInt /*aBearers*/, TInt /*aIsps*/ )
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::DoHandleApBlankNewL@Deprecated")
    
    //  Deprecated as moved to iImpl
    __ASSERT_DEBUG( EFalse, Panic( ENullPointer ) );
    User::Leave(KErrNotSupported);
    return KErrNone;
    }




//----------------------------------------------------------
// CApSettingsHandler::HandleNetworkDeleteCmdL
//----------------------------------------------------------
//
void CApSettingsHandler::HandleNetworkDeleteCmdL( TUint32 /*aUid*/ )
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::HandleNetworkDeleteCmdL@Deprecated")
    
    //  Deprecated as moved to iImpl
    __ASSERT_DEBUG( EFalse, Panic( ENullPointer ) );
    User::Leave(KErrNotSupported);
    }



//----------------------------------------------------------
// CApSettingsHandler::GetVariantL
//----------------------------------------------------------
//
TInt CApSettingsHandler::GetVariantL()
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::GetVariantL@Deprecated")
    
    //  Deprecated as moved to iImpl
    __ASSERT_DEBUG( EFalse, Panic( ENullPointer ) );
    User::Leave(KErrNotSupported);
    return KErrNone;
    }


//----------------------------------------------------------
// CApSettingsHandler::ConfirmDeleteL
//----------------------------------------------------------
//
TInt CApSettingsHandler::ConfirmDeleteL( TUint32 /*aUid*/, TBool /*aIsLast*/ )
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::ConfirmDeleteL@Deprecated")
    
    //  Deprecated as moved to iImpl
    __ASSERT_DEBUG( EFalse, Panic( ENullPointer ) );
    User::Leave(KErrNotSupported);
    return KErrNone;
    }



//----------------------------------------------------------
// CApSettingsHandler::HandleRunSettingsL
//----------------------------------------------------------
//
void CApSettingsHandler::HandleRunSettingsL( TUint32 /*aHighLight*/, 
                                             TUint32& /*aSelected*/ )
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::HandleRunSettingsL@Deprecated")
    
    //  Deprecated as moved to iImpl
    __ASSERT_DEBUG( EFalse, Panic( ENullPointer ) );
    User::Leave(KErrNotSupported);
    }



//----------------------------------------------------------
// CApSettingsHandler::ChooseBearerTypeL
//----------------------------------------------------------
//
void CApSettingsHandler::ChooseBearerTypeL( TInt /*aBearers*/, 
                                            CApAccessPointItem& /*aItem*/ )
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::ChooseBearerTypeL@Deprecated")
    
    //  Deprecated as moved to iImpl
    __ASSERT_DEBUG( EFalse, Panic( ENullPointer ) );
    User::Leave(KErrNotSupported);
    }




//----------------------------------------------------------
// CApSettingsHandler::HandleDuplicateL
//----------------------------------------------------------
//
TUint32 CApSettingsHandler::HandleDuplicateL( TUint32 /*aUid*/ )
    {
    APSETUILOGGER_ENTERFN( EAPI,"Handler::HandleDuplicateL@Deprecated")
    
    //  Deprecated as moved to iImpl
    __ASSERT_DEBUG( EFalse, Panic( ENullPointer ) );    
    User::Leave(KErrNotSupported);
    return KErrNone;
    }


// End of File

