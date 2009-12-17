/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Plug-in object subsession.
*
*/

#ifndef __CPLUGIN_H
#define __CPLUGIN_H

#include <rconnmon.h>
#include "ConnMonSess.h"

_LIT( KPluginName,"Plugin[%08x]");

/**
*  TAttributeRequest
*  Record for the plug-in's request to serve the next attribute query.
*  @lib CONNMONEXT.LIB
*  @since
*/
class TAttributeRequest
    {
    public:
        // Constructor
        TAttributeRequest( TUint aType,
                           TUint aAttribute,
                           TUint aData );
    public:
        inline void Reset(){ iType = 0; iAttribute = 0; iData = 0; }

    public: // Data
        TUint iType;
        TUint iAttribute;
        TUint iData;
    };

/**
*  TClientRequest
*  Record for a pending client requst.
*  This request has not yet been completed in the server side.
*  @lib CONNMON.LIB
*  @since
*/
class TClientRequest
    {
    public: // Constructor
        TClientRequest( TUint aType, TBool aPending, const RMessage2& aMessage )
            : iType( aType ),
              iPending( aPending  ),
              iMessage( aMessage ) {};

    public: // Data
        // Type of the request
        TUint iType;

        // Has the request been sent to the plug-in
        TBool iPending;

        // Client message
        const RMessage2 iMessage;
    };


/**
*  CPlugin
*  @lib CONNMONEXT.LIB
*  @since
*/
NONSHARABLE_CLASS( CPlugin ) : public CObject
    {
    public: // Constructors and destructor
        ~CPlugin();
        static CPlugin* NewL( CConnMonSession* aSession, const TUint aPluginId );

    public: // New methods (called by the PLUGIN)
        /**
        * Registers a new attribute.
        * @since
        * @param aAttribute, The attribute identifier.
        * @return KErrNone if successfull, otherwise a system wide error code..
        */
        TInt RegisterAttribute( const TUint aAttribute );

        /**
        * Cancels the attribute registration.
        * @since
        * @param aAttribute, The attribute identifier.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt CancelRegisterAttribute( const TUint aAttribute );

        /**
        * Handles a new event sent by the plugin.
        * @since
        * @param aMessage The message object.
        * @param aPtr Pointer to extra buffer.
        * @param aSize Size of the extra buffer.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt Event( const RMessage2& aMessage,
                    TEventInfo&      aEventInfo,
                    TUint8*&         aPtr,
                    TInt&            aSize  ) const;

        /**
        * Handles the attribute sent by the plugin.
        * @since
        * @param aMessage The message object.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt Attribute( const RMessage2& aMessage );

        /**
        * Sends the next attribute request to the plugin.
        * @since
        * @return KErrNone or KRequestPending if successfull, otherwise a system wide error code.
        */
        TInt GetQuery();

        /**
        * Sends the next attribute request to the plugin (plugin is explicitly requesting the
        * the next attribute request).
        * @since
        * @param aMessage The message object.
        * @return KErrNone or KRequestPending if successfull, otherwise a system wide error code.
        */
        TInt GetQuery( const RMessage2& aMessage );

        /**
        * Cancels the query issued by the plugin.
        * @since
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt CancelGetQuery();

    public: // New methods (called by a CLIENT)
        /**
        * Handles an attribute request issued by a client.
        * @since
        * @param aType The type of the attribute (TInt, TUint, TBool, TDes, TDes8).
        * @param aMessage Message sent by the client
        * @return KRequestPending if successfull, otherwise a system wide error code.
        */
        TInt GetAttributeForClient( const TInt aType, const RMessage2& aMessage );

         /**
        * Cancels the attribute request issued by a client.
        * @since
        * @param aClient Reference to the client thread.
        * @param aType The type of the attribute (TInt, TUint, TBool, TDes, TDes8).
        * @return void.
        */
        void CancelGetAttributeForClient( const RThread& aClient, const TInt aType );

        /**
        * Handles a request to set a value for an attribute issued by a client.
        * @since
        * @param aType The type of the attribute (TInt, TUint, TBool, TDes, TDes8).
        * @param aMessage Message sent by the client
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt SetAttributeForClient( const TInt aType, const RMessage2& aMessage );

    private:
        CPlugin();

        /**
        * Second phase construction.
        * @since
        * @param aSession Session owning this plug-in object.
        * @param aPluginId Generic bearer specific connection id that identifies the plug-in.
        * @return void
        */
        void ConstructL( CConnMonSession* aSession, const TUint aPluginId );

        /**
        * Panics the client thread that has a request pending (because of a bad descriptor).
        * @since
        * @param aPanic The panic code.
        * @param aMessage The pending message object.
        * @return void
        */
        void PanicWaitingClient( TInt aPanic, const RMessage2& aMessage ) const;

    private: // Data
        CConnMonSession*           iSession;
        TUint                      iPluginId;
        RArray< TUint >            iAttributesSupported;
        RArray< TClientRequest >   iClientRequests;

        // True if plugin has a request active
        TBool iPluginReqActive;

        // The request message plugin has sent
        RMessage2  iPluginReqMessage;

        // Pointer to the plugin memory area for the attribute request
        const TAny* iPluginReqBuffer;
    };

#endif //__CPLUGIN_H

// End-of-file
