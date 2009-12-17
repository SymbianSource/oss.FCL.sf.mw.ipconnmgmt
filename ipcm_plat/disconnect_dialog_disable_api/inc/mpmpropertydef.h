/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines property category and key for publish and subscribe.
*               This property should be defined and set by MMS.
*
*/

#ifndef MPMPROPERTYDEF_H
#define MPMPROPERTYDEF_H

#include <e32property.h>

const TUid KMPMCathegory = { 0x101F6D34 };
const TUint KMPMPropertyKeyMMS = 0;
const TInt KMPMPropertyTypeMMS = RProperty::EInt;

#endif // MPMPROPERTYDEF_H
