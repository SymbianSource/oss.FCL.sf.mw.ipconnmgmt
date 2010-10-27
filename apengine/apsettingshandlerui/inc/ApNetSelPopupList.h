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
* Description: 
*     Declares CApNetSelPopupList for network selection.
*
*/


#ifndef APNETSEL_POPUP_LIST_DIALOG_H
#define APNETSEL_POPUP_LIST_DIALOG_H

// INCLUDES

#include <aknradiobuttonsettingpage.h>
#include <ActiveApDb.h>  // for MActiveApDbObserver

#include <ApSettingsHandlerCommons.h>


// FORWARD DECLARATIONS

class CApNetSelectorListboxModel;
class CApSettingsHandlerImpl;
class CApSettingsModel;


// CLASS DECLARATION
/**
* CApNetSelPopupList class
* Pop-up network selector 'control' object.
*/
NONSHARABLE_CLASS( CApNetSelPopupList ) : public CAknRadioButtonSettingPage, 
                                          public MActiveApDbObserver
    {

    public: // Constructors and destructor

        /**
        * 2 phase construction
        * @param aModel     The data model object
        * @param aHandler   The data handler object
        * @param aCurrentSelectionIndex The required selection index
        * @param aPreferredUid  The preferred UID
        * @param aEventStore    Stores the events
        * @param aItemArray     Itemarray
        * @param aNeedsNone Gives whether it disoplays the item 'None' or not
        * @return the constructed CApNetSelPopupList object
        */
        static CApNetSelPopupList* NewL
                                (
                                CApSettingsModel& aModel,
                                CApSettingsHandlerImpl& aHandler,
                                TInt& aCurrentSelectionIndex,
                                TInt& aPreferredUid,
                                TUint32& aEventStore,
                                CDesCArrayFlat* aItemArray,
                                TBool aNeedsNone
                                );

        // Two phased construction
        virtual void ConstructL();

    private:
        /**
        * 2 phase construction
        * @param aModel     The data model object
        * @param aHandler   The data handler object
        * @param aCurrentSelectionIndex The required selection index
        * @param aPreferredUid  The preferred UID
        * @param aEventStore    Stores the events
        * @param aItemArray     Itemarray
        * @param aNeedsNone Gives whether it disoplays the item 'None' or not
        * @return the constructed CApNetSelPopupList object
        */
        CApNetSelPopupList
            (
            CApSettingsModel& aModel,
            CApSettingsHandlerImpl& aHandler,
            TInt& aIndex,
            TInt& aPreferredUid,
            CDesCArrayFlat* aItemArray,
            TUint32& aEventStore,
            TBool aNeedsNone
            );

        virtual ~CApNetSelPopupList();

    public: // New functions
        void SetHighlighted();

    private:
        /**
        * It tells whether the Select menu item needs to be deleted or not
        * @param aResourcfeId   The menu resource ID
        * @return Boolean indicating whether the Select menu item 
        * needs to be deleted or not
        */
        TBool Need2DeleteSelect( TInt aResourceId );

        /**
        * It tells whether the Open & the Dlete menu item needs
        * to be deleted or not
        * @param aResourcfeId   The menu resource ID
        * @return Boolean indicating whether the Delete menu item 
        * needs to be deleted or not        
        */
        TBool Need2DeleteOpenDelete( TInt aResourceId );


        /**
        * Creates a new network group
        * @return TBool indicating successful creation
        */
        TBool HandleCreateNewNetworkGroupL();


    public: // Functions from base classes
        // From CCoeControl
        virtual void GetHelpContext(TCoeHelpContext& aContext) const;

        /**
        * Implements the MActiveApDbObserver protocol
        */
        virtual void HandleApDbEventL( TEvent anEvent );


        /**
        * Overrides ActivateL
        */
        virtual void ActivateL();

    protected:
        // From CAknRadioButtonPage
        /*
        * Needed to know about SELECTION (not HIGHLIGHT!) changes
        * to be able to update currently valid selection UID.
        */
        virtual void SelectCurrentItemL();

    private:
        /**
        * From CAknSettingPage
        */
        TBool OkToExitL( TInt aButtonId );

    protected:  // New functions
        /**
        * Fills up listbox control with data dynamically
        */
        void FillListBoxWithDataL();

        /*
        * Sets the Selected item in the list.
        * Needed to initialize the list according to the Uid and 
        * not to 'itemindex'...
        */
        void SetSelectedL();

    protected:  // Functions from base classes

        /**
        * From CAknSettingPage
        */
        virtual void DynInitMenuPaneL( TInt aResourceId, 
                                       CEikMenuPane* aMenuPane );

        // From MEikCommandObserver
        virtual void ProcessCommandL( TInt aCommandId );

        // From CCoeControl
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                     TEventCode aType);

    private: //data
        CApSettingsModel*           iDataModel; // not owned, passed for usage
        CAknSetStyleListBox*        iList;
        
        CApNetSelectorListboxModel* iModel;     // owns it, just passes 
                                                // items for usage to list!
        
        TInt*                       iSelected;  // not owns it,passed for usage
        CApSettingsHandlerImpl*     iHandler;   // not owns it,passed for usage
        TUint32*                    iEventStore;// to hold the events
        TUint32                     iPreferredUid;
        TInt                        iPreferredLine;
        TBool                       iNeedsNone;
        TBool                       iNeedUnlock;
        TBool						iHelpSupported;
        TBool                       iProtectionSupported;

    };

#endif // APNETSEL_POPUP_LIST_DIALOG_H

// End of File
