/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Data handling encapsulation for CM Application Settings UI.
*
*/

#ifndef CMSETTINGDATA_H
#define CMSETTINGDATA_H

// System includes
#include <QString>
#include <QFlags>
#include <QSet>
#include <QList>
#include <QSharedPointer>
#include <cmmanager_shim.h>
#include <cmconnectionmethod_shim.h>
#include <cmdestination_shim.h>

// User includes

// Forward declarations

// External data types

// Constants

// Class declaration

class CmSettingData
{

public:

    // Data types

    // Data class for Destination
    class Destination
    {
    public:
        int id;         //!< Destination ID.
        QString name;   //!< Destination name.
    };

    // Data class for Connection Method
    class ConnMethod
    {
    public:
        int id;         //!< Connection Method ID.
        QString name;   //!< Connection Method name.
    };

    explicit CmSettingData();
    
    ~CmSettingData();
    
    void setBearerFilter(const QSet<uint> &bearers);
    
    bool fetchDestinations(QList< QSharedPointer<Destination> > &destinations);

    bool fetchConnMethods(QList< QSharedPointer<ConnMethod> > &connMethods);

signals:

public slots:

protected:

protected slots:

private:

    void getCmManager();

    bool validateDestination(CmDestinationShim *destination) const;
    
    bool validateConnMethod(CmConnectionMethodShim *connMethod) const;

    static bool connMethodLessThan(
        const QSharedPointer<ConnMethod> connMethod1,
        const QSharedPointer<ConnMethod> connMethod2);
    
private slots:

private: // data

    QSet<uint> mBearers;                        //!< Bearer filter.
    QSharedPointer<CmManagerShim> mCmManager;   //!< CM Manager client instance

    // Friend classes
};

#endif // CMSETTINGDATA_H
