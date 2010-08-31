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
* IPv4 input filter header file.
*
*/

#ifndef CPIPV4FILTER_H
#define CPIPV4FILTER_H

// System includes
#include <HbInputFilter>

// User includes

// Forward declarations

// External data types

// Constants

// Class declaration
class CpIpv4Filter : public HbInputFilter
{
public:
    static CpIpv4Filter* instance();
    virtual ~CpIpv4Filter();
    virtual bool filter(QChar aChar);

signals:

public slots:

protected:

protected slots:

private:
    Q_DISABLE_COPY(CpIpv4Filter)

    CpIpv4Filter();

private slots:
    
private: // data
};

#endif // CPIPV4FILTER_H
