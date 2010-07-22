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
* Description:  Represents a connection
*
*
*/


// INCLUDE FILES
//#include <nifvar.h>
#include <StringLoader.h>
#include <connectionmonitorui.rsg>
#include <ConnectionMonitorUi.mbg>
#include <AknUtils.h>

#include "ConnectionMonitorUiLogger.h"
#include "CsdConnectionInfo.h"
#include "ActiveWrapper.h"

// CONSTANTS

LOCAL_D const TUint KDetailsArrayGranularityInternal = 9;
LOCAL_D const TUint KDetailsArrayGranularityExternal = 4;

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CCsdConnectionInfo::NewL
// ---------------------------------------------------------
//
CCsdConnectionInfo* CCsdConnectionInfo::NewL( TInt aConnectionId,
                              RConnectionMonitor* const aConnectionMonitor,
                              TConnMonBearerType aConnectionBearerType,
                              CActiveWrapper* aActiveWrapper )
    {
    CMUILOGGER_WRITE( "NewL, CCsdsConnectionInfo - start " );

    CCsdConnectionInfo* self = new ( ELeave ) CCsdConnectionInfo(
                                                    aConnectionId,
                                                    aConnectionMonitor,
                                                    aConnectionBearerType,
                                                    aActiveWrapper );
    CMUILOGGER_WRITE( "NewL, CCsdsConnectionInfo - created " );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    CMUILOGGER_WRITE( "NewL, CCsdsConnectionInfo - end " );
    return self;
    }


// ---------------------------------------------------------
// CCsdConnectionInfo::~CCsdConnectionInfo
// ---------------------------------------------------------
//
CCsdConnectionInfo::~CCsdConnectionInfo()
    {
    }


// ---------------------------------------------------------
// CCsdConnectionInfo::CCsdConnectionInfo
// ---------------------------------------------------------
//
CCsdConnectionInfo::CCsdConnectionInfo(  TInt aConnectionId,
                             RConnectionMonitor* const aConnectionMonitor,
                             TConnMonBearerType aConnectionBearerType,
                             CActiveWrapper* aActiveWrapper ) :
    CConnectionInfoBase( aConnectionId,
                         aConnectionMonitor,
                         aConnectionBearerType,
                         aActiveWrapper )
        {
        CMUILOGGER_WRITE( "CCsdConnectionInfo constuctor" );
        }


// ---------------------------------------------------------
// CCsdConnectionInfo::ConstructL
// ---------------------------------------------------------
//
void CCsdConnectionInfo::ConstructL()
    {
    CMUILOGGER_WRITE( "CCsdConnectionInfo::ConstructL() start" );
    
    iActiveWrapper->StartGetStringAttribute( iConnectionId,
                                             iConnectionMonitor,
                                             KTelNumber,
                                             iDialupNumber );
    TInt err( iActiveWrapper->iStatus.Int() );
    CMUILOGGER_WRITE_F( "CCsdConnectionInfo::ConstructL() status: %d",
                        err );
    if ( err != KErrNone )
        {
        User::Leave( err );
        }
        
    CConnectionInfoBase::ConstructL();
    CMUILOGGER_WRITE( "CCsdConnectionInfo::ConstructL() end" );
    }


// ---------------------------------------------------------
// CCsdConnectionInfo::DeepCopyL
// ---------------------------------------------------------
//        
CConnectionInfoBase* CCsdConnectionInfo::DeepCopyL()
	{
	CCsdConnectionInfo* temp = new ( ELeave ) CCsdConnectionInfo(
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
	
	temp->iDialupNumber = iDialupNumber;
		
	if ( iDetailsArray )
		{
		CleanupStack::PushL( temp );		
		temp->ToArrayDetailsL();
		CleanupStack::Pop( temp );			
		}
		
	return temp;	
	}


// ---------------------------------------------------------
// CCsdConnectionInfo::GetIconId
// ---------------------------------------------------------
//
TUint CCsdConnectionInfo::GetIconId() const
    {
    TUint iconId = ECsd;
    if ( ( iConnectionBearerType == EBearerHSCSD ) || 
         ( iConnectionBearerType == EBearerExternalHSCSD ) )
        {
        iconId = EHscsd;
        }
    return iconId;
    }

// ---------------------------------------------------------
// CCsdConnectionInfo::RefreshDetailsL
// ---------------------------------------------------------
//
void CCsdConnectionInfo::RefreshDetailsL()
    {
    CMUILOGGER_WRITE( "CCsdConnectionInfo::RefreshDetailsL start" );
    
    CMUILOGGER_WRITE_F( "before iConnectionBearerType: %d", 
                        ( TInt )iConnectionBearerType );
    
    TConnMonBearerType bearerType;
    
    iActiveWrapper->StartGetIntAttribute( iConnectionId,
                                          iConnectionMonitor,
                                          KBearer,
                                          ( TInt& )bearerType );
        
    if ( !iActiveWrapper->iStatus.Int() )
        {
        iConnectionBearerType = bearerType;
        }
    
    CMUILOGGER_WRITE_F( "status.Int(): %d", iActiveWrapper->iStatus.Int() );
    CMUILOGGER_WRITE_F( "bearerType: %d", ( TInt )bearerType );    
    CMUILOGGER_WRITE_F( "after iConnectionBearerType: %d", ( TInt )iConnectionBearerType );
    
    if ( iConnectionBearerType < EBearerExternalCSD )
        {
        RefreshTransferSpeedsL();
        }
    RefreshDuration();
    CMUILOGGER_WRITE( "CCsdConnectionInfo::RefreshDetailsL end" );
    }

// ---------------------------------------------------------
// CCsdConnectionInfo::ToStringDialupNumberLC
// ---------------------------------------------------------
//
HBufC* CCsdConnectionInfo::ToStringDialupNumberLC() const
    {
    HBufC* dialupNumber = iDialupNumber.AllocLC();
    TPtr temp( dialupNumber->Des() );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( temp );
    
    return dialupNumber;
    }


// ---------------------------------------------------------
// CCsdConnectionInfo::RefreshConnectionListBoxItemTextL
// ---------------------------------------------------------
//
void CCsdConnectionInfo::RefreshConnectionListBoxItemTextL()
    {
    CMUILOGGER_WRITE(
        "CCsdConnectionInfo::RefreshConnectionListBoxItemTextL start" );

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
                             &iConnectionName, secondRow );
    CleanupStack::PopAndDestroy( secondRow );
    CMUILOGGER_WRITE(
        "CCsdConnectionInfo::RefreshConnectionListBoxItemTextL end" );
    }


// ---------------------------------------------------------
// CCsdConnectionInfo::RefreshDetailsArrayL
// ---------------------------------------------------------
//
void CCsdConnectionInfo::RefreshDetailsArrayL()
    {
    CMUILOGGER_WRITE( "CCsdConnectionInfo::RefreshDetailsArrayL start" );
    if ( iDetailsArray )
        {
        TBool internal = iConnectionBearerType < EBearerExternalCSD ;
        HBufC* temp = NULL;

        if ( internal )
            {
            temp = ToStringStatusLC();
            ToDetailsListBoxItemTextWithReplaceL( R_QTN_CMON_HEADING_STATUS,
                                                  temp, 2 );
                                                  
            ToDetailsListBoxItemTextWithReplaceL
                ( 
                R_QTN_CMON_HEADING_DATA_TOTAL,
                ToStringTotalTransmittedDataLC(), 
                3   
                );                                                  

            ToDetailsListBoxItemTextWithReplaceL
                (
                R_QTN_CMON_HEADING_DATA_RECEIVED,
                ToStringReceivedDataLC(),
                4
                );

            ToDetailsListBoxItemTextWithReplaceL
                (
                R_QTN_CMON_HEADING_DATA_SENT,
                ToStringSentDataLC(),
                5
                );

            ToDetailsListBoxItemTextWithReplaceL( R_QTN_CMON_HEADING_DURATION,
                                                  ToStringDurationLC(), 6 );

            ToDetailsListBoxItemTextWithReplaceL
                (
                R_QTN_CMON_HEADING_SPEED,
                ToStringTransferSpeedDownLC(),
                7
                );

            ToDetailsListBoxItemTextWithReplaceL
                (
                0, 
                ToStringTransferSpeedUpLC(),
                8
                );

            CleanupStack::PopAndDestroy( 7, temp );   // ToString...LC()

            TInt sharing = RefreshAppNamesL();

            if ( sharing > 1 )
                {
                if ( iDetailsArray->Count() > 10 )
                    {
                    iDetailsArray->Delete( 10, 
                                           iDetailsArray->Count() - 10 );
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
                CleanupStack::PopAndDestroy( sharing ); 
                                                    // ToStringAppNameLC()
                }
            else
                {
                if ( iDetailsArray->Count() > 10 )
                    {
                    iDetailsArray->Delete( 10, iDetailsArray->Count() - 10 );
                    iDetailsArray->Compress();
                    }
                }
            }
        else
            {
            ToDetailsListBoxItemTextWithReplaceL( R_QTN_CMON_HEADING_DURATION,
                                                  ToStringDurationLC(), 2 );
            CleanupStack::PopAndDestroy( 1 );   // ToStringDurationLC()
            }
        }
    CMUILOGGER_WRITE( "CCsdConnectionInfo::RefreshDetailsArrayL end" );
    }


// ---------------------------------------------------------
// CCsdConnectionInfo::ToArrayDetailsL
// ---------------------------------------------------------
//
void CCsdConnectionInfo::ToArrayDetailsL()
    {
    CMUILOGGER_ENTERFN("CCsdConnectionInfo::ToArrayDetailsL");
    TBool internal = iConnectionBearerType < EBearerExternalCSD ;
    TUint granularity = KDetailsArrayGranularityExternal;
    HBufC* temp = NULL;

    if ( internal )
        {
        granularity = KDetailsArrayGranularityInternal;
        }
    delete iDetailsArray;
    iDetailsArray = NULL;
    iDetailsArray = new ( ELeave ) CDesCArrayFlat( granularity );

    temp = iConnectionName.AllocLC();
    ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_CONN_NAME,
                                         temp );

    ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_BEARER,
                                         ToStringBearerLC() );
    CleanupStack::PopAndDestroy( 2, temp );   // ToString...LC()


    if ( internal )
        {
        temp = ToStringStatusLC();
        ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_STATUS,
                                             temp );

        ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_DATA_TOTAL,
                                             ToStringTotalTransmittedDataLC() );
                                             
        ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_DATA_RECEIVED,
                                             ToStringReceivedDataLC() );

        ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_DATA_SENT,
                                             ToStringSentDataLC() );
        CleanupStack::PopAndDestroy( 4, temp );   // ToString...LC()
        }

    ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_DURATION,
                                         ToStringDurationLC() );
    CleanupStack::PopAndDestroy( 1 );   // ToStringDurationLC()

    if ( internal )
        {
        temp = ToStringTransferSpeedDownLC();

        ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_SPEED,
                                             temp );

        ToDetailsListBoxItemTextWithAppendL( 0, 
                                             ToStringTransferSpeedUpLC() );
        CleanupStack::PopAndDestroy( 2, temp );   // ToString...LC()
        }

    ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_CSD_NUMBER,
                                         ToStringDialupNumberLC() );
    CleanupStack::PopAndDestroy( 1 );   // ToStringDialupNumberLC()

    if ( internal )
        {
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
            CleanupStack::PopAndDestroy( sharing ); 
                                                    // ToStringAppNameLC()
            }
        }
    CMUILOGGER_LEAVEFN("CCsdConnectionInfo::ToArrayDetailsL");
    }



// End of File
