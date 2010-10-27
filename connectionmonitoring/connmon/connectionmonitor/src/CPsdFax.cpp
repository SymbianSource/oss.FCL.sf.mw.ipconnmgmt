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
* Description:  PSD fax connection information.
*
*/

#include <utf.h>

#include "ConnMonServ.h"
#include "ConnMonIAP.h"
#include "CEventQueue.h"
#include "CPsdFax.h"
#include "log.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPsdFax::CPsdFax
// -----------------------------------------------------------------------------
//
CPsdFax::CPsdFax(
        CConnMonServer* aServer,
        RTelServer* aTelServer,
        RMobilePhone* aMobilePhone,
        RPacketService* aPacketService )
        :
        iServer( aServer ),
        iTelServer( aTelServer ),
        iMobilePhone( aMobilePhone ),
        iPacketService( aPacketService )
    {
    }

// -----------------------------------------------------------------------------
// CPsdFax::ConstructL
// -----------------------------------------------------------------------------
//
void CPsdFax::ConstructL()
    {
    //LOGENTRFN("CPsdFax::ConstructL()")

    // Create external connection Up notifier object
    if ( !iConnUpNotifier )
        {
        iConnUpNotifier = new( ELeave ) CPsdFaxUpNotifier(
                this,
                iServer,
                iPacketService );
        // Set object to active scheduler
        iConnUpNotifier->Construct();

        // Start listening events from RConnection
        iConnUpNotifier->Receive();
        }

    //LOGEXITFN("CPsdFax::ConstructL()")
    }

// -----------------------------------------------------------------------------
// CPsdFax::~CPsdFax
// -----------------------------------------------------------------------------
//
CPsdFax::~CPsdFax()
    {
    // Delete connection Up notifier object
    if ( iConnUpNotifier )
        {
        iConnUpNotifier->Cancel();
        delete iConnUpNotifier;
        iConnUpNotifier = NULL;
        }

    if ( iRestoreAttachModeTimer )
        {
        iRestoreAttachModeTimer->Cancel();
        delete iRestoreAttachModeTimer;
        iRestoreAttachModeTimer = NULL;
        }

    // Delete all ConnectionData objects
    for ( TUint index = 0; index < KMaxPsdConnectionCount; index++ )
        {
        if ( iConnectionData[index] )
            {
            if ( iConnectionData[index]->IsOpen() )
                {
                iConnectionData[index]->CloseContext();
                }

            delete iConnectionData[index];
            iConnectionData[index] = NULL;
            }
        }
    }


// -----------------------------------------------------------------------------
// CPsdFax::OpenContext
// -----------------------------------------------------------------------------
//
TInt CPsdFax::OpenContext( const TUint aIndex, const TDes& aName )
    {
    LOGENTRFN("CPsdFax::OpenContext()")
    TInt err( KErrNone );

    // Create ConnectionData object
    if ( !iConnectionData[aIndex] )
        {
        // Leave left out intentionally, check for NULL instead
        iConnectionData[aIndex] = new CPsdFaxConnectionData( this, iServer, iPacketService ); // No (ELeave)
        if ( !iConnectionData[aIndex] )
            {
            err = KErrNoMemory;
            }
        else
            {
            // Start external connection status notifier
            err = iConnectionData[aIndex]->Construct();
            if ( err != KErrNone )
                {
                // Free allocated resources if failed
                err = KErrNoMemory;
                delete iConnectionData[aIndex];
                iConnectionData[aIndex] = NULL;
                }
            }
        }

    if ( KErrNone == err )
        {
        // Open context to be used in queries
        err = iConnectionData[aIndex]->OpenContext( aName );
        if ( KErrNone == err )
            {
            // Set start time to current time
            iConnectionData[aIndex]->ResetStartTime();
            }
        else
            {
            // Free allocated resources if failed
            delete iConnectionData[aIndex];
            iConnectionData[aIndex] = NULL;
            }
        }

    LOGEXITFN1("CPsdFax::OpenContext()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CPsdFax::IsValid
// -----------------------------------------------------------------------------
//
TBool CPsdFax::IsValid( const TUint aConnectionId )
    {
    LOGENTRFN("CPsdFax::IsValid()")
    TBool result( EFalse );

    TInt index = ConnectionIndex( aConnectionId );
    if ( KErrNotFound != index && iConnectionData[index]->IsOpen() )
        {
        LOGIT("IsValid: connection is valid (open)")
        result = ETrue;
        }
    else if ( !iConnUpNotifier->IsActive() )
        {
        LOGIT("IsValid: connection up notifier is not active -> restart listening")
        iConnUpNotifier->Receive(); // Restart listening
        }

    LOGEXITFN1("CPsdFax::IsValid()", result)
    return result;
    }


// -----------------------------------------------------------------------------
// CPsdFax::ConnectionIndex
// -----------------------------------------------------------------------------
//
TInt CPsdFax::ConnectionIndex( const TUint aConnectionId )
    {
    LOGENTRFN("CPsdFax::ConnectionIndex()")

    // Check PSD connection IDs
    for ( TInt index = 0; index < KMaxPsdConnectionCount; index++ )
        {
        if ( iConnectionData[index] )
            {
            // Return connection table index if IDs match
            if ( iConnectionData[index]->ConnectionId() == aConnectionId )
                {
                LOGIT2("CPsdFax::ConnectionIndex() - ConnID %d match, return index %d", aConnectionId, index)
                LOGEXITFN("CPsdFax::ConnectionIndex()")
                return index;
                }
            }
        }

    LOGEXITFN("CPsdFax::ConnectionIndex()")
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CPsdFax::SetConnectionId
// -----------------------------------------------------------------------------
//
TInt CPsdFax::SetConnectionId( const TUint aIndex, const TUint aConnectionId )
    {
    LOGENTRFN("CPsdFax::SetConnectionId()")
    TInt err( KErrNone );

    // Check if connection is open
    if ( !iConnectionData[aIndex]->IsOpen() )
        {
        err = KErrNotFound;
        }
    else
        {
        // Set connection ID
        iConnectionData[aIndex]->SetConnectionId( aConnectionId );
        }

    LOGEXITFN1("CPsdFax::SetConnectionId()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CPsdFax::GetBearer
// -----------------------------------------------------------------------------
//
TInt CPsdFax::GetBearer( TInt& aBearer, TBearerInfo& aBearerInfo )
    {
    LOGENTRFN("CPsdFax::GetBearer()")
    TInt err( KErrNone );
    RMobilePhone::TMobilePhoneNetworkMode mode;

    err = iMobilePhone->GetCurrentMode( mode );
    if ( KErrNone == err )
        {
        switch( mode )
            {
            case RMobilePhone::ENetworkModeWcdma:
                aBearer = EBearerExternalWCDMA;
                aBearerInfo.iBearer = iServer->Iap()->HsxpaStatus();
                aBearerInfo.iInternal = EFalse;
                break;
            case RMobilePhone::ENetworkModeCdma2000:
                aBearer = EBearerExternalCDMA2000;
                aBearerInfo.iBearer = EBearerInfoCDMA2000;
                aBearerInfo.iInternal = EFalse;
                break;
            case RMobilePhone::ENetworkModeGsm:
                if ( iServer->Iap()->IsEdgeCell() )
                    {
                    aBearer = EBearerExternalEdgeGPRS;
                    aBearerInfo.iBearer = EBearerInfoEdgeGPRS;
                    aBearerInfo.iInternal = EFalse;
                    }
                else
                    {
                    aBearer = EBearerExternalGPRS;
                    aBearerInfo.iBearer = EBearerInfoGPRS;
                    aBearerInfo.iInternal = EFalse;
                    }
                break;
            default:
                err = KErrUnknown;
                break;
            }
        }

    LOGEXITFN1("CPsdFax::GetBearer()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CPsdFax::GetAPN
// -----------------------------------------------------------------------------
//
TInt CPsdFax::GetApn( const TUint aConnectionId, TDes& aName )
    {
    LOGENTRFN("CPsdFax::GetApn()")
    TInt err( KErrNone );
    TInt bearer( 0 );
    TBearerInfo bearerInfo;

    // Check if connection ID is valid and connection is open
    TInt index = ConnectionIndex( aConnectionId );
    if ( KErrNotFound == index || !iConnectionData[index]->IsOpen() )
        {
        err = KErrNotFound;
        }
    else
        {
        err = GetBearer( bearer, bearerInfo );
        if ( KErrNone == err )
            {
            if ( !( bearer != EBearerExternalCDMA2000 &&
                    ( bearerInfo.iBearer != EBearerInfoCDMA2000 && !bearerInfo.iInternal ) ) )
                {
                err = KErrNotSupported;
                LOGIT("GetApn: bearer == EBearerExternalCDMA2000, returning KErrNotSupported")
                }
            else
                {
                // Get connection APN
                err = iConnectionData[index]->GetApn( aName );
                }
            }
        }

    LOGEXITFN1("CPsdFax::GetApn()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CPsdFax::GetProtocolType
// -----------------------------------------------------------------------------
//
TInt CPsdFax::GetProtocolType( const TUint aConnectionId, TInt& aProtocolType )
    {
    LOGENTRFN("CPsdFax::GetProtocolType()")
    TInt err( KErrNone );
    TInt bearer( 0 );
    TBearerInfo bearerInfo;

    // Check if connection ID is valid and connection is open
    TInt index = ConnectionIndex( aConnectionId );
    if ( KErrNotFound == index || !iConnectionData[index]->IsOpen() )
        {
        err = KErrNotFound;
        }
    else
        {
        err = GetBearer( bearer, bearerInfo );
        if ( KErrNone == err )
            {
            if ( !( bearer != EBearerExternalCDMA2000 &&
                    ( bearerInfo.iBearer != EBearerInfoCDMA2000 && !bearerInfo.iInternal ) ) )
                {
                aProtocolType = EProtocolTypeUnknown;
                LOGIT("GetProtocolType: bearer == EBearerExternalCDMA2000, returning KErrNone")
                }
            else
                {
                // Get connection protocol type
                err = iConnectionData[index]->GetProtocolType( aProtocolType );
                }
            }
        }

    LOGEXITFN1("CPsdFax::GetProtocolType()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CPsdFax::GetStatus
// -----------------------------------------------------------------------------
//
TInt CPsdFax::GetStatus( const TUint aConnectionId, TInt& aStatus )
    {
    LOGENTRFN("CPsdFax::GetStatus()")
    TInt err( KErrNone );

    // Check if connection ID is valid and connection is open
    TInt index = ConnectionIndex( aConnectionId );
    if ( KErrNotFound == index || !iConnectionData[index]->IsOpen() )
        {
        err = KErrNotFound;
        }
    else
        {
        // Get connection status
        err = iConnectionData[index]->GetStatus( aStatus );
        }

    LOGEXITFN1("CPsdFax::GetStatus()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CPsdFax::GetDataVolumes
// -----------------------------------------------------------------------------
//
TInt CPsdFax::GetDataVolumes( const TUint aConnectionId, TUint& aDLVolume, TUint& aULVolume )
    {
    LOGENTRFN("CPsdFax::GetDataVolumes()")
    TInt err( KErrNone );

    TInt index = ConnectionIndex( aConnectionId );
    if ( KErrNotFound == index || !iConnectionData[index]->IsOpen() )
        {
        err = KErrNotFound;
        }
    else
        {
        // Get data volumes
        err = iConnectionData[index]->GetDataVolumes( aDLVolume, aULVolume );
        }

    LOGEXITFN1("CPsdFax::GetDataVolumes()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CPsdFax::GetStartTime
// -----------------------------------------------------------------------------
//
TInt CPsdFax::GetStartTime( const TUint aConnectionId, TTime& aTime )
    {
    LOGENTRFN("CPsdFax::GetStartTime()")
    TInt err( KErrNone );

    // Check if connection ID is valid and connection is open
    TInt index = ConnectionIndex( aConnectionId );
    if ( KErrNotFound == index || !iConnectionData[index]->IsOpen() )
        {
        err = KErrNotFound;
        }
    else
        {
        // Get connection start time
        err = iConnectionData[index]->GetStartTime( aTime );
        }

    LOGEXITFN1("CPsdFax::GetStartTime()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CPsdFax::Stop (sync)
// -----------------------------------------------------------------------------
//
TInt CPsdFax::Stop( const TUint aConnectionId )
    {
    LOGENTRFN("CPsdFax::Stop()")
    TInt err( KErrNone );

    // Check if connection ID is valid and connection is open
    TInt index = ConnectionIndex( aConnectionId );
    if ( index == KErrNotFound || !iConnectionData[index]->IsOpen() )
        {
        LOGIT1("CPsdFax::Stop() connection not found, index %d", index)
        err = KErrNotFound;
        }
    else
        {
        // Stop connection
        err = iConnectionData[index]->Stop();
        }

    LOGEXITFN1("CPsdFax::Stop()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CPsdFax::Stop (async)
// -----------------------------------------------------------------------------
//
TInt CPsdFax::Stop( const TUint aConnectionId, TRequestStatus& aStatus )
    {
    LOGENTRFN("CPsdFax::Stop(TUint, TRequestStatus)")
    TInt err( KErrNone );

    // Check if connection ID is valid and connection is open
    TInt index = ConnectionIndex( aConnectionId );
    if ( index == KErrNotFound || !iConnectionData[index]->IsOpen() )
        {
        LOGIT1("CPsdFax::Stop() connection not found, index %d", index)
        err = KErrNotFound;
        }
    else
        {
        // Stop connection
        iConnectionData[index]->Stop( aStatus );
        }

    LOGEXITFN1("CPsdFax::Stop(TUint, TRequestStatus)", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CPsdFax::CleanupConnectionInfo
// -----------------------------------------------------------------------------
//
TInt CPsdFax::CleanupConnectionInfo( const TUint aConnectionId )
    {
    LOGENTRFN("CPsdFax::CleanupConnectionInfo()")
    TInt err( KErrNone );

    TInt index = ConnectionIndex( aConnectionId );
    if ( index != KErrNotFound )
        {
        iConnectionData[index]->CleanupConnectionInfo();
        }
    else
        {
        LOGIT1("CleanupConnectionInfo: connection ID %d not found", aConnectionId)
        err = KErrNotFound;
        }

    LOGEXITFN1("CPsdFax::CleanupConnectionInfo()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CPsdFax::DeleteConnections
// -----------------------------------------------------------------------------
//
void CPsdFax::DeleteConnections()
    {
    LOGENTRFN("CPsdFax::DeleteConnections()")

    // Delete all ended ConnectionData objects
    for ( TUint index = 0; index < KMaxPsdConnectionCount; index++ )
        {
        if ( iConnectionData[index] )
            {
            if ( iConnectionData[index]->IsEnded() )
                {
                LOGIT1("DeleteConnections: deleting connection index %d", index)
                delete iConnectionData[index];
                iConnectionData[index] = NULL;
                }
            }
        }
    LOGEXITFN("CPsdFax::DeleteConnections()")
    }

// ---------------------------------------------------------
// CPsdFax::RestoreAttachModeCb
// ---------------------------------------------------------
//
TInt CPsdFax::RestoreAttachModeCb( TAny* aObject )
    {
    LOGENTRFN("CPsdFax::RestoreAttachModeCb()")

    CPsdFax* myself = static_cast<CPsdFax*>( aObject );
    
    myself->iRestoreAttachModeTimer->Cancel();
    
    // Make sure that ETel goes to correct network status
    // in case AT+CGATT=0 has been sent from PC. 
    myself->iServer->Iap()->RestoreAttachMode();

    LOGEXITFN("CPsdFax::RestoreAttachModeCb()")

    return 0;
    }
    
 // ---------------------------------------------------------
// CPsdFax::RestoreAttachMode
// ---------------------------------------------------------
//
void CPsdFax::RestoreAttachMode()
    {
    LOGENTRFN("CPsdFax::RestoreAttachMode()")

    if ( iRestoreAttachModeTimer )
        {
        iRestoreAttachModeTimer->Cancel();
        }
    else
        {
        iRestoreAttachModeTimer = CPeriodic::NewL( CActive::EPriorityStandard );
        }

    iRestoreAttachModeTimer->Start( 
        TTimeIntervalMicroSeconds32( KRestoreAttachModeInterval ), 
        TTimeIntervalMicroSeconds32( KRestoreAttachModeInterval ), 
        TCallBack( RestoreAttachModeCb, this ) );

    LOGEXITFN("CPsdFax::RestoreAttachMode()")
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPsdFaxUpNotifier::CPsdFaxUpNotifier
// -----------------------------------------------------------------------------
//
CPsdFaxUpNotifier::CPsdFaxUpNotifier(
        CPsdFax* aFaxModule,
        CConnMonServer* aServer,
        RPacketService* aPacketService )
        :
        CActive( EConnMonPriorityNormal ),
        iFaxModule( aFaxModule ),
        iServer( aServer ),
        iPacketService( aPacketService )
    {
    }

// -----------------------------------------------------------------------------
// CPsdFaxUpNotifier::ConstructL
// -----------------------------------------------------------------------------
//
void CPsdFaxUpNotifier::Construct()
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CPsdFaxUpNotifier::~CPsdFaxUpNotifier
// -----------------------------------------------------------------------------
//
CPsdFaxUpNotifier::~CPsdFaxUpNotifier()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CPsdFaxUpNotifier::Receive
// Requests a new event (connection up/down) from RConnection
// -----------------------------------------------------------------------------
//
void CPsdFaxUpNotifier::Receive()
    {
    if ( IsActive() )
        {
        Cancel();
        }

    iName.FillZ();
    iName.Zero();

    iPacketService->NotifyContextAdded( iStatus, iName );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CPsdFaxUpNotifier::DoCancel
// Cancels the request from RConnection.
// -----------------------------------------------------------------------------
//
void CPsdFaxUpNotifier::DoCancel()
    {
    if ( IsActive() )
        {
        iPacketService->CancelAsyncRequest( EPacketNotifyContextAdded );
        }
    }

// -----------------------------------------------------------------------------
// CPsdFaxUpNotifier::RunL
// Handles the event that has arrived from RConnection
// -----------------------------------------------------------------------------
//
void CPsdFaxUpNotifier::RunL()
    {
    // All RunL()'s outside CServer-derived main class MUST NOT LEAVE.
    // Use TRAPD when needed.
    //LOGENTRFN("CPsdFaxUpNotifier::RunL()")

    LOGIT(".")
    LOGIT1("RunL: CPsdFaxUpNotifier, status %d", iStatus.Int())

    if ( KErrNone != iStatus.Int() )
        {
        LOGIT1("CPsdFaxUpNotifier: PSD context up event FAILED <%d>", iStatus.Int())
        }
    else
        {
        // For external connection, context name must be "External, External2, External3 etc"
        LOGIT1("CPsdFaxUpNotifier: new PSD context name: %s", iName.PtrZ())

        TName contextName = iName;
        contextName.LowerCase();

        // Check which external connection created
        for ( TUint index = 0; index < KMaxPsdConnectionCount; index++ )
            {
            TBuf<KMaxName> tempName( KExternalName );

            // Add connection number to string if index is > 1
            if ( index > 0)
                {
                tempName.AppendNum( index+1 );
                }

            // Compare to context name
            TInt res = contextName.Compare( tempName );
            if ( res == 0 )
                {
                LOGIT1("CPsdFaxUpNotifier: new external PSD connection with index %d", index)

                // Open context
                TInt err = iFaxModule->OpenContext( index, iName );

                // Break from loop
                break;
                }
            }

        // New request
        Receive();
        }
    //LOGEXITFN("CPsdFaxUpNotifier::RunL()")
    }



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::CPsdFaxConnectionData
// -----------------------------------------------------------------------------
//
CPsdFaxConnectionData::CPsdFaxConnectionData(
        CPsdFax* aFaxModule,
        CConnMonServer* aServer,
        RPacketService* aPacketService )
        :
        iConnectionId ( 0 ),
        iOpen ( EFalse ),
        iTimeValid ( EFalse ),
        iEnded ( EFalse ),
        iFaxModule( aFaxModule ),
        iServer( aServer ),
        iPacketService( aPacketService )
    {
    }


// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::ConstructL
// -----------------------------------------------------------------------------
//
void CPsdFaxConnectionData::ConstructL()
    {
    LOGENTRFN("CPsdFaxConnectionData::ConstructL()")

    // Create external connection Status notifier object
    if ( !iStatusNotifier )
        {
        iStatusNotifier = new( ELeave ) CPsdFaxStatusNotifier(
                iFaxModule,
                iServer,
                iPacketService,
                this );
        // Start external connection Status notifier
        iStatusNotifier->Construct();
        }

    LOGEXITFN("CPsdFaxConnectionData::ConstructL()")
    }

// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::Construct
// -----------------------------------------------------------------------------
//
TInt CPsdFaxConnectionData::Construct()
    {
    LOGENTRFN("CPsdFaxConnectionData::Construct()")
    TInt err( KErrNone );

    // Create external connection Status notifier object
    if ( !iStatusNotifier )
        {
        // Leave left out intentionally, check for NULL instead
        iStatusNotifier = new CPsdFaxStatusNotifier( iFaxModule, iServer, iPacketService, this ); // No (ELeave)
        if ( !iStatusNotifier )
            {
            err = KErrNoMemory;
            }
        else
            {
            // Start external connection Status notifier
            iStatusNotifier->Construct();
            }
        }

    LOGEXITFN1("CPsdFaxConnectionData::Construct()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::~CPsdFaxConnectionData
// -----------------------------------------------------------------------------
//
CPsdFaxConnectionData::~CPsdFaxConnectionData()
    {
    LOGENTRFN("CPsdFaxConnectionData::~CPsdFaxConnectionData()")

    if ( iOpen )
        {
        CloseContext();
        }

    if ( iStatusNotifier )
        {
        iStatusNotifier->Cancel();
        delete iStatusNotifier;
        iStatusNotifier = NULL;
        }

    LOGEXITFN("CPsdFaxConnectionData::~CPsdFaxConnectionData()")
    }


// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::OpenContext
// -----------------------------------------------------------------------------
//
TInt CPsdFaxConnectionData::OpenContext( const TDes& aName )
    {
    LOGENTRFN("CPsdFaxConnectionData::OpenContext()")
    TInt err( KErrNone );

    if ( iOpen )
        {
        CloseContext();
        }

    err = iContext.OpenExistingContext( *iPacketService, aName );
    if ( KErrNone == err )
        {
        iOpen = ETrue;

        // Start status notifier
        if ( iStatusNotifier && !iStatusNotifier->IsActive() )
            {
            LOGIT("OpenContext: start status notifier")
            iStatusNotifier->Start( iContext );
            }
        }

    LOGEXITFN1("CPsdFaxConnectionData::OpenContext()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::CloseContext
// -----------------------------------------------------------------------------
//
void CPsdFaxConnectionData::CloseContext()
    {
    LOGENTRFN("CPsdFaxConnectionData::CloseContext()")

    if ( iOpen )
        {
        if ( iStatusNotifier && iStatusNotifier->IsActive() )
            {
            LOGIT("CloseContext: cancel status notifier")
            iStatusNotifier->Cancel();
            }

        LOGIT("CloseContext: close context")
        iContext.Close();
        }

    iConnectionId = 0;
    iOpen = EFalse;
    iTimeValid = EFalse;
    iEnded = ETrue;

    LOGEXITFN("CPsdFaxConnectionData::CloseContext()")
    }


// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::IsOpen
// -----------------------------------------------------------------------------
//
TBool CPsdFaxConnectionData::IsOpen()
    {
    if ( iOpen )
        {
        return ETrue;
        }
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::GetDataVolumes
// -----------------------------------------------------------------------------
//
TInt CPsdFaxConnectionData::GetDataVolumes( TUint& aDLVolume, TUint& aULVolume ) const
    {
    LOGENTRFN("CPsdFaxConnectionData::GetDataVolumes()")
    TInt err( KErrNone );
    RPacketContext::TDataVolume volume;
    TRequestStatus status( KErrNone );

    if ( !iOpen )
        {
        err = KErrNotFound;
        }
    else
        {
        iContext.GetDataVolumeTransferred( status, volume );
        User::WaitForRequest( status );
        err = status.Int();

        if ( KErrNone == err )
            {
            #ifdef _DEBUG
            if ( volume.iOverflowCounterReceived || volume.iOverflowCounterSent )
                {
                LOGIT2("ERROR: 4GB overflow (ignored), dl:%d ul:%d",
                        volume.iOverflowCounterReceived, volume.iOverflowCounterSent)
                }
            #endif // _DEBUG

            // volume.iOverflowCounterReceived*(2^32) + volume.iBytesReceived
            aDLVolume = volume.iBytesReceived;

            // volume.iOverflowCounterSent*(2^32) + volume.iBytesSent
            aULVolume = volume.iBytesSent;
            }
        }

    LOGEXITFN1("CPsdFaxConnectionData::GetDataVolumes()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::Stop (sync)
// -----------------------------------------------------------------------------
//
TInt CPsdFaxConnectionData::Stop()
    {
    LOGENTRFN("CPsdFaxConnectionData::Stop()")
    TInt err( KErrNone );
    TRequestStatus status( KErrNone );

    iContext.Deactivate( status );
    User::WaitForRequest( status );
    err = status.Int();

    // Remove from server tables if status notifier is not active.
    if ( !iStatusNotifier || !iStatusNotifier->IsActive() )
        {
        // Remove from server tables
        RemoveFromServer();

        // Close the context
        CloseContext();

        // Delete all old connection objects
        // This method should be used carefully
        // because it will delete ConnectionData
        // and statusnotifier objects.
        // Get out fast from Stop()
        iFaxModule->DeleteConnections();
        }

    LOGEXITFN1("CPsdFaxConnectionData::Stop()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::Stop (async)
// -----------------------------------------------------------------------------
//
void CPsdFaxConnectionData::Stop( TRequestStatus& aStatus )
    {
    LOGENTRFN("CPsdFaxConnectionData::Stop(TRequestStatus)")
    iContext.Deactivate( aStatus );
    LOGEXITFN("CPsdFaxConnectionData::Stop(TRequestStatus)")
    }

// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::CleanupConnectionInfo
// -----------------------------------------------------------------------------
//
void CPsdFaxConnectionData::CleanupConnectionInfo()
    {
    LOGENTRFN("CPsdFaxConnectionData::CleanupConnectionInfo()")

    // Remove from server tables if status notifier is not active.
    if ( iStatusNotifier && !iStatusNotifier->IsActive() )
        {
        // Remove from server tables
        RemoveFromServer();

        // Close the context
        CloseContext();

        // Delete all old connection objects. This method should be used
        // carefully because it will delete ConnectionData and statusnotifier
        // objects. Get out fast from Stop()
        iFaxModule->DeleteConnections();
        }

    LOGEXITFN("CPsdFaxConnectionData::CleanupConnectionInfo()")
    }

// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::RemoveFromServer
// -----------------------------------------------------------------------------
//
void CPsdFaxConnectionData::RemoveFromServer()
    {
    LOGENTRFN("CPsdFaxConnectionData::RemoveFromServer()")

    // Remove the connection from internal table
    TInt bearer( 0 );
    TBearerInfo bearerInfo;
    TConnInfo connInfo( 0, 0, iConnectionId, EBearerExternalGPRS, bearerInfo );

    TInt err = iFaxModule->GetBearer( bearer, bearerInfo );
    if ( KErrNone == err )
        {
        connInfo.iBearer = bearer;
        connInfo.iBearerInfo = bearerInfo;
        }

    // Remove connection from server
    iServer->Iap()->RemoveConnection( connInfo );

    LOGEXITFN("CPsdFaxConnectionData::RemoveFromServer()")
    }


// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::SetConnectionId
// -----------------------------------------------------------------------------
//
void CPsdFaxConnectionData::SetConnectionId( const TUint& aConnectionId )
    {
    LOGIT1("CPsdFaxConnectionData::SetConnectionId() - ConnID <%d>", aConnectionId )
    iConnectionId = aConnectionId;
    }

// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::ConnectionId
// -----------------------------------------------------------------------------
//
TUint CPsdFaxConnectionData::ConnectionId() const
    {
    //LOGIT1("CPsdFaxConnectionData::ConnectionId() - ConnID %d", iConnectionId)
    return iConnectionId;
    }

// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::ResetStartTime
// -----------------------------------------------------------------------------
//
void CPsdFaxConnectionData::ResetStartTime()
    {
    LOGENTRFN("CPsdFaxConnectionData::ResetStartTime()")
    iStartTime.UniversalTime();
    iTimeValid = ETrue;
    LOGEXITFN("CPsdFaxConnectionData::ResetStartTime()")
    }

// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::GetStartTime
// -----------------------------------------------------------------------------
//
TInt CPsdFaxConnectionData::GetStartTime( TTime& aTime ) const
    {
    LOGENTRFN("CPsdFaxConnectionData::GetStartTime()")
    TInt err( KErrNone );

    // Start time is not know for external connections that have been up before
    // connection monitor started
    if ( !iTimeValid )
        {
        err = KErrUnknown;
        }
    else
        {
        aTime = iStartTime;
        }

    LOGEXITFN1("CPsdFaxConnectionData::GetStartTime()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::GetAPN
// -----------------------------------------------------------------------------
//
TInt CPsdFaxConnectionData::GetApn( TDes& aName )
    {
    LOGENTRFN("CPsdFaxConnectionData::GetApn()")
    TInt err( KErrNone );
    TRequestStatus status( KErrNone );
    RPacketContext::TContextConfigGPRS* getParams;

    // Leave left out intentionally, check for NULL instead
    getParams = new RPacketContext::TContextConfigGPRS(); // No (ELeave)
    if ( !getParams )
        {
        err = KErrNoMemory;
        }
    else
        {
        TPckg<RPacketContext::TContextConfigGPRS> getContextConfigPckg( *getParams );
        iContext.GetConfig( status, getContextConfigPckg );
        User::WaitForRequest( status );
        err = status.Int();

        if ( KErrNone == err )
            {
            // Access Point Name
            ConvertAPN( aName, getParams->iAccessPointName );
            }
        delete getParams;
        }

    LOGEXITFN1("CPsdFaxConnectionData::GetApn()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::ConvertAPN
// -----------------------------------------------------------------------------
//
void CPsdFaxConnectionData::ConvertAPN( TDes& aResult, const TDesC& aName ) const
    {
    aResult = aName;
    }

// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::ConvertAPN
// -----------------------------------------------------------------------------
//
void CPsdFaxConnectionData::ConvertAPN( TDes& aResult, const TDesC8& aName ) const
    {
    CnvUtfConverter::ConvertToUnicodeFromUtf8( aResult, aName );
    }

// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::GetProtocolType
// -----------------------------------------------------------------------------
//
TInt CPsdFaxConnectionData::GetProtocolType( TInt& aProtocolType )
    {
    LOGENTRFN("CPsdFaxConnectionData::GetProtocolType()")
    TInt err( KErrNone );
    TRequestStatus status( KErrNone );
    RPacketContext::TContextConfigGPRS* getParams;

    // Leave left out intentionally, check for NULL instead
    getParams = new RPacketContext::TContextConfigGPRS(); // No (ELeave)
    if ( !getParams )
        {
        err = KErrNoMemory;
        }

    if ( KErrNone == err )
        {
        TPckg<RPacketContext::TContextConfigGPRS> getContextConfigPckg( *getParams );
        iContext.GetConfig( status, getContextConfigPckg );
        User::WaitForRequest( status );
        err = status.Int();

        if ( KErrNone == err )
            {
            if ( getParams->iPdpType == RPacketContext::EPdpTypeIPv4 )
                {
                aProtocolType = EProtocolTypeIPv4;
                }
            else if ( getParams->iPdpType == RPacketContext::EPdpTypeIPv6 )
                {
                aProtocolType = EProtocolTypeIPv6;
                }
            else
                {
                aProtocolType = EProtocolTypeUnknown;
                }
            }
        delete getParams;
        }

    LOGEXITFN1("CPsdFaxConnectionData::GetProtocolType()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::GetStatus
// -----------------------------------------------------------------------------
//
TInt CPsdFaxConnectionData::GetStatus( TInt& aStatus ) const
    {
    LOGENTRFN("CPsdFaxConnectionData::GetStatus()")

    RPacketContext::TContextStatus status;

    TInt err = iContext.GetStatus( status );
    if ( KErrNone == err )
        {
        aStatus = status;
        err = MapStatus( aStatus );
        }

    LOGEXITFN("CPsdFaxConnectionData::GetStatus()")
    return err;
    }

// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::MapStatus
// -----------------------------------------------------------------------------
//
TInt CPsdFaxConnectionData::MapStatus( TInt& aStatus ) const
    {
    LOGENTRFN("CPsdFaxConnectionData::MapStatus()")

    // Map to internal PDP context status
    switch( aStatus )
        {
        case RPacketContext::EStatusUnknown:
            return KErrUnknown;

        case RPacketContext::EStatusInactive:
            return KErrUnknown;

        case RPacketContext::EStatusActivating:
            aStatus = KPsdStartingActivation;
            break;

        case RPacketContext::EStatusActive:
            aStatus = KPsdFinishedActivation;
            break;

        case RPacketContext::EStatusDeactivating:
            aStatus = KPsdStartingDeactivation;
            break;

        case RPacketContext::EStatusSuspended:
            aStatus = KPsdSuspended;
            break;

        case RPacketContext::EStatusDeleted:
            aStatus = KPsdFinishedDeactivation;
            break;

        default:
            return KErrUnknown;
        }

    LOGEXITFN("CPsdFaxConnectionData::MapStatus()")
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPsdFaxConnectionData::IsEnded
// -----------------------------------------------------------------------------
//
TBool CPsdFaxConnectionData::IsEnded()
    {
    if ( iEnded )
        {
        return ETrue;
        }
    return EFalse;
    }



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPsdFaxStatusNotifier::CPsdFaxStatusNotifier
// -----------------------------------------------------------------------------
//
CPsdFaxStatusNotifier::CPsdFaxStatusNotifier(
        CPsdFax* aFaxModule,
        CConnMonServer* aServer,
        RPacketService* aPacketService,
        CPsdFaxConnectionData* aConnDataModule )
        :
        CActive( EConnMonPriorityNormal ),
        iFaxModule( aFaxModule ),
        iServer( aServer ),
        iPacketService( aPacketService ),
        iConnDataModule ( aConnDataModule )
    {
    }

// -----------------------------------------------------------------------------
// CPsdFaxStatusNotifier::Construct
// -----------------------------------------------------------------------------
//
void CPsdFaxStatusNotifier::Construct()
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CPsdFaxStatusNotifier::~CPsdFaxStatusNotifier
// -----------------------------------------------------------------------------
//
CPsdFaxStatusNotifier::~CPsdFaxStatusNotifier()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CPsdFaxStatusNotifier::Start
// Requests a new event (connection up/down) from RConnection
// -----------------------------------------------------------------------------
//
void CPsdFaxStatusNotifier::Start( const RPacketContext& aContext )
    {
    iContext = aContext;
    Receive();
    }

// -----------------------------------------------------------------------------
// CPsdFaxStatusNotifier::Receive
// Requests a new event (connection up/down) from RConnection
// -----------------------------------------------------------------------------
//
void CPsdFaxStatusNotifier::Receive()
    {
    if ( IsActive() )
        {
        Cancel();
        }

    iContext.NotifyStatusChange( iStatus, iContextStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CPsdFaxStatusNotifier::DoCancel
// Cancels the request from RConnection.
// -----------------------------------------------------------------------------
//
void CPsdFaxStatusNotifier::DoCancel()
    {
    if ( IsActive() )
        {
        iContext.CancelAsyncRequest( EPacketContextNotifyStatusChange );
        }
    }

// -----------------------------------------------------------------------------
// CPsdFaxStatusNotifier::RunL
// Handles the event that has arrived from RConnection
// -----------------------------------------------------------------------------
//
void CPsdFaxStatusNotifier::RunL()
    {
    // All RunL():s outside CServer-derived main class MUST NOT LEAVE.
    // Use TRAPD when needed.
    //LOGENTRFN("CPsdFaxStatusNotifier::RunL()")

    LOGIT(".")
    LOGIT1("RunL: CPsdFaxStatusNotifier, status %d", iStatus.Int())

    if ( iStatus.Int() != KErrNone )
        {
        LOGIT1("CPsdFaxStatusNotifier: external PSD status event FAILED <%d>", iStatus.Int())
        }
    else
        {
        LOGIT1("CPsdFaxStatusNotifier: external PSD connection status changed: %d", iContextStatus)

        iEventInfo.iConnectionId = iConnDataModule->ConnectionId();
        if ( iEventInfo.iConnectionId == 0 )
            {
            // Send connection create event when the first status event arrives.
            TInt ret = SendConnectionCreateEvent();
            if ( KErrNone != ret )
                {
                // Close the context
                iConnDataModule->CloseContext();

                // Delete all old connection objects
                iFaxModule->DeleteConnections();

                // Stop listening
                return;
                }
            }

        iEventInfo.Reset();
        iEventInfo.iEventType = EConnMonConnectionStatusChange;
        iEventInfo.iConnectionId = iConnDataModule->ConnectionId();

        TInt status = iContextStatus;
        TInt err = iConnDataModule->MapStatus( status );
        iEventInfo.iData = status;

        if ( ( iServer->NumberOfListeners() > 0 ) && ( err == KErrNone ) )
            {
            // Send event to all clients that are listening
            iServer->EventQueue()->Add( iEventInfo );
            }

        if ( iContextStatus == RPacketContext::EStatusDeleted )
            {
            // Send connection closed event to all clients that are listening
            iEventInfo.Reset();
            iEventInfo.iEventType = EConnMonDeleteConnection;
            iEventInfo.iConnectionId = iConnDataModule->ConnectionId();
            iConnDataModule->GetDataVolumes( iEventInfo.iData, iEventInfo.iData2 );
            iServer->EventQueue()->Add( iEventInfo );

            // Remove from server tables
            iConnDataModule->RemoveFromServer();

            // Close the context
            iConnDataModule->CloseContext();

            // Dial-up connection has gone down. Make sure the dial-up PDP
            // context override is disabled.
            LOGIT("External PSD connection status EStatusDeleted, disabling dial-up override")
            iServer->SetDialUpOverrideStatus( EConnMonDialUpOverrideInactive );
            
            // Restore packet data network attach mode in case it has been
            // changed by sending at+cgatt=0 from PC.
            iFaxModule->RestoreAttachMode();

            // Delete all old connection objects. This method should be used
            // carefully because it will delete ConnectionData and
            // statusnotifier objects. Get out fast from RunL().
            iFaxModule->DeleteConnections();

            // Stop listening
            return;
            }
        else if ( iContextStatus == RPacketContext::EStatusActive )
            {
            // Dial-up connection has been established. Make sure the dial-up
            // PDP context override is disabled.
            LOGIT("External PSD connection status EStatusActive, disabling dial-up override")
            iServer->SetDialUpOverrideStatus( EConnMonDialUpOverrideInactive );
            }

        // New request
        Receive();
        }

    //LOGEXITFN("CPsdFaxStatusNotifier::RunL()")
    }

// -----------------------------------------------------------------------------
// CPsdFaxStatusNotifier::SendConnectionCreateEvent
// Send created event.
// -----------------------------------------------------------------------------
//
TInt CPsdFaxStatusNotifier::SendConnectionCreateEvent()
    {
    TInt bearer( 0 );
    TBearerInfo bearerInfo;
    TInt err( KErrNone );

    LOGENTRFN("CPsdFaxStatusNotifier::SendConnectionCreateEvent()")

    // Get bearer info
    err = iFaxModule->GetBearer( bearer, bearerInfo );

    if ( KErrNone != err )
        {
        bearer = EBearerExternalGPRS;
        bearerInfo.iBearer = EBearerInfoGPRS;
        bearerInfo.iInternal = EFalse;
        }

    TConnInfo connInfo( 0, 0, 0, bearer, bearerInfo );

    // Add to the connection table and fill in the new connectionId to connInfo
    TRAPD( ret, ( err = iServer->Iap()->AddConnectionL( connInfo ) ) );
    if ( ret || ( err != KErrNone ) )
        {
        LOGIT("AddConnectionL failed.")
        LOGEXITFN("CPsdFaxStatusNotifier::SendConnectionCreateEvent()")
        return KErrUnknown;
        }

    // Send connection create event to all clients that are listening
    iEventInfo.Reset();
    iEventInfo.iEventType = EConnMonCreateConnection;
    iEventInfo.iConnectionId = connInfo.iConnectionId;
    iConnDataModule->SetConnectionId( connInfo.iConnectionId );
    iServer->EventQueue()->Add( iEventInfo );

    LOGEXITFN("CPsdFaxStatusNotifier::SendConnectionCreateEvent()")
    return KErrNone;
    }

// End-of-file
