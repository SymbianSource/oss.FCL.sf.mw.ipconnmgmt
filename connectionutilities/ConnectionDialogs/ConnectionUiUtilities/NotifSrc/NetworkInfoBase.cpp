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
* Description:  Base class of network info objects
*     
*
*/


// INCLUDE FILES
#include <StringLoader.h>
#include <connuiutilsnotif.rsg>

#include "NetworkInfoBase.h"
#include "ConnUiUtilsNotif.hrh"
#include "ConnectionDialogsLogger.h"


// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNetworkInfoBase::CNetworkInfoBase
// ---------------------------------------------------------
//
CNetworkInfoBase::CNetworkInfoBase()
: iSignalStrength( ESignalStrengthMin ),
  iNetworkMode( ENetworkModeInfrastructure ),
  iSecurityMode( EWlanConnectionSecurityOpen ),
  iOthersEntry( EFalse ),
  iProtectedSetupSupported( EFalse ),
  iExtendedSecurityMode( EWlanConnectionExtentedSecurityModeOpen )
    {
    }


// ---------------------------------------------------------
// CNetworkInfoBase::~CNetworkInfoBase
// ---------------------------------------------------------
//
CNetworkInfoBase::~CNetworkInfoBase()
    {
    }


// ---------------------------------------------------------
// CNetworkInfoBase::RefreshNetworksListBoxItemTextL
// ---------------------------------------------------------
//
void CNetworkInfoBase::RefreshNetworksListBoxItemTextL()
    {
    CLOG_ENTERFN( "CNetworkInfoBase::RefreshNetworksListBoxItemTextL" );  

    if ( iOthersEntry ) 
        {
        _LIT( KFormatNoIcons, "\t%S" );
        HBufC *othersEntryBuf = StringLoader::LoadL( 
                                                R_QTN_WLAN_MANUAL_SSID_INPUT );
        iListBoxItemText.Format( KFormatNoIcons, othersEntryBuf );
        delete othersEntryBuf;
        }
    else
        {
        _LIT( KFormatOneIcon, "%d\t%S\t%d" );
        _LIT( KFormatTwoIcons, "%d\t%S\t%d\t%d" );
            
        TInt iconSignal;
        TInt iconNetworkMode;
    
        if ( iSignalStrength > ESignalStrengthLow )
            {
            iconSignal = ECUUNotifIconLowSignal;
            }
        else if ( iSignalStrength < ESignalStrengthGood )
            {
            iconSignal = ECUUNotifIconGoodSignal;
            }
        else
            {
            iconSignal = ECUUNotifIconMediumSignal;
            }   
        
        iconNetworkMode = ( iNetworkMode == ENetworkModeAdHoc ) ? 
                            ECUUNotifIconAdHocNetwork : 
                            ECUUNotifIconInfrastructureNetwork;
        
        if ( iProtectedSetupSupported )
            {
            CLOG_WRITE( "Protected Setup supported" );
            iListBoxItemText.Format( KFormatTwoIcons,
                                     iconNetworkMode, 
                                     &iNetworkName,
                                     iconSignal, 
                                     ECUUNotifIconProtectedSetupSupported );
            }
        else
            {
            CLOG_WRITE( "Protected Setup not supported" );
            
     	    if ( iSecurityMode )
            	{
            	iListBoxItemText.Format( KFormatTwoIcons,
                                     	iconNetworkMode, 
                                     	&iNetworkName,
                                     	iconSignal, 
                                     	ECUUNotifIconSecureNetwork );
	            }
    	    else
        	    {
            	iListBoxItemText.Format( KFormatOneIcon,
                	                     iconNetworkMode, 
                    	                 &iNetworkName,
                        	             iconSignal );                        
            	}
			}

        }

    CLOG_LEAVEFN( "CNetworkInfoBase::RefreshNetworksListBoxItemTextL" );  
    }

// End of File
