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
* Description:  Declaration of Generic Connection Dialog Plugins
*
*/



#ifndef __CONNDLGPLUGIN_H__
#define __CONNDLGPLUGIN_H__


// INCLUDES
#include "ConnectionDialogsNotifBase.h"

#include <e32base.h>
#include <agentdialog.h>
#include <AknNotifyStd.h>


// ENUMERATIONS
enum TConnDlgPluginPanic                        // Reasons for panic
    {
    EConnDlgPctNotOpen,                         // Post Connection Terminal not
                                                // open
    EConnDlgPctAlreadyOpen,                     // Post Connection Terminal 
                                                // already open
    EConnDlgReadPctOutstandingOnClose,          // Post Connection Terminal 
                                                // outstanding on close
    EConnDlgDestroyPctNotfcnOutstandingOnClose, // Destroying Post Connection 
                                                // Terminal 
    EConnDlgRMessageWriteFailed,                // Failed RMessage Write
    EConnDlgRMessageReadFailed,                 // Failed RMessage Read
    EConnDlgIllegalRequest                      // Illegal request
    };


enum TConnDlgNotifierRequest                    // Opcodes used in message 
    {                                           // passing in notifiers
    EGetIAP,                                    // Get IAP
    EGetISP,                                    // Get ISP
    EWarnNewIAP,                                // Warning New IAP
    EWarnNewISP,                                // Warning New ISP
    EGetAuthentication,                         // Get Authentication
    EGetLogin,                                  // Get Login
    EGetReconnectReq,                           // Get Reconnect request
    EOpenPct,                                   // Open Post Connection
                                                // Terminal
    EWritePct,                                  // Write Post Connection
                                                // Terminal
    EReadPct,                                   // Read Post Connection
                                                // Terminal
    EDestroyPctNotification,                    // Destroy Post Connection 
                                                // Terminal Notification
    EClosePct,                                  // Close Post Connection 
                                                // Terminal
    ECancelGetIAP,                              // Cancel Get IAP
    ECancelGetISP,                              // Cancel Get ISP
    ECancelWarnIAP,                             // Cancel Warning IAP
    ECancelWarnISP,                             // Cancel Warning ISP
    ECancelLogin,                               // Cancel Login
    ECancelAuthenticate,                        // Cancel Authenticate
    ECancelReconnect,                           // Cancel Reconnect
    ECancelReadPct,                             // Cancel Read Post Connection
                                                // Terminal
    ECancelDestroyPctNotification               // Destroy Post Connection
                                                // Terminal notification
    };


// GLOBAL FUNCTIONS
//
IMPORT_C CArrayPtr<MEikSrvNotifierBase2>* NotifierArray();


// FORWARD DECLARATION
class CAknMultiLineDataQueryDialog;
class CAknQueryDialog;
class CActiveCConnDlgIapPlugin;
class CActiveSelectConnectionPlugin;

// CLASS DECLARATION

class TAuthenticationPairBuff
    {
public:
    TBuf<KCommsDbSvrDefaultTextFieldLength> iUsername;
    TBuf<KCommsDbSvrDefaultTextFieldLength> iPassword;
    };


/**
 * IAP Plugin class
 */
NONSHARABLE_CLASS( CConnDlgIapPlugin ) : public CConnectionDialogsNotifBase
    {
public:
    /**
    * NewL function
    * @param  -
    * return CConnDlgIapPlugin*
    */
    static CConnDlgIapPlugin* NewL( const TBool aResourceFileResponsible );

    /**
    * RegisterL register the client notifier function
    * @param  -
    * return TNotifierInfo
    */
    TNotifierInfo RegisterL();

    /**
    * Start the Notifier
    * @param  aBuffer    Buffer
    * @param  aReplySlot Identifies which message argument to use for the 
    *                    reply. This message argument will refer to a 
    *                    modifiable descriptor, a TDes8 type, into which data
    *                    can be returned. 
    * @param  aMessage   Message
    * return -
    */
    void StartL( const TDesC8& aBuffer, TInt aReplySlot, 
                 const RMessagePtr2& aMessage );

    /**
    * Cancel() the notifier
    * @param  -
    * return -
    */
    void Cancel();

public:

    /**
    * CompleteL the notifier is complete
    * @param  aStatus status
    * return  -
    */
    void CompleteL( TInt aStatus );
    
    /**
    * Sets the preferred iap into db.
    * calls iActivePlugin    
    * @param aIAPId id of the preferred iap
    */
    void SetPreferredIapIdL( TUint32 aIAPId );
    
private:

    /**
    * Gets user connection info.    
    * @param aIapId id of the iap
    */ 
    TInt GetUserConnection( TInt& aIapId );
    
    /**
    * Gets active connection info.    
    * @param aIapId id of the iap
    * @param aBearer bearer type
    */
    TInt GetActiveConnection( TInt& aIapId, TInt& aBearer );    
    
private:
    TUint32 iIAP;                       // Internet Access Point
    TPckgBuf<TConnectionPrefs> iPrefs;  // Selected preferences
    CActiveCConnDlgIapPlugin* iActivePlugin;    // pointer to active object
    };


/**
 * Authentication Plugin class
 */
NONSHARABLE_CLASS( CConnDlgAuthenticationPlugin ) : 
                                            public CConnectionDialogsNotifBase
    {
public:
    CConnDlgAuthenticationPlugin::CConnDlgAuthenticationPlugin();

    /**
    * NewL function
    * @param  -
    * return CConnDlgAuthenticationPlugin*
    */
    static CConnDlgAuthenticationPlugin* NewL( 
                                        const TBool aResourceFileResponsible );

    /**
    * RegisterL register the client notifier function
    * @param  -
    * return TNotifierInfo
    */
    TNotifierInfo RegisterL();

    /**
    * Start the Notifier
    * @param  aBuffer    Buffer
    * @param  aReplySlot Identifies which message argument to use for the 
    *                    reply. This message argument will refer to a 
    *                    modifiable descriptor, a TDes8 type, into which data
    *                    can be returned. 
    * @param  aMessage   Message
    * return -
    */
    void StartL( const TDesC8& aBuffer, TInt aReplySlot, 
                 const RMessagePtr2& aMessage );

    /**
    * Cancel() the notifier
    * @param  -
    * return -
    */
    void Cancel();

public:

    /**
    * GetAuthenticationL() show the Authenticate  dialog
    * @param  -
    * return -
    */
    void GetAuthenticationL();

    /**
    * CompleteL the notifier is complete
    * @param  aStatus status
    * return  -
    */
    void CompleteL( TInt aStatus );

private:
    TPckgBuf<TAuthenticationPair> iAuthPair;    // Authentication pair 
                                                // (username and password)
    TPckgBuf<TAuthenticationPairBuff> iAuthPairBuff;    // Authentication pair 
                                                // (username and password)
    CAknMultiLineDataQueryDialog *iDialog;      // Pointer to the dialog
    };


/**
 * Reconnect Plugin class
 */
NONSHARABLE_CLASS( CConnDlgReconnectPlugin ) : 
                                            public CConnectionDialogsNotifBase
    {
public:

    /**
    * NewL function
    * @param  -
    * return CConnDlgReconnectPlugin*
    */
    static CConnDlgReconnectPlugin* NewL( 
                                        const TBool aResourceFileResponsible );

    /**
    * RegisterL register the client notifier function
    * @param  -
    * return TNotifierInfo
    */
    TNotifierInfo RegisterL();

    /**
    * Start the Notifier
    * @param  aBuffer    Buffer
    * @param  aReplySlot Identifies which message argument to use for the 
    *                    reply. This message argument will refer to a 
    *                    modifiable descriptor, a TDes8 type, into which data
    *                    can be returned. 
    * @param  aMessage   Message
    * return -
    */
    void StartL( const TDesC8& aBuffer, TInt aReplySlot, 
                 const RMessagePtr2& aMessage );

    /**
    * Cancel() the notifier
    * @param  -
    * return -
    */
    void Cancel();

public:

    /**
    * GetReconnectL() call the reconnect dialog
    * @param  -
    * return -
    */
    void GetReconnectL();

    /**
    * CompleteL the notifier is complete
    * @param  aStatus status
    * return  -
    */
    void CompleteL(TInt aStatus);

private:
    CAknQueryDialog* iDialog;   // Pointer to the dialog
    TBool iBool;                // Tells if it has to connect to the IAP
    };


/**
 * Quality of service Plugin class
 */
NONSHARABLE_CLASS( CConnDlgQosPlugin ) : public CConnectionDialogsNotifBase
    {
public:

    /**
    * NewL function
    * @param  -
    * return CConnDlgReconnectPlugin*
    */
    static CConnDlgQosPlugin* NewL( const TBool aResourceFileResponsible );

    /**
    * RegisterL register the client notifier function
    * @param  -
    * return TNotifierInfo
    */
    TNotifierInfo RegisterL();

    /**
    * Start the Notifier
    * @param  aBuffer    Buffer
    * @param  aReplySlot Identifies which message argument to use for the 
    *                    reply. This message argument will refer to a 
    *                    modifiable descriptor, a TDes8 type, into which data
    *                    can be returned. 
    * @param  aMessage   Message
    * return -
    */
    void StartL( const TDesC8& aBuffer, TInt aReplySlot, 
                 const RMessagePtr2& aMessage );

    /**
    * Cancel() the notifier
    * @param  -
    * return -
    */
    void Cancel();

public:

    /**
    * GetReconnectL() call the reconnect dialog
    * @param  -
    * return -
    */
    void GetReconnectL();

    /**
    * CompleteL the notifier is complete
    * @param  aStatus status
    * return  -
    */
    void CompleteL( TInt aStatus );

private:
    CAknQueryDialog* iDialog;   // Pointer to the dialog
    TBool iBool;                // Tells if it has to connect to the IAP
    };


/**
 * New IAP Plugin class
 */
NONSHARABLE_CLASS( CConnDlgNewIapPlugin ) : public CConnectionDialogsNotifBase
    {
public:

    /**
    * NewL function
    * @param  -
    * return CConnDlgNewIapPlugin*
    */
    static CConnDlgNewIapPlugin* NewL( const TBool aResourceFileResponsible );

    /**
    * RegisterL register the client notifier function
    * @param  -
    * return TNotifierInfo
    */
    TNotifierInfo RegisterL();

    /**
    * Start the Notifier
    * @param  aBuffer    Buffer
    * @param  aReplySlot Identifies which message argument to use for the 
    *                    reply. This message argument will refer to a 
    *                    modifiable descriptor, a TDes8 type, into which data
    *                    can be returned. 
    * @param  aMessage   Message
    * return -
    */
    void StartL( const TDesC8& aBuffer, TInt aReplySlot, 
                 const RMessagePtr2& aMessage );

    /**
    * Cancel() the notifier
    * @param  -
    * return -
    */
    void Cancel();

public:

    /**
    * GetNewIapL show NewIap dialog
    * @param  -
    * return  -
    */
    void GetNewIapL();

    /**
    * CompleteL the notifier is complete
    * @param  aStatus status
    * return  -
    */
    void CompleteL( TInt aStatus );

private:
    CAknQueryDialog* iDialog;                   // Pointer to the dialog
    TBool iConnect;                             // Tells if it has to connect 
                                                // to the IAP
    TPckgBuf<TNewIapConnectionPrefs> iPrefs;    // Selected preferences
    };



/**
 * Connection Selection Plugin class
 */
NONSHARABLE_CLASS( CConnDlgSelectConnectionPlugin ) : 
                                            public CConnectionDialogsNotifBase
    {
public:
    /**
    * NewL function
    * @param  -
    * return CConnDlgSelectConnectionPlugin*
    */
    static CConnDlgSelectConnectionPlugin* NewL( 
                                        const TBool aResourceFileResponsible );

    /**
    * RegisterL register the client notifier function
    * @param  -
    * return TNotifierInfo
    */
    TNotifierInfo RegisterL();

    /**
    * Start the Notifier
    * @param  aBuffer    Buffer
    * @param  aReplySlot Identifies which message argument to use for the 
    *                    reply. This message argument will refer to a 
    *                    modifiable descriptor, a TDes8 type, into which data
    *                    can be returned. 
    * @param  aMessage   Message
    * return -
    */
    void StartL( const TDesC8& aBuffer, TInt aReplySlot, 
                 const RMessagePtr2& aMessage );

    /**
    * Cancel() the notifier
    * @param  -
    * return -
    */
    void Cancel();

public:

    /**
    * CompleteL the notifier is complete
    * @param  aStatus status
    * return  -
    */
    void CompleteL( TInt aStatus );
    
    /**
    * Sets the preferred iap into db.
    * calls iActivePlugin    
    * @param aIAPId id of the preferred iap
    * @param aDestinationId id of the preferred Destination
    */
    void SetElementIDL( TUint32 aIAPId, TUint32 aDestinationId );
    
private:

    /**
    * Gets user connection info.    
    * @param aIapId id of the iap
    * @param aSnapId id of the destination
    */
    TInt GetUserConnection( TInt& aIapId, TInt& aSnapId );
    
    /**
    * Gets active connection info.    
    * @param aIapId id of the iap
    * @param aSnapId id of the destination
    * @param aBearer bearer type
    */    
    TInt GetActiveConnection( TInt& aIapId, TInt& aSnapId, TInt& aBearer );

private:
    TUint32 iElementID;
    TPckgBuf<TConnectionPrefs> iPrefs;  // Selected preferences
    CActiveSelectConnectionPlugin* iActivePlugin;   // pointer to active object
    };

#endif

// End of File
