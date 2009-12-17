/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Displays a dialog on the UI with the help of Symbian OS
*                notification services.
*
*/

#include <e32base.h>

#include "ConnMonServ.h"
#include "ConnMonIAP.h"
#include "CUiDialogAO.h"
#include "ConnMonAvailabilityManager.h"
#include "log.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUiDialogAO::CUiDialogAO
// -----------------------------------------------------------------------------
//
CUiDialogAO::CUiDialogAO( CConnMonServer* aServer )
    :
    CActive( EPriorityStandard ),
    iServer( aServer ),
    iDummy( KNullDesC8 )
    {
    }

// -----------------------------------------------------------------------------
// CUiDialogAO::ConstructL
// -----------------------------------------------------------------------------
//
void CUiDialogAO::ConstructL()
    {
    CActiveScheduler::Add( this );

    // Open RNotifier
    TInt ret = iNotifier.Connect();

    if ( ret != KErrNone )
        {
        LOGIT1("Could not connect to RNotifier, err <%d>", ret )
        User::Leave( ret );
        }
    }

// -----------------------------------------------------------------------------
// CUiDialogAO::NewL
// -----------------------------------------------------------------------------
//
CUiDialogAO* CUiDialogAO::NewL( CConnMonServer* aServer )
    {
    CUiDialogAO* self = new ( ELeave ) CUiDialogAO( aServer );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CUiDialogAO::~CUiDialogAO()
    {
    Cancel();

    iNotifier.Close();

    iServer = NULL;
    }

// -----------------------------------------------------------------------------
// CUiDialogAO::Start
// -----------------------------------------------------------------------------
//
void CUiDialogAO::Start()
    {
    if ( !IsActive() )
        {
        iNetIds.FillZ();
        iNetIds.Zero();

        iNotifier.StartNotifierAndGetResponse(
                iStatus,
                KUidSelectNetwork,
                iDummy,
                iNetIds );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CUiDialogAO::DoCancel
// -----------------------------------------------------------------------------
//
void CUiDialogAO::DoCancel()
    {
    if ( IsActive() )
        {
        iNotifier.CancelNotifier( KUidSelectNetwork );
        }
    }

// -----------------------------------------------------------------------------
// CUiDialogAO::RunL
// -----------------------------------------------------------------------------
//
void CUiDialogAO::RunL()
    {
    if ( iStatus == KErrNone )
        {
        // Set user's LAN network selection
        iServer->Iap()->SetEndUserLanPreference( iNetIds );

        // Write to log

        if ( iNetIds.Length() > 0 )
            {
            /////////// 3.1U availability staff
            LOGIT("CUiDialogAO::RunL() triggered HandleAvailabilityChange()")
            iServer->AvailabilityManager()->HandleAvailabilityChange();
            /////////////////////////////////////////////////////////////
            }

        iNotifier.CancelNotifier( KUidSelectNetwork );
        }
    else if ( iStatus == KErrCancel )
        {
        // Reset LAN network selection
        iServer->Iap()->ResetEndUserLanPreference();
        }
    else
        {
        // Write to log
        }

    delete this;
    }

// End-of-file
