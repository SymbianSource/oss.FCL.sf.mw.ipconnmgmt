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


#ifndef CMCONNECTIONMETHODAPI_H
#define CMCONNECTIONMETHODAPI_H

#include <e32std.h>


NONSHARABLE_CLASS( RCmConnectionMethodApi ) : public RSubSessionBase
    {
public:
    RCmConnectionMethodApi();
    TBool SessionConnected() const;

public:
    void ConnectAndGetConnMethodWithIdL(
            const RSessionBase& aSession,
            const TUint32 aIapId,
            TInt& aExistingHandle );
    void ConnectAndGetConnMethodFromDestWithIndexL(
            const RSessionBase& aSession,
            const TInt aDestHandle,
            const TInt aIndex,
            TInt& aExistingHandle );
    void ConnectAndGetConnMethodFromDestWithIdL(
            const RSessionBase& aSession,
            const TInt aDestHandle,
            const TInt aIapId,
            TInt& aExistingHandle );
    void ConnectAndCreateConnMethodL(
            const RSessionBase& aSession,
            const TUint32 aBearerType );
    void ConnectAndCreateConnMethodWithIdL(
            const RSessionBase& aSession,
            const TUint32 aBearerType,
            const TUint32 aIapId );
    TInt ConnectAndCreateConnMethodCopy(
            const RSessionBase& aSession,
            const TInt aConnMethodHandle );
    void ConnectAndCreateConnMethodToDestL(
            const RSessionBase& aSession,
            const TInt aDestHandle,
            const TUint32 aBearerType );
    void ConnectAndCreateConnMethodToDestWithIdL(
            const RSessionBase& aSession,
            const TInt aDestHandle,
            const TUint32 aBearerType,
            const TUint32 aIapId );

    void Close();

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
            HBufC8* aBuffer8 );

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

private:
    // True when a subsession is established to the server.
    TBool iSessionConnected;
    };

#endif // CMCONNECTIONMETHODAPI_H

// End of file
