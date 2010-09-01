/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Client interface to MPM Default Connection server
*
*/

#ifndef RMPMDEFAULTCONNECTION_H
#define RMPMDEFAULTCONNECTION_H

#include <e32std.h>

/**
 *  Cleant API for MPM Default Connection server
 *  services.
 *
 *  Enables registering IAP to be used as default connection. 
 *  Only one client may be connected at a time 
 *  @code
 *  RMPMDefaultConnection rmpmdc;
 *  TInt err = KErrNone;
 *  //connect and set default IAP
 *  err = rmpmdc.Connect();
 *  err = rmpmdc.SetDefaultIAP( iapId );
 *
 *  //when IAP no longer used as default
 *  err = rmpmdc.ClearDefaultIAP();
 *  rmpmdc.Close();
 *  @endcode
 *
 *  @lib mpmdefaultconnection.dll
 *  @since S60 v3.2
 */
class RMPMDefaultConnection : public RSessionBase
    {

public:

    /**
     * Constructor.
     *
     * @since S60 v3.2
     */
    inline RMPMDefaultConnection() : RSessionBase(), 
    iClientConnected( EFalse )
    {};

    /**
     * Creates connection to server
     *
     * @since S60 v3.2
     * @return KErrNone if successful, otherwise one of the
     * system-wide error codes
     */
    IMPORT_C TInt Connect();

    /**
     * Closes connection to server
     * If IAP has been registered by the client it is also deregistered here
     *
     * @since S60 v3.2
     */
    IMPORT_C void Close();

    /**
     * Sets IAP to be used as default connection
     *
     * @since S60 v3.2
     * @param aIapId CommsDat record id of the IAP
     * @return KErrNone if successful, otherwise one of the
     * system-wide error codes
     */
    IMPORT_C TInt SetDefaultIAP( TInt aIapId );


    /**
     * Clears IAP from being used as default connection
     *
     * @since S60 v3.2
     * @param aIapId CommsDat record id of the IAP
     * @return KErrNone if successful, otherwise one of the
     * system-wide error codes
     */
    IMPORT_C TInt ClearDefaultIAP();
    
    /**
     * Returns MPM Default Connection server version.
     * @since 3.2
     * @return Version of the MPM Default Connection server
     */
     IMPORT_C TVersion Version() const;
    
private: // data

    /**
     * Is client connected to server or not
     */
     TBool iClientConnected;    

    };

#endif // RMPMDEFAULTCONNECTION_H
