/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Get events from Connection Monitor
*
*/

/**
@file mpmconnmonevents.cpp
Get events from Connection Monitor.
*/

// INCLUDE FILES
#include "mpmconnmonevents.h"
#include "mpmconnmonreqs.h"
#include "mpmserver.h"
#include "mpmserversession.h"
#include "mpmlogger.h"

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPMConnMonEvents::NewL
// -----------------------------------------------------------------------------
//             
CMPMConnMonEvents* CMPMConnMonEvents::NewL( CMPMServer& aServer )
    {
    CMPMConnMonEvents* self = new ( ELeave ) CMPMConnMonEvents( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::CMPMConnMonEvents
// -----------------------------------------------------------------------------
// 
CMPMConnMonEvents::CMPMConnMonEvents( CMPMServer& aServer )
    : iMyServer( aServer ),
      iDiscardAvailabilityNotification( EFalse )
    {
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::~CMPMConnMonEvents
// -----------------------------------------------------------------------------
// 
CMPMConnMonEvents::~CMPMConnMonEvents()
    {
    iConnMon.CancelNotifications();
    iConnMon.Close();
    iReqPtrs.ResetAndDestroy();
    iConnInfoArray.Close();
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::ConstructL
// -----------------------------------------------------------------------------
// 
void CMPMConnMonEvents::ConstructL()
    {
    MPMLOGSTRING( "CMPMConnMonEvents::ConstructL" )
    // Connect to Connection Monitor
    //
    TInt rv = iConnMon.ConnectL();
    if ( rv != KErrNone )
        {
        MPMLOGSTRING2(
            "CMPMConnMonEvents::ConstructL error connecting to ConnMon %d",
            rv )
        User::Leave( rv );
        }

    // Request for available IAPs
    //
    CMPMConnMonReqs* req = CMPMConnMonReqs::NewL( 
                                *const_cast<CMPMConnMonEvents*>( this ),
                                iConnMon,
                                0,
                                NULL );
    CleanupStack::PushL( req );
    iReqPtrs.AppendL( req ); 
    req->AvailableIapsSync();

    UpdateIAPRefreshTime();
    
    // Activate event notifications
    //
    iConnMon.NotifyEventL( *this );
    CleanupStack::Pop( req );
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::RemoveReqPtr
// -----------------------------------------------------------------------------
// 
void CMPMConnMonEvents::RemoveReqPtr( const CMPMConnMonReqs* aReq )
    {
    TInt index( KErrNotFound );

    index = iReqPtrs.Find( aReq );
    if ( index != KErrNotFound )
        {
        iReqPtrs.Remove( index );
        }
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::AppendConnInfo
// -----------------------------------------------------------------------------
// 
void CMPMConnMonEvents::AppendConnInfo( const TUint& aConnId  )
    {
    TInt index( KErrNotFound );
    TConnInfo info;
    
    info.iConnId = aConnId;
    index = iConnInfoArray.Find( info );

    if ( index == KErrNotFound )
        {
        iConnInfoArray.Append( info );
        }
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::RemoveConnInfo
// -----------------------------------------------------------------------------
// 
void CMPMConnMonEvents::RemoveConnInfo( const TUint& aConnId  )
    {
    TInt index( KErrNotFound );
    TConnInfo info;
    
    info.iConnId = aConnId;
    index = iConnInfoArray.Find( info );

    if ( ( index != KErrNotFound ) && ( index < iConnInfoArray.Count() ) )
        {
        iConnInfoArray.Remove( index );
        }
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::SetConnInfo
// -----------------------------------------------------------------------------
// 
void CMPMConnMonEvents::SetConnInfo( const TUint& aConnId, 
                                     const TUint& aIapId, 
                                     const TUint& aPresumedIapId, 
                                     const TUint& aNetId, 
                                     const TDesC& aAPN )
    {
    TInt index( KErrNotFound );
    TConnInfo info;

    info.iConnId = aConnId;
    index = iConnInfoArray.Find( info );

    if ( ( index != KErrNotFound ) && ( index < iConnInfoArray.Count() ) )
        {
        MPMLOGSTRING3(
            "CMPMConnMonEvents::SetConnInfo - aConnId = 0x%x, APN = %S", 
            aConnId, &aAPN )
        iConnInfoArray[index].iIapId = TUint32( aIapId );
        iConnInfoArray[index].iNetId = TUint32( aNetId );
        iConnInfoArray[index].iAPN.Copy( aAPN );
        iConnInfoArray[index].iSSID.FillZ();
        iConnInfoArray[index].iSSID.Zero();
        iConnInfoArray[index].iPresumedIapId = TUint32( aPresumedIapId );
        iConnInfoArray[index].iConnType = EConnectionGPRS; 
        }
    else
        {
        MPMLOGSTRING2(
            "CMPMConnMonEvents::SetConnInfo - aConnId = 0x%x not found", 
            aConnId )
        }
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::SetConnInfo
// -----------------------------------------------------------------------------
// 
void CMPMConnMonEvents::SetConnInfo( const TUint&       aConnId, 
                                     const TUint&       aIapId, 
                                     const TUint&       aPresumedIapId, 
                                     const TUint&       aNetId, 
                                     const TWlanSsid&   aSSID  )
    {
    TInt index( KErrNotFound );
    TConnInfo info;

    info.iConnId = aConnId;
    index = iConnInfoArray.Find( info );

    if ( ( index != KErrNotFound ) && ( index < iConnInfoArray.Count() ) )
        {
        MPMLOGSSID(
            "CMPMConnMonEvents::SetConnInfo - aConnId = 0x%x, SSID = %S",
            aConnId, &aSSID )

        iConnInfoArray[index].iIapId = TUint32( aIapId );
        iConnInfoArray[index].iNetId = TUint32( aNetId );
        iConnInfoArray[index].iAPN.FillZ();
        iConnInfoArray[index].iAPN.Zero();
        iConnInfoArray[index].iSSID.Copy( aSSID );
        iConnInfoArray[index].iPresumedIapId = TUint32( aPresumedIapId );
        iConnInfoArray[index].iConnType = EConnectionWLAN; 
        }
    else
        {
        MPMLOGSTRING2(
            "CMPMConnMonEvents::SetConnInfo - aConnId = 0x%x not found", 
            aConnId )
        }
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::SetConnInfoIapSsid
// -----------------------------------------------------------------------------
// 
void CMPMConnMonEvents::SetConnInfoIapSsid( const TUint&       aIapId, 
                                            const TWlanSsid&    aSSID  )
    {

    for ( TInt i( 0 ); ( i < iConnInfoArray.Count() ); i++ )
        {
        if ( iConnInfoArray[i].iIapId == aIapId )
            {
            iConnInfoArray[i].iSSID.Copy( aSSID );
            }
        }
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::ResetConnInfo
// -----------------------------------------------------------------------------
// 
void CMPMConnMonEvents::ResetConnInfo( const TUint& aConnId )
    {
    TInt index( KErrNotFound );
    TConnInfo info;

    info.iConnId = aConnId;
    index = iConnInfoArray.Find( info );

    if ( ( index != KErrNotFound ) && ( index < iConnInfoArray.Count() ) )
        {
        MPMLOGSTRING2(
            "CMPMConnMonEvents::ResetConnInfo - aConnId = 0x%x reset", 
            aConnId )
        iConnInfoArray[index].iIapId = 0;
        iConnInfoArray[index].iNetId = 0;
        iConnInfoArray[index].iPresumedIapId = 0;
        iConnInfoArray[index].iAPN.FillZ();
        iConnInfoArray[index].iAPN.Zero();
        iConnInfoArray[index].iSSID.FillZ();
        iConnInfoArray[index].iSSID.Zero();
        iConnInfoArray[index].iConnType = EConnectionGeneric; 
        }
    else
        {
        MPMLOGSTRING2(
            "CMPMConnMonEvents::ResetConnInfo - aConnId = 0x%x not found", 
            aConnId )
        }
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::ResetIapConnInfo
// -----------------------------------------------------------------------------
// 
void CMPMConnMonEvents::ResetIapConnInfo( const TUint32 aIapId )
    {

    // Reset all connections using aIapId
    // 
    for ( TInt index( 0 ); ( index < iConnInfoArray.Count() ); index++ )
        {
        if ( iConnInfoArray[index].iIapId == aIapId )
            {
            MPMLOGSTRING2(
                "CMPMConnMonEvents::ResetIapConnInfo - ConnId = 0x%x reset", 
                iConnInfoArray[index].iConnId )
            iConnInfoArray[index].iIapId = 0;
            iConnInfoArray[index].iNetId = 0;
            iConnInfoArray[index].iPresumedIapId = 0;
            iConnInfoArray[index].iAPN.FillZ();
            iConnInfoArray[index].iAPN.Zero();
            iConnInfoArray[index].iSSID.FillZ();
            iConnInfoArray[index].iSSID.Zero();
            iConnInfoArray[index].iConnType = EConnectionGeneric; 
            }
        }
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::GetAvailableIAPs
// -----------------------------------------------------------------------------
// 
const TConnMonIapInfo& CMPMConnMonEvents::GetAvailableIAPs()
    {
    return iAvailableIAPs;
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::SetAvailableIAPs
// -----------------------------------------------------------------------------
// 
void CMPMConnMonEvents::SetAvailableIAPs(
    const TConnMonIapInfo& aAvailableIAPs )
    {
    iAvailableIAPs = aAvailableIAPs;
    
    UpdateIAPRefreshTime();
    
    MPMLOGSTRING2( "CMPMConnMonEvents::SetAvailableIAPs - IAPs count: %d", 
        iAvailableIAPs.Count() )

#ifdef _DEBUG
    for ( TUint i = 0; i < iAvailableIAPs.Count(); i++ )
        {
        MPMLOGSTRING2( "CMPMConnMonEvents::SetAvailableIAPs - IAP: %d", 
            iAvailableIAPs.iIap[i].iIapId)
        }
#endif // _DEBUG
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::FindAPN
// -----------------------------------------------------------------------------
//
void CMPMConnMonEvents::FindAPN( const TDesC&   aDes,
                                 TUint32&       aIapId,
                                 TUint32&       aNetId )
    {
    TBool apnFound( EFalse );

    for ( TInt i( 0 ); ( (i < iConnInfoArray.Count()) && !apnFound ); i++ )
        {
        if ( ( aDes.Compare( iConnInfoArray[i].iAPN ) == KErrNone ) && 
             ( iConnInfoArray[i].iConnType == EConnectionGPRS ) )
            {
            aIapId = iConnInfoArray[i].iIapId;
            aNetId = iConnInfoArray[i].iNetId;
            MPMLOGSTRING2( "CMPMConnMonEvents::FindAPN found Iap Id: %i", 
                            aIapId)
            MPMLOGSTRING2( "CMPMConnMonEvents::FindAPN found Net Id: %i",
                            aNetId)
            apnFound = ETrue;
            }
        }
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::FindSSID
// -----------------------------------------------------------------------------
//
void CMPMConnMonEvents::FindSSID( const TWlanSsid&   aSSID,
                                  TUint32&          aIapId,
                                  TUint32&          aNetId )
    {
    TBool ssidFound( EFalse );

    for ( TInt i( 0 ); ( (i < iConnInfoArray.Count()) && !ssidFound ); i++ )
        {
        if ( ( aSSID.Compare( iConnInfoArray[i].iSSID ) == KErrNone ) && 
             ( iConnInfoArray[i].iConnType == EConnectionWLAN ) )
            {
            aIapId = iConnInfoArray[i].iIapId;
            aNetId = iConnInfoArray[i].iNetId;
            MPMLOGSTRING2( "CMPMConnMonEvents::FindSSID found Iap Id: %i", 
                            aIapId)
            MPMLOGSTRING2( "CMPMConnMonEvents::FindSSID found Net Id: %i",
                            aNetId)
            ssidFound = ETrue;
            }
        }
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::ActiveWlanConnection
// -----------------------------------------------------------------------------
//
TBool CMPMConnMonEvents::ActiveWlanConnection( TWlanSsid&   aSSID, 
                                               TUint32&     aIapId )
    {
    TWlanSsid   emptySSID;
    TBool       activeWlanFound( EFalse );

    emptySSID.FillZ();
    emptySSID.Zero();

    for ( TInt i( 0 ); ( (i < iConnInfoArray.Count()) && !activeWlanFound );
          i++ )
        {
        if ( emptySSID.Compare( iConnInfoArray[i].iSSID ) != KErrNone )
            {
            // Cross check that a started connection with the wlan IAP 
            // is found from ActiveBMConnections
            // 
            TUint32 connIap = iMyServer.GetBMIap( iConnInfoArray[i].iConnId );
            TConnectionState connState; 
            iMyServer.GetConnectionState( iConnInfoArray[i].iConnId, connState );
            if( connIap == iConnInfoArray[i].iIapId && 
                connState == EStarted )
                {
                aSSID.Copy( iConnInfoArray[i].iSSID );
                aIapId = iConnInfoArray[i].iIapId;
                activeWlanFound = ETrue;

                MPMLOGSSID( "CMPMConnMonEvents::ActiveWlanConnection found\
 ConnId = 0x%x, SSID = %S", iConnInfoArray[i].iConnId, &aSSID )

                MPMLOGSTRING2(
                    "CMPMConnMonEvents::ActiveWlanConnection found Iap Id: %i",
                iConnInfoArray[i].iIapId )
                }
            else
                {
                MPMLOGSSID( "CMPMConnMonEvents::ActiveWlanConnection found\
 but connection not in started state ConnId = 0x%x, SSID = %S", 
                    iConnInfoArray[i].iConnId, &aSSID )
                }
            }
        }
    return activeWlanFound;
    }

// -----------------------------------------------------------------------------
// CMPMConnMonEvents::IapAvailabilityChange
// -----------------------------------------------------------------------------
// 
void CMPMConnMonEvents::IapAvailabilityChange( const TPrefIAPNotifCaller aCaller )
    {
    if ( ( iAvailableIAPs.Count() > 0 ) && 
         !DiscardAvailabilityNotification() )
        {
        // Remove temporary blacklistings as fresh availability 
        // info is available.
        // 
        iMyServer.HandleServerUnblackListIap( ETemporary );
        
        // Send preferred IAP notifications, but DO NOT LEAVE 
        // or Connection Monitor CConnMonEventHandler::RunL() 
        // will also leave, which will make mpmserver.exe CRASH.
        // 
        TRAP_IGNORE( iMyServer.NotifyBMPrefIapL( iAvailableIAPs, aCaller ) )
        }
    else
        {
        MPMLOGSTRING2( "CMPMConnMonEvents::EventL - IAPs count: %d", 
            iAvailableIAPs.Count() )
        MPMLOGSTRING2( 
            "CMPMConnMonEvents::EventL - DiscardAvailabilityNotification = %d", 
            DiscardAvailabilityNotification() )
        }
    }

// -----------------------------------------------------------------------------
// CMPMConnMonEvents::EventL
// -----------------------------------------------------------------------------
// 
void CMPMConnMonEvents::EventL( const CConnMonEventBase& aConnMonEvent )
    {
    MPMLOGSTRING2( "CMPMConnMonEvents::EventL %d", aConnMonEvent.EventType())

    switch ( aConnMonEvent.EventType() )
        {
        case EConnMonCreateConnection: 
            {
            MPMLOGSTRING( "    EConnMonCreateConnection" )
            iMyServer.IncrementConnections();
            break; 
            }
        case EConnMonDeleteConnection:
            {
            MPMLOGSTRING( "    EConnMonDeleteConnection" )
            iMyServer.DecrementConnections(); 
            break; 
            }
        case EConnMonIapAvailabilityChange:
            {
#ifdef __DEBUG
            if ( iFilterEvents == ETrue )
                {
                MPMLOGSTRING( "    EConnMonIapAvailabilityChange FILTERED OUT" )
                return;
                }
#endif //__DEBUG
            // IAP Availability has changed 
            // 
            MPMLOGSTRING( "    EConnMonIapAvailabilityChange" )

            const CConnMonIapAvailabilityChange* eventIap;

            // Following line is correct, even though 
            // PC-Lint claims that here is a problem:
            // 
            // Suspicious pointer-to-pointer conversion (area too small)
            // 
            eventIap = static_cast<const CConnMonIapAvailabilityChange*>( 
                                                         &aConnMonEvent );

            iAvailableIAPs = eventIap->IapAvailability();
            UpdateIAPRefreshTime();
            IapAvailabilityChange( EConnMonEvent );
            
            TRAPD( err, iMyServer.UpdateSessionConnectionDlgL() )
            if( err != KErrNone )
                {
                MPMLOGSTRING2( "CMPMConnMonEvents::EventL UpdateSessionConnectionDlgL \
error code %d", err )
                }
            break;
            }
        default:
            {
            MPMLOGSTRING( "CMPMConnMonEvents::RunL: default" )
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::ScanWLANNetworksL
// -----------------------------------------------------------------------------
// 
void CMPMConnMonEvents::ScanWLANNetworksL( CMPMServerSession* aSession,
                                           const TConnectionId& aId,
                                           TWlanScanCallback aCallback,
                                           TInt aForceRefreshIntervalSeconds )
    {
    MPMLOGSTRING( "CMPMConnMonEvents::ScanWLANNetworksL" )
    // Sanity check null pointer 
    ASSERT( aSession != NULL 
            || aCallback == EWlanScanCallbackNone 
            || aCallback == EWlanScanCallbackGenerateEvent );
    // Request for available WLAN Networks
    //
    CMPMConnMonReqs* req = CMPMConnMonReqs::NewL( 
                                *const_cast<CMPMConnMonEvents*>( this ),
                                iConnMon,
                                aId, 
                                aSession );
    CleanupStack::PushL( req );
    iReqPtrs.AppendL( req ); 
    req->RefreshAvailableIAPs( aCallback, aForceRefreshIntervalSeconds );
    CleanupStack::Pop( req );
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::CancelScanL
// -----------------------------------------------------------------------------
// 
void CMPMConnMonEvents::CancelScanL( CMPMServerSession* aSession )
    {
    MPMLOGSTRING( "CMPMConnMonEvents::CancelScanL" )

    // Disable discarding availability notifications from Connection Monitor.
    // 
    DisableDiscarding(); 

    CMPMConnMonReqs* req = CMPMConnMonReqs::NewL( 
                                *const_cast<CMPMConnMonEvents*>( this ),
                                iConnMon,
                                aSession->ConnectionId(), 
                                aSession );
    // Delete ongoing req, if found
    TInt index = iReqPtrs.Find(req, CMPMConnMonReqs::CompareConnIds);
    if (index != KErrNotFound)
        {
        // Instead of delete iReqPtrs[index]
        // We just mark it as obsolete.
        iReqPtrs[index]->MyCancel();
        // And we can remove the index as the request will handle the deletion.
        iReqPtrs.Remove(index);
        }
    // Finally delete the temporary object.
    delete req;
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::PresumedIapId
// -----------------------------------------------------------------------------
// 
TUint32 CMPMConnMonEvents::PresumedIapId( const TUint& aConnId, 
                                          const TUint& aIapId )
    {

    for ( TInt i( 0 ); ( i < iConnInfoArray.Count() ); i++ )
        {
        if ( iConnInfoArray[i].iConnId == aConnId &&
             iConnInfoArray[i].iIapId == aIapId )
            {
            if ( iConnInfoArray[i].iPresumedIapId == 0 )
                {
                // Iap Id zero is not a valid Iap Id
                // 
                return aIapId; 
                }
            else
                {
                MPMLOGSTRING2(
                    "CMPMConnMonEvents::PresumedIapId found Iap: %i", 
                    iConnInfoArray[i].iPresumedIapId )

                return iConnInfoArray[i].iPresumedIapId;
                }
            }
        }

    // PresumedIapId returns aIapId in case no presumed IAP info is found. 
    // 
    return aIapId; 
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::UpdateIAPRefreshTime
// -----------------------------------------------------------------------------
// 
void CMPMConnMonEvents::UpdateIAPRefreshTime()
    {
    iLastIAPRefreshTime.UniversalTime();
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::GetIAPRefreshTime
// -----------------------------------------------------------------------------
// 
TTime& CMPMConnMonEvents::GetIAPRefreshTime()
    {
    return iLastIAPRefreshTime;
    }


// -----------------------------------------------------------------------------
// CMPMConnMonEvents::SetConnectionCounter
// -----------------------------------------------------------------------------
// 
void CMPMConnMonEvents::SetConnectionCounter( const TUint aValue )
    {
    iMyServer.SetConnectionCounter( aValue );
    }


// -----------------------------------------------------------------------------
// TConnInfo::TConnInfo
// -----------------------------------------------------------------------------
//
TConnInfo::TConnInfo() 
    : iConnId( 0 ),
      iIapId( 0 ),
      iPresumedIapId( 0 ),
      iNetId( 0 ), 
      iAPN(), 
      iSSID(), 
      iConnType( EConnectionGeneric )
    {
    }

//  End of File

