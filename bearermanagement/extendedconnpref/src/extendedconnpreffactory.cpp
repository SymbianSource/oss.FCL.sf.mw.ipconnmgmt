// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <extendedconnpref.h>
#include <ecom/implementationproxy.h>
#include <comms-infras/metatype.h>


// SMetaDataECom macros


NONSHARABLE_CLASS( CExtendedConnPrefFactory ) : public CBase
/** Connection generic parameter set factory.

@internalComponent
*/
    {
public:
    static Meta::SMetaDataECom* NewL( TAny* aConstructionParameters );
    };

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(TExtendedConnPref::EUid, CExtendedConnPrefFactory::NewL),
	};

/**
ECOM Implementation Factories
*/

// -----------------------------------------------------------------------------
// CExtendedConnPrefFactory::NewL
// -----------------------------------------------------------------------------
//
Meta::SMetaDataECom* CExtendedConnPrefFactory::NewL(TAny* aConstructionParameters)
    {
    TInt32 type = reinterpret_cast<TInt32>(aConstructionParameters);
    switch (type)
        {
    case (TExtendedConnPref::ETypeId):
        return new (ELeave) TExtendedConnPref;
    default:
        User::Leave(KErrNotFound);
        return NULL;
        }
    }
    
//extern "C" { IMPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount); }

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }
