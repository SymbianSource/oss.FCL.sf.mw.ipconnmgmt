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
* Description:   Declaration of the class CProcessorDN.
*
*/




#ifndef PROCESSOR_DN_H
#define PROCESSOR_DN_H


// INCLUDE FILES

#include <e32base.h>
#include <cmdestinationext.h>
#include <cmmanagerext.h>
#include <cmconnectionmethodext.h>
#include <cmmanagerdef.h>

#include "cdcprocessorbase.h"


// FORWARD DECLARATION

// CLASS DECLARATION

/**
* CProcessorDN
* Inherits from the class CProcessorBase.
* Creates Destination Network Item from CTagContainer and
* adds it to the database
*/
class CProcessorDN : public CProcessorBase
    {
    public:
        /**
        * Two-phase constructor.
        * @param aFileReader pointer to the file reader
        * @param aCmManager Connection Method Manager
        * @param aCmArray Array of the processed connection methods
        * @param aIsFirstRound ETrue if this is the first round of DN
        *                          processing. In this case DNs are created.
        *                    EFalse otherwise. This time connection methods
        *                          are added.
        * @return CProcessorDN instance.
        */
        static CProcessorDN* NewL( CReaderBase* aFileReader,
                                   RCmManagerExt& aCmManager,
                                   RPointerArray< RCmConnectionMethodExt >&
                                                            aPluginArray,
                                   RPointerArray< HBufC >& aPluginNames, 
                                   RPointerArray< RCmDestinationExt >& aDestArray,
                                   RPointerArray< HBufC >& aDestNames,
                                   RArray< TBool >& aCmInDest,
                                   TBool aIsFirstRound );
        /**
        * Destructor.
        */
        virtual ~CProcessorDN();

    protected:
        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();

        /**
        * Constructor.
        * @param aFileReader pointer to the file reader
        * @param aCmManager Connection Method Manager
        * @param aCmArray Array of the processed connection methods
        * @param aIsFirstRound ETrue if this is the first round of DN
        *                          processing. In this case DNs are created.
        *                    EFalse otherwise. This time connection methods
        *                          are added.
        */
        CProcessorDN( CReaderBase* aFileReader,
                      RCmManagerExt& aCmManager,
                      RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                      RPointerArray< HBufC >& aPluginNames, 
                      RPointerArray< RCmDestinationExt >& aDestArray,
                      RPointerArray< HBufC >& aDestNames,
                      RArray< TBool >& aCmInDest,
                      TBool aIsFirstRound );
        
        /**
        * From CProcessorBase. Processes the actual tag.
        * @param aFieldIDPresent shows if field id is read from input file
        * @return the result of the process, KErrNone if the actual tag is
        * valid.
        */
        virtual void ProcessTagL( TBool aFieldIDPresent );

        /**
        * From CProcessorBase. Processes the actual access point.
        * @return the result of the process.
        */
        virtual void ProcessAPL();

        /**
        * Writes the actual created access point to the log file.
        */
        void DoLogL();
    
    private:
        /**
        * Helper function for creating destinations. 
        * @param aDnId Destination ID for the new destination
        */
        void CreateDestination( TInt aDnId );
        
    private:
    
        // @var The destiantion that is created
        RCmDestinationExt iDestination;

        // @var Pointer to the destiantion that has been created and stored
        // in iDestArray
        RCmDestinationExt* iDest;
        
        // @var Stores the protection level of the destination
        CMManager::TProtectionLevel iProtectionLevel;
        
        // @var Stores whether this is the first round of DN processing or not
        TBool iIsFirstRound;
        
        // @var ETrue if the destination cannot processed so it is dropped away
        TBool iDestinationDropped;
        
        RArray< TBool >* iCmInDest;
    };


#endif PROCESSOR_DN_H


// End of File.
