/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Class definition for Flexible timer server. Server creates session
 * objects and manages message passing to those.
 *
 */
/*
 * %version: 1 %
 */
// Protection against nested includes
#ifndef FLEXTIMERSERVER_H
#define FLEXTIMERSERVER_H

// System includes
#include <e32base.h>

// Forward declarations
class CFlexTimerEngine;

// Class declaration
/**
 *  Flexible timer server class implements the server for FlexTimers. This
 *  class provides services for creating new sessions, and for passing
 *  a client message to dedicated session.
 */
class CFlexTimerServer : public CServer2
    {
    
public:
    /**
     * Two phased constructor 
     * 
     * @param aPriority Active object priority for the server 
     * @return Pointer to created server object
     */
    IMPORT_C static CFlexTimerServer* NewL( CActive::TPriority aPriority );

    /**
     * Destructor 
     */
    virtual ~CFlexTimerServer();

public:

    /**
     * From: CServer2
     * Function for creating a new session to this server. 
     *
     * @param aVersion Version identifier from client
     * @param aMessage Reference to creation message sent by client
     * @return Pointer to created session object
     */
    CSession2* NewSessionL( const TVersion& aVersion,
                            const RMessage2& aMessage ) const;

    /**
     * Function for creating server thread resources. Creates the server
     * object and active scheduler. Loops in the active scheduler to serve
     * client requests until explicitly shutdown. Function leaves if the
     * creation of needed resources fails.
     */
    static void StartServerL();

private:

    /**
     * Constructor
     * 
     * @param aPriority Active object priority 
     */
    CFlexTimerServer( CActive::TPriority aPriority );

    /**
     * 2nd phase constructor, creates CFlexTimerEngine
     */
    void ConstructL();

private:

    /**
     * Pointer to FlexTimerEngine created during server start-up
     */
    CFlexTimerEngine* iTimerEngine;

    };

#endif /* FLEXTIMERSERVER_H */
