/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class using non-blocking mechanism needs to implement virtual
*                defined by this class.
*
*/


#ifndef MCMDEXEC_H
#define MCMDEXEC_H

#include<e32def.h>

class MCmdExec
    {
public:
    virtual void Execute() = 0;
    virtual void HandleLeaveError( TInt aError ) = 0;
    };

#endif
