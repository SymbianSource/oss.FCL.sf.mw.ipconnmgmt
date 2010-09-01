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
        * Confirmation note is used after the connection has been successfully 
        * established via destination and connection method.
        * @param aDestId Id of used destination.
        * @param aConnMId Id of used connection method.
        * @param aStatus Status object of notifier.
        */
        void ConnectedViaDestAndConnMethodNote( const TUint32 aDestId, 
                                                const TUint32 aConnMId,
                                                TRequestStatus& aStatus );
        /**
        * Cancel ConnectedViaDestAndConnMethodNote notifier.
        */
        void CancelConnectedViaDestAndConnMethodNote();

        /**
        * Information note is used when "Automatically" roaming is enabled
        * The actual connection establishment take places in the background
        * (no wait note).
        * @param aConnMId Id of used connection method.
        * @param aStatus Status object of notifier.
        */
        void ChangingConnectionToNote( const TUint32 aConnMId,
                                       TRequestStatus& aStatus );

        /**
        * Cancel ChangingConnectionToNote notifier.
        */
        void CancelChangingConnectionToNote();

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
        * Confirmation note is used after the connection has been successfully 
        * established via a connection method.
        * @param aConnMId Id of used connection method.
        * @param aStatus Status object of notifier.
        */
        void ConnectedViaConnMethodNote( const TUint32 aConnMId,
                                        TRequestStatus& aStatus );
        /**
        * Cancel ConnectedViaConnMethodNote notifier.
        */
        void CancelConnectedViaConnMethodNote();
        
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

        /**
        * Pops up an information note: 
        * "No WLAN networks are available at the moment. Connection not available."
        */ 
        void NoWLANNetworksAvailableNote( TRequestStatus& aStatus );                  
        
        /**
        * Discreet popup. Shows "connecting via %U" discreet popup.
        * @param aInfo Information about bearer.
        */
        void ConnectingViaDiscreetPopup( TPckgBuf< TConnUiConnectingViaDiscreetPopup>& aInfo, 
                          TRequestStatus& aStatus );
        
        /**
        * Cancel ConnectingViaDiscreetPopup notifier.
        */                  
        void CancelConnectingViaDiscreetPopup();                  

        /**
        * Discreet popup. Shows discreet popup about connection error.
        * @param aErrCode Error code.
        */
        void ConnectionErrorDiscreetPopup( TPckgBuf< TInt >& aErrCode, 
                          TRequestStatus& aStatus );
        
        /**
        * Cancel ConnectionErrorDiscreetPopup notifier.
        */                  
        void CancelConnectionErrorDiscreetPopup();                  

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

        // used for get response from notifier
        TBuf8<8> iResponseStrNoWLANNetworksAvailableNote;
    };


#endif /* __CONNECTIONUIUTILITIESCLIENT_H__ */

// End of File

