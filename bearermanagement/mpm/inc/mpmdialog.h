/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: MPM class used for agent dialog initiation
*
*/

/**
@file mpmdialog.h
Mobility Policy Manager agent dialog initiation class.
*/

#ifndef MPMDIALOG_H
#define MPMDIALOG_H

//  INCLUDES
#include <e32base.h>
#include <agentdialog.h>

#include "mpmiapselection.h"
#include "mpmdialogbase.h" 

// CLASS DECLARATION
/**
*  MPM dialog initiation class definition.
*
*  @lib MPMServer.exe
*  @since 3.0
*/
class CMPMDialog : protected CMPMDialogBase
    {    
public:

    /**
     * Two-phased constructor.
     * @param aCommDbConnPref Connection preferences
     */
    static CMPMDialog* NewL( CMPMIapSelection&          aIapSelection,
                             const RAvailableIAPList&   aIAPList,
                             TUint32                    aBearerSet,
                             CArrayPtrFlat<CMPMDialog>& aQueue,
                             CMPMServer&                aServer );

    /**
     * Destructor.
     */
    virtual ~CMPMDialog();

    /**
     * Write sort snap information to publish and subscribe.
     * 
     * Publish and subscribe is used as there is otherwise circular 
     * reference between MPM and connection dialogs which causes 
     * @since 3.2 
     */
    void PublishSortSnapInfoL();

protected: // Functions from base classes
    /**
     * Called from the Base class implementation 
     * when dialog is started. In the current
     * call stack it's possible to query aDlg for 
     * more information and after that pointer is 
     * invalid.
     */
    virtual void Start( CMPMDialogBase* aDlg = NULL );

    
    /**
     * From CActive. Cancels UI dialog.
     * @since 3.0
     */        
    void DoCancel();

    /**
     * From CActive. Run when UI dialog finished.
     * @since 3.0
     */               
    void RunL();

    /**
     * From CActive. Run if RunL leaves.
     * @since 3.0
     * @param aError Error that caused leave.
     * @return KErrNone
     */
    TInt RunError( TInt aError );

private: // New methods
    /**
     * Initiates a UI dialog towards Agent Dialog Server.
     * @since 3.0
     */
    void InitSelectConnection();

    /**
     * C++ default constructor.
     * @param aCommDbConnPref Connection preferences
     */
    CMPMDialog( CMPMIapSelection&           aIapSelection,
                TUint32                     aBearerSet,
                CArrayPtrFlat<CMPMDialog>&  aQueue,
                CMPMServer&                 aServer );

    /**
     * 2nd phase constructor.
     */
    void ConstructL( const RAvailableIAPList& aIAPList );

    /**
     * Copies aOrigin to aCopy. 
     * @since 3.2 
     * @param aOrigin Original RAvailableIAPList.
     * @param aCopy Copy RAvailableIAPList.
     * @return KErrNone if successful, otherwise one of the 
     * system-wide error codes.
     */
    TInt CopyArray( const RAvailableIAPList& aOrigin, 
                    RAvailableIAPList&       aCopy );

    /**
     * Clean sort snap information from publish and subscribe.
     * 
     * @since 3.2 
     */
    void CleanSortSnapInfo();

private: // Data
    // Agent Dialog server
    RGenConAgentDialogServer    iDlgServ;

    // Iap selection object reference
    CMPMIapSelection&           iIapSelection;

    // SNAP or IAP id
    TUint32                     iSnapOrIAPId;

    // Bearer set as a combination of
    // TExtendedConnPref::TExtendedConnBearer values.
    TUint32                     iBearerSet;

    // Handle to server class
    CMPMServer&                 iServer;

    RAvailableIAPList           iIAPList;
    };


#endif // MPMDIALOG_H

// End of file
