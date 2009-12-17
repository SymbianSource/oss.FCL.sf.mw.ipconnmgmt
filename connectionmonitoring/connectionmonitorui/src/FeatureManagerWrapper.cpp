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
* Description:  Implementation of CFeatureManagerWrapper.
*
*/


// INCLUDE FILES
#include    "FeatureManagerWrapper.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFeatureManagerWrapper::CFeatureManagerWrapper
// ---------------------------------------------------------
//
CFeatureManagerWrapper::CFeatureManagerWrapper() : CBase()
    {
    }

// ---------------------------------------------------------
// CFeatureManagerWrapper::~CFeatureManagerWrapper
// ---------------------------------------------------------
//
CFeatureManagerWrapper::~CFeatureManagerWrapper() 
    {
    if ( iIsFeatureMgrInitialized )
        {
        FeatureManager::UnInitializeLib();    
        }
    }

// ---------------------------------------------------------
// CFeatureManagerWrapper::ConstructL
// ---------------------------------------------------------
//
void CFeatureManagerWrapper::ConstructL()
    {
    FeatureManager::InitializeLibL();
    iIsFeatureMgrInitialized = ETrue ;
    }

// ---------------------------------------------------------
// CFeatureManagerWrapper::NewL
// ---------------------------------------------------------
//
CFeatureManagerWrapper*  CFeatureManagerWrapper::NewL()
    {
    CFeatureManagerWrapper* self = new ( ELeave ) CFeatureManagerWrapper;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

//End of file.

