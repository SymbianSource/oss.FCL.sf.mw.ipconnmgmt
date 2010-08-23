/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Data Connection Logger.
*/

#ifndef DCLEVENT_H
#define DCLEVENT_H

#include <logwrap.h>
#include <logcli.h>

/**
* CEvent
* Data structure that contains unique connection id provided by
* connection monitor server and the log event that LogWrap-library
* stores in log file.
*/
class CEvent : public CBase
    {
    public:
        // Constructor
        static CEvent* NewL();
        static CEvent* NewLC();

        // Destructor
        virtual ~CEvent();

    private: // New methods
        CEvent();
        void ConstructL();

    private: // Data
        TLogId      iConnectionId;
        TInt64      iPrevDownValue;
        TInt64      iPrevUpValue;
        TInt        iPrevDuration;
        CLogEvent*  iLogWrapEvent;

    private: // Friend classes
        friend class CEngine;
    };

#endif // DCLEVENT_H

// End of File
