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
* Description:   Implementation of the class CProcessorPacketData
*
*/




// INCLUDE FILES

#include "cdcprocessorpacketdata.h"
#include "cdclogger.h"
#include "cdctagcontainer.h"

#include <etelpckt.h>
#include <cmpluginpacketdatadef.h>
#include <cmpluginbaseeng.h>
#include <cmconnectionmethoddef.h>
#include <cmmanagerext.h>
#include <datamobilitycommsdattypes.h>

#include <centralrepository.h>
#include <pdpcontextmanagerinternalcrkeys.h>

using namespace CMManager;

// CONSTANTS

const TUint32 KDefaultPortNum = 80;
const TInt32 KDefaultLinger = 0;
const TUint KIapColumn        = 0x00000100;
const TUint KLingerColumn     = 0x00000200;
const TUint KColumnMask       = 0xFFFFFF00;
const TUint KRowMask          = 0x000000FF;

// These constants are used to identify tags of iInFile.

//ISP types
_LIT16( KStrISPInternet,      "0" );
_LIT16( KStrISPWAP,      "1" );

//Connection type
_LIT16( KStrContinuous,      "Continuous" );
_LIT16( KStrTemporary,       "Temporary" );

//Password authentication
_LIT16( KStrPwdAuthNormal,   "Normal" );
_LIT16( KStrPwdAuthSecure,   "Secure" );

//Network types
_LIT16( KStrNetworkIPv4,     "IPv4" );
_LIT16( KStrNetworkIPv6,     "IPv6" );


// ================= MEMBER FUNCTIONS =======================
CLingerElement::~CLingerElement()
    {
    delete iName;
    delete iLinger;
    }
// ---------------------------------------------------------
// CProcessorPacketData::NewL
// ---------------------------------------------------------
//
CProcessorPacketData* CProcessorPacketData::NewL( 
                                CReaderBase* aFileReader,
                                RCmManagerExt& aCmManager,
                                RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                                RPointerArray< HBufC >& aPluginNames,
                                RPointerArray< CLingerElement >& aLingerSettings)
    {
    CProcessorPacketData* self = new ( ELeave ) CProcessorPacketData
                                                            ( aFileReader,
                                                              aCmManager,
                                                              aPluginArray,
                                                              aPluginNames,
                                                              aLingerSettings);
    CleanupStack::PushL( self );

    // From base class
    self->ConstructL();
    CleanupStack::Pop( self ); // self
    return self;
    }
    
    
// ---------------------------------------------------------
// CProcessorPacketData::~CProcessorPacketData
// ---------------------------------------------------------
//
CProcessorPacketData::~CProcessorPacketData()
    {
    }
     
     
// ---------------------------------------------------------
// CProcessorPacketData::ConstructL
// ---------------------------------------------------------
//
void CProcessorPacketData::ConstructL()
    {
    }


// ---------------------------------------------------------
// CProcessorPacketData::CProcessorPacketData
// ---------------------------------------------------------
//
CProcessorPacketData::CProcessorPacketData( CReaderBase* aFileReader,
                      RCmManagerExt& aCmManager,
                      RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                      RPointerArray< HBufC >& aPluginNames,
                      RPointerArray< CLingerElement >& aLingerSettings ) :
                                      CProcessorBase( aFileReader,
                                                      aCmManager,
                                                      aPluginArray,
                                                      aPluginNames )
                                      
    {
    iLingerSettings = &aLingerSettings;
    iBearerType = KUidPacketDataBearerType;
    }

// ---------------------------------------------------------
// CProcessorPacketData::ProcessTagL
// ---------------------------------------------------------
//
void CProcessorPacketData::ProcessSpecialFieldsL( TInt aField, HBufC* aPtrTag, TInt /*aDx*/ )
    {

    switch ( aField )
        {
        case EPacketDataAPName:               //fall through
        case EPacketDataIFAuthName:           //fall through
        case EPacketDataIFAuthPass:           //fall through
        case EPacketDataIPAddr:               //fall through
            {
             // Unicode 16-bit text
            iPlugin.SetStringAttributeL( aField, *aPtrTag );

            break;
            }
        case EPacketDataIPNameServer1:        //fall through
        case EPacketDataIPNameServer2:        //fall through
        case EPacketDataIPIP6NameServer1:     //fall through
        case EPacketDataIPIP6NameServer2:     //fall through
            {
            iPlugin.SetStringAttributeL( aField, *aPtrTag );
            SetAddressFromServerL( EPacketDataIPNameServer1,
                                  EPacketDataIPNameServer2,
                                  EPacketDataIPDNSAddrFromServer,
                                  EPacketDataIPIP6NameServer1,
                                  EPacketDataIPIP6NameServer2,
                                  EPacketDataIPIP6DNSAddrFromServer );    
            break;
            }
        case ECmWapIPGatewayAddress:
            {
             // Unicode 16-bit text
            if ( aPtrTag->Length() )
                {
                iPlugin.SetStringAttributeL( aField, *aPtrTag );
                }

            break;
            }
        case EPacketDataIFPromptForAuth:
            {
            // Boolean
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
                //no valid data is given - use default value
                iPlugin.SetBoolAttributeL( aField, EFalse );
                CLOG_WRITE( "! Error : Invalid Prompt password. Default:No");
                }

            break;
            }
        case EPacketDataUseEdge:
            {
            // Boolean
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
                //no valid data is given - use default value
                iPlugin.SetBoolAttributeL( aField, ETrue );
                CLOG_WRITE( "! Error : Invalid Use EDGE. Default:Yes");
                }

            break;
            }
        case EPacketDataLinger:
            {
            // Pair linger with name for later saving
            CLingerElement* linger = new (ELeave) CLingerElement;

            CleanupStack::PushL( linger );
                    
            linger->iName = iName->AllocL();
            linger->iLinger = aPtrTag->AllocL(); 
                                            
            iLingerSettings->AppendL(linger);
                    
            CleanupStack::Pop( linger );
            break;
            }
        case ECmWapIPSecurity:
            {
             // TBool                
            if ( !aPtrTag->CompareF( KStrOn ) )
                {
                iPlugin.SetBoolAttributeL( aField, ETrue );
                }
            else if ( !aPtrTag->CompareF( KStrOff ) )
                {
                iPlugin.SetBoolAttributeL( aField, EFalse );
                }
            else
                {
                //no valid data is given - default data is used
                iPlugin.SetBoolAttributeL( aField, ETrue );
                CLOG_WRITE( "! Error : Invalid WTLS security setting. Default:On");
                }

            break;
            }
        case ECmWapIPWSPOption:
            {
            // UInt                
            if ( !aPtrTag->CompareF( KStrContinuous ) )
                {
                iPlugin.SetIntAttributeL( aField, 
                                  EWapWspOptionConnectionOriented );
                }
            else if ( !aPtrTag->CompareF( KStrTemporary ) )
                {
                iPlugin.SetIntAttributeL( aField, 
                                  EWapWspOptionConnectionless );
                }
            else
                {
                //no valid data is given - default data is used
                iPlugin.SetIntAttributeL( aField, 
                                  EWapWspOptionConnectionOriented );
                CLOG_WRITE( "! Error : Invalid connection type. Default:Continuous");
                }

            break;
            }
            
        case ECmProxyProtocolName:
        	{
        	// Do not enable proxy in this case yet
        	iPlugin.SetStringAttributeL( aField, *aPtrTag );
        	break;
        	}
                         
        case ECmProxyServerName:    
            {
            // First set proxy usage to enabled
            if( !iProxyEnabled )
                {
                iPlugin.SetBoolAttributeL( ECmProxyUsageEnabled, ETrue );
                iProxyEnabled = ETrue;
                }
            // Long text
            iPlugin.SetStringAttributeL( aField, *aPtrTag );
            break;
            }
        case ECmProxyPortNumber:
            {
            // First set proxy usage to enabled
            if( !iProxyEnabled )
                {
                iPlugin.SetBoolAttributeL( ECmProxyUsageEnabled, ETrue );
                iProxyEnabled = ETrue;
                }
             // TUint32 and should be read as text.
            TPtrC16 portptr = aPtrTag->Right( aPtrTag->Length() );
            TLex16 lex( portptr );
            TUint32 portnum( 0 );
            if ( lex.Val( portnum, EDecimal ) == KErrNone )
                {
                iPlugin.SetIntAttributeL( aField, portnum );
                }
            else 
                {
                //no valid data is given - default value is used
                iPlugin.SetIntAttributeL( aField, KDefaultPortNum );
                CLOG_WRITE( "! Error : Invalid port number. Default:80");  
                }
            break;
            }
        case EPacketDataDisablePlainTextAuth:
            {
             // TBool                
            if ( !aPtrTag->CompareF( KStrPwdAuthNormal ) )
                {
                // Normal, => auth. not secure, plain text NOT disabled
                iPlugin.SetBoolAttributeL( aField, EFalse );
                }
            else if ( !aPtrTag->CompareF( KStrPwdAuthSecure ) )
                {
                // Secure, => auth. secure, plain text disabled
                iPlugin.SetBoolAttributeL( aField, ETrue );
                }
            else
                {
                //no valid data is given - default value is used
                CLOG_WRITE( "! Error : Invalid password authentication. Default:Secure ");  
                iPlugin.SetBoolAttributeL( aField, ETrue );
                }
            break;
            }
        case EPacketDataPDPType:
            {
            if ( !aPtrTag->CompareF( KStrNetworkIPv4 ) )
                {
                iPlugin.SetIntAttributeL( aField, RPacketContext::EPdpTypeIPv4 );
                }
            else if ( !aPtrTag->CompareF( KStrNetworkIPv6 ) )
                {
                iPlugin.SetIntAttributeL( aField, RPacketContext::EPdpTypeIPv6 );
                }
            else
                {
                iPlugin.SetIntAttributeL( aField, RPacketContext::EPdpTypeIPv4 );
                CLOG_WRITE( "! Error : Invalid Network type. Default:IPv4" )
                }
            break;
            }
        case EPacketDataApType:
        		{
        		if ( !aPtrTag->CompareF( KStrISPInternet ) )
            		{
            		iPlugin.SetIntAttributeL( aField, 0 );              
                }
            else if ( !aPtrTag->CompareF( KStrISPWAP ) )
                {
                iPlugin.SetIntAttributeL( aField, 1 );                              
            		}
            else
               	{
               	iPlugin.SetIntAttributeL( aField, 2 );
                }  
        		break;
        	}
        default:
            {
            break;
            }
        }//switch
    }

// ---------------------------------------------------------
// CProcessorPacketData::AddLingerL
// ---------------------------------------------------------
//
void CProcessorPacketData::AddLingerL( const TInt aIapId, const TInt aLingerInterval)
    {
    TInt    err( KErrNone );
    
    // Open repository
    CRepository* repository = CRepository::NewLC( KCRUidPDPContextManager );
    
    // Find if an entry for "iapId" already exists in CentRep 
    RArray< TUint32 > foundKeys;
    foundKeys.Reset();
    
    err = repository->FindEqL( KIapColumn,       // partial key
                               KColumnMask,      // key mask
                               aIapId,           // value
                               foundKeys );      // found keys
    
    if ( err == KErrNone || err == KErrNotFound )
        {
        if ( foundKeys.Count() == 0 )
            {
            // Add a new entry
            TInt arrayCount( 0 );
            
            // Get number of iapId&linger entries in Centrep
            err = repository->Get( KPdpContextManagerLingerArrayCount, 
                                   arrayCount );
        
            if ( err == KErrNone )
                {
                arrayCount++;
                
                err = repository->Create( ( KIapColumn | arrayCount ), aIapId ); 
                
                if ( err == KErrNone )
                    {
                    err = repository->Create( ( KLingerColumn | arrayCount ), 
                                               aLingerInterval );     
                    }
                
                if ( err == KErrNone )
                    {
                    // Update number of iapId&linger entries in Centrep
                    err = repository->Set( KPdpContextManagerLingerArrayCount, 
                                           arrayCount );
                    }
                }
            }
        else
            {
            // Update existing entry on current row
            TInt32 row = foundKeys[ 0 ] & KRowMask;
            
            err = repository->Set( ( KLingerColumn | row ), aLingerInterval ); 
            }         
        }
        
    foundKeys.Close();
    CleanupStack::PopAndDestroy( repository );
    
    User::LeaveIfError( err );
    }

// ---------------------------------------------------------
// CProcessorPacketData::SaveLingerSettingsL
// ---------------------------------------------------------
//
void CProcessorPacketData::SaveLingerSettingsL()
    {
    for ( TInt i(0); i < iLingerSettings->Count(); i++ )
        {
        TInt id = GetPluginIdL(((*iLingerSettings)[i])->iName);
        
        TLex16 lex(*((*iLingerSettings)[i])->iLinger);
        TInt linger( 0 );
        if ( lex.Val( linger ) == KErrNone )         
            {
            AddLingerL( id, linger);
            }
            else 
            {
            // no valid data is given - default value is used
            AddLingerL( id, KDefaultLinger); 
            }
        }
        iLingerSettings->ResetAndDestroy();
    }
     
// End of File.
