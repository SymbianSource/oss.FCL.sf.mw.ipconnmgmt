/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Handling of Mobility Policy Manager notifications.
*
*/

/**
@file s60mpmnotifications.cpp
Handling of Mobility Policy Manager notifications.
*/
#include <rmpm.h>

#include "s60mpmnotifications.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPMPolicyNotifications::NewL
// -----------------------------------------------------------------------------
//
CMPMPolicyNotifications* CMPMPolicyNotifications::NewL( MMPMPolicyNotificationUser& aUser, RMPM& aMpm )
    {
    CMPMPolicyNotifications* self = new ( ELeave ) CMPMPolicyNotifications( aUser, aMpm );
    return self;
    }

// -----------------------------------------------------------------------------
// CMPMPolicyNotifications::CMPMPolicyNotifications
// -----------------------------------------------------------------------------
//
CMPMPolicyNotifications::CMPMPolicyNotifications( MMPMPolicyNotificationUser& aUser, RMPM& aMpm )
    : CActive( EPriorityStandard ),
      iNotificationUser( aUser ),
      iMpm( aMpm ),
      iCommitedToDeleteItself( EFalse )
    {
    CActiveScheduler::Add( this );
    S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyNotifications<%x>::CMPMPolicyNotifications()", this )
    }

// -----------------------------------------------------------------------------
// CMPMPolicyNotifications::~CMPMPolicyNotifications
// -----------------------------------------------------------------------------
//
CMPMPolicyNotifications::~CMPMPolicyNotifications()
    {
    S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyNotifications<%x>::~CMPMPolicyNotifications", this )
    }

// -----------------------------------------------------------------------------
// CMPMPolicyNotifications::CancelRequestAndDestroyD
// -----------------------------------------------------------------------------
//
void CMPMPolicyNotifications::CancelRequestAndDestroyD( TRequestStatus* aStatus )
    {
    S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyNotifications<%x>::CancelRequestAndDestroyD", this )
    iDeletedStatus = aStatus;
    if ( iMpm.Connected() && IsActive() )
        {
        S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyNotifications<%x>::CancelRequestAndDestroyD Start asynch delete", this )
        iMpm.SendCancelRequest( EMPMWaitNotification );
        iCommitedToDeleteItself = ETrue;
        *iDeletedStatus = KRequestPending;
        }
    else
        {
        S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyNotifications<%x>::CancelRequestAndDestroyD Delete immediately", this )
        ASSERT( !IsActive() );
        ASSERT( !aStatus );
        delete this;
        }
    }

// -----------------------------------------------------------------------------
// CMPMPolicyNotifications::Activate
// -----------------------------------------------------------------------------
//
void CMPMPolicyNotifications::Activate()
    {
    if ( !IsActive() )
        {
        S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyNotifications<%x>::Activate Wait notification", this )
        iMpm.WaitNotification( iMsgBuffer, iStatus );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CMPMPolicyNotifications::RunL
// -----------------------------------------------------------------------------
//
void CMPMPolicyNotifications::RunL()
    {
    ASSERT( iMpm.Connected() );

    if ( iCommitedToDeleteItself )
        {
        S60MCPRLOGSTRING2( "S60MCPR::CMPMPolicyNotifications<%x>::RunL Completing iDeletedStatus %d", this, KErrNone )
        User::RequestComplete( iDeletedStatus, KErrNone );
        delete this;
        return;
        }
    // In case notification, is errored we shouldn't resubmit any request. 
    if ( iStatus.Int() != KErrNone )
        {
        S60MCPRLOGSTRING2( "S60MCPR::CMPMPolicyNotifications<%x>::RunL status %d", this, iStatus.Int() )
        return;
        }
    // Typically deliver the notification and resubmit a request for new one.
    TUint8* msgBufferPtr = const_cast<TUint8*>( iMsgBuffer.Ptr() );
    TMpmNotification* notification = 
                          reinterpret_cast<TMpmNotification*>( msgBufferPtr );

    iNotificationUser.PolicyNotification( *const_cast<TMpmNotification*>( notification ) );

    S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyNotifications<%x>::RunL Wait notification", this )
    iMpm.WaitNotification( iMsgBuffer, iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CMPMPolicyNotifications::DoCancel
// -----------------------------------------------------------------------------
//
void CMPMPolicyNotifications::DoCancel()
    {
    // Not allowed! Cancel() may cause deadlock!
    _LIT( KPanicMsg, "CMPMPolicyNotifications::DoCancel" );
    User::Panic( KPanicMsg, KErrNotSupported );
    }

// End of file
