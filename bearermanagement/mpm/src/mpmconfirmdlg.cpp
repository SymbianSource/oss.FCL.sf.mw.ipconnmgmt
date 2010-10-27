/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Confirm Dialog implementation
*
*/

/**
@file mpmconfirmdlg.cpp
UI Note Handler implementation
*/

// INCLUDE FILES
#include "mpmconfirmdlg.h"
#include "mpmlogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPMConfirmDlg::NewL
// -----------------------------------------------------------------------------
//
CMPMConfirmDlg* CMPMConfirmDlg::NewL( CArrayPtrFlat<CMPMDialogBase>& aQueue,
                                      MMPMConfirmDlg&     aConfirmDlgUser,
                                      const TUint32       aSnapId,
                                      const TUint32       aIAP,
                                      const TDialogType   aDialogType )
    {
    CMPMConfirmDlg* self = new ( ELeave ) CMPMConfirmDlg( aQueue,
                                                          aConfirmDlgUser,
                                                          aSnapId,
                                                          aIAP,
                                                          aDialogType );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMPMConfirmDlg::CMPMConfirmDlg
// -----------------------------------------------------------------------------
//
CMPMConfirmDlg::CMPMConfirmDlg( CArrayPtrFlat<CMPMDialogBase>& aQueue,
                                MMPMConfirmDlg&     aConfirmDlgUser,
                                const TUint32       aSnapId,
                                const TUint32       aIAP,
                                const TDialogType   aDialogType )
    : CMPMDialogBase( *(CArrayPtrFlat<CMPMDialogBase>*)&aQueue ),
      iConfirmDlgUser( aConfirmDlgUser),
      iSnapId( aSnapId ),
      iIAP( aIAP ),
      iDialogType( aDialogType ),
      iUtils( NULL ),
      iResult( EMsgQueryCancelled )
    {
    }

// -----------------------------------------------------------------------------
// CMPMConfirmDlg::ConstructL
// -----------------------------------------------------------------------------
//
void CMPMConfirmDlg::ConstructL()
    {
    iUtils = CConnectionUiUtilities::NewL();
    CMPMDialogBase::ConstructL();
    }

// -----------------------------------------------------------------------------
// CMPMConfirmDlg::Start
// -----------------------------------------------------------------------------
//
void CMPMConfirmDlg::Start( CMPMDialogBase* aDlg )
    {
    //  Start the dialog if previous dialog does not exist: !aDlg
    //  OR
    //  if ConfirmDlgResponse is not accepted by the iConfirmDlgUser.
    //
    if ( !aDlg || !iConfirmDlgUser.ConfirmDlgResponse( 
                                   &((CMPMConfirmDlg*)aDlg)->iConfirmDlgUser ) )
        {
        MPMLOGSTRING3( "CMPMConfirmDlg::Start Display dialog: SNAP %d, IAP %d", 
                       iSnapId, iIAP )

        switch ( iDialogType )
            {
            case EConfirmDlgHomeNetwork:
                {                
                MPMLOGSTRING( "    Show home network confirm dialog" )
                iUtils->ConfirmMethodUsageQuery( iResult,
                                                 true,
                                                 iStatus);
                break;
                }
                
            case EConfirmDlgVisitorNetwork:
                {
                MPMLOGSTRING( "    Show visitor network confirm dialog" )
                iUtils->ConfirmMethodUsageQuery( iResult,
                                                 false,
                                                 iStatus);
                break;
                }
                
            default: 
                {
                return;
                }
            }
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CMPMConfirmDlg::~CMPMConfirmDlg
// -----------------------------------------------------------------------------
//
CMPMConfirmDlg::~CMPMConfirmDlg()
    {
    MPMLOGSTRING( "CMPMConfirmDlg::~CMPMConfirmDlg" )

    Cancel();
    delete iUtils;
    }

// -----------------------------------------------------------------------------
// CMPMConfirmDlg::RunL
// -----------------------------------------------------------------------------
//
void CMPMConfirmDlg::RunL()
    {
    if ( iStatus.Int() != KErrNone )
        {
        MPMLOGSTRING2( "CMPMConfirmDlg::RunL status %d", iStatus.Int() )
        }
    iConfirmDlgUser.ConfirmDlgResponse( iStatus.Int(), iResult );
    }

// -----------------------------------------------------------------------------
// CMPMConfirmDlg::RunError
// -----------------------------------------------------------------------------
//
TInt CMPMConfirmDlg::RunError( TInt aError )
    {
    MPMLOGSTRING2( "CMPMConfirmDlg::RunError, ERROR: %d", aError )
    aError = aError;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMPMConfirmDlg::DoCancel
// -----------------------------------------------------------------------------
//
void CMPMConfirmDlg::DoCancel()
    {
    MPMLOGSTRING( "CMPMConfirmDlg::DoCancel" )
    MPMLOGSTRING( "    Cancel normal confirm dialog" )

    iUtils->CancelConfirmMethodUsageQuery();
    }

// End of file
