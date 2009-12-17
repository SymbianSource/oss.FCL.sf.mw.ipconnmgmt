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
* Description:  Defines MAOConnectionManagerObserver interface
*
*/


#ifndef M_MAOCONNECTIONMANAGEROBSERVER_H
#define M_MAOCONNECTIONMANAGEROBSERVER_H

// INCLUDE FILES
#include "maoconnectionobserver.h"
#include "maoconnectionmonitorobserver.h"

// FORWARD DECLARATIONS

/**
*  Connection manager observer.
*  This interface combines MAOConnectionObserver and
*  MAOConnectionMonitorObserver interfaces together.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
class MAOConnectionManagerObserver : public MAOConnectionObserver,
                                     public MAOConnectionMonitorObserver
    {
protected:

    /**
     * Prohibit destruction
     *
     * @since S60 v3.1
     */
    virtual ~MAOConnectionManagerObserver(){};
    };

#endif // M_MAOCONNECTIONMANAGEROBSERVER_H
