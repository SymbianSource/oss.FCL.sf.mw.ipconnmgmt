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
* Cache side object representing a connection method.
*
*/


#ifndef CMMCONNMETHODSTRUCT_H_
#define CMMCONNMETHODSTRUCT_H_

#include <e32base.h>

#include "cmmserverdefs.h"

class CCmmConnMethodInstance;
class CCmPluginBaseEng;


/**
 * Cache side object representing a connection method. Reflects the current
 * state in database.
 */
NONSHARABLE_CLASS( CCmmConnMethodStruct ) : public CBase
    {
public:
    static CCmmConnMethodStruct* NewL( const TUint32 aConnMethodId );
    static CCmmConnMethodStruct* NewLC( const TUint32 aConnMethodId );
    ~CCmmConnMethodStruct();

private:
    CCmmConnMethodStruct( TUint32 aConnMethodId );
    void ConstructL();

public:
    /**
     * Return the reference count.
     */
    TInt GetReferenceCounter();

    /**
     * Increase the reference counter by one.
     */
    void IncrementReferenceCounter();

    /**
     * Return the connection method ID.
     */
    TUint32 GetId() const;

    /**
     * Set the connection method ID.
     */
    void SetId( const TUint32 aConnMethodId );

    /**
     * Return bearer type.
     */
    TUint32 GetBearerType() const;

    /**
     * Gets the current status of this connection method struct.
     */
    TCmmConnMethodStatus GetStatus() const;

    /**
     * Gets the current record status of this connection method struct.
     */
    TCmmRecordStatus GetRecordStatus() const;

    /**
     * Sets a new status value.
     */
    void SetStatus( const TCmmConnMethodStatus& aStatus );

    /**
     * Reload the connection method data in this cache side object to be in
     * synch with the database(only if needed).
     */
    void ReloadPluginDataIfNeededL();

    /**
     * Decrease the reference counter by one. Return the remaining number of
     * references.
     */
    TInt ConnMethodInstanceClosed();

    /**
     * Set the connection method plugin pointer, bearer type and status.
     */
    void SetPlugin(
            CCmPluginBaseEng* aPlugin,
            TUint32 aBearerType,
            TCmmConnMethodStatus aStatus );

    /**
     * Returns the connection method plugin.
     */
    CCmPluginBaseEng* GetPlugin();

    /**
     * Called after this connection method has been updated and database
     * transaction has completed successfully. Sets the internal state of this
     * connection method structure to reflect the new valid state.
     */
    void UpdateSuccessful();

    /**
     * Called after this connection method has been deleted and database
     * transaction has completed successfully. Sets the internal state of this
     * connection method structure to reflect the new deleted state.
     */
    void DeleteSuccessful( const TUint32 aNewSecondaryId );

    /**
     * Notify about a possible change in database on specified record table.
     */
    void NotifyRecordChange( const TUint32 aRecordType );

private:
    /**
     * Set the record status.
     */
    void SetRecordStatus( const TCmmRecordStatus aStatus );

private:
    // The connection method plugin.
    CCmPluginBaseEng* iConnMethodPlugin; // Owned.

    // Total number of handles that the clients have to this connection method.
    TInt iReferenceCounter;

    // Connection method ID.
    // For new connection methods, while no valid database ID is available,
    // will be assigned a temporary ID that will have a value above
    // above KTemporaryIdCounterStart.
    TUint32 iConnMethodId;

    // Connection method bearer type.
    TUint32 iBearerType;

    // Current status of this connection method.
    TCmmConnMethodStatus iStatus;

    // Current status of the loaded commsdat records of this connection method.
    TCmmRecordStatus iRecordStatus;
    };

#endif // CMMCONNMETHODSTRUCT_H_

// End of file
