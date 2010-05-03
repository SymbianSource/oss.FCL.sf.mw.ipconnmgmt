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
*
*/

#ifndef CPIPSETTINGSVIEW_H
#define CPIPSETTINGSVIEW_H

// System includes
#include <cpbasesettingview.h>

// User includes

// Forward declarations

// External data types

// Constants

// Class declaration
class CpIpSettingsView : public CpBaseSettingView
{
    Q_OBJECT
    
public:
    CpIpSettingsView(QGraphicsItem *parent = 0);
    ~CpIpSettingsView();

signals:

public slots:

protected:

protected slots:

private:

private slots:
    
public: // data
    CpItemDataHelper *mItemDataHelper;
};

#endif // CPIPSETTINGSVIEW_H
