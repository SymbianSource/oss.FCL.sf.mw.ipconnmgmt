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
* Control Panel WLAN AP advanced settings view implementation.
*
*/

// System includes
#include <HbDataForm>
#include <HbDataFormModel>
#include <cmconnectionmethod_shim.h>
#include <cpsettingformitemdata.h>

// User includes
#include "cpwlanapadvancedview.h"

/*!
    \class CpWlanApAdvancedView
    \brief This class implements the WLAN AP Control Panel advanced
           settings view.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/
CpWlanApAdvancedView::CpWlanApAdvancedView(
    CmConnectionMethodShim *cmConnectionMethod,
    QGraphicsItem *parent) :
        CpBaseSettingView(0, parent),
        mForm(0),
        mModel(0),
        mCmConnectionMethod(cmConnectionMethod)
{
    // Construct WLAN AP advanced settings UI
    mForm = new HbDataForm();
    this->setWidget(mForm);
    mModel = new HbDataFormModel(mForm);
    mForm->setModel(mModel);

    // Add advanced settings groups
    createAdvancedSettings();
}

/*!
    Destructor.
*/
CpWlanApAdvancedView::~CpWlanApAdvancedView()
{
}

/*!
    Creates all advanced settings groups.
*/
void CpWlanApAdvancedView::createAdvancedSettings()
{
    // TODO: Implement the advanced settings view.
}
