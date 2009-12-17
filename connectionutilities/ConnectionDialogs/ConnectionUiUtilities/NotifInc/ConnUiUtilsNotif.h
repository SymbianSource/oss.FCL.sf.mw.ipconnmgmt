/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of Generic Connection Ui Utilities Notifier
*
*/


#ifndef __CONNUIUTILSNOTIF_H__
#define __CONNUIUTILSNOTIF_H__


// INCLUDES
#include <eiknotapi.h>

// GLOBAL FUNCTIONS
//
/**
* Array of connection dialog plugins.
* @return A CArrayPtr of MEikSrvNotifierBase2 based classes.
*/
IMPORT_C CArrayPtr<MEikSrvNotifierBase2>* NotifierArray();


// RSC file name.
_LIT( KResourceFileName, "ConnUiUtilsNotif.rsc" );

    
#endif

// End of File
