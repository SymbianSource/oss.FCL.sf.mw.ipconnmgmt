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
* Description:  Shows the "Select destination:" dialog, where the user can 
                 select one of the available destinations.
*
*/

#ifndef SELECTDESTINATIONDLG_H
#define SELECTDESTINATIONDLG_H

// INCLUDES
#include <aknlistquerydialog.h>

// FORWARD DECLARATIONS
class CCmManagerImpl;
class CCmDestinationImpl;

// CLASS DECLARATION

/**
 *  Show a query containing a title, a message box (contain a short explanation
 *  of the query) and a listbox. The listbox contains a list of available
 *  destinations from which the user can choose.
 *
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CSelectDestinationDlg ) : public CAknListQueryDialog
    {
    public: // Constructors, destructor
    
        /**
        * Two-phase constructor.
        * @param aDestinationId Variable reference used to hold the return
        * value of list selection.
        * @param aCmManager Reference to CM Manager implementation used for
        * querying destination specific data.
        * @return Newly created dialog object.
        */
        static CSelectDestinationDlg* NewL( 
                                        TUint32& aDestinationId,
                                        CCmManagerImpl& aCmManager,
                                        RArray<TUint32>* aDestArray = NULL);

        /**
        * Two-phase constructor.
        * @param aDestinationId Variable reference used to hold the return
        * value of list selection.
        * @param aCmManager Reference to CM Manager implementation used for
        * querying destination specific data.
        * @return Newly created dialog object.
        */
        static CSelectDestinationDlg* NewLC( 
                                        TUint32& aDestinationId,
                                        CCmManagerImpl& aCmManager,
                                        RArray<TUint32>* aDestArray = NULL);

        /**
        * Destructor.
        */
        ~CSelectDestinationDlg();

    private: // constructors
    
        /**
        * Constructor.
        */
        CSelectDestinationDlg( TInt aDummyIndex, TUint32& aDestinationId );

        /**
        * Second-phase constructor.
        * @param aCmManager Reference to CM Manager implementation used for
        * querying destination specific data.
        */
        void ConstructL( CCmManagerImpl& aCmManager, 
                         RArray<TUint32>* aDestArray );

    private: // From CAknListQueryDialog
    
        /**
        * @see CAknListQueryDialog
        */
        virtual void PreLayoutDynInitL();

        /**
        * @see CAknListQueryDialog
        */
        TBool OkToExitL( TInt aButtonId );

    private: // internal helper methods
    
        /**
        * Handle resource change events. 
        * @param aType: The type of resources that have changed
        */
        void HandleResourceChange( TInt aType );

    private:  // New functions

        /**
        * Sets graphic icons
        */
        void SetIconsL();

    private: //data

        TUint32& iDestinationId;
        RPointerArray<CCmDestinationImpl> iDestinations;
    };

#endif  // SELECTDESTINATIONDLG_H
