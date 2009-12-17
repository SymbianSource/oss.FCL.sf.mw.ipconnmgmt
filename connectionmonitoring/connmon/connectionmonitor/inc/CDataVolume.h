/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Issues RConnection::DataTransferredRequest() and sends the
*                data volumes to sessions.
*
*/

#ifndef CONNMON_CDATAVOLUME_H
#define CONNMON_CDATAVOLUME_H

class CConnMonServer;

/**
*  CDataVolume
*  @lib CONNMON.LIB
*  @since
*/
NONSHARABLE_CLASS( CDataVolume ) : public CActive
    {
    public: // Constructors and destructor
        CDataVolume(
                CConnMonServer* aServer,
                RConnection* aConnection,
                const TUint& aConnectionId );
        void Construct();
        ~CDataVolume();


    public: // New methods
        /**
        * Requests an asyncronous service (data volumes) from RConnection.
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
        * From CActive Handles the data that has arrived from RConnection.
        * @since
        * @param
        * @return void
        */
        void RunL();

    private: // Data
        CConnMonServer* iServer;       // Not owned
        RConnection*    iConnection;   // Not owned
        const TUint     iConnectionId;
        TUint           iDlData;
        TPckg<TUint>    iPckgDlData;
        TUint           iUlData;
        TPckg<TUint>    iPckgUlData;
    };

#endif // CONNMON_CDATAVOLUME_H

// End-of-file
