/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef CMPLUGINBASE_H
#define CMPLUGINBASE_H

// INCLUDES
#include <cmpluginbaseeng.h>

/**
 *  This class wraps the CCmPluginBaseEng class and is used instead of that 
 *  in the framework and the 'R' classes. This way the CCmPluginBaseEng
 *  class could remain in the object pool after the CCmPluginBase object is
 *  deleted.
 *
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
class CCmPluginBase : public CBase
    {
    public:
    
    
        /** Constructor */
        IMPORT_C CCmPluginBase( CCmPluginBaseEng* plugin );
        
        /** Destructor. */
        IMPORT_C ~CCmPluginBase();
                        
        /**
        * Create a new instance of the given bearer type plugin.
        * Used from framework to avoid using slow ECom framework.
        * @param aInitParam initialization data
        */
        CCmPluginBase* 
                CreateInstanceL( TCmPluginInitParam& aInitParam );
                
    public: // Attribute handlers
    
        /**
        * Gets the value for a TInt attribute.
        * @param aAttribute Identifies the attribute to be retrived.
        * @return contains the requested TInt attribute.
        */
        IMPORT_C TUint32 
                        GetIntAttributeL( const TUint32 aAttribute ) const;

        /**
        * Gets the value for a TBool attribute.
        * @param aAttribute Identifies the attribute to be retrived.
        * @return contains the requested TBool attribute.
        */
        IMPORT_C TBool 
                        GetBoolAttributeL( const TUint32 aAttribute ) const;

        /**
        * Gets the value for a String16 attribute.
        * @param aAttribute Identifies the attribute to be retrived.
        * @return copy of the requested attribute. Ownership is passed.
        */
        IMPORT_C HBufC* 
                        GetStringAttributeL( const TUint32 aAttribute ) const;
        
        /**
        * Gets the value for a String8 attribute.
        * @param aAttribute Identifies the attribute to be retrived.
        * @return copy of the requested attribute. Ownership is passed.
        */
        IMPORT_C HBufC8* 
                        GetString8AttributeL( const TUint32 aAttribute ) const;

        /**
        * Sets the value for a TInt attribute.
        * @param aAttribute Identifies the attribute to be set.
        * @param aValue The value to be set.
        * @return None.
        */
        IMPORT_C void SetIntAttributeL( 
                             const TUint32 aAttribute, TUint32 aValue );

        /**
        * Sets the value for a TBool attribute.
        * @param aAttribute Identifies the attribute to be set.
        * @param aValue The value to be set.
        * @return None.
        */
        IMPORT_C void SetBoolAttributeL( 
                             const TUint32 aAttribute, TBool aValue );

        /**
        * Sets the value for a String16 attribute.
        * @param aAttribute Identifies the attribute to be set.
        * @param aValue The value to be set.
        * @return None.
        */
        IMPORT_C void SetStringAttributeL( 
                             const TUint32 aAttribute, const TDesC16& aValue );

        /**
        * Sets the value for a String8 attribute.
        * @param aAttribute Identifies the attribute to be set.
        * @param aValue The value to be set.
        * @return None.
        */
        IMPORT_C void SetString8AttributeL( 
                             const TUint32 aAttribute, const TDesC8& aValue );

        /**
        * Restore the original value of the attribute from commsdat field.
        * Base implementation can be used only with attributes 
        * stored directly in commsdat.
        * @param aAttribute attribute to be restored
        */
        IMPORT_C void RestoreAttributeL( const TUint32 aAttribute );
        
        /**
        * Returns the CCmDestinationImpl* if this is an 
        * embedded destination CM. Otherwise returns NULL.
        * @return embedded destination object or NULL
        */
        IMPORT_C CCmDestinationImpl* Destination() const;
        
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
        IMPORT_C void UpdateL();     
        
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
        IMPORT_C TBool DeleteL( TBool aForced,
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
        IMPORT_C void LoadL( TUint32 aIapId );

        /**
        * Reloads CM
        */
        void ReLoadL();

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
        IMPORT_C void CreateNewL();
        
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
        IMPORT_C CCmPluginBase* CreateCopyL() const;
        
        /*
        * Returns true if the CM has more than one parent destination
        */        
        IMPORT_C TInt NumOfConnMethodReferencesL();

    public:
    
        /**
        * Returns a pointer to the cmmanager
        * @return a pointer to the cmmanager
        */
        IMPORT_C CCmManagerImpl& CmMgr() const ;

        IMPORT_C CCmPluginBaseEng* Plugin() const ;

        IMPORT_C CommsDat::CMDBSession& Session() const;

       
        /**
        * Checks if the plug-in can handle the given AP.
        * @param aIapId IAPId of the AP to be checked
        * @return ETrue if plug-in can handle the IAP
        */
        TBool CanHandleIapIdL( TUint32 aIapId ) const;
        
        /**
        * Checks if the plug-in can handle the given AP.
        * @param aIapRecord IAP record to be checked
        * @return ETrue if plug-in can handle the IAP
        */
        TBool CanHandleIapIdL( CommsDat::CCDIAPRecord* aIapRecord ) const;
        
        /**
        * Return the parent destination of this connection method.
        * @return parent destination of this connection method
        */
        IMPORT_C CCmDestinationImpl* ParentDestination() const ;
        
        /**
        * Calls FeatureManager::FeatureSupported directly.
        * @param aFeature feature ID
        * @return feature support status
        * @since 3.2
        */
        IMPORT_C static TBool FeatureSupported( TInt aFeature );
                
        /**
        * Function should be overriden by bearer type plugins.
        * Returns if the passed IAP id is linked to that connection method.
        * @param aIapId IAP id to be checked
        * @return ETrue if the passed IAP id is linked to that connection method.
        */
        IMPORT_C TBool IsLinkedToIap( TUint32 aIapId );
        
        /**
        * Function should be overriden by bearer type plugins.
        * Returns if the passed SNAP id is linked to that connection method.
        * @param aSnapId SNAP id to be checked
        * @return ETrue if the passed SNAP id is linked to that connection method.
        */
        IMPORT_C TBool IsLinkedToSnap( TUint32 aSnapId );

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
        IMPORT_C void GetAdditionalUids( RArray<TUint32>& aIapIds );

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
        
        /** Second phase constructor */
        IMPORT_C void ConstructL();
    
        /** Open transaction */
        void OpenTransactionLC();
        
        /**
        * Commit transaction.
        * @param aError - error id
        */
        void CommitTransactionL( TInt aError );
        
        /** Make a reset on this class. */
        IMPORT_C void Reset();
        
        /**
        * Plug-in implementation can make its on
        * cleanup in this function. 
        * Always called from LoadL() and 
        * should be called from plugin's destructor.
        */
        void AdditionalReset();
        
        /**
        * Check if there's connection established with
        * this connection method (IAP id).
        */
        IMPORT_C TBool CheckIfAlreadyConnected() const;
        
    protected:  // loader function
        
        /**
        * Inherited class can make some preraration
        * before CCmPluginBaseEng would start loading
        * records from tables.
        */
        IMPORT_C void PrepareToLoadRecordsL();

        /**
        * Inherited class can load any additional
        * records after CCmPluginBaseEng finished.
        */
        void LoadAdditionalRecordsL();

        /**
        * Load service record. Default implementation can
        * create only factory supported record instances.
        */
        IMPORT_C void LoadServiceSettingL();

    protected:  // update functions
    
        /**
        * Check if connection method data is valid, before
        * it would be persisted to commsdat.
        * If invalid data found, set its attribute id in
        * iInvalidAttribute.
        * Base class implementation must be called from
        * overridden one.
        */
        IMPORT_C void PrepareToUpdateRecordsL();
        
        /**
        * Inherited class can update its additional
        * records here.
        */
        void UpdateAdditionalRecordsL();

        /**
        * Update service record.
        */
        IMPORT_C void UpdateServiceRecordL();

        /**
        * Return the service table name and record id 
        * of this connection in the service table.
        * @param - aServiceName name of the service table
        *           this connection method points to.
        * @param - aRecordId record id in the service table.
        */
        void ServiceRecordIdLC( HBufC* &aServiceName, 
                                       TUint32& aRecordId );

        /**
        * Return the bearer table name and record id 
        * of this connection in the bearer table.
        * @param - aBearerName name of the bearer table
        *           this connection method points to.
        * @param - aRecordId record id in the bearer table.
        */
        void BearerRecordIdLC( HBufC* &aBearerName, 
                                       TUint32& aRecordId );

    protected:  // delete function
    
        /**
        * Plugin can prepare to delete the connection method
        * with this function. Called from DeleteL() before
        * any record would be deleted. Do NOT delete any
        * record here.
        */
        void PrepareToDeleteRecordsL();
        
        /**
        * Plugin can delete additional, bearer specific record
        * with this function. Called from DeleteL().
        */
        void DeleteAdditionalRecordsL();

    protected:  // create function
    
        /**
        * Plugin has to create and initialize its bearer specific object
        * that points to the service record here. Pointer has to be stored in 
        * iServiceRecord data member.
        * Do NOT store service record in CommsDat yet.
        */
        void CreateNewServiceRecordL();
        
        /**
        * Plugin can create and initialize its bearer specific record(s) here.
        * Do NOT store them in CommsDat yet.
        */
        void CreateAdditionalRecordsL();
        
    protected:

        /**
        * Called from CreateCopyL().
        * Plugin can prepare for copying every records
        * added to the conversion tables.
        *
        * @param aDestInst copy attributes into this plugin instance
        */
        void PrepareToCopyDataL( CCmPluginBaseEng& aDestInst ) const;
                
        /**
        * Copy data of the connection method that is not 
        * added to the conversion table. (e.g. bearer specific
        * flags)
        *
        * @param aDestInst copy attributes into this plugin instance
        */    
        void CopyAdditionalDataL( CCmPluginBaseEng& aDestInst ) const;

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
        /** 
        * Returns pointer to the CmManager object
        */
        CCmManagerImpl* Cmmgr();

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
        void SetPlugin(CCmPluginBaseEng* aPlugin);
        
        /**
        * Scans aDests for destinations that may be valid parents 
        * for this plugin and removes those that may not.
        * @param aDests The array of destination IDs to be filtered.
        */
        void FilterPossibleParentsL( RArray<TUint32>& aDests );
        
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

    private: // Data
	    CCmPluginBaseEng*      iPlugin;
	    
	    friend class CCmPluginBaseEng;
    };
    
#endif // CMPLUGINBASE_H



