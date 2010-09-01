/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CCmListboxModel
*
*/

#include <AknIconArray.h>
#include <aknlists.h>
#include <StringLoader.h>
#include <cmmanager.rsg>

#include "cmlistboxmodel.h"
#include "cmlistitem.h"
#include <cmcommonconstants.h>
#include "cmlistitemlist.h"

// ---------------------------------------------------------------------------
// CCmListboxModel::CCmListboxModel
// ---------------------------------------------------------------------------
//
CCmListboxModel::CCmListboxModel()
    : iOffset( 0 )
    {
    }

// ---------------------------------------------------------------------------
// CCmListboxModel::~CCmListboxModel
// ---------------------------------------------------------------------------
//
CCmListboxModel::~CCmListboxModel()
    {
    //ResetAndCmroy(); // deletes all elements...
    }

// ---------------------------------------------------------------------------
// CCmListboxModel::MdcaCount
// ---------------------------------------------------------------------------
//
TInt CCmListboxModel::MdcaCount() const
    {
    return Count();
    }

// ---------------------------------------------------------------------------
// CCmListboxModel::SetOwnIconOffset
// ---------------------------------------------------------------------------
//
void CCmListboxModel::SetOwnIconOffset( TInt aOffset )
    {
    iOffset = aOffset;
    }

// ---------------------------------------------------------------------------
// CCmListboxModel::MdcaPoint
// ---------------------------------------------------------------------------
//
TPtrC CCmListboxModel::MdcaPoint( TInt aIndex ) const
    {
    // We need to cast away the const-ness from the buffer.
    // It was definitely made for formatting!
    FormatListboxText( aIndex,
                       MUTABLE_CAST( TBuf<KMaxCmListboxName>&, iBuf ) );

    return iBuf;
    }

// ---------------------------------------------------------------------------
// CCmListboxModel::FormatListboxText
// ---------------------------------------------------------------------------
//
void CCmListboxModel::FormatListboxText( TInt aIndex,
                                         TDes& aBuf ) const
    {
    TRAP_IGNORE( FormatListboxTextL( aIndex, aBuf ) );
    }
    
// ---------------------------------------------------------------------------
// CCmListboxModel::FormatListboxTextL
// ---------------------------------------------------------------------------
//
void CCmListboxModel::FormatListboxTextL( TInt aIndex,
                                          TDes& aBuf ) const
    {
    aBuf.Zero();
    TUint icon = At( aIndex )->IconIndex();
    if ( At(aIndex)->IsSingleLine() )
        {
        if ( At(aIndex)->IsProtected() )
            {
            aBuf.Format( KListItemFormatEmbDestProt, 
                         icon, 
                         &At( aIndex )->Name(), 
                         KCmReservedIconIndexProtected );
            }
        else            
            {
            aBuf.Format( KListItemFormatEmbDest, 
                         icon,
                         &At( aIndex )->Name() );            
            }
        }
     else
        {
        HBufC* secondLine;
        secondLine = StringLoader::LoadLC( R_CMMANAGERUI_METHOD_PRIORITY_OTHER,
                                           At( aIndex )->Priority() + 1 );
    if ( At( aIndex )->IsDefault() )
        {
            aBuf.Format( KListItemFormatCmProt,
                         icon, 
                         &At( aIndex )->Name(), 
                         secondLine, 
                         KCmReservedIconIndexDefaultConn );
        }
    else
        {
        if ( At(aIndex)->IsProtected() )
            {
            aBuf.Format( KListItemFormatCmProt, 
                         icon, 
                         &At( aIndex )->Name(), 
                         secondLine, 
                         KCmReservedIconIndexProtected );    
            }
        else if ( At(aIndex)->IsVpnOverDestination() )
            {
            CleanupStack::PopAndDestroy( secondLine );
            
            secondLine = StringLoader::LoadLC( R_QTN_NETW_CONSET_METHOD_PRIORITY_AUTO);

            aBuf.Format( KListItemFormatCm, 
                         icon,
                         &At( aIndex )->Name(),
                         secondLine );
            }
        else
            {
            aBuf.Format( KListItemFormatCm, 
                         icon, 
                         &At( aIndex )->Name(), 
                         secondLine );
            }
         }
            
        CleanupStack::PopAndDestroy( secondLine );
        }
    }

