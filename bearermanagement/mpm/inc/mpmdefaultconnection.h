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
* Description: Handles logic of selecting default connection, 
*              stores selected default connection when necessary.
*
*/

#ifndef C_DEFAULTCONNECTION_H
#define C_DEFAULTCONNECTION_H

#include <e32base.h>
#include <cmdefconnvalues.h>

const TCmDefConnType KDefaultMode = ECmDefConnAlwaysAsk;
const TUint32 KDefaultConnectionId = 0;

class CMPMServer;
/**
 *  Class handling default connection
 *  The class handles logic of selecting default connection
 *  based on the registered Default connections, user selection 
 *  stored in CommsDat and values queried from user.
 *
 *  The default connection can be queried from the class by 
 *  MPM Server session starting a connection. Following 
 *  example describes the behaviour expected from the session.
 *
 *  @code
 *  TCmDefConnType mode(0);
 *  TUint id(0);
 *  iMyServer.DefaultConnection().GetDefaultConnection( mode, id );
 *  if( mode == ECmDefConnAlwaysAsk )
 *      {
 *      //implicit connection start
 *      }
 *  else if (mode == ECmDefConnAskOnce)
 *      {
 *      if( id == 0 )
 *          {
 *          // Implicit connection start
 *          //
 *          // User selection must be registered by the session 
 *          // with call:
 *          MyServer().DefaultConnection().
 *                 SetAskOnceDefaultConnection( aId );
 *
 *          // When BM connection is added under the MPM server object
 *          // the parameter aUsingAskOnce must be set to ETrue in call
 *          MyServer().CMAppendBMConnection( connId, snap, iapId, 
 *                                           state, ETrue );
 *          }
 *      else  
 *          {
 *          // Explicit connection start using SNAP or IAP from
 *          // parameters.    
 *          //
 *          // When BM connection is added under the MPM server object
 *          // the parameter aUsingAskOnce must be set to ETrue in call
 *          MyServer().CMAppendBMConnection( connId, snap, iapId, 
 *                                           state, ETrue );
 *          }
 *      }
 *      else // snap or iap
 *          {
 *          // Explicit connection start using SNAP or IAP from
 *          //parameters
 *          }
 *      } 
 *  @endcode
 *
 *  @lib mpmserver.exe
 *  @since S60 v3.2
 */
class CMPMDefaultConnection : public CBase
    {

public:

    /**
     * Two-phased constructor.
     * @param aServer Reference to MPM Server
     */
    static CMPMDefaultConnection* NewL( const CMPMServer* aServer );
    

    /**
    * Destructor.
    */
    virtual ~CMPMDefaultConnection();

    /**
     * Sets IAP to be used as default connection
     *
     * @since S60 v3.2
     * @param aIapId The IAP to be set as default connection
     * return KErrNone if succesful, KErrArgument if id was not accepted
     */
    TInt SetDefaultIAP( TInt aIapId );

    /**
     * Clear IAP from being used as default connection,
     * if one is set
     *
     * @since S60 v3.2
     */
    void ClearDefaultIAP();

    /**
     * Get the current default connection value 
     *
     * @since S60 v3.2
     * @param aMode Points to snap or specific iap.
     * @param aConnection CommsDat elementId of the IAP or SNAP to be used. 
     * 0 if not defined.
     *
     */
    void GetDefaultConnectionL( TCmDefConnType& aMode, 
                                TUint32& aConnection );
    
private:

    /**
     * Converts int value to TCmDefaultConnection
     * Leaves with KErrArgument is value no in the range
     *
     * @since S60 v3.2
     * @param aInt Int value
     * @return TCmDefConn value
     */    
    TCmDefConnType IntToCmDefConnTypeL( const TUint& aInt );

    /**
     * Reads default connection setting from commsdat.
     * If no entry is found default values are returned.
     *
     * @since S60 v3.2
     * @param aType Default connection setting type
     * @param aId Default connection id
     */    
    void ReadDefConnSettingL( TCmDefConnType& aType,
                              TUint32&          aId );
    /**
     * C++ default constructor.
     */
    CMPMDefaultConnection( const CMPMServer* aServer );
    
    /**
     * 2nd phase constructor.
     */
    void ConstructL();
    

private: // data

    /**
     * CommsDat element id of IAP registered by Wlan active idle plugin, 
     * 0 if not defined
     */
    TUint32 iDefaultIapId;

    /**
     * Pointer to the MPM Server object
     * Not own
     */
    const CMPMServer* iServer;

    };

#endif // CMPMDEFAULTCONNECTION
