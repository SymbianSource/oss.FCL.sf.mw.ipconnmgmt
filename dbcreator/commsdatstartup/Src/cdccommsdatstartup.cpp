/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of CCommsDatStartup class.
*
*/




// INCLUDE FILES

#include <e32def.h>
#include <e32base.h>
#include <s32strm.h>
#include <s32file.h>
#include <etelpckt.h>

#include <centralrepository.h>

#include "cdccommsdatstartup.h"
#include "cdccommsdatcreator.h"
#include "cdcerrors.h"
#include "cdclogger.h"


// CONSTANTS
const TUid KCRUidCommsDb = { 0xcccccc00 };
_LIT( KVariantXmlFile, "Z:\\private\\10281BC3\\VariantData_commsdat.xml" );

//------------------------------------------------
// CCdcCommsDatStartup::DoStartL
//------------------------------------------------
//
void CCdcCommsDatStartup::DoStartL()
    {
    CLOG_WRITE( "-----------------------------" );
    CLOG_WRITE( "CCdcCommsDatStartup::DoStartL" );
    CLOG_WRITE( "-----------------------------" );
    
    CheckCreatorUIdL();

	CLOG_WRITE( "CCdcCommsDatStartup::ResetCommsDatToDefaultL()" );
	// In all RFS cases CommsDat must be reset 
	ResetCommsDatToDefaultL();
	
	// Reset EAP settings databases
	ResetEAPSettingsL();

	CLOG_WRITE( "CCdcCommsDatStartup::CCdcCommsDatCreator::NewLC()" );
	    
	CCdcCommsDatCreator* cdc = CCdcCommsDatCreator::NewLC();
	
	CLOG_WRITE( "CCdcCommsDatStartup::CreateTablesL" );
	
	// In all RFS cases CommsDat tables must be checked/created
	cdc->CreateTablesL();
	
    // If variant XML file exists, read it and create settings
    TFileName fileName( KVariantXmlFile );
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    RFile file;
    if ( KErrNone == file.Open( fs, fileName, EFileRead ) )
        {
        file.Close();
        CLOG_WRITE( "CCdcCommsDatStartup::CommsDat generation enabled" );
    
        TRAPD(err, cdc->CreateCommsDatL( fileName ) );
        if ( err != KErrNone )
            {
            // CommsDat creation leaved so something must be wrong.
            // Reset CommsDat and EAP settings.
            ResetCommsDatToDefaultL();
            ResetEAPSettingsL();
            cdc->CreateTablesL();
            User::Leave( err );
            }

        CLOG_WRITE( "CCdcCommsDatStartup::CommsDat generation done" );
        }
    else
        {
        CLOG_WRITE( "CCdcCommsDatStartup::CommsDat generation disabled" );
        }
    fs.Close();
        
    CleanupStack::PopAndDestroy( cdc );
    CLOG_WRITE( "CCdcCommsDatStartup::DoStartL end" );
    }

// ----------------------------------------------------------------------------
// CCdcCommsDatStartup::CheckCreatorUIdL()
// ----------------------------------------------------------------------------
//   	
void CCdcCommsDatStartup::CheckCreatorUIdL()
    {
    // UID of MainRfs
    const TUid KCreatorUid = { 0x102073EA };   
    
    CLOG_WRITE( "CCdcCommsDatStartup::CheckCreatorUIdL" );
    
    TUid creatorId = User::CreatorSecureId();

    CLOG_WRITE_FORMAT( "CreatorId : %x", creatorId );
   
    if( KCreatorUid != creatorId )
        {
        User::Leave( KErrPermissionDenied );
        }

    CLOG_WRITE( "CCdcCommsDatStartup::CheckCreatorUIdL" );
    }
    
// ----------------------------------------------------------------------------
// CCdcCommsDatStartup::~CCdcCommsDatStartup()
// ----------------------------------------------------------------------------
//   	
CCdcCommsDatStartup::~CCdcCommsDatStartup()
    {
    }

// ----------------------------------------------------------------------------
// CCdcCommsDatCreatorRfsPlugin::ResetCommsDatToDefaultL()
// ----------------------------------------------------------------------------
//   	
void CCdcCommsDatStartup::ResetCommsDatToDefaultL()
	{		
	CLOG_WRITE( "CCdcCommsDatStartup::ResetCommsDatToDefaultL" );
	
	CRepository* repository( NULL );
    
    repository = CRepository::NewL( KCRUidCommsDb );    
    CleanupStack::PushL( repository );

    CLOG_WRITE("Reset CommsDat CenRep");

    User::LeaveIfError( repository->Reset() );

    CleanupStack::PopAndDestroy( repository );

    CLOG_WRITE( "CCdcCommsDatStartup::ResetCommsDatToDefaultL end" );    
	}

// ----------------------------------------------------------------------------
// CCdcCommsDatCreatorRfsPlugin::ResetEAPSettingsL()
// ----------------------------------------------------------------------------
//   	
void CCdcCommsDatStartup::ResetEAPSettingsL()
{
	CLOG_WRITE( "CCdcCommsDatStartup::ResetEAPSettingsL" );
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );
	
	CFileMan *fileMan = CFileMan::NewL( fs );
	CleanupStack::PushL( fileMan );
	
	_LIT( KEapWildcard, "c:\\private\\100012a5\\DBS_102072E9_eap*" );
	
	TInt err = fileMan->Delete( KEapWildcard );
	
	if ( err != KErrNone 
		 && err != KErrNotFound 
		 && err != KErrPathNotFound )
		{
		User::Leave( err );
		}
			
	CleanupStack::PopAndDestroy( fileMan );
	CleanupStack::PopAndDestroy(); // fs		
	CLOG_WRITE( "CCdcCommsDatStartup::ResetEAPSettingsL end" );
}

// End of File.
