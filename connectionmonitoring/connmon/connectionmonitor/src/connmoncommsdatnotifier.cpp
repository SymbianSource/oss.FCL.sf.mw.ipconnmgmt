/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "connmoncommsdatnotifier.h"
#include "ConnMonAvailabilityManager.h"
#include "ConnMonDef.h"
#include "log.h"

// Repository for CommsDat
const TUid KCDCommsRepositoryId = { 0xCCCCCC00 };

CConnMonCommsDatNotifier* CConnMonCommsDatNotifier::NewL(
        CConnMonAvailabilityManager* aAvailabilityManager,
        TUint32 aTableId )
    {
    CConnMonCommsDatNotifier* self = CConnMonCommsDatNotifier::NewLC(
            aAvailabilityManager,
            aTableId );
    CleanupStack::Pop( self );
    return self;
    }

CConnMonCommsDatNotifier* CConnMonCommsDatNotifier::NewLC(
        CConnMonAvailabilityManager* aAvailabilityManager,
        TUint32 aTableId )
    {
    CConnMonCommsDatNotifier* self = new( ELeave ) CConnMonCommsDatNotifier(
            aAvailabilityManager,
            aTableId );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CConnMonCommsDatNotifier::~CConnMonCommsDatNotifier()
    {
    // Cancel outstanding request, if exists
    Cancel();
    delete iRepository;
    }

CConnMonCommsDatNotifier::CConnMonCommsDatNotifier(
        CConnMonAvailabilityManager* aAvailabilityManager,
        TUint32 aTableId )
        :
        CActive( EConnMonPriorityNormal ),
        iAvailabilityManager( aAvailabilityManager ),
        iTableId( aTableId )
    {
    iRepository = NULL;
    iErrorCounter = 0;
    }

void CConnMonCommsDatNotifier::ConstructL()
    {
    //LOGENTRFN("CConnMonCommsDatNotifier::ConstructL()")

    iRepository = CRepository::NewL( KCDCommsRepositoryId );
    CActiveScheduler::Add( this );
    TInt err( RequestNotifications() );
    User::LeaveIfError( err );

    //LOGEXITFN("CConnMonCommsDatNotifier::ConstructL()")
    }

TInt CConnMonCommsDatNotifier::RequestNotifications()
    {
    //LOGENTRFN("CConnMonCommsDatNotifier::RequestNotifications()")

    LOGIT1("Calling iRepository->NotifyRequest() for table 0x%08X", iTableId)
    TInt err = iRepository->NotifyRequest( iTableId, KCDMaskShowRecordType, iStatus );

    if ( KErrNone == err )
        {
        SetActive();
        }
    else
        {
        LOGIT1("ERROR, iRepository->NotifyRequest() <%d>", err)
        }

    //LOGEXITFN1("CConnMonCommsDatNotifier::RequestNotifications()", err)
    return err;
    }

void CConnMonCommsDatNotifier::DoCancel()
    {
    iRepository->NotifyCancel( iTableId, KCDMaskShowRecordType );
    }

void CConnMonCommsDatNotifier::RunL()
    {
    //LOGENTRFN("CConnMonCommsDatNotifier::RunL()")

    LOGIT(".")
    LOGIT2("RunL: CConnMonCommsDatNotifier 0x%08X, 0x%08X", iTableId, iStatus.Int())

    if ( iStatus.Int() < KErrNone )
        {
        LOGIT1("CConnMonCommsDatNotifier::RunL: error <%d>", iStatus.Int())
        iErrorCounter++;
        if ( iErrorCounter > KConnMonCenRepErrorThreshold )
            {
            LOGIT1("Over %d consecutive errors, stopping notifications permanently", KConnMonCenRepErrorThreshold)
            return;
            }
        }
    else
        {
        iErrorCounter = 0;
        }

    RequestNotifications();

    TRAPD( leaveCode, iAvailabilityManager->CommsDatEventReceivedL( iTableId ) )
    if ( leaveCode )
        {
        LOGIT1("CConnMonCommsDatNotifier::RunL: LEAVE <%d>", leaveCode)
        }

    //LOGEXITFN("CConnMonCommsDatNotifier::RunL()")
    }

// End-of-file
