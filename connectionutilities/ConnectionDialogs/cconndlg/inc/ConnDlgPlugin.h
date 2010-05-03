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


// NOTE that the functionality this header contains is DEPRECATED
// None on the methods have UI functionality, the plugins complete the requests
// immediately when they are started


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
    static CConnDlgIapPlugin* NewL( const TBool /*aResourceFileResponsible*/ );

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
    
private:
    TUint32 iIAP;                       // Internet Access Point
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
                                        const TBool /*aResourceFileResponsible*/ );

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
                                        const TBool /*aResourceFileResponsible*/ );

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
    void CompleteL(TInt aStatus);
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
    static CConnDlgQosPlugin* NewL( const TBool /*aResourceFileResponsible*/ );

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
    static CConnDlgNewIapPlugin* NewL( const TBool /*aResourceFileResponsible*/ );

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
                                        const TBool /*aResourceFileResponsible*/ );

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
    void StartL( const TDesC8& /*aBuffer*/, TInt aReplySlot, 
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

private:
    TUint32 iElementID;
    };

#endif

// End of File
