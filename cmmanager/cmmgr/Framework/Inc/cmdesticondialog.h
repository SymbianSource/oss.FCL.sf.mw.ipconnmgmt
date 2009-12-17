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
* Description:  Destination icon dialog
*
*/

#ifndef CMDESTICONDIALOG_H
#define CMDESTICONDIALOG_H

#include <AknDialog.h>// <eikdialg.h>

/**
 * Change Icon table for destinations.Contains all the icon available
 */
NONSHARABLE_CLASS(CCmDestinationIconDialog) : public CAknDialog
    {
    public:

        /**
        * Destinations change Icon table constructor. 
        *
        * @param aIconIndex selected icon index. 
        *
        */
        CCmDestinationIconDialog( TInt& aIconIndex );

        /**
        * Destinations change Icon table destructor. 
        * C++ destructor
        */
        ~CCmDestinationIconDialog();

    public: // from CEikDialog

        /**
        * Prepares and runs the dialog and returns the id of the button used to 
        * dismiss it. The dialog is constructed from the resource with id 
        * aResourceId and is destroyed on exit.
        *
        * @return Id of the button used to dismiss dialog.
        */
        TInt ExecuteLD();
        
#ifdef RD_SCALABLE_UI_V2

	    //From MCoeControlObserver
	    void HandleControlEventL( CCoeControl* aControl,
	                              TCoeEvent aEventType );
	    void HandleDialogPageEventL( TInt aEventID );
	    
#endif // RD_SCALABLE_UI_V2 	

    public: // from CCoeControl

        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                      TEventCode /*aType*/ );
        void HandleResourceChange(TInt aType);
        SEikControlInfo CreateCustomControlL(TInt aControlType);

    protected: // from CEikDialog
    
         virtual TBool OkToExitL(TInt);
         virtual void PreLayoutDynInitL();
         virtual void SetSizeAndPosition(const TSize& aSize);

    private: 

         virtual void CEikDialog_Reserved_1();
         virtual void CEikDialog_Reserved_2();

    private: // new function
    
         virtual void CAknIconMapDialog_Reserved();

    private:
    
        TInt* iIconIndex;

#ifdef RD_SCALABLE_UI_V2     

    	TBool iConSelected;

#endif // RD_SCALABLE_UI_V2  
    };

#endif  // LMKICONDIALOG_H
