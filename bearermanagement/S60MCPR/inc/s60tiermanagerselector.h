/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: S60 IP tier manager selector
*
*/

/**
@file s60tiermanagerselector.h
S60 IP TM selector.
*/

#ifndef S60TIERMANAGERSELECTOR_H
#define S60TIERMANAGERSELECTOR_H

#include <comms-infras/simpleselectorbase.h>

/**
 * TS60ProviderSelectorFactory
 */
class TS60ProviderSelectorFactory
    {
public:
    static ESock::MProviderSelector* NewSelectorL(const Meta::SMetaData& aSelectionPreferences);
    };

/**
 * CSimpleS60SelectorBase
 * Base for simple selectors.
 */
class CSimpleS60SelectorBase : public CBase, public ASimpleSelectorBase
    {
public:
    explicit CSimpleS60SelectorBase( const Meta::SMetaData& aSelectionPreferences );
    virtual ~CSimpleS60SelectorBase();

protected:
    ESock::ISelectionNotify iSelectionNotify; //Needed as a member because of the asynch selection
private:
    /**
     * Implements the selection logic
     */
    virtual void SelectL( ESock::ISelectionNotify& aSelectionNotify );
    /**
     * Instantiates the MCPR or finds an old one that can be reused.
     */
    ESock::CMetaConnectionProviderBase* FindOrCreateProviderL( TUint aAccessPoint );
private:
    TUint32 iDefaultAp;    
    TSubSessionUniqueId iSubSessionUniqueId;    
    };

#endif // S60IPTIERMANAGERSELECTOR_H
