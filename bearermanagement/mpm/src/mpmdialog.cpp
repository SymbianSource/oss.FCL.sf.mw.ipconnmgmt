/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: MPM user dialog handling
*
*/

/**
@file mpmdialog.cpp
Mobility Policy Manager user dialog initiation.
*/

// INCLUDE FILES
#include <connpref.h> // for TConnPref
#include <gsmerror.h>
#include <metadatabase.h>
#include <datamobilitycommsdattypes.h>
#include <commdb.h>
#include <bldvariant.hrh>   // For feature flags
#include <e32property.h>    //publish and subscribe

#include "mpmdialog.h"
#include "mpmlogger.h"
#include "mpmdefaultconnection.h"
#include "rmpm.h"
#include "mpmcommsdataccess.h"

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPMDialog::NewL
// -----------------------------------------------------------------------------
//
CMPMDialog* CMPMDialog::NewL( CMPMIapSelection&             aIapSelection,
                              const RAvailableIAPList&      aIAPList,
                              TUint32                       aBearerSet,
                              CArrayPtrFlat<CMPMDialog>&    aQueue,
                              CMPMServer&                   aServer )
    {
    CMPMDialog* self = new ( ELeave ) CMPMDialog( aIapSelection, 
                                                  aBearerSet,
                                                  aQueue,
                                                  aServer );
    CleanupStack::PushL( self );
    self->ConstructL( aIAPList );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CMPMDialog::CMPMDialog
// -----------------------------------------------------------------------------
//
CMPMDialog::CMPMDialog( CMPMIapSelection&           aIapSelection,
                        TUint32                     aBearerSet,
                        CArrayPtrFlat<CMPMDialog>&  aQueue,
                        CMPMServer&                 aServer )
    :   CMPMDialogBase( *(CArrayPtrFlat<CMPMDialogBase>*)&aQueue ),
        iIapSelection( aIapSelection ), 
        iSnapOrIAPId( 0 ),
        iBearerSet( aBearerSet ),
        iServer ( aServer )
    {
    }


// -----------------------------------------------------------------------------
// CMPMDialog::~CMPMDialog
// -----------------------------------------------------------------------------
//
CMPMDialog::~CMPMDialog()
    {
    Cancel();
    MPMLOGSTRING2( "CMPMDialog::~CMPMDialog: deleted, Completing with code = %i", 
                       iStatus.Int() )
    iDlgServ.Close();
    // Clean up all other stuff
    iIAPList.Close();
    }


// -----------------------------------------------------------------------------
// CMPMDialog::ConstructL
// -----------------------------------------------------------------------------
//
void CMPMDialog::ConstructL( const RAvailableIAPList& aIAPList )
    {
    User::LeaveIfError( CopyArray( aIAPList, iIAPList ) );
    CMPMDialogBase::ConstructL();
    }

// -----------------------------------------------------------------------------
// CMPMDialog::ConstructL
// -----------------------------------------------------------------------------
//
void CMPMDialog::Start( CMPMDialogBase* /*aDlg*/ )
    {
    InitSelectConnection();
    }

// -----------------------------------------------------------------------------
// CMPMDialog::IntSelectConnection
// -----------------------------------------------------------------------------
//
void CMPMDialog::InitSelectConnection()
    {
    TInt err = iDlgServ.Connect();
    // Try to publish sort snap data
    //
    if ( err == KErrNone )
        {
        TRAP( err, PublishSortSnapInfoL() )
        }
    if ( err != KErrNone )
        {
        // Jump to RunL()
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        SetActive();
        return;
        }

    // Map bearer set. bearerSet variable is a combination of values
    // mapped to TCommDbBearer while iBearerSet is a combination of
    // TExtendedConnPref::TExtendedConnBearer values.

    TUint32 bearerSet( ECommDbBearerVirtual );
    if ( iBearerSet == TExtendedConnPref::EExtendedConnBearerUnknown )
        {
        bearerSet |= ECommDbBearerCSD | ECommDbBearerWcdma |
                     ECommDbBearerWLAN;
        }
    else
        {
        if ( iBearerSet & TExtendedConnPref::EExtendedConnBearerWLAN )
            {
            bearerSet |= ECommDbBearerWLAN;
            }
        if ( iBearerSet & TExtendedConnPref::EExtendedConnBearerCellular )
            {
            bearerSet |= ECommDbBearerWcdma;
            }
        }

    // API definition was provided by Symbian, but we don't really need 
    // these parameters with AccessPointConnection. 
    // 
    TUint32 unusedVariable( 0 );

    /*  AccessPointConnection API implements also the ChangeConnection in 
     *  the ALR "world", you should also be careful of the input value of 
     *  iSnapOrIAPId, because the Destination with the input ElementID is 
     *  considered the "current Destination in use", so it will not be 
     *  listed by the notifier. So if you really want to see the ALR 
     *  version of AccessPointConnection, in input iSnapOrIAPId MUST be  
     *  an invalid ElementID (e.g. 0), to be sure that no Destinations 
     *  will be removed from the list. 
     */
    iDlgServ.AccessPointConnection( unusedVariable,
                                    unusedVariable,
                                    iSnapOrIAPId,
                                    bearerSet,
                                    iStatus );

    SetActive();
    }

// -----------------------------------------------------------------------------
// CMPMDialog::PublishSortSnapInfoL
// -----------------------------------------------------------------------------
//
void CMPMDialog::PublishSortSnapInfoL()
    {
    // allow PublishSortSnapInfoL to run only 
    // if the dialog is the first one in the queue.
    // => First one is active one.
    if ( !IsFirst() )
        {
        return;
        }
        
    MPMLOGSTRING( "CMPMDialog::PublishSortSnapInfoL" )
    RArray<TUint> snaps;
    CleanupClosePushL( snaps );
    
    // set first snap to 0 as ungathegorised
    //
    snaps.AppendL( KSortUncategorisedIaps );
    
    // Find Snap ids
    //
    iServer.CommsDatAccess()->FindAllSnapsL( snaps );
    
    // Write iaps for each snap, also 0 is 
    // included for uncategorised
    //
    TMpmSnapPubSubFormat snapData;
    TInt i( 0 ), err( KErrNone );
    for(; ( i < snaps.Count() ) && 
          ( i < KMPMSortSnapMaxKey ); i++ )
        {
        snapData.iSnap = snaps[ i ];
        snapData.iSortedIaps.Reset();
        // Get sorted iaps
        //
        iIapSelection.Session()->SortSnapL( snaps[ i ], snapData.iSortedIaps );
        
        // Define pub&sub key if not already defined
        //
        err = RProperty::Define( KMPMSortSnapCategory, 
                                 i,
                                 KMPMSortSnapType,
                                 KMPMSortSnapReadPolicy,
                                 KMPMSortSnapWritePolicy );
        
        if ( err != KErrAlreadyExists &&
             err != KErrNone )
            {
            MPMLOGSTRING2( "CMPMDialog::PublishSortSnapInfoL: error in define: %d ", err )
            }
        
        TPtrC8 dataPtr( reinterpret_cast< TUint8* >( &snapData  ), sizeof( snapData ) );

        err = RProperty::Set( KMPMSortSnapCategory, 
                              i, 
                              dataPtr );
        if ( err != KErrNone )
            {
            MPMLOGSTRING2( "CMPMDialog::PublishSortSnapInfoL: error in set: %d ", err )
            }    
        MPMLOGSTRING3( "CMPMDialog::PublishSortSnapInfoL: snap %d iap count %d",
                       snapData.iSnap, snapData.iSortedIaps.Count() )
        }

    // set rest of the keys to undefined
    // 
    while( i < KMPMSortSnapMaxKey )
        {
        err = RProperty::Delete( KMPMSortSnapCategory, i );
        if ( err != KErrNotFound &&
             err != KErrNone )
            {
            MPMLOGSTRING2( "CMPMDialog::PublishSortSnapInfoL: error in delete: %d ", err )
            }
        i++;
        }
    CleanupStack::PopAndDestroy( &snaps );    
    }

// -----------------------------------------------------------------------------
// CMPMDialog::RunL
// -----------------------------------------------------------------------------
//
void CMPMDialog::RunL()
    {
    MPMLOGSTRING2( "CMPMDialog::RunL: status = %i", 
        iStatus.Int() )
        
    TMDBElementId tableId = iSnapOrIAPId & KCDMaskShowRecordType; 
    TBool isIap( EFalse );
    if ( tableId == KCDTIdIAPRecord )
        {
        isIap = ETrue;
        }
    
    // Clean data from publish & subscribe
    //
    CleanSortSnapInfo();
        
    TUint32 id = ( iSnapOrIAPId & KCDMaskShowRecordId ) >> KShift8;
    iIapSelection.HandleUserSelectionL( isIap, id, iStatus.Int() );
    }


// -----------------------------------------------------------------------------
// CMPMDialog::RunError
// -----------------------------------------------------------------------------
//
TInt CMPMDialog::RunError(TInt aError)
    {
    // Handles a leave occurring in the request completion event handler 
    // RunL(). The active scheduler calls this function if this active 
    // object's RunL() function leaves. This gives this active object 
    // the opportunity to perform any necessary cleanup.
    // 
    MPMLOGSTRING2( "CMPMDialog::RunError: RunL left with error = %i", aError )
    iIapSelection.HandleUserSelectionError( aError );

    // Clean data from publish & subscribe
    //
    CleanSortSnapInfo();

    // Return KErrNone to prevent panic 
    // 
    return KErrNone;
    }
    
    
// -----------------------------------------------------------------------------
// CMPMDialog::DoCancel
// -----------------------------------------------------------------------------
//
void CMPMDialog::DoCancel()
    {
    iDlgServ.CancelAccessPointConnection();
    // Clean data from publish & subscribe
    //
    CleanSortSnapInfo();
    }


// -----------------------------------------------------------------------------
// CMPMDialog::CopyArray
// -----------------------------------------------------------------------------
//
TInt CMPMDialog::CopyArray( const RAvailableIAPList& aOrigin, 
                            RAvailableIAPList&       aCopy )
	{
	TInt err( KErrNone );

    // Clean array first
    // 
	aCopy.Reset(); 

	for( TInt i( 0 ); ( i < aOrigin.Count() ) && ( err == KErrNone ); ++i )
		{
		err = aCopy.Append( aOrigin[i] );
		if ( err != KErrNone )
			{
			// Cleanup and return
			// 
            MPMLOGSTRING2( "CMPMDialog::CopyArray: Append error = %i", err )
			aCopy.Reset();
			}
		}

	return err;
	} 

// -----------------------------------------------------------------------------
// CMPMDialog::CleanSortSnapInfoL
// -----------------------------------------------------------------------------
//
void CMPMDialog::CleanSortSnapInfo()
    {
    MPMLOGSTRING( "CMPMDialog::CleanSortSnapInfoL" )

    // Clean info from all keys
    //
    for( TInt i(0); i < KMPMSortSnapMaxKey; i++ )
        {
        TInt err = RProperty::Delete( KMPMSortSnapCategory, i );
        if ( err != KErrNotFound &&
             err != KErrNone )
            {
            MPMLOGSTRING2( "CMPMDialog::CleanSortSnapInfoL: error in delete: %d ", err )
            }
        }
    }

//  End of File
