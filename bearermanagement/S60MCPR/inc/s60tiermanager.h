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
* Description: S60 IP tier manager
*
*/

/**
@file s60tiermanager.h
S60 IP TM.
*/

#ifndef S60TIERMANAGER_H
#define S60TIERMANAGER_H

#include <comms-infras/coretiermanager.h>

/**
 * CS60 Tier Manager selects the MCPR for the connection. 
 * In practise it creates a new S60 MCPR and provisions selection preferences.
 */
class CS60TierManager : public CCoreTierManager
    {
public:
    /**
     * Symbian two-phase constructor.
     * @return pointer to new CS60TierManager
     */
    static CS60TierManager* NewL( ESock::CTierManagerFactoryBase& aFactory );
    /**
     * C++ destructor
     */
    ~CS60TierManager();
protected:
    /**
     * C++ constructor
     */
    CS60TierManager( ESock::CTierManagerFactoryBase& aFactory, 
                     const MeshMachine::TNodeActivityMap& aActivityMap );
    /**
     * Decide which selector to create  and creates it based on the information available.
     * @param aSelectionPreferences selection preferences that tell which kind of selection should be done.
     * @return CSimpleS60SelectorBase 
     */
    virtual ESock::MProviderSelector* DoCreateProviderSelectorL( const Meta::SMetaData& aSelectionPreferences );
    /**
     * Receives message.
     */
    virtual void ReceivedL( const Messages::TRuntimeCtxId& aSender, 
                            const Messages::TNodeId& aRecipient, 
                            Messages::TSignatureBase& aMessage );
    };

#endif // S60TIERMANAGER_H

