/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of the class CProcessorWlan
*
*/




// INCLUDE FILES

#include "cdcprocessorwlan.h"
#include "cdclogger.h"
#include "cdcerrors.h"

#include <cmpluginwlandef.h>
#include <cmpluginbaseeng.h>
#include <cmconnectionmethoddef.h>
#include <cmmanagerext.h>
#include <datamobilitycommsdattypes.h>
#include <commsdattypesv1_1.h>
#include <WlanCdbCols.h>
#include <commsdattypesv1_1.h>
#include <wlancontainer.h>
#include <EapType.h>

using namespace CMManager;

// CONSTANTS

// network modes.
_LIT16( KStrAd_hoc,         "Ad-hoc" );
_LIT16( KStrInfrastructure, "Infrastructure" ); // Default value.

// security modes.
_LIT16( KStrWEP,            "WEP" );
_LIT16( KStr802_1x,         "802.1x" );
_LIT16( KStrWPA,            "WPA" );          // WPA/WPA2 mixed mode
_LIT16( KStrWPA2,           "WPA2" );         // WPA2-only mode
_LIT16( KStrOpen,           "Open" );         // Default value.

// default WLAN channel Id value
const TInt KDefaultChannelId = 0;
const TInt KMaximumChannelId = 11;

// default port number
const TUint32 KDefaultPortNum = 0;

// ratio between sizes of ascii and unicode characters
const TUint KAsciiUnicodeRatio = 2;

// Length of expanded EAP type identifier
const TUint KExpandedEAPIdLength = 8;

// Plain MSCHAPv2 EAP identifier. Needed because of special handling
const TUint8 KMschapv2TypeId[] = {0xfe, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x63};

// ================= MEMBER FUNCTIONS =======================

CEapTypeElement::~CEapTypeElement()
    {
    delete iEapSettings;
    delete iName;
    }


// ---------------------------------------------------------
// CProcessorWlan::NewL
// ---------------------------------------------------------
//
CProcessorWlan* CProcessorWlan::NewL( CReaderBase* aFileReader,
                                      RCmManagerExt& aCmManager,
                                      RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                                      RPointerArray< HBufC >& aPluginNames,
                                      RPointerArray< HBufC >& aSecurityInfo,
									  RPointerArray< CEapTypeElement >& aEapSettings )
    {
    CProcessorWlan* self = new ( ELeave ) CProcessorWlan( aFileReader,
                                                          aCmManager,
                                                          aPluginArray,
                                                          aPluginNames,
                                                          aSecurityInfo,
														  aEapSettings );
    CleanupStack::PushL( self );

    // From base class
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------
// CProcessorWlan::ConstructL
// ---------------------------------------------------------
//
void CProcessorWlan::ConstructL()
    {
    iEmptyTag = KStrEmpty.operator const TDesC16&().Alloc();

    Reset(); // iWepData, iWpaData
    }


// ---------------------------------------------------------
// CProcessorWlan::CProcessorWlan
// ---------------------------------------------------------
//
CProcessorWlan::CProcessorWlan( CReaderBase* aFileReader,
                                RCmManagerExt& aCmManager,
                                RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                                RPointerArray< HBufC >& aPluginNames,
                                RPointerArray< HBufC >& aSecurityInfo,
								RPointerArray< CEapTypeElement >& aEapSettings ) :
             CProcessorBase( aFileReader,
                             aCmManager,
                             aPluginArray,
                             aPluginNames ),
             iSecurityMode( ESecurityModeOpen ),
             iEapSettings( aEapSettings ),
             iIsEasyWlan( ETrue )                     
    {
    iBearerType = KUidWlanBearerType;
    iSecurityInfo = &aSecurityInfo;
    iDataStart = 0;
    }


// ---------------------------------------------------------
// CProcessorWlan::~CProcessorWlan
// ---------------------------------------------------------
//
CProcessorWlan::~CProcessorWlan()
    {
    delete iEmptyTag;
    }

// ---------------------------------------------------------
// CProcessorWlan::ProcessSpecialFieldsL
// ---------------------------------------------------------
//
void CProcessorWlan::ProcessSpecialFieldsL( TInt aField, HBufC* aPtrTag, TInt /*aDx*/ )
    {
        switch( aField )
            {
            // TDesC16
            case EWlanSSID:
                {
                if ( aPtrTag->CompareF( KStrEmpty ) != 0 )
                    {
                    iPlugin.SetStringAttributeL( aField, *aPtrTag );
                    iIsEasyWlan = EFalse;
                    }
                else
                    {
                    // Access point is Easy WLAN 
                    iIsEasyWlan = ETrue;
                    }
                break;
                }
            case EWlanIpAddr:
                {
                if ( aPtrTag->CompareF( KStrEmpty ) != 0 )
                    {
                    iPlugin.SetStringAttributeL( aField, *aPtrTag );
                    }
                break;
                }
            // TDesC16
            case EWlanIpGateway:             //fall through
            case EWlanIpNetMask:             //fall through
                {
                iPlugin.SetStringAttributeL( aField, *aPtrTag );
                break;
                }
            case EWlanIpNameServer1:         //fall through
            case EWlanIpNameServer2:         //fall through
            case EWlanIp6NameServer1:        //fall through
            case EWlanIp6NameServer2:        //fall through
                {
                iPlugin.SetStringAttributeL( aField, *aPtrTag );
                SetAddressFromServerL( EWlanIpNameServer1,
                                      EWlanIpNameServer2,
                                      EWlanIpDNSAddrFromServer,
                                      EWlanIp6NameServer1,
                                      EWlanIp6NameServer2,
                                      EWlanIp6DNSAddrFromServer );    
                break;
                }
            // TUint32
            case EWlanSecurityMode:
                {

                if ( aPtrTag->CompareF( KStrWEP ) == 0 )
                    {
                    // WEP
                    iPlugin.SetIntAttributeL( aField, EWlanSecModeWep );
                    iSecurityMode = ESecurityModeWEP;
                    }
                else if ( aPtrTag->CompareF( KStr802_1x ) == 0 )
                    {
                    // 802.1x
                    iPlugin.SetIntAttributeL( aField, EWlanSecMode802_1x );
                    iSecurityMode = ESecurityMode802;
                    }
                else if ( aPtrTag->CompareF( KStrWPA ) == 0 )
                    {
                    // WPA/WPA2 mixed mode
                    iPlugin.SetIntAttributeL( aField, EWlanSecModeWpa );
                    iSecurityMode = ESecurityModeWPA;
                    }
                else if ( aPtrTag->CompareF( KStrWPA2 ) == 0 )
                    {
                    // WPA2
                    iPlugin.SetIntAttributeL( aField, EWlanSecModeWpa2 );
                    iSecurityMode = ESecurityModeWPA2;
                    }
                else if ( aPtrTag->CompareF( KStrOpen ) == 0 )
                    {
                    // Open network (default)
                    iPlugin.SetIntAttributeL( aField, EWlanSecModeOpen );
                    iSecurityMode = ESecurityModeOpen;
                    }
                else 
                    {
                    iPlugin.SetIntAttributeL( aField, EWlanSecModeOpen );
                    iSecurityMode = ESecurityModeOpen;
                    CLOG_WRITE( "! Error : Invalid Security Mode. Default:Open" )
                    }
                break;
                }
                
            // TUint32
            case EWlanConnectionMode:
                {

                if ( !aPtrTag->CompareF( KStrAd_hoc ) )
                    {
                    // Ad-hoc
                    iPlugin.SetIntAttributeL( aField, EAdhoc );
                    }
                else if ( !aPtrTag->CompareF( KStrInfrastructure ) )
                    {
                    // Infrastructure
                    iPlugin.SetIntAttributeL( aField, EInfra );
                    }
                else
                    {
                    iPlugin.SetIntAttributeL( aField, EInfra );
                    CLOG_WRITE( "! Error : Invalid Network Mode. Default:Infrastructure" )
                    }
                break;
                }
                
            // TUint32
            case ECmProxyPortNumber:
                {
                // First set proxy usage to enabled
                if( !iProxyEnabled )
                    {
                    iPlugin.SetBoolAttributeL( ECmProxyUsageEnabled, ETrue );
                    iProxyEnabled = ETrue;
                    }
                TLex16 lex( *aPtrTag );
                TUint32 tmp( 0 );
                if ( lex.Val( tmp, EDecimal ) == KErrNone )
                    {
                    iPlugin.SetIntAttributeL( aField, tmp );
                    }
                else
                    {
                    //no valid data is given
                    iPlugin.SetIntAttributeL( aField, KDefaultPortNum );
                    CLOG_WRITE( "! Error : Invalid port number. Default:0" );
                    }
                break;
                }
            // Long text.
            case ECmStartPage:
                {

                if ( aPtrTag->CompareF( KStrEmpty ) != 0 )
                    {
                    iPlugin.SetStringAttributeL( aField, *aPtrTag );
                    }
                break;
                }
                
	        case ECmProxyProtocolName:
	        	{
	        	// Do not enable proxy in this case yet
	        	iPlugin.SetStringAttributeL( aField, *aPtrTag );
	        	break;
	        	}
	                
            // Long text.
            case ECmProxyServerName:            
                {
                // First set proxy usage to enabled
                if( !iProxyEnabled )
                    {
                    iPlugin.SetBoolAttributeL( ECmProxyUsageEnabled, ETrue );
                    iProxyEnabled = ETrue;
                    }
                iPlugin.SetStringAttributeL( aField, *aPtrTag );
                break;
                }
            // Bool
            case EWlanScanSSID:
                {
                if ( !aPtrTag->CompareF( KStrYes ) )
                    {
                    iPlugin.SetBoolAttributeL( aField, ETrue );
                    }
                else if ( !aPtrTag->CompareF( KStrNo ) )
                    {
                    iPlugin.SetBoolAttributeL( aField, EFalse );
                    }
                else 
                    {
                    iPlugin.SetBoolAttributeL( aField, EFalse );
                    CLOG_WRITE( "! Error : Invalid Scan SSID. Default:No" )
                    }
                break;
                }

            // TUint32
            case EWlanChannelID:
                {
                TLex16 lex( *aPtrTag );
                TUint32 tmp( 0 );
                lex.Val( tmp, EDecimal );

                if( tmp <= KMaximumChannelId )
                    {
                    iPlugin.SetIntAttributeL( aField, tmp );
                    }
                else
                    {
                    // Default value
                    iPlugin.SetIntAttributeL( aField, KDefaultChannelId );
                    CLOG_WRITE( "! Error : Invalid Chanel Id. Default:0" )
                    }
                break;
                }
            //WEP
            case EWEPKeyInUse:
            case EWEPAuthType:
            case EWEPKey1Length:
            case EWEPKey1Format:
            case EWEPKey1Data:
            case EWEPKey2Length:
            case EWEPKey2Format:
            case EWEPKey2Data:
            case EWEPKey3Length:
            case EWEPKey3Format:
            case EWEPKey3Data:
            case EWEPKey4Length:
            case EWEPKey4Format:
            case EWEPKey4Data:
                {
                if( iSecurityMode == ESecurityModeWEP )
                    {
                    AddSecurityDataL( aField, aPtrTag, ETrue );
                    }
                }
                break;
            //WPA
            case EWPAPresharedKey:
            case EWPAKeyLength:
            case EWPAEapMethod:
            case EWPAUseOfPresharedKey:
                {
                if( iSecurityMode != ESecurityModeWEP && iSecurityMode != ESecurityModeOpen )
                    {
                    AddSecurityDataL( aField, aPtrTag, EFalse );
                    }
                }
                break;
                
            default:
            	// Check if this is EAP setting
            	if ( EAPSetting( aField ) )
            		{
            		AddEAPSettingL( aField, aPtrTag );
            		}
            	else
            		{            		
            		CLOG_WRITE_FORMAT( "! Warning: Unhandled setting for WLAN: %d", aField );
            		}
                break;
            }//switch
    }
     
     
// ---------------------------------------------------------
// CProcessorWlan::ProcessAPL
// ---------------------------------------------------------
//
void CProcessorWlan::ProcessAPL()
    {
    
    RCmConnectionMethodExt& iPlugin = GetLastPluginL();
    
    if( iIsProtected )
        {
        iPlugin.SetBoolAttributeL( ECmProtected, ETrue );
        }
    
    // Ensure that Easy WLAN is always an open network
    if( iIsEasyWlan )
        {
        iSecurityMode = ESecurityModeOpen;
        }
        
    FillSecurityInfoL();        
    
    // Writes into the log file.
    DoLogL();
    
    Reset(); // iWepData, iWpaData
    }
    
// ---------------------------------------------------------
// CProcessorWlan::ProcessAPL
// ---------------------------------------------------------
//
void CProcessorWlan::FillSecurityInfoL()
    {
    //Set Security mode settings
    switch ( iSecurityMode )
        {
        case ESecurityModeWEP:
            if ( WEPFieldValidate() )
                {
                FillSecurityInfoL( ETrue ); //WEP
                }
            else
                {
                CLOG_WRITE( "! Error : WEP settings are not valid." )
                }
            break;  
        case ESecurityMode802:
        case ESecurityModeWPA:
        case ESecurityModeWPA2:
            {
            FillSecurityInfoL( EFalse ); //WPA
            break;
            }
        default:
            {
            }
            break;
        }
    }    

// ---------------------------------------------------------
// CProcessorWlan::ProcessAPL
// ---------------------------------------------------------
//
void CProcessorWlan::FillSecurityInfoL( TBool aIsWep )
    {
    // iName has been deleted by Base. Name is retrieved from name array.
    TInt lastPos = iPluginArray->Count() - 1;
    HBufC* name = (*iPluginName)[ lastPos ];
    
    // Name appended to the array
    iSecurityInfo->Append( name->AllocL() );
    CLOG_WRITE_FORMAT( "FillSecurityInfoL %S", name );
    CLOG_WRITE_FORMAT( "FillSecurityInfoL start %d", iSecurityInfo->Count() );
    
    if( aIsWep )
        {
        // Security type appended to the array
        iSecurityInfo->Append( KStrWEP.operator const TDesC16&().Alloc() );
        for( TInt i( 0 ); i < KWEPKeyNumOfFields; i++ )
            {
            
            if( iWepData[i] != NULL )
                {
                CLOG_WRITE_FORMAT( "wep info %S", iWepData[i] );
                }
            else
                {
                CLOG_WRITE( "wep info NULL" );
                }
            iSecurityInfo->Append( iWepData[i] );
            }
        }
    else
        {
        // Security type appended to the array
        switch ( iSecurityMode )
            {
            case ESecurityMode802:
                iSecurityInfo->Append( KStr802_1x.operator const TDesC16&().Alloc() );
            break;            
            case ESecurityModeWPA2:
                iSecurityInfo->Append( KStrWPA2.operator const TDesC16&().Alloc() );
            break;            
            default:
                iSecurityInfo->Append( KStrWPA.operator const TDesC16&().Alloc() );
            break;            
            }

        for( TInt i( 0 ); i < KWPAKeyNumOfFields; i++ )
            {
            
            if( iWpaData[i] != NULL )
                {
                CLOG_WRITE_FORMAT( "wpa info %S", iWpaData[i] );
                }
            else
                {
                CLOG_WRITE( "wep info NULL" );
                }
            iSecurityInfo->Append( iWpaData[ i ] );
            }
        }

    CLOG_WRITE_FORMAT( "FillSecurityInfoL end %d", iSecurityInfo->Count() );
    }    

// ---------------------------------------------------------
// CProcessorWlan::SaveSecurityInfoL
// ---------------------------------------------------------
//
void CProcessorWlan::SaveSecurityInfoL()
    {
    TInt i(0);

    while( i < iSecurityInfo->Count() )
        {
        if( (*iSecurityInfo)[i] != NULL )
            {
            CLOG_WRITE_FORMAT( "Sec Info: %S", (*iSecurityInfo)[i] );
            }
        else
            {
            CLOG_WRITE( "Sec Info: NULL" );
            }
        
        i++;
        }


    i = 0;

    while( i < iSecurityInfo->Count() )
        {
        CLOG_WRITE_FORMAT( "SaveSecurityInfoL %S", (*iSecurityInfo)[i] );

        TInt id = GetPluginIdL( (*iSecurityInfo)[i] );
        if( id != KErrNotFound )
            {
            i++;
            SaveSecurityInfoL( i, id );
            }
        else
            {
            CLOG_WRITE_FORMAT( "WLan IAp cannot find %S", (*iSecurityInfo)[i] );
            CLOG_WRITE( "Security save likely collapsed" );
            // Tries to look for an existing IAP.
            i++;
            }    
        }

	// EAP settings
	CLOG_WRITE( "Starting EAP settings saving" );
	CMDBSession* session = CMDBSession::NewL( CMDBSession::LatestVersion() );
    CleanupStack::PushL( session );
    
    // Load IAP record in order to get the service id
	for ( i = 0; i < iEapSettings.Count() ; i++ )
        {
        CEapTypeElement* eap = iEapSettings[i];
        // 
        TInt id = GetPluginIdL( eap->iName );
        
	    CCDIAPRecord* iapRecord = static_cast< CCDIAPRecord * >
	                        ( CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
	                        
	    CleanupStack::PushL( iapRecord );
	    
	    iapRecord->SetRecordId( id );
	    
	    iapRecord->LoadL( *session );
	   
	    TInt serviceId = iapRecord->iService;
	    
	    CleanupStack::PopAndDestroy( iapRecord );	        
	            
		TInt err = KErrNone;
	    TUint8 expandedEapId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	    TBuf8<KExpandedEAPIdLength> cue;
	    
		// Set-up 64-bit expanded EAP id
		if ( eap->iEapSettings->iEAPType == KMschapv2TypeId[7] )
			{
			// This is plain MSCHAPv2. Set vendor ID correctly
			expandedEapId[1] = KMschapv2TypeId[1];
			expandedEapId[2] = KMschapv2TypeId[2];
			expandedEapId[3] = KMschapv2TypeId[3];
			}
		
		expandedEapId[KExpandedEAPIdLength - 1] = static_cast<TUint8> ( eap->iEapSettings->iEAPType );	
		cue.Copy( expandedEapId, KExpandedEAPIdLength );
		
		// Try loading EAP ECOM module
		CLOG_WRITE_FORMAT( "Try to load EAP module: %d", expandedEapId[7]);
		CEapType* eapType = 0;
		TRAP( err, eapType = CEapType::NewL( cue, ELan, serviceId ) );
		// The error is caused by probably missing EAP method from the device. Ignore the error
		// because same scripts can be used for devices with and without certain methods.
		if( err == KErrNone )
			{
		    CleanupStack::PushL( eapType );
		        
		    // Check if this type is tunneled
		    if ( eap->iEncapsulatingEapId != EAPSettings::EEapNone )
		    	{
		    	// It is tunneled. Take the last byte of the expanded id.
		    	eapType->SetTunnelingType( eap->iEncapsulatingEapId );    	
		    	}
		    CLOG_WRITE( "Calling eapType->SetConfiguration" );
		    eapType->SetConfigurationL( *eap->iEapSettings );
		    CLOG_WRITE( "eapType->SetConfiguration success!" );
		    CleanupStack::PopAndDestroy( eapType );
		    REComSession::FinalClose();
			}
        }
        
    session->Close();
    CleanupStack::PopAndDestroy( session );
    CLOG_WRITE( "Finished EAP settings saving" );
	//iEapSettings.ResetAndDestroy();

    CLOG_WRITE_FORMAT( "SaveSecurityInfoL end %d", iSecurityInfo->Count() );
}

// ---------------------------------------------------------
// CProcessorWlan::SaveSecurityInfoL
// ---------------------------------------------------------
//
void CProcessorWlan::SaveSecurityInfoL( TInt& aCounter, TInt aId )
    {
    if( (*iSecurityInfo)[ aCounter ]->Compare( KStrWEP ) == 0 )
        {
        aCounter++;
        TInt end( aCounter + KWEPKeyNumOfFields );
        
        for( TInt index(0); aCounter < end; aCounter++, index++ )
            {
            if( (*iSecurityInfo)[ aCounter ] != NULL )
                {
                CLOG_WRITE_FORMAT( "wep info %S", (*iSecurityInfo)[ aCounter ] );
                }
            else
                {
                CLOG_WRITE( "wep info NULL" );
                }
            iWepData[ index] = (*iSecurityInfo)[ aCounter ] ;
            }
        SaveWEPL( aId );
        }
    else
        {
        
        // Set security mode.        
        if ( (*iSecurityInfo)[ aCounter ]->CompareF( KStr802_1x ) == 0 )
            {
            // 802.1x
            iSecurityMode = ESecurityMode802;
            }
        else if ( (*iSecurityInfo)[ aCounter ]->CompareF( KStrWPA2 ) == 0 )
            {
            // WPA2 mode
            iSecurityMode = ESecurityModeWPA2;
            }
        else 
            {
            // WPA
            iSecurityMode = ESecurityModeWPA;
            }

        aCounter++;
        TInt end( aCounter + KWPAKeyNumOfFields );
        
        for( TInt index(0); aCounter < end; aCounter++, index++ )
            {
            if( (*iSecurityInfo)[ aCounter ] != NULL )
                {
                CLOG_WRITE_FORMAT( "wpa info %S", (*iSecurityInfo)[ aCounter ] );
                }
            else
                {
                CLOG_WRITE( "wpa info NULL" );
                }
            iWpaData[ index ] = (*iSecurityInfo)[ aCounter ] ;
            }
        SaveWPAL( aId );
        }
    }


     
// ---------------------------------------------------------
// WEP Security settings
// ---------------------------------------------------------     

// ---------------------------------------------------------
// CProcessorWlan::SaveWEPL
// ---------------------------------------------------------
//
void CProcessorWlan::SaveWEPL( TUint32 aIapId ) 
    {
    CMDBSession* session = CMDBSession::NewL( CMDBSession::LatestVersion() );
    CleanupStack::PushL( session );
    // Load IAP record
    CCDIAPRecord* iapRecord = static_cast< CCDIAPRecord * >
                        ( CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
    CleanupStack::PushL( iapRecord );
    iapRecord->SetRecordId( aIapId );
    iapRecord->LoadL( *session );
   
    // Load WLAN service table
    // first get WLAN table id
    CMDBGenericRecord* generic = static_cast<CMDBGenericRecord*>
        ( CCDRecordBase::RecordFactoryL( 0 ) );
    CleanupStack::PushL( generic );    
    generic->InitializeL( TPtrC( WLAN_SERVICE ), NULL );
    generic->LoadL( *session );
    TMDBElementId wlanTableId = generic->TableId();
    
    CMDBField<TUint32>* sidField = static_cast<CMDBField<TUint32>*>
                             ( generic->GetFieldByIdL( KCDTIdWlanServiceId ) );
    
    TInt service = iapRecord->iService;
    // prime with service id                
    *sidField = (TUint32)service;
       
    User::LeaveIfError( generic->FindL( *session ) );

    // Save index of key in use
    TUint32 keyInUse( WEPKeyInUse() );
    CMDBField<TUint>* keyInUseField = static_cast<CMDBField<TUint>*>
                            ( generic->GetFieldByIdL( KCDTIdWlanWepIndex ) );
    keyInUseField->SetL( keyInUse );
    
    // Save authentication mode
    TUint32 auth( WEPAuthentication() );
    CMDBField<TUint>* authenticationField = static_cast<CMDBField<TUint>*>
                            ( generic->GetFieldByIdL( KCDTIdWlanAuthMode ) );
    authenticationField->SetL( auth );
    
    // Save WEP keys and key formats  
    if ( WEPFieldData( EWEPKey1Data )->Length() )
        {
        CMDBField<TDesC8>* wepKey1Field = static_cast<CMDBField<TDesC8>*>
                        ( generic->GetFieldByIdL( KCDTIdWlanWepKey1 ) );
        WriteKeyL( wepKey1Field,
                   WEPFieldData( EWEPKey1Data ),
                   WEPKeyFormat( EWEPKey1Format ) );
        CMDBField<TUint>* formatKey1Field = static_cast<CMDBField<TUint>*>
                            ( generic->GetFieldByIdL( KCDTIdWlanFormatKey1 ) );
        formatKey1Field->SetL( WEPKeyFormat( EWEPKey1Format ) );
        }
    if ( WEPFieldData( EWEPKey2Data )->Length() )
        {
        CMDBField<TDesC8>* wepKey2Field = static_cast<CMDBField<TDesC8>*>
                        ( generic->GetFieldByIdL( KCDTIdWlanWepKey2 ) );
        WriteKeyL( wepKey2Field,
                   WEPFieldData( EWEPKey2Data ),
                   WEPKeyFormat( EWEPKey2Format ) );
        CMDBField<TUint>* formatKey2Field = static_cast<CMDBField<TUint>*>
                            ( generic->GetFieldByIdL( KCDTIdWlanFormatKey2 ) );
        formatKey2Field->SetL( WEPKeyFormat( EWEPKey2Format ) );
        }
    if ( WEPFieldData( EWEPKey3Data )->Length() )
        {
        CMDBField<TDesC8>* wepKey3Field = static_cast<CMDBField<TDesC8>*>
                        ( generic->GetFieldByIdL( KCDTIdWlanWepKey3 ) );
        WriteKeyL( wepKey3Field,
                   WEPFieldData( EWEPKey3Data ),
                   WEPKeyFormat( EWEPKey3Format ) );
        CMDBField<TUint>* formatKey3Field = static_cast<CMDBField<TUint>*>
                            ( generic->GetFieldByIdL( KCDTIdWlanFormatKey3 ) );
        formatKey3Field->SetL( WEPKeyFormat( EWEPKey3Format ) );
        }
    if ( WEPFieldData( EWEPKey4Data )->Length() )
        {
        CMDBField<TDesC8>* wepKey4Field = static_cast<CMDBField<TDesC8>*>
                        ( generic->GetFieldByIdL( KCDTIdWlanWepKey4 ) );
        WriteKeyL( wepKey4Field,
                   WEPFieldData( EWEPKey4Data ),
                   WEPKeyFormat( EWEPKey4Format ) );
        CMDBField<TUint>* formatKey4Field = static_cast<CMDBField<TUint>*>
                            ( generic->GetFieldByIdL( KCDTIdWlanFormatKey4 ) );
        formatKey4Field->SetL( WEPKeyFormat( EWEPKey4Format ) );
        }
    
    generic->ModifyL( *session );

    CleanupStack::PopAndDestroy( generic ); // generic
    CleanupStack::PopAndDestroy( iapRecord ); // iapRecord
    CleanupStack::PopAndDestroy( session ); // session
    }
    

// ---------------------------------------------------------
// CProcessorWlan::WriteKeyL
// ---------------------------------------------------------
//
void CProcessorWlan::WriteKeyL( CMDBField<TDesC8>* aKeyField,                                
                                HBufC16* aKey,
                                const TInt aKeyFormat ) 
    {
    TBuf8<KMaxLengthOfKeyData> key;
    
    //convert to 8 bit
    key.Copy( aKey->Des() );
    
    if ( aKeyFormat == EAscii )
        {
        // Must be converted to hexa and stored as a hexa
        // Ascii key is half the length of Hex
        HBufC8* buf8Conv = HBufC8::NewLC( key.Length() * KAsciiUnicodeRatio );
        ConvertAsciiToHex( key, buf8Conv );
        aKeyField->SetL( buf8Conv->Des() );
        CleanupStack::PopAndDestroy( buf8Conv );
        }
    else if ( VerifyHex( key ) )
        {
        //already in hexa format
        aKeyField->SetL( key );    
        }
    else
        {
        CLOG_WRITE( "! Error : Invalid hexadecimal format" )
        }
    }
    
// ---------------------------------------------------------
// CProcessorWlan::WEPFieldData
// ---------------------------------------------------------
//
HBufC16* CProcessorWlan::WEPFieldData( TDbCreatorWEPFields aFieldId ) 
    {
    TInt index = WEPIndex( aFieldId );
    if( iWepData[ index ] == NULL )
        {
        return iEmptyTag;
        }
    else
        {
        return iWepData[ index ];
        }
    }
    
// ---------------------------------------------------------
// CProcessorWlan::WEPAuthentication
// ---------------------------------------------------------
//   
TInt CProcessorWlan::WEPAuthentication()
    {
    HBufC16* authTypeStr = WEPFieldData( EWEPAuthType );
    TInt authType ( EAuthOpen ) ;
    
    _LIT16( KStrOpen,   "Open" );  
    _LIT16( KStrShared, "Shared" );
    
    if ( authTypeStr->CompareF( KStrOpen ) == 0 )
        {
        //CLOG_WRITE( "Authentication type : Open" )
        }
    else if ( authTypeStr->CompareF( KStrShared ) == 0 )
        {
        //CLOG_WRITE( "Authentication type : Shared" )
        authType = EAuthShared;
        }
    else 
        {
        //aField is either empty or invalid
        authType = KErrNotFound;
        }
        
    return authType;
    }

// ---------------------------------------------------------
// CProcessorWlan::WEPKeyInUse
// ---------------------------------------------------------
//
TInt CProcessorWlan::WEPKeyInUse()
    {  
    HBufC16* keyStr = WEPFieldData( EWEPKeyInUse );
    TInt key ( EKeyNumber1 ) ;
    
    _LIT16( KStrKey1,   "key1" );  
    _LIT16( KStrKey2,   "key2" );
    _LIT16( KStrKey3,   "key3" );
    _LIT16( KStrKey4,   "key4" );
    
    if ( keyStr->CompareF( KStrKey1 ) == 0 )
        {
        //CLOG_WRITE( "WEP key in use : key 1" )
        }
    else if ( keyStr->CompareF( KStrKey2 ) == 0 )
        {
        //CLOG_WRITE( "WEP key in use : key 2" )
        key = EKeyNumber2;
        }
    else if ( keyStr->CompareF( KStrKey3 ) == 0 )
        {
        //CLOG_WRITE( "WEP key in use : key 3" )
        key = EKeyNumber3;
        }
    else if ( keyStr->CompareF( KStrKey4 ) == 0 )
        {
        //CLOG_WRITE( "WEP key in use : key 4" )
        key = EKeyNumber4;
        }
    else 
        {
        //aField is either empty or invalid
        key = KErrNotFound;
        }
        
    return key;
    }


    
// ---------------------------------------------------------
// CProcessorWlan::WEPKeyFormat
// ---------------------------------------------------------
// 
TInt CProcessorWlan::WEPKeyFormat(  TDbCreatorWEPFields aFieldId  )
    {
    __ASSERT_ALWAYS( ( aFieldId == EWEPKey1Format || 
                       aFieldId == EWEPKey2Format ||
                       aFieldId == EWEPKey3Format ||
                       aFieldId == EWEPKey4Format ),
                     User::Panic ( KPanicOutOfRange, KErrOutOfRange ) );
                    
    HBufC16* keyFormatStr = WEPFieldData( aFieldId );
    TWEPKeyFormat keyFormat ( EAscii ) ;
    
    _LIT16( KStrAscii,       "Ascii" );  
    _LIT16( KStrHexadecimal, "Hexadecimal" );
    
    if ( keyFormatStr->CompareF( KStrAscii ) == 0 )
        {
        //CLOG_WRITE( "Key format : Ascii" )
        }
    else if ( keyFormatStr->CompareF( KStrHexadecimal ) == 0 )
        {
        //CLOG_WRITE( "Key format : Hexadecimal" )
        keyFormat = EHexadecimal;
        }
    else 
        {
        CLOG_WRITE( "! Error : Invalid key format. Default:Ascii" )
        }
        
    return keyFormat;
    }
    
// ---------------------------------------------------------
// CProcessorWlan::WEPKeyLength
// ---------------------------------------------------------
// 
TWEPKeyLength CProcessorWlan::WEPKeyLength(  TDbCreatorWEPFields aFieldId  )
    {
    __ASSERT_ALWAYS( ( aFieldId == EWEPKey1Length || 
                       aFieldId == EWEPKey2Length ||
                       aFieldId == EWEPKey3Length ||
                       aFieldId == EWEPKey4Length ),
                     User::Panic ( KPanicOutOfRange, KErrOutOfRange ) );
                    
    HBufC16* keyLengthStr = WEPFieldData( aFieldId );
    TWEPKeyLength keyLen ( E40Bits ) ;
    
    _LIT16( KStr64,       "64" );  
    _LIT16( KStr128,      "128" );
    _LIT16( KStr256,      "256" );
    
    if ( keyLengthStr->CompareF( KStr64 ) == 0 )
        {
        //CLOG_WRITE( "Key length : 64" )
        }
    else if ( keyLengthStr->CompareF( KStr128 ) == 0 )
        {
        //CLOG_WRITE( "Key length : 128" )
        keyLen = E104Bits;
        }
    else if ( keyLengthStr->CompareF( KStr256 ) == 0 )
        {
        //CLOG_WRITE( "Key length : 256" )
        keyLen = E232Bits;
        }
    else 
        {
        CLOG_WRITE( "! Error : Invalid key length. Default:64" )
        }
        
    return keyLen;
    }
    
// ---------------------------------------------------------
// CProcessorWlan::WEPKeyValidLen
// ---------------------------------------------------------
//     
TInt CProcessorWlan::WEPKeyValidLen(TInt aFormat , TWEPKeyLength aLen )
    {
    __ASSERT_ALWAYS( aFormat == EAscii || aFormat == EHexadecimal, 
                     User::Panic ( KPanicOutOfRange, KErrOutOfRange ) );
                     
    __ASSERT_ALWAYS( aLen == E40Bits || aLen == E104Bits || aLen == E232Bits,
                     User::Panic ( KPanicOutOfRange, KErrOutOfRange ) );
                     
    TInt len( 0 );
    
    switch ( aFormat )
        {
        case EAscii:
            switch ( aLen )
                {
                case E40Bits :  len = KKeyDataLength40Bits /
                                                        KAsciiUnicodeRatio;
                    break;
                case E104Bits : len = KKeyDataLength104Bits /
                                                        KAsciiUnicodeRatio;
                    break;
                case E232Bits : len = KKeyDataLength232Bits /
                                                        KAsciiUnicodeRatio;  
                }
            break;
        case EHexadecimal:
            switch ( aLen )
                {
                case E40Bits :  len = KKeyDataLength40Bits;
                    break;
                case E104Bits : len = KKeyDataLength104Bits;
                    break;
                case E232Bits : len = KKeyDataLength232Bits;
                }
        }
        
    return len;
    }

// ---------------------------------------------------------
// CProcessorWlan::WEPIndex
// ---------------------------------------------------------
//       
TInt CProcessorWlan::WEPIndex( TDbCreatorWEPFields aFieldId )
    {
    __ASSERT_ALWAYS( aFieldId >= EWEPKeyInUse && aFieldId <= EWEPKey4Data, 
                     User::Panic ( KPanicOutOfRange, KErrOutOfRange ) );
                     
    return aFieldId - 0x1000 + iDataStart;                     
                     
    ////return iWEPIndex[ aFieldId - 0x1000 ];
    }

// ---------------------------------------------------------
// CProcessorWlan::WEPFieldValidate
// ---------------------------------------------------------
//   
TBool CProcessorWlan::WEPFieldValidate()
    {
    TBool valid (ETrue);
    
    if ( ( WEPAuthentication() == KErrNotFound ) ||
         ( WEPKeyInUse() == KErrNotFound ) )
        {
        valid = EFalse;
        }
    else
        {
        //gets the actual keylength
        TInt keyLen[ KMaxNumberofKeys ] = { 0,0,0,0 };
       
        keyLen[EKeyNumber1] = WEPFieldData( EWEPKey1Data )->Length();
        keyLen[EKeyNumber2] = WEPFieldData( EWEPKey2Data )->Length();
        keyLen[EKeyNumber3] = WEPFieldData( EWEPKey3Data )->Length();
        keyLen[EKeyNumber4] = WEPFieldData( EWEPKey4Data )->Length();


        //check validity of key1 length
        TInt validLen = WEPKeyValidLen( WEPKeyFormat( EWEPKey1Format ), 
                                        WEPKeyLength( EWEPKey1Length ) );
        if ( keyLen[EKeyNumber1] && keyLen[EKeyNumber1] != validLen) 
            {
            valid = EFalse;
            CLOG_WRITE_FORMAT( "! Error : Key1 length is invalid %d", 
                                        keyLen[EKeyNumber1] );
            CLOG_WRITE_FORMAT( "ValidLen1: %d \n", validLen );
            }
        else if ( keyLen[EKeyNumber2] )
            {    
            //check validity of key2 length
            validLen = WEPKeyValidLen( WEPKeyFormat( EWEPKey2Format ), 
                                       WEPKeyLength( EWEPKey2Length ) );
            if ( keyLen[EKeyNumber2] != validLen )
                {
                valid = EFalse;
                CLOG_WRITE_FORMAT( "! Error : Key2 length is invalid %d", 
                                            keyLen[EKeyNumber2] )
                CLOG_WRITE_FORMAT( "ValidLen2: %d \n", validLen );
                }
            else if ( keyLen[EKeyNumber3] )
                {
                //check validity of key3 length
                validLen = WEPKeyValidLen( WEPKeyFormat( EWEPKey3Format ), 
                                           WEPKeyLength( EWEPKey3Length ) );
                if ( keyLen[EKeyNumber3] != validLen )
                    {
                    valid = EFalse;
                    CLOG_WRITE_FORMAT( "! Error : Key3 length is invalid %d", 
                                                keyLen[EKeyNumber3] );
                    CLOG_WRITE_FORMAT( "ValidLen3: %d \n", validLen );
                    }
                else if ( keyLen[EKeyNumber4] )
                    {
                    //check validity of key4 length
                    validLen = WEPKeyValidLen( WEPKeyFormat( EWEPKey4Format ), 
                                           WEPKeyLength( EWEPKey4Length ) );
                    if ( keyLen[EKeyNumber4] != validLen )
                        {
                        valid = EFalse;
                        CLOG_WRITE_FORMAT( "! Error : Key4 length is invalid %d", 
                                                    keyLen[EKeyNumber4] );
                        CLOG_WRITE_FORMAT( "ValidLen4: %d \n", validLen );
                        }//if
                    }//else                
                }//else
            }//else
        }//else
        
    return valid;
    }

//----------------------------------------------------------
// CProcessorWlan::ConvertAsciiToHex
//----------------------------------------------------------
//
void CProcessorWlan::ConvertAsciiToHex( const TDesC8& aSource, 
                                              HBufC8*& aDest )
	{
	_LIT( hex, "0123456789ABCDEF" );
	TInt size = aSource.Size();
	TPtr8 ptr = aDest->Des();
	for ( TInt ii = 0; ii < size; ii++ )
		{
		TText8 ch = aSource[ii];
		ptr.Append( hex()[(ch/16)&0x0f] );
		ptr.Append( hex()[ch&0x0f] );
		}
	}
		
//----------------------------------------------------------
// CWEPSecuritySettingsDlg::VerifyHex
//----------------------------------------------------------
//
TBool CProcessorWlan::VerifyHex( const TDesC8& aHex )
    {
    TBool err( ETrue );
    
    for ( TInt i = 0; i < aHex.Length(); i++ )
        {
        TChar c( aHex[i] );

        if ( !c.IsHexDigit() ) 
            {
            err = EFalse;
            break;
            }
        }

    return err;
    }
   
    
// ---------------------------------------------------------
// WPA Security settings
// ---------------------------------------------------------


// ---------------------------------------------------------
// CProcessorWlan::SaveWPAL
// ---------------------------------------------------------
//
void CProcessorWlan::SaveWPAL( TUint32 aIapId )
    {
    CLOG_WRITE( "CProcessorWlan::SaveWPAL enter" );
    CMDBSession* session = CMDBSession::NewL( CMDBSession::LatestVersion() );
    CleanupStack::PushL( session );
    
    // Load IAP record
    CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord *>
                        ( CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
    CleanupStack::PushL( iapRecord );
    iapRecord->SetRecordId( aIapId );
    iapRecord->LoadL( *session );
    
    // Load WLAN service table
    // first get WLAN table id
    CMDBGenericRecord* generic = static_cast<CMDBGenericRecord*>
        ( CCDRecordBase::RecordFactoryL( 0 ) );
    CleanupStack::PushL( generic );
        
    generic->InitializeL( TPtrC( WLAN_SERVICE ), NULL );
    generic->LoadL( *session );
    TMDBElementId wlanTableId = generic->TableId();
    CMDBField<TUint32>* sidField = static_cast<CMDBField<TUint32>*>
                             ( generic->GetFieldByIdL( KCDTIdWlanServiceId ) );
    
    TInt service = iapRecord->iService;
    // prime with service id                
    *sidField = (TUint32)service;
    
    User::LeaveIfError( generic->FindL( *session ) );
       
    // Set WPA mode
    CMDBField<TUint>* enableWpaPskField = static_cast<CMDBField<TUint>*>
                            ( generic->GetFieldByIdL( KCDTIdWlanEnableWpaPsk ) );
                            
    enableWpaPskField->SetL( WPAPresharedKeyInUse() );
	
	CLOG_WRITE( "Wrote enableWpaPskField" );
    
    // Set security mode
    CMDBField<TUint>* secModeField = static_cast<CMDBField<TUint>*>
                        ( generic->GetFieldByIdL( KCDTIdWlanSecMode ) );
    secModeField->SetL( iSecurityMode );
    
    CLOG_WRITE( "Wrote securityMode" );

    // Save EAP list
    SetExpandedEapListL( generic );
	CLOG_WRITE( "Wrote expandedEapList" );	
	
    // Save PreShared Key
    TBuf8<KMaxLengthOfKeyData> keyWPA;
    //convert to 8 bit
    keyWPA.Copy( WPAFieldData( EWPAPresharedKey )->Des() );
    CMDBField<TDesC8>* wpaPskField = static_cast<CMDBField<TDesC8>*>
                        ( generic->GetFieldByIdL( KCDTIdWlanWpaPreSharedKey ) );
    wpaPskField->SetL( keyWPA );
    
    CLOG_WRITE( "Wrote pre-shared key" );
    if ( keyWPA.Length() < 8 )
        {
        CLOG_WRITE( "Error: WPA pre-shared key is less then 8 chars." );	        
        }

    // Check and save PreShared Key Length
    TInt len( WPAKeyLength() );
    if ( len != keyWPA.Length() )
        {
        CLOG_WRITE( "! Error : WPA key lengths do not match" );
        }
    
    CMDBField<TUint>* keyLengthField = static_cast<CMDBField<TUint>*>
                        ( generic->GetFieldByIdL( KCDTIdWlanWpaKeyLength ) );
    keyLengthField->SetL( len );
    
    generic->ModifyL( *session );
    
    CleanupStack::PopAndDestroy( generic ); 
    CleanupStack::PopAndDestroy( iapRecord );
    CleanupStack::PopAndDestroy( session );
    CLOG_WRITE( "CProcessorWlan::SaveWPAL exit" );
    }

// ---------------------------------------------------------
// CProcessorWlan::SetExpandedEapListL
// ---------------------------------------------------------
//
void CProcessorWlan::SetExpandedEapListL( CMDBGenericRecord* generic )
    {
    CLOG_WRITE( "CProcessorWlan::SetExpandedEapListL" )
    // Gets the text format eap list
    HBufC16* eapList = WPAFieldData( EWPAEapMethod );
    
    if ( eapList != NULL && 0 < eapList->Length() )
        {

        // Read EAP method and save it as enabled eap list.
        // Fills expandedForm with 8 bytes: 0xFE, 6 zeros and EAP ID.
        // That's the format expected by EAPOL.
        TBuf8<8> expandedForm;
        expandedForm.AppendFill( 0xFE, 1 );
        expandedForm.AppendFill( 0x00, 6 );
            
        TUint8 resultByte;
        TLex16 lex( eapList->Ptr() );
        User::LeaveIfError( lex.Val( resultByte, EDecimal ) );
        expandedForm.AppendFill( resultByte, 1 );
        CLOG_WRITE_FORMAT( "CProcessorWlan::SetExpandedEapListL: EAP %d",
                (int)resultByte)

        CMDBField<TDesC8>* wlanEnabledEapsField = static_cast<CMDBField<TDesC8>*>
                                ( generic->GetFieldByIdL( KCDTIdWlanEnabledEaps ) );
        wlanEnabledEapsField->SetL( expandedForm );

        // Save empty disabled EAP list
        HBufC8* disabledEapList = HBufC8::NewLC(0);
        CMDBField<TDesC8>* wlanDisabledEapsField = static_cast<CMDBField<TDesC8>*>
                                ( generic->GetFieldByIdL( KCDTIdWlanDisabledEaps ) );
        wlanDisabledEapsField->SetL( disabledEapList->Des() );
        CleanupStack::PopAndDestroy( disabledEapList );
        }
    }


// ---------------------------------------------------------
// CProcessorWlan::WPAIndex
// ---------------------------------------------------------
//       
TInt CProcessorWlan::WPAIndex( TDbCreatorWPAFields aFieldId )
    {
    __ASSERT_ALWAYS( aFieldId >= EWPAPresharedKey && aFieldId <= EWPAUseOfPresharedKey, 
                     User::Panic ( KPanicOutOfRange, KErrOutOfRange ) );
                     
    return aFieldId - 0x2000 + iDataStart;                     
    }
    
    
// ---------------------------------------------------------
// CProcessorWlan::WPAFieldData
// ---------------------------------------------------------
//
HBufC16* CProcessorWlan::WPAFieldData( TDbCreatorWPAFields aFieldId ) 
    {
    TInt index = WPAIndex( aFieldId );
    if( iWpaData[ index ] == NULL )
        {
        return iEmptyTag;
        }
    else
        {
        return iWpaData[ index ];
        }
    }
    
// ---------------------------------------------------------
// CProcessorWlan::WPAPresharedKeyInUse
// ---------------------------------------------------------
//
TBool CProcessorWlan::WPAPresharedKeyInUse()
    {
    HBufC16* useStr = WPAFieldData( EWPAUseOfPresharedKey );   

    TBool usePSK ( EFalse ) ;
    
    if ( useStr->CompareF( KStrYes ) == 0 )
        {
        CLOG_WRITE( "Preshared key in use" )
        usePSK = ETrue;
        }
    else if ( useStr->CompareF( KStrNo ) == 0 )
        {
        CLOG_WRITE( "Preshared key not in use" );
        }
    else 
        {
        //aField is either empty or invalid
        CLOG_WRITE( "! Error : Invalid Use WPA preshared key. Default:No" );
        }
        
    return usePSK;
    }
    
// ---------------------------------------------------------
// CProcessorWlan::WPAKeyLength
// ---------------------------------------------------------
//
TInt CProcessorWlan::WPAKeyLength() 
    {
    HBufC16* tag = WPAFieldData( EWPAKeyLength );   
    
    TInt num( 0 );
    TLex parser( tag->Des() );

    if ( parser.Val( num ) != KErrNone )
        {
        CLOG_WRITE( "! Error : Invalid input : WPA key length" );
        }
        
    return num;
    }

// ---------------------------------------------------------
// CProcessorWlan::AddSecurityDataL
// ---------------------------------------------------------
//   
void CProcessorWlan::AddSecurityDataL( TInt aField, HBufC* aPtrTag, TBool aIsWep )
    {
    if( aIsWep )
        {
        iWepData[ WEPIndex( (TDbCreatorWEPFields)aField ) ] = aPtrTag->AllocL();
        ////iWepData.Insert( aPtrTag->AllocL(), WEPIndex( (TDbCreatorWEPFields)aField ) );
        }
    else    
        {
        iWpaData[ WPAIndex( (TDbCreatorWPAFields)aField ) ] = aPtrTag->AllocL();
        ////iWpaData.Insert( aPtrTag->AllocL(), WPAIndex( (TDbCreatorWPAFields)aField ) );
        }
    }        
        
// ---------------------------------------------------------
// CProcessorWlan::DoLogL
// ---------------------------------------------------------
//
void CProcessorWlan::DoLogL()
    {
    // Writes some data of the actual WLAN access point into the log file.
    HBufC16* tmp;
    RCmConnectionMethodExt& plugin = GetLastPluginL();

    // Writes some data of the Internet acces point into the log file.
    if ( iIsEasyWlan )
        {
        CLOG_WRITE( "Easy WLAN Access Point created:" )
        }
    else
        {
        CLOG_WRITE( "WLAN Access Point created:" )
        }
        
    CLOG_WRITE_FORMAT( "\t Connection name in array: %S ", 
                        (*iPluginName)[ PluginCount() - 1] );  
        
    tmp = plugin.GetStringAttributeL( ECmName );
    if( tmp )
        {
        CleanupStack::PushL( tmp );
        CLOG_WRITE_FORMAT( "\t Connection name: %S ", tmp )
        CleanupStack::PopAndDestroy( tmp );
        }

    // For the WLAN network name
    tmp = plugin.GetStringAttributeL( EWlanSSID );
    if( tmp )
        {
        CleanupStack::PushL( tmp );
        CLOG_WRITE_FORMAT( "\t Network name: %S", tmp )
        CleanupStack::PopAndDestroy( tmp );
        }
    CLOG_WRITE( "\t \r\n" )
    }


// ---------------------------------------------------------
// CProcessorWlan::Reset
// ---------------------------------------------------------
//
void CProcessorWlan::Reset()
    {
    for( TInt i = 0; i < KWEPKeyNumOfFields; i++ )
        {
        iWepData[i] = NULL;
        }
    for( TInt i = 0; i < KWPAKeyNumOfFields; i++ )
        {
        iWpaData[i] = NULL;
        }
    }
    
// ---------------------------------------------------------
// CProcessorWlan::EAPSetting
// ---------------------------------------------------------
//
TBool CProcessorWlan::EAPSetting( const TInt aField )
	{
	// Checks if the supplied field is EAP settings and returns ETrue if it is
	if ( aField > EEapParameterFirst
		 && aField < EEapParameterLast )
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}
// ---------------------------------------------------------
// CProcessorWlan::GetEapTypeIdFromSettingId
// ---------------------------------------------------------
//
EAPSettings::TEapType CProcessorWlan::GetEapTypeIdFromSettingId( const TInt aField )
	{
	switch ( aField )
		{
		case EEapGtcUsername:
		case EEapGtcSessionValidityTime:
		case EEapGtcEncapsulation:
			{
			return EAPSettings::EEapGtc;
			}
		case EEapTlsUsername:
		case EEapTlsRealm:
		case EEapTlsVerifyServerRealm:
		case EEapTlsRequireClientAuth:
		case EEapTlsSessionValidityTime:
		case EEapTlsCipherSuites:
		case EEapTlsUserCertSubjectKeyId:
		case EEapTlsUserCertIssuerName:
		case EEapTlsUserCertSerialNumber:
		case EEapTlsCaCertSubjectKeyId:
		case EEapTlsCaCertIssuerName:
		case EEapTlsCaCertSerialNumber:
		case EEapTlsEncapsulation:
			{
			return EAPSettings::EEapTls;
			}
		case EEapLeapUsername:
		case EEapLeapPassword:
		case EEapLeapSessionValidityTime:
			{
			return EAPSettings::EEapLeap;
			}
		case EEapSimUsername:
		case EEapSimRealm:
		case EEapSimUsePseudonyms:
		case EEapSimSessionValidityTime:
		case EEapSimEncapsulation:
			{
			return EAPSettings::EEapSim;
			}
		case EEapTtlsUsername:
		case EEapTtlsRealm:
		case EEapTtlsVerifyServerRealm:
		case EEapTtlsRequireClientAuth:
		case EEapTtlsSessionValidityTime:
		case EEapTtlsCipherSuites:
		case EEapTtlsEncapsulatedTypes:
		case EEapTtlsUserCertSubjectKeyId:
		case EEapTtlsUserCertIssuerName:
		case EEapTtlsUserCertSerialNumber:
		case EEapTtlsCaCertSubjectKeyId:
		case EEapTtlsCaCertIssuerName:
		case EEapTtlsCaCertSerialNumber:
			{
			return EAPSettings::EEapTtls;
			}
		case EEapAkaUsername:
		case EEapAkaRealm:
		case EEapAkaUsePseudonyms:
		case EEapAkaSessionValidityTime:
		case EEapAkaEncapsulation:
			{
			return EAPSettings::EEapAka;
			}
		case EEapPeapUsername:
		case EEapPeapRealm:
		case EEapPeapVerifyServerRealm:
		case EEapPeapRequireClientAuth:
		case EEapPeapSessionValidityTime:
		case EEapPeapCipherSuites:
		case EEapPeapV0Allowed:
		case EEapPeapV1Allowed:
		case EEapPeapV2Allowed:
		case EEapPeapEncapsulatedTypes:
		case EEapPeapUserCertSubjectKeyId:
		case EEapPeapUserCertIssuerName:
		case EEapPeapUserCertSerialNumber:
		case EEapPeapCaCertSubjectKeyId:
		case EEapPeapCaCertIssuerName:
		case EEapPeapCaCertSerialNumber:		
			{
			return EAPSettings::EEapPeap;
			}
		case EEapMschapv2Username:
		case EEapMschapv2Password:
		case EEapMschapv2SessionValidityTime:
		case EEapMschapv2Encapsulation:
			{
			return EAPSettings::EEapMschapv2;
			}
		case EEapFastUsername:
		case EEapFastRealm:
		case EEapFastVerifyServerRealm:
		case EEapFastRequireClientAuth:
		case EEapFastSessionValidityTime:
		case EEapFastCipherSuites:
		case EEapFastEncapsulatedTypes:
		case EEapFastAuthProvModeAllowed:
		case EEapFastUnauthProvModeAllowed:
		case EEapFastWarnADHPNoPAC:
		case EEapFastWarnADHPNoMatchingPAC:
		case EEapFastWarnNotDefaultServer:
		case EEapFastUserCertSubjectKeyId:
		case EEapFastUserCertIssuerName:
		case EEapFastUserCertSerialNumber:
		case EEapFastCaCertSubjectKeyId:
		case EEapFastCaCertIssuerName:
		case EEapFastCaCertSerialNumber:
			{
			return EAPSettings::EEapFast; 
			}
		
		case EMschapv2Username:
		case EMschapv2Password:
		case EMschapv2SessionValidityTime:
		case EMschapv2Encapsulation:
			{
			return EAPSettings::EPlainMschapv2;
			}
		default:
			{
			return EAPSettings::EEapNone;
			}
		
		}
	}
// ---------------------------------------------------------
// CProcessorWlan::AddEAPSetting
// ---------------------------------------------------------
//
void CProcessorWlan::AddEAPSettingL( const TInt aField, const HBufC16* const aValue  )
    {
    EAPSettings::TEapType eapId = GetEapTypeIdFromSettingId( aField );
    if ( eapId == EAPSettings::EEapNone )
    	{
    	CLOG_WRITE( "! Error : Unknown EAP method" );
    	User::Leave( KErrArgument ); 
    	}
    
	TInt eapIndex(0);
	// Search if the EAP instance already exists in the array for this
	for ( eapIndex = 0 ; eapIndex < iEapSettings.Count() ; eapIndex++ )
		{       				
		if ( ( iEapSettings[eapIndex]->iEapSettings->iEAPType == eapId )
			 && ( iEapSettings[eapIndex]->iName != NULL ) 
			 && ( iEapSettings[eapIndex]->iName->Compare( *iName ) == 0 ))        					 	
		 	{       				
		 	// Found it
		 	break;
		 	}
		}
		
	if ( eapIndex == iEapSettings.Count() )
		{
		// Not found. Create new
		CEapTypeElement* newEap = new (ELeave) CEapTypeElement;
		CleanupStack::PushL( newEap );
		
		newEap->iEapSettings = new (ELeave) EAPSettings;
		
		newEap->iEapSettings->iEAPType = eapId;
		
		newEap->iName = iName->AllocL();                           
		               	    	
		iEapSettings.AppendL(newEap);
		
		CleanupStack::Pop( newEap );
		
		// Set the index to the newly created EAP settings instance
		eapIndex = iEapSettings.Count() - 1;
		
		}
	switch ( aField )
		{
		case EEapGtcUsername:
		case EEapTlsUsername:
		case EEapLeapUsername:
		case EEapSimUsername:
		case EEapTtlsUsername:
		case EEapAkaUsername:
		case EEapPeapUsername:
		case EEapMschapv2Username:
		case EEapFastUsername:
		case EMschapv2Username:
			{
			iEapSettings[eapIndex]->iEapSettings->iUsernamePresent = ETrue;
			iEapSettings[eapIndex]->iEapSettings->iUsername.Copy( *aValue );
			break;
			}
			
		case EEapLeapPassword:
		case EEapMschapv2Password:
		case EMschapv2Password:
			{
			iEapSettings[eapIndex]->iEapSettings->iPasswordPresent = ETrue;
			iEapSettings[eapIndex]->iEapSettings->iPassword.Copy( *aValue );			
			break;
			}

		case EEapTlsRealm:
		case EEapSimRealm:
		case EEapTtlsRealm:
		case EEapAkaRealm:
		case EEapPeapRealm:
		case EEapFastRealm:
			{
			iEapSettings[eapIndex]->iEapSettings->iRealmPresent = ETrue;
			iEapSettings[eapIndex]->iEapSettings->iRealm.Copy( *aValue );			
			break;
			}
				
		case EEapGtcSessionValidityTime:
		case EEapTtlsSessionValidityTime:
		case EEapTlsSessionValidityTime:
		case EEapSimSessionValidityTime:
		case EEapLeapSessionValidityTime:
		case EMschapv2SessionValidityTime:
		case EEapAkaSessionValidityTime:
		case EEapPeapSessionValidityTime:
		case EEapMschapv2SessionValidityTime:
		case EEapFastSessionValidityTime:
			{
		    // Convert the input parameter to integer
	    	TLex lex( *aValue );
			TUint value( 0 );
			if( lex.Val( value, EDecimal) != KErrNone )
				{				
				CLOG_WRITE_FORMAT( "! Error : Invalid session validity time value. EapId: %d", eapId );
				User::Leave( KErrArgument );
				}			

			iEapSettings[eapIndex]->iEapSettings->iSessionValidityTimePresent = ETrue;
			iEapSettings[eapIndex]->iEapSettings->iSessionValidityTime = value;
			break;
			}
				
		case EEapGtcEncapsulation:
		case EEapTlsEncapsulation:
		case EEapSimEncapsulation:
		case EEapAkaEncapsulation:
		case EEapMschapv2Encapsulation:
		case EMschapv2Encapsulation:
			{
			TLex lex( *aValue );				
			TUint eapTypeId( 0 );						
			if( lex.Val( eapTypeId, EDecimal) != KErrNone )
				{
				CLOG_WRITE_FORMAT( "! Error : Invalid encapsulation value. EapId: %d", eapId );
				User::Leave( KErrArgument );
				}			

			iEapSettings[eapIndex]->iEncapsulatingEapId = static_cast< EAPSettings::TEapType >( eapTypeId );
			break;
			}
		
		case EEapTlsVerifyServerRealm:
		case EEapTtlsVerifyServerRealm:
		case EEapPeapVerifyServerRealm:
		case EEapFastVerifyServerRealm:
			{
            if ( !aValue->CompareF( KStrYes ) )
                {
				iEapSettings[eapIndex]->iEapSettings->iVerifyServerRealmPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iVerifyServerRealm = ETrue;
                }
            else if ( !aValue->CompareF( KStrNo ) )
                {
                iEapSettings[eapIndex]->iEapSettings->iVerifyServerRealmPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iVerifyServerRealm = EFalse;
                }
            else 
                {                
                CLOG_WRITE_FORMAT( "! Error : Invalid VerifyServerRealm. EapId: %d", eapId );                
                User::Leave( KErrArgument );
                }

			break;
			}
						
		case EEapTlsRequireClientAuth:
		case EEapTtlsRequireClientAuth:		
		case EEapPeapRequireClientAuth:
		case EEapFastRequireClientAuth:
			{
            if ( !aValue->CompareF( KStrYes ) )
                {
				iEapSettings[eapIndex]->iEapSettings->iRequireClientAuthenticationPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iRequireClientAuthentication = ETrue;
                }
            else if ( !aValue->CompareF( KStrNo ) )
                {
                iEapSettings[eapIndex]->iEapSettings->iRequireClientAuthenticationPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iRequireClientAuthentication = EFalse;
                }
            else 
                {                
                CLOG_WRITE_FORMAT( "! Error : Invalid RequireClientAuth. EapId: %d", eapId );                
                User::Leave( KErrArgument );
                }
			
			break;
			}
		
		case EEapTlsCipherSuites:
		case EEapTtlsCipherSuites:
		case EEapPeapCipherSuites:
		case EEapFastCipherSuites:
			{
			TRAPD( err, FillCipherSuitesL( aValue, eapIndex ) );
			if( err != KErrNone )
				{
				CLOG_WRITE_FORMAT( "! Error : Invalid CipherSuites. EapId: %d", eapId );
				User::Leave( KErrArgument );
				}
			break;
			}
		
		case EEapTlsUserCertSubjectKeyId:
		case EEapTtlsUserCertSubjectKeyId:
		case EEapPeapUserCertSubjectKeyId:
		case EEapFastUserCertSubjectKeyId:
			{
			TInt certIndex = FindCertificateEntryL( CertificateEntry::EUser, eapIndex );

			TBuf<KKeyIdentifierLength> key;
			
			TRAPD( err, ConvertSubjectKeyIdToBinaryL( aValue, key) );
			if( err != KErrNone )
				{
				CLOG_WRITE_FORMAT( "! Error : Invalid UserCertSubjectKeyId. EapId: %d", eapId );
				User::Leave( KErrArgument );
				}
   			
   			iEapSettings[eapIndex]->iEapSettings->iCertificatesPresent = ETrue;
			iEapSettings[eapIndex]->iEapSettings->iCertificates[certIndex].iSubjectKeyID.Copy(key);
   			iEapSettings[eapIndex]->iEapSettings->iCertificates[certIndex].iSubjectKeyIDPresent = ETrue;
			break;
			}
		
		case EEapTlsUserCertIssuerName:
		case EEapTtlsUserCertIssuerName:
		case EEapPeapUserCertIssuerName:
		case EEapFastUserCertIssuerName:
			{
			TUint certIndex = FindCertificateEntryL( CertificateEntry::EUser, eapIndex );
			
			iEapSettings[eapIndex]->iEapSettings->iCertificatesPresent = ETrue;
			iEapSettings[eapIndex]->iEapSettings->iCertificates[certIndex].iIssuerNamePresent= ETrue;
	 		iEapSettings[eapIndex]->iEapSettings->iCertificates[certIndex].iIssuerName.Copy( *aValue );			
			break;
			}
		
		case EEapTlsUserCertSerialNumber:
		case EEapTtlsUserCertSerialNumber:
		case EEapPeapUserCertSerialNumber:
		case EEapFastUserCertSerialNumber:
			{
			TUint certIndex = FindCertificateEntryL( CertificateEntry::EUser, eapIndex );
			
			iEapSettings[eapIndex]->iEapSettings->iCertificatesPresent = ETrue;
			iEapSettings[eapIndex]->iEapSettings->iCertificates[certIndex].iSerialNumberPresent= ETrue;
	 		iEapSettings[eapIndex]->iEapSettings->iCertificates[certIndex].iSerialNumber.Copy( *aValue );			
			break;
			}
		
		case EEapTlsCaCertSubjectKeyId:
		case EEapTtlsCaCertSubjectKeyId:
		case EEapPeapCaCertSubjectKeyId:
		case EEapFastCaCertSubjectKeyId:
			{
			TInt certIndex = FindCertificateEntryL( CertificateEntry::ECA, eapIndex );

			TBuf<KKeyIdentifierLength> key;
			
			TRAPD( err, ConvertSubjectKeyIdToBinaryL( aValue, key) );
			if( err != KErrNone )
				{
				CLOG_WRITE_FORMAT( "! Error : Invalid UserCertSubjectKeyId. EapId: %d", eapId );
				User::Leave( KErrArgument );
				}

			iEapSettings[eapIndex]->iEapSettings->iCertificatesPresent = ETrue;
			iEapSettings[eapIndex]->iEapSettings->iCertificates[certIndex].iSubjectKeyID.Copy(key);   			
   			iEapSettings[eapIndex]->iEapSettings->iCertificates[certIndex].iSubjectKeyIDPresent = ETrue;
			break;
			}
		
		case EEapTlsCaCertIssuerName:
		case EEapTtlsCaCertIssuerName:		
		case EEapPeapCaCertIssuerName:
		case EEapFastCaCertIssuerName:
			{
			TUint certIndex = FindCertificateEntryL( CertificateEntry::ECA, eapIndex );
			iEapSettings[eapIndex]->iEapSettings->iCertificatesPresent = ETrue;
			iEapSettings[eapIndex]->iEapSettings->iCertificates[certIndex].iIssuerNamePresent= ETrue;
	 		iEapSettings[eapIndex]->iEapSettings->iCertificates[certIndex].iIssuerName.Copy( *aValue );			
			
			break;
			}
		
		case EEapTlsCaCertSerialNumber:
		case EEapTtlsCaCertSerialNumber:		
		case EEapPeapCaCertSerialNumber:
		case EEapFastCaCertSerialNumber:
			{
			TUint certIndex = FindCertificateEntryL( CertificateEntry::ECA, eapIndex );
			iEapSettings[eapIndex]->iEapSettings->iCertificatesPresent = ETrue;
			iEapSettings[eapIndex]->iEapSettings->iCertificates[certIndex].iSerialNumberPresent= ETrue;
	 		iEapSettings[eapIndex]->iEapSettings->iCertificates[certIndex].iSerialNumber.Copy( *aValue );
			break;
			}
		
		case EEapSimUsePseudonyms:
		case EEapAkaUsePseudonyms:
			{
            if ( !aValue->CompareF( KStrYes ) )
	            {
				iEapSettings[eapIndex]->iEapSettings->iUsePseudonymsPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iUsePseudonyms = ETrue;
	            }
	        else if ( !aValue->CompareF( KStrNo ) )
	            {
	            iEapSettings[eapIndex]->iEapSettings->iUsePseudonymsPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iUsePseudonyms = EFalse;
	            }
	        else 
	            {
	            CLOG_WRITE_FORMAT( "! Error : Invalid UsePseudonyms. EapId: %d", eapId );                
                User::Leave( KErrArgument );
	            }
			break;
			}
				
		case EEapTtlsEncapsulatedTypes:
		case EEapPeapEncapsulatedTypes:
		case EEapFastEncapsulatedTypes:
			{
		    // Lenght of a 3 digit long signed number 
		     const TInt sliceLength = 4;
    
		    TInt pos = 0;
    		while ( pos + sliceLength <= aValue->Length() )
        		{
	        	// Getting a slice
	        	TPtrC16 slice = aValue->Mid( pos, sliceLength );
	        
	        	// Checks the sign
	        	if( slice[0] == '+' )
	            	{
         		   	TLex lex( slice.Ptr() + 1 );		
					TUint encapsEapId( 0 );		
					
					if( lex.Val( encapsEapId, EDecimal) != KErrNone )
						{				
						CLOG_WRITE_FORMAT( "! Error : Invalid EncapsulatedTypes. EapId: %d", eapId );
						User::Leave( KErrArgument );
						}								
					
           			iEapSettings[eapIndex]->iEapSettings->iEncapsulatedEAPTypes.Append( encapsEapId );
					iEapSettings[eapIndex]->iEapSettings->iEncapsulatedEAPTypesPresent = ETrue;
	            	}
	                    	       
		        // Step over one slice and "," e.g. "+023,"
		        pos+=5;            
        		}

			break;
			}
		
		case EEapPeapV0Allowed:
			{
            if ( !aValue->CompareF( KStrYes ) )
	            {
				iEapSettings[eapIndex]->iEapSettings->iPEAPVersionsPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iPEAPv0Allowed = ETrue;
	            }
	        else if ( !aValue->CompareF( KStrNo ) )
	            {
	            iEapSettings[eapIndex]->iEapSettings->iPEAPVersionsPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iPEAPv0Allowed = EFalse;
	            }
	        else 
	            {
   	            CLOG_WRITE_FORMAT( "! Error : Invalid EapPeapV0Allowed. EapId: %d", eapId );                
                User::Leave( KErrArgument );
	            }
			
			break;
			}
		
		case EEapPeapV1Allowed:
			{
            if ( !aValue->CompareF( KStrYes ) )
	            {
				iEapSettings[eapIndex]->iEapSettings->iPEAPVersionsPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iPEAPv1Allowed = ETrue;
	            }
	        else if ( !aValue->CompareF( KStrNo ) )
	            {
	            iEapSettings[eapIndex]->iEapSettings->iPEAPVersionsPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iPEAPv1Allowed = EFalse;
	            }
	        else 
	            {
   	            CLOG_WRITE_FORMAT( "! Error : Invalid EapPeapV1Allowed. EapId: %d", eapId );                
                User::Leave( KErrArgument );
	            }
			
			break;
			}
		
		case EEapPeapV2Allowed:
			{
            if ( !aValue->CompareF( KStrYes ) )
	            {
				iEapSettings[eapIndex]->iEapSettings->iPEAPVersionsPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iPEAPv2Allowed = ETrue;
	            }
	        else if ( !aValue->CompareF( KStrNo ) )
	            {
	            iEapSettings[eapIndex]->iEapSettings->iPEAPVersionsPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iPEAPv2Allowed = EFalse;
	            }
	        else 
	            {
   	            CLOG_WRITE_FORMAT( "! Error : Invalid EapPeapV2Allowed. EapId: %d", eapId );
                User::Leave( KErrArgument );
	            }
			
			break;
			}
				
		case EEapFastAuthProvModeAllowed:
			{
            if ( !aValue->CompareF( KStrYes ) )
	            {
				iEapSettings[eapIndex]->iEapSettings->iAuthProvModeAllowedPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iAuthProvModeAllowed = ETrue;
	            }
	        else if ( !aValue->CompareF( KStrNo ) )
	            {
	            iEapSettings[eapIndex]->iEapSettings->iAuthProvModeAllowedPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iAuthProvModeAllowed = EFalse;
	            }
	        else 
	            {
  	            CLOG_WRITE_FORMAT( "! Error : Invalid EEapFastAuthProvModeAllowed. EapId: %d", eapId );                
                User::Leave( KErrArgument );
	            }
			
			break;
			}
		
		case EEapFastUnauthProvModeAllowed:
			{
            if ( !aValue->CompareF( KStrYes ) )
	            {
				iEapSettings[eapIndex]->iEapSettings->iUnauthProvModeAllowedPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iUnauthProvModeAllowed = ETrue;
	            }
	        else if ( !aValue->CompareF( KStrNo ) )
	            {
	            iEapSettings[eapIndex]->iEapSettings->iUnauthProvModeAllowedPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iUnauthProvModeAllowed = EFalse;
	            }
	        else 
	            {

  	            CLOG_WRITE_FORMAT( "! Error : Invalid EapFastUnauthProvModeAllowed. EapId: %d", eapId );                
                User::Leave( KErrArgument );
	            }
			
			break;
			}
		
		case EEapFastWarnADHPNoPAC:
			{
            if ( !aValue->CompareF( KStrYes ) )
	            {
				iEapSettings[eapIndex]->iEapSettings->iWarnADHPNoPACPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iWarnADHPNoPAC = ETrue;
	            }
	        else if ( !aValue->CompareF( KStrNo ) )
	            {
	            iEapSettings[eapIndex]->iEapSettings->iWarnADHPNoPACPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iWarnADHPNoPAC = EFalse;
	            }
	        else 
	            {
  	            CLOG_WRITE_FORMAT( "! Error : Invalid EapFastWarnADHPNoPAC. EapId: %d", eapId );                
                User::Leave( KErrArgument );
	            }
			
			break;
			}
		
		case EEapFastWarnADHPNoMatchingPAC:
			{
            if ( !aValue->CompareF( KStrYes ) )
	            {
				iEapSettings[eapIndex]->iEapSettings->iWarnADHPNoMatchingPACPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iWarnADHPNoMatchingPAC = ETrue;
	            }
	        else if ( !aValue->CompareF( KStrNo ) )
	            {
	            iEapSettings[eapIndex]->iEapSettings->iWarnADHPNoMatchingPACPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iWarnADHPNoMatchingPAC = EFalse;
	            }
	        else 
	            {
  	            CLOG_WRITE_FORMAT( "! Error : Invalid EapFastWarnADHPNoMatchingPAC. EapId: %d", eapId );                
                User::Leave( KErrArgument );
	            }
			
			break;
			}
		
		case EEapFastWarnNotDefaultServer:
			{
            if ( !aValue->CompareF( KStrYes ) )
	            {
				iEapSettings[eapIndex]->iEapSettings->iWarnNotDefaultServerPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iWarnNotDefaultServer = ETrue;
	            }
	        else if ( !aValue->CompareF( KStrNo ) )
	            {
	            iEapSettings[eapIndex]->iEapSettings->iWarnNotDefaultServerPresent = ETrue;
				iEapSettings[eapIndex]->iEapSettings->iWarnNotDefaultServer = EFalse;
	            }
	        else 
	            {
  	            CLOG_WRITE_FORMAT( "! Error : Invalid EapFastWarnNotDefaultServer. EapId: %d", eapId );                
                User::Leave( KErrArgument );
	            }
			
			break;
			}
			
		default:		
			{
			
			}
		}
    }
// ---------------------------------------------------------
// CProcessorWlan::FillCipherSuitesL
// ---------------------------------------------------------
//
void CProcessorWlan::FillCipherSuitesL( const HBufC16* const aPtrTag, const TInt aEapIndex )
    {
    if ( iEapSettings[aEapIndex] == NULL )
        {
		User::Leave( KErrArgument );
        }

    iEapSettings[aEapIndex]->iEapSettings->iCipherSuites.Reset();
    iEapSettings[aEapIndex]->iEapSettings->iCipherSuitesPresent = EFalse; // init to EFalse
    
    TChar plus( '+' );
    
    TBuf8<256> tmpBuf;
   	tmpBuf.Copy( *aPtrTag);
   			
    TInt length = tmpBuf.Length();
    
    for ( TInt i = 0 ; i + 3 < length ; i += 5)
        {
        if ( tmpBuf[i] == plus )
            {
            TLex8 lex( tmpBuf.Mid( i + 1, 3) );
            TUint suite;
            User::LeaveIfError( lex.Val( suite ) );
            iEapSettings[aEapIndex]->iEapSettings->iCipherSuites.Append(suite);
            iEapSettings[aEapIndex]->iEapSettings->iCipherSuitesPresent = ETrue;
            }
        }
	}
    
// ---------------------------------------------------------
// CProcessorWlan::FindCertificateEntry
// ---------------------------------------------------------
//
TUint CProcessorWlan::FindCertificateEntryL( const CertificateEntry::TCertType aCertType, const TInt aEapIndex )
    {
    TUint certIndex( 0 );
	for( certIndex = 0; certIndex < iEapSettings[aEapIndex]->iEapSettings->iCertificates.Count() ; certIndex++ )
		{
		if( iEapSettings[aEapIndex]->iEapSettings->iCertificates[certIndex].iCertType == aCertType )
			{
			// Found
			break;
			}
		}
	if( certIndex == iEapSettings[aEapIndex]->iEapSettings->iCertificates.Count() )
		{
		// Not found. Create
		CertificateEntry entry;

		entry.iCertType = aCertType;

		iEapSettings[aEapIndex]->iEapSettings->iCertificates.AppendL( entry );

		certIndex = iEapSettings[aEapIndex]->iEapSettings->iCertificates.Count() - 1;
		}
	return certIndex;	
	}

// ---------------------------------------------------------
// CProcessorWlan::ConvertSubjectKeyIdToBinary
// ---------------------------------------------------------
//
void CProcessorWlan::ConvertSubjectKeyIdToBinaryL( const HBufC16* const aSubjectKeyIdString, TDes& aBinaryKey)
	{
	TInt err( KErrNone );
	
		// Define literals to search the subject key for possible 0X/0x beginning
	_LIT(KHexIdLC, "0x");
	_LIT(KHexIdUC, "0X");
	TBuf<2> hexIdBuf;
	
	// The lenght of the key given as input
	TInt keyLen = aSubjectKeyIdString->Length();
	
	// setting the given key to the key buffer
	TBuf8<256> origKey;
	
	origKey.Copy( *aSubjectKeyIdString );
	origKey.SetLength(keyLen);
		
	TLex8 tmpByte;
	TUint16 byte;
	
	// Remove possible spaces from the beginning
	origKey.TrimLeft();
	
	// the key should be atleast two chars long
	if (origKey.Length() >= 2)
		{
		// Copy the two left most characters in to the buffer
		hexIdBuf.Copy(origKey.Left(2));
		
		// If the first characters are 0x or 0X, then thet should be ignored
		if (hexIdBuf.Compare(KHexIdLC) == 0
				|| hexIdBuf.Compare(KHexIdUC) == 0)
			{
			// delete two characters
			origKey.Delete(0, 2);
			}
		}


	// looping the subject key through, removing whitespaces
	for (TInt i = 0; i < KKeyIdentifierLength; i++)
		{
		// removing white spaces from the left side of the key
		origKey.TrimLeft();
		// check that there are characters left
		if (origKey.Length() >= 2)
			{
			// pick the two left most bytes from the key
			tmpByte = origKey.Left(2);
			// convert byte into binary format
			err = tmpByte.Val(byte, EHex);
			
			// check whether conversion to decimal went ok
			if (err != KErrNone)
				{
				// if there are problems, then leave the loop
				break;
				}
			
			// store the appended byte into the key variable
			aBinaryKey.Append(byte);
			// delete two characters from the left side of the character array in the buffer
			origKey.Delete(0, 2);
			}
		else {
			// leave the loop, no characters are left			
			break;
			}
		}
	User::LeaveIfError( err );		
	}
	

// End of File.
