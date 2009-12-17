/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of CActiveDisconnectDlgPlugin
*
*/



#ifndef __ACTIVE_DISCONNECT_DLG_PLUGIN_H__
#define __ACTIVE_DISCONNECT_DLG_PLUGIN_H__

// INCLUDES
#include "DisconnectDlgUi.h"
#include "DisconnectDlgDialog.h"

// FORWARD DECLARATIONS
class CConnectionInfo;

/**
 * CActiveDisconnectDlgPlugin class,
 * an active object for CDisconnectDialogUi
 */     
NONSHARABLE_CLASS( CActiveDisconnectDlgPlugin ) : public CActive
    {
    public:
        /**
        * NewL function
        * @param  aPlugin a pointer to notifier plugin
        */
        static CActiveDisconnectDlgPlugin* NewL( 
                                                CDisconnectDialogUi* aPlugin );

        /**
        * StartSearchConnections setactive and starts active object
        */    
        void StartSearchConnections();

        /**
        * Destructor
        */    
        virtual ~CActiveDisconnectDlgPlugin();          
        
    protected:
        /**
        * ConstructL 
        */
        void ConstructL();
    
        /**
        * Constructor
        * @param  aPlugin a pointer to notifier plugin    
        */    
        CActiveDisconnectDlgPlugin( CDisconnectDialogUi* aPlugin );
            
        /**
        * DoCancel from CActive
        */    
        virtual void DoCancel();
      
        /**
        * RunL from CActive
        */      
        virtual void RunL();
    
        /**
        * Check if there is any connection
        * @param CConnectionInfo of the first finded connection
        * @return ETrue if there is any connection, EFalse otherwise
        */
        TBool IsConnectionL( CConnectionInfo* aConnectionInfo );
    
            
    protected:

        ///< a pointer to notifer, not owned     
        CDisconnectDialogUi* iPlugin;    

        ///< used for complete runl, not owned  
        TRequestStatus* iClientStatus;
    };     

#endif // __ACTIVE_DISCONNECT_DLG_PLUGIN_H__

// End of file
