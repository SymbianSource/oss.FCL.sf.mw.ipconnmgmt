/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Core functionality for ConnMon.
*
*/

#include <mmtsy_names.h>
#include <featmgr.h>
#include <rmmcustomapi.h>
#include <utf.h>
#include <es_sock_partner.h>

#include "ConnMonServ.h"
#include "ConnMonSess.h"
#include "CEventQueue.h"
#include "CCsdFax.h"
#include "CPsdFax.h"
#include "ConnMonIAP.h"
#include "ConnMonNoti.h"
#include "CDataVolume.h"
#include "connmondtmnoti.h"
#include "connmontelnoti.h"
#include "ConnMonBearerNotifier.h"
#include "log.h"
#include "cellulardatausagekeyupdater.h"
#include "connmonasyncstopdaemon.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TAsyncStopQueueElement::TAsyncStopQueueElement
// -----------------------------------------------------------------------------
//
TAsyncStopQueueElement::TAsyncStopQueueElement( const RMessage2& aMessage )
        :
        iMessage( aMessage ),
        iStatus( 0 ),
        iIdCount( 0 ),
        iStopCount( 0 )
    {
    }

// -----------------------------------------------------------------------------
// TAsyncStopQueueElement::Add
// Add a connection ID to the ID array. Does nothing if the array is already full. 
// -----------------------------------------------------------------------------
//
void TAsyncStopQueueElement::Add( TUint aConnectionId )
    {
    if ( iIdCount < KMaxConnectionCount )
        {
        iId[iIdCount] = aConnectionId;
        iIdCount++;
        }
    }

// -----------------------------------------------------------------------------
// TConnInfo::TConnInfo
// -----------------------------------------------------------------------------
//
TConnInfo::TConnInfo(
        TInt aIapId,
        TInt aNetId,
        TUint aConnectionId,
        TInt aBearer,
        TBearerInfo aBearerInfo )
        :
        iIapId( aIapId ),
        iNetId( aNetId ),
        iConnectionId( aConnectionId ),
        iBearer( aBearer ),
        iConnAttach( 0 ),
        iProgressNotifier( 0 ),
        iDLDataNotifier( 0 ),
        iULDataNotifier( 0 ),
        iDataVolumeAO( 0 ),
        iConnDownNotifier( 0 ),
        iActivityNotifier( 0 ),
        iActivity( 0 ),
        iBearerInfo( aBearerInfo ),
        iBearerNotifier( 0 ),
        iAsyncStopStatus( EConnMonAsyncStopNotStarted )
    {
    }

// -----------------------------------------------------------------------------
// TConnInfo::Reset
// -----------------------------------------------------------------------------
//
void TConnInfo::Reset()
    {
    // Only call this function to initialize a newly created TConnInfo-object.
    // (There is no cleanup)
    iIapId            = 0;
    iNetId            = 0;
    iConnectionId     = 0;
    iBearer           = EBearerUnknown;
    iBearerInfo.iBearer = EBearerInfoUnknown;
    iBearerInfo.iInternal = ETrue;
    iProgressNotifier = 0;
    iDLDataNotifier   = 0;
    iULDataNotifier   = 0;
    iDataVolumeAO     = 0;
    iConnAttach       = 0;
    iConnDownNotifier = 0;
    iActivityNotifier = 0;
    iActivity         = 0;
    iBearerNotifier   = 0;
    iAsyncStopStatus  = EConnMonAsyncStopNotStarted;
    }

// -----------------------------------------------------------------------------
// TConnInfo::Compare
// Compares two TConnInfo items (by connection id).
// -----------------------------------------------------------------------------
//
TInt TConnInfo::Compare( const TConnInfo& aFirst, const TConnInfo& aSecond )
    {
    if ( aFirst.iConnectionId < aSecond.iConnectionId )
        {
        return -1;
        }

    if ( aFirst.iConnectionId > aSecond.iConnectionId )
        {
        return 1;
        }
    return 0;
    }

// -----------------------------------------------------------------------------
// TConnInfo::MatchId
// Finds TConnInfo item matching by connection Id.
// -----------------------------------------------------------------------------
//
TBool TConnInfo::MatchId( const TConnInfo& aFirst, const TConnInfo& aSecond )
    {
    if ( aFirst.iConnectionId == aSecond.iConnectionId )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TConnInfo::MatchIap
// Finds TConnInfo item matching by iap id and net id.
// -----------------------------------------------------------------------------
//
TBool TConnInfo::MatchIap( const TConnInfo& aFirst, const TConnInfo& aSecond )
    {
    // Only internal bearers can be matched by IapId and NetId
    if ( ( aFirst.iIapId == aSecond.iIapId ) && ( aFirst.iNetId == aSecond.iNetId ) &&
         ( aFirst.iBearer < EBearerExternalCSD ) && ( aSecond.iBearer < EBearerExternalCSD ) &&
         ( aFirst.iBearerInfo.iInternal ) && ( aSecond.iBearerInfo.iInternal ) )
        {
        return ETrue;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// TConnInfo::MatchBearer
// Finds TConnInfo item matching by bearer.
// -----------------------------------------------------------------------------
//
TBool TConnInfo::MatchBearer( const TConnInfo& aFirst, const TConnInfo& aSecond )
    {
    // External CSD bearers (there is no event availabe for bearer change)
    if ( ( aFirst.iBearer == EBearerExternalCSD || aFirst.iBearer == EBearerExternalHSCSD ||
           aFirst.iBearer == EBearerExternalWcdmaCSD )
         &&
         ( aSecond.iBearer == EBearerExternalCSD || aSecond.iBearer == EBearerExternalHSCSD ||
           aSecond.iBearer == EBearerExternalWcdmaCSD ) )
        {
        return ETrue;
        }

    // External PSD bearers (there is not always an event availabe for bearer change)
    else if ( ( aFirst.iBearer == EBearerExternalGPRS     ||
                aFirst.iBearer == EBearerExternalEdgeGPRS ||
                aFirst.iBearer == EBearerExternalWCDMA    ||
                aFirst.iBearer == EBearerExternalCDMA2000 ||
                ( !aFirst.iBearerInfo.iInternal &&
                  ( aFirst.iBearerInfo.iBearer == EBearerInfoHSDPA ||
                    aFirst.iBearerInfo.iBearer == EBearerInfoHSUPA ||
                    aFirst.iBearerInfo.iBearer == EBearerInfoHSxPA ) ) )
              &&
              ( aSecond.iBearer == EBearerExternalGPRS     ||
                aSecond.iBearer == EBearerExternalEdgeGPRS ||
                aSecond.iBearer == EBearerExternalWCDMA    ||
                aSecond.iBearer == EBearerExternalCDMA2000 ||
                ( !aSecond.iBearerInfo.iInternal &&
                  ( aSecond.iBearerInfo.iBearer == EBearerInfoHSDPA ||
                    aSecond.iBearerInfo.iBearer == EBearerInfoHSUPA ||
                    aSecond.iBearerInfo.iBearer == EBearerInfoHSxPA ) ) ) )

        {
        return ETrue;
        }

    // Other bearers
    else if ( aFirst.iBearer == aSecond.iBearer &&
            aFirst.iBearerInfo.iBearer == aSecond.iBearerInfo.iBearer )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonIAP::CConnMonIAP
// -----------------------------------------------------------------------------
//
CConnMonIAP::CConnMonIAP( CConnMonServer* aServer )
        :
        iServer( aServer ),
        iEndUserNetIds( KNullDesC8 )
    {
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::ConstructL
// -----------------------------------------------------------------------------
//
void CConnMonIAP::ConstructL()
    {
    LOGENTRFN("CConnMonIAP::ConstructL()")
    TInt err( KErrNone );

    // Connect to socket server and use system global pool (-1) for message slots.
    User::LeaveIfError( iSocketServer.Connect( static_cast<TUint>( -1 ) ) );
    LOGIT("(01/11) Loaded OK: ESock")

    // Open RConnection. This object is not attached to any connection
    User::LeaveIfError( iConnection.Open( iSocketServer ) );
    LOGIT("(02/11) Opened OK: RConnection")

    // Connect to ETel server
    TBuf<KCommsDbSvrMaxColumnNameLength> tsyName;
    tsyName.Copy( KMmTsyModuleName );

    // The RTelServer::Connect() might not always work with the first trial,
    // because of a coding error related to using semaphores in the method.
    for ( TInt a = 0; a < KPhoneRetryCount; a++ )
        {
        err = iTelServer.Connect();
        if ( KErrNone == err )
            {
            break;
            }
        User::After( KPhoneRetryTimeout );
        LOGIT("RTelServer::Connect() failed, trying again.")
        }
    User::LeaveIfError( err );
    LOGIT("(03/11) Loaded OK: ETel")

    // Initialize FeatureManager
    FeatureManager::InitializeLibL();
    LOGIT("(04/11) Loaded OK: FeatureManager")

    // Try to load tsy
    err = iTelServer.LoadPhoneModule( tsyName );
    if ( KErrNone == err )
        {
        LOGIT("(05/11) Loaded OK: TSY")
        iTSYLoaded = 1;

        // Phone info can be retrieved with value 0 if there is only one phone
        RTelServer::TPhoneInfo info;
        User::LeaveIfError( iTelServer.GetPhoneInfo( 0, info ) );
        LOGIT("(06/11) Loaded OK: Phone info")

        User::LeaveIfError( iMobilePhone.Open( iTelServer, info.iName ) );
        LOGIT("(07/11) Opened OK: RMobilePhone")

        // Open packet service
        err = iPacketService.Open( iMobilePhone );
        if ( KErrNone == err )
            {
            LOGIT("(08/11) Loaded OK: Packet service")
            iPacketServLoaded = 1;

            // CDMA network does not support external connections. PSD Fax handler module
            // should not be created when in CDMA network. The code below works OK as long
            // as there is no phone that supports both CDMA and GSM at the same time.
            if ( !FeatureManager::FeatureSupported( KFeatureIdProtocolCdma ) )
                {
                // Create PSD Fax handler module
                iPsdFax = new( ELeave ) CPsdFax(
                        iServer,
                        &iTelServer,
                        &iMobilePhone,
                        &iPacketService );
                iPsdFax->ConstructL();
                LOGIT("(09/11) Loaded OK: PSD module")
                }
            else
                {
                LOGIT("(09/11) Nosupport: PSD module not loaded, no PSD support in CDMA networks.")
                }
            }
        else
            {
            LOGIT1("(08/11) Loaderror: Packet service failed with code <%d>", err)
            }

        // CDMA network does not support external connections. CSD Fax handler module
        // should not be created when in CDMA network. The code below works OK as long
        // as there is no phone that supports both CDMA and GSM at the same time.
        if ( !FeatureManager::FeatureSupported( KFeatureIdProtocolCdma ) )
            {
            // Create CSD Fax handler module
            iCsdFax = new( ELeave ) CCsdFax( iServer, &iTelServer, &iMobilePhone );
            iCsdFax->ConstructL();
            LOGIT("(10/11) Loaded OK: CSD module")
            }
        else
            {
            LOGIT("(10/11) Nosupport: CSD module not loaded, no CSD support in CDMA networks")
            }
        }
    else
        {
        LOGIT1("(05/11) Loaderror: TSY failed with code <%d>", err)
        }

    // Open WLAN Management service
    if ( FeatureManager::FeatureSupported( KFeatureIdProtocolWlan ) )
        {
        TRAPD( leaveCode, iWlanSupport = CWlanSupport::NewL( iServer ) );
        if ( KErrNone != leaveCode )
            {
            LOGIT1("(11/11) Loaderror: WLAN module failed with code <%d>", leaveCode)
            }
        else
            {
            LOGIT("(11/11) Loaded OK: WLAN module")
            }
        }
    else
        {
        LOGIT("(11/11) Nosupport: WLAN module not loaded, no WLAN support detected")
        }

    LOGEXITFN("CConnMonIAP::ConstructL()")
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::~CConnMonIAP
// -----------------------------------------------------------------------------
//
// Destructor
CConnMonIAP::~CConnMonIAP()
    {
    LOGENTRFN("CConnMonIAP::~CConnMonIAP()")
            
    iAsyncStopQueue.Close();
    
    // Delele dial up modules.
    delete iCsdFax;
    iCsdFax = NULL;

    delete iPsdFax;
    iPsdFax = NULL;

    // Delete global notifications. This must be called before
    // services are closed or destroyed below.
    DeleteNotifications();

    // Delete connection up/down notification.
    if ( iConnUpDownNotifier != 0 )
        {
        iConnUpDownNotifier->Cancel();
        delete iConnUpDownNotifier;
        }

    // Close all attached connections
    TInt count = iConnInfos.Count();

    for ( TInt i = 0; i < count; i++ )
        {
        if ( iConnInfos[i].iConnAttach != 0 )
            {
            iConnInfos[i].iConnAttach->Close();
            delete iConnInfos[i].iConnAttach;
            }
        }

    // Note. all sessions have closed by now.
    // There is no need to delete connection parameters
    // because session destructors have already done that.
    iConnInfos.Close();

    // Close "global" ESOCK objects
    iConnection.Close();
    iSocketServer.Close();

    // Close "global" ETEL objects
    if ( iPacketServLoaded == 1 )
        {
        iPacketService.Close();
        }

    iMobilePhone.Close();
    iTelServer.Close();

    // Unload FeatureManager
    FeatureManager::UnInitializeLib();

    // Destroy WLAN service
    delete iWlanSupport;
    iWlanSupport = NULL;

    // Reset global pointers
    iServer                        = NULL;
    iPsdNetwStatusNotifier         = NULL;
    iNetwRegistrationNotifier      = NULL;
    iEdgeNotifier                  = NULL;
    iWcdmaNotifier                 = NULL;
    iModeNotifier                  = NULL;
    iGSMSignalNotifier             = NULL;
    iGSMBearerAvailabilityNotifier = NULL;

    // Clean up all ECom sessions
    // More info about it: TSW EGUO-64LD36
    REComSession::FinalClose();
    LOGEXITFN("CConnMonIAP::~CConnMonIAP()")
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::EnumerateConnections
// Enumerates active connections using RConnection API.
// Refreshes iConnInfos array (server view) with the latest information.
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::EnumerateConnections( TUint& aCount )
    {
    // Server must be 'blocked' while updating the connection table.
    TBearerInfo bearerInfo;
    TConnInfo connInfo( 0, 0, 0, 0, bearerInfo );
    TInt err( KErrNone );

    aCount = 0;
    RArray<TConnInfo> tempArray;

    // Get existing external (fax) CSD connections
    // These can't be enumerated from ETEL at the moment
    if ( iCsdFax )
        {
        // In practise there can be only ONE fax connection of this type
        connInfo.Reset();
        connInfo.iBearer = EBearerExternalCSD; // This will match any ext-CSD
        connInfo.iBearerInfo.iBearer = EBearerInfoCSD;
        connInfo.iBearerInfo.iInternal = EFalse;

        TInt index = iConnInfos.Find( connInfo, TConnInfo::MatchBearer );
        if ( ( KErrNotFound != index ) && iCsdFax->IsValid() )
            {
            // Add to temp array
            tempArray.Append( iConnInfos[index] );
            aCount++;
            }
        else if ( !iConnUpDownNotifier )
            {
            // Try to find out about external CSD connections
            // that were alive before CM server started.
            // The code below will be executed only when
            // this method is called for the first time.
            err = iCsdFax->FindCall();
            if ( KErrNone == err )
                {
                iCsdFax->GetBearer( connInfo.iBearer, connInfo.iBearerInfo );
                connInfo.iConnectionId = NewConnectionId(); // New unique Id
                iCsdFax->SetConnectionId( connInfo.iConnectionId );
                tempArray.Append( connInfo );
                aCount++;
                }
            }
        }

    // Get existing external (fax) PSD connections
    // These connections can't be enumerated from ETEL at the moment
    // Only notification of a new connection is available from ETEL.
    if ( iPsdFax )
        {
        connInfo.Reset();
        connInfo.iBearer = EBearerExternalGPRS; // This will match any ext-PSD
        connInfo.iBearerInfo.iBearer = EBearerInfoGPRS;
        connInfo.iBearerInfo.iInternal = EFalse;

        // Check all external PSD connections
        for ( TUint index = 0; index < iConnInfos.Count(); index++ )
            {
            // Check that connection bearer is external PSD
            if ( iConnInfos[index].iBearer == EBearerExternalGPRS       ||
                 iConnInfos[index].iBearer == EBearerExternalEdgeGPRS   ||
                 iConnInfos[index].iBearer == EBearerExternalWCDMA      ||
                 iConnInfos[index].iBearer == EBearerExternalCDMA2000   ||
                 ( !iConnInfos[index].iBearerInfo.iInternal &&
                   ( iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSDPA ||
                     iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSUPA ||
                     iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSxPA ) ) )
                {
                // Check if connection is still valid
                if ( iPsdFax->IsValid( iConnInfos[index].iConnectionId ) )
                    {
                    // Add to temp array
                    tempArray.Append( iConnInfos[index] );
                    aCount++;
                    }
                }
            }

        if ( !iConnUpDownNotifier )
            {
            for( TUint index = 0; index < KMaxPsdConnectionCount; index++ )
                {
                // Try to find out about external PSD connections
                // that were alive before CM server started.
                // The code below will be executed only when
                // this method is called for the first time.
                TBuf<KMaxName> tempName( KExternalName );

                // Add connection number to string if index is > 1
                if ( index > 0 )
                    {
                    tempName.AppendNum( index+1 );
                    }

                // Try to open connection context
                err = iPsdFax->OpenContext( index, tempName );
                if ( KErrNone == err )
                    {
                    iPsdFax->GetBearer( connInfo.iBearer, connInfo.iBearerInfo );
                    connInfo.iConnectionId = NewConnectionId(); // New unique Id
                    err = iPsdFax->SetConnectionId( index, connInfo.iConnectionId );
                    if ( KErrNone == err )
                        {
                        tempArray.Append( connInfo );
                        aCount++;
                        }
                    }
                }
            }
        }

    const TUint initialCount = iConnInfos.Count();
    __ASSERT_DEBUG( ( aCount == static_cast<TUint>( tempArray.Count() ) ),
            PanicServer( EIapArrayError ) );

    TUint k( 0 );
    // Remove resources owned by obsolete connections
    for ( k = 0; k < initialCount; k++ )
        {
        TInt index( KErrNotFound );

        if ( iConnInfos[k].iBearer < EBearerExternalCSD )
            {
            index = tempArray.Find( iConnInfos[k], TConnInfo::MatchIap ); // Internal
            }
        else
            {
            index = tempArray.Find( iConnInfos[k], TConnInfo::MatchBearer ); // External
            }

        if ( KErrNotFound == index )
            {
            TBool deleteSent( ETrue );

            if ( iConnInfos[k].iConnDownNotifier != 0 )
                {
                deleteSent = iConnInfos[k].iConnDownNotifier->DeleteSent();
                }

            TInt connStatus( 0 );
            TInt ret = GetConnectionStatus( k, connStatus );

            if ( ( ( KErrNone != ret ) || ( connStatus == 0 ) ) && deleteSent )
                {
                LOGIT3("Removing old connection, index %d, id %d, bearer %d",
                        k, iConnInfos[k].iConnectionId, iConnInfos[k].iBearer)
                iServer->RemoveConnSettingsFromSessions( iConnInfos[k].iConnectionId );
                DeleteConnNotifications( k );

                // Close attached connection
                if ( iConnInfos[k].iConnAttach != 0 )
                    {
                    iConnInfos[k].iConnAttach->Close();
                    delete iConnInfos[k].iConnAttach;
                    }
                }
            else
                {
                // Add connection to temp array because connection status is valid.
                tempArray.Append( iConnInfos[k] );
                aCount++;
                }
            }
        }

    // Replace old iConnInfos array with tempArray
    // Reserve enough room
    for ( TUint i = initialCount; i < aCount; i++ )
        {
        TInt error = iConnInfos.Append( tempArray[0] );
        if ( error )
            {
            // Rollback reserved entries
            for ( TUint j = i; j > initialCount; j-- )
                {
                iConnInfos.Remove( j-1 );
                }
            tempArray.Reset();
            tempArray.Close();
            return error;
            }
        }

    // Safe from leaves from here. Copy over new contents
    iConnInfos.Reset();
    for ( k = 0; k < aCount; k++ )
        {
        iConnInfos.Append( tempArray[k] );
        }

    // Print connection info array to log
    #ifdef _DEBUG
    LOGIT("EnumerateConnections: Connection info array:")
    for ( TInt m = 0; m < iConnInfos.Count(); m++ )
        {
        LOGIT4("    id %d, bearer %d, iapId %d, netId %d", iConnInfos[m].iConnectionId,
                iConnInfos[m].iBearer, iConnInfos[m].iIapId, iConnInfos[m].iNetId)
        }
    #endif // _DEBUG

    tempArray.Reset();
    tempArray.Close();

    err = KErrNone;
    // Start connection up/down notifier if not already running
    if ( !iConnUpDownNotifier )
        {
        // Leave left out intentionally, check for NULL instead
        iConnUpDownNotifier = new CConnUpDownNotifier( iServer ); // No (ELeave)
        if ( !iConnUpDownNotifier )
            {
            err = KErrNoMemory;
            }
        else
            {
            iConnUpDownNotifier->Construct();
            }
        }
    if ( KErrNone == err )
        {
        if ( !iConnUpDownNotifier->IsActive() )
            {
            iConnUpDownNotifier->Receive(); // (re)start listening
            }
        // Add event watchers for the new connections. Old connections already have
        // notifications on and AddConnNotificationsL() will only restart the event
        // watchers if they have stopped (because of an error situation).
        if ( iCountListeners > 0 )
            {
            TRAP( err,
                    {
                    for ( k = 0; k < aCount; k++ )
                        {
                        AddConnNotificationsL( k );
                        }
                    } );
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::EnumerateConnectionsL
// Enumerates active connections using RConnection API.
// Refreshes iConnInfos array (server view) with the latest information.
// Fills aEnumInfo array (client view) with the newly enumerated connection ids.
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::EnumerateConnectionsL( TUint& aCount, CArrayFixFlat<TUint>& aEnumInfo )
    {
    LOGENTRFN("CConnMonIAP::EnumerateConnectionsL()")
    TInt err( KErrNone );

    err = EnumerateConnections( aCount );
    if ( KErrNone == err )
        {
        // Fill in table (connection ids) owned by the session
        // Client needs this table when querying connection info index by index.
        aEnumInfo.Reset();

        if ( aCount > 0 )
            {
            for ( TUint k = 0; k < aCount; k++ )
                {
                // Sometimes there is a delay after the DELETE event has arrived and
                // before the closed connection is removed from Esock. Hide these closing
                // connections from clients.
                TBool deleteSent( EFalse );

                if ( iConnInfos[k].iConnDownNotifier != 0 )
                    {
                    deleteSent = iConnInfos[k].iConnDownNotifier->DeleteSent();
                    }

                if ( !deleteSent )
                    {
                    aEnumInfo.AppendL( iConnInfos[k].iConnectionId );
                    }
                }
            aCount = aEnumInfo.Count();
            }
        }

    LOGEXITFN1("CConnMonIAP::EnumerateConnectionsL()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::ConnectionExists
// Checks if given connection id is valid
// -----------------------------------------------------------------------------
//
TBool CConnMonIAP::ConnectionExists( const TUint& aConnectionId )
    {
    LOGENTRFN("CConnMonIAP::ConnectionExists()")
    TBool res( EFalse );

    if ( aConnectionId == 0 || aConnectionId >= KMaxConnectionId )
        {
        res = EFalse;
        }
    else
        {
        TInt index = Index( aConnectionId );
        if ( index < 0 )
            {
            res = EFalse;
            }
        else
            {
            res = ETrue;
            }
        }

    LOGEXITFN1("CConnMonIAP::ConnectionExists()", res)
    return res;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetConnectionInfo
// Returns the number of subconnections in aSubConnectionCount
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetConnectionInfo(
        const TUint& aConnectionId,
        TUint& aSubConnectionCount )
    {
    LOGENTRFN("CConnMonIAP::GetConnectionInfo()")
    TInt err( KErrNone );

    // Find connection matching the given Id
    TInt index = Index( aConnectionId );
    if ( index < 0  )
        {
        err = KErrNotFound;
        }
    else
        {
        // Get the number of subconnections
        if ( iConnInfos[index].iBearer < EBearerExternalCSD )
            {
            // Internal connections
            aSubConnectionCount = 1;
            err = iConnInfos[index].iConnAttach->EnumerateSubConnections(
                    aSubConnectionCount );
            if ( KErrNotReady == err )
                {
                err = KErrNone;
                }
            }
        else
            {
            // External connections
            aSubConnectionCount = 1;
            }
        }

    LOGEXITFN1("CConnMonIAP::GetConnectionInfo()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetIntAttributeL
// Returns the integer attribute in aValue
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetIntAttributeL( const RMessage2& aMessage, TInt& aValue )
    {
    LOGENTRFN("CConnMonIAP::GetIntAttributeL()")
    TInt ret( KErrNone );
    TUint connectionId( aMessage.Int0() );
    TUint attribute( aMessage.Int2() );

    LOGIT2("SERVER: CConnMonIAP::GetIntAttributeL(): connId %d, attribute %d", connectionId, attribute)

    if ( KBearerInfo == attribute )
        {
        LOGIT("SERVER: CConnMonIAP::GetIntAttributeL(): KBearerInfo")
        TBearerInfo bearerInfo;
        ret = GetBearerInfo( connectionId, bearerInfo );
        if ( KErrNone == ret )
            {
            aValue = bearerInfo.iBearer;
            }
        return ret;
        }

    if ( KWlanScanCacheLifetime == attribute )
        {
        // Check WLAN support for the given id
        ret = WlanSupportCheck( connectionId );
        if ( KErrNone == ret )
            {
            ret = iWlanSupport->GetIntAttributeL( aMessage, aValue );
            }
        return ret;
        }

    // Handle requests that use a bearer specific connection IDs
    if ( ( connectionId == EBearerIdGPRS || connectionId == EBearerIdWCDMA ) &&
         ( attribute == KNetworkStatus ) )
        {
        return ( GetPsdNetworkStatus( aValue ) );
        }
    else if ( ( connectionId == EBearerIdGPRS || connectionId == EBearerIdWCDMA ||
                connectionId == EBearerIdCSD || connectionId == EBearerIdGSM ||
                connectionId == EBearerIdWcdmaCSD )
                && ( attribute == KNetworkRegistration ) )
        {
        return ( GetNetworkRegistration( aValue ) );
        }
    else if ( ( connectionId == EBearerIdGPRS || connectionId == EBearerIdWCDMA ||
                connectionId == EBearerIdCSD || connectionId == EBearerIdGSM ||
                connectionId == EBearerIdWcdmaCSD )
                && ( attribute == KNetworkRegistration_v2 ) )
        {
        return ( GetNetworkRegistration_v2( aValue ) );
        }
    else if ( ( connectionId == EBearerIdGPRS || connectionId == EBearerIdWCDMA ||
                connectionId == EBearerIdCSD || connectionId == EBearerIdGSM ||
                connectionId == EBearerIdWcdmaCSD )
                && ( attribute == KSignalStrength ) )
        {
        return ( GetGSMSignalStrength( aValue ) );
        }
    else if ( connectionId == EBearerIdWLAN && attribute == KSignalStrength )
        {
        if ( iWlanSupport )
            {
            ret = iWlanSupport->GetSignalStrengthL( aMessage, aValue );
            }
        else
            {
            ret = KErrNotSupported;
            }
        return ret;
        }
    else if ( ( connectionId == EBearerIdGPRS ) && ( attribute == KBearer ) )
        {
        // Check which is the bearer of the GPRS cell.
        if ( IsEdgeCell() )
            {
            aValue = EBearerEdgeGPRS;
            }
        else
            {
            aValue = EBearerGPRS;
            }
        return KErrNone;
        }

    // Find connection matching the given Id
    TInt index = Index( connectionId );
    if ( index < 0  )
        {
        return KErrNotFound;
        }

    LOGIT1("SERVER: CConnMonIAP::GetIntAttributeL(): index %d", index)

    // Find out bearer if not already set.
    if ( iConnInfos[index].iBearer == EBearerUnknown )
        {
        ret = GetBearer(
                connectionId,
                iConnInfos[index].iBearer,
                iConnInfos[index].iBearerInfo );

        if ( ret != KErrNone )
            {
            LOGIT1("SERVER: CConnMonIAP::GetIntAttributeL(): Error getting bearer info for connection <%d>", ret)
            return ret;
            }
        LOGIT1("SERVER: CConnMonIAP::GetIntAttributeL(): bearer was unknown, now %d", iConnInfos[index].iBearer)
        }

    switch ( attribute )
        {
        case KBearer:
            {
            // BEARER
            LOGIT1("SERVER: CConnMonIAP::GetIntAttributeL(): case KBearer, bearer %d", iConnInfos[index].iBearer)
            // Bearer is never unknown for the external connections
            // but sometimes it is not valid (e.g. CSD/HSCSD) until
            // connection reaches certain status.
            if ( ( iConnInfos[index].iBearer == EBearerExternalCSD ) ||
                 ( iConnInfos[index].iBearer == EBearerExternalHSCSD ) ||
                 ( iConnInfos[index].iBearer == EBearerExternalWcdmaCSD ) )
                {
                if ( iCsdFax )
                    {
                    iCsdFax->GetBearer( iConnInfos[index].iBearer, iConnInfos[index].iBearerInfo );
                    }
                }
            else if ( ( iConnInfos[index].iBearer == EBearerExternalGPRS ) ||
                      ( iConnInfos[index].iBearer == EBearerExternalEdgeGPRS ) ||
                      ( iConnInfos[index].iBearer == EBearerExternalWCDMA ) )
                {
                if ( iPsdFax )
                    {
                    iPsdFax->GetBearer( iConnInfos[index].iBearer, iConnInfos[index].iBearerInfo );
                    }
                }
            else if ( iConnInfos[index].iBearer < EBearerExternalCSD )
                {
                // CSD/HSCSD info could change after the first query
                // also GPRS/EdgeGPRS/WCDMA info can change
                TInt bearer( EBearerUnknown );
                TBearerInfo bearerInfo;

                ret = GetBearer( connectionId, bearer, bearerInfo );
                if ( iConnInfos[index].iBearer != bearer )
                    {
                    LOGIT4("Bearer changed: id %d, iapId %d, old bearer %d, new bearer %d",
                            iConnInfos[index].iConnectionId, iConnInfos[index].iIapId,
                            iConnInfos[index].iBearer, bearer)
                    }

                if ( ( bearer != EBearerUnknown && bearerInfo.iBearer != EBearerInfoUnknown )
                     && ( ret == KErrNone ) )
                    {
                    iConnInfos[index].iBearer = bearer;
                    iConnInfos[index].iBearerInfo = bearerInfo;
                    }
                }
            aValue = iConnInfos[index].iBearer;
            }
            break;

        case KNetworkStatus:
            {
            // NETWORK STATUS
            if ( iConnInfos[index].iBearer == EBearerGPRS ||
                 iConnInfos[index].iBearer == EBearerEdgeGPRS ||
                 iConnInfos[index].iBearer == EBearerWCDMA ||
                 iConnInfos[index].iBearer == EBearerCDMA2000 ||
                 iConnInfos[index].iBearer == EBearerExternalGPRS ||
                 iConnInfos[index].iBearer == EBearerExternalEdgeGPRS ||
                 iConnInfos[index].iBearer == EBearerExternalWCDMA ||
                 iConnInfos[index].iBearer == EBearerExternalCDMA2000 )
                {
                // psd
                ret = GetPsdNetworkStatus( aValue );
                }
            else if ( iConnInfos[index].iBearer == EBearerUnknown )
                {
                 // GetBearer() : iMobilePhone.GetCurrentMode()
                 // is not always working for PSD when starting the first
                 // connection. So this must be some PSD...
                ret = GetPsdNetworkStatus( aValue );
                }
            else
                {
                // Others not supported at the moment
                aValue = EConnMonStatusNotAvailable;
                ret = KErrNone;
                }
            }
            break;

        case KConnectionStatus:
            {
            // CONNECTION STATUS
            if ( iConnInfos[index].iBearer < EBearerExternalCSD )
                {
                // Internal connections
                ret = GetConnectionStatus( index, aValue );
                }
            else if ( iConnInfos[index].iBearer == EBearerExternalCSD ||
                      iConnInfos[index].iBearer == EBearerExternalHSCSD ||
                      iConnInfos[index].iBearer == EBearerExternalWcdmaCSD )
                {
                // External csd
                if ( iCsdFax )
                    {
                    ret = iCsdFax->GetStatus( aValue );
                    }
                else
                    {
                    ret = KErrNotFound;
                    }
                }
            else
                {
                // External psd
                if ( iPsdFax )
                    {
                    ret = iPsdFax->GetStatus( connectionId, aValue );
                    }
                else
                    {
                    ret = KErrNotFound;
                    }
                }
            }
            break;

        case KProtocolType:
            {
            // PROTOCOL TYPE
            if ( iConnInfos[index].iBearer == EBearerGPRS ||
                 iConnInfos[index].iBearer == EBearerEdgeGPRS ||
                 iConnInfos[index].iBearer == EBearerWCDMA )
                {
                ret = GetProtocolTypeL( connectionId, aValue );
                }
            else if ( iConnInfos[index].iBearer == EBearerExternalGPRS ||
                      iConnInfos[index].iBearer == EBearerExternalEdgeGPRS ||
                      iConnInfos[index].iBearer == EBearerExternalWCDMA )
                {
                if ( iPsdFax )
                    {
                    ret = iPsdFax->GetProtocolType( connectionId, aValue );
                    }
                else
                    {
                    ret = KErrNotFound;
                    }
                }
            else
                {
                aValue = EProtocolTypeUnknown;
                }
            }
            break;

        case KNetworkRegistration:
            {
            // NETWORK REGISTRATION STATUS
            if ( iConnInfos[index].iBearer == EBearerGPRS ||
                 iConnInfos[index].iBearer == EBearerEdgeGPRS ||
                 iConnInfos[index].iBearer == EBearerWCDMA ||
                 iConnInfos[index].iBearer == EBearerCSD ||
                 iConnInfos[index].iBearer == EBearerHSCSD ||
                 iConnInfos[index].iBearer == EBearerWcdmaCSD ||
                 iConnInfos[index].iBearer == EBearerExternalGPRS ||
                 iConnInfos[index].iBearer == EBearerExternalEdgeGPRS ||
                 iConnInfos[index].iBearer == EBearerExternalWCDMA ||
                 iConnInfos[index].iBearer == EBearerExternalCSD ||
                 iConnInfos[index].iBearer == EBearerExternalHSCSD ||
                 iConnInfos[index].iBearer == EBearerExternalWcdmaCSD )
                {
                ret = GetNetworkRegistration( aValue );
                }
            else
                {
                aValue = ENetworkRegistrationNotAvailable;
                ret = KErrNone;
                }
            }
            break;
        case KNetworkRegistration_v2:
            {
            // NETWORK REGISTRATION STATUS
            if ( iConnInfos[index].iBearer == EBearerGPRS ||
                 iConnInfos[index].iBearer == EBearerEdgeGPRS ||
                 iConnInfos[index].iBearer == EBearerWCDMA ||
                 iConnInfos[index].iBearer == EBearerCSD ||
                 iConnInfos[index].iBearer == EBearerHSCSD ||
                 iConnInfos[index].iBearer == EBearerWcdmaCSD ||
                 iConnInfos[index].iBearer == EBearerExternalGPRS ||
                 iConnInfos[index].iBearer == EBearerExternalEdgeGPRS ||
                 iConnInfos[index].iBearer == EBearerExternalWCDMA ||
                 iConnInfos[index].iBearer == EBearerExternalCSD ||
                 iConnInfos[index].iBearer == EBearerExternalHSCSD ||
                 iConnInfos[index].iBearer == EBearerExternalWcdmaCSD )
                {
                ret = GetNetworkRegistration_v2( aValue );
                }
            else
                {
                aValue = ENetworkRegistrationNotAvailable;
                ret = KErrNone;
                }
            }
            break;
        case KSignalStrength:
            {
            // SIGNAL STRENGTH
            ret = KErrNotSupported;
            if ( iConnInfos[index].iBearer == EBearerGPRS ||
                 iConnInfos[index].iBearer == EBearerEdgeGPRS ||
                 iConnInfos[index].iBearer == EBearerWCDMA ||
                 iConnInfos[index].iBearer == EBearerCSD ||
                 iConnInfos[index].iBearer == EBearerHSCSD ||
                 iConnInfos[index].iBearer == EBearerWcdmaCSD ||
                 iConnInfos[index].iBearer == EBearerExternalGPRS ||
                 iConnInfos[index].iBearer == EBearerExternalEdgeGPRS ||
                 iConnInfos[index].iBearer == EBearerExternalWCDMA ||
                 iConnInfos[index].iBearer == EBearerExternalCSD ||
                 iConnInfos[index].iBearer == EBearerExternalHSCSD ||
                 iConnInfos[index].iBearer == EBearerExternalWcdmaCSD )
                {
                ret = GetGSMSignalStrength( aValue );
                }
            else if ( iConnInfos[index].iBearer == EBearerWLAN )
                {
                if ( iWlanSupport )
                    {
                    ret = iWlanSupport->GetSignalStrengthL( aMessage, aValue );
                    }
                }
            }
            break;

        case KNetworkMode:
            // NETWORK MODE
            ret = KErrNotSupported;
            if ( iConnInfos[index].iBearer == EBearerWLAN )
                {
                if ( iWlanSupport )
                    {
                    ret = iWlanSupport->GetConnectionMode( aValue );
                    }
                }
            break;

        case KSecurityMode:
            // SECURITY MODE
            ret = KErrNotSupported;
            if ( iConnInfos[index].iBearer == EBearerWLAN )
                {
                if ( iWlanSupport )
                    {
                    TInt secModeV2( EConnMonSecurityV2Open );
                    ret = iWlanSupport->GetConnectionSecurity( secModeV2 );
                    aValue = iWlanSupport->ConvertConMonSecModeV2ToConnMonSecMode( secModeV2 );
                    }
                }
            break;

        case KSecurityMode_v2:
            // SECURITY MODE V2
            ret = KErrNotSupported;
            if ( iConnInfos[index].iBearer == EBearerWLAN )
                {
                if ( iWlanSupport )
                    {
                    ret = iWlanSupport->GetConnectionSecurity( aValue );
                    }
                }
            break;

        case KTrafficClass:
            ret = KErrNotSupported;
            break;
        case KDeliveryOrder:
            ret = KErrNotSupported;
            break;
        case KErroneousSDUDelivery:
            ret = KErrNotSupported;
            break;
        case KResidualBitErrorRatio:
            ret = KErrNotSupported;
            break;
        case KSDUErrorRatio:
            ret = KErrNotSupported;
            break;
        default:
            ret = KErrArgument;
            break;
        }

    LOGEXITFN1("CConnMonIAP::GetIntAttributeL()", ret)
    return ret;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetBearerInfo
// Returns the integer attribute in aValue
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetBearerInfo( const TUint aConnectionId, TBearerInfo& aBearerInfo )
    {
    LOGENTRFN("CConnMonIAP::GetBearerInfo()")
    TInt err( KErrNone );
    TBool getBearerCalled( EFalse );

    // Find connection matching the given Id
    TInt index = Index( aConnectionId );
    if ( index < 0  )
        {
        err = KErrNotFound;
        }
    else
        {
        // Find out bearer if not already set. This can be only with internal
        // connections.
        if ( iConnInfos[index].iBearerInfo.iBearer == EBearerInfoUnknown )
            {
            LOGIT("GetBearerInfo: EBearerInfoUnknown")
            err = GetBearer(
                    aConnectionId,
                    iConnInfos[index].iBearer,
                    iConnInfos[index].iBearerInfo );
            getBearerCalled = ETrue;
            }
        if ( KErrNone == err )
            {
            // Bearer is never unknown for the external connections but
            // sometimes it is not valid (e.g. CSD/HSCSD) until connection
            // reaches certain status.
            if ( !iConnInfos[index].iBearerInfo.iInternal &&
                    ( iConnInfos[index].iBearerInfo.iBearer == EBearerInfoCSD ||
                    iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSCSD ||
                    iConnInfos[index].iBearerInfo.iBearer == EBearerInfoWcdmaCSD ) )
                {
                if ( iCsdFax )
                    {
                    LOGIT("GetBearerInfo: case iCsdFax")
                    err = iCsdFax->GetBearer(
                            iConnInfos[index].iBearer,
                            iConnInfos[index].iBearerInfo );
                    }
                }
            else if ( !iConnInfos[index].iBearerInfo.iInternal &&
                    ( iConnInfos[index].iBearerInfo.iBearer == EBearerInfoGPRS ||
                    iConnInfos[index].iBearerInfo.iBearer == EBearerInfoEdgeGPRS ||
                    iConnInfos[index].iBearerInfo.iBearer == EBearerInfoWCDMA ||
                    iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSDPA ||
                    iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSUPA ||
                    iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSxPA ) )
                {
                if ( iPsdFax )
                    {
                    LOGIT("GetBearerInfo: case iPsdFax")
                    err = iPsdFax->GetBearer(
                            iConnInfos[index].iBearer,
                            iConnInfos[index].iBearerInfo );
                    }
                }
            else if ( iConnInfos[index].iBearerInfo.iInternal &&
                    ( iConnInfos[index].iBearerInfo.iBearer == EBearerInfoCSD ||
                    iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSCSD ||
                    iConnInfos[index].iBearerInfo.iBearer == EBearerInfoGPRS ||
                    iConnInfos[index].iBearerInfo.iBearer == EBearerInfoEdgeGPRS ||
                    iConnInfos[index].iBearerInfo.iBearer == EBearerInfoWCDMA ||
                    iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSDPA ||
                    iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSUPA ||
                    iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSxPA ) )
                {
                // If GetBearer() was just called, no need to do it again.
                if ( !getBearerCalled )
                    {
                    // CSD/HSCSD info could change after the first query also
                    // GPRS/EdgeGPRS/WCDMA info can change.
                    TInt bearer( EBearerUnknown );
                    TBearerInfo bearerInfo;

                    LOGIT("GetBearerInfo: GetBearer() 2nd query")
                    err = GetBearer( aConnectionId, bearer, bearerInfo );
                    if ( ( KErrNone == err ) && ( bearerInfo.iBearer != EBearerInfoUnknown ) )
                        {
                        iConnInfos[index].iBearer = bearer;
                        iConnInfos[index].iBearerInfo = bearerInfo;
                        }
                    }
                }
            }
        if ( KErrNone == err )
            {
            aBearerInfo.iBearer = iConnInfos[index].iBearerInfo.iBearer;
            aBearerInfo.iInternal = iConnInfos[index].iBearerInfo.iInternal;
            LOGIT2("GetBearerInfo: result: bearer %d, internal %d", aBearerInfo.iBearer, aBearerInfo.iInternal)
            }
        }

    LOGEXITFN1("CConnMonIAP::GetBearerInfo()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetUintAttributeL
// Returns the unsigned integer attribute in aValue
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetUintAttributeL( const RMessage2& aMessage, TUint& aValue )
    {
    LOGENTRFN("CConnMonIAP::GetUintAttributeL()")
    TInt err( KErrNone );
    TUint connectionId( aMessage.Int0() );
    TUint attribute( aMessage.Int2() );

    LOGIT2("SERVER: CConnMonIAP::GetUintAttributeL: connId %d, attribute %d", connectionId, attribute)

    // Find connection matching the given ID
    TInt index = Index( connectionId );
    if ( index < 0 )
        {
        LOGEXITFN1("CConnMonIAP::GetUintAttributeL()", KErrNotFound)
        return KErrNotFound;
        }

    switch ( attribute )
        {
        case KIAPId:
            {
            err = KErrNotSupported;
            if ( iConnInfos[index].iBearer < EBearerExternalCSD &&
                    iConnInfos[index].iBearerInfo.iInternal )
                {
                err = KErrNone;
                aValue = iConnInfos[index].iIapId;
                LOGIT1("GetUintAttributeL: iap id %d", aValue)
                }
            }
            break;

        case KNetworkIdentifier:
            {
            err = KErrNotSupported;
            if ( iConnInfos[index].iBearer < EBearerExternalCSD &&
                    iConnInfos[index].iBearerInfo.iInternal )
                {
                err = KErrNone;
                aValue = iConnInfos[index].iNetId;
                LOGIT1("GetUintAttributeL: net id %d", aValue)
                }
            }
            break;

        case KDownlinkData:
        case KUplinkData:
            {
            if ( iConnInfos[index].iBearer < EBearerExternalCSD &&
                    iConnInfos[index].iBearerInfo.iInternal )
                {
                // INTERNAL
                // Query data volumes asyncronously
                if ( !iConnInfos[index].iDataVolumeAO )
                    {
                    // Create an active object and add it to scheduler
                    iConnInfos[index].iDataVolumeAO = new( ELeave ) CDataVolume(
                            iServer,
                            iConnInfos[index].iConnAttach,
                            iConnInfos[index].iConnectionId );

                    iConnInfos[index].iDataVolumeAO->Construct();
                    }
                if ( !iConnInfos[index].iDataVolumeAO->IsActive() )
                    {
                    iConnInfos[index].iDataVolumeAO->Receive(); // start a new request
                    }
                // Don't complete this request yet
                err = KRequestPending;
                }

            else if ( iConnInfos[index].iBearer == EBearerExternalGPRS     ||
                      iConnInfos[index].iBearer == EBearerExternalEdgeGPRS ||
                      iConnInfos[index].iBearer == EBearerExternalWCDMA    ||
                      iConnInfos[index].iBearer == EBearerExternalCDMA2000 ||
                      ( !iConnInfos[index].iBearerInfo.iInternal &&
                        ( iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSDPA ||
                          iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSUPA ||
                          iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSxPA ) ) )
                {
                // EXTERNAL PSD
                if ( iPsdFax )
                    {
                    TUint down( 0 );
                    TUint up( 0 );
                    err = iPsdFax->GetDataVolumes( connectionId, down, up );
                    if ( KErrNone == err )
                        {
                        if ( attribute == KDownlinkData )
                            {
                            aValue = down;
                            }
                        else
                            {
                            aValue = up;
                            }
                        }
                    }
                else
                    {
                    err = KErrNotFound;
                    }
                }

            else
                {
                // EXTERNAL CSD
                err = KErrNotSupported;
                }
            }
            break;

        case KTransmitPower:
            {
            err = KErrNotSupported;
            if ( iConnInfos[index].iBearer == EBearerWLAN && iWlanSupport )
                {
                err = iWlanSupport->GetTransmitPower( aValue );
                LOGIT1("GetUintAttributeL: transmit power %d", aValue)
                }
            }
            break;

        case KMaximumBitrateDownlink:
        case KMaximumBitrateUplink:
        case KMaximumSDUSize:
        case KTransferDelay:
        case KGuaranteedBitrateDownlink:
        case KGuaranteedBitrateUplink:
        case KTrafficHandlingPriority:
            err = KErrNotSupported;
            break;

        default:
            err = KErrArgument;
            break;
        }

    LOGEXITFN1("CConnMonIAP::GetUintAttributeL()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CConnMonIAP::GetBoolAttributeL
// Returns the TBool attribute in aValue
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetBoolAttributeL( const RMessage2& aMessage, TBool& aValue )
    {
    LOGENTRFN("CConnMonIAP::GetBoolAttributeL()")
    TInt err( KErrNone );
    TUint connectionId( aMessage.Int0() );
    TUint attribute( aMessage.Int2() );

    switch ( attribute )
        {
        case KConnectionActive:
            {
            // Find connection matching the given ID
            TInt index = Index( connectionId );
            if ( index < 0  )
                {
                LOGIT("GetBoolAttributeL: connection id not found")
                err = KErrNotFound;
                break;
                }
            if ( iConnInfos[index].iBearer < EBearerExternalCSD &&
                    iConnInfos[index].iBearerInfo.iInternal )
                {
                TInt connectionStatus( 0 );
                GetConnectionStatus( index, connectionStatus );
                LOGIT1("GetBoolAttributeL: connection status %d", connectionStatus)

                if ( connectionStatus != KLinkLayerOpen )
                    {
                    aValue = EFalse;
                    break;
                    }
                if ( ( iCountListeners > 0 ) && ( iConnInfos[index].iActivityNotifier != 0 ) )
                    {
                    if ( iConnInfos[index].iActivityNotifier->IsActive() )
                        {
                        // Notifications are ON and are updating the activity info.
                        // Return the info immediately from internal table
                        if ( iConnInfos[index].iActivity == 0 )
                            {
                            aValue = EFalse;
                            }
                        else
                            {
                            aValue = ETrue;
                            }
                        }
                    else
                        {
                        GetActivityOneShotL( index );
                        err = KRequestPending;
                        }
                    }
                else
                    {
                    // Notifications are OFF
                    GetActivityOneShotL( index );
                    err = KRequestPending;
                    }
                }
            else
                {
                err = KErrNotSupported;
                }
            }
            break;

        case KBearerAvailability:
            {
            TUint bearer( EBearerIdAll );
            err = BearerIdForBearerAvailability( connectionId, bearer );
            if ( KErrNone == err )
                {
                if ( bearer == EBearerIdWLAN )
                    {
                    if ( iWlanSupport )
                        {
                        err = iWlanSupport->GetBearerAvailabilityL( aMessage, aValue );
                        }
                    else
                        {
                        err = KErrNotSupported;
                        }
                    }
                else
                    {
                    TBool byCell( EFalse );
                    TBool byPhone( EFalse );
                    err = GetBearerSupportInfo( bearer, byCell, byPhone );
                    if ( byCell && byPhone )
                        {
                        aValue = ETrue;
                        }
                    else
                        {
                        aValue = EFalse;
                        }
                    }
                }
            }
            break;

        case KPacketDataAvailability:
            {
            if ( connectionId != EBearerIdGPRS && connectionId != EBearerIdWCDMA )
                {
                err = KErrNotSupported;
                }
            else
                {
                // When not in dual mode and call is active, gprs and wcdma bearers are unavailable
                // Note that even the WCDMA can be unavailable, if the phone doesn't support DTM
                if ( iTelephonyNotifier && iDualTransferModeNotifier &&
                        iTelephonyNotifier->IsCallActive() &&
                        !iDualTransferModeNotifier->IsInDualMode() )
                    {
                    LOGIT("SERVER: Packet data UNAVAILABLE (2G&3G), no DTM and call is active")
                    aValue = EFalse;
                    }
                // If DTM status says available, lets check the availability of the bearer.
                // This is done because the DTM status remains as it was if the psd link is broken.
                // So there might be a situation where the notifier says we are in dual mode even
                // though the whole bearer is unavailable
                else
                    {
                    TBool byPhone( EFalse );
                    TBool byCell( EFalse );
                    err = GetBearerSupportInfo( connectionId, byCell, byPhone );
                    LOGIT4("Bearer support: bearer %d, byCell %d, byPhone %d, err <%d>", connectionId, byCell, byPhone, err)
                    aValue = ETrue;
                    if ( KErrNone == err )
                        {
                        if ( byCell && byPhone )
                            {
                            LOGIT("SERVER: Packet data AVAILABLE, DTM or no call is active")
                            }
                        else
                            {
                            LOGIT("SERVER: Packet data UNAVAILABLE, no bearer")
                            aValue = EFalse;
                            }
                        }
                    }
                }
            }
            break;

        default:
            // Unknown attribute
            err = KErrArgument;
            break;
        }

    LOGEXITFN1("CConnMonIAP::GetBoolAttributeL()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetStringAttribute
// Returns the TDes attribute in aValue
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetStringAttribute( const RMessage2& aMessage, TDes& aValue )
    {
    LOGENTRFN("CConnMonIAP::GetStringAttribute()")
    TInt err( KErrNone );
    TUint connectionId( aMessage.Int0() );
    TUint attribute( aMessage.Int2() );

    // Find connection matching the given ID
    TInt index = Index( connectionId );
    if ( index < 0  )
        {
        LOGEXITFN1("CConnMonIAP::GetStringAttribute()", KErrNotFound)
        return KErrNotFound;
        }

    // Find out bearer if not already set
    if ( iConnInfos[index].iBearer == EBearerUnknown )
        {
        err = GetBearer( connectionId, iConnInfos[index].iBearer, iConnInfos[index].iBearerInfo );
        if ( KErrNone != err )
            {
            LOGEXITFN1("CConnMonIAP::GetStringAttribute()", err)
            return err;
            }
        }

    switch ( attribute )
        {
        case KIAPName:
            {
            // From RConnection
            if ( iConnInfos[index].iBearer >= EBearerExternalCSD &&
                    !iConnInfos[index].iBearerInfo.iInternal )
                {
                err = KErrNotSupported;
                }
            else
                {
                TBuf<KCommsDbSvrMaxFieldLength> settingName;
                settingName.Copy( TPtrC( IAP ) );
                settingName.Append( TChar( '\\' ) );
                settingName.Append( TPtrC( COMMDB_NAME ) );
                // IAP name max. length in commsdat should be 50
                err = iConnInfos[index].iConnAttach->GetDesSetting( TPtrC( settingName ), aValue );
                }
            }
            break;

        case KAccessPointName:
            {
            if ( iConnInfos[index].iBearer < EBearerExternalCSD )
                {
                err = GetAccessPointName( iConnInfos[index].iConnectionId, aValue );
                }
            else if ( iConnInfos[index].iBearer == EBearerExternalGPRS ||
                    iConnInfos[index].iBearer == EBearerExternalEdgeGPRS ||
                    iConnInfos[index].iBearer == EBearerExternalWCDMA ||
                    ( !iConnInfos[index].iBearerInfo.iInternal &&
                            ( iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSDPA ||
                                    iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSUPA ||
                                    iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSxPA ) ) )
                {
                if ( iPsdFax )
                    {
                    err = iPsdFax->GetApn( connectionId, aValue );
                    }
                else
                    {
                    err = KErrNotFound;
                    }
                }
            else
                {
                err = KErrNotSupported;
                }
            }
            break;

        case KTelNumber:
            {
            if ( iConnInfos[index].iBearer < EBearerExternalCSD )
                {
                err = GetTelNumber( iConnInfos[index].iConnectionId, aValue );
                }
            else if ( iConnInfos[index].iBearer == EBearerExternalCSD ||
                    iConnInfos[index].iBearer == EBearerExternalHSCSD ||
                    iConnInfos[index].iBearer == EBearerExternalWcdmaCSD )
                {
                if ( iCsdFax )
                    {
                    err = iCsdFax->GetTelNumber( aValue );
                    }
                else
                    {
                    err = KErrNotFound;
                    }
                }
            else
                {
                err = KErrNotSupported;
                }
            }
            break;

        case KNetworkName:
            {
            err = KErrNotSupported;
            if ( iConnInfos[index].iBearer == EBearerWLAN )
                {
                if ( iWlanSupport )
                    {
                    // Max SSID length is 32
                    err = iWlanSupport->GetConnectionSsid( aValue );
                    }
                }
            }
            break;

        default:
            err = KErrArgument;
            break;
        }

    LOGEXITFN1("CConnMonIAP::GetStringAttribute()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetUids
// RConnection::EnumerateConnections() must be called to get the right
// RConnection index for the connection (iConnInfos can't be trusted because
// connections are added by events, too).
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetUids(
        const TUint aConnectionId,
        TConnMonClientEnum& aClientEnum )
    {
    TInt                     err( KErrNone );
    TConnEnumArgBuf          enumArgBuf;
    TConnGetClientInfoArgBuf clientInfoBuf;
    TUint                    count( 0 );
    TConnectionInfoBuf       connInfoBuf;
    TUint                    j;

    // Find connection matching the given Id
    TInt index = Index( aConnectionId );

    if ( index < 0  )
        {
        return KErrNotFound;
        }

    // Uids are not available for external connections
    if ( iConnInfos[index].iBearer >= EBearerExternalCSD &&
         !iConnInfos[index].iBearerInfo.iInternal )
        {
        return KErrNotSupported;
        }

    // Get RConnection index for the connection
    err = iConnection.EnumerateConnections( count );

    if ( err != KErrNone )
        {
        return( err );
        }

    for ( j = 1; j <= count; j++ )
        {
        err = iConnection.GetConnectionInfo( j, connInfoBuf );

        if ( err != KErrNone )
            {
            return( err );
            }

        if ( ( iConnInfos[index].iIapId == connInfoBuf().iIapId ) &&
                ( iConnInfos[index].iNetId == connInfoBuf().iNetId ) )
            {
            break;
            }
        }

    // If connection does not exist anymore
    if ( j > count )
        {
        return KErrNotFound;
        }

    enumArgBuf().iIndex = j;

    // Get the number of clients
    err = iConnection.Control( KCOLConnection, KCoEnumerateConnectionClients, enumArgBuf );

    if ( err != KErrNone )
        {
        return err;
        }

    aClientEnum.iCount = enumArgBuf().iCount;

    // Count connection monitor OUT of the number of clients
    if ( aClientEnum.iCount > 0 )
        {
        --aClientEnum.iCount;
        }

    // Get client uids one by one
    // Note. in WINS uids are zero.
    TInt k( 0 );

    for ( TUint i = 1; i <= enumArgBuf().iCount; i++ )
        {
        clientInfoBuf().iIndex = i;

        err = iConnection.Control( KCOLConnection, KCoGetConnectionClientInfo, clientInfoBuf );

        if ( err != KErrNone )
            {
            return err;
            }

        // Exclude Connection Monitor UID
        if ( ( clientInfoBuf().iClientInfo.iUid.iUid != KConnMonDllUid.iUid ) &&
             ( clientInfoBuf().iClientInfo.iUid.iUid != KGenericDllUid.iUid ) &&
             ( clientInfoBuf().iClientInfo.iUid.iUid != KConnMonServerUid.iUid ) &&
             ( clientInfoBuf().iClientInfo.iUid.iUid != 0 ) )
            {
            TInt m( 0 );

            // Look for duplicates
            for ( m = 0; m < k; m++ )
                {
                if ( aClientEnum.iUid[m].iUid == clientInfoBuf().iClientInfo.iUid.iUid )
                    {
                    break;
                    }
                }

            if ( m == k )
                {
                // This is a new unique ID
                aClientEnum.iUid[k].iUid = clientInfoBuf().iClientInfo.iUid.iUid;
                k++;
                }
            else
                {
                // Duplicate
                --aClientEnum.iCount;
                }
            }
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::SetUintAttribute
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::SetUintAttribute(
        const RMessage2& /*aMessage*/,
        const TUint /*aValue*/ )
    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::AsyncConnectionStopL
// Checks parameter validity first. If all ok creates an active object that
// completes the connection stop in a separate thread. 
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::AsyncConnectionStopL( const RMessage2& aMessage )
    {
    LOGENTRFN("CConnMonIAP::AsyncConnectionStopL()")
    TInt result( KErrGeneral );
    TUint connectionId( aMessage.Int0() );
    
    TInt index = Index( connectionId );
    if ( index < 0 )
        {
        LOGIT1("AsyncConnectionStopL: unknown connection index %d", connectionId)
        result = KErrNotFound;
        }
    else
        {
        // Check if value is true or false
        if ( !( aMessage.Int3() ) )
            {
            LOGIT("AsyncConnectionStopL: boolean value was false, quitting")
            result = KErrNone;
            }
        else
            {
            if ( iConnInfos[index].iAsyncStopStatus != EConnMonStopCompleted )
                {
                TInt threadStartError( KErrNone );
                if ( iConnInfos[index].iAsyncStopStatus == EConnMonAsyncStopNotStarted )
                    {
                    threadStartError = StartAsyncStopThreadL( index );
                    }
                if ( threadStartError == KErrNone )
                    {
                    // Associate this connection into this stop request
                    TAsyncStopQueueElement request( aMessage );
                    request.Add( connectionId );
                    iAsyncStopQueue.Append( request );
                    result = KRequestPending;
                    }
                else
                    {
                    result = threadStartError;
                    }
                }
            else
                {
                // Connection has already been closed, but not yet removed from connection table
                result = KErrNone;
                }
            }
        }
    LOGEXITFN1("CConnMonIAP::AsyncConnectionStopL()", result)
    return result;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::AsyncConnectionStopAllL
// Checks parameter validity first. If all ok creates an active object for each
// connection that completes the connection stop in a separate thread. If any
// errors occur, returns the first encountered error code.
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::AsyncConnectionStopAllL( const RMessage2& aMessage )
    {
    LOGENTRFN("CConnMonIAP::AsyncConnectionStopAllL()")

    TInt result( KErrNone );
    // Check if value is true or false
    if ( !( aMessage.Int3() ) )
        {
        LOGIT("AsyncConnectionStopAllL: boolean value was false, quitting")
        result = KErrNone;
        }
    else
        {
        TAsyncStopQueueElement request( aMessage );
        TInt count = iConnInfos.Count();
        result = KErrNone;
        
        for ( TInt i = 0; i < count; i++ )
            {
            if ( iConnInfos[i].iAsyncStopStatus != EConnMonStopCompleted )
                {
                TInt threadStartError( KErrNone );
                if ( iConnInfos[i].iAsyncStopStatus == EConnMonAsyncStopNotStarted )
                    {
                    threadStartError = StartAsyncStopThreadL( i );
                    }
                if ( threadStartError == KErrNone )
                    {
                    // Associate this connection into this stop request
                    request.Add( iConnInfos[i].iConnectionId );
                    }
                else
                    {
                    // Thread start failed
                    if ( result == KErrNone )
                        {
                        // Keep only the first error that occurs
                        result = threadStartError;
                        }
                    LOGIT2("AsyncConnectionStopAllL: Error starting stop thread for conn. id %d <%d>",
                            iConnInfos[i].iConnectionId, threadStartError)
                    }
                }
            }
        // If one or more async connection stop threads have been started, and
        // no errors have been encountered, set request to pending and add to
        // queue to wait for thread(s) to complete.
        if ( request.iIdCount > 0 )
            {
            if ( result == KErrNone )
                {
                iAsyncStopQueue.Append( request );
                result = KRequestPending;
                }
            }
        }
    LOGEXITFN1("CConnMonIAP::AsyncConnectionStopAllL()", result)
    return result;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::StartAsyncStopThread
// Starts a new thread to asynchronously stop a connection. Returns KErrNone if
// thread started successfully.
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::StartAsyncStopThreadL( TInt aIndex )
    {
    LOGENTRFN("CConnMonIAP::StartAsyncStopThreadL()")
    TInt err( KErrNone );
    
    if ( aIndex < 0 || aIndex >= iConnInfos.Count() )
        {
        return KErrArgument;
        }

    iConnInfos[aIndex].iAsyncStopStatus = EConnMonAsyncStopStarted;
    if ( iConnInfos[aIndex].iBearer < EBearerExternalCSD )
        {
        // Stop an internal connection
        LOGIT("StartAsyncStopThreadL: connection type internal")

        // Cancel data volume and activity notifiers before stopping
        if ( iConnInfos[aIndex].iDLDataNotifier != 0 )
            {
            iConnInfos[aIndex].iDLDataNotifier->Cancel();
            }
        if ( iConnInfos[aIndex].iULDataNotifier != 0 )
            {
            iConnInfos[aIndex].iULDataNotifier->Cancel();
            }
        if ( iConnInfos[aIndex].iDataVolumeAO != 0 )
            {
            iConnInfos[aIndex].iDataVolumeAO->Cancel();
            }
        if ( iConnInfos[aIndex].iActivityNotifier != 0 )
            {
            iConnInfos[aIndex].iActivityNotifier->Cancel();
            }

        // Create and start a oneshot active object that starts and waits for
        // the async stop thread to complete
        CConnMonAsyncStopDaemon* asyncStopDaemon = CConnMonAsyncStopDaemon::NewL( this );
        err = asyncStopDaemon->Start(
                iConnInfos[aIndex].iConnectionId,
                iConnInfos[aIndex].iIapId,
                iConnInfos[aIndex].iNetId );
        if ( err )
            {
            // Error, cleanup the active object since it did not start
            delete asyncStopDaemon;
            }
        }
    else if ( iConnInfos[aIndex].iBearer == EBearerExternalGPRS     ||
              iConnInfos[aIndex].iBearer == EBearerExternalEdgeGPRS ||
              iConnInfos[aIndex].iBearer == EBearerExternalWCDMA    ||
              iConnInfos[aIndex].iBearer == EBearerExternalCDMA2000 ||
              ( !iConnInfos[aIndex].iBearerInfo.iInternal &&
                ( iConnInfos[aIndex].iBearerInfo.iBearer == EBearerInfoHSDPA ||
                  iConnInfos[aIndex].iBearerInfo.iBearer == EBearerInfoHSUPA ||
                  iConnInfos[aIndex].iBearerInfo.iBearer == EBearerInfoHSxPA ) ) )
        {
        // Stop an external PSD connection
        LOGIT("StartAsyncStopThreadL: connection type external PSD")
        if ( iPsdFax )
            {
            CConnMonAsyncStopDaemon* asyncStopDaemon = CConnMonAsyncStopDaemon::NewL( this );
            err = asyncStopDaemon->Start(
                    iConnInfos[aIndex].iConnectionId,
                    iPsdFax );
            if ( err )
                {
                // Error, cleanup the active object since it did not start
                delete asyncStopDaemon;
                }
            }
        else
            {
            err = KErrNotFound;
            }
        }
    else if ( iConnInfos[aIndex].iBearer == EBearerExternalCSD   ||
              iConnInfos[aIndex].iBearer == EBearerExternalHSCSD ||
              iConnInfos[aIndex].iBearer == EBearerExternalWcdmaCSD )
        {
        LOGIT("AsyncConnectionStopL: Error, connection type external CSD not supported")
        err = KErrNotSupported;
        }
    else
        {
        LOGIT("AsyncConnectionStopL: Error, unknown bearer")
        err = KErrNotSupported;
        }

    LOGEXITFN1("CConnMonIAP::StartAsyncStopThreadL()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::CompleteAsyncStopReqs
// Called when an asynchronous connection stop has completed.
// This method will go through all pending async connection stop requests and
// mark this connections status to those requests that are waiting for it.
// Those requests that were only waiting for this connection anymore, are
// completed.
// -----------------------------------------------------------------------------
//
void CConnMonIAP::CompleteAsyncStopReqs( const TUint aConnectionId, const TInt aError )
    {
    LOGENTRFN("CConnMonIAP::CompleteAsyncStopReqs()")

    // Mark the internal status of the connection to stopped
    TInt index = Index( aConnectionId );
    LOGIT3("Connection %d (index %d) marked as stopped <%d>", aConnectionId, index, aError)
    if ( index >= 0 )
        {
        iConnInfos[index].iAsyncStopStatus = EConnMonStopCompleted;
        }

#ifdef _DEBUG    
    LOGIT1("iAsyncStopQueue contents (%d:)", iAsyncStopQueue.Count() )
    for ( TInt i = 0; i < iAsyncStopQueue.Count(); i++ )
        {
        LOGIT1("iIdCount   = %d", iAsyncStopQueue[i].iIdCount)
        LOGIT1("iStopCount = %d", iAsyncStopQueue[i].iStopCount)
        LOGIT1("iStatus    = %d", iAsyncStopQueue[i].iStatus)
        for ( TInt j = 0; j < iAsyncStopQueue[i].iIdCount; j++ )
            {
            LOGIT1(".       id = %02d", iAsyncStopQueue[i].iId[j])
            }
        }
#endif // _DEBUG

    // Go through all pending async stop requests. If a request has been
    // cancelled, it has been removed from this array already.
    for ( TInt i = 0; i < iAsyncStopQueue.Count(); i++ )
        {
        // Go through all connections related to this request
        for ( TInt j = 0; j < iAsyncStopQueue[i].iIdCount; j++ )
            {
            // Was this request waiting for the connection that was just stopped
            if ( iAsyncStopQueue[i].iId[j] == aConnectionId )
                {
                iAsyncStopQueue[i].iStopCount++;
                // Set iAsyncStopQueue[i].iId[j] to 0 if tracking is needed

                // Store the first encountered error
                if ( aError && iAsyncStopQueue[i].iStatus == KErrNone )
                    {
                    iAsyncStopQueue[i].iStatus = aError;
                    }
                }
            }
        // Have all connections stopped that this request is waiting for
        if ( iAsyncStopQueue[i].iStopCount >= iAsyncStopQueue[i].iIdCount )
            {
            LOGIT3("Completing request at index %d, id count %d <%d>",
                    i, iAsyncStopQueue[i].iIdCount, iAsyncStopQueue[i].iStatus)
            iAsyncStopQueue[i].iMessage.Complete( iAsyncStopQueue[i].iStatus );
            iAsyncStopQueue.Remove( i );
            i--;
            }
        }
    LOGEXITFN("CConnMonIAP::CompleteAsyncStopReqs()")
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::CleanupConnectionInfo
// If the stopped connection is still in the connection info array and
// connection up/down notifier isn't running, removes the obsolete information
// and notifiers from the connection info array.
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::CleanupConnectionInfo( const TUint& aConnectionId )
    {
    LOGENTRFN("CConnMonIAP::CleanupConnectionInfo()")
    TInt err( KErrNone );
    
    TInt index = Index( aConnectionId );
    if ( index < 0 )
        {
        LOGIT("CleanupConnectionInfo: ID no longer in connection table")
        err = KErrNotFound;
        }
    else
        {
        // If conn up/down notifier is active, let the event remove connection later.
        if ( !iConnUpDownNotifier || !iConnUpDownNotifier->IsActive() )
            {
            err = RemoveConnection( iConnInfos[index] );
            }
        else
            {
            LOGIT("CleanupConnectionInfo: iConnUpDownNotifier active, letting event do cleanup")
            err = KErrNone;
            }
        }
    LOGEXITFN1("CConnMonIAP::CleanupConnectionInfo()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::CancelAsyncStopReqs
// Cancels all asynchronous connection stop request for a specific client.
// -----------------------------------------------------------------------------
//
void CConnMonIAP::CancelAsyncStopReqs( CSession2* aSession )
    {
    LOGENTRFN("CConnMonIAP::CancelAsyncStopReqs()")

    LOGIT1("AsyncStopQueue count = %d", iAsyncStopQueue.Count())
    for ( TInt i = 0; i < iAsyncStopQueue.Count(); i++ )
        {
        if ( iAsyncStopQueue[i].iMessage.Session() == aSession )
            {
            iAsyncStopQueue[i].iMessage.Complete( KErrCancel );
            iAsyncStopQueue.Remove( i );
            i--;
            LOGIT1("Cancelled a request for session %d", aSession)
            }
        }

    LOGEXITFN("CConnMonIAP::CancelAsyncStopReqs()")
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::SetBoolAttributeL
// Used for stopping the connections.
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::SetBoolAttribute( const RMessage2& aMessage )
    {
    LOGENTRFN("CConnMonIAP::SetBoolAttribute()")
    TInt err( KErrNone );
    TUint attribute( aMessage.Int2() );
    TBool value( aMessage.Int3() );

    switch ( attribute )
        {
        case KConnectionStop:
            if ( value )
                {
                // Find connection matching the given Id
                TInt index = Index( aMessage.Int0() );
                if ( index < 0  )
                    {
                    err = KErrNotFound;
                    }
                else
                    {
                    err = StopConnection( index );
                    if ( KErrCouldNotConnect == err )
                        {
                        err = KErrNotFound;
                        }
                    }
                }
            break;

        case KConnectionStopAll:
            if ( value )
                {
                TUint i( 0 );
                TUint count( 0 );
                TUint newCount( 0 );

                // Refresh array
                err = EnumerateConnections( count );
                LOGIT2("SetBoolAttribute: KConnectionStopAll: Enumerated all connections, count %d <%d>", count, err)
                if ( KErrNone == err )
                    {
                    while ( count > 0 && i < count )
                        {
                        // Stop (and remove if up/down event watcher is dead) the connection
                        err = StopConnection( i );
                        if ( KErrNone != err )
                            {
                            LOGIT2("SetBoolAttribute: KConnectionStopAll: Error stopping connection %d <%d>", i, err)
                            return err;
                            }
                        newCount = iConnInfos.Count();
                        if ( count > newCount )
                            {
                            count = newCount; // Connection has been removed
                            }
                        else
                            {
                            i++; // Cconnection has not been removed, increment index to next connection
                            }
                        }
                    }
                }
            break;

        default:
            err = KErrArgument;
            break;
        }

    LOGEXITFN1("CConnMonIAP::SetBoolAttribute()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::SetStringAttributeL
//
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::SetStringAttributeL(
        const RMessage2& /*aMessage*/,
        const TDes& /*aValue*/ )
    {
    return KErrNotSupported;
    }


// -----------------------------------------------------------------------------
// CConnMonIAP::ListenL
// Starts event watchers.
// -----------------------------------------------------------------------------
//
void CConnMonIAP::ListenL()
    {
    LOGIT(".")
    LOGENTRFN("CConnMonIAP::ListenL()")
    iCountListeners++;

    // GLOBAL notifiers

    // Connection Up/Down events
    if ( iConnUpDownNotifier == 0 )
        {
        iConnUpDownNotifier = new( ELeave ) CConnUpDownNotifier( iServer );
        iConnUpDownNotifier->Construct();
        }
    if ( !iConnUpDownNotifier->IsActive() )
        {
        iConnUpDownNotifier->Receive(); // (re)start listening
        }

    if ( iTSYLoaded && iPacketServLoaded )
        {
        // PSD network status events
        if ( iPsdNetwStatusNotifier == 0 )
            {
            iPsdNetwStatusNotifier = CNetwStatusNotifier::NewL( iServer );
            }
        if ( !iPsdNetwStatusNotifier->IsActive() )
            {
            iPsdNetwStatusNotifier->Receive(); // (re)start listening
            }

        // GPRS bearer availability changed events
        if ( iGSMBearerAvailabilityNotifier == 0 )
            {
            iGSMBearerAvailabilityNotifier = CBearerAvailabilityNotifier::NewL( iServer );
            }
        if ( !iGSMBearerAvailabilityNotifier->IsActive() )
            {
            iGSMBearerAvailabilityNotifier->Receive(); // (re)start listening
            }
        }

    if ( iTSYLoaded )
        {
        // Mode change events
        if ( iModeNotifier == 0 )
            {
            iModeNotifier = CModeNotifier::NewL( iServer, iMobilePhone );
            }
        if ( !iModeNotifier->IsActive() )
            {
            iModeNotifier->Receive(); // (re)start listening
            }

        // Network registration status events
        if ( iNetwRegistrationNotifier == 0 )
            {
            iNetwRegistrationNotifier = CNetwRegistrationNotifier::NewL( iServer, iMobilePhone );
            }
        if ( !iNetwRegistrationNotifier->IsActive() )
            {            
            iNetwRegistrationNotifier->Receive(); // (re)start listening
            
            // We might have missed the network registration notification before 
            // we started listening for notifications here. Update the 
            // network registration information.            
            TInt registration( ENetworkRegistrationExtNotAvailable );                
            User::LeaveIfError( GetNetworkRegistration_v2( registration ) );
            iServer->CellularDataUsageKeyUpdater()->UpdateKeyL( registration );            
            }

        // Bearer change (GPRS/Edge GPRS) status events
        if ( FeatureManager::FeatureSupported( KFeatureIdEdgeKnowledge ) )
            {
            if ( iEdgeNotifier == 0 )
                {
                iEdgeNotifier = new( ELeave ) CBearerNotifier( iServer, iMobilePhone );
                TRAPD( ret, iEdgeNotifier->ConstructL() );
                if ( ret != KErrNone )
                    {
                    // Construction (=opening ETel CustomAPI) failed
                    delete iEdgeNotifier;
                    iEdgeNotifier = 0;
                    }
                }
            if ( iEdgeNotifier != 0 )
                {
                if ( !iEdgeNotifier->IsActive() )
                    {
                    iEdgeNotifier->Receive(); // (re)start listening
                    }
                }
            }

        if ( !iWcdmaNotifier )
            {
            iWcdmaNotifier = CWcdmaBearerNotifier::NewL( iServer, iTelServer );
            if ( iWcdmaNotifier )
                {
                iWcdmaNotifier->Receive();
                }
            }

        if (  FeatureManager::FeatureSupported( KFeatureIdConnMonExtension ) )
            {
            // GSM signal strength change events
            if ( iGSMSignalNotifier == 0 )
                {
                iGSMSignalNotifier = CGsmSignalNotifier::NewL( iServer, iMobilePhone );
                }
            if ( !iGSMSignalNotifier->IsActive() )
                {
                iGSMSignalNotifier->Receive(); // (re)start listening
                }
            }
        }

    if ( !iTelephonyNotifier )
        {
        iTelephonyNotifier = CConnMonTelNotifier::NewL( *this, &iMobilePhone );
        iTelephonyNotifier->Receive();
        }

    if ( !iDualTransferModeNotifier && iPacketServLoaded )
        {
        iDualTransferModeNotifier = CConnMonDtmNotifier::NewL( iPacketService, *this );
        iDualTransferModeNotifier->Receive();
        }

    if ( iWlanSupport )
        {
        iWlanSupport->EnableEventsToClients();
        }

    // Notifiers owned by the connections
    // (connection status, data volumes, connection activity, subconnection up/down)
    TInt count = iConnInfos.Count();

    for ( TInt i = 0; i < count; i++ )
        {
        AddConnNotificationsL( i );
        }
    LOGEXITFN("CConnMonIAP::ListenL()")
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::CancelListen
// Stops and deletes event watchers.
// -----------------------------------------------------------------------------
//
void CConnMonIAP::CancelListen()
    {
    iCountListeners--;
    LOGIT1("CancelListen() remaining listeners: %d", iCountListeners)

    if ( iCountListeners == 0 )
        {
        DeleteNotifications();
        }
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::AddConnectionL
// Adds a new connection to the iConnInfos array. Starts event watchers for the
// connection if there are clients that want events.
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::AddConnectionL( TConnInfo& aConnection )
    {
    TInt err( KErrNone );
    TInt index( KErrNotFound );
    TUint8 internal( 0 );

    if ( aConnection.iBearer < EBearerExternalCSD )
        {
        internal = 1;
        }

    if ( internal )
        {
        index = iConnInfos.Find( aConnection, TConnInfo::MatchIap );
        }
    /*
    else
        {
        index = iConnInfos.Find( aConnection, TConnInfo::MatchBearer );
        }
    */

    if ( KErrNotFound != index )
        {
        LOGIT("AddConnectionL: New connection occured with same IAP id, remove old")
        // To remove the connection we have to complete the
        // connection by sending the notifications needed by clients.
        CompleteConnectionClosing( index );
        }

    // Add to connection table
    aConnection.iConnectionId = NewConnectionId();

    if ( internal )
        {
        err = OpenAndAttachTo( aConnection );
        if ( err != KErrNone )
            {
            LOGIT1("AddConnectionL: ERROR attach failed <%d>", err)
            return err;
            }
        }

    err = iConnInfos.Append( aConnection );

    if ( ( KErrNone == err ) && internal )
        {
        index = iConnInfos.Find( aConnection, TConnInfo::MatchIap );
        if ( ( index != KErrNotFound ) && ( iCountListeners > 0 ) )
            {
            TRAPD( ret, AddConnNotificationsL( index ) );
            if ( ret != KErrNone )
                {
                // Close attached connection
                if ( iConnInfos[ index ].iConnAttach != 0 )
                    {
                    iConnInfos[ index ].iConnAttach->Close();
                    delete iConnInfos[ index ].iConnAttach;
                    }

                // Remove
                iConnInfos.Remove( index );

                // Leave
                User::Leave( ret );
                }
            }
        }

    // Print connection info array to log
    #ifdef _DEBUG
    LOGIT("AddConnectionL: Connection info array:")
    for ( TInt m = 0; m < iConnInfos.Count(); m++ )
        {
        LOGIT4("  id %d, bearer %d, iapId %d, netId %d", iConnInfos[m].iConnectionId,
                iConnInfos[m].iBearer, iConnInfos[m].iIapId, iConnInfos[m].iNetId)
        }
    #endif // _DEBUG

    return err;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::CompleteConnectionClosing
// -----------------------------------------------------------------------------------
//
void CConnMonIAP::CompleteConnectionClosing( TInt& aIndex )
    {
    LOGENTRFN("CConnMonIAP::CompleteConnectionClosing()")

    if ( iConnInfos[aIndex].iBearerNotifier->Listening() )
        {
        iConnInfos[aIndex].iBearerNotifier->CancelListen();
        }

    if ( iConnInfos[aIndex].iProgressNotifier->IsActive() )
        {
        // Progress notification request is still active --> Cancel it
        iConnInfos[aIndex].iProgressNotifier->Cancel();
        }
    // Send KLinkLayerClosed event
    LOGIT("CompleteConnectionClosing: ProgressNotifier->SendClosedEvent()")
    iConnInfos[aIndex].iProgressNotifier->SendClosedEvent();

    if ( iConnInfos[aIndex].iConnDownNotifier->IsActive() )
        {
        // Allinterface notification request is still active --> Cancel it
        iConnInfos[aIndex].iConnDownNotifier->Cancel();
        }

    if ( !iConnInfos[aIndex].iConnDownNotifier->DeleteSent() )
        {
        // After SetInterfaceClosed() call SendDeletedEvent() removes
        // the connection
        iConnInfos[aIndex].iConnDownNotifier->SetInterfaceClosed();
        iConnInfos[aIndex].iConnDownNotifier->SendDeletedEvent();
        }
    else
        {
        // Delete has already been sent (shouldn't be possible) so
        // connection has to be removed
        LOGIT("CompleteConnectionClosing: Delete already sent, removing connection")
        RemoveConnection( iConnInfos[aIndex] );
        }

    LOGEXITFN("CConnMonIAP::CompleteConnectionClosing()")
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::RemoveConnection
// Removes connection settings from sessions, stops and deletes event watchers and
// removes the connection info from the iConnInfos array.
// Connection is deleted by an event (or by calling RConnection::Stop()).
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::RemoveConnection( TConnInfo& aConnection )
    {
    LOGENTRFN("CConnMonIAP::RemoveConnection()")

    TInt index( KErrNotFound );
    // Check if bearer is external CSD
    if ( aConnection.iBearer == EBearerExternalCSD ||
            aConnection.iBearer == EBearerExternalHSCSD ||
            aConnection.iBearer == EBearerExternalWcdmaCSD )
        {
        index = iConnInfos.Find( aConnection, TConnInfo::MatchBearer );
        }
    else if ( aConnection.iConnectionId > 0 )
        {
        index = iConnInfos.Find( aConnection, TConnInfo::MatchId );
        }
    else
        {
        index = iConnInfos.Find( aConnection, TConnInfo::MatchIap );
        }

    TInt err( KErrNotFound );
    if ( index >= 0 )
        {
        // Set connection id for passing it back to the caller (client event)
        aConnection.iConnectionId = iConnInfos[index].iConnectionId;

        // Remove threshold settings from the client tables
        iServer->RemoveConnSettingsFromSessions( aConnection.iConnectionId );

        // Stop notifications (data volume, connection status, etc.)
        DeleteConnNotifications( index );

        // Close attached connection
        if ( iConnInfos[index].iConnAttach != 0 )
            {
            iConnInfos[index].iConnAttach->Close();
            delete iConnInfos[index].iConnAttach;
            iConnInfos[index].iConnAttach = NULL;
            }

        iConnInfos.Remove( index );

        // If dial-up PDP context override feature is enabled and active (a dial-up
        // connection is about to start), connection closures need to be reported
        // in case dial-up connection is waiting for a free PDP context.
        if ( iServer->GetDialUpOverrideStatus() == EConnMonDialUpOverrideActive )
            {
            if ( !ActivePacketdataConnectionsFound() )
                {
                iServer->ConnectionsClosedForDialUpOverride();
                }
            }

        err = KErrNone;
        }

    // Print connection info array to log
    #ifdef _DEBUG
    LOGIT("RemoveConnection: Connection info array:")
    for ( TInt m = 0; m < iConnInfos.Count(); m++ )
        {
        LOGIT4("  id %d, bearer %d, iapId %d, netId %d", iConnInfos[m].iConnectionId,
                iConnInfos[m].iBearer, iConnInfos[m].iIapId, iConnInfos[m].iNetId)
        }
    #endif // _DEBUG

    LOGEXITFN1("CConnMonIAP::RemoveConnection()", err)
    return err;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::CalculateNetworkStatus
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::CalculateNetworkStatus( const RPacketService::TStatus& aPacketStatus ) const
    {
    switch ( aPacketStatus )
        {
        case ( RPacketService::EStatusUnattached ):
            return EConnMonStatusUnattached;

        case ( RPacketService::EStatusAttached ):
            return EConnMonStatusAttached;

        case ( RPacketService::EStatusActive ):
            return EConnMonStatusActive;

        case ( RPacketService::EStatusSuspended ):
            return EConnMonStatusSuspended;

        default:
            break;
        }
    return aPacketStatus;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::CalculateNetworkRegistration
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::CalculateNetworkRegistration(
        const RMobilePhone::TMobilePhoneRegistrationStatus&
        aRegistration ) const
    {
    switch ( aRegistration )
        {
        case ( RMobilePhone::ERegistrationUnknown ):
            return ENetworkRegistrationUnknown;

        case ( RMobilePhone::ENotRegisteredNoService ):
            return ENetworkRegistrationNoService;

        case ( RMobilePhone::ENotRegisteredEmergencyOnly ):
            return ENetworkRegistrationEmergencyOnly;

        case ( RMobilePhone::ENotRegisteredSearching ):
            return ENetworkRegistrationSearching;

        case ( RMobilePhone::ERegisteredBusy ):
            return ENetworkRegistrationBusy;

        case ( RMobilePhone::ERegisteredOnHomeNetwork ):
            return ENetworkRegistrationHomeNetwork;

        case ( RMobilePhone::ERegistrationDenied ):
            return ENetworkRegistrationDenied;

        case ( RMobilePhone::ERegisteredRoaming ):
            return ENetworkRegistrationRoaming;

        default:
            break;
        }
    return aRegistration;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::CalculateNetworkRegistration_v2
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::CalculateNetworkRegistration_v2(
        const RMobilePhone::TMobilePhoneRegistrationStatus&
        aRegistration ) const
    {
    switch ( aRegistration )
        {
        case ( RMobilePhone::ERegistrationUnknown ):
            return ENetworkRegistrationExtUnknown;

        case ( RMobilePhone::ENotRegisteredNoService ):
            return ENetworkRegistrationExtNoService;

        case ( RMobilePhone::ENotRegisteredEmergencyOnly ):
            return ENetworkRegistrationExtEmergencyOnly;

        case ( RMobilePhone::ENotRegisteredSearching ):
            return ENetworkRegistrationExtSearching;

        case ( RMobilePhone::ERegisteredBusy ):
            return ENetworkRegistrationExtBusy;

        case ( RMobilePhone::ERegisteredOnHomeNetwork ):
            return ENetworkRegistrationExtHomeNetwork;

        case ( RMobilePhone::ERegistrationDenied ):
            return ENetworkRegistrationExtDenied;

        case ( RMobilePhone::ERegisteredRoaming ):
            {
            RMobilePhone::TMobilePhoneNetworkInfoV1 network;
            RMobilePhone::TMobilePhoneNetworkInfoV1Pckg networkPckg( network );

            RMobilePhone::TMobilePhoneNetworkInfoV1 homeNetwork;
            RMobilePhone::TMobilePhoneNetworkInfoV1Pckg homeNetworkPckg( homeNetwork );

            TRequestStatus status( KErrNone );

            iMobilePhone.GetCurrentNetwork(status, networkPckg);
            User::WaitForRequest( status );

            if(status.Int() == KErrNone)
                {
                iMobilePhone.GetHomeNetwork(status, homeNetworkPckg);
                User::WaitForRequest( status );

                if(network.iCountryCode.Compare(homeNetwork.iCountryCode) == 0)
                    {
                    return ENetworkRegistrationExtRoamingNational;
                    }
                else
                    {
                    return ENetworkRegistrationExtRoamingInternational;
                    }
                }
          }

        default:
            break;
        }
    return aRegistration;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::IsEdgeCell
// -----------------------------------------------------------------------------
//
TBool CConnMonIAP::IsEdgeCell()
    {
    LOGENTRFN("CConnMonIAP::IsEdgeCell()")
    TBool edgeStatus( EFalse );

    LOGTIMINGSTART("CConnMonIAP::IsEdgeCell")

    if ( FeatureManager::FeatureSupported( KFeatureIdEdgeKnowledge ) )
        {
        RMmCustomAPI customApi;
        RMmCustomAPI::TGprsInformation gprsInfo;
        RMmCustomAPI::TGprsInformationPckg gprsInfoPckg( gprsInfo );
        TRequestStatus status( KErrNone );

        TInt ret = customApi.Open( iMobilePhone );

        if ( KErrNone == ret )
            {
            customApi.GetEGprsInfo( status, gprsInfoPckg );
            User::WaitForRequest( status );

            customApi.Close();

            if ( KErrNone == status.Int() )
                {
                if ( gprsInfoPckg().iGprsInfo == RMmCustomAPI::EEdgeGprs )
                    {
                    edgeStatus = ETrue;
                    }
                }
            else
                {
                LOGIT1("IsEdgeCell: Error, RMmCustomAPI.GetEGprsInfo() <%d>", status.Int())
                }
            }
        else
            {
            LOGIT1("IsEdgeCell: Error, RMmCustomAPI.Open() <%d>", ret)
            }
        }
    else
        {
        LOGIT("IsEdgeCell: KFeatureIdEdgeKnowledge not supported")
        }
    LOGTIMINGEND("CConnMonIAP::IsEdgeCell")

    LOGEXITFN1("CConnMonIAP::IsEdgeCell()", (TInt)edgeStatus)
    return edgeStatus;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::IsHsdpaCell
// -----------------------------------------------------------------------------
//
TBool CConnMonIAP::IsHsdpaCell()
    {
    LOGENTRFN("CConnMonIAP::IsHsdpaCell()")

    RPacketService::TDynamicCapsFlags dynamicCaps( 0 );
    TBool hsdpaStatus( EFalse );

    LOGTIMINGSTART("CConnMonIAP::IsHsdpaCell")

    TInt ret = iPacketService.GetDynamicCaps( dynamicCaps );
    if ( dynamicCaps & RPacketService::KCapsHSDPA )
        {
        hsdpaStatus = ETrue;
        }
    LOGTIMINGEND("CConnMonIAP::IsHsdpaCell")
    LOGIT2("IsHsdpaCell: Dynamic caps 0x%06X <%d>", dynamicCaps, ret)

    LOGEXITFN1("CConnMonIAP::IsHsdpaCell()", (TInt)hsdpaStatus)
    return hsdpaStatus;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::HsxpaStatus
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::HsxpaStatus()
    {
    LOGENTRFN("CConnMonIAP::HsxpaStatus()")

    RPacketService::TDynamicCapsFlags dynamicCaps( 0 );
    TInt hspaState( EBearerInfoWCDMA );

    LOGTIMINGSTART("CConnMonIAP::HsxpaStatus")

    TInt ret = iPacketService.GetDynamicCaps( dynamicCaps );
    if ( dynamicCaps & RPacketService::KCapsHSDPA )
        {
        if ( dynamicCaps & RPacketService::KCapsHSUPA )
            {
            hspaState = EBearerInfoHSxPA;
            }
        else
            {
            hspaState = EBearerInfoHSDPA;
            }
        }
    else
        {
        if ( dynamicCaps & RPacketService::KCapsHSUPA )
            {
            hspaState = EBearerInfoHSUPA;
            }
        else
            {
            hspaState = EBearerInfoWCDMA;
            }
        }
    LOGTIMINGEND("CConnMonIAP::HsxpaStatus")
    LOGIT2("HsxpaStatus: Dynamic caps 0x%06X <%d>", dynamicCaps, ret)

    LOGEXITFN1("CConnMonIAP::HsxpaStatus()", hspaState)
    return hspaState;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetStartTime
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetStartTime( const TUint& aConnectionId, TTime& aTime )
    {
    TInt err( KErrNone );

    // Find connection matching the given Id
    TInt index = Index( aConnectionId );

    if ( index < 0  )
        {
        return KErrNotFound;
        }

    if ( iConnInfos[index].iBearer < EBearerExternalCSD
         && iConnInfos[index].iBearerInfo.iBearer <= EBearerInfoHSxPA )
        {
        // INTERNAL CONNECTIONS
        TUint subConnectionCount( 0 );

        // Find the start time of the earliest subconnection
        err = iConnInfos[index].iConnAttach->EnumerateSubConnections( subConnectionCount );

        if ( KErrNone != err )
            {
            return err;
            }

        LOGIT1("GetStartTime: Sub connection count %d", subConnectionCount)
        for ( TUint i = 1; i <= subConnectionCount; i++ )
            {
            TSubConnectionInfo subInfo;
            TPckg<TSubConnectionInfo> pckgSubInfo( subInfo );

            err = iConnInfos[index].iConnAttach->GetSubConnectionInfo( i, pckgSubInfo );

            if ( KErrNone != err )
                {
                LOGIT2("GetStartTime: Error getting conn.info for index %d, <%d>", i, err)
                return err;
                }

            if ( i == 1 )
                {
                aTime = pckgSubInfo().iTimeStarted;
                }
            else if ( aTime > pckgSubInfo().iTimeStarted )
                {
                aTime = pckgSubInfo().iTimeStarted;
                }
            }
        }
    else if ( iConnInfos[index].iBearer == EBearerExternalGPRS     ||
              iConnInfos[index].iBearer == EBearerExternalEdgeGPRS ||
              iConnInfos[index].iBearer == EBearerExternalWCDMA    ||
              iConnInfos[index].iBearer == EBearerExternalCDMA2000 ||
              ( !iConnInfos[index].iBearerInfo.iInternal &&
                ( iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSDPA ||
                  iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSUPA ||
                  iConnInfos[index].iBearerInfo.iBearer == EBearerInfoHSxPA ) ) )
        {
        // EXTERNAL PSD
        if ( iPsdFax )
            {
            err = iPsdFax->GetStartTime( aConnectionId, aTime );

            if ( KErrNone != err )
                {
                return err;
                }
            }
        else
            {
            return KErrNotFound;
            }
        }
    else if ( iConnInfos[index].iBearer == EBearerExternalCSD   ||
              iConnInfos[index].iBearer == EBearerExternalHSCSD ||
              iConnInfos[index].iBearer == EBearerExternalWcdmaCSD )
        {
        // EXTERNAL CSD
        if ( iCsdFax )
            {
            err = iCsdFax->GetStartTime( aTime );

            if ( KErrNone != err )
                {
                return err;
                }
            }
        else
            {
            err = KErrNotFound;
            }
        }
    else
        {
        return KErrNotSupported;
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetBearer
// Gets the bearer setting for the internal connections.
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetBearer(
        const TUint& aConnectionId,
        TInt& aBearer,
        TBearerInfo& aBearerInfo )
    {
    TInt err( KErrNone );
    TUint32 bearerType( EBearerUnknown );
    TBuf<KCommsDbSvrMaxFieldLength> settingName;
    TBuf<KCommsDbSvrMaxFieldLength> settingValue;

    LOGENTRFN("CConnMonIAP::GetBearer()")

    // Find connection matching the given Id
    TInt index = Index( aConnectionId );
    if ( index < 0 )
        {
        LOGEXITFN1("CConnMonIAP::GetBearer()", KErrNotFound)
        return KErrNotFound;
        }

    if ( iConnInfos[index].iConnAttach == 0 )
        {
        LOGIT("GetBearer: error, iConnAttach was NULL")
        LOGEXITFN1("CConnMonIAP::GetBearer()", KErrArgument)
        return KErrArgument;
        }

    // Find out BEARER TYPE
    settingName.Copy( TPtrC( IAP ) );
    settingName.Append( TChar( '\\' ) );
    settingName.Append( TPtrC( IAP_BEARER_TYPE ) );

    err = iConnInfos[index].iConnAttach->GetDesSetting( TPtrC( settingName ), settingValue );
    if ( KErrNone != err )
        {
        LOGIT1("GetBearer: ERROR getting IAP bearer type <%d>", err)
        LOGEXITFN1("CConnMonIAP::GetBearer()", err)
        return err;
        }

    if ( settingValue == TPtrC( VIRTUAL_BEARER ) )
        {
        aBearer = EBearerVirtual;
        aBearerInfo.iBearer = EBearerInfoVirtual;
        aBearerInfo.iInternal = ETrue;
        }

    // Find out SERVICE TYPE
    settingName.Copy( TPtrC( IAP ) );
    settingName.Append( TChar( '\\' ) );
    settingName.Append( TPtrC( IAP_SERVICE_TYPE ) );

    err = iConnInfos[index].iConnAttach->GetDesSetting( TPtrC( settingName ), settingValue );
    if ( KErrNone != err )
        {
        LOGIT1("GetBearer: ERROR getting IAP service type <%d>", err)
        LOGEXITFN1("CConnMonIAP::GetBearer()", err)
        return err;
        }

    // Is this virtual bearer + VPN service
    if ( ( aBearer == EBearerVirtual ) && ( settingValue == TPtrC( VPN_SERVICE ) ) )
        {
        aBearer = EBearerVirtualVPN;
        aBearerInfo.iBearer = EBearerInfoVirtualVPN;
        aBearerInfo.iInternal = ETrue;
        }

    // Return if bearer has been solved
    if ( aBearer != EBearerUnknown )
        {
        LOGEXITFN1("CConnMonIAP::GetBearer()", KErrNone)
        return KErrNone;
        }

    // Study service type if bearer is still unknown
    if ( settingValue == TPtrC( DIAL_OUT_ISP ) || settingValue == TPtrC( DIAL_IN_ISP ) )
        {
        LOGIT("GetBearer: type is dial_x_isp")
        settingName.Copy( settingValue );
        settingName.Append( TChar( '\\' ) );
        settingName.Append( TPtrC( ISP_BEARER_TYPE ) );

        err = iConnInfos[index].iConnAttach->GetIntSetting( TPtrC( settingName ), bearerType );

        if ( KErrNone != err )
            {
            return err;
            }

        if ( bearerType == EBearerTypeCSD )
            {
            aBearer = EBearerCSD;
            aBearerInfo.iBearer = EBearerInfoCSD;
            aBearerInfo.iInternal = ETrue;
            }
        else
            {
            // EBearerTypeHSCSD
            aBearer = EBearerHSCSD;
            aBearerInfo.iBearer = EBearerInfoHSCSD;
            aBearerInfo.iInternal = ETrue;
            }

        if ( iTSYLoaded )
            {
            RMobilePhone::TMobilePhoneNetworkMode mode;

            err = iMobilePhone.GetCurrentMode( mode );

            if ( ( KErrNone == err ) && ( mode == RMobilePhone::ENetworkModeWcdma ) )
                {
                aBearer = EBearerWcdmaCSD;
                aBearerInfo.iBearer = EBearerInfoWcdmaCSD;
                aBearerInfo.iInternal = ETrue;
                }
            }

        if ( aBearer == EBearerHSCSD || aBearer == EBearerCSD )
            {
            // If TsyName == PHONETSY try to find out the real life situation (CSD/HSCSD) from ETel
            // For point-to-point connections TSY is NOT PHONETSY.
            settingName.Copy( TPtrC( IAP ) );
            settingName.Append( TChar( '\\' ) );
            settingName.Append( TPtrC( IAP_BEARER_TYPE ) );

            err = iConnInfos[index].iConnAttach->GetDesSetting(
                    TPtrC( settingName ),
                    settingValue );

            if ( ( KErrNone == err ) && ( settingValue == TPtrC( MODEM_BEARER ) ) )
                {
                settingName.Copy( TPtrC( MODEM_BEARER ) );
                settingName.Append( TChar( '\\' ) );
                settingName.Append( TPtrC( MODEM_TSY_NAME ) );

                err = iConnInfos[index].iConnAttach->GetDesSetting(
                        TPtrC( settingName ),
                        settingValue );

                TBuf<KConnMonSmallBufferLen> tmp( KMmTsyModuleName );
                tmp.UpperCase();

                if ( ( KErrNone == err ) && ( settingValue == tmp ) )
                    {
                    // TSY is PHONETSY
                    TBuf<KConnMonSmallBufferLen> number;

                    err = GetTelNumber( aConnectionId, number );

                    if ( KErrNone == err )
                        {
                        GetBearerFromETel( number, aBearer, aBearerInfo );
                        }
                    }
                }
            }
        }
    else if ( settingValue == TPtrC( OUTGOING_WCDMA ) ||
              settingValue == TPtrC( INCOMING_WCDMA ) )
        {
        LOGIT("GetBearer: type is wcdma")
        if ( iTSYLoaded )
            {
            RMobilePhone::TMobilePhoneNetworkMode mode;
            err = iMobilePhone.GetCurrentMode( mode );
            LOGIT2("GetBearer: phone mode %d <%d>", (TInt)mode, err)
            if ( ( KErrNone != err ) ||
                 ( mode == RMobilePhone::ENetworkModeUnknown ) ||
                 ( mode == RMobilePhone::ENetworkModeUnregistered ) )
                {
                // MODE is UNKNOWN
                LOGIT2("GetBearer: mode is unknown %d <%d>", mode, err)
                if ( FeatureManager::FeatureSupported( KFeatureIdProtocolGsm ) )
                    {
                    // KFeatureIdProtocolGsm means that phone supports only GSM
                    if ( IsEdgeCell() )
                        {
                        aBearer = EBearerEdgeGPRS;
                        aBearerInfo.iBearer = EBearerInfoEdgeGPRS;
                        }
                    else
                        {
                        aBearer = EBearerGPRS;
                        aBearerInfo.iBearer = EBearerInfoGPRS;
                        }
                    aBearerInfo.iInternal = ETrue;
                    }
                else
                    {
                    LOGIT("GetBearer: KFeatureIdProtocolGsm not supported")
                    }
                }
            else
                {
                // MODE is KNOWN
                if ( mode == RMobilePhone::ENetworkModeWcdma )
                    {
                    aBearer = EBearerWCDMA;
                    aBearerInfo.iBearer = HsxpaStatus();
                    aBearerInfo.iInternal = ETrue;
                    }
                else if ( mode == RMobilePhone::ENetworkModeGsm )
                    {
                    if ( IsEdgeCell() )
                        {
                        aBearer = EBearerEdgeGPRS;
                        aBearerInfo.iBearer = EBearerInfoEdgeGPRS;
                        }
                    else
                        {
                        aBearer = EBearerGPRS;
                        aBearerInfo.iBearer = EBearerInfoGPRS;
                        }
                    aBearerInfo.iInternal = ETrue;
                    }
                else
                    {
                    LOGIT1("GetBearer: error, mode was unexpected %d", mode)
                    }
                }
            }
        }

    else if ( settingValue == TPtrC( LAN_SERVICE ) )
        {
        LOGIT("GetBearer: type is (w)lan")
        aBearer = EBearerLAN;
        aBearerInfo.iBearer = EBearerInfoLAN;
        aBearerInfo.iInternal = ETrue;

        settingName.FillZ();
        settingValue.FillZ();
        settingName.Copy( TPtrC( LAN_BEARER ) );
        settingName.Append( TChar( '\\' ) );
        settingName.Append( TPtrC( COMMDB_NAME ) );

        err = iConnInfos[index].iConnAttach->GetDesSetting( TPtrC( settingName ), settingValue );

        if ( KErrNone != err )
            {
            return err;
            }

        if ( settingValue == TPtrC( KWlanBearerRecordName ) )
            {
            aBearer = EBearerWLAN;
            aBearerInfo.iBearer = EBearerInfoWLAN;
            }
        }
    LOGIT2("GetBearer: bearer %d, bearerInfo %d", aBearer, aBearerInfo.iBearer)
    LOGEXITFN1("CConnMonIAP::GetBearer()", err)
    return err;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::UpdateActivity
// Updates connection activity information.
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::UpdateActivity( const TUint& aConnectionId, const TUint& aActivity )
    {
    // Find connection matching the given Id
    TInt index = Index( aConnectionId );

    if ( index < 0  )
        {
        return KErrNotFound;
        }

    if ( aActivity == 0 )
        {
        iConnInfos[index].iActivity = 0;
        }
    else
        {
        iConnInfos[index].iActivity = 1;
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::GetDeleteNotifyStatus
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::GetDeleteNotifyStatus( TConnInfo& aConnection )
    {
    TInt index( KErrNotFound );

    if ( aConnection.iBearer >= EBearerExternalCSD )
        {
        return KErrNotSupported;
        }

    index = iConnInfos.Find( aConnection, TConnInfo::MatchIap );

    if ( index < 0 )
        {
        return KErrNotFound;
        }

    aConnection.iConnectionId = iConnInfos[index].iConnectionId;
    aConnection.iBearer = iConnInfos[index].iBearer;

    return KErrNone;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::GetActivityNotifier
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::GetActivityNotifier(
        const TUint& aConnectionId,
        CActivityNotifier** aActivityNotifier )
    {
    // Find connection matching the given Id
    TInt index = Index( aConnectionId );

    if ( index < 0  )
        {
        return KErrNotFound;
        }

    if ( aActivityNotifier == 0 )
        {
        return KErrUnknown;
        }

    *aActivityNotifier = iConnInfos[ index ].iActivityNotifier;

    return KErrNone;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::GetProgressNotifier
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::GetProgressNotifier(
        const TUint& aConnectionId,
        CProgressNotifier** aProgressNotifier )
    {
    // Find connection matching the given Id
    TInt index = Index( aConnectionId );

    if ( index < 0  )
        {
        return KErrNotFound;
        }

    if ( aProgressNotifier == 0 )
        {
        return KErrUnknown;
        }

    *aProgressNotifier = iConnInfos[index].iProgressNotifier;

    return KErrNone;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::GetSubConnUpDownNotifier
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::GetSubConnUpDownNotifier(
        const TUint& aConnectionId,
        CSubConnUpDownNotifier** aSubConnUpDownNotifier )
    {
    // Find connection matching the given Id
    TInt index = Index( aConnectionId );

    if ( index < 0  )
        {
        return KErrNotFound;
        }

    if ( aSubConnUpDownNotifier == 0 )
        {
        return KErrUnknown;
        }

    *aSubConnUpDownNotifier = iConnInfos[index].iConnDownNotifier;

    return KErrNone;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::WakeUpNotifications
// Wakes up sleeping CONNECTION SPECIFIC event watchers that depend on a threshold.
// (data volume, activity).
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::WakeUpNotifications( const TUint& aConnectionId )
    {
    // Find connection matching the given Id
    TInt index = Index( aConnectionId );

    if ( index < 0  )
        {
        return KErrNotFound;
        }

    if ( ( iConnInfos[index].iBearer < EBearerExternalCSD ) &&  ( iCountListeners > 0 ) )
        {
        // INTERNAL CONNECTIONS
        TInt connectionStatus( 0 );
        GetConnectionStatus( index, connectionStatus );

        if ( ( connectionStatus == KLinkLayerOpen ) || ( connectionStatus == KPsdSuspended ) )
            {
            // -------------------------
            // Amount of Downlink data
            // -------------------------
            if ( iConnInfos[index].iDLDataNotifier != 0 )
                {
                if ( !iConnInfos[index].iDLDataNotifier->IsActive() )
                    {
                    iConnInfos[index].iDLDataNotifier->Receive(); // (re)start listening
                    }
                }

            // -------------------------
            // Amount of Uplink data
            // -------------------------
            if ( iConnInfos[index].iULDataNotifier != 0 )
                {
                if ( !iConnInfos[index].iULDataNotifier->IsActive() )
                    {
                    iConnInfos[index].iULDataNotifier->Receive(); // (re)start listening
                    }
                }

            // ----------------------
            // Connection Activity
            // ----------------------
            if ( iConnInfos[index].iActivityNotifier != 0 )
                {
                if ( !iConnInfos[index].iActivityNotifier->IsActive() )
                    {
                    iConnInfos[index].iActivityNotifier->Receive( iConnInfos[index].iActivity );
                    }
                }

            if ( iConnInfos[index].iBearerNotifier )
                {
                if ( !iConnInfos[index].iBearerNotifier->Listening() )
                    {
                    iConnInfos[index].iBearerNotifier->Listen();
                    }
                }
            }
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::WakeUpNotifications
// Wakes up GENERAL sleeping event watchers that depend on a threshold.
// (signal strength, bearer availability).
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::WakeUpNotifications()
    {
    if ( iGSMSignalNotifier != 0 )
        {
        if ( !iGSMSignalNotifier->IsActive() )
            {
            iGSMSignalNotifier->Receive(); // (re)start listening
            }
        }

    if ( iGSMBearerAvailabilityNotifier != 0 )
        {
        if ( !iGSMBearerAvailabilityNotifier->IsActive() )
            {
            iGSMBearerAvailabilityNotifier->Receive(); // (re)start listening
            }
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::LaunchActivityNotifierL
// Launches the activity notifier for the given connection id.
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::LaunchActivityNotifierL( const TUint& aConnectionId )
    {
    TInt index = Index( aConnectionId );

    if ( index < 0  )
        {
        return KErrNotFound;
        }

    GetActivityOneShotL( index );
    return KErrNone;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::LaunchDataNotifiers
// Launches the data notifiers (uplink & downlink) for the given connection id.
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::LaunchDataNotifiers( const TUint& aConnectionId )
    {
    TInt index = Index( aConnectionId );

    if ( index < 0  )
        {
        return KErrNotFound;
        }

    if ( iConnInfos[index].iDLDataNotifier != 0 )
        {
        if ( !iConnInfos[index].iDLDataNotifier->IsActive() )
            {
            iConnInfos[index].iDLDataNotifier->Receive();
            }
        }

    if ( iConnInfos[index].iULDataNotifier != 0 )
        {
        if ( !iConnInfos[index].iULDataNotifier->IsActive() )
            {
            iConnInfos[index].iULDataNotifier->Receive();
            }
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::LaunchDataNotifiers
// Launches the data notifiers (uplink & downlink) for the given connection id.
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::LaunchBearerNotifier( const TUint& aConnectionId )
    {
    TInt index = Index( aConnectionId );

    if ( index < 0  )
        {
        return KErrNotFound;
        }

    if ( iConnInfos[index].iBearerNotifier )
        {
        if ( !iConnInfos[index].iBearerNotifier->Listening() )
            {
            iConnInfos[index].iBearerNotifier->Listen();
            }
        }

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CConnMonIAP::AppendActiveConnections
// -----------------------------------------------------------------------------
//
void CConnMonIAP::GetActiveConnectionsIds( const TUint& aBearerId, RArray<TUint>& aIdArray )
    {
    //LOGENTRFN("CConnMonIAP::GetActiveConnectionsIds()")

    const TInt count = iConnInfos.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        // Bearer might still be uninitialized
        if ( iConnInfos[i].iBearer == EBearerUnknown )
            {
            LOGIT1("GetActiveConnectionsIds: bearer unknown for conn.id %d",
                    iConnInfos[i].iConnectionId)
            TInt err = GetBearer(
                    iConnInfos[i].iConnectionId,
                    iConnInfos[i].iBearer,
                    iConnInfos[i].iBearerInfo );

            if ( KErrNone != err )
                {
                // Skip because bearer is unknown. This connection has not yet fully started.
                continue;
                }
            }
        switch ( iConnInfos[i].iBearer )
            {
            case EBearerGPRS:
            case EBearerEdgeGPRS:
            case EBearerWCDMA:
                {
                if ( aBearerId == EBearerIdGPRS )
                    {
                    aIdArray.Append( iConnInfos[i].iIapId );
                    }
                }
                break;
            case EBearerWLAN:
                {
                if ( aBearerId == EBearerIdWLAN )
                    {
                    TInt connectionStatus( 0 );
                    GetConnectionStatus( i, connectionStatus );
                    if ( connectionStatus == KLinkLayerOpen )
                        {
                        aIdArray.Append( iConnInfos[i].iIapId );
                        }
                    }
                }
                break;
            case EBearerCSD:
            case EBearerHSCSD:
            case EBearerWcdmaCSD:
                {
                if ( aBearerId == EBearerIdCSD )
                    {
                    aIdArray.Append( iConnInfos[i].iIapId );
                    }
                }
                break;
            case EBearerLAN:
                {
                if ( aBearerId == EBearerIdLAN )
                    {
                    aIdArray.Append( iConnInfos[i].iIapId );
                    }
                }
                break;
            default:
                break;
            }
        }
    //LOGEXITFN("CConnMonIAP::GetActiveConnectionsIds()")
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::ActivePacketdataConnectionsFound
// Return ETrue if one or more active packetdata connections are found,
// EFalse otherwise.
// -----------------------------------------------------------------------------
//
TBool CConnMonIAP::ActivePacketdataConnectionsFound()
    {
    LOGENTRFN("CConnMonIAP::ActivePacketdataConnectionsFound()")
    TBool result( EFalse );
    TInt unknownBearers( 0 );

    const TInt count = iConnInfos.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        // Bearer might still be uninitialized
        if ( iConnInfos[i].iBearer == EBearerUnknown )
            {
            unknownBearers++;
            }
        else if ( iConnInfos[i].iBearer == EBearerGPRS ||
                iConnInfos[i].iBearer == EBearerEdgeGPRS ||
                iConnInfos[i].iBearer == EBearerWCDMA )
            {
            result = ETrue;
            break;
            }
        }
    if ( !result && unknownBearers )
        {
        for ( TInt i = 0; i < count; i++ )
            {
            if ( iConnInfos[i].iBearer == EBearerUnknown )
                {
                LOGIT1("ActivePacketdataConnectionsFound: bearer unknown for conn.id %d",
                        iConnInfos[i].iConnectionId)
                TInt err = GetBearer(
                        iConnInfos[i].iConnectionId,
                        iConnInfos[i].iBearer,
                        iConnInfos[i].iBearerInfo );

                if ( err == KErrNone )
                    {
                    if ( iConnInfos[i].iBearer == EBearerGPRS ||
                            iConnInfos[i].iBearer == EBearerEdgeGPRS ||
                            iConnInfos[i].iBearer == EBearerWCDMA )
                        {
                        result = ETrue;
                        break;
                        }
                    }
                }
            }
        }

    LOGEXITFN1("CConnMonIAP::ActivePacketdataConnectionsFound()", result)
    return result;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetBearerSupportInfo
// Checks whether the current cell and the phone support a bearer (GPRS or CSD).
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetBearerSupportInfo(
        const TUint aConnectionId,
        TBool& aByCell,
        TBool& aByPhone ) const
    {
    LOGENTRFN("CConnMonIAP::GetBearerSupportInfo()")
    TInt err( KErrNone );

    aByCell = EFalse;
    aByPhone = EFalse;

    if ( !iTSYLoaded  )
        {
        LOGIT("GetBearerSupportInfo: ERROR, TSY not loaded")
        LOGEXITFN1("CConnMonIAP::GetBearerSupportInfo()", err)
        return err;
        }

    RPacketService::TDynamicCapsFlags capsDynamic( 0 );
    if ( iPacketServLoaded )
        {
        err = iPacketService.GetDynamicCaps( capsDynamic );
        if ( KErrNone != err )
            {
            LOGIT1("GetBearerSupportInfo: ERROR getting dynamic caps <%d>", err)
            capsDynamic = 0;
            }
        }

    if ( aConnectionId == EBearerIdGPRS || aConnectionId == EBearerIdWCDMA )
        {
        LOGIT2("GetBearerSupportInfo: GPRS and WCDMA, dynamic caps:   0x%04X <%d>", capsDynamic, err)

        // 1. Is GPRS/WCDMA supported by this NETWORK CELL
        if ( ( capsDynamic & RPacketService::KCapsManualAttach ) ||
             ( capsDynamic & RPacketService::KCapsManualDetach ) )
            {
            // Is the phone registered on the packet network
            RPacketService::TRegistrationStatus nwRegStatus( RPacketService::EUnknown );
            TRequestStatus status( KErrNone );
            iPacketService.GetNtwkRegStatus( status, nwRegStatus );
            User::WaitForRequest( status );

            LOGIT2("GetBearerSupportInfo: pckt service ntwrk reg status:  %d <%d>", nwRegStatus, status.Int())
            if ( KErrNone == status.Int() &&
                 nwRegStatus != RPacketService::ENotRegisteredNotSearching &&
                 nwRegStatus != RPacketService::ENotRegisteredSearching    &&
                 nwRegStatus != RPacketService::ERegistrationDenied        &&
                 nwRegStatus != RPacketService::ENotRegisteredAndNotAvailable )
                {
                aByCell = ETrue;    
                }
            }

        // 2. Check the PHONE'S MODE if it is known
        TUint bearerId( 0 );
        err = GetBearerId( bearerId );

        // If phone mode query failed, mode is unknown, or bearer ID equals connection ID.
        if ( bearerId == 0 || bearerId == aConnectionId )
            {
            // 3. Check the PHONE'S MODE CAPABILITIES
            TUint32 capsPhone( 0 );
            err = iMobilePhone.GetMultimodeCaps( capsPhone );

            LOGIT2("GetBearerSupportInfo: mobile phone mode caps:         0x%04X <%d>", capsPhone, err)
            if ( KErrNone == err )
                {
                if ( aConnectionId == EBearerIdGPRS &&
                     ( capsPhone & RMobilePhone::KCapsGprsSupported ) )
                    {
                    aByPhone = ETrue;
                    }
                else if ( aConnectionId == EBearerIdWCDMA &&
                        ( capsPhone &
                        ( RMobilePhone::KCapsWcdmaSupported |
                        RMobilePhone::KCapsGprsSupported ) ) )
                    {
                    aByPhone = ETrue;
                    }
                }
            }
        }
    else if ( aConnectionId == EBearerIdCSD || aConnectionId == EBearerIdWcdmaCSD )
        {
        LOGIT2("GetBearerSupportInfo: CSD and WcdmaCSD, dynamic caps: 0x%04X <%d>", capsDynamic, err)

        // 1. Is CSD/WcdmaCSD supported by this NETWORK CELL
        if ( capsDynamic & RPacketService::KCapsRxCSCall )
            {
            // Has phone registered to the network
            TRequestStatus status( KErrNone );
            RMobilePhone::TMobilePhoneRegistrationStatus gsmReg(
                    RMobilePhone::ERegistrationUnknown );

            iMobilePhone.GetNetworkRegistrationStatus( status, gsmReg );
            User::WaitForRequest( status );

            LOGIT2("GetBearerSupportInfo: mobile phone reg status:        %d <%d>", gsmReg, status.Int())
            if ( KErrNone == status.Int() )
                {
                if ( gsmReg == RMobilePhone::ERegisteredBusy ||
                        gsmReg == RMobilePhone::ERegisteredOnHomeNetwork ||
                        gsmReg == RMobilePhone::ERegisteredRoaming )
                    {
                    aByCell = ETrue;
                    }
                }
            }

        // 2. Check the PHONE'S MODE if it is known
        TUint bearerId( 0 );
        err = GetBearerId( bearerId, ETrue );

        // If phone mode query failed, mode is unknown, or bearer ID equals connection ID.
        if ( bearerId == 0 || bearerId == aConnectionId )
            {
            // 3. Check the PHONE'S CAPABILITIES
            err = iTelServer.IsSupportedByModule(
                    KMmTsyModuleName,
                    KETelFuncMobileDataCall,
                    aByPhone );
            }
        }
    else
        {
        LOGIT1("GetBearerSupportInfo: invalid connection id: %d", aConnectionId)
        err = KErrArgument;
        }

    LOGIT2("GetBearerSupportInfo: by cell: %d, by phone: %d", aByCell, aByPhone)
    LOGEXITFN1("CConnMonIAP::GetBearerSupportInfo()", err)
    return err;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::GetBearerId
// Gets the bearer ID matching the phone's current mode.
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::GetBearerId( TUint& aBearerId, TBool aCsd ) const
    {
    aBearerId = 0;
    RMobilePhone::TMobilePhoneNetworkMode mode;
    TInt err = iMobilePhone.GetCurrentMode( mode );
    // If GetCurrentMode() failed for any reason, aBearerId will be 0.

    LOGIT2("CConnMonIAP::GetBearerId(): current phone mode %d <%d>", mode, err)
    if ( err == KErrNone )
        {
        switch ( mode )
            {
            case RMobilePhone::ENetworkModeWcdma:
                if ( aCsd )
                    {
                    aBearerId  = EBearerIdWcdmaCSD;
                    }
                else
                    {
                    aBearerId  = EBearerIdWCDMA;
                    }
                break;

            case RMobilePhone::ENetworkModeGsm:
            case RMobilePhone::ENetworkModeUnknown: // Emulator default
                if ( aCsd )
                    {
                    aBearerId = EBearerIdCSD;
                    }
                else
                    {
                    aBearerId = EBearerIdGPRS;
                    }
                break;

            case RMobilePhone::ENetworkModeCdma2000:
                aBearerId = EBearerIdCDMA2000;
                break;

            // ENetworkModeUnregistered
            // ENetworkModeAmps
            // ENetworkModeCdma95
            // ENetworkModeTdcdma
            default:
                break;
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::Reconnect
// A new connection has been established by reconnecting to an old connection.
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::Reconnect( TUint& aConnectionId )
    {
    LOGENTRFN("CConnMonIAP::Reconnect()")
    TInt err( KErrNone );

    TInt index = Index( aConnectionId );
    if ( index < 0  )
        {
        //return KErrNotFound;
        err = KErrNotFound;
        }
    else
        {
        // Prevent ConnDown notifier from being deleted
        // It will destroy itself later in reconnect case.
        iConnInfos[index].iConnDownNotifier = 0;
        TBearerInfo bearerInfo( EBearerInfoUnknown, EFalse );

        TConnInfo connInfo( iConnInfos[index].iIapId, iConnInfos[index].iNetId, 0, 0, bearerInfo );

        // AddConnectionL will remove the old connection
        TInt err2( KErrNone );
        TRAP( err2, ( err = AddConnectionL( connInfo ) ) );
        if ( ( err2 != KErrNone ) || ( err != KErrNone ) )
            {
            err = KErrUnknown;
            }
        else
            {
            aConnectionId = connInfo.iConnectionId;
            }
        }

    LOGEXITFN1("CConnMonIAP::Reconnect()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::WlanSupportCheck
//
// Returns KErrNone if the given connection ID is EBearerIdWLAN, or belongs to
// an active connection where bearer has been marked as EBearerIdWLAN.
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::WlanSupportCheck( const TUint& aConnectionId )
    {
    LOGENTRFN("CConnMonIAP::WlanSupportCheck()")
    TInt ret( KErrNotSupported );

    // Check if WLAN is supported at all
    if ( !iWlanSupport )
        {
        LOGIT("WlanSupportCheck: no WLAN support")
        ret = KErrNotSupported;
        }
    else
        {
        if ( EBearerIdWLAN == aConnectionId )
            {
            ret = KErrNone;
            }
        else
            {
            // Find connection matching the given ID
            TInt index = Index( aConnectionId );
            LOGIT2("WlanSupportCheck: checking connection id %d, index %d ", aConnectionId, index)
            if ( index < 0 )
                {
                ret = KErrArgument;
                }
            else
                {
                // Connection found, check if bearer is EBearerWLAN
                if ( EBearerWLAN == iConnInfos[index].iBearer )
                    {
                    ret = KErrNone;
                    }
                else
                    {
                    LOGIT1("WlanSupportCheck: connection bearer %d is not WLAN", iConnInfos[index].iBearer)
                    ret = KErrArgument;
                    }
                }
            }
        }

    LOGEXITFN1("CConnMonIAP::WlanSupportCheck()", ret)
    return ret;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::AppendAvailableWLANIaps
// Gets available WLAN IAPs from WLAN engine.
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::AppendAvailableWLANIaps( RArray<TUint>& aIdArray )
    {
    //LOGENTRFN("CConnMonIAP::AppendAvailableWLANIaps()")
    TInt err( KErrNone );

    if ( !iWlanSupport )
        {
        LOGIT("AppendAvailableWLANIaps: No WLAN support")
        err = KErrNotSupported;
        }
    else
        {
        err = iWlanSupport->AppendAvailableIaps( aIdArray );
        }

    //LOGEXITFN1("CConnMonIAP::AppendAvailableWLANIaps()", err)
    return err;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::AppendAvailableWLANIapsBySsid
// Gets available WLAN IAPs from CommsDat, matching with current WLAN connection SSID.
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::AppendAvailableWLANIapsBySsid( RArray<TUint>& aIdArray )
    {
    //LOGENTRFN("CConnMonIAP::AppendAvailableWLANIapsBySsid()")
    TInt err( KErrNone );

    if ( !iWlanSupport )
        {
        LOGIT("AppendAvailableWLANIapsBySsid: No WLAN support")
        err = KErrNotSupported;
        }
    else
        {
        TRAPD( err2, err = iWlanSupport->AppendAvailableIapsBySsidL( aIdArray ) );
        if ( err2 != KErrNone )
            {
            LOGIT1("AppendAvailableWLANIapsBySsid: error, LEAVE <%d> from AppendAvailableIapsBySsidL()", err2)
            err = err2;
            }
        }

    //LOGEXITFN1("CConnMonIAP::AppendAvailableWLANIapsBySsid()", err)
    return err;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::EnableWlanScan
// Enables WLAN scan for the next iap availability query.
// -----------------------------------------------------------------------------------
//
void CConnMonIAP::EnableWlanScan()
    {
    if ( iWlanSupport )
        {
        iWlanSupport->EnableWlanScan();
        }
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::DtmStateChanged
// -----------------------------------------------------------------------------
//
void CConnMonIAP::DtmStateChanged()
    {
    LOGENTRFN("CConnMonIAP::DtmStateChanged()")

    TEventInfo eventInfo;
    eventInfo.Reset();

    // When not in dual mode and call is active, gprs and wcdma bearers are unavailable
    // Note that even the WCDMA can be unavailable, if the phone doesn't support DTM
    if ( iDualTransferModeNotifier && !iDualTransferModeNotifier->IsInDualMode() &&
            iTelephonyNotifier && iTelephonyNotifier->IsCallActive() )
        {
        LOGIT("SERVER: DtmStateChanged: Packet data UNAVAILABLE (2G&3G), no DTM and call is active")
        eventInfo.iEventType = EConnMonPacketDataUnavailable;

        // Send event to all clients that are listening for bearer EBearerIdGPRS
        LOGIT("SERVER: EVENT -> Packet data UNAVAILABLE (GPRS), no DTM and call is active")
        eventInfo.iConnectionId = EBearerIdGPRS;
        iServer->EventQueue()->Add( eventInfo ); // Create bearer availability event if needed

        // Send event to all clients that are listening for bearer EBearerIdWCDMA
        LOGIT("SERVER: EVENT -> Packet data UNAVAILABLE (WCDMA), no DTM and call is active")
        eventInfo.iConnectionId = EBearerIdWCDMA;
        iServer->EventQueue()->Add( eventInfo );
        }
    else
        {
        TBool byPhone( EFalse );
        TBool byCell( EFalse );

        // Lets check if GPRS bearer is available at all
        TInt ret = GetBearerSupportInfo( EBearerIdGPRS, byCell, byPhone );
        LOGIT2("Bearer support (GPRS): byCell %d, byPhone %d", byCell, byPhone)
        if ( ret != KErrNone )
            {
            LOGIT1("SERVER: DtmStateChanged: GetBearerSupportInfo (GPRS) returned error <%d>", ret)
            return; // No event on error
            }

        eventInfo.iConnectionId = EBearerIdGPRS;
        if ( byCell && byPhone )
            {
            LOGIT("SERVER: EVENT -> Packet data AVAILABLE (GPRS), DTM or no call is active")
            eventInfo.iEventType = EConnMonPacketDataAvailable;
            }
        else
            {
            LOGIT("SERVER: EVENT -> Packet data UNAVAILABLE (GPRS), no GPRS bearer")
            eventInfo.iEventType = EConnMonPacketDataUnavailable;
            }
        // Send event to all clients that are listening for bearer EBearerIdGPRS
        iServer->EventQueue()->Add( eventInfo );


        // Same for WCDMA as for GPRS above
        ret = GetBearerSupportInfo( EBearerIdWCDMA, byCell, byPhone );
        LOGIT2("Bearer support (WCDMA): byCell %d, byPhone %d", byCell, byPhone)
        if ( ret != KErrNone )
            {
            LOGIT1("SERVER: DtmStateChanged: GetBearerSupportInfo (WCDMA) returned error <%d>", ret)
            return; // No event on error
            }

        eventInfo.iConnectionId = EBearerIdWCDMA;
        if ( byCell && byPhone )
            {
            LOGIT("SERVER: EVENT -> Packet data AVAILABLE (WCDMA), DTM or no call is active")
            eventInfo.iEventType = EConnMonPacketDataAvailable;
            }
        else
            {
            LOGIT("SERVER: EVENT -> Packet data UNAVAILABLE (WCDMA), no WCDMA bearer")
            eventInfo.iEventType = EConnMonPacketDataUnavailable;
            }
        // Send event to all clients that are listening for bearer EBearerIdWCDMA
        iServer->EventQueue()->Add( eventInfo );
        }

    LOGEXITFN("CConnMonIAP::DtmStateChanged()")
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::PhoneLineStatusChange
// -----------------------------------------------------------------------------
//
void CConnMonIAP::PhoneLineStatusChange()
    {
    LOGENTRFN("CConnMonIAP::PhoneLineStatusChange()")

    // At this time the implementation would be identical
    // to DtmStateChanged so we just call it
    DtmStateChanged();

    LOGEXITFN("CConnMonIAP::PhoneLineStatusChange()")
    }


// PRIVATE METHODS BELOW

// -----------------------------------------------------------------------------------
// CConnMonIAP::OpenAndAttachTo
// Opens and attaches an RConnection instance.
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::OpenAndAttachTo( TConnInfo& aConnInfo )
    {
    //LOGENTRFN("CConnMonIAP::OpenAndAttachTo()")
    TInt err( KErrNone );
    TConnectionInfo info;
    TPckg<TConnectionInfo> pckgInfo( info );

    // Leave left out intentionally, check for NULL instead
    aConnInfo.iConnAttach = new RConnection(); // No (ELeave)
    if ( !aConnInfo.iConnAttach )
        {
        err = KErrNoMemory;
        }
    if ( KErrNone == err )
        {
        err = aConnInfo.iConnAttach->Open( iSocketServer, KAfInet );
        }

    if ( KErrNone == err )
        {
        info.iIapId = aConnInfo.iIapId;
        info.iNetId = aConnInfo.iNetId;

        err = aConnInfo.iConnAttach->Attach( pckgInfo, RConnection::EAttachTypeMonitor );
        if ( err != KErrNone )
            {
            aConnInfo.iConnAttach->Close();
            delete aConnInfo.iConnAttach;
            }
        }

    //LOGEXITFN1("CConnMonIAP::OpenAndAttachTo()", err)
    return err;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::StopConnection
// Closes a connection.
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::StopConnection( const TInt aIndex )
    {
    LOGENTRFN("CConnMonIAP::StopConnection()")
    TInt err( KErrNone );

    if ( ( aIndex < 0 ) || ( aIndex >= iConnInfos.Count() ) )
        {
        LOGEXITFN1("CConnMonIAP::StopConnection()", KErrArgument)
        return KErrArgument;
        }

    if ( iConnInfos[aIndex].iBearer < EBearerExternalCSD )
        {
        // Stop an internal connection
        RConnection connection;
        TConnectionInfo info;

        // Cancel data volume and activity notifiers before stopping
        if ( iConnInfos[aIndex].iDLDataNotifier != 0 )
            {
            iConnInfos[aIndex].iDLDataNotifier->Cancel();
            }
        if ( iConnInfos[aIndex].iULDataNotifier != 0 )
            {
            iConnInfos[aIndex].iULDataNotifier->Cancel();
            }
        if ( iConnInfos[aIndex].iDataVolumeAO != 0 )
            {
            iConnInfos[aIndex].iDataVolumeAO->Cancel();
            }
        if ( iConnInfos[aIndex].iActivityNotifier != 0 )
            {
            iConnInfos[aIndex].iActivityNotifier->Cancel();
            }

        // RConnection object with EAttachTypeNormal is needed for stopping
        err = connection.Open( iSocketServer, KAfInet );
        if ( KErrNone == err )
            {
            info.iIapId = iConnInfos[aIndex].iIapId;
            info.iNetId = iConnInfos[aIndex].iNetId;

            err = connection.Attach( TPckg<TConnectionInfo>( info ),
                    RConnection::EAttachTypeNormal );
            if ( KErrNone == err )
                {
                err = connection.Stop( RConnection::EStopAuthoritative );
                if ( KErrNone == err )
                    {
                    // Stop notifications, delete connection settings and remove from the table
                    // If conn up/down notifier is active, let the event remove connection later.
                    if ( !iConnUpDownNotifier || !iConnUpDownNotifier->IsActive() )
                        {
                        RemoveConnection( iConnInfos[aIndex] );
                        }
                    }
                else
                    {
                    LOGIT1("StopConnection: RConnection::Stop() failed <%d>", err)
                    }
                }
            else
                {
                LOGIT1("StopConnection: RConnection::Attach() failed <%d>", err)
                }
            connection.Close();
            }
        else
            {
            LOGIT1("StopConnection: RConnection::Open() failed <%d>", err)
            }
        }

    else if ( iConnInfos[aIndex].iBearer == EBearerExternalGPRS     ||
              iConnInfos[aIndex].iBearer == EBearerExternalEdgeGPRS ||
              iConnInfos[aIndex].iBearer == EBearerExternalWCDMA    ||
              iConnInfos[aIndex].iBearer == EBearerExternalCDMA2000 ||
              ( !iConnInfos[aIndex].iBearerInfo.iInternal &&
                ( iConnInfos[aIndex].iBearerInfo.iBearer == EBearerInfoHSDPA ||
                  iConnInfos[aIndex].iBearerInfo.iBearer == EBearerInfoHSUPA ||
                  iConnInfos[aIndex].iBearerInfo.iBearer == EBearerInfoHSxPA ) ) )
        {
        // EXTERNAL PSD
        if ( iPsdFax )
            {
            err = iPsdFax->Stop( iConnInfos[aIndex].iConnectionId );
            }
        else
            {
            err = KErrNotFound;
            }
        }

    else if ( iConnInfos[aIndex].iBearer == EBearerExternalCSD   ||
              iConnInfos[aIndex].iBearer == EBearerExternalHSCSD ||
              iConnInfos[aIndex].iBearer == EBearerExternalWcdmaCSD )
        {
        // EXTERNAL CSD
        if ( iCsdFax )
            {
            err = iCsdFax->Stop();
            }
        else
            {
            err = KErrNotFound;
            }
        }
    else
        {
        err = KErrNotSupported;
        }

    LOGEXITFN1("CConnMonIAP::StopConnection()", err)
    return err;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::AddConnNotificationsL
// Adds connection specific event watchers (connection status, data volume, activity).
// -----------------------------------------------------------------------------------
//
void CConnMonIAP::AddConnNotificationsL( const TInt& aIndex )
    {
    LOGENTRFN("CConnMonIAP::AddConnNotificationsL()")

    if ( iConnInfos[aIndex].iBearer < EBearerExternalCSD )
        {
        // INTERNAL CONNECTIONS
        TInt connectionStatus( 0 );
        GetConnectionStatus( aIndex, connectionStatus );

        // ----------------------
        // Connection Status
        // ----------------------
        if ( iConnInfos[aIndex].iProgressNotifier == 0 )
            {
            TInt subConnectionId( 0 );

            // Create an active object and add it to scheduler
            iConnInfos[aIndex].iProgressNotifier = new( ELeave ) CProgressNotifier(
                    iServer,
                    iConnInfos[aIndex].iConnAttach,
                    iConnInfos[aIndex].iConnectionId,
                    subConnectionId );

            iConnInfos[aIndex].iProgressNotifier->Construct();

            if ( iConnUpDownNotifier->IsActive() )
                {
                // Client is starting notifications. Filter old status events.
                iConnInfos[aIndex].iProgressNotifier->Receive( connectionStatus );
                }
            else
                {
                // Event is starting notifification. No filtering is needed.
                iConnInfos[aIndex].iProgressNotifier->Receive();
                }
            }

        // -------------------------
        // Amount of Downlink data
        // -------------------------
        if ( iConnInfos[aIndex].iDLDataNotifier == 0 )
            {
            TInt subConnectionId( 0 );

            // Create an active object and add it to scheduler
            iConnInfos[aIndex].iDLDataNotifier = new( ELeave ) CDataNotifier(
                    iServer,
                    iConnInfos[aIndex].iConnAttach,
                    iConnInfos[aIndex].iConnectionId,
                    subConnectionId,
                    CDataNotifier::EDownlink);

            iConnInfos[aIndex].iDLDataNotifier->Construct();

            if ( ( connectionStatus == KLinkLayerOpen ) ||
                 ( connectionStatus == KPsdSuspended ) )
                {
                iConnInfos[aIndex].iDLDataNotifier->Receive();
                }
            }

        // -------------------------
        // Amount of Uplink data
        // -------------------------
        if ( iConnInfos[aIndex].iULDataNotifier == 0 )
            {
            TInt subConnectionId( 0 );

            // Create an active object and add it to scheduler
            iConnInfos[aIndex].iULDataNotifier = new( ELeave ) CDataNotifier(
                    iServer,
                    iConnInfos[aIndex].iConnAttach,
                    iConnInfos[aIndex].iConnectionId,
                    subConnectionId,
                    CDataNotifier::EUplink);

            iConnInfos[aIndex].iULDataNotifier->Construct();

            if ( ( connectionStatus == KLinkLayerOpen ) ||
                 ( connectionStatus == KPsdSuspended ) )
                {
                iConnInfos[aIndex].iULDataNotifier->Receive();
                }
            }

        // ---------------------------------------
        // Connection down event with data volumes
        // ---------------------------------------
        if ( iConnInfos[aIndex].iConnDownNotifier == 0 )
            {
            TInt subConnectionId( 0 );

            // Create an active object and add it to scheduler
            iConnInfos[aIndex].iConnDownNotifier = new( ELeave ) CSubConnUpDownNotifier(
                    iServer,
                    iConnInfos[aIndex].iConnAttach,
                    iConnInfos[aIndex].iConnectionId,
                    subConnectionId );

            iConnInfos[aIndex].iConnDownNotifier->Construct();

            // Receiving will start only when connection status reaches KLinkLayerOpen
            if ( ( connectionStatus == KLinkLayerOpen ) ||
                 ( connectionStatus == KPsdSuspended ) )
                {
                iConnInfos[aIndex].iConnDownNotifier->Receive();
                }
            }

        // ----------------------
        // Connection Activity
        // ----------------------
        if ( iConnInfos[aIndex].iActivityNotifier == 0 )
            {
            // To update internal table fast at the first time
            if ( ( connectionStatus == KLinkLayerOpen ) ||
                 ( connectionStatus == KPsdSuspended ) )
                {
                GetActivityOneShotL( aIndex );
                }
            }
        else
            {
            if ( !iConnInfos[aIndex].iActivityNotifier->IsActive() )
                {
                iConnInfos[aIndex].iActivityNotifier->Receive( iConnInfos[aIndex].iActivity );
                }
            }

        // ----------------------
        // Connection Bearer
        // ----------------------
        if ( iConnInfos[aIndex].iBearerNotifier == 0 )
            {
            // Create an active object and add it to scheduler
            iConnInfos[aIndex].iBearerNotifier = CConnMonBearerNotifier::NewL(
                    iServer,
                    iMobilePhone,
                    iConnInfos[aIndex].iConnectionId );

            // Receiving will start only when connection status reaches KLinkLayerOpen
            if ( ( connectionStatus == KLinkLayerOpen ) ||
                 ( connectionStatus == KPsdSuspended ) )
                {
                iConnInfos[aIndex].iBearerNotifier->Listen();
                }
            }

        // Note. iDataVolumeAO is created when client asks data volume info
        }
    LOGEXITFN("CConnMonIAP::AddConnNotificationsL()")
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::DeleteConnNotifications
// Deletes event watchers owned by a connection
// -----------------------------------------------------------------------------------
//
void CConnMonIAP::DeleteConnNotifications( const TInt& aIndex )
    {
    LOGENTRFN("CConnMonIAP::DeleteConnNotifications()")
    if ( iConnInfos[ aIndex ].iBearer < EBearerExternalCSD )
        {
        if ( iConnInfos[ aIndex ].iProgressNotifier )
            {
            iConnInfos[ aIndex ].iProgressNotifier->Cancel();
            delete iConnInfos[ aIndex ].iProgressNotifier;
            iConnInfos[ aIndex ].iProgressNotifier = NULL;
            }

        if ( iConnInfos[ aIndex ].iDLDataNotifier )
            {
            iConnInfos[ aIndex ].iDLDataNotifier->Cancel();
            delete iConnInfos[ aIndex ].iDLDataNotifier;
            iConnInfos[ aIndex ].iDLDataNotifier = NULL;
            }

        if ( iConnInfos[ aIndex ].iULDataNotifier )
            {
            iConnInfos[ aIndex ].iULDataNotifier->Cancel();
            delete iConnInfos[ aIndex ].iULDataNotifier;
            iConnInfos[ aIndex ].iULDataNotifier = NULL;
            }

        if ( iConnInfos[ aIndex ].iDataVolumeAO )
            {
            iConnInfos[ aIndex ].iDataVolumeAO->Cancel();
            delete iConnInfos[ aIndex ].iDataVolumeAO;
            iConnInfos[ aIndex ].iDataVolumeAO = NULL;
            }

        if ( iConnInfos[ aIndex ].iConnDownNotifier )
            {
            iConnInfos[ aIndex ].iConnDownNotifier->Cancel();
            delete iConnInfos[ aIndex ].iConnDownNotifier;
            iConnInfos[ aIndex ].iConnDownNotifier = NULL;
            }

        if ( iConnInfos[ aIndex ].iActivityNotifier )
            {
            iConnInfos[ aIndex ].iActivityNotifier->Cancel();
            delete iConnInfos[ aIndex ].iActivityNotifier;
            iConnInfos[ aIndex ].iActivityNotifier = NULL;
            }

        if ( iConnInfos[ aIndex ].iBearerNotifier )
            {
            iConnInfos[ aIndex ].iBearerNotifier->CancelListen();
            delete iConnInfos[ aIndex ].iBearerNotifier;
            iConnInfos[ aIndex ].iBearerNotifier = NULL;
            }
        }
    LOGEXITFN("CConnMonIAP::DeleteConnNotifications()")
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::DeleteNotifications
// Deletes all the event watchers from the system (expect connection up/down events)
// -----------------------------------------------------------------------------------
//
void CConnMonIAP::DeleteNotifications()
    {
    if( iTelephonyNotifier )
        {
        delete iTelephonyNotifier;
        iTelephonyNotifier = NULL;
        }

    if( iDualTransferModeNotifier )
        {
        delete iDualTransferModeNotifier;
        iDualTransferModeNotifier = NULL;
        }

    if ( iPsdNetwStatusNotifier )
        {
        // Cancel any outstanding requests
        iPsdNetwStatusNotifier->Cancel();
        delete iPsdNetwStatusNotifier;
        iPsdNetwStatusNotifier = NULL;
        }

    if ( iNetwRegistrationNotifier )
        {
        // Cancel any outstanding requests
        iNetwRegistrationNotifier->Cancel();
        delete iNetwRegistrationNotifier;
        iNetwRegistrationNotifier = NULL;
        }

    if ( iEdgeNotifier )
        {
        // Cancel any outstanding requests
        iEdgeNotifier->Cancel();
        delete iEdgeNotifier;
        iEdgeNotifier = NULL;
        }

    if ( iWcdmaNotifier )
        {
        iWcdmaNotifier->Cancel();
        delete iWcdmaNotifier;
        iWcdmaNotifier = NULL;
        }

    if ( iModeNotifier )
        {
        // Cancel any outstanding requests
        iModeNotifier->Cancel();
        delete iModeNotifier;
        iModeNotifier = NULL;
        }

    if ( iGSMSignalNotifier )
        {
        // Cancel any outstanding requests
        iGSMSignalNotifier->Cancel();
        delete iGSMSignalNotifier;
        iGSMSignalNotifier = NULL;
        }

    if ( iGSMBearerAvailabilityNotifier )
        {
        // Cancel any outstanding requests
        iGSMBearerAvailabilityNotifier->Cancel();
        delete iGSMBearerAvailabilityNotifier;
        iGSMBearerAvailabilityNotifier = NULL;
        }

    if ( iWlanSupport )
        {
        iWlanSupport->DisableEventsToClients();
        }

    // Delete notifications owned by the connections
    TInt count = iConnInfos.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        DeleteConnNotifications( i );
        }
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::NewConnectionId
// Returns a new unique connection id for the connection.
// -----------------------------------------------------------------------------------
//
TUint CConnMonIAP::NewConnectionId()
    {
    ++iIdCounter;

    if ( iIdCounter == KMaxConnectionId )
        {
        iIdCounter = 1;
        }

    return iIdCounter;
    }

// -----------------------------------------------------------------------------------
// CConnMonIAP::Index
// Finds the index of the matching object within the array.
// KErrNotFound, if no matching object can be found.
// -----------------------------------------------------------------------------------
//
TInt CConnMonIAP::Index( const TUint& aConnectionId )
    {
    TInt count = iConnInfos.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iConnInfos[i].iConnectionId == aConnectionId )
            {
            return i;
            }
        }

    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetAccessPointName
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetAccessPointName( const TUint& aConnectionId, TDes& aName )
    {
    LOGENTRFN("CConnMonIAP::GetAccessPointName()")
    TInt err( KErrNone );

    // Find connection matching the given Id
    TInt index = Index( aConnectionId );
    if ( index < 0  )
        {
        err = KErrNotFound;
        }
    else
        {
        // Find out access point name
        TBuf<KCommsDbSvrMaxFieldLength> settingName;
        TBuf<KCommsDbSvrMaxFieldLength> settingValue;
        settingName.Copy( TPtrC( IAP ) );
        settingName.Append( TChar( '\\' ) );
        settingName.Append( TPtrC( IAP_SERVICE_TYPE ) );

        err = iConnInfos[index].iConnAttach->GetDesSetting(
                TPtrC( settingName ),
                settingValue );
        if ( KErrNone == err )
            {
            if ( settingValue == TPtrC( OUTGOING_WCDMA ) ||
                    settingValue == TPtrC( INCOMING_WCDMA ) )
                {
                settingName.Copy( settingValue );
                settingName.Append( TChar( '\\' ) );
                settingName.Append( TPtrC( GPRS_APN ) );

                err = iConnInfos[index].iConnAttach->GetDesSetting(
                        TPtrC( settingName ),
                        aName );
                }
            else
                {
                err = KErrNotFound;
                }
            }
        }

    LOGEXITFN1("CConnMonIAP::GetAccessPointName()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetTelNumber
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetTelNumber( const TUint& aConnectionId, TDes& aTelNum )
    {
    LOGENTRFN("CConnMonIAP::GetTelNumber()")
    TInt err( KErrNone );

    // Find connection matching the given Id
    TInt index = Index( aConnectionId );
    if ( index < 0  )
        {
        err = KErrNotFound;
        }
    else
        {
        // Find out tel. number
        TBuf<KCommsDbSvrMaxFieldLength> settingName;
        TBuf<KCommsDbSvrMaxFieldLength> settingValue;
        settingName.Copy( TPtrC( IAP ) );
        settingName.Append( TChar( '\\' ) );
        settingName.Append( TPtrC( IAP_SERVICE_TYPE ) );

        err = iConnInfos[index].iConnAttach->GetDesSetting(
                TPtrC( settingName ),
                settingValue );
        if ( KErrNone == err )
            {
            if ( settingValue == TPtrC( DIAL_OUT_ISP ) ||
                    settingValue == TPtrC( DIAL_IN_ISP ) )
                {
                settingName.Copy( settingValue );
                settingName.Append( TChar( '\\' ) );
                settingName.Append( TPtrC( ISP_DEFAULT_TEL_NUM ) );

                err = iConnInfos[index].iConnAttach->GetDesSetting(
                        TPtrC( settingName ),
                        aTelNum );
                }
            else
                {
                err = KErrNotFound;
                }
            }
        }

    LOGEXITFN1("CConnMonIAP::GetTelNumber()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetConnectionStatus
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetConnectionStatus( const TInt& aIndex, TInt& aConnectionStatus )
    {
    //LOGENTRFN("CConnMonIAP::GetConnectionStatus()")
    TInt err( KErrNone );
    TNifProgress nifProgress;

    if ( ( aIndex < 0 ) || ( aIndex >= iConnInfos.Count() ) )
        {
        LOGIT1("GetConnectionStatus: invalid index value: %d", aIndex)
        err = KErrArgument;
        }
    else
        {
        aConnectionStatus = 0;
        err = iConnInfos[aIndex].iConnAttach->Progress( nifProgress );
        if ( KErrNone == err )
            {
            aConnectionStatus = nifProgress.iStage;
            LOGIT1("GetConnectionStatus: iConnAttach->Progress() status: %d", aConnectionStatus)
            }
        else
            {
            LOGIT1("GetConnectionStatus: ERROR getting progress status <%d>", err)
            }
        }

    //LOGEXITFN1("CConnMonIAP::GetConnectionStatus()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetPsdNetworkStatus
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetPsdNetworkStatus( TInt& aNetworkStatus ) const
    {
    LOGENTRFN("CConnMonIAP::GetPsdNetworkStatus()")
    TInt err( KErrNone );

    if ( iTSYLoaded && iPacketServLoaded )
        {
        RPacketService::TStatus packetStatus;
        err = iPacketService.GetStatus( packetStatus );
        if ( KErrNone == err )
            {
            aNetworkStatus = CalculateNetworkStatus( packetStatus );
            LOGIT1("GetPsdNetworkStatus: status %d", aNetworkStatus)
            }
        }
    else
        {
        aNetworkStatus = EConnMonStatusUnattached;
        }

    LOGEXITFN1("CConnMonIAP::GetPsdNetworkStatus()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetNetworkRegistrationMode
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetNetworkRegistrationMode( TInt& aRegistration ) const
    {
    return GetNetworkRegistration( aRegistration );
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetNetworkRegistration
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetNetworkRegistration( TInt& aRegistration ) const
    {
    LOGENTRFN("CConnMonIAP::GetNetworkRegistration()")
    TInt err( KErrNone );

    if ( !FeatureManager::FeatureSupported( KFeatureIdNetworkRegistration ) )
        {
        LOGIT("GetNetworkRegistration: KFeatureIdNetworkRegistration not supported")
        err = KErrNotSupported;
        }
    else
        {
        if ( iTSYLoaded )
            {
            TRequestStatus status( KErrNone );
            RMobilePhone::TMobilePhoneRegistrationStatus registration(
                    RMobilePhone::ERegistrationUnknown );

            iMobilePhone.GetNetworkRegistrationStatus( status, registration );
            User::WaitForRequest( status );
            err = status.Int();
            if ( KErrNone == err )
                {
                aRegistration = CalculateNetworkRegistration( registration );
                LOGIT1("GetNetworkRegistration: registration %d", aRegistration)
                }
            }
        else
            {
            aRegistration = ENetworkRegistrationNotAvailable;
            }
        }

    LOGEXITFN1("CConnMonIAP::GetNetworkRegistration()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetNetworkRegistration_v2
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetNetworkRegistration_v2( TInt& aRegistration ) const
    {
    LOGENTRFN("CConnMonIAP::GetNetworkRegistration_v2()")
    TInt err( KErrNone );

    if ( !FeatureManager::FeatureSupported( KFeatureIdNetworkRegistration ) )
        {
        LOGIT("GetNetworkRegistration_v2: KFeatureIdNetworkRegistration not supported")
        err = KErrNotSupported;
        }
    else
        {
        if ( iTSYLoaded )
            {
            TRequestStatus status( KErrNone );
            RMobilePhone::TMobilePhoneRegistrationStatus registration(
                    RMobilePhone::ERegistrationUnknown );

            iMobilePhone.GetNetworkRegistrationStatus( status, registration );
            User::WaitForRequest( status );
            err = status.Int();
            if ( KErrNone == err )
                {
                aRegistration = CalculateNetworkRegistration_v2( registration );
                LOGIT1("GetNetworkRegistration_v2: registration <%d>", aRegistration )
                }
            }
        else
            {
            aRegistration = ENetworkRegistrationExtNotAvailable;
            }
        }

    LOGEXITFN1("CConnMonIAP::GetNetworkRegistration_v2()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetGSMSignalStrength
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetGSMSignalStrength( TInt& aSignalStrength ) const
    {
    LOGENTRFN("CConnMonIAP::GetGSMSignalStrength()")
    TInt err( KErrNone );

    if ( iTSYLoaded )
        {
        TRequestStatus status( KErrNone );
        TInt32 strength( 0 );
        TInt8 bar( -1 );

        iMobilePhone.GetSignalStrength( status, strength, bar );
        User::WaitForRequest( status );

        err = status.Int();
        if ( KErrNone == err )
            {
            aSignalStrength = strength;
            LOGIT1("GetGSMSignalStrength: strength %d", aSignalStrength)
            }
        }
    else
        {
        err = KErrNotSupported;
        }

    LOGEXITFN1("CConnMonIAP::GetGSMSignalStrength()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetProtocolTypeL
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetProtocolTypeL( const TUint& aConnectionId, TInt& aProtocolType )
    {
    LOGENTRFN("CConnMonIAP::GetProtocolTypeL()")
    TInt err( KErrNone );

    // Find connection matching the given Id
    TInt index = Index( aConnectionId );
    if ( index < 0  )
        {
        LOGEXITFN1("CConnMonIAP::GetProtocolTypeL()", KErrNotFound)
        return KErrNotFound;
        }

    aProtocolType = EProtocolTypeUnknown;

    if ( iTSYLoaded && iPacketServLoaded )
        {
        // Get AP name for the connection
        TBuf<KCommsDbSvrMaxFieldLength> apName;

        err = GetAccessPointName( iConnInfos[index].iConnectionId, apName );

        if ( KErrNotFound == err )
            {
            LOGEXITFN1("CConnMonIAP::GetProtocolTypeL()", err)
            return err;
            }

        // Enumerate all PDP contexts
        TRequestStatus status( KErrNone );
        TInt contextCount( 0 );
        TInt maxContextCount( 0 );

        iPacketService.EnumerateContexts( status, contextCount, maxContextCount );
        User::WaitForRequest( status );

        err = status.Int();
        if ( err != KErrNone )
            {
            LOGEXITFN1("CConnMonIAP::GetProtocolTypeL()", err)
            return err;
            }

        RPacketService::TContextInfo contextInfo;
        RPacketContext context;
        CleanupClosePushL( context );

        // Find PDP contexts matching the given connectionId
        for ( TInt i = 0; ( i < contextCount ) && ( aProtocolType != EProtocolTypeIP ); i++ )
            {
            iPacketService.GetContextInfo( status, i, contextInfo ); //iName, iStatus
            User::WaitForRequest( status );

            err = status.Int();
            if ( err != KErrNone )
                {
                LOGEXITFN1("CConnMonIAP::GetProtocolTypeL()", err)
                return err;
                }

            err = context.OpenExistingContext( iPacketService, contextInfo.iName );
            if ( err != KErrNone )
                {
                LOGEXITFN1("CConnMonIAP::GetProtocolTypeL()", err)
                return err;
                }

            RPacketContext::TContextConfigGPRS* getParams;
            getParams = new( ELeave ) RPacketContext::TContextConfigGPRS();
            TPckg<RPacketContext::TContextConfigGPRS> getContextConfigPckg( *getParams );

            context.GetConfig( status, getContextConfigPckg );
            User::WaitForRequest( status );

            err = status.Int();
            if ( KErrNone == err )
                {
                TInt ret = CompareAccessPointNames( apName, getParams->iAccessPointName );

                if ( ret == 0 )
                    {
                    // PDP context matching the IAP connection found
                    if ( getParams->iPdpType == RPacketContext::EPdpTypeIPv4 )
                        {
                        if ( aProtocolType == EProtocolTypeIPv6 )
                            {
                            aProtocolType = EProtocolTypeIP;    // Both 4 & 6
                            }
                        else
                            {
                            aProtocolType = EProtocolTypeIPv4;
                            }
                        }
                    else if ( getParams->iPdpType == RPacketContext::EPdpTypeIPv6 )
                        {
                        if ( aProtocolType == EProtocolTypeIPv4 )
                            {
                            aProtocolType = EProtocolTypeIP;    // Both 4 & 6
                            }
                        else
                            {
                            aProtocolType = EProtocolTypeIPv6;
                            }
                        }
                    else
                        {
                        aProtocolType = EProtocolTypeUnknown;
                        }
                    }
                }

            delete getParams;
            context.Close();

            if ( err != KErrNone )
                {
                break;
                }
            } // for
        CleanupStack::PopAndDestroy( &context );
        } // if

    LOGEXITFN1("CConnMonIAP::GetProtocolTypeL()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetActivityOneShotL
// -----------------------------------------------------------------------------
//
void CConnMonIAP::GetActivityOneShotL( const TInt& aIndex )
    {
    if ( iConnInfos[aIndex].iActivityNotifier == 0 )
        {
        iConnInfos[aIndex].iActivityNotifier = new( ELeave ) CActivityNotifier(
                iServer,
                iConnInfos[aIndex].iConnAttach,
                iConnInfos[aIndex].iConnectionId,
                0 );
        iConnInfos[aIndex].iActivityNotifier->Construct();
        }

    if ( !iConnInfos[aIndex].iActivityNotifier->IsActive() )
        {
        iConnInfos[aIndex].iActivityNotifier->ReceiveOne(
                iConnInfos[aIndex].iActivity );

        // This AO will cancel the activity notifier after one second in case it has not completed.
        // AO will destroy itself in RunL.
        CTimerAO* timerAO = new( ELeave ) CTimerAO(
                iServer,
                iConnInfos[aIndex].iConnectionId,
                iConnInfos[aIndex].iActivity );

        CleanupStack::PushL( timerAO );
        timerAO->ConstructL();
        CleanupStack::Pop( timerAO );

        timerAO->Start();
        }
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::GetBearerFromETel
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::GetBearerFromETel(
        const TDes& aNumber,
        TInt& aBearer,
        TBearerInfo& aBearerInfo )
    {
    TInt ret( KErrNone );
    TInt countCalls( 0 );
    RLine line;
    RMobileCall call;

    // Get phone info
    RTelServer::TPhoneInfo phoneInfo;
    ret = iTelServer.GetPhoneInfo( 0, phoneInfo );
    if ( KErrNone != ret )
        {
        return ret;
        }

    ret = line.Open( iMobilePhone, KMmTsyDataLineName );
    if ( KErrNone != ret )
        {
        return ret;
        }

    // Enumerate calls on data line
    ret = line.EnumerateCall( countCalls );
    if ( KErrNone != ret )
        {
        line.Close();
        return ret;
        }

    // Get call info
    for ( TInt j = 0; j < countCalls; j++ )
        {
        RLine::TCallInfo callInfo;

        ret = line.GetCallInfo( j, callInfo );
        if ( KErrNone != ret )
            {
            line.Close();
            return ret;
            }

        if ( callInfo.iStatus != RCall::EStatusIdle )
            {
            TName callName;

            callName.Copy( phoneInfo.iName );
            callName.Append( KDoubleColon );
            callName.Append( KMmTsyDataLineName );
            callName.Append( KDoubleColon );
            callName.Append( callInfo.iCallName );

            ret = call.OpenExistingCall( iTelServer, callName );
            if ( KErrNone != ret )
                {
                line.Close();
                return ret;
                }

            RMobileCall::TMobileCallInfoV1 info;
            RMobileCall::TMobileCallInfoV1Pckg infoPckg( info );

            ret = call.GetMobileCallInfo( infoPckg );
            if ( KErrNone == ret )
                {
                if ( infoPckg().iRemoteParty.iRemoteNumber.iTelNumber.Length() > 0 )
                    {
                    ret = aNumber.Compare( infoPckg().iRemoteParty.iRemoteNumber.iTelNumber );
                    }
                else if ( infoPckg().iDialledParty.iTelNumber.Length() > 0 )
                    {
                    ret = aNumber.Compare( infoPckg().iDialledParty.iTelNumber );
                    }
                else
                    {
                    ret = KErrNotFound;
                    }

                if ( ret == 0 )
                    {
                    aBearer = EBearerCSD;
                    aBearerInfo.iBearer = EBearerInfoCSD;
                    aBearerInfo.iInternal = ETrue;

                    RMobileCall::TMobileCallHscsdInfoV1 hscsdInfo;
                    RMobileCall::TMobileCallHscsdInfoV1Pckg hscsdInfoPckg( hscsdInfo );

                    ret = call.GetCurrentHscsdInfo( hscsdInfoPckg );
                    if ( KErrNone == ret )
                        {
                        if ( hscsdInfo.iAiur != RMobileCall::EAiurBpsUnspecified )
                            {
                            aBearer = EBearerHSCSD;
                            aBearerInfo.iBearer = EBearerInfoHSCSD;
                            }
                        }
                    call.Close();
                    line.Close();
                    return KErrNone;
                    }
                }
            call.Close();
            }
        } // for
    line.Close();
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::CompareAccessPointNames
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::CompareAccessPointNames(
        const TDesC& aNameFromCommDB,
        const TDesC& aNameFromETel ) const
    {
    return ( aNameFromETel.Compare( aNameFromCommDB ) );
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::CompareAccessPointNames
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::CompareAccessPointNames(
        const TDesC& aNameFromCommDB,
        const TDesC8& aNameFromETel ) const
    {
    TBuf<KCommsDbSvrMaxFieldLength> tempBuffer;

    CnvUtfConverter::ConvertToUnicodeFromUtf8( tempBuffer, aNameFromETel );

    return( tempBuffer.Compare( aNameFromCommDB ) );
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::SendConnectionEvents
// -----------------------------------------------------------------------------
//
void CConnMonIAP::SendConnectionEvents( TConnMonBearerInfo& /*aBearerinfo*/ )
    {
    if ( iConnInfos.Count() > 0 )
        {
        for ( TInt i = 0; i < iConnInfos.Count(); i++ )
            {
            if ( iConnInfos[i].iBearerNotifier )
                {
                iConnInfos[i].iBearerNotifier->SendBearerInfoEvent();
                iConnInfos[i].iBearerNotifier->SendBearerGroupEvent();
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CConnMonIAP::LastProgressError
// -----------------------------------------------------------------------------
//
TInt CConnMonIAP::LastProgressError(
        const TUint& aConnectionId,
        TNifProgress& aProgress )
    {
    TInt err( KErrNone );

    // Find connection matching the given Id
    TInt index = Index( aConnectionId );

    if ( index < 0  )
        {
        return KErrNotFound;
        }

    err = iConnInfos[index].iConnAttach->LastProgressError( aProgress );

    LOGIT3("iConnection->Progress() ret <%d> stage %d error <%d>", err, aProgress.iStage, aProgress.iError)
    return err;
    }

// ---------------------------------------------------------------------------
// CConnMonIAP::WlanRssGoodEnough
// ---------------------------------------------------------------------------
//
TBool CConnMonIAP::WlanRssGoodEnough()
    {
    //LOGENTRFN("CConnMonIAP::WlanRssGoodEnough()")
    TBool value( EFalse );

    if ( !iWlanSupport )
        {
        LOGIT("WlanRssGoodEnough: No WLAN support")
        value = EFalse;
        }
    else
        {
        value = iWlanSupport->WlanRssGoodEnough();
        }

    //LOGEXITFN1("CConnMonIAP::WlanRssGoodEnough()", value)
    return value;
    }

// ---------------------------------------------------------------------------
// CConnMonIAP::DiscoverBearerId
// ---------------------------------------------------------------------------
//
TInt CConnMonIAP::BearerIdForBearerAvailability(
        const TUint& aConnId,
        TUint& aBearerId )
    {
    LOGENTRFN("CConnMonIAP::BearerIdForBearerAvailability()")
    TInt err( KErrNone );

    TInt index = Index( aConnId );
    if ( index < 0 )
        {
        // Bearer specific connection id given as attribute
        switch ( aConnId )
            {
            case EBearerIdWLAN:     aBearerId = EBearerIdWLAN;      break;
            case EBearerIdGPRS:     aBearerId = EBearerIdGPRS;      break;
            case EBearerIdWCDMA:    aBearerId = EBearerIdWCDMA;     break;
            case EBearerIdCSD:      aBearerId = EBearerIdCSD;       break;
            case EBearerIdWcdmaCSD: aBearerId = EBearerIdWcdmaCSD;  break;
            // Let session send this request to the plugin
            case EBearerIdLAN:      err = KErrCancel;               break;
            default:                err = KErrNotFound;             break;
            }
        }
    else
        {
        // Valid connection id given as attribute
        switch ( iConnInfos[index].iBearer )
            {
            case EBearerWLAN:               aBearerId = EBearerIdWLAN;      break;
            case EBearerGPRS:
            case EBearerEdgeGPRS:
            case EBearerExternalGPRS:
            case EBearerExternalEdgeGPRS:   aBearerId = EBearerIdGPRS;      break;
            case EBearerWCDMA:
            case EBearerExternalWCDMA:      aBearerId = EBearerIdWCDMA;     break;
            case EBearerCSD:
            case EBearerHSCSD:
            case EBearerExternalCSD:
            case EBearerExternalHSCSD:      aBearerId = EBearerIdCSD;       break;
            case EBearerWcdmaCSD:
            case EBearerExternalWcdmaCSD:   aBearerId = EBearerIdWcdmaCSD;  break;
            // Let session send this request to the plugin
            case EBearerLAN:                err = KErrCancel;               break;
            default:                        err = KErrNotSupported;         break;
            }
        }

    LOGEXITFN1("CConnMonIAP::BearerIdForBearerAvailability()", err)
    return err;
    }

// ---------------------------------------------------------------------------
// CConnMonIAP::RestoreAttachMode
// ---------------------------------------------------------------------------
//
void CConnMonIAP::RestoreAttachMode()
    {
    // AT command AT+CGATT=0 will detach phone from cellular network.
    // Restore attach mode to cellmo when attach mode is set to EAttachWhenPossible.
    //
    RPacketService::TAttachMode attachMode( RPacketService::EAttachWhenNeeded );
    TInt err = iPacketService.GetAttachMode( attachMode );
    LOGIT2("CConnMonIAP::RestoreAttachMode: attach mode:       %d <%d>", attachMode, err)
    
    if ( err == KErrNone && attachMode == RPacketService::EAttachWhenPossible )
        {
        TRequestStatus status( KErrNone );

        iPacketService.SetAttachMode( status, RPacketService::EAttachWhenPossible );
        User::WaitForRequest( status );
        LOGIT1("CConnMonIAP::RestoreAttachMode: set --> EAttachWhenPossible, error <%d>", status.Int() )

        return;
        }
    LOGIT("No need to restore attach mode.")    
    }

// End-of-file
