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
* Description:  Implementation of CApNetSelectorListboxModel.
*
*/


// INCLUDE FILES
#include "ApNetSelectorListBoxModel.h"
#include "ApNetworkItem.h"
#include <ApSettingsHandlerCommons.h>

#include "ApSettingsHandlerLogger.h"

// CONSTANTS


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CApNetSelectorListboxModel::CApNetSelectorListboxModel
// ---------------------------------------------------------
//
CApNetSelectorListboxModel::CApNetSelectorListboxModel( )
:CApNetworkItemList()
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelListModel::CApNetSelectorListboxModel<->")
    }


// ---------------------------------------------------------
// CApNetSelectorListboxModel::~CApNetSelectorListboxModel
// ---------------------------------------------------------
//
CApNetSelectorListboxModel::~CApNetSelectorListboxModel()
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelListModel::~CApNetSelectorListboxModel")
    
    ResetAndDestroy(); // deletes all elements...
    
    APSETUILOGGER_LEAVEFN( EListbox,"NetSelListModel::~CApNetSelectorListboxModel")
    }


// ---------------------------------------------------------
// CApNetSelectorListboxModel::MdcaCount
// ---------------------------------------------------------
//
TInt CApNetSelectorListboxModel::MdcaCount() const
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelListModel::MdcaCount<->")
    return Count();
    }


// ---------------------------------------------------------
// CApNetSelectorListboxModel::MdcaPoint
// ---------------------------------------------------------
//
TPtrC CApNetSelectorListboxModel::MdcaPoint( TInt aIndex ) const
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelListModel::MdcaPoint")
    
    // We need to cast away the const-ness from the buffer.
    // It was definitely made for formatting!
    FormatListboxText( aIndex,
        ( MUTABLE_CAST(  TBuf<KMaxApSelectorListboxName>&, iBuf ) ) );

    
    APSETUILOGGER_LEAVEFN( EListbox,"NetSelListModel::MdcaPoint")
    return iBuf;
    }


// ---------------------------------------------------------
// CApNetSelectorListboxModel::Item4Uid
// ---------------------------------------------------------
//
TInt CApNetSelectorListboxModel::Item4Uid( TUint32 aUid, TInt& aErr ) const
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelListModel::Item4Uid")
    
    aErr = KErrNone;
    TInt retval( KErrNotFound );
    for ( TInt i=0; i<Count(); i++ )
        {
        if ( At( i )->Uid() == aUid )
            {
            retval = i;
            break;
            }
        }
    if ( retval == KErrNotFound )
        {
        aErr = KErrNotFound;
        }
    
    APSETUILOGGER_LEAVEFN( EListbox,"NetSelListModel::Item4Uid")
    return retval;
    }



// ---------------------------------------------------------
// CApNetSelectorListboxModel::FormatListboxText
// ---------------------------------------------------------
//
void CApNetSelectorListboxModel::FormatListboxText( TInt aIndex,
                                                TDes& aBuf ) const
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelListModel::FormatListboxText")
    
    aBuf.Format(
                KListItemFormatGraphicOnOff,
                1,
                Min( At( aIndex )->Name().Length(),
                KMaxListItemNameLength ),
                &At( aIndex )->Name()
                );
    
    APSETUILOGGER_LEAVEFN( EListbox,"NetSelListModel::FormatListboxText")
    }

// End of File
