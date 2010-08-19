/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Listens for changes in one CommsDat table through CenRep.
*
*/

#ifndef CELLULARDATAUSAGEKEYUPDATER_H
#define CELLULARDATAUSAGEKEYUPDATER_H

class CConnMonServer;
class CRepository;
class TCmGenConnSettings;

// Stop requesting new notifications after this many consecutive errors
const TInt KCenRepErrorRetryCount = 80;

NONSHARABLE_CLASS( CCellularDataUsageKeyUpdater ) : public CActive
    {
public:
    static CCellularDataUsageKeyUpdater* NewL(
            CConnMonServer* aServer );

    static CCellularDataUsageKeyUpdater* NewLC(
            CConnMonServer* aServer );

    virtual ~CCellularDataUsageKeyUpdater();
    
    void UpdateKeyL( const TInt aRegistration ) const;

    /**
     * Return ETrue if dial-up PDP context override feature is enabled.
     */
    TBool DialUpOverrideEnabled() const;

private:
    CCellularDataUsageKeyUpdater(
            CConnMonServer* aServer );

    void ConstructL();
    TInt RequestNotifications();
    
    TCmGenConnSettings ReadGenConnSettingsL() const;
    TInt GetNetworkRegistration( TInt& aRegistration ) const;
    void UpdateKeyL() const;

private: // Methods from base class
    void DoCancel();
    void RunL();

private:
    CConnMonServer* iServer;
    TUint32         iTableId;

    // CommsDat central repository.
    CRepository*    iCommsRepository;

    // CmManager central repository.
    CRepository*    iCmmRepository;

    TInt            iErrorCounter;
    // used for complete runl. Not owned.
    TRequestStatus* iInitStatus;
    };

#endif // CELLULARDATAUSAGEKEYUPDATER_H

// End-of-file
