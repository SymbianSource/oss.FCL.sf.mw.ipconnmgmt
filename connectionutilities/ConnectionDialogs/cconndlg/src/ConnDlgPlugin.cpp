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
#include "ConnDlgPlugin.h"

#include <centralrepository.h>

#include "ConnectionDialogsUidDefs.h"
#include "ConnectionDialogsLogger.h"

#include <eikenv.h>
#include <bautils.h>
#include <e32property.h> 
#include <rmpm.h>
#include <cmmanager.h>
#include <cmdestination.h>
#include <commsdat.h>

#include <data_caging_path_literals.hrh>


// NOTE that the functionality this file is DEPRECATED
// None on the methods have UI functionality, the plugins complete the requests
// immediately when they are started

using namespace CommsDat;

// CONSTANTS

LOCAL_D const TInt KPluginGranularity = 4;


LOCAL_C void CreateNotifiersL( 
                            CArrayPtrFlat<MEikSrvNotifierBase2>* aNotifiers );




// ---------------------------------------------------------
// NotifierArray()
// Lib main entry point
// ---------------------------------------------------------
//

EXPORT_C CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
    {
    CArrayPtrFlat<MEikSrvNotifierBase2>* array = new (ELeave) 
                    CArrayPtrFlat<MEikSrvNotifierBase2>( KPluginGranularity );

    if ( array )
        {
        TRAPD( err, CreateNotifiersL( array ) );
        if( err )
            {
            TInt count = array->Count();
            while( count-- )
                {
                (*array)[count]->Release();
                }
            delete array;
            array = NULL;
            }
        }

    return( array );
    }


// ---------------------------------------------------------
// CreateNotifiersL
// ---------------------------------------------------------
//
LOCAL_C void CreateNotifiersL( 
                              CArrayPtrFlat<MEikSrvNotifierBase2>* aNotifiers )
    {
    MEikSrvNotifierBase2 *serNotify;
    TBool resourceFileResponsible = ETrue;

    serNotify = CConnDlgIapPlugin::NewL( resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );

    resourceFileResponsible = EFalse; // Only the first has to load resources

    serNotify = CConnDlgAuthenticationPlugin::NewL( resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );

    serNotify = CConnDlgReconnectPlugin::NewL( resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );

    serNotify = CConnDlgNewIapPlugin::NewL( resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );

    serNotify = CConnDlgQosPlugin::NewL( resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );

    serNotify = CConnDlgSelectConnectionPlugin::NewL( 
                                                    resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );
    }



// ---------------------------------------------------------
// CConnDlgIapPlugin::TNotifierInfo CConnDlgIapPlugin::RegisterL()
// ---------------------------------------------------------
//
CConnDlgIapPlugin::TNotifierInfo CConnDlgIapPlugin::RegisterL()
    {
    iInfo.iUid = KUidCConnDlgIap;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel = KUidCConnDlgIap;

    return iInfo;
    }


// ---------------------------------------------------------
// void CConnDlgIapPlugin::StartL
// ---------------------------------------------------------
//

void CConnDlgIapPlugin::StartL( const TDesC8& aBuffer, TInt aReplySlot,
                                const RMessagePtr2& aMessage )
    {

    iCancelled = EFalse;
    iReplySlot = aReplySlot;
    iMessage = aMessage;
    
    // The UI is deprectad, complete the request and return the best available iap
    RCmManager cmManager;
    cmManager.OpenL();     
    CleanupClosePushL( cmManager ); 
    
    RArray<TUint32> destinationArray;    
    cmManager.AllDestinationsL( destinationArray );
    CleanupClosePushL(destinationArray);   
    TInt error = KErrNotFound;
    
    // loop the destinations and find the internet snap
    for (TInt i = 0; i < destinationArray.Count(); ++i)
        {   
        RCmDestination destination = cmManager.DestinationL( destinationArray[i] );
        CleanupClosePushL(destination);
        TUint32 purposeMetaData = destination.MetadataL( CMManager::ESnapMetadataPurpose );
        
        // If the internet snap was found, check out the IAPs
        if ( CMManager::ESnapPurposeInternet ==  purposeMetaData )
            {
            RMPM mpm;
            User::LeaveIfError(mpm.Connect());
            TMpmSnapBuffer buffer;
            buffer.Reset();
            User::LeaveIfError( mpm.SortSNAP( destination.Id(), buffer )); 
            mpm.Close();
            // if there are iaps in the list, return the first
            // if there are none, return an error
            if ( buffer.Count() > 0 )
                {
                iIAP = buffer.iIapId[0];
                error = KErrNone;
                }
            CleanupStack::PopAndDestroy(); //destination
            break;
            }                
        CleanupStack::PopAndDestroy(); //destination
        }
    CleanupStack::PopAndDestroy(); //destinationArray
    CleanupStack::PopAndDestroy(); //cmManager
    
    CompleteL( error );
    }

// ---------------------------------------------------------
// void CConnDlgIapPlugin::Cancel()
// ---------------------------------------------------------
//
void CConnDlgIapPlugin::Cancel()
    { 
    if ( !iCancelled )
        {
        iCancelled = ETrue;
        if ( !iMessage.IsNull() )
            {
            iMessage.Complete( KErrCancel );
            }
        }
    }


// ---------------------------------------------------------
// void CConnDlgIapPlugin::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CConnDlgIapPlugin::CompleteL( TInt aStatus )
    {
    CLOG_ENTERFN( "CConnDlgIapPlugin::CompleteL" );      

    iCancelled = ETrue;
    if ( !iMessage.IsNull() )
        {
        if ( aStatus == KErrNone )
            {
            iMessage.WriteL( iReplySlot, TPckg<TUint32>( iIAP ) );
            }
        
        CLOG_WRITEF( _L( "CConnDlgIapPlugin::CompleteL iIAP : %d" ), iIAP );
                
        iMessage.Complete( aStatus );
        }
    Cancel();

    CLOG_LEAVEFN( "CConnDlgIapPlugin::CompleteL" );   
    }


// ---------------------------------------------------------
// CConnDlgIapPlugin* CConnDlgIapPlugin::NewL()
// ---------------------------------------------------------
//
CConnDlgIapPlugin* CConnDlgIapPlugin::NewL( 
                                        const TBool /*aResourceFileResponsible*/ )
    {
    CConnDlgIapPlugin* self = new (ELeave) CConnDlgIapPlugin();
    return self;
    }
            

// ---------------------------------------------------------
// CConnDlgAuthenticationPlugin::TNotifierInfo
//                                  CConnDlgAuthenticationPlugin::RegisterL()
// ---------------------------------------------------------
//
CConnDlgAuthenticationPlugin::TNotifierInfo 
                                    CConnDlgAuthenticationPlugin::RegisterL()
    {
    iInfo.iUid = KUidCConnDlgAuthentication;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel = KUidCConnDlgAuthentication;

    return iInfo;
    }


// ---------------------------------------------------------
// void CConnDlgAuthenticationPlugin::StartL
// ---------------------------------------------------------
//

void CConnDlgAuthenticationPlugin::StartL( const TDesC8& aBuffer, 
                                           TInt aReplySlot,
                                           const RMessagePtr2& aMessage )
    {
    CLOG_ENTERFN( "CConnDlgAuthenticationPlugin::StartL" );

    iReplySlot = aReplySlot;
    iMessage = aMessage;
    iCancelled = EFalse;

    // The UI is deprecated, just complete the request
    CompleteL(KErrNotSupported);

    CLOG_LEAVEFN( "CConnDlgAuthenticationPlugin::StartL" );
    }



// ---------------------------------------------------------
// void CConnDlgAuthenticationPlugin::Cancel()
// ---------------------------------------------------------
//
void CConnDlgAuthenticationPlugin::Cancel()
    {
    CLOG_ENTERFN( "CConnDlgAuthenticationPlugin::Cancel" );

    if ( !iCancelled )
        {
        iCancelled = ETrue;
        if ( !iMessage.IsNull() )
            {
            iMessage.Complete( KErrCancel );
            }
        }

    CLOG_LEAVEFN( "CConnDlgAuthenticationPlugin::Cancel" );
    }


// ---------------------------------------------------------
// void CConnDlgAuthenticationPlugin::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CConnDlgAuthenticationPlugin::CompleteL( TInt aStatus )
    {
    CLOG_ENTERFN( "CConnDlgAuthenticationPlugin::CompleteL" );

    iCancelled = ETrue;
    if ( !iMessage.IsNull() )
        {
        iMessage.Complete( aStatus );
        }
    Cancel();

    CLOG_LEAVEFN( "CConnDlgAuthenticationPlugin::CompleteL" );
    }


// ---------------------------------------------------------
// CConnDlgAuthenticationPlugin* CConnDlgAuthenticationPlugin::NewL()
// ---------------------------------------------------------
//
CConnDlgAuthenticationPlugin* CConnDlgAuthenticationPlugin::NewL( 
                                        const TBool /*aResourceFileResponsible*/ )
    {
    CConnDlgAuthenticationPlugin* self = new (ELeave) 
                                                CConnDlgAuthenticationPlugin();
    return self;
    }


// ---------------------------------------------------------
// CConnDlgAuthenticationPlugin::CConnDlgAuthenticationPlugin()
// ---------------------------------------------------------
//
CConnDlgAuthenticationPlugin::CConnDlgAuthenticationPlugin()
    {
    }


// ---------------------------------------------------------
// CConnDlgQosPlugin::TNotifierInfo CConnDlgQosPlugin::RegisterL()
// ---------------------------------------------------------
//
CConnDlgQosPlugin::TNotifierInfo CConnDlgQosPlugin::RegisterL()
    {
    iInfo.iUid = KUidCConnDlgQos;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel = KUidCConnDlgQos;

    return iInfo;
    }


// ---------------------------------------------------------
// void CConnDlgQosPlugin::StartL
// ---------------------------------------------------------
//
void CConnDlgQosPlugin::StartL( const TDesC8& /*aBuffer*/, TInt aReplySlot,
                                const RMessagePtr2& aMessage )
    {
    iReplySlot = aReplySlot;
    iMessage = aMessage;
    iCancelled = EFalse;

    CompleteL(KErrNotSupported);
    }


// ---------------------------------------------------------
// void CConnDlgQosPlugin::Cancel()
// ---------------------------------------------------------
//
void CConnDlgQosPlugin::Cancel()
    {
    if ( !iCancelled )
        {
        iCancelled = ETrue;
        if ( !iMessage.IsNull() )
            {
            iMessage.Complete( KErrCancel );
            }
        }
    }


// ---------------------------------------------------------
// void CConnDlgQosPlugin::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CConnDlgQosPlugin::CompleteL( TInt aStatus )
    {
    iCancelled = ETrue;
    if ( !iMessage.IsNull() )
        {
        iMessage.Complete( aStatus );
        }
    Cancel();
    }


// ---------------------------------------------------------
// CConnDlgQosPlugin* CConnDlgQosPlugin::NewL()
// ---------------------------------------------------------
//
CConnDlgQosPlugin* CConnDlgQosPlugin::NewL( 
                                        const TBool /*aResourceFileResponsible*/ )
    {
    CConnDlgQosPlugin* self = new (ELeave) CConnDlgQosPlugin();
    return self;
    }


// ---------------------------------------------------------
// CConnDlgReconnectPlugin::RegisterL()
// ---------------------------------------------------------
//
CConnDlgReconnectPlugin::TNotifierInfo CConnDlgReconnectPlugin::RegisterL()
    {
    iInfo.iUid = KUidCConnDlgReconnect;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel = KUidCConnDlgReconnect;

    return iInfo;
    }


// ---------------------------------------------------------
// void CConnDlgReconnectPlugin::StartL
// ---------------------------------------------------------
//
void CConnDlgReconnectPlugin::StartL( const TDesC8& /*aBuffer*/, 
                                      TInt aReplySlot,
                                      const RMessagePtr2& aMessage )
    {
    iReplySlot = aReplySlot;
    iMessage = aMessage;
    iCancelled = EFalse;

    // The UI is deprecated, just complete the request
    CompleteL(KErrNotSupported);
    }


// ---------------------------------------------------------
// void CConnDlgReconnectPlugin::Cancel()
// ---------------------------------------------------------
//
void CConnDlgReconnectPlugin::Cancel()
    {
    if ( !iCancelled )
        {
        iCancelled = ETrue;
        if ( !iMessage.IsNull() )
            {
            iMessage.Complete( KErrCancel );
            }
        }
    }


// ---------------------------------------------------------
// void CConnDlgReconnectPlugin::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CConnDlgReconnectPlugin::CompleteL( TInt aStatus )
    {
    iCancelled = ETrue;
    if ( !iMessage.IsNull() )
        {
        iMessage.Complete( aStatus );
        }
    Cancel();
    
    }


// ---------------------------------------------------------
// CConnDlgReconnectPlugin* CConnDlgReconnectPlugin::NewL()
// ---------------------------------------------------------
//
CConnDlgReconnectPlugin* CConnDlgReconnectPlugin::NewL( 
                                        const TBool /*aResourceFileResponsible*/ )
    {
    CConnDlgReconnectPlugin* self = new (ELeave) CConnDlgReconnectPlugin();
    return self;
    }


// ---------------------------------------------------------
// CConnDlgNewIapPlugin::TNotifierInfo CConnDlgNewIapPlugin::RegisterL()
// ---------------------------------------------------------
//
CConnDlgNewIapPlugin::TNotifierInfo CConnDlgNewIapPlugin::RegisterL()
    {
    iInfo.iUid = KUidCConnDlgNewIap;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel = KUidCConnDlgNewIap;

    return iInfo;
    }


// ---------------------------------------------------------
// void CConnDlgNewIapPlugin::StartL
// ---------------------------------------------------------
//

void CConnDlgNewIapPlugin::StartL( const TDesC8& aBuffer, TInt aReplySlot,
                                   const RMessagePtr2& aMessage )
    {
    iReplySlot = aReplySlot;
    iMessage = aMessage;
    iCancelled = EFalse;

    // This api has been deprecated, just Complete the request
    CompleteL(KErrNotSupported);
    }


// ---------------------------------------------------------
// void CConnDlgNewIapPlugin::Cancel()
// ---------------------------------------------------------
//
void CConnDlgNewIapPlugin::Cancel()
    {
    if ( !iCancelled )
        {
        iCancelled = ETrue;
        if ( !iMessage.IsNull() )
            {
            iMessage.Complete( KErrCancel );
            }
        }
    }


// ---------------------------------------------------------
// void CConnDlgNewIapPlugin::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CConnDlgNewIapPlugin::CompleteL( TInt aStatus )
    {
    iCancelled = ETrue;
    if ( !iMessage.IsNull() )
        {
        iMessage.Complete( aStatus );
        }
    Cancel();
    }


// ---------------------------------------------------------
// CConnDlgNewIapPlugin* CConnDlgNewIapPlugin::NewL()
// ---------------------------------------------------------
//
CConnDlgNewIapPlugin* CConnDlgNewIapPlugin::NewL( 
                                        const TBool /*aResourceFileResponsible*/ )
    {
    CConnDlgNewIapPlugin* self = new (ELeave) CConnDlgNewIapPlugin();
    return self;
    }

    
// ---------------------------------------------------------
// CConnDlgSelectConnectionPlugin::TNotifierInfo 
//                                  CConnDlgSelectConnectionPlugin::RegisterL()
// ---------------------------------------------------------
//
CConnDlgSelectConnectionPlugin::TNotifierInfo 
                                    CConnDlgSelectConnectionPlugin::RegisterL()
    {
    iInfo.iUid = KUidCConnDlgSelectConn;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel = KUidCConnDlgSelectConn;

    return iInfo;
    }


// ---------------------------------------------------------
// void CConnDlgSelectConnectionPlugin::StartL
// ---------------------------------------------------------
//

void CConnDlgSelectConnectionPlugin::StartL( const TDesC8& /*aBuffer*/, 
                                             TInt aReplySlot,
                                             const RMessagePtr2& aMessage )
    {
        
    iReplySlot = aReplySlot;
    iMessage = aMessage;
    
    // because the connection dialog is deprecated and removed functionality,
    // return the Internet SNAP and complete
    RCmManager cmManager;
    cmManager.OpenL();     
    CleanupClosePushL( cmManager ); 
    
    RArray<TUint32> destinationArray;    
    cmManager.AllDestinationsL( destinationArray );
    CleanupClosePushL(destinationArray);   
    bool found = false;
    
    // loop the destinations and find the internet snap
    for (TInt i = 0; i < destinationArray.Count() && !found; ++i)
        {   
        RCmDestination destination = cmManager.DestinationL( destinationArray[i] );
        CleanupClosePushL(destination);
        
        TUint32 purposeMetaData = destination.MetadataL( CMManager::ESnapMetadataPurpose );
        if ( CMManager::ESnapPurposeInternet ==  purposeMetaData )
            {
            iElementID = destination.ElementId();
            found = true;
            }                
        CleanupStack::PopAndDestroy(); //destination
        }
    CleanupStack::PopAndDestroy(); //destinationArray
    CleanupStack::PopAndDestroy(); //cmManager
    
    if (found)
        {
        CompleteL(KErrNone);
        } 
    else 
        {
        CompleteL(KErrNotFound);
        }
    }

// ---------------------------------------------------------
// void CConnDlgSelectConnectionPlugin::Cancel()
// ---------------------------------------------------------
//
void CConnDlgSelectConnectionPlugin::Cancel()
    {
    CLOG_ENTERFN( "CConnDlgSelectConnectionPlugin::Cancel" );          
    
    if ( !iCancelled )
        {
        CLOG_WRITE( "iCancelled == EFalse" );      
        iCancelled = ETrue;
        if ( !iMessage.IsNull() )
            {
            CLOG_WRITE( "Going to Complete iMessage (KErrCancel)" );      
            iMessage.Complete( KErrCancel );
            CLOG_WRITE( "iMessage completed" );
            }
        }

    CLOG_LEAVEFN( "CConnDlgSelectConnectionPlugin::Cancel" );      
    }


// ---------------------------------------------------------
// void CConnDlgSelectConnectionPlugin::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CConnDlgSelectConnectionPlugin::CompleteL( TInt aStatus )
    {
    CLOG_ENTERFN( "CConnDlgSelectConnectionPlugin::CompleteL" );      

    iCancelled = ETrue;
    if ( !iMessage.IsNull() )
        {
        if ( aStatus == KErrNone )
            {
            iMessage.WriteL( iReplySlot, TPckg<TUint32>( iElementID ) );
            }
        
        CLOG_WRITEF( _L( "CConnDlgSelectConnectionPlugin::CompleteL iElementID : %d" ), iElementID );
                
        iMessage.Complete( aStatus );
        }
    Cancel();

    CLOG_LEAVEFN( "CConnDlgSelectConnectionPlugin::CompleteL" );   
    }


// ---------------------------------------------------------
// CConnDlgSelectConnectionPlugin* CConnDlgSelectConnectionPlugin::NewL()
// ---------------------------------------------------------
//
CConnDlgSelectConnectionPlugin* CConnDlgSelectConnectionPlugin::NewL( 
                                        const TBool /*aResourceFileResponsible*/ )
    {
    CConnDlgSelectConnectionPlugin* self = 
                            new (ELeave) CConnDlgSelectConnectionPlugin();
    return self;
    }
    

// End of File
