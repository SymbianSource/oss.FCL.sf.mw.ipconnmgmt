/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Declaration of the class CProcessorVpn.
*
*/




#ifndef PROCESSOR_VPN_H
#define PROCESSOR_VPN_H


// INCLUDE FILES

#include <e32base.h>
#include <cmmanagerext.h>
#include <cmconnectionmethodext.h>

#include "cdcprocessorbase.h"

// FORWARD DECLARATION

// CLASS DECLARATION

/**
* CProcessorVpn
* Inhertits from the class CProcessorBase.
* Creates internet Access Point Item from CTagContainer and 
* adds AP item to database.
*/
class CProcessorVpn : public CProcessorBase
    {
    public:
        /**
        * Two-phase constructor.
        * @param aFileReader pointer to the file reader
        * @param aCmManager Connection Method Manager
        * @param aPluginArray Array that stores the processed plugins
        * @return CProcessorVpn instance.
        */
        static CProcessorVpn* NewL( CReaderBase* aFileReader,
                         RCmManagerExt& aCmMnaager,
                         RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                         RPointerArray< HBufC >& aPluginNames,
                         RPointerArray< HBufC >& aUnderLying);
                         
        void ProcessUnderLyingL( RPointerArray< RCmDestinationExt >& aDestArray,
                                 RPointerArray< HBufC >& aDestNames );
                         
                         
        /**
        * Destructor.
        */
        virtual ~CProcessorVpn();

    protected:
        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();

        /**
        * Constructor.
        * @param aFileReader pointer to the file reader
        * @param aCmManager Connection Method Manager
        * @param aPluginArray Array that stores the processed plugins
        */
        CProcessorVpn( CReaderBase* aFileReader,
                       RCmManagerExt& aCmManager,
                       RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                       RPointerArray< HBufC >& aPluginNames,
                       RPointerArray< HBufC >& aUnderLying );
        
        /**
        * From CProcessorBase. 
        */
        virtual void ProcessSpecialFieldsL( TInt aField, HBufC* aTagPtr, TInt aDx );

    private:
    
        // @var Stores whether this is the first round of VPN processing or not
        TBool iIsFirstRound;

        // @var Stores the IAP name underLying IAP/SNAP name pairs
        RPointerArray< HBufC >* iUnderLying; 
    };


#endif PROCESSOR_VPN_H


// End of File.
