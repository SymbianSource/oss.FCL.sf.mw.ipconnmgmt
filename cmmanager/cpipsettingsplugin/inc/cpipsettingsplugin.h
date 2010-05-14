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
* Control Panel "Network Settings" plugin header file.
*   
*/

#ifndef	CPIPSETTINGSPLUGIN_H
#define	CPIPSETTINGSPLUGIN_H

// System includes
#include <QObject>
#include <QSharedPointer>
#include <cpplugininterface.h>

// User includes

// Forward declarations
class HbTranslator;

// External data types

// Constants

// Class declaration
class CpIpSettingsPlugin : public QObject, public CpPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(CpPluginInterface)
    
public:
    CpIpSettingsPlugin();
    ~CpIpSettingsPlugin();
    QList<CpSettingFormItemData*> createSettingFormItemData(CpItemDataHelper &itemDataHelper) const;
    
signals:

public slots:

protected:

protected slots:

private:
    Q_DISABLE_COPY(CpIpSettingsPlugin)

private slots:
    
private: // data
    //! Translator instance
    QSharedPointer<HbTranslator> mTranslator;
};

#endif // CPIPSETTINGSPLUGIN_H
