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
* Description:  application class
*     
*
*/


// INCLUDE FILES
#include "ConnectionMonitorUiDocument.h"
#include "ConnectionMonitorUiAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// constructor
CConnectionMonitorUiDocument::CConnectionMonitorUiDocument
    (
    CEikApplication& aApp
    )
: CAknDocument(aApp)    
    {
    }

// destructor
CConnectionMonitorUiDocument::~CConnectionMonitorUiDocument()
    {
    }

// EPOC default constructor can leave.
void CConnectionMonitorUiDocument::ConstructL()
    {
    }

// Two-phased constructor.
CConnectionMonitorUiDocument* CConnectionMonitorUiDocument::NewL(
        CEikApplication& aApp)     // CConnectionMonitorUiApp reference
    {
    CConnectionMonitorUiDocument* myself =
        new (ELeave) CConnectionMonitorUiDocument( aApp );
    CleanupStack::PushL( myself );
    myself->ConstructL();
    CleanupStack::Pop( myself );

    return myself;
    }
    
// ----------------------------------------------------
// CConnectionMonitorUiDocument::CreateAppUiL()
// constructs CConnectionMonitorUiAppUi
// ----------------------------------------------------
//
CEikAppUi* CConnectionMonitorUiDocument::CreateAppUiL()
    {
    return new (ELeave) CConnectionMonitorUiAppUi;
    }

// End of File  
