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
* Control Panel packet data AP advanced settings view implementation.  
*
*/

// System includes
#include <HbDataForm>
#include <HbDataFormModel>
#include <cpsettingformitemdata.h>
#include <cmconnectionmethod_shim.h>

// User includes
#include "cppacketdataapadvancedview.h"

/*!
    \class CpPacketDataApAdvancedView
    \brief Implements the advanced settings view for packet data bearer
           access points.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/
CpPacketDataApAdvancedView::CpPacketDataApAdvancedView(
    CmConnectionMethodShim *cmConnectionMethod,
    QGraphicsItem *parent) :
        CpBaseSettingView(0, parent),
        mForm(0),
        mModel(0),
        mCmConnectionMethod(cmConnectionMethod)
{
    // Construct packet data AP settings UI
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
CpPacketDataApAdvancedView::~CpPacketDataApAdvancedView()
{
}

/*!
    Adds settings items to the model.
*/
void CpPacketDataApAdvancedView::createAdvancedSettings()
{
    // TODO: Implement the advanced settings view.
}
