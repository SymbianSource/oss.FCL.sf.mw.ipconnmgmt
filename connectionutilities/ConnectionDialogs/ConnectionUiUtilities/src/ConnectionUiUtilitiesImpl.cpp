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
* Description: 
*      Implementation of class CConnectionUiUtilitiesImpl.   
*      
*
*/


// INCLUDE FILES

#include <bautils.h>
#include <StringLoader.h>
#include <commdb.h>
#include <aknnotewrappers.h>
#include <AknGlobalNote.h>
#include <aknradiobuttonsettingpage.h>
#include <ConnectionUiUtilities.h>
#include <AknsUtils.h>
#include <featmgr.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat.h>
#else
#include <commsdat.h>
#include <commsdat_partner.h>
#endif

#ifndef __WINS__
#include <wlanmgmtclient.h>
#include <WlanCdbCols.h>
#endif  // ! __WINS__

#include "ConnectionUiUtilitiesImpl.h"
#include "ChangeConnectionDlg.h"
#include "ActiveCChangeConnectionDlg.h"

#include <ConnectionUiUtilities.rsg>
#include <data_caging_path_literals.hrh>

#include "ConnectionDialogsLogger.h"


// CONSTANTS

// ROM folder
_LIT( KDriveZ, "z:" );

// RSC file name.
_LIT( KConnUiUtilsResName, "ConnectionUiUtilities.rsc" );



// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::NewL
// ---------------------------------------------------------
//
CConnectionUiUtilitiesImpl* CConnectionUiUtilitiesImpl::NewL()
    {
    CConnectionUiUtilitiesImpl* utilsImpl = 
                                new ( ELeave ) CConnectionUiUtilitiesImpl();
    CleanupStack::PushL( utilsImpl );
    utilsImpl->ConstructL();
    CleanupStack::Pop( utilsImpl ); 
    return utilsImpl;
    }


// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::CConnectionUiUtilitiesImpl
// ---------------------------------------------------------
//
CConnectionUiUtilitiesImpl::CConnectionUiUtilitiesImpl() 
: iResOffset( 0 ),
  iIsWlanSupported( EFalse )
    {
    for ( TInt i = 0; i < KNumberOfWrappedDialogs; i++ )
        {
        iActiveWrapper[i] = NULL;
        }
    }


// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::ConstructL
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::ConstructL()
    {
    User::LeaveIfError( iNotif.Connect() );    

    FeatureManager::InitializeLibL();
    iIsWlanSupported = 
                FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );
    FeatureManager::UnInitializeLib();
    }


// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::~CConnectionUiUtilitiesImpl
// ---------------------------------------------------------
//
CConnectionUiUtilitiesImpl::~CConnectionUiUtilitiesImpl()
    {    
    if ( iResOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResOffset );
        }
    iNotif.Close();

    for ( TInt i = 0; i < KNumberOfWrappedDialogs; i++ )
        {
        delete iActiveWrapper[i];
        }
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::LoadResourceFileL
// ---------------------------------------------------------
//    
void CConnectionUiUtilitiesImpl::LoadResourceFileL()
    {
    if ( !iResOffset )
        {
        // can't use resource here because it has not been added yet....
        TFileName fileName;

        fileName.Append( KDriveZ );
        fileName.Append( KDC_RESOURCE_FILES_DIR );
        fileName.Append( KConnUiUtilsResName );

        BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), 
                                        fileName );
        iResOffset = CCoeEnv::Static()->AddResourceFileL( fileName );
        }
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::AlwaysAskPageL
// ---------------------------------------------------------
//

TBool CConnectionUiUtilitiesImpl::AlwaysAskPageL( 
                                                TCuuAlwaysAskResults& aResult )
    {
    LoadResourceFileL();
    // options array
    CDesCArrayFlat* values = new( ELeave )CDesCArrayFlat( 1 );
    CleanupStack::PushL( values );

    HBufC* value1 = CCoeEnv::Static()->AllocReadResourceLC( 
                                                R_ALWAYS_ASK_ALWAYS_ASK );
    values->AppendL( *value1 );
    CleanupStack::PopAndDestroy( value1 );

    HBufC* value2 = CCoeEnv::Static()->AllocReadResourceLC( 
                                                R_ALWAYS_ASK_USER_DEFINED );
    values->AppendL( *value2 );
    CleanupStack::PopAndDestroy( value2 );

    TInt index( 0 );
      switch ( aResult )
        {
        case ECuuAlwaysAsk:
            {
            index = 0;
            break;
            }
        case ECuuUserDefined:
            {
            index = 1;
            break;
            }
        default:
            break;
        }
  
    CAknRadioButtonSettingPage* dlg = new ( ELeave )CAknRadioButtonSettingPage(
                                            R_ALWAYS_ASK_PAGE, index, values );

    TBool oKPressed( EFalse );
    
    CleanupStack::PushL( dlg );

    AknsUtils::SetAvkonSkinEnabledL( ETrue );
    
    CleanupStack::Pop( dlg );
    
    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
        {
        oKPressed = ETrue;
        }

    switch ( index )
        {
        case 0:
            {
            aResult = ECuuAlwaysAsk;
            break;
            }
        case 1:
            {
            aResult = ECuuUserDefined;
            break;
            }
        default:
            break;
        }

      CleanupStack::PopAndDestroy( values );

    return oKPressed;
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::ChangeConnectionL
// ---------------------------------------------------------
//
TBool CConnectionUiUtilitiesImpl::ChangeConnectionL( TUint32& aIAPId,
                                                     TConnectionPrefs aPrefs )
    {
    CLOG_ENTERFN( "CConnectionUiUtilitiesImpl::ChangeConnectionL " );  
    
    TUint32 origId = aIAPId;
    LoadResourceFileL();
    CCommsDatabase* db = CCommsDatabase::NewL( EDatabaseTypeUnspecified );
    CleanupStack::PushL( db );

    TBuf<CommsDat::KMaxTextLength> connName;

    GetConnectionNameL( aIAPId, *db, connName );
    CleanupStack::PopAndDestroy( db ); // db
    
    CActiveCChangeConnectionDlg* activeChangeConnDlg =
                CActiveCChangeConnectionDlg::NewL( &aIAPId, aPrefs, connName );
    CleanupStack::PushL( activeChangeConnDlg );
            
    TBool retval = activeChangeConnDlg->StartSearchIAPsL();    
    
    CLOG_WRITE( "activeChangeConnDlg->StartSearchIAPsL();" );
            
    CLOG_WRITEF( _L( "aIAPId : %d" ),  aIAPId );
    CLOG_WRITEF( _L( "origId : %d" ),  origId );
    CLOG_WRITEF( _L( "retval : %d" ),  retval );
    
    CleanupStack::PopAndDestroy( activeChangeConnDlg );     
    
    CLOG_LEAVEFN( "CConnectionUiUtilitiesImpl::ChangeConnectionL " );  

    return retval;
    }


// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::ShowConnectionChangedNoteL
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::ShowConnectionChangedNoteL( 
                                                        const TUint32& aIAPId )

    {
    LoadResourceFileL();
    CCommsDatabase* db = CCommsDatabase::NewL( EDatabaseTypeUnspecified );
    CleanupStack::PushL( db );

    TBuf<CommsDat::KMaxTextLength> connName;

    GetConnectionNameL( aIAPId, *db, connName );
    CleanupStack::PopAndDestroy( db );

    HBufC* stringLabel = StringLoader::LoadLC( R_CONN_CHANGED, connName );

    CAknInformationNote* infNote = new( ELeave )CAknInformationNote();
    infNote->ExecuteLD( *stringLabel );

    CleanupStack::PopAndDestroy( stringLabel );
    }



// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::SearchWLANNetwork
// ---------------------------------------------------------
//
TBool CConnectionUiUtilitiesImpl::SearchWLANNetwork( TWlanSsid& aSSID,
                                TWlanConnectionMode& aConnectionMode,
                                TWlanConnectionSecurityMode& aSecurityMode )
    {
    return SearchWLANNetwork( aSSID, 
                              aConnectionMode, 
                              aSecurityMode, 
                              iDummyExtSecMode, 
                              iDummy );
    }
    
// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::SearchWLANNetwork
// ---------------------------------------------------------
//
TBool CConnectionUiUtilitiesImpl::SearchWLANNetwork( TWlanSsid& aSSID,
                                TWlanConnectionMode& aConnectionMode,
                                TWlanConnectionExtentedSecurityMode& aExtSecurityMode )
    {
    return SearchWLANNetwork( aSSID, 
                              aConnectionMode, 
                              iDummySecMode, 
                              aExtSecurityMode, 
                              iDummy );
    }    

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::SearchWLANNetwork
// ---------------------------------------------------------
//
TBool CConnectionUiUtilitiesImpl::SearchWLANNetwork( TWlanSsid& aSSID,
                                TWlanConnectionMode& aConnectionMode,
                                TWlanConnectionSecurityMode& aSecurityMode, 
                                TBool& aProtectedSetupSupported )
    {
    return SearchWLANNetwork( aSSID, 
                              aConnectionMode, 
                              aSecurityMode, 
                              iDummyExtSecMode, 
                              aProtectedSetupSupported );
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::SearchWLANNetwork
// ---------------------------------------------------------
//    
TBool CConnectionUiUtilitiesImpl::SearchWLANNetwork( TWlanSsid& aSSID,
                                TWlanConnectionMode& aConnectionMode,
                                TWlanConnectionExtentedSecurityMode& aExtSecurityMode, 
                                TBool& aProtectedSetupSupported )
    {
    return SearchWLANNetwork( aSSID, 
                              aConnectionMode, 
                              iDummySecMode, 
                              aExtSecurityMode, 
                              aProtectedSetupSupported );
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::SearchWLANNetwork
// ---------------------------------------------------------
//
TBool CConnectionUiUtilitiesImpl::SearchWLANNetwork( TWlanSsid& aSSID,
                                TWlanConnectionMode& aConnectionMode,
                                TWlanConnectionSecurityMode& aSecurityMode,
                                TWlanConnectionExtentedSecurityMode& aExtSecurityMode, 
                                TBool& aProtectedSetupSupported )
    {
    TBool result( EFalse );
    
    if ( iIsWlanSupported )
        {
        iNetworkPrefs();

        if ( !iActiveWrapper[ESearchWlan] )
            {
            TRAP_IGNORE( iActiveWrapper[ESearchWlan] = 
                                        CActiveWrapper::NewL( ESearchWlan ) );
            }

        if ( iActiveWrapper[ESearchWlan] )
            {
            iActiveWrapper[ESearchWlan]->StartSearchWLANNetwork( 
                                                            iNetworkPrefs );

            CLOG_WRITEF( _L( "iActiveWrapper[ESearchWlan]->iStatus.Int(): %d" ),  
                           iActiveWrapper[ESearchWlan]->iStatus.Int() );

            result = iActiveWrapper[ESearchWlan]->iStatus.Int() ? EFalse : 
                                                                  ETrue;
            }
    
        if( result )
            {
            aSSID = iNetworkPrefs().iSsId;
            aConnectionMode = iNetworkPrefs().iNetworkMode;
            aSecurityMode = iNetworkPrefs().iSecMode;
            aExtSecurityMode = iNetworkPrefs().iExtSecMode;
            aProtectedSetupSupported = 
                                    iNetworkPrefs().iProtectedSetupSupported;
            }
        }    
    
    CLOG_WRITEF( _L( "SearchWLANNetwork result : %b" ),  result ); 
                       
    return result;        
    }


// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::CancelSearchWLANNetwork
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::CancelSearchWLANNetwork()
    {
    if ( iIsWlanSupported )
        {
        iActiveWrapper[ESearchWlan]->Cancel();
        }
    }


// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::GetConnectionNameL
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::GetConnectionNameL( const TUint32& aIAPId,
                                                     CCommsDatabase& aDb,
                                                     TDes& aConnectionName )
    {
    __ASSERT_DEBUG( &aDb, User::Panic( KErrNullPointer, KErrNone ) );

    CCommsDbTableView* table = aDb.OpenViewMatchingUintLC( TPtrC( IAP ), 
                                                           TPtrC( COMMDB_ID ),
                                                           aIAPId );
    User::LeaveIfError( table->GotoFirstRecord() );
    table->ReadTextL( TPtrC( COMMDB_NAME ), aConnectionName );
    if ( aConnectionName.Length() == 0 || 
         aConnectionName.Left( KMrouterName().Length() ).CompareF( 
                                                    KMrouterName ) == 0 )
        {
        User::Leave( KErrGeneral );
        }

    TUint32 serviceId;
    // Get IAP service
    table->ReadUintL( TPtrC( IAP_SERVICE ), serviceId );

    CleanupStack::PopAndDestroy( table );

#ifndef __WINS__        
    if ( iIsWlanSupported )
        {
        CCommsDbTableView* wLanServiceTable = NULL;
            
        TRAPD( err, 
            { // this leaves if the table is empty....
            wLanServiceTable = aDb.OpenViewMatchingUintLC( 
                                                    TPtrC( WLAN_SERVICE ),
                                                    TPtrC( WLAN_SERVICE_ID ),
                                                    serviceId );
            CleanupStack::Pop( wLanServiceTable ); // wLanServiceTable
            } );

        CLOG_WRITEF( _L( "OpenViewMatchingUintLC returned %d" ), err );

        if ( err == KErrNone )
            {
            CleanupStack::PushL( wLanServiceTable );

            TInt errorCode = wLanServiceTable->GotoFirstRecord();
            TWlanSsid ssid;
            
            if ( errorCode == KErrNone )
                {
                wLanServiceTable->ReadTextL( TPtrC( NU_WLAN_SSID ), ssid );
                if ( ssid.Length() == 0 )
                    {
                    CLOG_WRITEF( _L( "It is the EasyWlan!" ) );

                    TWlanSsid ssidConn;
                    CWlanMgmtClient *wlanMgmtClient = CWlanMgmtClient::NewL();
                    CleanupStack::PushL( wlanMgmtClient );

                    if ( !wlanMgmtClient->GetConnectionSsid( ssidConn ) )
                        {
                        CLOG_WRITEF( _L( "ssidConn.Length() = %d" ), ssidConn.Length() );

                        aConnectionName.Copy( ssidConn );
                        }

                    wlanMgmtClient->CancelNotifications();
                    CleanupStack::PopAndDestroy( wlanMgmtClient );
                    }
                }

            CleanupStack::PopAndDestroy( wLanServiceTable );
            }
        else if ( err != KErrNotFound )
            {
            User::LeaveIfError( err );
            }
        }

#endif  // !__WINS__

    CLOG_WRITEF( _L( "connectionName = %S" ), &aConnectionName );
    }
    
    
// ---------------------------------------------------------    
// CConnectionUiUtilitiesImpl::OffLineWlanNote
// ---------------------------------------------------------
//
TBool CConnectionUiUtilitiesImpl::OffLineWlanNote()
    { 
    CLOG_ENTERFN( "CConnectionUiUtilitiesImpl::OffLineWlanNote" );    

    if ( iIsWlanSupported )
        {
        if ( !iActiveWrapper[EOffLineWlan] )
            {
            TRAP_IGNORE( iActiveWrapper[EOffLineWlan] = 
                                        CActiveWrapper::NewL( EOffLineWlan ) );
            CLOG_WRITE( "iActiveWrapper created" );
            }

        if ( iActiveWrapper[EOffLineWlan] )
            {
            CLOG_WRITE( "Starting generic note" );
            iActiveWrapper[EOffLineWlan]->StartGenericNote( EOffLineWlan );
            CLOG_WRITE( "Generic note started" );
            return iActiveWrapper[EOffLineWlan]->iStatus.Int() ? EFalse : 
                                                                 ETrue;
            }
        }
       
    CLOG_LEAVEFN( "CConnectionUiUtilitiesImpl::OffLineWlanNote" );

    return EFalse;
    }


// ---------------------------------------------------------    
// CConnectionUiUtilitiesImpl::OffLineWlanNote
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::OffLineWlanNote( TRequestStatus& aStatus )
    { 
    CLOG_ENTERFN( "CConnectionUiUtilitiesImpl::OffLineWlanNote Async" );    

    if ( iIsWlanSupported )
        {
        if ( iActiveWrapper[EOffLineWlan] )
            {   // deleting, to be sure to call the right Cancel function
            delete iActiveWrapper[EOffLineWlan];
            iActiveWrapper[EOffLineWlan] = NULL;
            }

        iNotif.OffLineWlanNote( aStatus, ETrue );
        }
       
    CLOG_LEAVEFN( "CConnectionUiUtilitiesImpl::OffLineWlanNote Async" );
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::CancelOffLineWlanNote
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::CancelOffLineWlanNote()
    {
    if ( iIsWlanSupported )    
        {
        if ( iActiveWrapper[EOffLineWlan] )
            {
            iActiveWrapper[EOffLineWlan]->Cancel();
            }
        else
            {   // Asynchronous version
            iNotif.CancelOffLineWlanNote();
            }
        }
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::OffLineWlanDisabledNote
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::OffLineWlanDisabledNote()
    {
    if ( iIsWlanSupported )    
        {
        if ( !iActiveWrapper[EWlanDisabled] )
            {
            TRAP_IGNORE( iActiveWrapper[EWlanDisabled] = 
                                    CActiveWrapper::NewL( EWlanDisabled ) );
            }

        if ( iActiveWrapper[EWlanDisabled] )
            {
            iActiveWrapper[EWlanDisabled]->StartGenericNote( EWlanDisabled );
            }
        }
    }

// ---------------------------------------------------------    
// CConnectionUiUtilitiesImpl::EasyWepDlg
// ---------------------------------------------------------
//
TBool CConnectionUiUtilitiesImpl::EasyWepDlg( TDes* aKey, TBool& aHex )
    {
    TInt status = KErrNone;
    
    if ( iIsWlanSupported )    
        {
        iWepKey().iKey.Copy( *aKey );
        if ( !iActiveWrapper[EWlanEasyWep] )
            {
            TRAP_IGNORE( iActiveWrapper[EWlanEasyWep] = 
                                    CActiveWrapper::NewL( EWlanEasyWep ) );
            }

        if ( iActiveWrapper[EWlanEasyWep] )
            {
            status = iActiveWrapper[EWlanEasyWep]->StartEasyWepDlg( iWepKey );
            } 
        aKey->Copy( iWepKey().iKey );
        aHex = iWepKey().iHex;        
        }
    return ( status == KErrNone ) ? ETrue : EFalse;
    }

// ---------------------------------------------------------    
// CConnectionUiUtilitiesImpl::EasyWepDlgAsync
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::EasyWepDlgAsync( TRequestStatus& aStatus, TDes* aKey, TBool& aHex )
    {
    if ( iIsWlanSupported )    
        {
        if ( !iActiveWrapper[EWlanEasyWep] )
            {
            TRAP_IGNORE( iActiveWrapper[EWlanEasyWep] = 
                                    CActiveWrapper::NewL( EWlanEasyWep ) );
            }

        if ( iActiveWrapper[EWlanEasyWep] )
            {
            iActiveWrapper[EWlanEasyWep]->StartEasyWepDlgAsync( aStatus, aKey, aHex );
            } 
        }
    }



// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::CancelEasyWepDlg
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::CancelEasyWepDlg()
    {
    if ( iIsWlanSupported )    
        {
        iActiveWrapper[EWlanEasyWep]->Cancel();      
        }
    }

// ---------------------------------------------------------    
// CConnectionUiUtilitiesImpl::EasyWpaDlg
// ---------------------------------------------------------
//
TBool CConnectionUiUtilitiesImpl::EasyWpaDlg( TDes* aKey )
    { 
    TInt status = KErrNone;
    if ( iIsWlanSupported )    
        {
        iWpaKey().Copy( *aKey );
        if ( !iActiveWrapper[EWlanEasyWpa] )
            {
            TRAP_IGNORE( iActiveWrapper[EWlanEasyWpa] = 
                                    CActiveWrapper::NewL( EWlanEasyWpa ) );
            }

        if ( iActiveWrapper[EWlanEasyWpa] )
            {
            status = iActiveWrapper[EWlanEasyWpa]->StartEasyWpaDlg( iWpaKey );
            }        
        aKey->Copy( iWpaKey() );        
        }
    return ( status == KErrNone ) ? ETrue : EFalse;
    }

// ---------------------------------------------------------    
// CConnectionUiUtilitiesImpl::StartEasyWpaDlgAsync
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::EasyWpaDlgAsync( TRequestStatus& aStatus, TDes* aKey )
    { 
    if ( iIsWlanSupported )    
        {
        if ( !iActiveWrapper[EWlanEasyWpa] )
            {
            TRAP_IGNORE( iActiveWrapper[EWlanEasyWpa] = 
                                    CActiveWrapper::NewL( EWlanEasyWpa ) );
            }

        if ( iActiveWrapper[EWlanEasyWpa] )
            {
            iActiveWrapper[EWlanEasyWpa]->StartEasyWpaDlgAsync( aStatus, aKey );
            }        
        }
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::CancelEasyWpaDlg
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::CancelEasyWpaDlg()
    {
    if ( iIsWlanSupported )    
        {
        iActiveWrapper[EWlanEasyWpa]->Cancel();         
        }
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::WLANNetworkUnavailableNote
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::WLANNetworkUnavailableNote()
    {
    if ( iIsWlanSupported )    
        {
        if ( !iActiveWrapper[EWlanNetwUnavail] )
            {
            TRAP_IGNORE( iActiveWrapper[EWlanNetwUnavail] = 
                                    CActiveWrapper::NewL( EWlanNetwUnavail ) );
            }

        if ( iActiveWrapper[EWlanNetwUnavail] )
            {
            iActiveWrapper[EWlanNetwUnavail]->StartGenericNote( 
                                                            EWlanNetwUnavail );
            }
        }
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::SearchWLANNetworkSync
// ---------------------------------------------------------
//
TBool CConnectionUiUtilitiesImpl::SearchWLANNetworkSync( TWlanSsid& aSSID,
                                TWlanConnectionMode& aConnectionMode,
                                TWlanConnectionSecurityMode& aSecurityMode )
    {
    return SearchWLANNetworkSync( aSSID, aConnectionMode, aSecurityMode, 
                                  iDummy );
    }


// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::SearchWLANNetworkSync
// ---------------------------------------------------------
//
TBool CConnectionUiUtilitiesImpl::SearchWLANNetworkSync( TWlanSsid& aSSID,
                                TWlanConnectionMode& aConnectionMode,
                                TWlanConnectionSecurityMode& aSecurityMode, 
                                TBool& aProtectedSetupSupported )
    {
    TBool result( EFalse );
    
    if ( iIsWlanSupported )
        {
        TRequestStatus status;
        iNetworkPrefs();

        iNotif.SearchWLANNetwork( iNetworkPrefs, status, EFalse );
        User::WaitForRequest( status ); 
        
        if( status == KErrNone )
            {
            aSSID = iNetworkPrefs().iSsId;
            aConnectionMode = iNetworkPrefs().iNetworkMode;
            aSecurityMode = iNetworkPrefs().iSecMode;
            aProtectedSetupSupported = 
                                    iNetworkPrefs().iProtectedSetupSupported;
            result = ETrue;
            }
        }    
    
    CLOG_WRITEF( _L( "SearchWLANNetworkSync result : %b" ),  result );
    return result;
    }


// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::SearchWLANNetworkAsync
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::SearchWLANNetworkAsync( 
                                TRequestStatus& aStatus,
                                TWlanSsid& aSSID,
                                TWlanConnectionMode& aConnectionMode,
                                TWlanConnectionSecurityMode& aSecurityMode )
    {
    SearchWLANNetworkAsync( aStatus, 
                            aSSID, 
                            aConnectionMode, 
                            aSecurityMode, 
                            iDummyExtSecMode, 
                            iDummy );
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::SearchWLANNetworkAsync
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::SearchWLANNetworkAsync( 
                                TRequestStatus& aStatus,
                                TWlanSsid& aSSID,
                                TWlanConnectionMode& aConnectionMode,
                                TWlanConnectionExtentedSecurityMode& aExtSecurityMode )
    {
    SearchWLANNetworkAsync( aStatus, 
                            aSSID, 
                            aConnectionMode, 
                            iDummySecMode, 
                            aExtSecurityMode, 
                            iDummy );
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::SearchWLANNetworkAsync
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::SearchWLANNetworkAsync( 
                                TRequestStatus& aStatus,
                                TWlanSsid& aSSID,
                                TWlanConnectionMode& aConnectionMode,
                                TWlanConnectionSecurityMode& aSecurityMode, 
                                TBool& aProtectedSetupSupported )
    {
    SearchWLANNetworkAsync( aStatus, 
                            aSSID, 
                            aConnectionMode, 
                            aSecurityMode, 
                            iDummyExtSecMode,
                            aProtectedSetupSupported );
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::SearchWLANNetworkAsync
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::SearchWLANNetworkAsync( 
                                TRequestStatus& aStatus,
                                TWlanSsid& aSSID,
                                TWlanConnectionMode& aConnectionMode,
                                TWlanConnectionExtentedSecurityMode& aExtSecurityMode, 
                                TBool& aProtectedSetupSupported )
    {
    SearchWLANNetworkAsync( aStatus, 
                            aSSID, 
                            aConnectionMode, 
                            iDummySecMode, 
                            aExtSecurityMode, 
                            aProtectedSetupSupported );
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::SearchWLANNetworkAsync
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::SearchWLANNetworkAsync( 
                                TRequestStatus& aStatus,
                                TWlanSsid& aSSID,
                                TWlanConnectionMode& aConnectionMode,
                                TWlanConnectionSecurityMode& aSecurityMode,
                                TWlanConnectionExtentedSecurityMode& aExtSecurityMode, 
                                TBool& aProtectedSetupSupported )
    {
    CLOG_ENTERFN( "CConnectionUiUtilitiesImpl::SearchWLANNetworkAsync" );    

    if ( iIsWlanSupported )
        {
        if ( !iActiveWrapper[ESearchWlan] )
            {
            TRAP_IGNORE( iActiveWrapper[ESearchWlan] = 
                                        CActiveWrapper::NewL( ESearchWlan ) );
            }

        if ( iActiveWrapper[ESearchWlan] )
            {
            iActiveWrapper[ESearchWlan]->StartSearchWLANNetworkAsync( aStatus,
                                    aSSID, aConnectionMode, aSecurityMode,
                                    aExtSecurityMode, 
                                    aProtectedSetupSupported );

            CLOG_WRITEF( _L( "iActiveWrapper[ESearchWlan]->iStatus.Int(): %d" ),  
                           iActiveWrapper[ESearchWlan]->iStatus.Int() );
            }
        }    

    CLOG_LEAVEFN( "CConnectionUiUtilitiesImpl::SearchWLANNetworkAsync" );    
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::ConnectedViaDestAndConnMethodNote
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::ConnectedViaDestAndConnMethodNote( 
                                                const TUint32 aDestId, 
                                                const TUint32 aConnMId )
    {
    if ( !iActiveWrapper[EConnViaDestCM] )
        {
        TRAP_IGNORE( iActiveWrapper[EConnViaDestCM] = 
                                    CActiveWrapper::NewL( EConnViaDestCM ) );
        }

    if ( iActiveWrapper[EConnViaDestCM] )
        {
        iActiveWrapper[EConnViaDestCM]->StartGenericNote( EConnViaDestCM, 
                                                          aConnMId, aDestId );
        }
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::ConnectedViaDestAndConnMethodNote
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::ConnectedViaDestAndConnMethodNote( 
                                                const TUint32 aDestId, 
                                                const TUint32 aConnMId, 
                                                TRequestStatus& aStatus )
    {
    iNotif.ConnectedViaDestAndConnMethodNote( aDestId, aConnMId, aStatus );
    }
        
// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::CancelConnectedViaDestAndConnMethodNote
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::CancelConnectedViaDestAndConnMethodNote()
    {
    iNotif.CancelConnectedViaDestAndConnMethodNote();
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::ChangingConnectionToNote
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::ChangingConnectionToNote( 
                                                const TUint32 aConnMId )
    {
    if ( !iActiveWrapper[EChangingConnTo] )
        {
        TRAP_IGNORE( iActiveWrapper[EChangingConnTo] = 
                                    CActiveWrapper::NewL( EChangingConnTo ) );
        }

    if ( iActiveWrapper[EChangingConnTo] )
        {
        iActiveWrapper[EChangingConnTo]->StartGenericNote( EChangingConnTo,
                                                           aConnMId );
        }
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::ChangingConnectionToNote
// ---------------------------------------------------------
//    
void CConnectionUiUtilitiesImpl::ChangingConnectionToNote( 
                                                const TUint32 aConnMId, 
                                                TRequestStatus& aStatus )
    {
    iNotif.ChangingConnectionToNote( aConnMId, aStatus );  
    }


// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::CancelChangingConnectionToNote
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::CancelChangingConnectionToNote()
    {
    iActiveWrapper[EChangingConnTo]->Cancel();
    }

// ---------------------------------------------------------    
// CConnectionUiUtilitiesImpl::ConfirmMethodUsageQuery
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::ConfirmMethodUsageQuery( 
                                               TMsgQueryLinkedResults& aResult,
                                               const TBool aHomeNetwork,
                                               TRequestStatus& aStatus )
    { 
    iNotif.ConfirmMethodUsageQuery( aResult, aHomeNetwork, aStatus );
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::CancelConfirmMethodUsageQuery
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::CancelConfirmMethodUsageQuery()
    {
    iNotif.CancelConfirmMethodUsageQuery();
    }


// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::ConnectedViaConnMethodNote
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::ConnectedViaConnMethodNote( 
                                                    const TUint32 aConnMId )
    {
    if ( !iActiveWrapper[EConnViaCM] )
        {
        TRAP_IGNORE( iActiveWrapper[EConnViaCM] = 
                                        CActiveWrapper::NewL( EConnViaCM ) );
        }

    if ( iActiveWrapper[EConnViaCM] )
        {
        iActiveWrapper[EConnViaCM]->StartGenericNote( EConnViaCM, aConnMId );
        }
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::ConnectedViaConnMethodNote
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::ConnectedViaConnMethodNote( 
                                                    const TUint32 aConnMId,
                                                    TRequestStatus& aStatus )
    {
    iNotif.ConnectedViaConnMethodNote( aConnMId, aStatus );
    }

        
// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::CancelConnectedViaConnMethodNote
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::CancelConnectedViaConnMethodNote()
    {
    iNotif.CancelConnectedViaConnMethodNote();
    }
    
    
// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::WlanPowerSaveTestNote
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::WlanPowerSaveTestNote( 
                                                    TBool&          aDisable,
                                                    TRequestStatus& aStatus )
    {
    iNotif.WlanPowerSaveTestNote( aDisable, aStatus );
    }

        
// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::CancelWlanPowerSaveTestNote
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::CancelWlanPowerSaveTestNote()
    {
    iNotif.CancelWlanPowerSaveTestNote();
    }
    
// ---------------------------------------------------------    
// CConnectionUiUtilitiesImpl::EasyWapiDlg
// ---------------------------------------------------------
//
TBool CConnectionUiUtilitiesImpl::EasyWapiDlg( TDes* aKey )
    { 
    TInt status( KErrNone );
    
    if ( iIsWlanSupported )    
        {
        iWapiKey().Copy( *aKey );
        
        if ( !iActiveWrapper[ EWlanEasyWapi ] )
            {
            TRAP_IGNORE( iActiveWrapper[ EWlanEasyWapi ] = 
                                    CActiveWrapper::NewL( EWlanEasyWapi ) );
            }

        if ( iActiveWrapper[ EWlanEasyWapi ] )
            {
            status = iActiveWrapper[ EWlanEasyWapi ]->StartEasyWapiDlg( iWapiKey );
            }
                    
        aKey->Copy( iWapiKey() );        
        }
    return ( status == KErrNone ) ? ETrue : EFalse;
    }

// ---------------------------------------------------------
// CConnectionUiUtilitiesImpl::NoWLANNetworksAvailableNote
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::NoWLANNetworksAvailableNote()
    {
    if ( iIsWlanSupported )    
        {
        if ( !iActiveWrapper[ ENoWlanNetwsAvail ] )
            {
            TRAP_IGNORE( iActiveWrapper[ ENoWlanNetwsAvail ] = 
                                    CActiveWrapper::NewL( ENoWlanNetwsAvail ) );
            }

        if ( iActiveWrapper[ ENoWlanNetwsAvail ] )
            {
            iActiveWrapper[ ENoWlanNetwsAvail ]->StartGenericNote( 
                                                            ENoWlanNetwsAvail );
            }
        }
    }


// ---------------------------------------------------------    
// CConnectionUiUtilitiesImpl::ConnectingViaDiscreetPopup
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::ConnectingViaDiscreetPopup( const TUint32& aIapId, TBool aConnectionAlreadyActive )
    { 
    iConnInfo().iIapId = aIapId;
    iConnInfo().iConnectionAlreadyActive = aConnectionAlreadyActive;
    
    if ( !iActiveWrapper[ EConnectingViaDiscreetPopup ] )
        {
        TRAP_IGNORE( iActiveWrapper[ EConnectingViaDiscreetPopup ] = 
                                CActiveWrapper::NewL( EConnectingViaDiscreetPopup ) );
        }

    if ( iActiveWrapper[ EConnectingViaDiscreetPopup ] )
        {
        iActiveWrapper[ EConnectingViaDiscreetPopup ]
                        ->StartConnectingViaDiscreetPopup( iConnInfo );
        }
    }



// ---------------------------------------------------------    
// CConnectionUiUtilitiesImpl::ConnectionErrorDiscreetPopup
// ---------------------------------------------------------
//
void CConnectionUiUtilitiesImpl::ConnectionErrorDiscreetPopup( const TInt& aErrCode )
    {
    TPckgBuf< TInt > buf;
    buf() = aErrCode;
    
    if ( !iActiveWrapper[ EConnectionErrorDiscreetPopup ] )
        {
        TRAP_IGNORE( iActiveWrapper[ EConnectionErrorDiscreetPopup ] = 
                                CActiveWrapper::NewL( EConnectionErrorDiscreetPopup ) );
        }

    if ( iActiveWrapper[ EConnectionErrorDiscreetPopup ] )
        {
        iActiveWrapper[ EConnectionErrorDiscreetPopup ]
                        ->StartConnectionErrorDiscreetPopup( buf );
        }
    }
        
// End of File
