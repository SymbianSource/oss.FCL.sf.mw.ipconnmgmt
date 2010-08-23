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
* Description:   Declaration of the class CProcessorLan.
*
*/




#ifndef PROCESSOR_LAN_H
#define PROCESSOR_LAN_H


// INCLUDE FILES

#include <e32base.h>
#include <cmmanagerext.h>
#include <cmconnectionmethodext.h>

#include "cdcprocessorbase.h"


// FORWARD DECLARATION

// CLASS DECLARATION

/**
* CProcessorLan
* Inhertits from the class CProcessorBase.
* Creates internet Access Point Item from CTagContainer and 
* adds AP item to database.
*/
class CProcessorLan : public CProcessorBase
    {
    public:
        /**
        * Two-phase constructor.
        * @param aFileReader pointer to the file reader
        * @param aCmManager Connection Method Manager
        * @param aPluginArray Array that stores the processed plugins
        * @return CProcessorLan instance.
        */
        static CProcessorLan* NewL( CReaderBase* aFileReader,
                                    RCmManagerExt& aCmManager,
                                    RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                                    RPointerArray< HBufC >& aPluginNames );
        /**
        * Destructor.
        */
        virtual ~CProcessorLan();

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
        CProcessorLan( CReaderBase* aFileReader,
                       RCmManagerExt& aCmManager,
                       RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                       RPointerArray< HBufC >& aPluginNames );
        
        /**
        * From CProcessorBase. Processes the actual tag.
        * @param aFieldIDPresent shows if field id is read from input file
        * @return the result of the process, KErrNone if the actual tag is
        * valid.
        */
        virtual void ProcessSpecialFieldsL( TInt aField, HBufC* aTagPtr, TInt aDx );
    };


#endif PROCESSOR_LAN_H


// End of File.