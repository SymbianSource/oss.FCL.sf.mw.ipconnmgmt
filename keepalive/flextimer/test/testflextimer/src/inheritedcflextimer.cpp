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
#include "inheritedcflextimer.h"
//For exteded test result
#include "testflextimer.h"
// External function prototypes
// Local constants go here
// Panic category

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// Constructs the object.
// ---------------------------------------------------------------------------
//
CInheritedCFlexTimer* CInheritedCFlexTimer::NewL( TInt aPriority,
        TCallBack aCallBack )
    {

    CInheritedCFlexTimer* self = new (ELeave) CInheritedCFlexTimer(aPriority,
            aCallBack);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructs the object.
// ---------------------------------------------------------------------------
//
CInheritedCFlexTimer::~CInheritedCFlexTimer()
    {

    }

// ---------------------------------------------------------------------------
// Desc
// ---------------------------------------------------------------------------
//
void CInheritedCFlexTimer::SetCB( TCallBack aCallBack )
    {
    iCallBack = aCallBack;
    }

// ---------------------------------------------------------------------------
// Desc
// ---------------------------------------------------------------------------
//
TInt CInheritedCFlexTimer::MyOwnConfigure(
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
void CInheritedCFlexTimer::RunL()
    {
    //Give code to test case
    TExtendedTestResult *eRes =
            reinterpret_cast<TExtendedTestResult*> (iCallBack.iPtr);
    eRes->iStatusCode = iStatus.Int();
    iCallBack.CallBack();
    }

// ---------------------------------------------------------------------------
// Second part of the two-phase construction.
// ---------------------------------------------------------------------------
//
void CInheritedCFlexTimer::ConstructL()
    {
    CActiveScheduler::Add(this);
    CFlexTimer::ConstructL();
    }

// ---------------------------------------------------------------------------
// Private constructor with priority.
// ---------------------------------------------------------------------------
//
CInheritedCFlexTimer::CInheritedCFlexTimer( TInt aPriority,
        TCallBack aCallBack ) :
    CFlexTimer(aPriority), iCallBack(aCallBack)
    {

    }
