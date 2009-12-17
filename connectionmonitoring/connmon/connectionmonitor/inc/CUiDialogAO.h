/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Displays a dialog on the UI with the help of Symbian OS
*                notification services.
*
*/

#ifndef __CUIDIALOGAO_H
#define __CUIDIALOGAO_H

#include <cdblen.h>

const TUid KUidSelectNetwork = { 0x101F6D16 };

class CConnMonServer;

/**
 * CUiDialogAO
 * Timer that cancel activity notifier after one second.
 * @lib CONNMON.LIB
 */
NONSHARABLE_CLASS( CUiDialogAO ) : public CActive
    {
public:
    static CUiDialogAO* NewL( CConnMonServer* aServer );
    virtual ~CUiDialogAO();

public:
    /**
     * Starts canceller timer.
     * @return KErrNone if OK, otherwise a system wide error code.
     */
    void Start();

private:
    /**
     * From CActive Cancels the timer.
     * @return void
     */
    void DoCancel();

    /**
     * From CActive Cancels the activity notifier and destroys itself.
     * @return void
     */
    void RunL();

private:
    CUiDialogAO( CConnMonServer* aServer );
    void ConstructL();

private:
    CConnMonServer* iServer;
    TPtrC8 iDummy;
    TBuf8< KCommsDbSvrMaxFieldLength > iNetIds;
    RNotifier iNotifier;
    };

#endif //__CUIDIALOGAO_H

// End-of-file
