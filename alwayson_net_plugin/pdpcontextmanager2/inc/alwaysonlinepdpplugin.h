/*
* Copyright (c) 2004,2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements the CAlwaysOnlineEComInterface interface
*
*/


#ifndef C_CALWAYSONLINEPDPPLUGIN_H
#define C_CALWAYSONLINEPDPPLUGIN_H

// INCLUDE FILES
#include <e32base.h>
#include <AlwaysOnlineEComInterface.h>

// FORWARD DECLARATIONS
class CAOServer;

/**
*  Implements the CAlwaysOnlineEComInterface interface
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
NONSHARABLE_CLASS( CAlwaysOnlinePDPPlugin ):
    public CAlwaysOnlineEComInterface
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since S60 v3.1
     * @return pointer to the created CAlwaysOnlinePDPPlugin object
     */
    static CAlwaysOnlinePDPPlugin* NewL();

    /**
     * Destructor
     *
     * @since S60 v3.1
     */
    virtual ~CAlwaysOnlinePDPPlugin();

    /**
     * Interface function from CAlwaysOnlineEComInterface
     * Command ID's can be found in alwaysonlinemanagercommon.h
     * Handles the command coming from the Always Online Manager
     *
     * @since S60 v3.1
     * @param aCommand The command as an integer
     * @param aParameters A Buffer containing parameters
     * @return TAny A return value, KErrNone if successfull
     *           KErrNotSupported if command wasn't supported
     */
    TAny* HandleServerCommandL( TInt aCommand, TDesC8* aParameters );

private:    // New methods

    /**
     * Constructor.
     *
     * @since S60 v3.1
     */
    CAlwaysOnlinePDPPlugin();

    /**
     * Default Symbian second phase constructor
     *
     * @since S60 v3.1
     */
    void ConstructL();

private:    // Data

    /**
     * Always-On Server
     * Own
     */
    CAOServer* iAlwaysOnServer;
    };

#endif // C_CALWAYSONLINEPDPPLUGIN_H
