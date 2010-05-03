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
*   CpDestinationGroup contains all destination UI items as well as 
*   items for uncategorized access points and adding new destinations.
*/
 
#ifndef CPDESTINATIONGROUP_H
#define CPDESTINATIONGROUP_H

// System includes
#include <QSharedPointer>
#include <cpsettingformitemdata.h>

// User includes
#include "cpbearerapplugininterface.h"

// Forward declarations
class CpDestinationEntryItemData;
class CmDestinationShim;
class CmManagerShim;

// External data types

// Constants

// Class declaration
class CpDestinationGroup : public CpSettingFormItemData
{
    Q_OBJECT
    
public:
    CpDestinationGroup(CpItemDataHelper &itemDataHelper);
    ~CpDestinationGroup();
    
    void addDestination(const QString &dest, int destId);
    void deleteDestination(int destId);
    CpBearerApPluginInterface *findBearerPlugin(int apId);
    
signals:
    
public slots:
    void updateDestinationInformation();
    
protected:
    
protected slots:
        
private: 
    void createUncategorisedDestination();
    void fetchDestinations(QList<QSharedPointer<CmDestinationShim> > &destinationList);
    void loadBearerPlugins();
	QString getDestinationAdditionalText(int iapCount);
	
private slots:

private: // data

    //! Helper for connecting signals to underlying widgets
    CpItemDataHelper                    *mItemDataHelper;
    //! True is uncategorised "destination" is currently shown
    bool                                mUncategorisedShown;
    //! Pointer to CmManager for commsdat operations
    CmManagerShim                       *mCmManager;
    /*! List of plugins that implement views for access points 
     *  according to bearer type
     */
    QList<CpBearerApPluginInterface *>  *mBearerPlugins;
};

#endif // CPDESTINATIONGROUP_H
