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


#ifndef CMDESTINATIONAPI_H
#define CMDESTINATIONAPI_H

#include <e32std.h>


NONSHARABLE_CLASS( RCmDestinationApi ) : public RSubSessionBase
    {
public:
    RCmDestinationApi();
    TBool SessionConnected();

public:
    void ConnectAndGetDestWithIdL(
            const RSessionBase& aSession,
            const TUint32 aId,
            TInt& aExistingHandle );
    void ConnectAndCreateDestWithNameL(
            const RSessionBase& aSession,
            const TDesC& aName );
    void ConnectAndCreateDestWithNameAndIdL(
            const RSessionBase& aSession,
            const TDesC& aName,
            const TUint32 aId );
    void ConnectAndGetConnMethodAsEmbeddedDestL(
            const RSessionBase& aSession,
            const TInt aConnMethodHandle,
            TInt& aExistingHandle );

    void Close();
    TInt ConnectionMethodCount( TInt& aCount );

    TInt Priority( const TInt aHandle, TUint& aPriority );
    TInt GetName( HBufC* aBuffer );

    TInt GetId( TUint32& aId );
    TInt GetElementId( TUint32& aElementId );
    TInt GetMetadata(
            const TUint32 aMetadataField,
            TUint32& aValue );
    TInt GetProtectionLevel( TInt& aProtectionLevel );

    TInt AddConnectionMethod(
            const TInt aConnMethodHandle,
            TInt& aIndex );
    TInt AddEmbeddedDestination(
            const TInt aDestHandle,
            TInt& aIndex );
    TInt DeleteConnectionMethod(
            const TInt aConnMethodHandle );
    TInt RemoveConnectionMethod(
            const TInt aConnMethodHandle );

    TInt ModifyPriority(
            const TInt aConnMethodHandle,
            const TUint aIndex );
    TInt SetName( const TDesC& aValue );
    TInt SetMetadata(
            const TUint32 aMetaField,
            const TUint32 aValue );
    TInt SetProtection( const TInt aValue );
    TInt SetHidden( const TBool aHidden );
    TInt Update();
    TInt Refresh();
    TInt Delete();
    TInt IsConnected( TBool& aResult );
    TInt IsHidden( TBool& aResult );
    TInt IsEqual( const TInt aHandle, TBool& aResult );
    TInt GetIcon( HBufC* aBuffer );
    TInt SetIcon( const TDesC& aValue );

private:
    // True when a subsession is established to the server.
    TBool iSessionConnected;
    };

#endif // CMDESTINATIONAPI_H

// End of file
