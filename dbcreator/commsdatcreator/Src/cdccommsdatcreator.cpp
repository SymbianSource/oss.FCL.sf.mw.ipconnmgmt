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
* Description:   Module for creating CommsDat from an input file
*
*/




// INCLUDE FILES

#include "cdccommsdatcreator.h"
#include "cdclogger.h"
#include "cdcreaderxml.h"
#include "cdcprocessorpacketdata.h"
#include "cdcprocessorwlan.h"
#include "cdcprocessorlan.h"
#include "cdcprocessorvpn.h"
#include "cdcprocessordn.h"
#include "cdcprocessorglobal.h"
#include "cdcwlantables.h"
#include "cdcwlandevicesettings.h"
#include "cdcdatamobilitytables.h"
#include "cdcdumptables.h"
#include "centralrepository.h"

#include <cmconnectionmethoddef.h>
#include <cmmanagerext.h>

#include <e32def.h>
#include <e32base.h>
#include <s32strm.h>
#include <s32file.h>
#include <etelpckt.h>
#include <featmgr.h>
#include <commsdattypesv1_1.h>
#include <commsdattypeinfov1_1.h>


// CONSTANTS
//maximum length of access point name
const TInt KLengthOfExtention = 4;

//extension
_LIT16( KExtensionXML,     ".xml" );    //XML file


//-----------------------------------------------------------------------------
//  CCdcCommsDatCreator::NewL()
//-----------------------------------------------------------------------------
//
EXPORT_C CCdcCommsDatCreator* CCdcCommsDatCreator::NewL()
    {
    CCdcCommsDatCreator* self = NewLC();
    CleanupStack::Pop( self );   // self
    
    return self;
    }


//-----------------------------------------------------------------------------
//  CCdcCommsDatCreator::NewLC()
//-----------------------------------------------------------------------------
//
EXPORT_C CCdcCommsDatCreator* CCdcCommsDatCreator::NewLC()
    {
    CCdcCommsDatCreator* self = new (ELeave) CCdcCommsDatCreator();
    CleanupStack::PushL( self );
    self->ConstructL();
    
    return self;
    }


//------------------------------------------------
// CDbCreator::CreateTablesL
//------------------------------------------------
//
EXPORT_C void CCdcCommsDatCreator::CreateTablesL()
    {
    CLOG_WRITE( "=====================\n" )
    CLOG_WRITE( "Creating tables ...\n" )
    CLOG_WRITE( "=====================\n" )   

    
    //extends architecture of CommsDat
    //creates WLAN tables
    //-------------------------------
    if ( iIsWlanS )
        {
        CLOG_WRITE( "Creates WLAN service tables\n" )
        CLOG_WRITE( "---------------------------\n" )
        
        CWlanTables* wlanTables = CWlanTables::NewL();
        CleanupStack::PushL( wlanTables );
        wlanTables->CheckAndCreateTablesL();
        
        CleanupStack::PopAndDestroy( wlanTables ); 
        
        // WLAN Device Settings Table.
        CLOG_WRITE( "Writes Default settings to WLAN Device Settings Table.\n" )
        
        // Creating a settings object does it all.
        CWlanDeviceSettings* wlanDeviceSettings = CWlanDeviceSettings::NewL();
        delete wlanDeviceSettings;
        }

    CLOG_WRITE( "CDataMobilityTables\n" )   
        
    //extends architecture of CommsDat
    //creates Data Mobility tables
    //----------------------------
    CDataMobilityTables* dmTables = CDataMobilityTables::NewL();
    CleanupStack::PushL( dmTables );
    
    CLOG_WRITE( "Creates Data Mobility ALR tables\n" )
    CLOG_WRITE( "--------------------------------\n" )
    dmTables->CheckAndCreateAlrTablesL();
    
    CLOG_WRITE( "Creates Data Mobility MIP tables\n" )
    CLOG_WRITE( "--------------------------------\n" )
    dmTables->CheckAndCreateMipTablesL();
    
    CleanupStack::PopAndDestroy( dmTables );
        
    //if there is only an "e" parameter then DbCreator extends CommsDat, but
    //does not create access points. This functionality earlier was done
    //by WlanToCDb modul.
    }


//------------------------------------------------
// CDbCreator::CreateCommsDatL
//------------------------------------------------
//
EXPORT_C void CCdcCommsDatCreator::CreateCommsDatL( const TFileName& aInFileName )
    {
    CLOG_WRITE( "=====================\n" )
    CLOG_WRITE( "Processing started...\n" )
    CLOG_WRITE( "=====================\n" )               

	iCmManager.OpenL();

    TDbCreatorInputCharSet charSet( ECharSetUnknown );//input character set

    //Creates access points
    //----------------------
            
    if ( InputFileType( aInFileName ) == EFileXML )
        {
        CLOG_WRITE( "Tries to open file");
        // Logs file name            
        HBufC *fileName = aInFileName.AllocLC();
        CLOG_WRITE_FORMAT( "Input file : %S\n", fileName )
        CleanupStack::PopAndDestroy( fileName );                          
        
        delete iReader;
        iReader = NULL;
        iReader = CReaderXML::NewLC( aInFileName, charSet );
    	CleanupStack::Pop(iReader);
    
        //locates feature in xml file
        iReader->LocateFeatureL();
        }
    else    
        {
        User::Leave( KErrInvalidExtension );
        }  
    	

    TBool found( EFalse );
    
    // First, add all destinations to the database. Connection methods and
    // embedded destinations are not added at this time.
    while ( iReader->CurrentFeature() != EFeatureNone && !found )
        {
        if( iReader->CurrentFeature() == EFeatureDN )
            {
            ProcessDNL( iCmInDest, ETrue );
            }
        iReader->LocateFeatureL();
        }
    
    // Reset input file
    iReader->Reset();
    iReader->LocateFeatureL();
    
    // Then add connection methods, and assign them to destinations if
    // needed
    while ( iReader->CurrentFeature() != EFeatureNone )
        {
        switch ( iReader->CurrentFeature() )
            {
            case EFeaturePacketData:
                {
                //creates GPRS access point
                CLOG_WRITE( "Feature found : Packet Data Access point(s)\n" )
                
                iFileProcessor = CProcessorPacketData::NewL( iReader,
                                                       iCmManager,
                                                       iPluginArray,
                                                       iPluginNames,
                                                       iLingerSettings );
                iFileProcessor->ParseFileL();
                
                delete iFileProcessor;
                iFileProcessor = NULL;

                break;
                }
                
            case EFeatureWLAN:
                {
                if( iIsWlanS )
                    {
                    //creates WLAN access point   
                    CLOG_WRITE( "Feature found : WLAN Access point(s)\n" )
                    
                    // Reset the EAP settings just to be safe although it should be empty always here.
                    iEapSettings.ResetAndDestroy();
                    
                    iFileProcessor = CProcessorWlan::NewL( iReader,
                                                           iCmManager,
                                                           iPluginArray,
                                                           iPluginNames,
                                                           iWlanSecurity,
                                                           iEapSettings );
                    iFileProcessor->ParseFileL();
                    
                    delete iFileProcessor;
                    iFileProcessor = NULL;

                    }
                break;
                }
                
            case EFeatureLAN:
                {
                //creates LAN access point   
                CLOG_WRITE( "Feature found : LAN Access point(s)\n" )

                iFileProcessor = CProcessorLan::NewL( iReader,
                                                      iCmManager,
                                                      iPluginArray,
                                                      iPluginNames );
                iFileProcessor->ParseFileL();
               
                delete iFileProcessor;
                iFileProcessor = NULL;
                break;
                }
                
            case EFeatureVPN:
                {
                //creates VPN access point   
                CLOG_WRITE( "Feature found : VPN Access point(s)\n" )
                CLOG_WRITE( "-----------------------------------\n" )
                
                iFileProcessor = CProcessorVpn::NewL( iReader,
                                                       iCmManager,
                                                       iPluginArray,
                                                       iPluginNames,
                                                       iUnderLying );
                iFileProcessor->ParseFileL();
                
                delete iFileProcessor;
                iFileProcessor = NULL;
                
                break;
                }
            
            case EFeatureDN:
                {
               
                ProcessDNL( iCmInDest, EFalse );
                
                break;
                }
                
            case EFeatureGlobal:
                {
                // Valid tag but does nothing now. 
                // Global can be processed at the last end.

                
                //sets global settings 
                CLOG_WRITE( "Feature found : Global settings\n" )
                CLOG_WRITE( "-------------------------------\n" )
                    
                iFileProcessor = CProcessorGlobal::NewL( iReader,
                                                          iCmManager,
                                                          iPluginArray,
                                                          iPluginNames,
                                                          iDestArray,
                                                          iDestNames,
                                                          iDefCon ); 
                iFileProcessor->ParseFileL();

                delete iFileProcessor;
                iFileProcessor = NULL;

                break;
                }
                
            default:
                User::Panic( KPanicInvalidCase, KErrInvalidCase );
             
            }//switch
            
            //gets next feature
            iReader->LocateFeatureL();
            
        }//while
                  

    // Updates the IAPs that are not in destinations.
    UpdateUncatCmsL( iCmInDest /*, iPluginNames, iPluginArray*/ );

    // Sets the underlying IAP/SNAPS for vpn IAPs    
    SetUnderlyingIapL( iUnderLying );

    for( TInt i(0); i < iWlanSecurity.Count(); i++ )
        {
        CLOG_WRITE_FORMAT( "Sec Pos: %d", i );
        if( iWlanSecurity[i] != NULL )
            {
            CLOG_WRITE_FORMAT( "Sec data: %S", iWlanSecurity[i] );
            }
        else    
            {
            CLOG_WRITE( "Sec data NULL" );
            }
        }

	// Sets the WLAN security paramters including EAP settings
    SetWlanSecurityL( iWlanSecurity );
    
    // Sets linger settings
    SetLingerSettingsL( iLingerSettings );

    // Sets the default connection    
    SetDefaultConnectionL( iDefCon );

    // Logs IAPs in UDEB mode
    #ifdef _DEBUG
    LogIapIdsL();
    #endif
}

//------------------------------------------------
// CCdcCommsDatCreator::DumpCommsDatL
//------------------------------------------------
//
EXPORT_C void CCdcCommsDatCreator::DumpCommsDatL( const TFileName& aOutFileName )
    {
    CLOG_WRITE( "==========================\n" )
    CLOG_WRITE( "Dump processing started...\n" )
    CLOG_WRITE( "==========================\n" )   
    

    CDumpTables* dump = CDumpTables::NewLC();
    dump->DoDumpTablesL( aOutFileName );
    CleanupStack::PopAndDestroy( dump ); //dump   

    CLOG_WRITE( "======== end dump =========\n" )   

    }

//------------------------------------------------
// CCdcCommsDatCreator::CCdcCommsDatCreator
//------------------------------------------------
//
CCdcCommsDatCreator::CCdcCommsDatCreator()
    {
    }

//------------------------------------------------
// CCdcCommsDatCreator::~CCdcCommsDatCreator
//------------------------------------------------
//
CCdcCommsDatCreator::~CCdcCommsDatCreator()
    {
    delete iReader;
    
    iEapSettings.ResetAndDestroy();

	for ( TInt i = 0; i < iPluginArray.Count(); i++ )
	{
		iPluginArray[i]->Close();
	}
    iPluginArray.ResetAndDestroy();
    iPluginNames.ResetAndDestroy();

	for ( TInt i = 0; i < iDestArray.Count(); i++ )
	{
		iDestArray[i]->Close();
	}
    iDestArray.ResetAndDestroy();
    
    iDestNames.ResetAndDestroy();    
    
    iUnderLying.ResetAndDestroy();    
    
    iWlanSecurity.ResetAndDestroy();
    
    iLingerSettings.ResetAndDestroy();

    iDefCon.ResetAndDestroy();
    
    iCmInDest.Close(); 
    
    iCmManager.Close();
    }


//------------------------------------------------
// CCdcCommsDatCreator::ConstructL
//------------------------------------------------
//
void CCdcCommsDatCreator::ConstructL()
    {
    CLOG_WRITE( "Initialising FeatureManager\n" )   
    //checks if Wlan feature is supported
    FeatureManager::InitializeLibL();
    iIsWlanS = FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );
    FeatureManager::UnInitializeLib();
    CLOG_WRITE( "FeatureManager closed\n" )               
    }
    
//------------------------------------------------
// CCdcCommsDatCreator::InputFileType
//------------------------------------------------
//
TUint CCdcCommsDatCreator::InputFileType( const TFileName& aFileName )
    {
    TBuf<KLengthOfExtention> extension;
    
    //get extension
    extension.Copy( aFileName.Right( KLengthOfExtention ) );
    
    TDbCreatorInputFile type ( EFileUnknown );
    
    if ( extension.CompareF( KExtensionXML ) == 0 )
        {
        type = EFileXML;            
        }
    else
        {
        //type keeps default value
        }
            
    return type;
    
    }
    
  
//------------------------------------------------
// CCdcCommsDatCreator::UpdateUncatCmsL
//------------------------------------------------
//
void CCdcCommsDatCreator::UpdateUncatCmsL( RArray< TBool >& aCmInDest )
    {
    CLOG_WRITE( "-------------------------------\n" )
    CLOG_WRITE( "Uncategorised Cms:\n" )

    // No destination defined
    if( aCmInDest.Count() == 0 )
        {
        for( TInt i(0); i < iPluginArray.Count(); i++ )
            {
            iPluginArray[i]->UpdateL();
            CLOG_WRITE_FORMAT( "Uncat CM Name: %S", iPluginNames[i] );
            }
        }
    else
        {
        
        for( TInt i(0); i < iPluginArray.Count(); i++ )
            {
            if( aCmInDest[ i ] == EFalse )
                {
                iPluginArray[i]->UpdateL();
                CLOG_WRITE_FORMAT( "Uncat CM Name: %S", iPluginNames[i] );
                }
            }
        }
        
    CLOG_WRITE( "-------------------------------\n" )
    }    


//------------------------------------------------
// CCdcCommsDatCreator::LogIapIdsL
//------------------------------------------------
//
void CCdcCommsDatCreator::LogIapIdsL()
    {
    for( TInt i(0); i < iPluginArray.Count(); i++ )
        {
        TUint32 uid1 = iPluginArray[i]->GetIntAttributeL( CMManager::ECmId );
        CLOG_WRITE_FORMAT( "IAP Name: %S", iPluginNames[i] );
        CLOG_WRITE_FORMAT( "ID: %d\n", uid1 );
        }

    for( TInt i(0); i < iDestArray.Count(); i++ )
        {
        TUint32 uid1 = iDestArray[i]->Id();
        CLOG_WRITE_FORMAT( "SNAP Name: %S", iDestNames[i] );
        CLOG_WRITE_FORMAT( "ID: %d\n", uid1);
        }
    }

// ---------------------------------------------------------
// CCdcCommsDatCreator::ProcessDNL
// ---------------------------------------------------------
//
void CCdcCommsDatCreator::ProcessDNL( RArray< TBool >& aCmInDest,
                                     TBool aFirstRound )
    {
    CLOG_WRITE( "Feature found : Destination Network(s)\n" )
    CLOG_WRITE( "--------------------------------------\n" )
    
    
    if( !aFirstRound )
        {
        for( TInt i(0); i < iPluginArray.Count(); i++ )
            {
            aCmInDest.Append( EFalse );
            }
        }
    
    iFileProcessor = CProcessorDN::NewL( iReader,
                                         iCmManager,
                                         iPluginArray,
                                         iPluginNames,
                                         iDestArray,
                                         iDestNames,
                                         aCmInDest,
                                         aFirstRound ); 
    iFileProcessor->ParseFileL();
    
    delete iFileProcessor;
    iFileProcessor = NULL;
    CLOG_WRITE( "--------------------------------------\n" )
    }    

// ---------------------------------------------------------
// CCdcCommsDatCreator::SetUnderlyingIapL
// ---------------------------------------------------------
//
void CCdcCommsDatCreator::SetUnderlyingIapL( RPointerArray< HBufC >& aUnderLying )
    {
    // Sets the underlying IAP/SNAPS for vpn IAPs    
    if( aUnderLying.Count() > 0 )
        {
        CLOG_WRITE_FORMAT( "SetUnderlyingIapL: %d\n", aUnderLying.Count() )
        CLOG_WRITE( "--------------------------------------\n" )

        CProcessorVpn* vpn = CProcessorVpn::NewL( iReader,
                                               iCmManager,
                                               iPluginArray,
                                               iPluginNames,
                                               aUnderLying );
        
        CleanupStack::PushL( vpn );
        vpn->ProcessUnderLyingL(  iDestArray, iDestNames );
        
                
        CleanupStack::PopAndDestroy( vpn );

        CLOG_WRITE( "--------------------------------------\n" )
        }
    }

// ---------------------------------------------------------
// CCdcCommsDatCreator::ProcessDNL
// ---------------------------------------------------------
//
void CCdcCommsDatCreator::SetDefaultConnectionL( RPointerArray< HBufC >& aDefCon )
    {
    // Sets the default connection    
    if( aDefCon.Count() > 0 )
        {
        CLOG_WRITE_FORMAT( "SetDefaultConnectionL: %d\n", aDefCon.Count() )
        CLOG_WRITE( "--------------------------------------\n" )

        CProcessorGlobal* glb = CProcessorGlobal::NewL( iReader,
                                               iCmManager,
                                               iPluginArray,
                                               iPluginNames,
                                               iDestArray,
                                               iDestNames,
                                               aDefCon );
        
        CleanupStack::PushL( glb );
        glb->SetDefaultConnectionL();
                
        CleanupStack::PopAndDestroy( glb );

        CLOG_WRITE( "--------------------------------------\n" )
        }
    }

// ---------------------------------------------------------
// CCdcCommsDatCreator::SetUnderlyingIapL
// ---------------------------------------------------------
//
void CCdcCommsDatCreator::SetWlanSecurityL( RPointerArray< HBufC >& aWlanSecurity )
    {
    if( aWlanSecurity.Count() > 0 
		|| iEapSettings.Count() > 0 )
        {
        CLOG_WRITE_FORMAT( "SetWlanSecurityL: %d\n", aWlanSecurity.Count() );
        CLOG_WRITE_FORMAT( "SetWlanSecurityL: EAPs: %d\n", iEapSettings.Count() );
        CLOG_WRITE( "--------------------------------------\n" )

        CProcessorWlan* wlan = CProcessorWlan::NewL( iReader,
                                                     iCmManager,
                                                     iPluginArray,
                                                     iPluginNames,
                                                     aWlanSecurity,
													 iEapSettings);
        
        CleanupStack::PushL( wlan );
        wlan->SaveSecurityInfoL();
                
        CleanupStack::PopAndDestroy( wlan );
        
        // EAP settings were saved. Delete them.
        iEapSettings.ResetAndDestroy();

        CLOG_WRITE( "--------------------------------------\n" )
        }
    }

// ---------------------------------------------------------
// CCdcCommsDatCreator:::SetLingerSettingsL
// ---------------------------------------------------------
//
void CCdcCommsDatCreator::SetLingerSettingsL( 
        RPointerArray< CLingerElement >& aLingerSettings )
    {
    CLOG_WRITE_FORMAT( "SetLingerSettingsL: %d\n", aLingerSettings.Count() );
    if ( aLingerSettings.Count() > 0 )
        {
        CProcessorPacketData* pdp = CProcessorPacketData::NewL( iReader, 
                                                                iCmManager,
                                                                iPluginArray,
                                                                iPluginNames,
                                                                iLingerSettings );
        
        CleanupStack::PushL( pdp );
        pdp->SaveLingerSettingsL();
                
        CleanupStack::PopAndDestroy( pdp );
        }  
    CLOG_WRITE( "--------------------------------------\n" )
    }


// End of File.
