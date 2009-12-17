/*
* Copyright (c) 2003-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Data Connection Logger.
*
*/

#include "dclevent.h"

// --------------------------------------------------------------------------
// CEvent::CEvent
// Constructor
// --------------------------------------------------------------------------
//
CEvent::CEvent()
        :
        iConnectionId( 0 ),
        iPrevDownValue( 0 ),
        iPrevUpValue( 0 ),
        iPrevDuration( 0 )
    {
    }

// --------------------------------------------------------------------------
// CEvent::NewLC
// 1st phase constructor - for stack pointers to object
// --------------------------------------------------------------------------
//
CEvent* CEvent::NewLC()
    {
    CEvent* self = new( ELeave ) CEvent;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CEvent::NewL
// 1st phase constructor - for heap pointers to object
// --------------------------------------------------------------------------
//
CEvent* CEvent::NewL()
    {
    CEvent* self = NewLC();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------
// CEvent::ConstructL
// 2nd phase constructor
// --------------------------------------------------------------------------
//
void CEvent::ConstructL()
    {
    iLogWrapEvent = CLogEvent::NewL();
    }

// --------------------------------------------------------------------------
// CEvent::~CEvent
// Destructor
// Frees reserved resources.
// --------------------------------------------------------------------------
//
CEvent::~CEvent()
    {
    delete iLogWrapEvent;
    iLogWrapEvent = NULL;
    }

// End of File
