/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  IF implementation class for connection method plugins.
*
*/

#ifndef CMPLUGINBASEENG_H
#define CMPLUGINBASEENG_H

// INCLUDES
#include <cmconnectionmethoddef.h>

#include <e32std.h>
#include <e32base.h>
#include <ecom/ecom.h>        // For REComSession
#include <metadatabase.h>
#include <CommsDatTypesV1_1.h>

#define IAP_ELEMENT_ID( iapRecId )  (KCDTIdIAPRecord | \
                                     KCDMaskShowFieldType | \
                                     (iapRecId << 8))

#define QUERY_HBUFC_FIELD( record, fieldId ) *STATIC_CAST(CommsDat::CMDBField<HBufC*>*, record->GetFieldByIdL( fieldId ))
#define QUERY_UINT32_FIELD( record, fieldId ) *STATIC_CAST(CommsDat::CMDBField<TUint32>*, record->GetFieldByIdL( fieldId ))
#define QUERY_INT_FIELD( record, fieldId ) *STATIC_CAST(CommsDat::CMDBField<TInt>*, record->GetFieldByIdL( fieldId ))
#define SET_HBUFC_FIELD( record, fieldId, stringVal ) STATIC_CAST(CommsDat::CMDBField<TDesC>*, record->GetFieldByIdL( fieldId ))->SetL( stringVal->Des() )
#define SET_INT_FIELD( record, fieldId, intVal ) STATIC_CAST(CommsDat::CMDBField<TInt>*, record->GetFieldByIdL( fieldId ))->SetL( intVal )
#define FIELD_TO_TDESC( field ) ( (const TDesC&)( field ) )


// CONSTANTS
const TInt32 KExtensionBaseLevel = 1;
const TUint32 KDefAttrsArrayGranSize = 32;
const TUint32 KMaxProxyServerNameLength = 1000;

const TUint32 KDummyBearerType = 1;

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

class CCmPluginBaseEng;
class CCmPluginBase;

/**
* Validation function called before data is stored.
* @param aThis 'this' pointer of the plugin instance
* @param aAttribute attribute to be set
* @param aValue value to be checked
* @return ETrue if value is stored.
*/
typedef TBool (*TValidationFunctionL)( CCmPluginBaseEng* aThis,
                                      TUint32 aAttribute,
                                      const TAny* aValue );

/**
* Structure of conversion table between CM attributes and
* CommsDat ids. The first element must indicate the 
* range of the conversion table. iAttribId is the lowest Id, 
* iCommsDatId is the highest one. The last element is a
* terminator. e.g. Proxy table:
* static const TCmAttribConvTable SProxyConvTbl[] = 
*   {
        { ECMProxyUsageEnabled, ECMProxyRangeMax, NULL }
        { ECMProxyUseProxyServer, KCDTIdUseProxyServer, NULL },
        { ECmProxyPortNumber, KCDTIdPortNumber, &CheckPortNumberValidityL },
        ...
        { 0, 0 }
    }
*/
typedef struct
    {
    TInt    iAttribId;
    TInt    iCommsDatId;
    TValidationFunctionL iValidFuncL;
    TInt    iTitleId;
    TInt    iMaxLength;
    TUint32 iAttribFlags;   // see TCmConvAttribFlags
    TInt    iDefSettingResId;
    TInt    iEditorResId;
    TInt    iDefValueResId;
    TAny*   iNotUsed1;
    TAny*   iNotUsed2;
    }TCmAttribConvTable;
    
enum TCmConvAttribFlags
    {
    EConvNoZeroLength   = 0x00000001,   // Zero length NOT acceptable
    EConvCompulsory     = 0x00000002,
    EConv8Bits          = 0x00000004,
    EConvReadOnly       = 0x00000008,
    EConvNumber         = 0x00000010,
    EConvIPv4           = 0x00000020,
    EConvIPv6           = 0x00000040,
    EConvPassword       = 0x00000080,
    };

typedef struct
    {
    CommsDat::CCDRecordBase*      *iRecord;
    TBool*              iEnabled;
    const TCmAttribConvTable* iConvTable;
    }TCmAttrConvArrayItem;

typedef struct
    {
    TInt                iAttribId;
    TInt                iCommonAttribId;
    }TCmCommonAttrConvArrayItem;
    
enum TCmMetaDataFields
    {
    EMetaHighlight    = 0x00000001,
    EMetaHiddenAgent  = 0x00000002
    };
    
// FUNCTION PROTOTYPES

/**
* TValidationFunctionL functions.
*/

/**
* Function should be called if client tries to set attribute that is read-only
* @param aThis 'this' pointer of the plugin instance
* @param aAttribute not used
* @param aValue not used
* @leave KErrNotSupported in every cases
*/
IMPORT_C TBool ReadOnlyAttributeL( CCmPluginBaseEng* aThis, 
                                  TUint32 aAttribute, 
                                  const TAny* aValue );

/**
* Function to check if the passed IPv4 address is valid.
* @param aThis 'this' pointer of the plugin instance
* @param aAttribute attribute client wants to set
* @param aValue TDesC* IPv4 address buffer to be checked
*/
IMPORT_C TBool CheckIPv4ValidityL( CCmPluginBaseEng* aThis, 
                                  TUint32 aAttribute, 
                                  const TAny* aValue );

/**
* Function to check if the passed IPv6 address is valid.
* @param aThis 'this' pointer of the plugin instance
* @param aAttribute attribute client wants to set
* @param aValue TDesC* IPv6 address buffer to be checked
*/
IMPORT_C TBool CheckIPv6ValidityL( CCmPluginBaseEng* aThis, 
                                  TUint32 aAttribute, 
                                  const TAny* aValue );

/**
* Function to check if the passed port number is valid.
* @param aThis 'this' pointer of the plugin instance
* @param aAttribute attribute client wants to set
* @param aValue TUint32 the port number to be check
*/
IMPORT_C TBool CheckPortNumberValidityL( CCmPluginBaseEng* aThis, 
                                        TUint32 aAttribute, 
                                        const TAny* aValue );

/**
* Function to check if the passed record is valid.
* @param aThis 'this' pointer of the plugin instance
* @param aAttribute attribute client wants to set
* @param aValue TUint32 the record id to be checked
*/
IMPORT_C TBool CheckRecordIdValidityL( CCmPluginBaseEng* aThis, 
                                      TUint32 aAttribute, 
                                      const TAny* aValue );

/**
* Function to check if the passed WAPWspOption is valid.
* @param aThis 'this' pointer of the plugin instance
* @param aAttribute attribute client wants to set
* @param TUint32 the WPSOption to be checked
*/
IMPORT_C TBool CheckWAPWspOptionValidityL( CCmPluginBaseEng* aThis, 
                                          TUint32 aAttribute, 
                                          const TAny* aValue );

/**
* @param aThis 'this' pointer of the plugin instance
* @param aAttribute attribute client wants to set
* @param TUint32 the WPSOption to be checked
*/
IMPORT_C TBool SetProxyServerNameL( CCmPluginBaseEng* aThis, 
                                   TUint32 aAttribute, 
                                   const TAny* aValue );
                          
/**
* Miscellaneous utility functions.
*/

/**
* Check if the passed IP address is '0.0.0.0' or empty string
* @param aIpAddress IP address to be checked
*/
IMPORT_C TBool IsUnspecifiedIPv4Address( const TDesC& aIpv4Address );

/**
* Check if the passed IP address is one of these:
* - empty string    -   
* - '0:0:0:0:0:0:0:0' 
* - 'fec0:000:0000:ffff::1'
* - 'fec0:000:0000:ffff::2'
* @param aIpAddress IP address to be checked
*/
IMPORT_C CMManager::TIPv6Types ClassifyIPv6Address( const TDesC& aIpv6Address );

// FORWARD DECLARATIONS
class CCmDestinationImpl;
class CCmManagerImpl;
class CCDIAPMetadataRecord;
class CCmTransactionHandler;
class CCmPluginBasePrivate;
        
// CLASS DECLARATION
NONSHARABLE_CLASS(TCmPluginInitParam)
    {
    public:
    
        TCmPluginInitParam( CCmManagerImpl& aCmMgr );
        
    public:
    
        CCmManagerImpl&     iCmMgr;
        TUint32             iParentDest;// Parent destination
        TAny*               iNotused1;
        TAny*               iNotused2;
    };

/**
 *  CCmPluginBaseEng is base class for every connection method plugin
 *
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
class CCmPluginBaseEng : public CBase
    {
    public:
    
        /** Destructor. */
        IMPORT_C virtual ~CCmPluginBaseEng();
        
        /**
        * Create a new instance of the given bearer type plugin.
        * Used from framework to avoid using slow ECom framework.
        * @param aInitParam initialization data
        */
        virtual CCmPluginBaseEng* 
                CreateInstanceL( TCmPluginInitParam& aInitParam ) const = 0;
        
    public: // Attribute handlers
    
        /**
        * Gets the value for a TInt attribute.
        * @param aAttribute Identifies the attribute to be retrived.
        * @return contains the requested TInt attribute.
        */
        IMPORT_C virtual TUint32 
                        GetIntAttributeL( const TUint32 aAttribute ) const;

        /**
        * Gets the value for a TBool attribute.
        * @param aAttribute Identifies the attribute to be retrived.
        * @return contains the requested TBool attribute.
        */
        IMPORT_C virtual TBool 
                        GetBoolAttributeL( const TUint32 aAttribute ) const;

        /**
        * Gets the value for a String16 attribute.
        * @param aAttribute Identifies the attribute to be retrived.
        * @return copy of the requested attribute. Ownership is passed.
        */
        IMPORT_C virtual HBufC* 
                        GetStringAttributeL( const TUint32 aAttribute ) const;
        
        /**
        * Gets the value for a String8 attribute.
        * @param aAttribute Identifies the attribute to be retrived.
        * @return copy of the requested attribute. Ownership is passed.
        */
        IMPORT_C virtual HBufC8* 
                        GetString8AttributeL( const TUint32 aAttribute ) const;

        /**
        * Sets the value for a TInt attribute.
        * @param aAttribute Identifies the attribute to be set.
        * @param aValue The value to be set.
        * @return None.
        */
        IMPORT_C virtual void SetIntAttributeL( 
                             const TUint32 aAttribute, TUint32 aValue );

        /**
        * Sets the value for a TBool attribute.
        * @param aAttribute Identifies the attribute to be set.
        * @param aValue The value to be set.
        * @return None.
        */
        IMPORT_C virtual void SetBoolAttributeL( 
                             const TUint32 aAttribute, TBool aValue );

        /**
        * Sets the value for a String16 attribute.
        * @param aAttribute Identifies the attribute to be set.
        * @param aValue The value to be set.
        * @return None.
        */
        IMPORT_C virtual void SetStringAttributeL( 
                             const TUint32 aAttribute, const TDesC16& aValue );

        /**
        * Sets the value for a String8 attribute.
        * @param aAttribute Identifies the attribute to be set.
        * @param aValue The value to be set.
        * @return None.
        */
        IMPORT_C virtual void SetString8AttributeL( 
                             const TUint32 aAttribute, const TDesC8& aValue );

        /**
        * Restore the original value of the attribute from commsdat field.
        * Base implementation can be used only with attributes 
        * stored directly in commsdat.
        * @param aAttribute attribute to be restored
        */
        IMPORT_C virtual void RestoreAttributeL( const TUint32 aAttribute );
        
        /**
        * Returns the CCmDestinationImpl* if this is an 
        * embedded destination CM. Otherwise returns NULL.
        * @return embedded destination object or NULL
        */
        IMPORT_C virtual CCmDestinationImpl* Destination() const;
        
        /**
        * Update or if this is a new CM, creates CM in CommsDat. 
        * - OpenTransactionLC()
        * - PrepareToUpdateRecordsL()
        * - UpdateChargeCardRecordL()
        * - UpdateServiceRecordL()
        * - UpdateIAPRecordL()
        *   - UpdateLocationRecordL()
        *   - BearerRecordIdLC()
        *   - UpdateNetworkRecordL()
        *   - UpdateWapRecordL()
        *   - UpdateProxyRecordL()
        *   - UpdateConnPrefSettingL()
        * - UpdateAdditionalRecordsL()
        * - CommitTransactionL()
        * @return None.
        */        
        IMPORT_C virtual void UpdateL();     
        
        /*
        * Delete from the database if this CM has no more reference
        * from the DN_IAP table.
        * - IsMultipleReferencedL()
        * - OpenTransactionLC()
        * - PrepareToDeleteL()
        * - Delete records (in this order)
        *   - IAP
        *   - Proxy
        *   - Service
        *   - ChargeCard
        *   - Network
        *   - Location
        *   - WapAP
        *   - WapBearer
        * - DeleteAdditionalRecordsL()
        * - CommitTransactionL()
        * @param aForced forced delete doesn't check referencies
        * @param aOneRefsAllowed ETrue if one reference from a destination is 
        *           acceptable to delete this connection method.
        */    
        IMPORT_C virtual TBool DeleteL( TBool aForced,
                                        TBool aOneRefAllowed = ETrue );

        /**
        * Performs the basic loading of records related
        * to this connection method:
        * - OpenTransactionLC();
        * - PrepareToLoadRecordsL()
        * - LoadIAPRecordL();
        *       - load IAP
        *       - load WAP (AP & IP Bearer)
        *       - load proxy
        *       - load charge card
        *       - load network
        *       - load location
        * - LoadServiceSettingL();
        * - LoadAdditionalRecordsL();
        */
        IMPORT_C virtual void LoadL( TUint32 aIapId );

        /**
        * Create a new connection method in memory.
        * Call UpdateL() to store it in CommsDat.
        * No transaction operation is performed.
        * - Creates new WAP, IAP, Network records
        * - CreateNewServiceRecordL()
        * - CreateAdditionalRecordsL()
        * - Loads default AP name from resource 
        *       (R_CMMANAGERENG_DEFAULT_AP_NAME)
        */
        IMPORT_C virtual void CreateNewL();
        
        /**
        * Create a copy of the connection method:
        * - CreateInstanceL()
        * - CreateNewL()
        * - PrepareToCopyDataL()
        * - Copies data:
        *   - name
        *   - bearer type
        *   - records added to the convertion table
        * - Calls CopyAdditionalDataL().
        */
        IMPORT_C virtual CCmPluginBaseEng* CreateCopyL() const;
        
        /*
        * Returns true if the CM has more than one parent destination
        */        
        IMPORT_C virtual TInt NumOfConnMethodReferencesL();

    public:
    
        /**
        * Returns a pointer to the cmmanager
        * @return a pointer to the cmmanager
        */
        CCmManagerImpl& CmMgr() const { return iCmMgr; };

        IMPORT_C CommsDat::CMDBSession& Session() const;

        /**
        * Launches the settings dialog of the plugin
        *
        * @since S60 3.2
        * @return soft key selection
        */
        virtual TInt RunSettingsL() = 0;

        /**
        * Called on a newly created connection method to initialize it properly
        * with user interaction (e.g. APN setting for a packet data-, WEP-key
        * setting for a WLAN connection method, etc.).
        * Note: each plug-in should set its name (i.e. ECmName) in this
        * function call.
        *
        * @since S60 3.2
        * @param aManuallyConfigure let's the plugin know if a plugin should 
        *                           be configured manually or automatically
        * @return ETrue if initialization was successful and wasn't cancelled.
        * EFalse, if initialization process was cancelled (i.e. user pressed
        * Cancel button).
        */
        virtual TBool InitializeWithUiL( TBool aManuallyConfigure ) = 0;
        
        /**
        * Checks if the plug-in can handle the given AP.
        * @param aIapId IAPId of the AP to be checked
        * @return ETrue if plug-in can handle the IAP
        */
        virtual TBool CanHandleIapIdL( TUint32 aIapId ) const = 0;
        
        /**
        * Checks if the plug-in can handle the given AP.
        * @param aIapRecord IAP record to be checked
        * @return ETrue if plug-in can handle the IAP
        */
        virtual TBool CanHandleIapIdL( CommsDat::CCDIAPRecord* aIapRecord ) const = 0;
        
        /**
        * Return the parent destination of this connection method.
        * @return parent destination of this connection method
        */
        IMPORT_C CCmDestinationImpl* ParentDestination() const;
        
        /**
        * Calls FeatureManager::FeatureSupported directly.
        * @param aFeature feature ID
        * @return feature support status
        * @since 3.2
        */
        IMPORT_C static TBool FeatureSupported( TInt aFeature );
                
        /**
        * Function should be overriden by virtual bearer type plugins.
        * Returns if the passed IAP id is linked to that connection method.
        * @param aIapId IAP id to be checked
        * @return ETrue if the passed IAP id is linked to that connection method.
        */
        virtual TBool IsLinkedToIap( TUint32 /*aIapId*/ ){ return EFalse; }
        
        /**
        * Function should be overriden by virtual bearer type plugins.
        * Returns if the passed SNAP id is linked to that connection method.
        * @param aSnapId SNAP id to be checked
        * @return ETrue if the passed SNAP id is linked to that connection method.
        */
        virtual TBool IsLinkedToSnap( TUint32 /*aSnapId*/ ){ return EFalse; }

        /**
        * Increments the reference counter. Used by the object pool.
        */
        void IncrementRefCounter();
        /**
        * Increments the reference counter. Used by the object pool.
        */
        void DecrementRefCounter();
        /**
        * Returns the reference counter. Used by the object pool.
        * @return TInt
        */
        TInt GetRefCounter();

        /**
        * Appends uids of connection methods - which were also created during connection method
        * creation - to the array given as parameter 
        * @param aIapIds the array the additonal cm uids are appended to
        */        
        IMPORT_C virtual void GetAdditionalUids( RArray<TUint32>& aIapIds );

    protected: // Conversion table API
    
        IMPORT_C void AddConverstionTableL( 
                            CommsDat::CCDRecordBase* *aRecord,
                            TBool* aEnabled,
                            const TCmAttribConvTable* aConvTable );
                                            
        IMPORT_C void AddCommonConversionTableL( 
                                const TCmCommonAttrConvArrayItem* aConvTable );
        
        IMPORT_C void RemoveConversionTable( 
                                        const TCmAttribConvTable* aConvTable );
        
        IMPORT_C void RemoveCommonConversionTable( 
                                const TCmCommonAttrConvArrayItem* aConvTable );
                                   
    protected: // Resource handling
    
        /**
        * Add file name to the resource file array.
        * Open it and/or increment reference count.
        * @param aName resource file name
        */
        IMPORT_C void AddResourceFileL( const TDesC& aName );
        
        /**
        * Remove file name from the resource file name array.
        * Close the resource file and/or decrement the reference count.
        * @param aName resource file name
        */
        IMPORT_C void RemoveResourceFile( const TDesC& aName );

        /**
        * Read string from resource file. Resource file has to be
        * opened prior this by calling AddResourceFileL().
        */
        IMPORT_C HBufC* AllocReadL( TInt aResourceId ) const;
        
            
    protected:
    
        /** Constructor */
        IMPORT_C CCmPluginBaseEng( TCmPluginInitParam *aInitParam );
        
        /** Second phase constructor */
        IMPORT_C virtual void ConstructL();
    
        /** Open transaction */
        void OpenTransactionLC();
        
        /**
        * Commit transaction.
        * @param aError - error id
        */
        void CommitTransactionL( TInt aError );
        
        /**
        * Rollback transaction.
        */
        void RollbackTransaction();
        
        /** Make a reset on this class. */
        IMPORT_C void Reset();
        
        /**
        * Plug-in implementation can make its on
        * cleanup in this function. 
        * Always called from LoadL() and 
        * should be called from plugin's destructor.
        */
        virtual void AdditionalReset() = 0;
        
        /**
        * Check if there's connection established with
        * this connection method (IAP id).
        */
        IMPORT_C virtual TBool CheckIfAlreadyConnected() const;
        
    protected:  // virtual loader function
        
        /**
        * Inherited class can make some preraration
        * before CCmPluginBaseEng would start loading
        * records from tables.
        */
        IMPORT_C virtual void PrepareToLoadRecordsL();

        /**
        * Inherited class can load any additional
        * records after CCmPluginBaseEng finished.
        */
        virtual void LoadAdditionalRecordsL(){};

        /**
        * Load service record. Default implementation can
        * create only factory supported record instances.
        */
        IMPORT_C virtual void LoadServiceSettingL();

    protected:  // virtual update functions
    
        /**
        * Check if connection method data is valid, before
        * it would be persisted to commsdat.
        * If invalid data found, set its attribute id in
        * iInvalidAttribute.
        * Base class implementation must be called from
        * overridden one.
        */
        IMPORT_C virtual void PrepareToUpdateRecordsL();
        
        /**
        * Inherited class can update its additional
        * records here.
        */
        virtual void UpdateAdditionalRecordsL(){};

        /**
        * Update service record.
        */
        IMPORT_C virtual void UpdateServiceRecordL();

        /**
        * Return the service table name and record id 
        * of this connection in the service table.
        * @param - aServiceName name of the service table
        *           this connection method points to.
        * @param - aRecordId record id in the service table.
        */
        virtual void ServiceRecordIdLC( HBufC* &aServiceName, 
                                       TUint32& aRecordId ) = 0;

        /**
        * Return the bearer table name and record id 
        * of this connection in the bearer table.
        * @param - aBearerName name of the bearer table
        *           this connection method points to.
        * @param - aRecordId record id in the bearer table.
        */
        virtual void BearerRecordIdLC( HBufC* &aBearerName, 
                                       TUint32& aRecordId ) = 0;

    protected:  // virtual delete function
    
        /**
        * Plugin can prepare to delete the connection method
        * with this function. Called from DeleteL() before
        * any record would be deleted. Do NOT delete any
        * record here.
        */
        virtual void PrepareToDeleteRecordsL(){};
        
        /**
        * Plugin can delete additional, bearer specific record
        * with this function. Called from DeleteL().
        */
        virtual void DeleteAdditionalRecordsL(){};

    protected:  // virtual create function
    
        /**
        * Plugin has to create and initialize its bearer specific object
        * that points to the service record here. Pointer has to be stored in 
        * iServiceRecord data member.
        * Do NOT store service record in CommsDat yet.
        */
        virtual void CreateNewServiceRecordL() = 0;
        
        /**
        * Plugin can create and initialize its bearer specific record(s) here.
        * Do NOT store them in CommsDat yet.
        */
        virtual void CreateAdditionalRecordsL(){};
        
    protected:

        /**
        * Called from CreateCopyL().
        * Plugin can prepare for copying every records
        * added to the conversion tables.
        *
        * @param aDestInst copy attributes into this plugin instance
        */
        virtual void PrepareToCopyDataL( CCmPluginBaseEng& /*aDestInst*/ ) const {};
                
        /**
        * Copy data of the connection method that is not 
        * added to the conversion table. (e.g. bearer specific
        * flags)
        *
        * @param aDestInst copy attributes into this plugin instance
        */    
        virtual void CopyAdditionalDataL( CCmPluginBaseEng& /*aDestInst*/ ) const {};

    private:
    
        void EnableProxyL( TBool aEnable );
        void EnableChargeCardL( TBool aEnable );
        void EnableLocationL( TBool aEnable );

        // Loader functions
        void DoLoadL( TUint32 aIapId );
        void LoadIAPRecordL( TUint32 aIapId );
        void LoadChargeCardSettingL( TUint32 aChargeCardId );
        void LoadProxySettingL();
        void LoadNetworkSettingL();
        void LoadLocationSettingL();
        void LoadWapRecordL();
        void LoadSeamlessnessRecordL();
        
        // Updater functions
        void UpdateIAPRecordL();
        void UpdateProxyRecordL();
        void UpdateChargeCardRecordL();
        void UpdateConnPrefSettingL();
        void UpdateNetworkRecordL();
        void UpdateLocationRecordL();
        void UpdateWapRecordL();
        void UpdateSeamlessnessRecordL();
        
        /**
        * No WAP record found in load process,
        * but client wants to set start page.
        */
        void NewWapRecordL();
        
        /**
        * Create a new metadata record.
        * @param aSetDef ETrue if setting default seamlessness valud is needed
        * @return create metadata record point
        */
        CCDIAPMetadataRecord* NewSeamlessnessRecordL( TBool aSetDef );
        
        CommsDat::CCDWAPIPBearerRecord* FindWAPRecordL();
        CCDIAPMetadataRecord* FindSeamlessnessRecordL();

        /**
        * Set attribute flag on the given record
        * @param aRecord record to be set
        * @param aAttribute e.g. ECDHidden
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
        * Find field element associated with the passed attribute.
        * @param aAttribute aatribute to find
        * @aType type of the attribute (e.g. bool, int or string)
        */               
        TValidationFunctionL FindFieldL( TUint32 aAttribute,
                                           TCMFieldTypes aType,
                                           CommsDat::CMDBElement* &aElement ) const;

        
        /**
        * Return Location id
        * @return location id
        */
        TUint32 GetLocationIdL() const;
        
        /**
        * Search for common attribute and convert it to plugin specific one.
        * @param aAttribute common attribute id
        * @return bearer specific attribute id
        */
        TUint32 CheckForCommonAttribute( const TUint32 aAttribute ) const;
        
        /**
        * Returns the common attribute for the bearer specific attribute.
        * Panics if there is no matching common attribute.
        * @param aAttribute bearer specific attribute
        * @return common attribute
        */
        TUint32 CommonAttribute( const TUint32 aAttribute ) const;
        
    public: // Util functions

        IMPORT_C CommsDat::TMDBElementId IAPRecordElementId() const;
        
        IMPORT_C TBool IsProtected() const;
        
        TBool SetProxyServerNameL( const TDesC& aProxyServer );

        /**
        * Return the coversion table item of the attribute
        * @param aAttribute attribute of which conversion item is requested
        * @return conversion table item
        */
        IMPORT_C const TCmAttribConvTable* ConvTableItem( TUint32 aAttribute );

        /**
        * Function to set up DNS addresses and DNS address from server flag.
        * Can be used with both IPv4 and IPv6 attribute. 
        * Passed bearer specific attributes must be defined in common conversion 
        * table.
        * @param aSrv1Attr first DNS server address attribute
        * @param aSrv1 DNS first DNS server address
        * @param aSrv2Attr second DNS server address
        * @param aSrv2 DNS second DNS server address attribute
        * @param aAddrFromSrvAttr DNS address from server flag attribute
        * @param aIPv6 ETrue if passed addresses are in IPv6 form
        */
        IMPORT_C void SetDNSServerAddressL( TUint32 aSrv1Attr,
                                            const TDesC& aSrv1,
                                            TUint32 aSrv2Attr,
                                            const TDesC& aSrv2,
                                            TUint32 aAddrFromSrvAttr,
                                            TBool aIPv6  );

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
        IMPORT_C void CheckDNSServerAddressL( TBool aIPv6,
                                              CommsDat::CMDBField<TDesC>& aDNS1,
                                              CommsDat::CMDBField<TDesC>& aDNS2,
                                              CommsDat::CMDBField<TBool>& aDNSFromServer );

    protected:
            
        /**
        * Checks if the ECmName attribute was set since the last update.
        * If so, name is updated in passed record
        * @param aRecord record to be checked against iIapRecord
        */
        IMPORT_C void CheckIfNameModifiedL( CommsDat::CCDRecordBase* aRecord ) const;
        
        /**
        * Returns the global priority of the passed bearer type
        * @param aServiceType bearer type of which priority is requested
        * @return global bearer priority
        */        
        IMPORT_C TInt GlobalBearerPriority( const TDesC& aServiceType ) const;
        
        /**
        * Returns the global UI priority of the passed bearer type
        * @param aServiceType bearer type of which priority is requested
        * @return global UI bearer priority
        */
        IMPORT_C TInt GlobalUiBearerPriority( const TDesC& aServiceType ) const;

    private: // Util functions
    
        HBufC* DoMakeValidNameL( const TDesC& aName );
        HBufC* EnsureMaxLengthLC( const TDesC& aName, 
                                  TBool& aChanged );
                                  
        TPtrC GetPrefix( const TDesC& aName );
        TInt GetPostfix( const TDesC& aName, const TDesC& aPrefix );

        TBool IsValidNameL( const TDesC& aNameText );
        HBufC* EscapeTextLC( const TDesC& aLiteral );
        
        void SetNameL( const TDesC& aName );
        
        TCmAttrConvArrayItem* ConversionTable( TUint32 aAttribute ) const;
        CommsDat::CMDBElement& FieldByAttributeL( const TCmAttrConvArrayItem* aItem,
                                        const TUint32 aAttribute,
                                        TInt& aIndex ) const;
        
        /**
        * Writes the passed text into the passed attribute.
        * @param aAttribute The attribute to write into
        * @param aValue The value to write
        */
        void GenericStringWriterL( const TUint32 aAttribute, 
                                   const TDesC16& aValue );
        
        /**
        * Returns the common attribute for the bearer specific attribute, if 
        * it is mapped to any. Otherwise, it returns the original attribute.
        * @param aAttribute bearer specific attribute
        * @return common/original attribute
        */
        TUint32 MappedCommonAttribute( const TUint32 aAttribute ) const;        
    public:

        /**
        * Indicates if the Id is valid or if this is a new object which sitll
        * haven't been updated. 
        * Used by the object pool.
        * @return TBool
        */
        TBool IdIsValid();

        /**
        * Sets iIdIsValid attribute
        * Used by the object pool.
        */
        void SetIdValidity(TBool validity);
        
        /**
        * Scans aDests for destinations that may be valid parents 
        * for this plugin and removes those that may not. The function may be
        * overriden by virtual bearer type plugins.
        * @param aDests The array of destination IDs to be filtered.
        */
        virtual void FilterPossibleParentsL( RArray<TUint32>& /*aDests*/ ) {}
        
        /**
         * Sets the predefined id for the plugin. This should only be
         * used by cmmanager framework.
         */
        IMPORT_C void SetPreDefinedId( const TUint32 aPreDefId );

    public:

        /**
        * Check if there is space to save record
        * @return ETrue if space is available; otherwise leave with KLeaveWithoutAlert
        */
        IMPORT_C TBool CheckSpaceBelowCriticalLevelL() const;

    public: // Data
    
        // Required attribute for the framework
        // (An identifier used during destruction)
        TUid iDtor_ID_Key;
        
    protected:

        CCmManagerImpl&     iCmMgr;         //< Not owned    
        TUint32             iParentDest;    // id of the parent destinaton
        
        // IAP id of the CM. 0 means that this is a newly
        // created one.
        TUint32         iIapId;
        TUint32         iBearerType;        //< Should be set by plugin 
                                            //< constructor
        
        CommsDat::CCDIAPRecord*           iIapRecord;         // created by base class
        CommsDat::CCDProxiesRecord*       iProxyRecord;       // optional
        CommsDat::CCDRecordBase*          iServiceRecord;     // created by plugin
        CommsDat::CCDChargecardRecord*    iChargeCardRecord;  // optional
        CommsDat::CCDNetworkRecord*       iNetworkRecord;     // created by base class
        CommsDat::CCDLocationRecord*      iLocationRecord;    // optional
        
        TBool           iChargeCardEnabled;
        TBool           iLocationEnabled;

        TUint32         iInvalidAttribute;

    private: // Data
        
        CCmPluginBasePrivate* iPriv;
        //Reference counter for the object pool
	    TInt                  iRefCounter;
        //indicates if the Id is valid or not (for the object pool)
        TBool                 iIdIsValid;
        // Predefined connection method id(iap id)
        TUint32               iPreDefIapId;
	    friend class CCmPluginBase;
    };
    
#endif // CMPLUGINBASEENG_H
