/*
* Copyright (c) 2009- Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Listens for changes in one CommsDat table through CenRep.
*
*/

#include <commsdat.h>
#include <centralrepository.h>
#include <cmcommonconstants.h>

#include "cmcommsdatnotifier.h"
#include "cmmanagerimpl.h"

// Repository for CommsDat
const TUid KCDCommsRepositoryId = { 0xCCCCCC00 };

CCmCommsDatNotifier* CCmCommsDatNotifier::NewL( TUint32 aTableId )
    {
    CCmCommsDatNotifier* self = CCmCommsDatNotifier::NewLC( aTableId );
    CleanupStack::Pop( self );
    return self;
    }

CCmCommsDatNotifier* CCmCommsDatNotifier::NewLC( TUint32 aTableId )
    {
    CCmCommsDatNotifier* self = new( ELeave ) CCmCommsDatNotifier( aTableId );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CCmCommsDatNotifier::~CCmCommsDatNotifier()
    {
    // Cancel outstanding request, if exists
    if( iWatcher )
        {
        Cancel();
        }
    delete iRepository;
    
    DeleteCallBackArray();
    
    iWatcher = NULL;
    }

void CCmCommsDatNotifier::DeleteCallBackArray()
    {
    TInt count = iCallBackArray->Count();
    TInt index( 0 );
    while( count > 0 )
        {
        iCallBackArray->Delete( index );
        iCallBackArray->Compress();
        
        count = iCallBackArray->Count();
        }
    
    delete iCallBackArray;
    iCallBackArray = NULL;
    }

CCmCommsDatNotifier::CCmCommsDatNotifier( TUint32 aTableId )
        :
        CActive( EPriorityStandard ),
        iTableId( aTableId ),
        iWatcher( NULL )
    {
    iRepository = NULL;
    }

void CCmCommsDatNotifier::ConstructL()
    {
    iCallBackArray = new (ELeave) CCmCallBackArray( KCmArrayBigGranularity );
    
    iRepository = CRepository::NewL( KCDCommsRepositoryId );
    CActiveScheduler::Add( this );
    }

void CCmCommsDatNotifier::WatcherRegisterL( MCmCommsDatWatcher* aWatcher )
    {
    if( !aWatcher )
        {
        return;
        }
    
    if( !iWatcher )
        {
        TInt err( RequestNotifications() );
        User::LeaveIfError( err );
        }
    
    iCallBackArray->AppendL( aWatcher );
    
    iWatcher = aWatcher;
    }

void CCmCommsDatNotifier::WatcherUnRegister( )
    {
    TInt index = iCallBackArray->Count();
    index --;
    
    if (index >= 0)
        {
        iCallBackArray->Delete( index );
        iCallBackArray->Compress();
        }
    
    index = iCallBackArray->Count();
    index --;
    if (index >= 0)
        {
        iWatcher = (*iCallBackArray)[index];
        }
    else
        {
        iWatcher = NULL;
        }
    
    if( !iWatcher )
        {
        Cancel();
        }
    }

TInt CCmCommsDatNotifier::RequestNotifications()
    {
    TInt err = iRepository->NotifyRequest( iTableId, KCDMaskShowRecordType, iStatus );

    if ( KErrNone == err )
        {
        SetActive();
        }
        
    return err;
    }

void CCmCommsDatNotifier::DoCancel()
    {
    iRepository->NotifyCancel( iTableId, KCDMaskShowRecordType );
    }

void CCmCommsDatNotifier::RunL()
    {
    if ( iStatus.Int() < KErrNone )
        {
        return;
        }

    if ( iWatcher )
        {
        iWatcher->CommsDatChangesL();
        
        RequestNotifications();
        }
    }

// End-of-file
