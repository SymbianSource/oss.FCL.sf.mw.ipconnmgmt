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
* Description:  Provides an interface to WLAN Engine.
*
*/

#include <e32base.h>
#include <utf.h>

#include "CWlanSupport.h"
#include "CEventQueue.h"
#include "ConnMonIAP.h"
#include "ConnMonAvailabilityManager.h"
#include "connmoncommsdatcache.h"
#include "log.h"
#include "ConnMonUtils.h"
#include "connmonwlannetwork.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWlanSupport::NewL
// -----------------------------------------------------------------------------
//
CWlanSupport* CWlanSupport::NewL( CConnMonServer* aServer )
    {
    //LOGENTRFN("CWlanSupport::NewL()")
    CWlanSupport* self = new( ELeave ) CWlanSupport( aServer );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    //LOGEXITFN("CWlanSupport::NewL()")
    return self;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::CWlanSupport
// -----------------------------------------------------------------------------
//
CWlanSupport::CWlanSupport(
        CConnMonServer* aServer )
        :
        iServer( aServer ),
        iEnableScan( ETrue ),
        iEnableEventsToClients( EFalse )
    {
    // iEnableScan has to be initialized to ETrue, so iAvailableWlanIapsCache will be initialized
    // properly when WLAN IAP availability is asked for the first time.
    }

// -----------------------------------------------------------------------------
// CWlanSupport::ConstructL
// -----------------------------------------------------------------------------
//
void CWlanSupport::ConstructL()
    {
    iWlanMgmt = CWlanMgmtClient::NewL();
    ActivateNotificationsL();
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CWlanSupport::~CWlanSupport()
    {
    CancelNotifications(); // Deletes iEventWatcher

    delete iWlanMgmt;
    iWlanMgmt = NULL;

    // Cleanup WLAN IAPs cache array
    iAvailableWlanIapsCache.Close();

    // Cleanup WLAN sessions pointer array
    iWlanSessionsPtrArray.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CWlanSupport::GetBearerAvailabilityL
// Called from CWlanEvent::OldNetworksLost
// -----------------------------------------------------------------------------
//
void CWlanSupport::GetBearerAvailabilityL( TBool& aAvailability )
    {
    LOGENTRFN("CWlanSupport::GetBearerAvailabilityL()")
    aAvailability = EFalse;

    // Check if connection exists
    TInt mode( 0 );
    TInt ret = GetConnectionMode( mode );
    if ( KErrNone == ret )
        {
        // An active WLAN connection exists.
        aAvailability = ETrue;
        }
    else // No active WLAN connections... Scan WLAN networks
        {
        CWlanScanInfo* scanInfo = CWlanScanInfo::NewL();
        CleanupStack::PushL( scanInfo );

        // Get scan results without parameters
        ret = iWlanMgmt->GetScanResults( *scanInfo );
        if ( KErrNone == ret )
            {
            // Check scan results
            if ( scanInfo->IsDone() )
                {
                // Empty scan info, no WLAN networks available
                aAvailability = EFalse;
                }
            else
                {
                // WLAN networks found
                aAvailability = ETrue;
                }
            }
        else
            {
            LOGIT1("GetBearerAvailabilityL: GetScanResults error <%d>", ret)
            User::Leave( ret );
            }

        CleanupStack::PopAndDestroy( scanInfo );
        }

    LOGEXITFN("CWlanSupport::GetBearerAvailabilityL()")
    }

// -----------------------------------------------------------------------------
// CWlanSupport::GetBearerAvailabilityL
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::GetBearerAvailabilityL( const RMessage2& aMessage, TBool& aAvailability )
    {
    LOGENTRFN("CWlanSupport::GetBearerAvailabilityL()")
    TInt result( KErrNone );
    aAvailability = EFalse;

    // Check if event watcher has the availability information
    TBool eventWatcherKnowsAvailability( EFalse );
    if ( iEventWatcher )
        {
        eventWatcherKnowsAvailability = iEventWatcher->HasAvailabilityKnowledge();
        }

    // Use WLAN bearer availability info from event watcher if possible.
    if ( eventWatcherKnowsAvailability )
        {
        aAvailability = iEventWatcher->WlanAvailability();
        result = KErrNone;
        }
    else
        {
        // Check if a WLAN connection exists
        TInt mode( 0 );
        TInt ret = GetConnectionMode( mode );
        if ( KErrNone == ret )
            {
            // An active WLAN connection equals bearer availability.
            aAvailability = ETrue;
            result = KErrNone;
            }
        else // No active WLAN connections... Scan WLAN networks
            {
            // Add session to wlan session table (if not already there)
            TInt index = AddWlanSessionL( aMessage );
            iWlanSessionsPtrArray[index]->GetScanResultsL( aMessage );
            result = KRequestPending;
            }
        }

    LOGEXITFN1("CWlanSupport::GetBearerAvailabilityL()", result)
    return result;
    }


// -----------------------------------------------------------------------------
// CWlanSupport::GetSignalStrength
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::GetSignalStrengthL(
        const RMessage2& aMessage,
        TInt& aSignalStrength )
    {
    LOGENTRFN("CWlanSupport::GetSignalStrengthL()")
    TInt result( KErrNone );
    TInt mode( 0 );
    aSignalStrength = 0;

    // Get connection mode to see if connection exists
    TInt ret = GetConnectionMode( mode );
    if ( KErrNone == ret )
        {
        // An active WLAN connection exists -> get signal quality
        result = GetConnectionSignalQuality( aSignalStrength );
        }
    else // No connection -> scan networks
        {
        // Add session to wlan session table (if not already there)
        TInt index = AddWlanSessionL( aMessage );
        iWlanSessionsPtrArray[index]->GetScanResultsL( aMessage );
        result = KRequestPending;
        }

    LOGEXITFN1("CWlanSupport::GetSignalStrengthL()", result)
    return result;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::GetNetworkNamesL
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::GetNetworkNamesL( const RMessage2& aMessage )
    {
    LOGENTRFN("CWlanSupport::GetNetworkNamesL()")

    // Add session to wlan session table (if not already there)
    TInt index = AddWlanSessionL( aMessage );
    iWlanSessionsPtrArray[index]->GetScanResultsL( aMessage );

    LOGEXITFN1("CWlanSupport::GetNetworkNamesL()", KRequestPending)
    return KRequestPending;
    }


// -----------------------------------------------------------------------------
// CWlanSupport::ParseNetworkNamesL
// -----------------------------------------------------------------------------
//
void CWlanSupport::ParseNetworkNamesL(
        CWlanScanInfo* aScanInfo,
        TConnMonNetworkNames& aNetInfo )
    {
    LOGENTRFN("CWlanSupport::ParseNetworkNamesL()")

    TInt ret( KErrNone );
    RArray< TConnMonNetwork > sortedNetworkNames;
    CleanupClosePushL( sortedNetworkNames );
    aNetInfo.iCount = 0;

    // For logging only
    LOG(TInt count(0);)
    LOGIT("Parsing networks:")

    // Iterate through scan results.
    for ( aScanInfo->First(); !aScanInfo->IsDone(); aScanInfo->Next() )
        {
        TConnMonNetwork nw;

        // Operating mode
        if ( aScanInfo->Capability() & KWlan802Dot11CapabilityEssMask )
            {
            nw.iType = EConnMonInfraStructure;
            }
        else
            {
            nw.iType = EConnMonAdHoc;
            }

        // Signal strength
        nw.iSignalStrength = aScanInfo->RXLevel();

        // SSID
        TUint8 ieLen( 0 );
        const TUint8* ieData( NULL );
        ret = aScanInfo->InformationElement( KWlan802Dot11SsidIE, ieLen, &ieData );
        if ( ( KErrNone == ret ) && ( ieLen <= CConnMonWlanNetwork::KMaxNameLength ) )
            {
            // ieData points to the beginning of the 802.11 SSID payload data i.e.
            // the header is bypassed. ieLen contains the length of payload data.
            nw.iName.Copy( ieData, ieLen );
            }

        sortedNetworkNames.InsertInOrderAllowRepeatsL( nw, CWlanSupport::CompareNetworkStrengths );

#ifdef _DEBUG
        // Print network information to log
        count++;
        if ( ( KErrNone == ret ) && ( ieLen <= CConnMonWlanNetwork::KMaxNameLength ) )
            {
            TBuf<CConnMonWlanNetwork::KMaxNameLength+1> debug_name;
            ConnMonUtils::TDes8ToTDes( nw.iName, debug_name );
            TWlanBssid bssid;
            aScanInfo->Bssid( bssid );
            LOGIT11("%02d: %02X:%02X:%02X:%02X:%02X:%02X sig.str %d, type %d, name(%02d): %s",
                    count, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5],
                    nw.iSignalStrength, nw.iType, debug_name.Length(), debug_name.PtrZ())
            }
#endif // _DEBUG
        } // for

    // Go through the items one by one starting from the largest signal strength.
    // Always uses the first item in array, since it is removed in the end of loop.
    while ( sortedNetworkNames.Count() > 0 && aNetInfo.iCount < KConnMonMaxNetworkCount )
        {
        TConnMonNetwork &item( sortedNetworkNames[0] );

        TBool duplicate( EFalse );
        for ( TUint i = 0; i < aNetInfo.iCount; i++ )
            {
            if ( aNetInfo.iNetwork[i].iType == item.iType &&
                 aNetInfo.iNetwork[i].iName == item.iName )
                {
                duplicate = ETrue;
                }
            }
        if ( !duplicate )
            {
            // Adding the item
            aNetInfo.iNetwork[aNetInfo.iCount].iType = item.iType;
            aNetInfo.iNetwork[aNetInfo.iCount].iSignalStrength = item.iSignalStrength;
            aNetInfo.iNetwork[aNetInfo.iCount].iName.Copy( item.iName );
#ifdef _DEBUG
            // One letter longer than max WLAN network name, since we are adding a '\0' when printing
            TBuf<CConnMonWlanNetwork::KMaxNameLength+1> name;
            ConnMonUtils::TDes8ToTDes( aNetInfo.iNetwork[aNetInfo.iCount].iName, name );
            LOGIT3("Added network %.2d, sig.str %d, name: %s",
                    aNetInfo.iCount,
                    aNetInfo.iNetwork[aNetInfo.iCount].iSignalStrength,
                    name.PtrZ())
#endif // _DEBUG
            aNetInfo.iCount++;
            }

        sortedNetworkNames.Remove( 0 );
        }

    CleanupStack::PopAndDestroy( &sortedNetworkNames );

    LOGEXITFN("CWlanSupport::ParseNetworkNamesL()")
    }


// -----------------------------------------------------------------------------
// CWlanSupport::CompareNetworkStrengths
//
// Compares two WLAN network signal strengths.
// It should return:
// 1. zero, if the two objects are equal.
// 2. a negative value, if the first object is less than the second.
// 3. a positive value, if the first object is greater than the second.
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::CompareNetworkStrengths(
        const TConnMonNetwork& aFirst,
        const TConnMonNetwork& aSecond )
    {
    //LOGENTRFN("CWlanSupport::CompareNetworkStrengths()")
    TInt result( 0 );

    if ( aFirst.iSignalStrength < aSecond.iSignalStrength )
        {
        result = -1;
        }
    else if ( aFirst.iSignalStrength > aSecond.iSignalStrength )
        {
        result = 1;
        }

    //LOGEXITFN1("CWlanSupport::CompareNetworkStrengths()", result)
    return result;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::AppendAvailableIaps
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::AppendAvailableIaps( RArray<TUint>& aIdArray )
    {
    //LOGENTRFN("CWlanSupport::AppendAvailableIaps()")
    TInt err( KErrNone );

    if ( iEnableScan )
        {
        iAvailableWlanIapsCache.Reset();
        err = iWlanMgmt->GetAvailableIaps( iAvailableWlanIapsCache );
        LOGIT2("WLAN IAPs available: %d, err <%d> (from WLAN engine)", iAvailableWlanIapsCache.Count(), err)
        iEnableScan = EFalse;
        }
    else
        {
        LOGIT1("WLAN IAPs available: %d (from cache, no scan)", iAvailableWlanIapsCache.Count())
        }

    const TInt wlanIapCount( iAvailableWlanIapsCache.Count() );
    if ( KErrNone == err && wlanIapCount > 0 )
        {
        for ( TInt i = 0; i < wlanIapCount; i++ )
            {
            err = aIdArray.Append( iAvailableWlanIapsCache[i] );
            if ( err )
                {
                break;
                }
            }
        }

    //LOGEXITFN1("CWlanSupport::AppendAvailableIaps()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::AppendAvailableIapsBySsid
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::AppendAvailableIapsBySsidL( RArray<TUint>& aIdArray )
    {
    LOGENTRFN("CWlanSupport::AppendAvailableIapsBySsidL()")
    TInt ret( KErrNone );

    // Get ssid of current connection
    TWlanSsid currentSsid;
    ret = iWlanMgmt->GetConnectionSsid( currentSsid );
    if ( KErrNone != ret )
        {
        LOGIT1("ERROR reading current connection ssid from wlan engine <%d>", ret)
        ret = KErrDisconnected;
        }
    else
        {
        LOGTIMINGSTART("CWlanSupport::AppendAvailableIapsBySsidL")

        // Create a session with the latest version CommsDat
        CMDBSession* db = CMDBSession::NewLC( CMDBSession::LatestVersion() );
        db->SetAttributeMask( ECDProtectedWrite );

        TBuf<CConnMonWlanNetwork::KMaxNameLength> ssid16;
        CnvUtfConverter::ConvertToUnicodeFromUtf8( ssid16, currentSsid );
        LOGIT2("AppendAvailableIapsBySsidL: ssid: %S, length: %d", &ssid16, ssid16.Length())

        // Create wlan service record set
        CMDBRecordSet<CCDWlanServiceRecord>* wlanSet =
                new( ELeave ) CMDBRecordSet<CCDWlanServiceRecord>(
                        CCDWlanServiceRecord::TableIdL( *db ) );
        CleanupStack::PushL( wlanSet );

        // Create wlan service record
        CCDWlanServiceRecord* wlanRecord =
            new( ELeave ) CCDWlanServiceRecord( CCDWlanServiceRecord::TableIdL( *db ) );
        CleanupStack::PushL( wlanRecord );

        // Set ssid field in wlan service record (see wlancontainer.h)
        wlanRecord->iWLanSSID.SetL( ssid16 );
        // Append wlan service record to wlan service record set (currently empty)
        wlanSet->iRecords.AppendL( wlanRecord );
        CleanupStack::Pop( wlanRecord ); // Ownership moved

        // Find matching wlan service records (all wlan service records with same SSID)
        if ( wlanSet->FindL( *db ) )
            {
            TInt wlanRecordCount( wlanSet->iRecords.Count() );
            LOGIT1("AppendAvailableIapsBySsidL: wlan record count: %d", wlanRecordCount)

            // Loop for each record found
            for ( TInt i = 0; i < wlanRecordCount; i++ )
                {
                // Load current record to make sure all fields are up to date
                // LoadL() will only look at ElementId and updates the rest of the fields
                wlanRecord->SetElementId( wlanSet->iRecords[i]->ElementId() );
                wlanRecord->LoadL( *db );

                // Create IAP record
                CCDIAPRecord* iapRecord =
                    static_cast<CCDIAPRecord*>( CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
                CleanupStack::PushL( iapRecord );

                iapRecord->iServiceType.SetL( TPtrC( KCDTypeNameLANService ) );
                // Set the service field to the same value as in the wlan service record we are currently looking
                iapRecord->iService = wlanRecord->iWlanServiceId;
                // Find matching IAP record
                if ( iapRecord->FindL( *db ) )
                    {
                    // Read IAP ID and append to buffer
                    TUint iapId = iapRecord->RecordId();
                    LOGIT2("AppendAvailableIapsBySsidL: index: %d, id: %d", i, iapId)
                    aIdArray.Append( iapId ); // Ignore if Append fails, ID will be absent from availability list.
                    }
                else
                    {
                    LOGIT("AppendAvailableIapsBySsidL: no matching IAP record found")
                    }
                CleanupStack::PopAndDestroy( iapRecord );
                }
            }
        else
            {
            LOGIT("AppendAvailableIapsBySsidL: no matching IAP records found")
            ret = KErrNotFound;
            }
        CleanupStack::PopAndDestroy( wlanSet );
        CleanupStack::PopAndDestroy( db );

        LOGTIMINGEND("CWlanSupport::AppendAvailableIapsBySsidL")
        }

    LOGEXITFN1("CWlanSupport::AppendAvailableIapsBySsidL()", ret)
    return ret;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::GetConnectionSsid
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::GetConnectionSsid( TDes& aSsid )
    {
    LOGENTRFN("CWlanSupport::GetConnectionSsid()")
    TInt err( KErrNone );
    TWlanSsid ssid;

    err = iWlanMgmt->GetConnectionSsid( ssid );
    if ( KErrNone == err )
        {
        CnvUtfConverter::ConvertToUnicodeFromUtf8( aSsid, ssid );
#ifdef _DEBUG
        // One letter longer than max WLAN network name, since we are adding a '\0' when printing
        TBuf<CConnMonWlanNetwork::KMaxNameLength + 1> debug_name( aSsid );
        LOGIT1("GetConnectionSsid: SSID <%s>", debug_name.PtrZ())
#endif // _DEBUG
        }

    LOGEXITFN1("CWlanSupport::GetConnectionSsid()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::GetConnectionMode
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::GetConnectionMode( TInt& aConnectionMode )
    {
    LOGENTRFN("CWlanSupport::GetConnectionMode()")
    TInt err( KErrNone );

    TWlanConnectionMode mode( EWlanConnectionModeNotConnected );
    aConnectionMode = KErrNotFound;

    // Get connection mode from WLAN engine
    err = iWlanMgmt->GetConnectionMode( mode );
    if ( KErrNone == err )
        {
        switch ( mode )
            {
            case EWlanConnectionModeNotConnected:
                err = KErrNotFound; // Connection does not exist
                break;
            case EWlanConnectionModeInfrastructure:
                aConnectionMode = EConnMonInfraStructure;
                break;
            case EWlanConnectionModeAdhoc:
                aConnectionMode = EConnMonAdHoc;
                break;
            case EWlanConnectionModeSecureInfra:
                aConnectionMode = EConnMonSecureInfra;
                break;
            case EWlanConnectionModeSearching:
                err = KErrNotFound; // Connection does not exist
                break;
            default:
                err = KErrNotFound;
                break;
            }
        LOGIT1("CWlanSupport::GetConnectionMode: %d", aConnectionMode)
        }

    LOGEXITFN1("CWlanSupport::GetConnectionMode()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::GetConnectionSecurity
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::GetConnectionSecurity( TInt& aConnectionSecurity )
    {
    LOGENTRFN("CWlanSupport::GetConnectionSecurity()")
    TInt err( KErrNone );

    TWlanConnectionSecurityMode mode( EWlanConnectionSecurityOpen );

    // Get security mode
    err = iWlanMgmt->GetConnectionSecurityMode( mode );
    if ( KErrNone == err )
        {
        switch ( mode )
            {
            case EWlanConnectionSecurityOpen:
                aConnectionSecurity = EConnMonSecurityOpen;
                break;
            case EWlanConnectionSecurityWep:
                aConnectionSecurity = EConnMonSecurityWep;
                break;
            case EWlanConnectionSecurity802d1x:
                aConnectionSecurity = EConnMonSecurity802d1x;
                break;
            case EWlanConnectionSecurityWpa:
                aConnectionSecurity = EConnMonSecurityWpa;
                break;
            case EWlanConnectionSecurityWpaPsk:
                aConnectionSecurity = EConnMonSecurityWpaPsk;
                break;
            default:
                aConnectionSecurity = EConnMonSecurityOpen;
                break;
            }
        LOGIT1("GetConnectionSecurity: security mode %d", aConnectionSecurity)
        }

    LOGEXITFN1("CWlanSupport::GetConnectionSecurity()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::GetConnectionSignalQuality
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::GetConnectionSignalQuality( TInt& aSignalQuality )
    {
    LOGENTRFN("CWlanSupport::GetConnectionSignalQuality()")
    TInt err( KErrNone );
    TInt32 rssi( 0 );

    err = iWlanMgmt->GetConnectionSignalQuality( rssi );
    if ( KErrNone == err )
        {
        aSignalQuality = static_cast<TInt>( rssi );
        LOGIT1("CWlanSupport::GetConnectionSignalQuality: %d dBm", aSignalQuality)
        }

    LOGEXITFN1("CWlanSupport::GetConnectionSignalQuality()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::GetConnectionBssid
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::GetConnectionBssid( TBuf8<CConnMonWlanNetwork::KWlanBssId>& aBssid )
    {
    LOGENTRFN("CWlanSupport::GetConnectionBssid()")
    TInt err( KErrNone );

    err = iWlanMgmt->GetConnectionBssid( aBssid );
    #ifdef _DEBUG
    if ( KErrNone == err )
        {
        LOGIT6("Bssid %02X:%02X:%02X:%02X:%02X:%02X", aBssid[0], aBssid[1], aBssid[2], aBssid[3], aBssid[4], aBssid[5])
        }
    #endif // _DEBUG

    LOGEXITFN1("CWlanSupport::GetConnectionBssid()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::GetTransmitPower
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::GetTransmitPower( TUint& aPower )
    {
    LOGENTRFN("CWlanSupport::GetTransmitPower()")
    TInt err( KErrNone );

    if ( iEventWatcher )
        {
        aPower = iEventWatcher->TxPower();
        LOGIT1("CWlanSupport:GetTransmitPower: %d mW", aPower)
        }
    else
        {
        err = KErrUnknown;
        }

    LOGEXITFN1("CWlanSupport::GetTransmitPower()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::ActivateNotificationsL
// -----------------------------------------------------------------------------
//
void CWlanSupport::ActivateNotificationsL()
    {
    //LOGENTRFN("CWlanSupport::ActivateNotificationsL()")

    if ( !iEventWatcher )
        {
        iEventWatcher = CWlanEvent::NewL( this, iServer );
        iWlanMgmt->ActivateNotificationsL( *iEventWatcher );
        }

    //LOGEXITFN("CWlanSupport::ActivateNotificationsL()")
    }

// -----------------------------------------------------------------------------
// CWlanSupport::CancelNotifications
// -----------------------------------------------------------------------------
//
void CWlanSupport::CancelNotifications()
    {
    LOGENTRFN("CWlanSupport::CancelNotifications()")

    if ( iEventWatcher )
        {
        iWlanMgmt->CancelNotifications();

        delete iEventWatcher;
        iEventWatcher = NULL;
        }

    LOGEXITFN("CWlanSupport::CancelNotifications()")
    }

// -----------------------------------------------------------------------------
// CWlanSupport::GetWLANNetworksL
// -----------------------------------------------------------------------------
//
void CWlanSupport::GetWLANNetworksL( RConnMonWLANNetworksArray& aWlanNetworks )
    {
    LOGENTRFN("CWlanSupport::GetWLANNetworksL()")

    // ScanInfo points to a CScanInfo object that will contain results from a scan.
    CWlanScanInfo* scanInfo = CWlanScanInfo::NewL();
    CleanupStack::PushL( scanInfo );

    TInt ret = iWlanMgmt->GetScanResults( *scanInfo );
    if ( KErrNone == ret )
       {
        // For logging only
        LOG(TInt count(0);)

        // Iterate through scan results
        for ( scanInfo->First(); !scanInfo->IsDone(); scanInfo->Next() )
            {
            LOG(count++;)
            LOGIT1("CWlanSupport::GetWLANNetworksL: count %d", count)

#ifdef _DEBUG
            // BSSID
            TWlanBssid bssid;
            scanInfo->Bssid( bssid );
            LOGIT6("Bssid %02X:%02X:%02X:%02X:%02X:%02X",
                    bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5])
#endif //_DEBUG

            // Connection mode
            TUint connectionMode( EConnMonInfraStructure );
            if ( scanInfo->Capability() & KWlan802Dot11CapabilityEssMask )
                {
                connectionMode = EConnMonInfraStructure;
                }
            else
                {
                connectionMode = EConnMonAdHoc;
                }

            // Signal strength
            TUint signalStrength(scanInfo->RXLevel());
            LOGIT1("CWlanSupport::GetWLANNetworksL: signalStrength %d", signalStrength)

            // SecurityMode
            TUint8 ieLength( 0 );
            const TUint8* wpaData;

            TUint securityMode( EConnMonSecurityOpen );
            if ( scanInfo->Capability() & KWlan802Dot11CapabilityPrivacyMask )
                {
                if ( ( scanInfo->InformationElement(
                        KWlan802Dot11RsnIE,
                        ieLength,
                        &wpaData ) == KErrNone ) ||
                     ( scanInfo->WpaIE(
                        ieLength,
                        &wpaData ) == KErrNone ) )
                    {
                    securityMode = EConnMonSecurityWpa; // WPA
                    }
                else
                    {
                    securityMode = EConnMonSecurityWep; // WEP
                    }
                }
            LOGIT1("CWlanSupport::GetWLANNetworksL: securityMode %d", securityMode)

            // SSID == name
            TBuf8<TConnMonWLANNetwork::KMaxNameLength> name8;
            TUint8 ieLen( 0 );
            const TUint8* ieData;
            ieData = NULL;

            ret = scanInfo->InformationElement( KWlan802Dot11SsidIE, ieLen, &ieData );
            if ( KErrNone == ret && ieLen <= CConnMonWlanNetwork::KMaxNameLength )
                {
                // ieData points to the beginning of the 802.11 SSID payload data i.e.
                // the header is bypassed. ieLen contains the length of payload data.
                name8.Copy( ieData, ieLen );
                }
            LOGIT1("CWlanSupport::GetWLANNetworksL: ieLen %d", ieLen)

            TBuf<TConnMonWLANNetwork::KMaxNameLength> name;
            ConnMonUtils::TDes8ToTDes( name8, name );

#ifdef _DEBUG
            // One letter longer than max WLAN network name, since we are adding a '\0' when printing
            TBuf<CConnMonWlanNetwork::KMaxNameLength+1> debug_name;
            ConnMonUtils::TDes8ToTDes( name8, debug_name );
            LOGIT2("CWlanSupport::GetWLANNetworksL: name <%s>, length %d",
                    debug_name.PtrZ(), name.Length())
#endif // _DEBUG

            RArray<TInt> extrafields;
            CleanupClosePushL( extrafields );

            aWlanNetworks.AppendL( TConnMonWLANNetwork(
                    name,
                    connectionMode,
                    signalStrength,
                    securityMode,
                    extrafields ) );
            CleanupStack::PopAndDestroy( &extrafields );
            }
        aWlanNetworks.Sort( TLinearOrder<TConnMonWLANNetwork>(
                TConnMonWLANNetwork::LessBySignal ) );
        }
    else
        {
        // Could not get any scan results
        User::Leave( ret );
        }
    CleanupStack::PopAndDestroy( scanInfo );

    LOGEXITFN("CWlanSupport::GetWLANNetworksL()")
    }


// -----------------------------------------------------------------------------
// CWlanSupport::GetWlanNetworksL
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::GetWlanNetworksL( const RMessage2& aMessage )
    {
    LOGENTRFN("CWlanSupport::GetWlanNetworksL()")

    // Add session to wlan session table (if not already there)
    TInt index = AddWlanSessionL( aMessage );

    iWlanSessionsPtrArray[index]->GetScanResultsL( aMessage );

    LOGEXITFN1("CWlanSupport::GetWlanNetworksL()", KRequestPending)
    return KRequestPending;
    }


// -----------------------------------------------------------------------------
// CWlanSupport::ParseWlanNetworks
// -----------------------------------------------------------------------------
//
void CWlanSupport::ParseWlanNetworksL(
        CWlanScanInfo* aScanInfo,
        RConnMonWlanNetworksPtrArray& aWlanNetworks )
    {
    LOGENTRFN("CWlanSupport::ParseWlanNetworksL()")
    LOG(TInt count( 0 );) // For logging only

    LOGIT("ParseWlanNetworksL: WLAN scan info:")
    // Iterate through scan results
    for ( aScanInfo->First(); !aScanInfo->IsDone(); aScanInfo->Next() )
        {
        // BSSID
        TWlanBssid bssid;
        aScanInfo->Bssid( bssid );

        // Connection mode
        TUint connectionMode( EConnMonInfraStructure );
        if ( aScanInfo->Capability() & KWlan802Dot11CapabilityEssMask )
            {
            connectionMode = EConnMonInfraStructure;
            }
        else
            {
            connectionMode = EConnMonAdHoc;
            }

        // Signal strength
        TUint signalStrength( aScanInfo->RXLevel() );

        // SecurityMode
        TUint8 ieLength( 0 );
        const TUint8* wpaData;

        TUint securityMode( EConnMonSecurityOpen );
        if ( aScanInfo->Capability() & KWlan802Dot11CapabilityPrivacyMask )
            {
            if ( ( aScanInfo->InformationElement(
                    KWlan802Dot11RsnIE,
                    ieLength,
                    &wpaData ) == KErrNone ) ||
                 ( aScanInfo->WpaIE(
                    ieLength,
                    &wpaData ) == KErrNone ) )
                {
                securityMode = EConnMonSecurityWpa; // WPA
                }
            else
                {
                securityMode = EConnMonSecurityWep; // WEP
                }
            }

        // SSID == name
        TBuf8<CConnMonWlanNetwork::KMaxNameLength> name8;
        TUint8 ieLen( 0 );
        const TUint8* ieData;
        ieData = NULL;

        TInt ret = aScanInfo->InformationElement( KWlan802Dot11SsidIE, ieLen, &ieData );
        if ( ret == KErrNone && ieLen <= CConnMonWlanNetwork::KMaxNameLength )
            {
            // ieData points to the beginning of the 802.11 SSID payload data i.e.
            // the header is bypassed. ieLen contains the length of payload data.
            name8.Copy( ieData, ieLen );
            }

        TBuf<CConnMonWlanNetwork::KMaxNameLength> name;
        ConnMonUtils::TDes8ToTDes( name8, name );

#ifdef _DEBUG
        LOG(count++;)
        // One letter longer than max WLAN network name, since we are adding a '\0' when printing
        TBuf<CConnMonWlanNetwork::KMaxNameLength + 1> debug_name;
        ConnMonUtils::TDes8ToTDes( name8, debug_name );
        LOGIT11("%02d: %02X:%02X:%02X:%02X:%02X:%02X sig.str %d, sec.mode %d, name(%02d): %s",
                count, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5],
                signalStrength, securityMode, name.Length(), debug_name.PtrZ())
#endif // _DEBUG

        // Create CConnMonWlanNetwork object with scan results
        CConnMonWlanNetwork* net = CConnMonWlanNetwork::NewL(
                name,
                connectionMode,
                signalStrength,
                securityMode,
                bssid,
                KNullDesC() );
        CleanupStack::PushL( net );
        aWlanNetworks.AppendL( net );
        CleanupStack::Pop( net );
        }
    LOGIT("ParseWlanNetworksL: sorting wlan networks")
    aWlanNetworks.Sort( TLinearOrder<CConnMonWlanNetwork>(
            CConnMonWlanNetwork::CompareBySignal ) );

    LOGEXITFN("CWlanSupport::ParseWlanNetworksL()")
    }


// -----------------------------------------------------------------------------
// CWlanSupport::GetCurrentWlanNetwork
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::GetCurrentWlanNetworkL(
        RConnMonWlanNetworksPtrArray& aWlanNetwork )
    {
    LOGENTRFN("CWlanSupport::GetCurrentWlanNetworkL()")
    TInt err( KErrNone );

    TInt connectionMode;
    TInt connectionSecurity;
    TInt connectionSignalQuality;
    TBuf<CConnMonWlanNetwork::KMaxNameLength> connectionSsid;
    TBuf8<CConnMonWlanNetwork::KWlanBssId> connectionBssid;

    // Get connection mode to see if connection exists
    err = GetConnectionMode( connectionMode );
    if ( KErrNone == err )
        {
        // Get connection security mode
        err = GetConnectionSecurity( connectionSecurity );
        }
    if ( KErrNone == err )
        {
        // Get connection signal quality
        err = GetConnectionSignalQuality( connectionSignalQuality );
        }
    if ( KErrNone == err )
        {
        // Get connection SSID
        err = GetConnectionSsid( connectionSsid );
        }
    if ( KErrNone == err )
        {
        // Get connection BSSID
        err = GetConnectionBssid( connectionBssid );
        }
    if ( KErrNone == err )
        {
        // Create CConnMonWlanNetwork object with results
        CConnMonWlanNetwork* net = CConnMonWlanNetwork::NewL(
                connectionSsid,
                connectionMode,
                connectionSignalQuality,
                connectionSecurity,
                connectionBssid,
                KNullDesC() );
        CleanupStack::PushL( net );
        aWlanNetwork.AppendL( net );
        CleanupStack::Pop( net );
        }

    LOGEXITFN1("CWlanSupport::GetCurrentWlanNetworkL()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CWlanSupport::GetWlanProbeRawBuffersL
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::GetWlanProbeRawBuffersL( const RMessage2& aMessage )
    {
    LOGENTRFN("CWlanSupport::GetWlanProbeRawBuffersL()")

    // Add session to wlan session table (if not already there)
    TInt index = AddWlanSessionL( aMessage );
    iWlanSessionsPtrArray[index]->GetScanResultsL( aMessage );

    LOGEXITFN1("CWlanSupport::GetWlanProbeRawBuffersL()", KRequestPending)
    return KRequestPending;
    }


// -----------------------------------------------------------------------------
// CWlanSupport::ParseWlanNetworks
// -----------------------------------------------------------------------------
//
void CWlanSupport::ParseWlanProbeRawBuffersL(
        CWlanScanInfo* aScanInfo,
        RConnMonWlanProbeRawBuffersPtrArray& aWlanProbeRawBuffers )
    {
    LOGENTRFN("CWlanSupport::ParseWlanProbeRawBuffersL()")

    // For logging only
    LOG(TInt count( 0 );)

    // Iterate through scan results.
    for( aScanInfo->First(); !aScanInfo->IsDone(); aScanInfo->Next() )
        {
        LOG(count++;)
        LOGIT1("CWlanSupport::ParseWlanProbeRawBuffersL: network number %d", count)

        // Length of the raw data == Size - CNTRL_HEADER_LEN (==12)
        TUint16 length( 0 );
        length = aScanInfo->Size() - KWlanProbeControlHeaderLength;
        LOGIT1("CWlanSupport::ParseWlanProbeRawBuffersL: length %d", length)

        // Get pointer start of the raw data
        const TWlanScanInfoFrame* probeRawDataStartPtr =
                aScanInfo->Current() + KWlanProbeControlHeaderLength;

        // Create temporary heap descriptor for raw data
        HBufC8* rawBuffer = HBufC8::NewL( length );
        CleanupStack::PushL( rawBuffer );
        rawBuffer->Des().Copy( probeRawDataStartPtr, length );

#ifdef _CONNMON_HEXDUMP_LOGGING
#if ( _CONNMON_LOG_OUTPUT == 2 ) // Works only when logging to file
        const TText* header = _S( "header" );
        const TText* margin = _S( "margin" );
        Log::HexDump( header, margin, probeRawDataStartPtr, length );
#endif // _CONNMON_LOG_OUTPUT
#endif // _CONNMON_HEXDUMP_LOGGING

        // Create CConnMonWlanProbeRawBuffer object
        CConnMonWlanProbeRawBuffer* net = CConnMonWlanProbeRawBuffer::NewL( rawBuffer );
        CleanupStack::PushL( net );

        // Add object pointer to array
        aWlanProbeRawBuffers.AppendL( net );

        CleanupStack::Pop( net );
        // Clear temporary heap descriptor
        CleanupStack::PopAndDestroy( rawBuffer );
        LOGIT(".")
        }

    LOGEXITFN("CWlanSupport::ParseWlanProbeRawBuffersL()")
    }


// -----------------------------------------------------------------------------
// CWlanSupport::GetWlanSsidNetworksL
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::GetWlanSsidNetworksL( const RMessage2& aMessage )
    {
    LOGENTRFN("CWlanSupport::GetWlanSsidNetworksL()")
    TInt result( KErrNone );

    // Add session to wlan session table (if not already there)
    TInt index = AddWlanSessionL( aMessage );
    result = iWlanSessionsPtrArray[index]->GetSsidScanResultsL( aMessage );

    LOGEXITFN1("CWlanSupport::GetWlanSsidNetworksL()", result)
    return result;
    }


// -----------------------------------------------------------------------------
// CWlanSupport::AddWlanSessionL
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::AddWlanSessionL( const RMessage2& aMessage )
    {
    LOGENTRFN("CWlanSupport::AddWlanSessionL()")

    // Set active session Id to session object
    CConnMonSession* sessionId = (CConnMonSession*)aMessage.Session();

    // Check if session exist in WLAN sessions array
    TInt index = WlanSessionIndex( sessionId ); // Returns either KErrNotFound or a valid index
    if ( index != KErrNotFound )
        {
        LOGIT2("AddWlanSessionL: session id %d already in array at index %d", sessionId, index)
        }
    else
        {
        CWlanSession* session = CWlanSession::NewL( sessionId, this );
        CleanupStack::PushL( session );
        iWlanSessionsPtrArray.AppendL( session );
        CleanupStack::Pop( session );
        index = iWlanSessionsPtrArray.Count() - 1;
        LOGIT2("AddWlanSessionL: new session id %d added to array at index %d", sessionId, index)
        }

    LOGEXITFN1("CWlanSupport::AddWlanSessionL()", index)
    return index;
    }


// -----------------------------------------------------------------------------
// CWlanSupport::DeleteWlanSession
// -----------------------------------------------------------------------------
//
void CWlanSupport::DeleteWlanSession( CConnMonSession* aSessionId )
    {
    LOGENTRFN("CWlanSupport::DeleteWlanSession()")

    // Check if session found from WLAN sessions array
    TInt index = WlanSessionIndex( aSessionId );

    if ( index != KErrNotFound )
        {
        delete iWlanSessionsPtrArray[index];
        iWlanSessionsPtrArray[index] = NULL;
        iWlanSessionsPtrArray.Remove( index );
        LOGIT2("DeleteWlanSession: deleted session id %d from array index %d", aSessionId, index)
        }

    LOGEXITFN("CWlanSupport::DeleteWlanSession()")
    }


// -----------------------------------------------------------------------------
// CWlanSupport::WlanSessionIndex
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::WlanSessionIndex( CConnMonSession* aSessionId )
    {
    LOGENTRFN("CWlanSupport::WlanSessionIndex()")
    TInt result( KErrNotFound );

    // Check session index of the WLAN sessions array
    for ( TInt index = 0; index < iWlanSessionsPtrArray.Count(); index++ )
        {
        if ( aSessionId == iWlanSessionsPtrArray[index]->GetSessionId() )
            {
            LOGIT2("WlanSessionIndex: session id %d found from index %d", aSessionId, index)
            result = index;
            break;
            }
        }

    LOGEXITFN1("CWlanSupport::WlanSessionIndex()", result)
    return result;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::SetIntAttribute
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::SetIntAttributeL(
        const RMessage2& aMessage,
        const TInt aValue )
    {
    LOGENTRFN("CWlanSupport::SetIntAttributeL()")
    TInt result( KErrNone );
    TUint attribute = aMessage.Int2();

    // Add session to wlan session table (if not already there)
    TInt index = AddWlanSessionL( aMessage );

    switch ( attribute )
        {
        case KWlanScanCacheLifetime:
            iWlanSessionsPtrArray[index]->SetWlanScanCacheLifetime( aValue );
            LOGIT1("SetIntAttributeL: KWlanScanCacheLifetime set to value <%d>", aValue)
            result = KErrNone;
            break;
        default:
            LOGIT1("SetIntAttributeL: Error, unknown attribute <%d>", attribute)
            result = KErrArgument;
            break;
        }

    LOGEXITFN1("CWlanSupport::SetIntAttributeL()", result)
    return result;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::SetUintAttribute
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::SetUintAttributeL(
        const RMessage2& aMessage,
        const TUint aValue )
    {
    LOGENTRFN("CWlanSupport::SetUintAttributeL()")
    TInt result( KErrNone );
    TUint attribute = aMessage.Int2();

    // Add session to wlan session table (if not already there)
    TInt index = AddWlanSessionL( aMessage );

    switch ( attribute )
        {
        case KWlanScanMaxDelay:
            iWlanSessionsPtrArray[index]->SetWlanScanMaxDelay( aValue );
            LOGIT1("SetUintAttributeL: KWlanScanMaxDelay set to value %d", aValue)
            result = KErrNone;
            break;
        default:
            LOGIT1("SetUintAttributeL: Error, unknown attribute %d", attribute)
            result = KErrArgument;
            break;
        }

    LOGEXITFN1("CWlanSupport::SetUintAttributeL()", result)
    return result;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::GetIntAttribute
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::GetIntAttributeL( const RMessage2& aMessage, TInt& aValue )
    {
    LOGENTRFN("CWlanSupport::GetIntAttributeL()")
    TInt err( KErrNone );
    TUint attribute = aMessage.Int2();

    // Add session to wlan session table (if not already there)
    TInt index = AddWlanSessionL( aMessage );
    switch ( attribute )
        {
        case KWlanScanCacheLifetime:
            aValue = iWlanSessionsPtrArray[index]->GetWlanScanCacheLifetime();
            LOGIT1("GetIntAttributeL: KWlanScanCacheLifetime got value %d", aValue)
            err = KErrNone;
            break;
        default:
            LOGIT1("GetIntAttributeL: Error, unknown attribute %d", attribute)
            err = KErrArgument;
            break;
        }

    LOGEXITFN1("CWlanSupport::GetIntAttributeL()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::GetUintAttribute
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::GetUintAttributeL( const RMessage2& aMessage, TUint& aValue )
    {
    LOGENTRFN("CWlanSupport::GetUintAttributeL()")
    TInt err( KErrNone );
    TUint attribute( aMessage.Int2() );

    // Add session to wlan session table (if not already there)
    TInt index = AddWlanSessionL( aMessage );
    switch ( attribute )
        {
        case KWlanScanMaxDelay:
            aValue = iWlanSessionsPtrArray[index]->GetWlanScanMaxDelay();
            LOGIT1("GetUintAttributeL: KWlanScanMaxDelay got value %d", aValue)
            err = KErrNone;
            break;
        default:
            LOGIT1("GetUintAttributeL: Error, unknown attribute %d", attribute)
            err = KErrArgument;
            break;
        }

    LOGEXITFN1("CWlanSupport::GetUintAttributeL()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::SetStringAttribute
//
// Currently assumes the descriptor attribute 'aValue' length has been checked
// earlier and is short enough.
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::SetStringAttributeL(
        const RMessage2& aMessage,
        const TDes& aValue )
    {
    LOGENTRFN("CWlanSupport::SetStringAttributeL()")
    TInt err( KErrNone );
    TUint attribute( aMessage.Int2() );

    // Add session to wlan session table (if not already there)
    TInt index = AddWlanSessionL( aMessage );
    switch ( attribute )
        {
        case KWlanSsid:
            {
            TBuf8<CConnMonWlanNetwork::KMaxNameLength> ssid;
            ConnMonUtils::TDesToTDes8( aValue, ssid );
            iWlanSessionsPtrArray[index]->SetWlanSsid( ssid );
            }
            break;
        default:
            LOGIT1("SetStringAttributeL: invalid attribute %d", attribute)
            err = KErrArgument;
            break;
        }

    LOGEXITFN1("CWlanSupport::SetStringAttributeL()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::GetStringAttribute
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::GetStringAttributeL(
        const RMessage2& aMessage,
        TDes& aValue )
    {
    LOGENTRFN("CWlanSupport::GetStringAttributeL()")
    TInt err( KErrNone );
    TUint attribute( aMessage.Int2() );

    // Add session to wlan session table (if not already there)
    TInt index = AddWlanSessionL( aMessage );

    switch ( attribute )
        {
        case KWlanSsid:
            {
            TBuf8<CConnMonWlanNetwork::KMaxNameLength> ssid;
            iWlanSessionsPtrArray[index]->GetWlanSsid( ssid );
            ConnMonUtils::TDes8ToTDes( ssid, aValue );
            }
            break;
        default:
            LOGIT1("GetStringAttributeL: invalid attribute %d", attribute)
            err = KErrArgument;
            break;
        }

    LOGEXITFN1("CWlanSupport::GetStringAttributeL()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::CancelAsyncRequest
// -----------------------------------------------------------------------------
//
TInt CWlanSupport::CancelAsyncRequest( const RMessage2& aMessage )
    {
    LOGENTRFN("CWlanSupport::CancelAsyncRequest()")
    TInt err( KErrNone );

    // Check array index
    TInt index = WlanSessionIndex( (CConnMonSession*)aMessage.Session() );

    // Check if session exist in WLAN sessions array
    if ( KErrNotFound == index )
        {
        // Nothing to cancel on WLAN side
        err = KErrNotFound;
        }
    else
        {
        // Cancel client active requests
        iWlanSessionsPtrArray[index]->CancelActiveRequests( aMessage.Int0() );
        }

    LOGEXITFN1("CWlanSupport::CancelAsyncRequest()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::EnableEventsToClients
// -----------------------------------------------------------------------------
//
void CWlanSupport::EnableEventsToClients()
    {
    iEnableEventsToClients = ETrue;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::DisableEventsToClients
// -----------------------------------------------------------------------------
//
void CWlanSupport::DisableEventsToClients()
    {
    iEnableEventsToClients = EFalse;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::EventsToClientsEnabled
// -----------------------------------------------------------------------------
//
TBool CWlanSupport::EventsToClientsEnabled()
    {
    return iEnableEventsToClients;
    }

// -----------------------------------------------------------------------------
// CWlanSupport::WlanRssGoodEnough
// -----------------------------------------------------------------------------
//
TBool CWlanSupport::WlanRssGoodEnough()
    {
    LOGENTRFN("CWlanSupport::WlanRssGoodEnough()")

    // Return ETrue by default. Return EFalse only if there is no error
    // situation and WLAN engine has reported received signal strength
    // class as weak.
    TBool value( ETrue );

    if ( iEventWatcher )
        {
        if ( iEventWatcher->RssClass() == EConnMonWlanRssClassWeak )
            {
            value = EFalse;
            }
        }

    LOGEXITFN1("CWlanSupport::WlanRssGoodEnough()", value)
    return value;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWlanEvent::NewL
// -----------------------------------------------------------------------------
//
CWlanEvent* CWlanEvent::NewL( CWlanSupport* aWlanSupport, CConnMonServer* aServer )
    {
    CWlanEvent* self = new( ELeave ) CWlanEvent( aWlanSupport, aServer );
    return self;
    }

// -----------------------------------------------------------------------------
// CWlanEvent::CWlanEvent
// -----------------------------------------------------------------------------
//
CWlanEvent::CWlanEvent(
        CWlanSupport* aWlanSupport,
        CConnMonServer* aServer )
        :
        iWlanSupport( aWlanSupport ),
        iServer( aServer ),
        iWlanAvailable( EFalse ),
        iHasAvailabilityKnowledge( EFalse ),
        iRssClass( EConnMonWlanRssClassUnknown ),
        iRss( 0 )
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CWlanEvent::~CWlanEvent()
    {
    iWlanSupport = NULL;
    iServer = NULL;
    }

// -----------------------------------------------------------------------------
// CWlanEvent::NewNetworksDetected
// New networks have been detected during scan.
// -----------------------------------------------------------------------------
//
void CWlanEvent::NewNetworksDetected()
    {
    LOGIT(".")
    LOGENTRFN("CWlanEvent::NewNetworksDetected()")
    iHasAvailabilityKnowledge = ETrue;
    iWlanAvailable = ETrue;

    if ( iWlanSupport->EventsToClientsEnabled() )
        {
        TEventInfo eventInfo;
        // Check bearer availability threshold
        TUint threshold( 0 );
        iServer->CalculateThreshold( EBearerIdAll, EBearerAvailabilityThreshold, threshold );

        // Send bearer availability change event only if threshold is set by some of the clients
        if ( threshold > 0 )
            {
            // Create event info
            eventInfo.Reset();
            eventInfo.iConnectionId = EBearerIdWLAN;
            eventInfo.iEventType = EConnMonBearerAvailabilityChange;
            eventInfo.iData = iWlanAvailable;

            // Send EConnMonBearerAvailabilityChange event to clients
            iServer->EventQueue()->Add( eventInfo );
            LOGIT1("SERVER: EVENT -> WLAN bearer availability changed: %d (bool)", iWlanAvailable)
            }

        LOGIT("CWlanEvent::NewNetworksDetected triggered HandleAvailabilityChange()")
        iWlanSupport->EnableWlanScan();
        iServer->AvailabilityManager()->HandleAvailabilityChange();

        // Create and send EConnMonNewWLANNetworkDetected event to clients
        eventInfo.Reset();
        eventInfo.iConnectionId = EBearerIdWLAN;
        eventInfo.iEventType = EConnMonNewWLANNetworkDetected;

        iServer->EventQueue()->Add( eventInfo );
        LOGIT("SERVER: EVENT -> EConnMonNewWLANNetworkDetected")
        }

    LOGEXITFN("CWlanEvent::NewNetworksDetected()")
    }

// -----------------------------------------------------------------------------
// CWlanEvent::OldNetworksLost
// One or more networks have been lost since the last scan.
// -----------------------------------------------------------------------------
//
void CWlanEvent::OldNetworksLost()
    {
    LOGIT(".")
    LOGENTRFN("CWlanEvent::OldNetworksLost()")

    if ( iWlanSupport->EventsToClientsEnabled() )
        {
        TEventInfo eventInfo;
        if ( iWlanAvailable || !iHasAvailabilityKnowledge )
            {
            iHasAvailabilityKnowledge = ETrue;

            // Get availability from scan results
            TBool availability( EFalse );
            TRAPD( leaveCode, iWlanSupport->GetBearerAvailabilityL( availability ) );

            if ( !availability && leaveCode == KErrNone )
                {
                TUint threshold( 0 );
                iWlanAvailable = EFalse;

                // Check bearer availability threshold
                iServer->CalculateThreshold(
                        EBearerIdAll,
                        EBearerAvailabilityThreshold,
                        threshold );

                // Send the event only if threshold is set by some of the clients
                if ( threshold > 0 )
                    {
                    // Create event info
                    eventInfo.Reset();
                    eventInfo.iConnectionId = EBearerIdWLAN;
                    eventInfo.iEventType = EConnMonBearerAvailabilityChange;
                    eventInfo.iData = iWlanAvailable;

                    // Send EConnMonBearerAvailabilityChange event to clients
                    iServer->EventQueue()->Add( eventInfo );
                    LOGIT1("SERVER: EVENT -> WLAN bearer availability changed: %d (bool)", iWlanAvailable)
                    }
                }
            }

        LOGIT("CWlanEvent::OldNetworksLost triggered HandleAvailabilityChange()")
        iWlanSupport->EnableWlanScan();
        iServer->AvailabilityManager()->HandleAvailabilityChange();

        // Create and send EConnMonOldWLANNetworkLost event to clients
        eventInfo.Reset();
        eventInfo.iConnectionId = EBearerIdWLAN;
        eventInfo.iEventType = EConnMonOldWLANNetworkLost;

        iServer->EventQueue()->Add( eventInfo );
        LOGIT("SERVER: EVENT -> EConnMonOldWLANNetworkLost")
        }

    LOGEXITFN("CWlanEvent::OldNetworksLost()")
    }

// -----------------------------------------------------------------------------
// CWlanEvent::TransmitPowerChanged
// TransmitPower related to the active connection has changed.
// -----------------------------------------------------------------------------
//
void CWlanEvent::TransmitPowerChanged( TUint aPower )
    {
    LOGIT(".")
    LOGENTRFN("CWlanEvent::TransmitPowerChanged()")

    iTxPower = aPower;
    if ( iWlanSupport->EventsToClientsEnabled() )
        {
        LOGIT1("SERVER: EVENT -> WLAN transmit power changed: %d mW", aPower)
        TEventInfo eventInfo;

        // Send EConnMonTransmitPowerChange event to clients
        eventInfo.Reset();
        eventInfo.iConnectionId = EBearerIdWLAN;
        eventInfo.iEventType = EConnMonTransmitPowerChange;
        eventInfo.iData = iTxPower;

        iServer->EventQueue()->Add( eventInfo );
        }

    LOGEXITFN("CWlanEvent::TransmitPowerChanged()")
    }

// -----------------------------------------------------------------------------
// CWlanEvent::ConnectionStateChanged
// Connection status has changed.
// -----------------------------------------------------------------------------
//
void CWlanEvent::ConnectionStateChanged( TWlanConnectionMode aNewState )
    {
    LOGIT(".")
    LOGENTRFN("CWlanEvent::ConnectionStateChanged()")
    LOGIT1("SERVER: EVENT -> WLAN state changed: %d", aNewState)

    if ( aNewState == EWlanConnectionModeNotConnected )
        {
        LOGIT2("RSS class changed from %d to %d (unknown)", iRssClass, EConnMonWlanRssClassUnknown)
        iRssClass = EConnMonWlanRssClassUnknown;
        }

    LOGEXITFN("CWlanEvent::ConnectionStateChanged()")
    }

// -----------------------------------------------------------------------------
// CWlanEvent::RssChanged
// Received signal strength level has been changed.
// -----------------------------------------------------------------------------
//
void CWlanEvent::RssChanged( TWlanRssClass aRssClass, TUint aRss )
    {
    LOGIT(".")
    LOGENTRFN("CWlanEvent::RssChanged()")

    TInt previousRssClass = iRssClass;

    if ( aRssClass == EWlanRssClassWeak )
        {
        LOGIT3("Rss changed from %d to %d dBm class %d (weak)", iRss, aRss, aRssClass)
        iRssClass = EConnMonWlanRssClassWeak;
        }
    else
        {
        LOGIT3("Rss changed from %d to %d dBm class %d (normal)", iRss, aRss, aRssClass)
        iRssClass = EConnMonWlanRssClassNormal;
        }
    iRss = aRss;

    if ( ( aRssClass == EWlanRssClassWeak && previousRssClass != EConnMonWlanRssClassWeak ) ||
            ( aRssClass == EWlanRssClassNormal && previousRssClass == EConnMonWlanRssClassWeak ) )
        {
        LOGIT("Rss class change caused call to HandleAvailabilityChange()")
        iWlanSupport->EnableWlanScan(); 
        iServer->AvailabilityManager()->HandleAvailabilityChange();
        }

    LOGEXITFN("CWlanEvent::RssChanged()")
    }


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CWlanSession::NewL
// -----------------------------------------------------------------------------
//
CWlanSession* CWlanSession::NewL(
        CConnMonSession* aSessionId,
        CWlanSupport* aWlanSupport )
    {
    LOGENTRFN("CWlanSession::NewL()")
    CWlanSession* self = new( ELeave ) CWlanSession( aSessionId, aWlanSupport );
    LOGEXITFN("CWlanSession::NewL()")
    return self;
    }

// -----------------------------------------------------------------------------
// CWlanSession::CWlanSession
// -----------------------------------------------------------------------------
//
CWlanSession::CWlanSession(
        CConnMonSession* aSessionId,
        CWlanSupport* aWlanSupport )
        :
        iSessionId( aSessionId ),
        iWlanSupport( aWlanSupport ),
        iWlanScanMaxDelay( 0 ),
        iWlanScanCacheLifetime( -1 )
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CWlanSession::~CWlanSession()
    {
    LOGENTRFN("CWlanSession::~CWlanSession()")

    iSessionId = NULL;
    iWlanSupport = NULL;

    if ( iGetScanResultsAO )
        {
        delete iGetScanResultsAO;
        iGetScanResultsAO = NULL;
        }

    if ( iGetSsidScanResultsAO )
        {
        delete iGetSsidScanResultsAO;
        iGetSsidScanResultsAO = NULL;
        }

    // Cleanup message requests array
    iWlanRequestsArray.Close();

    LOGEXITFN("CWlanSession::~CWlanSession()")
    }

// -----------------------------------------------------------------------------
// CWlanSession::SetWlanScanCacheLifeTime
// -----------------------------------------------------------------------------
//
void CWlanSession::SetWlanScanCacheLifetime(
        const TInt& aWlanScanCacheLifetime )
    {
    LOGENTRFN("CWlanSession::SetWlanScanCacheLifetime()")
    if ( aWlanScanCacheLifetime < 0 )
        {
        iWlanScanCacheLifetime = -1;
        }
    else
        {
        iWlanScanCacheLifetime = aWlanScanCacheLifetime;
        }
    LOGEXITFN("CWlanSession::SetWlanScanCacheLifetime()")
    }

// -----------------------------------------------------------------------------
// CWlanSession::GetScanResults
// -----------------------------------------------------------------------------
//
void CWlanSession::GetScanResultsL( const RMessage2& aMessage )
    {
    LOGENTRFN("CWlanSession::GetScanResultsL()")

    // Create iGetScanResultsAO
    if ( !iGetScanResultsAO )
        {
        iGetScanResultsAO = CWlanGetScanResults::NewL( this );
        }

    // Set message to request message table to pend message completion
    iWlanRequestsArray.AppendL( aMessage );

    // Check if AO already active
    if ( !( iGetScanResultsAO->IsActive() ) )
        {
        // Start listening when scan results are ready
        iGetScanResultsAO->StartL();
        }

    LOGEXITFN("CWlanSession::GetScanResultsL()")
    }

// -----------------------------------------------------------------------------
// CWlanSession::GetSsidScanResultsL
// -----------------------------------------------------------------------------
//
TInt CWlanSession::GetSsidScanResultsL( const RMessage2& aMessage )
    {
    LOGENTRFN("CWlanSession::GetSsidScanResultsL()")
    TInt result( KRequestPending );

    // Create iGetSsidScanResultsAO
    if ( !iGetSsidScanResultsAO )
        {
        iGetSsidScanResultsAO = CWlanGetSsidScanResults::NewL( this );
        }

    // Check if AO already active
    if ( !( iGetSsidScanResultsAO->IsActive() ) )
        {
        // Set message to request message table to pend message completion
        // NOTE: ConnMon will allow only 1 concurrent ssid scan from a client.
        iWlanRequestsArray.AppendL( aMessage );

        // Start listening when scan results are ready
        iGetSsidScanResultsAO->StartL( iWlanSsid );
        }
    else
        {
        // Only one SSID scan can be done at a time.
        // Allowing more would cause conflicts with string attribute KWlanSsid
        result = KErrInUse;
        }

    LOGEXITFN1("CWlanSession::GetSsidScanResultsL()", result)
    return result;
    }


// -----------------------------------------------------------------------------
// CWlanSession::CancelActiveRequests
// -----------------------------------------------------------------------------
//
void CWlanSession::CancelActiveRequests( const TInt aReqType )
    {
    LOGENTRFN("CWlanSession::CancelActiveRequests()")
    TBool otherReqTypesInQueue = EFalse;
    TBool ssidScanInQueue = EFalse;
    TBool normalScanInQueue = EFalse;

    LOGIT2("Request type %d, requests waiting %d", aReqType, iWlanRequestsArray.Count())

    for ( TInt index = 0; index < iWlanRequestsArray.Count(); index++ )
        {
        TUint attribType = iWlanRequestsArray[index].Int2();
        LOGIT1("Attribute type %d", attribType)
        switch ( aReqType )
            {
            case EConnMonGetPckgAttribute:
                if ( ( attribType == KNetworkNames ) ||
                     ( attribType == KWlanNetworks ) ||
                     ( attribType == KWlanProbeRawBuffers ) )
                    {
                    normalScanInQueue = ETrue;
                    iWlanRequestsArray[index].Complete( KErrCancel );
                    iWlanRequestsArray.Remove( index );
                    index--;
                    }
                else if ( attribType == KWlanSsidNetworks )
                    {
                    ssidScanInQueue = ETrue; // Ssid scan
                    iWlanRequestsArray[index].Complete( KErrCancel );
                    iWlanRequestsArray.Remove( index );
                    index--;
                    }
                else
                    {
                    // Non-cancelled requests still remain.
                    otherReqTypesInQueue = ETrue;
                    }
                break;

            case EConnMonGetIntAttribute:
                if ( KSignalStrength == attribType )
                    {
                    normalScanInQueue = ETrue;
                    iWlanRequestsArray[index].Complete( KErrCancel );
                    iWlanRequestsArray.Remove( index );
                    index--;
                    }
                else
                    {
                    // Non-cancelled requests still remain.
                    otherReqTypesInQueue = ETrue;
                    }
                break;

            case EConnMonGetBoolAttribute:
                if ( KBearerAvailability == attribType )
                    {
                    normalScanInQueue = ETrue;
                    iWlanRequestsArray[index].Complete( KErrCancel );
                    iWlanRequestsArray.Remove( index );
                    index--;
                    }
                else
                    {
                    // Non-cancelled requests still remain.
                    otherReqTypesInQueue = ETrue;
                    }
                break;

            default:
                // Nothing about async WLAN calls in other request types
                break;
            }
        }
    LOGIT3("Flags: other req. types %d, normal scan %d, ssid scan %d",
            otherReqTypesInQueue, normalScanInQueue, ssidScanInQueue)

    if ( ssidScanInQueue )
        {
        // iGetSsidScanResultsAO only listens to a single type of request,
        // it can be safely cancelled and deleted.
        if ( iGetSsidScanResultsAO )
            {
            LOGIT("Cancel and delete iGetSsidScanResultsAO")
            iGetSsidScanResultsAO->Cancel();
            }
        }

    if ( normalScanInQueue && ( !otherReqTypesInQueue ) )
        {
        if ( iGetScanResultsAO )
            {
            LOGIT("Cancel and delete iGetScanResultsAO")
            iGetScanResultsAO->Cancel();
            }
        }

    LOGEXITFN("CWlanSession::CancelActiveRequests()")
    }

// -----------------------------------------------------------------------------
// CWlanSession::CompleteActiveRequestsWithError
// -----------------------------------------------------------------------------
//
void CWlanSession::CompleteActiveRequestsWithError(
        const TInt aErrorCode,
        const TBool aSsidScan )
    {
    //LOGENTRFN("CWlanSession::CompleteActiveRequestsWithError()")

    LOGIT3("Cancelling wlan requests (%d), ssid flag %d, error code <%d>",
            iWlanRequestsArray.Count(), aSsidScan, aErrorCode)
    for ( TInt index = 0; index < iWlanRequestsArray.Count(); index++ )
        {
        TInt type = iWlanRequestsArray[index].Int2();

        // Check if request came from SSID scan
        if ( aSsidScan )
            {
            // Complete only KWlanSsidNetworks request(s)
            if ( type == KWlanSsidNetworks )
                {
                LOGIT("Cancelled a wlan request, type: ssid")
                iWlanRequestsArray[index].Complete( aErrorCode );
                iWlanRequestsArray.Remove( index );
                index--;
                }
            }
        else // Otherwise, complete all other messages
            {
            if ( type == KBearerAvailability ||
                    type == KSignalStrength ||
                    type == KNetworkNames ||
                    type == KWlanNetworks ||
                    type == KWlanProbeRawBuffers )
                {
                LOGIT("Cancelled a wlan request, type: normal")
                iWlanRequestsArray[index].Complete( aErrorCode );
                iWlanRequestsArray.Remove( index );
                index--;
                }
            }
        }
    //LOGEXITFN("CWlanSession::CompleteActiveRequestsWithError()")
    }

// -----------------------------------------------------------------------------
// CWlanSession::CompleteActiveRequests
// -----------------------------------------------------------------------------
//
void CWlanSession::CompleteActiveRequestsL(
        const TInt aStatus,
        CWlanScanInfo* aScanInfo,
        const TBool aSsidScan )
    {
    LOGENTRFN("CWlanSession::CompleteActiveRequestsL()")

    // Check status
    if ( KErrNone != aStatus )
        {
        LOGIT1("CWlanSession::CompleteActiveRequestsL(): WLAN scanning failed, err <%d>", aStatus)
        CompleteActiveRequestsWithError( aStatus, aSsidScan );
        }
    else
        {
        LOGIT("CWlanSession::CompleteActiveRequestsL(): WLAN scanning successful")

        // Complete messages according to message type
        for ( TUint index = 0; index < iWlanRequestsArray.Count(); index++ )
            {
            TUint type = iWlanRequestsArray[index].Int2();

            // Check if request came from SSID scan
            if ( aSsidScan )
                {
                // Complete only KWlanSsidNetworks message
                if ( type == KWlanSsidNetworks )
                    {
                    LOGIT("CWlanSession::CompleteActiveRequestsL(): complete KWlanSsidNetworks")
                    CompleteGetWlanSsidNetworksRequestL( aScanInfo, index );
                    iWlanRequestsArray.Remove( index );
                    index--;
                    }
                }
            else // Complete all other messages
                {
                if ( type == KBearerAvailability )
                    {
                    LOGIT("CWlanSession::CompleteActiveRequestsL(): complete KBearerAvailability")
                    CompleteGetBearerAvailabilityRequest( aScanInfo, index );
                    iWlanRequestsArray.Remove( index );
                    index--;
                    }

                else if ( type == KSignalStrength )
                    {
                    LOGIT("CWlanSession::CompleteActiveRequestsL(): complete KSignalStrength")
                    CompleteGetSignalStrengthRequest( aScanInfo, index );
                    iWlanRequestsArray.Remove( index );
                    index--;
                    }

                else if ( type == KNetworkNames )
                    {
                    LOGIT("CWlanSession::CompleteActiveRequestsL(): complete KNetworkNames")
                    CompleteGetNetworkNamesRequestL( aScanInfo, index );
                    iWlanRequestsArray.Remove( index );
                    index--;
                    }

                else if ( type == KWlanNetworks )
                    {
                    LOGIT("CWlanSession::CompleteActiveRequestsL(): complete KWlanNetworks")
                    CompleteGetWlanNetworksRequestL( aScanInfo, index );
                    iWlanRequestsArray.Remove( index );
                    index--;
                    }

                else if ( type == KWlanProbeRawBuffers )
                    {
                    LOGIT("CWlanSession::CompleteActiveRequestsL(): complete KWlanProbeRawBuffers")
                    CompleteGetWlanProbeRawBuffersRequestL( aScanInfo, index );
                    iWlanRequestsArray.Remove( index );
                    index--;
                    }
                }
            }
        }

    LOGEXITFN("CWlanSession::CompleteActiveRequestsL()")
    }



// -----------------------------------------------------------------------------
// CWlanSession::CompleteGetBearerAvailabilityRequest
// Completes the asynchronous client request: GetBoolAttribute( KBearerAvailability )
// -----------------------------------------------------------------------------
//
void CWlanSession::CompleteGetBearerAvailabilityRequest(
        CWlanScanInfo* aScanInfo,
        const TUint aIndex )
    {
    LOGENTRFN("CWlanSession::CompleteGetBearerAvailabilityRequest()")
    TBool availability = EFalse;

    // Set pointer to first IAP, just in case
    aScanInfo->First();

    // Check scan results
    if ( aScanInfo->IsDone() )
        {
        // No WLAN networks available
        availability = EFalse;
        }
    else
        {
        // Found WLAN networks
        availability = ETrue;
        }

    LOGIT1("CompleteGetBearerAvailabilityRequest: availability %d", availability)

    // Write result back to client's address space, and complete message
    TPtrC8 d( reinterpret_cast<TUint8*>( &availability ), sizeof( availability ) );
    TInt res = iWlanRequestsArray[aIndex].Write( KAttributeParameterIndex, d );
    if ( KErrNone != res )
        {
        LOGIT1("ERROR writing data to client <%d>", res)
        iWlanRequestsArray[aIndex].Complete( KErrBadDescriptor );
        }
    else
        {
        iWlanRequestsArray[aIndex].Complete( KErrNone );
        }

    LOGEXITFN("CWlanSession::CompleteGetBearerAvailabilityRequest()")
    }


// -----------------------------------------------------------------------------
// CWlanSession::CompleteGetSignalStrengthRequest
// Completes the asynchronous client request: GetIntAttribute ( KSignalStrength )
// -----------------------------------------------------------------------------
//
void CWlanSession::CompleteGetSignalStrengthRequest(
        CWlanScanInfo* aScanInfo,
        const TUint aIndex )
    {
    LOGENTRFN("CWlanSession::CompleteGetSignalStrengthRequest()")
    TInt signalStrength( KMaxTInt ); // Means infinitely weak

    // Iterate through scan results.
    for ( aScanInfo->First(); !aScanInfo->IsDone(); aScanInfo->Next() )
        {
        // RXLevel() returns signal strength in unsigned dBm (-60dBm == 60dBm)
        if ( static_cast<TInt>( aScanInfo->RXLevel() ) < signalStrength )
            {
            signalStrength = static_cast<TInt>( aScanInfo->RXLevel() );
            }
        }

    LOGIT1("CWlanSession::CompleteGetSignalStrengthRequest(): signalStrength %d", signalStrength)

    // Write result back to client's address space, and complete message
    TPtrC8 d( reinterpret_cast<TUint8*>( &signalStrength ), sizeof( signalStrength ) );
    TInt res = iWlanRequestsArray[aIndex].Write( KAttributeParameterIndex, d );
    if ( res != KErrNone )
        {
        LOGIT1("ERROR writing data to client <%d>", res)
        iWlanRequestsArray[aIndex].Complete( KErrBadDescriptor );
        }
    else
        {
        iWlanRequestsArray[aIndex].Complete( KErrNone );
        }

    LOGEXITFN("CWlanSession::CompleteGetSignalStrengthRequest()")
    }

// -----------------------------------------------------------------------------
// CWlanSession::CompleteGetNetworkNamesRequest
// Completes the asynchronous client request: GetPckgAttribute( KNetworkNames )
// -----------------------------------------------------------------------------
//
void CWlanSession::CompleteGetNetworkNamesRequestL(
        CWlanScanInfo* aScanInfo,
        const TUint aIndex )
    {
    LOGENTRFN("CWlanSession::CompleteGetNetworkNamesRequestL()")
    TConnMonNetworkNames netInfo;

    // Parse network names
    iWlanSupport->ParseNetworkNamesL( aScanInfo, netInfo );

    // Write result back to client's address space, and complete message
    TConnMonNetworkNamesBuf namesBuf( netInfo );
    TInt res = iWlanRequestsArray[aIndex].Write( KAttributeParameterIndex, namesBuf );
    if ( KErrNone != res )
        {
        LOGIT1("ERROR writing data to client <%d>", res)
        iWlanRequestsArray[aIndex].Complete( KErrBadDescriptor );
        }
    else
        {
        iWlanRequestsArray[aIndex].Complete( KErrNone );
        }

    LOGEXITFN("CWlanSession::CompleteGetNetworkNamesRequestL()")
    }

// -----------------------------------------------------------------------------
// CWlanSession::CompleteGetWlanNetworksRequest
// Completes the asynchronous client request: GetPckgAttribute( KWlanNetworks )
// -----------------------------------------------------------------------------
//
void CWlanSession::CompleteGetWlanNetworksRequestL(
        CWlanScanInfo* aScanInfo,
        const TUint aIndex )
    {
    LOGENTRFN("CWlanSession::CompleteGetWlanNetworksRequestL()")

    TInt maxSize( iWlanRequestsArray[aIndex].GetDesMaxLength( KAttributeParameterIndex ) );
    if ( maxSize < 2 )
        {
        LOGIT1("ERROR with message descriptor <%d>", maxSize)
        iWlanRequestsArray[aIndex].Complete( KErrBadDescriptor );
        }
    else
        {
        RConnMonWlanNetworksPtrArray wlanNetworks;
        ConnMonCleanupResetAndDestroyPushL( wlanNetworks );

        // Parse WLAN networks
        iWlanSupport->ParseWlanNetworksL( aScanInfo, wlanNetworks );

        CConnMonWlanNetworksPtrArrayPckg arr( wlanNetworks, maxSize ); // maxSize >= 2
        CleanupStack::PopAndDestroy( &wlanNetworks );

        if ( !arr.Buf() )
            {
            LOGIT("ERROR no memory for CConnMonWlanNetworksPtrArrayPckg")
            iWlanRequestsArray[aIndex].Complete( KErrNoMemory );
            }
        else
            {
            // Write result back to client's address space, and complete message
            TInt res = iWlanRequestsArray[aIndex].Write( KAttributeParameterIndex, *arr.Buf() );
            if ( res != KErrNone )
                {
                LOGIT1("ERROR writing data to client <%d>", res)
                iWlanRequestsArray[aIndex].Complete( KErrBadDescriptor );
                }
            else
                {
                iWlanRequestsArray[aIndex].Complete( KErrNone );
                }
            }
        }

    LOGEXITFN("CWlanSession::CompleteGetWlanNetworksRequestL()")
    }

// -----------------------------------------------------------------------------
// CWlanSession::CompleteGetWlanSsidNetworksRequest
// Completes the asynchronous client request: GetPckgAttribute( KWlanSsidNetworks )
// -----------------------------------------------------------------------------
//
void CWlanSession::CompleteGetWlanSsidNetworksRequestL(
        CWlanScanInfo* aScanInfo,
        const TUint aIndex )
    {
    LOGENTRFN("CWlanSession::CompleteGetWlanSsidNetworksRequestL()")

    TInt maxSize( iWlanRequestsArray[aIndex].GetDesMaxLength( KAttributeParameterIndex ) );
    if ( maxSize < 2 )
        {
        LOGIT1("ERROR with message descriptor <%d>", maxSize)
        iWlanRequestsArray[aIndex].Complete( KErrBadDescriptor );
        }
    else
        {
        RConnMonWlanNetworksPtrArray wlanNetworks;
        ConnMonCleanupResetAndDestroyPushL( wlanNetworks );

        // Parse WLAN networks
        iWlanSupport->ParseWlanNetworksL( aScanInfo, wlanNetworks );

        CConnMonWlanNetworksPtrArrayPckg arr( wlanNetworks, maxSize ); // maxSize >= 2
        CleanupStack::PopAndDestroy( &wlanNetworks );

        if ( !arr.Buf() )
            {
            LOGIT("ERROR no memory for CConnMonWlanNetworksPtrArrayPckg")
            iWlanRequestsArray[aIndex].Complete( KErrNoMemory );
            }
        else
            {
            // Write result back to client's address space, and complete message
            TInt res = iWlanRequestsArray[aIndex].Write( KAttributeParameterIndex, *arr.Buf() );
            if ( res != KErrNone )
                {
                LOGIT1("ERROR writing data to client <%d>", res)
                iWlanRequestsArray[aIndex].Complete( KErrBadDescriptor );
                }
            else
                {
                iWlanRequestsArray[aIndex].Complete( KErrNone );
                }
            }
        }

    LOGEXITFN("CWlanSession::CompleteGetWlanSsidNetworksRequestL()")
    }

// -----------------------------------------------------------------------------
// CWlanSession::CompleteGetWlanProbeRawBuffersRequest
// Completes the asynchronous client request: GetPckgAttribute( KWlanProbeRawBuffers )
// -----------------------------------------------------------------------------
//
void CWlanSession::CompleteGetWlanProbeRawBuffersRequestL(
        CWlanScanInfo* aScanInfo,
        const TUint aIndex )
    {
    LOGENTRFN("CWlanSession::CompleteGetWlanProbeRawBuffersRequestL()")

    TInt maxSize( iWlanRequestsArray[aIndex].GetDesMaxLength( KAttributeParameterIndex ) );
    if ( maxSize < 2 )
        {
        LOGIT1("ERROR with message descriptor <%d>", maxSize)
        iWlanRequestsArray[aIndex].Complete( KErrBadDescriptor );
        }
    else
        {
        RConnMonWlanProbeRawBuffersPtrArray wlanProbeRawBuffers;
        ConnMonCleanupResetAndDestroyPushL( wlanProbeRawBuffers );

        // Parse raw buffers networks
        iWlanSupport->ParseWlanProbeRawBuffersL( aScanInfo, wlanProbeRawBuffers );

        CConnMonWlanProbeRawBuffersPckg arr( wlanProbeRawBuffers, maxSize ); // maxSize >= 2
        CleanupStack::PopAndDestroy( &wlanProbeRawBuffers );

        if ( !arr.Buf() )
            {
            LOGIT("ERROR no memory for CConnMonWlanProbeRawBuffersPckg")
            iWlanRequestsArray[aIndex].Complete( KErrNoMemory );
            }
        else
            {
            // Write result back to client's address space, and complete message
            TInt res = iWlanRequestsArray[aIndex].Write( KAttributeParameterIndex, *arr.Buf() );
            if ( KErrNone != res )
                {
                LOGIT1("ERROR writing data to client <%d>", res)
                iWlanRequestsArray[aIndex].Complete( KErrBadDescriptor );
                }
            else
                {
                iWlanRequestsArray[aIndex].Complete( KErrNone );
                }
            }
        }

    LOGEXITFN("CWlanSession::CompleteGetWlanProbeRawBuffersRequestL()")
    }


// =========================== MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWlanGetScanResults::NewL
// -----------------------------------------------------------------------------
//
CWlanGetScanResults* CWlanGetScanResults::NewL( CWlanSession* aWlanSession )
    {
    LOGENTRFN("CWlanGetScanResults::NewL()")
    CWlanGetScanResults* self = new( ELeave ) CWlanGetScanResults( aWlanSession );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    LOGEXITFN("CWlanGetScanResults::NewL()")
    return self;
    }

// -----------------------------------------------------------------------------
// CWlanGetScanResults::CWlanGetScanResults
// -----------------------------------------------------------------------------
//
CWlanGetScanResults::CWlanGetScanResults(
        CWlanSession* aWlanSession )
        :
        CActive( EConnMonPriorityNormal ),
        iWlanSession( aWlanSession )
    {
    }

// -----------------------------------------------------------------------------
// CWlanGetScanResults::ConstructL
// -----------------------------------------------------------------------------
//
void CWlanGetScanResults::ConstructL()
    {
    iWlanMgmt = CWlanMgmtClient::NewL();
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CWlanGetScanResults::~CWlanGetScanResults
// -----------------------------------------------------------------------------
//
CWlanGetScanResults::~CWlanGetScanResults()
    {
    LOGENTRFN("CWlanGetScanResults::~CWlanGetScanResults()")

    Cancel();

    if ( iWlanMgmt )
        {
        delete iWlanMgmt;
        iWlanMgmt = NULL;
        }

    if ( iScanInfo )
        {
        delete iScanInfo;
        iScanInfo = NULL;
        }

    LOGEXITFN("CWlanGetScanResults::~CWlanGetScanResults()")
    }

// -----------------------------------------------------------------------------
// CWlanGetScanResults::StartL
// -----------------------------------------------------------------------------
//
void CWlanGetScanResults::StartL()
    {
    LOGENTRFN("CWlanGetScanResults::StartL()")

    if ( IsActive() )
        {
        LOGIT("CWlanGetScanResults::StartL(): Already active, calling Cancel()")
        Cancel();
        }

    // Create scaninfo object for scan results
    if ( !iScanInfo )
        {
        iScanInfo = CWlanScanInfo::NewL();
        }

    // Get scan results from WLAN engine
    TInt oldWlanScanCacheLifetime( iWlanSession->GetWlanScanCacheLifetime() );
    TInt newWlanScanCacheLifetime( oldWlanScanCacheLifetime );

    TUint oldWlanScanMaxDelay( iWlanSession->GetWlanScanMaxDelay() );
    TUint newWlanScanMaxDelay( oldWlanScanMaxDelay );

    TBuf8<CConnMonWlanNetwork::KMaxNameLength> emptySsid( 0 );
    LOGIT2("wlanScanCacheLifetime %d, wlanScanMaxDelay %d",
            newWlanScanCacheLifetime, newWlanScanMaxDelay)
    iWlanMgmt->GetScanResults(
            newWlanScanCacheLifetime,
            newWlanScanMaxDelay,
            emptySsid,
            iStatus,
            *iScanInfo );

    // Check if wlan engine has changed the timing parameters
    if ( oldWlanScanCacheLifetime != newWlanScanCacheLifetime )
        {
        LOGIT2("CWlanGetScanResults::StartL(): wlanScanCacheLifetime changed from %d to %d",
                oldWlanScanCacheLifetime, newWlanScanCacheLifetime)
        iWlanSession->SetWlanScanCacheLifetime( newWlanScanCacheLifetime );
        }
    if ( oldWlanScanMaxDelay != newWlanScanMaxDelay )
        {
        LOGIT2("CWlanGetScanResults::StartL(): wlanScanMaxDelay changed from %d to %d",
                oldWlanScanMaxDelay, newWlanScanMaxDelay)
        iWlanSession->SetWlanScanMaxDelay( newWlanScanMaxDelay );
        }

    // Start listening when scan results are ready
    SetActive();

    LOGIT3("wlanScanCacheLifetime %d, wlanScanMaxDelay %d, status <%d>",
            newWlanScanCacheLifetime, newWlanScanMaxDelay, iStatus.Int())
    LOGEXITFN("CWlanGetScanResults::StartL()")
    }

// -----------------------------------------------------------------------------
// CWlanGetScanResults::DoCancel
// -----------------------------------------------------------------------------
//
void CWlanGetScanResults::DoCancel()
    {
    LOGIT("CWlanGetScanResults::DoCancel()")

    if ( IsActive() )
        {
        // Cancel async request from WLAN engine
        iWlanMgmt->CancelGetScanResults();
        }
    }

// -----------------------------------------------------------------------------
// CWlanGetScanResults::RunL
// -----------------------------------------------------------------------------
//
void CWlanGetScanResults::RunL()
    {
    LOGIT(".")
    LOGIT1("CWlanGetScanResults::RunL(): sessionId %d", iWlanSession->GetSessionId())

    // Complete all session requests
    iWlanSession->CompleteActiveRequestsL( iStatus.Int(), iScanInfo, EFalse );

    // Delete scaninfo object to save memory
    if ( iScanInfo )
        {
        delete iScanInfo;
        iScanInfo = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CWlanGetScanResults::RunError
// -----------------------------------------------------------------------------
//
TInt CWlanGetScanResults::RunError( TInt aLeaveCode )
    {
    //LOGENTRFN("CWlanGetScanResults::RunError()")

    LOGIT1("CWlanGetScanResults::RunError() <%d>", aLeaveCode)
    iWlanSession->CompleteActiveRequestsWithError( aLeaveCode, EFalse );

    //LOGEXITFN("CWlanGetScanResults::RunError()")
    return KErrNone;
    }


// =========================== MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWlanGetSsidScanResults::NewL
// -----------------------------------------------------------------------------
//
CWlanGetSsidScanResults* CWlanGetSsidScanResults::NewL(
        CWlanSession* aWlanSession )
    {
    LOGENTRFN("CWlanGetSsidScanResults::NewL()")
    CWlanGetSsidScanResults* self = new( ELeave ) CWlanGetSsidScanResults( aWlanSession );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    LOGEXITFN("CWlanGetSsidScanResults::NewL()")
    return self;
    }

// -----------------------------------------------------------------------------
// CWlanGetSsidScanResults::CWlanGetSsidScanResults
// -----------------------------------------------------------------------------
//
CWlanGetSsidScanResults::CWlanGetSsidScanResults(
        CWlanSession* aWlanSession )
        :
        CActive( EConnMonPriorityNormal ),
        iWlanSession( aWlanSession )
    {
    }

// -----------------------------------------------------------------------------
// CWlanGetSsidScanResults::ConstructL
// -----------------------------------------------------------------------------
//
void CWlanGetSsidScanResults::ConstructL()
    {
    iWlanMgmt = CWlanMgmtClient::NewL();
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CWlanGetSsidScanResults::~CWlanGetSsidScanResults
// -----------------------------------------------------------------------------
//
CWlanGetSsidScanResults::~CWlanGetSsidScanResults()
    {
    LOGENTRFN("CWlanGetSsidScanResults::~CWlanGetSsidScanResults()")

    Cancel();

    if ( iWlanMgmt )
        {
        delete iWlanMgmt;
        iWlanMgmt = NULL;
        }

    if ( iScanInfo )
        {
        delete iScanInfo;
        iScanInfo = NULL;
        }

    LOGEXITFN("CWlanGetSsidScanResults::~CWlanGetSsidScanResults()")
    }

// -----------------------------------------------------------------------------
// CWlanGetSsidScanResults::StartL
// -----------------------------------------------------------------------------
//
void CWlanGetSsidScanResults::StartL(
        TBuf8<CConnMonWlanNetwork::KMaxNameLength>& aWlanSsid )
    {
    LOGENTRFN("CWlanGetSsidScanResults::StartL()")

    if ( IsActive() )
        {
        LOGIT("StartL(): Already active, calling Cancel()")
        Cancel();
        }

    // Create scaninfo object for scan results
    if ( !iScanInfo )
        {
        iScanInfo = CWlanScanInfo::NewL();
        }

    // Get scan results from WLAN engine
    TInt oldWlanScanCacheLifetime( iWlanSession->GetWlanScanCacheLifetime() );
    TInt newWlanScanCacheLifetime( oldWlanScanCacheLifetime );

    TUint oldWlanScanMaxDelay( iWlanSession->GetWlanScanMaxDelay() );
    TUint newWlanScanMaxDelay( oldWlanScanMaxDelay );

    LOGIT2("wlanScanCacheLifetime %d, wlanScanMaxDelay %d", newWlanScanCacheLifetime, newWlanScanMaxDelay)
    iWlanMgmt->GetScanResults(
            newWlanScanCacheLifetime,
            newWlanScanMaxDelay,
            aWlanSsid,
            iStatus,
            *iScanInfo );

    // Check if wlan engine has changed the timing parameters
    if ( oldWlanScanCacheLifetime != newWlanScanCacheLifetime )
        {
        LOGIT2("wlanScanCacheLifetime changed from %d to %d", oldWlanScanCacheLifetime, newWlanScanCacheLifetime)
        iWlanSession->SetWlanScanCacheLifetime( newWlanScanCacheLifetime );
        }
    if ( oldWlanScanMaxDelay != newWlanScanMaxDelay )
        {
        LOGIT2("wlanScanMaxDelay changed from %d to %d", oldWlanScanMaxDelay, newWlanScanMaxDelay)
        iWlanSession->SetWlanScanMaxDelay( newWlanScanMaxDelay );
        }

    // Start listening when scan results are ready
    SetActive();

    LOGEXITFN("CWlanGetSsidScanResults::StartL()")
    }

// -----------------------------------------------------------------------------
// CWlanGetSsidScanResults::DoCancel
// -----------------------------------------------------------------------------
//
void CWlanGetSsidScanResults::DoCancel()
    {
    LOGIT("CWlanGetSsidScanResults::DoCancel()")

    if ( IsActive() )
        {
        // Cancel async request from WLAN engine
        iWlanMgmt->CancelGetScanResults();
        }
    }

// -----------------------------------------------------------------------------
// CWlanGetSsidScanResults::RunL
// -----------------------------------------------------------------------------
//
void CWlanGetSsidScanResults::RunL()
    {
    LOGIT(".")
    LOGIT1("CWlanGetSsidScanResults::RunL(): sessionId %d", iWlanSession->GetSessionId())

    // Complete all session requests
    iWlanSession->CompleteActiveRequestsL( iStatus.Int(), iScanInfo, ETrue );

    // Delete scaninfo object to save memory
    if ( iScanInfo )
        {
        delete iScanInfo;
        iScanInfo = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CWlanGetSsidScanResults::RunError
// -----------------------------------------------------------------------------
//
TInt CWlanGetSsidScanResults::RunError( TInt aLeaveCode )
    {
    //LOGENTRFN("CWlanGetSsidScanResults::RunError()")

    LOGIT1("CWlanGetSsidScanResults::RunError() <%d>", aLeaveCode)
    iWlanSession->CompleteActiveRequestsWithError( aLeaveCode, ETrue );

    //LOGEXITFN("CWlanGetSsidScanResults::RunError()")
    return KErrNone;
    }


// =========================== MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
//  Constructor
// ---------------------------------------------------------------------------
//
CConnMonWlanProbeRawBuffer::CConnMonWlanProbeRawBuffer()
    {
    }

// ---------------------------------------------------------------------------
//  Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CConnMonWlanProbeRawBuffer* CConnMonWlanProbeRawBuffer::NewL()
    {
    CConnMonWlanProbeRawBuffer* self = new( ELeave ) CConnMonWlanProbeRawBuffer();
    return self;
    }

// ---------------------------------------------------------------------------
//  Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CConnMonWlanProbeRawBuffer* CConnMonWlanProbeRawBuffer::NewL(
        const HBufC8* aRawBuffer )
    {
    CConnMonWlanProbeRawBuffer* self = new( ELeave ) CConnMonWlanProbeRawBuffer();

    CleanupStack::PushL( self );
    self->ConstructL( aRawBuffer );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
//  Second-phase constructor
// ---------------------------------------------------------------------------
//
void CConnMonWlanProbeRawBuffer::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
//  Second-phase constructor
// ---------------------------------------------------------------------------
//
void CConnMonWlanProbeRawBuffer::ConstructL( const HBufC8* aRawBuffer )
    {
    if ( aRawBuffer && aRawBuffer->Length() > 0 )
        {
        iRawBuffer = HBufC8::NewL( aRawBuffer->Length() );
        iRawBuffer->Des().Copy( *aRawBuffer );
        }
    }

// ---------------------------------------------------------------------------
//  Copy Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CConnMonWlanProbeRawBuffer* CConnMonWlanProbeRawBuffer::NewL(
        CConnMonWlanProbeRawBuffer& aConnMonWlanProbeRawBuffer )
    {
    CConnMonWlanProbeRawBuffer* self = new( ELeave ) CConnMonWlanProbeRawBuffer();

    CleanupStack::PushL( self );
    self->ConstructL( aConnMonWlanProbeRawBuffer.iRawBuffer );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
//  Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CConnMonWlanProbeRawBuffer::~CConnMonWlanProbeRawBuffer()
    {
    delete iRawBuffer;
    }

// ---------------------------------------------------------------------------
//  = operator
// ---------------------------------------------------------------------------
//
EXPORT_C CConnMonWlanProbeRawBuffer& CConnMonWlanProbeRawBuffer::operator=(
        CConnMonWlanProbeRawBuffer& aConnMonWlanProbeRawBuffer )
    {
    TInt err( KErrNone );

    if ( aConnMonWlanProbeRawBuffer.iRawBuffer )
        {
        delete iRawBuffer;
        iRawBuffer = NULL;
        TRAP( err, iRawBuffer = HBufC8::NewL( aConnMonWlanProbeRawBuffer.iRawBuffer->Size() ) );
        if ( err )
            {
            LOGIT1("Raw buffer handling error <%d>", err)
            }
        else
            {
            iRawBuffer->Des().Copy( *aConnMonWlanProbeRawBuffer.iRawBuffer );
            }
        }

    return *this;
    }


// ---------------------------------------------------------------------------
// Packages object to HBufC8 descriptor
// ---------------------------------------------------------------------------
//
EXPORT_C const HBufC8* CConnMonWlanProbeRawBuffer::ToBuf() const
    {
    //LOGENTRFN("CConnMonWlanProbeRawBuffer::ToBuf()")

    // Allocate buffer to all data
    HBufC8* buf = HBufC8::New( sizeof( CConnMonWlanProbeRawBuffer ) + iRawBuffer->Length() );
    if ( !buf )
        {
        LOGEXITFN("ToBuf: out of memory error")
        return NULL;
        }

    TPtr8 ptr( buf->Des() );

    // Add CConnMonWlanProbeRawBuffer object
    ptr.Copy( (TUint8*)this, (TInt)sizeof( CConnMonWlanProbeRawBuffer ) );

    // If rawdata exists add it too
    if ( iRawBuffer->Length() > 0 )
        {
        ptr.Append( iRawBuffer->Des() );
        }

    //LOGEXITFN("CConnMonWlanProbeRawBuffer::ToBuf()")
    return buf;
    }


// ---------------------------------------------------------------------------
// Unpackages CConnMonWlanProbeRawBuffer from TPtrC descriptor
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CConnMonWlanProbeRawBuffer::FromPtrC(
        const TPtrC8& aPtrC,
        CConnMonWlanProbeRawBuffer* aConnMonWlanProbeRawBuffer )
    {
    //LOGENTRFN("CConnMonWlanProbeRawBuffer::FromPtrC()")
    TInt err( KErrNone );

    // Copy CConnMonWlanProbeRawBuffer object data from the beginning of aPtrC
    memcpy( aConnMonWlanProbeRawBuffer, aPtrC.Ptr(), sizeof( CConnMonWlanProbeRawBuffer ) );

    // Reset rawdata pointer
    aConnMonWlanProbeRawBuffer->iRawBuffer = NULL;

    // If rawdata data exists, allocate memory and read it (it exists just after
    // CConnMonWlanProbeRawBuffer object data) from aPtrC
    if ( aPtrC.Length() > sizeof( CConnMonWlanProbeRawBuffer ) )
        {
        TInt rawDataLen = aPtrC.Length() - sizeof( CConnMonWlanProbeRawBuffer );

        aConnMonWlanProbeRawBuffer->iRawBuffer = HBufC8::New( rawDataLen );
        if ( !aConnMonWlanProbeRawBuffer->iRawBuffer )
            {
            err = KErrNoMemory;
            LOGIT("FromPtrC: out of memory error")
            }
        else
            {
            aConnMonWlanProbeRawBuffer->iRawBuffer->Des().Copy(
                    aPtrC.Mid( sizeof( CConnMonWlanProbeRawBuffer ), rawDataLen ) );
            }
        }

    //LOGEXITFN1("CConnMonWlanProbeRawBuffer::FromPtrC()", err)
    return err;
    }

// ---------------------------------------------------------------------------
// Getter
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* CConnMonWlanProbeRawBuffer::RawBuffer() const
    {
    return iRawBuffer;
    }





// =========================== MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CConnMonWlanProbeRawBuffersPckg::CConnMonWlanProbeRawBuffersPckg(
        const RConnMonWlanProbeRawBuffersPtrArray& aRef,
        TUint aBufLen )
    {
    LOGENTRFN("CConnMonWlanProbeRawBuffersPckg::CConnMonWlanProbeRawBuffersPckg()")

    const TUint KItemLengthFieldSize = 2; // 2 bytes (item lenth can be larger than 255)

    // First 2 bytes in buffer will contain the total number of objects and the number of transfered objects.
    TUint currentPosition( 2 );
    TInt totalItemCount = aRef.Count();

    LOGIT2("CConnMonWlanProbeRawBuffersPckg: buffer size %d, item count %d", aBufLen, totalItemCount)

    // Create a temporary pointer array table for buffer objects
    RPointerArray<HBufC8> items;

    for ( TUint i = 0; i < totalItemCount; ++i )
        {
        const HBufC8* item( aRef[i]->ToBuf() );
        if ( item )
            {
            // Check if there is room in buffer for the next item
            if ( ( currentPosition + KItemLengthFieldSize + item->Length() ) > aBufLen )
                {
                LOGIT4("Buffer full, current position %d, item length %d, buffer size %d, i %d",
                        currentPosition, item->Length(), aBufLen, i)
                delete item;
                item = NULL;
                break;
                }
            TInt err = items.Append( item );
            if ( !err )
                {
                currentPosition += KItemLengthFieldSize + item->Length();
                }
            else
                {
                LOGIT1("ERROR: append failed <%d>", err)
                delete item;
                item = NULL;
                }
            }
        }

    // Check that given buffer length (aBufLen) is not smaller than one
    // item (CConnMonWlanProbeRawBuffer) + aRef count + items count + item length
    if ( items.Count() == 0 && totalItemCount > 0 )
        {
        aBufLen = 2; // aRef.Count(), items.Count()
        }

    // Allocate memory for buffer
    iBuf = HBufC8::New( aBufLen );
    if ( !iBuf )
        {
        LOGIT("CConnMonWlanProbeRawBuffersPckg: out of memory error")
        }
    else
        {
        // Add total and transferred counts to buffer
        iBuf->Des().Append( totalItemCount ); // Total amount of objects
        iBuf->Des().Append( items.Count() ); // Amount of objects in buffer

        // Add item length and item data to buffer
        for ( TUint i = 0; i < items.Count(); ++i )
            {
            // Add item length by splitting 16bit number
            TUint8 tempLength1 = (TUint8)( items[i]->Length() & 0x00FF );
            TUint8 tempLength2 = (TUint8)( ( items[i]->Length() & 0xFF00 ) >> KBitsInByte );
            iBuf->Des().Append( tempLength1 );
            iBuf->Des().Append( tempLength2 );

            iBuf->Des().Append( *items[i] );
            }
        LOGIT1("Used buffer %d", iBuf->Length())
        }
    items.ResetAndDestroy(); // Clear array

    LOGEXITFN("CConnMonWlanProbeRawBuffersPckg::CConnMonWlanProbeRawBuffersPckg()")
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CConnMonWlanProbeRawBuffersPckg::CConnMonWlanProbeRawBuffersPckg(
        TUint aBufSize )
    {
    iBuf = HBufC8::New( aBufSize );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CConnMonWlanProbeRawBuffersPckg::~CConnMonWlanProbeRawBuffersPckg()
    {
    delete iBuf;
    }

// ---------------------------------------------------------------------------
// Unpacking
// ---------------------------------------------------------------------------
//
EXPORT_C void CConnMonWlanProbeRawBuffersPckg::UnpackToL(
        RConnMonWlanProbeRawBuffersPtrArray& aRef ) const
    {
    LOGENTRFN("CConnMonWlanProbeRawBuffersPckg::UnpackToL()")

    if ( !iBuf || iBuf->Length() < 2 )
        {
        User::Leave( KErrBadDescriptor );
        }

    // Parse total and transferred data counts
    TUint index( 0 );
    TUint total( (*iBuf)[index++] ); // Total amount of objects
    TUint count( (*iBuf)[index++] ); // Amount of objects in buffer
    LOGIT2("UnpackToL: total %d, in buffer %d", total, count)

    // Parse CConnMonWlanProbeRawBuffer objects from buffer
    for ( TUint i = 0; i < count; ++i )
        {
        // Parse object length (stored in 2 bytes)
        TUint len( 0 );
        TUint8 len1( (*iBuf)[index++] ); // 1st byte of length
        TUint8 len2( (*iBuf)[index++] ); // 2nd byte of length
        len = (TUint)len2;
        len = len<<8 & 0xFF00;
        len = len | (TUint)len1;

        // Parse object data
        TPtrC8 ptr( iBuf->Mid( index, len ) );
        index += len;

        CConnMonWlanProbeRawBuffer* net = CConnMonWlanProbeRawBuffer::NewL();
        TInt err = CConnMonWlanProbeRawBuffer::FromPtrC( ptr, net );
        if ( err != KErrNone )
            {
            LOGIT2("UnpackToL: error <%d> reading data, stopping. i %d", err, i)
            break;
            }

        CleanupStack::PushL( net );
        aRef.AppendL( net );
        CleanupStack::Pop( net );
        }

    LOGEXITFN("CConnMonWlanProbeRawBuffersPckg::UnpackToL()")
    }

// ---------------------------------------------------------------------------
// Getter
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* CConnMonWlanProbeRawBuffersPckg::Buf() const
    {
    return iBuf;
    }

// ---------------------------------------------------------------------------
// Returns all available networks
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CConnMonWlanProbeRawBuffersPckg::Total() const
    {
    TUint ret( 0 );
    if ( iBuf && iBuf->Length() > 0 )
        {
        ret = (*iBuf)[0]; // Total amount of objects
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Returns transfered networks
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CConnMonWlanProbeRawBuffersPckg::Count() const
    {
    TUint ret( 0 );
    if ( iBuf && iBuf->Length() > 1 )
        {
        ret = (*iBuf)[1]; // Amount of objects in buffer
        }
    return ret;
    }

// End-of-file
