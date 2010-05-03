/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Listens for changes in one CommsDat table through CenRep.
*
*/


#ifndef CMMDBCHANGELISTENER_H_
#define CMMDBCHANGELISTENER_H_

#include <e32base.h>

class CCmmListenerManager;
class CRepository;

// Stop requesting new notifications after this many consecutive errors
const TInt KCmmCenRepErrorThreshold = 50;


NONSHARABLE_CLASS( CCmmDbChangeListener ) : public CActive
    {
public:
    static CCmmDbChangeListener* NewL(
            CCmmListenerManager* aListenerManager,
            TUint32 aTableId );

    static CCmmDbChangeListener* NewLC(
            CCmmListenerManager* aListenerManager,
            TUint32 aTableId );

    virtual ~CCmmDbChangeListener();

private:
    CCmmDbChangeListener(
            CCmmListenerManager* aListenerManager,
            TUint32 aTableId );

    void ConstructL();

public:
    TUint32 GetTableId();
    TInt Start();

private: // Methods from base class
    void DoCancel();
    void RunL(); // Don't leave, or implement RunError() //TODO, implement RunError()

private:
    TInt RequestNotification();

private:
    // Manager object that handles all similar listeners.
    CCmmListenerManager* iListenerManager; // Not owned

    // Central Repository handle.
    CRepository* iRepository; // Owned

    // Table ID of the commsdat table to be listened.
    TUint32 iTableId;

    // Safeguard in case of CenRep errors.
    TInt iErrorCounter;
    };

#endif // CMMDBCHANGELISTENER_H_

// End of file
