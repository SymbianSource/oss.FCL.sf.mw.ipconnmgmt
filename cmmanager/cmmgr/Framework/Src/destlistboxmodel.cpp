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
* Description:  Implementation of CDestListboxModel
*
*/

#include <AknIconArray.h>
#include <aknlists.h>
#include <StringLoader.h>
#include <cmmanager.rsg>

#include "destlistboxmodel.h"
#include "destlistitem.h"
#include "destlistitemlist.h"
#include <cmcommonconstants.h>

using namespace CMManager;

// ---------------------------------------------------------------------------
// CDestListboxModel::CDestListboxModel
// ---------------------------------------------------------------------------
//
CDestListboxModel::CDestListboxModel()
    : iOffset( 0 )
    {
    }

// ---------------------------------------------------------------------------
// CDestListboxModel::~CDestListboxModel
// ---------------------------------------------------------------------------
//
CDestListboxModel::~CDestListboxModel()
    {
    ResetAndDestroy(); // deletes all elements...
    }

// ---------------------------------------------------------------------------
// CDestListboxModel::MdcaCount
// ---------------------------------------------------------------------------
//
TInt CDestListboxModel::MdcaCount() const
    {
    return Count();
    }

// ---------------------------------------------------------------------------
// CDestListboxModel::SetOwnIconOffset
// ---------------------------------------------------------------------------
//
void CDestListboxModel::SetOwnIconOffset( TInt aOffset )
    {
    iOffset = aOffset;
    }

// ---------------------------------------------------------------------------
// CDestListboxModel::MdcaPoint
// ---------------------------------------------------------------------------
//
TPtrC CDestListboxModel::MdcaPoint( TInt aIndex ) const
    {
    // We need to cast away the const-ness from the buffer.
    // It was definitely made for formatting!
    FormatListboxText( aIndex,
        ( MUTABLE_CAST(  TBuf<KMaxDestListboxName>&, iBuf ) ) );

    return iBuf;
    }

// ---------------------------------------------------------------------------
// CDestListboxModel::FormatListboxTextL
// ---------------------------------------------------------------------------
//
void CDestListboxModel::FormatListboxTextL( TInt aIndex,
                                                TDes& aBuf ) const
    {
    aBuf.Zero();
    HBufC* secondLine;
    if ( At(aIndex)->NumOfCMs()  )
        {
        if ( At(aIndex)->NumOfCMs()  == 1 )
            {
            secondLine = 
                  StringLoader::LoadLC( R_CMMANAGERUI_CONN_METHODS_AMOUNT_ONE );
            }
         else
            {
            secondLine = 
                   StringLoader::LoadLC( R_CMMANAGERUI_CONN_METHODS_AMOUNT_MANY, 
                                         At( aIndex )->NumOfCMs() );
            }
        }
     else
        {
        if ( At( aIndex )->Uid() == KDestItemNewConnection )
            {
            //add cm if list is empty
            secondLine = 
                    StringLoader::LoadLC( R_CMMANAGERUI_ADD_CONNECTION_METHOD );
            }
         else
            {
            secondLine = 
                 StringLoader::LoadLC( R_CMMANAGERUI_CONN_METHODS_AMOUNT_NONE );
            }
        }
        
    if ( At( aIndex )->IsDefault() )
        {
            aBuf.Format( KListItemFormatDestProt,
                         At(aIndex)->IconIndex(), 
                         &At(aIndex)->Name(), 
                         secondLine, 
                         KDestReservedIconIndexDefaultConn );
        }
    else
        {
        // Protected icon must be put only if the protection level is 1
        if ( At( aIndex )->GetProtectionLevel() == EProtLevel1 )
            {
            aBuf.Format( KListItemFormatDestProt,
                         At(aIndex)->IconIndex(), 
                         &At(aIndex)->Name(), 
                         secondLine, 
                         KDestReservedIconIndexProtected );
            }
         else
            {
            aBuf.Format( KListItemFormatDest,
                         At(aIndex)->IconIndex(), 
                         &At(aIndex)->Name(), 
                         secondLine );
            }
        }
    CleanupStack::PopAndDestroy( secondLine );
    }
    
// ---------------------------------------------------------------------------
// CDestListboxModel::FormatListboxText
// ---------------------------------------------------------------------------
//

void CDestListboxModel::FormatListboxText( TInt aIndex,
                                           TDes& aBuf ) const
    {
    TRAP_IGNORE( FormatListboxTextL( aIndex, aBuf ) );
    }
