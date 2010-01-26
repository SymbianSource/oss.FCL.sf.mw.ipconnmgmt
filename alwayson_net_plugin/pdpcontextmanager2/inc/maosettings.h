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
* Description:  Defines MAOSettings interface
*
*/


#ifndef MAOSETTINGS_H
#define MAOSETTINGS_H

// INCLUDE FILES

// FORWARD DECLARATIONS

// CONSTANTS

/**
*  Settings interface.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
class MAOSettings
    {
public:

    /**
     * Gets current acces point id.
     *
     * @since S60 v3.1
     * @return TInt the acces point ID.
     */
    virtual TInt AccessPointId() const = 0;

    /**
     * Gets current retry timer value in seconds.
     *
     * @since S60 v3.1
     * @return TInt the retry timer interval in seconds.
     */
    virtual TInt RetryTimerValue() const = 0;
    
    /**
     * Gets Always-On support in HPLMN (home network).
     *
     * @since S60 v3.1
     * @return ETrue if supported
     */
    virtual TBool AlwaysOnSupportedInHPLMN() const = 0;
    
    /**
     * Gets Always-On support in VPLMN (visitor network).
     *
     * @since S60 v3.1
     * @return ETrue if supported.
     */
    virtual TBool AlwaysOnSupportedInVPLMN() const = 0;
    
    /**
     * Gets current connection timer value in seconds.
     *
     * @since S60 v3.1
     * @return TInt the connection timer value in seconds.
     */
    virtual TInt ConnectionTimerValue() const = 0;
    
    /**
     * Gets current unconnect timer value in seconds.
     *
     * @since 3.0
     * @return TInt
     */
    virtual TInt UnconnectTimerValue() const = 0;
    
    /**
     * Gets current linger timer value in seconds.
     *
     * @param aIapId Iap id of the connection.
     * @since 3.2
     * @return TInt
     */
    virtual TInt LingerTimerValue( const TUint aIapId ) const = 0;
    
    /**
     * Checks whether user allows cellular connections
     * or not.
     *
     * @since 5.1
     * @return ETrue if cellular connections are allowed,
     *         EFalse otherwise.
     */
     virtual TBool IsCellularAllowedByUser() const = 0;
        
protected:

    /**
     * Prohibit destruction
     *
     * @since 3.1
     */
    virtual ~MAOSettings(){};
    };

#endif // M_MAOSETTINGS_H
