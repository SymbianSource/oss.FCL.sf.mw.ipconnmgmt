/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides functionality for text resolving
*
*/

#include <hb/hbcore/hbtextresolversymbian.h>
#include "ccmpluginbaseengtextresolver.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ccmpluginbaseengtextresolverTraces.h"
#endif


HBufC* CCmPluginBaseEngTextResolver::ResolveTextL( const TDesC& aTextId )
    {
    _LIT(KCMManagerTranslationFile, "cmmanager_");
    _LIT(KTranslationPath, "\\resource\\qt\\translations");

    TBool result = HbTextResolverSymbian::Init(
            KCMManagerTranslationFile,
            KTranslationPath );
    if ( result )
        {
        OstTrace0( TRACE_ERROR, CCMPLUGINBASEENGTEXTRESOLVER_RESOLVETEXTL, "CCmPluginBaseEngTextResolver::ResolveTextL" );
        }

    return HbTextResolverSymbian::LoadL( aTextId );
    }

// End of file
