/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  PSD fax connection information.
*
*/

#ifndef __CPSDFAX_H
#define __CPSDFAX_H

#include <etel.h>
#include <etelmm.h>
#include <etelpckt.h>
#include <pcktcs.h>
#include <rconnmon.h>

#include "ConnMonDef.h"

class CConnMonServer;
class CPsdFaxUpNotifier;
class CPsdFaxStatusNotifier;
class CPsdFaxConnectionData;

_LIT( KExternalName, "external" );

// max PSD connection number
const TInt KMaxPsdConnectionCount = 2;


/**
 * CPsdFax
 * @lib CONMONN.LIB
 */
NONSHARABLE_CLASS( CPsdFax ) : public CBase
    {
    public:
        // constructors and destructor
        CPsdFax(
                CConnMonServer* aServer,
                RTelServer* aTelServer,
                RMobilePhone* aMobilePhone,
                RPacketService* aPacketService );

        void ConstructL();
        ~CPsdFax();

        /**
        * Opens the PDP context.
        * @since
        * @param aIndex The index of the PSD connection data table.
        * @param aName The name of the context.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt OpenContext( const TUint aIndex, const TDes& aName );

        /**
        * Checks if external PDP context is still valid.
        * @since
        * @param aConnectionId The connection ID of the PSD connection.
        * @return ETrue if context is valid, EFalse otherwise.
        */
        TBool IsValid( const TUint aConnectionId );

        /**
        * Checks PSD connection data table index
        * @since
        * @param aConnectionId The connection ID of the PSD connection.
        * @return index if found, ohterwise otherwise a system wide error code.
        */
        TInt ConnectionIndex( const TUint aConnectionId );

        /**
        * Sets connection ID
        * @since
        * @param aIndex The connection index of the PSD connection.
        * @param aConnectionId The connection ID of the PSD connection.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt SetConnectionId( const TUint aIndex, const TUint aConnectionId );

        /**
        * Gets the fax bearer.
        * @since
        * @param aBearer On return, contains the bearer.
        * @param aBearerInfo On return, contains the detailed(new concept) bearer.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt GetBearer( TInt& aBearer, TBearerInfo& aBearerInfo );

        /**
        * Gets the access point name of the fax connection.
        * @since
        * @param aConnectionId The connection ID of the PSD connection.
        * @param aName On return, contains the access point name.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt GetApn( const TUint aConnectionId, TDes& aName );

        /**
        * Gets the protocol type of the fax connection.
        * @since
        * @param aConnectionId The connection ID of the PSD connection.
        * @param aProtocolType On return, contains the protocol type.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt GetProtocolType( const TUint aConnectionId, TInt& aProtocolType );

        /**
        * Gets the status of the fax connection.
        * @since
        * @param aConnectionId The connection ID of the PSD connection.
        * @param aStatus On return, contains the connection status.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt GetStatus( const TUint aConnectionId, TInt& aStatus );

        /**
        * Gets the amount of data sent and received (in bytes).
        * @since
        * @param aConnectionId The connection ID of the PSD connection.
        * @param aDLVolume On return, contains the amount received data.
        * @param aULVolume On return, contains the amount sent data.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt GetDataVolumes( const TUint aConnectionId, TUint& aDLVolume, TUint& aULVolume );

        /**
        * Gets the amount of data sent and received (in bytes).
        * @since
        * @param aConnectionId The connection ID of the PSD connection.
        * @param aTime On return, contains start time of the connection.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt GetStartTime( const TUint aConnectionId, TTime& aTime );

        /**
        * Stops the external connection.
        * @since
        * @param aConnectionId The connection ID of the PSD connection.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt Stop( const TUint aConnectionId );

         /**
        * Delete connections allocated recources.
        * @since
        * @return void
        */
        void DeleteConnections();

    private:

    private: // Data
        // Pointer to ConnMon server
        CConnMonServer* iServer;

        // Pointer to ETel server
        RTelServer* iTelServer;

        // Pointer to mobile phone
        RMobilePhone* iMobilePhone;

        // Pointer to the packet service
        RPacketService* iPacketService;

        // Connection up event watcher
        CPsdFaxUpNotifier* iConnUpNotifier;

        // Data table for each connection
        CPsdFaxConnectionData* iConnectionData[KMaxPsdConnectionCount];
    };


/**
 * CPsdFaxUpNotifier
 * @lib CONNMON.LIB
 */
NONSHARABLE_CLASS( CPsdFaxUpNotifier ) : public CActive
    {
    public: // Constructors and destructor
        CPsdFaxUpNotifier(
                CPsdFax* aFaxModule,
                CConnMonServer* aServer,
                RPacketService* aPacketService );

        void Construct();
        ~CPsdFaxUpNotifier();

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
        CPsdFax*        iFaxModule;
        CConnMonServer* iServer;
        RPacketService* iPacketService;
        TBuf<KMaxName>  iName;
        TEventInfo      iEventInfo;
    };


/**
 * CPsdFaxConnectionData
 * @lib CONNMON.LIB
 */
NONSHARABLE_CLASS( CPsdFaxConnectionData ) : public CBase
    {
    public: // Constructors and destructor
        CPsdFaxConnectionData(
                CPsdFax* aFaxModule,
                CConnMonServer* aServer,
                RPacketService* aPacketService );

        void ConstructL();
        TInt Construct();
        ~CPsdFaxConnectionData();

        /**
        * Opens the PDP context.
        * @since
        * @param aName The name of the context.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt OpenContext( const TDes& aName );

        /**
        * Closes the PDP context.
        * @since
        * @return void
        */
        void CloseContext();

        /**
        * Checks if the PDP context is open.
        * @since
        * @return ETrue if context is open, EFalse otherwise.
        */
        TBool IsOpen();

        /**
        * Gets the amount of data sent and received (in bytes).
        * @since
        * @param aDLVolume On return, contains the amount received data.
        * @param aULVolume On return, contains the amount sent data.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt GetDataVolumes( TUint& aDLVolume, TUint& aULVolume ) const;

        /**
        * Stops the external connection.
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

        /**
        * Sets connection ID.
        * @since
        * @param aConnectionId The connection ID of the PSD connection.
        * @return void.
        */
        void SetConnectionId( const TUint& aConnectionId );

        /**
        * Gets connection ID.
        * @since
        * @return connection ID.
        */
        TUint ConnectionId() const;

        /**
        * Resets connection start time.
        * @since
        * @return void.
        */
        void ResetStartTime();

        /**
        * Gets connection start time.
        * @since
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt GetStartTime( TTime& aTime ) const;

        /**
        * Gets the access point name of the fax connection.
        * @since
        * @param aName On return, contains the access point name.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt GetApn( TDes& aName );

        /**
        * Converts the access point name to correct format (UNICODE).
        * @since
        * @param aResult On return, contains the access point name in correct format.
        * @param aName Access point name in the original format (UNICODE).
        * @return void.
        */
        void ConvertAPN( TDes& aResult, const TDesC& aName ) const ;

        /**
        * Converts the access point name to correct format (UNICODE).
        * @since
        * @param aResult On return, contains the access point name in correct format.
        * @param aName Access point name in the original format (UTF8).
        * @return void.
        */
        void ConvertAPN( TDes& aResult, const TDesC8& aName ) const;

        /**
        * Gets the protocl type of the fax connection.
        * @since
        * @param aProtocolType On return, contains the protocol type.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt GetProtocolType( TInt& aProtocolType );

        /**
        * Gets the status of the fax connection.
        * @since
        * @param aStatus On return, contains the connection status.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt GetStatus( TInt& aStatus ) const;

        /**
        * Maps the PDP context status to nifvar values
        * @since
        * @param aStatus On return, contains the mapped connection status.
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt MapStatus( TInt& aStatus ) const;

        /**
        * Checks if the context is closed.
        * @since
        * @return ETrue if context is closed, EFalse otherwise.
        */
        TBool IsEnded();


    private:
        // Connection ID
        TUint iConnectionId;

        // ETrue if context is open
        TBool iOpen;

        // ETrue if start time is valid
        TBool iTimeValid;

        // ETrue if connection has been up and then ended (killed)
        TBool iEnded;

        // Start time
        TTime iStartTime;

        // Open fax context
        RPacketContext iContext;

        // pointer to PsdFax module
        CPsdFax* iFaxModule;

        // pointer to ConnMon server
        CConnMonServer* iServer;

        // pointer to packet service
        RPacketService* iPacketService;

        // Connection status event watcher
        CPsdFaxStatusNotifier* iStatusNotifier;
    };


/**
 * CPsdFaxStatusNotifier
 * @lib CONNMON.LIB
 */
NONSHARABLE_CLASS( CPsdFaxStatusNotifier ) : public CActive
    {
    public: // Constructors and destructor
        CPsdFaxStatusNotifier(
                CPsdFax* aFaxModule,
                CConnMonServer* aServer,
                RPacketService* aPacketService,
                CPsdFaxConnectionData* aConnDataModule );

        void Construct();
        ~CPsdFaxStatusNotifier();

    public:  // New methods
        /**
        * Starts listening to status change events from ETEL.
        * @since
        * @param aContext The open context to listen to.
        * @return void
        */
        void Start( const RPacketContext& aContext );

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

    private:  // New methods
        /**
        * Sends connection create event to client.
        * @since
        * @return KErrNone if successfull, otherwise a system wide error code.
        */
        TInt SendConnectionCreateEvent();

    private: // Data
        CPsdFax*                       iFaxModule;
        CConnMonServer*                iServer;
        RPacketService*                iPacketService;
        CPsdFaxConnectionData*         iConnDataModule;
        TEventInfo                     iEventInfo;
        RPacketContext                 iContext;
        RPacketContext::TContextStatus iContextStatus;

    };

#endif // __CPSDFAX_H

// End-of-file
