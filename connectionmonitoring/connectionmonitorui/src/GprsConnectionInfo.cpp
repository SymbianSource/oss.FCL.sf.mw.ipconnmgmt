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
* Description:  Represents a GPRS connection
*     
*
*/


// INCLUDE FILES
//#include <nifvar.h>
#include <StringLoader.h>
#include <ConnectionMonitorUi.rsg>
#include <ConnectionMonitorUi.mbg>
//#include <Avkon.rsg>

#include "ConnectionMonitorUiLogger.h"
#include "GprsConnectionInfo.h"
#include "ActiveWrapper.h"

// CONSTANTS
LOCAL_D const TUint KDetailsArrayGranularityInternal = 11;
LOCAL_D const TUint KDetailsArrayGranularityExternal = 10;


// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CGprsConnectionInfo::NewL
// ---------------------------------------------------------
//
CGprsConnectionInfo* CGprsConnectionInfo::NewL( TInt aConnectionId, 
                              RConnectionMonitor* const aConnectionMonitor,
                              TConnMonBearerType aConnectionBearerType,
                              CActiveWrapper* aActiveWrapper )
    {
    CGprsConnectionInfo* self = new ( ELeave ) CGprsConnectionInfo( 
                                                    aConnectionId, 
                                                    aConnectionMonitor, 
                                                    aConnectionBearerType,
                                                    aActiveWrapper );
    CleanupStack::PushL( self );
    self->ConstructL(); // Have to call this ConstructL at first,
    CleanupStack::Pop( self );
    CMUILOGGER_WRITE( "NewL, CGprsConnectionInfo" );
    return self;
    }


// ---------------------------------------------------------
// CGprsConnectionInfo::~CGprsConnectionInfo
// ---------------------------------------------------------
//
CGprsConnectionInfo::~CGprsConnectionInfo()
    {
    }


// ---------------------------------------------------------
// CGprsConnectionInfo::CGprsConnectionInfo
// ---------------------------------------------------------
//
CGprsConnectionInfo::CGprsConnectionInfo(  TInt aConnectionId, 
                             RConnectionMonitor* const aConnectionMonitor,
                             TConnMonBearerType aConnectionBearerType,
                             CActiveWrapper* aActiveWrapper ) :
    CConnectionInfoBase( aConnectionId, 
                         aConnectionMonitor, 
                         aConnectionBearerType,
                         aActiveWrapper )
        {
        CMUILOGGER_WRITE( "CGprsConnectionInfo constuctor" );
        }


// ---------------------------------------------------------
// CGprsConnectionInfo::ConstructL
// ---------------------------------------------------------
//
void CGprsConnectionInfo::ConstructL()
    {
    CMUILOGGER_ENTERFN( "CGprsConnectionInfo::ConstructL() start" );
    
    iActiveWrapper->StartGetStringAttribute( iConnectionId,
                                             iConnectionMonitor,
                                             KAccessPointName,
                                             iApName );
#ifdef  __WINS__
    iApName = _L( "LAN" );
#else
    TInt err( iActiveWrapper->iStatus.Int() );
    CMUILOGGER_WRITE_F( "CGprsConnectionInfo::ConstructL status: %d", err );
    if ( err != KErrNone )
        {
        User::Leave( err );
        }
#endif
    CMUILOGGER_WRITE_F( "iApName Connection name: %S", &iApName );
    
    CConnectionInfoBase::ConstructL(); // and base class ConstructL next.
    CMUILOGGER_WRITE( "CGprsConnectionInfo::ConstructL() end" );
    }
    
// ---------------------------------------------------------
// CGprsConnectionInfo::DeepCopyL
// ---------------------------------------------------------
//        
CConnectionInfoBase* CGprsConnectionInfo::DeepCopyL()
	{
	CGprsConnectionInfo* temp = new ( ELeave ) CGprsConnectionInfo(
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
	
	temp->iApName = iApName;
	
	if ( iDetailsArray )
		{
		CleanupStack::PushL( temp );		
		temp->ToArrayDetailsL();
		CleanupStack::Pop( temp );			
		}

	return temp;	
	}
    

// ---------------------------------------------------------
// CGprsConnectionInfo::GetIconId
// ---------------------------------------------------------
//
TUint CGprsConnectionInfo::GetIconId() const
    {
    TUint iconId = EGprs;
    if ( iConnectionStatus == EConnectionSuspended )
	    {
	    iconId = ESuspended;
	    }
    return iconId;
    }

// ---------------------------------------------------------
// CGprsConnectionInfo::RefreshDetailsL
// ---------------------------------------------------------
//
void CGprsConnectionInfo::RefreshDetailsL()
    {
    CMUILOGGER_WRITE( "CGprsConnectionInfo::RefreshDetailsL start" );

    iActiveWrapper->StartGetStringAttribute( iConnectionId,
                                             iConnectionMonitor,
                                             KAccessPointName,
                                             iApName );
#ifdef  __WINS__
    iActiveWrapper->Cancel();
    iApName = _L( "LAN" );
#else
    TInt err( iActiveWrapper->iStatus.Int() );
    CMUILOGGER_WRITE_F( "CGprsConnectionInfo::RefreshDetailsL status: %d",
                        err );
    if ( err != KErrNone )
        {
        User::Leave( err );
        }
#endif
    CMUILOGGER_WRITE_F( "iApName Connection name: %S", &iApName );
          
    RefreshTransferSpeedsL();
    RefreshDuration();
    CMUILOGGER_WRITE( "CGprsConnectionInfo::RefreshDetailsL end" );
    }

// ---------------------------------------------------------
// CGprsConnectionInfo::ToStringApNameLC
// ---------------------------------------------------------
//
HBufC* CGprsConnectionInfo::ToStringApNameLC() const
    {
    HBufC* apName = iApName.AllocLC();
    return apName;    
    }


// ---------------------------------------------------------
// CGprsConnectionInfo::RefreshConnectionListBoxItemTextL
// ---------------------------------------------------------
//
void CGprsConnectionInfo::RefreshConnectionListBoxItemTextL()
    {
    CMUILOGGER_WRITE( 
        "CGprsConnectionInfo::RefreshConnectionListBoxItemTextL start" );

    TUint  iconId;
    HBufC* secondRow = NULL;
    
    iconId = GetIconId();

    if ( iConnectionStatus == EConnectionCreated )
        {
        CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(2);
        CleanupStack::PushL( array );
      
        HBufC* stringReceivedData = ToStringReceivedDataLC( ETrue );
        HBufC* stringSentData = ToStringSentDataLC( ETrue );

        StringMarkerRemoval( stringReceivedData );
        StringMarkerRemoval( stringSentData );
        
        array->AppendL( *stringReceivedData );
        array->AppendL( *stringSentData );

        secondRow = StringLoader::LoadL( R_QTN_CMON_UL_DL_DATA_AMOUNTS, 
                                         *array );

        CleanupStack::PopAndDestroy( stringSentData );
        CleanupStack::PopAndDestroy( stringReceivedData );
        CleanupStack::PopAndDestroy( array );
        }
    else // if connection has not created yet gives back the status of its.
        {
        secondRow = ToStringStatusLC( EFalse );
        CleanupStack::Pop( secondRow ); 
        }

    iListBoxItemText.Format( KConnectonListItemTextFormat, iconId, 
                             &iConnectionName, secondRow );
    delete secondRow;
    CMUILOGGER_WRITE( 
        "CGprsConnectionInfo::RefreshConnectionListBoxItemTextL end" );
    }

// ---------------------------------------------------------
// CGprsConnectionInfo::RefreshDetailsArrayL
// ---------------------------------------------------------
//
void CGprsConnectionInfo::RefreshDetailsArrayL()
    {
    CMUILOGGER_WRITE( "CGprsConnectionInfo::RefreshDetailsArrayL start" );
    if ( iDetailsArray )
        {
        TBool internal = iConnectionBearerType < EBearerExternalCSD ;   
        HBufC* temp = NULL;

        temp = ToStringStatusLC();
        ToDetailsListBoxItemTextWithReplaceL( R_QTN_CMON_HEADING_STATUS,
                                              temp, 2 );

        ToDetailsListBoxItemTextWithReplaceL( R_QTN_CMON_HEADING_DATA_TOTAL,
                                              ToStringTotalTransmittedDataLC(),
                                              3 );
        ToDetailsListBoxItemTextWithReplaceL
            (
            R_QTN_CMON_HEADING_DATA_RECEIVED,
            ToStringReceivedDataLC(),
            4
            );                                              

        ToDetailsListBoxItemTextWithReplaceL( R_QTN_CMON_HEADING_DATA_SENT,
                                              ToStringSentDataLC(), 5 );
    
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
    
        if ( internal )
            {
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
        }
    CMUILOGGER_WRITE( "CGprsConnectionInfo::RefreshDetailsArrayL end" );    
    }


// ---------------------------------------------------------
// CGprsConnectionInfo::ToArrayDetailsL
// ---------------------------------------------------------
//
void CGprsConnectionInfo::ToArrayDetailsL()
    {
    CMUILOGGER_ENTERFN("CGprsConnectionInfo::ToArrayDetailsL");
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

    ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_STATUS,
                                         ToStringStatusLC() );
    
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
    
    ToDetailsListBoxItemTextWithAppendL( R_QTN_CMON_HEADING_GPRS_APN,
                                         ToStringApNameLC() );
    CleanupStack::PopAndDestroy( 10, temp );   // ToString...LC()
    
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
    CMUILOGGER_LEAVEFN("CGprsConnectionInfo::ToArrayDetailsL");
    }


// End of File
