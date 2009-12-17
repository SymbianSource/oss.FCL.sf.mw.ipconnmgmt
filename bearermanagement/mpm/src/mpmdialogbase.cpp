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
* Description: MPM user dialog handling
*
*/

/**
@file mpmdialogbase.cpp
Mobility Policy Manager user dialog initiation.
*/

// INCLUDE FILES
#include "mpmdialogbase.h"

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPMDialogBase::CMPMDialogBase
// -----------------------------------------------------------------------------
//
CMPMDialogBase::CMPMDialogBase( CArrayPtrFlat<CMPMDialogBase>& aQueue )
    :   CActive( CActive::EPriorityStandard ),
        iQueue( aQueue )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CMPMDialog::ConstructL
// -----------------------------------------------------------------------------
//
void CMPMDialogBase::ConstructL()
    {
    // Start dialog immediately if the first one and otherwise
    // just append this dialog into the queue
    if ( iQueue.Count() == 0 ) 
        {
        MPMLOGSTRING2( "CMPMDialogBase<0x%x>::ConstructL() Display Dialog immediately", this )
        iQueue.AppendL( this );
        Start();
        }
    else
        {
        MPMLOGSTRING2( "CMPMDialog<0x%x>::ConstructL() Append Dialog to queue", this )
        iQueue.AppendL( this );
        }
    }

// -----------------------------------------------------------------------------
// CMPMDialog::~CMPMDialog
// -----------------------------------------------------------------------------
//
CMPMDialogBase::~CMPMDialogBase()
    {
    // Cancel everything first if we were running.
    //
    if ( iQueue.Count() > 0 && iQueue.At( 0 ) == this )
        {
        Cancel();
        MPMLOGSTRING2( "CMPMDialogBase::~CMPMDialogBase: deleted, Completing with code = %i", 
                       iStatus.Int() )
        // Remove this from the queue
        iQueue.Delete( 0 );
        // And if there are still pending dialogs, start the first one.
        if ( iQueue.Count() > 0 )
            {
            // Give pointer to this dialog in case
            // the starting dialog can utilize the
            // result from this dialog.
            iQueue.At(0)->Start( this );
            }
        }
    // We're not the first one. We can just delete ourselves.
    //
    else 
        {
        for ( TInt i = 0; i < iQueue.Count(); i++ )
            {
            if ( iQueue.At( i ) == this )
                {
                iQueue.Delete( i );
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPMDialog::IsFirst
// -----------------------------------------------------------------------------
//
TBool CMPMDialogBase::IsFirst()
    {
    if ( iQueue.Count() == 0 )
        {
        return EFalse;
        }
    else
        {
        return iQueue.At( 0 ) == this;
        }
    }

//  End of File
