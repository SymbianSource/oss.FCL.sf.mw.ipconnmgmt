/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: MPM CommsDat access functions
*
*/

/**
@file mpmcommsdataccess.cpp
Mobility Policy Manager CommsDat access functions.
*/

// INCLUDE FILES
#include <e32base.h>
#include <commdb.h>
#include <metadatabase.h>
#include <commsdattypeinfov1_1.h>
#include <commsdattypesv1_1.h>
#include <datamobilitycommsdattypes.h>
#include <cmpluginbaseeng.h>
#include <WlanCdbCols.h>
#include <cmmanagerdef.h>
#include <cmconnectionmethoddef.h>
#include <wlancontainer.h>
#include <utf.h>
#include <cmpluginwlandef.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdattypeinfov1_1_internal.h>
#include <commsdattypesv1_1_partner.h>
#endif

#include <in_sock.h>
#include <cmgenconnsettings.h>
#include <cmdestination.h>

#include "mpmcommsdataccess.h"
#include "mpmserversession.h"
#include "mpmconnmonevents.h"
#include "mpmiapselection.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::NewL
// -----------------------------------------------------------------------------
//
CMPMCommsDatAccess* CMPMCommsDatAccess::NewL()
    {
    CMPMCommsDatAccess* self = new ( ELeave ) CMPMCommsDatAccess();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::CMPMCommsDatAccess
// -----------------------------------------------------------------------------
//
CMPMCommsDatAccess::CMPMCommsDatAccess()
    {
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::ConstructL
// -----------------------------------------------------------------------------
//
void CMPMCommsDatAccess::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::~CMPMCommsDatAccess
// -----------------------------------------------------------------------------
//
CMPMCommsDatAccess::~CMPMCommsDatAccess()
    {
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::FindAllSnapsL
// -----------------------------------------------------------------------------
//
void CMPMCommsDatAccess::FindAllSnapsL( RArray<TUint>& aSnapIds )
    {
    MPMLOGSTRING( "CMPMCommsDatAccess::FindAllSnapsL " )

    CMDBSession* db = CMDBSession::NewLC( KCDVersion1_1 );
    
    // Get the table id from generic
    // 
    CMDBGenericRecord* generic = static_cast<CMDBGenericRecord*>
        ( CCDRecordBase::RecordFactoryL( 0 ) );
    CleanupStack::PushL( generic );
    generic->InitializeL( TPtrC( KCDTypeNameDataMobilitySelectionPolicy ), 
                          NULL );
    generic->LoadL( *db );

    // Create the DN-IAP record set
    // 
    CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>* dnIAPRecordSet = 
        new ( ELeave ) CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>( 
        generic->TableId() );
    CleanupStack::PushL( dnIAPRecordSet );

    dnIAPRecordSet->LoadL( *db );
    for ( TInt i = 0; i < dnIAPRecordSet->iRecords.Count(); i++ )
        {
        CCDDataMobilitySelectionPolicyRecord* dnRecord = new ( ELeave ) 
        CCDDataMobilitySelectionPolicyRecord( generic->TableId() );
                
        dnRecord->SetElementId( GetRealElementId( dnIAPRecordSet->iRecords[i] ) );
        dnRecord->LoadL( *db );

        TUint snapId = dnRecord->iSNAP;
        MPMLOGSTRING2( "CMPMCommsDatAccess::FindAllSnapsL snap %d", snapId )

        // Insert unique ids to array
        //
        aSnapIds.InsertInOrder( snapId );
        delete dnRecord;
        }

    CleanupStack::PopAndDestroy( dnIAPRecordSet );
    CleanupStack::PopAndDestroy( generic );
    CleanupStack::PopAndDestroy( db );
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::ValidateIapL
// -----------------------------------------------------------------------------
//
void CMPMCommsDatAccess::ValidateIapL( 
    const TConnectionId         aConnId,
    TUint32&                    aIapId, 
    TUint32&                    aNetId, 
    TBool&                      aLanOrWlan,
    CMPMServerSession&          aSession )
    {
    MPMLOGSTRING2(
        "CMPMCommsDatAccess::ValidateIapL: Validating chosen IAP Id = %i", 
        aIapId )

    aLanOrWlan = EFalse;

    SearchAccessNetIdL( aIapId, aNetId );

    CCommsDatabase* commsDatabase = CCommsDatabase::NewL();
    CleanupStack::PushL( commsDatabase );

    // Make hidden records visible
    // 
    commsDatabase->ShowHiddenRecords();

    CCommsDbTableView* commsDbIapTableView = 
        commsDatabase->OpenViewMatchingUintLC( TPtrC( IAP ),
                                               TPtrC( COMMDB_ID ),
                                               aIapId );

    User::LeaveIfError( commsDbIapTableView->GotoFirstRecord() );

    TInt length1( 0 );

    commsDbIapTableView->ReadColumnLengthL( TPtrC( IAP_SERVICE_TYPE ), 
                                            length1 );
    HBufC* buf1 = HBufC::NewLC( length1 );
    TPtr ptr1( buf1->Des() );

    // Read IAP's service type
    //
    commsDbIapTableView->ReadTextL(TPtrC( IAP_SERVICE_TYPE ), ptr1 );
    MPMLOGSTRING2(
        "CMPMCommsDatAccess::ValidateIapL: IAP service type = %S",
        &ptr1)

    // In case offline mode is enabled, we need to find out what type 
    // of bearer is used. Only LAN or WLAN is allowed in offline mode.
    // 
    if ( ptr1.Compare( TPtrC( LAN_SERVICE ) ) == KErrNone )
        {
        aLanOrWlan = ETrue;

        TUint32 service( 0 );
        commsDbIapTableView->ReadUintL( TPtrC( IAP_SERVICE ), service );

        // Now open a view to LAN_SERVICE table
        //
        CCommsDbTableView* commsDbLanTableView = 
            commsDatabase->OpenViewMatchingUintLC( TPtrC( LAN_SERVICE ),
                                                   TPtrC( COMMDB_ID ),
                                                   service );

        User::LeaveIfError( commsDbLanTableView->GotoFirstRecord() );

        TUint32 commDbId( 0 );

        commsDbLanTableView->ReadUintL( TPtrC( COMMDB_ID ), commDbId );

        // Now open a view to WLAN_SERVICE table
        //
        // If non wlan product, wlan tables won't exist and this will 
        // leave. In that case do not check WLAN related issues 
        //
        CCommsDbTableView* commsDbWlanTableView( NULL );
        TRAPD( err2, commsDbWlanTableView = 
                    commsDatabase->OpenViewMatchingUintLC( TPtrC( WLAN_SERVICE ),
                                                   TPtrC( WLAN_SERVICE_ID ),
                                                   commDbId );
                    CleanupStack::Pop( commsDbWlanTableView ) );
        if( err2 == KErrNone )
            {
            // Push back to cleanup stack. Object must be popped within 
            // TRAP, to avoid unbalance in cleanup stack and panic 71
            CleanupStack::PushL( commsDbWlanTableView );
            
            TInt err = commsDbWlanTableView->GotoFirstRecord();

            if ( err == KErrNone )
                {
                TWlanSsid ssid;
                commsDbWlanTableView->ReadTextL( TPtrC( NU_WLAN_SSID ), ssid );

                MPMLOGSSID( "CMPMCommsDatAccess<0x%x>::ValidateIapL: SSID = %S", 
                    aSession.ConnectionId(), &ssid )

                TUint32 presumedIapId = aIapId; 

                // Store Iap Id, Net Id and empty APN to TConnInfo, 
                // so that FindAPN can utilise the information 
                // during the validation of next IAP to support 
                // using existing WLAN connection SSID.
                //
                aSession.MyServer().Events()->AppendConnInfo( aConnId );
                aSession.MyServer().Events()->SetConnInfo( aConnId, 
                                                           aIapId, 
                                                           presumedIapId, 
                                                           aNetId, 
                                                           ssid );
                }
            else
                {
                MPMLOGSTRING2( "CMPMCommsDatAccess::ValidateIapL\
 GotoFirstRecord returned %d", err )
                }
            CleanupStack::PopAndDestroy( commsDbWlanTableView );
            }
        else
            {
            MPMLOGSTRING2(
                        "CMPMCommsDatAccess::ValidateIapL: WLAN table view failed, err %d", err2 )
            }                
        CleanupStack::PopAndDestroy( commsDbLanTableView );
        }

    // If this IAP needs PDP Context management, we'll do it here
    // 
    else if ( ptr1.Compare(TPtrC(OUTGOING_WCDMA)) == KErrNone )
        {
        TUint32 service;
        commsDbIapTableView->ReadUintL( TPtrC( IAP_SERVICE ), service );

        // Now open a view to OUTGOING_WCDMA table
        //
        CCommsDbTableView* commsDbPsdTableView = 
            commsDatabase->OpenViewMatchingUintLC( TPtrC( OUTGOING_WCDMA ),
                                                   TPtrC( COMMDB_ID ),
                                                   service );

        User::LeaveIfError( commsDbPsdTableView->GotoFirstRecord() );

        TInt length2( 0 );

        commsDbPsdTableView->ReadColumnLengthL( TPtrC( GPRS_APN ), length2 );

        HBufC* buf2 = HBufC::NewLC( length2 );
        TPtr ptr2( buf2->Des() );

        // Read APN
        //
        commsDbPsdTableView->ReadTextL( TPtrC( GPRS_APN ), ptr2 );
        MPMLOGSTRING2(
            "CMPMCommsDatAccess::ValidateIapL: APN = %S", &ptr2)

        TUint32 presumedIapId = aIapId; 

        // Find and change Iap and Net Id if APN exist
        //
        aSession.MyServer().Events()->FindAPN( ptr2, aIapId, aNetId ); 

        // Store Iap Id, Net Id and APN to TConnInfo, 
        // so that FindAPN can utilise the information 
        // during the validation of next IAP to support 
        // using existing PDP context per APN.
        //
        aSession.MyServer().Events()->AppendConnInfo( aConnId );
        aSession.MyServer().Events()->SetConnInfo( aConnId, 
                                                   aIapId, 
                                                   presumedIapId, 
                                                   aNetId, 
                                                   ptr2 );
        CleanupStack::PopAndDestroy( buf2 );
        CleanupStack::PopAndDestroy( commsDbPsdTableView );
        }
    else if ( ptr1.Compare(TPtrC( VPN_SERVICE )) == KErrNone )
        {
        MPMLOGSTRING(
            "CMPMCommsDatAccess::ValidateIapL: Enable VPN in offline mode" )
        aLanOrWlan = ETrue;
        }
    else
        {
        MPMLOGSTRING(
            "CMPMCommsDatAccess::ValidateIapL: Service Type out of range" )
        }

    CleanupStack::PopAndDestroy( buf1 );
    CleanupStack::PopAndDestroy( commsDbIapTableView );
    CleanupStack::PopAndDestroy( commsDatabase );

    MPMLOGSTRING2(
        "CMPMCommsDatAccess::ValidateIapL: Validated IAP Id = %i", aIapId )
    }


// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::CheckBearerL
// -----------------------------------------------------------------------------
//
void CMPMCommsDatAccess::CheckBearerL( 
    TUint32&                    aIapId, 
    TBool&                      aLanOrWlan, 
    const RAvailableIAPList&    aAvailableIAPList,
    CMPMServerSession&          aSession )
    {
    MPMLOGSTRING2( "CMPMCommsDatAccess::CheckBearerL: IAP Id = %i", aIapId )

    aLanOrWlan = EFalse;

    // Use FindRealIapL to find the real IAP behind the virtual IAP.
    // 
    TUint32 realIapId( 0 );
    FindRealIapL( aIapId, 
                  realIapId, 
                  aAvailableIAPList,
                  aSession );

    if ( ( realIapId != 0 ) && ( aIapId != realIapId ) )
        {
        aIapId = realIapId;
        MPMLOGSTRING2(
            "CMPMCommsDatAccess::CheckBearerL: Found real IAP Id = %i", 
            aIapId )
        }

    CCommsDatabase* commsDatabase = CCommsDatabase::NewL();
    CleanupStack::PushL(commsDatabase);

    // Make hidden records visible
    // 
    commsDatabase->ShowHiddenRecords();

    CCommsDbTableView* commsDbIapTableView = 
        commsDatabase->OpenViewMatchingUintLC( TPtrC(IAP),
                                               TPtrC(COMMDB_ID),
                                               aIapId);

    User::LeaveIfError( commsDbIapTableView->GotoFirstRecord() );

    TInt length1( 0 );

    commsDbIapTableView->ReadColumnLengthL(TPtrC(IAP_SERVICE_TYPE), length1);
    HBufC* buf1 = HBufC::NewLC(length1);
    TPtr ptr1( buf1->Des() );

    // Read IAP's service type
    //
    commsDbIapTableView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), ptr1);
    MPMLOGSTRING2(
        "CMPMCommsDatAccess::CheckBearerL: IAP_SERVICE_TYPE from CommsDat = %S",
        &ptr1 )

    // In case offline mode is enabled, we need to find out what type 
    // of bearer is used. Only LAN or WLAN is allowed in offline mode.
    // 
    if ( ptr1.Compare(TPtrC(LAN_SERVICE)) == KErrNone )
        {
        aLanOrWlan = ETrue;
        MPMLOGSTRING( "CMPMCommsDatAccess::CheckBearerL: Bearer type LAN/WLAN" )
        }
    else
        {
        aLanOrWlan = EFalse;
        MPMLOGSTRING( "CMPMCommsDatAccess::CheckBearerL: Bearer type GSM/UMTS" )
        }

    CleanupStack::PopAndDestroy(buf1);
    CleanupStack::PopAndDestroy(commsDbIapTableView);    
    CleanupStack::PopAndDestroy(commsDatabase);
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::DumpIAPsL
// -----------------------------------------------------------------------------
//
void CMPMCommsDatAccess::DumpIAPsL() const
    {
#ifdef _DEBUG

    // Dump CommsDat IAPs to log in order to support testing
    // 
    TInt err( KErrNone );

    MPMLOGSTRING( "Display CommsDat IAPs - Start" )
    MPMLOGSTRING( "" )

    CCommsDatabase* commsDatabase = CCommsDatabase::NewL();
    CleanupStack::PushL(commsDatabase);

    // Make hidden records visible
    // 
    commsDatabase->ShowHiddenRecords();

    // Iterate the IAP table from CommsDat
    CCommsDbTableView* table = NULL;
    table = commsDatabase->OpenTableLC( TPtrC( IAP ) );

    err = table->GotoFirstRecord();

    while ( !err )
        {
        TUint32 iapId( 0 );
        TInt    length1( 0 );
        TInt    length2( 0 );

        // Read IAP's ID
        //
        table->ReadUintL( TPtrC( COMMDB_ID ), iapId );

        // Read IAP's connection name
        //
        table->ReadColumnLengthL( TPtrC( COMMDB_NAME ), length1 );
        HBufC* buf1 = HBufC::NewLC( length1 );
        TPtr ptr1( buf1->Des() );
        table->ReadTextL( TPtrC( COMMDB_NAME ), ptr1 );

        // Read IAP's service type
        //
        table->ReadColumnLengthL(TPtrC( IAP_SERVICE_TYPE ), length2 );
        HBufC* buf2 = HBufC::NewLC( length2 );
        TPtr ptr2( buf2->Des() );
        table->ReadTextL(TPtrC( IAP_SERVICE_TYPE ), ptr2 );

        // Dump information to log
        // 
        MPMLOGSTRING3( "IAP %i: Name = %S", iapId, &ptr1 )
        MPMLOGSTRING2( "Service type = %S", &ptr2 )

        MPMLOGSTRING( "" )
        
        err = table->GotoNextRecord();

        CleanupStack::PopAndDestroy( buf2 );
        CleanupStack::PopAndDestroy( buf1 );
        }

    MPMLOGSTRING( "Display CommsDat IAPs - End" )

    CleanupStack::PopAndDestroy( table );
    CleanupStack::PopAndDestroy( commsDatabase );
#endif // _DEBUG
    }


// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::DumpSnapsL
// -----------------------------------------------------------------------------
//
void CMPMCommsDatAccess::DumpSnapsL()
    {
#ifdef _DEBUG

    // Dump CommsDat Snaps to log in order to support testing
    // 
    MPMLOGSTRING( "Display CommsDat Snaps - Start" )
    MPMLOGSTRING( "" )

    // Creating a session with the latest version
    // 
    CMDBSession* db = CMDBSession::NewLC( KCDVersion1_1 );

    db->SetAttributeMask( ECDHidden | ECDProtectedWrite );

    // Create the generic record set
    // 
    CMDBGenericRecord* generic = static_cast<CMDBGenericRecord*>
        ( CCDRecordBase::RecordFactoryL( 0 ) ); 
    CleanupStack::PushL( generic );  
    generic->InitializeL( TPtrC( KCDTypeNameDataMobilitySelectionPolicy ), 
                          NULL );
    generic->LoadL( *db );

    // Create the DN-IAP record set
    // 
    CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>* dnIAPRecordSet = 
        new ( ELeave ) CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>( 
        generic->TableId() );
    CleanupStack::PushL( dnIAPRecordSet ); 

    CCDDataMobilitySelectionPolicyRecord* dnIAPRecord = new ( ELeave ) 
        CCDDataMobilitySelectionPolicyRecord( generic->TableId() );
    CleanupStack::PushL( dnIAPRecord );

    // Find all the DN-IAP records
    // 
    dnIAPRecordSet->LoadL( *db );

    CCDIAPRecord*       linkedIAP;
    //CCDNetworkRecord*   linkedDN;
    //CCDNetworkRecord*   linkedDN2;
    CCDAccessPointRecord*   linkedAP;
    CCDAccessPointRecord*   linkedAP2;
    RArray<TInt>        prevRecordIds;
    CleanupClosePushL( prevRecordIds );

    for ( TInt i = 0; i < dnIAPRecordSet->iRecords.Count(); i++ )
        {
        // Load each DN-IAP entry based on the elementId in the record set
        // 
        dnIAPRecord->SetElementId( GetRealElementId( dnIAPRecordSet->iRecords[i] ) );
        dnIAPRecord->LoadL( *db );


        linkedAP =  static_cast<CCDAccessPointRecord*>( CCDAccessPointRecord::RecordFactoryL( KCDTIdAccessPointRecord ) );
        CleanupStack::PushL( linkedAP );

        // Find the SNAP AP pointed by DN-IAP entry
        //
        linkedAP->iRecordTag = (TInt)dnIAPRecord->iSNAP;
        TBool found = linkedAP->FindL( *db );

        // Don't print duplicates
        // 
        if ( found && KErrNotFound == prevRecordIds.Find( linkedAP->RecordId() ) )
            {
            prevRecordIds.Append( linkedAP->RecordId() );
            
            // Print the DN and host name pointed by DN-IAP entry.
            // HostName is EText field.
            // 
            TBuf<KCommsDatTextBufferLength> dn = linkedAP->iRecordName.GetL();
            MPMLOGSTRING3( "Snap: %d Snap Name = %s", (TInt)linkedAP->iRecordTag, dn.PtrZ() )
            
            // Related DN-IAP records 
            // Get the table id from generic record
            // 
            CMDBGenericRecord* generic2 = static_cast<CMDBGenericRecord*>
                ( CCDRecordBase::RecordFactoryL( 0 ) ); 
            CleanupStack::PushL( generic2 );  
            generic2->InitializeL( TPtrC( 
                KCDTypeNameDataMobilitySelectionPolicy ), NULL );
            generic2->LoadL( *db );
            
            // Doesn't quite work with generic records
            // 
            CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>* 
                dnIAPRecordSet2 = new ( ELeave ) 
                CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>( 
                generic2->TableId() );
            CleanupStack::PushL( dnIAPRecordSet2 ); 
            
            // Prime one record with DN 
            // 
            CCDDataMobilitySelectionPolicyRecord* dnIAPRecord2 = 
                new ( ELeave ) CCDDataMobilitySelectionPolicyRecord( 
                generic2->TableId() );
            CleanupStack::PushL( dnIAPRecord2 );
            dnIAPRecord2->iSNAP = (TInt)linkedAP->iRecordTag;
            dnIAPRecordSet2->iRecords.Append( dnIAPRecord2 );
            CleanupStack::Pop( dnIAPRecord2 );

            if ( dnIAPRecordSet2->FindL( *db ) )
                {
                for (TInt j = 0; j < dnIAPRecordSet2->iRecords.Count(); j++)
                    {
                    dnIAPRecord2->SetElementId( GetRealElementId( dnIAPRecordSet2->iRecords[j] ) );
                    dnIAPRecord2->LoadL( *db );

                    if ( dnIAPRecord2->iIAP != 0 )
                        {
                        linkedIAP = static_cast<CCDIAPRecord*>
                            ( LoadLinkedRecordL( *db, dnIAPRecord2->iIAP ) );
                        MPMLOGSTRING3( "IAP = %d Priority = %d", 
                            linkedIAP->RecordId(), 
                            static_cast<TUint32>( dnIAPRecord2->iPriority ) )
                        delete linkedIAP;
                        dnIAPRecord2->iIAP = 0;
                        }
                    else if( dnIAPRecord2->iEmbeddedSNAP != 0 )
                        {
                        linkedAP2 =  static_cast<CCDAccessPointRecord*>( 
                                CCDAccessPointRecord::RecordFactoryL( KCDTIdAccessPointRecord ) );
                        CleanupStack::PushL( linkedAP2 );
                        TInt esnap = dnIAPRecord2->iEmbeddedSNAP;
                        linkedAP2->iRecordTag = esnap;
                        if ( !linkedAP->FindL( *db ))
                            {
                            User::Leave( KErrNotFound );
                            }
                        
                        MPMLOGSTRING3( "Embedded Snap = %d Priority = %d", 
                            (TInt)linkedAP2->iRecordTag, 
                            static_cast<TUint32>( dnIAPRecord2->iPriority ) )
                        CleanupStack::PopAndDestroy( linkedAP2 );
                        dnIAPRecord2->iEmbeddedSNAP = 0;
                        }
                    else
                        {
                        // Each Snap has an initial entry in this table,
                        // where both the IAP and EmbeddedSNAP links 
                        // have null value. 
                        }
                    }
                }
            CleanupStack::PopAndDestroy( dnIAPRecordSet2 );
            CleanupStack::PopAndDestroy( generic2 );
            MPMLOGSTRING( "" )
            }
        CleanupStack::PopAndDestroy( linkedAP );
        }

    MPMLOGSTRING( "Display CommsDat Snaps - End" )

    CleanupStack::PopAndDestroy( &prevRecordIds );
    CleanupStack::PopAndDestroy( dnIAPRecord );
    CleanupStack::PopAndDestroy( dnIAPRecordSet );
    CleanupStack::PopAndDestroy( generic );
    CleanupStack::PopAndDestroy( db );
#endif // _DEBUG
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::DeterminePrioritiesL
// -----------------------------------------------------------------------------
//
void CMPMCommsDatAccess::DeterminePrioritiesL( 
    RArray<TNetIap>&          aDNIaps, 
    const RAvailableIAPList&  aAvailableIAPList,
    CMPMServerSession&        aSession )
    {
    MPMLOGSTRING( "CMPMCommsDatAccess::DeterminePrioritiesL" )

    TBool useGlobalPriorities( EFalse );

    for( TInt i( 0 ); ( i < aDNIaps.Count() ) && !useGlobalPriorities; i++ )
        {
        if( aDNIaps[i].iRanking == 
            CMManager::KDataMobilitySelectionPolicyPriorityWildCard )
            {
            // If any of the IAPs in the array has wildcard priority, 
            // then global bearer priorities should be used. 
            //
            useGlobalPriorities = ETrue;
            }
        }

    if( !useGlobalPriorities )
        {
        // If not a single IAP in the array had wildcard priority, 
        // then local priorities defined by user should be used. 
        //
        return;
        }

    for( TInt i( 0 ); i < aDNIaps.Count() ; i++ )
        {
        MPMLOGSTRING2(
            "CMPMCommsDatAccess::DeterminePrioritiesL: IapId = %i \
has wildcard priority", aDNIaps[i].iIapId )

        // Find out the real IAP Id and check 
        // the global bearer priority for that.
        // 
        TUint32 priority( KLowestPriority );
        TUint32 iapId;
        FindRealIapL( aDNIaps[i].iIapId, 
                      iapId, 
                      aAvailableIAPList,
                      aSession );

        GlobalBearerPriorityL( iapId, 
                               priority );

        aDNIaps[i].iGlobalPriority = priority;

        MPMLOGSTRING3(
            "CMPMCommsDatAccess::DeterminePrioritiesL: IapId = %i \
has global priority: %i", aDNIaps[i].iIapId, priority )
        }

    // Finally, sort the array according to global bearer priorities. 
    // 
    aDNIaps.Sort( TNetIap::CompareGlobalAndLocalPriority );
    }


// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::GlobalBearerPriorityL
// -----------------------------------------------------------------------------
//
void CMPMCommsDatAccess::GlobalBearerPriorityL( const TUint32   aIapId,
                                               TUint32&        aPriority )
    {
    MPMLOGSTRING2( "CMPMCommsDatAccess::GlobalBearerPriorityL: IapId: %i", 
        aIapId )

    CMDBSession* db = CMDBSession::NewLC( KCDVersion1_1 );

    db->SetAttributeMask( ECDHidden | ECDProtectedWrite );

    // Get the IAP record
    // 
    CCDIAPRecord* iap = LoadIapRecordLC( aIapId, db );

    // Get the service type of aIapId
    // 
    TBuf<KMaxTextLength + 1> service;
    service = iap->iServiceType.GetL();
    MPMLOGSTRING2(
        "CMPMCommsDatAccess::GlobalBearerPriorityL: Service type = %s",
        service.PtrZ() )

    // Get the table id from generic
    // 
    CMDBGenericRecord* record = static_cast<CMDBGenericRecord*>
        ( CCDRecordBase::RecordFactoryL( 0 ) );
    CleanupStack::PushL( record );
    record->InitializeL( TPtrC( KCDTypeNameGlobalService ), NULL );
    record->SetRecordId( KCDInitialRecordId );
    record->LoadL( *db );

    // Create Global Bearer Type Priorization Record
    // 
    CCDGlobalBearerTypePriorizationRecord* globalBearer = new ( ELeave ) 
        CCDGlobalBearerTypePriorizationRecord( record->TableId() );
    CleanupStack::PushL( globalBearer );

    // Set the service type of aIapId to Global 
    // Bearer Type Priorization Record
    // 
    globalBearer->iServiceType.SetL( service );

    if( globalBearer->FindL( *db ) )
        {
        // Get the priority of specified service type
        // 
        aPriority = ( TUint32 )globalBearer->iPriority;
        MPMLOGSTRING2(
            "CMPMCommsDatAccess::GlobalBearerPriorityL: Priority = %i", 
            aPriority )
        }

    // Release memory
    //
    CleanupStack::PopAndDestroy( globalBearer );
    CleanupStack::PopAndDestroy( record );
    CleanupStack::PopAndDestroy( iap );
    CleanupStack::PopAndDestroy( db );
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::CheckWLANIapL
// -----------------------------------------------------------------------------
//
void CMPMCommsDatAccess::CheckWLANIapL(CMPMServerSession& aSession)
    {
    MPMLOGSTRING( "CMPMCommsDatAccess::CheckWLANIapL" )

    TInt err( KErrNone );
    TBool found( EFalse );

    CCommsDatabase* commsDatabase = CCommsDatabase::NewL();
    CleanupStack::PushL( commsDatabase );

    // Make hidden records visible
    // 
    commsDatabase->ShowHiddenRecords();

    // Iterate the IAP table from CommsDat
    CCommsDbTableView* table = NULL;
    table = commsDatabase->OpenTableLC( TPtrC( IAP ) );

    err = table->GotoFirstRecord();

    while ( !err && !found )
        {
        TUint32 iapId( 0 );
        TInt    length( 0 );

        // Read IAP's ID
        //
        table->ReadUintL( TPtrC( COMMDB_ID ), iapId );

        // Read IAP's service type
        //
        table->ReadColumnLengthL(TPtrC( IAP_SERVICE_TYPE ), length );
        HBufC* buf = HBufC::NewLC( length );
        TPtr ptr( buf->Des() );
        table->ReadTextL(TPtrC( IAP_SERVICE_TYPE ), ptr );

        if ( ptr.Compare( TPtrC( LAN_SERVICE ) ) == KErrNone )
            {
            TUint32 service( 0 );
            table->ReadUintL( TPtrC( IAP_SERVICE ), service );

            // Now open a view to LAN_SERVICE table
            //
            CCommsDbTableView* commsDbLanTableView = 
                commsDatabase->OpenViewMatchingUintLC( TPtrC( LAN_SERVICE ),
                                                       TPtrC( COMMDB_ID ),
                                                       service );

            User::LeaveIfError( commsDbLanTableView->GotoFirstRecord() );

            TUint32 commDbId( 0 );

            commsDbLanTableView->ReadUintL( TPtrC( COMMDB_ID ), commDbId );

            // Now open a view to WLAN_SERVICE table
            //
            // If non wlan product, wlan tables won't exist and this will 
            // leave. In that case do not check WLAN related issues 
            //

            CCommsDbTableView* commsDbWlanTableView( NULL );
            TRAP( err, commsDbWlanTableView = 
                         commsDatabase->OpenViewMatchingUintLC( 
                                                TPtrC( WLAN_SERVICE ),
                                                TPtrC( WLAN_SERVICE_ID ),
                                                commDbId ); 
                         CleanupStack::Pop( commsDbWlanTableView ); );
            if ( err == KErrNone )
                {
                // Push back to cleanup stack. Object must be popped within 
                // TRAP, to avoid unbalance in cleanup stack and panic 71
                CleanupStack::PushL( commsDbWlanTableView );
                
                TInt error = commsDbWlanTableView->GotoFirstRecord();

                if ( error == KErrNone )
                    {
                    // Easy WLAN is not considered to be "real" WLAN IAP
                    // 
                    TInt ssidLength( 0 );
                    commsDbWlanTableView->ReadColumnLengthL( 
                                                    TPtrC( NU_WLAN_SSID ), 
                                                    ssidLength );
                    if ( ssidLength != 0 )
                        {
                        found = ETrue;

                        MPMLOGSTRING( "CMPMCommsDatAccess::CheckWLANIapL:\
 Found WLAN IAP" )

                        // If a single WLAN IAP was found in CommsDat, 
                        // then set the need to scan WLAN networks as true.
                        // 
                        aSession.MyServer().SetWLANScanRequired( ETrue );
                        }
                    }

                

                // Release memory
                //
                CleanupStack::PopAndDestroy( commsDbWlanTableView );
                }
            else
                {
                MPMLOGSTRING2(
                            "CMPMCommsDatAccess::CheckWLANIapL: WLAN table view failed, err %d", err )
                }
            CleanupStack::PopAndDestroy( commsDbLanTableView );
            }
        err = table->GotoNextRecord();

        CleanupStack::PopAndDestroy( buf );
        }

    // If not a single WLAN IAP was found in CommsDat, 
    // then set the need to scan WLAN networks as false.
    // 
    if ( !found )
        {
        MPMLOGSTRING( "CMPMCommsDatAccess::CheckWLANIapL: Not found WLAN IAP" )
        aSession.MyServer().SetWLANScanRequired( EFalse );
        }

    CleanupStack::PopAndDestroy( table );
    CleanupStack::PopAndDestroy( commsDatabase );
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::CheckHiddenAgentL
// -----------------------------------------------------------------------------
//
TBool CMPMCommsDatAccess::CheckHiddenAgentL( const TUint32 aIapId ) const
    {
    MPMLOGSTRING2( "CMPMCommsDatAccess::CheckHiddenAgentL: IapId: %i", aIapId )

    TBool retValue( EFalse );

    CMDBSession* db = CMDBSession::NewLC( KCDVersion1_1 );

    db->SetAttributeMask( ECDHidden | ECDProtectedWrite );

    // First check that IAP exists
    // 
    CCDIAPRecord* iap = LoadIapRecordLC( aIapId, db );

    // Initialize generic record
    // 
    CMDBGenericRecord* generic = static_cast<CMDBGenericRecord*> ( 
        CCDRecordBase::RecordFactoryL( 0 ) );
    CleanupStack::PushL( generic );
    generic->InitializeL( TPtrC( KCDTypeNameIAPMetadata ), NULL );
    generic->LoadL( *db );

    CCDIAPMetadataRecord* record = new( ELeave ) CCDIAPMetadataRecord( 
        generic->TableId() );
    CleanupStack::PushL( record );

    // Check if the metadata entry exists
    // 
    record->iIAP.SetL( GetRealElementId( iap ) );
    if ( record->FindL( *db ) )
        {
        // Metadata is specified, check is it true or false.
        // 
        if ( record->iMetadata & EMetaHiddenAgent )
            {
            MPMLOGSTRING( "CMPMCommsDatAccess::CheckHiddenAgentL: True" )
            retValue = ETrue;
            }
        else
            {
            MPMLOGSTRING( "CMPMCommsDatAccess::CheckHiddenAgentL: False" )
            retValue = EFalse;
            }
        }
    else
        {
        // Metadata unspecified, use default value.
        // 
        MPMLOGSTRING( "CMPMCommsDatAccess::CheckHiddenAgentL: False" )
        retValue = EFalse;
        }

    // Release memory
    //
    CleanupStack::PopAndDestroy( record );
    CleanupStack::PopAndDestroy( generic );
    CleanupStack::PopAndDestroy( iap );
    CleanupStack::PopAndDestroy( db );

    return retValue;
    }


// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::MatchSSIDL
// -----------------------------------------------------------------------------
//
void CMPMCommsDatAccess::MatchSSIDL( const TWlanSsid&   aSSID,
                                     TUint32            aIapId, 
                                     TBool&             aMatchFound,
                                     CMPMServerSession& aSession )
    {
#ifdef __WINS__    	
    // To suppress compilation warning in WINSCW UREL build. aSession is used only for logging. 
    aSession;
#endif
    
    MPMLOGSSID( "CMPMCommsDatAccess<0x%x>::MatchSSIDL: aSSID = %S", 
        aSession.ConnectionId(), &aSSID )

    MPMLOGSTRING3( "CMPMCommsDatAccess<0x%x>::MatchSSIDL: aIapId = %d", 
        aSession.ConnectionId(), aIapId )

    aMatchFound = EFalse;

    CCommsDatabase* commsDatabase = CCommsDatabase::NewL();
    CleanupStack::PushL( commsDatabase );

    // Make hidden records visible
    // 
    commsDatabase->ShowHiddenRecords();

    CCommsDbTableView* commsDbIapTableView = 
        commsDatabase->OpenViewMatchingUintLC( TPtrC( IAP ),
                                               TPtrC( COMMDB_ID ),
                                               aIapId );

    User::LeaveIfError( commsDbIapTableView->GotoFirstRecord() );

    TInt length1( 0 );

    commsDbIapTableView->ReadColumnLengthL( TPtrC( IAP_SERVICE_TYPE ), 
                                            length1 );
    HBufC* buf1 = HBufC::NewLC( length1 );
    TPtr ptr1( buf1->Des() );

    // Read IAP's service type
    //
    commsDbIapTableView->ReadTextL(TPtrC( IAP_SERVICE_TYPE ), ptr1 );

    MPMLOGSTRING3(
        "CMPMCommsDatAccess<0x%x>::MatchSSIDL: IAP service type = %S",
        aSession.ConnectionId(), &ptr1 )

    // In case offline mode is enabled, we need to find out what type 
    // of bearer is used. Only LAN or WLAN is allowed in offline mode.
    // 
    if ( ptr1.Compare( TPtrC( LAN_SERVICE ) ) == KErrNone )
        {
        TUint32 service( 0 );
        commsDbIapTableView->ReadUintL( TPtrC( IAP_SERVICE ), service );

        // Now open a view to LAN_SERVICE table
        //
        CCommsDbTableView* commsDbLanTableView = 
            commsDatabase->OpenViewMatchingUintLC( TPtrC( LAN_SERVICE ),
                                                   TPtrC( COMMDB_ID ),
                                                   service );

        User::LeaveIfError( commsDbLanTableView->GotoFirstRecord() );

        TUint32 commDbId( 0 );

        commsDbLanTableView->ReadUintL( TPtrC( COMMDB_ID ), commDbId );

        // Now open a view to WLAN_SERVICE table
        //
        CCommsDbTableView* commsDbWlanTableView = 
            commsDatabase->OpenViewMatchingUintLC( TPtrC( WLAN_SERVICE ),
                                                   TPtrC( WLAN_SERVICE_ID ),
                                                   commDbId );

        TInt err = commsDbWlanTableView->GotoFirstRecord();

        if ( err == KErrNone )
            {
            TWlanSsid ssid;
            commsDbWlanTableView->ReadTextL( TPtrC( NU_WLAN_SSID ), ssid );

            if ( aSSID.Compare( ssid ) == KErrNone )
                {
                aMatchFound = ETrue;
                MPMLOGSTRING2(
                    "CMPMCommsDatAccess<0x%x>::MatchSSIDL Found matching SSID",
                    aSession.ConnectionId() )
                }
            }
        else
            {
            MPMLOGSTRING3( "CMPMCommsDatAccess<0x%x>::MatchSSID\
 GotoFirstRecord returned %d", aSession.ConnectionId(), err )
            }

        // Release memory
        //
        CleanupStack::PopAndDestroy( commsDbWlanTableView );
        CleanupStack::PopAndDestroy( commsDbLanTableView );
        }

    // Release memory
    //
    CleanupStack::PopAndDestroy( buf1 );
    CleanupStack::PopAndDestroy( commsDbIapTableView );
    CleanupStack::PopAndDestroy( commsDatabase );
    }


// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::IsVirtualIapL
// -----------------------------------------------------------------------------
//
TBool CMPMCommsDatAccess::IsVirtualIapL( const TUint32 aVirtualIapId ) const
    {
    MPMLOGSTRING2( "CMPMCommsDatAccess::IsVirtualIapL: VirtualIapId: %i", 
        aVirtualIapId )
    TBool isVirtual( EFalse );

    // Open transaction
    // 
    CMDBSession* db = CMDBSession::NewLC( KCDVersion1_1 );

    db->SetAttributeMask( ECDHidden | ECDProtectedWrite );

    // Get IAP
    // 
    CCDIAPRecord* iap = LoadIapRecordLC( aVirtualIapId, db );

    TDesC& type = iap->iBearerType.GetL();

    if( type.Compare( TPtrC ( KCDTypeNameVirtualBearer ) ) == 0 )
        {
        isVirtual = ETrue;
        MPMLOGSTRING( "CMPMCommsDatAccess::IsVirtualIapL: \
is virtual" )
        }
    CleanupStack::PopAndDestroy( iap );
    CleanupStack::PopAndDestroy( db );
    return isVirtual;
    }
    
// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::SnapContainsWlanL
// -----------------------------------------------------------------------------
//
TBool CMPMCommsDatAccess::SnapContainsWlanL( TUint32			aSnapRecord, 
                                             RArray<TUint>& 	aIapPath, 
                                             TInt 		    	aMinNrWlan )
    {
    MPMLOGSTRING3( "CMPMCommsDatAccess::SnapContainsWlanL: snap %d, nr Wlans %d", 
        aSnapRecord, aMinNrWlan )
    TInt wlanFound(0);
    RArray<TNetIap> destNetIaps;
    CleanupClosePushL( destNetIaps );

    // Find the valid IAPs belonging to the Snap 
    // and possible embedded SNAP. Store both in same array.
    //
    SearchDNEntriesWithEmbeddedL( aSnapRecord, destNetIaps, destNetIaps );

    // Check type of each IAP 
    // Stop when specified number of WLAN IAPs has been found
    // 
    for( TInt j( 0 ); ( j < destNetIaps.Count() ) && 
                      ( wlanFound < aMinNrWlan ); j++ )
        {
        // If entry is for embedded SNAP there are more than two chained 
        // embedded snaps. This is illegal configuration. Skip this entry.
        // 
        if ( destNetIaps[j].iEmbeddedSnap != 0 )
            {
            MPMLOGSTRING( "CMPMCommsDatAccess::SnapContainsWlanL: \
Illegal configuration, more than one chained SNAP" )
            }
        else
            {
            if( CheckWlanL( destNetIaps[j].iIapId ) != ENotWlanIap )
                {
                MPMLOGSTRING(
                    "CMPMCommsDatAccess::SnapContainsWlanL: WLAN IAP found" )
                wlanFound++;
                }
            // If virtual IAP, check whether it may be using WLAN
            // 
            if( wlanFound < aMinNrWlan && IsVirtualIapL( destNetIaps[j].iIapId ) )
                {
                if ( WlanUnderVirtualL( destNetIaps[j].iIapId,
                                        aIapPath ) )
                    {
                    MPMLOGSTRING( "CMPMCommsDatAccess::SnapContainsWlanL: \
WLAN IAP under virtual IAP found" )
                    wlanFound++;
                    }
            
                }
            }
        }
    CleanupStack::PopAndDestroy( &destNetIaps );
    if( wlanFound < aMinNrWlan ) 
        {
        MPMLOGSTRING( "CMPMCommsDatAccess::SnapContainsWlanL: result false" )
        return EFalse;
        }
    else
        {
        MPMLOGSTRING( "CMPMCommsDatAccess::SnapContainsWlanL: result true" )
        return ETrue;
        }
    }


// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::SearchDNEntriesWithEmbeddedL
// -----------------------------------------------------------------------------
//
void CMPMCommsDatAccess::SearchDNEntriesWithEmbeddedL( TUint32 aSnap, 
                                                      RArray<TNetIap>& aDestNetIaps,
                                                      RArray<TNetIap>& aEmbeddedIaps )
    {
    // Find the valid IAPs belonging to the Snap.
    //
    SearchDNEntriesL( aSnap, aDestNetIaps );

    TBool embeddedFound( EFalse );

    // Find the valid IAPs belonging to the embedded Snaps.
    // 
    for( TInt j = 0;( ( j < aDestNetIaps.Count() ) && !embeddedFound ); j++ )
        {
        // Go through the destNetIaps-array looking for EmbeddedId.
        // 
        if( aDestNetIaps[j].iEmbeddedSnap != 0 )
            {
            MPMLOGSTRING2( "CMPMCommsDatAccess::SearchDNEntriesWithEmbeddedL: \
Search Embedded SNAP: %i", aDestNetIaps[j].iEmbeddedSnap )

            TUint32 embedded = aDestNetIaps[j].iEmbeddedSnap;
            // Exclude embedded Snap from destNetIaps, since 
            // the IAPs it contains are listed in embeddedIaps. 
            // 
            aDestNetIaps.Remove( j );

            // If found, then search the IAPs belonging 
            // to that embedded Snap.
            // 
            SearchDNEntriesL( embedded, aEmbeddedIaps );
            embeddedFound = ETrue;
            }
        }
    }


// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::CheckWLanL
// -----------------------------------------------------------------------------
//
TWlanIapType CMPMCommsDatAccess::CheckWlanL( TUint32 aIapId ) const
    {
    MPMLOGSTRING2( "CMPMCommsDatAccess::CheckWlanL: IAP Id = %i", aIapId)

    TWlanIapType wlan( ENotWlanIap );
    CCommsDatabase* commsDatabase = CCommsDatabase::NewL();
    CleanupStack::PushL( commsDatabase );

    // Make hidden records visible
    // 
    commsDatabase->ShowHiddenRecords();

    CCommsDbTableView* commsDbIapTableView = 
        commsDatabase->OpenViewMatchingUintLC( TPtrC( IAP ),
                                               TPtrC( COMMDB_ID ),
                                               aIapId );

    User::LeaveIfError( commsDbIapTableView->GotoFirstRecord() );

    TInt length1( 0 );

    commsDbIapTableView->ReadColumnLengthL( TPtrC( IAP_SERVICE_TYPE ), 
                                            length1 );

    HBufC* buf1 = HBufC::NewLC( length1 );
    TPtr ptr1( buf1->Des() );

    // Read IAP's service type
    //
    commsDbIapTableView->ReadTextL(TPtrC( IAP_SERVICE_TYPE ), ptr1 );
    MPMLOGSTRING2(
        "CMPMCommsDatAccess::CheckWlanL: IAP service type = %S", &ptr1 )

    if ( ptr1.Compare( TPtrC( LAN_SERVICE ) ) == KErrNone )
        {
        TUint32 service( 0 );
        commsDbIapTableView->ReadUintL( TPtrC( IAP_SERVICE ), service );

        // Now open a view to LAN_SERVICE table
        //
        CCommsDbTableView* commsDbLanTableView = 
            commsDatabase->OpenViewMatchingUintLC( TPtrC( LAN_SERVICE ),
                                                   TPtrC( COMMDB_ID ),
                                                   service );

        User::LeaveIfError( commsDbLanTableView->GotoFirstRecord() );

        TUint32 commDbId( 0 );

        commsDbLanTableView->ReadUintL( TPtrC( COMMDB_ID ), commDbId );

        // Now open a view to WLAN_SERVICE table
        //
        // If non wlan product, wlan tables won't exist and this will 
        // leave. In that case do not check WLAN related issues 
        //

        CCommsDbTableView* commsDbWlanTableView( NULL );
        TRAPD( err2, commsDbWlanTableView = 
                     commsDatabase->OpenViewMatchingUintLC( TPtrC( WLAN_SERVICE ),
                                                            TPtrC( WLAN_SERVICE_ID ),
                                                            commDbId );
                     CleanupStack::Pop( commsDbWlanTableView ) );
        if ( err2 == KErrNone )
            {
            // Push back to cleanup stack. Object must be popped within 
            // TRAP, to avoid unbalance in cleanup stack and panic 71
            CleanupStack::PushL( commsDbWlanTableView );

            TInt err = commsDbWlanTableView->GotoFirstRecord();

            if ( err == KErrNone )
                {
                MPMLOGSTRING( "CMPMCommsDatAccess::CheckWlanL: IAP is WLAN" )
                wlan = EWlanIap;
                TInt ssidLength( 0 );
                commsDbWlanTableView->ReadColumnLengthL( TPtrC( NU_WLAN_SSID ), 
                                                     ssidLength );
                }
            else
                {
                MPMLOGSTRING2( "CMPMCommsDatAccess::CheckWlanL\
 GotoFirstRecord returned %d", err )
               }
            // Release memory
            //
            CleanupStack::PopAndDestroy( commsDbWlanTableView );            
            }
        else
            {
            MPMLOGSTRING2(
                        "CMPMCommsDatAccess::CheckWlanL: WLAN table view failed, err %d", err2 )
            }                
        CleanupStack::PopAndDestroy( commsDbLanTableView );
        }

    // Release memory
    //
    CleanupStack::PopAndDestroy( buf1 );
    CleanupStack::PopAndDestroy( commsDbIapTableView );
    CleanupStack::PopAndDestroy( commsDatabase );

    return wlan;
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::RemoveCategorisedIapsL
// -----------------------------------------------------------------------------
//
void CMPMCommsDatAccess::RemoveCategorisedIapsL( RAvailableIAPList& aIAPList )
    {
    MPMLOGSTRING( "CMPMCommsDatAccess::RemoveCategorisedIapsL" )

    // Creating a session with the latest version
    // 
    CMDBSession* db = CMDBSession::NewLC( KCDVersion1_1 );

    db->SetAttributeMask( ECDHidden | ECDProtectedWrite );

    // Create the generic record set
    // 
    CMDBGenericRecord* generic = static_cast<CMDBGenericRecord*>
        ( CCDRecordBase::RecordFactoryL( 0 ) ); 
    CleanupStack::PushL( generic );  
    generic->InitializeL( TPtrC( KCDTypeNameDataMobilitySelectionPolicy ), 
                          NULL );
    generic->LoadL( *db );

    // Create the DN-IAP record set
    // 
    CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>* dnIAPRecordSet = 
        new ( ELeave ) CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>( 
        generic->TableId() );
    CleanupStack::PushL( dnIAPRecordSet ); 

    CCDDataMobilitySelectionPolicyRecord* dnIAPRecord = new ( ELeave ) 
        CCDDataMobilitySelectionPolicyRecord( generic->TableId() );
    CleanupStack::PushL( dnIAPRecord );

    // Find all the DN-IAP records
    // Trap leave in case no records exist
    // 
    TRAPD( err, dnIAPRecordSet->LoadL( *db ) )
    if( err != KErrNone )
        {
        // print error code to log
        //
        MPMLOGSTRING2( "CMPMCommsDatAccess::RemoveCategorisedIapsL, \
loading DM selection policy records failed with %d", err )
        CleanupStack::PopAndDestroy( dnIAPRecord );
        CleanupStack::PopAndDestroy( dnIAPRecordSet );
        CleanupStack::PopAndDestroy( generic );
        CleanupStack::PopAndDestroy( db );
        return;
        }

    CCDIAPRecord*           linkedIAP;
    CCDAccessPointRecord*   linkedAP;
    RArray<TInt>            prevRecordIds;
    CleanupClosePushL( prevRecordIds );

    // prepare for searching APs.

    for ( TInt i = 0; i < dnIAPRecordSet->iRecords.Count(); i++ )
        {
        linkedAP =  static_cast<CCDAccessPointRecord*>( CCDAccessPointRecord::RecordFactoryL( KCDTIdAccessPointRecord ) );
        CleanupStack::PushL( linkedAP );
        // Load each DN-IAP entry based on the elementId in the record set
        // 
        dnIAPRecord->SetElementId( GetRealElementId( dnIAPRecordSet->iRecords[i] ) );
        dnIAPRecord->LoadL( *db );

        // Find the SNAP AP pointed by DN-IAP entry
        //
        linkedAP->iRecordTag = (TInt)dnIAPRecord->iSNAP;
        TBool found = linkedAP->FindL( *db );

        if ( found && KErrNotFound == prevRecordIds.Find( linkedAP->RecordId() ) )
            {
            prevRecordIds.Append( linkedAP->RecordId() );
            
            // Print the DN and host name pointed by DN-IAP entry.
            // HostName is EText field.
            // 
            TBuf<KMaxTextLength + 1> dn = linkedAP->iRecordName.GetL();
            MPMLOGSTRING3( "    Snap: %d Snap Name = %s", (TInt)linkedAP->iRecordTag, dn.PtrZ() )
            
            // Related DN-IAP records 
            // Get the table id from generic record
            // 
            CMDBGenericRecord* generic2 = static_cast<CMDBGenericRecord*>
                ( CCDRecordBase::RecordFactoryL( 0 ) ); 
            CleanupStack::PushL( generic2 );  
            generic2->InitializeL( TPtrC( 
                KCDTypeNameDataMobilitySelectionPolicy ), NULL );
            generic2->LoadL( *db );
            
            // Doesn't quite work with generic records
            // 
            CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>* 
                dnIAPRecordSet2 = new ( ELeave ) 
                CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>( 
                generic2->TableId() );
            CleanupStack::PushL( dnIAPRecordSet2 ); 
            
            // Prime one record with DN 
            // 
            CCDDataMobilitySelectionPolicyRecord* dnIAPRecord2 = 
                new ( ELeave ) CCDDataMobilitySelectionPolicyRecord( generic2->TableId() );
            CleanupStack::PushL( dnIAPRecord2 );
            dnIAPRecord2->iSNAP = (TInt)linkedAP->iRecordTag;
            dnIAPRecordSet2->iRecords.Append( dnIAPRecord2 );
            CleanupStack::Pop( dnIAPRecord2 );

            if ( dnIAPRecordSet2->FindL( *db ) )
                {
                for (TInt j = 0; j < dnIAPRecordSet2->iRecords.Count(); j++)
                    {
                    dnIAPRecord2->SetElementId( GetRealElementId( dnIAPRecordSet2->iRecords[j] ) );
                    dnIAPRecord2->LoadL( *db );

                    if ( dnIAPRecord2->iIAP != 0 )
                        {
                        linkedIAP = static_cast<CCDIAPRecord*>
                            ( LoadLinkedRecordL( *db, dnIAPRecord2->iIAP ) );

                        // Remove categorised IAP from aIAPList
                        // 
                        TInt index = KErrNotFound;
                        index = aIAPList.Find( linkedIAP->RecordId() );
                        if ( index != KErrNotFound )
                            {
                            MPMLOGSTRING2(
                                "        Remove categorised IAP = %i", 
                                linkedIAP->RecordId() )
                            aIAPList.Remove( index );
                            }

                        delete linkedIAP;
                        dnIAPRecord2->iIAP = 0;
                        }
                    }
                }
            CleanupStack::PopAndDestroy( dnIAPRecordSet2 );
            CleanupStack::PopAndDestroy( generic2 );
            }
        CleanupStack::PopAndDestroy( linkedAP );
        }

    CleanupStack::PopAndDestroy( &prevRecordIds );
    CleanupStack::PopAndDestroy( dnIAPRecord );
    CleanupStack::PopAndDestroy( dnIAPRecordSet );
    CleanupStack::PopAndDestroy( generic );
    CleanupStack::PopAndDestroy( db );
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::GetIapServiceIdL
// -----------------------------------------------------------------------------
//
TUint32 CMPMCommsDatAccess::GetIapServiceIdL( TUint32 aIapId )
    {
    TUint32 serviceId( 0 );
    CMDBSession* db = CMDBSession::NewLC( KCDVersion1_1 );

    db->SetAttributeMask( ECDHidden | ECDProtectedWrite );

    // First check that IAP exists
    // 
    CCDIAPRecord* iap = LoadIapRecordLC( aIapId, db );
    serviceId = iap->iService;
    CleanupStack::PopAndDestroy( iap );
    CleanupStack::PopAndDestroy( db );

    MPMLOGSTRING3( "CMPMCommsDatAccess::GetiapServiceIdL - IAP Id: %d, service id: %d",     
        aIapId, serviceId )

    return serviceId;
    }

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::LoadIapRecordLC
// -----------------------------------------------------------------------------
//
CCDIAPRecord* CMPMCommsDatAccess::LoadIapRecordLC( const TUint32 aIapId, CMDBSession* aDb ) const
    {
    CCDIAPRecord* iap = static_cast<CCDIAPRecord*> ( 
        CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
    CleanupStack::PushL( iap );
    iap->SetRecordId( aIapId );
    iap->LoadL( *aDb );
    return iap;    
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::SearchAccessNetIdL
// -----------------------------------------------------------------------------
//
void CMPMCommsDatAccess::SearchAccessNetIdL( const TUint32&  aIapId,
                                            TUint32&        aNetId )
    {
    MPMLOGSTRING2( "CMPMCommsDatAccess::SearchAccessNetIdL - IAP Id: %d", 
        aIapId )

    CCommsDatabase* commsDatabase = CCommsDatabase::NewL();
    CleanupStack::PushL(commsDatabase);

    // Make hidden records visible
    // 
    commsDatabase->ShowHiddenRecords();

    // Open IAP table view by matching IAP Id
    //
    CCommsDbTableView* commsDbTableView = 
        commsDatabase->OpenViewMatchingUintLC( TPtrC( IAP ),
                                               TPtrC( COMMDB_ID ),
                                               aIapId );

    User::LeaveIfError( commsDbTableView->GotoFirstRecord() );

    commsDbTableView->ReadUintL(TPtrC( IAP_NETWORK ), aNetId );
    MPMLOGSTRING2(
        "CMPMCommsDatAccess::SearchAccessNetIdL - Found Access NetId: %d", 
        aNetId )

    CleanupStack::PopAndDestroy(commsDbTableView);
    CleanupStack::PopAndDestroy(commsDatabase);
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::WlanUnderVirtualL
// -----------------------------------------------------------------------------
//
TBool CMPMCommsDatAccess::WlanUnderVirtualL( const TUint32  aVirtualIapId, 
                                             RArray<TUint>& aIapPath )
    {
    MPMLOGSTRING2( "CMPMCommsDatAccess::WlanUnderVirtualL: VirtualIapId: %i", 
        aVirtualIapId )
    // Check that this Iap has not already traversed
    //
    if( aIapPath.Find( aVirtualIapId ) == KErrNotFound )
        {
        // append this Iap to path
        //
        aIapPath.Append( aVirtualIapId );
        }
    else
        {
        MPMLOGSTRING( "CMPMCommsDatAccess::WlanUnderVirtualL: Error, loop\
detected in configuration. Iap handling not continued" )
        return EFalse;
        }

    TBool wlan( EFalse );

    // Open session
    // 
    CMDBSession* db = CMDBSession::NewLC( KCDVersion1_1 );

    db->SetAttributeMask( ECDHidden | ECDProtectedWrite );

    CMDBGenericRecord * generic = static_cast<CMDBGenericRecord*>
        ( CCDRecordBase::RecordFactoryL( 0 ) );
    CleanupStack::PushL( generic );
    generic->InitializeL( TPtrC( KCDTypeNameVirtualIAPNextLayer ), 
                          NULL );
    generic->LoadL( *db );

    CCDVirtualIAPNextLayerRecord* virtualNext = new ( ELeave ) 
        CCDVirtualIAPNextLayerRecord( generic->TableId() );

    CleanupStack::PushL( virtualNext );
    virtualNext->iIAP = aVirtualIapId;
    if ( virtualNext->FindL( *db ) )
        {
        if ( virtualNext->iNextLayerSNAP != 0 )
            {
            // SNAP
            // 
            TInt snap = virtualNext->iNextLayerSNAP;
            MPMLOGSTRING2(
                "CMPMCommsDatAccess::WlanUnderVirtualL: SNAP: %i", snap )

            // Check if SNAP contains WLAN
            // 
            wlan = SnapContainsWlanL( snap, aIapPath, KMPMNrWlansOne );
            
            }
        else if ( virtualNext->iNextLayerIAP != 0 )
            {
            // IAP
            // 
            TInt iap = virtualNext->iNextLayerIAP;
            MPMLOGSTRING2(
                "CMPMCommsDatAccess::WlanUnderVirtualL: IAP: %i", iap )
            if( CheckWlanL( iap ) != ENotWlanIap )
                {
                wlan = ETrue;
                }
        
            // If virtual IAP, check whether it may be using WLAN
            // 
            if( !wlan && IsVirtualIapL( iap ) )
                {
                wlan = WlanUnderVirtualL( iap, aIapPath );
                }
            }
        else
            {
            MPMLOGSTRING( "CMPMCommsDatAccess::WlanUnderVirtualL: No underlying IAP\
 defined for virtual IapId" )
            }
        }
    // Remove this Iap from path 
    //
    MPMLOGSTRING2( "CMPMCommsDatAccess::WlanUnderVirtualL: removing %d from iap path", 
                aIapPath.Count() - 1 )
    aIapPath.Remove( aIapPath.Count() - 1 );

    // Release memory
    //
    CleanupStack::PopAndDestroy( virtualNext );
    CleanupStack::PopAndDestroy( generic );
    CleanupStack::PopAndDestroy( db );
    MPMLOGSTRING2(
                "CMPMCommsDatAccess::WlanUnderVirtualL: found: %i", wlan )
    return wlan;
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::SearchDNEntriesL
// -----------------------------------------------------------------------------
//
void CMPMCommsDatAccess::SearchDNEntriesL( const TUint32&   aSnapId,
                                           RArray<TNetIap>& aDestNetIds )
    {
    MPMLOGSTRING2( "CMPMCommsDatAccess::SearchDNEntriesL: SNAP: %d", 
        aSnapId )

    CCDAccessPointRecord* snapAPr =  static_cast<CCDAccessPointRecord*>( 
            CCDAccessPointRecord::RecordFactoryL( KCDTIdAccessPointRecord ) );
    
    snapAPr->iRecordTag = aSnapId;
    CleanupStack::PushL( snapAPr );

//    CCDNetworkRecord* dnRecord = static_cast<CCDNetworkRecord*>
//        ( CCDRecordBase::RecordFactoryL( KCDTIdNetworkRecord ) );
//        dnRecord->SetRecordId( aSnapId );

    CMDBSession* db = CMDBSession::NewLC( KCDVersion1_1 );

    db->SetAttributeMask( ECDHidden | ECDProtectedWrite );

//    TRAPD( err, snapAPr->LoadL( *db ) );
//    if ( err != KErrNone )
    if ( !snapAPr->FindL( *db ) )
        {
        MPMLOGSTRING("CMPMCommsDatAccess::SearchDNEntriesL: SNAP not found")
        CleanupStack::PopAndDestroy( db );
        CleanupStack::PopAndDestroy( snapAPr );
        return;
        }

    // Get the table id from generic
    // 
    CMDBGenericRecord* generic = static_cast<CMDBGenericRecord*>
        ( CCDRecordBase::RecordFactoryL( 0 ) );
    CleanupStack::PushL( generic );
    generic->InitializeL( TPtrC( KCDTypeNameDataMobilitySelectionPolicy ), 
                          NULL );
    generic->LoadL( *db );

    // Create the DN-IAP record set
    // 
    CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>* dnIAPRecordSet = 
        new ( ELeave ) CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>( generic->TableId() );
    CleanupStack::PushL( dnIAPRecordSet );

    CCDDataMobilitySelectionPolicyRecord* dnIAPRecord = new ( ELeave ) 
        CCDDataMobilitySelectionPolicyRecord( generic->TableId() );
    CleanupStack::PushL( dnIAPRecord );
    dnIAPRecord->iSNAP = (TInt)snapAPr->iRecordTag;
    dnIAPRecordSet->iRecords.Append( dnIAPRecord );
    CleanupStack::Pop( dnIAPRecord );

    if ( dnIAPRecordSet->FindL( *db ) )
        {
        for ( TInt i = 0; i < dnIAPRecordSet->iRecords.Count(); i++ )
            {
            TNetIap elem;
            dnIAPRecord->SetElementId( GetRealElementId( dnIAPRecordSet->iRecords[i] ) );
            dnIAPRecord->LoadL( *db );

            if ( dnIAPRecord->iIAP != 0 )
                {
                // Add values to elem
                // 
                TUint32 iapElementId( dnIAPRecord->iIAP ); 
                elem.iSnap      = aSnapId;
                elem.iIapId     = ( iapElementId & 
                                    KCDMaskShowRecordId ) >> KShift8;
                elem.iRanking   = static_cast<TUint32>( dnIAPRecord->iPriority );
                dnIAPRecord->iIAP = 0;
                }
            else if( dnIAPRecord->iEmbeddedSNAP != 0 )
                {
                // Add values to elem
                // 
                elem.iSnap          = aSnapId;
                elem.iEmbeddedSnap  = dnIAPRecord->iEmbeddedSNAP;
                elem.iRanking       = static_cast<TUint32>( dnIAPRecord->iPriority );
                dnIAPRecord->iEmbeddedSNAP = 0;
                }
            else
                {
                // Each Snap has an initial entry in this table,
                // where both the IAP and EmbeddedSNAP links 
                // have null value. 
                }

            // MPM should treat IAPs with Network Weight zero as the deleted 
            // ones. If an IAP seems to belong to a group but it has weight 
            // zero, it means that the IAP is not actually added to the group 
            // but an IAP from that group is deleted and a new one is created 
            // and this new IAP has got the same access network id that used 
            // to belong to the deleted IAP.
            // 
            if ( elem.iRanking > 0 )
                {
                // Insert elem into RArray
                // 
                aDestNetIds.InsertInOrderAllowRepeats(elem, TNetIap::CompareRanking);
                }
            else
                {
                if ( elem.iIapId != 0 )
                    {
                    MPMLOGSTRING2( "CMPMCommsDatAccess::SearchDNEntriesL: \
IapId %i removed because ranking is zero", elem.iIapId )
                    }
                if ( elem.iEmbeddedSnap != 0 )
                    {
                    MPMLOGSTRING2( "CMPMCommsDatAccess::SearchDNEntriesL: \
EmbeddedSnap %i removed because ranking is zero", elem.iEmbeddedSnap )
                    }
                }
            }
        }

    // Release memory
    //
    CleanupStack::PopAndDestroy( dnIAPRecordSet );
    CleanupStack::PopAndDestroy( generic );
    CleanupStack::PopAndDestroy( db );
    CleanupStack::PopAndDestroy( snapAPr );

#ifdef _DEBUG
    // Print info into the log file
    //
    for ( TInt j = 0; j < aDestNetIds.Count(); j++ )
        {
        MPMLOGSTRING2(
            "CMPMCommsDatAccess::SearchDNEntriesL: SNAP to array     = %i", 
            aDestNetIds[j].iSnap )
        MPMLOGSTRING2(
            "CMPMCommsDatAccess::SearchDNEntriesL: Embedded to array = %i", 
            aDestNetIds[j].iEmbeddedSnap )
        MPMLOGSTRING2(
            "CMPMCommsDatAccess::SearchDNEntriesL: IapId to array    = %i", 
            aDestNetIds[j].iIapId )
        MPMLOGSTRING2(
            "CMPMCommsDatAccess::SearchDNEntriesL: Ranking to array  = %i", 
            aDestNetIds[j].iRanking )
        }
#endif // _DEBUG
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::FindRealIapL
// -----------------------------------------------------------------------------
//
void CMPMCommsDatAccess::FindRealIapL( 
    const TUint32               aVirtualIapId,
    TUint32&                    aRealIapId, 
    const RAvailableIAPList&    aAvailableIAPList,
    CMPMServerSession&          aSession )
    {
    MPMLOGSTRING2( "CMPMCommsDatAccess::FindRealIapL: VirtualIapId: %i", 
        aVirtualIapId )

    // Open transaction
    // 
    CMDBSession* db = CMDBSession::NewLC( KCDVersion1_1 );

    db->SetAttributeMask( ECDHidden | ECDProtectedWrite );

    // Get IAP
    // 
    CCDIAPRecord* iap = LoadIapRecordLC( aVirtualIapId, db );

    TDesC& type = iap->iBearerType.GetL();

    if( type.Compare( TPtrC ( KCDTypeNameVirtualBearer ) ) != 0 )
        {
        MPMLOGSTRING( "CMPMCommsDatAccess::FindRealIapL: \
aVirtualIapId is not virtual, return as aRealIapId" )

        aRealIapId = aVirtualIapId;

        // Release memory
        //
        CleanupStack::PopAndDestroy( iap );
        CleanupStack::PopAndDestroy( db );
        return;
        }

    CMDBGenericRecord * generic = static_cast<CMDBGenericRecord*>
        ( CCDRecordBase::RecordFactoryL( 0 ) );
    CleanupStack::PushL( generic );
    generic->InitializeL( TPtrC( KCDTypeNameVirtualIAPNextLayer ), 
                          NULL );
    generic->LoadL( *db );

    CCDVirtualIAPNextLayerRecord* virtualNext = new ( ELeave ) 
        CCDVirtualIAPNextLayerRecord( generic->TableId() );

    CleanupStack::PushL( virtualNext );
    virtualNext->iIAP = iap->RecordId();
    if ( virtualNext->FindL( *db ) )
        {
        if ( virtualNext->iNextLayerSNAP != 0 )
            {
            // SNAP
            // 
            TInt snap = virtualNext->iNextLayerSNAP;
            MPMLOGSTRING2(
                "CMPMCommsDatAccess::FindRealIapL: SNAP: %i", snap )
            MPMLOGSTRING(
                "CMPMCommsDatAccess::FindRealIapL: Call ChooseBestIAPL" )

            TMpmConnPref tempMpmConnPref;
            tempMpmConnPref.SetSnapId( snap );

            // Ensure that there will be no infinite loops by dropping
            // this virtual iap out from the list of available iaps.
            // E.g. next layer SNAP could be the same as current SNAP.
            RAvailableIAPList tempList;
            CleanupClosePushL( tempList );
              
            for( TInt i=0; i < aAvailableIAPList.Count(); i++ )
                {
                // drop this virtual iap out from the list of
                // available iaps to prevent from infinitely looping
                //	
                if ( aAvailableIAPList[i] != aVirtualIapId )
                    {
                    tempList.Append( aAvailableIAPList[i] );
                    }
                }
            aSession.IapSelectionL()->ChooseBestIAPL( tempMpmConnPref, tempList );
            CleanupStack::PopAndDestroy( &tempList );

            aRealIapId = tempMpmConnPref.IapId();
            }
        else if ( virtualNext->iNextLayerIAP != 0 )
            {
            // IAP
            // 
            CCDIAPRecord* iap2 = LoadIapRecordLC( virtualNext->iNextLayerIAP, db );
            TDesC& type2 = iap2->iBearerType.GetL();
            if( type2.Compare( TPtrC ( KCDTypeNameVirtualBearer ) ) == 0 )
                {
                // Virtual IAP
                // 
                MPMLOGSTRING2(
                    "CMPMCommsDatAccess::FindRealIapL: Virtual IAP: %i", 
                    iap2->RecordId() )
                MPMLOGSTRING(
                    "CMPMCommsDatAccess::FindRealIapL: Call FindRealIapL" )

                if ( aVirtualIapId != virtualNext->iNextLayerIAP )
                    {
                    FindRealIapL( iap2->RecordId(), 
                                  aRealIapId, 
                                  aAvailableIAPList,
                                  aSession );
                    }
                else
                    {
                    aRealIapId = 0;
                    MPMLOGSTRING( "CMPMCommsDatAccess::FindRealIapL: Virtual iap points to itself" )
                    }
                }
            else
                {
                aRealIapId = iap2->RecordId();

                MPMLOGSTRING2(
                    "CMPMCommsDatAccess::FindRealIapL: NextLayerIAP: %i", 
                    aRealIapId )
                }
            // Release memory
            //
            CleanupStack::PopAndDestroy( iap2 );
            }
        else
            {
            MPMLOGSTRING( "CMPMCommsDatAccess::FindRealIapL: No underlying IAP\
 defined for virtual IapId" )
            }
        }

    // Release memory
    //
    CleanupStack::PopAndDestroy( virtualNext );
    CleanupStack::PopAndDestroy( generic );
    CleanupStack::PopAndDestroy( iap );
    CleanupStack::PopAndDestroy( db );
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::LoadLinkedRecordL
// -----------------------------------------------------------------------------
//
CCDRecordBase* CMPMCommsDatAccess::LoadLinkedRecordL( CMDBSession&  aSession, 
                                  TMDBElementId aElementId )
    {
    // This is copied from commsdatSnapTests.cpp. 
    // Loading linked records does not work currently.
    // 
    CCDRecordBase *record = static_cast<CCDRecordBase*>( 
        CCDRecordBase::RecordFactoryL( aElementId ) );
    CleanupStack::PushL( record );
    record->LoadL( aSession );
    CleanupStack::Pop( record );
    return record;
    }
    
// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::GetRealElementId
// -----------------------------------------------------------------------------
//
TUint32 CMPMCommsDatAccess::GetRealElementId( CMDBRecordBase* aRecord )
    {
    return aRecord->ElementId() & KCDMaskShowField;
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::WlanBackgroundScanIsOnL
// -----------------------------------------------------------------------------
//
TBool CMPMCommsDatAccess::WlanBackgroundScanIsOnL()
    {
    CCommsDatabase*    commDB = NULL;
    CCommsDbTableView* table  = NULL;

    // Open commDB
    commDB = CCommsDatabase::NewL();
    CleanupStack::PushL( commDB );

    // from connmondef.h
    _LIT( KCmWlanDeviceSettings,        "WLANDeviceTable" );
    _LIT( KCmWlanDeviceSettingsType,    "WlanDeviceSettingsType" );
    _LIT( KCmBgScanInterval,            "WlanBgScanInterval" );
    const TUint32 KCmWlanUserSettings  = 1;

    table = commDB->OpenViewMatchingUintLC( KCmWlanDeviceSettings,
                                            KCmWlanDeviceSettingsType,
                                            KCmWlanUserSettings );

    TInt err = table->GotoFirstRecord();

    if ( err )
        {
        User::Leave( err );  
        }
    
    TUint32 scanInterval;
    table->ReadUintL( KCmBgScanInterval, scanInterval ); 

    // cleanup
    CleanupStack::PopAndDestroy( table );
    CleanupStack::PopAndDestroy( commDB );
    
    return scanInterval != 0 ? ETrue : EFalse;
    
    }

// -----------------------------------------------------------------------------
// IsInternetSnapL
// -----------------------------------------------------------------------------
//
TBool CMPMCommsDatAccess::IsInternetSnapL( TUint32 aIapId, TUint32 aSnapId )
    {
    MPMLOGSTRING3( 
        "CMPMCommsDatAccess::IsInternetSnapL - aIapId = %d, aSnapId = %d", 
        aIapId, aSnapId )

    TBool ret = EFalse;
    
    // Open transaction
    CMDBSession* db = CMDBSession::NewLC( KCDVersion1_1 );

    if ( aSnapId )
        {
        // Check that snap purpose is internet
        ret = IsInternetSnapL( *db, aSnapId );
        }
    else if ( aIapId )
        {
        RArray<TUint> snapIds;
        
        // Get destinations
        FindAllSnapsL( snapIds );
        CleanupClosePushL( snapIds );
        
        for (TInt index = 0; index < snapIds.Count(); index++)
            {
            RArray<TNetIap> ids;
            // Get iaps belonging to this destination
            SearchDNEntriesL( snapIds[index], ids );
            CleanupClosePushL( ids );
            
            for (TInt index2 = 0; index2 < ids.Count(); index2++)
                {
                // Check if iap belongs to this destination
                if (ids[index2].iIapId == aIapId)
                    {
                    ret = IsInternetSnapL( *db, snapIds[index] );

                    if (ret)
                        {
                        break;
                        }
                    }
                }            
            
            CleanupStack::PopAndDestroy( &ids );
            
            if (ret)
                {
                break;
                }
            }
        
        CleanupStack::PopAndDestroy( &snapIds );
        }

    CleanupStack::PopAndDestroy( db );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// IsInternetSnapL
// -----------------------------------------------------------------------------
//
TBool CMPMCommsDatAccess::IsInternetSnapL(CMDBSession& db, TUint32 aSnapId)
    {
    MPMLOGSTRING( "CMPMCommsDatAccess::IsInternetSnapL" )        
    TBool ret = EFalse;
    
    CCDAccessPointRecord* snapAPr =  static_cast<CCDAccessPointRecord*>( 
            CCDAccessPointRecord::RecordFactoryL( KCDTIdAccessPointRecord ) );
    
    snapAPr->iRecordTag = aSnapId;
    CleanupStack::PushL( snapAPr );

    if ( !snapAPr->FindL( db ) )
        {
        MPMLOGSTRING("CMPMCommsDatAccess::IsInternetSnapL: SNAP not found")
        CleanupStack::PopAndDestroy( snapAPr );
        return ret;
        }

    // Create snap metadata record
    CMDBRecordSet<CCDSNAPMetadataRecord>* metaSet = 
           new ( ELeave ) CMDBRecordSet<CCDSNAPMetadataRecord>( 
            CCDSNAPMetadataRecord::TableIdL( db ) );
    CleanupStack::PushL( metaSet );

    // Load snap metedata record
    metaSet->LoadL( db );

    for ( TInt i = 0; i < metaSet->iRecords.Count(); ++i )
        {
        // Get value
        TInt meta = QUERY_INT_FIELD( metaSet->iRecords[i], 
                                     KCDTIdSNAPMetadataMetadata );
        
        TInt snap = QUERY_INT_FIELD( metaSet->iRecords[i],
                                     KCDTIdSNAPMetadataSNAP ); 
        // Check whether internet
        if( meta & ESnapMetadataInternet )
            {            
            MPMLOGSTRING("CMPMCommsDatAccess::IsInternetSnapL: Internet snap found")
            
            if ( snap == aSnapId )
                {
                MPMLOGSTRING2("CMPMCommsDatAccess::IsInternetSnapL: This is internet snap: %d",
                              aSnapId)
                ret = ETrue;
                break;
                }
            }
        }
        
    CleanupStack::PopAndDestroy( metaSet );
    CleanupStack::PopAndDestroy( snapAPr );
    return ret;
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::GetBearerTypeL
// -----------------------------------------------------------------------------
//
TMPMBearerType CMPMCommsDatAccess::GetBearerTypeL( TUint32 aIapId )
    {
    MPMLOGSTRING( "CMPMCommsDatAccess::GetBearerTypeL" )
    TMPMBearerType bearerType = EMPMBearerTypeOther;
    RArray<TUint32> wlanArray;
    
    CMDBSession* db = CMDBSession::NewLC( KCDVersion1_1 );
    
    CleanupClosePushL( wlanArray );
    BuildWlanArrayL(*db, wlanArray);
    
    CCDIAPRecord* record = LoadIapRecordLC( aIapId, db );
    
    TBuf<KMaxTextLength> bearerTypeName( record->iBearerType.GetL() );
    TBuf<KMaxTextLength> serviceTypeName( record->iServiceType.GetL() );
        
    if ( ( serviceTypeName == TPtrC( KCDTypeNameOutgoingWCDMA ) ) ||
         ( serviceTypeName == TPtrC( KCDTypeNameIncomingWCDMA ) ) )
        {
        // Packet data
        bearerType = EMPMBearerTypePacketData;
        }
    else if ( serviceTypeName == TPtrC( KCDTypeNameLANService ) )
        {
        // LAN or WLAN
        TUint32 bearerId( record->iBearer );
            
        if ( ( bearerTypeName == TPtrC( KCDTypeNameLANBearer ) ) &&
             ( IsWlanBearer( wlanArray, record->iBearer ) ) )
            {
            // WLAN
            bearerType = EMPMBearerTypeWlan;
            }
        }
    else if ( ( bearerTypeName == TPtrC( KCDTypeNameVirtualBearer ) ) &&
              ( serviceTypeName == TPtrC ( KCDTypeNameVPNService ) ) )
        {
        // VPN
        bearerType = EMPMBearerTypeVpn;
        }
    
    CleanupStack::PopAndDestroy( record );
    CleanupStack::PopAndDestroy( &wlanArray );
    CleanupStack::PopAndDestroy( db );
    
    return bearerType;
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::IsWlanBearerL
// -----------------------------------------------------------------------------
//
TBool CMPMCommsDatAccess::IsWlanBearer(RArray<TUint32>& aWlanArray, TInt aBearerId)
    {
    MPMLOGSTRING( "CMPMCommsDatAccess::IsWlanBearer" )

    TInt index = aWlanArray.Find( aBearerId );

    if ( index != KErrNotFound )
        {
        return ETrue;
        }
    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::BuildWlanArrayL
// -----------------------------------------------------------------------------
//
void CMPMCommsDatAccess::BuildWlanArrayL(CMDBSession& aDb, 
                                         RArray<TUint32>& aWlanArray)
    {
    MPMLOGSTRING( "CMPMCommsDatAccess::BuildWlanArrayL" )
    
    CMDBRecordSet<CCDLANBearerRecord>* ptrLanBearerRecordSet = new( ELeave )
            CMDBRecordSet<CCDLANBearerRecord>( KCDTIdLANBearerRecord );
    CleanupStack::PushL( ptrLanBearerRecordSet );
    ptrLanBearerRecordSet->LoadL( aDb );

    for ( TInt i = 0; i < ptrLanBearerRecordSet->iRecords.Count(); i++ )
        {
        TBuf<KMaxTextLength> bearerRecordName(
                ( (CCDLANBearerRecord*)ptrLanBearerRecordSet->iRecords[i] )->iRecordName.GetL() );

        // Check if bearer is WLAN
        if ( bearerRecordName == TPtrC( KModemBearerWLAN ) )
            {
            aWlanArray.AppendL( ptrLanBearerRecordSet->iRecords[i]->RecordId() );
            }
        }
    
    CleanupStack::PopAndDestroy( ptrLanBearerRecordSet );    
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::IsSnapEmptyL
// -----------------------------------------------------------------------------
//
TBool CMPMCommsDatAccess::IsSnapEmptyL( TUint32 aSnapId )
    {
    MPMLOGSTRING( "CMPMCommsDatAccess::IsSnapEmptyL" )
    TBool empty = ETrue;
    
    RArray<TNetIap> ids;
    // Get iaps belonging to this snap
    CleanupClosePushL( ids );
    SearchDNEntriesL( aSnapId, ids );
    
    if ( ids.Count() )
        {
        empty = EFalse;
        }
    
    CleanupStack::PopAndDestroy( &ids );    
    
    return empty;
    }
        
// -----------------------------------------------------------------------------
// VerifyIapExistsL
// -----------------------------------------------------------------------------
//
void CMPMCommsDatAccess::VerifyIapExistsL( TUint32 aIapId )
    {
    MPMLOGSTRING2( "CMPMCommsDatAccess::VerifyIapExistsL: aIapId %d", aIapId )
    CMDBSession* db = CMDBSession::NewLC( KCDVersion1_1 );
    db->SetAttributeMask( ECDProtectedWrite );
    CCDIAPRecord* iapRecord = LoadIapRecordLC( aIapId, db );
    CleanupStack::PopAndDestroy( iapRecord );
    CleanupStack::PopAndDestroy( db );
    }

// -----------------------------------------------------------------------------
// ForcedRoamingL
// -----------------------------------------------------------------------------
//
TCmUsageOfWlan CMPMCommsDatAccess::ForcedRoamingL() const
    {
    TCmGenConnSettings cmGenConnSettings;
    RCmManager rCmManager;
    
    rCmManager.CreateTablesAndOpenL();
    CleanupClosePushL( rCmManager ); 
    rCmManager.ReadGenConnSettingsL( cmGenConnSettings );
    CleanupStack::PopAndDestroy( &rCmManager );
    
    return cmGenConnSettings.iUsageOfWlan;
    }

// -----------------------------------------------------------------------------
// MapAPRecIdtoSnapAPL
// -----------------------------------------------------------------------------
//
TInt CMPMCommsDatAccess::TierLinktoTagIdL( CMDBSession* aDbs, TUint32 aElementId )
    {
    ASSERT(aDbs);
    
    // declare tier record
    CCDTierRecord* tier = static_cast<CCDTierRecord*>( 
            CCDTierRecord::RecordFactoryL( KCDTIdTierRecord ) );
    
    CleanupStack::PushL( tier );
    
    tier->SetElementId( aElementId );
    tier->LoadL( *aDbs );
    TInt tag = tier->iRecordTag;

    CleanupStack::PopAndDestroy( tier );
    return  tag;
    }

// -----------------------------------------------------------------------------
// MapAPRecIdtoSnapAPL
// -----------------------------------------------------------------------------
//
TInt CMPMCommsDatAccess::MapAPRecIdtoSnapAPL( TInt aApRecId )
    {
    CMDBSession* dbs = CMDBSession::NewLC( KCDVersion1_1 );
        
    CCDAccessPointRecord* ipt =  static_cast<CCDAccessPointRecord*>( 
            CCDAccessPointRecord::RecordFactoryL( KCDTIdAccessPointRecord ) );

    CleanupStack::PushL( ipt );

    ipt->SetElementId( aApRecId );
    
    TBool found = ipt->FindL( *dbs );
    if ( !found )
        User::Leave( KErrNotFound );
    
    if ( TierLinktoTagIdL( dbs, ipt->iTier ) == KAfInet )  // ipprototm = 271064560 , iptm = 271064536
        {
        User::Leave( KErrArgument );
        }

    TInt ap = ipt->iRecordTag;
    
    CleanupStack::PopAndDestroy( ipt );
    CleanupStack::PopAndDestroy( dbs );

    return ap;
    }

// -----------------------------------------------------------------------------
// MapNetIdtoSnapAPL
// -----------------------------------------------------------------------------
//
TInt CMPMCommsDatAccess::MapNetIdtoSnapAPL( TInt aNetId )
    {
    CMDBSession* dbs = CMDBSession::NewLC( KCDVersion1_1 );
        
    CCDAccessPointRecord* ipt =  static_cast<CCDAccessPointRecord*>( 
            CCDAccessPointRecord::RecordFactoryL( KCDTIdAccessPointRecord ) );

    CleanupStack::PushL( ipt );
    ipt->iCustomSelectionPolicy = aNetId;

    TBool found = ipt->FindL( *dbs );
    if ( !found )
        User::Leave( KErrNotFound );
    
    if ( TierLinktoTagIdL( dbs, ipt->iTier ) == KAfInet )  // ipprototm = 271064560 , iptm = 271064536
        {
        User::Leave( KErrArgument );
        }

    TInt ap = ipt->iRecordTag;
    
    CleanupStack::PopAndDestroy( ipt );
    CleanupStack::PopAndDestroy( dbs );

    return ap;
    }

// -----------------------------------------------------------------------------
// MapSnapAPtoNetIdL
// -----------------------------------------------------------------------------
//
TInt CMPMCommsDatAccess::MapSnapAPtoNetIdL( TInt aAP )
    {
    CMDBSession* dbs = CMDBSession::NewLC( KCDVersion1_1 );
        
    CCDAccessPointRecord* ipt =  static_cast<CCDAccessPointRecord*>( 
            CCDAccessPointRecord::RecordFactoryL( KCDTIdAccessPointRecord ) );
    CleanupStack::PushL( ipt );
    ipt->iRecordTag = aAP;
    TBool found = ipt->FindL( *dbs );
    if ( !found )
        User::Leave( KErrNotFound );
    
    if ( TierLinktoTagIdL( dbs, ipt->iTier ) == KAfInet )  // ipprototm = 271064560 , iptm = 271064536
        {
        User::Leave( KErrArgument );
        }

    TInt netid = ipt->iSelectionPolicy;
    
    CleanupStack::PopAndDestroy( ipt );
    CleanupStack::PopAndDestroy( dbs );

    return netid;
    }


// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::ReadGenConnSettings
// -----------------------------------------------------------------------------
//
TCmGenConnSettings CMPMCommsDatAccess::ReadGenConnSettingsL() const
    {
    TCmGenConnSettings cmGenConnSettings;
    RCmManager rCmManager;
    
    rCmManager.CreateTablesAndOpenL();
    CleanupClosePushL( rCmManager ); 
    rCmManager.ReadGenConnSettingsL( cmGenConnSettings );
    CleanupStack::PopAndDestroy( &rCmManager );
    
    return cmGenConnSettings;
    }

// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::WriteGenConnSettings
// -----------------------------------------------------------------------------
//
void CMPMCommsDatAccess::WriteGenConnSettingsL(TCmGenConnSettings& aGenConnSettings )
    {
    RCmManager rCmManager;
    rCmManager.CreateTablesAndOpenL();
    CleanupClosePushL( rCmManager ); 
    rCmManager.WriteGenConnSettingsL( aGenConnSettings );
    CleanupStack::PopAndDestroy( &rCmManager );
    }


// -----------------------------------------------------------------------------
// CMPMCommsDatAccess::DestinationIdL
// -----------------------------------------------------------------------------
//
TUint32 CMPMCommsDatAccess::DestinationIdL( CMManager::TSnapPurpose aSnapPurpose )
    {
    MPMLOGSTRING( "CMPMCommsDatAccess::DestinationIdL" )
    TUint32 destinationId = 0;
    TInt snapMetadata = 0;
    
    RArray<TUint32> snapIds;    
    CleanupClosePushL( snapIds );

    RCmManager rCmManager;
    CleanupClosePushL( rCmManager ); 
    rCmManager.CreateTablesAndOpenL();

    rCmManager.AllDestinationsL( snapIds );

    for ( TInt index = 0; index < snapIds.Count(); index++ )
         {
         RCmDestination dest;
         CleanupClosePushL( dest );

         dest = rCmManager.DestinationL( snapIds[index] );
         
         snapMetadata = dest.MetadataL( CMManager::ESnapMetadataPurpose );
         
         if ( snapMetadata == aSnapPurpose )
             {
             destinationId = snapIds[index];
             CleanupStack::PopAndDestroy( &dest );
             break;
             }        
         CleanupStack::PopAndDestroy( &dest );
         }
    
    CleanupStack::PopAndDestroy( &rCmManager );
    CleanupStack::PopAndDestroy( &snapIds );

    if (!destinationId)
        {
        User::Leave(KErrNotFound);
        }
    
    return destinationId;
    }

//  End of File
