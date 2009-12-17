/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CCmConnectionMethodInfo
*
*/
#include <cmpluginbase.h>
#include "cmmanagerimpl.h"
#include "cmlogger.h"
#include "cmconnectionmethodinfo.h"

using namespace CommsDat;

// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::CCmConnectionMethodInfo
// ---------------------------------------------------------------------------
//
CCmConnectionMethodInfo::CCmConnectionMethodInfo( 
                                                TCmPluginInitParam *aInitParam )
    : CCmPluginBaseEng( aInitParam )
    {
    CLOG_CREATE;
    }

// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::ConstructL
// ---------------------------------------------------------------------------
//
void CCmConnectionMethodInfo::ConstructL()
    {
    // Dummy bearer type
    iBearerType = KDummyBearerType;
    
    CCmPluginBaseEng::ConstructL();
    }

// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::NewL
// ---------------------------------------------------------------------------
//
CCmConnectionMethodInfo* 
                CCmConnectionMethodInfo::NewL( TCmPluginInitParam *aInitParam )
    {
    CCmConnectionMethodInfo* self = CCmConnectionMethodInfo::NewLC( aInitParam );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::NewLC
// ---------------------------------------------------------------------------
//
CCmConnectionMethodInfo* 
                CCmConnectionMethodInfo::NewLC( TCmPluginInitParam *aInitParam )
    {
    CCmConnectionMethodInfo* self = 
                            new( ELeave ) CCmConnectionMethodInfo( aInitParam );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::~CCmConnectionMethodInfo
// ---------------------------------------------------------------------------
//
CCmConnectionMethodInfo::~CCmConnectionMethodInfo()
    {
    CLOG_CLOSE;
    }

// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::GetConnectionInfoIntL
// ---------------------------------------------------------------------------
//
TUint32 CCmConnectionMethodInfo::GetConnectionInfoIntL( const TUint32 aIapId, 
                                                 const TUint32 aAttribute )
    {
    CCmPluginBase* plugin = CmMgr().GetConnectionMethodL( aIapId );
    CleanupStack::PushL( plugin );
     
    if ( !plugin ) 
        {
        User::Leave( KErrNotFound );
        }

    ResetAndLoadL( aIapId );
        
    TUint32 info = plugin->GetIntAttributeL( aAttribute );
    CleanupStack::PopAndDestroy( plugin );

    return info;
    }
    
// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::GetConnectionInfoBoolL
// ---------------------------------------------------------------------------
//
TBool CCmConnectionMethodInfo::GetConnectionInfoBoolL( const TUint32 aIapId, 
                                                const TUint32 aAttribute )
    {
    CCmPluginBase* plugin = CmMgr().GetConnectionMethodL( aIapId );
    CleanupStack::PushL( plugin );
    
    if ( !plugin ) 
        {
        User::Leave( KErrNotFound );
        }

    ResetAndLoadL( aIapId );
        
	TBool info = plugin->GetBoolAttributeL( aAttribute );
    CleanupStack::PopAndDestroy( plugin );
    
    return info;
	}
                                 
// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::GetConnectionInfoStringL
// ---------------------------------------------------------------------------
//
HBufC* 
   CCmConnectionMethodInfo::GetConnectionInfoStringL( const TUint32 aIapId, 
                                                      const TUint32 aAttribute )
    {
    CCmPluginBase* plugin = CmMgr().GetConnectionMethodL( aIapId );
    CleanupStack::PushL( plugin );
    
    if ( !plugin ) 
        {
        User::Leave( KErrNotFound );
        }

    ResetAndLoadL( aIapId );

    HBufC* info = plugin->GetStringAttributeL( aAttribute );
    CleanupStack::PopAndDestroy( plugin );
    
    return info;
    }

// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::GetConnectionInfoStringL
// ---------------------------------------------------------------------------
//
HBufC8* CCmConnectionMethodInfo::GetConnectionInfoString8L( 
                                                     const TUint32 aIapId, 
                                                     const TUint32 aAttribute )
    {
    CCmPluginBase* plugin = CmMgr().GetConnectionMethodL( aIapId );
    CleanupStack::PushL( plugin );
    
    if ( !plugin ) 
        {
        User::Leave( KErrNotFound );
        }

    ResetAndLoadL( aIapId );

    HBufC8* info = plugin->GetString8AttributeL( aAttribute );
    CleanupStack::PopAndDestroy( plugin );
    
    return info;
    }

// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::RunSettingsL
// ---------------------------------------------------------------------------
//
TInt CCmConnectionMethodInfo::RunSettingsL()
    {
    User::Leave( KErrNotSupported );
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::InitializeWithUiL
// ---------------------------------------------------------------------------
//
TBool CCmConnectionMethodInfo::InitializeWithUiL( TBool /*aManuallyConfigure*/ )
    {
    User::Leave( KErrNotSupported );

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::CanHandleIapIdL
// ---------------------------------------------------------------------------
//
TBool CCmConnectionMethodInfo::CanHandleIapIdL( TUint32 /*aIapId*/ ) const
    {
    User::Leave( KErrNotSupported );
    
    return EFalse;
    }
    
// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::CanHandleIapIdL
// ---------------------------------------------------------------------------
//
TBool CCmConnectionMethodInfo::CanHandleIapIdL( 
                                            CCDIAPRecord* /*aIapRecord*/ ) const
    {
    User::Leave( KErrNotSupported );
    
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::ServiceRecordIdLC
// ---------------------------------------------------------------------------
//
void CCmConnectionMethodInfo::ServiceRecordIdLC( HBufC* &/*aBearerName*/, 
                                         TUint32& /*aRecordId*/ )
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::BearerRecordIdLC
// ---------------------------------------------------------------------------
//
void CCmConnectionMethodInfo::BearerRecordIdLC( HBufC* &/*aBearerName*/, 
                                         TUint32& /*aRecordId*/ )
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::CreateNewServiceRecordL
// ---------------------------------------------------------------------------
//
void CCmConnectionMethodInfo::CreateNewServiceRecordL()
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::ResetAndLoadL
// ---------------------------------------------------------------------------
//
void CCmConnectionMethodInfo::ResetAndLoadL( TUint32 aIapId )
    {
    if( iIapId != aIapId )
        {
        Reset();
        LoadL( aIapId );
        }
    }

// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::CopyAdditionalDataL
// ---------------------------------------------------------------------------
//
void CCmConnectionMethodInfo::CopyAdditionalDataL( CCmPluginBaseEng* /*aDestInst*/ )
    {
    User::Leave( KErrNotSupported );
    }
    
// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::CreateInstanceL
// ---------------------------------------------------------------------------
//
CCmPluginBaseEng* CCmConnectionMethodInfo::
                CreateInstanceL( TCmPluginInitParam& /*aInitParam*/ ) const
    {
    User::Leave( KErrNotSupported );
    
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::CreateCopyL
// ---------------------------------------------------------------------------
//
CCmPluginBaseEng* CCmConnectionMethodInfo::CreateCopyL() const
    {
    User::Leave( KErrNotSupported );
    
    return NULL;
    }

// ---------------------------------------------------------------------------
// CCmConnectionMethodInfo::ResetIfInMemory
// ---------------------------------------------------------------------------
//
void CCmConnectionMethodInfo::ResetIfInMemory( CCmPluginBaseEng* aCM )
    {
    TUint32 cmId = 0;
    TRAPD( err, cmId = aCM->GetIntAttributeL( CMManager::ECmId ) );
    if ( KErrNone == err )
        {
        if( cmId > 0 && iIapId == cmId )
            {
            Reset();
            TRAP_IGNORE( LoadL( cmId ) );
            }
        }
    }
