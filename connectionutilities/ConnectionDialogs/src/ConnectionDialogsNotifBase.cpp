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

#include "ConnectionDialogsNotifBase.h"

#include <bautils.h>
#include <eikenv.h>
#include <data_caging_path_literals.hrh>


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
: iReplySlot( 0 ),
  iCancelled( EFalse ),
  iResource( 0 )
    {
    iInfo.iUid = TUid::Null();
    iInfo.iChannel = TUid::Null();
    iInfo.iPriority = 0;
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



// End of File
