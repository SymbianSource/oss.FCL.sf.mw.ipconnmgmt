/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides information on CSD fax calls.
*
*/

#include <mmtsy_names.h>
#include <rmmcustomapi.h>
#include "ConnMonServ.h"
#include "ConnMonIAP.h"
#include "CEventQueue.h"
#include "CCsdFax.h"
#include "log.h"

// -----------------------------------------------------------------------------
// CCsdFax::CCsdFax
// -----------------------------------------------------------------------------
//
CCsdFax::CCsdFax(
        CConnMonServer* aServer,
        RTelServer* aTelServer,
        RMobilePhone* aMobilePhone )
        :
        iServer( aServer ),
        iTelServer( aTelServer ),
        iMobilePhone( aMobilePhone )
    {
    }

// -----------------------------------------------------------------------------
// CCsdFax::ConstructL
// -----------------------------------------------------------------------------
//
void CCsdFax::ConstructL()
    {
    //LOGENTRFN("CCsdFax::ConstructL()")
    // Open "Data" line
    TInt ret = iLine.Open( *iMobilePhone, KMmTsyDataLineName );

    if ( ret != KErrNone )
        {
        LOGIT("CCsdFax: Could not open data line.")
        return;
        }
    else
        {
        iState = ELineOpen;
        }

    // Start external connection Up notifier
    if ( iConnUpNotifier == 0 )
        {
        iConnUpNotifier = new( ELeave ) CCsdFaxUpNotifier(
                this,
                iServer,
                iMobilePhone,
                iLine);
        iConnUpNotifier->Construct();
        iConnUpNotifier->Receive();
        }

    // Start external connection Up notifier
    if ( iStatusNotifier == 0 )
        {
        iStatusNotifier = new( ELeave ) CCsdStatusNotifier(
                this,
                iServer );
        iStatusNotifier->Construct();
        }
    //LOGEXITFN("CCsdFax::ConstructL()")
    }

// Destructor
CCsdFax::~CCsdFax()
    {
    if ( iConnUpNotifier != 0 )
        {
        iConnUpNotifier->Cancel();
        delete iConnUpNotifier;
        iConnUpNotifier = 0;
        }

    if ( iStatusNotifier != 0 )
        {
        iStatusNotifier->Cancel();
        delete iStatusNotifier;
        iStatusNotifier = 0;
        }

    if ( iState == ECallOpen )
        {
        CloseCall();
        }

    if ( iState > EModuleClosed )
        {
        iLine.Close();
        }
    }

// -----------------------------------------------------------------------------
// CCsdFax::OpenCall
// -----------------------------------------------------------------------------
//
TInt CCsdFax::OpenCall( const TDes& aName )
    {
    if ( iState == ECallOpen )
        {
        CloseCall();
        }

    RTelServer::TPhoneInfo phoneInfo;
    TInt err = iTelServer->GetPhoneInfo( 0, phoneInfo );

    if ( err != KErrNone )
        {
        return err;
        }

    iCsdFaxCallName.Copy( phoneInfo.iName );
    iCsdFaxCallName.Append( KDoubleColon );
    iCsdFaxCallName.Append( KMmTsyDataLineName );
    iCsdFaxCallName.Append( KDoubleColon );
    iCsdFaxCallName.Append( aName );

    err = iCall.OpenExistingCall( *iTelServer, iCsdFaxCallName );

    if ( KErrNone == err )
        {
        iState = ECallOpen;

        // Start status event watcher
        if ( !iStatusNotifier->IsActive() )
            {
            iStatusNotifier->Start( &iCall );
            }
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CCsdFax::CloseCall
// -----------------------------------------------------------------------------
//
void CCsdFax::CloseCall()
    {
    if ( iStatusNotifier != 0 )
        {
        iStatusNotifier->Cancel();
        }

    iCall.Close();
    iState = ELineOpen;

    iCsdFaxCallName.FillZ();
    iCsdFaxCallName.Zero();
    }

// -----------------------------------------------------------------------------
// CCsdFax::IsValid
// -----------------------------------------------------------------------------
//
TBool CCsdFax::IsValid()
    {
    if ( iState == ECallOpen )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CCsdFax::FindCall
// -----------------------------------------------------------------------------
//
TInt CCsdFax::FindCall()
    {
    TInt ret( KErrNone );
    TInt countCalls( 0 );

    if ( iState == ECallOpen )
        {
        return KErrNone;
        }
    else if ( iState < ELineOpen )
        {
        return KErrNotFound;
        }

    // Enumerate calls on data line
    ret = iLine.EnumerateCall( countCalls );

    if ( ret != KErrNone )
        {
        return ret;
        }
    else if ( countCalls == 0 )
        {
        return KErrNotFound;
        }

    RMmCustomAPI customApi;
    RMmCustomAPI::TCallOrigin origin;

    ret = customApi.Open( *iMobilePhone );

    if ( ret != KErrNone )
        {
        return ret;
        }

    // Get call info
    for ( TInt j = 0; j < countCalls; j++ )
        {
        RLine::TCallInfo callInfo;

        ret = iLine.GetCallInfo( j, callInfo );
        if ( ret != KErrNone )
            {
            customApi.Close();
            return ret;
            }

        customApi.CallOrigin( callInfo.iCallName, origin );

        // TF
        if ( origin == RMmCustomAPI::EOutsider )
            {
            ret = KErrNone;
            OpenCall( callInfo.iCallName );
            break;
            }
        else
            {
            ret = KErrNotFound;
            }
        }

    customApi.Close();
    return ret;
    }

// -----------------------------------------------------------------------------
// CCsdFax::GetBearer
// -----------------------------------------------------------------------------
//
TInt CCsdFax::GetBearer( TInt& aBearer, TBearerInfo& aBearerInfo ) const
    {
    TInt ret( KErrNone );
    RMobileCall::TMobileCallStatus mobileCallStatus;

    if ( iState < ECallOpen )
        {
        return KErrNotFound;
        }

    // Is network WCDMA
    RMobilePhone::TMobilePhoneNetworkMode mode;
    ret = iMobilePhone->GetCurrentMode( mode );

    if ( ret != KErrNone )
        {
        return ret;
        }
    if ( mode == RMobilePhone::ENetworkModeWcdma )
        {
        aBearer = EBearerExternalWcdmaCSD;
        aBearerInfo.iBearer = EBearerInfoWcdmaCSD;
        aBearerInfo.iInternal = EFalse;

        return KErrNone;
        }

    ret = iCall.GetMobileCallStatus( mobileCallStatus );

    if ( ret != KErrNone )
        {
        return ret;
        }

    aBearer = EBearerExternalCSD;
    aBearerInfo.iBearer = EBearerInfoCSD;
    aBearerInfo.iInternal = EFalse;

    if ( mobileCallStatus != RMobileCall::EStatusIdle )
        {
        RMobileCall::TMobileCallHscsdInfoV1 hscsdInfo;
        RMobileCall::TMobileCallHscsdInfoV1Pckg hscsdInfoPckg( hscsdInfo );

        for ( TInt a = 0; a < KBearerQueryCount; a++ )
            {
            ret = iCall.GetCurrentHscsdInfo( hscsdInfoPckg );

            if ( ret != KErrEtelCallNotActive )
                {
                break;
                }

            User::After( KBearerQueryTimeout );
            }

        if ( KErrNone == ret )
            {
            if ( hscsdInfo.iAiur != RMobileCall::EAiurBpsUnspecified )
                {
                aBearer = EBearerExternalHSCSD;
                aBearerInfo.iBearer = EBearerInfoHSCSD;
                }
            }
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCsdFax::GetStatus
// -----------------------------------------------------------------------------
//
TInt CCsdFax::GetStatus( TInt& aStatus ) const
    {
    TInt ret( KErrNone );

    if ( iState < ECallOpen )
        {
        return KErrNotFound;
        }

    RMobileCall::TMobileCallStatus mobileCallStatus;
    ret = iCall.GetMobileCallStatus( mobileCallStatus );

    if ( ret != KErrNone )
        {
        return ret;
        }

    aStatus = mobileCallStatus;
    ret = MapStatus( aStatus );

    return ret;
    }

// -----------------------------------------------------------------------------
// CCsdFax::MapStatus
// -----------------------------------------------------------------------------
//
TInt CCsdFax::MapStatus( TInt& aStatus ) const
    {
    switch( aStatus )
        {
        case RMobileCall::EStatusUnknown:
            return KErrUnknown;

        case RMobileCall::EStatusIdle:
            aStatus = KConnectionUninitialised;
            break;

        case RMobileCall::EStatusDialling:
            aStatus = KCsdStartingDialling;
            break;

        case RMobileCall::EStatusRinging:
            return KErrUnknown;

        case RMobileCall::EStatusAnswering:
            aStatus = KCsdStartingAnswer;
            break;

        case RMobileCall::EStatusConnecting:
            aStatus = KCsdStartingConnect;
            break;

        case RMobileCall::EStatusConnected:
            aStatus = KCsdConnectionOpen;
            break;

         case RMobileCall::EStatusDisconnecting:
         case RMobileCall::EStatusDisconnectingWithInband:
            aStatus = KCsdStartingHangUp;
            break;

        default:
            return KErrUnknown;
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCsdFax::GetTelNumber
// -----------------------------------------------------------------------------
//
TInt CCsdFax::GetTelNumber( TDes& aNumber ) const
    {
    LOGENTRFN("CCsdFax::GetTelNumber()")
    TInt err( KErrNone );

    if ( iState < ECallOpen )
        {
        err = KErrNotFound;
        }
    else
        {
        RMobileCall::TMobileCallInfoV1 info;
        RMobileCall::TMobileCallInfoV1Pckg infoPckg( info );
        err = iCall.GetMobileCallInfo( infoPckg );

        if ( KErrNone == err )
            {
            if ( infoPckg().iRemoteParty.iRemoteNumber.iTelNumber.Length() > 0 )
                {
                aNumber = infoPckg().iRemoteParty.iRemoteNumber.iTelNumber;
                }
            else if ( infoPckg().iDialledParty.iTelNumber.Length() > 0 )
                {
                aNumber = infoPckg().iDialledParty.iTelNumber;
                }
            }
        }

    LOGEXITFN1("CCsdFax::GetTelNumber()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CCsdFax::GetStartTime
// -----------------------------------------------------------------------------
//
TInt CCsdFax::GetStartTime( TTime& aTime ) const
    {
    TInt ret( KErrNone );

    if ( iState < ECallOpen )
        {
        return KErrNotFound;
        }

    TTimeIntervalSeconds duration;
    ret = iCall.GetCallDuration( duration );

    if ( KErrNone == ret )
        {
        TTime current;
        current.UniversalTime();
        aTime = current - duration;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CCsdFax::Stop
// -----------------------------------------------------------------------------
//
TInt CCsdFax::Stop()
    {
    TInt err( KErrNone );

    if ( iState != ECallOpen )
        {
        return KErrNotFound;
        }

    err = iCall.HangUp();

    // Remove from server tables if
    // status notifier is not active.
    if ( !iStatusNotifier->IsActive() )
        {
        RemoveFromServer();
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CCsdFax::RemoveFromServer
// -----------------------------------------------------------------------------
//
void CCsdFax::RemoveFromServer()
    {
    // Remove the connection from internal table
    TInt bearer( 0 );
    TBearerInfo bearerInfo( EBearerInfoCSD, ETrue);

    TConnInfo connInfo( 0, 0, 0, EBearerExternalCSD, bearerInfo );

    TInt err = GetBearer( bearer, bearerInfo );

    if ( KErrNone == err )
        {
        connInfo.iBearer = bearer;
        connInfo.iBearerInfo.iBearer = bearerInfo.iBearer;
        connInfo.iBearerInfo.iInternal = bearerInfo.iInternal;
        }

    iServer->Iap()->RemoveConnection( connInfo );
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCsdFaxUpNotifier::CCsdFaxUpNotifier
// -----------------------------------------------------------------------------
//
CCsdFaxUpNotifier::CCsdFaxUpNotifier(
        CCsdFax* aFaxModule,
        CConnMonServer* aServer,
        RMobilePhone* aMobilePhone,
        RLine& aLine)
        :
        CActive( EConnMonPriorityNormal ),
        iFaxModule( aFaxModule ),
        iServer( aServer ),
        iMobilePhone( aMobilePhone ),
        iLine( aLine )
    {
    }

// -----------------------------------------------------------------------------
// CCsdFaxUpNotifier::Construct
// -----------------------------------------------------------------------------
//
void CCsdFaxUpNotifier::Construct()
    {
    CActiveScheduler::Add( this );
    }

// Destructor
CCsdFaxUpNotifier::~CCsdFaxUpNotifier()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CCsdFaxUpNotifier::Receive
// Requests a new event (connection up/down) from RConnection
// -----------------------------------------------------------------------------
//
void CCsdFaxUpNotifier::Receive()
    {
    if ( IsActive() )
        {
        Cancel();
        }

    iName.FillZ();

    iLine.NotifyCallAdded( iStatus, iName );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CCsdFaxUpNotifier::DoCancel
// Cancels the request from RConnection.
// -----------------------------------------------------------------------------
//
void CCsdFaxUpNotifier::DoCancel()
    {
    if ( IsActive() )
        {
        iLine.NotifyCallAddedCancel();
        }
    }

// -----------------------------------------------------------------------------
// CCsdFaxUpNotifier::RunL
// Handles the event that has arrived from RConnection
// -----------------------------------------------------------------------------
//
void CCsdFaxUpNotifier::RunL()
    {
    // All RunL():s outside CServer-derived main class MUST NOT LEAVE.
    // Use TRAPD when needed.

    if ( iStatus.Int() != KErrNone )
        {
        LOGIT1("External Csd up event FAILED <%d>", iStatus.Int() )
        }
    else
        {
        RMmCustomAPI customApi;
        RMmCustomAPI::TCallOrigin origin( RMmCustomAPI::EUnknown );

        TInt ret = customApi.Open( *iMobilePhone );

        if ( KErrNone == ret )
            {
            customApi.CallOrigin( iName, origin );
            customApi.Close();
            }

        if ( origin == RMmCustomAPI::EOutsider )
            {
            TInt bearer( 0 );
            TBearerInfo bearerInfo;
            TInt err( KErrNone );

            LOGIT("SERVER: EVENT -> new EXTERNAL CSD connection")

            if ( iFaxModule->IsValid() )
                {
                // Sometimes when data call is rejected by the other end, a new
                // data call is started before the old one has been notified to
                // be closed. In this case send the EConnMonDeleteConnection to
                // all clients that are listening.
                iEventInfo.Reset();
                iEventInfo.iEventType = EConnMonDeleteConnection;
                iEventInfo.iConnectionId = iFaxModule->ConnectionId();

                iServer->EventQueue()->Add( iEventInfo );

                // Remove from server tables
                iFaxModule->RemoveFromServer();

                // Close the call
                iFaxModule->CloseCall();

                LOGIT("SERVER: Closed the unfinished EXTERNAL CSD connection")
                }

            err = iFaxModule->OpenCall( iName );

            if ( KErrNone == err )
                {
                err = iFaxModule->GetBearer( bearer, bearerInfo );
                }

            if ( KErrNone == err )
                {
                // Send event to clients
                TConnInfo connInfo( 0, 0, 0, bearer, bearerInfo );

                iEventInfo.Reset();

                iEventInfo.iEventType = EConnMonCreateConnection;

                // Add to the connection table and fill in the new connectioId to connInfo
                TRAP( ret, ( err = iServer->Iap()->AddConnectionL( connInfo ) ) );

                if ( ret )
                    {
                    LOGIT("AddConnectionL failed.")
                    return; // Can't leave
                    }

                // Set connection id
                iEventInfo.iConnectionId = connInfo.iConnectionId;
                iFaxModule->SetConnectionId( connInfo.iConnectionId );

                // Send event to all clients that are listening
                iServer->EventQueue()->Add( iEventInfo );
                }
            else
                {
                LOGIT("Could not get the external call info!")
                }
            }

        // New request
        Receive();
        }
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCsdStatusNotifier::CCsdStatusNotifier
// -----------------------------------------------------------------------------
//
CCsdStatusNotifier::CCsdStatusNotifier(
        CCsdFax* aFaxModule,
        CConnMonServer* aServer )
        :
        CActive( EConnMonPriorityNormal ),
        iFaxModule( aFaxModule ),
        iServer( aServer )
    {
    }

// -----------------------------------------------------------------------------
// CCsdStatusNotifier::Construct
// -----------------------------------------------------------------------------
//
void CCsdStatusNotifier::Construct()
    {
    CActiveScheduler::Add( this );
    }

// Destructor
CCsdStatusNotifier::~CCsdStatusNotifier()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CCsdStatusNotifier::Start
// Requests a new event (connection up/down) from RConnection
// -----------------------------------------------------------------------------
//
void CCsdStatusNotifier::Start( RMobileCall* aCall )
    {
    // Cancel must be called before assigning a new aCall handle
    if ( IsActive() )
        {
        Cancel();
        }

    iCall = aCall;
    Receive();
    }

// -----------------------------------------------------------------------------
// CCsdStatusNotifier::Receive
// Requests a new event (connection up/down) from RConnection
// -----------------------------------------------------------------------------
//
void CCsdStatusNotifier::Receive()
    {
    if ( IsActive() )
        {
        Cancel();
        }

    iCall->NotifyMobileCallStatusChange( iStatus, iMobileCallStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CCsdStatusNotifier::DoCancel
// Cancels the request from RConnection.
// -----------------------------------------------------------------------------
//
void CCsdStatusNotifier::DoCancel()
    {
    if ( IsActive() )
        {
        iCall->CancelAsyncRequest( EMobileCallNotifyMobileCallStatusChange );
        }
    }

// -----------------------------------------------------------------------------
// CCsdStatusNotifier::RunL
// Handles the event that has arrived from RConnection
// -----------------------------------------------------------------------------
//
void CCsdStatusNotifier::RunL()
    {
    // All RunL():s outside CServer-derived main class MUST NOT LEAVE.
    // Use TRAPD when needed.

    if ( iStatus.Int() != KErrNone )
        {
        LOGIT1("External Csd status FAILED <%d>", iStatus.Int() )
        }
    else
        {
        // Send event to clients
        LOGIT1("SERVER: EVENT -> EXTERNAL CSD status changed <%d>", iMobileCallStatus )

        iEventInfo.Reset();
        iEventInfo.iEventType = EConnMonConnectionStatusChange;

        iEventInfo.iConnectionId = iFaxModule->ConnectionId();

        TInt status = iMobileCallStatus;
        TInt err = iFaxModule->MapStatus( status );
        iEventInfo.iData = status;

        if ( ( iServer->NumberOfListeners() > 0 ) && ( err == KErrNone ) )
            {
            // Send event to all clients that are listening
            iServer->EventQueue()->Add( iEventInfo );
            }

        if ( ( iMobileCallStatus == ( TInt )RMobileCall::EStatusDisconnecting )
              ||
             ( iMobileCallStatus == ( TInt )RMobileCall::EStatusDisconnectingWithInband ) )
            {
            // Send connection closed event to all clients that are listening
            iEventInfo.Reset();
            iEventInfo.iEventType = EConnMonDeleteConnection;
            iEventInfo.iConnectionId = iFaxModule->ConnectionId();

            iServer->EventQueue()->Add( iEventInfo );

            // Remove from server tables
            iFaxModule->RemoveFromServer();

            // Close the call
            iFaxModule->CloseCall();

            // Stop listening
            return;
            }

        // New request
        Receive();
        }
    }

// End-of-file
