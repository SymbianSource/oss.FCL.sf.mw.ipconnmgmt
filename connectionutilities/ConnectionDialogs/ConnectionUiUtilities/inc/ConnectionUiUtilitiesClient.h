/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares Connection Ui Utilities Client classes.
*
*/


#ifndef __CONNECTIONUIUTILITIESCLIENT_H__
#define __CONNECTIONUIUTILITIESCLIENT_H__

// INCLUDES
#include <e32std.h>
#include <ConnectionUiUtilities.h>
#include "ConnectionUiUtilitiesCommon.h"


// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
 * RConnectionUiUtilitiesSession
 */
class RConnectionUiUtilitiesSession : public RSessionBase
    {
    public:
    
        /**
        * Constructor.
        */
        RConnectionUiUtilitiesSession();

        /**
        * Destructor.
        */
        ~RConnectionUiUtilitiesSession();

        /**
        * Connect to the notifier server. Must be called before any other 
        * function (c'tor excepted).
        * @return KErrNone if connection succeeded and a standard error code
        * otherwise.
        */
        TInt Connect();

        /**
        * Disconnect from the notifier server.
        */
        void Close();

    public:
        /**
        * Notifier. Search available WLAN networks, and user can select one of 
        * them
        * @param aNetworkPrefs      Return parameter, contains id, connection 
        *                           mode and security mode of selected wlan 
        *                           network
        * @param aStatus            status object of notifier or user does not
        *                           select a network.
        * @param aIsAsyncVersion    ETrue if called by a really asynchronous
        *                           function
        */        
        void SearchWLANNetwork( 
                            TPckgBuf<TConnUiUiWlanNetworkPrefs>& aNetworkPrefs,
                            TRequestStatus& aStatus,
                            TBool aIsAsyncVersion );

        /**
        * Cancel SearchWLANNetwork
        */     
        void CancelSearchWLANNetwork();
        
        /**
        * Notifier. Shows a query, "Do you want to create a WLAN connection in 
        * Off-line mode?"
        * @param aStatus            status object of notifier
        * @param aIsAsyncVersion    ETrue if called by a really asynchronous
        *                           function
        */         
        void OffLineWlanNote( TRequestStatus& aStatus, TBool aIsAsyncVersion );
        
        /**
        * Cancel OffLineWlan Note
        */
        void CancelOffLineWlanNote();
        
        /**
        * Notifier. Pops up an information note: 
        * "Operation not possible in Off-line mode"
        */                        
        void OffLineWlanDisabledNote( TRequestStatus& aStatus );       

        /**
        * Notifier. Shows a data query, "Enter WEP key for WLAN" 
        * @param aKey Key entered by user
        * @return ETrue if user select OK, EFalse otherwise
        */
        void EasyWepDlg( TPckgBuf< TWepKeyData >& aKey, 
                         TRequestStatus& aStatus );
        
        /**
        * Cancel Easy Wep dialog
        */
        void CancelEasyWepDlg();

        /**
        * Notifier. Shows a data query, "Enter pre-shared key for WLAN" 
        * @param aKey Key entered by user
        * @return ETrue if user select OK, EFalse otherwise
        */
        void EasyWpaDlg( TPckgBuf< TBuf< KEasyWpaQueryMaxLength > >& aKey, 
                         TRequestStatus& aStatus );
        
        /**
        * Cancel Easy Wpa dialog
        */
        void CancelEasyWpaDlg();

        /**
        * Pops up an information note: 
        * "Selected WLAN network has no coverage"
        */        
        void WLANNetworkUnavailableNote( TRequestStatus& aStatus );       

        /**
        * Notifier. Shows a query, "Connect to\n '%0U' via\n '%1U'?"
        * @param aResult Result of user selection, ETrue if user accepted
        * roaming, to more preferred method, EFlase otherwise
        * @param aHomeNetwork   home or foreign network
        * @param aStatus Status object of notifier.
        */
        void ConfirmMethodUsageQuery( TMsgQueryLinkedResults& aResult,                                     
                                      const TBool aHomeNetwork,
                                      TRequestStatus& aStatus);
        
        /**
        * Cancel ConfirmMethodUsageQuery
        */
        void CancelConfirmMethodUsageQuery();

        /**
        * This note is displayed when power-save feature of the WLAN station 
        * is incompatible and thus battery consumption of the mobile will increase.
        * @param aDisable ETrue if user wants to disable this note in the future.
        * @param aStatus Status object of notifier.
        */
        void WlanPowerSaveTestNote( TBool& aDisable, TRequestStatus& aStatus );
        
        /**
        * Cancel WlanPowerSaveTestNote notifier.
        */
        void CancelWlanPowerSaveTestNote();
        
        /**
        * Notifier. Prompts WAPI-PSK.
        * Shows a data query, "Enter pre-shared key for WLAN" 
        * @param aKey Key entered by user
        * @return ETrue if user select OK, EFalse otherwise
        */
        void EasyWapiDlg( TPckgBuf< TBuf< KEasyWapiQueryMaxLength > >& aKey, 
                          TRequestStatus& aStatus );
        
        /**
        * Cancel EasyWapiDlg notifier.
        */                  
        void CancelEasyWapiDlg();

    private:

        // Pointer to the client interface
        RNotifier* iNotifier;

        // used for get response from notifier
        TBuf8<8> iResponseStrOffLineWlanNote;

        // used for get response from notifier
        TBuf8<8> iResponseStrOffLineWlanDisabledNote;

        // used for get response from notifier
        TBuf8<8> iResponseStrWLANNetworkUnavailableNote;

        // used for get response from notifier
        TBuf8<8> iResponseStrConnectingNote;
        
        TPckg<TBool> iBool;         // get response from Query
        TPckg<TMsgQueryLinkedResults> iResponseMsgQuery;

        TPckgBuf<TConnUiUiDestConnMethodNoteId> iPassedInfo;

    };


#endif /* __CONNECTIONUIUTILITIESCLIENT_H__ */

// End of File

