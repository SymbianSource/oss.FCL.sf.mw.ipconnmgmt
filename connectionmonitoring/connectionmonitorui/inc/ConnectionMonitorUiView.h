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


#ifndef CONNECTIONMONITORUI_VIEW_H
#define CONNECTIONMONITORUI_VIEW_H

// INCLUDES
#include <aknview.h>
#include <aknlists.h>

// CONSTANTS

// UID of view
const TUid KConnectionsViewId = {2};

// FORWARD DECLARATIONS
class CConnectionMonitorUiContainer;
class CConnectionArray;
class CActiveWrapper;

// CLASS DECLARATION
/**
*  CConnectionMonitorUiView view class.
*/
class CConnectionMonitorUiView : public CAknView, public MEikListBoxObserver
    {
    public: // Constructors and destructor
        /**
        * Constructor
        * @param aConnectionArray array contains connection infos
        * @param aMonitor Connection Monitor engine session
        * @param aActiveWrapper the for refreshing
        */
#ifdef RD_CONTROL_PANEL        
        CConnectionMonitorUiView( CConnectionArray* const aConnectionArray,
                                  RConnectionMonitor* aMonitor,
                                  TBool aIsEmbedded,
                                  CActiveWrapper* aActiveWrapper );
#else
        CConnectionMonitorUiView( CConnectionArray* const aConnectionArray,
                                  RConnectionMonitor* aMonitor,
                                  CActiveWrapper* aActiveWrapper );
#endif // RD_CONTROL_PANEL                                  

        /**
        * EPOC default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CConnectionMonitorUiView();

    public: // Functions from base classes
        /**
        * From AknView
        */
        TUid Id() const;

        /**
        * From AknView
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From MEikMenuObserver 
        * Dynamic handling of menus.
        */
        virtual void DynInitMenuPaneL( TInt aResourceId,
                                       CEikMenuPane* aMenuPane );

        /**
        * Observe the connection stat and set the right Soft Keys.
        */
        void ConnectionKeyObserver() const;

        /**
        * Workaround for Avkon event sequence problem, occuring when
        * connection list has been added elements.
        */
        void HandleItemAdditionL();

    protected: 
        /**
        * from MEikListBoxObserver    
        */
        void HandleListBoxEventL( CEikListBox* aListBox, 
                                  TListBoxEvent aEventType );

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
        * Sets the tile pane to the given resource.
        * @param aResourceId the id of resource string
        */
        void SetTitlePaneTextL( TInt aResourceId );

    private:
        /**
        * Brings up details view.
        */
        void BringupDetailsViewL();
        
        /**
        * Check if selecred connection is Alilve or not
        * @return if the selected connection is alive ETrue, oterwise EFalse
        */
        TBool IsSelectedConnectionAlive() const;
        
        /**
        * Check if selecred connection is Suspended or not
        * @return if the selected connection is alive ETrue, oterwise EFalse
        */
        TBool IsSelectedConnectionSuspended() const;

    private: // Data   
        /**
        * Continer instance of view.
        */
        CConnectionMonitorUiContainer*        iContainer; // Owned

        /**
        * Constant pointe to array of connections.
        */
        CConnectionArray* const             iConnectionArray; // Not Owned
        
        /**
        * Pointer to RConnectionMonitor for deleting connection(s).
        */
        RConnectionMonitor*                 iMonitor;  // Not Owned
                
        /**
        * ETrue if Help is supported, EFalse otherwise.
        */
        TBool isHelpSupported;

#ifdef RD_CONTROL_PANEL

        /**
        * ETrue:  The application is launched in embedded mode.
        * EFalse: The application is launched in standalone mode.
        */
        TBool iIsEmbedded;

#endif // RD_CONTROL_PANEL

        /**
        * The active object for info. refreshing. NOT Owned.
        */
        CActiveWrapper* iActiveWrapper;
    };

#endif // CONNECTIONMONITORUI_VIEW_H

// End of File
