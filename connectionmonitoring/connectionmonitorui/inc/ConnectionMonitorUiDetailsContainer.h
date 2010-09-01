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
* Description:  container control class
*     
*
*/

#ifndef CONNECTIONMONITORUIDETAILSCONTAINER_H
#define CONNECTIONMONITORUIDETAILSCONTAINER_H

// INCLUDES
#include <aknlists.h>
#include "ConnectionMonitorUiAppUi.h"

// FORWARD DECLARATIONS
class CConnectionArray;
class CAknNavigationDecorator;
class CAknNavigationControlContainer;
class CConnectionMonitorUiDetailsView;

// CLASS DECLARATION
/**
*  CConnectionMonitorUiDetailsContainer  container control class.
*/
class CConnectionMonitorUiDetailsContainer : 
                                          public CCoeControl,
                                          public MActiveEventObserverInterface,
                                          public MAknNaviDecoratorObserver
    {
    public: // Constructors and destructor
        /**
        * Constructor
        * @param aConnectionArray array containing connection list
        */
        CConnectionMonitorUiDetailsContainer( 
                            const CConnectionArray* const aConnectionArray );

        /**
        * EPOC second-phase constructor.
        * @param aRect Frame rectangle for container.
        * @param aParentView ParentView of the container
        */
        void ConstructL( const TRect& aRect, 
                              CConnectionMonitorUiDetailsView* aParentView );

        /**
        * Destructor.
        */
        ~CConnectionMonitorUiDetailsContainer();

    public: // New functions
        /**
        * Sets iConnectionInfo and iConnectionId to the selected
        * connection
        * @param aConnectionIndex the index of selected connection
        */
        void SetSelectedConnectionL( TUint aConnectionIndex );

        /**
        * Returns pointer to the listbox.
        * @return pointer to the listbox.
        */
        CAknSingleHeadingStyleListBox* ListBox() const;

        /**
        * Refresh the content of Navi pane and 
        * Push it to stack. Called by View DoActivateL()
        */
        void PushAndRefreshNaviPaneL();

        /**
        * Pop the NaviPane from stack. Called by View DoDeactivateL()
        */
        void PopNaviPane();


    public: 
        /**
         * Handler for pointer events, when the Navi Pane arrows have been tapped (from MAknNaviDecoratorObserver)
         *
         * @param aEventID  The enum indicating wether right or left arrow was tapped
         */
        void HandleNaviDecoratorEventL( TInt aEventID ); 

        /**
        * from MActiveEventObserverInterface
        */
        virtual void OnEventL( const CConnMonEventBase& aConnMonEvent,
                               const TInt aIndex );

        /**
        * from MActiveEventObserverInterface
        */
        void OnTimerEventL();

        /**
        * From CCoeControl
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

        /**
        * Called by HandleForegroundEventL, draw all item of list box.
        */
        void DrawFirstAllListBoxItem();

        /**
        * From CCoeControl,ComponentControl.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;
        
	    /**
        * From CCoeControl, used for sclable ui and landscape support.
        */        
        void HandleResourceChange( TInt aType );    
        
        /**
        * Gets index of the selected item
        * @return the index of selected item
        */
        TInt GetIndexOfSelectedItem(); 
        
        /**
        * Gets iConnectionInfo of the selected connection
        * @return CConnectionInfoBase* pointer to the connection info object
        */
        CConnectionInfoBase* GetSelectedConnectionInfo() const;

    protected:
        /**
        * Gets the title panes.
        */
        void GetPanesL();

    private: // Functions from base classes
        /**
        * From CoeControl,SizeChanged.
        */
        void SizeChanged();

        /**
        * From CoeControl,CountComponentControls.
        */
        TInt CountComponentControls() const;
        
        /**
        * From CoeControl,SizeChanged.
        */        
        virtual void FocusChanged(TDrawNow aDrawNow);        

    protected: // data       
        /**
        * Listbox contains details a connection
        */
        CAknSingleHeadingStyleListBox* iListBox;

        /**
        * Refers to the current selected connection
        */
        CConnectionInfoBase*            iConnectionInfo;    // Not Owned.

        /**
        * The index of selected connection
        */
        TUint                           iConnectionIndex;
        
        /**
        * A pointer to connections array
        */
        const CConnectionArray* const   iConnectionArray; // Not Owned.

        /**
        * Owned. For NaviPane.
        */
        CAknNavigationDecorator*        iNaviDecorator;

        /**
        * Not Owned. For NaviPane.
        */
        CAknNavigationControlContainer* iNaviPane;

        /**
        * Not Owned. A pointer to DetailsView
        */
        CConnectionMonitorUiDetailsView* iParentView; // Not Owned.
    };

#endif

// End of File
