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

#ifndef CMDIALOG_H
#define CMDIALOG_H

//  INCLUDES

#include <AknForm.h>
#include <AknInfoPopupNoteController.h>
#include "cmlistboxmodel.h"
#include "cmlistbox.h"
#include "cmdestinationimpl.h"
#include "cmmanagerimpl.h"
#include <mcmdexec.h>
#include "cmcommsdatnotifier.h"

//#include <EIKLBO.H>

// CLASS DECLARATION
class CEikStatusPane;
class CAknTitlePane;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CCmWizard;
class CCmdExec;
class CDestDlg;

/**
 *  Dialog for displaying and editing connection methods
 *  also base class for CUncatDlg
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCmDlg ) : public CAknDialog
                            , MEikListBoxObserver
                            , MAknInfoPopupNoteObserver
                            , public MCmdExec
                            , public MCmCommsDatWatcher
    {
    public :    // construction
        CCmDlg( CCmManagerImpl* aCmManager, TUint32 aDestUid,
                CDestDlg* aDestDlg );

        ~CCmDlg();
        
        /**
        * Two-phased constructor.
        */
        static CCmDlg* NewL( CCmManagerImpl* aCmManager, TUint32 aDestUid,
                             CDestDlg* aDestDlg );
        
        TInt ConstructAndRunLD( TUint32 aHighLight, TUint32& aSelected );        

        // From MEikMenuObserver
        virtual void DynInitMenuPaneL( TInt aResourceId, 
                                       CEikMenuPane* aMenuPane );
        // From MEikCommandObserver
        virtual void ProcessCommandL( TInt aCommandId ) ;
        
        /**
        * Updates listbox data
        */
        virtual void HandleListboxDataChangeL();   
        
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
        
    public: // From MCmCommsDatWatcher
        
        /**
         * Watch changes in CommsDat
         */
        virtual void CommsDatChangesL();
                 
    protected :    // from CEikDialog
        TBool OkToExitL( TInt aButtonId ) ;
        
        virtual void PreLayoutDynInitL();
        
        SEikControlInfo CreateCustomControlL ( TInt aControlType );

        // From MEikListBoxObserver
        void HandleListBoxEventL( CEikListBox* aListBox, 
                                  TListBoxEvent aEventType );
        
        /**
        * Initialises listbox texts
        */
        virtual void InitTextsL();
        
        /**
        * Sets the number of conn methods text on navi pane
        */
        void SetNoOfCMsL(TInt aCount);
        
    
        //from MAknInfoPopupNoteObserver
        /* Handles events reported from info popup note.
        * @param aController The controller that controls the note, from
        *   where the event originates.
        * @param aEvent The event.
        */
        void HandleInfoPopupNoteEvent(
            CAknInfoPopupNoteController* aController,
            TAknInfoPopupNoteEvent aEvent ); 

        /**
        * Shows the 'bearer type: ' popup note
        */
        void ShowPopupNoteL();          
        
        /**
        * @return the currently focused connection method in the list
        */
        virtual CCmPluginBase* CurrentCML();  
        
        /**
        * @return the connection method in the list at the specified index
        */
        virtual CCmPluginBase* CMByIndexL( TInt aIndex );
        
        /**
        * @return the connection mehtod count
        */
        virtual TInt CMCount();     
        
        /**
        * deletes the currently highlighted conneciton method
        */
        void DeleteCurrentCmL();
        
        void RenameConnectionMethodL();
        void PrioritiseConnectionMethodL();
        void AddConnectionMethodL();
        void CopyConnectionMethodL();
        void MoveConnectionMethodL();
        void EditConnectionMethodL();
        
        /**
        * Indicates if a connection is in use and displays and error message
        *
        * @return ETrue if in use
        */
        TBool CurrentCmInUseL();
        
        virtual void ConstructCMArrayL( RArray<TUint32>& aCmIds );

    public:
        /**
         * From base class MCmdExec
         */
        void HandleLeaveError( TInt aError );
        
    private:
     
        /**
        * Finishes the priorizing method
        *
        * @param aOkPushed ETrue if the OK button pushed.
        *                  EFalse if the Cancel button pushed. 
        *
        */
        void FinishPriorizingL( TBool aOkPushed );    

        /**
         * Rebuild item index array to remove some Iap that has been deleted from
         * CommsDat by other application
         */
        void ReBuildItemIndexArrayL();
        
        /**
         * Reorder connection methods according to underlying connection in case of vpn
         */
        void ReOrderCMsL();
        
        /**
         * Remove hidden Connection Methods from the Array
         */
        void ClearHiddenCMsFromArrayL( RArray<TUint32>& aCmIds );
        
        /**
         * Get a proper index in list for priority
         */
        TInt GetInsertIndexL( TInt aCount, TUint32 aPriority );
        
    protected:
        CCmListboxModel*    iModel;
        TUint32             iHighlight;
        TUint32*            iSelected;           
        TBool               iModelPassed;
        CCmListbox*         iListbox;
        CEikStatusPane*     iStatusPane;    // NOT owned
        CAknTitlePane*      iTitlePane;     // NOT owned
        CAknNavigationControlContainer* iNaviPane;      // NOT owned
        CAknNavigationDecorator*        iNaviDecorator; // owned
        HBufC*              iOldTitleText;  
        CCmManagerImpl*     iCmManager;   // NOT owned   
        TUint32             iDestUid;  // uid of the destination the
                                       // connection mehtods belong to
        TBool               iPrioritising;
        TInt                iExitReason; // Exit reason, can be KUserExit or 
                                         // KUserBack used to pass user exit 
                                         // event                                         
        CAknInfoPopupNoteController*    iInfoPopupNoteController;
        
    private:        
    
        TInt                iCmToPrioritise; // connection mehthod index 
                                            // selected to prioritise
        CCmDestinationImpl* iCmDestinationImpl; // the destination the 

        CCmWizard*          iCmWizard; // wizard for connection method creation
        // Indicated that Escape has arrived and it should be forwarded when CmWizard
        // finished its work
        TBool               iEscapeArrived;
        
        TBool 				iProcessing;
        TBool               iAlreadyConfirmedDelete;
                                                // connection mehtods belongs to
        TBool               iExitduringProcessing;
        TBool               iBackduringProcessing;
        CDestDlg*           iDestDlg;
        
    protected:
        // This is nescessary becaue in case of not supported CMs the data base 
        // and the lisbox content is different.(The listbox doesn' show the
        // unsupported ones.)
        RArray<TInt>        iItemIndex;
        CCmdExec*           iCmdExec;
    };

#endif
