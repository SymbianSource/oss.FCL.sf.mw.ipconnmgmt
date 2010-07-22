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
* IPv6 input filter implementation.
*
*/

// System includes
#include <HbInputFilter>

// User includes
#include "cpipv6filter.h"

/*!
    \class CpIpv6Filter
    \brief IPv6 input filter implementation.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Returns static instance of the class.
*/
CpIpv6Filter* CpIpv6Filter::instance()
{
    static CpIpv6Filter myInstance;
    return &myInstance;
}

/*!
    Constructor.
*/
CpIpv6Filter::CpIpv6Filter()
{
}

/*!
    Destructor.
*/
CpIpv6Filter::~CpIpv6Filter()
{
}

/*!
    Returns true if given character is valid for an IPv6 address.
*/
bool CpIpv6Filter::filter(QChar aChar)
{
    if ((aChar >= 'a' && aChar <= 'f')
        || (aChar >= 'A' && aChar <= 'F')
        || (aChar >= '0' && aChar <= '9')) {
        return true;
    }
    if (aChar == ':' || aChar == '.') {
        return true;
    }

    return false;
}
