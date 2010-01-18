/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Handles selecting Iap for connection
*
*/

#include "mpmiapselection.h"
#include "mpmlogger.h"
#include "mpmdialog.h"
#include "mpmserversession.h"
#include "mpmcommsdataccess.h"
#include "mpmconnmonevents.h"
#include "mpmconfirmdlgstarting.h"
#include "mpmdefaultconnection.h"
#include "mpmwlanquerydialog.h"
#include "extendedconnpref.h"

// ---------------------------------------------------------------------------
// CMPMIapSelection::CMPMIapSelection
// ---------------------------------------------------------------------------
//
CMPMIapSelection::CMPMIapSelection( CMPMCommsDatAccess*  aCommsDatAccess,
                                    CMPMServerSession*   aSession )
    : iChooseIapState( ENoConnection ),
      iCommsDatAccess( aCommsDatAccess ),
      iStoredIapInfo(),
      iSession( aSession ),
      iConfirmDlgStarting( NULL ),
      iDialog( NULL ),
      iWlanDialog( NULL ),
      iNextBestExists( EFalse ),
      iUserSelectionIapId( 0 ),
      iUserSelectionSnapId( 0 ),
      iImplicitState( EImplicitStart ),
      iOfflineNoteResponse( EOfflineResponseUndefined ),
      iIsRoaming( EFalse ),
      iNewWlansAllowed ( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// CMPMIapSelection::ConstructL
// ---------------------------------------------------------------------------
//
void CMPMIapSelection::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// CMPMIapSelection::NewL
// ---------------------------------------------------------------------------
//
CMPMIapSelection* CMPMIapSelection::NewL( CMPMCommsDatAccess*  aCommsDatAccess,
                                          CMPMServerSession*   aSession )
    {
    CMPMIapSelection* self = new ( ELeave ) CMPMIapSelection( aCommsDatAccess,
                                                              aSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CMPMIapSelection::~CMPMIapSelection
// ---------------------------------------------------------------------------
//
CMPMIapSelection::~CMPMIapSelection()
    {
    // stop confirm dialog in case one exists
    //
    StopDisplayingStartingDlg();
    
    // Cancel WLAN availability check in case it is active
    //
    if ( iSession )
        {
    	  iSession->MyServer().Events()->CancelCheckWlanWlanAvailability( this );	
        }	
    
    delete iConfirmDlgStarting;
    delete iDialog;
    delete iWlanDialog;
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::ChooseIapL
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::ChooseIapL( const TMpmConnPref& aChooseIapPref )
    {
#ifdef _DEBUG
    // Dump CommsDat IAPs to log in order to support testing
    // 
    iCommsDatAccess->DumpIAPsL();
    TRAPD( err, iCommsDatAccess->DumpSnapsL() );
    if ( err != KErrNone )
        {
        MPMLOGSTRING2(
            "CMPMIapSelection::ChooseIapL: DumpSnapsL fail %d",
            err )
        }
#endif // _DEBUG
    
    iChooseIapPref = aChooseIapPref;

    if ( iSession->IsWlanOnlyL( iNewWlansAllowed ) )
        {
        if ( iChooseIapPref.BearerSet() ==
             TExtendedConnPref::EExtendedConnBearerUnknown )
            {
            // Force bearerset to WLAN if received bearerset was empty
            iChooseIapPref.SetBearerSet(
                TExtendedConnPref::EExtendedConnBearerWLAN );
            }
        else if ( iChooseIapPref.BearerSet() &
                  TExtendedConnPref::EExtendedConnBearerWLAN )
            {
            // Force bearerset to WLAN only in case where wlan was included
            // in the received bearerset.
            iChooseIapPref.SetBearerSet(
                TExtendedConnPref::EExtendedConnBearerWLAN );          
            }
        else
            {
            // If wlan was not included in received bearerset,
            // complete selection with error code
            ChooseIapComplete( KErrPermissionDenied,
                               iSession->MyServer().UserConnPref() );
            return; 
            }
        }
    
    // Always use stored connection info.
    // If stored information doesn't exist, a normal sequence is used.
    TUint32 snap( 0 );
    TUint32 iap ( 0 );
    
    snap = iSession->MyServer().GetBMSnap( iSession->ConnectionId() );
    iap = iSession->MyServer().GetBMIap( iSession->ConnectionId() );
    if ( snap )
        {
        MPMLOGSTRING2( "CMPMIapSelection::ChooseIapL: Set snap %i to preferences",
            snap )
        iChooseIapPref.SetSnapId( snap );
        iChooseIapPref.SetIapId( 0 );
        }
    else if ( iap ) 
        {      
        MPMLOGSTRING2( "CMPMIapSelection::ChooseIapL: Set iap %i to preferences",
                iap )
        iChooseIapPref.SetIapId( iap );
        iChooseIapPref.SetSnapId( 0 );
        }

    MPMLOGSTRING2( "CMPMIapSelection::ChooseIapL: IapID: %i",
            iChooseIapPref.IapId() )
    
    // Check if direct IAP connection is tried to make
    if ( iChooseIapPref.IapId() != 0 )
        { 
        TMPMBearerType bearerType = iSession->MyServer().CommsDatAccess()->GetBearerTypeL( iChooseIapPref.IapId() );
        MPMLOGSTRING2( "CMPMIapSelection::ChooseIapL: bearerType: %i", bearerType )

        // Complete selection with error code if wlan only was set and cellular IAP other 
        // than MMS IAP was tried to access  
        if ( ( iChooseIapPref.BearerSet() ==
               TExtendedConnPref::EExtendedConnBearerWLAN ) && 
                ( bearerType == EMPMBearerTypePacketData ) && 
                ( iSession->IsMMSIap( iChooseIapPref.IapId() ) == EFalse ) ) 
            {
            ChooseIapComplete( KErrPermissionDenied, NULL );
            return;
            }
        }
        
    MPMLOGSTRING2( "CMPMIapSelection::ChooseIapL:\
 connection type %d", iChooseIapPref.ConnType() )

    if ( iChooseIapPref.ConnType() == TMpmConnPref::EConnTypeDefault )
        {
        // Default connection is requested.

        MPMLOGSTRING( "CMPMIapSelection::ChooseIapL:\
 Default Connection" )

        if ( iSession->UseUserConnPref() )
            {
            // User connection active -> use it
            // Add info into the BM connections
            iSession->MyServer().AppendBMConnection(iSession->ConnectionId(),
                iSession->MyServer().UserConnPref()->SnapId(),
                iSession->MyServer().UserConnPref()->IapId(), 
                EStarting,
				*iSession );

            ChooseIapComplete( KErrNone, iSession->MyServer().UserConnPref());
            return;
            }

        TCmDefConnType type( TCmDefConnType( 0 ) );
        TUint32 id( 0 );
        iSession->MyServer().DefaultConnection()->GetDefaultConnectionL( type, id );
        
        MPMLOGSTRING3( "CMPMIapSelection::ChooseIapL:\
 default connection type %d id %d", type, id )
       
        if( type == ECmDefConnConnectionMethod )
            {
            iChooseIapPref.SetIapId( id );
            iChooseIapPref.SetSnapId( 0 );
            ExplicitConnectionL();
            }
        else if( type == ECmDefConnDestination )
            { 
            iChooseIapPref.SetSnapId( id );
            ExplicitConnectionL();
            }
        else
            {
            if( iCommsDatAccess->IsSnapEmptyL( id ) )
                {
                // start implicit connection
                ImplicitConnectionL();
                return;
                }

            iChooseIapPref.SetSnapId( id );

            // start connection as if explicitly defined 
            // by application
            ExplicitConnectionL();
            }       
        }
    else if ( iChooseIapPref.ConnType() == TMpmConnPref::EConnTypeExplicit )
        {
        // Client has defined SNAP or IAP connection to be used.
        
        // If usage of given IAP is mandated, do not allow user connection.
        if ( !iChooseIapPref.MandateIap() )
            {
            TBool useUCPref = iSession->UseUserConnPref();

            // Check that user connection is active, it is not in internet snap 
            // and destination id is something else than zero.
            // OR
            // User connection is active and iap id is non-zero.
            if ( ( useUCPref &&
                   !iSession->MyServer().UserConnectionInInternet() &&
                   iChooseIapPref.SnapId() ) ||
                 ( useUCPref && iChooseIapPref.IapId() ) )
                 {
                 // Add info into the BM connections
                 iSession->MyServer().AppendBMConnection(
                     iSession->ConnectionId(),
                     iSession->MyServer().UserConnPref()->SnapId(),
                     iSession->MyServer().UserConnPref()->IapId(), 
                     EStarting,
                     *iSession );
    
                 ChooseIapComplete( KErrNone, iSession->MyServer().UserConnPref() );
                 return;                
                 }
            }
        ExplicitConnectionL();
        }
    else
        {
        // Used connection will be asked from the user.
        ASSERT( iChooseIapPref.ConnType() == TMpmConnPref::EConnTypeImplicit );

        if ( iSession->UseUserConnPref() )
            {
            // Do not prompt the user in this case.
            // Add info into the BM connections
            iSession->MyServer().AppendBMConnection(
                iSession->ConnectionId(),
                iSession->MyServer().UserConnPref()->SnapId(),
                iSession->MyServer().UserConnPref()->IapId(), 
                EStarting,
				*iSession );

            ChooseIapComplete( KErrNone, iSession->MyServer().UserConnPref() );
            return;
            }
        
        ImplicitConnectionL();
        }
    }


// -----------------------------------------------------------------------------
// CMPMIapSelection::ExplicitConnectionL
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::ExplicitConnectionL()
    {
    TBool   iapTypeLanOrWlan = EFalse;
    TUint32 validateIapId    = iChooseIapPref.IapId();
    
    MPMLOGSTRING2(
        "CMPMIapSelection::ExplicitConnectionL - Connection Id = 0x%x",
        iSession->ConnectionId() )

    TUint32 snap( 0 );
    
    if ( validateIapId == 0 )
        {
        if ( iChooseIapPref.SnapId() )
            {
            snap = iChooseIapPref.SnapId();
            }
        else
            {
            // Neither defined -> attempt to use 
            // already stored connection info
            snap = iSession->MyServer().GetBMSnap( iSession->ConnectionId() );
            iChooseIapPref.SetSnapId( snap );
            if( !snap )
                {
                // If snap not defined, this must be iap connection
                //
                validateIapId = iSession->MyServer().GetBMIap( iSession->ConnectionId() );
                iChooseIapPref.SetIapId( validateIapId );
                }
            }
        }

    // IAP Id explicitly defined
    // 
    if ( validateIapId )
        {
        MPMLOGSTRING2(
            "CMPMIapSelection::ExplicitConnectionL - IAP Id = %i", 
            validateIapId )

        TUint32 retNetId = 0;
        iCommsDatAccess->ValidateIapL( iSession->ConnectionId(), 
                                       validateIapId, 
                                       retNetId, 
                                       iapTypeLanOrWlan,
                                       *iSession );

        // Set validated IAP Id and returned Access Network Id
        // in connection preferences
        //
        iChooseIapPref.SetIapId( validateIapId );
        iChooseIapPref.SetNetId( retNetId );

        // In case offline mode is enabled, only LAN or WLAN is allowed.
        // If some other bearer has been requested, then error code 
        // KErrGprsOfflineMode should be returned instead of KErrNone.
        // 
        
        if ( !iapTypeLanOrWlan && iSession->IsPhoneOfflineL() )
            {
            ChooseIapComplete( KErrGprsOfflineMode, &iChooseIapPref );
            }
        else
            {
            TWlanIapType wlanType = iCommsDatAccess->CheckWlanL( validateIapId );
            if( wlanType != ENotWlanIap )
                {
                iChooseIapState = EExplicitConnection;
                iWlanDialog = CMPMWlanQueryDialog::NewL( *this, validateIapId );
                iWlanDialog->StartWlanQueryL();
                return;                
                }

            // Check whether confirmation from user is needed for allowing cellular usage.
            // When application is starting an IAP confirmation is asked only when roaming.
            if ( iSession->IsConfirmFirstL( validateIapId ) )
                {
                // Check whether queries are disabled
                if ( !( iChooseIapPref.NoteBehaviour() & TExtendedConnPref::ENoteBehaviourConnDisableQueries ) )
                    {
                    if ( iSession->MyServer().RoamingWatcher()->RoamingStatus() == EMPMInternationalRoaming )
                        {
                        // Check if cellular data usage query has already been presented to the user in this country
                       if ( iSession->MyServer().RoamingWatcher()->AskCellularDataUsageAbroad() == true )
                            {
                            TConnectionId connId = iSession->ConnectionId();
                                                            
                            // International roaming
                            iConfirmDlgStarting = CMPMConfirmDlgStarting::NewL( 
                                                  *this, 
                                                  connId,
                                                  snap, 
                                                  validateIapId, 
                                                  CMPMConfirmDlg::EConfirmDlgVisitorNetwork,
                                                  iChooseIapPref,
                                                  iSession->MyServer(),
                                                  *iSession,
                                                  EExplicitConnection );
                            return;    
                            }
                        }
                    }
                }
                           
            // Add info into the BM connections
            //
            iSession->MyServer().AppendBMConnection( iSession->ConnectionId(), 
                                                     snap, 
                                                     validateIapId, 
                                                     EStarting,
                                                     *iSession );
            ChooseIapComplete( KErrNone, &iChooseIapPref );
            }
        }
    // Snap Id explicitly defined
    // 
    else
        {
        MPMLOGSTRING2(
            "CMPMIapSelection::ExplicitConnectionL - SNAP = %i", snap )

        // If the SNAP contains WLAN IAP, 
        // scan wlan networks
        //
        RArray<TUint> iapPath;
        CleanupClosePushL( iapPath ); 
   
        //Check if WLAN bearerset is in use
        if ( ( iChooseIapPref.BearerSet() & TExtendedConnPref::EExtendedConnBearerWLAN ) ||
             ( iChooseIapPref.BearerSet() == TExtendedConnPref::EExtendedConnBearerUnknown ) )
            {
            if ( iNewWlansAllowed && 
            	   ( iChooseIapPref.BearerSet() == 
            	     TExtendedConnPref::EExtendedConnBearerWLAN ) )
                {
                // User allows only WLAN connections, check WLAN availability.
                // A note will be shown if no WLANs are available.
                //
                iSession->MyServer().Events()->CheckWlanAvailabilityL( this );
                }
            else if( iCommsDatAccess->SnapContainsWlanL( snap, iapPath, KMPMNrWlansOne ) )
                {
                // Scan wlan networks. After that execution continues 
                // in CompleteExplicitSnapConnectionL()
                //
                iChooseIapState = EExplicitConnection; 
                iSession->MyServer().Events()->ScanWLANNetworksL( iSession,
                        iSession->ConnectionId(),
                        EWlanScanCallbackChooseIap );
                }
            else
                {
                CompleteExplicitSnapConnectionL();
                }
            }
        else
            {
            CompleteExplicitSnapConnectionL();
            }
        CleanupStack::PopAndDestroy( &iapPath );
        }        
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::UpdateConnectionDialog
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::UpdateConnectionDialogL()
    {
    if( iDialog )
        {
        MPMLOGSTRING( "CMPMIapSelection::UpdateConnectionDialogL data will be updated" )
        iDialog->PublishSortSnapInfoL();
        }
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::CompleteExplicitSnapConnectionL
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::CompleteExplicitSnapConnectionL()
    {
    MPMLOGSTRING( "CMPMIapSelection::CompleteExplicitSnapConnectionL" )

    // Store Snap id
    // 
    TUint32 snap = iChooseIapPref.SnapId();

    RAvailableIAPList availableIAPList;
    CleanupClosePushL( availableIAPList );
    iSession->AvailableUnblacklistedIapsL( availableIAPList, iSession->ConnectionId() );

    TBool   iapTypeLanOrWlan = EFalse;
    TConnectionId connId = iSession->ConnectionId();

    ChooseBestIAPL( iChooseIapPref, availableIAPList, iNextBestExists );
    TUint32 validateIapId = iChooseIapPref.IapId();
    // Check if any suitable IAP's were found, if not then complete selection with error code
    if ( validateIapId == 0 )
        {
        if ( iChooseIapPref.ConnType() == TMpmConnPref::EConnTypeDefault )
            {
            ImplicitConnectionL();
            }
        else
            {
            ChooseIapComplete( KErrNotFound, NULL );
            }
        CleanupStack::PopAndDestroy( &availableIAPList );
        return;
        }
    
    TUint32 retNetId( 0 );
    iCommsDatAccess->ValidateIapL( connId, 
            validateIapId, 
            retNetId, 
            iapTypeLanOrWlan,
            *iSession );
    
    // Set validated IAP Id and returned Access Network Id
    // in connection preferences
    //
    iChooseIapPref.SetIapId( validateIapId );
    iChooseIapPref.SetNetId( retNetId );

    if ( !CheckGprsServicesAllowedL( iapTypeLanOrWlan ) )
        {
        ChooseIapComplete( KErrGprsServicesNotAllowed, NULL );
        }
    else if ( !iapTypeLanOrWlan && iSession->IsPhoneOfflineL() )
        {
        // In case offline mode is enabled, only LAN or WLAN is allowed.
        // If some other bearer has been requested, then error code 
        // KErrGprsOfflineMode should be returned instead of KErrNone.
        // 
        ChooseIapComplete( KErrGprsOfflineMode, &iChooseIapPref );
        }
    else
        {
        // Check whether confirmation from user is needed for allowing cellular usage
        if ( ( iSession->IsConfirmFirstL( validateIapId ) ) && 
             ( iSession->MyServer().CommsDatAccess()->CheckWlanL( validateIapId ) == ENotWlanIap ) )
            {
            // Check whether queries are disabled
            if ( !( iChooseIapPref.NoteBehaviour() & TExtendedConnPref::ENoteBehaviourConnDisableQueries ) )
                {
                if ( iSession->MyServer().RoamingWatcher()->RoamingStatus() == EMPMInternationalRoaming )
                    {
                    // Check if cellular data usage query has already been presented to the user in this country
                    if ( iSession->MyServer().RoamingWatcher()->AskCellularDataUsageAbroad() == true )
                        {
                        // International roaming
                        iConfirmDlgStarting = CMPMConfirmDlgStarting::NewL( 
                                *this, 
                                connId,
                                snap, 
                                validateIapId, 
                                CMPMConfirmDlg::EConfirmDlgVisitorNetwork,
                                iChooseIapPref,
                                iSession->MyServer(),
                                *iSession,
                                EExplicitConnection );
                        }
                    else
                        {
                        // If user has already been queried in this country just complete the IAP selection.
    
                        iSession->MyServer().AppendBMConnection( connId, 
                                snap, 
                                validateIapId, 
                                EStarting,
                                *iSession );
    
                        ChooseIapComplete( KErrNone, &iChooseIapPref );
                        }
    
                    }
                else
                    {
                    // Home network
                    iConfirmDlgStarting = CMPMConfirmDlgStarting::NewL( 
                            *this, 
                            connId,
                            snap, 
                            validateIapId, 
                            CMPMConfirmDlg::EConfirmDlgHomeNetwork, 
                            iChooseIapPref,
                            iSession->MyServer(),
                            *iSession,
                            EExplicitConnection );
    
                    }
                }
            else
                {
                // Queries are disabled and connection creation will fail
                ChooseIapComplete( KErrPermissionDenied, &iChooseIapPref );
                }
            }
        else
            {
            if ( !StartWlanQueryIfNeededL( validateIapId ) )
                {
                // Add info into the BM connections
                //
                iSession->MyServer().AppendBMConnection( connId, 
                        snap, 
                        validateIapId, 
                        EStarting,
                        *iSession );
                ChooseIapComplete( KErrNone, &iChooseIapPref );

                }

            }
        }

    CleanupStack::PopAndDestroy( &availableIAPList );    
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::CheckGprsServicesAllowedL
// -----------------------------------------------------------------------------
//
TBool CMPMIapSelection::CheckGprsServicesAllowedL( TBool aIapLanOrWlan )
    {
    if ( iSession->MyServer().IsVoiceCallActiveL() )
        {
        // Check whether we are in GSM or 3G mode.
        // Connection request during voice call is always allowed 
        // in 3G. In GSM it's allowed in case phone supports dual 
        // transfer mode (DTM)
        // 
        if ( iSession->MyServer().IsModeGSM() )
            {
            // If phone doesn't support DTM, then 
            // return KErrGprsServicesNotAllow
            // 
            if ( !iSession->MyServer().IsDTMSupported() )
                {
                if ( !aIapLanOrWlan )
                    {
                    MPMLOGSTRING( "CMPMIapSelection::CheckGprsServicesAllowedL not allowed" )
                    return EFalse;
                    }
                }
            }
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::StartWlanQueryIfNeededL
// -----------------------------------------------------------------------------
//
TBool CMPMIapSelection::StartWlanQueryIfNeededL( TUint32 aIapId, TBool aIsRoaming )
    {
    MPMLOGSTRING3( "CMPMIapSelection::StartWlanQueryIfNeededL iap %d, isRoaming %d ", 
                   aIapId,
                   aIsRoaming )
    TBool wlanQueryNeeded( EFalse );
    // If wlan iap check if offline note needed
    TWlanIapType wlanType = iCommsDatAccess->CheckWlanL( aIapId );
    if( wlanType != ENotWlanIap )
        {
        iIsRoaming = aIsRoaming;
        iChooseIapState = EExplicitConnection;
        iWlanDialog = CMPMWlanQueryDialog::NewL( *this, aIapId );
        iWlanDialog->StartWlanQueryL();
        wlanQueryNeeded = ETrue;
        }
    return wlanQueryNeeded;
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::ImplicitConnectionCheckWlanScanNeededL
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::ImplicitConnectionCheckWlanScanNeededL()
    {
    __ASSERT_DEBUG( !iDialog, PanicServer( EMPMReceiveAlreadyActive ) );

    MPMLOGSTRING( "CMPMIapSelection::ImplicitConnectionCheckWlanScanNeededL" )

    iCommsDatAccess->CheckWLANIapL( *iSession );

    // Scan WLAN networks before displaying Connection Dialog in case: 
    //  - there is a WLAN IAP configured in the phone (other than Easy WLAN)
    // 
    if ( iSession->MyServer().IsWLANScanRequired() )
        {
        // Store state of choose iap during 
        // asynchronous calls. 
        // 
        iChooseIapState = EImplicitConnection;
        // In case of offline mode hidden WLAN networks will 
        // not be discovered by this WLAN scan.
        //
        iSession->MyServer().Events()->ScanWLANNetworksL( iSession, 
                                                          iSession->ConnectionId(),
                                                          EWlanScanCallbackChooseIap );
        return;
        }
    ImplicitConnectionL();
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::ChooseIapWLANScanCompletedL
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::ChooseIapWLANScanCompletedL( TInt aStatus )
    {
    MPMLOGSTRING( "CMPMIapSelection::ChooseIapWLANScanCompletedL" )
    if( iChooseIapState == EExplicitConnection )
        {
        MPMLOGSTRING( "CMPMIapSelection::ChooseIapWLANScanCompletedL: explicit " )
        CompleteExplicitSnapConnectionL(  );
        }
    else if( iChooseIapState == EImplicitConnection )
        {
        // If getting available IAPs failed, complete request with error code
        // 
        if( aStatus != KErrNone )
            {
            MPMLOGSTRING2( "CMPMIapSelection::ChooseIapWLANScanCompletedL: implicit, err %d", 
                aStatus )
            ChooseIapComplete( aStatus, NULL ); 
            }
        else
            {
            MPMLOGSTRING( "CMPMIapSelection::ChooseIapWLANScanCompletedL: implicit " )
            ImplicitConnectionL();            
            }
        }
    else
        {
        MPMLOGSTRING( "CMPMIapSelection::ChooseIapWLANScanCompletedL: no state \
to complete connection" )        
        }
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::ChooseIapComplete
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::ChooseIapComplete( 
    TInt                aError,
    const TMpmConnPref* aPolicyPref )
    {
    MPMLOGSTRING2( "CMPMIapSelection::ChooseIapComplete aError = %d", aError )
    
    if ( ( aError == KErrNone ) &&
        !( iChooseIapPref.NoteBehaviour() &
           TExtendedConnPref::ENoteBehaviourConnDisableNotes ) )
        {
        TBool connectionAlreadyActive =
            iSession->MyServer().CheckIfStarted( aPolicyPref->IapId() );
        CConnectionUiUtilities* connUiUtils = NULL;
        TRAPD( popupError,
               connUiUtils = CConnectionUiUtilities::NewL();
               connUiUtils->ConnectingViaDiscreetPopup(
                   aPolicyPref->IapId(),
                   connectionAlreadyActive );
               delete connUiUtils; );
        if ( popupError && connUiUtils )
            {
            delete connUiUtils;
            }
        }
    
    if( iWlanDialog )
        {
        delete iWlanDialog;
        iWlanDialog = NULL;
        }

    iSession->ChooseIapComplete( aError, aPolicyPref );
    // Set choose iap state to none
    iChooseIapState = ENoConnection;
    iNextBestExists = EFalse;
    iUserSelectionSnapId = 0;
    iUserSelectionIapId = 0;
    iImplicitState = EImplicitStart;
    }


// -----------------------------------------------------------------------------
// CMPMIapSelection::UserWlanSelectionDoneL
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::UserWlanSelectionDoneL( TInt aError, TUint32 aIapId )
    {
    MPMLOGSTRING( "CMPMIapSelection::UserWlanSelectionDoneL" )

    if( iIsRoaming )
        {
        iIsRoaming = EFalse;
        iSession->MigrateCallbackL( aError );
        delete iWlanDialog;
        iWlanDialog = NULL;
        }
    else
        {
        if( aError != KErrNone )
            {
            MPMLOGSTRING2( "CMPMIapSelection::UserWlanSelectionDoneL - Error = %d, completing", aError ) 
            ChooseIapComplete( aError, NULL );        
            }
        else
            {
            if( iChooseIapState == EImplicitConnection )
                {
                iUserSelectionIapId = aIapId;
                ImplicitConnectionL();    
                }
            else if( iChooseIapState == EExplicitConnection )
                {
                MPMLOGSTRING( "CMPMIapSelection::UserWlanSelectionDoneL completing explicit iap connection" ) 
                iChooseIapPref.SetIapId( aIapId );
                // Add info into the BM connections
                //
                iSession->MyServer().AppendBMConnection( iSession->ConnectionId(), 
                                                         iChooseIapPref.SnapId(),
                                                         aIapId,
                                                         EStarting,
                                                         *iSession );
                iWlanDialog->StoreEasyWlanSelectionL();
                delete iWlanDialog;
                iWlanDialog = NULL;

                ChooseIapComplete( KErrNone, &iChooseIapPref );
                }
            else
                {
                MPMLOGSTRING( "CMPMIapSelection::UserWlanSelectionDoneL - error, no connection state" ) 
                }
            
            }
        
        }
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::HandleUserSelectionError
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::HandleUserSelectionError( TInt aError  )
    {
    MPMLOGSTRING2( "CMPMIapSelection::HandleUserIapSelectionError - error %d", 
        aError )
    
    ChooseIapComplete( aError, NULL );
    delete iDialog;
    iDialog = NULL;
    }

       
// -----------------------------------------------------------------------------
// CMPMIapSelection::HandleUserIapSelectionL
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::HandleUserSelectionL( TBool aIsIap, TUint32 aId, TInt aError  )
    {
    MPMLOGSTRING2( "CMPMIapSelection::HandleUserSelectionL - Connection Id = 0x%x", 
        iSession->ConnectionId() )

    if( aError != KErrNone )
        {
        HandleUserSelectionError( aError );
        return;
        }
    
    if ( !aIsIap )
        {
        // User selected SNAP
        // 
        TMpmConnPref userPref;
        iUserSelectionSnapId = iCommsDatAccess->MapNetIdtoSnapAPL( aId );
        userPref.SetSnapId( iUserSelectionSnapId );

        
// Agreed that MPM will be migrated to SNAP TAG ID SNAPS.
//        if ( iUserSelectionSnapId < 0x1000 )  // TODO: remove when connection dialog return valid SNAP TAG ID.
//            {
//            // legacy snaps ok
//            userPref.SetNetId( aId );
//            iUserSelectionSnapId = aId;
//            }
//        else
//            {
//            // 5.2 snaps, MPM uses internally legacy snaps. Convert back. -jl- TODO: use CommsDat mapping in future.
//            userPref.SetNetId( aId - 3000 );
//            iUserSelectionSnapId = aId - 3000;
//            }
        
        MPMLOGSTRING2(
                "CMPMIapSelection::HandleUserSelectionL: Snap = %i selected by the User", 
                iUserSelectionSnapId )

//        userPref.SetNetId( aId );
        userPref.SetIapId( 0 );

        ChooseBestIAPL( userPref, iStoredAvailableIaps );
        iUserSelectionIapId = userPref.IapId();

        }
    else
        {
        // User explicit selection of IAP
        // 
        iUserSelectionIapId = aId;
        iUserSelectionSnapId = 0; 
        MPMLOGSTRING2(
                "CMPMIapSelection::HandleUserSelectionL: IAP Id <%i> selected by the User", 
                iUserSelectionIapId )

        }
    
    // We are done
    // 
    delete iDialog;
    iDialog = NULL;
    
    ImplicitConnectionL();
    }
    
// -----------------------------------------------------------------------------
// CMPMIapSelection::ImplicitConnectionWlanNoteL
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::ImplicitConnectionWlanNoteL()
    {
    MPMLOGSTRING( "CMPMIapSelection::ImplicitConnectionWlanNoteL" )
    
    TWlanIapType wlanType = iCommsDatAccess->CheckWlanL( iUserSelectionIapId );
    // In case offline mode is enabled, only LAN or WLAN is allowed.
    // If some other bearer has been requested, then error code 
    // KErrGprsOfflineMode should be returned instead of KErrNone.
    // 
    if( wlanType != ENotWlanIap )
        {
        iChooseIapState = EImplicitConnection;
        iWlanDialog = CMPMWlanQueryDialog::NewL( *this, iUserSelectionIapId );
        iWlanDialog->StartWlanQueryL();
        return;                
        }
    else
        {
        // Check whether confirmation from user is needed for allowing cellular usage.
        // When user is starting implicit IAP/SNAP confirmation is asked only when roaming.
        if ( iSession->IsConfirmFirstL( iUserSelectionIapId ) )
            {
            // Check whether queries are disabled
            if ( !( iChooseIapPref.NoteBehaviour() & TExtendedConnPref::ENoteBehaviourConnDisableQueries ) )
                {
                if ( iSession->MyServer().RoamingWatcher()->RoamingStatus() == EMPMInternationalRoaming )
                    {
                    // Check if cellular data usage query has already been presented to the user in this country
                    if ( iSession->MyServer().RoamingWatcher()->AskCellularDataUsageAbroad() == true )
                        {
                        TConnectionId connId = iSession->ConnectionId();
                                                                    
                        // International roaming
                        iConfirmDlgStarting = CMPMConfirmDlgStarting::NewL( 
                                              *this, 
                                              connId,
                                              iUserSelectionSnapId, 
                                              iUserSelectionIapId, 
                                              CMPMConfirmDlg::EConfirmDlgVisitorNetwork,
                                              iChooseIapPref,
                                              iSession->MyServer(),
                                              *iSession,
                                              EImplicitConnection );
                        return;    
                        }
                    }
                }
            }

        CompleteImplicitConnectionL();
        }
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::OfflineNoteResponse
// -----------------------------------------------------------------------------
//
TOfflineNoteResponse CMPMIapSelection::OfflineNoteResponse()
    {
#ifndef _PLATFORM_SIMULATOR_
    MPMLOGSTRING2( "CMPMIapSelection::OfflineNoteResponse: %d",
                   iOfflineNoteResponse )
    return iOfflineNoteResponse;
#else
    // Platsim simulates WLAN and offline-mode. To ease automated testing,
    // offline connection confirmation is not asked in Platsim-variant
    MPMLOGSTRING( "CMPMIapSelection::OfflineNoteResponse: yes for Platsim" )
    return EOfflineResponseYes;
#endif
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::ConnectionStarted
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::ConnectionStarted()
    {
    MPMLOGSTRING( "CMPMIapSelection::ConnectionStarted: reset offline response" )
    SetOfflineNoteResponse( EOfflineResponseUndefined );
    }
    
// -----------------------------------------------------------------------------
// CMPMIapSelection::SetOfflineNoteShown
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::SetOfflineNoteResponse( TOfflineNoteResponse aResponse )
    {
    MPMLOGSTRING2( "CMPMIapSelection::SetOfflineNoteResponse %d ", aResponse )
    iOfflineNoteResponse = aResponse;
    }
// -----------------------------------------------------------------------------
// CMPMIapSelection::CompleteImplicitConnectionL
// -----------------------------------------------------------------------------
//

void CMPMIapSelection::CompleteImplicitConnectionL()
    {
    TBool   iapTypeLanOrWlan( EFalse );
    TUint32 retNetId( 0 );
    iCommsDatAccess->ValidateIapL( iSession->ConnectionId(), 
                                   iUserSelectionIapId, 
                                   retNetId, 
                                   iapTypeLanOrWlan,
                                   *iSession );
                                   
    if ( !iapTypeLanOrWlan && iSession->IsPhoneOfflineL() )
        {
        MPMLOGSTRING2( "CMPMIapSelection::CompleteImplicitConnectionL: Completing with code = %i",
                KErrGprsOfflineMode )
        ChooseIapComplete( KErrGprsOfflineMode, NULL );
        }
    else
        {
        // Add info into the BM connections
        //
        iSession->MyServer().AppendBMConnection( iSession->ConnectionId(),
                                                 iUserSelectionSnapId,
                                                 iUserSelectionIapId, 
                                                 EStarting,
                                                 *iSession );
                                                 
        // Set validated IAP Id and returned Access Network Id
        // in connection preferences
        //            
        iChooseIapPref.SetIapId( iUserSelectionIapId );
        iChooseIapPref.SetNetId( retNetId );
            
        
        if( iWlanDialog )
            {
            iWlanDialog->StoreEasyWlanSelectionL();
            delete iWlanDialog;
            iWlanDialog = NULL;
            }
        
        ChooseIapComplete( KErrNone, &iChooseIapPref );
        }
    
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::ImplicitConnectionIapSelectionL
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::ImplicitConnectionIapSelectionL()
    {
    MPMLOGSTRING( "CMPMIapSelection::ImplicitConnectionIapSelectionL" )
    iSession->AvailableUnblacklistedIapsL( iStoredAvailableIaps, iSession->ConnectionId() );

    // Create and initiate user dialog
    //
    iDialog = CMPMDialog::NewL( *this,
                                iStoredAvailableIaps,
                                iChooseIapPref.BearerSet(),
                                *iSession->MyServer().ConnectDialogQueue(),
                                iSession->MyServer() );
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::ImplicitConnectionL
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::ImplicitConnectionL()
    {
    if( iImplicitState  == EImplicitStart )
        {
        MPMLOGSTRING( "CMPMIapSelection::ImplicitConnectionIapSelectionL EImplicitStart" )
        iImplicitState = EImplicitWlanScan;
        ImplicitConnectionCheckWlanScanNeededL();
        }
    else if( iImplicitState  == EImplicitWlanScan )
        {
        MPMLOGSTRING( "CMPMIapSelection::ImplicitConnectionIapSelectionL EImplicitWlanScan" )
        iImplicitState = EImplicitUserSelection;
        ImplicitConnectionIapSelectionL();
        }
    else if( iImplicitState  == EImplicitUserSelection )
        {
        MPMLOGSTRING( "CMPMIapSelection::ImplicitConnectionIapSelectionL EImplicitUserSelection" )
        iImplicitState = EImplicitWlanQuery;
        ImplicitConnectionWlanNoteL();
        }
    else //EImplicitWlanQuery
        {
        MPMLOGSTRING( "CMPMIapSelection::ImplicitConnectionIapSelectionL EImplicitWlanQuery" )
        CompleteImplicitConnectionL();
        }
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::StopDisplayingStartingDlg
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::StopDisplayingStartingDlg()
    {
    MPMLOGSTRING( "CMPMIapSelection::StopDisplayingStartingDlg" )

    if ( iConfirmDlgStarting )
        {
        // We probably have to complete the ChooseBestIAP message with KErrCancel
        // 
        ChooseIapComplete( KErrCancel, NULL );
        }
    
    delete iConfirmDlgStarting;
    iConfirmDlgStarting = NULL;
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::ChooseBestIAPL
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::ChooseBestIAPL(
    TMpmConnPref&               aMpmConnPref,
    const RAvailableIAPList&    aAvailableIAPs )
    {
    MPMLOGSTRING( "CMPMIapSelection::ChooseBestIAPL" )
    TBool nextBestExists( EFalse );
    // reuse different version of this function
    //
    ChooseBestIAPL( aMpmConnPref, aAvailableIAPs, nextBestExists ); 
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::ChooseBestIAPL
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::ChooseBestIAPL(
    TMpmConnPref&               aMpmConnPref,
    const RAvailableIAPList&    aAvailableIAPs, 
    TBool&                      aNextBestExists )
    {
    MPMLOGSTRING2( "CMPMIapSelection::ChooseBestIAPL: SNAP: %d", 
        aMpmConnPref.SnapId() )

    RArray<TNetIap> destNetIaps, embeddedIaps;
    CleanupClosePushL( destNetIaps );
    CleanupClosePushL( embeddedIaps );

    // Find the valid IAPs belonging to the Snap.
    //
    iCommsDatAccess->SearchDNEntriesWithEmbeddedL( aMpmConnPref.SnapId(),
                                                   destNetIaps, 
                                                   embeddedIaps );

    // Remove any unavailable IAP from destNetIaps
    // 
    TInt ret        = KErrNotFound;
    TInt destCount  = destNetIaps.Count();
    
    // Decrement by one, because count is n, 
    // but indexes in array are 0 .. n-1.
    // 
    destCount--;

    // This time we are browsing the array from the end to the beginning, 
    // because removing one element from array affects index numbering.
    // 
    for ( TInt k = destCount; k >= 0; k-- )
        {
        ret = aAvailableIAPs.Find( destNetIaps[k].iIapId );
        if ( ret == KErrNotFound )
            {
            MPMLOGSTRING2( "CMPMIapSelection::ChooseBestIAPL: \
Remove unavailable IAP = %i", destNetIaps[k].iIapId )
            destNetIaps.Remove( k );
            }
        }

    // Remove any unavailable IAP from embeddedIaps
    // 
    if ( embeddedIaps.Count() > 0 )
        {
        TInt embedCount = embeddedIaps.Count();
        embedCount--;

        for ( TInt m = embedCount; m >= 0; m-- )
            {
            ret = aAvailableIAPs.Find( embeddedIaps[m].iIapId );
            if ( ret == KErrNotFound )
                {
                // Remove IapId because it's not available
                // 
                MPMLOGSTRING2( "CMPMIapSelection::ChooseBestIAPL: \
Remove unavailable IAP = %i", embeddedIaps[m].iIapId )
                              embeddedIaps.Remove( m );
                }
            }
        }

    // Determine the actual priorities for virtual IAPs and embedded Snaps
    // 
    iCommsDatAccess->DeterminePrioritiesL( destNetIaps, aAvailableIAPs, 
                                           *iSession );
    if ( embeddedIaps.Count() > 0 )
        {
        iCommsDatAccess->DeterminePrioritiesL( embeddedIaps, aAvailableIAPs,
                                               *iSession );
        }

    // Start checking if a matching available IAP is found.
    // 
    // Pick the first item from both arrays and check the global bearer 
    // priority for both destNetIaps and embeddedIaps.
    // 
    TUint32 destNetPriority( KLowestPriority );
    TUint32 embeddedPriority( KLowestPriority );

    if( destNetIaps.Count() > 0 )
        {
        iCommsDatAccess->GlobalBearerPriorityL( destNetIaps[0].iIapId, 
                               destNetPriority );
        }

    if( embeddedIaps.Count() > 0 )
        {
        iCommsDatAccess->GlobalBearerPriorityL( embeddedIaps[0].iIapId, 
                               embeddedPriority );
        }

    // Compare the global bearer priorities of the selected IAPs and 
    // select the better. 
    // 
    // If the priorities are equal, prioritize the Snap 
    // over the embedded Snap.
    // 
    // When comparing the priorities, smaller value is better.
    // 
    // Set found IAP Id and Access Network Id
    // in connection preferences
    //
    if( destNetPriority <= embeddedPriority )
        {
        if( destNetIaps.Count() > 0 )
            {
            aMpmConnPref.SetIapId( destNetIaps[0].iIapId );
            aMpmConnPref.SetSnapId( destNetIaps[0].iSnap );

            MPMLOGSTRING2(
                "CMPMIapSelection::ChooseBestIAPL: Found IapId = %i", 
                destNetIaps[0].iIapId )
            MPMLOGSTRING2(
                "CMPMIapSelection::ChooseBestIAPL: Found SNAP  = %i", 
                destNetIaps[0].iSnap )
            }
        }
    else
        {
        if( embeddedIaps.Count() > 0 )
            {
            aMpmConnPref.SetIapId( embeddedIaps[0].iIapId );
            aMpmConnPref.SetSnapId( embeddedIaps[0].iSnap );

            MPMLOGSTRING2(
                "CMPMIapSelection::ChooseBestIAPL: Found IapId = %i", 
                embeddedIaps[0].iIapId )
            MPMLOGSTRING2(
                "CMPMIapSelection::ChooseBestIAPL: Found SNAP  = %i", 
                embeddedIaps[0].iSnap )
            }
        }

    if( ( destNetIaps.Count() + embeddedIaps.Count() ) > 1 )
        {
        MPMLOGSTRING(
            "CMPMIapSelection::ChooseBestIAPL: aNextBestExists = ETrue" )
        aNextBestExists = ETrue;
        }
    else
        {
        MPMLOGSTRING(
            "CMPMIapSelection::ChooseBestIAPL: aNextBestExists = EFalse" )
        aNextBestExists = EFalse;
        }

    if ( iSession->UseUserConnPref() &&
         iSession->MyServer().UserConnectionInInternet() )
        {
        MPMLOGSTRING3(
            "CMPMIapSelection::ChooseBestIAPL: Change iap id %d to %d\
 when user connection exists",
            aMpmConnPref.IapId(),
            iSession->MyServer().UserConnPref()->IapId() )
        aMpmConnPref.SetIapId( iSession->MyServer().UserConnPref()->IapId() );
        }
    
    // Release memory
    //
    CleanupStack::PopAndDestroy( &embeddedIaps );
    CleanupStack::PopAndDestroy( &destNetIaps );
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::SetConfirmDlgStartingPtrNull
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::SetConfirmDlgStartingPtrNull()
    {
    iConfirmDlgStarting = NULL;
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::Session
// -----------------------------------------------------------------------------
//
CMPMServerSession* CMPMIapSelection::Session()
    {
    return iSession;
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::TriggerInformationNote
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::TriggerInformationNoteL()
    {
    // ConnectioUiUtilities client side has a nonblocking active object 
    // implementation
    //
    if ( !( iChooseIapPref.NoteBehaviour() &
           TExtendedConnPref::ENoteBehaviourConnDisableNotes ) )
        {           	
        CConnectionUiUtilities* connUiUtils = CConnectionUiUtilities::NewL();        
                            
        connUiUtils->NoWLANNetworksAvailableNote();
                            
        delete connUiUtils;
        }            
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::WlanAvailabilityResponse
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::WlanAvailabilityResponse( const TInt  aError, 
                                                 const TBool aResult )
    {
    if ( ( aError == KErrNone && aResult == EFalse )
          || ( aError == KErrNotSupported ) )
        {
        // no WLANs are available and user allows only
        // WLAN connections
        TRAPD( err, TriggerInformationNoteL() );
        
        if ( err )
            {
            MPMLOGSTRING2( "TriggerInformationNoteL leaved %d", err )
            }
            
        ChooseIapComplete( KErrNotFound, NULL );
        }
    else
        {
        // Some WLANs are available,
        // or an error has occured while requesting available WLANs.
        //
        TRAPD( err, CompleteExplicitSnapConnectionL() );
        
        if ( err )
            {
            MPMLOGSTRING2( "CompleteExplicitSnapConnectionL leaved %d", err )
            ChooseIapComplete( KErrCancel, NULL );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::MpmConnPref
// -----------------------------------------------------------------------------
//
TMpmConnPref CMPMIapSelection::MpmConnPref()
    {
    return iChooseIapPref;
    }

