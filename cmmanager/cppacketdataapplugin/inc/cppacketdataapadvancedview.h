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
* Control Panel packet data AP advanced settings view header file.  
*
*/

#ifndef CPPACKETDATAAPADVANCEDVIEW_H
#define CPPACKETDATAAPADVANCEDVIEW_H

// System includes
#include <cpbasesettingview.h>

// User includes

// Forward declarations
class HbDataForm;
class HbDataFormModel;
class CmConnectionMethodShim;

// External data types

// Constants

// Class declaration
class CpPacketDataApAdvancedView : public CpBaseSettingView
{
    Q_OBJECT
    
public:
    CpPacketDataApAdvancedView(
        CmConnectionMethodShim *cmConnectionMethod,
        QGraphicsItem *parent = 0);
    ~CpPacketDataApAdvancedView();
    
signals:

public slots:

protected:

protected slots:
 
private:
    void createAdvancedSettings();
    
private slots:
    
private: // data
    //! Dataform
    HbDataForm *mForm;
    //! Dataform model
    HbDataFormModel *mModel;
    //! Connection Settings Shim connection method pointer
    CmConnectionMethodShim *mCmConnectionMethod;
};

#endif // CPPACKETDATAAPADVANCEDVIEW_H
