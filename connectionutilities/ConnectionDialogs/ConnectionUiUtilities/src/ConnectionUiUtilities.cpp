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
*      Implementation of class CConnectionUiUtilities.
*      
*
*/


// INCLUDE FILES

#include <agentdialog.h>
#include <ConnectionUiUtilities.h>

#include "ConnectionUiUtilitiesImpl.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CConnectionUiUtilities::NewL
// ---------------------------------------------------------
//
EXPORT_C CConnectionUiUtilities* CConnectionUiUtilities::NewL()
    {
    CConnectionUiUtilities* utils = new ( ELeave ) CConnectionUiUtilities();
    CleanupStack::PushL( utils );
    utils->iImpl = CConnectionUiUtilitiesImpl::NewL();
    CleanupStack::Pop( utils ); 
    return utils;
    }


// ---------------------------------------------------------
// CConnectionUiUtilities::~CConnectionUiUtilities
// ---------------------------------------------------------
//
EXPORT_C CConnectionUiUtilities::~CConnectionUiUtilities()
    {
    delete iImpl;
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::AlwaysAskPageL
// ---------------------------------------------------------
//
EXPORT_C TBool CConnectionUiUtilities::AlwaysAskPageL( 
                                              TCuuAlwaysAskResults& aResult )
    {
    return iImpl->AlwaysAskPageL( aResult );
    }


// ---------------------------------------------------------
// CConnectionUiUtilities::ChangeConnectionL
// ---------------------------------------------------------
//
EXPORT_C TBool CConnectionUiUtilities::ChangeConnectionL( 
                                    TUint32& aIAPId, TConnectionPrefs aPrefs )
    {
    return iImpl->ChangeConnectionL( aIAPId, aPrefs );
    }


// ---------------------------------------------------------
// CConnectionUiUtilities::ShowConnectionChangedNoteL
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::ShowConnectionChangedNoteL( 
                                                        const TUint32& aIAPId )
    {
    iImpl->ShowConnectionChangedNoteL( aIAPId );
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::SearchWLANNetwork
// ---------------------------------------------------------
//
EXPORT_C TBool CConnectionUiUtilities::SearchWLANNetwork( TWlanSsid& aSSID )
    {
    TWlanConnectionMode connectionMode;
    TWlanConnectionSecurityMode securityMode;
    return iImpl->SearchWLANNetwork( aSSID, connectionMode, securityMode );
    }


// ---------------------------------------------------------
// CConnectionUiUtilities::CancelSearchWLANNetwork
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::CancelSearchWLANNetwork()
    {
    iImpl->CancelSearchWLANNetwork();
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::OffLineWlanNote
// ---------------------------------------------------------
//
EXPORT_C TBool CConnectionUiUtilities::OffLineWlanNote()
    {
    return iImpl->OffLineWlanNote();
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::CancelOffLineWlanNote
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::CancelOffLineWlanNote()
    {
    iImpl->CancelOffLineWlanNote();
    }
    
// ---------------------------------------------------------
// CConnectionUiUtilities::OffLineWlanDisabledNote
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::OffLineWlanDisabledNote()
    {
    iImpl->OffLineWlanDisabledNote();
    }
 
// ---------------------------------------------------------
// CConnectionUiUtilities::EasyWepDlg
// ---------------------------------------------------------
//
EXPORT_C TBool CConnectionUiUtilities::EasyWepDlg( TDes* aKey )
    {
    TBool hex;
    return iImpl->EasyWepDlg( aKey, hex );
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::CancelEasyWepDlg
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::CancelEasyWepDlg()
    {
    iImpl->CancelEasyWepDlg();
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::EasyWpaDlg
// ---------------------------------------------------------
//
EXPORT_C TBool CConnectionUiUtilities::EasyWpaDlg( TDes* aKey )
    {
    return iImpl->EasyWpaDlg( aKey );
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::EasyWpaDlg
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::EasyWpaDlg( TRequestStatus& aStatus, TDes* aKey )
    {
    iImpl->EasyWpaDlgAsync( aStatus, aKey );
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::CancelEasyWpaDlg
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::CancelEasyWpaDlg()
    {
    iImpl->CancelEasyWpaDlg();
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::SearchWLANNetwork
// ---------------------------------------------------------
//
EXPORT_C TBool CConnectionUiUtilities::SearchWLANNetwork( TWlanSsid& aSSID,
                                TWlanConnectionMode& aConnectionMode,
                                TWlanConnectionSecurityMode& aSecurityMode )
    {
    return iImpl->SearchWLANNetwork( aSSID, aConnectionMode, aSecurityMode );
    }
    
// ---------------------------------------------------------
// CConnectionUiUtilities::SearchWLANNetwork
// ---------------------------------------------------------
//
EXPORT_C TBool CConnectionUiUtilities::SearchWLANNetwork( TWlanSsid& aSSID,
                TWlanConnectionMode& aConnectionMode,
                TWlanConnectionExtentedSecurityMode& aExtendedSecurityMode )
    {
    return iImpl->SearchWLANNetwork( aSSID, 
                                     aConnectionMode, 
                                     aExtendedSecurityMode );
    }    

// ---------------------------------------------------------
// CConnectionUiUtilities::EasyWepDlg
// ---------------------------------------------------------
//
EXPORT_C TBool CConnectionUiUtilities::EasyWepDlg( TDes* aKey, TBool& aHex )
    {
    return iImpl->EasyWepDlg( aKey, aHex );
    }
    
// ---------------------------------------------------------
// CConnectionUiUtilities::EasyWepDlg
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::EasyWepDlg( TRequestStatus& aStatus, TDes* aKey, TBool& aHex )
    {
    iImpl->EasyWepDlgAsync( aStatus, aKey, aHex );
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::WLANNetworkUnavailableNote
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::WLANNetworkUnavailableNote()
    {
    iImpl->WLANNetworkUnavailableNote();
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::SearchWLANNetworkSync
// ---------------------------------------------------------
//
EXPORT_C TBool CConnectionUiUtilities::SearchWLANNetworkSync
            (
            TWlanSsid& aSSID, 
            TWlanConnectionMode& aConnectionMode,
            TWlanConnectionSecurityMode& aSecurityMode
            )
    {
    return iImpl->SearchWLANNetworkSync( aSSID, aConnectionMode, 
                                         aSecurityMode );
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::SearchWLANNetworkAsync
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::SearchWLANNetworkAsync( 
                                TRequestStatus& aStatus,
                                TWlanSsid& aSSID, 
                                TWlanConnectionMode& aConnectionMode, 
                                TWlanConnectionSecurityMode& aSecurityMode )
    {
    iImpl->SearchWLANNetworkAsync( aStatus, 
                                   aSSID, aConnectionMode, aSecurityMode );
    }
    
// ---------------------------------------------------------
// CConnectionUiUtilities::SearchWLANNetworkAsync
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::SearchWLANNetworkAsync( 
                TRequestStatus& aStatus,
                TWlanSsid& aSSID, 
                TWlanConnectionMode& aConnectionMode, 
                TWlanConnectionExtentedSecurityMode& aExtendedSecurityMode )
    {
    iImpl->SearchWLANNetworkAsync( aStatus, 
                                   aSSID, 
                                   aConnectionMode, 
                                   aExtendedSecurityMode );
    }        

// ---------------------------------------------------------
// CConnectionUiUtilities::ConnectedViaDestAndConnMethodNote
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::ConnectedViaDestAndConnMethodNote( 
                                                const TUint32 aDestId, 
                                                const TUint32 aConnMId )
    {
    iImpl->ConnectedViaDestAndConnMethodNote( aDestId, aConnMId );    
    }
 
// ---------------------------------------------------------
// CConnectionUiUtilities::ConnectedViaDestAndConnMethodNote
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::ConnectedViaDestAndConnMethodNote( 
                                                const TUint32 aDestId, 
                                                const TUint32 aConnMId, 
                                                TRequestStatus& aStatus  )
    {
    iImpl->ConnectedViaDestAndConnMethodNote( aDestId, aConnMId, aStatus ); 
    }
           
// ---------------------------------------------------------
// CConnectionUiUtilities::CancelConnectedViaDestAndConnMethodNote
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::CancelConnectedViaDestAndConnMethodNote()
    {
    iImpl->CancelConnectedViaDestAndConnMethodNote();            
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::ReconnectToMethodQuery
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::ReconnectToMethodQuery( 
                                                const TUint32 /*aConnMId*/, 
                                                TBool& aResult,
                                                TRequestStatus& aStatus )
    {
    aResult = EFalse;
    TRequestStatus* pS = &aStatus;
    User::RequestComplete( pS, KErrNotSupported );        
    }
    
// ---------------------------------------------------------
// CConnectionUiUtilities::CancelReconnectToMethodQuery
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::CancelReconnectToMethodQuery()
    {    
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::RoamingToMorePrefMethodQuery
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::RoamingToMorePrefMethodQuery( 
                                            const TUint32 /*aConnMId*/, 
                                            TBool& aResult,
                                            TRequestStatus& aStatus )
    {
    aResult = EFalse;
    TRequestStatus* pS = &aStatus;
    User::RequestComplete( pS, KErrNotSupported );
    }
    
// ---------------------------------------------------------
// CConnectionUiUtilities::CancelRoamingToMorePrefMethodQuery
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::CancelRoamingToMorePrefMethodQuery()
    {      
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::ChangingConnectionToNote
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::ChangingConnectionToNote( 
                                                const TUint32 aConnMId )
    {
    iImpl->ChangingConnectionToNote( aConnMId );                
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::ChangingConnectionToNote
// ---------------------------------------------------------
//    
EXPORT_C void CConnectionUiUtilities::ChangingConnectionToNote( const TUint32 aConnMId, 
        											TRequestStatus& aStatus )
	{
	iImpl->ChangingConnectionToNote( aConnMId, aStatus ); 
	}

// ---------------------------------------------------------
// CConnectionUiUtilities::CancelChangingConnectionToNote
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::CancelChangingConnectionToNote()
    {
    iImpl->CancelChangingConnectionToNote();            
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::ConfirmMethodUsageQuery
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::ConfirmMethodUsageQuery(
                                                    const TUint32 /*aDestId*/,
                                                    const TUint32 /*aConnMId*/, 
                                                    TBool& aResult, 
                                                    TRequestStatus& aStatus )
    {
    aResult = EFalse;
    TRequestStatus* pS = &aStatus;
    User::RequestComplete( pS, KErrNotSupported );
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::CancelConfirmMethodUsageQuery
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::CancelConfirmMethodUsageQuery()
    {
    iImpl->CancelConfirmMethodUsageQuery();
    }
    

// ---------------------------------------------------------
// CConnectionUiUtilities::ConnectedViaConnMethodNote
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::ConnectedViaConnMethodNote( 
                                                    const TUint32 aConnMId )
    {
    iImpl->ConnectedViaConnMethodNote( aConnMId );    
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::ConnectedViaConnMethodNote
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::ConnectedViaConnMethodNote( 
                                                    const TUint32 aConnMId, 
                                                    TRequestStatus& aStatus )
    {
    iImpl->ConnectedViaConnMethodNote( aConnMId , aStatus );    
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::CancelConnectedViaConnMethodNote
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::CancelConnectedViaConnMethodNote()
    {
    iImpl->CancelConnectedViaConnMethodNote();
    }


// ---------------------------------------------------------
// CConnectionUiUtilities::ReconnectToMethodQuery
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::ReconnectToMethodQuery( 
                                              const TUint32 /*aDestId*/, 
                                              const TUint32 /*aConnMId*/, 
                                              const TBool /*aNextBestExists*/,
                                              TMsgQueryLinkedResults& aResult,
                                              TRequestStatus& aStatus )
    {
    aResult = EMsgQueryCancelled;
    TRequestStatus* pS = &aStatus;
    User::RequestComplete( pS, KErrNotSupported );
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::RoamingToMorePrefMethodQuery
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::RoamingToMorePrefMethodQuery( 
                                            const TUint32 /*aDestId*/, 
                                            const TUint32 /*aConnMId*/, 
                                            const TBool /*aNextBestExists*/,
                                            TMsgQueryLinkedResults& aResult,
                                            TRequestStatus& aStatus )
    {
    aResult = EMsgQueryCancelled;
    TRequestStatus* pS = &aStatus;
    User::RequestComplete( pS, KErrNotSupported );
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::ConfirmMethodUsageQuery
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::ConfirmMethodUsageQuery(
                                               TMsgQueryLinkedResults& aResult,
                                               const TBool aHomeNetwork,
                                               TRequestStatus& aStatus )
    {
    iImpl->ConfirmMethodUsageQuery( aResult, aHomeNetwork, aStatus );
    }


// ---------------------------------------------------------
// CConnectionUiUtilities::OffLineWlanNote
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::OffLineWlanNote( 
                                                      TRequestStatus& aStatus )
    {
    iImpl->OffLineWlanNote( aStatus );
    }
    
// ---------------------------------------------------------
// CConnectionUiUtilities::WlanPowerSaveTestNote
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::WlanPowerSaveTestNote(
                                               TBool&          aDisable,
                                               TRequestStatus& aStatus )
    {
    iImpl->WlanPowerSaveTestNote( aDisable, aStatus );
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::CancelWlanPowerSaveTestNote
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::CancelWlanPowerSaveTestNote()
    {
    iImpl->CancelWlanPowerSaveTestNote();
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::EasyWapiDlg
// ---------------------------------------------------------
//
EXPORT_C TBool CConnectionUiUtilities::EasyWapiDlg( TDes* aKey )
    {
    return iImpl->EasyWapiDlg( aKey );
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::NoWLANNetworksAvailable
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::NoWLANNetworksAvailableNote()
    {
    iImpl->NoWLANNetworksAvailableNote();
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::SearchWLANNetworkAsync
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::SearchWLANNetworkAsync( 
                                TRequestStatus& aStatus,
                                TWlanSsid& aSSID, 
                                TWlanConnectionMode& aConnectionMode, 
                                TWlanConnectionSecurityMode& aSecurityMode, 
                                TBool& aProtectedSetupSupported )
    {
    iImpl->SearchWLANNetworkAsync( aStatus, aSSID, 
                                   aConnectionMode, aSecurityMode,
                                   aProtectedSetupSupported );
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::SearchWLANNetworkAsync
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::SearchWLANNetworkAsync( 
                 TRequestStatus& aStatus,
                 TWlanSsid& aSSID, 
                 TWlanConnectionMode& aConnectionMode, 
                 TWlanConnectionExtentedSecurityMode& aExtendedSecurityMode, 
                 TBool& aProtectedSetupSupported )
    {
    iImpl->SearchWLANNetworkAsync( aStatus, 
                                   aSSID, 
                                   aConnectionMode, 
                                   aExtendedSecurityMode,
                                   aProtectedSetupSupported );
    }
    
// ---------------------------------------------------------
// CConnectionUiUtilities::SearchWLANNetworkSync
// ---------------------------------------------------------
//
EXPORT_C TBool CConnectionUiUtilities::SearchWLANNetworkSync( TWlanSsid& aSSID,
                                    TWlanConnectionMode& aConnectionMode,
                                    TWlanConnectionSecurityMode& aSecurityMode,
                                    TBool& aProtectedSetupSupported )
    {
    return iImpl->SearchWLANNetworkSync( aSSID, aConnectionMode, aSecurityMode,
                                         aProtectedSetupSupported );
    }

// ---------------------------------------------------------
// CConnectionUiUtilities::SearchWLANNetwork
// ---------------------------------------------------------
//
EXPORT_C TBool CConnectionUiUtilities::SearchWLANNetwork( TWlanSsid& aSSID,
                                    TWlanConnectionMode& aConnectionMode,
                                    TWlanConnectionSecurityMode& aSecurityMode,
                                    TBool& aProtectedSetupSupported )
    {
    return iImpl->SearchWLANNetwork( aSSID, aConnectionMode, aSecurityMode, 
                                     aProtectedSetupSupported );
    }
        
// ---------------------------------------------------------
// CConnectionUiUtilities::SearchWLANNetwork
// ---------------------------------------------------------
//
EXPORT_C TBool CConnectionUiUtilities::SearchWLANNetwork( 
                    TWlanSsid& aSSID,
                    TWlanConnectionMode& aConnectionMode,
                    TWlanConnectionExtentedSecurityMode& aExtendedSecurityMode,
                    TBool& aProtectedSetupSupported )
    {
    return iImpl->SearchWLANNetwork( aSSID, 
                                     aConnectionMode, 
                                     aExtendedSecurityMode, 
                                     aProtectedSetupSupported );
    } 

// ---------------------------------------------------------
// CConnectionUiUtilities::ConnectingViaDiscreetPopup
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::ConnectingViaDiscreetPopup( 
        const TUint32& aIapId )
    {
    iImpl->ConnectingViaDiscreetPopup( aIapId );
    } 

// ---------------------------------------------------------
// CConnectionUiUtilities::ConnectionErrorDiscreetPopup
// ---------------------------------------------------------
//
EXPORT_C void CConnectionUiUtilities::ConnectionErrorDiscreetPopup( 
        const TInt& aErrCode )
    {
    iImpl->ConnectionErrorDiscreetPopup( aErrCode );
    } 

// End of File
