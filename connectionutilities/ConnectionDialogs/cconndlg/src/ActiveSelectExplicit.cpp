/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of plugins (base & all derived).
*
*/


// INCLUDE FILES
#include "ActiveSelectExplicit.h"
#include "SelectExplicitDialog.h"
#include "ConnectionInfo.h"
#include "ConnectionInfoArray.h"
#include "CConnDlgPrivateCRKeys.h"
#include "ConnectionDialogsLogger.h"
#include "sortsnaputils.h"

#include <commdb.h>
#include <StringLoader.h>
#include <wlanmgmtcommon.h>
#include <cmdestinationext.h>
#include <cmpluginwlandef.h>
#ifndef __WINS__
#include <wlanmgmtclient.h>
#endif
#include <AknWaitDialog.h>
#include <AknIconArray.h>
#include <AknsUtils.h>

#include <CConnDlgPlugin.rsg>
#include <data_caging_path_literals.hrh>
#include <cmmanager.mbg>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat.h>
#else
#include <commsdat.h>
#include <commsdat_partner.h>
#endif
#include <wlancontainer.h>


// CONSTANTS

LOCAL_D const TInt KMaxEntryLength = CommsDat::KMaxTextLength*2 + 20;
LOCAL_D const TInt KIconsGranularity = 4;

using namespace CMManager;


///////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------
// CActiveSelectExplicit::CActiveSelectExplicit()
// ---------------------------------------------------------
//
CActiveSelectExplicit::CActiveSelectExplicit( 
                                        CSelectConnectionDialog* aCallerDialog,
                                        TUint aDestId, 
                                        TInt aRefreshInterval,
                                        TUint32 aBearerSet, 
                                        TBool aIsWLANFeatureSupported, 
                                        TUint aDefaultCMId )
: iCallerDialog( aCallerDialog ),
  iDestId( aDestId ),
//  isDialogDeleting( EFalse ),
  iRefreshInterval( aRefreshInterval ),
  iBearerSet( aBearerSet ),
  iIsWLANFeatureSupported( aIsWLANFeatureSupported ), 
  iDefaultCMId( aDefaultCMId )
    {
    }

    
// ---------------------------------------------------------
// CActiveSelectExplicit* CActiveSelectExplicit::NewL()
// ---------------------------------------------------------
//
CActiveSelectExplicit* CActiveSelectExplicit::NewL( 
                                        CSelectConnectionDialog* aCallerDialog,
                                        TUint aDestId, 
                                        TInt aRefreshInterval,
                                        TUint32 aBearerSet, 
                                        TBool aIsWLANFeatureSupported, 
                                        TUint aDefaultCMId )
    {
    CLOG_ENTERFN( "CActiveSelectExplicit::NewL " );  
    
    CActiveSelectExplicit* self = new ( ELeave ) CActiveSelectExplicit(
                                    aCallerDialog, aDestId, aRefreshInterval,
                                    aBearerSet, aIsWLANFeatureSupported, 
                                    aDefaultCMId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    CLOG_LEAVEFN( "CActiveSelectExplicit::NewL " );      

    return self;        
    }


// ---------------------------------------------------------
// CActiveSelectExplicit::ConstructL()
// ---------------------------------------------------------
//
void CActiveSelectExplicit::ConstructL()
    {
    CActiveBase::ConstructL();

    iItems = new( ELeave ) CConnectionInfoArray();
    iIcons = new( ELeave ) CAknIconArray( KIconsGranularity );

    iCmManagerExt.OpenL();
    }

// ---------------------------------------------------------
// CActiveSelectExplicit::~CActiveSelectExplicit()
// ---------------------------------------------------------
//    
CActiveSelectExplicit::~CActiveSelectExplicit()
    {
    Cancel();

    if ( iItems )
        {
        iItems->ResetAndDestroy();
        }
    delete iItems;    
    iItems = NULL;

    iCmManagerExt.Close();
    }


// ---------------------------------------------------------
// CActiveSelectExplicit::Cancel()
// ---------------------------------------------------------
// 
void CActiveSelectExplicit::Cancel()
    {
    StopTimer();
    CActive::Cancel();

    if( iDialog  && !( iDialog->GetOkToExit() ) )
        {
        CLOG_WRITEF( _L( "iDialog->GetOkToExit() : %b" ), iDialog->GetOkToExit() );
        delete iDialog;
        iDialog = NULL;
        }
    }


// ---------------------------------------------------------
// CActiveSelectExplicit::DoCancel()
// ---------------------------------------------------------
// 
void CActiveSelectExplicit::DoCancel()
    {
    CLOG_ENTERFN( "CActiveSelectExplicit::DoCancel " );  
    
/*    if( isDialogDeleting )
        {
        CLOG_WRITE( "isDialogDeleting" );
        
        delete iDialog;
        iDialog = NULL;   
        
        isDialogDeleting = EFalse;         
        }
  */      
    CLOG_LEAVEFN( "CActiveSelectExplicit::DoCancel " );          
    }

    
// ---------------------------------------------------------
// CActiveSelectExplicit::RunL()
// ---------------------------------------------------------
//     
void CActiveSelectExplicit::RunL()
    {
    CLOG_ENTERFN( "CActiveSelectExplicit::RunL " );  
    
    if( iStatus == KErrNone )
        {
        BuildConnectionListL();
        iDialog->RefreshDialogL( iItems, iIcons, ETrue );
        }     
    
    CLOG_LEAVEFN( "CActiveSelectExplicit::RunL " );      
        
    }

    
// ---------------------------------------------------------
// CActiveSelectExplicit::StartSearchIAPsL()
// ---------------------------------------------------------
//    
void CActiveSelectExplicit::ShowSelectExplicitL()
    {    
    BuildConnectionListL();

    iDialog = CSelectExplicitDialog::NewL( iCallerDialog );
    iDialog->PrepareAndRunLD( iItems, iIcons, EFalse );

    if( iRefreshInterval )
        {
        StartTimerL( iRefreshInterval );
        }     
    }
   
 
// ---------------------------------------------------------
// CActiveSelectExplicit::BuildConnectionListL()
// ---------------------------------------------------------
//
void CActiveSelectExplicit::BuildConnectionListL()
    {
    CLOG_ENTERFN( "CActiveSelectExplicit::BuildConnectionListL " );  

    if ( iItems )
        {
        iItems->ResetAndDestroy();
        }

    if ( iIcons )
        {
        iIcons->ResetAndDestroy();
        }

    if ( iDefaultCMId != 0 )  
        {   // It automatically means also that the feature is supported
        _LIT( KDriveZ, "z:" );  // ROM folder
        // Name of the MBM file containing icons
        _LIT( KManagerIconFilename, "cmmanager.mbm" );
    
        TFileName iconsFileName;

        iconsFileName.Append( KDriveZ );
        iconsFileName.Append( KDC_APP_BITMAP_DIR );
        iconsFileName.Append( KManagerIconFilename );

        iIcons->AppendL( AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(),
                                    KAknsIIDQgnIndiCbHotAdd,
                                    iconsFileName, 
                                    EMbmCmmanagerQgn_indi_cb_hot_add, 
                                    EMbmCmmanagerQgn_indi_cb_hot_add_mask ) );
        }

    TBool easyWlanIsInDb = EFalse;

#ifdef _DEBUG
    if ( iDestId )
        {   // Real destination, not for uncategorized
        RCmDestinationExt cmDestExt = iCmManagerExt.DestinationL( iDestId );
        CleanupClosePushL( cmDestExt );
        CLOG_WRITEF( _L( "Destination id %d, Total number of methods %d" ),
                    iDestId, cmDestExt.ConnectionMethodCount() );

        for ( TInt j = 0; j < cmDestExt.ConnectionMethodCount(); j++ )
            {
            RCmConnectionMethodExt cmConnMethodExt;
            TRAPD( err, cmConnMethodExt = cmDestExt.ConnectionMethodL( j ) );
            if ( err == KErrNone )
                {
                CleanupClosePushL( cmConnMethodExt );
                HBufC* bufMethod = cmConnMethodExt.GetStringAttributeL(
                                                                    ECmName );
                CleanupStack::PushL( bufMethod );
                TBuf<CommsDat::KMaxTextLength+1> nameMethod;
                nameMethod = *bufMethod;
                nameMethod.ZeroTerminate();

                CLOG_WRITEF( _L( "ConnMethod id %d, Method name %s" ), 
                    cmConnMethodExt.GetIntAttributeL( ECmIapId ), nameMethod.Ptr() );

                CleanupStack::PopAndDestroy( 2 ); // bufMethod, cmConnMethodExt
                }
            }
        CleanupStack::PopAndDestroy();  // cmDestExt
        }
#endif
        TSortSnapUtils sortSnapUtils( iMPM );
        TMpmSnapBuffer snapBuffer;
        TInt err = sortSnapUtils.GetSortSnapData( iDestId, snapBuffer );
        CLOG_WRITEF( _L( "GetSortSNAPDataL returned = %d. list.Count() = %d" ), err, snapBuffer.Count() );

        if ( err == KErrNone )
            {
            RCmConnectionMethodExt cmConnMethodExt;
            CleanupClosePushL( cmConnMethodExt );
            for ( TInt numCM = 0; numCM < snapBuffer.Count(); numCM++ )
                {
                AddConnectionMethodItemL( snapBuffer.iIapId[numCM], 
                                          iDestId == 0,
                                          easyWlanIsInDb, 
                                          snapBuffer.iIapId[numCM] == iDefaultCMId );
                }
            CleanupStack::PopAndDestroy();  // cmConnMethodExt
            }   

    // Add any ad-hoc WLAN iaps to the list
    if ( ( iBearerSet & ECommDbBearerWLAN ) && ( iDestId == 0 ) && iIsWLANFeatureSupported )
        {
        AddAdhocWlanIapsL();	
        }

    CLOG_LEAVEFN( "CActiveSelectExplicit::BuildConnectionListL " );
    }


// ---------------------------------------------------------
// CActiveSelectExplicit::AddConnectionMethodItemL()
// ---------------------------------------------------------
//
void CActiveSelectExplicit::AddConnectionMethodItemL( TUint32 aId,
                                                TBool aCheckForEasyWlan, 
                                                TBool& aEasyWlanIsInDb, 
                                                TBool aDefaultCM )
    {
    CLOG_ENTERFN( "CActiveSelectExplicit::AddConnectionMethodItemL" );  

    CConnectionInfo* conIn;

    RCmConnectionMethodExt cmConnMethodExt;
    CLOG_WRITEF( _L( "Uncat dest id %d" ), aId );

    TRAPD( err, cmConnMethodExt = iCmManagerExt.ConnectionMethodL( aId ) );
    CLOG_WRITEF( _L( "err %d" ), err );

    if ( err == KErrNone )
        {
        CleanupClosePushL( cmConnMethodExt );

        TUint32 bearer = cmConnMethodExt.GetIntAttributeL( 
                                                        ECmCommsDBBearerType );
        CLOG_WRITEF( _L( "bearer %d" ), bearer );

        if ( bearer & iBearerSet )
            {
            CLOG_WRITE( "To be listed!" );

            TBool isCMEasyWlan = EFalse;

            HBufC* bufName = cmConnMethodExt.GetStringAttributeL(
                                                                ECmName );

            TBuf<CommsDat::KMaxTextLength+1> nameCM;
            nameCM = *bufName;
            nameCM.ZeroTerminate();
            delete bufName;

            CLOG_WRITEF( _L( "Method name %s. Bearer = %d" ), nameCM.Ptr(), bearer );

            if ( aCheckForEasyWlan && bearer == KCommDbBearerWLAN && 
                 iIsWLANFeatureSupported )
                {
                if ( !aEasyWlanIsInDb )
                    {
                    HBufC* bufSSID = cmConnMethodExt.GetStringAttributeL( 
                                                                EWlanSSID );
                    if ( !bufSSID || bufSSID->Des().Length() == 0 )
                        {
                        isCMEasyWlan = ETrue;
                        aEasyWlanIsInDb = ETrue;
                        }
                    delete bufSSID;
                    }
                }

            if ( !isCMEasyWlan )
                {
                CGulIcon* iconCM = ( CGulIcon* )
                                    cmConnMethodExt.GetIntAttributeL(
                                                            ECmBearerIcon );
                if ( iconCM )
                    {
                    CLOG_WRITEF( _L( "iconCM found" ) );
                    CleanupStack::PushL( iconCM );
                    iIcons->AppendL( iconCM );

                    // icon array took over ownership
                    CleanupStack::Pop( iconCM );
                    }
                else
                    {
                    CLOG_WRITEF( _L( "iconCM not found" ) );
                    }

                TBuf<KMaxEntryLength> temp;
                _LIT( KIconIdName, "%d\t%s" );
                _LIT( KIconIdNameDefaultConn, "%d\t%s\t0" );

                if ( aDefaultCM )
                    {
                    temp.Format( KIconIdNameDefaultConn, iIcons->Count()-1,
                                 nameCM.Ptr() );
                    }
                else
                    {
                    temp.Format( KIconIdName, iIcons->Count()-1, 
                                 nameCM.Ptr() );
                    }

                conIn = CConnectionInfo::NewL( aId, 0, temp );
                CleanupStack::PushL( conIn );
                iItems->AppendL( conIn );
                CleanupStack::Pop( conIn );
                }
            }
        CleanupStack::PopAndDestroy();  // cmConnMethodExt
        }

    CLOG_LEAVEFN( "CActiveSelectExplicit::AddConnectionMethodItemL" );
    }

// ---------------------------------------------------------
// CActiveSelectExplicit::AddAdhocWlanIapsL()
// ---------------------------------------------------------
//
void CActiveSelectExplicit::AddAdhocWlanIapsL()
    {
    CLOG_ENTERFN( "CActiveSelectExplicit::AddAdhocWlanIapsL " );
    
    // Creating a session with the latest version
    CMDBSession* db = CMDBSession::NewLC( CMDBSession::LatestVersion() );
    db->SetAttributeMask( ECDProtectedWrite );
    
    // Create wlan service record set (RArray (or RPtrArray?))
    CMDBRecordSet<CCDWlanServiceRecord>* wlanSet =
        new (ELeave)CMDBRecordSet<CCDWlanServiceRecord>(   CCDWlanServiceRecord::TableIdL( *db )   );
    CleanupStack::PushL( wlanSet ); 
    
    // Create wlan service record
    CCDWlanServiceRecord* wlanRecord =
        new (ELeave)CCDWlanServiceRecord(   CCDWlanServiceRecord::TableIdL( *db )   );
    
    CleanupStack::PushL( wlanRecord );
    
    // Set ConnectionMode field in wlan service record (see wlancontainer.h)
    // This works with EWlanOperatingModeAdhoc, fails with EWlanConnectionModeAdhoc
    wlanRecord->iWlanConnMode.SetL( EWlanOperatingModeAdhoc );
    
    // Append wlan service record to wlan service record set (currently empty)
    wlanSet->iRecords.AppendL( wlanRecord );
    CleanupStack::Pop( wlanRecord ); // ownership moved to RPointerArray
    
    // Find matcing wlan service records
    if( wlanSet->FindL( *db ) )
        {
        TInt wlanRecordCount( wlanSet->iRecords.Count() );
        CLOG_WRITEF( _L("wlanSet record count = %d"), wlanRecordCount );
        
        TBool easyWlanIsInDb = EFalse;
        
        // loop for each record found
        for ( TInt i = 0; i < wlanRecordCount; i++ )
            {
            // Seems we need to specifically load the current record again to be sure all fields are up to date
            wlanRecord->SetElementId( wlanSet->iRecords[i]->ElementId() );
            // Load will only look at ElementId and updates the rest of the fields
            wlanRecord->LoadL( *db );
            
            if ( !( wlanRecord->iWLanSSID.IsNull() ) )
                {
                // Don't include Easy WLAN
                // lets print some stuff to log            
                CLOG_WRITEF( _L("New ad-hoc WLAN IAP:" ));
                CLOG_WRITEF( _L("  iWlanServiceId = %d"), static_cast<TUint>( wlanRecord->iWlanServiceId ) );
                CLOG_WRITEF( _L("  iWlanConnMode  = %d"), static_cast<TUint>( wlanRecord->iWlanConnMode ) );
                CLOG_WRITEF( _L("  iWLanSSID      = %S"), &(static_cast<TDesC>( wlanRecord->iWLanSSID ) ) );
            
                // Create an iap record
                CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*> ( 
                    CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
                CleanupStack::PushL( iapRecord );
            
                iapRecord->iServiceType.SetL( TPtrC( KCDTypeNameLANService ) );
                // Set the service field to the same value as in the wlan service record we are currently looking
                iapRecord->iService = wlanRecord->iWlanServiceId;
                // Find the matching iap record
                if( iapRecord->FindL( *db ) )
                    {
                    TUint iapId( 0 );
                
                    // read the iap id
                    iapId = iapRecord->RecordId();
                    CLOG_WRITEF( _L("  iapId          = %d"), iapId );
                
                    AddConnectionMethodItemL( iapId, 
                                              iDestId == 0,
                                              easyWlanIsInDb, 
                                              iapId == iDefaultCMId );
                    }
                CleanupStack::PopAndDestroy( iapRecord );
                }
            }
        }
    else
        {
        CLOG_WRITEF( _L("no matcing wlan records found." ));
        }
        
    CleanupStack::PopAndDestroy( wlanSet );
    CleanupStack::PopAndDestroy( db );
    
    CLOG_LEAVEFN( "CActiveSelectExplicit::AddAdhocWlanIapsL" );
    }
    
// End of File
