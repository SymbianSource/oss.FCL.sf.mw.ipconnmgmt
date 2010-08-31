/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Implementation of CFlexPeriodic class
 *
 */
/*
 * %version: 1 %
 */
// System include files
// User include files go here:
#include "inheritedcflextimer2.h"
//For exteded test result
#include "testcflextimer.h"
#include <e32debug.h>
// External function prototypes
// Local constants go here
// Panic category

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// Constructs the object.
// ---------------------------------------------------------------------------
//
CInheritedCFlexTimer2* CInheritedCFlexTimer2::NewL( TInt aPriority,
        TCallBack aCallBack, TInt aId )
    {
    RDebug::Printf("CInheritedCFlexTimer2::NewL");
    CInheritedCFlexTimer2* self = new (ELeave) CInheritedCFlexTimer2(
            aPriority, aCallBack, aId);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructs the object.
// ---------------------------------------------------------------------------
//
CInheritedCFlexTimer2::~CInheritedCFlexTimer2()
    {

    }

// ---------------------------------------------------------------------------
// Desc
// ---------------------------------------------------------------------------
//
void CInheritedCFlexTimer2::SetCB( TCallBack aCallBack )
    {
    iCallBack = aCallBack;
    }

// ---------------------------------------------------------------------------
// Desc
// ---------------------------------------------------------------------------
//
TInt CInheritedCFlexTimer2::MyOwnConfigure(
        TTimeIntervalMicroSeconds32 aDelayWindow )
    {
    TInt ret = CFlexTimer::Configure(aDelayWindow);
    if (ret == KErrNone)
        {
        //
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Handles the active objects request completion event.
// ---------------------------------------------------------------------------
//
void CInheritedCFlexTimer2::RunL()
    {
    RDebug::Printf("CInheritedCFlexTimer2::RunL");
    //Give code to test case
    RArray<TTimerStruct> *list =
            reinterpret_cast<RArray<TTimerStruct>*> (iCallBack.iPtr);
    (*list)[0].iCurrentlyActive = iId;
    (*list)[iId].iEResult->iStatusCode = iStatus.Int();
    iCallBack.CallBack();
    }

// ---------------------------------------------------------------------------
// Second part of the two-phase construction.
// ---------------------------------------------------------------------------
//
void CInheritedCFlexTimer2::ConstructL()
    {
    CActiveScheduler::Add(this);
    CFlexTimer::ConstructL();
    }

// ---------------------------------------------------------------------------
// Private constructor with priority.
// ---------------------------------------------------------------------------
//
CInheritedCFlexTimer2::CInheritedCFlexTimer2( TInt aPriority,
        TCallBack aCallBack, TInt aId ) :
    CFlexTimer(aPriority), iCallBack(aCallBack), iId(aId)
    {
    RDebug::Printf("CInheritedCFlexTimer2::CInheritedCFlexTimer2");
    }
