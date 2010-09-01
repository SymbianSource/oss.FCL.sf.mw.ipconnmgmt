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


#ifndef CONNECTIONMONITORUICONTAINER_H
#define CONNECTIONMONITORUICONTAINER_H

// INCLUDES
#include <AknUtils.h>
#include "ConnectionMonitorUiAppUi.h"
   
// FORWARD DECLARATIONS
class CConnectionArray;
class RConnectionMonitor;
class CAknNavigationDecorator;
class CAknNavigationControlContainer;
class CActiveWrapper;

// CLASS DECLARATION
/**
*  CConnectionMonitorUiContainer  container control class.
*/
class CConnectionMonitorUiContainer : public CCoeControl,
                                      public MActiveEventObserverInterface
    {
    public: // Constructors and destructor
        /**
        * Constructor
        * @param aConnectionArray array contains connection infos
        * @param aMonitor to handlin connection details
        * @param aActiveWrapper the for refreshing
        */
        CConnectionMonitorUiContainer( 
                                     CConnectionArray* const aConnectionArray,
                                     RConnectionMonitor* aMonitor,
                                     CActiveWrapper* aActiveWrapper );
        
        /**
        * Second phase constructor.
        * @param aSelectedItem index of selected item in the list
        * @param aRect Frame rectangle for container.        
        */
        void ConstructL( TInt aSelectedItem, const TRect& aRect  );

        /**
        * Destructor.
        */
        ~CConnectionMonitorUiContainer();

    public: // Functions from base classes
    
        /** 
        * From CCoeControl, gets the control's help context.
     	* @param aContext help context.
        */
        virtual void GetHelpContext( TCoeHelpContext& aContext ) const;

        /**
        * From CoeControl OfferKeyEventL.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

        /**
        * From CoeControl,CountComponentControls.
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl, ComponentControl.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl        
        */
        void HandleResourceChange( TInt aType ) ;
        
    public:  
        /**
        * from MActiveEventObserverInterface
        */
        void OnEventL( const CConnMonEventBase& aConnMonEvent, 
                       const TInt aIndex );
        /**
        * from MActiveEventObserverInterface
        */
        void OnTimerEventL();

        /**
        * Returns the item index of selected connection
        * @return index of selected connection
        */
        TInt SelectedConnection();

        /**
        * Close the selected connection.
        */
        void EndConnectionL();

        /**
        * Close all connection
        */
        void EndAllConnectionL();

        /**
        * Returns pointer to the listbox.
        * @return pointer to the listbox.
        */
        CAknDoubleGraphicStyleListBox*  ListBox() const;

        /**
        * Refresh the content of Navi pane and 
        * Push it to stack. Called by View DoActivateL()
        */
        void PushAndRefreshNaviPaneL();

        /**
        * Pop and destroy the NaviPane from stack. 
        * Called by View DoDeactivateL()
        */
        void PopNaviPane();

        /**
        * Called by HandleForegroundEventL, draw all item of list box.
        */
        void DrawFirstAllListBoxItem();
    
    protected:
        /**
        * From CoeControl,SizeChanged.
        */
        void SizeChanged();
        
        /**
        * From CoeControl,SizeChanged.
        */        
        virtual void FocusChanged(TDrawNow aDrawNow);

    protected: // New functions
        /**
        * Sets the icons of listbox
        */
        void SetIconsL();

        /**
        * Gets the title panes.
        */
        void GetPanesL();        

    protected: // data
        /**
        * List box contains connection items
        */
        CAknDoubleGraphicStyleListBox* iListBox;
        
        /**
        * To handling details of connections
        */
        RConnectionMonitor* iMonitor;   

        /**
        * Contains connections info
        */
        CConnectionArray* const iConnectionArray;

        /**
        * Owned. For NaviPane.
        */
        CAknNavigationDecorator* iNaviDecorator;

        /**
        * Not Owned. For NaviPane.
        */
        CAknNavigationControlContainer* iNaviPane;
    
    private: // data

        /**
        * ETrue if Wlan connection is supported, EFalse otherwise.
        */
        TBool isWlanSupported;
        
        /**
        * The active object for info. refreshing. NOT Owned.
        */
        CActiveWrapper* iActiveWrapper;
        
        /**
        * Connection count
        */
        TInt iOldConnectionCount;
        
        /**
        * Set when main view list comes to focus
        */
        TBool iFocusChanged;
    };

#endif // CONNECTIONMONITORUICONTAINER_H

// End of File
