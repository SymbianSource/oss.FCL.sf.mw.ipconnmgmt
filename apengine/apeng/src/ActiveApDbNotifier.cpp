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
* Description:  Implementation of CActiveApDbNotifier.
*
*/


// INCLUDE FILES

#include "ActiveApDb.h"
#include "ActiveApDbNotifier.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CActiveApDbNotifier::CActiveApDbNotifier
// ---------------------------------------------------------
//
CActiveApDbNotifier::CActiveApDbNotifier( CActiveApDb& aDb )
: CActive( EPriorityStandard )
    {
    }

// ---------------------------------------------------------
// CActiveApDbNotifier::~CActiveApDbNotifier
// ---------------------------------------------------------
//
CActiveApDbNotifier::~CActiveApDbNotifier()
    {
    }

// ---------------------------------------------------------
// CActiveApDbNotifier::RunL
// ---------------------------------------------------------
//
void CActiveApDbNotifier::RunL()
    {
    }


// ---------------------------------------------------------
// CActiveApDbNotifier::DoCancel
// ---------------------------------------------------------
//
void CActiveApDbNotifier::DoCancel()
    {
    }


// ---------------------------------------------------------
// CActiveApDbNotifier::Stop
// ---------------------------------------------------------
//
void CActiveApDbNotifier::Stop()
    {
    }

// ---------------------------------------------------------
// CActiveApDbNotifier::Start
// ---------------------------------------------------------
//
void CActiveApDbNotifier::Start()
    {
    }

// ---------------------------------------------------------
// CActiveApDbNotifier::NotifyChange
// ---------------------------------------------------------
//
void CActiveApDbNotifier::NotifyChange()
    {
    }

// End of File
