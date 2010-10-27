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
* Description:  Implementation of CActiveApDb.
*
*/


// INCLUDE FILES

#include <ActiveApDb.h>
#include "ActiveApDbNotifier.h"
#include <ApEngineConsts.h>
#include "ApEngineCommons.h"
#include "ApEngineLogger.h"

const TInt KObserverArrayGranularity = 1;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CActiveApDb::NewL
// ---------------------------------------------------------
//
EXPORT_C CActiveApDb* CActiveApDb::NewL( TCommDbDatabaseType /*aType*/ )
    {
    return NULL;
    }

// ---------------------------------------------------------
// CActiveApDb::~CActiveApDb
// ---------------------------------------------------------
//
EXPORT_C CActiveApDb::~CActiveApDb()
    {
    }


// ---------------------------------------------------------
// CActiveApDb::CActiveApDb
// ---------------------------------------------------------
//
CActiveApDb::CActiveApDb()
    {
    }


// ---------------------------------------------------------
// CActiveApDb::ConstructL
// ---------------------------------------------------------
//
void CActiveApDb::ConstructL()
    {
    }


// ---------------------------------------------------------
// CActiveApDb::AddObserverL
// ---------------------------------------------------------
//
EXPORT_C void CActiveApDb::AddObserverL( MActiveApDbObserver* anObserver )
    {
    }


// ---------------------------------------------------------
// CActiveApDb::RemoveObserver
// ---------------------------------------------------------
//
EXPORT_C void CActiveApDb::RemoveObserver( MActiveApDbObserver* anObserver )
    {
    }


// ---------------------------------------------------------
// CActiveApDb::Database
// ---------------------------------------------------------
//
EXPORT_C CCommsDatabase*  CActiveApDb::Database()
    {
    return NULL;
    }

// ---------------------------------------------------------
// CActiveApDb::HandleDbEventL
// ---------------------------------------------------------
//
void CActiveApDb::HandleDbEventL( TInt anEvent )
    {

    }




// ---------------------------------------------------------
// CActiveApDb::NotifyObserversL
// ---------------------------------------------------------
//
void CActiveApDb::NotifyObserversL( MActiveApDbObserver::TEvent anEvent )
    {

    }

// End of File
