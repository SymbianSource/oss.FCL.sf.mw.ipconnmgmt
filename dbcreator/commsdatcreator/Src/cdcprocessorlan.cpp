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
* Description:   Implementation of the class CProcessorLan
*
*/




// INCLUDE FILES

#include "cdcprocessorlan.h"
#include "cdclogger.h"
#include "cdctagcontainer.h"

#include <cmpluginlandef.h>
#include <cmpluginlanbasedef.h>
#include <cmpluginbaseeng.h>
#include <cmconnectionmethoddef.h>
#include <cmmanagerext.h>
#include <datamobilitycommsdattypes.h>

using namespace CMManager;

// CONSTANTS

const TUint32 KDefaultPortNum = 80;

// These constants are used to identify tags of iInFile.

//Connection type
_LIT16( KStrContinuous,      "Continuous" );
_LIT16( KStrTemporary,       "Temporary" );

_LIT16( KStrTrue,               "True" );
_LIT16( KStrFalse,              "False" );


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CProcessorLan::NewL
// ---------------------------------------------------------
//
CProcessorLan* CProcessorLan::NewL( CReaderBase* aFileReader,
                      RCmManagerExt& aCmManager,
                      RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                      RPointerArray< HBufC >& aPluginNames )
    {
    CProcessorLan* self = new ( ELeave ) CProcessorLan( aFileReader,
                                                        aCmManager,
                                                        aPluginArray,
                                                        aPluginNames );
    CleanupStack::PushL( self );

    // From base class
    self->ConstructL();
    CleanupStack::Pop( self ); // self
    return self;
    }
    
    
// ---------------------------------------------------------
// CProcessorLan::~CProcessorLan
// ---------------------------------------------------------
//
CProcessorLan::~CProcessorLan()
    {
    }
     
     
// ---------------------------------------------------------
// CProcessorLan::ConstructL
// ---------------------------------------------------------
//
void CProcessorLan::ConstructL()
    {
    }


// ---------------------------------------------------------
// CProcessorLan::CProcessorLan
// ---------------------------------------------------------
//
CProcessorLan::CProcessorLan( CReaderBase* aFileReader,
                      RCmManagerExt& aCmManager,
                      RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                      RPointerArray< HBufC >& aPluginNames) :
                              CProcessorBase( aFileReader,
                                              aCmManager,
                                              aPluginArray,
                                              aPluginNames )
    {
    iBearerType = KUidLanBearerType;
    }


// ---------------------------------------------------------
// CProcessorLan::ProcessTagL
// ---------------------------------------------------------
//
void CProcessorLan::ProcessSpecialFieldsL( TInt aField, HBufC* aPtrTag, TInt /*aDx*/ )
    {
    switch ( aField )
        {
        case ECmWapIPGatewayAddress:
            {
             // Unicode 16-bit text
            if ( aPtrTag->Length() )
                {
                iPlugin.SetStringAttributeL( aField, *aPtrTag );
                }

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
        case ELanIpAddrFromServer:
        case ELanIpDNSAddrFromServer:
            {
             // TBool                
            if ( !aPtrTag->CompareF( KStrTrue ) )
                {
                iPlugin.SetBoolAttributeL( aField, ETrue );
                }
            else if ( !aPtrTag->CompareF( KStrFalse ) )
                {
                iPlugin.SetBoolAttributeL( aField, EFalse );
                }
            else
                {
                //no valid data is given - default data is used
                iPlugin.SetBoolAttributeL( aField, ETrue );
                CLOG_WRITE( "! Error : Invalid LAN server setting. Default:On");
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
        case ELanIfNetworks:
        case ELanIpNetMask:
        case ELanIpGateway:
        case ELanIpAddr:
        case ELanConfigDaemonManagerName:
        case ELanConfigDaemonName:
            {
            iPlugin.SetStringAttributeL( aField, *aPtrTag );
            break;
            }
        case ELanIpNameServer1:
        case ELanIpNameServer2:
        case ELanIp6NameServer1:
        case ELanIp6NameServer2:
            {
            iPlugin.SetStringAttributeL( aField, *aPtrTag );
            SetAddressFromServerL( ELanIpNameServer1,
                                  ELanIpNameServer2,
                                  ELanIpDNSAddrFromServer,
                                  ELanIp6NameServer1,
                                  ELanIp6NameServer2,
                                  ELanIp6DNSAddrFromServer );    
            break;
            }
        default:
            {
            break;
            }
        }//switch
    }
     
// End of File.
