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
* Description: 
*    Defines common methods.
*
*/



// INCLUDE FILES
#include <e32def.h>
#include <e32std.h>
#include <eikenv.h>
#include <ApSettingsHandlerCommons.h>
#include "ApSettingsHandlerUI.hrh"
#include <apsetui.rsg>
#include <AknQueryDialog.h>
#include <aknnotedialog.h>
#include <StringLoader.h>

#include <AknProgressDialog.h>
#include <AknWaitDialog.h>

#include "ApSettingsHandlerLogger.h"

void Panic( TApSettingsHandlerUiPanicCodes aPanic )
    {
    APSETUILOGGER_ENTERFN( EOther,"Commons::Panic")
    
    _LIT( kApSet, "APSettingsHandlerUi" ) ;
    User::Panic( kApSet, aPanic ) ;
    }


// ---------------------------------------------------------
// AskQueryL
// ---------------------------------------------------------
//
TInt AskQueryL( TInt aResId, TDesC* aVar )
    {
    return 0;
    }



// ---------------------------------------------------------
// ShowNoteL
// ---------------------------------------------------------
//
TInt ShowNoteL( TInt aResId, TDesC* aVar )
    {
    return 0;
    }


#ifdef __TEST_OOMDEBUG
    TInt GetTestStateL()
        {
        return 0;
        }
#endif // __TEST_OOMDEBUG

// End of File
