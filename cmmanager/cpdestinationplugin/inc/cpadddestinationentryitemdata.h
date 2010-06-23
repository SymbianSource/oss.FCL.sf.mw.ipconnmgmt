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
 *  Data item for representing "Add Destination" button in UI.
 */

#ifndef CPADDDESTINATIONENTRYITEMDATA_H
#define CPADDDESTINATIONENTRYITEMDATA_H

// System includes
#include <cpsettingformentryitemdata.h>

// User includes

// Forward declarations
class CpDestinationGroup;
class CmManagerShim;
class HbInputDialog;
class HbAction;

// External data types

// Constants

// Class declaration
class CpAddDestinationEntryItemData : public CpSettingFormEntryItemData
{
	Q_OBJECT
public:
	explicit CpAddDestinationEntryItemData( 
	    CpItemDataHelper &itemDataHelper, 
	    CpDestinationGroup *parent = 0);
	
	virtual ~CpAddDestinationEntryItemData();
	
signals:

public slots:
    void setNewDestinationName();

protected:

protected slots:
	
private:
	virtual CpBaseSettingView *createSettingView() const;
	bool isDestinationNameValid(const QString dest, CmManagerShim *cmm) const;
	void showErrorNote();
    
private slots:
    void onLaunchView();
	
private: // data
	
    //! Parent object
	CpDestinationGroup *mParent;
	//! New Destination name query dialog
	HbInputDialog      *mDialog;
	//! New Destination name query's OK action
	HbAction           *mOkAction;
};

#endif //CPADDDESTINATIONENTRYITEMDATA_H
