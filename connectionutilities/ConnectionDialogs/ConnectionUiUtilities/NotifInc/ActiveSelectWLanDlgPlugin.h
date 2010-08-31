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
* Description:  Declaration of CActiveSelectWLanDlgPlugin
*
*/



#ifndef __ACTIVE_SELECT_WLAN_DLG_PLUGIN_H__
#define __ACTIVE_SELECT_WLAN_DLG_PLUGIN_H__

// INCLUDES
#include "SelectWLanDlgPlugin.h"
#include "SelectWLANDlg.h"
#include "WlanNetworkDataProvider.h"
#include "ConnectionUiUtilitiesCommon.h"

/**
 * CActiveSelectWLanDlgPlugin class,
 * an active object for CSelectWLanDlgPlugin
 */     
NONSHARABLE_CLASS( CActiveSelectWLanDlgPlugin ) : public CActive
{
public:
    /**
    * NewL function
    * @param  aPlugin a pointer to notifier plugin
    */
    static CActiveSelectWLanDlgPlugin* NewL( CSelectWLanDlgPlugin* aPlugin );
    
    /**
    * StartSearchWlanNetworkL setactive and starts active object
    */    
    void StartSearchWlanNetworkL();

    /**
    * Destructor
    */    
    virtual ~CActiveSelectWLanDlgPlugin();
    
    /**
    * Timer callback of iPeridoc.
    */
    static TInt Tick( TAny* aObject );    
    
    /**
    * Timer callback of iPeridoc.
    */
    void DoTick();    
      
        
protected:
    /**
    * ConstructL 
    */
    void ConstructL();
    
    /**
    * Constructor
    * @param  aPlugin a pointer to notifier plugin    
    */    
    CActiveSelectWLanDlgPlugin( CSelectWLanDlgPlugin* aPlugin );
    
    /**
    * Starts Timer
    * @param aTickInterval refersh interval
    */
    void StartTimerL(  TInt aTickInterval );
    
    /**
    * Stops Timer
    */
    void StopTimer();                                            
        
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

    /**
    * Destroy wait dialog 
    */
    void DestroyWaitDialog();
    
    /**
    * Destroy results dialog
    */
    void DestroyResultsListDialog();
    

protected:
    
    // a pointer to notifer, not owned 
    CSelectWLanDlgPlugin* iPlugin;

    //owned
    CWlanNetworkDataProvider* iWlanDataProv;

    // Timer, to refresh details of networks. Owned.
    CPeriodic* iPeriodic;

    // Pointer to the dialog
    CSelectWLANDlg* iResultsListDialog;
    
    CAknWaitDialog* iWaitDialog;
    
};     

#endif // __ACTIVE_SELECT_WLAN_DLG_PLUGIN_H__

// End of file
