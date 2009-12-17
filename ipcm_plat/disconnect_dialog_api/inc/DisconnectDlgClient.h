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
* Description:  Declares Disconnect Dialog Client classes.
*
*/



#ifndef __DDLG_DIALOG_CLIENT_H__
#define __DDLG_DIALOG_CLIENT_H__

// INCLUDES
#include <e32std.h>
#include <rconnmon.h>

// CONSTANTS


// Version of this API
const TUint KDisconnectDlgMajorVersionNumber = 1;     ///< Major version number
const TUint KDisconnectDlgMinorVersionNumber = 1;     ///< Minor version number
const TUint KDisconnectDlgBuildVersionNumber = 1;     ///< Build version number

// CLASS DECLARATION

/**
 * Defines preferences of the connection
 */
class TDisconnectConnectionPrefs
    {
    public:
        TUint                 iConnId;     // id of icon represents connection
        TInt                  iBearerType; // bearer type of connection
        TConnMonClientEnumBuf iClientInfo; // info from clients, use connection
        TName                 iIapName;    // Owned. Access point name.
        TBool                 iPrompt;
    };


/**
 * RDisconnectDlgServer. 
 */
class RDisconnectDlgServer : public RSessionBase
    {
    public:
        /**
        * Constructor.
        */
        IMPORT_C RDisconnectDlgServer();

        /**
        * Destructor.
        */
        IMPORT_C ~RDisconnectDlgServer();

        /**
        * Version of the dialog server API.
        * @return The API version.
        */
        IMPORT_C TVersion Version() const;

        /**
        * Connect to the notifier server. Must be called before any other 
        * function (c'tor excepted).
        * @return KErrNone if connection succeeded and a standard error code 
        * otherwise.
        */
        IMPORT_C TInt Connect();

        /**
        * Disconnect from the notifier server.
        */
        IMPORT_C void Close();

    public:

        /**
        * Starts the notifier to DisconnectDlg.
        * @param TRequestStatus status of notifier
        * @param aShowQuery indicates, if show a information query before,
        * connections are shown
        */
        IMPORT_C void DisconnectDlg( TRequestStatus& aStatus, 
                                     const TBool aShowQuery = EFalse );
                                 
                                 
        /**
        * Starts the notifier to DisconnectDlg.
        * @param aStatus status of notifier
        * @param aIapId IAP of the closed connection
        * @param aShowQuery indicates, if to show a information query before,
        * connections are shown
        */
        IMPORT_C void DisconnectDlg( TRequestStatus& aStatus, 
                                     TUint32& aIapId,
                                     const TBool aShowQuery = EFalse );

        /**
        * Cancels the notifier.
        */
        IMPORT_C void CancelDisconnectDlg();

    private:

        // Pointer to the client interface
        RNotifier* iNotifier;

        // Passed information to Notifier
        TPckgBuf<TDisconnectConnectionPrefs> iPassedInfo;
        
        // IAP id of the closed connection
        TPckg<TUint32> iIapId;
    };

#endif /* __DDLG_DIALOG_CLIENT_H__ */

// End of File
