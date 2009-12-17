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
*     The model of RConnectionMobitor.
*
*/


#ifndef DDLG_CONNECTIONMODEL_H
#define DDLG_CONNECTIONMODEL_H

// INCLUDES
#include <e32base.h>
#include <rconnmon.h>
#include <cdblen.h>

// FORWARD DECLARATIONS
class CConnectionInfo;
class CConnectionCArray;
class CDisconnectDialogUi;

// CLASS DECLARATION
/**
* Contains business logic of DisconnectDlg
*/
NONSHARABLE_CLASS( CConnectionModel ) : public CBase, 
                                        public MConnectionMonitorObserver
                                        
    {
    public:

        /**
        * @param CDisconnectDialogUi* notifier will be completed if 
        * @param TDisconnectConnectionPrefs& preferences of the 1st connection
        * connection is closed
        */
        static CConnectionModel* NewL(
                                    CDisconnectDialogUi* aDisconnectDialogUi,
                                    CConnectionInfo* aConnectionInfo );

        /**
        * Destructor.
        */
        virtual ~CConnectionModel();

    private:

        /**
        * Constructor.
        * @param CDisconnectDialogUi* notifier will be completed if 
        * connection is closed
        */
        CConnectionModel( CDisconnectDialogUi* aDisconnectDialogUi );

        /**
        * Constructor.
        * @param CConnectionInfo preferences of the first connection 
        */
        void ConstructL( CConnectionInfo* aConnectionInfo );
    
    protected:
        /**
        * Fills up connection array
        */
        void InitConnectionArrayL();

        /**
        * It is called when RConnectionMonitor has sent its event after ending
        * connection
        * @return -
        */
        void SelectedConnectionClosedL();


    public:

        /**
        * Display information note
        * @param aPromptResourceId - resuorce id to the text
        * @param aPrompt additional text
        */
        void InfoNoteL( const TInt aPromptResourceId, 
                        const TDesC* aPrompt = NULL );


        /**
        * Starts syncronous request to end the given connection.
        * @param aConnId connection index
        * @return the result of the request
        */
        TInt EndConnection( TUint aConnIndex );

        // Getter functions

        /**
        * Get the connection array.
        * @param -
        * @return pointer to the connection array
        */
        CConnectionCArray* GetConnArray() const;


        /**
        * Cancels Notification
        */
        void CancelNotification();
        

    public: // from MConnectionMonitorObserver

        /**
        * Event method is called every time an event occures.
        * @since 
        * @param aConnMonEvent The event information.
        * @return void
        */
        void EventL( const CConnMonEventBase &aConnMonEvent );

    private:
        /**
        * For get connection informations
        */
        RConnectionMonitor iMonitor;

        /**
        * Contains connection info instances
        */
        CConnectionCArray* iConnArray;   ///< Owned.

        TUint iClosingConnectionIndex;  ///< index of connection to be closed

        CDisconnectDialogUi* iDisconnectDialogUi;// Not Owned

        TBool iDeleting;
    };


#endif /* DDLG_CONNECTIONMODEL_H */

// End of File
