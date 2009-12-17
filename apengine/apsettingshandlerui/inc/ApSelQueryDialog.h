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
*     Declares dialog CApSelQueryDialog for access point selection.
*
*/


#ifndef APSEL_QUERY_DIALOG_H
#define APSEL_QUERY_DIALOG_H

// INCLUDES
#include <aknlistquerydialog.h>
#include <ApSettingsHandlerCommons.h>
#include <ActiveApDb.h>

// FORWARD DECLARATIONS
class CApSelectorListboxModel;

// CLASS DECLARATION
/**
* CApSelQueryDialog class
* 
*/
NONSHARABLE_CLASS( CApSelQueryDialog ) : public CAknListQueryDialog, 
                                         public MActiveApDbObserver
    {

    public: // Constructors and destructor

        /**
        * Constructor
        * @param aModel a reference to the data model
        * @param aIndex a pointer to hold the UID of the selected item
        * @return The constructed access point selection query dialog
        */
        CApSelQueryDialog( CApSettingsModel& aModel, 
                           TUint32* aIndex );

        /**
        * Destructor
        */
        virtual ~CApSelQueryDialog();


    public: // Functions from base classes
        /**
        * from MActiveApDbObserver
        */
        virtual void HandleApDbEventL( TEvent anEvent );


    private:
        /**
        * From CAknListQueryDialog
        */
        virtual void PreLayoutDynInitL( void );


        /**
        * From CAknListQueryDialog
        */
        virtual void PostLayoutDynInitL();


        /**
        * From CAknListQueryDialog 
        */
        TBool OkToExitL( TInt aButtonId );


        // From CCoeControl
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                     TEventCode aType); 


        // From MEikCommandObserver
        void ProcessCommandL( TInt aCommandId );


    protected:  // New functions
        /**
        * Fills up listbox control with data 
        */
        void FillListBoxWithDataL();


        /**
        * Loads protected icons
        * @return The icon offset where they were appended.
        */
        TInt LoadIconsL();


    protected:  // Functions from base classes


    private: //data
        CApSettingsModel*               iDataModel; // does not own it, 
                                                    // just passed for usage!
        CEikListBox*                    iList;
        CApSelectorListboxModel*        iModel;
        TUint32*                        iSelected;
        TBool                           iNeedUnlock;

    };

#endif

// End of File
