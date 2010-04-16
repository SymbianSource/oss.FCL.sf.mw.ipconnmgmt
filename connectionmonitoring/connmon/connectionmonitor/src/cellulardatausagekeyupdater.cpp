/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Keeps KCurrentCellularDataUsage -key 
*               in Repository KCRUidCmManager up-to-date based
*               on current network registration status and
*               iCellularDataUsageHome and iCellularDataUsageVisitor keys.  
*/

#include <commsdat.h>
#include <centralrepository.h>
#include <cmmanager.h>
#include <cmmanagerkeys.h>
#include <cmgenconnsettings.h>
#include <rconnmon.h>
#include <datamobilitycommsdattypes.h>
#include <cmpluginbaseeng.h>

#include "cellulardatausagekeyupdater.h"
#include "ConnMonServ.h"
#include "ConnMonAvailabilityManager.h"
#include "ConnMonIAP.h"
#include "ConnMonDef.h"
#include "log.h"

// Repository for CommsDat
const TUid KCDCommsRepositoryId = { 0xCCCCCC00 };

// -----------------------------------------------------------------------------
// CCellularDataUsageKeyUpdater::NewL
// -----------------------------------------------------------------------------
//
CCellularDataUsageKeyUpdater* CCellularDataUsageKeyUpdater::NewL( 
	    CConnMonServer* aServer )
    {
    CCellularDataUsageKeyUpdater* self = 
             CCellularDataUsageKeyUpdater::NewLC( aServer );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCellularDataUsageKeyUpdater::NewLC
// -----------------------------------------------------------------------------
//
CCellularDataUsageKeyUpdater* CCellularDataUsageKeyUpdater::NewLC( 
        CConnMonServer* aServer )
    {
    CCellularDataUsageKeyUpdater* self = 
             new( ELeave ) CCellularDataUsageKeyUpdater( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCellularDataUsageKeyUpdater::~CCellularDataUsageKeyUpdater
// -----------------------------------------------------------------------------
//
CCellularDataUsageKeyUpdater::~CCellularDataUsageKeyUpdater()
    {
    // Cancel outstanding request, if exists
    Cancel();
    delete iRepository;
    }

// -----------------------------------------------------------------------------
// CCellularDataUsageKeyUpdater::UpdateKeyL
// -----------------------------------------------------------------------------
//
void CCellularDataUsageKeyUpdater::UpdateKeyL( const TInt aRegistration ) const
    {
    LOGENTRFN("CCellularDataUsageKeyUpdater::UpdateKeyL()")
    LOGIT1("CCellularDataUsageKeyUpdater::UpdateKeyL: aRegistration <%d>", 
            aRegistration)
                
    TCmGenConnSettings occSettings = ReadGenConnSettingsL();
  
    TInt value( ECmCellularDataUsageDisabled );
        
    if ( aRegistration == ENetworkRegistrationExtRoamingInternational )
        {           
        value = occSettings.iCellularDataUsageVisitor;
        }
    else if ( aRegistration == ENetworkRegistrationExtHomeNetwork || 
              aRegistration == ENetworkRegistrationExtRoamingNational )
        {           
        value = occSettings.iCellularDataUsageHome;
        }

    CRepository* cmRepository = NULL;
    
    TRAPD( err, cmRepository = CRepository::NewL( KCRUidCmManager ) )
    
    if ( err == KErrNone )
        {
        TInt previous( 0 );
        err = cmRepository->Get( KCurrentCellularDataUsage, previous );
           
        if ( err == KErrNone && ( value != previous ) )
            {
            cmRepository->Set( KCurrentCellularDataUsage, value );
            LOGIT1("KCurrentCellularDataUsage set to %d", value)
            }
        else
            {
            LOGIT1("KCurrentCellularDataUsage already up-to-date %d", previous)
            }
        
        delete cmRepository;    
        }
    else
        {
        LOGIT1("CCRepository::NewL( KCRUidCmManager ) FAILED <%d>", err)
        }

    LOGEXITFN("CCellularDataUsageKeyUpdater::UpdateKeyL()")
    }

// -----------------------------------------------------------------------------
// CCellularDataUsageKeyUpdater::CCellularDataUsageKeyUpdater
// -----------------------------------------------------------------------------
//
CCellularDataUsageKeyUpdater::CCellularDataUsageKeyUpdater( CConnMonServer* aServer )
        :
        CActive( EConnMonPriorityNormal ),
        iServer( aServer )
    {
    iRepository = NULL;
    iErrorCounter = 0;
    }

// -----------------------------------------------------------------------------
// CCellularDataUsageKeyUpdater::ConstructL
// -----------------------------------------------------------------------------
//
void CCellularDataUsageKeyUpdater::ConstructL()
    {
    iRepository = CRepository::NewL( KCDCommsRepositoryId );
    
    // Find out Default connection table id.
    // It contains iCellularDataUsageHome and iCellularDataUsageVisitor keys.
    CMDBSession* db = CMDBSession::NewLC( CMDBSession::LatestVersion() );	
    TRAPD( err, iTableId = CCDDefConnRecord::TableIdL( *db ) )
    	
    if ( err )
        {
        LOGIT1("ERROR, CCDDefConnRecord::TableIdL() <%d>", err)
        // Default connection table did not exist. 
        // Try once more because CMManager might have created it.
        iTableId = CCDDefConnRecord::TableIdL( *db );          	
        }
    CleanupStack::PopAndDestroy( db );
    	    
    CActiveScheduler::Add( this );
    
    // Initialize later asynchronously and let ConnMonServ continue now
    SetActive(); 
    iInitStatus = &iStatus;
    User::RequestComplete( iInitStatus, KErrNone );
    }

// -----------------------------------------------------------------------------
// CCellularDataUsageKeyUpdater::RequestNotifications
// -----------------------------------------------------------------------------
//
TInt CCellularDataUsageKeyUpdater::RequestNotifications()
    {  	    	
    LOGIT1("Calling iRepository->NotifyRequest() for table 0x%08X", iTableId)
    TInt err = iRepository->NotifyRequest( iTableId, KCDMaskShowRecordType, iStatus );

    if ( err == KErrNone )
        {
        SetActive();
        }
    else
        {
        LOGIT1("ERROR, iRepository->NotifyRequest() <%d>", err)
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CCellularDataUsageKeyUpdater::DoCancel
// -----------------------------------------------------------------------------
//
void CCellularDataUsageKeyUpdater::DoCancel()
    {
    iRepository->NotifyCancel( iTableId, KCDMaskShowRecordType );
    }

// -----------------------------------------------------------------------------
// CCellularDataUsageKeyUpdater::RunL
// -----------------------------------------------------------------------------
//
void CCellularDataUsageKeyUpdater::RunL()
    {
    LOGIT(".")
    LOGIT2("RunL: CCellularDataUsageKeyUpdater 0x%08X, 0x%08X", 
            iTableId, iStatus.Int())

    if ( iStatus.Int() < KErrNone )
        {
        LOGIT1("CCellularDataUsageKeyUpdater::RunL: error <%d>", iStatus.Int())
        iErrorCounter++;
        if ( iErrorCounter > KCenRepErrorRetryCount )
            {
            LOGIT1("Over %d consecutive errors, stopping notifications permanently", 
                   KCenRepErrorRetryCount)
            return;
            }
        }
    else
        {
        iErrorCounter = 0;
        
        TRAPD( leaveCode, UpdateKeyL() )
        
        LOGIT("CCellularDataUsageKeyUpdater triggered HandleAvailabilityChange()")
        iServer->AvailabilityManager()->HandleAvailabilityChange();
    
        if ( leaveCode )
            {
            LOGIT1("CCellularDataUsageKeyUpdater::RunL: LEAVE <%d>", leaveCode)
            }
        }

    RequestNotifications();
    }

// -----------------------------------------------------------------------------
// CCellularDataUsageKeyUpdater::ReadGenConnSettings
// -----------------------------------------------------------------------------
//
TCmGenConnSettings CCellularDataUsageKeyUpdater::ReadGenConnSettingsL() const
    {
    TCmGenConnSettings cmGenConnSettings;
    
    CMDBSession* db = CMDBSession::NewLC( CMDBSession::LatestVersion() );
    
    CMDBRecordSet<CCDDefConnRecord>* defConnRecordSet = 
        new ( ELeave ) CMDBRecordSet<CCDDefConnRecord>( iTableId );
    CleanupStack::PushL( defConnRecordSet );
     
    defConnRecordSet->LoadL( *db );

    TInt value = QUERY_INT_FIELD( defConnRecordSet->iRecords[0], 
                 KCDTIdUsageOfWlan );
    cmGenConnSettings.iUsageOfWlan = TCmUsageOfWlan( value );

    value = QUERY_INT_FIELD( defConnRecordSet->iRecords[0], 
                             KCDTIdCellularDataUsageHome );
    cmGenConnSettings.iCellularDataUsageHome = TCmCellularDataUsage( value );
    LOGIT1("cmGenConnSettings.iCellularDataUsageHome <%d>", 
            cmGenConnSettings.iCellularDataUsageHome)

    value = QUERY_INT_FIELD( defConnRecordSet->iRecords[0], 
                             KCDTIdCellularDataUsageVisitor );
    cmGenConnSettings.iCellularDataUsageVisitor = TCmCellularDataUsage( value );
    LOGIT1("cmGenConnSettings.iCellularDataUsageVisitor <%d>", 
            cmGenConnSettings.iCellularDataUsageVisitor)
            
    CleanupStack::PopAndDestroy( defConnRecordSet );	
    CleanupStack::PopAndDestroy( db );	
    
    return cmGenConnSettings;
    }

// -----------------------------------------------------------------------------
// CCellularDataUsageKeyUpdater::GetNetworkRegistration
// -----------------------------------------------------------------------------
//
TInt CCellularDataUsageKeyUpdater::GetNetworkRegistration( TInt& aRegistration ) const
    {
    return iServer->Iap()->GetNetworkRegistration_v2( aRegistration );
    }
    
// -----------------------------------------------------------------------------
// CCellularDataUsageKeyUpdater::UpdateKeyL
// -----------------------------------------------------------------------------
//
void CCellularDataUsageKeyUpdater::UpdateKeyL() const
    {
    TInt registration( ENetworkRegistrationExtHomeNetwork );
    TInt err( KErrNone );
    	
#ifndef __WINSCW__  // emulator does not support cellular network    	
    err = GetNetworkRegistration( registration );
#endif
    
    if ( err == KErrNone )
        {
        UpdateKeyL( registration );
        }
    else
        {
        LOGIT1("GetNetworkRegistration failed: <%d>", err );
        }
    }

// End-of-file
