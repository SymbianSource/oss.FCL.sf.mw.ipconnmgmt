/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Listens for changes in dial-up PDP context override P&S-key.
*
*/

#include <e32base.h>
#include <e32property.h>

#include "connmondialupoverridenotifier.h"
#include "connectionmonitorpskeys.h"
#include "ConnMonServ.h"
#include "ConnMonIAP.h"
#include "log.h"


// -----------------------------------------------------------------------------
// Two phased construction.
// -----------------------------------------------------------------------------
//
CConnMonDialUpOverrideNotifier* CConnMonDialUpOverrideNotifier::NewL(
        CConnMonServer* aServer )
    {
    CConnMonDialUpOverrideNotifier* self =
            CConnMonDialUpOverrideNotifier::NewLC( aServer );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Two phased construction.
// -----------------------------------------------------------------------------
//
CConnMonDialUpOverrideNotifier* CConnMonDialUpOverrideNotifier::NewLC(
        CConnMonServer* aServer )
    {
    CConnMonDialUpOverrideNotifier* self =
            new( ELeave ) CConnMonDialUpOverrideNotifier( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CConnMonDialUpOverrideNotifier::~CConnMonDialUpOverrideNotifier()
    {
    // Cancel outstanding request, if exists.
    Cancel();
    iDialUpProperty.Delete( KConnectionMonitorPS, KDialUpConnection );
    iDialUpProperty.Close();
    }

// -----------------------------------------------------------------------------
// CConnMonDialUpOverrideNotifier::ResetStatus
// -----------------------------------------------------------------------------
//
TInt CConnMonDialUpOverrideNotifier::ResetStatus()
    {
    // Set property value to EConnMonReady.
    TInt err = iDialUpProperty.Set( EConnMonReady );
    LOGIT1("ResetStatus: Dial-up property value set to EConnMonReady <%d>", err)
    return err;
    }

// -----------------------------------------------------------------------------
// Constructor.
// -----------------------------------------------------------------------------
//
CConnMonDialUpOverrideNotifier::CConnMonDialUpOverrideNotifier(
        CConnMonServer* aServer )
        :
        CActive( EConnMonPriorityMedium ),
        iServer( aServer),
        iErrorCounter( 0 )
    {
    }

// -----------------------------------------------------------------------------
// Two phased construction.
// -----------------------------------------------------------------------------
//
void CConnMonDialUpOverrideNotifier::ConstructL()
    {
    LOGENTRFN("CConnMonDialUpOverrideNotifier::ConstructL()")

    // Define Dial-up property.
    TInt err = iDialUpProperty.Define(
            KConnectionMonitorPS,
            KDialUpConnection,
            RProperty::EInt,
            KDialUpOverrideReadC0,  // Read: Always pass
            KDialUpOverrideWriteC2, // Write: NetworkControl, NetworkServices
            0 );
    LOGIT1("Dial-up property defined <%d>", err)
    User::LeaveIfError( err );

    // Attach to property.
    err = iDialUpProperty.Attach(
            KConnectionMonitorPS,
            KDialUpConnection );
    LOGIT1("Dial-up property attach <%d>", err)
    User::LeaveIfError( err );

    // Set property value to EConnMonReady.
    ResetStatus();

    CActiveScheduler::Add( this );
    RequestNotifications();

    LOGEXITFN("CConnMonDialUpOverrideNotifier::ConstructL()")
    }

// -----------------------------------------------------------------------------
// CConnMonDialUpOverrideNotifier::RequestNotifications
// -----------------------------------------------------------------------------
//
void CConnMonDialUpOverrideNotifier::RequestNotifications()
    {
    LOGENTRFN("CConnMonDialUpOverrideNotifier::RequestNotifications()")

    if ( !IsActive() )
        {
        iDialUpProperty.Subscribe( iStatus );
        LOGIT("KDialUpConnection-key, Subscribe()")
        SetActive();
        }

    LOGEXITFN("CConnMonDialUpOverrideNotifier::RequestNotifications()")
    return;
    }

// -----------------------------------------------------------------------------
// CConnMonDialUpOverrideNotifier::DoCancel
// -----------------------------------------------------------------------------
//
void CConnMonDialUpOverrideNotifier::DoCancel()
    {
    LOGIT("Canceling dialup property subscription.")
    iDialUpProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// CConnMonDialUpOverrideNotifier::RunL
// -----------------------------------------------------------------------------
//
void CConnMonDialUpOverrideNotifier::RunL()
    {
    LOGIT(".")
    LOGIT1("RunL: CConnMonDialUpOverrideNotifier <%d>", iStatus.Int())

    if ( iStatus.Int() == KErrNone )
        {
        iErrorCounter = 0;
        TInt value = -1;
        TInt err = iDialUpProperty.Get( value );
        LOGIT2("Dial-up override value is %d <%d>", value, err)

        switch ( value )
            {
            case EConnMonDialUpClosed:
                {
                // Deactivate the dial-up override (if active). This will return
                // cellular data connectivity back to normal.
                iServer->SetDialUpOverrideStatus( EConnMonDialUpOverrideInactive );
                ResetStatus();
                }
                break;
            case EConnMonDialUpInit:
                {
                // Set the dial-up override status to active. This will block
                // other cellular data connectivity and start the timeout timer.
                iServer->SetDialUpOverrideStatus( EConnMonDialUpOverrideActive );

                // If there are no active packetdata connections, signal EConnMonReady
                // immediately. If there are active connections, EConnMonReady is
                // signaled when those connections have gone down.
                if ( !iServer->Iap()->ActivePacketdataConnectionsFound() )
                    {
                    ResetStatus();
                    }
                }
                break;
            case EConnMonReady:
            default:
                break;
            }
        }
    else
        {
        LOGIT1("Dial-up override notification error <%d>", iStatus.Int())
        iErrorCounter++;
        if ( iErrorCounter > KConnMonPsEventErrorThreshold )
            {
            LOGIT1("Over %d consecutive errors, stopping", KConnMonPsEventErrorThreshold)
            return;
            }
        }
    RequestNotifications();
    }

// End of file
