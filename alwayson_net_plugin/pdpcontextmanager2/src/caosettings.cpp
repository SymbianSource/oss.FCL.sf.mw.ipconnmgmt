/*
* Copyright (c) 2004,2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements the CAOSettings class
*
*/


// INCLUDE FILES
#include <centralrepository.h>

#include "caosettings.h"
#include "logger.h"
#include "maosettingsobserver.h"
#include "pdpcontextmanagerinternalcrkeys.h"

// UNNAMED NAMESPACE FOR LOCAL DEFINITIONS
namespace
    {
    // CONSTANTS
#ifdef _DEBUG
    _LIT( KPanicCat, "Settings" );
#endif

    // Default values    
    const TInt  KDefaultRetryInterval      = 1;
    const TInt  KDefaultIAPUid             = 1;
    const TInt  KDefaultConnectionInterval = 25;
    const TInt  KDefaultUnconnectInterval  = 25;
    const TInt  KLingerOff                 = 0;
    const TInt  KLingerForEver             = -1;
    
    const TUint KIapColumn                 = 0x00000100;
    const TUint KLingerColumn              = 0x00000200;
    
    // DATA TYPES
    enum TPanicCode
        {
        EAlreadyFetchingSettings,
        ENotActive
        };
    
    // LOCAL FUNCTIONS
#ifdef _DEBUG
    LOCAL_C void Panic( TPanicCode aCode )
        {
        User::Panic( KPanicCat, aCode );
        }
#endif
    }

// METHODS

// ---------------------------------------------------------------------------
// CAOSettings::NewL
// ---------------------------------------------------------------------------
//
CAOSettings* CAOSettings::NewL( MAOSettingsObserver& aObserver )
    {
    LOG_1( _L("CAOSettings::NewL") );
    
    CAOSettings* self = new( ELeave ) CAOSettings( aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// CAOSettings::~CAOSettings
// ---------------------------------------------------------------------------
//
CAOSettings::~CAOSettings()
    {
    LOG_1( _L("CAOSettings::~CAOSettings") );
    
    Cancel();
    if ( iCenRepNotifyHandler )
        {
        iCenRepNotifyHandler->StopListening();
        delete iCenRepNotifyHandler;
        }
    delete iRepository;
    
    iLingerSettings.Close();
    }

// ---------------------------------------------------------------------------
// CAOSettings::CAOSettings
// ---------------------------------------------------------------------------
//
CAOSettings::CAOSettings( MAOSettingsObserver& aObserver ):
    CActive( CActive::EPriorityStandard ),
    iObserver( aObserver ),
    iIAP( KDefaultIAPUid ),
    iRetryTimerValue( KDefaultRetryInterval ),
    iSupportedInHPLMN( EFalse ),
    iSupportedInVPLMN( EFalse ),
    iLingerTimerValue( KLingerOff )
    {
    LOG_1( _L("CAOSettings::CAOSettings") );
    
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CAOSettings::ConstructL
// ---------------------------------------------------------------------------
//
void CAOSettings::ConstructL()
    {
    LOG_1( _L("CAOSettings::ConstructL") );
    
    // Create cenrep
    iRepository = CRepository::NewL( KCRUidPDPContextManager );
    
    // Create cenrep listener
    iCenRepNotifyHandler = CCenRepNotifyHandler::NewL(
        *this,
        *iRepository );
    iCenRepNotifyHandler->StartListeningL();
    
    // Get connection timer value
    iConnectionTimerValue = KDefaultConnectionInterval;
    UpdateConnectionTimerSetting();
        
    // Get unconnect timer value
    iUnconnectTimerValue = KDefaultUnconnectInterval;
    UpdateUnconnectTimerSetting();

    }

// ---------------------------------------------------------------------------
// CAOSettings::FetchSettings
// ---------------------------------------------------------------------------
//
void CAOSettings::FetchSettings( TUint32 aId )
    {
    LOG_2( _L("CAOSettings::FetchSettings: aId: %d"), aId );
    
    __ASSERT_DEBUG( !IsActive(), Panic( EAlreadyFetchingSettings ) );
    
    // Let just scheduler run and get settings when RunL is called
    iSettingsToFetch = aId;
    iStatus = KRequestPending;
    SetActive();
    CompleteSelf( iStatus, KErrNone );
    }

// ---------------------------------------------------------------------------
// CAOSettings::RunL
// ---------------------------------------------------------------------------
//
void CAOSettings::RunL()
    {
    LOG_2( _L("CAOSettings::RunL: iStatus: %d"), iStatus.Int() );
    
    // Currently we cannot complete with an error
    SetupSettings( iSettingsToFetch );
    iObserver.HandleSettingsChangedL();
    }

// ---------------------------------------------------------------------------
// CAOSettings::DoCancel
// ---------------------------------------------------------------------------
//
void CAOSettings::DoCancel()
    {
    LOG_1( _L("CAOSettings::DoCancel") );
    
    // We must complete pending request if needed
    // Checking IsActive is not sufficient, we must check also
    // iStatus
    if ( iStatus == KRequestPending && IsActive() )
        {
        CompleteSelf( iStatus, KErrCancel );
        }
    }

// ---------------------------------------------------------------------------
// CAOSettings::RunError
// ---------------------------------------------------------------------------
//
TInt CAOSettings::RunError( TInt /*aError*/ )
    {
    LOG_1( _L("CAOSettings::RunError") );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CAOSettings::AccessPointId
// ---------------------------------------------------------------------------
//
TInt CAOSettings::AccessPointId() const
    {
    LOG_1( _L("CAOSettings::AccessPointId") );
    
    return iIAP;
    }

// ---------------------------------------------------------------------------
// CAOSettings::RetryTimerValue
// ---------------------------------------------------------------------------
//
TInt CAOSettings::RetryTimerValue() const
    {
    LOG_1( _L("CAOSettings::RetryTimerValue") );
    
    return iRetryTimerValue;
    }

// ---------------------------------------------------------------------------
// CAOSettings::AlwaysOnSupportedInHPLMN
// ---------------------------------------------------------------------------
//
TBool CAOSettings::AlwaysOnSupportedInHPLMN() const
    {
    LOG_1( _L("CAOSettings::AlwaysOnSupportedInHPLMN") );
    
    return iSupportedInHPLMN;
    }

// ---------------------------------------------------------------------------
// CAOSettings::AlwaysOnSupportedInVPLMN
// ---------------------------------------------------------------------------
//
TBool CAOSettings::AlwaysOnSupportedInVPLMN() const
    {
    LOG_1( _L("CAOSettings::AlwaysOnSupportedInVPLMN") );
    
    return iSupportedInVPLMN;
    }

// ---------------------------------------------------------------------------
// CAOSettings::ConnectionTimerValue
// ---------------------------------------------------------------------------
//
TInt CAOSettings::ConnectionTimerValue() const
    {
    LOG_1( _L("CAOSettings::ConnectionTimerValue") );
    
    return iConnectionTimerValue;
    }

// ---------------------------------------------------------------------------
// CAOSettings::UnconnectTimerValue
// ---------------------------------------------------------------------------
//
TInt CAOSettings::UnconnectTimerValue() const
    {
    LOG_1( _L("CAOSettings::UnconnectTimerValue") );
    
    return iUnconnectTimerValue;
    }
    
// ---------------------------------------------------------------------------
// CAOSettings::LingerTimerValue
// ---------------------------------------------------------------------------
//
TInt CAOSettings::LingerTimerValue( const TUint aIapId ) const
    {
    LOG_1( _L("CAOSettings::LingerTimerValue") );
    
    for ( TInt j=0; j < iLingerSettings.Count(); j++ )
        {
        if ( iLingerSettings[ j ].iIap == aIapId )
            {
            LOG_3( _L("Linger setting found >> iap: %d, interval: %d"),
                   aIapId,
                   iLingerSettings[ j ].iInterval);  
        
            if ( iLingerSettings[ j ].iInterval == 0 )
                {
                return KLingerForEver;    
                }
            else
                {
                return ( iLingerSettings[ j ].iInterval );    
                }
            }    
        }
        
    LOG_2( _L("LingerTimerValue() not found >> iap: %d"),
           aIapId );
           
    return KLingerOff; // Linger is off           
    }    

// ---------------------------------------------------------------------------
// CAOSettings::HandleNotifyGeneric
// ---------------------------------------------------------------------------
//
void CAOSettings::HandleNotifyGeneric( TUint32 aId )
    {
    LOG_2( _L("CAOSettings::HandleNotifyGeneric: aId: %d"), aId );
    
    switch ( aId )
        {
        // We are interested only in Always-On settings
        case KPDPContextManagerDefaultUid:
        case KPDPContextManagerRetryTimer:
        case KPDPContextManagerConnectionTimer:
        case KPDPContextManagerUnconnectTimer:
        case KPDPContextManagerEnableWhenRoaming:
        case KPDPContextManagerEnableWhenHome:
            {
            FetchSettings( aId );
            break;
            }
        default:
            {
            if ( ( aId & KLingerColumn ) == KLingerColumn )
                {
                FetchSettings( KPDPContextManagerLinger );    
                }
            
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CAOSettings::SetupSettings
// ---------------------------------------------------------------------------
//
void CAOSettings::SetupSettings( TUint32 aId )
    {
    LOG_2( _L("CAOSettings::SetupSettings: aId: %d"), aId );
    
    switch ( aId )
        {
        case KPDPContextManagerDefaultUid:
            {
            UpdateIAPSetting();
            break;
            }
        case KPDPContextManagerRetryTimer:
            {
            UpdateRetryTimerSetting();
            break;
            }
        case KPDPContextManagerConnectionTimer:
            {
            UpdateConnectionTimerSetting();
            break;
            }
        case KPDPContextManagerUnconnectTimer:
            {
            UpdateUnconnectTimerSetting();
            break;
            }
        case KPDPContextManagerEnableWhenHome:
            {
            UpdateHPLMNSetting();
            break;
            }
        case KPDPContextManagerEnableWhenRoaming:
            {
            UpdateVPLMNSetting();
            break;
            }
        case KPDPContextManagerLinger:
            {
            UpdateLingerTimerSetting();
            break;
            }    
        default:
            {
            // Set all
            UpdateIAPSetting();
            UpdateRetryTimerSetting();
            UpdateConnectionTimerSetting();
            UpdateUnconnectTimerSetting();
            UpdateHPLMNSetting();
            UpdateVPLMNSetting();
            UpdateLingerTimerSetting();
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CAOSettings::CompleteSelf
// ---------------------------------------------------------------------------
//
void CAOSettings::CompleteSelf( TRequestStatus& aStatus, TInt aReason )
    {
    LOG_1( _L("CAOSettings::CompleteSelf") );
    
    __ASSERT_DEBUG( IsActive(), Panic( ENotActive ) );
    
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, aReason );
    }

// ---------------------------------------------------------------------------
// CAOSettings::UpdateIAPSetting
// ---------------------------------------------------------------------------
//
void CAOSettings::UpdateIAPSetting()
    {
    LOG_2( _L("CAOSettings::UpdateIAPSettingL >> Old value: %d"), iIAP );
    
    UpdateSetting( KPDPContextManagerDefaultUid, iIAP );
        
    LOG_2( _L("CAOSettings::UpdateIAPSettingL >> New value: %d"), iIAP );
    }

// ---------------------------------------------------------------------------
// CAOSettings::UpdateRetryTimerSetting
// ---------------------------------------------------------------------------
//
void CAOSettings::UpdateRetryTimerSetting()
    {
    LOG_2( _L("CAOSettings::UpdateRetryTimerSetting >> Old value: %d"),
        iRetryTimerValue );
        
    UpdateSetting( KPDPContextManagerRetryTimer, iRetryTimerValue );
        
    LOG_2( _L("CAOSettings::UpdateRetryTimerSetting >> New value: %d"),
        iRetryTimerValue );
    }

// ---------------------------------------------------------------------------
// CAOSettings::UpdateConnectionTimerSetting
// ---------------------------------------------------------------------------
//
void CAOSettings::UpdateConnectionTimerSetting()
    {
    LOG_2( _L("CAOSettings::UpdateConnectionTimerSetting >> Old value: %d"),
        iConnectionTimerValue );
        
    UpdateSetting( KPDPContextManagerConnectionTimer, iConnectionTimerValue );
        
    LOG_2( _L("CAOSettings::UpdateConnectionTimerSetting >> New value: %d"),
        iConnectionTimerValue );
    }

// ---------------------------------------------------------------------------
// CAOSettings::UpdateUnconnectTimerSetting
// ---------------------------------------------------------------------------
//
void CAOSettings::UpdateUnconnectTimerSetting()
    {
    LOG_2( _L("CAOSettings::UpdateUnconnectTimerSetting >> Old value: %d"),
        iUnconnectTimerValue );
        
    UpdateSetting( KPDPContextManagerUnconnectTimer, iUnconnectTimerValue );
        
    LOG_2( _L("CAOSettings::UpdateUnconnectTimerSetting >> New value: %d"),
        iUnconnectTimerValue );
    }


// ---------------------------------------------------------------------------
// CAOSettings::UpdateHPLMNSetting
// ---------------------------------------------------------------------------
//
void CAOSettings::UpdateHPLMNSetting()
    {
    LOG_2( _L("CAOSettings::UpdateHPLMNSetting >> Old value: %d"),
        iSupportedInHPLMN );
        
    UpdateSetting( KPDPContextManagerEnableWhenHome, iSupportedInHPLMN );
        
    LOG_2( _L("CAOSettings::UpdateHPLMNSetting >> New value: %d"),
        iSupportedInHPLMN );
    }

// ---------------------------------------------------------------------------
// CAOSettings::UpdateVPLMNSetting
// ---------------------------------------------------------------------------
//
void CAOSettings::UpdateVPLMNSetting()
    {
    LOG_2( _L("CAOSettings::UpdateVPLMNSetting >> Old value: %d"),
        iSupportedInVPLMN );
        
    UpdateSetting( KPDPContextManagerEnableWhenRoaming, iSupportedInVPLMN );
        
    LOG_2( _L("CAOSettings::UpdateVPLMNSetting >> New value: %d"),
        iSupportedInVPLMN );
    }

// ---------------------------------------------------------------------------
// CAOSettings::UpdateLingerTimerSetting
// ---------------------------------------------------------------------------
//
void CAOSettings::UpdateLingerTimerSetting()
    {
    LOG_1( _L("CAOSettings::UpdateLingerTimerSetting"));
        
    TInt           count( 0 );
    TInt           err( KErrNone );
    TLingerSetting ls;
        
    iLingerSettings.Reset();
    
    // Get number of entries (iapId&linger) in Centrep
    err = iRepository->Get( KPdpContextManagerLingerArrayCount, count );
        
    if ( err == KErrNone )
        {
        // read all entries from Centrep
        for ( TInt row=1; row <= count; row++ )
            {
            err = iRepository->Get( ( KIapColumn | row ), ls.iIap ); 
        
            if ( err == KErrNone )
                {
                err = iRepository->Get( ( KLingerColumn | row ), ls.iInterval );     
                }
                
            if ( err == KErrNone ) 
                {
                iLingerSettings.Append( ls );    
                }
            else
                {
                LOG_3( _L("CRepository::Get() failed >> err: %d, row: %d"),
                err, row);
                
                return;        
                }    
            }
        }
    else
        {
        LOG_2( _L("CRepository::Get( KPdpContextManagerLingerArrayCount) >> err: %d"),
                err);       
        }        
      
    // Write to log    
    for ( TInt j=0; j < iLingerSettings.Count(); j++ )
        {
        LOG_3( _L("iLingerSettings >> iap: %d, interval: %d"),
        iLingerSettings[ j ].iIap,
        iLingerSettings[ j ].iInterval);    
        }
    }

// ---------------------------------------------------------------------------
// CAOSettings::UpdateSetting
// ---------------------------------------------------------------------------
//
void CAOSettings::UpdateSetting( TUint32 aId, TInt& aValue )
    {
    LOG_2( _L("CAOSettings::UpdateSetting: %d"), aId );

    TInt value = 0;
    TInt err = iRepository->Get( aId, value );
    if ( err == KErrNone )
        {
        // New value got, store it
        aValue = value;
        }
    }

// End of file
