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
* CM Manager Application Settings UI implementation.  
*
*/

// System includes

// User includes

#include "cmapplsettingsui_p.h"

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

CmApplSettingsUi::CmApplSettingsUi(QObject *parent) :
    QObject(parent),
    d_ptr(new CmApplSettingsUiPrivate(this))
{
}

CmApplSettingsUi::~CmApplSettingsUi()
{
}

void CmApplSettingsUi::setOptions(
    const QFlags<SelectionDialogItems> &listItems,
    const QSet<BearerTypeFilter> &bearerFilter)
{
    d_ptr->setOptions(listItems, bearerFilter);
}

void CmApplSettingsUi::setSelection(const SettingSelection &selection)
{
    d_ptr->setSelection(selection);
}

CmApplSettingsUi::SettingSelection CmApplSettingsUi::selection() const
{
    return d_ptr->selection();
}

void CmApplSettingsUi::open()
{
    d_ptr->open();
}
