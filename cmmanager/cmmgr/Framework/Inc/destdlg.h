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
* Description:  Dialog for displaying and editing destinations.
*
*/

#ifndef DESTDIALOG_H
#define DESTDIALOG_H

//  INCLUDES
#include <AknForm.h>
#include <cmsettingsui.h>
#include "destlistboxmodel.h"
#include "destlistbox.h"
#include "cmmanagerimpl.h"
#include "cmconnsettingsuiimpl.h"
#include <mcmdexec.h>
#include "cmcommsdatnotifier.h"

// CLASS DECLARATION
class CEikStatusPane;
class CAknTitlePane;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CCmWizard;
class CCmdExec;

/**
*  Dialog for setting the gateway's IP address
*/
NONSHARABLE_CLASS( CDestDlg ) : public CAknDialog
                              , MEikListBoxObserver
                              , public MCmdExec
                              , public MCmCommsDatWatcher
    {
    public :    // construction
    
        /**
        * Default constructor.
        */
        CDestDlg();

        /**
        * Destructor.
        */
        ~CDestDlg();
        
         /**
        * Create and launch dialog.
        * @return           The code returned by the dialogs' ExecuteLD
        */
        TInt ConstructAndRunLD( CCmManagerImpl* aCmManager, TUint32 aHighLight, 
                                TUint32& aSelected, TBool& aExiting );

        // From MEikMenuObserver
        virtual void DynInitMenuPaneL( TInt aResourceId, 
                                       CEikMenuPane* aMenuPane );

        // From MEikCommandObserver
        virtual void ProcessCommandL( TInt aCommandId ) ; 

        /**
        * Updates listbox data
        */
        void HandleListboxDataChangeL();

        /**
        * Handle key event.
        * @param aKeyEvent The key event.
        * @param aType Key event type.
        * @return Response (was the key event consumed?).
        */
        TKeyResponse OfferKeyEventL
            ( const TKeyEvent& aKeyEvent, TEventCode aType );        
        
     
	public:
        /**
         * From base class MCmdExec
         */
        void Execute();

        /**
         * From base class MCmdExec
         */
        void HandleLeaveError( TInt aError );

    public: // From CCoeControl
    
        /**
        * Returns the help context
        * From CCoeControl
        * 
        * @since S60 3.2
        * @param aContext the returned help context
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;
        
        /**
        * Handles the resource change
        * From CCoeControl
        * 
        * @since S60 5.0
        * @param aType Type of the resource change
        */
		void HandleResourceChange( TInt aType );
        
        /**
        * Show default connection note
        * 
        * @since S60 5.0
        * @param aOldConn Old default connection
        */
        void ShowDefaultConnectionNoteL(TCmDefConnValue aOldDefConn);
        
    public: // From MCmCommsDatWatcher
        
        /**
         * Watch changes in CommsDat
         */
        void CommsDatChangesL();
        
    protected :    // from CEikDialog
    
        /**
        * Set the gateway's IP address, if the user pressed OK.
        * @param aButtonId The ID of the pressed button.
        * @return May the dialog exit?
        */
        TBool OkToExitL( TInt aButtonId ) ;
        
        void PreLayoutDynInitL();
        
        SEikControlInfo CreateCustomControlL ( TInt aControlType );

        // From MEikListBoxObserver
        void HandleListBoxEventL( CEikListBox* aListBox, 
                                  TListBoxEvent aEventType );
                                  
        /**
        * Sets the number of connections text on navi pane
        */                                  
        void SetNoOfDestsL(TInt aCount);                                  

    private:
    
        /**
        * Initialises listbox texts
        */    
        void InitTextsL();

        /**
        * Creates a new destination and adds it to Destination view.
        */
        void AddDestinationL();
        
        /**
        * Called when user selected edit on a destination.
        */
        void OnCommandDestinationEditL();
        
        /**
        * Called when user selected delete on a destination.
        */
        void OnCommandDestinationDeleteL();
        
        /**
        * Called when user selected rename on a destination.
        */
        void OnCommandDestinationRenameL();
        
        /**
        * Called when destination is about to be deleted.
        */
        TBool IsThereProtectedMethodL( CCmDestinationImpl* aDest );
        
        /**
        * Sets the text of the middle soft key by the text resource. 
        */
        void SetMskL( TInt aResourceId );
        
        /**
        * Shows the 'Default set to:' note.
        */
        void ShowDefaultSetNoteL(TCmDefConnValue aSelection);
        
        /**
        * Shows the 'Default:' in navi pane.
        */
        void SetDefaultNaviL( TCmDefConnValue aSelection );
        
        /**
        * Counts how many CMs can be displayed.(Shouldn't display
        * CMs for which the plugin is not loaded.)
        */
        TUint32 NumberOfCMsL( RArray<TUint32> cmArray );

        /**
        * Enforced to release allocated memory and reset protection
        * when some exceptions occur
        */
        void EnforcedResetCDestDlg();
     
    protected:
    
        CDestListboxModel*      iModel;
        CDestListbox*           iListbox;
        TUint32                 iHighlight;
        TUint32*                iSelected;           
        TBool                   iModelPassed;
        CEikStatusPane*         iStatusPane;    // NOT owned
        CAknTitlePane*          iTitlePane;     // NOT owned
        CAknNavigationControlContainer* iNaviPane;      // NOT owned
        CAknNavigationDecorator*        iNaviDecorator; // owned
        HBufC*                  iOldTitleText;
        CCmManagerImpl*         iCmManagerImpl; // NOT owned
        TInt                    iExitReason;    // Exit reason, can be 
                                                // KUserExit or KUserBack
                                                // used to pass user exit event   
        CCmConnSettingsUiImpl* iConnSettingsImpl;
        CCmdExec*             iCmdExec;        
        
     private:     

        CCmWizard*          iCmWizard; // wizard for connection method creation
        // Indicated that Escape has arrived and it should be forwarded when CmWizard
        // finished its work
        TBool               iEscapeArrived;
        
        // Indicates whether the Exit button was pressed. Not owned.
        TBool*              iExiting;
        
        // This dialog shouldn't exit while processing in a plugin
        // Indicates whether processing or not
        TBool               iProcessing;
    };

#endif
