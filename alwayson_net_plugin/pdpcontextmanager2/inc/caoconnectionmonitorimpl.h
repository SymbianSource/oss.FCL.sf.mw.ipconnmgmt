/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/


#ifndef CAOCONNECTIONMONITORIMPL_H
#define CAOCONNECTIONMONITORIMPL_H

// INCLUDE FILES
#include <e32base.h>
#include <rconnmon.h>
#include <etel3rdparty.h>

// FORWARD DECLARATIONS
class MAOSettings;
class MAOConnectionMonitorObserver;
class CConnMonEventBase;
template<class T> class CAOAsyncWrapper;
class CActiveSchedulerWait;

/**
 *  Connection monitor implementation.
 *  @see CAOConnectionMonitor
 *
 *  @lib PDPContextManager.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CAOConnectionMonitorImpl ):
    public CBase,
    public MConnectionMonitorObserver
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since S60 v3.1
     * @param aObserver ?
     * @param aSettings Commsdat settings
     */
    static CAOConnectionMonitorImpl* NewL(
        MAOConnectionMonitorObserver& aObserver,
        MAOSettings& aSettings  );

    /**
     * Default destructor
     *
     * @since S60 v3.1
     */
    virtual ~CAOConnectionMonitorImpl();
    
public:
    
    /**
     * @see CAOConnectionMonitor
     *
     * @since S60 v3.1
     */
    void FetchNetworkRegistrationStatus();
    
    /**
     * @see CAOConnectionMonitor
     *
     * @since S60 v3.1
     */
    CTelephony::TRegistrationStatus NetworkRegistration() const;
    

private: // New methods

    /**
     * Default constructor.
     *
     * @since S60 v3.1
     * @param aObserver ?
     * @param aSettings Commsdat settings
     */
    CAOConnectionMonitorImpl(
        MAOConnectionMonitorObserver& aObserver,
        MAOSettings& aSettings );

    /**
     * Default Symbian constructor
     *
     * @since S60 v3.1
     */
    void ConstructL();

    /**
     * Start listening nw registration
     *
     * @since S60 v3.1
     */
    void ListenNetworkRegistrationStatusChange();
    
    /**
     * Handle error situations
     *
     * @since S60 v3.1
     * @param aError ?
     */
    void HandleError( TInt aError );
    
    /**
     * Network registration status
     *
     * @since S60 v3.1
     * @param aStatus ?
     */
    void NWRegStatusIssueRequest( TRequestStatus& aStatus );
    
    /**
     * ?
     *
     * @since S60 v3.1
     * @param aStatus ?
     */
    void NWRegStatusRunL( TInt aStatus );
    
    /**
     * ?
     *
     * @since S60 v3.1
     */
    void NWRegStatusDoCancel();
    
    /**
     * ?
     *
     * @since S60 v3.1
     * @param aError ?
     * @return ?
     */
    TInt NWRegStatusRunError( TInt aError );

    /**
     * Network registration status
     *
     * @since S60 v3.1
     * @param aStatus ?
     */
    void NWRegStatusChangeIssueRequest( TRequestStatus& aStatus );
    
    /**
     * ?
     *
     * @since S60 v3.1
     * @param aStatus ?
     */
    void NWRegStatusChangeRunL( TInt aStatus );
    
    /**
     * ?
     *
     * @since S60 v3.1
     */
    void NWRegStatusChangeDoCancel();
    
    /**
     * ?
     *
     * @since S60 v3.1
     * @param aError ?
     * @return ?
     */
    TInt NWRegStatusChangeRunError( TInt aError );

    /**
     * Bearer type
     *
     * @since S60 v3.1
     */
    void FetchBearerType();
    
    /**
     * ?
     *
     * @since S60 v3.1
     */
    void CancelFetchBearerType();
    
    /**
     * ?
     *
     * @since S60 v3.1
     */
    void BearerIssueRequest( TRequestStatus& aStatus );
    
    /**
     * ?
     *
     * @since S60 v3.1
     */
    void BearerRunL( TInt aStatus );
    
    /**
     * ?
     *
     * @since S60 v3.1
     */
    void BearerDoCancel();
    
    /**
     * ?
     *
     * @since S60 v3.1
     */
    TInt BearerRunError( TInt aError );

// From base class MConnectionMonitorObserver

    /**
     * From MConnectionMonitorObserver
     *
     * @since S60 v3.1
     * @param aConnMonEvent ?
     */
    void EventL( const CConnMonEventBase &aConnMonEvent );

private: // Data

    /**
     * Ref: Observer
     */
    MAOConnectionMonitorObserver& iObserver;
    
    /**
     * Ref: Settings
     */
    MAOSettings& iSettings;
    
    /**
     * Own: Connection monitor
     */
    RConnectionMonitor iConnectionMonitor;
    
    /**
     * Own: Telephony
     */
    CTelephony* iTelephony;
    
    /**
     * Own: NW registration status package
     */
    CTelephony::TNetworkRegistrationV1Pckg iWNRegPckg;
    
    /**
     * Own: NW registration status
     */
    CTelephony::TNetworkRegistrationV1 iWNReg;
    
    /**
     * Own: NW registration status change package
     */
    CTelephony::TNetworkRegistrationV1Pckg iWNRegChangePckg;
    
    /**
     * Own: NW registration status change
     */
    CTelephony::TNetworkRegistrationV1 iWNChangeReg;
    
    /**
     * Own: Network registration status fetcher ao
     */
    CAOAsyncWrapper<CAOConnectionMonitorImpl>*
        iNWRegistrationStatus;

    /**
     * Own: Network registration status change handler ao
     */
    CAOAsyncWrapper<CAOConnectionMonitorImpl>*
        iNWRegistrationStatusChange;
    
    /**
     * Own: Bearer type fetcher
     */
    CAOAsyncWrapper<CAOConnectionMonitorImpl>*
        iBearer;
        
    /**
     * Own: New created connection id
     */
    TUint iMyConnectionId;
    
    /**
     * Own: Shows if the connection is beeing open.
     */
    TBool iConnectionAlive;
    
    /**
     * Own: Bearer type
     */
    TInt iBearerValue;
    
    /**
     * Own: Retry counter
     */
    TInt iCounter;
    };

#endif /* CAOCONNECTIONMONITORIMPL_H */
