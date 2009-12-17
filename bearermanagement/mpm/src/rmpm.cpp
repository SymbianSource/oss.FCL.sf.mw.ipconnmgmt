/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: MPM server client interface
*
*/


/**
@file rmpm.cpp
Mobility Policy Manager server client interface.
*/

// INCLUDE FILES
#include "rmpm.h"
#include "mpmcommon.h"
#include "mpmlauncher.h"
#include "mpmlogger.h"

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Panic 
// Panics the client in case of programming error.
// -----------------------------------------------------------------------------
//
void Panic( TInt aPanic )
    {
    User::Panic( KPanicCategory, aPanic );
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RMPM::LaunchServer
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMPM::LaunchServer()
    {
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// RMPM::Connect
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMPM::Connect(  )
    {
    TRequestStatus status;
    Connect( status );
    User::WaitForRequest( status );
    TInt err = status.Int();
    if ( err != KErrNone )
        {
        iConnected = EFalse;
        MPMLOGSTRING2( "RMPM::Connect - Error <%i> in CreateSession", err )
        } 
    return err;
    }

// -----------------------------------------------------------------------------
// RMPM::Connect
// -----------------------------------------------------------------------------
//
EXPORT_C void RMPM::Connect( TRequestStatus& aStatus )
    {
    TInt err( KErrNone );

    if ( !iConnected )
        {
        MPMLOGSTRING( "RMPM::Connect: Client connecting to MPM Server" )
        err = CreateSession( 
                KMPMServerName,
                Version(),
                KNumMessageSlots,
                EIpcSession_Unsharable,
                (TSecurityPolicy*)0,
                &aStatus );
        if ( err != KErrNone )
            {
            //Connecting failed, probably because this is the first Connect() attempt.
            TVersion version = Version();
            MPMLOGSTRING3("Mobility Policy Manager version %d.%d", version.iMajor, 
                                                                   version.iMinor )
            err = MPMLauncher::LaunchServer( KMPMServerImg, 
                                             KServerUid2, 
                                             KServerUid3 );
            if ( err != KErrNone )
                {
                MPMLOGSTRING2( "RMPM::Connect - Error <%i> in LaunchServer", err )
                }
            else
                {
                err = CreateSession( 
                    KMPMServerName,
                    Version(),
                    KNumMessageSlots,
                    EIpcSession_Unsharable,
                    (TSecurityPolicy*)0,
                    &aStatus );
#ifdef _DEBUG
                if (err != KErrNone)
                    MPMLOGSTRING2( "RMPM::Connect - Error <%i> in CreateSession", err )
#endif
                }
            }   
        
        //This can't be else, in case the inner CreateSession() succeeds.
        if (err == KErrNone)
           {
           // session ok but waiting for MPM startup.
           // We have to assume that it'll be ok.
           TPtr8 ptr( reinterpret_cast< TUint8* >( NULL ), 0 );
           TBool errFound = EFalse;
           for (TUint i=0; ( (i < KNumPtrs) && !errFound ); i++)
               {
               err = iPtrArray.Append( ptr );
               if ( err != KErrNone )
                   {
                   MPMLOGSTRING2(
                       "RMPM::Connect - Error <%i> in ptrArray Append", err )
                    errFound = ETrue;
                   }
               }
           // If everything is fine, set client connected to true.
           if ( err == KErrNone )
               {
               iConnected = ETrue;
               }
           // Else close the created session.
           else
               {
               Close();
               }
           }
        }
    else
        {
        MPMLOGSTRING( "RMPM::Connect: Already connected to MPM Server" )
        err = KErrNone;
        }

    // Finally, always complete the failing connects 
    if ( err != KErrNone )
        {
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, err );
        }
    }



// -----------------------------------------------------------------------------
// RMPM::ChooseBestIAP
// -----------------------------------------------------------------------------
//
EXPORT_C void RMPM::ChooseBestIAP( TConnPref&           aConnPref,
                                   const TConnectionId  aConnId,
                                   const TUid           aAppUid,
                                   TConnPref&           aPolicyPref,
                                   TRequestStatus&      aStatus )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );
    MPMLOGSTRING( "RMPM::ChooseBestIAP: Client asking iap" )

    SendReceive( EMPMServerChooseIap,
                 TIpcArgs( &aConnPref,
                           aConnId, 
                           aAppUid.iUid, 
                           &aPolicyPref ),
                 aStatus );
    }

// -----------------------------------------------------------------------------
// RMPM::ReselectBestIAP
// -----------------------------------------------------------------------------
//
EXPORT_C void RMPM::ReselectBestIAP( const TConnectionId    aConnId,
                                     TConnPref&             aPolicyPref,
                                     TRequestStatus&        aStatus ) 
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );

    MPMLOGSTRING( "RMPM::ReselectBestIAP: Client asking iap" )

    SendReceive( EMPMServerReselectIap,
                 TIpcArgs( aConnId, &aPolicyPref ),
                 aStatus );
    }

// -----------------------------------------------------------------------------
// RMPM::ApplicationLeavesConnection
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMPM::ApplicationLeavesConnection( const TUint32       aIapId,
                                                 const TConnectionId aConnId )
    {
    TRequestStatus status;
    ApplicationLeavesConnection( aIapId, aConnId, status );
    User::WaitForRequest( status );
    return status.Int();
    }


// -----------------------------------------------------------------------------
// RMPM::IAPConnectionStopped
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMPM::IAPConnectionStopped( const TUint32         aIapId,
                                          const TConnectionId   aConnId )
    {
    TRequestStatus status;
    IAPConnectionStopped( aIapId, aConnId, status );
    User::WaitForRequest( status );
    return status.Int();
    }


EXPORT_C void RMPM::IAPConnectionStopped( const TUint32         aIapId,
                                          const TConnectionId   aConnId,
                                          TRequestStatus&       aStatus )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );

    TIpcArgs args( aIapId, aConnId );

    SendReceive( EMPMServerIapConnectionStopped, args, aStatus );
    }


// -----------------------------------------------------------------------------
// RMPM::ProcessError
// -----------------------------------------------------------------------------
//
EXPORT_C void RMPM::ProcessError( TInt&                 aError,
                                  const TConnectionId   aConnId,
                                  TBMNeededAction&      aNeededAction,
                                  TRequestStatus&       aStatus )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );

    TPtr8& pkg = NextPtr();
    pkg.Set( reinterpret_cast< TUint8* >( &aError ),
             sizeof( aError ),
             sizeof( aError ) );

    TPtr8& pkgAction = NextPtr();
    pkgAction.Set( reinterpret_cast< TUint8* >( &aNeededAction ),
                   sizeof( aNeededAction ),
                   sizeof( aNeededAction ) );

    TIpcArgs args( &pkg, aConnId, &pkgAction );

    SendReceive( EMPMServerProcessError, args, aStatus );
    }

// -----------------------------------------------------------------------------
// RMPM::ProcessError
// -----------------------------------------------------------------------------
//
EXPORT_C void RMPM::ProcessError( TConnPref&           /*aConnPref*/,
                                  TInt&                /*aError*/,
                                  const TConnectionId  /*aConnId*/,
                                  TBMNeededAction&     /*aNeededAction*/,
                                  TRequestStatus&      /*aStatus*/ )
    {
    __ASSERT_ALWAYS( EFalse, Panic( KErrNotSupported ) );
    //-jl- TODO remove this function from the API.
    }

// -----------------------------------------------------------------------------
// RMPM::RegisterPrefIAPNotif
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMPM::RegisterPrefIAPNotif( const TConnectionId   aConnId )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );

    TIpcArgs args( aConnId );

    // SendReceive used here. MPM first sends preferred iap notifs before 
    // completing this
    // 
    TRequestStatus status;
    RegisterPrefIAPNotif( aConnId, status );
    User::WaitForRequest( status );
    return status.Int();
    }

// -----------------------------------------------------------------------------
// RMPM::RegisterPrefIAPNotif
// -----------------------------------------------------------------------------
//
EXPORT_C void RMPM::RegisterPrefIAPNotif( const TConnectionId   aConnId,
                                          TRequestStatus&       aStatus )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );

    TIpcArgs args( aConnId );

    // SendReceive used here. MPM first sends preferred iap notifs before 
    // completing this
    // 
    return SendReceive( EMPMServerRegisterPrefIAPNotif, args, aStatus );
    }

// -----------------------------------------------------------------------------
// RMPM::RegisterPrefIAPNotif
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMPM::RegisterPrefIAPNotif( const TConnPref&     /*aConnPref*/,
                                          const TConnectionId  /*aConnId*/ )
    {
    __ASSERT_ALWAYS( EFalse, Panic( KErrNotSupported ) );
    //-jl- TODO remove this function from the API.
    return NULL;
    }


// -----------------------------------------------------------------------------
// RMPM::UnregisterPrefIAPNotif
// -----------------------------------------------------------------------------
//
EXPORT_C void RMPM::UnregisterPrefIAPNotif( const TConnectionId aConnId )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );

    TIpcArgs args( aConnId );

    // Server should update the connection state before function returns 
    // No time consuming activities are done.
    //
    TRequestStatus status;
    UnregisterPrefIAPNotif( aConnId, status );
    User::WaitForRequest( status );
    }

// -----------------------------------------------------------------------------
// RMPM::UnregisterPrefIAPNotif
// -----------------------------------------------------------------------------
//
EXPORT_C void RMPM::UnregisterPrefIAPNotif( const TConnectionId aConnId,
                                            TRequestStatus&     aStatus )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );

    TIpcArgs args( aConnId );

    // Server should update the connection state before function returns 
    // No time consuming activities are done.
    //
    SendReceive( EMPMServerUnregisterPrefIAPNotif, args, aStatus );
    }

// -----------------------------------------------------------------------------
// RMPM::WaitNotification
// -----------------------------------------------------------------------------
//
EXPORT_C void RMPM::WaitNotification( TMPMMessageBuffer& aMpmNotification,
                                      TRequestStatus&    aStatus )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );

    TIpcArgs args( &aMpmNotification );

    SendReceive( EMPMServerWaitNotification, args, aStatus );
    }


// -----------------------------------------------------------------------------
// RMPM::SortSNAP
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMPM::SortSNAP( const TUint32     aSNAP, 
                              TMpmSnapBuffer&   aSortedList )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );
    TRequestStatus status;
    SortSNAP( aSNAP, aSortedList, status );
    User::WaitForRequest( status );
    return status.Int();
    }


// -----------------------------------------------------------------------------
// RMPM::ApplicationJoinsConnection
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMPM::ApplicationJoinsConnection( const TUint32       aIapId,
                                                const TConnectionId aConnId )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );
    TRequestStatus status;
    ApplicationJoinsConnection( aIapId, aConnId, status );
    User::WaitForRequest( status );
    return status.Int();
    }

// -----------------------------------------------------------------------------
// RMPM::IAPConnectionActivated
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMPM::IAPConnectionActivated( const TUint32       aIapId,
                                            const TConnectionId aConnId )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );

    // Server response not necessary for the client, using Send
    //
    return Send( EMPMServerIapConnectionActivated, 
                 TIpcArgs( aIapId, 
                           aConnId ) );
    }


// -----------------------------------------------------------------------------
// RMPM::IAPConnectionStarted
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMPM::IAPConnectionStarted( const TUint32         aIapId,
                                          const TConnectionId   aConnId )
    {
    TRequestStatus status;
    IAPConnectionStarted( aIapId, aConnId, status );
    User::WaitForRequest( status );
    return status.Int();
    }


EXPORT_C void RMPM::IAPConnectionStarted( const TUint32         aIapId,
                                          const TConnectionId   aConnId,
                                          TRequestStatus&       aStatus )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );

    // Server should update the connection state before function returns 
    // No time consuming activities are done.
    //
    return SendReceive( EMPMServerIapConnectionStarted, 
                        TIpcArgs( aIapId, aConnId ),
                        aStatus );
    }


// -----------------------------------------------------------------------------
// RMPM::ApplicationConnectionEnds
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMPM::ApplicationConnectionEnds( const TConnectionId aConnId ) 
    {
    TRequestStatus status;
    ApplicationConnectionEnds( aConnId, status );
    User::WaitForRequest( status );
    return status.Int();
    }


// -----------------------------------------------------------------------------
// RMPM::ApplicationMigratesToCarrier
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMPM::ApplicationMigratesToCarrier( 
    const TUint32         aIapId,
    const TConnectionId   aConnId )
    {
    TRequestStatus status;
    ApplicationMigratesToCarrier( aIapId, aConnId, status );
    User::WaitForRequest( status );
    return status.Int();
    }


// -----------------------------------------------------------------------------
// RMPM::ApplicationIgnoredTheCarrier
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMPM::ApplicationIgnoredTheCarrier( 
    const TUint32         aIapId,
    const TConnectionId   aConnId )
    {
    TRequestStatus status;
    ApplicationIgnoredTheCarrier( aIapId, aConnId, status );
    User::WaitForRequest( status );
    return status.Int();
    }


// -----------------------------------------------------------------------------
// RMPM::ApplicationAcceptedTheCarrier
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMPM::ApplicationAcceptedTheCarrier( 
    const TUint32        aIapId,
    const TConnectionId  aConnId )
    {
    TRequestStatus status;
    ApplicationAcceptedTheCarrier( aIapId, aConnId, status );
    User::WaitForRequest( status );
    return status.Int();
    }


// -----------------------------------------------------------------------------
// RMPM::ApplicationRejectedTheCarrier
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMPM::ApplicationRejectedTheCarrier( 
    const TUint32        aIapId,
    const TConnectionId  aConnId )
    {
    TRequestStatus status;
    ApplicationRejectedTheCarrier( aIapId, aConnId, status );
    User::WaitForRequest( status );
    return status.Int();
    }


// -----------------------------------------------------------------------------
// RMPM::EasyWlanSsid
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMPM::EasyWlanSsid( const TWlanSsid& aSSID )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );

    return SendReceive( EMPMServerEasyWlanSsid, 
                        TIpcArgs( &aSSID ) );
    }


// -----------------------------------------------------------------------------
// RMPM::Version
// -----------------------------------------------------------------------------
//
EXPORT_C TVersion RMPM::Version() const
    {
    return TVersion( KMPMServerMajorVersionNumber,
                     KMPMServerMinorVersionNumber,
                     KMPMServerBuildVersionNumber );
    }


// -----------------------------------------------------------------------------
// RMPM::Cancel
// -----------------------------------------------------------------------------
//
EXPORT_C void RMPM::Cancel( TMpmAsynchCodes aReqToCancel ) const
    {
    __ASSERT_ALWAYS( iConnected, Panic(ERMPMPanicClientNotConnected) );
    MPMLOGSTRING("RMPM::Cancel: Client canceling the request")
    SendReceive( EMPMServerCancelRequest, TIpcArgs( aReqToCancel ) );
    }


// -----------------------------------------------------------------------------
// RMPM::Close
// -----------------------------------------------------------------------------
//
EXPORT_C void RMPM::Close()
    {
    MPMLOGSTRING("RMPM::Close: Client closing connection")

    iPtrArray.Close() ;
    iConnected = EFalse;

    RSessionBase::Close();
    }


// -----------------------------------------------------------------------------
// RMPM::Connected
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RMPM::Connected()
    {
    return iConnected;
    }


// -----------------------------------------------------------------------------
// RMPM::ApplicationJoinsConnection
// -----------------------------------------------------------------------------
//
EXPORT_C void RMPM::ApplicationJoinsConnection( const TUint32       aIapId,
                                                const TConnectionId aConnId,
                                                TRequestStatus&     aStatus )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );

    // Server should update the connection state before function returns 
    // No time consuming activities are done.
    //
    return SendReceive( EMPMServerApplicationJoinsConnection, 
                        TIpcArgs( aIapId, aConnId ),
                        aStatus );
    }


// -----------------------------------------------------------------------------
// RMPM::ApplicationLeavesConnection
// -----------------------------------------------------------------------------
//
EXPORT_C void RMPM::ApplicationLeavesConnection( const TUint32       aIapId,
                                                 const TConnectionId aConnId,
                                                 TRequestStatus&     aStatus )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );

    TIpcArgs args( aIapId, aConnId );

    return SendReceive( EMPMServerApplicationLeavesConnection, args, aStatus );
    }


// -----------------------------------------------------------------------------
// RMPM::ApplicationConnectionEnds
// -----------------------------------------------------------------------------
//
EXPORT_C void RMPM::ApplicationConnectionEnds( const TConnectionId aConnId,
                                               TRequestStatus&     aStatus ) 
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );

    // Server should update the connection state before function returns 
    // No time consuming activities are done.
    //
    return SendReceive( EMPMServerApplicationConnectionEnds, 
                        TIpcArgs( aConnId ), aStatus );
    }


// -----------------------------------------------------------------------------
// RMPM::SendCancelRequest
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMPM::SendCancelRequest( TMpmAsynchCodes aReqToCancel ) const
    {
    __ASSERT_ALWAYS( iConnected, Panic(ERMPMPanicClientNotConnected) );
    MPMLOGSTRING("RMPM::SendCancelRequest: Client canceling the request")
    // There must always be one free slot.
    return Send( EMPMServerCancelRequest, TIpcArgs( aReqToCancel ) );
    }


// -----------------------------------------------------------------------------
// RMPM::ApplicationMigratesToCarrier
// -----------------------------------------------------------------------------
//
EXPORT_C void RMPM::ApplicationMigratesToCarrier( const TUint32         aIapId,
                                                  const TConnectionId   aConnId,
                                                  TRequestStatus&       aStatus )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );

    return SendReceive( EMPMServerApplicationMigratesToCarrier,  
                        TIpcArgs( aIapId, aConnId ), 
                        aStatus );
    }


// -----------------------------------------------------------------------------
// RMPM::ApplicationIgnoredTheCarrier
// -----------------------------------------------------------------------------
//
EXPORT_C void RMPM::ApplicationIgnoredTheCarrier( const TUint32         aIapId,
                                                  const TConnectionId   aConnId,
                                                  TRequestStatus&       aStatus )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );

    // SendReceive used here. MPM sends preferred iap or error notification 
    // before completing this
    // 
    return SendReceive( EMPMServerApplicationIgnoredTheCarrier,  
                        TIpcArgs( aIapId, aConnId ), 
                        aStatus );
    }


// -----------------------------------------------------------------------------
// RMPM::ApplicationAcceptedTheCarrier
// -----------------------------------------------------------------------------
//
EXPORT_C void RMPM::ApplicationAcceptedTheCarrier( const TUint32        aIapId,
                                                   const TConnectionId  aConnId,
                                                   TRequestStatus&      aStatus )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );

    // Server should update the connection state before function returns 
    // No time consuming activities are done.
    //
    return SendReceive( EMPMServerApplicationAcceptedTheCarrier, 
                        TIpcArgs( aIapId, aConnId ),
                        aStatus );
    }


// -----------------------------------------------------------------------------
// RMPM::ApplicationRejectedTheCarrier
// -----------------------------------------------------------------------------
//
EXPORT_C void RMPM::ApplicationRejectedTheCarrier( const TUint32        aIapId,
                                                   const TConnectionId  aConnId,
                                                   TRequestStatus&      aStatus )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );

    // SendReceive used here. MPM sends preferred iap or error notifications 
    // before completing this
    // 
    return SendReceive( EMPMServerApplicationRejectedTheCarrier, 
                        TIpcArgs( aIapId, aConnId ),
                        aStatus);
    }

// -----------------------------------------------------------------------------
// RMPM::SortSNAP
// -----------------------------------------------------------------------------
//
EXPORT_C void RMPM::SortSNAP( const TUint32   aSNAP, 
                              TMpmSnapBuffer& aSortedList,
                              TRequestStatus& aStatus,
                              const TUint32   aMaxAgeSeconds )
    {
    __ASSERT_ALWAYS( iConnected, Panic( ERMPMPanicClientNotConnected ) );

    TPtr8 n( reinterpret_cast< TUint8* >( &aSortedList ), sizeof( aSortedList ) );
    TIpcArgs args( aSNAP, &n, aMaxAgeSeconds );

    // Server response is needed before returning from the call
    //
    return SendReceive( EMPMServerSortSNAP, args, aStatus );
    }

// -----------------------------------------------------------------------------
// RMPM::NextPtr
// -----------------------------------------------------------------------------
//
TPtr8& RMPM::NextPtr()
    {
    __ASSERT_ALWAYS( iConnected, Panic(ERMPMPanicClientNotConnected) );

    if ( iIndex >= iPtrArray.Count() )
        {
        iIndex = 0;
        }

    return iPtrArray[ iIndex++ ];
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
