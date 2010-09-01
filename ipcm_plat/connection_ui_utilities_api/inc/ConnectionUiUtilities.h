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
*     Declares the main handler, CConnectionUiUtilities and public API for the
*     Connection Ui Utilities.
*
*/


#ifndef CONNECTIONUIUTILITIES_H
#define CONNECTIONUIUTILITIES_H

// INCLUDES

#include <e32base.h>
#include <wlanmgmtcommon.h>


// FORWARD DECLARATIONS
class CConnectionUiUtilitiesImpl;
class TConnectionPrefs;


// CONSTANT DECLARATIONS


// TYPE DECLARATIONS

enum TCuuAlwaysAskResults
    {
    ECuuAlwaysAsk = 0,
    ECuuUserDefined        
    };
    

enum TMsgQueryLinkedResults
    {
    EMsgQueryCancelled = 0,     // Message query was cancelled
    EMsgQueryAutomatically,     
    EMsgQueryThisTime
    };
    

// CLASS DECLARATION
/*
* Connection Ui Utilities, it is a set of Miscellaneous networking related 
* ui functions
* Proxy around the real implementation in CConnectionUiUtilitiesUiImpl. 
* No details of the actual data are exposed.
*/
NONSHARABLE_CLASS( CConnectionUiUtilities ) : public CBase
    {

    public: // Constructors and destructor

        /**
        * Two-phased constructor. Leaves on failure.
        * @return The constructed CConnectionUiUtilities object.
        */
        IMPORT_C static CConnectionUiUtilities* NewL();


        /**
        * Destructor.
        */
        IMPORT_C ~CConnectionUiUtilities();


    public:     // New methods

        /**
        * @param aResult contains ECuuAlwaysAsk if always ask radio button 
        * was selected, and ECuuUserDefined if user defined 
        * radio button was selected by user
        * It acts also as input parameter to default this setting.
        * @return ETrue if OK Soft key was pressed, 
        * EFalse if Cancel Soft key was selected on the dialog.
        */
        IMPORT_C TBool AlwaysAskPageL( TCuuAlwaysAskResults& aResult );
                
        /**
        * @param aIAPId specifies the IAP to be excluded from the list
        * @param aIAPId gives back the iapid of selected connection
        * @param aPrefs Specifies the rank and desired direction and bearer
        * of the connections will appear in the dialog
        * @return ETrue if Select Soft key was pressed, 
        * EFalse if Cancel Soft key was selected on the dialog.
        */        
        IMPORT_C TBool ChangeConnectionL( TUint32& aIAPId, 
                                          TConnectionPrefs aPrefs );
        /**
        * Pops up an information note, about connection is changed to 
        * the given accespont
        * @param aIAPId The id of access point which has been selected.
        */
        IMPORT_C void  ShowConnectionChangedNoteL( const TUint32& aIAPId );
        
        /**
        * Pops up discreet popup indicating that connection is being
        * established to the given access point.
        * This method should only be used from the MPM in order to provide
        * a consistent popup behavior across the platform.
        * @param aIAPId The id of the access point.
        */
        IMPORT_C void ConnectingViaDiscreetPopup( const TUint32& aIAPId );

        /**
        * Pops up a discreet popup indicating a connection error.
        * Only appropriate error codes show the popup, otherwise
        * (such as with general errors) the popup is not shown.
        * This method should only be used from the MPM in order to provide
        * a consistent popup behavior across the platform.
        * @param aErrCode Symbian error code.
        */
        IMPORT_C void ConnectionErrorDiscreetPopup( const TInt& aErrorCode );
        
        /**
        * Notifier. Search available WLAN networks, and user can select one of
        * them
        * @param aSSID the id of selected wlan network
        * @return ETrue if user selected one of networks, EFalse if an error
        * is occured or user does not select a network.
        */
        IMPORT_C TBool SearchWLANNetwork( TWlanSsid& aSSID );

        /**
        * Cancel SearchWLANNetwork
        */
        IMPORT_C void  CancelSearchWLANNetwork();

        
        /**
        * Notifier. Shows a query, "Do you want to create a WLAN connection in 
        * Off-line mode?"
        * @return ETrue if user select Yes, EFalse otherwise
        */
        IMPORT_C TBool OffLineWlanNote();
        
        /**
        * Cancel OffLineWlan Note
        */
        IMPORT_C  void CancelOffLineWlanNote();
        
        /**
        * Notifier. Pops up an information note: 
        * "Operation not possible in Off-line mode"
        */        
        IMPORT_C void OffLineWlanDisabledNote();               

        /**
        * @deprecated
        *
        * Use TBool EasyWepDlg( TDes* aKey, TBool& aHex )!
        *
        * Notifier. Shows a data query, "Enter WEP key for WLAN" 
        * @param aKey Key entered by user. Default value can be passed
        * @return ETrue if user select OK, EFalse otherwise
        */
        IMPORT_C TBool EasyWepDlg( TDes* aKey );
        
        /**
        * Cancel Easy Wep dialog
        */
        IMPORT_C  void CancelEasyWepDlg();

        /**
        * Notifier. Shows a data query, "Enter pre-shared key for WLAN" 
        * @param aKey Key entered by user. Default value can be passed
        * @return ETrue if user select OK, EFalse otherwise
        */
        IMPORT_C TBool EasyWpaDlg( TDes* aKey );

        /**
        * Notifier. Shows a data query, "Enter pre-shared key for WLAN" 
        * Async version
        * @param aStatus caller's TRequestStatus to handle asynchronous call.        
        * @param aKey Key entered by user. Default value can be passed
        */
        IMPORT_C void EasyWpaDlg( TRequestStatus& aStatus, TDes* aKey );
                
        /**
        * Cancel Easy Wpa dialog
        */
        IMPORT_C  void CancelEasyWpaDlg();

        /**
        * Notifier. Seatrch available WLAN networks, and user can select one 
        * of them
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aSecurityMode security mode of selected wlan network
        * @return ETrue if user selected one of networks, EFalse if an error
        * is occured or user does not select a network.
        */
        IMPORT_C TBool SearchWLANNetwork
            (
            TWlanSsid& aSSID, 
            TWlanConnectionMode& aConnectionMode,
            TWlanConnectionSecurityMode& aSecurityMode
            );
            
        /**
        * Notifier. Search available WLAN networks, and user can select one 
        * of them
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aExtendedSecurityMode extended security mode of selected wlan network
        * @return ETrue if user selected one of networks, EFalse if an error
        * is occured or user does not select a network.
        */
        IMPORT_C TBool SearchWLANNetwork
            (
            TWlanSsid& aSSID, 
            TWlanConnectionMode& aConnectionMode,
            TWlanConnectionExtentedSecurityMode& aExtendedSecurityMode
            );    

        /**
        * Notifier. Shows a data query, "Enter WEP key for WLAN" 
        * @param aKey Key entered by user. Default value can be passed
        * @param aHex Returns ETrue if user entered hexadecimal key, 
        * EFalse, if user entered ASCII key
        * @return ETrue if user pressed OK, EFalse otherwise
        */
        IMPORT_C TBool EasyWepDlg( TDes* aKey, TBool& aHex );
        
        /**
        * Notifier. Shows a data query, "Enter WEP key for WLAN" 
        * Async version
        * @param aStatus caller's TRequestStatus to handle asynchronous call.
        * @param aKey Key entered by user. Default value can be passed
        * @param aHex Returns ETrue if user entered hexadecimal key, 
        * EFalse, if user entered ASCII key
        */
        IMPORT_C void EasyWepDlg( TRequestStatus& aStatus, TDes* aKey, TBool& aHex );

        /**
        * Notifier. Pops up an information note: 
        * "Selected WLAN network has no coverage"
        */        
        IMPORT_C void WLANNetworkUnavailableNote();               

        /**
        * Notifier. Search available WLAN networks, and user can select one of
        * them. Totally synchronous version, to be used from Engines, not from
        * UIs
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aSecurityMode security mode of selected wlan network
        * @return ETrue if user selected one of networks, EFalse if an error
        * is occured or user does not select a network.
        */
        IMPORT_C TBool SearchWLANNetworkSync
            (
            TWlanSsid& aSSID, 
            TWlanConnectionMode& aConnectionMode,
            TWlanConnectionSecurityMode& aSecurityMode
            );

        /**
        * Notifier. Search available WLAN networks, and user can select one of 
        * them. Totally asynchronous version, to be used from Engines, not 
        * from UIs
        * @param aStatus status object of notifier
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aSecurityMode security mode of selected wlan network
        * @return 
        */
        IMPORT_C void SearchWLANNetworkAsync( TRequestStatus& aStatus, 
                                TWlanSsid& aSSID, 
                                TWlanConnectionMode& aConnectionMode, 
                                TWlanConnectionSecurityMode& aSecurityMode );
                                
        /**
        * Notifier. Search available WLAN networks, and user can select one of 
        * them. Totally asynchronous version, to be used from Engines, not 
        * from UIs
        * @param aStatus status object of notifier
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aExtendedSecurityMode extended security mode of selected wlan network
        * @return 
        */
        IMPORT_C void SearchWLANNetworkAsync( TRequestStatus& aStatus, 
                    TWlanSsid& aSSID, 
                    TWlanConnectionMode& aConnectionMode, 
                    TWlanConnectionExtentedSecurityMode& aExtendedSecurityMode );                        

        /**
        * DEPRECATED, PLEASE USE ASYNC VERSION!
        * Confirmation note is used after the connection has been successfully 
        * established via destination and connection method.
        * @param aDestId Id of used destination.
        * @param aConnMId Id of used connection method.
        */
        IMPORT_C void ConnectedViaDestAndConnMethodNote( const TUint32 aDestId,
                                                    const TUint32 aConnMId );

        /**
        * Confirmation note is used after the connection has been successfully 
        * established via destination and connection method.
        * @param aDestId Id of used destination.
        * @param aConnMId Id of used connection method.
        * @param aStatus caller's TRequestStatus to handle asynchronous call.
        */
        void ConnectedViaDestAndConnMethodNote( const TUint32 aDestId, 
                                                const TUint32 aConnMId, 
                                                TRequestStatus& aStatus );
                                                    
        /**
        * Cancel ConnectedViaDestAndConnMethodNote notifier.
        */
        IMPORT_C void CancelConnectedViaDestAndConnMethodNote();

        /**
        * @deprecated
        *
        * Use void ReconnectToMethodQuery( const TUint32 aDestId, 
        *                                  const TUint32 aConnMId, 
        *                                  const TBool aNextBestExists,
        *                                  TMsgQueryLinkedResults& aResult,
        *                                  TRequestStatus& aStatus );
        *
        * Confirmation query is used when current method is lost and 
        * connection is going to be established via another lower priority 
        * method.
        * @param aConnMId Id of used connection method.
        * @param aResult Result of user selection, ETrue if user accepted 
        * roaming, to less preferred method, EFlase otherwise
        * @param aStatus Status object of notifier.
        */
        IMPORT_C void ReconnectToMethodQuery( const TUint32 aConnMId, 
                                              TBool& aResult,
                                              TRequestStatus& aStatus );
        /**
        * Cancel ReconnectToMethodQuery notifier.
        */
        IMPORT_C void CancelReconnectToMethodQuery();

        /**
        * @deprecated
        *
        * Use void RoamingToMorePrefMethodQuery( const TUint32 aDestId, 
        *                                     const TUint32 aConnMId, 
        *                                     const TBool aNextBestExists,
        *                                     TMsgQueryLinkedResults& aResult,
        *                                     TRequestStatus& aStatus );
        *
        * Confirmation query is used when more preferred method becomes 
        * available
        * @param aConnMId Id of used connection method.
        * @param aResult Result of user selection, ETrue if user accepted 
        * roaming, to more preferred method, EFlase otherwise
        * @param aStatus Status object of notifier.
        */
        IMPORT_C void RoamingToMorePrefMethodQuery( const TUint32 aConnMId, 
                                                    TBool& aResult,
                                                    TRequestStatus& aStatus );
        /**
        * Cancel RoamingToMorePrefMethodQuery notifier.
        */
        IMPORT_C void CancelRoamingToMorePrefMethodQuery();

        /**
        * Information note is used when "Automatically" roaming is enabled. The
        * actual connection establishment take places in the background 
        * (no wait note).
        * @param aConnMId Id of used connection method.
        */
        IMPORT_C void ChangingConnectionToNote( const TUint32 aConnMId );
		
		/**
        * Information note is used when "Automatically" roaming is enabled. The
        * actual connection establishment take places in the background 
        * (no wait note). This is the asynchronous version of the previous one
        * @param aConnMId Id of used connection method.
        * @param aStatus Status object of the notifier
        */
        IMPORT_C void ChangingConnectionToNote( const TUint32 aConnMId, 
        											TRequestStatus& aStatus );
		
        /**
        * Cancel ChangingConnectionToNote notifier.
        */
        IMPORT_C void CancelChangingConnectionToNote();

        /**
        * @deprecated
        *
        * Use void ConfirmMethodUsageQuery( const TUint32 aDestId, 
        *                                   const TUint32 aConnMId, 
        *                                   const TBool aNextBestExists,
        *                                   TMsgQueryLinkedResults& aResult,
        *                                   TRequestStatus& aStatus );
        *
        * Notifier. Shows a query, "Connect to\n '%0U' via\n '%1U'?"
        * @param aDestId Id of used destination.
        * @param aConnMId Id of used connection method.
        * @param aResult Result of user selection, ETrue if user accepted 
        *                query, EFalse otherwise
        * @param aStatus Status object of notifier.
        */
        IMPORT_C void ConfirmMethodUsageQuery( const TUint32 aDestId, 
                                               const TUint32 aConnMId, 
                                               TBool& aResult, 
                                               TRequestStatus& aStatus );
        
        /**
        * Cancel ConfirmMethodUsageQuery
        */
        IMPORT_C  void CancelConfirmMethodUsageQuery();

        /**
        * DEPRECATED, PLEASE USE ASYNC VERSION!
        * Confirmation note is used after the connection has been successfully 
        * established via a connection method.
        * @param aConnMId Id of used connection method.
        */
        IMPORT_C void ConnectedViaConnMethodNote( const TUint32 aConnMId );

        /**
        * Confirmation note is used after the connection has been successfully 
        * established via a connection method.
        * @param aConnMId Id of used connection method.
        * @param aStatus caller's TRequestStatus to handle asynchronous call.
        */
        void ConnectedViaConnMethodNote( const TUint32 aConnMId, 
                                         TRequestStatus& aStatus );
                                         
        /**
        * Cancel ConnectedViaConnMethodNote notifier.
        */
        IMPORT_C void CancelConnectedViaConnMethodNote();

        /**
        * Confirmation query is used when current method is lost and 
        * connection is going to be established via another lower priority 
        * method.
        * @param aDestId Id of used destination.
        * @param aConnMId Id of used connection method.
        * @param aNextBestExists ETrue if there is another available CM inside
        *                        the destination that could be used
        * @param aResult Result of user selection
        * @param aStatus Status object of notifier.
        */
        IMPORT_C void ReconnectToMethodQuery( const TUint32 aDestId, 
                                              const TUint32 aConnMId, 
                                              const TBool aNextBestExists,
                                              TMsgQueryLinkedResults& aResult,
                                              TRequestStatus& aStatus );

        /**
        * Confirmation query is used when more preferred method becomes 
        * available
        * @param aDestId Id of used destination.
        * @param aConnMId Id of used connection method.
        * @param aNextBestExists ETrue if there is another available CM inside
        *                        the destination that could be used
        * @param aResult Result of user selection
        * @param aStatus Status object of notifier.
        */
        IMPORT_C void RoamingToMorePrefMethodQuery( const TUint32 aDestId, 
                                            const TUint32 aConnMId, 
                                            const TBool aNextBestExists,
                                            TMsgQueryLinkedResults& aResult,
                                            TRequestStatus& aStatus );

        /**
        * Notifier. Shows a query, "Connect to\n '%0U' via\n '%1U'?"
        * @param aResult Result of user selection
        * @param aHomeNetwork Home or foreign network.
        * @param aStatus Status object of notifier
        */
        IMPORT_C void ConfirmMethodUsageQuery( TMsgQueryLinkedResults& aResult, 
                                               const TBool aHomeNetwork,
                                               TRequestStatus& aStatus );


        /**
        * Notifier. Shows a query, "Do you want to create a WLAN connection in 
        * Off-line mode?"
        * @param aStatus Status object of notifier.
        */
        IMPORT_C void OffLineWlanNote( TRequestStatus& aStatus );
        

        
        /**
        * Notifier. Shows a data query prompting for WAPI pre-shared key. 
        * @param aKey Key entered by user. Default value can be passed
        * @return ETrue if user select OK, EFalse otherwise
        */
        IMPORT_C TBool EasyWapiDlg( TDes* aKey );

        /**
        * Notifier. Pops up an information note: 
        * "No WLAN networks are available at the moment. Connection not available."
        */        
        IMPORT_C void NoWLANNetworksAvailableNote(); 

        /**
        * Notifier. Search available WLAN networks, and user can select one of 
        * them. Totally asynchronous version, to be used from Engines, not 
        * from UIs
        * @param aStatus status object of notifier
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aSecurityMode security mode of selected wlan network, 
        * @param aProtectedSetupSupported if the wlan supports Protected Setup
        * @return 
        */
        IMPORT_C void SearchWLANNetworkAsync( TRequestStatus& aStatus, 
                                TWlanSsid& aSSID, 
                                TWlanConnectionMode& aConnectionMode, 
                                TWlanConnectionSecurityMode& aSecurityMode, 
                                TBool& aProtectedSetupSupported );
                                
         /**
        * Notifier. Search available WLAN networks, and user can select one of 
        * them. Totally asynchronous version, to be used from Engines, not 
        * from UIs
        * @param aStatus status object of notifier
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aExtendedSecurityMode extended security mode of selected wlan network, 
        * @param aProtectedSetupSupported if the wlan supports Protected Setup
        * @return 
        */
        IMPORT_C void SearchWLANNetworkAsync( TRequestStatus& aStatus, 
                    TWlanSsid& aSSID, 
                    TWlanConnectionMode& aConnectionMode, 
                    TWlanConnectionExtentedSecurityMode& aExtendedSecurityMode, 
                    TBool& aProtectedSetupSupported );                        

        /**
        * Notifier. Search available WLAN networks, and user can select one of
        * them. Totally synchronous version, to be used from Engines, not from
        * UIs
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aSecurityMode security mode of selected wlan network
        * @param aProtectedSetupSupported if the wlan supports Protected Setup
        * @return ETrue if user selected one of networks, EFalse if an error
        * is occured or user does not select a network.
        */
        IMPORT_C TBool SearchWLANNetworkSync( TWlanSsid& aSSID, 
                                    TWlanConnectionMode& aConnectionMode,
                                    TWlanConnectionSecurityMode& aSecurityMode,
                                    TBool& aProtectedSetupSupported );

        /**
        * Notifier. Seatrch available WLAN networks, and user can select one 
        * of them
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aSecurityMode security mode of selected wlan network
        * @param aProtectedSetupSupported if the wlan supports Protected Setup
        * @return ETrue if user selected one of networks, EFalse if an error
        * is occured or user does not select a network.
        */
        IMPORT_C TBool SearchWLANNetwork( TWlanSsid& aSSID, 
                                    TWlanConnectionMode& aConnectionMode,
                                    TWlanConnectionSecurityMode& aSecurityMode,
                                    TBool& aProtectedSetupSupported );
                                                        
        /**
        * Notifier. Search available WLAN networks, and user can select one 
        * of them
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aExtendedSecurityMode extended security mode of selected wlan network
        * @param aProtectedSetupSupported if the wlan supports Protected Setup
        * @return ETrue if user selected one of networks, EFalse if an error
        * is occured or user does not select a network.
        */
        IMPORT_C TBool SearchWLANNetwork( TWlanSsid& aSSID, 
                     TWlanConnectionMode& aConnectionMode,
                     TWlanConnectionExtentedSecurityMode& aExtendedSecurityMode,
                     TBool& aProtectedSetupSupported );                                        


    private:    // Data 

        CConnectionUiUtilitiesImpl* iImpl;  ///< Implementation. Owned.

    };

#endif

// End of File
