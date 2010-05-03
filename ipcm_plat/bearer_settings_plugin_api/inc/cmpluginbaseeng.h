/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* IF implementation class for connection method plugins.
*
*/

#ifndef CMPLUGINBASEENG_H
#define CMPLUGINBASEENG_H

// INCLUDES
#include <cmconnectionmethoddef.h>

#include <e32std.h>
#include <e32base.h>
#include <ecom/ecom.h>
#include <metadatabase.h>
#include <commsdattypesv1_1.h>
#include <cmmanagerdef.h>


/**
 * Macro for getting element id from Iap Id.
 * @param iapRecId Record id of an Iap.
 */
#define IAP_ELEMENT_ID( iapRecId )  (KCDTIdIAPRecord | \
                                     KCDMaskShowFieldType | \
                                     (iapRecId << 8))

/**
 * Macros for getting/setting diffrerent type of values from/to the records(CommsDat)
 * @param record Pointer to the record which has the field requested/to be set.
 * @param fieldId Id of the field to get/set the value.
 * @param ***Val Value to be set.
 * @return Returns the requested value in get operation.
 */

#define QUERY_HBUFC_FIELD( record, fieldId ) *STATIC_CAST(CommsDat::CMDBField<HBufC*>*, record->GetFieldByIdL( fieldId ))
#define QUERY_UINT32_FIELD( record, fieldId ) *STATIC_CAST(CommsDat::CMDBField<TUint32>*, record->GetFieldByIdL( fieldId ))
#define QUERY_INT_FIELD( record, fieldId ) *STATIC_CAST(CommsDat::CMDBField<TInt>*, record->GetFieldByIdL( fieldId ))
#define SET_HBUFC_FIELD( record, fieldId, stringVal ) STATIC_CAST(CommsDat::CMDBField<TDesC>*, record->GetFieldByIdL( fieldId ))->SetL( stringVal->Des() )
#define SET_INT_FIELD( record, fieldId, intVal ) STATIC_CAST(CommsDat::CMDBField<TInt>*, record->GetFieldByIdL( fieldId ))->SetL( intVal )
#define FIELD_TO_TDESC( field ) ( (const TDesC&)( field ) )


// CONSTANTS
const TInt32 KExtensionBaseLevel = 1;
const TUint32 KMaxProxyServerNameLength = 1024;

const TUint32 KDummyBearerType = 1;

/**
 * Record indexes for generic iap records in the Client instance table.
 */
const TInt KIapRecordIndex = 0;
const TInt KServiceRecordIndex = 1;
const TInt KNetworkRecordIndex = 2;
const TInt KWAPAPRecordIndex = 3;
const TInt KWAPBearerRecordIndex = 4;
const TInt KMetaDataRecordIndex = 5;
const TInt KLocationRecordIndex = 6;
const TInt KProxyRecordIndex = 7;


// DATA TYPES
typedef enum
    {
    ECmBool,
    ECmInt,
    ECmText,
    ECmText8
    }TCMFieldTypes;
    
// Attributes that are related to bearer plugin creation
enum TBearerCreationCommonAttributes
    {
    ECmBearerCreationCommonAttributes = 9000,   /**<
                                                * Marks the beginning of
                                                * bearer creation related
                                                * attributes
                                                */
    
    ECmExtensionLevel,          /**<
                                * Return the extension level of the given 
                                * bearer type. 1 means the level 1st.
                                * (TUint32 - default: none - read only)
                                */
    
    ECmBearerCreationCommonAttributesEnd = 9999 /**<
                                                * Marks the end of bearer
                                                * creation related attributes
                                                */
    };

/**
 * Flags for Iap metadata to indicate:
 * EMetaHighlight: If it's set it indicates that a connection method is
 *                 highlighted in Agent dialog. Only one connection 
 *                 method can have this flag set.
 * EMetaHiddenAgent: If it's set it indicates that an connection method is 
 *                   hidden connection method in Agent dialog.
 */
enum TCmMetaDataFields
    {
    EMetaHighlight    = 0x00000001,
    EMetaHiddenAgent  = 0x00000002
    };

// FORWARD DECLARATIONS
class CCmPluginBaseEng;
class CCDIAPMetadataRecord;
        
// CLASS DECLARATION
/**
 * Initialisation instance for plugin creation. Used in plugin
 * construction phase.
 */
NONSHARABLE_CLASS(TCmPluginInitParam)
    {
    public:
        /**
         * Constructor.
         * @param aSessionRef Reference for open Session handle to CommsDat.
         */
        IMPORT_C TCmPluginInitParam( CommsDat::CMDBSession& aSessionRef );
        
    public:
        /**
         * Reference to CommsDat session handle.
         */
        CommsDat::CMDBSession& iSessionRef;
            
        /**
         * Reserved for future.
         */
        TAny*               iNotused1;
        TAny*               iNotused2;
    };

// CLASS DECLARATION
/**
 * This defines the data object used for moving data between
 * cmm server and plugins.
 */
NONSHARABLE_CLASS( CCmClientPluginInstance ): public CBase
    {
    public:
        /**
         * Two-phase constructor. Returns pointer to the new
         * CCmClientPluginInstance object.
         */
        IMPORT_C static CCmClientPluginInstance* NewL();

        /**
         * Two-phase constructor. Returns pointer to the new
         * CCmClientPluginInstance object.
         */
        IMPORT_C static CCmClientPluginInstance* NewLC();
        
        /**
         * Destructor
         */
        IMPORT_C ~CCmClientPluginInstance();

    private:
        /**
         * Default constructor.
         */
        CCmClientPluginInstance();
        
        /**
         * Second phase constructor.
         */
        void ConstructL();

    public:
        /**
         * Pointer array for generic record pointers. Those are copies
         * from CommsDat versions for the client usage. 
         */
        RPointerArray<CommsDat::CCDRecordBase> iGenRecordArray;

        /**
         * Pointer array for bearer specific record pointers. Those are copies
         * from CommsDat versions for the client usage. These are asked from
         * the different plugins.
         */
        RPointerArray<CommsDat::CCDRecordBase> iBearerSpecRecordArray;
        
        /** Naming way of the CM. e.g. ENamingUnique */
        CMManager::TNamingMethod               iNamingMethod;
        
        /** Boolean to keep the information if location is enabled */
        TBool                                  iLocationEnabled;
        
        /** Iap Record id of this Connection Method */
        TUint32                                iIapId;
    };

/**
 *  CCmPluginBaseEng is base class for every connection method plugin.
 *  It handles all the generic iap informations e.g. record handling
 *  and attribute reaquests to generic records.
 *
 *  @lib cmmpluginbase.lib
 *  @since S60 v5.2
 */
class CCmPluginBaseEng : public CBase
    {
    public:
       /** 
        * Destructor.
        */
       IMPORT_C virtual ~CCmPluginBaseEng();
        
        /**
         * Create a new instance of the given bearer type plugin.
         * Used from CMM server to avoid using slow ECom framework.
         * @param aInitParam initialization data
         * @return Returns CCmPluginBaseEng type pointer which represents pure
         * bearer instance for the CMM server.
         */
        virtual CCmPluginBaseEng* 
                CreateInstanceL( TCmPluginInitParam& aInitParam ) const = 0;

        /**
         * Following GetBearerInfoXXXL methods return the values of the 
         * requested attributes. These values are bearerspecific so they
         * don't vary between CMs with same bearer type.
         * @param aAttribute An attribute identifier.
         * @return Returns the value requested. If not found leaves with
         * KErrNotFound error code. 
         */
        IMPORT_C virtual TUint32 GetBearerInfoIntL( 
                                                TUint32 aAttribute ) const = 0;
        
        IMPORT_C virtual TBool GetBearerInfoBoolL( 
                                                TUint32 aAttribute ) const = 0;

        IMPORT_C virtual HBufC* GetBearerInfoStringL( 
                                                TUint32 aAttribute ) const = 0;
        
        IMPORT_C virtual HBufC8* GetBearerInfoString8L( 
                                                TUint32 aAttribute ) const = 0;

        /**
         * Checks if the plug-in can handle the Connection Method identified
         * with parameter aIapId.
         * @param aIapId IAPId of the AP to be checked
         * @return ETrue if plug-in can handle the IAP, otherwise EFalse.
         */
        virtual TBool CanHandleIapIdL( TUint32 aIapId ) const = 0;
         
        /**
         * Checks if the plug-in can handle the given IAP record.
         * @param aIapRecord IAP record to be checked
         * @return ETrue if plug-in can handle the IAP, otherwise EFalse.
         */
        virtual TBool CanHandleIapIdL( 
                 CommsDat::CCDIAPRecord* aIapRecord ) const = 0;

    public: // Client interface
        /**
         * Loads all the records belonging to this Connection Method.
         * @param aIapId Id of the IAP record. Identifies the CM
         * related records to load.
         */
        IMPORT_C void LoadL( TUint32 aIapId );

        /**
         * Resets and loads all the records belonging to this Connection Method.
         */
        IMPORT_C void ReLoadL();

        /**
         * Creates a new Connection Method in memory. Call UpdateL() to store 
         * it in CommsDat. No CommsDat transaction operation is needed for this.
         * @param aCmId Predefined iapid to be used when saving to CommsDat. 
         *       0 means that CommsDat gives the id.
         */
        IMPORT_C void CreateNewL( TUint32 aCmId );
         
        /**
         * Creates a copy of an existing Connection Method.
         * @param aClientPluginInstance The source data to create the copy from.
         * @return CCmPluginBaseEng type pointer to an object which represents
         * the new plugin to the CMM server.
         */
        IMPORT_C CCmPluginBaseEng* CreateCopyL( 
                         CCmClientPluginInstance* aClientPluginInstance );

        /**
         * Updates all records to commsdat. Data is copied from the client's copy
         * to the original data.
         * This method does not open/rollback/commit transaction to CommsDat. 
         * Caller must handle transactions.
         */
        IMPORT_C virtual void UpdateL( 
                CCmClientPluginInstance* aClientPluginInstance );

        /*
         * Deletes all the records of this CM from the database. 
         * All the preventing conditions must be checked before calling this. 
         * This method does not open/rollback/commit transaction to CommsDat. 
         * Caller must handle transactions.
         */    
        IMPORT_C virtual void DeleteL();

        /**
         * Creates a copy of all plugin data( most of them are CommsDat data ) for a client.
         * @param aClientPluginInstance An object where to copy the data.
         */
        IMPORT_C void GetPluginDataL( CCmClientPluginInstance* aClientPluginInstance );
        
        /**
         * Following GetXXXXAttributeL methods return the values of the 
         * requested attributes. Attributes are returned from the client's
         * copy of the data. These values are CM specific.
         * @param aAttribute An attribute identifier.
         * @param aClientPluginInstance Client's copy of the data where
         * to get the return value.
         * @return Returns the value requested. If not found leaves with
         * KErrNotFound error code. 
         */
        IMPORT_C TUint32 GetIntAttributeL( 
                            TUint32 aAttribute, 
                            CCmClientPluginInstance* aClientPluginInstance );
        
        IMPORT_C TBool GetBoolAttributeL( 
                            TUint32 aAttribute,
                            CCmClientPluginInstance* aClientPluginInstance );
                                         
        IMPORT_C HBufC* GetStringAttributeL( 
                            TUint32 aAttribute, 
                            CCmClientPluginInstance* aClientPluginInstance );
        
        IMPORT_C HBufC8* GetString8AttributeL( 
                            TUint32 aAttribute, 
                            CCmClientPluginInstance* aClientPluginInstance );

        /**
         * Following SetXXXXAttributeL methods set the given values 
         * identified with attributes. Attributes are set to the 
         * client's copy of the data.
         * @param aAttribute An attribute identifier.
         * @param aValue Value to set.
         * @param aClientPluginInstance Client's copy of the data where
         * to set the given value.
         */
        IMPORT_C void SetIntAttributeL( TUint32 aAttribute, TUint32 aValue, 
                                        CCmClientPluginInstance* aClientPluginInstance );
        
        IMPORT_C void SetBoolAttributeL( TUint32 aAttribute, TBool aValue,
                                         CCmClientPluginInstance* aClientPluginInstance );
                                         
        IMPORT_C void SetStringAttributeL( TUint32 aAttribute, const TDesC16& aValue,
                                           CCmClientPluginInstance* aClientPluginInstance );
        
        IMPORT_C void SetString8AttributeL( TUint32 aAttribute, const TDesC8& aValue,
                                            CCmClientPluginInstance* aClientPluginInstance );

        /**
         * Requests CommsDat table ids to be observed for changes by the cmm
         * server. This is only for generic iap related tables.
         * @param aTableIdArray A reference to an array where iap related generic
         * table ids are added.
         */
        IMPORT_C void GetGenericTableIdsToBeObserved( RArray<TUint32>& aTableIdArray ) const;

        /**
         * Requests CommsDat table ids to be observed for changes by the cmm
         * server. If a table used is not mentioned to be modified a bearer
         * shouldn't return this. Bearers do not reset the array before adding
         * ids.
         * @param aTableIdArray A reference to an array where plugin must add
         * the ids of the tables it want's to be observed.
         */
        IMPORT_C virtual void GetBearerTableIdsToBeObservedL( 
                RArray<TUint32>& aTableIdArray ) const = 0;

    public: // plugin interface
        /**
         * Creates a copy of all bearer specific CommsDat data for the client.
         * Called as result of GetPluginDataL().
         * @param aRecordArray An array where the copy the records. Only 
         * the bearer specific implementation knows the amount and order
         * of these records in the array.
         */
        virtual void GetBearerSpecificRecordsL( 
                RPointerArray<CommsDat::CCDRecordBase>& aRecordArray ) = 0;

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
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray ) = 0;
        
        virtual TBool GetBearerBoolAttributeL( 
                TUint32 aAttribute, 
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray ) = 0;
                                         
        virtual HBufC* GetBearerStringAttributeL( 
                TUint32 aAttribute, 
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray ) = 0;
        
        virtual HBufC8* GetBearerString8AttributeL( 
                TUint32 aAttribute, 
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray ) = 0;

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
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray ) = 0;
        
        virtual void SetBearerBoolAttributeL( 
                TUint32 aAttribute, 
                TBool aValue,
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray ) = 0;
                                         
        virtual void SetBearerStringAttributeL( 
                TUint32 aAttribute, 
                const TDesC16& aValue,
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray ) = 0;

        virtual void SetBearerString8AttributeL( 
                TUint32 aAttribute, 
                const TDesC8& aValue,                                                    
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray ) = 0;

        /**
         * Inherited class can make some preraration before CCmPluginBaseEng 
         * would start loading bearer specific records.
         */
        virtual void PreparePluginToLoadRecordsL() = 0;
        
        /**
         * Gives the plugin a possibility to do some preparing operations
         * if needed.
         * @param aCopyInstance Client side data instance.
         */
        virtual void PrepareToCopyDataL( CCmPluginBaseEng* aCopyInstance ) = 0;

        /**
         * Inherited class can make some preraration before CCmPluginBaseEng 
         * would start to update all the records.
         * @param aGenRecordArray Reference to generic records pointer array.
         * @param aBearerSpecRecordArray Reference to bearer specific records 
         * pointer array.
         */
        virtual void PreparePluginToUpdateRecordsL(
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray ) = 0;

        /**
         * Plugin can prepare to delete the Connection Method with this 
         * function. Called from DeleteL() before any record would be 
         * deleted. No records are deleted here.
         */
        virtual void PrepareToDeleteRecordsL(){};

        /**
         * Loads the service record. Service record type is known and loaded by 
         * plugin(bearer specific) part of the implementation even if the record
         * is saved to the generic side. Service record is linked to IAP record.
         */
        virtual void LoadServiceRecordL() = 0;

        /**
         * Creates the service records.
         */
        virtual void CreateServiceRecordL() = 0;

        /**
         * Update service record.
         * @param aGenRecordArray Reference to generic records pointer array.
         * @param aBearerSpecRecordArray Reference to bearer specific records 
         * pointer array.
         */
        virtual void UpdateServiceRecordL( 
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray ) = 0;

        /**
         * Copies the service record. Bearer specific part knows the type of it's
         * service record.
         * @return Returns the service record pointer. 
         */
        virtual CommsDat::CCDRecordBase* CopyServiceRecordL() = 0;

        /**
         * Returns the service record id of the Connection Method.
         * @return Returns the id of the service record id of the Connection Method.
         */
        virtual TUint32 ServiceRecordId() const = 0;

        /**
         * Returns the service record name of the Connection Method.
         * @param aServiceName Name of the service record the CM's iap record 
         * points to.
         */
        virtual void ServiceRecordNameLC( HBufC* &aServiceName ) = 0;

        /**
         * Inherited class loads all the bearer specific records after 
         * loading generic records. Called from LoadL(). 
         */
        virtual void LoadBearerRecordsL() = 0;

        /**
         * Creates bearer specific records.
         */
        virtual void CreateBearerRecordsL(){};

        /**
         * Update bearer specific records.
         * @param aGenRecordArray Reference to generic records pointer array.
         * @param aBearerSpecRecordArray Reference to bearer specific records 
         */
        virtual void UpdateBearerRecordsL( 
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray ) = 0;

        /**
         * Plugin(generic part) can delete bearer specific records
         * with this function. Called from DeleteL().
         */
        virtual void DeleteBearerRecordsL() = 0;

         /** 
          * Resets the bearer specific records. 
          */
         virtual void ResetBearerRecords() = 0;

        /**
         * Copies the bearer specific records to copy instance given as 
         * parameter. 
         * @param aCopyInstance Pointer to instance where to copy.
         */
        virtual void CopyBearerRecordsL( CCmPluginBaseEng* aCopyInstance ) = 0;

        /**
         * Returns the bearer record id of the Connection Method.
         * @param aRecordId Id of the bearer record this CM's iap record 
         * points to.
         */
        virtual void BearerRecordIdL( TUint32& aRecordId ) = 0;

        /**
         * Returns the bearer record name of the Connection Method.
         * @param aBearerName Name of the bearer record this CM's iap record 
         * points to.
         */
        virtual void BearerRecordNameLC( HBufC* &aBearerName ) = 0;

    protected:
        /** 
         * Constructor 
         */
        IMPORT_C CCmPluginBaseEng( TCmPluginInitParam *aInitParam );
        
        /** 
         * Second phase constructor
         */
        IMPORT_C virtual void ConstructL();
        
        /**
         * Checks if the ECmName attribute was set since the last update.
         * If so, name is updated in passed record
         * @param aSrcRecord record to be checked against
         * @param aDestRecord record to be checked against aSrcRecord
         */
        IMPORT_C void CheckIfNameModifiedL( 
                CommsDat::CCDRecordBase* aSrcRecord,
                CommsDat::CCDRecordBase* aDestRecord ) const;

        /**
         * Plugins can check DNS setting with this function.
         * Suggested to be called from PrepareToUpdateRecordsL().
         * It checked the DNS server addresses, moves secord into
         * first if first is dynamic, and update DNSFromServer field
         * according to address values.
         * @param aIPv6 passed DNS server address are in IPv6 form
         * @param aDNS1 first DNS server address
         * @param aDNS1 secord DNS server address
         * @param aDNSFromServer DNS address from server flag
         */                                            
        IMPORT_C void CheckDNSServerAddressL( 
                TBool aIPv6,
                CommsDat::CMDBField<TDesC>& aDNS1,
                CommsDat::CMDBField<TDesC>& aDNS2,
                CommsDat::CMDBField<TBool>& aDNSFromServer );

    private:
        /**
         * Handles all the Connection Method data copying to instance given as 
         * parameter. This is called from CreateCopyL().
         * @ param aCopyInstance Pointer to plugin to copy the data.
         */
        void CopyDataL( CCmPluginBaseEng* aCopyInstance );

        /**
         * Handles all the Connection Method related record copying. Called from
         * CopyDataL().
         * @ param aCopyInstance Pointer to plugin to copy the data.
         */
        void CopyRecordsL( CCmPluginBaseEng* aCopyInstance );

        /**
         * Copies the record identified by aRecordIdentifier to copy instance.
         * @param aRecordIdentifier Identifies the record which is copied copy.
         * @param aCopyInstance Pointer to plugin to copy the data. 
         */
        void CopyRecordDataL( TUint32 aRecordIdentifier,
                              CCmPluginBaseEng* aCopyInstance );

        /**
         * Creates proxy record and sets the proxy settings enabled for this
         * Connection Method.
         */
        void EnableProxyL( TBool aEnable );
        
        /**
         * Creates location record and sets the location enabled for this 
         * Connection Method.
         */
        void EnableLocationL( TBool aEnable );

        /**
         * Calls all the loading methods. 
         */
        void DoLoadL( TUint32 aIapId );
        
        /**
         * Loads IAP record.
         * @param aIapId Identifies the IAP record to load.
         */
        void LoadIAPRecordL( TUint32 aIapId );

        /**
         * Loads proxy record related to this IAP. This is called as a result
         * of calling LoadL().
         */
        void LoadProxyRecordL();

        /**
         * Loads network record related to this IAP. This is called as a result
         * of calling LoadL().
         */
        void LoadNetworkRecordL();
        
        /**
         * Loads location record related to this IAP. This is called as a result
         * of calling LoadL().
         */
        void LoadLocationRecordL();

        /**
         * Loads WAP AP and WAP IP records related to this IAP. This is called as a result
         * of calling LoadL().
         */
        void LoadWapRecordL();

        /**
         * Loads iap metadata record related to this IAP. This is called as a result
         * of calling LoadL().
         */
        void LoadMetadataRecordL();
        
        /**
         * Following methods are called as a result of calling method UpdateL().
         * Each method overwrites the original data in memory with the client copy
         * of that data. Client data has all the possible changes set by client.
         * After overwrite the data is written to CommsDat. 
         * @param aClientPluginInstance The client copy of the original data with the
         * possible changes.
         */
        void UpdateIAPRecordL( CCmClientPluginInstance* aClientPluginInstance );
        void UpdateProxyRecordL( CCmClientPluginInstance* aClientPluginInstance );
        void UpdateConnPrefSettingL();
        void UpdateNetworkRecordL( CCmClientPluginInstance* aClientPluginInstance );
        void UpdateLocationRecordL( CCmClientPluginInstance* aClientPluginInstance );
        void UpdateWapRecordL( CCmClientPluginInstance* aClientPluginInstance );
        void UpdateMetadataRecordL( CCmClientPluginInstance* aClientPluginInstance );
        void UpdateServiceRecordL( CCmClientPluginInstance* aClientPluginInstance );
        
        /**
        * Creates new wap records.
        */
        void NewWapRecordL();

        /**
         * Searches the WAP records related to this iap. This is called as a 
         * result of LoadL().
         * @return Returns pointer to WAP IP record. If it's not found NULL is
         * returned.
         */
        CommsDat::CCDWAPIPBearerRecord* FindWAPRecordL();

        /**
        * Creates a new metadata record.
        * @param aSetDef ETrue if setting default seamlessness value is needed
        * @return Returns metadata record pointer.
        */
        CCDIAPMetadataRecord* NewMetadataRecordL( TBool aSetDef );
        
        /**
         * Searches the metadata record belonging to this iap. This is called as a 
         * result of LoadL().
         * @return Returns pointer to WAP IP record. If it's not found NULL is
         * returned.
         */
        CCDIAPMetadataRecord* FindMetadataRecordL();

        /**
        * Set attribute flag on the given record.
        * @param aRecord Record to be set.
        * @param aAttribute Attribute to be set.
        * @param aSet ETrue to set, EFalse to clear
        */
        void SetAttribute( CommsDat::CCDRecordBase* aRecord, 
                           TUint32 aAttribute, 
                           TBool aSet );
                           
        /**
        * Copy the attributes of the source record to the destination.
        * @param aSrcRecord record of which attributes are copied
        * @param ADstRecord record of which attributes are set
        */
        void CopyAttributes( CommsDat::CCDRecordBase* aSrcRecord, 
                             CommsDat::CCDRecordBase* aDstRecord );

        
        /**
        * Returns the default location record id.
        * @return Returns the default location record id.
        */
        TUint32 GetLocationIdL() const;

    private:

        /**
         * Gives a possibility to make some prerarations before 
         * starting to load bearer specific records.
         */
        void PrepareToLoadRecordsL();

        /**
         * Gives a possibility to make some prerarations before 
         * starting to update bearer specific records.
         */
        void PrepareToUpdateRecordsL( 
                CCmClientPluginInstance* aClientPluginInstance );

        /**
         * Returns the element id of the iap record.
         * @return Returns the element id of the iap record.
         */
        CommsDat::TMDBElementId IAPRecordElementId() const;
        
        /**
         * Checks the protection attribute of the IAP.
         * @return Returns ETrue if the IAP is protected. Otherwise returns
         * EFalse.
         */
        TBool IsProtected() const;

        /**
         * Sets the name of the proxy server to proxy record. If name's
         * length is 0, proxyport is set to 0 and proxyserver usage
         * boolean is set to EFalse. If the name is longer than 0, 
         * boolean is set to ETrue.
         * @param aProxyServer Address to be set.
         * @param Record pointer where to set.
         */
        void SetProxyServerNameL( const TDesC& aProxyServer,
                                  CommsDat::CCDRecordBase* aProxyRecord );

        /**
         * Sets a name to IAP.
         * @param aName Name to set.
         * @param aClientPluginInstance Client copy of data where to set the 
         * name. UpdateL() updates the name to the CommsDat.
         */
        void SetNameL( const TDesC& aName,
                       CommsDat::CCDRecordBase* aIapRecord,
                       const CMManager::TNamingMethod aNamingMethod );

        /**
         * Following methods are called as a result of SetNameL() to
         * make a valid name of the given string.
         */
        HBufC* DoMakeValidNameL( const TDesC& aName, 
                                 const TUint32& aIapId );
        HBufC* EnsureMaxLengthLC( const TDesC& aName, 
                                  TBool& aChanged );
        TPtrC GetPrefix( const TDesC& aName );
        TInt GetPostfix( const TDesC& aName, const TDesC& aPrefix );
        TBool IsValidNameL( const TDesC& aNameText,
                            const TUint32& aIapId );
        HBufC* EscapeTextLC( const TDesC& aLiteral );
        void SetDefaultNameL( const TDesC& aName );

        /**
        * Check if the passed IP address is '0.0.0.0' or empty string
        * @param aIpAddress IP address to be checked
        */
        TBool IsUnspecifiedIPv4Address( const TDesC& aIpv4Address );

        /**
        * Check if the passed IP address is one of these:
        * - empty string    -   
        * - '0:0:0:0:0:0:0:0' 
        * - 'fec0:000:0000:ffff::1'
        * - 'fec0:000:0000:ffff::2'
        * @param aIpAddress IP address to be checked
        */
        CMManager::TIPv6Types ClassifyIPv6Address( const TDesC& aIpv6Address );

    public: // Data
    
        // Required attribute for the framework
        // (An identifier used during destruction)
        TUid    iDtor_ID_Key;
        TUint32 iBearerType;

    protected:
        /**
         * Reference to cmm's session handle. Initialised at the construction
         * phase.
         */
        CommsDat::CMDBSession& iSession;

        /**
         * Generic record pointers. 
         */
        CommsDat::CCDIAPRecord*             iIapRecord;
        CommsDat::CCDProxiesRecord*         iProxyRecord;
        CommsDat::CCDRecordBase*            iServiceRecord;
        CommsDat::CCDNetworkRecord*         iNetworkRecord;
        CommsDat::CCDLocationRecord*        iLocationRecord;
        CommsDat::CCDWAPAccessPointRecord*  iWapAPRecord;
        CommsDat::CCDWAPIPBearerRecord*     iWapIPBearerRecord;
        CCDIAPMetadataRecord*               iMetaDataRecord;
        
    private:
        /**
         * IAP record id of the CM. 0 means that this is not stored to CommsDat
         * yet. If in UpdateL phase iap record has an id but iIapId == 0 --> 
         * predefined iap id
         */
        TUint32         iIapId;

        /**
         * Naming method of the CM.
         */
        CMManager::TNamingMethod    iNamingMethod;
        
        /**
         * Table id of the iap metadata table. 
         */
        CommsDat::TMDBElementId     iMetadataTableId;
        
        /**
         * Identifies if the location is enabled.
         */
        TBool                       iLocationEnabled;
    };
    
#endif // CMPLUGINBASEENG_H
