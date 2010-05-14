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
* Description:   Implementation of cdccommsdatcreatorrfsplugin class.
*
*/



#include <centralrepository.h>

#include "cdccommsdatcreatorrfsplugin.h"
#include "cdclogger.h"
#include "cdcprocesslauncher.h"

// CONSTANTS

// ----------------------------------------------------------------------------
// CCdcCommsDatCreatorRfsPlugin::NewL
// ----------------------------------------------------------------------------
//
CCdcCommsDatCreatorRfsPlugin* CCdcCommsDatCreatorRfsPlugin::NewL( 
                                        const TUid& /*aImplementationUid*/ )
	{
    CLOG_WRITE( "CCdcCommsDatCreatorRfsPlugin::NewL()" );
	
	CCdcCommsDatCreatorRfsPlugin * self = new( ELeave ) 
               CCdcCommsDatCreatorRfsPlugin ();

    CLOG_WRITE( "CCdcCommsDatCreatorRfsPlugin::NewL()" );
    return self;
	}

// ----------------------------------------------------------------------------
// CCdcCommsDatCreatorRfsPlugin::RestoreFactorySettingsL
// ----------------------------------------------------------------------------
//
void CCdcCommsDatCreatorRfsPlugin::RestoreFactorySettingsL( 
                                                     const TRfsReason aType )
	{
    CLOG_WRITE_FORMAT( 
    "CCdcCommsDatCreatorRfsPlugin::RestoreFactorySettingsL() Rfs type: %d", aType );
        	
    if( aType == ENormalRfs )
    {
    	// Do nothing in Normal RFS case so that user IAPs are preserved (TSW: EJYG-7FZCKJ)
    	return;
    }
	// Launch the separate process to 1. reset 2. check/create the tables in CommsDat 
	// and 3. Generate the CommsDat based on the input file
   	StartProcessL();                        

    CLOG_WRITE( "CCdcCommsDatCreatorRfsPlugin::RestoreFactorySettingsL() end" );
	}

// ----------------------------------------------------------------------------
// CCdcCommsDatCreatorRfsPlugin::GetScriptL
// ----------------------------------------------------------------------------
//
void CCdcCommsDatCreatorRfsPlugin::GetScriptL( 
                                 const TRfsReason /*aType*/, TDes& /*aPath*/ )
	{
	}

// ----------------------------------------------------------------------------
// CCdcCommsDatCreatorRfsPlugin::ExecuteCustomCommandL
// ----------------------------------------------------------------------------
//
void CCdcCommsDatCreatorRfsPlugin::ExecuteCustomCommandL( 
                              const TRfsReason /*aType*/, TDesC& /*aCommand*/ )
	{
	}

// ----------------------------------------------------------------------------
// CCdcCommsDatCreatorRfsPlugin::CCdcCommsDatCreatorRfsPlugin
// ----------------------------------------------------------------------------
//
CCdcCommsDatCreatorRfsPlugin::CCdcCommsDatCreatorRfsPlugin () :
    CRFSPlugin()
	{
	}


// ----------------------------------------------------------------------------
// CCdcCommsDatCreatorRfsPlugin::StartProcessL()
// ----------------------------------------------------------------------------
//   
void CCdcCommsDatCreatorRfsPlugin::StartProcessL()
    {
    CLOG_WRITE( "CCdcCommsDatCreatorRfsPlugin::StartProcessL" );

    CCdcProcessLauncher* pl = CCdcProcessLauncher::NewL();

    CLOG_WRITE( "CCdcCommsDatCreatorRfsPlugin::Launch" );

    pl->Launch();

    CLOG_WRITE( "CCdcCommsDatCreatorRfsPlugin::Launch" );
    
    delete ( pl );

    CLOG_WRITE( "CCdcCommsDatCreatorRfsPlugin::StartProcessL end" );
    }

// ----------------------------------------------------------------------------
// CCdcCommsDatCreatorRfsPlugin::CCdcCommsDatCreatorRfsPlugin()
// ----------------------------------------------------------------------------
//   	
CCdcCommsDatCreatorRfsPlugin::~CCdcCommsDatCreatorRfsPlugin()
    {
    }

