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
* Description:  Declaration of Generic Connection Dialogs Notifier
*
*/


#ifndef __CONNECTIONDIALOGSNOTIFBASE_H__
#define __CONNECTIONDIALOGSNOTIFBASE_H__


// INCLUDES
#if !defined(__EIKNOTAPI_H__)
#include <eiknotapi.h>
#endif


// FORWARD DECLARATION


// CLASS DECLARATION
/**
 * Plugin base class
 */
NONSHARABLE_CLASS( CConnectionDialogsNotifBase ) : public CBase, public MEikSrvNotifierBase2
                                                   
    {
    public:

        /**
        * ~CConnectionDialogsNotifBase() destructor
        * @param  -
        * return  -
        */
        virtual ~CConnectionDialogsNotifBase();

        // From MEikSrvNotifierBase

        /**
        * Get Notifier information
        * @param  -
        * return TNotifierInfo
        */
        TNotifierInfo Info() const;

        /**
        * Start the Notifier
        * @param  aBuffer   Not used
        * return TPtrC8     Always NULL
        */
        TPtrC8 StartL( const TDesC8& aBuffer );

        /**
        * Cancel() the notifier
        * @param  -
        * return -
        */
        void Cancel();

        /**
        * Release the notifier
        * @param  -
        * return -
        */
        void Release();

        /**
        * Update Notifier
        * @param  aBuffer   Not used
        * return TPtrC8     Always NULL
        */
        TPtrC8 UpdateL( const TDesC8& aBuffer );

        /**
        * CompleteL the notifier is complete
        * @param  aStatus status
        * return  -
        */
        virtual void CompleteL( TInt aStatus ) = 0;

        inline void SetCancelledFlag( TBool aCancelled );

    protected:

        /**
        * CConnectionDialogsNotifBase() constructor
        * @param  -
        * return  -
        */
        CConnectionDialogsNotifBase();

        /**
        * CConnectionDialogsNotifBase() second level constructor
        * @param  -
        * return  -
        */
        void ConstructL( const TDesC& aResourceFileName,
                         const TBool aResourceFileResponsible );

    private:

        /**
        * CheckBusyL() check busy function
        * @param  -
        * return  -
        */
        void CheckBusyL();

    protected:
        TNotifierInfo iInfo;        // Notifier info

        RMessagePtr2 iMessage;      // Message
        TInt iReplySlot;            // Reply slot

        TBool iCancelled;           // ETrue if dialog cancelled

    private:
        TInt iResource;             // Resource
    };

#include "ConnectionDialogsNotifBase.inl"
    
#endif

// End of File
