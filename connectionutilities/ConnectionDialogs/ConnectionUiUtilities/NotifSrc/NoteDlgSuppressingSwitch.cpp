/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class CNoteDlgSuppressingSwitch.
*
*/


// INCLUDE FILES
#include "NoteDlgSuppressingSwitch.h"

#include <uikon/eiksrvui.h>


// ================= MEMBER FUNCTIONS =======================
//
// ---------------------------------------------------------
// CNoteDlgSuppressingSwitch::CNoteDlgSuppressingSwitch
// ---------------------------------------------------------
//
CNoteDlgSuppressingSwitch::CNoteDlgSuppressingSwitch( CEikDialog** aSelfPtr )
: CAknNoteDialog( aSelfPtr )
    {
    }


// ---------------------------------------------------------
// CNoteDlgSuppressingSwitch::~CNoteDlgSuppressingSwitch
// ---------------------------------------------------------
//
CNoteDlgSuppressingSwitch::~CNoteDlgSuppressingSwitch()
    {
    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( EFalse );
    }


// ---------------------------------------------------------
// CNoteDlgSuppressingSwitch::PreLayoutDynInitL()
// ---------------------------------------------------------
//
void CNoteDlgSuppressingSwitch::PreLayoutDynInitL()
    {
    CAknNoteDialog::PreLayoutDynInitL();

    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( ETrue );
    }


// End of File
