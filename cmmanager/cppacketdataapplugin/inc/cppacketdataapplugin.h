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
* Control Panel packet data AP plugin header file.
*   
*/

#ifndef	CPPACKETDATAAPPLUGIN_H
#define	CPPACKETDATAAPPLUGIN_H

// System includes
#include <QObject>
#include <QSharedPointer>
#include <cpbearerapplugininterface.h>

// User includes

// Forward declarations
class HbTranslator;
class CmManagerShim;
class CmConnectionMethodShim;

// External data types

// Constants

// Class declaration
class CpPacketDataApPlugin : public QObject, public CpBearerApPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(CpBearerApPluginInterface)
    
public:
    CpPacketDataApPlugin();
    ~CpPacketDataApPlugin();
    
    uint bearerType() const;
    CpBaseSettingView *createSettingView(uint connectionMethod);
    
signals:
    
public slots:

protected:
    
protected slots:

private:
    Q_DISABLE_COPY(CpPacketDataApPlugin)

private slots:
    
private: // data
    //! Translator instance
    QSharedPointer<HbTranslator> mTranslator;
    //! CM Manager Shim instance
    CmManagerShim *mCmManager;
    //! CM Connection Method Shim instance
    CmConnectionMethodShim *mCmConnectionMethod;
};

#endif // CPPACKETDATAAPPLUGIN_H
