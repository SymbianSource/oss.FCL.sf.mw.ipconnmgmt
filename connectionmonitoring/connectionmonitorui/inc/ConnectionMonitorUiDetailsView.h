/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  view class
*     
*
*/


#ifndef CONNECTIONMONITORUIDETAILSVIEW_H
#define CONNECTIONMONITORUIDETAILSVIEW_H

// INCLUDES
#include <aknview.h>
#include <aknlists.h>

// CONSTANTS

/**
* UID of view
*/
const TUid KDetailsViewId = {3};

// FORWARD DECLARATIONS
class CConnectionMonitorUiDetailsContainer;
class CConnectionInfo;
class CConnectionInfoBase;
class CConnectionArray;

// CLASS DECLARATION
/**
*  CConnectionMonitorUiDetailsView view class.
*/
class CConnectionMonitorUiDetailsView : public CAknView, 
                                        public MEikListBoxObserver
    {
    public: // Constructors and destructor
        /**
        * Constructor 
        * @param aConnectionArray array containing connection list
        */
        CConnectionMonitorUiDetailsView( 
                              const CConnectionArray* const aConnectionArray );

        /**
        * EPOC second phase constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CConnectionMonitorUiDetailsView();

    public: // Functions from base classes        
        /**
        * From CAknView
        */
        TUid Id() const;

        /**
        * From CAknView
        */
        void HandleCommandL( TInt aCommand );
        
    public: // New functions
        /**
        * Call SetSelectedConnection function of iContrainer  
        * @param aIndex index of connection in the connection array
        */
        void SetSelectedConnection( TUint aIndex );
        
        /**
        * Returns index of selected connection 
        * @return index of selected connection in the connection array
        */
        TUint GetSelectedConnection();

        /**
        * Returns ETrue if the refreshing stopped for the View
        * @return the staus of View refreshing
        */
        TBool GetViewRefreshState();

        /**
        * Set the View the refreshing status. Stopped or not
        * @param aViewRefreshState staus of View refreshing
        */
        void SetViewRefreshState( TBool aViewRefreshState );

        /**
        * Closes the Details View.
        */
        void CloseViewL();

        /**
        * Activate the Main View
        */
        void ActivateMainViewL();

    protected:
        /**
        * From AknView
        */
        void DoActivateL( const TVwsViewId& aPrevViewId, 
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
        * From AknView
        */
        void DoDeactivate();

        /** 
        * From MEikListBoxObserver    
        */
        void HandleListBoxEventL( CEikListBox* aListBox, 
                                  TListBoxEvent aEventType );

    private: // Data
        /**
        * Container of View
        */
        CConnectionMonitorUiDetailsContainer*   iContainer;

        /**
        * Array contains connections
        */
        const CConnectionArray* const           iConnectionArray; // Not Owned.

        /**
        * The index of selected connection
        */
        TUint                                   iConnectionId;

        /**
        * Staus of View refreshing. If the refreshing stopped its ETrue
        */
        TBool                                   iViewRefreshState;
    };

#endif // CONNECTIONMONITORUIDETAILSVIEW_H

// End of File
