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
* Description:
* Provides functionality for text resolving
*
*/

#ifndef CMPLUGINBASEENGTEXTRESOLVER_H
#define CMPLUGINBASEENGTEXTRESOLVER_H

/**
 *  Utility class for resolving localised text.
 */
NONSHARABLE_CLASS( CCmPluginBaseEngTextResolver ) : public CBase
    {
public:    
    /**
     * Resolves localised text from cmmanager translation file.
     * @param aTextId Text Id from TS file
     * @return Resolved text
     */
    static HBufC* ResolveTextL( const TDesC& aTextId );        
    };

#endif // CMPLUGINBASEENGTEXTRESOLVER_H
