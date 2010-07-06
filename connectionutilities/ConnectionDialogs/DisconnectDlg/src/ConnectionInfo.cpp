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
*      Declares the connection info 
*
*/


// INCLUDE FILES
#include "ConnectionInfo.h"
#include "ConnectionDialogsLogger.h"

#include <StringLoader.h>
#include <AknIconUtils.h>
#include <apgcli.h>
#include <AknsUtils.h>

#include <disconnectdlg.rsg>
#include <disconnectdlgui.mbg>
#include <data_caging_path_literals.hrh>


// CONSTANTS

// ROM folder
_LIT( KDriveZ, "z:" );

// Name of the MBM file containing bitmaps
_LIT( KDisconnectDlgUiMbmFile, "DisconnectDlgUi.mbm" );

const TInt KConnInfoMaxApplName = 256;

const TInt KFeedsServerUid         = 0x1020728E;
const TInt KDownloadMgrServerUid   = 0x10008D60;
const TInt KMessagingServerUid     = 0x1000484b;
const TInt KJavaVMUid              = 0x102033E6;
const TInt KDHCPServerUid          = 0x101fd9c5;
const TInt KPositioningServerUid   = 0x102073CA;

LOCAL_D const TInt KStringsGranularity = 3;


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CConnectionInfo::NewLC
// ---------------------------------------------------------
//
CConnectionInfo* CConnectionInfo::NewLC()
    {
    CLOG_ENTERFN("CConnectionInfo::NewLC");    
    
    CConnectionInfo* self = new( ELeave ) CConnectionInfo();
    CleanupStack::PushL( self );
    self->ConstructL();
    CLOG_LEAVEFN("CConnectionInfo::NewLC");    
    
    return self;
    }


void CConnectionInfo::ConstructL()
    {
    }

// ---------------------------------------------------------
// CConnectionInfo::CConnectionInfo
// ---------------------------------------------------------
//
CConnectionInfo::CConnectionInfo()
: iConnId( 0 ),
  iBearerType( EBearerUnknown )
    {
    }


// ---------------------------------------------------------
// CConnectionInfo::~CConnectionInfo
// ---------------------------------------------------------
//
CConnectionInfo::~CConnectionInfo()
    {
    CLOG_ENTERFN("CConnectionInfo::~CConnectionInfo");    
    
    delete iAppName;
    delete iIapName;
    delete iAppIcon;

    CLOG_LEAVEFN("CConnectionInfo::~CConnectionInfo");    
    }


// ---------------------------------------------------------
// CConnectionInfo::GetAppNameL
// ---------------------------------------------------------
//
TPtrC CConnectionInfo::GetAppNameL( TBool &aUseSharedIcon )
    {
    CLOG_ENTERFN("CConnectionInfo::GetAppNameL");    
    
    aUseSharedIcon = EFalse;
    TBool specialServer = EFalse;
    if( !iAppName )
        {
        iAppName = HBufC::NewL( KConnInfoMaxApplName );
        TPtr infoBuf = iAppName->Des();
        HBufC* actAppName = NULL;
        // Check whether it is an internal or external (modem) connection
        // External (modem) connections does not need application name
        if( iBearerType < EBearerExternalCSD )
            {

#ifndef __WINS__ // Appl. uid is always zero in emulator

            RApaLsSession appSess;
            TApaAppInfo appInfo;

            TInt count = iClientInfo.iCount;
            CLOG_WRITEF(_L( "iClientInfo.iCount: %d" ), count );

            // The connection could be shared by several applications
                    
            User::LeaveIfError( appSess.Connect() );
                    
            _LIT( KStrCommaSpace, ", " );

            CDesCArrayFlat* strings = new( ELeave ) CDesCArrayFlat( 
                                                        KStringsGranularity );
            CleanupStack::PushL( strings );
            
            TUint i;
            TUint realNumberOfOwners = 0;
            for( i = 0; i < count; i++ )
                {
                realNumberOfOwners++;
                actAppName = NULL;
                appInfo.iCaption.Zero();

                TInt result = appSess.GetAppInfo( appInfo, 
                                                  iClientInfo.iUid[i] );

                //================================
                CLOG_WRITEF( _L( "result: %d" ), result ); 
                CLOG_WRITEF( _L( "iClientInfo.iUid[i].iUid: %x" ), iClientInfo.iUid[i].iUid );
                if( result != KErrNone )
                    {
                    TInt resId = 0;
                    switch( iClientInfo.iUid[i].iUid )
                        {
                        case KFeedsServerUid:
                            {
                            resId = R_QTN_NETW_DDLG_APP_RSS_SERVER;
                            break;
                            }
                        case KDownloadMgrServerUid:
                            {
                            resId = R_QTN_NETW_DDLG_APP_DL_MGR;
                            break;
                            }
                        case KMessagingServerUid:
                            {
                            resId = R_QTN_NETW_DDLG_APP_MSG_SERVER;
                            break;
                            }
                        case KJavaVMUid:
                            {
                            resId = R_QTN_NETW_DDLG_APP_JAVA_MIDLET;
                            break;
                            }
                        case KDHCPServerUid:
                            {   // Forget about DHCP server!
                            realNumberOfOwners--;
                            break;
                            }
			case KPositioningServerUid:
                        	{
                        	resId = R_QTN_NETW_DDLG_APP_SUPL;
                        	break;	
                        	}
                        default:
                            {
                            resId = R_QTN_NETW_DDLG_APP_UNKNOWN;
                            break;
                            }
                        }

                    if ( resId )
                        {
                        actAppName = StringLoader::LoadL( resId );
                        specialServer = ETrue;
                        }
                    }
                else
                    {
                    actAppName =  HBufC::NewL( appInfo.iCaption.Length() );
                    TPtr actAppNameBuf = actAppName->Des();
                    actAppNameBuf.Append( appInfo.iCaption );
                    }

                if ( actAppName )
                    {
                    strings->AppendL( actAppName->Des() );        
                    delete actAppName;
                    }
                }

                if ( realNumberOfOwners > 1 || specialServer )
                    {
                    aUseSharedIcon = ETrue;
                    }

                strings->Sort();
                for( TUint i = 0; i < realNumberOfOwners; i++ )
                    {
                    if ( strings->MdcaPoint( i ).Length() )
                        {
                        infoBuf.Append( strings->MdcaPoint( i ) );
                        if( i < realNumberOfOwners-1 )
                            {
                            // This case the application names should be comma
                            // separated
                            infoBuf.Append( KStrCommaSpace );
                            }
                        }
                    }
                CleanupStack::PopAndDestroy( strings );

                appSess.Close();

#else
            _LIT( KStrApplication, "Application" );
            // Give a dummy name of the application
            infoBuf.Append( KStrApplication );

#endif /* __WINS__ */

            }
        /* modem connections does not need application name
        else 
            {
            }
        */
        }
    CLOG_LEAVEFN("CConnectionInfo::GetAppNameL");    

    return iAppName->Des();
    }


// ---------------------------------------------------------
// CConnectionInfo::GetIapNameL
// ---------------------------------------------------------
//
TPtrC CConnectionInfo::GetIapNameL()
    {
    CLOG_ENTERFN("CConnectionInfo::GetIapNameL");    
    
    if( iBearerType >= EBearerExternalCSD )
        {
        // It is a modem connection, therefore 
        // new IAP name must be constructed
        delete iIapName;
        iIapName = NULL;

        iIapName = StringLoader::LoadL( R_QTN_CMON_ITEM_CONN_NAME_MODEM );
        }
    CLOG_LEAVEFN("CConnectionInfo::GetIapNameL");    

    return iIapName->Des();
    }


// ---------------------------------------------------------
// CConnectionInfo::GetAppIconL
// ---------------------------------------------------------
//
#ifdef __WINS__
CGulIcon* CConnectionInfo::GetAppIconL( TBool /*aUseSharedIcon*/ )
#else
CGulIcon* CConnectionInfo::GetAppIconL( TBool aUseSharedIcon )
#endif	// __WINS__
    {
    CLOG_ENTERFN("CConnectionInfo::GetAppIconL");    
    
    if( !iAppIcon )
        {
        CFbsBitmap* bitmap = NULL; 
        CFbsBitmap* mask = NULL;
        TInt err( KErrNone );
        
        TFileName iconsFileName;

        iconsFileName.Append( KDriveZ );
        iconsFileName.Append( KDC_APP_BITMAP_DIR );
        iconsFileName.Append( KDisconnectDlgUiMbmFile );        

#ifndef __WINS__ // Appl. uid is always zero in emulator
        MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();

        if( iBearerType < EBearerExternalCSD )
            {
            // It is an interanl connection.
            // Let's start to get the application icon

            TInt i( 0 );
            TBool iconFound = EFalse;

            if( !aUseSharedIcon )
                {
                // The connection is not shared by many application
                // Get the application icon
                TUid uid = iClientInfo.iUid[i]; 
                CLOG_WRITEF(_L( "uid: %x" ), uid );               
                
                TRAP( err, 
                    {
                    AknsUtils::CreateAppIconLC( skinInstance, uid, 
                                                EAknsAppIconTypeList, 
                                                bitmap, mask );
                    CleanupStack::Pop( 2 );
                    } );
                
                CLOG_WRITEF(_L( "err: %d" ), err ); 
                
                if ( err == KErrNone )
                    {
                    CLOG_WRITEF(_L( "Using the icon proper of the application. err: %d" ), err ); 
                    TRAP( err, iAppIcon = CGulIcon::NewL( bitmap, mask ) );
                    if ( err )
                        {
                        delete bitmap;
                        delete mask;
                        User::Leave( err );
                        }
                    else
                        {
                        iconFound = ETrue;
                        }
                    }
                }

            if ( !iconFound )
                {   // shared connection: qgn_prop_netw_shared_conn
                iAppIcon = AknsUtils::CreateGulIconL( skinInstance, 
                           KAknsIIDQgnPropNetwSharedConn,
                           iconsFileName, 
                           EMbmDisconnectdlguiQgn_prop_netw_shared_conn, 
                           EMbmDisconnectdlguiQgn_prop_netw_shared_conn_mask );
                }
            }
        else // it is a modem connection: qgn_menu_fax_lst
            {
            AknIconUtils::CreateIconL( bitmap, mask, iconsFileName, 
                                   EMbmDisconnectdlguiQgn_menu_fax_lst, 
                                   EMbmDisconnectdlguiQgn_menu_fax_lst_mask );
            
            TRAP( err, iAppIcon = CGulIcon::NewL( bitmap, mask ) );
            if ( err )
                {
                delete bitmap;
                delete mask;
                User::Leave( err );
                }
            }

#else // testing on emulator: dummy icon

            AknIconUtils::CreateIconL( bitmap, mask, iconsFileName, 
                                   EMbmDisconnectdlguiQgn_menu_fax_lst, 
                                   EMbmDisconnectdlguiQgn_menu_fax_lst_mask );
            
            TRAP( err, iAppIcon = CGulIcon::NewL( bitmap, mask ) );
            if ( err )
                {
                delete bitmap;
                delete mask;
                User::Leave( err );
                }
#endif /* __WINS__ */

        }
    CLOG_LEAVEFN("CConnectionInfo::GetAppIconL");    
        
    return iAppIcon;
    }


// End of File
