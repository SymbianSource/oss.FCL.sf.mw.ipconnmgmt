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
* Description:  CConnectionMonitorUiApp application class
*     
*
*/


// INCLUDE FILES
#include    <eikstart.h>
#include    "ConnectionMonitorUiApp.h"
#include    "ConnectionMonitorUiDocument.h"

// ---------------------------------------------------------
// Panic
// ---------------------------------------------------------
//
void Panic( TInt aReason )
    {
    _LIT( KAppName, "Conn. Monitor" );
    User::Panic( KAppName, aReason );
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CConnectionMonitorUiApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CConnectionMonitorUiApp::AppDllUid() const
    {
    return KUidConnectionMonitorUi;
    }

// ---------------------------------------------------------
// CConnectionMonitorUiApp::CreateDocumentL()
// Creates CConnectionMonitorUiDocument object
// ---------------------------------------------------------
//
CApaDocument* CConnectionMonitorUiApp::CreateDocumentL()
    {
    return CConnectionMonitorUiDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============


LOCAL_C CApaApplication* NewApplication()
    {
    return new CConnectionMonitorUiApp;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }


// End of File  

