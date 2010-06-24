/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
@file mpmconfirmdlgroaming.cpp
Roaming Dialog implementation
*/

// INCLUDE FILES
#include "mpmconfirmdlgroaming.h"
#include "mpmserversession.h"
#include "mpmconnmonevents.h"
#include "mpmlogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPMConfirmDlgRoaming::NewL
// -----------------------------------------------------------------------------
//
CMPMConfirmDlgRoaming* CMPMConfirmDlgRoaming::NewL( 
    CMPMServerSession&                  aSession,
    const TUint32                       aSnapId,
    const TUint32                       aIAP,
    const CMPMConfirmDlg::TDialogType   aDialogType, 
    const TBool                         aReconnect )
    {
    CMPMConfirmDlgRoaming* self = new ( ELeave ) CMPMConfirmDlgRoaming( 
                                                          aSession,
                                                          aSnapId,
                                                          aIAP,
                                                          aDialogType, 
                                                          aReconnect );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CMPMConfirmDlgRoaming::CMPMConfirmDlgRoaming
// -----------------------------------------------------------------------------
//
CMPMConfirmDlgRoaming::CMPMConfirmDlgRoaming( 
    CMPMServerSession&                  aSession,
    const TUint32                       aSnapId,
    const TUint32                       aIAP,
    const CMPMConfirmDlg::TDialogType   aDialogType, 
    const TBool                         aReconnect )
    : iSession( aSession),
      iSnapId( aSnapId ),
      iIAP( aIAP ),
      iDialogType( aDialogType ),
      iConfirmDlg( NULL ),
      iMsgQuery( EMsgQueryCancelled ),
      iError( 0 ),
      iReconnect( aReconnect )
    {
    }

// -----------------------------------------------------------------------------
// CMPMConfirmDlgRoaming::ConstructL
// -----------------------------------------------------------------------------
//
void CMPMConfirmDlgRoaming::ConstructL()
    {
    MPMLOGSTRING2("CMPMConfirmDlgRoaming<0x%x>::ConstructL()", iSession.ConnectionId())
    CArrayPtrFlat<CMPMDialogBase>* queue = ((CArrayPtrFlat<CMPMDialogBase>*)iSession.MyServer().RoamingQueue());
    iConfirmDlg = CMPMConfirmDlg::NewL( *queue,
                                        *this,
                                        iSnapId, 
                                        iIAP, 
                                        iDialogType );
    }


// -----------------------------------------------------------------------------
// CMPMConfirmDlgRoaming::~CMPMConfirmDlgRoaming
// -----------------------------------------------------------------------------
//
CMPMConfirmDlgRoaming::~CMPMConfirmDlgRoaming()
    {
    MPMLOGSTRING2( "CMPMConfirmDlgRoaming<0x%x>::~CMPMConfirmDlgRoaming", 
        iSession.ConnectionId() )
    delete iConfirmDlg;
    }


// -----------------------------------------------------------------------------
// CMPMConfirmDlgRoaming::ConfirmDlgResponse
// -----------------------------------------------------------------------------
//
void CMPMConfirmDlgRoaming::ConfirmDlgResponse( 
    const TInt                      aError,
    const TMsgQueryLinkedResults    aResult )
    {
    MPMLOGSTRING4( "CMPMConfirmDlgRoaming<0x%x>::ConfirmDlgResponse\
 error: %d result: %d", iSession.ConnectionId(), aError, aResult )

    // Store results
    //
    iError = aError;
    iMsgQuery = aResult;

    HandleResponse( aError, aResult );

    // We are done
    // 
    iSession.SetConfirmDlgRoamingPtrNull();
    delete this;
    }


// -----------------------------------------------------------------------------
// CMPMConfirmDlgRoaming::ConfirmDlgResponse
// -----------------------------------------------------------------------------
//
TBool CMPMConfirmDlgRoaming::ConfirmDlgResponse( MMPMConfirmDlg* aDlg )
    {
    TBool ret( EFalse );
    CMPMConfirmDlgRoaming* dlg = (CMPMConfirmDlgRoaming*)aDlg;
    if ( dlg && iIAP == dlg->iIAP )
        {
        ConfirmDlgResponse( dlg->iError, dlg->iMsgQuery );
        ret = ETrue;
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CMPMConfirmDlgRoaming::HandleResponse
// -----------------------------------------------------------------------------
//
void CMPMConfirmDlgRoaming::HandleResponse( const TInt                      aError,
                                            const TMsgQueryLinkedResults    aResult )
    {
    MPMLOGSTRING5( "CMPMConfirmDlgRoaming<0x%x>::HandleResponse: aError %d, aResult %d, dlg type %d", 
                   iSession.ConnectionId(),
                   aError,
                   aResult,
                   iDialogType )
                   
    TInt error( KErrNone );

    if( aError == KErrCancel )
        {
        TRAP(error, iSession.RoamingConfirmationCompletedL( KErrNone, 
                                                            EMsgQueryCancelled, 
                                                            iReconnect ) )        
        }
    else
        {
        TRAP(error, iSession.RoamingConfirmationCompletedL( aError, 
                                                            aResult, 
                                                            iReconnect ) )        
        }
    if( error != KErrNone )
        {
        MPMLOGSTRING2( "CMPMConfirmDlgRoaming<0x%x>::HandleResponse: error occurred %d", 
                       error )
        }
    }

// End of file
