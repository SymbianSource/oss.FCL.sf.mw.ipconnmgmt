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
* Session side object representing a connection method.
*
*/


#ifndef CMMCONNMETHODINSTANCE_H_
#define CMMCONNMETHODINSTANCE_H_

#include <e32base.h>

#include "cmmserverdefs.h"

class CCmmSession;
class CCmmCache;
class CCmPluginBaseEng;
class CCmClientPluginInstance;
class CCmmConnMethodStruct;
class TCmmIdStruct;


/**
 * Session side object representing a connection method. Keeps track of any
 * changes the client makes before they are updated to database.
 */
NONSHARABLE_CLASS( CCmmConnMethodInstance ) : public CObject
    {
public:
    /**
     * NewL.
     */
    static CCmmConnMethodInstance* NewL( CCmmSession* aCmmSession, CCmmCache* aCache );

    /**
     * NewLC.
     */
    static CCmmConnMethodInstance* NewLC( CCmmSession* aCmmSession, CCmmCache* aCache );

    /**
     * Destructor.
     */
    ~CCmmConnMethodInstance();

private:
    /**
     * Constructor.
     *
     * @param aCache database cache.
     */
    CCmmConnMethodInstance( CCmmSession* aCmmSession, CCmmCache* aCache );

    /**
     * Second phase constructor.
     */
    void ConstructL();

public:
    /**
     * Get connection method ID.
     */
    TUint32 GetId() const;

    /**
     * Set connection method ID.
     */
    void SetId( const TUint32& aConnMethodId );

    /**
     * Return bearer type.
     */
    TUint32 GetBearerType() const;

    /**
     * Get handle ID.
     */
    TInt GetHandle() const;

    /**
     * Set handle ID.
     */
    void SetHandle( const TInt& aHandle );

    /**
     * Get pointer to the plugin data container.
     */
    CCmClientPluginInstance* GetPluginDataInstance() const;

    /**
     * Return ETrue, if this connection method represents an embedded destination.
     */
    TBool IsEmbeddedDestination() const;

    /**
     * Save the contents of this connection method into database. The attribute
     * aTemporaryHandle must be true if this connection instance was
     * temporarily created for the duration of this update operation only. 
     */
    void UpdateL( TBool aTemporaryHandle = EFalse );

    /**
     * Copy the relevant connection method information from cache side object.
     */
    void CopyDataL( CCmmConnMethodStruct* aConnMethodStruct );

    /**
     * Returns integer value identified by aAttribute. Some special cases
     * are handled in this module and the rest in plugin.
     * @param aAttribute Parameter which specifies the requested attribute.
     * @return Returns the value of the requested attribute if succees. Otherwise
     * leaves with KErrArgument
     */
    TUint32 GetIntAttributeL( const TUint32& aAttribute );

    /**
     * Returns integer value identified by aAttribute. Some special cases
     * are handled in this module and the rest in plugin.
     * @param aAttribute Parameter which specifies the requested attribute.
     * @return Returns the value of the requested attribute if succees. Otherwise
     * leaves with KErrArgument
     */
    TBool GetBoolAttributeL( const TUint32& aAttribute );

    /**
     * Returns 16 bit string value identified by aAttribute.
     * @param aAttribute Parameter which specifies the requested attribute.
     * @return Returns the value of the requested attribute if succees. Otherwise
     * leaves with KErrArgument
     */
    HBufC* GetStringAttributeL( const TUint32& aAttribute );

    /**
     * Returns 8 bit string value identified by aAttribute.
     * @param aAttribute Parameter which specifies the requested attribute.
     * @return Returns the value of the requested attribute if succees. Otherwise
     * leaves with KErrArgument
     */
    HBufC8* GetString8AttributeL( const TUint32& aAttribute );

    /**
     * Sets the integer value identified by aAttribute.
     * @param aAttribute Identifies the attribute to be set.
     * @param aValue The value to be set.
     * @return None
     */
    void SetIntAttributeL(
            const TUint32& aAttribute,
            const TUint32& aValue );

    /**
     * Sets the integer value identified by aAttribute.
     * @param aAttribute Identifies the attribute to be set.
     * @param aValue The value to be set.
     * @return None
     */
    void SetBoolAttributeL(
            const TUint32& aAttribute,
            const TBool& aValue );

    /**
     * Sets the 16 bit string value identified by aAttribute.
     * @param aAttribute Identifies the attribute to be set.
     * @param aValue The value to be set.
     * @return None
     */
    void SetStringAttributeL(
            const TUint32& aAttribute,
            const TDesC16& aValue );

    /**
     * Sets the 8 bit string value identified by aAttribute.
     * @param aAttribute Identifies the attribute to be set.
     * @param aValue The value to be set.
     * @return None
     */
    void SetString8AttributeL(
            const TUint32& aAttribute,
            const TDesC8& aValue );

    /**
     * Sets a new status value.
     */
    void SetStatus( const TCmmConnMethodStatus& aStatus );

    /*
     * Called after this connection method has been updated and database
     * transaction has completed successfully. Sets the internal state of this
     * connection method instance to reflect the new valid state.
     */
    void UpdateSuccessful();

    /**
     * Called after this connection method has been deleted and database
     * transaction has completed successfully. Sets the internal state of this
     * connection method structure to reflect the new deleted state.
     */
    void DeleteSuccessful( const TUint32& aNewSecondaryId );

    /**
     * After update/delete to database, refresh temporary ID to real ID if
     * necessary and refresh status information for any related handles for
     * all client sessions.
     */
    void RefreshHandlesForAllSessions( const TCmmIdStruct& aIdStruct );

    /**
     * Removes a connection method from any open destination handle in the same
     * session.
     */
    void RemoveConnMethodFromSessionDestinationHandles( const TUint32& aConnMethodId );

private:
    // Session that owns this instance.
    CCmmSession* iCmmSession; // Not owned.

    // Database cache.
    CCmmCache* iCache; // Not owned.

    // Pointer to original bearer plugin owned by cache.
    CCmPluginBaseEng* iPlugin; // Not owned.

    // Object to contain the data of this connection method instance.
    CCmClientPluginInstance* iPluginDataInstance; // Owned.

    // Connection method ID.
    TUint32 iConnMethodId;

    // Connection method bearer type.
    TUint32 iBearerType;

    // Connection method handle status.
    TCmmConnMethodStatus iStatus;

    // Subsession handle ID.
    TInt iHandle;
    };

#endif // CMMCONNMETHODINSTANCE_H_

// End of file
