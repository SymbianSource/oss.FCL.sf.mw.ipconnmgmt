/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Signaling class for destination edit.
*
*/


#ifndef C_CCMDEXEC_H
#define C_CCMDEXEC_H

class MCmdExec;

NONSHARABLE_CLASS(CCmdExec) : public CActive
    {

public:

    /**
    * Destructor.
    */
    virtual ~CCmdExec();
    
    void Execute();
    
// from base class CActive
public:
    virtual void RunL();

    virtual TInt RunError( TInt /*aError*/ );

    virtual void DoCancel();

    CCmdExec(MCmdExec& aOwner);

protected: // data

    /**
     * Owner
     * Not own.
     */
    MCmdExec& iOwner;
    };

#endif
