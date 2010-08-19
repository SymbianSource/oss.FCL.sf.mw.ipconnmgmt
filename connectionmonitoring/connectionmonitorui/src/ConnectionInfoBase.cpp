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
#include <stdlib.h>
#include <nifvar.h>
#include <StringLoader.h>
#include <ConnectionMonitorUi.rsg>
#include <avkon.rsg>
#include <AknUtils.h>
#include <apgcli.h>

#include "ConnectionMonitorUiLogger.h"
#include "ConnectionInfoBase.h"
#include "ActiveWrapper.h"

// CONSTANTS

const TUint KUpArrowChar = 0x2191; ///< ASCII code of UpArrow
const TUint KDownArrowChar = 0x2193; ///< ASCII code of DownArrow

#ifndef _DEBUG
const TUint KHiddenDhcpServerUid = 0x101fd9c5; ///< Hidden DHCP server UID (dhcpserv.exe)
const TUint KHiddenDnsServerUid = 0x10000882; ///< Hidden DNS server UID (dnd.exe)
#endif

const TInt KFeedsServerUid         = 0x1020728E;
const TInt KDownloadMgrServerUid   = 0x10008D60;
const TInt KMessagingServerUid     = 0x1000484b;
const TInt KJavaVMUid              = 0x102033E6;
const TInt KSUPLServerUid          = 0x102073CA;

LOCAL_D const TInt KStringsGranularity = 5;

_LIT( KMrouterName, "mRouter"); ///< see CheckMrouteIap
_LIT( KEmpty, "");      ///< Empty string




// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CConnectionInfoBase::~CConnectionInfoBase
// ---------------------------------------------------------
//
CConnectionInfoBase::~CConnectionInfoBase()
    {
    delete iDetailsArray;
    delete iAppNames;
    }


// ---------------------------------------------------------
// CConnectionInfoBase::CConnectionInfoBase
// ---------------------------------------------------------
//
CConnectionInfoBase::CConnectionInfoBase(
                           TInt aConnectionId,
                           RConnectionMonitor* const aConnectionMonitor,
                           TConnMonBearerType aConnectionBearerType,
                           CActiveWrapper* aActiveWrapper ) :
    iConnectionMonitor( aConnectionMonitor ), 
    iStartTime( NULL ),
    iLastSpeedUpdate( 0 ),
    iDeletedFromCMUI( EFalse ),
    iActiveWrapper( aActiveWrapper )
    {
    CMUILOGGER_WRITE( "CConnectionInfoBase constuctor - start " );
    iConnectionStatus = EConnectionUninitialized;
    iConnectionId = aConnectionId;
    iConnectionBearerType = aConnectionBearerType;
    iClientBuf().iCount = 0;
    CMUILOGGER_WRITE( "CConnectionInfoBase constuctor - end " );
    }


// ---------------------------------------------------------
// CConnectionInfoBase::ConstructL
// ---------------------------------------------------------
//
void CConnectionInfoBase::ConstructL()
    {
    CMUILOGGER_ENTERFN( "CConnectionInfoBase::ConstructL() start" );

    if ( iConnectionBearerType >= EBearerExternalCSD )
        {
        StringLoader::Load( iConnectionName,
                            R_QTN_CMON_ITEM_CONN_NAME_MODEM );
        }
    else
        {
        iActiveWrapper->StartGetStringAttribute( iConnectionId,
                                                 iConnectionMonitor,
                                                 KIAPName,
                                                 iConnectionName );
        TInt err( iActiveWrapper->iStatus.Int() );
        CMUILOGGER_WRITE_F( "CConnectionInfoBase::ConstructL() status: %d",
                            err );
        CMUILOGGER_WRITE_F( "iConnectionName : %S", &iConnectionName );
        CMUILOGGER_WRITE_F( "iConnectionId : %d", iConnectionId );
        CMUILOGGER_WRITE_F( "iConnectionBearerType : %d", 
                            ( TInt )iConnectionBearerType );
        if ( err != KErrNone )
            {
            User::Leave( err );
            }
        }
    
    if ( !iAppNames )
        {
        iAppNames = new( ELeave ) CDesCArrayFlat( KStringsGranularity );
        }

    StatusChangedL();
    if ( GetStatus() == EConnectionSuspended )
        {
        InitializeConnectionInfoL();
        }
    CMUILOGGER_LEAVEFN( "CConnectionInfoBase::ConstructL() end" );
    }


// ---------------------------------------------------------
// CConnectionInfoBase::StatusChangedL
// ---------------------------------------------------------
//
void CConnectionInfoBase::StatusChangedL()
    {
    CMUILOGGER_WRITE( "CConnectionInfoBase::StatusChangedL() start" );

    RefreshConnectionStatus();

    switch ( iConnectionStatus )
        {
        case EConnectionCreated:
            {
            InitializeConnectionInfoL();
            break;
            }
        case EConnectionSuspended:
            {
            RefreshDetailsL();
            if ( !iDetailsArray )
                {
                ToArrayDetailsL();
                }
            RefreshDetailsArrayL();
            break;
            }
        case EConnectionClosing:
        case EConnectionClosed:
        case EConnectionUninitialized:
        	{
        	if ( iDetailsArray ) // if there is details array
        	    {
        	    RefreshDetailsArrayL();
        	    }
            break;
        	}
        case EConnectionCreating:
        default :
            {
            break;
            }
        }

    RefreshConnectionListBoxItemTextL();

    CMUILOGGER_WRITE( "CConnectionInfoBase::StatusChangedL() end" );
    }

// ---------------------------------------------------------
// CConnectionInfoBase::IsAlive
// ---------------------------------------------------------
//
TBool CConnectionInfoBase::IsAlive() const
    {
    TBool alive( EFalse );
    if ( ( iConnectionStatus == EConnectionSuspended ) ||
         ( iConnectionStatus == EConnectionCreated ) )
        {
        alive = ETrue;
        }
    return alive;
    }
    
// ---------------------------------------------------------
// CConnectionInfoBase::IsSuspended
// ---------------------------------------------------------
//
TBool CConnectionInfoBase::IsSuspended() const
    {
    TBool suspended( EFalse );
    if ( iConnectionStatus == EConnectionSuspended )
        {
        suspended = ETrue;
        }
    return suspended;
    }    

// ---------------------------------------------------------
// CConnectionInfoBase::GetConnectionId
// ---------------------------------------------------------
//
CDesCArrayFlat* CConnectionInfoBase::GetDetailsArray() const
    {
    return iDetailsArray;
    }

// ---------------------------------------------------------
// CConnectionInfoBase::GetConnectionId
// ---------------------------------------------------------
//
TUint CConnectionInfoBase::GetConnectionId() const
    {
    return iConnectionId;
    }

// ---------------------------------------------------------
// CConnectionInfoBase::GetStatus
// ---------------------------------------------------------
//
TConnectionStatus CConnectionInfoBase::GetStatus() const
    {
    return iConnectionStatus;
    }

// ---------------------------------------------------------
// CConnectionInfoBase::GetName
// ---------------------------------------------------------
//
HBufC* CConnectionInfoBase::GetNameLC() const
    {
    HBufC* name = iConnectionName.AllocLC();
    return name;
    }

// ---------------------------------------------------------
// CConnectionInfoBase::GetBearerType
// ---------------------------------------------------------
//
TConnMonBearerType CConnectionInfoBase::GetBearerType() const
    {
    return iConnectionBearerType;
    }


// ---------------------------------------------------------
// CConnectionInfoBase::GetConnectionListBoxItemText
// ---------------------------------------------------------
//
TPtrC CConnectionInfoBase::GetConnectionListBoxItemText() const
    {
    return iListBoxItemText;
    }

// ---------------------------------------------------------
// CConnectionInfoBase::RefreshConnectionStatus
// ---------------------------------------------------------
//
void CConnectionInfoBase::RefreshConnectionStatus()
    {
    TConnectionStatus connStatus( EConnectionUninitialized );
    iActiveWrapper->StartGetIntAttribute( iConnectionId,
                                          iConnectionMonitor,
                                          KConnectionStatus,
                                          ( ( TInt& )( connStatus ) ) );
    TInt err( iActiveWrapper->iStatus.Int() );
    CMUILOGGER_WRITE_F( "RefreshConnectionStatus status : %d",
                        err );
    if ( err == KErrNone )
        {
        RefreshConnectionStatus( connStatus );
        }
    else
        {
        iActiveWrapper->Cancel();
        }
    }

// ---------------------------------------------------------
// CConnectionInfoBase::RefreshConnectionStatus
// ---------------------------------------------------------
//
void CConnectionInfoBase::RefreshConnectionStatus(
                                   const TInt aConnectionStatus )
    {
    CMUILOGGER_ENTERFN( "RefreshConnectionStatus" );
    CMUILOGGER_WRITE_F( "RefreshConnectionStatus aConnectionStatus %d",
                        ( TInt )aConnectionStatus );
    CMUILOGGER_WRITE_F( "iConnectionStatus before %d",
                        ( TInt )iConnectionStatus );
                        

    if ( ( aConnectionStatus == KLinkLayerOpen ) ||
          ( ( aConnectionStatus == KConnectionOpen ) &&
            ( iConnectionBearerType >= EBearerExternalCSD ) ) )
        {
        CMUILOGGER_WRITE("1");
        iConnectionStatus = EConnectionCreated;
        }
    else if ( ( aConnectionStatus == KConnectionClosed ) ||
              ( aConnectionStatus == KLinkLayerClosed ) ) 
        {
        CMUILOGGER_WRITE("2");        
        iConnectionStatus = EConnectionClosed;
        }
    else if ( ( ( aConnectionStatus >= KMinAgtProgress ) &&
               ( aConnectionStatus <= KConnectionOpen ) ) ||
               ( aConnectionStatus == KStartingSelection ) ||
               ( aConnectionStatus == KFinishedSelection ) ||
               ( aConnectionStatus == KConfigDaemonStartingRegistration ) ||
               ( aConnectionStatus == KConfigDaemonFinishedRegistration ) )
        // creating
        {
        CMUILOGGER_WRITE("5");
        iConnectionStatus = EConnectionCreating;
        }
    else if ( aConnectionStatus == KDataTransferTemporarilyBlocked )
        {
        CMUILOGGER_WRITE("6");
        iConnectionStatus = EConnectionSuspended;
        }
    else if ( ( iConnectionStatus == EConnectionCreated ) &&
              ( ( aConnectionStatus >= KConnectionStartingClose ) &&
                ( aConnectionStatus < KConnectionClosed ) ||  
                ( aConnectionStatus == KConfigDaemonFinishedDeregistrationStop ) || 
                ( aConnectionStatus == KConfigDaemonStartingDeregistration ) ||
                ( aConnectionStatus == KLinkLayerClosed ) ||
                ( aConnectionStatus == KConnectionUninitialised )
              ) )
        // closing
        {
        CMUILOGGER_WRITE("7");        
        iConnectionStatus = EConnectionClosing;
        }
    else if ( ( aConnectionStatus == KConnectionUninitialised ) &&
             ( iConnectionStatus == EConnectionSuspended ) )
        { // Back to created from suspended
        CMUILOGGER_WRITE("4");
        iConnectionStatus = EConnectionCreated;
        }
    else if ( aConnectionStatus == KConnectionUninitialised )
        {
        CMUILOGGER_WRITE("3");
        iConnectionStatus = EConnectionUninitialized;
        }
        
    else
        {
        CMUILOGGER_WRITE_F( " else iConnectionStatus  %d",
                            ( TInt )iConnectionStatus );
        CMUILOGGER_WRITE_F( "else aConnectionStatus  %d",
                            ( TInt )aConnectionStatus );
        
        CMUILOGGER_WRITE("else");
        }


    CMUILOGGER_WRITE_F( "iConnectionStatus after %d",
                        ( TInt )iConnectionStatus );
    CMUILOGGER_LEAVEFN( "RefreshConnectionStatus" );                                                
    }


// ---------------------------------------------------------
// CConnectionInfoBase::RefreshSentReceivedDataL
// ---------------------------------------------------------
//
void CConnectionInfoBase::RefreshSentReceivedDataL()
    {
    CMUILOGGER_ENTERFN( "CConnectionInfoBase::RefreshSentReceivedDataL" );
    // Downloaded
	CMUILOGGER_WRITE("Downloaded before");
    iActiveWrapper->StartGetUintAttribute( iConnectionId,
                                           iConnectionMonitor,
                                           KDownlinkData,
                                           iDownloaded );
    TInt errDownload( iActiveWrapper->iStatus.Int() );
    CMUILOGGER_WRITE_F( "Download status: %d", errDownload );
    if ( errDownload != KErrNone )
        {
        iActiveWrapper->Cancel();
        User::Leave( errDownload );
        }
   	CMUILOGGER_WRITE("Downloaded after");
    // Uploaded
	CMUILOGGER_WRITE("Uploaded before");
    iActiveWrapper->StartGetUintAttribute( iConnectionId,
                                           iConnectionMonitor,
                                           KUplinkData,
                                           iUploaded );
    TInt errUpload( iActiveWrapper->iStatus.Int() );
    CMUILOGGER_WRITE_F( "Upload status: %d", errUpload );
    if ( errUpload != KErrNone )
        {
        iActiveWrapper->Cancel();
        User::Leave( errUpload );
        }
	CMUILOGGER_WRITE("Uploaded after");

    CMUILOGGER_LEAVEFN( "CConnectionInfoBase::RefreshSentReceivedDataL" );
    }


// ---------------------------------------------------------
// CConnectionInfoBase::RefreshTransferSpeedsL
// ---------------------------------------------------------
//
void CConnectionInfoBase::RefreshTransferSpeedsL()
    {
    CMUILOGGER_ENTERFN( "CConnectionInfoBase::RefreshTransferSpeedsL" );
    const TInt KUpdatingIntervalInMillisec = 500;
    const TInt KStandardKilo = 1000;
    const TInt KDataKilo = 1024;
    
    TUint up = iUploaded;
    TUint down = iDownloaded;

    RefreshSentReceivedDataL();

    TTime now;
    now.UniversalTime();
    
    // calculate time difference in milliseconds
    TInt diffTime = I64INT( now.MicroSecondsFrom( iLastSpeedUpdate ).Int64() / 
            TInt64( KStandardKilo ) );

    if ( iLastSpeedUpdate.Int64() == 0 )
        {
        iLastSpeedUpdate = now;

        // speed cannot be calculated at the first time
        iUpSpeed.quot = 0;
        iUpSpeed.rem = 0;
        iDownSpeed.quot = 0;
        iDownSpeed.rem = 0;
        }
    else if ( diffTime > KUpdatingIntervalInMillisec )         
        {
        // bytes/millisec
        TInt upSpeed = ( iUploaded - up ) / diffTime;
        TInt downSpeed = ( iDownloaded - down ) / diffTime;
        
        // bytes/sec       
        upSpeed *= KStandardKilo;
        downSpeed *= KStandardKilo;
    
        // kbytes/sec
        iUpSpeed = div( upSpeed, KDataKilo );
        iDownSpeed = div( downSpeed, KDataKilo );

        iLastSpeedUpdate = now;
        }
    
    CMUILOGGER_LEAVEFN( "CConnectionInfoBase::RefreshTransferSpeedsL" );
    }


// ---------------------------------------------------------
// CConnectionInfoBase::RefreshDuration
// ---------------------------------------------------------
//
void CConnectionInfoBase::RefreshDuration()
    {
    CMUILOGGER_ENTERFN( "CConnectionInfoBase::RefreshDuration" );
    TTime now;
    now.UniversalTime();

    // passed time since connection established.
    iDuration = now.MicroSecondsFrom( iStartTime ).Int64();
    CMUILOGGER_LEAVEFN( "CConnectionInfoBase::RefreshDuration" );
    }


// ---------------------------------------------------------
// CConnectionInfoBase::RefreshAppNamesL
// ---------------------------------------------------------
//
TInt CConnectionInfoBase::RefreshAppNamesL()
    {
    CMUILOGGER_ENTERFN( "CConnectionInfoBase::RefreshAppNamesL" );

    TInt sharings( 0 );

    if ( IsAlive() )
        {
        if ( iAppNames )
            {
            // Check whether it is an internal or external (modem) connection
            // External (modem) connections does not need application name
            if ( iConnectionBearerType >= EBearerExternalCSD )
                {
                CMUILOGGER_WRITE( "External (modem) connections does not need application name" );
                return sharings;
                }
            else
                {
                TInt count( 0 );
                TConnMonClientEnumBuf clientBuf;
                iActiveWrapper->StartGetConnSharings( iConnectionId,
                                                      iConnectionMonitor,
                                                      clientBuf );

                TInt err( iActiveWrapper->iStatus.Int() );
                CMUILOGGER_WRITE_F( "RefreshSharings status: %d", err );

                if ( !err )
                    {
                    count = clientBuf().iCount;
                    }
                CMUILOGGER_WRITE_F( "clientBuf().iCount: %d", count );

                if ( IsTheSameUids( clientBuf ) )
                    {
                    // Same client list, use cached application names.
                    CMUILOGGER_WRITE( "Client list unchanged, use cached names" );
                    CMUILOGGER_WRITE_F( "sharings: %d", iAppNames->Count() );
                    return iAppNames->Count();
                    }
                else
                    {
                    CMUILOGGER_WRITE( "Client list changed, update names" );
                    CopyUidsToBuf ( clientBuf );
                    }

                iAppNames->Reset();
                HBufC* actAppName = NULL;

#ifndef __WINS__ // Appl. uid is always zero in emulator

                RApaLsSession appSess;
                TApaAppInfo appInfo;

                // The connection could be shared by several applications
                User::LeaveIfError( appSess.Connect() );

                TUint i;
                for ( i = 0; i < count; i++ )
                    {
                    actAppName = NULL;
                    appInfo.iCaption.Zero();

                    TInt result = appSess.GetAppInfo( appInfo,
                            clientBuf().iUid[i] );

                    //================================
                    CMUILOGGER_WRITE_F( "result: %d", result );
                    CMUILOGGER_WRITE_F( "iClientInfo.iUid[i].iUid: %x",
                            clientBuf().iUid[i].iUid );

                    if ( result != KErrNone )
                        {
                        TInt resId = 0;
                        switch ( clientBuf().iUid[i].iUid )
                            {
                            case KMessagingServerUid: // Messaging
                                {
                                resId = R_QTN_CMON_SHARING_APP_MSG_SERVER;
                                break;
                                }
                            case KDownloadMgrServerUid: // Downloads
                                {
                                resId = R_QTN_CMON_SHARING_APP_DL_MG;
                                break;
                                }
                            case KFeedsServerUid: // Web Feeds
                                {
                                resId = R_QTN_CMON_SHARING_APP_RSS_SERVER;
                                break;
                                }
                            case KJavaVMUid: // Application
                                {
                                resId = R_QTN_CMON_SHARING_APP_JAVA_MIDLET;
                                break;
                                }
#ifndef _DEBUG
                            // Hide DHCP & DNS from UI in release build.
                            case KHiddenDhcpServerUid:
                            case KHiddenDnsServerUid:
                                {
                                break;
                                }
#endif
                            case KSUPLServerUid:
                                {
                                resId = R_QTN_CMON_SHARING_APP_SUPL;
                                break;
                                }
                            default: // (unknown)
                                {
                                resId = R_QTN_CMON_SHARING_APP_UNKNOWN;
                                break;
                                }
                            }

                        if ( resId && resId != R_QTN_CMON_SHARING_APP_UNKNOWN  )
                            {
                            actAppName = StringLoader::LoadL( resId );
                            }
                        else if ( resId ) // Try to find application process name from system.
                            {
                            TFileName name;
                            TFindProcess find;
                            TBool name_solved( EFalse );
                            while ( find.Next( name ) == KErrNone )
                                {
                                RProcess ph;
                                TInt err2( KErrNone );
                                err2 = ph.Open( name );

                                if ( err2 )
                                    {
                                    ph.Close();
                                    }
                                else
                                    {
                                    TUidType procUid = ph.Type();
                                    ph.Close();

                                    // There are three UIDs in procUid. The last one is the second
                                    // UID defined in MMP file and clientBuf().iUid[i].iUid is also
                                    // the same kind of UID. So, we only need to compare them to
                                    // find application name.
                                    if ( procUid[2].iUid == clientBuf().iUid[i].iUid )
                                        {
                                        // Name is in format "app_name[uid]001", so cut rest of
                                        // string starting from "[".
                                        TInt index = name.LocateReverse('[');
                                        if ( index >= 1 )
                                            {
                                            name.SetLength( index );
                                            }

                                        actAppName = HBufC::NewL( name.Length() );
                                        TPtr actAppNameBuf = actAppName->Des();
                                        actAppNameBuf.Append( name );

#ifdef _DEBUG
                                        CMUILOGGER_WRITE( "--------------------" );
                                        CMUILOGGER_WRITE( " Application is Found:" );
                                        CMUILOGGER_WRITE_F( "Client Uid: %x", clientBuf().iUid[i].iUid );
                                        CMUILOGGER_WRITE_F( "Proc Uid: %x", procUid[2].iUid );
                                        CMUILOGGER_WRITE_F( "App Name: %S", &name);
                                        CMUILOGGER_WRITE( "--------------------" );
#endif

                                        name_solved = ETrue;
                                        break;
                                        }
                                    }
                                }

                            if ( !name_solved )
                                {
                                actAppName = StringLoader::LoadL( R_QTN_CMON_SHARING_APP_UNKNOWN );
                                }
                            }
                        }
                    else
                        {
                        actAppName = HBufC::NewL( appInfo.iCaption.Length() );
                        TPtr actAppNameBuf = actAppName->Des();
                        actAppNameBuf.Append( appInfo.iCaption );
                        }

                    if ( actAppName )
                        {
                        iAppNames->AppendL( actAppName->Des() );
                        delete actAppName;
                        }
                    }

                iAppNames->Sort();
                appSess.Close();
#else
                TInt countBan( 0 );

#ifndef _DEBUG
                if ( !err )
                    {
                    // Remove DHCP and DNS from the client list. 
                    for ( TUint tmp = 0; tmp < count; tmp++ )
                        {
                        TUid uid = clientBuf().iUid[tmp];

                        if ( uid.iUid == KHiddenDhcpServerUid || uid.iUid == KHiddenDnsServerUid )
                            {
                            countBan++;
                            }
                        }
                    }
#endif

                _LIT( KStrApplication, "Application" );
                // Give a dummy name for all applications.
                for ( TInt i = 0; i < ( count - countBan ); i++ )
                    {
                    iAppNames->AppendL( KStrApplication );
                    }

#endif // __WINS__
                } // End of else in if ( iConnectionBearerType > EBearerExternalCSD )
            sharings = iAppNames->Count();
            CMUILOGGER_WRITE_F( "sharings: %d", sharings );
            } // End of if ( iAppNames )
        } // End of if ( IsAlive() )

    CMUILOGGER_LEAVEFN( "CConnectionInfoBase::RefreshAppNamesL" );
    return sharings;
    }

// ---------------------------------------------------------
// CConnectionInfoBase::IsTheSameUids
// ---------------------------------------------------------
//
TBool CConnectionInfoBase::IsTheSameUids( TConnMonClientEnumBuf& aClients )
    {
    TBool ret( EFalse );

    if ( iClientBuf().iCount != aClients().iCount )
        {
        return ret;
        }
    else
        {
        TInt count = aClients().iCount;
        for ( TInt i = 0; i < count; i++ )
            {
            // Is the current value inside aClients found from iClientBuf.
            TBool found( EFalse );
            for ( TInt j = 0; j < count; j++ )
                {
                if ( aClients().iUid[i].iUid == iClientBuf().iUid[j].iUid )
                    {
                    found = ETrue;
                    break;
                    }
                }

            if ( !found )
                {
                return ret;
                }
            }

        // If we reach this point, aClients contents are identical with iClientBuf.
        ret = ETrue;
        }

    return ret;
    }

// ---------------------------------------------------------
// CConnectionInfoBase::CopyUidsToBuf
// ---------------------------------------------------------
//
void CConnectionInfoBase::CopyUidsToBuf( TConnMonClientEnumBuf& aClients )
    {
    TInt count = aClients().iCount;

    for ( TInt i = 0; i < count; i++ )
        {
        iClientBuf().iUid[i].iUid = aClients().iUid[i].iUid;
        }
    iClientBuf().iCount = count;
    }

// ---------------------------------------------------------
// CConnectionInfoBase::ToDetailsListBoxItemTextL
// ---------------------------------------------------------
//
HBufC*  CConnectionInfoBase::ToDetailsListBoxItemTextL(
                                                const TUint aResourceId,
                                                const HBufC* aValueText ) const
    {
    CMUILOGGER_ENTERFN( "CConnectionInfoBase::ToDetailsListBoxItemTextL" );

    HBufC* columnName = NULL;
    if ( aResourceId )
        {
        columnName = StringLoader::LoadLC( aResourceId );
        }
    else
        {
        columnName = KEmpty().AllocLC();
        }

    HBufC* listBoxItemText = HBufC::NewL( KDetailsListItemText );

    TPtr ptrToListBoxItemText = listBoxItemText->Des();

    ptrToListBoxItemText.Format( KDetailsListItemTextFormat,
                                 columnName, aValueText );
    CleanupStack::PopAndDestroy( columnName );


    CMUILOGGER_LEAVEFN( "CConnectionInfoBase::ToDetailsListBoxItemTextL" );

    return listBoxItemText;
    }

// ---------------------------------------------------------
// CConnectionInfoBase::ToDetailsListBoxItemTextWithAppendL
// ---------------------------------------------------------
//
void  CConnectionInfoBase::ToDetailsListBoxItemTextWithAppendL(
                                                const TUint aResourceId,
                                                const HBufC* aValueText )
    {
    HBufC* listBoxItemText = ToDetailsListBoxItemTextL( aResourceId,
                                                        aValueText );
    CleanupStack::PushL( listBoxItemText );
    CMUILOGGER_WRITE_F( "LISTBOXITEM: %S", listBoxItemText );
    iDetailsArray->AppendL( *listBoxItemText );
    CleanupStack::PopAndDestroy( listBoxItemText );
    }


// ---------------------------------------------------------
// CConnectionInfoBase::ToDetailsListBoxItemTextWithReplaceL
// ---------------------------------------------------------
//
void  CConnectionInfoBase::ToDetailsListBoxItemTextWithReplaceL(
                                                    const TUint aResourceId,
                                                    const HBufC* aValueText,
                                                    TInt aIndex )
    {
    CMUILOGGER_ENTERFN
        ( "CConnectionInfoBase::ToDetailsListBoxItemTextWithReplaceL" );
    CMUILOGGER_WRITE_F( "aValueText: %S", aValueText );

    if ( /*( iDetailsArray ) && */
            ( aIndex < iDetailsArray->Count() ) &&
                ( aIndex > KErrNotFound ) )
        {
        HBufC* listBoxItemText = ToDetailsListBoxItemTextL( aResourceId,
                                                            aValueText );
        iDetailsArray->Delete( aIndex );
        CleanupStack::PushL( listBoxItemText );
        iDetailsArray->InsertL( aIndex, *listBoxItemText );
        CleanupStack::PopAndDestroy( listBoxItemText );
        iDetailsArray->Compress();
        }
    CMUILOGGER_LEAVEFN
        ( "CConnectionInfoBase::ToDetailsListBoxItemTextWithReplaceL" );

    }

// ---------------------------------------------------------
// CConnectionInfoBase::ToStringBearerLC
// ---------------------------------------------------------
//
HBufC* CConnectionInfoBase::ToStringBearerLC() const
    {
    HBufC* bearer = NULL;
    TUint resourceId( 0 );

    // Bearer type
    if ( iConnectionBearerType == EBearerHSCSD ||
        iConnectionBearerType == EBearerExternalHSCSD )
        {
        resourceId = R_QTN_CMON_BEARER_HSCSD;

        }
    else if ( iConnectionBearerType == EBearerCSD ||
             iConnectionBearerType == EBearerExternalCSD ||
             iConnectionBearerType == EBearerWcdmaCSD ||
             iConnectionBearerType == EBearerExternalWcdmaCSD )
        {
        resourceId = R_QTN_SET_BEARER_DATA_CALL;
        }
    else if ( iConnectionBearerType == EBearerWLAN )
        {
        resourceId = R_QTN_WLAN_SETT_BEARER_WLAN;
        }
    else
        {
        resourceId = R_QTN_SET_BEARER_PACKET_DATA;
        }

    bearer = StringLoader::LoadLC( resourceId );

    return bearer;
    }


// ---------------------------------------------------------
// CConnectionInfoBase::ToStringStatusLC
// ---------------------------------------------------------
//
HBufC* CConnectionInfoBase::ToStringStatusLC( TBool aIsDetailsView ) const
    {
    HBufC* status = NULL;
    TUint resourceId( 0 );
    switch ( iConnectionStatus )
        {
        case EConnectionCreated:
            {
            if ( iUpSpeed.quot || iUpSpeed.rem ||
                iDownSpeed.quot || iDownSpeed.rem )
                {
                resourceId = aIsDetailsView ? R_QTN_CMON_STATUS_DETAILS_ACTIVE 
                                            : R_QTN_CMON_STATUS_ACTIVE;
                }
            else
                {
                resourceId = aIsDetailsView ? R_QTN_CMON_STATUS_DETAILS_IDLE 
                                            : R_QTN_CMON_STATUS_IDLE;
                }
            break;
            }
        case EConnectionUninitialized:
        case EConnectionClosed:
            {
            resourceId = aIsDetailsView ? R_QTN_CMON_STATUS_DETAILS_CLOSED 
                                        : R_QTN_CMON_STATUS_CLOSED;
            break;
            }
        case EConnectionClosing:
            {
            resourceId = aIsDetailsView ? R_QTN_CMON_STATUS_DETAILS_CLOSING 
                                        : R_QTN_CMON_STATUS_CLOSING;
            break;
            }
        case EConnectionSuspended:
            {
            resourceId = aIsDetailsView ? R_QTN_CMON_STATUS_DETAILS_SUSPENDED 
                                        : R_QTN_CMON_STATUS_SUSPENDED;
            break;
            }
        case EConnectionCreating:
        default :
            {
            resourceId = aIsDetailsView ? R_QTN_CMON_STATUS_DETAILS_CREATING 
                                        : R_QTN_CMON_STATUS_CREATING;
            break;
            }
        }

    status = StringLoader::LoadLC( resourceId );

    return status;
    }
    
    
// ---------------------------------------------------------
// CConnectionInfoBase::ToStringTotalTransmittedDataLC
// ---------------------------------------------------------
//
HBufC* CConnectionInfoBase::ToStringTotalTransmittedDataLC() const
    {
    TUint total = iUploaded + iDownloaded;
    CMUILOGGER_WRITE_F( "iUploaded : %d", iUploaded );
    CMUILOGGER_WRITE_F( "iDownloaded : %d", iDownloaded );
    CMUILOGGER_WRITE_F( "total : %d", total );
    
    return ToStringLoadedDataLC( total );
    }

// ---------------------------------------------------------
// CConnectionInfoBase::ToStringSentDataLC
// ---------------------------------------------------------
//
HBufC* CConnectionInfoBase::ToStringSentDataLC( TBool aToBeThreeDigit ) const
    {
    return ToStringLoadedDataLC( iUploaded, aToBeThreeDigit, KUpArrowChar );
    }


// ---------------------------------------------------------
// CConnectionInfoBase::ToStringReceivedDataLC
// ---------------------------------------------------------
//
HBufC* CConnectionInfoBase::ToStringReceivedDataLC( TBool aToBeThreeDigit ) const
    {
    return ToStringLoadedDataLC( iDownloaded, aToBeThreeDigit, KDownArrowChar );
    }


// ---------------------------------------------------------
// CConnectionInfoBase::ToStringTransferSpeedUpLC
// ---------------------------------------------------------
//
HBufC* CConnectionInfoBase::ToStringTransferSpeedUpLC() const
    {
    return ToStringTransferSpeedLC( iUpSpeed, KUpArrowChar );
    }


// ---------------------------------------------------------
// CConnectionInfoBase::ToStringTransferSpeedDownLC
// ---------------------------------------------------------
//
HBufC* CConnectionInfoBase::ToStringTransferSpeedDownLC() const
    {
    return ToStringTransferSpeedLC( iDownSpeed, KDownArrowChar );
    }


// ---------------------------------------------------------
// CConnectionInfoBase::ToStringDurationLC
// ---------------------------------------------------------
//
HBufC* CConnectionInfoBase::ToStringDurationLC() const
    {
    const TUint KTimeStringLength = 64;
    const TUint KTimeFormatBuf = 16;
    const TUint KDateFormatBuf = 16;
    _LIT( KDayFormat, "%d" );


    TTime now;
    now.UniversalTime();
    HBufC* timeString = NULL;
    
    TTimeIntervalDays days = NULL;
    
    if  ( iStartTime != NULL )
        {
        // calculate days passed since connection started
        days = now.DaysFrom( iStartTime );
        CMUILOGGER_WRITEF( _L( "ToStringDurationLC days before: %d" ),
                               days.Int() );
        }
    
    // passed time since connection established.
    if ( days.Int() <= 0 )
        // it's still within 1 day
        {
        timeString = HBufC::NewLC( KTimeStringLength );
        TPtr16 tempBuf( timeString->Des() );

        HBufC* temp = StringLoader::LoadLC( R_QTN_TIME_DURAT_LONG_WITH_ZERO );

        iDuration.FormatL( tempBuf, *temp );
        CleanupStack::PopAndDestroy( temp );
        }
    else
        // more than 1 day
        {
        CMUILOGGER_WRITEF( _L( "ToStringDurationLC days after: %d" ),
                               days.Int() );
            
        TBuf<KDateFormatBuf>  dayFormat;
        dayFormat.Format( KDayFormat, days.Int() );

        // format time format string
        HBufC* tempFormat = StringLoader::LoadLC(
                                            R_QTN_TIME_DURAT_SHORT_WITH_ZERO );
        TBuf<KTimeFormatBuf> tempBuf;

        // format hours:mins
        iDuration.FormatL( tempBuf, *tempFormat );

        CDesCArrayFlat* strings = new( ELeave ) CDesCArrayFlat( 2 );
        CleanupStack::PushL( strings );

        strings->AppendL( dayFormat );
        strings->AppendL( tempBuf );

        timeString = StringLoader::LoadL( R_QTN_CMON_DURATION_OVER_24H,
                                          *strings );

        CleanupStack::PopAndDestroy( strings );
        CleanupStack::PopAndDestroy( tempFormat );
        CleanupStack::PushL( timeString );
        }

    CMUILOGGER_WRITE_F( "duration time1: %S", timeString );
    
    TPtr temp( timeString->Des() );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( temp );
    CMUILOGGER_WRITE_F( "duration temp: %S", &temp );
    
    CMUILOGGER_WRITE_F( "duration time2: %S", timeString );
    

    return timeString;
    }


// ---------------------------------------------------------
// CConnectionInfoBase::CheckMrouterIap
// ---------------------------------------------------------
//
TBool CConnectionInfoBase::CheckMrouterIap()
    {
    CMUILOGGER_ENTERFN( "CheckMrouterIap" );
    TBool result( ETrue );

    CMUILOGGER_WRITE_F( "IAP name: %S", &iConnectionName );
    iConnectionName.Trim();
    if ( iConnectionName.Left(
                      KMrouterName().Length() ).CompareF( KMrouterName ) )
        {
        result = EFalse;
        }
    CMUILOGGER_LEAVEFN( "CheckMrouterIap" );
    return result;
    }


// ---------------------------------------------------------
// CConnectionInfoBase::ToStringLoadedDataLC
// ---------------------------------------------------------
//
HBufC* CConnectionInfoBase::ToStringLoadedDataLC( TUint aLoadedData, 
                                                  TBool aToBeThreeDigit, 
                                                  const TUint aArrowChar ) const
    {
    const TUint KTransferedBytesFormatBuf = 16;
    const TUint KTransferedBytesWidth = 10;
    TUint transferedBytesDecimalPlaces(2);
    TUint resId;
    HBufC* loadedTxt = NULL;

    TBuf<KTransferedBytesFormatBuf> buf;

    if ( aLoadedData < 1000 )          // bytes
        {
        if ( aToBeThreeDigit ) // added arrow to listbox
            {
            _LIT( KUdLinkFormatBytes, "%c%d" );

            buf.Format( KUdLinkFormatBytes, aArrowChar, aLoadedData );
            }
        else
            {
            _LIT( KUdLinkFormatBytes, "%d" );

            buf.Format( KUdLinkFormatBytes, aLoadedData );
            }
        resId = R_QTN_LOG_GPRS_AMOUNT_B;
        }
    else
        {                           // KB, MB, GB
        TUint denom;

        if ( aLoadedData < 1000 * 1024 )
            {
            denom = 1024;
            resId = R_QTN_LOG_GPRS_AMOUNT_KB;
            }
        else if ( aLoadedData < 1000 * 1024 * 1024 )
            {
            denom = 1024 * 1024;
            resId = R_QTN_LOG_GPRS_AMOUNT_MB;
            }
        else
            {
            denom = 1024 * 1024 * 1024;
            resId = R_QTN_LOG_GPRS_AMOUNT_GB;
            }

        TReal value = STATIC_CAST( TReal, aLoadedData ) /
                      STATIC_CAST( TReal, denom );
                      
		if ( aToBeThreeDigit )
			{
			if ( value < 10 )	// number of decimal places
				{
				transferedBytesDecimalPlaces = 2;
				}
			else if ( value < 100 )
				{
				transferedBytesDecimalPlaces = 1;
				}
			else
				{
				transferedBytesDecimalPlaces = 0;
				}
			}

		TRealFormat Format( KTransferedBytesWidth, 
							transferedBytesDecimalPlaces );

        HBufC* number = HBufC::NewLC( KTransferedBytesWidth );
        number->Des().Num( value, Format );

        if ( aToBeThreeDigit ) // added arrow to listbox
            {
            _LIT( KUdFormatLarge, "%c%S" );
            buf.Format( KUdFormatLarge, aArrowChar, number );
            }
        else
            {
            _LIT( KUdFormatLarge, "%S" );
            buf.Format( KUdFormatLarge, number );
            }
        CleanupStack::PopAndDestroy( number );
        }

    loadedTxt = StringLoader::LoadLC( resId, buf );

    TPtr temp( loadedTxt->Des() );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( temp );

    return  loadedTxt;
    }


// ---------------------------------------------------------
// CConnectionInfoBase::ToStringTransferSpeedLC
// ---------------------------------------------------------
//
HBufC* CConnectionInfoBase::ToStringTransferSpeedLC( div_t aSpeed,
                                               const TUint aArrowChar ) const
    {
    const TUint KUpDownSpeedBufSize = 16;
    _LIT( KUdSpeedFmt, "%c%d%S%02d" );

    TBuf<KUpDownSpeedBufSize> buf;
    TInt resId;

    HBufC* decSep = StringLoader::LoadLC( R_TEXT_CALC_DECIMAL_SEPARATOR );

    resId = R_QTN_CMON_DATA_TRANSF_RATE_UL;
    buf.Format( KUdSpeedFmt, aArrowChar, aSpeed.quot, decSep, aSpeed.rem );

    CleanupStack::PopAndDestroy( decSep );

    HBufC* speedTxt = StringLoader::LoadLC( resId, buf );

    TPtr temp( speedTxt->Des() );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( temp );

    return speedTxt;
    }

// ---------------------------------------------------------
// CConnectionInfoBase::InitializeConnectionInfoL
// ---------------------------------------------------------
//
void CConnectionInfoBase::InitializeConnectionInfoL()
    {
    CMUILOGGER_WRITE( "InitializeConnectionInfoL - start " );
    TConnMonTimeBuf timeBuf;
    iActiveWrapper->StartGetConnTime( iConnectionId,
                                      iConnectionMonitor,
                                      timeBuf );
    TInt err( iActiveWrapper->iStatus.Int() );
    CMUILOGGER_WRITE_F( "IntiConnectionInfo() status: %d", err );
    if ( err != KErrNone )
        {
        User::Leave( err );
        }

    iStartTime = timeBuf();
    TDateTime dt = iStartTime.DateTime();
    CMUILOGGER_WRITE_F( "Day : %d", dt.Day() );
    CMUILOGGER_WRITE_F( "Hour : %d", dt.Hour() );
    CMUILOGGER_WRITE_F( "Minute : %d", dt.Minute() );
    CMUILOGGER_WRITE_F( "Second : %d", dt.Second() );
    
    
    RefreshDetailsL();
    if ( !iDetailsArray )
        {
        ToArrayDetailsL();
        }
    CMUILOGGER_WRITE( "InitializeConnectionInfoL - end " );
    }

// ---------------------------------------------------------
// CConnectionInfoBase::StringMarkerRemoval
// ---------------------------------------------------------
//
void CConnectionInfoBase::StringMarkerRemoval( HBufC* aStringHolder )
    {
    TPtr bufPtr = aStringHolder->Des();
    
    const TText KLRMarker = 0x200E;
    const TText KRLMarker = 0x200F;

    TBuf<2> markers;
    markers.Append( KLRMarker );
    markers.Append( KRLMarker );

    // This function is requested to remove directional markers, because they
    // cause a display error in languages such Hebrew. Directional markers will
    // be correctly added later, when the one or two parts of the time string 
    // are merged, again with StringLoader functions.
    AknTextUtils::StripCharacters( bufPtr, markers ); 
    }
    
// ---------------------------------------------------------
// CConnectionInfoBase::ToStringAppNameLC
// ---------------------------------------------------------
//
HBufC* CConnectionInfoBase::ToStringAppNameLC( TInt aAppIndex )
    {
    HBufC* appName = ( iAppNames->MdcaPoint( aAppIndex ) ).AllocLC();
    
    return appName;
    }

// ---------------------------------------------------------
// CConnectionInfoBase::GetDeletedFromCMUI
// ---------------------------------------------------------
//
TBool CConnectionInfoBase::GetDeletedFromCMUI() const
	{
	return iDeletedFromCMUI;	
	}

// ---------------------------------------------------------
// CConnectionInfoBase::SetAllDeletedFromCMUI
// ---------------------------------------------------------
//
void CConnectionInfoBase::SetAllDeletedFromCMUI()
	{
	iConnectionStatus = EConnectionClosing;
	}

// ---------------------------------------------------------
// CConnectionInfoBase::SetDeletedFromCMUI
// ---------------------------------------------------------
//
void CConnectionInfoBase::SetDeletedFromCMUI()
	{
	iConnectionStatus = EConnectionClosing;
	
	iDeletedFromCMUI = ETrue;	
	}


// End of File
