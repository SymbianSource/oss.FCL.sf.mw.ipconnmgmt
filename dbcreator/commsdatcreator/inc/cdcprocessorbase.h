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
* Description:   Declaration of the class CProcessorBase.
*
*/




#ifndef APFILE_PARSER_BASE_H
#define APFILE_PARSER_BASE_H


// INCLUDE FILES

#include <e32base.h>
#include <e32std.h>
#include <f32file.h>
#include <flogger.h>
#include <cmmanagerext.h>
#include <cmconnectionmethodext.h>

#include "cdcerrors.h"
#include "cdccommon.h"

// FORWARD DECLARATION
class CTagContainer;
class CReaderBase;

// CONSTANTS

/**
* Max length of a line in the log file.
*/
LOCAL_D const TInt KMaxLogLineLength = 256;


// CLASS DECLARATION
/**
* CProcessorBase
* Abstract base class for processor classes
*/
class CProcessorBase : public CBase
    {
    
    public: 
        /**
        * Destructor.
        */
        virtual ~CProcessorBase();

        /**
        * Processes the input files line by line.
        */
        void ParseFileL();


    protected:
        /**
        * Constructor
        * @param aFileReader pointer to the file reader
        * @param aCmManager Connection Method Manager
        * @param aPluginArray array that holds the plugins - and so the CMs
        * @param aPluginNames array that holds the CM names
        */
        CProcessorBase( CReaderBase* aFileReader,
                        RCmManagerExt& aCmManager,
                        RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                        RPointerArray< HBufC >& aPluginNames );
               
               
        /**
        * Constructor
        * @param aFileReader pointer to the file reader
        * @param aCmManager Connection Method Manager
        * @param aPluginArray array that holds the plugins - and so the CMs
        * @param aPluginNames array that holds the CM names
        * @param aDestArray array that holds the Destination
        * @param aDestNames array that holds the Destination names
        */
        CProcessorBase( CReaderBase* aFileReader,
                        RCmManagerExt& aCmManager,
                        RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                        RPointerArray< HBufC >& aPluginNames,
                        RPointerArray< RCmDestinationExt >& aDestArray,
                        RPointerArray< HBufC >& aDestNames );       
        /**
        * Processes the actual tag.
        * @param aFieldIDPresent shows if field id is read from input file
        */
        virtual void ProcessTagL( TBool aFieldIDPresent );

        /**
        * Processes the special fields of the tag. Every descendand can implement
        * processes of their special fields
        * @param aField ID of the field
        * @param aTagPtr data conatining by the field
        * @param aDx serial number of the field
        */
        virtual void ProcessSpecialFieldsL( TInt /*aField*/, 
                                            HBufC* /*aTagPtr*/, 
                                            TInt /*aDx*/ ) {};
        
        /**
        * Does the necessary settings before processing tag. Every descendand 
        * can implement their special needs.
        */
        virtual void Reset() {};

        /**
        * Calculates and sets the AddressFromServer field
        */
        virtual void SetAddressFromServerL( TInt aIPField1, 
                                           TInt aIPField2, 
                                           TInt aAddrFromSField,
                                           TInt aIPField1IP6,
                                           TInt aIPField2IP6,
                                           TInt aAddrFromSFieldIP6 );
        /**
        * Virtual function. Processes the actual access point.
        */
        virtual void ProcessAPL();

        /**
        * Writes the actual created access point to the log file.
        */
        virtual void DoLogL();

        /**
        * Returns the TagContainer
        * @return iTagContainer
        */
        CTagContainer& TagContainer();

        /**
        * Adds one item to the plugin array. 
        */
        void AddToPluginArrayL();
        
        /**
        * Retrieves the last plugin from the plugin array. 
        * @return the last plugin in the plugin array
        */
        RCmConnectionMethodExt& GetLastPluginL();
        
        /**
        * Retrieves the plugin in position from the plugin array. 
        * @param aPos position of the desired plugin. 
        * @return the plugin in position from the plugin array
        */
        RCmConnectionMethodExt& GetPluginL( TInt aPos );

        /**
        * Retrieves count of plugins in the plugin array. 
        * @return count of plugins in the plugin array
        */
        TInt PluginCount();
        
        RCmDestinationExt* FindDestinationL( HBufC* aName );
        
        TInt GetDestinationIdL( HBufC16* aName );
 
        RCmConnectionMethodExt* FindPluginL( HBufC* aName );

        RCmConnectionMethodExt* FindPluginL( HBufC* aName, TInt& aPos );

        TInt GetPluginIdL( HBufC16* aName );
        
        TBool ProcessTag( HBufC* aTag, RCmConnectionMethodExt* aPlugin );
        TBool SetGeneralFieldL( TInt aField, HBufC* aPtrTag ); 

    private:
        
        
        TInt CheckPluginName(  HBufC* aName );


    protected:
       
        // @var Gives back AP data from the file.
        CReaderBase*                   iFileReader;
        
        // @var Pointer to Connection Method Manager that is required to access
        // Access Point data
        RCmManagerExt*                 iCmManager;
        
        // @var Shows whether proxies have already been enabled or not.
        TBool iProxyEnabled;   

        // @var Indicates whether the connection method is protected or not
        TBool iIsProtected;
        
        // @var Name of the actual IAP or Destination
        HBufC* iName;    

        // @var Actual IAP
        RCmConnectionMethodExt iPlugin;
        
        // @var Actual Bearer Type
        TUint iBearerType;

    protected:
    
        // @var Plugin array that is used to store pointers to access point data
        RPointerArray< RCmConnectionMethodExt >* iPluginArray;
        // @var Plugin array that is used to store names of access point data
        RPointerArray< HBufC >* iPluginName;
        
        // @var Destination array that is used to store pointers to Destination data
        RPointerArray< RCmDestinationExt >* iDestArray;
        // @var Destination array that is used to store names of Destination
        RPointerArray< HBufC >* iDestName;

    };


#endif // APFILE_PARSER_BASE_H


// End of File.
