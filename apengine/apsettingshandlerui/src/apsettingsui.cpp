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
    APSETUILOGGER_CREATE;
    APSETUILOGGER_ENTERFN( EAPI,"SettingsUi::NewLC")

    CApSettingsUi* db = 
        new( ELeave ) CApSettingsUi( aStartWithSelection,
                                          aListType,
                                          aSelMenuType,
                                          aReqIpvType
                                          );
    CleanupStack::PushL( db );
    

    db->ConstructL( aIspFilter, aBearerFilter, 
                    aSortType, aVpnFilterType );
    APSETUILOGGER_LEAVEFN( EAPI,"SettingsUi::NewLC")    
    return db;
    }



// Destructor
EXPORT_C CApSettingsUi::~CApSettingsUi()
    {
    APSETUILOGGER_DELETE;

    delete iImpl;    
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
    APSETUILOGGER_ENTERFN( EAPI,"SettingsUi::ConstructL");

    // must place it to a temp variable, otherwise CodeScanner reports 
    // it as a bad practice putting up member to Cleanupstack, but 
    // we have NO NewL so we must use this.
    CApSettingsHandlerImpl* tmp = CApSettingsHandlerImpl::NewLC( 
                                     iStartWithSelection,
                                     iListType, iSelMenuType,
                                     aIspFilter, aBearerFilter, 
                                     aSortType, iReqIpvType,
                                     aVpnFilterType,
                                     EFalse );
                                     
    CleanupStack::Pop( tmp );   // soon will be a member, need to pop
    iImpl = tmp;
    
    APSETUILOGGER_LEAVEFN( EAPI,"SettingsUi::ConstructL")    
    }



// ---------------------------------------------------------
// CApSettingsUi::RunSettingsL
// ---------------------------------------------------------
//
EXPORT_C TInt CApSettingsUi::RunSettingsL( TUint32 aHighLight,
                                                TUint32& aSelected )
    {
    APSETUILOGGER_ENTERFN( EAPI,"SettingsUi::RunSettingsL")
    __ASSERT_DEBUG( iImpl, Panic( ENullPointer ) );

    iImpl->RunSettingsL( aHighLight, aSelected );
    
    APSETUILOGGER_LEAVEFN( EAPI,"SettingsUi::RunSettingsL")
    
    return iImpl->EventStore();
    }



// End of File

