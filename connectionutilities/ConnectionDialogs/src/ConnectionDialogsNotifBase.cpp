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
* Description:  Implementation of base plugin.
*
*/


// INCLUDE FILES

#include <e32property.h>
#include <ScreensaverInternalPSKeys.h>
#include <coreapplicationuisdomainpskeys.h>
#include <bautils.h>
#include <eikenv.h>
#include <data_caging_path_literals.hrh>

#include "ConnectionDialogsLogger.h"
#include "ConnectionDialogsNotifBase.h"

#ifdef _DEBUG
#include <e32debug.h>
#endif

// CONSTANTS

// ROM folder
_LIT( KDriveZ, "z:" );


// ================= MEMBER FUNCTIONS =======================
    

// ---------------------------------------------------------
// CConnectionDialogsNotifBase::TNotifierInfo 
//                                  CConnectionDialogsNotifBase::Info() const
// ---------------------------------------------------------
//
CConnectionDialogsNotifBase::TNotifierInfo 
                                    CConnectionDialogsNotifBase::Info() const
    {
    return iInfo;
    }


// ---------------------------------------------------------
// TPtrC8 CConnectionDialogsNotifBase::StartL()
// ---------------------------------------------------------
//
TPtrC8 CConnectionDialogsNotifBase::StartL( const TDesC8& /*aBuffer*/ )
    {
    return KNullDesC8().Ptr();
    }

// ---------------------------------------------------------
// void CConnectionDialogsNotifBase::Cancel()
// ---------------------------------------------------------
//
void CConnectionDialogsNotifBase::Cancel()
    {
    }

// ---------------------------------------------------------
// TPtrC8 CConnectionDialogsNotifBase::UpdateL()
// ---------------------------------------------------------
//
TPtrC8 CConnectionDialogsNotifBase::UpdateL(const TDesC8& /*aBuffer*/)
    {
    return KNullDesC8().Ptr();
    }


// ---------------------------------------------------------
// void CConnectionDialogsNotifBase::Release()
// ---------------------------------------------------------
//
void CConnectionDialogsNotifBase::Release()
    {
    delete this;
    }


// ---------------------------------------------------------
// CConnectionDialogsNotifBase::CConnectionDialogsNotifBase()
// ---------------------------------------------------------
//
CConnectionDialogsNotifBase::CConnectionDialogsNotifBase()
: iCancelled( EFalse ), 
  iResource( 0 )
    {
    }


// ---------------------------------------------------------
// void CConnectionDialogsNotifBase::ConstructL()
// ---------------------------------------------------------
//
void CConnectionDialogsNotifBase::ConstructL( const TDesC& aResourceFileName,
                                         const TBool aResourceFileResponsible )
    {
    if ( aResourceFileResponsible )
        {
        // Since this is a .DLL, resource files that are to be used by the
        // notifier aren't added automatically so we do that here.
        TFileName fileName;

        fileName.Append( KDriveZ );
        fileName.Append( KDC_RESOURCE_FILES_DIR );   
        fileName.Append( aResourceFileName );

        BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(),
                                        fileName );
        iResource = CCoeEnv::Static()->AddResourceFileL( fileName );
        }
    }


// ---------------------------------------------------------
// CConnectionDialogsNotifBase::~CConnectionDialogsNotifBase()
// ---------------------------------------------------------
//
CConnectionDialogsNotifBase::~CConnectionDialogsNotifBase()
    {   
    if ( iResource )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResource );        
        }
    }

// ---------------------------------------------------------
// CConnectionDialogsNotifBase::ScreenSaverOn()
// ---------------------------------------------------------
//
TBool CConnectionDialogsNotifBase::ScreenSaverOn()
    {
    TInt err( KErrNone );
    TInt screenSaverOn( 0 );

    // Cancel the dialog if screen saver is on.
    err = RProperty::Get( KPSUidScreenSaver, 
            KScreenSaverOn, 
            screenSaverOn );
    
    return (err == KErrNone && screenSaverOn > 0); 
    }

// ---------------------------------------------------------
// CConnectionDialogsNotifBase::AutolockOn()
// ---------------------------------------------------------
//
TBool CConnectionDialogsNotifBase::AutolockOn()
    {
    TBool retval( EFalse );

    CLOG_ENTERFN( "CConnectionDialogsNotifBase::AutolockOn" );
    
#ifdef RD_STARTUP_CHANGE
    TInt err( KErrNone );
    TInt autolockOn( 0 );
    // Cancel the dialog if screen saver is on.
    err = RProperty::Get( KPSUidCoreApplicationUIs, 
            KCoreAppUIsAutolockStatus, 
            autolockOn );
    // In boot there may come EAutolockStatusUninitialized, when it should be EAutolockOn...
    retval = (err == KErrNone && autolockOn != EAutolockOff); 

#ifdef _DEBUG
    RDebug::Print( _L("CConnectionDialogsNotifBase::AutolockOn: autolockOn: %d"), autolockOn );
    RDebug::Print( _L("CConnectionDialogsNotifBase::AutolockOn: err: %d"), err );
    RDebug::Print( _L("CConnectionDialogsNotifBase::AutolockOn: %d"), retval );
#endif
    
#endif // RD_STARTUP_CHANGE
    
    CLOG_LEAVEFN( "CConnectionDialogsNotifBase::AutolockOn" );

    return retval; 
    }


// End of File
