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
 *      Session implementation for FlexTimerServerMonitor server
 */
/*
 * %version: 1 %
 */
// Protection against nested includes
#ifndef FLEXTIMERSERVERMONITORSESSION_H
#define FLEXTIMERSERVERMONITORSESSION_H

// System includes
#include <e32base.h>

// Forward declarations
class RFlexTimer;

// Class declaration
/**
 * Session for FlexTimerServerMonitor server.
 * Just receives requests from client and calls server's interface.
 */
class CFlexTimerServerMonitorSession : public CSession2
    {

public:

    /**
     * Two phase constructor
     */
    static CFlexTimerServerMonitorSession* NewL();

    /**
     * Destructor
     */
    virtual ~CFlexTimerServerMonitorSession();

    /**
     * From CSession2
     * Message handler servicing client requests. Interprets message type and
     * passes it to correct function for processing.
     * 
     * @param aMessage Message from client
     */
    void ServiceL( const RMessage2& aMessage );

private:

    /**
     * Constructor
     */
    CFlexTimerServerMonitorSession();

    /**
     * 2nd phase constructor
     */
    void ConstructL();

private: // Data

    };

#endif /* FLEXTIMERSERVERMONITORSESSION_H */
