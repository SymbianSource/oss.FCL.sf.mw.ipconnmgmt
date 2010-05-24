/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*      Declaration of VPN CmManager plugin interface implementation 
*      
*
*/

#ifndef CMPLUGINVPN_H
#define CMPLUGINVPN_H

// INCLUDES
#include <cmpluginbaseeng.h>
#include <cmpluginvpndef.h>

// FORWARD DECLARATION
class CCDVirtualIAPNextLayerRecord;

/**
*  CCmPluginVpn defines the functionality of vpn bearer support.
*  Most of the API is inherited from CCmPluginBaseEng class.
*/
NONSHARABLE_CLASS( CCmPluginVpn ) : public CCmPluginBaseEng
    {
    public: // Constructors and destructor
        /**
         * Two phased constructor.
         * @return Returns the VPN plugin object.
         */      
		static CCmPluginVpn* NewL( TCmPluginInitParam* aInitParam );

        /**
         * Destructor.
         */      
        virtual ~CCmPluginVpn();

    public: // From CCmPluginBaseEng
        /**
         * Creates a new instance of VPN bearer plugin. The instance
         * created represents a bearer, not a connection method.
         * @param aInitParam Initialization data.
         * @return Returns CCmPluginBaseEng type pointer which represents pure
         * bearer instance for the cmm server.
         */
        virtual CCmPluginBaseEng* CreateInstanceL( 
                TCmPluginInitParam& aInitParam ) const;
                
        /**
         * Following GetBearerInfoXXXL methods return the values of the 
         * requested attributes. These values are bearerspecific so they
         * don't vary between CMs with same bearer type.
         * @param aAttribute An attribute identifier.
         * @return Returns the value requested. If not found leaves with
         * KErrNotFound error code. 
         */
        virtual TUint32 GetBearerInfoIntL( TUint32 aAttribute ) const;
        
        virtual TBool GetBearerInfoBoolL( TUint32 aAttribute ) const;

        virtual HBufC* GetBearerInfoStringL( TUint32 aAttribute ) const;
        
        virtual HBufC8* GetBearerInfoString8L( TUint32 aAttribute ) const;

        /**
         * Checks if the plug-in can handle the Connection Method identified
         * with parameter aIapId.
         * @param aIapId IAPId of the AP to be checked
         * @return ETrue if plug-in can handle the IAP, otherwise EFalse.
         */
        TBool CanHandleIapIdL( TUint32 aIapId ) const;
         
        /**
         * Checks if the plug-in can handle the given IAP record.
         * @param aIapRecord IAP record to be checked
         * @return ETrue if plug-in can handle the IAP, otherwise EFalse.
         */
        TBool CanHandleIapIdL( CommsDat::CCDIAPRecord* aIapRecord ) const;

        /**
         * Requests CommsDat table ids to be observed for changes by the cmm
         * server. If a table used is not mentioned to be modified a bearer
         * shouldn't return this. Bearers do not reset the array before adding
         * ids.
         * @param aTableIdArray A reference to an array where plugin must add
         * the ids of the tables it want's to be observed.
         */
        void GetBearerTableIdsToBeObservedL( RArray<TUint32>& aTableIdArray ) const;

        
        /**
         * Creates a copy of all bearer specific CommsDat data for the client.
         * Called as result of GetPluginDataL().
         * @param aRecordArray An array where the copy the records. Only 
         * the bearer specific implementation knows the amount and order
         * of these records in the array.
         */
        virtual void GetBearerSpecificRecordsL( 
                RPointerArray<CommsDat::CCDRecordBase>& aRecordArray );

        /**
         * Following GetBearerXXXXAttribute methods get only the
         * fields in records in pointer arrays(parameters).
         * @param aAttribute Identifier of the requested value.
         * @param aGenRecordArray An array containing pointers to generic 
         * records of the Connection Method.
         * @param aBearerSpecRecordArray An array containing pointers to bearer
         * specific records of the Connection Method. aAttribute
         * parameter should identify one field(integer, boolean string)
         * in one of these records. 
         * @return Returns the requested value. In error case leaves with 
         * system-wide error code.
         */

        virtual TUint32 GetBearerIntAttributeL( 
                TUint32 aAttribute, 
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );
        
        virtual TBool GetBearerBoolAttributeL( 
                TUint32 aAttribute, 
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );
                                         
        virtual HBufC* GetBearerStringAttributeL( 
                TUint32 aAttribute, 
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );
        
        virtual HBufC8* GetBearerString8AttributeL( 
                TUint32 aAttribute, 
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );

        /**
         * Following SetBearerXXXXAttribute methods set only the
         * fields in records in pointer arrays(parameters). They are not 
         * allowed to update the original records in plugins.
         * @param aAttribute Identifier of the field to set.
         * @param aValue The value to set.
         * @param aGenRecordArray An array containing pointers to generic 
         * records of the Connection Method.
         * @param aBearerSpecRecordArray An array containing pointers to bearer
         * specific records of the Connection Method. aAttribute
         * parameter should identify one field(integer, boolean string)
         * in one of these records. 
         * @return None.
         */

        virtual void SetBearerIntAttributeL( 
                TUint32 aAttribute, 
                TUint32 aValue, 
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );
        
        virtual void SetBearerBoolAttributeL( 
                TUint32 aAttribute, 
                TBool aValue,
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );
                                         
        virtual void SetBearerStringAttributeL( 
                TUint32 aAttribute, 
                const TDesC16& aValue,
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );

        virtual void SetBearerString8AttributeL( 
                TUint32 aAttribute, 
                const TDesC8& aValue,                                                    
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );

        /**
         * Inherited from CCmPluginBaseEng. 
         * Implementation is empty.
         */
        virtual void PreparePluginToLoadRecordsL();
        
        /**
         * Inherited from CCmPluginBaseEng. 
         * Implementation is empty.
         * @param aCopyInstance Client side data instance.
         */
        virtual void PrepareToCopyDataL( CCmPluginBaseEng* aCopyInstance );

        /**
         * Inherited from CCmPluginBaseEng. 
         * Implementation is empty.
         * @param aGenRecordArray Reference to generic records pointer array.
         * @param aBearerSpecRecordArray Reference to bearer specific records 
         * pointer array.
         */
        virtual void PreparePluginToUpdateRecordsL(
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );

        /**
         * Plugin can prepare to delete the Connection Method with this 
         * function. Called from DeleteL() before any record would be 
         * deleted. No records are deleted here.
         */
        virtual void PrepareToDeleteRecordsL();

        /**
         * Loads the service record. Service record type is known and loaded by 
         * plugin(bearer specific) part of the implementation even if the record
         * is saved to the generic side. Service record is linked to IAP record.
         */
        virtual void LoadServiceRecordL();

        /**
         * Creates the service records.
         */
        virtual void CreateServiceRecordL();

        /**
         * Update service record.
         * @param aGenRecordArray Reference to generic records pointer array.
         * @param aBearerSpecRecordArray Reference to bearer specific records 
         * pointer array.
         */
        virtual void UpdateServiceRecordL( 
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );

        /**
         * Copies the service record. Bearer specific part knows the type of it's
         * service record.
         * @return Returns the service record pointer. 
         */
        virtual CommsDat::CCDRecordBase* CopyServiceRecordL();

        /**
         * Returns the service record id of the Connection Method.
         * @return Returns the id of the service record id of the Connection Method.
         */
        virtual TUint32 ServiceRecordId() const;

        /**
         * Returns the service record name of the Connection Method.
         * @param aServiceName Name of the service record the CM's iap record 
         * points to.
         */
        virtual void ServiceRecordNameLC( HBufC* &aServiceName );

        /**
         * Inherited class loads all the bearer specific records after 
         * loading generic records. Called from LoadL(). 
         */
        virtual void LoadBearerRecordsL();

        /**
         * Creates bearer specific records.
         */
        virtual void CreateBearerRecordsL();

        /**
         * Update bearer specific records.
         * @param aGenRecordArray Reference to generic records pointer array.
         * @param aBearerSpecRecordArray Reference to bearer specific records 
         */
        virtual void UpdateBearerRecordsL( 
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );

        /**
         * Plugin(generic part) can delete bearer specific records
         * with this function. Called from DeleteL().
         */
        virtual void DeleteBearerRecordsL();

         /** 
          * Resets the bearer specific records. 
          */
         virtual void ResetBearerRecords();

        /**
         * Copies the bearer specific records to copy instance given as 
         * parameter. 
         * @param aCopyInstance Pointer to instance where to copy.
         */
        virtual void CopyBearerRecordsL( CCmPluginBaseEng* aCopyInstance );

        /**
         * Returns the bearer record id of the Connection Method.
         * @param aRecordId Id of the bearer record this CM's iap record 
         * points to.
         */
        virtual void BearerRecordIdL( TUint32& aRecordId );

        /**
         * Returns the bearer record name of the Connection Method.
         * @param aBearerName Name of the bearer record this CM's iap record 
         * points to.
         */
        virtual void BearerRecordNameLC( HBufC* &aBearerName );

        
    private: // Constructors
        /**
        * Constructor.
        */      
		CCmPluginVpn( TCmPluginInitParam* aInitParam );

        /**
        * Second phase constructor. Leaves on failure.
        */      
		void ConstructL();   
		
		/**
		 * Gets the default priority and default UI priority 
		 * from the CommsDat.
		 */
		TUint32 GetDefPriorityL( const TUint32 aAttribute ) const;
		
		/**
		 * Gets the virtual bearer record from the CommsDat.
		 */
		CommsDat::CCDVirtualBearerRecord* GetVirtualBearerRecordLC() const;
		
		/**
		 * Type casts the service record in the base class to 
		 * CommsDat::CCDVPNServiceRecord type. 
		 */
		CommsDat::CCDVPNServiceRecord& ServiceRecord() const;
		
		/**
		 * Virtual table Id
		 */
		CommsDat::TMDBElementId                 iVirtualTableId;
		
		/**
		 * Bearer priority table id
		 */
		CommsDat::TMDBElementId                 iBearerPriorityTableId;
		
		/**
		 * Virtual IAP next layer record
		 */
		CCDVirtualIAPNextLayerRecord*           iVirtualIapNextLayerRecord;
    };
    
#endif //CMPLUGINVPN_H
