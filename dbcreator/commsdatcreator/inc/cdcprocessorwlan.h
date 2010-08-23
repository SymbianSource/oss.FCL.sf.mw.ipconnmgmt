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
* Description:   Declaration of the class CProcessorWlan.
*
*/




#ifndef PROCESSOR_WLAN_H
#define PROCESSOR_WLAN_H


// INCLUDE FILES

#include <e32base.h>
#include <cmmanagerext.h>
#include <cmconnectionmethodext.h>
#include <metadatabase.h>
#include <EapGeneralSettings.h>
#include <EapExpandedType.h>

#include "cdcprocessorbase.h"

using namespace CommsDat;

// FORWARD DECLARATIONS


// CLASS DECLARATION


/**
 * CEapTypeElement is array element for several EapTypes  
 */ 
class CEapTypeElement: public CBase
        {
        public:
        ~CEapTypeElement();
		HBufC* iName;
        EAPSettings* iEapSettings; 
        TEapExpandedType iEncapsulatingEapId; 
        };


/**
* CProcessorWlan
* Inhertits from the class CProcessorBase.
* Creates WLAN Access Point Item from CTagContainer and 
* adds AP item to database.
*/
class CProcessorWlan : public CProcessorBase
    {
    public:
    
        /**
        * Two-phase constructor.
        * @param aCmManager Connection Method Manager
        * @param aPluginArray Array that stores the processed plugins
        * @return CProcessorWlan instance.
        */
        static CProcessorWlan* NewL( CReaderBase* aFileReader,
                                     RCmManagerExt& aCmManager,
                                     RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                                     RPointerArray< HBufC >& aPluginNames,
                                     RPointerArray< HBufC >& aSecurityInfo,
                                     RPointerArray< CEapTypeElement >& eapSettings );
        
        void SaveSecurityInfoL();

        /**
        * Destructor.
        */
        virtual ~CProcessorWlan();

    protected:
    
        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();

        /**
        * Constructor.
        * @param aCmManager Connection Method Manager
        * @param aPluginArray Array that stores the processed plugins
        */
        CProcessorWlan( CReaderBase* aFileReader,
                        RCmManagerExt& aCmManager,
                        RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                        RPointerArray< HBufC >& aPluginNames,
                        RPointerArray< HBufC >& aSecurityInfo,
                        RPointerArray< CEapTypeElement >& aEapSettings );
        
        /**
        * From CProcessorBase. Processes the actual tag.
        * @param aFieldIDPresent shows if field id is read from input file
        * @return TInt The result of the process, KErrNone if the actual tag is
        * valid.
        */
        virtual void ProcessSpecialFieldsL( TInt aField, HBufC* aTagPtr, TInt aDx );


        /**
        * From CProcessorBase. Processes the actual access point.
        * @return TInt The result of the process.
        */
        virtual void ProcessAPL();
        
        void FillSecurityInfoL();
        
        void FillSecurityInfoL( TBool aIsWep );
        
        void SaveSecurityInfoL( TInt& aCounter, TInt aId );

        /**
        * Saves WEP security settings
        * @param aIapId Access point id WEP security settings belongs to.
        */
        void SaveWEPL( TUint32 aIapId );
        
        /**
        * Writes key data to CommsDat.
        * @param aKeyField The field to be written to
        * @param aKey The key data to be written
        * @param aKeyformat The format of the key (ascii/hex)
        */
        void WriteKeyL( CMDBField<TDesC8>* aKeyField,
                        HBufC16* aKey,
                        const TInt aKeyFormat );
        
        /**
        * Gets WEP field value in text format from TagContainer
        * @param aFieldId is the id of the field 
        * @return pointer to the data of the field
        */
        HBufC16* WEPFieldData( TDbCreatorWEPFields aFieldId );        
        
        /**
        * Gets Authentication type
        * @return   TWEPAuthentication - authentication type
        *           KErrNotFound - if authentication type is not present 
        */
        TInt WEPAuthentication();
        
        /**
        * Gets the WEP key in use
        * @return TWEPKeyInUse - the WEP key in use
        *         KErrNotFound - if WEP key in use is not present 
        */
        TInt WEPKeyInUse();
        
        /**
        * Gets WEP key format
        * @param aFieldId is the id of the field 
        * should be EWEPKeyXFormat
        * @return   TWEPKeyFormat - key format
        *           KErrNotFound - if WEP key format is not present 
        */
        TInt WEPKeyFormat( TDbCreatorWEPFields aFieldId );
        
        /**
        * Gets WEP key length
        * @param aFieldId is the id of the length field 
        * should be EWEPKeyXLength
        * @return TWEPKeyLength - WEP key length
        */
        TWEPKeyLength WEPKeyLength( TDbCreatorWEPFields aFieldId );
        
        /**
        * Gets valid WEP key length in characters
        * @param aFormat is the id of the field - should be EWEPKeyXFormat
        * @param aLen is the WEP key length - should be TWEPKeyLength
        * @return TInt - valid key length in characters
        */
        TInt WEPKeyValidLen( TInt aFormat, TWEPKeyLength aLen );
        
        /**
        * Gets the TagContainer index that belongst to the given WEP field
        * @param aFieldId is the id of the field 
        * @return index in TagContainer
        */
        TInt WEPIndex( TDbCreatorWEPFields aFieldId );
        
        /**
        * Check the existence and validity of WEP fields
        * @return  - TRUE if all WEP fields are present and valid
        *          - FALSE if any of the fields is missing or invalid.
        */
        TBool WEPFieldValidate();
        
        /**
        * Converts keyData enetered in Ascii format to hex format
        * @param aSource    Source string
        * @param aDest      destination string
        */
        void ConvertAsciiToHex( const TDesC8& aSource, HBufC8*& aDest );
        
        /**
        * Veryfies hexadecimal format
        * @param aHex   hexadecimal key
        * @return Etrue if hexadecimal key is valid, Efalse otherwise
        */
        TBool VerifyHex( const TDesC8& aHex );
        
        /**
        * Saves WPA security settings
        * @param aIapId Access point id WEP security settings belongs to.
        */
        void SaveWPAL( TUint32 aIapId );
        
        /**
        * Gets the expanded EAP type
        * @param aFieldId is the id of the field 
        * @return expanded EAP type
        */
        TEapExpandedType GetExpandedEapTypeIdL( TDesC& aField );
        
        /**
        * Gets the TagContainer index that belongst to the given WPA field
        * @param aFieldId is the id of the field 
        * @return index in TagContainer
        */
        TInt WPAIndex( TDbCreatorWPAFields aFieldId );
        
        /**
        * Gets WPA field value in text format from TagContainer
        * @param aFieldId is the id of the field 
        * @return pointer to the data of the field
        */
        HBufC16* WPAFieldData( TDbCreatorWPAFields aFieldId );
        
        /**
        * Gets the use of preshared key
        * @return ETrue  - PSK is in use
        *         EFalse - PSK is not in use
        */
        TBool WPAPresharedKeyInUse();
        
        /**
        * Gets WPA key length
        * @return length of the key
        */
        TInt WPAKeyLength();
        
        /**
        * Writes the actual created access point to the log file.
        */
        void DoLogL();
        
    private:
    
        /**
         * Sets the necessary initial values
         */
        virtual void Reset(); 
        
        /*
         * Sets the values of the expanded Eap lists to the database 
         * @param aServiceId for accessing the EAP data 
         */
        void SetExpandedEapListL( const TUint aServiceId );
        
    private:
    
        void AddSecurityDataL( TInt aField, HBufC* aPtrTag, TBool aIsWep );

		void AddEAPSettingL( const TInt aField, HBufC16* aValue );
		
		TBool EAPSetting( const TInt aField );
	
		void FillCipherSuitesL( const HBufC16* const aPtrTag, const TInt aEapIndex );
		
		TEapExpandedType GetEapTypeIdFromSettingId( const TInt aField ); 
		
		TUint FindCertificateEntryL( const EapCertificateEntry::TCertType aCertType, const TInt aEapIndex );
		
		void ConvertSubjectKeyIdToBinaryL( const HBufC16* const aSubjectKeyIdString, TKeyIdentifier& aBinaryKey);
        
        // @var Indicates the WLAN security mode
        TDbCreatorSecurityMode iSecurityMode;
        // @var Indicates the WLAN security mode name
        HBufC* iSecurityModeString;
        
        // @var Stores WLAN security mode info
        RPointerArray< HBufC >* iSecurityInfo;
        
        // @var Stores WLAN EAP settings
        RPointerArray< CEapTypeElement >& iEapSettings;
        
		// @var access point is Easy WLAN if true
        TBool iIsEasyWlan;
       
        // @var HBufC for empty WEP and WPA tags
        HBufC* iEmptyTag;
        
        HBufC* iWepData[KWEPKeyNumOfFields];
        
        HBufC* iWpaData[KWPAKeyNumOfFields];

        TInt iDataStart;

        RArray< RPointerArray< HBufC > >* iTest;

    };


#endif PROCESSOR_WLAN_H


// End of File.
