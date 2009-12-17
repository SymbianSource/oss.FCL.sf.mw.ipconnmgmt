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
* Description:  Wizard used for adding connection methods
*
*/

#ifndef T_TCMWIZARD_H
#define T_TCMWIZARD_H

// INCLUDE FILES
#include <e32def.h>
#include <AknProgressDialog.h>

// FORWARD DECLARATIONS
class CAknWaitDialog;
class CCmDestinationImpl;
class CCmManagerImpl;
class CCmPluginBase;

/**
 *  Connection Method wizard
 *
 *  Wizard used for creating a new Connection Method.
 *
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCmWizard ) : public CBase,
                                 public MProgressDialogCallback
    {
    public: // constructor

        /**
         * Constructor.
         *
         * @param aCmManager Connection Method Manager implementation passed
         * for queries.
         */
        CCmWizard( CCmManagerImpl& aCmManager );

        /**
        * Destructor.
        */
        ~CCmWizard();

    public: // public methods
    
        /**
        * Key method for creating a new Connection Method. This function
        * guides the user through several steps and then finally creates
        * a new Connection Method based on the information the user has given.
         * @param aTargetDestination Target destination to which the new
         * connection method will be added. Can be NULL if the
         * destination is to be selected by the user later on (via a dialog).
        */
        TInt CreateConnectionMethodL
                            ( CCmDestinationImpl* aTargetDestination = NULL );

    public: // from MProgressDialogCallback
    
        /**
        * @see MProgressDialogCallback mixin for more details.
        */
        void DialogDismissedL( TInt aButtonId );

    private: // internal helper methods
    
        /**
        * Shows a dialog asking the user if he/she wants to have the available
        * connection methods automatically checked for.
        * @return ETrue if the user wants the conn methods be automatically
        * discovered, EFalse if he/she wants to discover them manually.
        */
        TBool AutoCheckForCmsL() const;

        /**
        * If automatic CM detection has failed, the user is prompted a dialog
        * asking if he/she wants to proceed with manual configuration.
        * @return ETrue if the user wants to proceed with manual configuration,
        * EFalse otherwise.
        */
        TBool ManualCheckForCmsL() const;

        /**
        * Checks for availability of packet data and WLAN bearer types. Shows
        * a wait dialog until discovery is not complete.
        * @param aCoverageArray This array is supposed to hold the UIDs of
        * bearers that have network coverage for the moment.
        */
        void CheckForCmsL( RArray<TUint32>& aCoverageArray );
        
        
        /**
         * Finds the target destination to which the currently creted VPN
         * connection method points. It can be a destination or the destination
         * which includes the connection method to which the VPN points.
         * @param aPlugin VPN plugin.
         * @return the Id of the destination 
         */
        TUint32 FindTargetDestinationL( CCmPluginBase* aPlugin );

        /**
         * Finds the parent destination of a connection method.
         * @param Id of the connection method
         * @return Id of the destination which contains cmId 
         */
        TUint32 FindParentDestinationL( TUint32 aCmId );

    private: // data
    
        CAknWaitDialog* iCheckConnDlg;  ///< Owned.
        TBool iCheckConnDlgCancelled;
        
        CCmManagerImpl& iCmManager;     ///< Only a reference == not owned.
    };

#endif // T_TCMWIZARD_H
