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
* Description:   Implementation of the class CProcessorVpn
*
*/




// INCLUDE FILES

#include "cdcprocessorvpn.h"
#include "cdclogger.h"
#include "cdctagcontainer.h"

#include <cmpluginvpndef.h>
#include <cmpluginbaseeng.h>
#include <cmconnectionmethoddef.h>
#include <cmdestinationext.h>
#include <cmmanagerext.h>
#include <datamobilitycommsdattypes.h>

using namespace CMManager;

// CONSTANTS

const TUint32 KDefaultPortNum = 80;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CProcessorVpn::NewL
// ---------------------------------------------------------
//
CProcessorVpn* CProcessorVpn::NewL( CReaderBase* aFileReader,
                                    RCmManagerExt& aCmManager,
                                    RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                                    RPointerArray< HBufC >& aPluginNames,
                                    RPointerArray< HBufC >& aUnderLying )
    {
    CProcessorVpn* self = new ( ELeave ) CProcessorVpn( aFileReader,
                                                        aCmManager,
                                                        aPluginArray,
                                                        aPluginNames,
                                                        aUnderLying );
    CleanupStack::PushL( self );

    // From base class
    self->ConstructL();
    CleanupStack::Pop( self ); // self
    return self;
    }
    
    
// ---------------------------------------------------------
// CProcessorVpn::~CProcessorVpn
// ---------------------------------------------------------
//
CProcessorVpn::~CProcessorVpn()
    {
    }
     
     
// ---------------------------------------------------------
// CProcessorVpn::ConstructL
// ---------------------------------------------------------
//
void CProcessorVpn::ConstructL()
    {
    }


// ---------------------------------------------------------
// CProcessorVpn::CProcessorVpn
// ---------------------------------------------------------
//
CProcessorVpn::CProcessorVpn( CReaderBase* aFileReader,
                              RCmManagerExt& aCmManager,
                              RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                              RPointerArray< HBufC >& aPluginNames,
                              RPointerArray< HBufC >& aUnderLying  ) :
                              
                              CProcessorBase( aFileReader,
                                              aCmManager,
                                              aPluginArray,
                                              aPluginNames )
    {
    iUnderLying = &aUnderLying;
    iBearerType = KPluginVPNBearerTypeUid;
    }

// ---------------------------------------------------------
// CProcessorVpn::ProcessTagL
// ---------------------------------------------------------
//
void CProcessorVpn::ProcessUnderLyingL( RPointerArray< RCmDestinationExt >& aDestArray,
                                        RPointerArray< HBufC >& aDestNames )
    {
    iDestArray = &aDestArray;
    iDestName = &aDestNames;
    
    for( TInt i(0); i < iUnderLying->Count(); i++ )
        {
        // Finds the vpn IAP by its name
        RCmConnectionMethodExt* iPlugin = FindPluginL( (*iUnderLying)[i] );
        if( iPlugin != NULL )
            {
            CLOG_WRITE_FORMAT( "Underlying set for:%S", (*iUnderLying)[i] );  
            
            // Finds the underlying IAP by its name
            i++;
            RCmConnectionMethodExt* up = FindPluginL( (*iUnderLying)[i] );
            if( up != NULL )
                {
                iPlugin->SetIntAttributeL( ECmNextLayerIapId,
                            up->GetIntAttributeL( ECmIapId ) );
                iPlugin->UpdateL();                            

                CLOG_WRITE_FORMAT( "Underlying IAP Set:%S", (*iUnderLying)[i] );  
                }
            else
                {
                // Underlyinf can be destination. If not IAP finds destination.
                RCmDestinationExt* ud = FindDestinationL( (*iUnderLying)[i] );
                if( ud != NULL )
                    {
                    iPlugin->SetIntAttributeL( ECmNextLayerSNAPId, ud->Id() );
                    iPlugin->UpdateL();                            

                    CLOG_WRITE_FORMAT( "Underlying SNAP Set: %S", (*iUnderLying)[i] );  
                    }
                }    
            }
        }
    }


// ---------------------------------------------------------
// CProcessorVpn::ProcessTagL
// ---------------------------------------------------------
//
void CProcessorVpn::ProcessSpecialFieldsL( TInt aField, HBufC* aPtrTag, TInt /*aDx*/ )
    {
        CLOG_WRITE_FORMAT( "field: %d", aField );  
        switch ( aField )
            {
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
            case EVPN_IapName: // Underlying IAP/destination
                {
                if( !iName )
                    {
                    TInt len = aPtrTag->Length();
                
                    iName = HBufC::NewL( 8 + len );
                    TPtr name = iName->Des();
                    name.Append( _L( "VPN to " ) );
                    name.Append( *aPtrTag );
                    
                    SetGeneralFieldL( ECmName, iName );
                    
                    }
                    
                // Adds vpnIAP name - underlying IAP/Dest name pair to a list
                if( iName )
                    {
                    iUnderLying->AppendL( iName->AllocL() );
                    iUnderLying->AppendL( aPtrTag->AllocL() );
                    }
                break;
                }
            case EVpnServicePolicy:
            ////case ECmName:                //fall through
                {
                iPlugin.SetStringAttributeL( aField, *aPtrTag );
                if( !iName )
                    {
                    iName = aPtrTag->AllocLC();
                    }

                break;
                }
            default:
                {
                break;
                }
            }//switch
    }

// End of File.
