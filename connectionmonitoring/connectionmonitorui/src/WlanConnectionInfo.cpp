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
* Description:  Represents a WLAN connection
*     
*
*/


// INCLUDE FILES
#include <connectionmonitorui.rsg>
#include <ConnectionMonitorUi.mbg>

#include "ConnectionArray.h"
#include "WlanConnectionInfo.h"
#include "EasyWLANConnectionInfo.h"
#include "ActiveWrapper.h"

#include <StringLoader.h>

#include "ConnectionMonitorUiLogger.h"

// CONSTANTS

/**
* Count of details of WLAN connection
*/
LOCAL_D const TUint KDetailsArrayMaxGranularityCCX = 15;

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CWlanConnectionInfo::NewL
// ---------------------------------------------------------
//
CWlanConnectionInfo* CWlanConnectionInfo::NewL( TInt aConnectionId, 
                                RConnectionMonitor* const aConnectionMonitor,
                                TConnMonBearerType aConnectionBearerType,
                                const HBufC* aEasyWlanIAPName,
                                CActiveWrapper* aActiveWrapper )
    {
    CMUILOGGER_ENTERFN( "CWlanConnectionInfo* CWlanConnectionInfo::NewL" ); 
       
    CWlanConnectionInfo* self = new ( ELeave ) CWlanConnectionInfo( 
                                                      aConnectionId, 
                                                      aConnectionMonitor, 
                                                      aConnectionBearerType,
                                                      aActiveWrapper );
    CleanupStack::PushL( self );
    self->ConstructL();
    
    if ( aEasyWlanIAPName )
        {
        if ( self->iConnectionName.Compare( *aEasyWlanIAPName ) )
            {
            CleanupStack::Pop( self );        
            }
        else
            {
            CEasyWlanConnectionInfo* temp = CEasyWlanConnectionInfo::NewL( 
                                                      aConnectionId, 
                                                      aConnectionMonitor, 
                                                      aConnectionBearerType,
                                                      aActiveWrapper );
            CleanupStack::PopAndDestroy( self );
            self = temp;
            }        
        }
    else
        {
        CleanupStack::Pop( self );                            
        }
 
    
    
    CMUILOGGER_LEAVEFN( "CWlanConnectionInfo* CWlanConnectionInfo::NewL" );
        
    return self;
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::CWlanConnectionInfo
// ---------------------------------------------------------
//
CWlanConnectionInfo::CWlanConnectionInfo( TInt aConnectionId, 
                                RConnectionMonitor* const aConnectionMonitor,
                                TConnMonBearerType aConnectionBearerType,
                                CActiveWrapper* aActiveWrapper ) 
    : CConnectionInfoBase( aConnectionId, 
                           aConnectionMonitor, 
                           aConnectionBearerType,
                           aActiveWrapper ), iEasyWlan( EFalse )
    {
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::ConstructL
// ---------------------------------------------------------
//
void CWlanConnectionInfo::ConstructL()
    {  
    CMUILOGGER_ENTERFN( "void CWlanConnectionInfo::ConstructL()" ); 
     
    RefreshNetworkNameL();
    RefreshSignalStrength();
    RefreshSecurityMode();
    RefreshNetworkMode();        
    RefreshTxPowerL();        
    
    CConnectionInfoBase::ConstructL(); // and base class ConstructL next.
    
    CMUILOGGER_LEAVEFN( "void CWlanConnectionInfo::ConstructL()" );    
    }


// ---------------------------------------------------------
// CWlanConnectionInfo::~CWlanConnectionInfo
// ---------------------------------------------------------
//
CWlanConnectionInfo::~CWlanConnectionInfo()
    {
    }
    
// ---------------------------------------------------------
// CWlanConnectionInfo::DeepCopyL
// ---------------------------------------------------------
//        
CConnectionInfoBase* CWlanConnectionInfo::DeepCopyL()
	{
	CWlanConnectionInfo* temp = new ( ELeave ) CWlanConnectionInfo(
                                                    iConnectionId,
                                                    iConnectionMonitor,
                                                    iConnectionBearerType,
                                                    iActiveWrapper );
                              							
	temp->iConnectionName = iConnectionName;
	temp->iConnectionStatus = iConnectionStatus;
	temp->iUploaded = iUploaded;
	temp->iDownloaded = iDownloaded;
	temp->iStartTime = iStartTime;
	temp->iDuration = iDuration;
	temp->iLastSpeedUpdate = iLastSpeedUpdate;
	temp->iUpSpeed = iUpSpeed;
	temp->iDownSpeed = iDownSpeed;
	temp->iListBoxItemText = iListBoxItemText;
	
	temp->iWlanNetworkName = iWlanNetworkName;
	temp->iSignalStrength = iSignalStrength;
	temp->iNetworkMode = iNetworkMode;
	temp->iSecurityMode = iSecurityMode;
	temp->iTxPower = iTxPower;
	
	if ( iDetailsArray )
		{
		CleanupStack::PushL( temp );		
		temp->ToArrayDetailsL();
		CleanupStack::Pop( temp );			
		}
		
	return temp;	
	}    

// ---------------------------------------------------------
// CWlanConnectionInfo::GetIconId
// ---------------------------------------------------------
//
TUint CWlanConnectionInfo::GetIconId() const
    {
    CMUILOGGER_ENTERFN( "TUint CWlanConnectionInfo::GetIconId() const" ); 
    
    TUint iconId = EWlan;    
    
    CMUILOGGER_LEAVEFN( "TUint CWlanConnectionInfo::GetIconId() const" );
    
    return iconId;
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::IsEasyWLAN
// ---------------------------------------------------------
//
TBool CWlanConnectionInfo::IsEasyWLAN() const
    {
    return iEasyWlan;    
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::GetWlanNetworkNameLC
// ---------------------------------------------------------
//
HBufC* CWlanConnectionInfo::GetWlanNetworkNameLC() const
    {
    CMUILOGGER_ENTERFN( "CWlanConnectionInfo::GetWlanNetworkNameLC()" ); 
    
    HBufC* name = iWlanNetworkName.AllocLC();
    
    CMUILOGGER_LEAVEFN( "CWlanConnectionInfo::GetWlanNetworkNameLC()" );
    
    return name;    
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::GetWlanSignalStrength
// ---------------------------------------------------------
//
TWlanSignalStrength CWlanConnectionInfo::GetWlanSignalStrength()  const
    {
    return iSignalStrength;
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::GetWlanNetworkMode
// ---------------------------------------------------------
//
TWlanNetworkMode CWlanConnectionInfo::GetWlanNetworkMode()  const
    {
    return iNetworkMode;
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::GetWlanSecurityMode
// ---------------------------------------------------------
//
TWlanSecurityMode CWlanConnectionInfo::GetWlanSecurityMode()  const
    {
    return iSecurityMode;
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::GetWlanTxPower
// ---------------------------------------------------------
//
TUint CWlanConnectionInfo::GetWlanTxPower() const
    {
    return iTxPower;
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::RefreshDetailsL
// ---------------------------------------------------------
//
void CWlanConnectionInfo::RefreshDetailsL()
    {
    CMUILOGGER_ENTERFN( "void CWlanConnectionInfo::RefreshDetailsL()" ); 

    if ( IsAlive() )
        {
        RefreshNetworkNameL();
        }
    
    if ( IsAlive() )
        {
        RefreshNetworkMode();
        }
    
    if ( IsAlive() )
        {
        RefreshSecurityMode();
        }
    
    if ( IsAlive() )
        {
        RefreshTransferSpeedsL();
        }
    
    if ( IsAlive() )
        {
        RefreshDuration(); 
        }
    
    if ( IsAlive() )
        {
        RefreshSignalStrength();
        }
    
    if ( IsAlive() )
        {
        RefreshTxPowerL();
        }
    
    CMUILOGGER_LEAVEFN( "void CWlanConnectionInfo::RefreshDetailsL()" );    
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::RefreshNetworkNameL
// ---------------------------------------------------------
//
void CWlanConnectionInfo::RefreshNetworkNameL()
    {
    CMUILOGGER_ENTERFN( "void CWlanConnectionInfo::RefreshNetworkNameL()" ); 
    
    iActiveWrapper->StartGetStringAttribute( iConnectionId,
                                             iConnectionMonitor,
                                             KNetworkName,
                                             iWlanNetworkName );
    
    CMUILOGGER_WRITE_F( "RefreshNetworkName status : %d", 
                        iActiveWrapper->iStatus.Int() );

    if ( iActiveWrapper->iStatus.Int() )
        {
        if ( KErrNotFound == iActiveWrapper->iStatus.Int() )
            {
            iConnectionStatus = EConnectionClosed;
            }
        HBufC* temp = StringLoader::LoadL( R_QTN_CMON_PARAMETER_UNAVAILABLE );
        iWlanNetworkName = *temp;  
        delete temp;
        }
    
    CMUILOGGER_LEAVEFN( "void CWlanConnectionInfo::RefreshNetworkNameL()" );
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::RefreshSignalStrength
// ---------------------------------------------------------
//
void CWlanConnectionInfo::RefreshSignalStrength()
    {
    CMUILOGGER_ENTERFN( "void CWlanConnectionInfo::RefreshSignalStrength()" );
    
    iActiveWrapper->StartGetIntAttribute( iConnectionId,
                                          iConnectionMonitor,
                                          KSignalStrength,
                                          ( TInt& )iSignalStrength );

    CMUILOGGER_WRITE_F( "RefreshSignalStrengthL status : %d",
                        iActiveWrapper->iStatus.Int() );
    CMUILOGGER_WRITE_F( "iSignalStrength : %d", ( TInt )iSignalStrength );

    if ( iActiveWrapper->iStatus.Int() )
        {
        if ( KErrNotFound == iActiveWrapper->iStatus.Int() )
            {
            iConnectionStatus = EConnectionClosed;
            }
        iSignalStrength = EWlanSignalUnavailable;   
        }
            
    CMUILOGGER_LEAVEFN( "void CWlanConnectionInfo::RefreshSignalStrength()" );
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::RefreshSecurityMode
// ---------------------------------------------------------
//
void CWlanConnectionInfo::RefreshSecurityMode()
    {
    CMUILOGGER_ENTERFN( "void CWlanConnectionInfo::RefreshSecurityMode()" ); 
    
    iActiveWrapper->StartGetIntAttribute( iConnectionId,
                                          iConnectionMonitor,
                                          KSecurityMode,
                                          ( TInt& )iSecurityMode );

    CMUILOGGER_WRITE_F( "RefreshSecurityMode status: %d",
                        iActiveWrapper->iStatus.Int() );
    
    if ( iActiveWrapper->iStatus.Int() )
        {
        if ( KErrNotFound == iActiveWrapper->iStatus.Int() )
            {
            iConnectionStatus = EConnectionClosed;
            }
        iSecurityMode = EWlanSecurityModeUnavailable;    
        }
    
    CMUILOGGER_LEAVEFN( "void CWlanConnectionInfo::RefreshSecurityMode()" );
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::RefreshNetworkMode
// ---------------------------------------------------------
//
void CWlanConnectionInfo::RefreshNetworkMode()
    {
    CMUILOGGER_ENTERFN( "void CWlanConnectionInfo::RefreshNetworkMode()" ); 
    
    iActiveWrapper->StartGetIntAttribute( iConnectionId,
                                          iConnectionMonitor,
                                          KNetworkMode,
                                          ( TInt& )iNetworkMode );

    CMUILOGGER_WRITE_F( "RefreshNetworkMode status : %d",
                        iActiveWrapper->iStatus.Int() );
    
    if ( iActiveWrapper->iStatus.Int() )
        {
        if ( KErrNotFound == iActiveWrapper->iStatus.Int() )
            {
            iConnectionStatus = EConnectionClosed;
            }
        iNetworkMode = EWlanNetworkModeUnavailable;   
        }
        
    CMUILOGGER_LEAVEFN( "void CWlanConnectionInfo::RefreshNetworkMode()" );
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::RefreshTxPowerL
// ---------------------------------------------------------
//
void CWlanConnectionInfo::RefreshTxPowerL()
    {
    CMUILOGGER_ENTERFN( "void CWlanConnectionInfo::RefreshTxPowerL()" ); 
    
    iActiveWrapper->StartGetUintAttribute( iConnectionId,
                                           iConnectionMonitor,
                                           KTransmitPower,
                                           iTxPower );

    if ( iActiveWrapper->iStatus.Int() )
        {
        if ( KErrNotFound == iActiveWrapper->iStatus.Int() )
            {
            iConnectionStatus = EConnectionClosed;
            }
        iSignalStrength = EWlanSignalUnavailable;   
        }
            
    CMUILOGGER_WRITE_F( "iTxPower : %d", iTxPower );
    TInt err( iActiveWrapper->iStatus.Int() );
    CMUILOGGER_WRITE_F( "RefreshTxPowerL status: %d", err );
            
    CMUILOGGER_LEAVEFN( "void CWlanConnectionInfo::RefreshTxPowerL()" );
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::ToStringSignalStrengthLC
// ---------------------------------------------------------
//
HBufC* CWlanConnectionInfo::ToStringSignalStrengthLC() const
    {
    CMUILOGGER_ENTERFN( "CWlanConnectionInfo::ToStringSignalStrengthLC()" );
    
    HBufC* signalStrength = NULL;
    TInt resourceId( 0 );
        
    if ( iSignalStrength == EWlanSignalUnavailable )
        {
        resourceId =  R_QTN_CMON_WLAN_SIGNAL_STRENGTH_NO_SIGNAL;   
        signalStrength = StringLoader::LoadLC( resourceId );
        }                    
    else if ( iSignalStrength < EWlanSignalStrengthGood )
        {
        resourceId =  R_QTN_CMON_WLAN_SINGAL_STRENGTH_GOOD;
        }
    else if ( iSignalStrength > EWlanSignalStrengthLow )
        {
        resourceId =  R_QTN_CMON_WLAN_SINGAL_STRENGTH_LOW;
        }
    else 
        {
        resourceId = R_QTN_CMON_WLAN_SINGAL_STRENGTH_MEDIUM;
        }
        
    TInt signalInPercent( 0 );
    if ( iSignalStrength <= EWlanSignalStrengthMax )
        {
        signalInPercent = 100;
        }
    else if ( iSignalStrength >= EWlanSignalStrengthMin )
        {
        signalInPercent = 0;        
        }
    else
        {
        TInt diff = EWlanSignalStrengthMin - iSignalStrength;
        signalInPercent = diff * 100 / KDiffOfWlanSignalStrength;        
        }        
    
    if ( !signalStrength ) // there is connection to network
        {
        CMUILOGGER_WRITE_F( "signalInPercent : %d", signalInPercent );
        
        signalStrength = StringLoader::LoadLC( resourceId, 
                                               signalInPercent );
        }        
            
    CMUILOGGER_LEAVEFN( "CWlanConnectionInfo::ToStringSignalStrengthLC()" );
            
    return signalStrength;    
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::ToStringNetworkModeLC
// ---------------------------------------------------------
//
HBufC* CWlanConnectionInfo::ToStringNetworkModeLC() const
    {
    CMUILOGGER_ENTERFN( "CWlanConnectionInfo::ToStringNetworkModeLC()" ); 
    
    HBufC* networkMode = NULL;
    TInt resourceId( 0 );

    switch ( iNetworkMode )
        {
        case EWlanNetworkModeInfrastructure:
        case EWlanNetworkModeSecInfrastructure:        
            {
            resourceId = R_QTN_CMON_WLAN_NEWTWORK_MODE_INFRA;    
            break;
            }
        case EWlanNetworkModeAdHoc:
            {
            resourceId = R_QTN_CMON_WLAN_NEWTWORK_MODE_ADHOC;
            break;
            }            
        case EWlanNetworkModeUnavailable:
        default:
            {
            resourceId = R_QTN_CMON_PARAMETER_UNAVAILABLE;    
            break;                
            }
        }
    
    networkMode = StringLoader::LoadLC( resourceId );        
    
    CMUILOGGER_LEAVEFN( "CWlanConnectionInfo::ToStringNetworkModeLC()" );
        
    return networkMode;
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::ToStringSecurityModeLC
// ---------------------------------------------------------
//
HBufC* CWlanConnectionInfo::ToStringSecurityModeLC() const
    {
    CMUILOGGER_ENTERFN( "CWlanConnectionInfo::ToStringSecurityModeLC()" );
    
    HBufC* securityMode = NULL;
    TInt resourceId( 0 );

    switch ( iSecurityMode )
        {
        case EWlanSecurityModeOpenNetwork:
            {
            resourceId = R_QTN_CMON_WLAN_SECURITY_MODE_OPEN;
            break;
            }
        case EWlanSecurityModeWEP:
            {
            resourceId = R_QTN_CMON_WLAN_SECURITY_MODE_WEP;
            break;
            }
        case EWlanSecurityMode8021x:
            {
            resourceId = R_QTN_CMON_WLAN_SECURITY_MODE_802_1X;
            break;
            }
        case EWlanSecurityModeWPA:
            {
            resourceId = R_QTN_CMON_WLAN_SECURITY_MODE_WPA;
            break;
            }
        case EWlanSecurityWpaPsk:
            {
            resourceId = R_QTN_CMON_WLAN_SECURITY_MODE_WPA;
            break;
            }
        case EWlanSecurityModeUnavailable:            
        default:
            {
            resourceId = R_QTN_CMON_PARAMETER_UNAVAILABLE;    
            break;
            }
        }
    
    securityMode = StringLoader::LoadLC( resourceId );    
    
    CMUILOGGER_LEAVEFN( "CWlanConnectionInfo::ToStringSecurityModeLC()" );
            
    return securityMode;    
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::ToStringTxPowerLC
// ---------------------------------------------------------
//
HBufC* CWlanConnectionInfo::ToStringTxPowerLC() const
    {
    CMUILOGGER_ENTERFN( "CWlanConnectionInfo::ToStringTxPowerLC()" ); 
    
    HBufC* txPower = StringLoader::LoadLC( R_QTN_CMON_WLAN_TX_POWER_LEVEL,
                                           iTxPower );
    
    CMUILOGGER_LEAVEFN( "CWlanConnectionInfo::ToStringTxPowerLC()" );
                                               
    return txPower;    
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::ToArrayDetailsL
// ---------------------------------------------------------
//
void CWlanConnectionInfo::ToArrayDetailsL()
    {
    CMUILOGGER_ENTERFN( "void CWlanConnectionInfo::ToArrayDetailsL()" ); 
    
    HBufC* temp = NULL;
    delete iDetailsArray;
    iDetailsArray = NULL;
    iDetailsArray = new ( ELeave ) CDesCArrayFlat
                                    ( KDetailsArrayMaxGranularityCCX );
    
    temp = iConnectionName.AllocLC();
 
    ToDetailsListBoxItemTextWithAppendL
        (
        R_QTN_CMON_HEADING_CONN_NAME,
        temp );    
  
    ToDetailsListBoxItemTextWithAppendL
        (
        R_QTN_CMON_HEADING_BEARER,
        ToStringBearerLC()
        );

    ToDetailsListBoxItemTextWithAppendL
        (
        R_QTN_CMON_HEADING_STATUS,
        ToStringStatusLC()
        );
    
    ToDetailsListBoxItemTextWithAppendL
    (
    R_QTN_CMON_HEADING_WLAN_NETWORK_NAME,
    GetWlanNetworkNameLC()
    );

    ToDetailsListBoxItemTextWithAppendL
        (
        R_QTN_CMON_HEADING_WLAN_SIGNAL_STRENGTH,
        ToStringSignalStrengthLC()
        );

    ToDetailsListBoxItemTextWithAppendL
        (
        R_QTN_CMON_HEADING_WLAN_NETWORK_MODE,
        ToStringNetworkModeLC()
        );

    ToDetailsListBoxItemTextWithAppendL
        (
        R_QTN_CMON_HEADING_WLAN_SECURITY_MODE,
        ToStringSecurityModeLC()
        );
        
    ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_WLAN_TX_POWER,
                                         ToStringTxPowerLC() );
        
    ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_DATA_TOTAL,
                                         ToStringTotalTransmittedDataLC() );
                                         
    ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_DATA_RECEIVED,
                                         ToStringReceivedDataLC() );

    ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_DATA_SENT,
                                         ToStringSentDataLC() );
    
    ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_DURATION,
                                         ToStringDurationLC() );
        
    ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_SPEED,
                                         ToStringTransferSpeedDownLC() );

    ToDetailsListBoxItemTextWithAppendL( 0, 
                                         ToStringTransferSpeedUpLC() );
                
                                         
    CleanupStack::PopAndDestroy
        (
        KDetailsArrayMaxGranularityCCX - 1, 
        temp );
                                 
    TInt sharing = RefreshAppNamesL();

    if ( sharing > 1 )
        {
        for ( TInt i = 0; i < sharing; i++ )
            {
            if ( i == 0 )
                {
                ToDetailsListBoxItemTextWithAppendL( 
                                    R_QTN_CMON_HEADING_SHARING_INFO,
                                    ToStringAppNameLC( i ) );
                }
            else
                {
                ToDetailsListBoxItemTextWithAppendL( 0, 
                                                ToStringAppNameLC( i ) );
                }
            }
        CleanupStack::PopAndDestroy( sharing ); // ToStringAppNameLC()
        }
    
    CMUILOGGER_LEAVEFN( "void CWlanConnectionInfo::ToArrayDetailsL()" );
    }

// ---------------------------------------------------------
// CWlanConnectionInfo::RefreshDetailsArrayL
// ---------------------------------------------------------
//
void CWlanConnectionInfo::RefreshDetailsArrayL()
    {
    CMUILOGGER_ENTERFN( "void CWlanConnectionInfo::RefreshDetailsArrayL()" );

    HBufC* temp = NULL;

    temp = ToStringStatusLC();

    ToDetailsListBoxItemTextWithReplaceL
        (
        R_QTN_CMON_HEADING_STATUS,
        temp,
        2
        );

    ToDetailsListBoxItemTextWithReplaceL
        (
        R_QTN_CMON_HEADING_WLAN_SIGNAL_STRENGTH,
        ToStringSignalStrengthLC(),
        4
        );
                                          
    ToDetailsListBoxItemTextWithReplaceL
        (
        R_QTN_CMON_HEADING_WLAN_TX_POWER,
        ToStringTxPowerLC(),
        7
        );
        
    ToDetailsListBoxItemTextWithReplaceL( R_QTN_CMON_HEADING_DATA_TOTAL,
                                          ToStringTotalTransmittedDataLC(),
                                          8 );
        

    ToDetailsListBoxItemTextWithReplaceL( R_QTN_CMON_HEADING_DATA_RECEIVED,
                                          ToStringReceivedDataLC(), 
                                          9 );

    ToDetailsListBoxItemTextWithReplaceL( R_QTN_CMON_HEADING_DATA_SENT,
                                          ToStringSentDataLC(), 
                                          10 );
    
    ToDetailsListBoxItemTextWithReplaceL( R_QTN_CMON_HEADING_DURATION,
                                          ToStringDurationLC(), 
                                          11 );
        
    ToDetailsListBoxItemTextWithReplaceL( R_QTN_CMON_HEADING_SPEED,
                                          ToStringTransferSpeedDownLC(), 
                                          12 );

    ToDetailsListBoxItemTextWithReplaceL( 0, 
                                          ToStringTransferSpeedUpLC(), 
                                          13 );
                                          
    CleanupStack::PopAndDestroy( 9, temp );
    

    TInt count = iDetailsArray->Count();
    TInt realElementsMaxCount = KDetailsArrayMaxGranularityCCX;
    
    CMUILOGGER_WRITE_F( "count : %d", count );
    CMUILOGGER_WRITE_F( "realElementsMaxCount : %d", realElementsMaxCount );
    
    TInt sharing = RefreshAppNamesL();

    if ( sharing > 1 )
        {
        if ( count >= realElementsMaxCount ) // 15
            {  
            iDetailsArray->Delete( realElementsMaxCount - 1,
                                   iDetailsArray->Count() - 14 );
            iDetailsArray->Compress(); 
            }
        
        for ( TInt i = 0; i < sharing; i++ )
            {
            if ( i == 0 )
                {
                ToDetailsListBoxItemTextWithAppendL( 
                                        R_QTN_CMON_HEADING_SHARING_INFO,
                                        ToStringAppNameLC( i ) );
                }
            else
                {
                ToDetailsListBoxItemTextWithAppendL( 0, 
                                        ToStringAppNameLC( i ) );
                }
            }
        CleanupStack::PopAndDestroy( sharing ); // ToStringAppNameLC()
        }
    else
        {
        if ( count >= realElementsMaxCount )
            {
            iDetailsArray->Delete( realElementsMaxCount - 1,
                                   iDetailsArray->Count() - 14 );
            iDetailsArray->Compress();              
            }
        }               
    
    CMUILOGGER_LEAVEFN( "void CWlanConnectionInfo::RefreshDetailsArrayL()" );
    }


// ---------------------------------------------------------
// CWlanConnectionInfo::RefreshConnectionListBoxItemTextL
// ---------------------------------------------------------
//
void CWlanConnectionInfo::RefreshConnectionListBoxItemTextL()
    {
    CMUILOGGER_ENTERFN( 
    "void CWlanConnectionInfo::RefreshConnectionListBoxItemTextL()" ); 
    
    TUint  iconId;
    HBufC* secondRow = NULL;
    
    iconId = GetIconId();

    if ( iConnectionStatus == EConnectionCreated )
        {
        secondRow =  ToStringDurationLC();          
        }
    else // if connection has not created yet gives back the status of itself.
        {
        secondRow = ToStringStatusLC( EFalse );
        }

    iListBoxItemText.Format( KConnectonListItemTextFormat, iconId, 
                             &iConnectionName, secondRow );
    CleanupStack::PopAndDestroy( secondRow );
    
    CMUILOGGER_LEAVEFN( 
    "void CWlanConnectionInfo::RefreshConnectionListBoxItemTextL()" );    
    }



// End of File
