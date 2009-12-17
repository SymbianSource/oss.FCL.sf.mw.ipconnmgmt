/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides information on CSD fax calls.
*
*/

#ifndef __CCSDFAX_H
#define __CCSDFAX_H

#include <etel.h>
#include <etelmm.h>
#include <rconnmon.h>

#include "ConnMonDef.h"

class CConnMonServer;
class CCsdFaxUpNotifier;
class CCsdStatusNotifier;

/**
*  CCsdFax
*  @lib CONMONN.LIB
*  @since
*/
NONSHARABLE_CLASS( CCsdFax ) : public CBase
    {
    public:
        // Constructors and destructor
        CCsdFax(
                CConnMonServer* aServer,
                RTelServer* aTelServer,
                RMobilePhone* aMobilePhone );

        void ConstructL();

        ~CCsdFax();

        enum TCsdFaxState
        {
        EModuleClosed = 0,
        ELineOpen,
        ECallOpen
        };

        /**
        * Checks if an external call exists.
        * @since
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt FindCall();

        /**
        * Opens an external data call.
        * @since
        * @param aName The name of the call.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt OpenCall( const TDes& aName );

        /**
        * Closes the external call.
        * @since
        * @return void
        */
        void CloseCall();

        /**
        * Checks if external CSD call is still valid.
        * @since
        * @return ETrue if context is valid, EFalse otherwise.
        */
        TBool IsValid();

        /**
        * Gets the fax bearer.
        * @since
        * @param aBearer On return, contains the bearer.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt GetBearer( TInt& aBearer, TBearerInfo& aBearerInfo ) const;

        /**
        * Gets the status of the external connection.
        * @since
        * @param aStatus On return, contains the connection status.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt GetStatus( TInt& aStatus ) const;

        /**
        * Maps the data call status to nifvar.h values
        * @since
        * @param aStatus On return, contains the mapped connection status (in/out).
        * @return KErrNone if successfull, otherwise a system wide error code.
        */

        TInt MapStatus( TInt& aStatus ) const;

        /**
        * Gets the dial up number of the external data call.
        * @since
        * @param aNumber On return, contains the dial up number.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt GetTelNumber( TDes& aNumber ) const;

        /**
        * Gets the start time of the connection.
        * @since
        * @param aTime On return, contains the start time.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt GetStartTime( TTime& aTime ) const;

        /**
        * Closes the external data call.
        * @since
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt Stop();

        /**
        * Removes connection from server tables.
        * @since
        * @return void.
        */
        void RemoveFromServer();

        void SetConnectionId( const TUint& aConnectionId ) { iConnectionId = aConnectionId; }
        TUint ConnectionId() const { return iConnectionId; }

    private: // Data
        CConnMonServer* iServer;

        // Pointer to ETel server
        RTelServer* iTelServer;

        // Pointer to mobile phone
        RMobilePhone* iMobilePhone;

        // "Data" line
        RLine iLine;

        // Open external data call
        RMobileCall iCall;

        // Name of the open external call
        TName iCsdFaxCallName ;

        // Connection ID
        TUint iConnectionId;

        // Tells fax module state
        TCsdFaxState iState;

        // Connection up event watcher
        CCsdFaxUpNotifier* iConnUpNotifier;

        // Connection status event watcher
        CCsdStatusNotifier* iStatusNotifier;
    };

/**
*  CCsdFaxUpNotifier
*  @lib CONNMON.LIB
*  @since
*/
NONSHARABLE_CLASS( CCsdFaxUpNotifier ) : public CActive
    {
    public: // Constructors and destructor
        CCsdFaxUpNotifier(
                CCsdFax* aFaxModule,
                CConnMonServer* aServer,
                RMobilePhone* aMobilePhone,
                RLine& aLine );
        void Construct();
        ~CCsdFaxUpNotifier();

    public:  // New methods
        /**
        * Requests a new event (connection up) from ETEL.
        * @since
        * @param
        * @return void
        */
        void Receive();

    private: // Methods from base classes
        /**
        * From CActive Cancels the asyncronous request
        * @since
        * @param
        * @return void
        */
        void DoCancel();

        /**
        * From CActive Handles the the event that has arrived from ETEL.
        * @since
        * @param
        * @return void
        */
        void RunL();

    private: // Data
        CCsdFax*        iFaxModule;
        CConnMonServer* iServer;
        RMobilePhone*   iMobilePhone;
        RLine&          iLine;
        TBuf<KMaxName>  iName;
        TEventInfo      iEventInfo;
    };

/**
*  CCsdStatusNotifier
*  @lib CONNMON.LIB
*  @since
*/
NONSHARABLE_CLASS( CCsdStatusNotifier ) : public CActive
    {
    public: // Constructors and destructor
        CCsdStatusNotifier(
                CCsdFax* aFaxModule,
                CConnMonServer* aServer );
        void Construct();
        ~CCsdStatusNotifier();

    public:  // New methods
        /**
        * Starts listening to status change events from ETEL.
        * @since
        * @param aCall The call whose status messages will be watched to.
        * @return void
        */
        void Start( RMobileCall* aCall );

        /**
        * Requests a new event (status changed) from ETEL.
        * @since
        * @param
        * @return void
        */
        void Receive();

    private: // Methods from base classes
        /**
        * From CActive Cancels the asyncronous request
        * @since
        * @param
        * @return void
        */
        void DoCancel();

        /**
        * From CActive Handles the the event that has arrived from ETEL.
        * @since
        * @param
        * @return void
        */
        void RunL();

    private: // Data
        CCsdFax*                       iFaxModule;
        CConnMonServer*                iServer;
        RMobileCall*                   iCall;
        RMobileCall::TMobileCallStatus iMobileCallStatus;
        TEventInfo                     iEventInfo;
    };

#endif // __CCSDFAX_H

// End-of-file
