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
* Description:  Implementation of ApControlListboxModel.
*
*/


// INCLUDE FILES
#include <aknlists.h>
#include <apcontrollistpluginrsc.rsg>

#include "apcontrollistboxmodel.h"
#include "apcontrollistpluginlogger.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CApControlListboxModel::CApControlListboxModel
// ---------------------------------------------------------
//
CApControlListboxModel::CApControlListboxModel()
:CTextListBoxModel()
    {
    CLOG( ( ESelector, 0, _L( 
    	"-> CApControlListboxModel::CApControlListboxModel" ) ) );
    CLOG( ( ESelector, 0, _L( 
    	"<- CApControlListboxModel::CApControlListboxModel" ) ) );
    }


// ---------------------------------------------------------
// CApControlListboxModel::~CApControlListboxModel
// ---------------------------------------------------------
//
CApControlListboxModel::~CApControlListboxModel()
    {
    CLOG( ( ESelector, 0, _L( 
    	"-> CApControlListboxModel::~CApControlListboxModel" ) ) );
    CLOG( ( ESelector, 0, _L( 
    	"<- CApControlListboxModel::~CApControlListboxModel" ) ) );
    }



// ---------------------------------------------------------
// CApControlListboxModel::MdcaCount
// ---------------------------------------------------------
//
TInt CApControlListboxModel::MdcaCount() const
    {
    CLOG( ( ESelector, 0, _L( "-> CApControlListboxModel::MdcaCount" ) ) );
    CLOG( ( ESelector, 0, _L( "<- CApControlListboxModel::MdcaCount" ) ) );
    return NumberOfItems();
    }


// ---------------------------------------------------------
// CApControlListboxModel::MdcaPoint
// ---------------------------------------------------------
//

TPtrC CApControlListboxModel::MdcaPoint( TInt aIndex ) const
    {
    CLOG( ( ESelector, 0, _L( "-> CApControlListboxModel::MdcaPoint" ) ) );
    CLOG( ( ESelector, 0, _L( "<- CApControlListboxModel::MdcaPoint" ) ) );
    return ItemText(aIndex);
    }




// End of File
