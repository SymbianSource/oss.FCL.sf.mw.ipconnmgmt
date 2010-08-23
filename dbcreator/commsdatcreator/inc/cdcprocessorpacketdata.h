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
* Description:   Declaration of the class CProcessorPacketData.
*
*/




#ifndef PROCESSOR_PACKET_DATA_H
#define PROCESSOR_PACKET_DATA_H


// INCLUDE FILES

#include <e32base.h>
#include <cmmanagerext.h>
#include <cmconnectionmethodext.h>

#include "cdcprocessorbase.h"


// FORWARD DECLARATION

// CLASS DECLARATION

/**
 * CLingerElement is array element for linger value end corresponding CM name.
 */ 
NONSHARABLE_CLASS( CLingerElement ) : public CBase 
        {
        public: 
        ~CLingerElement();
        HBufC* iName;
        HBufC* iLinger; 
        };

/**
* CProcessorPacketData
* Inhertits from the class CProcessorBase.
* Creates internet Access Point Item from CTagContainer and 
* adds AP item to database.
*/
class CProcessorPacketData : public CProcessorBase
    {
    public:
        /**
        * Two-phase constructor.
        * @param aFileReader pointer to the file reader
        * @param aCmManager Connection Method Manager
        * @param aPluginArray Array that stores the processed plugins
        * @param aLingerSettings stores processed GPRS linger values
        * @return CProcessorPacketData instance.
        */
        static CProcessorPacketData* NewL( 
                        CReaderBase* aFileReader,
                        RCmManagerExt& aCmMnaager,
                        RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                        RPointerArray< HBufC >& aPluginNames,
                        RPointerArray< CLingerElement >& aLingerSettings);
        /**
        * Destructor.
        */
        virtual ~CProcessorPacketData();
        
        /**
        * Maps previously stored linger values with connection ID's 
        * and writes them to repository.
        */
        void SaveLingerSettingsL();

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
        * @param aLingerSettings stores processed GPRS linger values
        */
        CProcessorPacketData( CReaderBase* aFileReader,
                              RCmManagerExt& aCmManager,
                              RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                              RPointerArray< HBufC >& aPluginNames,
                              RPointerArray< CLingerElement >& aLingerSettings);
        
        /**
        * From CProcessorBase. Processes the actual tag.
        * @param aFieldIDPresent shows if field id is read from input file
        * @return the result of the process, KErrNone if the actual tag is
        * valid.
        */
        virtual void ProcessSpecialFieldsL( TInt aField, HBufC* aTagPtr, TInt aDx );
        
        /**
         * Writes connections linger value to repository.
         * @param aIapId is the id of connection to which linger value is set.
         * @param aLingerInterval linger value to set to connection.
         */
        void AddLingerL( const TInt aIapId, const TInt aLingerInterval);
        
    private:
        
        // @var Stores linger values
        RPointerArray< CLingerElement >* iLingerSettings;
    };


#endif PROCESSOR_PACKET_DATA_H


// End of File.
