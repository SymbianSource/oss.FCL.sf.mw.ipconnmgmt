/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active object that starts a connection stop operation in a new
*               thread and waits for it to complete.
*/

#ifndef CONNMONASYNCSTOPDAEMON_H
#define CONNMONASYNCSTOPDAEMON_H

class CConnMonIAP;
class CPsdFax;

const TInt KCmESockMessageSlots = 4;
const TInt KCmStopThreadStackSize = 1024;

NONSHARABLE_CLASS( TAsyncStopThreadData )
    {
public:
    TAsyncStopThreadData( TUint32 aIapId, TUint32 aNetId );

public:
    TUint32 iIapId;
    TUint32 iNetId;

    TInt iStep;      // Indicates location if error occurs. 
    TInt iErrorCode; // Error code that occured at location indicated by iStep.
    };


/**
 * Method to stop a connection in a separate thread. 
 */
TInt ConnectionStopThreadFunction( TAny *aPtr );


NONSHARABLE_CLASS( CConnMonAsyncStopDaemon ) : public CActive
    {
private:
    enum TConnMonStopType
        {
        EConnMonStopTypeUnknown = 0,
        EConnMonStopTypeInternal,    // Stoped with RConnection handle
        EConnMonStopTypeExternalPsd, // Stopped through CPsdFax-class
        EConnMonStopTypeExternalCsd  // Stopped through CCsdFax-class
        };

public:
    static CConnMonAsyncStopDaemon* NewL( CConnMonIAP* aIap );
    static CConnMonAsyncStopDaemon* NewLC( CConnMonIAP* aIap );
    virtual ~CConnMonAsyncStopDaemon();

private:
    CConnMonAsyncStopDaemon( CConnMonIAP* aIap );
    void Construct();

public:
    /**
     * Starts the asynchronous stop task for an internal connection. This is
     * executed in a separate thread.
     */
    TInt Start( TUint aConnectionId, TUint32 aIapId, TUint32 aNetId );

    /**
     * Starts the asynchronous stop task for an external connection. This is
     * executed as a normal asunchronous request.
     */
    TInt Start( const TUint aConnectionId, CPsdFax* aPsdFax );

private: // Methods from base class
    void DoCancel();
    void RunL();

private:
    CConnMonIAP* iIap;           // Not owned
    CPsdFax* iPsdFax;            // Not owned
    TAsyncStopThreadData* iData; // Owned
    TConnMonStopType iConnectionType;
    TUint iConnectionId;
    };

#endif // CONNMONASYNCSTOPDAEMON_H

// End of file
