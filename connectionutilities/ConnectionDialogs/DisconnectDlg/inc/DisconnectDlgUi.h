/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Declares the main handler, CDisconnectDialogUi and the public API for 
*     the Disconnect Dialog UI.
*
*/


#ifndef DDLG_UI_H
#define DDLG_UI_H

// INCLUDES
#include <e32base.h>
#include <rconnmon.h>
#include <eiknotapi.h>
#include <DisconnectDlgClient.h>


// GLOBAL FUNCTIONS

// FORWARD DECLARATIONS
class CConnectionInfo;
class CDisconnectDlgDialog;
class CConnectionModel;
class CQueryDisconnectDlgDialog;
class CActiveDisconnectDlgPlugin;

// CLASS DECLARATION

/**
* Notifier class for DisconnectDlg
*/
NONSHARABLE_CLASS( CDisconnectDialogUi ) : public CBase,
                                           public MEikSrvNotifierBase2
    {
    public: // Constructors and destructor

        /**
        * Constructor.
        */
        static CDisconnectDialogUi* NewL();

        /** 
        * Destructor.
        */
        virtual ~CDisconnectDialogUi();

    public:

        /**
        * Component Validation Test.
        * @return KErrNone.
        */
        static TInt Cvt(); 

    protected: // Constructors

        /**
        * Constructor.
        */
        CDisconnectDialogUi();

        /**
        * Second-phase constructor.
        */
        void ConstructL( );

    public:
        /**
        * Creates a query dialog.
        * @return ETrue if  query created successfully.      
        */
        TInt AskDisconnectL();

    public: // from MEikSrvNotifierBase

        /**
        * from MEikSrvNotifierBase
        */
        void Release();
        
        /**
        * from MEikSrvNotifierBase
        */        
        TNotifierInfo RegisterL();

        /**
        * from MEikSrvNotifierBase
        */
        TNotifierInfo Info() const;

        /**
        * from MEikSrvNotifierBase
        */
        TPtrC8 StartL( const TDesC8& aBuffer );

        /**
        * Start the Notifier
        * @param  aBuffer info from caller  
        * @param  aReplySlot reply info
        * @param  aMessage   
        * return -
        */
        void StartL( const TDesC8& aBuffer, TInt aReplySlot, 
                     const RMessagePtr2& aMessage );

        /**
        * from MEikSrvNotifierBase
        */
        void Cancel();

        /**
        * from MEikSrvNotifierBase
        */        
        TPtrC8 UpdateL( const TDesC8& aBuffer );

    public:// from MActiveEventObserverInterface

        /**
        * Notify when the initialization process has been done
        * @return -
        */
        void InitializedL( CConnectionInfo* aConnectionInfo );
        
        /**
        * Sets selected IapId
        */
        void SetClosedIapId( TUint32 aIapId );

    public: // New functions

        void CompleteL( TInt aStatus );

        inline void SetStartedFlag( TBool aStarted );

        inline TBool Prompt();
        
    public:
    
        TBool iDialogRun;       ///< ETrue if dialog is run  
        TBool iQueryRun;       ///< ETrue if query is run

    private: //data

        TInt                  iResOffset; ///< Offset of loaded resource file
        
        /**
        * refers connections is intialized or not
        */
        TNotifierInfo         iInfo;             ///< Notifier info

        RMessagePtr2          iMessage;      // Message

        CDisconnectDlgDialog* iDisconnectDialog; ///< Owned.

        CQueryDisconnectDlgDialog* iQueryDisconnectDlgDialog; ///< Owned.

        CConnectionModel* iConnModel; ///< Owned

        /**
        * After the query is dismissed, the index will hold
        * the value of selected item at the iDisconnectDialog.
        */
        TInt iIndex; 

        TBool iStarted;           ///< ETrue if notifier is started
        
        HBufC*  iErrorMsgDetails;   ///< For query dialog

        TDisconnectConnectionPrefs  iConnectionPrefs;
        
        TInt iReplySlot;            // Reply slot
        
        TUint32 iIapId;            ///< selected Internet Access Point  
        
        CActiveDisconnectDlgPlugin* iActivePlugin; ///< Owned
    };

#include "DisconnectDlgUi.inl"

#endif /* DDLG_UI_H */

// End of File
