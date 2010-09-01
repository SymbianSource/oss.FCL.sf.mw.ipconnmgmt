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
* Description:  
*     Container class for APControl list plugin.
*
*/


#ifndef APCONTROLLIST_PLUGIN_CONTAINER_H
#define APCONTROLLIST_PLUGIN_CONTAINER_H

// INCLUDES
#include <coeccntx.h>
#include <eikclb.h>

// FORWARD DECLARATIONS
class CApControlListbox;
class CAknNavigationDecorator;
class CAknNavigationControlContainer;
class CAPControlListPlugin;
class MEikListBoxObserver;
class CApControlListboxModel;

// UID of general settings app, in which help texts are included
const   TUid    KHelpUidAPControlList = { 0x100058EC };


// CLASS DECLARATION

/**
*  CAPControlListPluginContainer container class
*  container class for APControl list plugin 
*/
class CAPControlListPluginContainer : public CCoeControl
    {
    
    public: 
        
        /**
        * Symbian OS constructor.
        *
        * @param aRect Listbox's rect.
        * @param aListObserver pointer to the list observer
        */
        void ConstructL( const TRect& aRect, 
                         MEikListBoxObserver* aListObserver );

        /**
        * Destructor.
        */
        ~CAPControlListPluginContainer();
    
    public: // From CCoeControl
    
        /**
        * From CCoeControl, gives back the count of contained controls.
        */
        virtual TInt CountComponentControls() const;

        /**
        * From CCoeControl, gives back the given indexed 
        * from contained controls.
		* @param aIndex index
        */        
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl, resizes the contained controls.
        */
        virtual void SizeChanged();
        
        /**
        * From CCoeControl, processes the given key event.
	    * @param aKeyEvent The key event that occured.
		* @param aType The type of key event that occured.
	    * @return @c EKeyWasConsumed if the event was processed,
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                     TEventCode aType );
    
        /** 
        * From CCoeControl, gets the control's help context.
     	* @param aContext help context.
        */
        virtual void GetHelpContext( TCoeHelpContext& aContext ) const;
        
        /**
        * From CCoeControl, used for sclable ui and landscape support.
     	* @param aType A message UID value.
        */        
        void HandleResourceChange( TInt aType );
          
        /**
        * From CoeControl,SizeChanged.
        * @param aDrawNow Contains the value that was passed to it by 
        *        @c SetFocus(). 
        */        
        virtual void FocusChanged(TDrawNow aDrawNow);  
        
        /**
        * Adds to the ListBox
        * @param newItem new item.
        */        
        void AddToListBoxL(TPtrC& newItem);              
        
        /**
        * Sets Current Item Index in the ListBox
        * @param index Current item index.
        */        
		void SetCurrentItemIndex(TInt index);
		
        /**
        * Gets current item index
        * @return Current item index in the ListBox
        */        
		TInt CurrentItemIndex();
		
        /**
        * Removes current item from ListBox
        */        
		void RemoveFromListBoxL();
		
        /**
        * Gets the number of items in the list
        * @return the number of items in the list
        */
        TInt NumberOfItems() const;
		
        /**
        * Writes text with ID to the NaviPane
        */
        void WriteToNaviPaneL( TInt aResourceId );
        
    private:

        /**
        * Sets the title pane to the appropriate text.
        * @param aResourceId Resource id of the showed text .
        */
        void TitlePaneTextsL( TInt aResourceId );

    public:
        
        //@var Listbox  - owned
        CApControlListbox* 				iListBox;
        //@var Model for the listbox  - owned
        CApControlListboxModel*     	iModel;

    private:
    
        //@var For NaviPane. - owned
        CAknNavigationDecorator* 		iNaviDecorator;

        //@var For NaviPane. - owned
        CAknNavigationControlContainer* iNaviPane;
        
    };

#endif //APCONTROLLIST_PLUGIN_CONTAINER_H   
