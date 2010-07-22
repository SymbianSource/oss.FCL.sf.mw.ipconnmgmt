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
#include "mpmserversession.h"
#include "mpmcommsdataccess.h"
#include "mpmconnmonevents.h"
#include "mpmconfirmdlgstarting.h"
#include "extendedconnpref.h"

// ---------------------------------------------------------------------------
// CMPMIapSelection::CMPMIapSelection
// ---------------------------------------------------------------------------
//
CMPMIapSelection::CMPMIapSelection( CMPMCommsDatAccess*  aCommsDatAccess,
                                    CMPMServerSession*   aSession,
                                    CConnectionUiUtilities* aConnUiUtils )
    : iChooseIapState( ENoConnection ),
      iCommsDatAccess( aCommsDatAccess ),
      iConnUiUtils( aConnUiUtils ),
      iStoredIapInfo(),
      iSession( aSession ),
      iConfirmDlgStarting( NULL ),
      iNextBestExists( EFalse ),
      iUserSelectionIapId( 0 ),
      iUserSelectionSnapId( 0 ),
      iImplicitState( EImplicitStart ),
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
                                          CMPMServerSession*   aSession,
                                          CConnectionUiUtilities* aConnUiUtils )
    {
    CMPMIapSelection* self = new ( ELeave ) CMPMIapSelection( aCommsDatAccess,
                                                              aSession,
                                                              aConnUiUtils );
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
    
    delete iConfirmDlgStarting;
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
        if ( iChooseIapPref.ConnType() == TMpmConnPref::EConnTypeImplicit )
            {
            // No Connection selection dialog on reselection.
            iChooseIapPref.SetConnType( TMpmConnPref::EConnTypeExplicit );
            }
        }
    else if ( iap ) 
        {      
        MPMLOGSTRING2( "CMPMIapSelection::ChooseIapL: Set iap %i to preferences",
                iap )
        iChooseIapPref.SetIapId( iap );
        iChooseIapPref.SetSnapId( 0 );
        if ( iChooseIapPref.ConnType() == TMpmConnPref::EConnTypeImplicit )
            {
            // No Connection selection dialog on reselection.
            iChooseIapPref.SetConnType( TMpmConnPref::EConnTypeExplicit );
            }
        }

    MPMLOGSTRING3( "CMPMIapSelection::ChooseIapL: IapID: %i SnapId: %i",
            iChooseIapPref.IapId(), iChooseIapPref.SnapId() )
    
    // Update WLAN only information and whether new WLAN network usage is allowed.
    TBool wlanOnly = iSession->IsWlanOnlyL( iNewWlansAllowed );
    
    // Check if direct IAP connection is tried to make
    if ( iChooseIapPref.IapId() != 0 )
        { 
        TMPMBearerType bearerType = iSession->MyServer().CommsDatAccess()->GetBearerTypeL( iChooseIapPref.IapId() );
        MPMLOGSTRING2( "CMPMIapSelection::ChooseIapL: bearerType: %i", bearerType )

        // Complete selection with error code if wlan only was set and cellular IAP other 
        // than MMS IAP was tried to access  
        if ( wlanOnly && 
                ( bearerType == EMPMBearerTypePacketData ) && 
                ( iSession->IsMMSIap( iChooseIapPref.IapId() ) == EFalse ) ) 
            {            
            ChooseIapComplete( KErrPermissionDenied, NULL );
            return;
            }
        }
        
           
    if  ( iChooseIapPref.ConnSelectionDialog() )
        {
        // Complete selection with error code if wlan only was set and bearer set as cellular 
        if ( wlanOnly && 
             iChooseIapPref.BearerSet() == TExtendedConnPref::EExtendedConnBearerCellular ) 
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

        // Read the default connection values
        TCmDefConnType type( TCmDefConnType( 0 ) );
        TUint32 id( 0 );
        iCommsDatAccess->GetDefaultConnectionL( type, id );
        
        MPMLOGSTRING3( "CMPMIapSelection::ChooseIapL:\
            default connection type %d id %d", type, id )
        
        // set the received snap id and open the connection
        iChooseIapPref.SetSnapId( id );
        ExplicitConnectionL();
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

        if ( !iapTypeLanOrWlan &&                  
             iSession->MyServer().RoamingWatcher()->RoamingStatus() == EMPMRoamingStatusUnknown )
            {
            ChooseIapComplete( KErrGprsOfflineMode, &iChooseIapPref );
            }
        else
            {
            TWlanIapType wlanType = iCommsDatAccess->CheckWlanL( validateIapId );
            if( wlanType != ENotWlanIap )
                {
                // Add info into the BM connections
                iSession->MyServer().AppendBMConnection( iSession->ConnectionId(), 
                                    iChooseIapPref.SnapId(),
                                    validateIapId,
                                    EStarting,
                                    *iSession );
                ChooseIapComplete( KErrNone, &iChooseIapPref );
                return;                
                }

            // Check whether confirmation from user is needed for allowing cellular usage.
            // When application is starting an IAP confirmation is asked only when roaming.
            // This is skipped for VPN (= virtual) IAPs
            if ( iSession->IsConfirmFirstL( validateIapId ) &&
                 !( iSession->MyServer().CommsDatAccess()->IsVirtualIapL( validateIapId ) ))
                {
            
                // Check whether queries are enabled
                if ( !( iChooseIapPref.NoteBehaviour()
                        & TExtendedConnPref::ENoteBehaviourConnDisableQueries ) )
                    {
                
                    TConnectionId connId = iSession->ConnectionId();

                    // Set confirmation type based on roaming status
                    CMPMConfirmDlg::TDialogType type = 
                            CMPMConfirmDlg::EConfirmDlgHomeNetwork;
                    if ( iSession->MyServer().RoamingWatcher()->RoamingStatus()
                            == EMPMInternationalRoaming ) {
                        type = CMPMConfirmDlg::EConfirmDlgVisitorNetwork;
                    }
                    
                    iConfirmDlgStarting = CMPMConfirmDlgStarting::NewL( 
                        *this, 
                        connId,
                        snap, 
                        validateIapId, 
                        type,
                        iChooseIapPref,
                        iSession->MyServer(),
                        *iSession,
                        EExplicitConnection );
                    return;    
                    }
                else
                    {
                    // Queries disabled, connection must fail
                    ChooseIapComplete( KErrPermissionDenied, &iChooseIapPref );
                    return;
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
            if( iCommsDatAccess->SnapContainsWlanL( snap, iapPath, KMPMNrWlansOne ) )
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
        if ( !( iChooseIapPref.NoteBehaviour() & TExtendedConnPref::ENoteBehaviourConnDisableQueries ) &&
                ( iChooseIapPref.ConnType() == TMpmConnPref::EConnTypeDefault ||
                ( iChooseIapPref.ConnType() == TMpmConnPref::EConnTypeExplicit &&
                  iCommsDatAccess->IsInternetSnapL( 0, snap ) ) ) )
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
    else if ( !iapTypeLanOrWlan &&                 
               iSession->MyServer().RoamingWatcher()->RoamingStatus() == EMPMRoamingStatusUnknown )
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
		// This is skipped for VPN (= virtual) IAPs
        if ( ( iSession->IsConfirmFirstL( validateIapId ) ) && 
             ( iSession->MyServer().CommsDatAccess()->CheckWlanL( validateIapId ) == ENotWlanIap ) &&
             !( iSession->MyServer().CommsDatAccess()->IsVirtualIapL( validateIapId ) ) )
            {
            // Check whether queries are disabled
            if ( !( iChooseIapPref.NoteBehaviour() & TExtendedConnPref::ENoteBehaviourConnDisableQueries ) )
                {
                if ( iSession->MyServer().RoamingWatcher()->RoamingStatus() == EMPMInternationalRoaming )
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
// CMPMIapSelection::IsIapWlanL
// -----------------------------------------------------------------------------
//
TBool CMPMIapSelection::IsIapWlanL( TUint32 aIapId )
    {
    MPMLOGSTRING2( "CMPMIapSelection::StartWlanQueryIfNeededL iap %d, ", aIapId)
    TWlanIapType wlanType = iCommsDatAccess->CheckWlanL( aIapId );
    if( wlanType != EWlanIap )
        {
        return EFalse;
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CMPMIapSelection::ImplicitConnectionCheckWlanScanNeededL
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::ImplicitConnectionCheckWlanScanNeededL()
    {
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

    iSession->ChooseIapComplete( aError, aPolicyPref );
    // Set choose iap state to none
    iChooseIapState = ENoConnection;
    iNextBestExists = EFalse;
    iUserSelectionSnapId = 0;
    iUserSelectionIapId = 0;
    iImplicitState = EImplicitStart;
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
        iUserSelectionSnapId = aId;
        userPref.SetSnapId( iUserSelectionSnapId );
        userPref.SetIapId( 0 );

        MPMLOGSTRING2(
                "CMPMIapSelection::HandleUserSelectionL: Snap = %i selected by the User", 
                iUserSelectionSnapId )


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
    
    // Letting a function leave here would panic the whole MPM.
    //
    TRAPD( err, ImplicitConnectionL() );
    if( err != KErrNone )
        {
        ChooseIapComplete( err, &iChooseIapPref );
        }
    }
    
// -----------------------------------------------------------------------------
// CMPMIapSelection::ImplicitConnectionWlanNoteL
// -----------------------------------------------------------------------------
//
void CMPMIapSelection::ImplicitConnectionWlanNoteL()
    {
    MPMLOGSTRING( "CMPMIapSelection::ImplicitConnectionWlanNoteL" )
    
    TWlanIapType wlanType = iCommsDatAccess->CheckWlanL( iUserSelectionIapId );

    if( wlanType != ENotWlanIap )
        {
        ImplicitConnectionL();
        return;                
        }
    else
        {
        // Check whether confirmation from user is needed for allowing cellular usage.
        // When user is starting implicit IAP/SNAP confirmation is asked only when roaming.
		// This is skipped for VPN (= virtual) IAPs
        if ( iSession->IsConfirmFirstL( iUserSelectionIapId ) &&
             !( iSession->MyServer().CommsDatAccess()->IsVirtualIapL( iUserSelectionIapId ) ) )
            {
            // Check if we are roaming and cellular data usage query has not yet been presented
            // to the user in this country
            if ( iSession->MyServer().RoamingWatcher()->RoamingStatus() == EMPMInternationalRoaming )
                {
                // Check whether queries are enabled
                if ( !( iChooseIapPref.NoteBehaviour() & TExtendedConnPref::ENoteBehaviourConnDisableQueries ) )
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
                else
                    {
                    // Queries disabled, connection must fail
                    ChooseIapComplete( KErrPermissionDenied, &iChooseIapPref );
                    return;
                    }
                }
            }

        CompleteImplicitConnectionL();
        }
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
                                   
    if ( !iapTypeLanOrWlan &&            
            iSession->MyServer().RoamingWatcher()->RoamingStatus() == EMPMRoamingStatusUnknown )
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

    // The connection selection dialog doesn't exist any longer, thus
    // the HandleUserSelection is called with the Internet SNAP that is returned instead
    TInt err = KErrNone;
    TInt internetSnapId = 0;
    // if the reading leaves, then the internet snap did not exist for some reason
    TRAP ( err, internetSnapId = 
        iCommsDatAccess->DestinationIdL( CMManager::ESnapPurposeInternet ));
    HandleUserSelectionL( false, internetSnapId, err );
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
// CMPMIapSelection::MpmConnPref
// -----------------------------------------------------------------------------
//
TMpmConnPref CMPMIapSelection::MpmConnPref()
    {
    return iChooseIapPref;
    }

