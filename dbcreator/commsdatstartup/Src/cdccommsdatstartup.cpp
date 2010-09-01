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
#include "cdccommsdatcreatorcrkeys.h"

#include "cdccommsdatstartup.h"
#include "cdccommsdatcreator.h"
#include "cdcerrors.h"
#include "cdclogger.h"


// CONSTANTS
const TUid KCRUidCommsDb = { 0xcccccc00 };

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

    CreateRepositoryL();
    
    // Set status to not initialised
    SetStartupStatusL( ECommsDatInitialisationFailed );

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
	
    TFileName fileName;
    GetInputFileNameL( fileName );
        
    // CommsDatCreator feature is supported if input file name
    // lenght is not 0. However file name lenght at least 5. (x.xml)     
    if( fileName.Length() > 0 )
        {
        CLOG_WRITE( "CCdcCommsDatStartup::CommsDat generation enabled" );
        
        TFileName path( _L("Z:\\private\\10281BC3\\" )  );
        path.Append( fileName );

		CLOG_WRITE( "CCdcCommsDatStartup::CreateCommsDatL begin" );
	
        TRAPD(err, cdc->CreateCommsDatL( path ) );
        if(err != KErrNone )
        	{
        	// CommsDat creation leaved so something must be wrong.
        	// Reset CommsDat and EAP settings.
        	ResetCommsDatToDefaultL();
        	ResetEAPSettingsL();
        	cdc->CreateTablesL();
        	User::Leave( err );
        	}

        CLOG_WRITE( "CCdcCommsDatStartup::CreateCommsDatL end" );               
        }
       else
       	{
       	CLOG_WRITE( "CCdcCommsDatStartup::CommsDat generation disabled" );
       	}
        
    // If nothing has leaved then everything is okay (both in disabled and
    // enabled cases.
    SetStartupStatusL( ECommsDatInitialised );
        
    CleanupStack::PopAndDestroy( cdc );
    CLOG_WRITE( "CCdcCommsDatStartup::DoStartL end" );
    }

// ----------------------------------------------------------------------------
// CCdcCommsDatStartup::CreateRepositoryL()
// ----------------------------------------------------------------------------
//    
void CCdcCommsDatStartup::CreateRepositoryL()
    {
    CLOG_WRITE( "CCdcCommsDatStartup::CreateRepositoryL()" );

    TRAPD( err, iRepository = CRepository::NewL( KCRUidCommsDatCreator ) );
    
    // Log is important now, because repository cannot contain the 
    // error value if it cannot be used 
    if( err != KErrNone )
        {
        CLOG_WRITE_FORMAT( "Repository could not created err : %d", err );
        User::Leave( err );
        }
    }

// ----------------------------------------------------------------------------
// CCdcCommsDatStartup::SetStartupStatusL()
// ----------------------------------------------------------------------------
//    
void CCdcCommsDatStartup::SetStartupStatusL( TInt aResult )
    {
    CLOG_WRITE_FORMAT( "CCdcCommsDatStartup::SetStartupStatusL() %d", aResult );

    TInt err = iRepository->Set( KCommsDatCreatorStartupStatus, aResult );
    if( err != KErrNone )
        {
        CLOG_WRITE_FORMAT( 
                "Value could not write to repository err : %d", err );
        User::Leave( err );
        }

    CLOG_WRITE_FORMAT( "CCdcCommsDatStartup::SetStartupStatusL() %d", aResult );
    }

// ----------------------------------------------------------------------------
// CCdcCommsDatStartup::GetInputFileNameL()
// ----------------------------------------------------------------------------
//    
void CCdcCommsDatStartup::GetInputFileNameL( TFileName &aFileName )
    {
    CLOG_WRITE( "CCdcCommsDatStartup::GetInputFileNameL()" );

    TInt err = iRepository->Get( KCommsDatCreatorInputFileName, aFileName );
    if( err != KErrNone )
        {
        CLOG_WRITE_FORMAT(
                 "Value could not red from repository err : %d", err );
        User::Leave( err );
        }

    CLOG_WRITE( "Input filename red" );

    HBufC *fileName = aFileName.AllocLC();

    CLOG_WRITE_FORMAT( "Input filename value : %S", fileName );
    
    CleanupStack::PopAndDestroy( fileName );
    
    // Check if file exists. If it doesn't reset filename.
    RFs fs;
    err = fs.Connect();//check
    
    if( err == KErrNone )
    	{
    	RFile file;
    
    	TFileName path( _L("Z:\\private\\10281BC3\\" )  );
    	path.Append( aFileName );

    	err = file.Open(fs, path, EFileRead);
    	if( err != KErrNone )
    	{
	    	CLOG_WRITE( "CCdcCommsDatStartup::GetInputFileNameL() File does not exist");
  	  	aFileName.Zero();
    	}
    	else
    	{
    		file.Close();
    	}
    	fs.Close();
    	}
    
    CLOG_WRITE( "CCdcCommsDatStartup::GetInputFileNameL()" );
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
    
    TUid creatorId = User::CreatorIdentity();

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
    if( iRepository )
        {
        delete iRepository;
        }
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
