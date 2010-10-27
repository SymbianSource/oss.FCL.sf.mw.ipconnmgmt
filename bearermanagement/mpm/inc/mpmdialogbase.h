/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MPM class used for agent dialog initiation
*
*/


/**
@file mpmdialogbase.h
Mobility Policy Manager agent dialog initiation class.
*/

#ifndef MPMDIALOGBASE_H
#define MPMDIALOGBASE_H

//  INCLUDES
#include <e32base.h>
#include <agentdialog.h>

#include "mpmlogger.h"

// CLASS DECLARATION
/**
*  MPM dialog initiation class definition.
*
*  @lib MPMServer.exe
*/
class CMPMDialogBase : public CActive
    {
protected: // Constructors and destructor

    /**
     * C++ default constructor.
     * @param aQueue Dialog queue.
     */
    CMPMDialogBase( CArrayPtrFlat<CMPMDialogBase>& aQueue );

    /**
     * 2phase constructor
     */
    virtual void ConstructL();
    
    /**
     * C++ destructor.
     */
    virtual ~CMPMDialogBase();

protected: // Functions from base classes

    /**
     * From CActive. Cancels UI dialog.
     * @since 3.0
     */        
    virtual void DoCancel() = 0;

    /**
     * From CActive. Run when UI dialog finished.
     * @since 3.0
     */               
    virtual void RunL() = 0;

    /**
     * From CActive. Run if RunL leaves.
     * @since 3.0
     * @param aError Error that caused leave.
     * @return KErrNone
     */
    virtual TInt RunError( TInt aError ) = 0;

    /**
     * Called automatically from the Base class implementation.
     * Creates the dialog and request.
     */
    virtual void Start( CMPMDialogBase* aDlg = NULL ) = 0;

    /**
     * @return ETrue if the instance of dialog is first in the queue.
     */
    TBool IsFirst();

private: // member variables

    CArrayPtrFlat<CMPMDialogBase>& iQueue;
    };

#endif //MPMDIALOGBASE_H
