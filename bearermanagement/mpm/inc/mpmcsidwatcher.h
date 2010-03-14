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
* Description: Listen connect screen id key changes in central repository.
*
*/

#ifndef MPMCSIDWATCHER_H
#define MPMCSIDWATCHER_H

//  INCLUDES
#include <e32base.h>

class CRepository;

/**
 *  Class for accessing central repository
 *  Follows KMpmConnectScreenId key in central repository.
 *  @since 5.1
 */
class CMpmCsIdWatcher : public CActive
    {

public:

    static CMpmCsIdWatcher* NewL();

    /**
    * Destructor.
    */
    virtual ~CMpmCsIdWatcher();

    /**
     * Call this when you want to start listen event.
     * @since 5.1
     */
    void StartL();
    
    /**
     * Returns connect screen id.
     * @since 5.1
	 * @return Connect screen id
     */
    TUint32 ConnectScreenId() const;
    
// from base class CActive

    void RunL();

    TInt RunError( TInt aError );

    void DoCancel();

private:

    CMpmCsIdWatcher();

    void ConstructL();

private: // data

    /**
     * Id of the connect screen process
     */
    TInt iConnectScreenId;

    /**
     * Central repository handle
     * Own.
     */
    CRepository* iRepository;

    /**
     * Set when user connection is enabled
     */
    TBool iUserConnectionSupported;
    };

#endif // MPMCSIDWATCHER_H
