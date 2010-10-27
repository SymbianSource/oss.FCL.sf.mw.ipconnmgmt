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
*     Declares dialog CApSelectorDialog for access point selection.
*
*/


#ifndef APSELECTORDIALOG_H
#define APSELECTORDIALOG_H

// INCLUDES
#include <aknlists.h>
#include <AknForm.h>

#include <ApSettingsHandlerCommons.h>
#include <ActiveApDb.h>

// FORWARD DECLARATIONS
class CApSelectorListboxModel;
class CApSelectorListbox;
class CTextOverrides;
class CEikStatusPane;
class CAknTitlePane;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CApSettingsModel;
class CApSettingsHandlerImpl;


// CLASS DECLARATION
/**
* CApSelectorDialog dialog class
* 
*/
NONSHARABLE_CLASS( CApSelectorDialog ) : 
        public CAknDialog, public MEikListBoxObserver, 
        public MActiveApDbObserver
    {

    public: // Constructors and destructor
        /**
        * Create and launch dialog.
        * @param aModel Model to be used for reading data. Not owned.
        * Must remain intact while the dialog exists.
        * @param aHandler A CApSettingsHandlerImpl 
        * @param aHighLight The item to highlight
        * @param aSelected  The UID of the selected item. 
        * @return The dialog's return code
        */
        TInt ConstructAndRunLD( CApSettingsModel& aModel, 
                                CApSettingsHandlerImpl& aHandler,
                                TUint32 aHighLight, TUint32& aSelected );


   
        /**
        * Construct the dialog. The passed parameters determine 
        * the filtering rules it will use.
        * @param aListType      The list type it should use to display 
        *                       the list of access points
        * @param aSelMenuType   The Options menu type it should have
        * @param aIspFilter     The ISP filtering criteria
        * @param aBearerFilter  The bearer filtering criteria
        * @param aSortType      The ordering type for the list
        * @param aEventStore    Event information holder
        * @param aNoEdit        Gives wether editing is allowed or not
        * @param aReqIpvType    The IPv type which are included in the list
        * @return The constructed dialog object
        */
        static CApSelectorDialog* NewL( TSelectionListType aListType,
                                        TSelectionMenuType aSelMenuType,
                                        TInt aIspFilter,
                                        TInt aBearerFilter,
                                        TInt aSortType,
                                        TUint32& aEventStore,
                                        TBool aNoEdit,
                                        TInt aReqIpvType = EIPv4 
                                        );


        /** 
        * Destructor.
        */
        ~CApSelectorDialog();

    protected:
        /**
        * Constructor. The passed parameters determine 
        * the filtering rules it will use.
        * @param aListType      The list type it should use to display 
        *                       the list of access points
        * @param aSelMenuType   The Options menu type it should have
        * @param aIspFilter     The ISP filtering criteria
        * @param aBearerFilter  The bearer filtering criteria
        * @param aSortType      The ordering type for the list
        * @param aEventStore    Event information holder
        * @param aReqIpvType    The IPv type which are included in the list
        * @param aNoEdit        Gives wether editing is allowed or not
        */
        CApSelectorDialog(  TSelectionListType aListType,
                            TSelectionMenuType aSelMenuType,
                            TInt aIspFilter,
                            TInt aBearerFilter,
                            TInt aSortType,
                            TUint32& aEventStore,
                            TInt aReqIpvType,
                            TBool aNoEdit
                          );



    public: // New functions

        /**
        * Sets initial texts
        */
        void InitTextsL();


        /**
        * Handles listbox data changes
        */
        void HandleListboxDataChangeL();

        
        /**
        * Sets the text overrides.
        * @param aOverrides An object containing the overriden texts
        */
        void SetTextOverrides( CTextOverrides* aOverrides );

        
        /**
        * Checks if the list is empty
        */
        void CheckIfEmptyL();

        
        /**
        * Sets the highlight 
        */
        void SetHighlighted();

    public: // Functions from base classes
        // From CCoeControl
        virtual void ActivateL();
        
        // From CCoeControl
        virtual void GetHelpContext(TCoeHelpContext& aContext) const;

        // From MActiveApDbObserver
        virtual void HandleApDbEventL( TEvent anEvent );

        // From MEikCommandObserver
        virtual void ProcessCommandL( TInt aCommandId ) ; 

        // From MEikListBoxObserver
        void HandleListBoxEventL( CEikListBox* aListBox, 
                                  TListBoxEvent aEventType );

        SEikControlInfo CreateCustomControlL( TInt aControlType );

        // From MEikMenuObserver
        virtual void DynInitMenuPaneL( TInt aResourceId, 
                                       CEikMenuPane* aMenuPane );

        // From CAknDialog
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType);


    protected:
        // From CEikDialog
        virtual void PreLayoutDynInitL();

        
        // From CEikDialog
        virtual void PostLayoutDynInitL();

        // From CAknDialog
        TBool OkToExitL( TInt aButtonId );

        /**
        * Internal representation of the implementation of 
        * ProcessCommandL from MEikCommandObserver
        */
        void DoProcessCommandL( TInt aCommandId );


    private: //data

        CEikStatusPane*                 iStatusPane;    // NOT owned
        CAknTitlePane*                  iTitlePane;     // NOT owned
        CAknNavigationControlContainer* iNaviPane;      // NOT owned
        CAknNavigationDecorator*        iNaviDecorator; // owned
        HBufC*                          iOldTitleText;
        CApSettingsModel*               iDataModel; // not owned,
                                                    // passed for usage
        CApSelectorListbox*             iList;

        CApSettingsHandlerImpl*         iHandler;
        CApSelectorListboxModel*        iModel;
        TSelectionListType              iListType;
        TSelectionMenuType              iSelMenuType;

        TUint32                         iHighLight;
        TUint32*                        iSelected;        

        TInt                            iIspFilter;
        TInt                            iBearerFilter;
        TInt                            iSortType;

        CTextOverrides*                 iTextOverrides; // not owned,
                                                        // passed for usage

        TUint32                         iPreferredUid;
        TInt                            iPreferredLine;
        TUint32*                        iEventStore;
        TBool                           iNeedUnlock;
        TBool                           iModelPassed;
        TApUiExitReason                 iExitReason;
        
        TInt                            iReqIpvType; /// Default is IPv4
        TInt                            iVariant;    /// Local variant value

#ifdef __TEST_OOMDEBUG
        TBool                           iMemTestOn;
#endif // __TEST_OOMDEBUG
        
        TBool                           iInitialised;
        TBool                           iNoEdit;
        TBool							iHelpSupported;
    };

#endif

// End of File
