/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Handles displaying wlan dialogs
*
*/


#include <wlanmgmtcommon.h>
#include <e32std.h>
#include <utf.h>
#include <cmpluginwlandef.h>
#include <ctsydomainpskeys.h>

#include "mpmwlanquerydialog.h"
#include "mpmiapselection.h"
#include "mpmconnmonevents.h"
#include "mpmlogger.h"

// valid Wep key lengths, to check wep key format
// (wep key format depends on key length)
const TInt KConnUiUtilsWepLengthASCII5 = 5;
const TInt KConnUiUtilsWepLengthASCII13 = 13;
const TInt KConnUiUtilsWepLengthASCII29 = 29;
const TInt KConnUiUtilsWepLengthHEX10 = 10;
const TInt KConnUiUtilsWepLengthHEX26 = 26;
const TInt KConnUiUtilsWepLengthHEX58 = 58; 

// Retry count and delay for storing Easy WLAN data
const TInt KStoreRetryCount = 10;
const TInt KStoreRetryDelay = 100000;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMPMWlanQueryDialog::CMPMWlanQueryDialog
// ---------------------------------------------------------------------------
//
CMPMWlanQueryDialog::CMPMWlanQueryDialog( CMPMIapSelection&  aSession,
                                          TUint32            aWlanIapId )
  : CActive( CActive::EPriorityStandard ),
    iIapSelection( aSession ),
    iNetworkPrefs(),
    iNotifWep(),
    iWlanQueryState( EWlanNetwork ),
    iWlanIapId( aWlanIapId ),
    iEasyWlanSelected( EFalse ),
    iOverrideStatus( KErrNone ),
    iConnUiUtils( NULL ), 
    iWps( NULL ),
    iWpsReturn( WiFiProt::EWiFiOK ),
    iWpsCompleted( EFalse ),
    iRetryCount( 0 )
    {
    
    }
    
// ---------------------------------------------------------------------------
// CMPMWlanQueryDialog::ConstructL
// ---------------------------------------------------------------------------
//
void CMPMWlanQueryDialog::ConstructL()
    {
    User::LeaveIfError(iNotifier.Connect());
    CActiveScheduler::Add( this );
    iConnUiUtils = CConnectionUiUtilities::NewL();
    User::LeaveIfError( iTimer.CreateLocal() );
    }

// ---------------------------------------------------------------------------
// CMPMWlanQueryDialog::NewL
// ---------------------------------------------------------------------------
//
CMPMWlanQueryDialog* CMPMWlanQueryDialog::NewL( CMPMIapSelection&  aSession,
                                                TUint32            aWlanIapElementId )
    {
    CMPMWlanQueryDialog* self = new( ELeave ) CMPMWlanQueryDialog( aSession,
                                                                   aWlanIapElementId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CMPMWlanQueryDialog::~CMPMWlanQueryDialog
// ---------------------------------------------------------------------------
//
CMPMWlanQueryDialog::~CMPMWlanQueryDialog()
    {
    MPMLOGSTRING( "CMPMWlanQueryDialog::~CMPMWlanQueryDialog enters" )
    // Check if this dialog instance was not started but only added to the queue
    if ( iIapSelection.Session()->MyServer().FirstInWlanQueryQueue() != this )
        {
        MPMLOGSTRING( "CMPMWlanQueryDialog::~CMPMWlanQueryDialog, not a active delete" )

        // Close notifier and timer
        iNotifier.Close();
        iTimer.Close();
        
        // We're not first in the queue, thus we can just delete.
        // But remember the pointer in the array.
        iIapSelection.Session()->MyServer().RemoveFromWlanQueryQueue( this );
        MPMLOGSTRING( "CMPMWlanQueryDialog::~CMPMWlanQueryDialog exits (break)" )
        return;
        }

    // Cancel previous dialogs if any.
    Cancel();

    // Close notifier and timer
    iNotifier.Close();
    iTimer.Close();

    delete iConnUiUtils;

    delete iWps;
    // Remove self from the queue
    iIapSelection.Session()->MyServer().RemoveFromWlanQueryQueue( this );
    
    // Start the next query
    CMPMWlanQueryDialog* dlg = iIapSelection.Session()->MyServer().FirstInWlanQueryQueue();
    if ( dlg )
        {
        MPMLOGSTRING( "CMPMWlanQueryDialog::~CMPMWlanQueryDialog starts new dialog" )
        dlg->OfferInformation( iWlanIapId, 
                               iIapSelection.OfflineNoteResponse(),
                               iStatus.Int() );
        // In destructor we cannot let the query leave
        TRAPD( err, dlg->StartWlanQueryL(); )
        if ( err != KErrNone )
            {
            MPMLOGSTRING2( "CMPMWlanQueryDialog::~CMPMWlanQueryDialog caught Leave %d, executing RunError()", err )
            dlg->RunError( err );
            }
        }

    MPMLOGSTRING( "CMPMWlanQueryDialog::~CMPMWlanQueryDialog exits" )
    }

// ---------------------------------------------------------------------------
// CMPMWlanQueryDialog::DoCancel
// ---------------------------------------------------------------------------
//
void CMPMWlanQueryDialog::DoCancel()
    {
    MPMLOGSTRING2( "CMPMWlanQueryDialog::DoCancel state %d", iWlanQueryState )
    if ( iWlanQueryState == EOffline )
        {
        iNotifier.CancelNotifier( KUidCOfflineWlanNoteDlg );
        }
    else if ( iWlanQueryState == EWlanNetwork )    
        {
        iConnUiUtils->CancelSearchWLANNetwork();
        }
    else if ( iWlanQueryState == EWepSettings )    
        {
        iNotifier.CancelNotifier( KUidEasyWepDlg );
        }
    else if( iWlanQueryState == EWPS )
        {
        iWps->CancelWiFiProt();
        }
    else if ( iWlanQueryState == EWpaSettings )
        {
        iNotifier.CancelNotifier( KUidEasyWpaDlg );
        }
    else // ERetrySettingsStorage
        {
        iTimer.Cancel();
        }
    }
    
// ---------------------------------------------------------------------------
// CMPMWlanQueryDialog::RunL
// ---------------------------------------------------------------------------
//
void CMPMWlanQueryDialog::RunL()
    {
    MPMLOGSTRING3( "CMPMWlanQueryDialog::RunL status %d state %d", 
                   iStatus.Int(), 
                   iWlanQueryState )
    if( iWlanQueryState == EOffline )
        {
        if( iStatus.Int() == KErrNone )
            {
            iIapSelection.SetOfflineNoteResponse( EOfflineResponseYes );
            if( iIapSelection.Session()->MyServer().CommsDatAccess()->CheckEasyWLanL( iWlanIapId ) )
                {
                MPMLOGSTRING( "CMPMWlanQueryDialog::RunL starting wlan network query" )
                GetNetworkPrefs();
                return;
                }
            }
        else if ( iStatus.Int() == KErrCancel )
            {
            iIapSelection.SetOfflineNoteResponse( EOfflineResponseNo );
            MPMLOGSTRING2( "CMPMWlanQueryDialog::RunL offline query returned %d", 
                           iStatus.Int() )
            }
        else
            {
            MPMLOGSTRING2( "CMPMWlanQueryDialog::RunL offline query returned %d", 
                           iStatus.Int() )
            }
        }
    else if( iWlanQueryState == EWlanNetwork )
        {
        if( iStatus.Int() == KErrNone )
            {
            
            TUint secMode( 0 );
            TWlanNetMode connMode( EInfra );
            TInt err = ConnSecModeToCommsDatSecMode( iNetworkPrefs().iSecMode,
                                                     secMode );
            if( err != KErrNone )
       	    	{
       	    	MPMLOGSTRING( "CMPMWlanQueryDialog::RunL, Unknown security mode" )
       	    	iWlanIapId = 0;
                iWlanQueryState = EWlanNetwork;
       	    	iIapSelection.UserWlanSelectionDoneL( KErrCouldNotConnect, iWlanIapId );
       	    	return;
       	    	}
            
            TUint32 configuredIap = iIapSelection.Session()->MyServer().
                CommsDatAccess()->CheckWLANIapWithSsidL( iNetworkPrefs().iSsId, 
                                                         secMode,
                                                         connMode );
            // If previously unknown IAP, query wep/wpa parameters
            //
            if( !configuredIap )
                {
                iEasyWlanSelected = ETrue;
                if ( iNetworkPrefs().iProtectedSetupSupported && 
                     iNetworkPrefs().iNetworkMode != EWlanConnectionModeAdhoc )
                    {
                    StartWpsDlgL();
                    return;                                    
                    }         
                // WEP
                //
                if( iNetworkPrefs().iSecMode == EWlanConnectionSecurityWep )
                    {
                    MPMLOGSTRING( "CMPMWlanQueryDialog::RunL unknown wep network" )
                    iWlanQueryState = EWepSettings;
                    iNotifier.StartNotifierAndGetResponse( iStatus, 
                                                           KUidEasyWepDlg, 
                                                           iNotifWep, 
                                                           iNotifWep );
                    SetActive();
                    return;
                    }
                // WPA
                //
                else if ( iNetworkPrefs().iSecMode == EWlanConnectionSecurityWpaPsk )
                    {
                    MPMLOGSTRING( "CMPMWlanQueryDialog::RunL unknown wpa network" )
                    iWlanQueryState = EWpaSettings;
                    iNotifier.StartNotifierAndGetResponse( iStatus, 
                                                           KUidEasyWpaDlg, 
                                                           iNotifWpaKey, 
                                                           iNotifWpaKey );
                    SetActive();
                    return;

                    }
                else
                    {
                    MPMLOGSTRING2( "CMPMWlanQueryDialog::RunL security mode %d", 
                                   iNetworkPrefs().iSecMode )
                    }
                }
        	
        	else
        	    {
        	    iWlanIapId = configuredIap;
        	    MPMLOGSTRING2( "CMPMWlanQueryDialog::RunL found configured iap matching ssid, id %d", 
        	                   iWlanIapId )
        	    
        	    }
            }
        else if( iStatus.Int() == KErrCancel )
            {
            iEasyWlanSelected = EFalse;
            MPMLOGSTRING( "CMPMWlanQueryDialog::RunL easy wlan dialog cancelled, \
setting easy wlan as not selected" )
            }
        else
            {
            MPMLOGSTRING2( "CMPMWlanQueryDialog::RunL EWlanNetwork error %d", 
                iStatus.Int() )
            }
        
        }
    else if( iWlanQueryState == EWepSettings || iWlanQueryState == EWpaSettings )
        {
        if( iStatus.Int() == KErrCancel )
      	   {
            MPMLOGSTRING( "CMPMWlanQueryDialog::Key dialog cancelled,back to easy wlan" )
            iNetworkPrefs().iSsId.FillZ();
            iNetworkPrefs().iNetworkMode = EWlanConnectionModeNotConnected;
            iNetworkPrefs().iSecMode = EWlanConnectionSecurityOpen;
            GetNetworkPrefs();
            return;
            }
        }
    else if( iWlanQueryState == EWPS )
        {
        MPMLOGSTRING2( "CMPMWlanQueryDialog::RunL WPS, status %d", iStatus.Int() )
        if( iStatus.Int()  == KErrNone )
            {
            if( iWpsReturn == WiFiProt::EWiFiOK)
                {
                MPMLOGSTRING( "CMPMWlanQueryDialog::RunL WPS ok" )
                iWpsCompleted = ETrue;
                }
            else if ( iWpsReturn == WiFiProt::EWiFiCancel )
                {
                MPMLOGSTRING( "CMPMWlanQueryDialog::RunL WPS cancelled" )
                iStatus = KErrCancel;
                }
            else // WiFiProt::EWiFiNoAuto
                {
                MPMLOGSTRING( "CMPMWlanQueryDialog::RunL No Auto, starting WPA key query" )
                iWlanQueryState = EWpaSettings;
                iNotifier.StartNotifierAndGetResponse( iStatus, 
                                                       KUidEasyWpaDlg, 
                                                       iNotifWpaKey, 
                                                       iNotifWpaKey );
                SetActive();
                return;
                }
            }
        }
    else if ( iWlanQueryState == ERetrySettingsStorage )
        {
        // Retry settings storage
        MPMLOGSTRING( "CMPMWlanQueryDialog::RunL Retry settings storage" )
        }
    else
        {
		MPMLOGSTRING2( "CMPMWlanQueryDialog::RunL, unknown state: %d", iWlanQueryState )
        User::Leave( KErrCancel );
        }
        
    TRAPD( leaveCode, iIapSelection.UserWlanSelectionDoneL( iStatus.Int(), iWlanIapId ) );
    if( leaveCode != KErrNone )
        {
        // Something caused method to leave, if CommsDat was locked we should retry 
        if ( iStatus.Int() == KErrNone &&
             leaveCode == KErrLocked &&
             iRetryCount > 0 )
            {
            MPMLOGSTRING2( "CMPMWlanQueryDialog::RunL Start retry timer, retry count %d", iRetryCount )
            iWlanQueryState = ERetrySettingsStorage;
            iRetryCount--;
            iTimer.After( iStatus, KStoreRetryDelay );
            SetActive();
            return;
            }
        else
            {
            User::Leave( leaveCode );
            }
        }
    
    iWlanIapId = 0;
    iWlanQueryState = EWlanNetwork;
    }

// ---------------------------------------------------------------------------
// CMPMWlanQueryDialog::RunError
// ---------------------------------------------------------------------------
//
TInt CMPMWlanQueryDialog::RunError( TInt aError )
    {
    MPMLOGSTRING2( "CMPMWlanQueryDialog::RunError failed with %d", aError )
    iEasyWlanSelected = EFalse;    
    iIapSelection.ChooseIapComplete( aError, NULL );    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMPMWlanQueryDialog::StartWlanQueryL
// -----------------------------------------------------------------------------
//
void CMPMWlanQueryDialog::StartWlanQueryL()
    {
    MPMLOGSTRING( "CMPMWlanQueryDialog::StartWlanQuery" )

    iRetryCount = KStoreRetryCount;
    
    // if first dialog user, add it to the array and continue.
    //
    if ( iIapSelection.Session()->MyServer().WlanQueryQueue()->Count() == 0 )
        {
        MPMLOGSTRING( "CMPMWlanQueryDialog::StartWlanQuery First one, start immediately" )
        iIapSelection.Session()->MyServer().AppendWlanQueryQueueL( this );
        }
    // if another dialog should be processed before this, just add and return.
    //
    else if ( iIapSelection.Session()->MyServer().FirstInWlanQueryQueue() != this )
        {
        MPMLOGSTRING( "CMPMWlanQueryDialog::StartWlanQuery Latter, process later" )
        iIapSelection.Session()->MyServer().AppendWlanQueryQueueL( this );
        return;
        }
    // else Continue to process the first dialog.(this one)
    // FirstInWlanQueryQueue() == this && Count() > 0
    
    TUint32 activeWlanIap = iIapSelection.Session()->MyServer().IsWlanConnectionStartedL( 
        iIapSelection.Session()->MyServer().CommsDatAccess() );

    // Get EmergencyCallInfo via Publish & Subscribe
    // 
    TInt emergencyCallEstablished( 0 );
    RProperty::Get( KPSUidCtsyEmergencyCallInfo, 
                    KCTSYEmergencyCallInfo, 
                    emergencyCallEstablished );  
    MPMLOGSTRING2( "CMPMWlanQueryDialog::StartWlanQuery KCTSYEmergencyCallInfo = %d", 
                   emergencyCallEstablished )

    if( !emergencyCallEstablished && 
        iIapSelection.Session()->IsPhoneOfflineL() && 
        !activeWlanIap && 
        iIapSelection.OfflineNoteResponse() != EOfflineResponseYes &&
        iOverrideStatus == KErrNone )
        {
        // if user has already refused offline note complete immediately
        //
        if ( iIapSelection.OfflineNoteResponse() == EOfflineResponseNo )
            {
            MPMLOGSTRING( "CMPMWlanQueryDialog::StartWlanQuery user has already refused offline" )
            iIapSelection.UserWlanSelectionDoneL( KErrCancel, iWlanIapId );
            }
        else
            {
            MPMLOGSTRING( "CMPMWlanQueryDialog::StartWlanQuery, starting offline note" )            
            iWlanQueryState = EOffline;
            iNotifier.StartNotifierAndGetResponse( iStatus, 
                                                   KUidCOfflineWlanNoteDlg, 
                                                   KNullDesC8(), 
                                                   iOfflineReply );
            SetActive();
            }
        }
    // if easy wlan iap and some wlan iap started, use existing connection
    //
    else if( iIapSelection.Session()->MyServer().CommsDatAccess()->CheckEasyWLanL( iWlanIapId )  && 
             activeWlanIap )
        {
        MPMLOGSTRING2( "CMPMWlanQueryDialog::StartWlanQuery using active WLAN IAP %d", 
                      activeWlanIap )
        iIapSelection.UserWlanSelectionDoneL( KErrNone, activeWlanIap );      
        }
    // if easy wlan iap and easy wlan is not already started
    // 
    else if( iIapSelection.Session()->MyServer().CommsDatAccess()->CheckEasyWLanL( iWlanIapId )  && 
             !iIapSelection.Session()->MyServer().CheckIfStarted( iWlanIapId ) &&
             iNetworkPrefs().iSsId.Length() == 0 &&
             iOverrideStatus == KErrNone )
        {
        MPMLOGSTRING( "CMPMWlanQueryDialog::StartWlanQuery starting network query" )
        GetNetworkPrefs();
        }
    // if an error was given through OfferInformation() -call we abort the execution.
    //
    else if ( iOverrideStatus != KErrNone )
        {
        MPMLOGSTRING2( "CMPMWlanQueryDialog::StartWlanQuery inherited error %d", iOverrideStatus )
        iIapSelection.UserWlanSelectionDoneL( iOverrideStatus, iWlanIapId );
        }
    else
        {
        MPMLOGSTRING( "CMPMWlanQueryDialog::StartWlanQuery no wlan dialog to show" )
        iIapSelection.UserWlanSelectionDoneL( KErrNone, iWlanIapId );
        }
    }


// -----------------------------------------------------------------------------
// CMPMWlanQueryDialog::OfferInformation
// -----------------------------------------------------------------------------
//
void CMPMWlanQueryDialog::OfferInformation(
                TUint32 aWlanIapId, 
                TOfflineNoteResponse aOfflineStatus,
                TInt aDialogStatus )
    {
    TBool isEasyWlan( EFalse );
    // Take information from earlier note into use
    // if the selected IAP is EasyWLAN or Offline note response was No.
    //
    TRAP_IGNORE( isEasyWlan = 
        iIapSelection.Session()->MyServer().CommsDatAccess()->CheckEasyWLanL( iWlanIapId ); )
    if ( isEasyWlan ) 
        {
        // iEasyWlanSelected is not passed as only the first connection 
        // should write the easy wlan settings to commsdat if easy wlan is used.
        // 
        MPMLOGSTRING3( "CMPMWlanQueryDialog<0x%x>::OfferInformation: EasyWLAN, iap %d", 
                       iIapSelection.Session()->ConnectionId(),
                       aWlanIapId ) 

        iWlanIapId      = aWlanIapId;
        iOverrideStatus = aDialogStatus;
        }
    if ( aOfflineStatus != EOfflineResponseUndefined )
        {
        MPMLOGSTRING3( "CMPMWlanQueryDialog<0x%x>::OfferInformation: offline response %d",
                       iIapSelection.Session()->ConnectionId(),
                       aOfflineStatus ) 
        iIapSelection.SetOfflineNoteResponse( aOfflineStatus );        
        iOverrideStatus = aDialogStatus;
        }

#ifdef _LOG
    else
        {
        MPMLOGSTRING( "CMPMWlanQueryDialog::OfferInformation, information not taken." ) 
        }
#endif 
    }


// -----------------------------------------------------------------------------
// CMPMWlanQueryDialog::StoreEasyWlanSelectionL
// -----------------------------------------------------------------------------
//
void CMPMWlanQueryDialog::StoreEasyWlanSelectionL()
    {
    if( iEasyWlanSelected )
        {
        TWlanSsid ssid;
        TWepKeyData wepData;
        TUint wpaPskEnabled( 0 );
        TUint wpaPskLen( 0 );
        TBuf8<KWLMMaxWpaPskLength> wpa;       
        TUint secMode( 0 );
        TWlanNetMode connMode( EInfra );

        if( iWpsCompleted )
            {
            GetEasyWlanDataForWpsL( ssid,
                                    secMode,
                                    connMode, 
                                    wepData,
                                    wpaPskEnabled, 
                                    wpa, 
                                    wpaPskLen );
            MPMLOGSTRING3( "CMPMWlanQueryDialog::StoreEasyWlanSelection: \
wps used, connection iap %d, ssid %S", iWlanIapId, &iWpsAttribute.iSsid )
            iIapSelection.Session()->MyServer().Events()->
                SetConnInfoIapSsid( iWlanIapId, 
                                    iWpsAttribute.iSsid );
            iIapSelection.Session()->MyServer().CommsDatAccess()->SetEasyWlanDataL( ssid,
                                                                         secMode,
                                                                         connMode, 
                                                                         wepData,
                                                                         wpaPskEnabled, 
                                                                         wpa, 
                                                                         wpaPskLen );
            return;
            }

        MPMLOGSTRING( "CMPMWlanQueryDialog::StoreEasyWlanSelection: setting easy wlan data" )
        GetEasyWlanDataL( ssid,
                          secMode,
                          connMode, 
                          wepData,
                          wpaPskEnabled, 
                          wpa, 
                          wpaPskLen );

        MPMLOGSTRING3( "CMPMWlanQueryDialog::StoreEasyWlanSelection: \
setting easy wlan iap %d ssid %S in connmon events", iWlanIapId, &iNetworkPrefs().iSsId )
        iIapSelection.Session()->MyServer().Events()->SetConnInfoIapSsid( iWlanIapId, 
                                                                          iNetworkPrefs().iSsId );
        iIapSelection.Session()->MyServer().CommsDatAccess()->SetEasyWlanDataL( ssid,
                                                                     secMode,
                                                                     connMode, 
                                                                     wepData,
                                                                     wpaPskEnabled, 
                                                                     wpa, 
                                                                     wpaPskLen );
                    
        }
    else
        {
        MPMLOGSTRING( "CMPMWlanQueryDialog::StoreEasyWlanSelection: no easy wlan selected by user" )
        }
    }


// -----------------------------------------------------------------------------
// CMPMWlanQueryDialog::GetEasyWlanDataL
// -----------------------------------------------------------------------------
//
void CMPMWlanQueryDialog::GetEasyWlanDataL( TWlanSsid&    aSsid,
                                            TUint&        aSecMode,
                                            TWlanNetMode& aConnMode,
                                            TWepKeyData&  aWepData,
                                            TUint&        aEnableWpaPsk,
                                            TDes8&        aWpaPsk,
                                            TUint&        aWpaKeyLen  )
    {
    aSsid.Copy( iNetworkPrefs().iSsId );
    if ( iNetworkPrefs().iNetworkMode == EWlanConnectionModeAdhoc )
        {
        aConnMode = EAdhoc;
        }

    // Set security mode
    TInt err = ConnSecModeToCommsDatSecMode( iNetworkPrefs().iSecMode,
                                             aSecMode );

    if( err != KErrNone )
        {
        MPMLOGSTRING2( "CMPMWlanQueryDialog::StoreEasyWlanSelection: \
unsupported sec mode %d. leaving", iNetworkPrefs().iSecMode )
        User::Leave( KErrNotSupported );
        }

    if( iNetworkPrefs().iSecMode == EWlanConnectionSecurityWep )
        {
        CnvUtfConverter::ConvertFromUnicodeToUtf8( aWepData.iWep1, iNotifWep().iKey );
        //MPMLOG8STRING2( "CMPMWlanQueryDialog::StoreEasyWlanSelection:wep-key8:%S ", &aWepData.iWep1 )
        if( iNotifWep().iHex )
            {
        	aWepData.iWepFormat1 = 1;
        	MPMLOGSTRING( "CMPMWlanQueryDialog::StoreEasyWlanSelection: wep format is 1 (hex)") 
        	}
        aWepData.iDefaultWep = EWlanDefaultWepKey1;
        }
    else if( iNetworkPrefs().iSecMode == EWlanConnectionSecurityWpaPsk )
        {        
        CnvUtfConverter::ConvertFromUnicodeToUtf8( aWpaPsk, iNotifWpaKey() );
        aEnableWpaPsk = 1;
        aWpaKeyLen = aWpaPsk.Length();
        MPMLOGSTRING3( "CMPMWlanQueryDialog::StoreEasyWlanSelection:wpapsk-key:%s, len:%d", 
                       &iNotifWpaKey(), 
                       aWpaKeyLen )
        }
    else
        {
        MPMLOGSTRING2( "CMPMWlanQueryDialog::StoreEasyWlanSelection: \
no key handling needed for sec mode %d", iNetworkPrefs().iSecMode )
        }
    }

// ---------------------------------------------------------------------------
// CMPMWlanQueryDialog::ConnSecModeToCommsDatSecMode
// ---------------------------------------------------------------------------
//
TInt CMPMWlanQueryDialog::ConnSecModeToCommsDatSecMode( TWlanConnectionSecurityMode aConnSecmode,
                                                        TUint& aCommsdatSecMode ) const
    {

    if ( aConnSecmode == EWlanConnectionSecurityOpen )
        {
        aCommsdatSecMode = EWlanSecModeOpen;
        }
    else if ( aConnSecmode == EWlanConnectionSecurityWep )
        {
        aCommsdatSecMode = EWlanSecModeWep;
        }
    else if ( aConnSecmode == EWlanConnectionSecurityWpaPsk )
       	{
       	aCommsdatSecMode = EWlanSecModeWpa2;
       	}
    else if ( aConnSecmode == EWlanConnectionSecurityWpa )
       	{
       	aCommsdatSecMode = EWlanSecModeWpa;
        }
    else
       	{
       	MPMLOGSTRING( "CMPMWlanQueryDialog::RunL, Unknown security mode" )
       	return KErrCouldNotConnect;
       	}
    return KErrNone;    
    }


// ---------------------------------------------------------------------------
// CMPMWlanQueryDialog::IapSecModeToCommsDatSecMode
// ---------------------------------------------------------------------------
//
TInt CMPMWlanQueryDialog::IapSecModeToCommsDatSecMode( TWlanIapSecurityMode aIapSecmode,
                                                       TUint& aCommsdatSecMode ) const
    {
    switch( aIapSecmode )
        {
        case EWlanIapSecurityModeAllowUnsecure:
    	    aCommsdatSecMode |= EWlanSecModeOpen;
            break;
        case EWlanIapSecurityModeWep:
            aCommsdatSecMode |= EWlanSecModeWep;
            break;
        case EWlanIapSecurityModeWpa:
            aCommsdatSecMode |= EWlanSecModeWpa;
            break;
        case EWlanIapSecurityModeWpa2Only:
            aCommsdatSecMode |= EWlanSecModeWpa2;
            break;
        default:
            MPMLOGSTRING2( "CMPMWlanQueryDialog::IapSecModeToCommsDatSecMode: \
unsupported sec mode %d ", aIapSecmode )
            return KErrNotSupported;
        }
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CMPMWlanQueryDialog::GetEasyWlanDataForWpsL
// -----------------------------------------------------------------------------
//
void CMPMWlanQueryDialog::GetEasyWlanDataForWpsL( TWlanSsid&    aSsid,
                                                  TUint&        aSecMode,
                                                  TWlanNetMode& aConnMode,
                                                  TWepKeyData&  aWepKeyData, 
                                                  TUint&        aEnableWpaPsk,
                                                  TDes8&        aWpaPsk,
                                                  TUint&        aWpaKeyLen  )
    {
    aSsid.Copy( iWpsAttribute.iSsid );
    if ( iWpsAttribute.iOperatingMode == EWlanOperatingModeAdhoc )
        {
        aConnMode = EAdhoc;
        }
    TInt err = IapSecModeToCommsDatSecMode( iWpsAttribute.iSecurityMode, 
                                            aSecMode );
    if( err != KErrNone )
        {
        MPMLOGSTRING( "CMPMWlanQueryDialog::GetEasyWlanDataForWpsL: \
unsupported wps sec mode leaving" )
        User::Leave( KErrNotSupported );
        }

    if( iWpsAttribute.iSecurityMode == EWlanIapSecurityModeWep )
        {
        if( iWpsAttribute.iWepKey1.Length() && 
            IsWepFormatHexL( iWpsAttribute.iWepKey1.Length() ) )
            {
            aWepKeyData.iWepFormat1 = 1;
            }
        if( iWpsAttribute.iWepKey2.Length() && 
            IsWepFormatHexL( iWpsAttribute.iWepKey2.Length() ) )
            {
            aWepKeyData.iWepFormat2 = 1;
            }
        if( iWpsAttribute.iWepKey3.Length() && 
            IsWepFormatHexL( iWpsAttribute.iWepKey3.Length() ) )
            {
            aWepKeyData.iWepFormat3 = 1;
            }
        if( iWpsAttribute.iWepKey4.Length() && 
            IsWepFormatHexL( iWpsAttribute.iWepKey4.Length() ) )
            {
            aWepKeyData.iWepFormat4 = 1;
            }
        aWepKeyData.iWep1.Copy( iWpsAttribute.iWepKey1 );
        aWepKeyData.iWep2.Copy( iWpsAttribute.iWepKey2 );
        aWepKeyData.iWep3.Copy( iWpsAttribute.iWepKey3 );
        aWepKeyData.iWep4.Copy( iWpsAttribute.iWepKey4 );
        aWepKeyData.iDefaultWep = iWpsAttribute.iWepDefaultKey;        
        }
    if( iWpsAttribute.iWpaPreSharedKey.Length() )
        {
        aEnableWpaPsk = 1;
        aWpaPsk.Copy( iWpsAttribute.iWpaPreSharedKey );
        aWpaKeyLen = aWpaPsk.Length();
        }

    }

// ---------------------------------------------------------------------------
// CMPMWlanQueryDialog::StartWpsDlgL
// ---------------------------------------------------------------------------
//
void CMPMWlanQueryDialog::StartWpsDlgL()
    {
    MPMLOGSTRING( "CMPMWlanQueryDialog::StartWpsDlgL, Starting WPS" )
    iWlanQueryState = EWPS;
    iWps = CWiFiProtUiClient::NewL();
    iWps->StartWiFiProtConnL( iNetworkPrefs().iSsId, // TWlanSsid&,
                              iWpsAttribute,
                              iWpsReturn,
                              iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CMPMWlanQueryDialog::IsWepFormatHexL
// ---------------------------------------------------------------------------
//
TBool CMPMWlanQueryDialog::IsWepFormatHexL( TInt aLength ) const
    {
    MPMLOGSTRING2( "CMPMWlanQueryDialog::IsWepFormatHexL length %d", aLength )

    if ( ( aLength == KConnUiUtilsWepLengthASCII5 ) ||
        ( aLength == KConnUiUtilsWepLengthASCII13 ) ||
        ( aLength == KConnUiUtilsWepLengthASCII29 ) )
        {
        return EFalse;
        }
    else if ( ( aLength == KConnUiUtilsWepLengthHEX10 ) ||
        ( aLength == KConnUiUtilsWepLengthHEX26 ) ||
        ( aLength == KConnUiUtilsWepLengthHEX58 ) )
        {
        return ETrue;
        }
    else
        {
        User::Leave( KErrNotSupported );
        }            
    return EFalse;
    }
    
// ---------------------------------------------------------------------------
// CMPMWlanQueryDialog::GetNetworkPrefs
// ---------------------------------------------------------------------------
//
void CMPMWlanQueryDialog::GetNetworkPrefs()
    {
    iWlanQueryState = EWlanNetwork;
    iConnUiUtils->SearchWLANNetworkAsync( iStatus,
                                          iNetworkPrefs().iSsId,
                                          iNetworkPrefs().iNetworkMode,
                                          iNetworkPrefs().iSecMode, 
                                          iNetworkPrefs().iProtectedSetupSupported
                                        );
    SetActive();
    }

