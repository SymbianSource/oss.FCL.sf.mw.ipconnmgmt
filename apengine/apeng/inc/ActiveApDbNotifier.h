/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Declares the CActiveApDbNotifier class.
*
*/


#ifndef ACTIVE_AP_DB_NOTIFIER_H
#define ACTIVE_AP_DB_NOTIFIER_H

//  INCLUDES

#include <e32base.h>
#include <ApEngineVer.h>


// FORWARD DECLARATIONS

class CActiveApDb;

// CLASS DECLARATION

/**
* Active object encapsulating an access point database notifier.
* Used by CActiveApDb.
*/
class CActiveApDbNotifier : public CActive
    {
    public:     // construct / destruct

        /**
        * Constructor.
        * @param aDb Active access point database to watch for.
        */
        CActiveApDbNotifier( CActiveApDb& aDb );

        /**
        * Destructor.
        */
        virtual ~CActiveApDbNotifier();

    protected:  // from CActive
        /**
        * Invoked when the outstanding request completes.
        */
        void RunL() ;

        /**
        * Implementation of the Cancel protocol;
        * cancel any outstanding request.
        */
        void DoCancel();

    public:     // new methods

        /**
        * Start the notifier. The notification request is automatically
        * renewed until it is cancelled, or the database is closed by
        * all clients (i.e. released by DBMS).
        */
        void Start();

        /**
        * Cancel and close the notifier now.
        */
        void Stop();

    private:
        /**
        * Request database change notification from the Access Point Engine.
        */
        void NotifyChange();

    private:    // data
        CActiveApDb* iActiveDb;     ///< Pointer to the database.
                                    ///< Does not owns it.
    };

#endif

// End of File
