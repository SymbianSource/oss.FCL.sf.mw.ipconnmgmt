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
* Description:  Dialog for displaying and editing legacy connection methods
*
*/

#ifndef UNCATDIALOG_H
#define UNCATDIALOG_H

//  INCLUDES
#include "cmdlg.h"

// CLASS DECLARATION
class CEikStatusPane;
class CAknTitlePane;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;

typedef struct
    {
    CCmPluginBase*      iPlugin;
    TUint32             iCmId;
    }TUncatItem;
    
/**
 *  Dialog for displaying and editing legacy iaps
 *
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CUncatDlg ) : public CCmDlg
    {
    public :    // construction
    
        /** Epoc constructor */
        static CUncatDlg* NewL( CCmManagerImpl* aCmManager );

        /** Destructor. */
        ~CUncatDlg();
        
    public: // New functions
    
        /**
        * Create and launch dialog.
        * @return The code returned by the dialogs' ExecuteLD
        */
        TInt ConstructAndRunLD( TUint32 aHighLight, TUint32& aSelected );        

        // From MEikMenuObserver
        void DynInitMenuPaneL( TInt aResourceId, 
                               CEikMenuPane* aMenuPane );

        // From MEikCommandObserver
        void ProcessCommandL( TInt aCommandId ) ; 
        
        /**
        * Updates listbox data
        */
        void ConstructCMArrayL( RArray<TUint32>& aCmIds );
        
        /**
        * Remove hidden Connection Methods from the Array
        */
        void ClearHiddenCMsFromArrayL( RArray<TUint32>& aCmIds );
        
    public: // From MCmCommsDatWatcher
        
        /**
         * Watch changes in CommsDat
         */        
        void CommsDatChangesL();
                
       private:    // Constructor 
        
        /**
        * Default constructor.
        */
        CUncatDlg( CCmManagerImpl* aCmManager );

    private :    // from CEikDialog
    
        /**
        * Set the gateway's IP address, if the user pressed OK.
        * @param aButtonId The ID of the pressed button.
        * @return May the dialog exit?
        */
        void InitTextsL();   
        
        /**
        * Returns the currently focused connection method in the list
        */
        CCmPluginBase* CurrentCML(); 
                
        /**
        * Returns the connection method in the list at the specified index
        */
        CCmPluginBase* CMByIndexL( TInt aIndex );
       
        /**
        * Returns the connection mehtod count
        */
        TInt CMCount();
        
        /**
        * Cleans up iCmUncatItems
        *
        * @param aReuse whether the array will be used again or not
        * @since S60 3.2
        */
        void CleanupUncatArray( TBool aReuseArray = EFalse );

    protected:
    
        RArray<TUncatItem>   iCmUncatItems;
    };

#endif

// End of File
