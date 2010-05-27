/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Declaration of the class CProcessorGlobal.
*
*/




#ifndef CDCPROCESSORGLOBAL_H
#define CDCPROCESSORGLOBAL_H


// INCLUDE FILES

#include <e32base.h>

#include <metadatabase.h>

#include "cdcprocessorbase.h"

using namespace CommsDat;

// FORWARD DECLARATIONS
class RCmConnectionMethodExt;
class RCmDestinationExt;
class CReaderBase;
class CommsDat::CMDBSession;
struct TCmGenConnSettings;
class RCmManagerExt;

// CLASS DECLARATION

/**
* CProcessorGlobal
* Inhertits from the class CProcessorBase.
* Reads Access Points related settings from CTagContainer.
*/
class CProcessorGlobal : public CProcessorBase
    {
    public:
        /**
        * Two-phase constructor.
        * @param aFileReader pointer to the file reader
        * @param aCmManager 
        * @return CProcessorGlobal instance.
        */
        static CProcessorGlobal* NewL( 
                                CReaderBase* aFileReader,
                                RCmManagerExt& aCmManager,
                                RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                                RPointerArray< HBufC >& aPluginNames, 
                                RPointerArray< RCmDestinationExt >& aDestArray,
                                RPointerArray< HBufC >& aDestNames,
                                RPointerArray< HBufC >& aDefCon );

        /**
        * Destructor.
        */
        virtual ~CProcessorGlobal();
        
    protected:
       
        /**
        * Constructor.
        * @param aFileReader pointer to the file reader
        */
        CProcessorGlobal( CReaderBase* aFileReader,
                          RCmManagerExt& aCmManager,
                          RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                          RPointerArray< HBufC >& aPluginNames, 
                          RPointerArray< RCmDestinationExt >& aDestArray,
                          RPointerArray< HBufC >& aDestNames,
                          RPointerArray< HBufC >& aDefCon);
                          
        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();
        
        /**
        * From CProcessorBase. Processes the actual tag.
        * @param aFieldIDPresent shows if field id is read from input file
        * @return TInt The result of the process, KErrNone if the actual tag is
        * valid.
        */
        virtual void ProcessTagL( TBool aFieldIDPresent );
        
        /**
        * From CProcessorBase. Processes the actual access point.
        * @return The result of the processing
        */
        virtual void ProcessAPL();
        

    private:
    
        /**
        * Updates bearer priority array with the specified bearers priority or
        * UI priority.
        * @param aField Identifier of the the bearer whose priority/UI-priority
        *               is to be updated.
        * @param aPrio The new priority.
        */
        void UpdateGlobalBearerArrayL
                            ( TDbCreatorGlobalSettings aField, TUint32 aPrio );
                
        /**
        * Sets the value of default connection type parsing and converting
        *                           the param string to the right enum. 
        * @param aPtrTag string of the type.
        */
        void SetDefaultConnectionTypeL( HBufC16* aPtrTag );
                            
        /**
        * Sets the id of default connection method. It founds the connection
        *     method or destination corresponding to the given name.
        *     Type depends on the default connection type.
        *     If the method/destination cannot be found then no id is set.
        * @param aPtrTag string of the connection method/destination name.
        */
        void SetDefaultConnectionNameL( HBufC16* aPtrTag );

        /**
        * Sets the WLAN Usage parameter in general connection settings.
        * @param aPtrTag string containing the parameter's value.
        */
        void SetGenConnSettingWlanUsage( HBufC16* aPtrTag );

        /**
        * Sets the "cellular data usage in home networks"
        * parameter in general connection settings.
        * @param aPtrTag string containing the parameter's value.
        */
        void SetGenConnSettingCellularDataUsageHome( HBufC16* aPtrTag );

        /**
        * Sets the "cellular data usage in visitor networks"
        * parameter in general connection settings.
        * @param aPtrTag string containing the parameter's value.
        */
        void SetGenConnSettingCellularDataUsageVisitor( HBufC16* aPtrTag );

        // Stores the general connection settings, using CMM
        void SetGenConnSettingsL();
        
        // Stores the default connection's values, using CMM
        void SetDefConnRecordL( const TInt aId );
        
        // Stores the WLAN parameters
        void SaveGlobalWlanParameterL( const TUint32 aTableType, 
        							   const TDbCreatorGlobalSettings aField, 
        							   const HBufC16* const aValue );

    private:
        
        // @var Default connection's data
        TCmDefConnType iDefaultConnectionType;
        TBool iDefaultConnectionSet;

        // @var GPRS attach mode : whenneeded/whenavailable
        TBool  iAttachWhenNeeded;
        
        RArray< TBearerPriority > iBearerArray;
        TBool iBearerArrayChanged;
        
        // @var Stores the default connection type / name pair
        RPointerArray< HBufC >* iDefCon; 
        
        // @var Stores the general connection settings
        TCmGenConnSettings* iGenConnSettings;
        
		//@var session - Owned
        CMDBSession* iSession;
		
	    //@var WLAN supported flag
	    TBool iIsWlanS;
    };

#endif CDCPROCESSORGLOBAL_H

// End of File.
