/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ConnMon utilities.
*
*/

#ifndef CONNMONUTILS_H
#define CONNMONUTILS_H

#include <etelmm.h>
#include <rconnmon.h>

/**
 * Utility class
 */
class ConnMonUtils
    {
public:
    /**
     * Converts from TMobilePhoneNetworkMode to TConnMonMobilePhoneNetworkMode
     * @param aValue RMobilePhone::TMobilePhoneNetworkMode
     * @param aMode TConnMonMobilePhoneNetworkMode
     * @return errorcode
     */
    static TInt MobilePhoneNetworkModeToConnMonMobilePhoneNetworkMode(
            RMobilePhone::TMobilePhoneNetworkMode aMode,
            TConnMonMobilePhoneNetworkMode& aValue );

    /**
     * Converts from TDesC8 to build independent TDesC
     * @param aBuf8 to be converted
     * @param aInd result of the conversion
     */
    static void TDes8ToTDes( const TDes8& aBuf8, TDes& aInd );

    /**
     * Converts from TDesC to TDesC8
     * @param aSrc to be converted
     * @param aDest result of the conversion
     */
    static void TDesToTDes8( const TDes& aSrc, TDes8& aDest );
    };


/*
* The following declarations need to be kept to prevent modifications in
* def-files and thus prevent binary brakes. These classes are otherwise
* no longer used.
*/

class TConnMonInap
    {
public:
    TConnMonInap();
    virtual TBool Real() const = 0;
    virtual TBool Valid() const = 0;
    };

class TConnMonVirtualIap : public TConnMonInap
    {
public:
    TConnMonVirtualIap();
    // From base class TConnMonInap
    TBool Real() const;
    TBool Valid() const;
    };

class TConnMonRealIap : public TConnMonInap
    {
public:
    TConnMonRealIap();
    // From base class TConnMonInap
    TBool Real() const;
    TBool Valid() const;
    };

class TConnMonSNAP
    {
public:
    TConnMonSNAP();
    virtual ~TConnMonSNAP();
    TBool Valid() const;
    TConnMonSNAP( const TConnMonSNAP& aSNAP );
    TConnMonSNAP( TUint aId );
protected:
    TUint iId;
    };

class CConnMonSNAPsAvailabilityManager : public CBase
    {
public:
    static CConnMonSNAPsAvailabilityManager* NewL();
    virtual ~CConnMonSNAPsAvailabilityManager();
private:
    CConnMonSNAPsAvailabilityManager();
    };

class CConnMonCommsDataNotifier : public CBase
    {
public:
    static CConnMonCommsDataNotifier* NewL();
    virtual ~CConnMonCommsDataNotifier();
private:
    CConnMonCommsDataNotifier();
    };

class CConnMonIapsAvailabilityManager : public CBase
    {
public:
    static CConnMonIapsAvailabilityManager* NewL();
    virtual ~CConnMonIapsAvailabilityManager();
private:
    CConnMonIapsAvailabilityManager();
    };

#endif // CONNMONUTILS_H

// End-of-file
