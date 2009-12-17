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
* Description:  Publish & Subscribe keys
*
*/


#ifndef PDPCONTEXTMANAGERPSKEYS_H
#define PDPCONTEXTMANAGERPSKEYS_H

// PDPContextManager2 property category
const TUid KPDPContextManager2 = {0x101F85EB};

// Factory settings reset key
const TUint KPDPContextManagerFactorySettingsReset = 0x00000001;
enum TPDPContextManagerFactorySettingsReset
    {
    EPDPContextManagerFactorySettingsResetStart,
    EPDPContextManagerFactorySettingsResetStartReply,
    EPDPContextManagerFactorySettingsResetStop
    };

#endif // PDPCONTEXTMANAGERPSKEYS_H
