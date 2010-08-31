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
* Collects API calls from RCmConnectionMethod- and
* RCmConnectionMethodExt-classes.
*
*/


#ifndef CMCONNECTIONMETHODWRAPPER_H
#define CMCONNECTIONMETHODWRAPPER_H

#include <e32base.h>
#include "cmconnectionmethodapi.h"

class RCmManagerApi;


NONSHARABLE_CLASS( CCmConnectionMethodWrapper ) : public CBase
    {
public:
    static CCmConnectionMethodWrapper* NewL();
    static CCmConnectionMethodWrapper* NewLC();
    virtual ~CCmConnectionMethodWrapper();

public:
    CCmConnectionMethodWrapper();

public:
    void GetConnMethodWithIdL(
            RCmManagerApi* aCmManagerApi,
            const TUint32 aIapId,
            TInt& aExistingHandle );
    void GetConnMethodFromDestWithIndexL(
            RCmManagerApi* aCmManagerApi,
            const TInt aDestHandle,
            const TInt aIndex,
            TInt& aExistingHandle );
    void GetConnMethodFromDestWithIdL(
            RCmManagerApi* aCmManagerApi,
            const TInt aDestHandle,
            const TInt aIapId,
            TInt& aExistingHandle );
    void CreateConnMethodL(
            RCmManagerApi* aCmManagerApi,
            const TUint32 aBearerType );
    void CreateConnMethodWithIdL(
            RCmManagerApi* aCmManagerApi,
            const TUint32 aBearerType,
            const TUint32 aIapId );
    TInt CreateConnMethodCopy(
            RCmManagerApi* aCmManagerApi,
            const TInt aConnMethodHandle );
    void CreateConnMethodToDestL(
            RCmManagerApi* aCmManagerApi,
            const TInt aDestHandle,
            const TUint32 aBearerType );
    void CreateConnMethodToDestWithIdL(
            RCmManagerApi* aCmManagerApi,
            const TInt aDestHandle,
            const TUint32 aBearerType,
            const TUint32 aIapId );

    TInt GetIntAttribute(
            const TUint32 aAttribute,
            TUint32& aValue );
    TInt GetBoolAttribute(
            const TUint32 aAttribute,
            TBool& aValue );
    TInt GetStringAttribute(
            const TUint32 aAttribute,
            HBufC* aBuffer );
    TInt GetString8Attribute(
            const TUint32 aAttribute,
            HBufC8* aBuffer );

    TInt SetIntAttribute(
            const TUint32 aAttribute,
            const TUint32 aValue );
    TInt SetBoolAttribute(
            const TUint32 aAttribute,
            const TBool aValue );
    TInt SetStringAttribute(
            const TUint32 aAttribute,
            const TDesC16& aValue );
    TInt SetString8Attribute(
            const TUint32 aAttribute,
            const TDesC8& aValue );

    TInt Update();
    TInt Refresh();
    TInt Delete( TBool& aResult );
    TInt IsEqual( const TInt aHandle, TBool& aResult );

    TBool SessionConnected() const;
    void CloseSession();
    TInt GetHandle() const;
    RCmManagerApi* GetMainSession() const;

    void IncreaseReferenceCounter();
    TInt DecreaseReferenceCounter();

private:
    // Subsession to server representing this connection method handle.
    RCmConnectionMethodApi iCmConnectionMethodApi;

    // Pointer for main session to server.
    RCmManagerApi* iCmManagerApi; // Not owned.

    // Counts the number of references client has to this specific handle.
    TInt iReferenceCounter;
    };

#endif // CMCONNECTIONMETHODWRAPPER_H

// End of file
