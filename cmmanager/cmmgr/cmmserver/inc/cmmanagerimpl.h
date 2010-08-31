/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation of connection method manager.
*
*/


#ifndef CMMANAGERIMPL_H
#define CMMANAGERIMPL_H

#include <metadatabase.h>
#include <datamobilitycommsdattypes.h>
#include <cmmanagerdef.h>

#include "cmmserverdefs.h"

class CCmmTransactionHandler;
class CCmPluginBaseEng;
class CCmmCache;


NONSHARABLE_CLASS( CCmManagerImpl ) : public CBase
    {
public:
    /**
     * Two phased construction.
     */
    static CCmManagerImpl* NewL();

    /**
     * Destructor.
     */
    virtual ~CCmManagerImpl();

private:
    /**
     * Constructor.
     */
    CCmManagerImpl();

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

public:
    /**
     * Check the existence of tables we need. Creates if it is not exist, get
     * the table id otherwise.
     */
    void CheckTablesL();

    /**
     * Query all of the supported bearer types.
     *
     * @param aArray List of all supported bearer types.
     */
    void SupportedBearersL( RArray<TUint32>& aArray ) const;

    /**
     * Return the requested table ID.
     */
    CommsDat::TMDBElementId TableId( TCmmDbRecords aRecord );

    /**
     * Get a handle to the database cache manager.
     */
    CCmmCache& Cache();

    /**
     * Get a handle to the database transaction handler.
     */
    CCmmTransactionHandler* GetTransactionHandler();

    /**
     * Get a handle to database session.
     */
    CommsDat::CMDBSession& Session() const;

    /**
     * Return true is WLAN is supported on phone.
     */
    TBool WlanSupported() const;

private:
    /**
     * Build an array of all bearer specific plugins.
     */
    void BuildPluginArrayL();

private:
    // Database cache manager.
    CCmmCache* iCache; //Owned.

    // Array for bearer plugins.
    CArrayPtrFlat<const CCmPluginBaseEng>* iPlugins; // Owned.

    // Database transaction handler.
    CCmmTransactionHandler* iTrans; //Owned.

    // Destination network table ID.
    CommsDat::TMDBElementId iSnapTableId;
    // Global bearer type priorization table ID.
    CommsDat::TMDBElementId iBearerPriorityTableId;
    // Destination metadata table ID.
    CommsDat::TMDBElementId iDestMetadataTableId;
    // Default connection table ID.
    CommsDat::TMDBElementId iDefConnTableId;

    // Flag for feature manager initialization status.
    TBool iIsFeatureManagerInitialised;

    // True if phone supports WLAN.
    TBool iWLanSupport;
    };

#endif // CMMANAGERIMPL_H

// End of file
