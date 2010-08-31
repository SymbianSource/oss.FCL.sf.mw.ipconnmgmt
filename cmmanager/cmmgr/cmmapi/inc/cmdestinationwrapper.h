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
* Collects API calls from RCmDestination- and RCmDestinationExt-classes.
*
*/


#ifndef CMDESTINATIONWRAPPER_H
#define CMDESTINATIONWRAPPER_H


#include <e32base.h>
#include "cmdestinationapi.h"

class RCmManagerApi;


NONSHARABLE_CLASS( CCmDestinationWrapper ) : public CBase
    {
public:
    static CCmDestinationWrapper* NewL();
    static CCmDestinationWrapper* NewLC();
    virtual ~CCmDestinationWrapper();

public:
    CCmDestinationWrapper();

public:
    void GetDestWithIdL(
            RCmManagerApi* aCmManagerApi,
            const TUint32 aId,
            TInt& aExistingHandle );
    void CreateDestByNameL(
            RCmManagerApi* aCmManagerApi,
            const TDesC& aName );
    void CreateDestByNameAndIdL(
            RCmManagerApi* aCmManagerApi,
            const TDesC& aName,
            const TUint32 aId );
    void GetConnMethodAsEmbeddedDestL(
            RCmManagerApi* aCmManagerApi,
            const TInt aConnMethodHandle,
            TInt& aExistingHandle );

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
            const TInt aConnMethodHandle);
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
    TInt SetIcon( const TDesC& aValue);

    TBool SessionConnected();
    void CloseSession();
    TInt GetHandle() const;
    RCmManagerApi* GetMainSession() const;

    void IncreaseReferenceCounter();
    TInt DecreaseReferenceCounter();

private:
    // Subsession to server representing this destination handle.
    RCmDestinationApi iCmDestinationApi;

    // Pointer for main session to server.
    RCmManagerApi* iCmManagerApi; // Not owned.

    // Counts the number of references client has to this specific handle.
    TInt iReferenceCounter;
    };

#endif // CMDESTINATIONWRAPPER_H

// End of file
