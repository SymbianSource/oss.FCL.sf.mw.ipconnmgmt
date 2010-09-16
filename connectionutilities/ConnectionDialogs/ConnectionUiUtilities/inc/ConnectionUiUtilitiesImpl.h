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
* Description: 
*      Declaration of class CConnectionUiUtilitiesImpl.
*      
*
*/


#ifndef CONNECTIONUIUTILITIESIMPL_H
#define CONNECTIONUIUTILITIESIMPL_H

// INCLUDES
#include <e32base.h>
#include "ConnectionUiUtilitiesClient.h"
#include "ConnectionUiUtilitiesCommon.h"
#include "ActiveWrapper.h"

// CONST DECLARATIONS

_LIT( KMrouterName, "mRouter" );

#if defined(_DEBUG)
_LIT( KErrNullPointer, "NULL pointer" );
#endif


// FORWARD DECLARATIONS
class CCommsDatabase;
class TConnectionPrefs;
class CAknGlobalNote;
class CConnectionStatusPopup;
class CCellularDataConfirmation;

// CLASS DECLARATION

/**
* Connection Ui Utilities.
* Implementation behind proxy class CConnectionUiUtilities.
*/
NONSHARABLE_CLASS( CConnectionUiUtilitiesImpl ) : public CBase
    {

    public:     // Constructors and destructor

        /**
        * Two-phased constructor. Leaves on failure.
        * @return The constructed CConnectionUiUtilities object.
        */
        static CConnectionUiUtilitiesImpl* NewL();

        /**
        * Destructor.
        */
        virtual ~CConnectionUiUtilitiesImpl();

    protected:  // Constructors

        /**
        * Constructor.
        */
        CConnectionUiUtilitiesImpl();

        /**
        * Second-phase constructor.
        */
        void ConstructL();

    public:     // New methods
        /**
        * @param aResult contains ECuuAlwaysAsk if always ask radio button 
        * was selected, and ECuuUserDefined if user defined 
        * radio button was selected by user
        * It acts also as input parameter to default this setting.
        * @return ETrue if OK Soft key was pressed, 
        * EFalse if Cancel Soft key was selected on the dialog.
        */
        TBool AlwaysAskPageL( TCuuAlwaysAskResults& aResult );
        
        /**
        * @param aIAPId gives back the iapid of selected connection
        * @param aPrefs Specifies the rank and desired direction and bearer
        * of the connections will appear in the dialog
        * @return ETrue if Select Soft key was pressed, 
        * EFalse if Cancel Soft key was selected on the dialog.
        */                
        TBool ChangeConnectionL( TUint32& aUid, TConnectionPrefs aPrefs );
        
        /**
        * Pops up an information note, about connection is changed to 
        * the given accespont
        * @param aIAPId The id of access point which has been selected.
        */        
        void  ShowConnectionChangedNoteL( const TUint32& aIAPId );
        
        /**
        * Notifier. Search available WLAN networks, and user can select one of 
        * them
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aSecurityMode security mode of selected wlan network        
        * @return ETrue if user selected one of networks, EFalse if an error
        * is occured or user does not select a network.
        */        
        TBool SearchWLANNetwork( TWlanSsid& aSSID, 
                                 TWlanConnectionMode& aConnectionMode,
                                 TWlanConnectionSecurityMode& aSecurityMode );
                                 
        /**
        * Notifier. Search available WLAN networks, and user can select one of 
        * them
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aExtSecurityMode extended security mode of selected wlan network        
        * @return ETrue if user selected one of networks, EFalse if an error
        * is occured or user does not select a network.
        */        
        TBool SearchWLANNetwork( TWlanSsid& aSSID, 
                                 TWlanConnectionMode& aConnectionMode,
                                 TWlanConnectionExtentedSecurityMode& aExtSecurityMode );                         
        
        /**
        * Notifier. Search available WLAN networks, and user can select one of 
        * them
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aSecurityMode security mode of selected wlan network        
        * @param aProtectedSetupSupported if the wlan supports Protected Setup
        * @return ETrue if user selected one of networks, EFalse if an error
        * is occured or user does not select a network.
        */        
        TBool SearchWLANNetwork( TWlanSsid& aSSID, 
                                 TWlanConnectionMode& aConnectionMode,
                                 TWlanConnectionSecurityMode& aSecurityMode,
                                 TBool& aProtectedSetupSupported );
                                 
         /**
        * Notifier. Search available WLAN networks, and user can select one of 
        * them
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aExtSecurityMode security mode of selected wlan network        
        * @param aProtectedSetupSupported if the wlan supports Protected Setup
        * @return ETrue if user selected one of networks, EFalse if an error
        * is occured or user does not select a network.
        */        
        TBool SearchWLANNetwork( TWlanSsid& aSSID, 
                                 TWlanConnectionMode& aConnectionMode,
                                 TWlanConnectionExtentedSecurityMode& aExtSecurityMode,
                                 TBool& aProtectedSetupSupported );                         
        
        
        /**
        * Notifier. Search available WLAN networks, and user can select one of 
        * them
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aSecurityMode security mode of selected wlan network
        * @param aExtSecurityMode security mode of selected wlan network         
        * @param aProtectedSetupSupported if the wlan supports Protected Setup
        * @return ETrue if user selected one of networks, EFalse if an error
        * is occured or user does not select a network.
        */        
        TBool SearchWLANNetwork( TWlanSsid& aSSID, 
                                 TWlanConnectionMode& aConnectionMode,
                                 TWlanConnectionSecurityMode& aSecurityMode,
                                 TWlanConnectionExtentedSecurityMode& aExtSecurityMode, 
                                 TBool& aProtectedSetupSupported );
        
        /**
        * Notifier. Search available WLAN networks, and user can select one of
        * them.
        * Totally synchronous version, to be used from Engines, not from UIs
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aSecurityMode security mode of selected wlan network        
        * @return ETrue if user selected one of networks, EFalse if an error
        * is occured or user does not select a network.
        */        
        TBool SearchWLANNetworkSync( TWlanSsid& aSSID, 
                                 TWlanConnectionMode& aConnectionMode,
                                 TWlanConnectionSecurityMode& aSecurityMode );

        
        /**
        * Notifier. Search available WLAN networks, and user can select one of
        * them.
        * Totally synchronous version, to be used from Engines, not from UIs
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aSecurityMode security mode of selected wlan network        
        * @param aProtectedSetupSupported if the wlan supports Protected Setup
        * @return ETrue if user selected one of networks, EFalse if an error
        * is occured or user does not select a network.
        */        
        TBool SearchWLANNetworkSync( TWlanSsid& aSSID, 
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
        * @param aSecurityMode security mode of selected wlan network
        * @return 
        */
        void SearchWLANNetworkAsync( TRequestStatus& aStatus, 
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
        * @param aExtSecurityMode extented security mode of selected wlan network
        * @return 
        */
        void SearchWLANNetworkAsync( TRequestStatus& aStatus, 
                                TWlanSsid& aSSID, 
                                TWlanConnectionMode& aConnectionMode, 
                                TWlanConnectionExtentedSecurityMode& aExtSecurityMode );                        

        /**
        * Notifier. Search available WLAN networks, and user can select one of 
        * them. Totally asynchronous version, to be used from Engines, not 
        * from UIs
        * @param aStatus status object of notifier
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aSecurityMode security mode of selected wlan network
        * @param aProtectedSetupSupported if the wlan supports Protected Setup
        * @return 
        */
        void SearchWLANNetworkAsync( TRequestStatus& aStatus, 
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
        * @param aExtSecurityMode extended security mode of selected wlan network
        * @param aProtectedSetupSupported if the wlan supports Protected Setup
        * @return 
        */
        void SearchWLANNetworkAsync( TRequestStatus& aStatus, 
                                TWlanSsid& aSSID, 
                                TWlanConnectionMode& aConnectionMode, 
                                TWlanConnectionExtentedSecurityMode& aExtSecurityMode, 
                                TBool& aProtectedSetupSupported );                        

        /**
        * Notifier. Search available WLAN networks, and user can select one of 
        * them. Totally asynchronous version, to be used from Engines, not 
        * from UIs
        * @param aStatus status object of notifier
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aSecurityMode security mode of selected wlan network
        * @param aExtSecurityMode extended security mode of selected wlan network
        * @param aProtectedSetupSupported if the wlan supports Protected Setup
        * @return 
        */
        void SearchWLANNetworkAsync( TRequestStatus& aStatus, 
                                TWlanSsid& aSSID, 
                                TWlanConnectionMode& aConnectionMode, 
                                TWlanConnectionSecurityMode& aSecurityMode,
                                TWlanConnectionExtentedSecurityMode& aExtSecurityMode, 
                                TBool& aProtectedSetupSupported );


        /**
        * Cancel SearchWLANNetwork
        */        
        void  CancelSearchWLANNetwork();
        
        /**
        * Notifier. Shows a query, "Do you want to create a WLAN connection in 
        * Off-line mode?"
        * @return ETrue if user select Yes, EFalse otherwise
        */        
        TBool OffLineWlanNote();
        
        /**
        * Notifier. Shows a query, "Do you want to create a WLAN connection in 
        * Off-line mode?"
        * Asynchronous.
        * @param aStatus Status object of notifier.
        */        
        void OffLineWlanNote( TRequestStatus& aStatus );

        /**
        * Cancel OffLineWlan Note
        */        
        void CancelOffLineWlanNote();
        
        /**
        * Notifier. Pops up an information note: 
        * "Operation not possible in Off-line mode"
        */                        
        void OffLineWlanDisabledNote();

        /**
        * Notifier. Shows a data query, "Enter WEP key for WLAN" 
        * @param aKey Key entered by user
        * @return ETrue if user select OK, EFalse otherwise
        */
        TBool EasyWepDlg( TDes* aKey, TBool& aHex );

        /**
        * Notifier. Shows a data query, "Enter WEP key for WLAN" 
        * Async version
        * @param aKey Key entered by user
        * @param aStatus caller's TRequestStatus to handle asynchronous call.
        */
        void EasyWepDlgAsync( TRequestStatus& aStatus, TDes* aKey, TBool& aHex  );
        
        /**
        * Cancel Easy Wep dialog
        */
        void CancelEasyWepDlg();

        /**
        * Notifier. Shows a data query, "Enter pre-shared key for WLAN" 
        * @param aKey Key entered by user
        * @return ETrue if user select OK, EFalse otherwise
        */
        TBool EasyWpaDlg( TDes* aKey );

        /**
        * Notifier. Shows a data query, "Enter pre-shared key for WLAN" 
        * @param aKey Key entered by user
        * @param aStatus caller's TRequestStatus to handle asynchronous call.
        */
        void EasyWpaDlgAsync(  TRequestStatus& aStatus, TDes* aKey );
        
        /**
        * Cancel Easy Wpa dialog
        */
        void CancelEasyWpaDlg();
        
        /**
        * Pops up an information note: 
        * "Selected WLAN network has no coverage"
        */        
        void WLANNetworkUnavailableNote();           

        /**
        * Notifier. Shows a query, "Connect to\n '%0U' via\n '%1U'?"
        * @param aResult Result of user selection, ETrue if user accepted 
        * roaming, to more preferred method, EFlase otherwise
        * @param aHomeNetwork   home or foreign network
        * @param aStatus Status object of notifier.
        */
        void ConfirmMethodUsageQuery( TMsgQueryLinkedResults& aResult,  
                                      const TBool aHomeNetwork,
                                      TRequestStatus& aStatus );
        
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
        * Notifier. Query for prompting WAPI-PSK. 
        * Displays a query: "Enter pre-shared key for WLAN" 
        * @param aKey Key entered by user
        * @return ETrue if user select OK, EFalse otherwise
        */
        TBool EasyWapiDlg( TDes* aKey );

        /**           
        * Discreet popup. Shows "Connecting" discreet popup 
        */
        void ConnectingViaDiscreetPopup( );

        /**           
        * Discreet popup. Shows "Connecting via %U" discreet popup 
        * @param aIapId IAP id of the access point.
        * @param aConnectionAlreadyActive True if connection is already active
        *   (shared).
        */
        void ConnectingViaDiscreetPopup( const TUint32& aIapId, 
                                         TBool aConnectionAlreadyActive );

        /**           
        * Cancels (hides) "Connecting via %U" discreet popup. 
        */
        void CancelConnectingViaDiscreetPopup();

        /**           
        * Discreet popup. Shows discreet popup about connection errors 
        * @param aErrorCode Error code
        */
        void ConnectionErrorDiscreetPopup( const TInt& aErrorCode );
        
    private:     // New methods

        /**
        * Gives back the name of connection uses aIAPId
        * @param aIAPId access point used for connection
        * @param aDb CCommsDatabase ofr seaching name of connection
        * @param aConnectionName asked connection name
        */        
        void GetConnectionNameL( const TUint32& aIAPId, 
                                 CCommsDatabase& aDb,
                                 TDes& aConnectionName );

        /**
        * Loads resource file of dll. It can be used if there is CCoeEnv.
        */                                         
        void LoadResourceFileL();                             

    private:    // Data 

        TInt iResOffset;        ///< offset of loaded resource file
        
        RConnectionUiUtilitiesSession iNotif; ///< notifier client
        
        TPckgBuf< TWepKeyData > iWepKey;
        
        TPckgBuf< TBuf< KEasyWpaQueryMaxLength > > iWpaKey;

        //return information from Notifier
        TPckgBuf<TConnUiUiWlanNetworkPrefs> iNetworkPrefs; 
         
        TBool iIsWlanSupported;      

        TBool iDummy;
        
        TWlanConnectionSecurityMode iDummySecMode;
        
        TWlanConnectionExtentedSecurityMode iDummyExtSecMode;

        // Use one wrapper for each note, to avoid to be overwritten, when
        // some instance is used to show more dialogs at the same time!
        CActiveWrapper* iActiveWrapper[KNumberOfWrappedDialogs];
        
        // buffer for passing WAPI key between client and server
        TPckgBuf< TBuf< KEasyWapiQueryMaxLength > > iWapiKey;
        
        // Connection status (via, error) implementation
        CConnectionStatusPopup* iConnStatusPopup;
        
        // Cellular confirmation prompt
        CCellularDataConfirmation* iCellularConfirmation;
        
    };


#endif  // CONNECTIONUIUTILITIESIMPL_H

// End of File

