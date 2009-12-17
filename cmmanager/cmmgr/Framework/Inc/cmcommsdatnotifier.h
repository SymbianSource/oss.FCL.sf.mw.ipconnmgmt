/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CMCOMMSDATNOTIFIER_H
#define CMCOMMSDATNOTIFIER_H

_LIT( KIapDisappear, "Iap disappears from CommsDat" );
_LIT( KDestDisappear, "Destination disappears from CommsDat" );

class CCmManagerImpl;
class CRepository;
class MCmCommsDatWatcher;

// A stack to store watchers
typedef CArrayPtrFlat<MCmCommsDatWatcher> CCmCallBackArray;

// Used to notify parent view what happens during son's view editing.
const TInt KCmNotifiedNone = 0;
const TInt KCmNotifiedDestinationDisappear = 1;
const TInt KCmNotifiedIapIsNotInThisDestination = 2;
const TInt KCmNotifiedIapDisappear = 3;

class MCmCommsDatWatcher
    {
public:
    // watcher should implement this function to get the notification of CommsDat changes.
    // see class CCmManagerImpl for watcher registration
    virtual void CommsDatChangesL() = 0;
    };

// Used for son's watcher to notify parent watcher that CommsDat has changes
// if farther watcher wants to know what heppens when son's watcher exiting
class CCmParentViewNotifier
    {
public:
    virtual void NotifyParentView( TInt aValue ) = 0;
    };

/*
 * CommsDat can suddenly change with some reasons, e.g., an application may suddenly add/delete a destination
 * and Iap. Those changes may affect CmManager. This class will listen to changes in one CommsDat table
 * through CenRep and inform related watcher that is interesting in CommsDat changes.
 */
NONSHARABLE_CLASS( CCmCommsDatNotifier ) : public CActive
    {
public:

    static CCmCommsDatNotifier* NewL( TUint32 aTableId );

    static CCmCommsDatNotifier* NewLC( TUint32 aTableId );

    virtual ~CCmCommsDatNotifier();
    
    void WatcherRegisterL( MCmCommsDatWatcher* aWatcher );
    void WatcherUnRegister( );

private:
    CCmCommsDatNotifier( TUint32 aTableId );

    void ConstructL();
    TInt RequestNotifications();
    
    void DeleteCallBackArray();

private: // Methods from base class
    void DoCancel();
    void RunL();

private:
    TUint32 iTableId;
    CRepository* iRepository;
    MCmCommsDatWatcher* iWatcher; // callback object that wants to see changes in CommsDat
    CCmCallBackArray* iCallBackArray; // used as stack to go to the next view
    };

#endif // CMCOMMSDATNOTIFIER_H

// End-of-file
