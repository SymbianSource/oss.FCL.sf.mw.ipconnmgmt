/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
*      Implementation of utilities functions.   
*      
*
*/


// INCLUDE FILES
#include "utilities.h"

#include <ErrorUI.h>
#include <AknGlobalNote.h>
#include <StringLoader.h>
#include <ApSettingsPluginRsc.rsg>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Utilities::ShowErrorNote
// ---------------------------------------------------------
//
void Utilities::ShowErrorNote(TInt aErrorCode)
    {
    TBool errorNoteShowed = EFalse;
        
    TRAPD(error, errorNoteShowed = ShowErrorNoteL(aErrorCode));
            
    if(error || !errorNoteShowed) 
        {
        TRAP_IGNORE( ShowGeneralErrorNoteL() );
        }    
    }

// ---------------------------------------------------------
// Utilities::ShowErrorNoteL
// ---------------------------------------------------------
//
TBool Utilities::ShowErrorNoteL(TInt aErrorCode)
    {
    CErrorUI* errorUi = CErrorUI::NewLC();
    
    TBool errorNoteShowed = errorUi->ShowGlobalErrorNoteL( aErrorCode );
    
    CleanupStack::PopAndDestroy( errorUi );
        
    return errorNoteShowed;
    }    

// ---------------------------------------------------------
// Utilities::ShowGeneralErrorNoteL
// ---------------------------------------------------------
//
void Utilities::ShowGeneralErrorNoteL()
    {
    HBufC *msg = StringLoader::LoadLC( R_QTN_ERR_OS_GENERAL );

    CAknGlobalNote *note = CAknGlobalNote::NewLC();
    
    
    note->ShowNoteL( EAknGlobalErrorNote, *msg );


    CleanupStack::PopAndDestroy( note );
    CleanupStack::PopAndDestroy( msg );
    }
