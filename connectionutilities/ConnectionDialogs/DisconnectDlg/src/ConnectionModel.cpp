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
*     The model of the connections.
*
*/


// INCLUDE FILES
#include "ConnectionModel.h"
#include "DisconnectDlgUi.h"
#include "ConnectionInfo.h"
#include "ConnectionCArray.h"
#include "DisconnectDlgDialog.h"
#include "ConnectionDialogsLogger.h"

#include <aknnotedialog.h>
#include <StringLoader.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat.h>
#else
#include <commsdat.h>
#include <commsdat_partner.h>
#endif

#include <DisconnectDlg.rsg>


// CONSTANTS

_LIT( KMrouterName, "mRouter" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CConnectionModel::NewL
// ---------------------------------------------------------
//
CConnectionModel* CConnectionModel::NewL(
                                  CDisconnectDialogUi* aDisconnectDialogUi,
                                  CConnectionInfo* aConnectionInfo )
    {
    CLOG_ENTERFN("CConnectionModel::NewL");    
    
    CConnectionModel* self = 
                        new( ELeave ) CConnectionModel( aDisconnectDialogUi );
    CleanupStack::PushL( self );
    self->ConstructL( aConnectionInfo );
    CleanupStack::Pop( self );
    CLOG_LEAVEFN("CConnectionModel::NewL");    

    return self;
    }

// ---------------------------------------------------------
// CConnectionModel::~CConnectionModel
// ---------------------------------------------------------
//
CConnectionModel::~CConnectionModel()
    {
    CLOG_ENTERFN("CConnectionModel::~CConnectionModel");    
    
    // RConnectionMonitor stop sending its notifications
    iMonitor.CancelNotifications();
    iMonitor.Close();

    // Delete connetion array and its content
    if( iConnArray )
        {
        delete iConnArray;
        }

    CLOG_LEAVEFN("CConnectionModel::~CConnectionModel");    
    }

// ---------------------------------------------------------
// CConnectionModel::CConnectionModel
// ---------------------------------------------------------
//
CConnectionModel::CConnectionModel( CDisconnectDialogUi* aDisconnectDialogUi )
: iClosingConnectionIndex( 0 ), 
  iDeleting( EFalse )
    {
    iDisconnectDialogUi = aDisconnectDialogUi;
    }


// ---------------------------------------------------------
// CConnectionModel::ConstructL
// ---------------------------------------------------------
//
void CConnectionModel::ConstructL( CConnectionInfo* aConnectionInfo  )
    {
    CLOG_ENTERFN("CConnectionModel::ConstructL");    
    
    // Connect to the RConectionMonitor and listen to its event's
    iMonitor.ConnectL();
    iMonitor.NotifyEventL( *this );

    // Construct the connection array
    iConnArray = CConnectionCArray::NewL();
    iConnArray->AppendL( aConnectionInfo );

    // Start collecting the connection info
    InitConnectionArrayL();

    // Sort on basis of connection duration. Oldest connection top.
    iConnArray->Sort();

    CLOG_LEAVEFN("CConnectionModel::ConstructL");    
    }


// ---------------------------------------------------------
// CConnectionModel::InitConnectionArrayL
// ---------------------------------------------------------
//
void CConnectionModel::InitConnectionArrayL()
    {   
    CLOG_ENTERFN("CConnectionModel::InitConnectionArrayL");    
    
    TUint i( 0 );
    TUint connNum( 0 );
    TUint subConnectionCount( 0 );
    TUint connId( 0 );    
    CConnectionInfo* info = iConnArray->At( 0 );
    TUint firstConnId = info->GetConnId();

    TInt bearer( 0 );
    TInt connStatus( 0 );

    TRequestStatus status;
    TConnMonClientEnumBuf clientEnum;
    TBuf<CommsDat::KMaxTextLength> apName;

    iMonitor.GetConnectionCount( connNum, status );
    User::WaitForRequest( status );
    
    CLOG_WRITEF(_L( "status: %d" ), status.Int() ); 
    CLOG_WRITEF(_L( "connNum: %d" ), connNum ); 
    
    for( i = 1; i < connNum + 1; ++i )                    
        {
        iMonitor.GetConnectionInfo( i, connId, subConnectionCount );
        if( firstConnId != connId )  
            {               // do not add the first connection to array, again
            iMonitor.GetStringAttribute( connId, 0, KIAPName, apName, status );
            User::WaitForRequest( status );
    
            CLOG_WRITEF(_L( "KIAPName status: %d" ), status.Int() ); 
    
            iMonitor.GetPckgAttribute( connId, 0, KClientInfo, clientEnum, 
                                       status );
            User::WaitForRequest( status );
            CLOG_WRITEF(_L( "KClientInfo status: %d" ), status.Int() );  
            
    
            iMonitor.GetIntAttribute( connId, 0, KBearer, bearer, status );
            User::WaitForRequest( status );
            CLOG_WRITEF(_L( "KBearer status: %d" ), status.Int() ); 
            
    
            iMonitor.GetIntAttribute( connId, 0, KConnectionStatus, connStatus,
                                      status );
            User::WaitForRequest( status );
            CLOG_WRITEF(_L( "KConnectionStatus status: %d" ), status.Int() ); 
            
            CLOG_WRITEF(_L( "bearer: %d" ), (TInt)bearer ); 

            TUint iapId( 0 );
            iMonitor.GetUintAttribute( connId, 0, KIAPId, iapId, status ); 
            User::WaitForRequest( status );
            CLOG_WRITEF(_L( "KIAPId status: %d" ), status.Int() );  
            CLOG_WRITEF(_L( "iapId: %d" ), iapId );
            
            TConnMonTimeBuf timeBuf;
            iMonitor.GetPckgAttribute( connId, 0, KStartTime, timeBuf, status );
            User::WaitForRequest( status );
            CLOG_WRITEF(_L( "KStartTime status: %d" ), status.Int() ); 
                       
            switch( bearer )
                {
                case EBearerGPRS :
                case EBearerEdgeGPRS :
                case EBearerExternalGPRS : 
                case EBearerExternalEdgeGPRS :
                case EBearerWCDMA :
                case EBearerExternalWCDMA :
    #ifdef __WINS__
                case EBearerLAN :
    #endif // WINS
                    {
                    if ( ( connStatus == KLinkLayerOpen ) ||
                         ( connStatus == KConnectionOpen && 
                           bearer >= EBearerExternalCSD ) )
                        {  
                        apName.Trim();
                        if( apName.Left( KMrouterName().Length() ).
                            CompareF( KMrouterName ) )
                            {                    
                            // add array:
                            CConnectionInfo* info = CConnectionInfo::NewLC();
        
                            info->SetConnId( connId );
                            info->SetBearerType( bearer);
                            info->SetIapNameL( apName );
                            info->SetIAPId( iapId );
                            info->SetClientInfo( clientEnum() );
                            info->SetStartTime( timeBuf() );

                            iConnArray->AppendL( info );

                            CleanupStack::Pop( info ); // info        
                            }
                        }
                    break;
                    }
                default :
                    {
                    break;
                    }                
                }
            }
        }
    CLOG_LEAVEFN("CConnectionModel::InitConnectionArrayL");    
        
    }


// ---------------------------------------------------------
// CConnectionModel::EndConnection
// ---------------------------------------------------------
//
TInt CConnectionModel::EndConnection( TUint aConnIndex )
    {
    CLOG_ENTERFN("CConnectionModel::EndConnection");    
    
    // Syncron request, we will wait for ConnMonEvent
    // and the we can say the connection is closed.
    iClosingConnectionIndex = aConnIndex;
    CConnectionInfo* info = iConnArray->At( iClosingConnectionIndex );
    TUint connId = info->GetConnId();

    CLOG_WRITEF(_L( "connId: %d" ), connId ); 
    
    TInt result = iMonitor.SetBoolAttribute( connId, 0, KConnectionStop, 
                                             ETrue );
    CLOG_WRITEF(_L( "result: %d" ), result ); 
                                             
    if ( result )   // if there is an error, notifier will finish, 
        {           // so cancel all other notifications
        iMonitor.CancelNotifications();
        if( KNullUnit == result )
            {
            TUint iapId = info->GetIAPId();
            iDisconnectDialogUi->SetClosedIapId( iapId );
            }
        }
    else
        {
        iDeleting = ETrue;
        TUint iapId = info->GetIAPId();
        iDisconnectDialogUi->SetClosedIapId( iapId );        
        }
    CLOG_LEAVEFN("CConnectionModel::EndConnection");    
        
    return result;
    }


// ---------------------------------------------------------
// CConnectionModel::CancelNotification
// ---------------------------------------------------------
//
void CConnectionModel::CancelNotification()
    {
    iMonitor.CancelNotifications();    
    }


// ---------------------------------------------------------
// CConnectionModel::EventL
// ---------------------------------------------------------
//
void CConnectionModel::EventL( const CConnMonEventBase& aConnMonEvent )
    {
    CLOG_ENTERFN("CConnectionModel::EventL");    
    
    CConnectionInfo* info = iConnArray->At( iClosingConnectionIndex );
    TUint connId = info->GetConnId();
    CLOG_WRITEF(_L( "connId: %d" ), connId ); 
    CLOG_WRITEF(_L( "iDeleting: %b" ), iDeleting ); 
    
    if( iDeleting && iDisconnectDialogUi  && 
        ( EConnMonDeleteConnection == aConnMonEvent.EventType() )
        && ( connId == aConnMonEvent.ConnectionId() ) )
        {
        // notifier will finish, so cancel all other notifications
        iMonitor.CancelNotifications();     
        SelectedConnectionClosedL();
        
        iDisconnectDialogUi->CompleteL( KErrNone );
        }
    else if( iDisconnectDialogUi  && 
             ( EConnMonDeleteConnection == aConnMonEvent.EventType() ) )
        {
        connId = aConnMonEvent.ConnectionId();
        CLOG_WRITEF(_L( "connId: %d" ), connId ); 

        TInt result = iConnArray->GetArrayIndex( connId );
        CLOG_WRITEF(_L( "result: %d" ), result ); 

        if ( result != KErrNotFound )
            {
            iMonitor.CancelNotifications();  
            info = iConnArray->At( result ); 
            TUint iapId = info->GetIAPId();
            CLOG_WRITEF(_L( "iapId: %d" ), iapId );

            iDisconnectDialogUi->SetClosedIapId( iapId );
            iDisconnectDialogUi->CompleteL( KErrNone );
            }
        }
        
    CLOG_LEAVEFN("CConnectionModel::EventL");            
    }


// ---------------------------------------------------------
// CConnectionModel::GetConnArray
// ---------------------------------------------------------
//
CConnectionCArray* CConnectionModel::GetConnArray() const
    {
    return iConnArray;
    }


// ---------------------------------------------------------
// CConnectionModel::SelectedConnectionClosedL
// ---------------------------------------------------------
//
void CConnectionModel::SelectedConnectionClosedL()
    {
    CLOG_ENTERFN("CConnectionModel::SelectedConnectionClosedL");    
    
    // the connection is succesfully closed
    CConnectionInfo* info = iConnArray->At( iClosingConnectionIndex );

    TPtrC iap = info->GetIapNameL();
    TInt bearerType = info->GetBearerType();

    if( bearerType < EBearerExternalCSD )
        {
        InfoNoteL( R_QTN_NETW_CONF_CONN_DISCONNECTED, &iap );
        }
    else
        {
        InfoNoteL( R_QTN_NETW_CONF_MODEM_CONN_DISCONNECTED );
        }

    CLOG_LEAVEFN("CConnectionModel::SelectedConnectionClosedL");    
    }


// ---------------------------------------------------------
// CDisconnectDlgDialog::InfoNoteL
// ---------------------------------------------------------
//
void CConnectionModel::InfoNoteL( const TInt aPromptResourceId,
                                  const TDesC* aPrompt )
    {
    CLOG_ENTERFN("CConnectionModel::InfoNoteL");    
    
    HBufC* informationText;
    if ( aPrompt )
        {
        informationText = StringLoader::LoadLC( aPromptResourceId, *aPrompt );
        }
    else
        {
        informationText = StringLoader::LoadLC( aPromptResourceId );        
        }

    CAknNoteDialog* dialog = new( ELeave )CAknNoteDialog( 
                                REINTERPRET_CAST( CEikDialog**, &dialog ) );
    dialog->PrepareLC( R_DISCONNECTDLG_CONFIRM_NOTE );
    dialog->SetCurrentLabelL( EGeneralNote, informationText->Des() );
    dialog->RunDlgLD( CAknNoteDialog::ELongTimeout, CAknNoteDialog::ENoTone );

    CleanupStack::PopAndDestroy( informationText );  // informationText
    
    CLOG_LEAVEFN("CConnectionModel::InfoNoteL");    
    }

    
// End of File
