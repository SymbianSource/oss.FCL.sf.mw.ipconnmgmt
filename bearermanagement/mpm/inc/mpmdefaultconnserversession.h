/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Session to MPM Default Connection server
*
*/

#ifndef CMPMDEFAULTCONNSERVERSESSION_H
#define CMPMDEFAULTCONNSERVERSESSION_H

#include <e32base.h>
#include "mpmdefaultconnserver.h"


/**
 *  Session to MPM Default Connection server
 *
 *  @lib MPMServer.exe
 *  @since S60 v3.2
 */
class CMPMDefaultConnServerSession : public CSession2
    {

public:

    /**
     * Two-phased constructor.
     */
    static CMPMDefaultConnServerSession* NewL( CMPMDefaultConnServer* aServer );
    

    /**
    * Destructor.
    */
    virtual ~CMPMDefaultConnServerSession();

   /**
     * Returns the default connection server object
     *
     * @since S60 v3.2
     * @return Default Connection server pointer
     */
     CMPMDefaultConnServer* DefaultConnServer();


// from base class CSession2

      /**
       * From CSession2.
       * Serves client request
       *
       * @since S60 v3.2
       * @param aMessage Message from client
       */
       void ServiceL( const RMessage2& aMessage );


private:

    CMPMDefaultConnServerSession( CMPMDefaultConnServer* aServer );

    void ConstructL();
    
    /**
     * Store IAP to be used as default connection in MPM Server
     *
     * @since S60 v3.2
     * @param aMessage Client message containing the IAP
     */
    void SetDefaultIAP( const RMessage2& aMessage );

    /**
     * Clear IAP from being used as default connection in MPM Server
     *
     * @since S60 v3.2
     */
    void ClearDefaultIAP( const RMessage2& aMessage );


private: // data

    
    /**
     * Poiter to DefaultConnServer object. Not own
     */
    CMPMDefaultConnServer* iDefaultConnServer;

    };

#endif // CMPMDEFAULTCONNSERVERSESSION_H
