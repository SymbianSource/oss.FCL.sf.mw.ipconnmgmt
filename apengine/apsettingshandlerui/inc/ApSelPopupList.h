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
*     Declares CApSelPopupList for access point selection.
*/


#ifndef APSEL_POPUP_LIST_DIALOG_H
#define APSEL_POPUP_LIST_DIALOG_H

// INCLUDES
#include <aknradiobuttonsettingpage.h>
#include <ApSettingsHandlerCommons.h>
#include <ActiveApDb.h>  // for MActiveApDbObserver


#include <AknIconArray.h>


// FORWARD DECLARATIONS
class CApSelectorListboxModel;
class CApSettingsHandlerImpl;


// CLASS DECLARATION
/**
* CApSelPopupList class
* Pop-up access point selector 'control' object.
*/
NONSHARABLE_CLASS( CApSelPopupList ) : public CAknRadioButtonSettingPage, 
                                       public MActiveApDbObserver
    {

    public: // Constructors and destructor

        /**
        * Creates the popup list selector object
        * @param aModel                 Data model to be used for reading data.
        *                               Not owned. Must remain intact while 
        *                               the dialog exists.
        * @param aHandler               A CApSettingsHandlerImpl handling data
        * @param aCurrentSelectionIndex The item to highlight
        * @param aPreferredUid          The preferred Uid to select
        * @param aSelMenuType           The Options menu type it should have
        * @param aIspFilter             The ISP filtering criteria
        * @param aBearerFilter          The bearer filtering criteria
        * @param aSortType              The ordering type for the list
        * @param aEventStore            Event information holder
        * @param aItemArray             The listbox items
        * @param aNeedsNone             Gives whether 'None' item is present
        * @param aNoEdit                Gives wether editing is allowed or not
        * @param aReqIpvType            The IPv types included in the list
        * @param aVpnFilterType         The VPN filtering type
        * @return The created popup list selector object
        */
        static CApSelPopupList* NewL
                                (
                                CApSettingsModel& aModel,
                                CApSettingsHandlerImpl& aHandler,
                                TInt& aCurrentSelectionIndex,
                                TInt& aPreferredUid,
                                TSelectionMenuType aSelMenuType,
                                TInt aIspFilter,
                                TInt aBearerFilter,
                                TInt aSortType,
                                TUint32& aEventStore,
                                CDesCArrayFlat* aItemArray,
                                TBool aNeedsNone,
                                TBool aNoEdit,
                                TInt aReqIpvType = EIPv4,
                                TVpnFilterType aVpnFilterType = EVpnFilterBoth
                                );



        /**
        * Creates the popup list selector object
        * @param aModel                 Data model to be used for reading data.
        *                               Not owned. Must remain intact while 
        *                               the dialog exists.
        * @param aHandler               A CApSettingsHandlerImpl handling data
        * @param aCurrentSelectionIndex The item to highlight
        * @param aPreferredUid          The preferred Uid to select
        * @param aSelMenuType           The Options menu type it should have
        * @param aIspFilter             The ISP filtering criteria
        * @param aBearerFilter          The bearer filtering criteria
        * @param aSortType              The ordering type for the list
        * @param aEventStore            Event information holder
        * @param aItemArray             The listbox items
        * @param aNeedsNone             Gives whether 'None' item is present
        * @param aReqIpvType            The IPv types included in the list
        * @param aVpnFilterType         The VPN filtering type
        * @param aIncludeEasyWlan       To include Easy WLAN AP's or not
        * @param aNoEdit                Gives wether editing is allowed or not
        * @return The created popup list selector object
        */
        static CApSelPopupList* NewL
                                (
                                CApSettingsModel& aModel,
                                CApSettingsHandlerImpl& aHandler,
                                TInt& aCurrentSelectionIndex,
                                TInt& aPreferredUid,
                                TSelectionMenuType aSelMenuType,
                                TInt aIspFilter,
                                TInt aBearerFilter,
                                TInt aSortType,
                                TUint32& aEventStore,
                                CDesCArrayFlat* aItemArray,
                                TBool aNeedsNone,
                                TInt aReqIpvType,
                                TVpnFilterType aVpnFilterType,
                                TBool aIncludeEasyWlan,
                                TBool aNoEdit
                                );

        /**
        * Constructor
        */
        virtual void ConstructL();


    protected:
        /**
        * Creates the popup list selector object
        * @param aModel                 Data model to be used for reading data.
        *                               Not owned. Must remain intact while 
        *                               the dialog exists.
        * @param aHandler               A CApSettingsHandlerImpl handling data
        * @param aIndex                 The item to highlight
        * @param aPreferredUid          The preferred Uid to select
        * @param aSelMenuType           The Options menu type it should have
        * @param aIspFilter             The ISP filtering criteria
        * @param aBearerFilter          The bearer filtering criteria
        * @param aSortType              The ordering type for the list
        * @param aItemArray             The listbox items
        * @param aEventStore            Event information holder
        * @param aNeedsNone             Gives whether 'None' item is present
        * @param aNoEdit                Gives wether editing is allowed or not
        * @param aReqIpvType            The IPv types included in the list
        * @param aVpnFilterType         The VPN filtering type
        * @return The created popup list selector object
        */
        CApSelPopupList
            (
            CApSettingsModel& aModel,
            CApSettingsHandlerImpl& aHandler,
            TInt& aIndex,
            TInt& aPreferredUid,
            TSelectionMenuType aSelMenuType,
            TInt aIspFilter,
            TInt aBearerFilter,
            TInt aSortType,
            CDesCArrayFlat* aItemArray,
            TUint32& aEventStore,
            TBool aNeedsNone,
            TBool aNoEdit,
            TInt aReqIpvType = EIPv4,
            TVpnFilterType aVpnFilterType = EVpnFilterBoth
            );



        /**
        * Creates the popup list selector object
        * @param aModel                 Data model to be used for reading data.
        *                               Not owned. Must remain intact while 
        *                               the dialog exists.
        * @param aHandler               A CApSettingsHandlerImpl handling data
        * @param aIndex                 The item to highlight
        * @param aPreferredUid          The preferred Uid to select
        * @param aSelMenuType           The Options menu type it should have
        * @param aIspFilter             The ISP filtering criteria
        * @param aBearerFilter          The bearer filtering criteria
        * @param aSortType              The ordering type for the list
        * @param aItemArray             The listbox items
        * @param aEventStore            Event information holder
        * @param aNeedsNone             Gives whether 'None' item is present
        * @param aReqIpvType            The IPv types included in the list
        * @param aVpnFilterType         The VPN filtering type
        * @param aIncludeEasyWlan       To include Easy WLAN AP's or not
        * @param aNoEdit                Gives wether editing is allowed or not
        * @return The created popup list selector object
        */
        CApSelPopupList
            (
            CApSettingsModel& aModel,
            CApSettingsHandlerImpl& aHandler,
            TInt& aIndex,
            TInt& aPreferredUid,
            TSelectionMenuType aSelMenuType,
            TInt aIspFilter,
            TInt aBearerFilter,
            TInt aSortType,
            CDesCArrayFlat* aItemArray,
            TUint32& aEventStore,
            TBool aNeedsNone,
            TInt aReqIpvType,
            TVpnFilterType aVpnFilterType,
            TBool aIncludeEasyWlan,
            TBool aNoEdit
            );

        /**
        * Destructor
        */
        virtual ~CApSelPopupList();


    public: // New functions
        /**
        * Sets the highlight.
        */
        void SetHighlighted();

    private:

        /**
        * Loads our own icons
        * @return The icon offset where they were appended.
        */
        TInt LoadIconsL();


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

        // from CCoeControl
        virtual void HandleResourceChange(TInt aType);

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
        * Needed to initialize the list according to the Uid 
        * and not to 'itemindex'...
        */
        void SetSelectedL();


        /*
        * Checks for & handles the 'empty list' situation.
        * According to the spec., prompts for creating a new one...
        */
        void CheckIfEmptyL();

    protected:  // Functions from base classes

        /**
        * From CAknSettingPage
        */
        virtual void DynInitMenuPaneL( TInt aResourceId, 
                                       CEikMenuPane* aMenuPane );

        // From MEikCommandObserver
        virtual void ProcessCommandL( TInt aCommandId );
        
        // From MEikListBoxObserver
        void HandleListBoxEventL( CEikListBox* aListBox, 
                MEikListBoxObserver::TListBoxEvent aEventType );

        // From CCoeControl
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                     TEventCode aType);

    private: //data
        CApSettingsModel*               iDataModel; // not owns it, 
                                                    // just passed for usage!
        CAknSetStyleListBox*            iList;
        CApSelectorListboxModel*        iModel;     // owns it, just passes
                                                    // items for usage to list!
        TInt*                           iSelected;  // does not owns it, 
                                                    // just passed for usage!
        TInt                            iBearerFilter;
        TInt                            iIspFilter;
        TInt                            iSortType;
        TSelectionMenuType              iSelMenuType;
        CApSettingsHandlerImpl*         iHandler;       // does not owns it, 
                                                        // passed for usage
        TUint32*                        iEventStore;    // to hold the events
        TUint32                         iPreferredUid;
        TInt                            iPreferredLine;
        TBool                           iNeedsNone;
        TBool                           iNeedUnlock;

        TInt                            iReqIpvType; // Default is IPv4
        TVpnFilterType                  iVpnFilterType;
        TInt                            iVariant;       /// Local variant value

        CAknIconArray*                  iIcons;

        TBool                           iIncludeEasyWlan;
        TBool                           iInitialised;
        TBool                           iNoEdit;
        TBool							iHelpSupported;
        TBool							iProtectionSupported;
        TBool                           iDoExit;
        TInt                            iOldSelection;
    };

#endif

// End of File
