/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Client side IPC handling for Connection Method Manager server.
*
*/


#ifndef CMMANAGERAPI_H
#define CMMANAGERAPI_H

#include <e32std.h>
#include <cmdefconnvalues.h>
#include <cmgenconnsettings.h>

class CCmDestinationWrapper;
class CCmConnectionMethodWrapper;
class TBearerPriority;
class TCmmIpcStructGetConnMethods;
class TCmmIpcStructMoveConnMethod;


NONSHARABLE_CLASS( RCmManagerApi ) : public RSessionBase
    {
public:
    /**
     * Constructor.
     */
    RCmManagerApi();

public:
    /**
     * Opens a connection (session) to the CMM server.
     * @return KErrNone if the connection succeeds, a system-wide error code
     * if not.
     */
    TInt Connect();

    /**
     * Closes the connection (session).
     */
    void Close();

    TInt StoreDestinationWrapper(
            CCmDestinationWrapper* aDestinationWrapper );
    TInt RemoveDestinationWrapper(
            CCmDestinationWrapper* aDestinationWrapper );
    CCmDestinationWrapper* GetDestinationWrapperL( const TInt aHandle );

    TInt StoreConnMethodWrapper(
            CCmConnectionMethodWrapper* aConnMethodWrapper );
    TInt RemoveConnMethodWrapper(
            CCmConnectionMethodWrapper* aConnMethodWrapper );
    CCmConnectionMethodWrapper* GetConnMethodWrapperL( const TInt aHandle );

    TInt GetBearerInfoInt(
            const TUint32 aBearerType,
            const TUint32 aAttribute,
            TUint32& aValue );
    TInt GetBearerInfoBool(
            const TUint32 aBearerType,
            const TUint32 aAttribute,
            TBool& aValue );
    TInt GetBearerInfoString(
            const TUint32 aBearerType,
            const TUint32 aAttribute,
            HBufC* aBuffer );
    TInt GetBearerInfoString8(
            const TUint32 aBearerType,
            const TUint32 aAttribute,
            HBufC8* aBuffer8 );

    TInt GetConnectionMethodInfoInt(
            const TUint32 aIapId,
            const TUint32 aAttribute,
            TUint32& aValue );
    TInt GetConnectionMethodInfoBool(
            const TUint32 aIapId,
            const TUint32 aAttribute,
            TBool& aValue );
    TInt GetConnectionMethodInfoString(
            const TUint32 aIapId,
            const TUint32 aAttribute,
            HBufC* aBuffer );
    TInt GetConnectionMethodInfoString8(
            const TUint32 aIapId,
            const TUint32 aAttribute,
            HBufC8* aBuffer8 );

    void GetConnMethodsL(
            const TCmmIpcStructGetConnMethods& aParameters,
            RArray<TUint32>& aCmArray );
    void GetAllDestinationsL(
            RArray<TUint32>& aDestArray );

    void GetBearerPriorityArrayL( RArray<TBearerPriority>& aArray );
    void SetBearerPriorityArrayL( const RArray<TBearerPriority>& aArray );

    void GetSupportedBearersL( RArray<TUint32>& aArray );

    TInt GetEasyWlanId( TUint32& aValue );

    TInt ReadDefaultConnection(
            TCmDefConnValue& aDefConnSetting );
    TInt WriteDefaultConnection(
            const TCmDefConnValue& aDefConnSetting );

    TInt ReadGeneralConnectionSettings(
            TCmGenConnSettings& aGenConnSettings );
    TInt WriteGeneralConnectionSettings(
            const TCmGenConnSettings& aGenConnSettings );

    TInt CopyConnectionMethod(
            const TInt aTargetDestHandle,
            const TInt aConnMethodHandle,
            TInt& aIndex );
    TInt MoveConnectionMethod(
            TCmmIpcStructMoveConnMethod& aPkgData );
    TInt RemoveConnectionMethod(
            const TInt aTargetDestHandle,
            const TInt aConnMethodHandle );
    TInt RemoveAllReferences(
            const TInt aConnMethodHandle );
    TInt GetUncategorizedIcon( HBufC* aBuffer );

private:
    // Array of destination handles opened through this session.
    // Does not have ownership of contained objects.
    RPointerArray<CCmDestinationWrapper> iDestinations;

    // Array of connection method handles opened through this session.
    // Does not have ownership of contained objects.
    RPointerArray<CCmConnectionMethodWrapper> iConnMethods;

    // True when a session is established to the server.
    TBool iConnected;

    // True when using smaller buffer for ConnectionMethodL(array)-query.
    // Bigger buffer is taken into use if needed.
    TBool iSmallBufsForDestinations;

    // True when using smaller buffer for AllDestinationsL(array)-query.
    // Bigger buffer is taken into use if needed.
    TBool iSmallBufsForConnMethods;
    };

#endif // CMMANAGERAPI_H

// End of file
