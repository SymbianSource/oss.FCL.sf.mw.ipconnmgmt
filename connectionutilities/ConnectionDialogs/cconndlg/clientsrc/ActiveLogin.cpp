/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CActiveLogin class
*
*/


// INCLUDE FILES
#include "ActiveLogin.h"


// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------------------------
// CActiveLogin::CActiveLogin
// ----------------------------------------------------------------------------
//
CActiveLogin::CActiveLogin( TDes& aUsername, TDes& aPassword )
: CActive( CActive::EPriorityUserInput ),
  iUsername( &aUsername ),
  iPassword( &aPassword )
    {
    iAuthenticationPairBuff().iUsername = aUsername;
    iAuthenticationPairBuff().iPassword = aPassword;
    }


// ----------------------------------------------------------------------------
// CActiveLogin::NewL
// ----------------------------------------------------------------------------
//
CActiveLogin* CActiveLogin::NewL( TDes& aUsername, TDes& aPassword )
    {
    CActiveLogin* self = new( ELeave )CActiveLogin( aUsername, aPassword );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// ----------------------------------------------------------------------------
// CActiveLogin::ConstructL
// ----------------------------------------------------------------------------
//
void CActiveLogin::ConstructL()
    {
    }


// ----------------------------------------------------
// CActiveLogin::~CActiveLogin()
// ----------------------------------------------------
//
CActiveLogin::~CActiveLogin()
    {
    Cancel();
    }


// ----------------------------------------------------------------------------
// CActiveLogin::RunL
// ----------------------------------------------------------------------------
//
void CActiveLogin::RunL()
    {
    if ( iStatus.Int() == KErrNone )
        {
        *iUsername = iAuthenticationPairBuff().iUsername;
        *iPassword = iAuthenticationPairBuff().iPassword;
        }

    TRequestStatus* pS = iRs;
    User::RequestComplete( pS, iStatus.Int() );

    delete this;
    }


// ----------------------------------------------------------------------------
// CActiveLogin::DoCancel
// ----------------------------------------------------------------------------
//
void CActiveLogin::DoCancel()
    {
    TRequestStatus* pS = iRs;
    User::RequestComplete( pS, KErrCancel );
    }


// ----------------------------------------------------------------------------
// CActiveLogin::Observe
// ----------------------------------------------------------------------------
//
void CActiveLogin::Observe( TRequestStatus &aStatus )
    {
    CActiveScheduler::Add( this );

    iRs = &aStatus;
    *iRs = KRequestPending;

    SetActive();
    }


// ----------------------------------------------------------------------------
// CActiveLogin::GetBuffer
// ----------------------------------------------------------------------------
//
TPckgBuf<TAuthenticationPairBuff>* CActiveLogin::GetBuffer() 
    { 
    return &iAuthenticationPairBuff; 
    }


// End of File  
