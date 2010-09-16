/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CActiveIAPListing
*
*/


// INCLUDE FILES

#include "ActiveIAPListing.h"
#include "ConnectionInfo.h"
#include "ConnectionInfoKey.h"
#include "ConnectionInfoArray.h"
#include "ConnectionDialogsLogger.h"

#include <commdb.h>
#include <featmgr.h>
#include <StringLoader.h>
#include <centralrepository.h>
#include <WlanCdbCols.h>
#include <wlanmgmtcommon.h>
#ifndef __WINS__
#include <wlanmgmtclient.h>
#endif
#include <AknWaitDialog.h>
#include <AknUtils.h>
#include <centralrepository.h>
#include <ProfileEngineSDKCRKeys.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat.h>
#else
#include <commsdat.h>
#include <commsdat_partner.h>
#endif



// CONSTANTS

/**
* For iPeriodic Timer, 60 seconds
*/
LOCAL_D const TInt KTickDefaultInterval = 60000000;

/**
* Length of a string made of the MAX_INT and a tab character.
*/
LOCAL_D const TInt KLenMaxIntAndTab = 12;

_LIT( KMrouterName, "mRouter" );

// Panic string
#if defined(_DEBUG)
_LIT( KErrNullPointer, "NULL pointer" );
#endif


// TYPES

/**
* Indexes of the icons
*/
enum 
    {
    EIconIndexGPRS,         // Icon for GPRS access point
    EIconIndexCSD,          // Icon for CSD access point
    EIconIndexHSCSD,        // Icon for HSCSD access point
    EIconIndexEasyWLAN,     // Icon for Easy WLan access point
    EIconIndexWLAN          // Icon for WLan access point
    };




///////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------
// CActiveIAPListing::CActiveIAPListing()
// ---------------------------------------------------------
//
CActiveIAPListing::CActiveIAPListing( const TConnectionPrefs& aPrefs )
: CActive( EPriorityUserInput ), 
  iDb( NULL ), 
  iPeriodic( NULL ),
  iWaitDialog( NULL ), 
#ifndef __WINS__
  iWlanMgmt( NULL ),
#endif // !__WINS__
  iIAPs( NULL ),
  iWlanIAPs( NULL ),
  iActIAPs( NULL ),
  iClientStatus( NULL ),
  iIsWLANFeatureSupported( EFalse ),
  iPrefs( aPrefs )
    {
    }

    
// ---------------------------------------------------------
// CActiveIAPListing::~CActiveIAPListing()
// ---------------------------------------------------------
//    
CActiveIAPListing::~CActiveIAPListing()
    {
    CLOG_ENTERFN( "~CActiveIAPListing " );
    Cancel();
    
    delete iDb;
    iDb = NULL;
    
    iAvailableIaps.Close();  
    
    if ( iIAPs )
        {
        iIAPs->ResetAndDestroy();
        }
    delete iIAPs;    
    iIAPs = NULL;
       
    if ( iWlanIAPs )
        {
        iWlanIAPs->ResetAndDestroy();
        }    
    delete iWlanIAPs;
    iWlanIAPs = NULL;

    delete iActIAPs;    // do not destroy, gets items from iIAPs and iWlanIAPs
    iActIAPs = NULL;
    
    CLOG_LEAVEFN( "~CActiveIAPListing " );
    }


// ---------------------------------------------------------
// CActiveIAPListing::ConstructL()
// ---------------------------------------------------------
//
void CActiveIAPListing::BaseConstructL()
    {     
    CLOG_ENTERFN( "CActiveIAPListing::ConstructL " );  
       
    CActiveScheduler::Add( this );            
    
    FeatureManager::InitializeLibL();
    
    if ( iPrefs.iBearerSet & ECommDbBearerWLAN )
        {
        iIsWLANFeatureSupported = 
            FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );    
        }
    else
        {
        iIsWLANFeatureSupported = EFalse;
        }    
    
    FeatureManager::UnInitializeLib();      

    if ( iIsWLANFeatureSupported )
        {
#ifndef __WINS__
        iWlanMgmt = CWlanMgmtClient::NewL();
#endif // !__WINS__
        }
                  
    iPeriodic = CPeriodic::NewL( CActive::EPriorityStandard ); 
    iDb = CCommsDatabase::NewL();
    
    iActIAPs = new( ELeave ) CConnectionInfoArray();
    
    CLOG_LEAVEFN( "CActiveIAPListing::ConstructL " );
    }


// ---------------------------------------------------------
// CActiveIAPListing::Cancel()
// ---------------------------------------------------------
// 
void CActiveIAPListing::Cancel()
    {
    CLOG_ENTERFN( "CActiveIAPListing::Cancel " );
    
    StopTimer();

    delete iWaitDialog;
    iWaitDialog = NULL;
                                    
#ifndef __WINS__
    delete iWlanMgmt;
    iWlanMgmt = NULL;     
#endif // !__WINS__
    
    CLOG_WRITEF( _L( "Cancel iStatus.Int: %d" ), iStatus.Int() );
    CActive::Cancel();
    
    CLOG_LEAVEFN( "CActiveIAPListing::Cancel " );
    }


// ---------------------------------------------------------
// CActiveIAPListing::DoCancel()
// ---------------------------------------------------------
// 
void CActiveIAPListing::DoCancel()
    {
    }

    
// ---------------------------------------------------------
// CActiveIAPListing::RunL()
// ---------------------------------------------------------
//     
void CActiveIAPListing::RunL()
    {
    }


// ---------------------------------------------------------
// CActiveIAPListing::StartTimerL
// ---------------------------------------------------------
//    
void CActiveIAPListing::StartTimerL( TInt aTickInterval )
    {    
    CLOG_ENTERFN( "CActiveIAPListing::StartTimerL " );
    
    if( !iPeriodic )
        {
        iPeriodic = CPeriodic::NewL( CActive::EPriorityStandard ); 
        }
        
    iPeriodic->Start( aTickInterval, aTickInterval, TCallBack( Tick, this ) );
    
    CLOG_LEAVEFN( "CActiveIAPListing::StartTimerL " );
    }


// ---------------------------------------------------------
// CActiveIAPListing::StopTimer
// ---------------------------------------------------------
//
void CActiveIAPListing::StopTimer()
    {
    CLOG_ENTERFN( "CActiveIAPListing::StopTimer " );  
    
    if( iPeriodic )
        {
        CLOG_WRITE( "iPeriodic" );
        iPeriodic->Cancel();
        CLOG_WRITE( "Cancel" );
        
        delete iPeriodic;
        CLOG_WRITE( "delete" );
        
        iPeriodic = NULL;
        }        
    
    CLOG_LEAVEFN( "CActiveIAPListing::StopTimer " );
    }

    
// ---------------------------------------------------------
// CActiveIAPListing::Tick
// ---------------------------------------------------------
//
TInt CActiveIAPListing::Tick( TAny* aObject )
    {
    CLOG_ENTERFN( "CActiveIAPListing::Tick " );  
    
    CActiveIAPListing* myself = 
                            static_cast<CActiveIAPListing*>( aObject );

    if ( myself->iStatus == KErrCancel )
        {
        CLOG_WRITE( "iStatus == KErrCancel" );        
        myself->StopTimer();            
        }
    else
        {
        CLOG_WRITE( "iStatus != KErrCancel" );    
                                                              
        myself->SetActive();
        myself->iClientStatus = &( myself->iStatus );  
        *( myself->iClientStatus ) = KRequestPending;  

#ifndef __WINS__
        if ( myself->iIsWLANFeatureSupported )
            {
            myself->iWlanMgmt->GetAvailableIaps( *( myself->iClientStatus ), 
                                                 myself->iAvailableIaps );
            }
        else
            {
            User::RequestComplete( myself->iClientStatus, KErrNone );         
            }
#else                                                     
        User::RequestComplete( myself->iClientStatus, KErrNone );         
#endif  // !__WINS__
                                               
        }
    
    CLOG_LEAVEFN( "CActiveIAPListing::Tick " );      
    
    return 1;
    }        

 

// ---------------------------------------------------------
// CActiveIAPListing::CreateArraysL()
// ---------------------------------------------------------
//     
void CActiveIAPListing::CreateArraysL( TInt aResourceId, 
                                       TBool aChangeConnection,
                                       TUint32 aIAPId )
    {
    iIAPs = new( ELeave ) CConnectionInfoArray();
    iWlanIAPs = new( ELeave ) CConnectionInfoArray();
    BuildConnectionListL( aResourceId, aChangeConnection, aIAPId);
    SetActive();
    iClientStatus = &iStatus;
    *iClientStatus = KRequestPending;  
        
#ifndef __WINS__
    if ( iIsWLANFeatureSupported )
        {
        iWlanMgmt->GetAvailableIaps( *iClientStatus, iAvailableIaps );
        }  
    else
#endif // !__WINS__
        {
        // send to runl
        User::RequestComplete( iClientStatus, KErrNone );  // send back to runl
        }                 
    }


// ---------------------------------------------------------
// CActiveIAPListing::StartSearchIAPsL()
// ---------------------------------------------------------
//
void CActiveIAPListing::BaseStartSearchIAPsL( TInt aResourceId )
    {    
    CLOG_ENTERFN( "CActiveIAPListing::StartSearchIAPsL " );
    
    if ( iIsWLANFeatureSupported )
        {    
        delete iWaitDialog;
        iWaitDialog = NULL;                

        iWaitDialog = new( ELeave )CAknWaitDialog( REINTERPRET_CAST( 
                                        CEikDialog**, &iWaitDialog ), ETrue );
        iWaitDialog->ExecuteLD( aResourceId ); 
        }
    
    SetActive();    
    iClientStatus = &iStatus;
    
    User::RequestComplete( iClientStatus, KErrNone );   // send to runl

    CLOG_LEAVEFN( "CActiveIAPListing::StartSearchIAPsL " );
    }    

    
// ---------------------------------------------------------
// CActiveIAPListing::GetRefreshInterval()
// ---------------------------------------------------------
//    
TInt CActiveIAPListing::GetRefreshInterval( const TUid aRepositoryUid,
                                            TUint32 aKey )
    {
    CLOG_ENTERFN( "CActiveIAPListing::GetRefreshInterval" );

    CRepository* repository = NULL;
    TInt err( KErrNone );

    TRAP( err, 
          repository = CRepository::NewL( aRepositoryUid ) );
          
    TInt variant( 0 );  
    CLOG_WRITEF( _L( "err : %d" ), err );
      
    if ( err == KErrNone )
        {
        TInt retval = repository->Get( aKey, variant );
                                       
        CLOG_WRITEF( _L( "retval : %d" ), retval );
                                                                             
        if ( retval == KErrNotFound )
            {
            variant = KTickDefaultInterval;
            }
        }
     else
        {
        variant = KTickDefaultInterval;
        }

    delete repository;

    CLOG_WRITEF( _L( "variant : %d" ), variant );
    
    CLOG_LEAVEFN( "CActiveIAPListing::GetRefreshInterval" );
    
    return variant;            
    }
            
    
// ----------------------------------------------------------------------------
// void CActiveIAPListing::MergeArraysL()
// ----------------------------------------------------------------------------
//
void CActiveIAPListing::MergeArraysL( TInt aResourceId )
    {
    CLOG_ENTERFN( "CActiveIAPListing::MergeArraysL " );

    iActIAPs->Reset();

    TInt countIAPs = iIAPs->Count();
    TInt i;

    for( i = 0; i < countIAPs; ++i )
        {
        iActIAPs->AppendL( iIAPs->At( i ) );
        }
    
    if ( iIsWLANFeatureSupported )
        {
        TInt countWlanIAPs = iWlanIAPs->Count();
        CLOG_WRITEF( _L( "iWlanIAPs count: %d" ), countWlanIAPs );

#ifdef __WINS__

        for( i = 0; i < countWlanIAPs; ++i )
            {
            iActIAPs->AppendL( iWlanIAPs->At( i ) );
            }

#else

        TInt countAvailIAPs = iAvailableIaps.Count();
        CLOG_WRITEF( _L( "iAvailableIaps count: %d" ), countAvailIAPs );

        CConnectionInfo* conInf = NULL;
        for( int j = 0; j < countAvailIAPs; ++j )
            {
            TUint32 actIap = TUint32( iAvailableIaps[j] );
            for( i = 0; i < countWlanIAPs; ++i )
                {     // Scan all the WlanIAPs, because there could be VPNs 
                      // that use the same network
                conInf = iWlanIAPs->At( i );
                if ( ( conInf->IsVPN() && conInf->VPNIapId() == actIap ) ||
                     ( !conInf->IsVPN() && conInf->Id() == actIap ) )
                    {
                    iActIAPs->AppendL( conInf );
                    }
                }
            }    
#endif // __WINS__
        }    

    SortNameListL( aResourceId );
        
    CLOG_LEAVEFN( "CActiveIAPListing::MergeArraysL " );
    }


// ---------------------------------------------------------
// CActiveIAPListing::SortNameListL
// ---------------------------------------------------------
//
void CActiveIAPListing::SortNameListL( TInt aResourceId )
    {
    CLOG_ENTERFN( "CActiveIAPListing::SortNameListL " );      
    
    CConnectionInfoKey* infoKey;

    HBufC *nameEasyWLAN = NULL;

    if ( iIsWLANFeatureSupported )
        {
        nameEasyWLAN = StringLoader::LoadL( aResourceId );
        CleanupStack::PushL( nameEasyWLAN );

        infoKey = CConnectionInfoKey::NewL( *nameEasyWLAN );
        }
    else
        {
        infoKey = CConnectionInfoKey::NewL();
        }

    infoKey->SetPtr( iActIAPs );

    CleanupStack::PushL( infoKey );
    
    // Sort returns KErrGeneral if stack overflow, otherwise, returns
    // KErrNone. So we will Leave only if stack overflow,
    // but than that really does not matter...
    User::LeaveIfError( iActIAPs->Sort( *infoKey ) );

    CleanupStack::Pop( infoKey );
    delete infoKey;

    if ( iIsWLANFeatureSupported )
        {
        CleanupStack::PopAndDestroy( nameEasyWLAN );
        }

    CLOG_LEAVEFN( "CActiveIAPListing::SortNameListL " );      
    }



// ---------------------------------------------------------
// CActiveIAPListing::BuildConnectionListL()
// ---------------------------------------------------------
//
void CActiveIAPListing::BuildConnectionListL( TInt aResourceId,
                                              TBool aChangeConnection, 
                                              TUint32 aIAPId )
    {
    CLOG_ENTERFN( "CActiveIAPListing::BuildConnectionListL " );  
    
    __ASSERT_DEBUG( iDb, User::Panic( KErrNullPointer, KErrNone ) );

    CCommsDbTableView* table;

    TUint32 id;
    TUint32 serviceId;
    TLanType lanType = ELan;   
    TBool isInfrastructure = ETrue;     
    
    TBuf<CommsDat::KMaxTextLength+1> name;     // +1 because it "Zero 
                                                // terminates" the string
    TBuf<CommsDat::KMaxTextLength+1> nameIAP;  // +1, as before
    TBuf<CommsDat::KMaxTextLength> serviceType;

    CCommsDbTableView* serviceTable;
    
    CLOG_WRITEF( _L( "iPrefs.iBearerSet: %x" ), iPrefs.iBearerSet );    
    CLOG_WRITEF( _L( "iPrefs.iDirection: %d" ), iPrefs.iDirection );     
    
    CRepository* cr = CRepository::NewLC( KCRUidProfileEngine );

	TInt selectedProfile( 0 );

	(void) cr->Get( KProEngActiveProfile, selectedProfile );

	// To get also VPN APs
    TUint bearerSet = iPrefs.iBearerSet | ECommDbBearerVirtual;
	
	TBool isOffLine = selectedProfile == 5;
	
	if ( isOffLine ) // testing if current profile is the offline profile
		{
    	TUint mask=0;
    	mask = ~mask ^ ECommDbBearerCSD ^ ECommDbBearerGPRS;
    	bearerSet &= mask;
    	}
    	
    CleanupStack::PopAndDestroy(cr); 	//cr	
    
#ifdef __WINS__
    bearerSet |= ECommDbBearerLAN;     // To add Ethernet AP and WLan (in case)
#else
    if ( iIsWLANFeatureSupported )
        {
        bearerSet |= ECommDbBearerLAN; // To add WLan AP
        }
#endif
    if ( ( bearerSet & ECommDbBearerLAN ) && 
         ( bearerSet & ECommDbBearerWLAN ) )
        {   // Otherwise OpenIAPTableViewMatchingBearerSetLC returns two
            // records for each WLan AP
        bearerSet ^= ECommDbBearerWLAN;     
        }

    CLOG_WRITEF( _L( "bearerSet1: %x" ), bearerSet  );        
        
    if ( ( bearerSet & ECommDbBearerLAN ) && 
         ( bearerSet & ECommDbBearerPAN ) )
        {   // Otherwise OpenIAPTableViewMatchingBearerSetLC returns two
            // records for each WLan AP
        bearerSet ^= ECommDbBearerPAN;     
        }
    CLOG_WRITEF( _L( "bearerSet2: %x" ), bearerSet  );                
        
    if ( ( bearerSet & ECommDbBearerPAN ) && 
         ( bearerSet & ECommDbBearerWLAN ) )
        {   // Otherwise OpenIAPTableViewMatchingBearerSetLC returns two
            // records for each WLan AP
        bearerSet ^= ECommDbBearerPAN;     
        }
    CLOG_WRITEF( _L( "bearerSet3: %x" ), bearerSet  );      

    table = iDb->OpenIAPTableViewMatchingBearerSetLC( bearerSet, 
                                                      iPrefs.iDirection );

    TInt errorCode = table->GotoFirstRecord();
    TBool easyWLanRecordFound = EFalse;
    TInt iconNum = EIconIndexGPRS;
    TBool showAP;
    TUint32 vpn_iap_id;
    
    TBuf<CommsDat::KMaxTextLength+KLenMaxIntAndTab> temp;
    _LIT( KIconIdText, "%d\t%s" );
    
    while ( errorCode == KErrNone )
        {
        lanType = ENotDefined;
        isInfrastructure = ETrue;
		
		// Get type of IAP service
        table->ReadTextL( TPtrC( IAP_SERVICE_TYPE ), serviceType );
        TBool isVPN = serviceType == TPtrC( VPN_SERVICE );

        // Get name for IAP
        table->ReadTextL( TPtrC( COMMDB_NAME ), name );
        name.ZeroTerminate();

        // Get id for IAP
        table->ReadUintL( TPtrC( COMMDB_ID ), id );

        // Get IAP service
        table->ReadUintL( TPtrC( IAP_SERVICE ), serviceId );

        CLOG_WRITEF( _L( "IAP name: %S" ), &name );
        CLOG_WRITEF( _L( "COMMDB_ID: %d" ), id );
        CLOG_WRITEF( _L( "ServiceId: %d" ), serviceId );

        if ( isVPN )
            {
            // Get Ip/phone number from IAP service table
            serviceTable = iDb->OpenViewMatchingUintLC( serviceType, 
                                                        TPtrC( COMMDB_ID ),
                                                        serviceId );
            errorCode = serviceTable->GotoFirstRecord();
            if ( errorCode == KErrNone )
                {
                TRAPD (err, serviceTable->ReadUintL( TPtrC( VPN_SERVICE_IAP ), 
                                                     vpn_iap_id ));
                                         
                if ( err != KErrNone )
                 { 
            	    CLOG_WRITEF( _L( "Put showAP as false: %d" ), err );
                  showAP = EFalse;
                  }

                CCommsDbTableView* serviceTableIAP = 
                            iDb->OpenViewMatchingUintLC( TPtrC( IAP ),
                                                         TPtrC( COMMDB_ID ),
                                                         vpn_iap_id );

                errorCode = serviceTableIAP->GotoFirstRecord();
                if ( errorCode == KErrNone )
                    {
                    // Get name for IAP
                    serviceTableIAP->ReadTextL( TPtrC( COMMDB_NAME ), 
                                                nameIAP );
                    nameIAP.ZeroTerminate();

                    serviceTableIAP->ReadTextL( TPtrC( IAP_SERVICE_TYPE ), 
                                                serviceType );
                    serviceTableIAP->ReadUintL( TPtrC( IAP_SERVICE ), 
                                                serviceId );
                    
                    TUint32 bearermask;                            
                    serviceTableIAP->ReadUintL(TPtrC( IAP_BEARER ), bearermask );
                    
                    //this is for dealing with vpns using gprs and csd iaps while being offline
                    if( ( ( bearermask & ECommDbBearerCSD ) || 
                    	  ( bearermask & ECommDbBearerGPRS ) )
                    	&& isOffLine )
                    	{
                    	//do not add it and jump to the next record
                    	CleanupStack::PopAndDestroy( serviceTableIAP );
                		CleanupStack::PopAndDestroy( serviceTable );
                    	errorCode = table->GotoNextRecord();
                    	continue;
                    	}
                    
                    }
                CleanupStack::PopAndDestroy( serviceTableIAP );
                }
            CleanupStack::PopAndDestroy( serviceTable );
            }
        else
            {
            nameIAP = name;
            }

        lanType = ( serviceType == TPtrC( LAN_SERVICE ) /*|| isVPN*/ ) ? 
                    GetTypeOfLanL( serviceId, isInfrastructure ) : ENotDefined;

        if( !aChangeConnection || ( id != aIAPId ) || 
            ( lanType == EEasyWLan && isVPN ) || 
            ( lanType == EEasyWLan && !easyWLanRecordFound ) )
            {
            // Filter out mRouter access points
            if ( errorCode == KErrNone && 
                 nameIAP.Left( KMrouterName().Length() ).CompareF( 
                               KMrouterName ) != 0 )
                {
                showAP = ETrue;

                if ( serviceType == TPtrC( OUTGOING_GPRS ) ||
                     serviceType == TPtrC( INCOMING_GPRS ) )
                    {
                    iconNum = EIconIndexGPRS;
                    if ( isVPN && 
                         !( iPrefs.iBearerSet & ECommDbBearerVirtual ) && 
                         !( iPrefs.iBearerSet & ECommDbBearerWcdma ) )
                        {
                        showAP = EFalse;
                        }
                    }
                else if ( serviceType == TPtrC( LAN_SERVICE )  )
                    {
                    if ( ( isVPN && 
                           !( iPrefs.iBearerSet & ECommDbBearerVirtual ) ) ||
                         !isVPN )
                        {
                        if ( lanType != ELan && !iIsWLANFeatureSupported )
                            {
                            showAP = EFalse;
                            }
                        

#ifndef __WINS__
                        else if ( lanType == ELan && 
                                  !( iPrefs.iBearerSet & ECommDbBearerLAN ) )
                            {
                            showAP = EFalse;
                            }
#endif  // __WINS__
                        }

                    // There is not an icon specific for LAN AP:
                    // GPRS icon is used
                    iconNum = EIconIndexGPRS;
                    if ( showAP && iIsWLANFeatureSupported && lanType != ELan )
                        {
                        if ( lanType == EEasyWLan && 
                             !easyWLanRecordFound && 
                             !isVPN )
                            {
                            HBufC *msg = StringLoader::LoadL( aResourceId );
                            CleanupStack::PushL( msg );

                            name = *msg;
                            name.ZeroTerminate();

                            CleanupStack::PopAndDestroy( msg );

                            iconNum = EIconIndexEasyWLAN;

                            easyWLanRecordFound = ETrue;
                            }
                        else if ( lanType == EEasyWLan && isVPN )
                            {
                            iconNum = EIconIndexEasyWLAN;
                            }
                        else
                            {    
                            iconNum = EIconIndexWLAN;
                            lanType = EWLan;
                            }
                        }
                    }
                else
                    {
                    if ( isVPN && 
                         !( iPrefs.iBearerSet & ECommDbBearerVirtual ) && 
                         !( iPrefs.iBearerSet & ECommDbBearerCSD ) )
                        {
                        showAP = EFalse;
                        }
                    else
                        {
                        // Get Ip/phone number from IAP service table
                        serviceTable = iDb->OpenViewMatchingUintLC( 
                                                            serviceType, 
                                                            TPtrC( COMMDB_ID ),
                                                            serviceId );

                        errorCode = serviceTable->GotoFirstRecord();
                        if ( errorCode == KErrNone )
                            {
                            TUint32 bearer = EBearerTypeHSCSD;
            
                            TRAPD( err, serviceTable->ReadUintL( 
                                                    TPtrC( ISP_BEARER_TYPE ),
                                                    bearer ) );
                                                       
                            if ( err != KErrNone )
                                { // don't leave if read value is not in table.
                                if ( err == KErrUnknown )
                                    {
                                    bearer = EBearerTypeCSD;
                                    }
                                else
                                    {
                                    showAP = EFalse;
                                    //User::Leave( err );
                                    }
                                }

                            iconNum = ( bearer == EBearerTypeHSCSD ) ? 
                                      EIconIndexHSCSD : EIconIndexCSD;
                            }
                        else
                            {
                            showAP = EFalse;
                            }

                        CleanupStack::PopAndDestroy( serviceTable );
                        }
                    }

                if ( showAP )
                    {
                    AknTextUtils::LanguageSpecificNumberConversion( name );

                    temp.FillZ( CommsDat::KMaxTextLength+KLenMaxIntAndTab );
                    temp.Format( KIconIdText, iconNum, name.Ptr() );
                
                    CConnectionInfo* conIn;

                    if ( isVPN )
                        {
                        conIn = CConnectionInfo::NewL( name, id, vpn_iap_id,
                                                       temp );
                        }
                    else
                        {
                        conIn = CConnectionInfo::NewL( name, id, temp );
                        }

                    CLOG_WRITEF( _L( "id: %d" ), id );
                    CLOG_WRITEF( _L( "lanType: %d" ), ( TInt )lanType );
                    CLOG_WRITEF( _L( "isInfrastructure: %d" ), ( TInt )isInfrastructure );
                
                
                    CleanupStack::PushL( conIn );
                                                                
                    if ( lanType == EWLan && isInfrastructure )
                        {
                        iWlanIAPs->AppendL( conIn );                    
                        }
                    else
                        {
                        iIAPs->AppendL( conIn );
                        }
                    
                    CleanupStack::Pop( conIn );
                    }
                }
            }

        // Next IAP
        errorCode = table->GotoNextRecord();
        }

    CleanupStack::PopAndDestroy( table );
    
    CLOG_WRITEF( _L( "iIAPs count: %d" ), iIAPs->Count() );
    CLOG_WRITEF( _L( "iWlanIAPs count: %d" ), iWlanIAPs->Count() );
    
    CLOG_LEAVEFN( "CActiveIAPListing::BuildConnectionListL " );
    }

    
// ----------------------------------------------------------------------------
// TLanType CActiveIAPListing::GetTypeOfLanL()
// ----------------------------------------------------------------------------
//
TLanType CActiveIAPListing::GetTypeOfLanL( TUint32 aServiceId, 
                                           TBool& aIsInfrastructure )
    {
    CLOG_ENTERFN( "CActiveIAPListing::GetTypeOfLanL " );      
    
    TLanType lanType = ELan;
    
    aIsInfrastructure = ETrue;
    if ( iIsWLANFeatureSupported )
        {
        __ASSERT_DEBUG( iDb, User::Panic( KErrNullPointer, KErrNone ) );

        CCommsDbTableView* wLanServiceTable = NULL;
            
        TRAPD( err, 
            { // this leaves if the table is empty....
            wLanServiceTable = iDb->OpenViewMatchingUintLC( 
                                                    TPtrC( WLAN_SERVICE ),
                                                    TPtrC( WLAN_SERVICE_ID ),
                                                    aServiceId );
            CleanupStack::Pop( wLanServiceTable ); // wLanServiceTable
            } );

        CLOG_WRITEF( _L( "OpenViewMatchingUintLC returned %d" ), err );
        CLOG_WRITEF( _L( "for ServiceId: %d" ), aServiceId );

        if ( err == KErrNone )
            {
            CleanupStack::PushL( wLanServiceTable );

            TInt errorCode = wLanServiceTable->GotoFirstRecord();

            CLOG_WRITEF( _L( "GotoFirstRecord returned %d" ), 
                                errorCode );
            TWlanSsid sSID;                                

            if ( errorCode == KErrNone )
                {
                wLanServiceTable->ReadTextL( TPtrC( NU_WLAN_SSID ), sSID );
                lanType = sSID.Length() ? EWLan : EEasyWLan;
                
                TUint32 connMode;
                TRAP( err, wLanServiceTable->ReadUintL( 
                                                TPtrC( WLAN_CONNECTION_MODE ),
                                                connMode ) );
                if ( err == KErrNone && !connMode )
                    {
                    aIsInfrastructure = EFalse;
                    }
                }

            CleanupStack::PopAndDestroy( wLanServiceTable );
            }
        else if ( err != KErrNotFound )
            {
            User::LeaveIfError( err );
            }
        }
    
    CLOG_LEAVEFN( "CActiveIAPListing::GetTypeOfLanL " );      
    
    return lanType;
    }


// End of File
