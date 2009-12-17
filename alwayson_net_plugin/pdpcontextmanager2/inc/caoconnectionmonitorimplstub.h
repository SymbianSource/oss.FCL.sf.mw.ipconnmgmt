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
* Description:  ?
*
*/


#ifndef CAOCONNECTIONMONITORIMPLSTUB_H
#define CAOCONNECTIONMONITORIMPLSTUB_H

// INCLUDE FILES
#include <e32base.h>
#include <e32property.h>
#include <etel3rdparty.h>

// FORWARD DECLARATIONS
class MAOSettings;
class MAOConnectionMonitorObserver;
template<class T> class CAOAsyncWrapper;
class CPeriodic;

/**
 *  Connection monitor implementation.
 *  This is the real implementation for connection monitor.  
 *
 *  @lib PDPContextManager.lib
 *  @since S60 v3.1
 */
class CAOConnectionMonitorImpl : public CBase
    {
public: // Constructors & destructors

    /**
     * Two-phased constructor.
     *
     * @since S60 v3.1
     * @param aObserver ?
     * @param aSettings ?
     * @return ?
     */
    static CAOConnectionMonitorImpl* NewL(
        MAOConnectionMonitorObserver& aObserver,
        MAOSettings& aSettings );

    /**
     * Default destructor
     *
     * @since S60 v3.1
     */
    virtual ~CAOConnectionMonitorImpl();
        
    /**
     * Fetches network registration status.
     * Asynchronous.
     *
     * @since S60 v3.1
     */
    void FetchNetworkRegistrationStatus();
    
    /**
     * Fetches current network registration status.
     * Synchronous method.
     *
     * @since S60 v3.1
     * @return Telephony::TRegistrationStatus
     */
    CTelephony::TRegistrationStatus NetworkRegistration() const;
    
private: // New methods

    /**
     * Default constructor.
     *
     * @since S60 v3.1
     * @param aObserver ?
     * @param aSettings ?
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
     * NW reg status change property
     *
     * @since S60 v3.1
     */
    void SubscribeNWChangeProperty();
    
    /**
     * ?
     *
     * @since S60 v3.1
     * @param aStatus ?
     */
    void NWChangePropertyIssueRequest( TRequestStatus& aStatus );
    
    /**
     * ?
     *
     * @since S60 v3.1
     * @param aStatus ?
     */
    void NWChangePropertyRunL( TInt aStatus );
    
    /**
     * ?
     *
     * @since S60 v3.1
     */
    void NWChangePropertyDoCancel();
    
    /**
     * External PDP context property
     *
     * @since S60 v3.1
     */
    void SubscribeExternalPDPContextProperty();
    
    /**
     * ?
     *
     * @since S60 v3.1
     * @param aStatus ?
     */
    void ExternalPDPContextIssueRequest( TRequestStatus& aStatus );
    
    /**
     * ?
     *
     * @since S60 v3.1
     * @param aStatus ?
     */
    void ExternalPDPContextRunL( TInt aStatus );
    
    /**
     * ?
     *
     * @since S60 v3.1
     */
    void ExternalPDPContextDoCancel();
    
    /**
     * New PDP context property
     *
     * @since S60 v3.1
     */
    void SubscribeNewPDPContextProperty();
    
    /**
     * ?
     *
     * @since S60 v3.1
     * @param aStatus ?
     */
    void NewPDPContextIssueRequest( TRequestStatus& aStatus );
    
    /**
     * ?
     *
     * @since S60 v3.1
     * @param aStatus ?
     */
    void NewPDPContextRunL( TInt aStatus );
    
    /**
     * ?
     *
     * @since S60 v3.1
     */
    void NewPDPContextDoCancel();
    
    /**
     * NW reg status fetching
     *
     * @since S60 v3.1
     * @param aSelf ?
     */
    static TInt NWRegStatusFetchCallBack( TAny* aSelf );
    
    /**
     * ?
     *
     * @since S60 v3.1
     */
    void HandleNWRegStatusFetchedL();
    
private: // Data

    /**
     * Ref: Not used
     */
    MAOConnectionMonitorObserver& iObserver;
    
    /**
     * Ref: Settings
     */
    MAOSettings& iSettings;

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
     * NW reg status change property
     */
    RProperty iNWRegStatusChangeProperty;
    
    /**
     * Own: ?
     */
    CAOAsyncWrapper<CAOConnectionMonitorImpl>*
        iNWRegStatusChangeSubscriber;
        
    /**
     * External PDP context property
     */
    RProperty iExternalPDPContextProperty;
    
    /**
     * Own: ?
     */
    CAOAsyncWrapper<CAOConnectionMonitorImpl>*
        iExternalPDPContextSubscriber;
        
    /**
     * New PDP context property
     */
    RProperty iNewPDPContextProperty;
    
    /**
     * Own: ?
     */
    CAOAsyncWrapper<CAOConnectionMonitorImpl>*
        iNewPDPContextSubscriber;
        
    /**
     * NW reg status fetcher
     */
    CPeriodic* iNWStatusFetcher;
    };
    
#endif /* CAOCONNECTIONMONITORIMPLSTUB_H */
