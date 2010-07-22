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
 * Description:  CInheritedCFlexTimer inherits and implements CFlexTimer for testing purposes.
 *
 */

/*
 * %version: 1 %
 */

#ifndef CINHERITEDCFLEXTIMER_H
#define CINHERITEDCFLEXTIMER_H

// INCLUDE FILES
#include "flextimer.h"

// CLASS DECLARATION
/**
 *  desc
 *
 *  @see CFlexTimer
 *  @see CPeriodic
 *
 */
class CInheritedCFlexTimer : public CFlexTimer
    {
public:

    /**
     * A leaving constructor for the object.
     * @param aPriority of the active object. If timing is critical, it
     * should be higher than that of all other active objects owned by the
     * scheduler.
     * @return A pointer to a CInheritedCFlexTimer object on success.
     * On error it leaves.
     */
    static CInheritedCFlexTimer* NewL( TInt aPriority,
            TCallBack aCallBack );

    /**
     * Destructor for the object.
     */
    virtual ~CInheritedCFlexTimer();

    /**
     * desc
     */
    void SetCB( TCallBack aCallBack );

    /**
     * desc
     */
    TInt MyOwnConfigure( TTimeIntervalMicroSeconds32 aDelayWindow );

protected:

    /**
     * Inherited from CActive.
     */
    virtual void RunL();

private:

    /**
     * Constructs the object. The second phase of the construction.
     */
    void ConstructL();

    /**
     * Private constructor for the object.
     * @param aPriority The priority of the active object. If timing is
     * critical, it should be higher than that of all other active objects
     * owned by the scheduler.
     */
    CInheritedCFlexTimer( TInt aPriority, TCallBack aCallBack );

private:

    /**
     * The callback function which is called at the completion of
     * flextimer server requests.
     */
    TCallBack iCallBack;

    };

#endif /* CINHERITEDCFLEXTIMER_H */
