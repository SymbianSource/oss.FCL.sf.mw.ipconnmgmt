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
* Description:  Provides lingering functionality for not-always-on connections.
*
*/


#ifndef CLINGERCONNECTION_H
#define CLINGERCONNECTION_H

#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>
#include <es_enum.h>

class MAOSettings;

/**
 *  Monitors and keeps open/stops not-always-on connections.
 *
 *  Keeps not-always-on connection open for a lingering period.
 *  (defined by T_notAlwaysOn) after connection has gone idle.
 *  Then stops connection if it is still idle.
 *
 *  @lib pdpcontextmanager2
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CLingerConnection ) : public CBase
    {
public:

    /** Possible status values */
    enum TLingerStatus
        {
        ELingerStopped,
        ELingerAttached,
        ELingerRunning
        };

    /**
     * Two-phased constructor.
     * @param aConnectionInfo Connection id.
     * @param aConnectionInfo Iap id and net id of the connection
     * @param aSettings Provides timer settings
     * @param aSocketServ Pointer to socket server
     */
    static CLingerConnection* NewL(
          TUint            aConnectionId,
          TConnectionInfo  aConnectionInfo, 
          MAOSettings&     aSettings, 
          RSocketServ*     aSocketServ );
          
    static CLingerConnection* NewLC(
          TUint            aConnectionId, 
          TConnectionInfo  aConnectionInfo, 
          MAOSettings&     aSettings, 
          RSocketServ*     aSocketServ );      
    
    /**
    * Destructor.
    */
    virtual ~CLingerConnection();
    
    /**
     * Attaches to connection and starts linger timer
     *
     * @since S60 v3.2
     * @return error code in case of error else KErrNone
     */
    TInt StartLinger();
    
    /**
     * Stops linger timer and detaches from connection
     *
     * @since S60 v3.2
     * @return void
     */
    void StopLinger();

    /**
     * Stops connection
     *
     * @since S60 v3.2
     * @return error code in case of error else KErrNone
     */
    TInt StopConnection();
    
    /**
     * Refreshes settings from CommsDat
     *
     * @since S60 v3.2
     * @return void
     */
    void HandleSettingsChanged();
    
    /**
     * Returns connection id
     *
     * @since S60 v3.2
     * @return connection id
     */
    TUint ConnectionId() { return iConnectionId; };

    /**
     * Returns iap id
     *
     * @since S60 v3.2
     * @return iap id
     */
    TUint IapId() { return iConnectionInfo.iIapId; };
    
    /**
     * Returns current status
     *
     * @since S60 v3.2
     * @return current status
     */
    TInt Status();

protected:

private:

    /**
     * Default constructor.
     * @param aConnectionInfo Connection id.
     * @param aConnectionInfo Iap id and net id of the connection
     * @param aSettings Provides timer settings
     * @param aSocketServ Pointer to socket server
     */
    CLingerConnection( TUint           aConnectionId,
                       TConnectionInfo aConnectionInfo,
                       MAOSettings&    aSettings, 
                       RSocketServ*    aSocketServ);

    void ConstructL();

    /**
     * Opens and attaches to RConnection object to keep
     * the connection open. 
     *
     * @since v3.2
     * @return error code in case of error else KErrNone
     */
    TInt OpenAndAttach();
    
    /**
     * Closes RConnection object.
     *
     * @since v3.2
     * @return void
     */
    void CloseConnection();

    /**
     * Checks if there has been any data transfer throuh the connection
     *
     * @since S60 v3.2
     * @return ETrue if connection is idle, otherwise EFalse.
     */
    TBool IsConnectionIdle();
    
    /**
     * Reads linger settings.
     *
     * @since S60 v3.2
     * @return void
     */
    void ReadSettings();
    
     /**
     * Starts a periodic timer
     *
     * @since S60 v3.2
     * @param aTimerInterval Timer  interval in seconds
     * @param aReset Resets timer call count to zero when ETrue
     * @return void
     */
    void StartTimer( TInt aTimerInterval, TBool aReset=ETrue );
    
    /**
     * Stops periodic timer
     *
     * @since S60 v3.2
     * @return void
     */
    void CancelTimer();
    
    /**
     * Timer callback
     *
     * @since v3.2
     * @param aSelf the caller
     * @return error code in case of error else KErrNone
     */
    static TInt TimerCallBack( TAny* aSelf );

private: // data

     /**
     * Connection id (from Connection Monitor)
     */
    TUint iConnectionId;
    
     /**
     * Identifies connection (Iap id & net id)
     */
    TConnectionInfo iConnectionInfo;
    
    /**
     * Reference to Settings object
     */
    MAOSettings& iSettings;

    /**
     * Pointer to socket server
     * Not own.
     */
    RSocketServ* iSocketServ;
    
    /**
     * Tells whether connection is linger mode or not
     */
    TBool iLingering;
    
    /**
     * Tells whether connection is attached or not
     */
    TBool iAttached;
    
    /**
     * Latest downlink data transferred
     */
    TUint iDlData;
    
    /**
     * Packaged downlink data 
     */
    TPckg< TUint >  iPckgDlData;
    
    /**
     * Latest uplink data transferred
     */
    TUint iUlData;
    
    /**
     * Packaged uplink data
     */
    TPckg< TUint >  iPckgUlData;
    
    /**
     * Connection object for keeping the connetion open
     * and for stopping the connection when needed.
     */
    RConnection iConnection;
    
    /**
     * Idle/linger timer
     * Own.
     */
    CPeriodic* iTimer;

    /**
     * Linger interval in seconds
     */
    TInt iLingerInterval;
    
     /**
     * Linger timer call count
     */
    TInt iLingerTimerCount;

    /**
     * Currently used timer interval
     */
    TInt iCurrentTimerInterval;
    
    };

#endif // CLINGERCONNECTION_H
