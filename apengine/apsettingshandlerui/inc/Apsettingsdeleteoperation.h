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
* Description:  Declaration of class CApSettingsDeleteOperation.
*
*/


#ifndef APSETTINGS_DELETE_OPERATION_H
#define APSETTINGS_DELETE_OPERATION_H

// INCLUDE FILES
#include <ApDataHandler.h>
#include <AknProgressDialog.h>
// CONSTANTS

// DATA TYPES

// FORWARD DECLARATION
class CActiveApDb;
class CApSettingsModel;
class CAknWaitDialog;
class CErrorUI;

// CLASS DECLARATION

/**
* Model for the access point settings ui.
*/
NONSHARABLE_CLASS( CApSettingsDeleteOperation ) : 
                    public CActive, public MProgressDialogCallback
    {
    public:
        enum TState             /// State of the finite state machine.
            {
            EProcOnDialogOn,
            EProcOffDialogOn,
            EProcOnDialogOff,
            EProcOffDialogOff
            };

    public:     // Construct / destruct
        /**
        * Two-phased constructor. Leaves on failure.
        * @param aNoteControlId The ID of the note control.
        * @param aResourceID The resource ID of the wait note dialog.
        * @param aUid The UID of the access point to delete.
        * @param aNoteText The label for the note text.
        * @return The constructed CApSettingsDeleteOperation object.
        */
        static CApSettingsDeleteOperation* NewL(
                                TInt aNoteControlId,
                                TInt aResourceID,
                                TUint32 aUid,
                                const TDesC& aNoteText
                                );


        /**
        * Destructor.
        */
        virtual ~CApSettingsDeleteOperation();


    protected:  // new functions
        /**
        * Constructor.
        * @param aNoteControlId The ID of the note control.
        * @param aResourceID The resource ID of the wait note dialog.
        * @param aUid The UID of the access point to delete.
        */
        CApSettingsDeleteOperation(
                                TInt aNoteControlId,
                                TInt aResourceID,
                                TUint32 aUid
                                );

        /**
        * Second phase constructor. Leaves on failure.
        * @param aNoteText The desired text of the note.
        * events.
        */
        void ConstructL( const TDesC& aNoteText );

    protected:  // from CActive
        /**
        * Run a step of the observer test.
        */
        void RunL();

        /**
        * Cancel protocol implementation.
        * must be present although does nothing
        * as it is pure virtual in base...
        */
        void DoCancel();

        /**
        * Callback for notification of the dismissal of the wait note dialog.
        * From MProgressDialogCallback.
        * The passed aButtonId is ignored.
        */
        virtual void DialogDismissedL( TInt aButtonId );

    public:     // new methods

        /**
        * Start the operation.
        */
        void StartL();


        /**
        * Moves into next wait round and forces RunL to execute.
        */
        void Next();


        /**
        * Starts the wait note with the current params.
        */
        void StartWaitNoteL();


        /**
        * Starts the separate delete operation thread with the current params.
        */
        void StartDeleteOperationThreadL();


        /**
        * Separate thread for the delete operation.
        * @param aPtr   A pointer to the operation to be started in the thread
        * @return The return code of the delete operation
        */
        static TInt RunDeleteThread( TAny* aPtr );


        /**
        * Separate thread for the delete operation.
        * @param aop   A pointer to the operation to be started in the thread
        * @return The return code of the delete operation
        */
        static TInt DoRunDeleteThreadL( CApSettingsDeleteOperation* aop );


        /**
        * Starts a transaction on the given database
        * @param aDb Pointer to the database object
        * @param aWrite boolean indicating whether write or read lock is needed
        * @param aRetry boolean indicating whether retry is needed
        * @return Whether the transaction is our own or not
        */
        static TBool StartTransactionLC
                            (
                            CCommsDatabase* aDb,
                            TBool aWrite,
                            TBool aRetry /* ETrue */
                            );


        /**
        * Gets write lock for the given database
        * @param aDb Pointer to the database object
        */
        static void GetWriteLockL( CCommsDatabase* aDb );


        /**
        * Cleanup function for the transaction.
        * In case of leave, it will be called and
        * transaction will be rolled back
        */
        static void RollbackTransactionOnLeave( TAny* aDb );


        /**
        * Get the list of VPN WAP Access point UID's which are linked to a 
        * specific normal internet access point WAP ID.
        * @param aDb a reference of a CCommsDatabase database object 
        * to operate on
        * @param aUid a TUint32 containing the WAP UID of the normal 
        * internet access point
        * @param aVpnList a TUint32 dynamic array passed by reference to 
        * hold the WAP UID's of the linked VPN access points.
        */
/*
        static void GetLinkedVpnAccessPointsL( CCommsDatabase& aDb,
                                        TUint32 aUid, 
                                        CArrayFixFlat<TUint32> &aVpnList );
*/

        /**
        * Reads up an integer from the given table
        * @param atable The CCommsDbTableView* table to perform the operation
        * @param aColumn The column to read
        * @param aValue A TUint32 passed by reference to hold the readed value.
        * @return TInt Error code. Returns either 0 or KErrnotFound. 
        * Leaves on any other error
        */
        static TInt ReadUintL( CCommsDbTableView* atable,
                               const TDesC& aColumn,
                               TUint32& aValue );

        /**
        * Returns the current state.
        */
        TState State(){ return iState;};
        
        /**
        * Set a new UID
        */
        void SetUid( TUint32 aUid ) { iUid = aUid;};
        
        
        /**
        * Set a new note-text
        */
        void SetNoteTextL( const TDesC& aNoteText );

    private:    // data
        TInt                iNoteControlId;
        TInt                iResourceID;
        TUint32             iUid;
        RThread             iDeleteOperation;
        HBufC*              iNoteText;
        CAknWaitDialog*     iWaitDialog;
        TState              iState;
        CErrorUI*           iErrorUi;
        TInt                iErr;
    };


#endif

// End of file
