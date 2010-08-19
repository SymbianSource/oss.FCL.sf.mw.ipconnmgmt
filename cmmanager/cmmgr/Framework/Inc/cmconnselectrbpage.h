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
* Description:  Dialog for setting the gateway. It's used temporarily.
*
*/

#ifndef C_CMCONNSELECTRBPAGE_H
#define C_CMCONNSELECTRBPAGE_H

#include <aknradiobuttonsettingpage.h>
#include <csxhelp/cp.hlp.hrh>
#include <cmcommonconstants.h>

#include "cmmanagerimpl.h"
#include "cmconnsettingsuiimpl.h"

/**
 * Application Settings UI radio button page
 *
 *  @lib cmmanager.lib
 *  @since S60 3.2
 */
NONSHARABLE_CLASS(CCMConnSelectRBPage) : public CAknRadioButtonSettingPage
    {
    
   public: // construction/destruction
    
        /**
         * Constructor
         *
         * @since S60 3.2
         * @param aDialogResourceId resource id for the dialog
         * @param aCurrentSelectionIndex the item to be highlighted
         * @param aItemArray the items to be displayed
         * @param aCmManagerImpl reference to the cm manager
         * @param aSelectionMode the selected item type
         * @param aHelpContext help context to use when Help menuitem is selected
         */
        CCMConnSelectRBPage( TInt aDialogResourceId,
                          TInt& aCurrentSelectionIndex, 
                          const MDesCArray* aItemArray, 
                          CCmManagerImpl& aCmManagerImpl, 
                          CMManager::TCmDCSettingSelectionMode& aSelectionMode,
                          RArray<TUint32>& aDestinations,
                          TBool areDestinations, 
                          TBool aOpenDestination,
                          TCoeContextName& aHelpContext );
        
        /**
         * Destructor
         */
        ~CCMConnSelectRBPage();
    
    public: // derived functions
    
        /**
        * From CCoeControl
        *
        * @since S60 3.2
        * @param aKeyEvent The key event.
        * @param aType Key event type.
        * @return key response (was the key event consumed?).
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                     TEventCode aType );    
        
        /**
        * Returns the help context
        * From CCoeControl
        * 
        * @since S60 3.2
        * @param aContext the returned help context
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;
        
        /**
        * Handle list box events
        * From CCoeControl
        * 
        * @since S60 5.0
        * @param aListBox Reference to list box object.
        * @param aEventType Type of event.
        */
        void HandleListBoxEventL(CEikListBox* aListBox, MEikListBoxObserver::TListBoxEvent aEventType);
		
    private: // derived functions
        
        /*
        * From CEikDialog
        *
        * @since S60 3.2
        * @param aButtonId
        */
        TBool OkToExitL( TInt aButtonId );
        
        /*
         * From MEikMenuObserver
         *
         * @since S60 3.2
         * @param aResourceId
         * @param aMenuPane
         */
        void DynInitMenuPaneL( TInt aResourceId,
                               CEikMenuPane* aMenuPane );    
        
        /**
        * From MEikCommandObserver
        *
        * @since S60 3.2
        * @param aCommandId
        */
        void ProcessCommandL( TInt aCommandId );

    private: // data
            
        /**
         * Resource id of the used dialog type
         */
		TInt iDialogResourceId;
		
        /**
         * reference to the cm manager
         */
        CCmManagerImpl& iCmManagerImpl;
        
        /**
         * The selection made by the user
         */
        CMManager::TCmDCSettingSelectionMode& iSelectionMode;
        
        /**
         * A list of destination ids
         */
        RArray<TUint32>& iDestinations;
        /**
         * Indicates whether iDestinations holds destinatos or CMs
         */
        TBool iAreDestinations;
        /**
         * Indicates whether iDestinations should be openable
         */
        TBool iOpenDestination;
        /**
         * help context
         */        
        TCoeContextName iHelpContext;
        /**
         * Previously selected item
         */
        TInt iPrevItem;
        /*
         * pressing the enter key first time will be handled as an uparrow key        
         */
        TBool iFirstEnter;
    };
    
#endif // C_CMAPPSETTRBPAGE_H
