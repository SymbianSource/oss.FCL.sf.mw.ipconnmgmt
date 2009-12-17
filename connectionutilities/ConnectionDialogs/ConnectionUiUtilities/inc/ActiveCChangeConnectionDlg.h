/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Declares CActiveCChangeConnectionDlg.
*
*/


#ifndef ACTIVE_CHANGECONNECTIONDLG_H
#define ACTIVE_CHANGECONNECTIONDLG_H


// INCLUDES
#include "ActiveIAPListing.h"
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat.h>
#else
#include <commsdat.h>
#include <commsdat_partner.h>
#endif


// FORWARD DECLARATION
class CChangeConnectionDlg;

/**
 * CActiveCChangeConnectionDlg class,
 * an active object for CChangeConnectionDlg
 */     
NONSHARABLE_CLASS( CActiveCChangeConnectionDlg ) : public CActiveIAPListing
{
public:
    /**
    * NewL function
    * @param aIAPId iap id
    * @param aPrefs preferences pointer    
    */
    static CActiveCChangeConnectionDlg* NewL( TUint32* aIAPId, 
                                              const TConnectionPrefs& aPrefs,
                                              TDes& aConnectionName );
    
    /**
    * StartSearchIAPs setactive and starts active object
    * @return ETrue if Select Soft key was pressed, 
    * EFalse if Cancel Soft key was selected on the dialog.
    */    
    TBool StartSearchIAPsL();

    /**
    * Destructor
    */    
    virtual ~CActiveCChangeConnectionDlg();
    
    /**
    * Cancels the active object
    */
    void Cancel();

    inline void SetSelected( TBool aSelected );

    inline CConnectionInfoArray* ActIAPs();

                          
protected:
    /**
    * ConstructL 
    */
    void ConstructL();
    
    /**
    * Constructor
    * @param aIAPId iap id
    * @param aPrefs preferences pointer        
    */    
    CActiveCChangeConnectionDlg( TUint32* aIAPId, 
                                 const TConnectionPrefs& aPrefs,
                                 TDes& aConnectionName );
    
    /**
    * DoCancel from CActive
    */    
    virtual void DoCancel();

    /**
    * RunL from CActive
    */
    virtual void RunL();

    /**
    * Gives back, refresh interval of scan for network
    */
    TInt GetRefreshInterval();

protected:
    
    CChangeConnectionDlg* iDialog;    ///< a pointer to dialog, not owned     
    
    TBool isDialogDeleting; 

    TUint32*                       iIAPId;      // ID of IAP        
    
    TBuf<CommsDat::KMaxTextLength> iConnectionName;  
    
    CActiveSchedulerWait            iWait;
    
    TBool                           iSelected;
      
};     

#include "ActiveCChangeConnectionDlg.inl"

#endif // ACTIVE_CHANGECONNECTIONDLG_H

// End of File