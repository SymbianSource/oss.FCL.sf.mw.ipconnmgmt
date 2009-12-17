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
#include    "ApSettingsHandlerLogger.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFeatureManagerWrapper::CFeatureManagerWrapper
// ---------------------------------------------------------
//
CFeatureManagerWrapper::CFeatureManagerWrapper() : CBase()
    {
    APSETUILOGGER_ENTERFN( EOther,"FeatureWrapper::CFeatureManagerWrapper<->")
    }

// ---------------------------------------------------------
// CFeatureManagerWrapper::~CFeatureManagerWrapper
// ---------------------------------------------------------
//
CFeatureManagerWrapper::~CFeatureManagerWrapper() 
    {
    APSETUILOGGER_ENTERFN( EOther,"FeatureWrapper::~CFeatureManagerWrapper")
    
    if ( iIsFeatureMgrInitialized )
        {
        FeatureManager::UnInitializeLib();    
        }
    
    APSETUILOGGER_LEAVEFN( EOther,"FeatureWrapper::~CFeatureManagerWrapper")
    }


// ---------------------------------------------------------
// CFeatureManagerWrapper::ConstructL
// ---------------------------------------------------------
//
void CFeatureManagerWrapper::ConstructL()
    {
    APSETUILOGGER_ENTERFN( EOther,"FeatureWrapper::ConstructL")
    
    FeatureManager::InitializeLibL();
    iIsFeatureMgrInitialized = ETrue ;
    
    APSETUILOGGER_LEAVEFN( EOther,"FeatureWrapper::ConstructL")
    }


// ---------------------------------------------------------
// CFeatureManagerWrapper::NewL
// ---------------------------------------------------------
//
CFeatureManagerWrapper*  CFeatureManagerWrapper::NewL()
    {
    APSETUILOGGER_ENTERFN( EOther,"FeatureWrapper::NewL")
    
    CFeatureManagerWrapper* self = new ( ELeave ) CFeatureManagerWrapper;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    APSETUILOGGER_LEAVEFN( EOther,"FeatureWrapper::NewL")
    return self;
    }

//End of file.

