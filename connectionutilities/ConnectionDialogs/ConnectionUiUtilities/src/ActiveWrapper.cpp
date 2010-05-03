/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
*    CActiveWrapper implementation file
*
*
*/


#include <e32base.h>

#include "ActiveWrapper.h"
#include "ConnectionDialogsLogger.h"

// LOCAL CONSTANTS AND MACROS

// ---------------------------------------------------------
// CActiveWrapper::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CActiveWrapper* CActiveWrapper::NewL( const TUint aIndexOfNote )
    {
    CLOG_ENTERFN( "CActiveWrapper::NewL()" );
    CActiveWrapper* self = new ( ELeave ) CActiveWrapper( aIndexOfNote );
    CleanupStack::PushL( self );

    self->ConstructL();
    
    CleanupStack::Pop( self );
    CLOG_LEAVEFN( "CActiveWrapper::NewL()" );
    
    return self;
    }

// ---------------------------------------------------------
// CActiveWrapper::ConstructL
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CActiveWrapper::ConstructL()
    {
    CLOG_ENTERFN( "CActiveWrapper::ConstructL()" );

    User::LeaveIfError( iNotif.Connect() );
    CActiveScheduler::Add( this );

    CLOG_LEAVEFN( "CActiveWrapper::ConstructL()" );
    }


// ---------------------------------------------------------
// CActiveWrapper::CActiveWrapper
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CActiveWrapper::CActiveWrapper( const TUint aIndexOfNote ) 
: CActive( CActive::EPriorityStandard ),
  iIndexOfNote( aIndexOfNote )
    {
    }


// ---------------------------------------------------------
// CActiveWrapper::~CActiveWrapper
//
// Destructor
// ---------------------------------------------------------
//
CActiveWrapper::~CActiveWrapper()
    {
    CLOG_ENTERFN( "CActiveWrapper::~CActiveWrapper()" );

    Cancel();

    CLOG_WRITE( "CActiveWrapper::~CActiveWrapper Canceled" );

    iNotif.Close();

    CLOG_LEAVEFN( "CActiveWrapper::~CActiveWrapper()" );
    }


// ---------------------------------------------------------
// CActiveWrapper::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CActiveWrapper::RunL() 
    {
    CLOG_ENTERFN( "CActiveWrapper::RunL()" );            

    CLOG_WRITEF( _L( "Completing request with %d" ), iStatus.Int() );

    if ( iWait.IsStarted() )
        {
        CLOG_WRITEF( _L( "AsyncStop" ) );

        iWait.AsyncStop();
        }
    else 
        {
        if( ( (iStatus == KErrNone) || ( iStatus == KErrNotFound ) ) )
            {
            switch ( iIndexOfNote )    
                {
                case ESearchWlan :
                    {
                    *iSSID = iNetworkPrefs().iSsId;
                    *iConnectionMode = iNetworkPrefs().iNetworkMode;
                    *iSecurityMode = iNetworkPrefs().iSecMode;
                    *iExtSecurityMode = iNetworkPrefs().iExtSecMode;
                    *iProtectedSetupSupported = 
                                            iNetworkPrefs().iProtectedSetupSupported;

                    HBufC* ssid16 = HBufC::NewLC( iNetworkPrefs().iSsId.Length() );
                    ssid16->Des().Copy( *iSSID ); 
                    CLOG_WRITEF( _L( "iSSID : %S" ), ssid16 );    
                    CleanupStack::PopAndDestroy( ssid16 );

                    CLOG_WRITEF( _L( "iConnectionMode : %d" ),  *iConnectionMode );    
                    CLOG_WRITEF( _L( "iSecurityMode : %d" ),  *iSecurityMode );
                    CLOG_WRITEF( _L( "iExtSecurityMode : %d" ),  *iExtSecurityMode );
                    break;
                    }
                case EWlanEasyWep :
                    {
                    iKey->Copy( iWepKey().iKey );
                    *iHex = iWepKey().iHex;        
                    break;
                    }
                case EWlanEasyWpa :
                    {
                    iKey->Copy( iWpaKey() );
                    break;
                    }
                default:
                    {
                    }
                }
            }

        if ( iRS )
            {
            TRequestStatus* pS = iRS;
            User::RequestComplete( pS, iStatus.Int() );
            iRS = NULL;
            }
        }
    
    CLOG_LEAVEFN( "CActiveWrapper::RunL()" );                
    }


// ---------------------------------------------------------
// CActiveWrapper::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CActiveWrapper::DoCancel()
    {
    CLOG_ENTERFN( "CActiveWrapper::DoCancel()" );

    switch( iIndexOfNote )
        {
        case ESearchWlan:
            {
            iNotif.CancelSearchWLANNetwork();
            
            if ( iRS )
                {
                TRequestStatus* pS = iRS;
                User::RequestComplete( pS, KErrCancel );	
                }
            
            break;
            }

        case EOffLineWlan:
            {
            iNotif.CancelOffLineWlanNote();
            break;
            }

        case EWlanEasyWep:
            {
            iNotif.CancelEasyWepDlg();
            
            if ( iRS )
                {
                TRequestStatus* pS = iRS;
                User::RequestComplete( pS, KErrCancel );	
                }
                        
            break;
            }
        case EWlanEasyWpa:
            {
            iNotif.CancelEasyWpaDlg();
            
            if ( iRS )
                {
                TRequestStatus* pS = iRS;
                User::RequestComplete( pS, KErrCancel );	
                }
                        
            break;
            }
        default:
            {
		    break;
            }
        }

    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }

    CLOG_LEAVEFN( "CActiveWrapper::DoCancel()" );
    }


// ---------------------------------------------------------
// CActiveWrapper::StartSearchWLANNetwork
//
// Starts the active object
// ---------------------------------------------------------
//
void CActiveWrapper::StartSearchWLANNetwork( 
                        TPckgBuf<TConnUiUiWlanNetworkPrefs>& aNetworkPrefs )
    {
    CLOG_ENTERFN( "CActiveWrapper::StartSearchWLANNetwork()" );
    
    if ( IsActive() == EFalse )
        {
        CLOG_WRITE( "NOT active" );
                        
        iNotif.SearchWLANNetwork( aNetworkPrefs, iStatus, EFalse );
        SetActive();
        iWait.Start();
        }
    else
        {
        CLOG_WRITE( "Already active" );
        }
                        
    CLOG_LEAVEFN( "CActiveWrapper::StartSearchWLANNetwork()" );
    }


// ---------------------------------------------------------
// CActiveWrapper::StartSearchWLANNetworkAsync
//
// Starts the active object
// ---------------------------------------------------------
//
void CActiveWrapper::StartSearchWLANNetworkAsync( TRequestStatus& aStatus, 
                                TWlanSsid& aSSID,
                                TWlanConnectionMode& aConnectionMode,
                                TWlanConnectionSecurityMode& aSecurityMode,
                                TWlanConnectionExtentedSecurityMode& aExtSecurityMode, 
                                TBool& aProtectedSetupSupported )
    {
    CLOG_ENTERFN( "CActiveWrapper::StartSearchWLANNetworkAsync()" );
    
    if ( IsActive() == EFalse )
        {
        CLOG_WRITE( "NOT active" );

        iSSID = &aSSID;
        iConnectionMode = &aConnectionMode;
        iSecurityMode = &aSecurityMode;
        iExtSecurityMode = &aExtSecurityMode;
        iProtectedSetupSupported = &aProtectedSetupSupported;
        iNetworkPrefs();

        iRS = &aStatus;
        *iRS = KRequestPending;

        SetActive();

        iNotif.SearchWLANNetwork( iNetworkPrefs, iStatus, ETrue );
        }
    else
        {
        CLOG_WRITE( "Already active" );
        }
                        
    CLOG_LEAVEFN( "CActiveWrapper::StartSearchWLANNetworkAsync()" );
    }
// ---------------------------------------------------------
// CActiveWrapper::StartEasyWepDlg
//
// Starts the active object
// ---------------------------------------------------------
//
TInt CActiveWrapper::StartEasyWepDlg( TPckgBuf< TWepKeyData >& aKey )
    {
    if ( IsActive() == EFalse )
        {
        iIndexOfNote = EWlanEasyWep;
        iNotif.EasyWepDlg( aKey, iStatus );
        SetActive();
        iWait.Start();
        }
    return iStatus.Int();  
    }

// ---------------------------------------------------------
// CActiveWrapper::StartEasyWepDlgAsync
//
// Starts the active object
// ---------------------------------------------------------
//
void CActiveWrapper::StartEasyWepDlgAsync( TRequestStatus& aStatus, TDes* aKey, TBool& aHex )
    {
    if ( IsActive() == EFalse )
        {
        iKey = aKey;
        iHex = &aHex;
        iWepKey();
        iWepKey().iKey.Copy( *aKey );
        iIndexOfNote = EWlanEasyWep;
        iNotif.EasyWepDlg( iWepKey, iStatus );
        iRS = &aStatus;
        *iRS = KRequestPending;        
        SetActive();
        }
    }


// ---------------------------------------------------------
// CActiveWrapper::StartEasyWpaDlg
//
// Starts the active object
// ---------------------------------------------------------
//
TInt CActiveWrapper::StartEasyWpaDlg( TPckgBuf< TBuf< KEasyWpaQueryMaxLength > >& aKey )
    {
    if ( IsActive() == EFalse )
        {
        iIndexOfNote = EWlanEasyWpa;
        iNotif.EasyWpaDlg( aKey, iStatus );
        SetActive();
        iWait.Start();
        }
    return iStatus.Int();  
    }

// ---------------------------------------------------------
// CActiveWrapper::StartEasyWpaDlgAsync
//
// Starts the active object
// ---------------------------------------------------------
//
void CActiveWrapper::StartEasyWpaDlgAsync( TRequestStatus& aStatus, TDes* aKey )
    {
    if ( IsActive() == EFalse )
        {
        iKey = aKey;
        iWpaKey();
        iWpaKey().Copy( *aKey );
        iIndexOfNote = EWlanEasyWpa;
        iNotif.EasyWpaDlg( iWpaKey, iStatus );
        iRS = &aStatus;
        *iRS = KRequestPending;        
        SetActive();
        }
    }

// ---------------------------------------------------------
// CActiveWrapper::StartGenericNote
//
// Starts the active object
// ---------------------------------------------------------
//
void CActiveWrapper::StartGenericNote( const TUint aIndexOfNote, 
                                       const TUint32 aConnMId /* = 0 */,
                                       const TUint32 aDestId /* = 0 */ )
    {
    // Do not use logs here it cause crash in MPM server

    if ( IsActive() == EFalse )
        {
        switch ( aIndexOfNote )
            {
            case EOffLineWlan:
                {
                iNotif.OffLineWlanNote( iStatus, EFalse );
                break;
                }

            case EWlanDisabled:
                {
                iNotif.OffLineWlanDisabledNote( iStatus );
                break;
                }

            case EWlanNetwUnavail:
                {
                iNotif.WLANNetworkUnavailableNote( iStatus );
                break;
                }

            default:
                {
                return;
                }
            }

        iIndexOfNote = aIndexOfNote;
        SetActive();
        iWait.Start();
        }
    else
        {
        }
    }

// ---------------------------------------------------------
// CActiveWrapper::StartEasyWapiDlg
//
// Starts the active object
// ---------------------------------------------------------
//
TInt CActiveWrapper::StartEasyWapiDlg( TPckgBuf< TBuf< KEasyWapiQueryMaxLength > >& aKey )
    {
    if ( IsActive() == EFalse )
        {
        iIndexOfNote = EWlanEasyWapi;
        iNotif.EasyWapiDlg( aKey, iStatus );
        SetActive();
        iWait.Start();
        }

    return iStatus.Int();  
    }

// End of File
