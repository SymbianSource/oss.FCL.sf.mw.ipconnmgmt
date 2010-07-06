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
*     Defines the main handler, CDisconnectDialogUi and public API for 
*     Disconnect Dialog UI.
*
*/


// INCLUDE FILES
#include "DisconnectDlgUi.h"
#include "ActiveDisconnectDlgPlugin.h"
#include "ConnectionModel.h"
#include "QueryDisconnectDlgDialog.h"
#include "ConnectionDialogsUidDefs.h"
#include "ConnectionDialogsLogger.h"

#include <bautils.h>
#include <textresolver.h>

#include <disconnectdlg.rsg>
#include <data_caging_path_literals.hrh>


// CONSTANTS

// ROM folder
_LIT( KDriveZ, "z:" );

// Name of the RSC file containing resources
_LIT( KDisconnectDlgUiRSCFile, "DisconnectDlg.rsc" );

/**
* Count of used MEikSrvNotifierBase
*/
LOCAL_D const TInt KPluginGranularity = 1;


LOCAL_C void CreateNotifiersL( 
                          CArrayPtrFlat<MEikSrvNotifierBase2>* aNotifiers );


EXPORT_C CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
    {
    CArrayPtrFlat<MEikSrvNotifierBase2>* array = new( ELeave )
                    CArrayPtrFlat<MEikSrvNotifierBase2>( KPluginGranularity );

    if ( array )
        {
        TRAPD( err, CreateNotifiersL( array ) );
        if( err )
            {
            TInt count = array->Count();
            while( count-- )
                {
                ( *array )[count]->Release();
                }
            delete array;
            array = NULL;
            }
        }

    return( array );
    }


// ---------------------------------------------------------
// CreateNotifiersL( CArrayPtrFlat<MEikSrvNotifierBase>* aNotifiers )
// ---------------------------------------------------------
//
LOCAL_C void CreateNotifiersL( 
                              CArrayPtrFlat<MEikSrvNotifierBase2>* aNotifiers )
    {
    MEikSrvNotifierBase2 *serNotify;

    serNotify = CDisconnectDialogUi::NewL();
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify ); // serNotify
    }


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDisconnectDialogUi::NewLC
// ---------------------------------------------------------
//
CDisconnectDialogUi* CDisconnectDialogUi::NewL()
    {
    CDisconnectDialogUi* self = new( ELeave ) CDisconnectDialogUi();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); // self

    return self;
    }


// ---------------------------------------------------------
// CDisconnectDialogUi::~CDisconnectDialogUi
// ---------------------------------------------------------
//
CDisconnectDialogUi::~CDisconnectDialogUi()
    {
    delete iActivePlugin; 
    delete iConnModel;
    delete iErrorMsgDetails;
    if ( iResOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResOffset );
        }
    }


// ---------------------------------------------------------
// CDisconnectDialogUi::CDisconnectDialogUi
// ---------------------------------------------------------
//
CDisconnectDialogUi::CDisconnectDialogUi()
: iDialogRun( EFalse ), 
  iQueryRun( EFalse ), 
  iResOffset( 0 ),
  iConnModel( NULL ), 
  iIndex( 0 ), 
  iStarted( EFalse ), 
  iActivePlugin( NULL )
    {
    }


// ---------------------------------------------------------
// CDisconnectDialogUi::ConstructL
// ---------------------------------------------------------
//
void CDisconnectDialogUi::ConstructL( )
    {
    // The resource has to be loaded manually since it is not an application.

    TFileName resourceFileName;

    resourceFileName.Append( KDriveZ );
    resourceFileName.Append( KDC_RESOURCE_FILES_DIR );   
    resourceFileName.Append( KDisconnectDlgUiRSCFile );

    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(),
                                    resourceFileName );
    iResOffset = CCoeEnv::Static()->AddResourceFileL( resourceFileName );
    }


// ---------------------------------------------------------
// CDisconnectDialogUi::Cvt
// ---------------------------------------------------------
//
TInt CDisconnectDialogUi::Cvt()
    {
    return KErrNone;
    }


// ---------------------------------------------------------
// CDisconnectDialogUi::InitializedL
// ---------------------------------------------------------
//
void CDisconnectDialogUi::InitializedL( CConnectionInfo* aConnectionInfo )
    {
    CLOG_ENTERFN( "CDisconnectDialogUi::InitializedL" );
    iQueryRun = EFalse;
    delete iConnModel;
    iConnModel = NULL;
    iConnModel = CConnectionModel::NewL( this, aConnectionInfo );

    // Now all the info are ready to show
    // Let's show the dialog        
    iDialogRun = ETrue;
    iDisconnectDialog = CDisconnectDlgDialog::NewL( this, iConnModel, 
                                                    &iIndex, iIapId );
    iDisconnectDialog->RunDlgLD();
    iDialogRun = EFalse;
    
    CLOG_LEAVEFN( "CDisconnectDialogUi::InitializedL" );    
    }


// ---------------------------------------------------------
// void CDisconnectDialogUi::Release()
// ---------------------------------------------------------
//
void CDisconnectDialogUi::Release()
    {
    delete this;
    }


// ---------------------------------------------------------
// TNotifierInfo CDisconnectDialogUi::RegisterL()
// ---------------------------------------------------------
//
CDisconnectDialogUi::TNotifierInfo CDisconnectDialogUi::RegisterL()
    {
    iInfo.iUid = KUidDisconnectDlg;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel = KUidDisconnectDlg;

    return iInfo;
    }


// ---------------------------------------------------------
// TNotifierInfo CDisconnectDialogUi::Info() const
// ---------------------------------------------------------
//
CDisconnectDialogUi::TNotifierInfo CDisconnectDialogUi::Info() const
    {
    return iInfo;
    }


// ---------------------------------------------------------
// TPtrC8 CDisconnectDialogUi::StartL( const TDesC8& /*aBuffer*/ )
// ---------------------------------------------------------
//
TPtrC8 CDisconnectDialogUi::StartL( const TDesC8& /*aBuffer*/ )
    {
    return KNullDesC8().Ptr();
    }


// ---------------------------------------------------------
// void CDisconnectDialogUi::StartL( const TDesC8& /*aBuffer*/, 
//                                  const TAny* /*aReturnVal*/, 
//                                  RMessage /*aMessage*/)
// ---------------------------------------------------------
//

void CDisconnectDialogUi::StartL( const TDesC8& aBuffer, 
                                  TInt aReplySlot,
                                  const RMessagePtr2& aMessage )
    { 
    CLOG_ENTERFN("CDisconnectDialogUi::StartL");        
      
    iStarted = ETrue;

    TPckgBuf<TDisconnectConnectionPrefs> passedInfo;
    passedInfo.Copy( aBuffer );
    iConnectionPrefs = passedInfo();
    
    iReplySlot = aReplySlot;    
    iMessage = aMessage;
    
    CLOG_WRITEF(_L( "iCntPrefs.iPrompt: %d" ), iConnectionPrefs.iPrompt );
    
    TInt err( KErrNone );    
    TRAP( err, iActivePlugin = CActiveDisconnectDlgPlugin::NewL( this ) ); 
    
    if ( err )
        {
        delete iActivePlugin;
        iActivePlugin = NULL;
        User::Leave( err );           
        }
        
    iActivePlugin->StartSearchConnections(); 
     
    CLOG_LEAVEFN("CDisconnectDialogUi::StartL");        
    }


// ---------------------------------------------------------
// void CDisconnectDialogUi::AskDisconnectL
// ---------------------------------------------------------
//

TInt CDisconnectDialogUi::AskDisconnectL()
    {
    CLOG_ENTERFN( "CDisconnectDialogUi::AskDisconnectL" );        
    
    const TInt KErrorCode = -6000;
    TInt proceed( KErrNone );
    TInt textId;
    TUint textFlags;

    delete iErrorMsgDetails;
    iErrorMsgDetails = NULL;

    CTextResolver* resolver = CTextResolver::NewLC();
    const TDesC& error = resolver->ResolveErrorString( KErrorCode, 
                                        textId, textFlags, 
                                        CTextResolver::ECtxNoCtxNoSeparator );

    if ( !( textFlags & EErrorResBlankErrorFlag ||
            textFlags & ETextResolverUnknownErrorFlag ) )
        {
        iErrorMsgDetails = error.AllocL();

        CleanupStack::PopAndDestroy( resolver );
            
        iQueryDisconnectDlgDialog = new ( ELeave ) CQueryDisconnectDlgDialog( 
                                                                        this );
        iQueryDisconnectDlgDialog->SetPromptL( *iErrorMsgDetails );
        iQueryRun = ETrue;
        proceed = iQueryDisconnectDlgDialog->ExecuteLD( 
                                                    R_DISCONNECTDLG_PROCEED );
        iQueryRun = EFalse;
        }
    else
        {
        CleanupStack::PopAndDestroy( resolver );
        }

    CLOG_LEAVEFN( "CDisconnectDialogUi::AskDisconnectL" );        

    return proceed;
    }


// ---------------------------------------------------------
// void CDisconnectDialogUi::Cancel()
// ---------------------------------------------------------
//
void CDisconnectDialogUi::Cancel()
    {
    CLOG_ENTERFN("CDisconnectDialogUi::Cancel");        
    
    delete iActivePlugin;
    iActivePlugin = NULL;
    
    if ( iStarted )
        {           
        iStarted = EFalse;
        if ( !iMessage.IsNull() )
            {
            iMessage.Complete( KErrCancel );
            }
        }
       
    if ( iQueryRun )
        {
        delete iQueryDisconnectDlgDialog;
        }

    if ( iDialogRun )
        {
        delete iDisconnectDialog;
        }
        
    iQueryDisconnectDlgDialog = NULL;
    iDisconnectDialog = NULL;

    delete iErrorMsgDetails;
    iErrorMsgDetails = NULL;
                
    CLOG_LEAVEFN("CDisconnectDialogUi::Cancel");                
    }


// ---------------------------------------------------------
// void CDisconnectDialogUi::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CDisconnectDialogUi::CompleteL( TInt aStatus )
    {
    CLOG_ENTERFN("CDisconnectDialogUi::CompleteL"); 
   
    if ( !iMessage.IsNull() )
        {
        if ( aStatus == KErrNone )
            {
            iMessage.WriteL( iReplySlot, TPckg<TUint32>( iIapId ) );
            CLOG_WRITEF( _L( "iIapId : %d" ), iIapId );        
            }
      
        iMessage.Complete( aStatus );
        }
    
    iStarted = EFalse;
    Cancel();                
    
    CLOG_LEAVEFN("CDisconnectDialogUi::CompleteL");        
    }


// ---------------------------------------------------------
// TPtrC8 CConnDlgPluginBase::UpdateL(const TDesC8& /*aBuffer*/)
// ---------------------------------------------------------
//
TPtrC8 CDisconnectDialogUi::UpdateL(const TDesC8& /*aBuffer*/)
    {
    return KNullDesC8().Ptr();
    }
    
    
// ---------------------------------------------------------
// void CDisconnectDialogUi::SetClosedIapId
// ---------------------------------------------------------
//
void CDisconnectDialogUi::SetClosedIapId( TUint32 aIapId )
    {
    CLOG_ENTERFN("CDisconnectDialogUi::SetClosedIapId");     
    
    iIapId = aIapId;

    CLOG_WRITEF( _L( "iIapId : %d" ), iIapId );
    CLOG_LEAVEFN("CDisconnectDialogUi::SetClosedIapId");            
    }
    

// End of File
