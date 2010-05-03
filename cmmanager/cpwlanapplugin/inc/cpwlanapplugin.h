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
* Control Panel WLAN AP plugin header file.
*   
*/

#ifndef	CPWLANAPPLUGIN_H
#define	CPWLANAPPLUGIN_H

// System includes
#include <QObject>
#include <cpbearerapplugininterface.h>

// User includes

// Forward declarations
class QTranslator;
class CmManagerShim;
class CmConnectionMethodShim;

// External data types

// Constants

// Class declaration
class CpWlanApPlugin : public QObject, public CpBearerApPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(CpBearerApPluginInterface)
    
public:
    CpWlanApPlugin();
    ~CpWlanApPlugin();
    
    uint bearerType() const;
    CpBaseSettingView *createSettingView(uint connectionMethod);
    
signals:
    
public slots:

protected:
    
protected slots:

private:

private slots:
    
private: // data
    //! Translator instance
    QTranslator *mTranslator;
    //! CM Manager Shim instance
    CmManagerShim *mCmManager;
    //! CM Connection Method Shim instance
    CmConnectionMethodShim *mCmConnectionMethod;
};

#endif // CPWLANAPPLUGIN_H
