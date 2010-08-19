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
#include <ConnectionMonitorUi.rsg>
#include <ConnectionMonitorUi.mbg>

#include "EasyWLANConnectionInfo.h"
#include "ConnectionArray.h"
#include <StringLoader.h>

#include "ConnectionMonitorUiLogger.h"


// CONSTANTS
/**
* Count of details of WLAN connection
*/
LOCAL_D const TUint KMaxNumOfListBoxItems = 14;


// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CEasyWlanConnectionInfo::NewL
// ---------------------------------------------------------
//
CEasyWlanConnectionInfo* CEasyWlanConnectionInfo::NewL( TInt aConnectionId,
                              RConnectionMonitor* const aConnectionMonitor,
                              TConnMonBearerType aConnectionBearerType,
                              CActiveWrapper* aActiveWrapper )
    {
    CMUILOGGER_ENTERFN(
        "CEasyWlanConnectionInfo* CEasyWlanConnectionInfo::NewL" );
    CEasyWlanConnectionInfo* self = new ( ELeave ) CEasyWlanConnectionInfo(
                                                    aConnectionId,
                                                    aConnectionMonitor,
                                                    aConnectionBearerType,
                                                    aActiveWrapper );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    CMUILOGGER_LEAVEFN(
        "CEasyWlanConnectionInfo* CEasyWlanConnectionInfo::NewL" );
    return self;
    }

// ---------------------------------------------------------
// CEasyWlanConnectionInfo::CEasyWlanConnectionInfo
// ---------------------------------------------------------
//
CEasyWlanConnectionInfo::CEasyWlanConnectionInfo( TInt aConnectionId,
                              RConnectionMonitor* const aConnectionMonitor,
                              TConnMonBearerType aConnectionBearerType,
                              CActiveWrapper* aActiveWrapper )
    : CWlanConnectionInfo( aConnectionId,
                           aConnectionMonitor,
                           aConnectionBearerType,
                           aActiveWrapper )
    {
    iEasyWlan= ETrue;
    }

// ---------------------------------------------------------
// CEasyWlanConnectionInfo::ConstructL
// ---------------------------------------------------------
//
void CEasyWlanConnectionInfo::ConstructL()
    {
    CMUILOGGER_ENTERFN( "void CEasyWlanConnectionInfo::ConstructL()" );

    CWlanConnectionInfo::ConstructL();

    CMUILOGGER_LEAVEFN( "void CEasyWlanConnectionInfo::ConstructL()" );
    }


// ---------------------------------------------------------
// CEasyWlanConnectionInfo::~CEasyWlanConnectionInfo
// ---------------------------------------------------------
//
CEasyWlanConnectionInfo::~CEasyWlanConnectionInfo()
    {
    }

// ---------------------------------------------------------
// CEasyWlanConnectionInfo::DeepCopyL
// ---------------------------------------------------------
//        
CConnectionInfoBase* CEasyWlanConnectionInfo::DeepCopyL()
	{
	CEasyWlanConnectionInfo* temp = new ( ELeave ) CEasyWlanConnectionInfo(
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
// CEasyWlanConnectionInfo::ToArrayDetailsL
// ---------------------------------------------------------
//
void CEasyWlanConnectionInfo::ToArrayDetailsL()
    {
    CMUILOGGER_ENTERFN( "void CEasyWlanConnectionInfo::ToArrayDetailsL()" );

    HBufC* temp = NULL;
    delete iDetailsArray;
    iDetailsArray = NULL;

    iDetailsArray = new ( ELeave ) CDesCArrayFlat
        ( KMaxNumOfListBoxItems );

    temp = ToStringBearerLC();

    ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_BEARER,
                                         temp );

    ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_STATUS,
                                         ToStringStatusLC() );

    ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_WLAN_NETWORK_NAME,
                                         GetWlanNetworkNameLC() );

    ToDetailsListBoxItemTextWithAppendL
        (
        R_QTN_CMON_HEADING_WLAN_SIGNAL_STRENGTH,
        ToStringSignalStrengthLC()
        );

    ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_WLAN_NETWORK_MODE,
                                         ToStringNetworkModeLC() );

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
        KMaxNumOfListBoxItems - 1,
        temp
        );

    CMUILOGGER_WRITE_F( "KMaxNumOfListBoxItems + cCXSupported : %d",
                        KMaxNumOfListBoxItems );

    TInt sharing = RefreshAppNamesL();
        
    if ( sharing >= 1 )
        {
        for ( TInt i = 0; i < ( sharing ); i++ )
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
        
    CMUILOGGER_LEAVEFN( "void CEasyWlanConnectionInfo::ToArrayDetailsL()" );
    }

// ---------------------------------------------------------
// CEasyWlanConnectionInfo::RefreshDetailsArrayL
// ---------------------------------------------------------
//
void CEasyWlanConnectionInfo::RefreshDetailsArrayL()
    {
    CMUILOGGER_ENTERFN( "CEasyWlanConnectionInfo::RefreshDetailsArrayL()" );

    HBufC* temp = NULL;

    temp = ToStringStatusLC();

    ToDetailsListBoxItemTextWithReplaceL( R_QTN_CMON_HEADING_STATUS,
                                          temp, 1 );

    ToDetailsListBoxItemTextWithReplaceL
        (
        R_QTN_CMON_HEADING_WLAN_SIGNAL_STRENGTH,
        ToStringSignalStrengthLC(),
        3
        );

    ToDetailsListBoxItemTextWithReplaceL
        (
        R_QTN_CMON_HEADING_WLAN_TX_POWER,
        ToStringTxPowerLC(),
        6
        );
        
    ToDetailsListBoxItemTextWithReplaceL( R_QTN_CMON_HEADING_DATA_TOTAL,
                                          ToStringTotalTransmittedDataLC(),
                                          7 );

    ToDetailsListBoxItemTextWithReplaceL( R_QTN_CMON_HEADING_DATA_RECEIVED,
                                          ToStringReceivedDataLC(),
                                          8 );

    ToDetailsListBoxItemTextWithReplaceL( R_QTN_CMON_HEADING_DATA_SENT,
                                          ToStringSentDataLC(),
                                          9 );

    ToDetailsListBoxItemTextWithReplaceL( R_QTN_CMON_HEADING_DURATION,
                                          ToStringDurationLC(),
                                          10 );

    ToDetailsListBoxItemTextWithReplaceL( R_QTN_CMON_HEADING_SPEED,
                                          ToStringTransferSpeedDownLC(),
                                          11 );

    ToDetailsListBoxItemTextWithReplaceL( 0, 
                                          ToStringTransferSpeedUpLC(),
                                          12 );

    CleanupStack::PopAndDestroy( 9, temp );

    TInt count = iDetailsArray->Count();
    TInt realElementsMaxCount = KMaxNumOfListBoxItems;

    CMUILOGGER_WRITE_F( "count : %d", count );
    CMUILOGGER_WRITE_F( "realElementsMaxCount : %d", 
                        realElementsMaxCount );

    TInt sharing = RefreshAppNamesL();

    if ( sharing >= 1 )
        {
        if ( count >= realElementsMaxCount )
            {  
            iDetailsArray->Delete( realElementsMaxCount - 1,
                                       iDetailsArray->Count() - 13 );
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
                                   iDetailsArray->Count() - 13 );
            iDetailsArray->Compress();              
            }
        } 

    CMUILOGGER_LEAVEFN(
        "void CEasyWlanConnectionInfo::RefreshDetailsArrayL()" );
    }

// ---------------------------------------------------------
// CEasyWlanConnectionInfo::RefreshConnectionListBoxItemTextL
// ---------------------------------------------------------
//
void CEasyWlanConnectionInfo::RefreshConnectionListBoxItemTextL()
    {
    CMUILOGGER_ENTERFN(
    "void CEasyWlanConnectionInfo::RefreshConnectionListBoxItemTextL()" );

    TUint  iconId;
    HBufC* secondRow = NULL;

    iconId = GetIconId();

    if ( iConnectionStatus == EConnectionCreated )
        {
        secondRow =  ToStringDurationLC();
        }
    else    // if connection has not created yet gives back the status of its.
        {
        secondRow = ToStringStatusLC( EFalse );
        }

    iListBoxItemText.Format( KConnectonListItemTextFormat, iconId,
                             &iWlanNetworkName, secondRow );
    CleanupStack::PopAndDestroy( secondRow );

    CMUILOGGER_LEAVEFN(
    "void CEasyWlanConnectionInfo::RefreshConnectionListBoxItemTextL()" );
    }



// End of File
