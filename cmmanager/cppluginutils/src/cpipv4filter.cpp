/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* IPv4 input filter implementation.
*
*/

// System includes
#include <HbInputFilter>

// User includes
#include "cpipv4filter.h"

/*!
    \class CpIpv4Filter
    \brief IPv4 input filter implementation.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Returns static instance of the class.
*/
CpIpv4Filter* CpIpv4Filter::instance()
{
    static CpIpv4Filter myInstance;
    return &myInstance;
}

/*!
    Constructor.
*/
CpIpv4Filter::CpIpv4Filter()
{
}

/*!
    Destructor.
*/
CpIpv4Filter::~CpIpv4Filter()
{
}

/*!
    Returns true if given character is valid for an IPv4 address.
*/
bool CpIpv4Filter::filter(QChar aChar)
{
    if (aChar >= '0' && aChar <= '9') {
        return true;
    }
    if (aChar == '.') {
        return true;
    }

    return false;
}
