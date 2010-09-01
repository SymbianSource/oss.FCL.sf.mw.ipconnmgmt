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
* Description:  Implementation of destination class.
*
*/

#ifndef DESTINATIONIMPL_H
#define DESTINATIONIMPL_H

// User includes
#include "cmmanager.hrh"
#include <cmmanagerdef.h>

// System includes
#include <e32def.h>
#include <e32base.h>
#include <metadatabase.h>
#include <commsdattypesv1_1.h>

// CONSTANTS
const TInt KCmInitIndex =  1;
const TInt KNumOfIcons = 12;

#define RECORD_FROM_ELEMENT_ID( elementId ) \
                                    (( elementId & KCDMaskShowRecordId ) >> 8 )

enum TSnapMetadataProtecionLevel
    {
    EProtectionLevel0 = 0x00000000,
    EProtectionLevel1 = 0x10000000,
    EProtectionLevel2 = 0x20000000
    };
    
#define KSnapMetadataProtecionLevelMask 0xF0000000;

// DATA TYPES    
    
// FORWARD DECLARATIONS
class CCmManagerImpl;
class CCmPluginBaseEng;
class CCmPluginBase;
class TCmPluginInitParam;
class CPluginItem;
class CCDDataMobilitySelectionPolicyRecord;
class CGulIcon;
class CCmTransactionHandler;
class CCDSNAPMetadataRecord;

typedef CArrayPtrFlat<CPluginItem> CCmArray;

#define KFlagRemove 0x00000001

typedef CCDDataMobilitySelectionPolicyRecord CCDSnapRecord;

const TUint32 KCmDefaultDestinationAPTagId = 0x1000;

// CLASS DECLARATION
NONSHARABLE_CLASS(CPluginItem) : public CBase
    {
    public:
    
        enum
            {
            ERemoveCm   = 0x00000001
            };
            
    public:
        
        CCDSnapRecord*     iDNRecord;
        CCmPluginBase*   iPlugin;
        TUint32             iFlags;
    };        

/**
 * Holds all the destination specific data of the CCmDestinationImpl
 * This class is put in the object pool and may be shared
 * among CCmDestinationImpl objects
 */
NONSHARABLE_CLASS( CCmDestinationData ): public CBase
    {
    //=====================================================================
    // Constructors/Destructors
    // 
    public:
        CCmDestinationData( CCmManagerImpl& aCmMgr );
        ~CCmDestinationData();
        /**
        * Increments the reference counter. Used by the object pool.
        */
        void IncrementRefCounter();
        /**
        * Increments the reference counter. Used by the object pool.
        */
        void DecrementRefCounter();
    private:
        /**
        * Reset and, if needed, destroy the passed array.
        * @since S60 3.2
        * @param aArray array to be reseted and deleted
        * @parar aDestroy ETrue if array has to be deleted
        */
        void ResetAndDestroyArray( CCmArray* &aArray, TBool aDestroy );
        
    public:
        /**
        * Returns the number of connection methods of the destination
        * @param None
        * @return TInt        
        */                  
        TInt ConnectionMethodCount();
        
        /**
        * Returns the destination's Id attribute
        * @param None
        * @return TUint32        
        */            
        inline TUint32 Id(){return iDestAPRecord->iRecordTag;};
                
        /**
        * Returns the destination's ElementId attribute
        * @param None
        * @return TUint32        
        */            
        inline TUint32 ElementId() 
        { return iNetworkRecord->ElementId() & KCDMaskShowField;};
    
        /**
        * Returns that the destination is protected or not.
        * @return protection level   
        */
        CMManager::TProtectionLevel ProtectionLevel();
        
        /**
        * Returns that the destination is hidden or not.
        * @param None
        * @return TBool   
        */
        TBool IsHiddenL(); 

        /**
        * Indicates if the Id is valid or if this is a new object which sitll
        * haven't been updated. 
        * Used by the object pool.
        * @return TBool
        */
        TBool IdIsValid();

        /**
        * Returns the reference counter. Used by the object pool.
        * @return TInt
        */
        TInt GetRefCounter();
        /** 
        * Returns pointer to the CmManager object
        * Used in 'R' classes
        */
        CCmManagerImpl* Cmmgr();

        /**
        * Returns the Metadata
        * @param None
        * @return TUint32        
        */            
        TUint32 MetadataL( CMManager::TSnapMetadataField aMetaField ) const;

    private:
        CCmManagerImpl&               iCmMgr;         //< Not owned
        CommsDat::CCDNetworkRecord*   iNetworkRecord;
                
        //To store Connection methods that belong to this destination
        CCmArray* iConnMethodArray;
        CCmArray* iDeleteCmArray;
        
        // DN_IAP Table has to be update if
        // - new CM is added to the destination
        // - CM is delete from the destination
        // - CM priority modified
        TBool iDNTableChanged;
        
        //Protection changed
        TBool   iProtectionChanged;
        //The last protection level of the destination
        TUint   iLastProtectionLevel;
        
        CCDSNAPMetadataRecord* iMetaDataRec;
        //Reference counter for the object pool
        TInt                iRefCounter;
        //indicates if the Id is valid or not (for the object pool)
        TBool               iIdIsValid;                
        CommsDat::CCDAccessPointRecord* iDestAPRecord;

        friend class CCmDestinationImpl;
        friend class CCmManagerImpl;
        TBool               iNewWithId;
    };
/**
 *  RCmDestination is for getting/setting values of a network destination.
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCmDestinationImpl ) : public CBase
    {
    public:
    //=====================================================================
    // Constructors/Destructors
    // 
    public:
    
        /** Destructor. */
        ~CCmDestinationImpl();
        
        /** Epoc constructor 
        * This should be used only by the object pool
        */
        IMPORT_C static CCmDestinationImpl* NewL( CCmManagerImpl& aCmMgr,
                                                  const TDesC& aName );
        
        /** Epoc constructor 
        * This should be used only by the object pool
        */
        static CCmDestinationImpl* NewL( CCmManagerImpl& aCmMgr,
                                         CCmDestinationData* aData );
        
        /** Epoc constructor 
        * This should be used only by the 'R' classes
        */
        static CCmDestinationImpl* NewLC( CCmDestinationData* aData );
        
        /** Epoc constructor 
        * This should be used only by the object pool
        */
        IMPORT_C static CCmDestinationImpl* NewL( CCmManagerImpl& aCmMgr,
                                                  TUint aDestinationId );
        
    public:
        /** 
        * Returns pointer to the CmManager object
        * Used in 'R' classes
        */
        CCmManagerImpl* Cmmgr();
        //=====================================================================
        // Conn method handling
        
        /**
        * Create a connection method belonging to the destination
        * @param aImplementationUid - implementation uid of the connection 
        * method
        * @return CCmPluginBaseEng*
        */
        CCmPluginBase* CreateConnectionMethodL( TUint32 aBearerType );

        /**
        * Create a connection method belonging to the destination
        * @param aBearerType - implementation uid of the connection 
        * method
        * @param aConnMethId - Predefined id for the connection method.
        * @return CCmPluginBaseEng*
        */
        CCmPluginBase* CreateConnectionMethodL( TUint32 aBearerType,
                                                TUint32 aConnMethId );
        /*
        * Adds an existing connection method to a destination
        * @param
        * @return TInt - index in the Connection Method list
        */
        TInt AddConnectionMethodL( CCmPluginBase& aConnectionMethod );
        
        /*
        * Embeds an existing destination to the destination
        * @param CCmDestinationImpl - Destinatio to be embedded
        * @return TInt - index in the Connection Method list
        */
        TInt AddEmbeddedDestinationL( CCmDestinationImpl& aDestination );        
        
        /**
        * Remove a connection method from a destination and delete it
        * on update. 
        * Exception: conenction method is not deleted  if it's referenced 
        * from any other destination.
        * @param aConnectionMethod the connection method to be deleted
        * @return none.
        */
        void DeleteConnectionMethodL( CCmPluginBase& aConnectionMethod ); 
    
        /**
        * Remove connection method from the destination
        * @param aConnectionMethod connection method to be removed
        */
        void RemoveConnectionMethodL( const CCmPluginBase& aConnectionMethod );
    
        /**
        * Remove connection method from the destination
        * @param aConnectionMethod connection method to be removed
        */
        void RemoveConnectionMethodL( const CCmPluginBaseEng& aConnectionMethod );
        
        /**
        * Notification from child plugin that it was deleted.
        * Remove it from the array and update priorities.
        * @param aConnectionMethod child connection method that was deleted.
        */
        void ConnectionMethodDeletedL( const CCmPluginBase& aConnectionMethod );
        
        /**
        * Set the CCMItem's priority based on the passed new index
        * @param aCCMItem The connection method item
        * @param aIndex The new index of the item in the list
        */
        void ModifyPriorityL( CCmPluginBase& aCCMItem, TUint aIndex );

        /**
        * Obsolete 
        */
        TUint PriorityL( CCmPluginBaseEng& aCCMItem );

        /**
        * Return the priority of the passed connection method
        * @param aCCMItem queried connection method
        * @return the priority of the queried connection method 
        */
        TUint PriorityL( CCmPluginBase& aCCMItem );
        
        /**
        * Returns the number of connection methods of the destination
        * @param None
        * @return TInt        
        */                  
        IMPORT_C TInt ConnectionMethodCount();
        
        //Operator

        /**
        * Obsolete
        */
        IMPORT_C CCmPluginBaseEng* ConnectionMethodL( TInt anIndex );

        /**
        * Returns a reference to the connection method. 
        * The index must be less than the return value of ConnMethodCount()
        * @param anIndex
        * @return plugin instance - ownership NOT passed
        */         
        IMPORT_C CCmPluginBase* GetConnectionMethodL( TInt anIndex );

        /**
        * Returns a reference to the connection method. 
        * The index must be less than the return value of ConnMethodCount()
        * @param anIapId IAP id of the queried connection method
        * @return plugin instance - ownership NOT passed
        */         
        CCmPluginBase* ConnectionMethodByIDL( TUint32 anIapId );

        void ConnectMethodIdArrayL( RArray<TUint32>& aCmIds );
        
        //Getting attributes

        /**
        * Returns the destination's Name attribute
        * HBuf ownership is passed to the caller
        * @param None
        * @return name of the destination. ownership is passed.
        */      
        IMPORT_C HBufC* NameLC();                        
        
        /**
        * Returns the destination's Id attribute
        * @param None
        * @return TUint32        
        */
        inline TUint32 Id(){return iData->Id();};

        /**
        * Returns the destination's ElementId attribute
        * @param None
        * @return TUint32
        */            
        inline TUint32 ElementId(){ return iData->ElementId();};
     
        /**
        * Returns the Icon pointer
        * This fucton leaves if the client does not have a valid UI context
        * @param None
        * @return TUint32  
        */ 
        IMPORT_C CGulIcon* IconL();

        /**
        * Returns the Metadata
        * @param None
        * @return TUint32        
        */            
        TUint32 MetadataL( CMManager::TSnapMetadataField aMetaField ) const;
        
        /**
        * Returns if there's connection created with any of the destination's
        * connection method.
        * @return ETrue if there's connection with any of the destination's
        * connection method.
        */
        IMPORT_C TBool IsConnectedL() const;

        //=====================================================================
        //Setting Attributes
        
        /**
        * Set the destination's Name attribute. Leaves if the name already 
        * exists
        * @param aDestinationName value to set
        * @return None   
        */
        IMPORT_C void SetNameL( const TDesC& aName );
           
        /**
        * Set the destination's Icon index.
        * This fucton leaves if the client does not have a valid UI context
        * @param aIconIndex value to set
        * @return None   
        */
        IMPORT_C void SetIconL( TUint32 anIndex );
    
        /**
        * Set the Metadata
        * @param aMetaField field to be modified
        * @param aValue value to be set
        */            
        void SetMetadataL( CMManager::TSnapMetadataField aMetaField, 
                           TUint32 aValue );    
    
        /**
        * Returns that the destination is protected or not.
        * @return protection level   
        */
        IMPORT_C CMManager::TProtectionLevel ProtectionLevel();
        
        /**
        * Set the protection of the destination.
        * @param aProtectionLevel
        * @return None   
        */
        IMPORT_C void SetProtectionL( CMManager::TProtectionLevel aProtectionLevel );
        
        /**
        * Update all the attributes of the Connection Methods and the 
        * Destination.
        * @return None   
        */        
        IMPORT_C void UpdateL();         
    
        /**
        * Deletes it's connection methods from comms.dat (unless shared)
        * then deletes itself
        */
        IMPORT_C void DeleteLD();
        
        /**
        * Performs RollbackTransactionL().
        */
        void Close();
        
        /**
        * Set iPlugin to NULL in plugin array, but not delete it.
        * Next reference to this connection method will load the
        * plugin again.
        */
        void PluginClosed( CCmPluginBase* aPlugin );
        
        /**
        * Returns that the destination is hidden or not.
        * @param None
        * @return TBool   
        */
        IMPORT_C TBool IsHidden(); 
        
        /**
        * Set that the destination is hidden or not.
        * @param aHidden - True if it is hidden.
        * @return None   
        */
        IMPORT_C void SetHiddenL( TBool aHidden );         
        
        /**
        * Check if the destination element id is already embeded.
        * Leaves with KErrNotSupported (multiple levels of embedding is not 
        * supported)
        */
        void CheckIfEmbeddedL( TInt aId );
        
        
        static CGulIcon* IconIdL( CCmManagerImpl& aCmMgr,
                                  CommsDat::TMDBElementId aId );
                
        static CMManager::TProtectionLevel ProtectionLevelL( CCmManagerImpl& aCmMgr,
                                                  CommsDat::TMDBElementId aId );
        
        /**
        * Removes this destination from the object pool.
        */
        IMPORT_C void RemoveFromPool();

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
        * Returns iData. Used by the object pool and the API oprators.
        * @return TInt
        */
        CCmDestinationData* GetData();
        /**
        * Used by the object pool and the API oprators.
        */
        void SetData(CCmDestinationData* aData);
        /**
        * Indicates if the Id is valid or if this is a new object which sitll
        * haven't been updated. 
        * Used by the object pool.
        * @return TBool
        */
        TBool IdIsValid();

        /**
        * Returns pointer to embedded destination if there is any
        * @return CCmDestinationImpl* Pointer to embedded destination
        * Contains NULL if there is no destination embedded
        */        
        CCmDestinationImpl* EmbeddedL();
        
        /**
        * Checks if this destination plus a possible embedded destinaion
        * contains a connection method which can be used for a connection
        * Doesn't check VPNs!
        * @return TBool ETrue if this destination or a destination embedded into
        * it contains a connection method. Otherwise returns EFalse
        */            
        TBool CanUseToConnectL();
        
        
        /**
        * Reload connection method under this destination so as to
        * keep synchronization with CommsDat
        */
        void ReLoadConnectionMethodsL();

        /** Epoc constructor 
        * This should be used only by the object pool
        */
        static CCmDestinationImpl* NewL( CCmManagerImpl& aCmMgr,
                                         const TDesC& aName,
                                         TUint32 aDestId);
                                         

        /**
        * Returns the Id of this Destination which is used to identify the Destination.
        * It is the Id saved to Destination Access Point's Tag field.
        */            
        TUint32 GetDestinationAPTagId();

        /**
        * Checks if this destination already contains the given Connection Method
        * @param aConnectionMethod Connection Method to be checked whether it 
        * already exists.
        * @return TBool ETrue if this Destination contais already the given CM. If the
        * Destination does not contain the given CM returns EFalse. Otherwise leaves
        * with system-wide error code.
        */            
        TBool CheckIfCMExistsL( CCmPluginBase& aConnectionMethod );

            
    //=========================================================================
    // Construction
    //
    private:    
    
        /**
        * Default constructor. 
        * @param aCmManager connection method manager
        */
        CCmDestinationImpl( CCmManagerImpl& aCmMgr );

        /*
        * Create a new destination with the specified name
        */
        void ConstructL( const TDesC& aName );

        void ConstructL( const TDesC& aName, TUint32 aDestId );

        /*
        * Create a destination and initializes it 
        * from the commsdat based on the id
        */
        void ConstructL( TUint aDestinationId );

    //=========================================================================
    // New functions
    //
    private:
                          
        /**
        * Check if the destination contains and embedded destination
        * @since S60 3.2
        * @return ETrue if the destination contains and embedded destination   
        */
        TBool HasEmbeddedL();        
    
        /**
        * - Check if transaction is already opened
        * - Set iOwnTransaction is not and opens it and
            put 'this' on cleanup&close stack
        */
        void OpenTransactionLC();
        
        /**
        * - Check if the transaction is ours.
        * - If yes, commits the transaction.
        * - anyway does nothing
        * @praram aError - error id
        */
        void CommitTransactionL( TInt aError );
        
        /**
        * Rollback transaction.
        */
        void RollbackTransaction();

        /**
        * Returns the CommsDat session.
        * Do not close!
        * @return commsdat session
        */
        CommsDat::CMDBSession& Session()const;
        
        /**
        * Find the connection method in iConnMethodArray.
        * @param aConnectionMethod connection method to be searched for
        * @return index of the found item
        */
        TInt FindConnectionMethodL( const CCmPluginBase& aConnectionMethod );
        
        /**
        * Find the connection method in iConnMethodArray.
        * @param aConnectionMethod connection method to be searched for
        * @return index of the found item
        */
        TInt FindConnectionMethodL( const CCmPluginBaseEng& aConnectionMethod );
        
        /**
        * Update protection attributes of every connection method if changed.
        */
        void UpdateProtectionL();

        /**
        * Update every connection method.
        */
        void UpdateConnectionMethodsL();
        
        /**
        * Create or update the record of the network table for this 
        * destination.
        */
        void UpdateNetworkRecordL();

        /**
        * Update the SNAP table
        */
        void UpdateSNAPTableL();
        
        /**
        * Update the SNAP Metadata table
        */        
        void UpdateSNAPMetadataTableL();
        
        /**
        * Load network record. Destination id is passed, because
        * id is from the client application, and destinaion id
        * is the network id for them. :)
        * @param aDestinationId network record id
        */        
        void LoadNetworkRecordL( TUint32 aDestinationId );

        /**
        * Load Destination AP record. Destination id is passed, because
        * id is from the client application, and destinaion id
        * is the network id for them. :)
        * @param aDestinationId network record id
        */        
        void LoadDestAPRecordL( TUint32 aDestinationId );

        /**
        * Load connection methods of the destination
        */        
        void LoadConnectionMethodsL();
        
        /**
        * Load SNAP Metadata table
        */        
        void LoadSNAPMetadataTableL();        
        
        /**
        * Create a connection method belonging to the destination
        * @param aImplementationUid - implementation uid of the connection 
        * method
        * @param aParams - plugin init param
        * @return CCmPluginBaseEng*
        */        
        CCmPluginBase* CreateConnectionMethodL( TUid aImplementationUid, 
                                                   TCmPluginInitParam& aParams );
                
        /**
        * Compare two records from DNIAP by their priority field.
        * @param aLeft one record from DN_IAP table.
        * @param aRight one record from DN_IAP table.
        * @return
        *   - negative value, if aLeft is less than aRight;
        *   - 0,              if aLeft equals to aRight;
        *   - positive value, if aLeft is greater than aRight.
        */
        static TInt SortRecordsByPriority( const CommsDat::CMDBRecordBase& aLeft, 
                                           const CommsDat::CMDBRecordBase& aRight);
        
        /**
        * Add an item to the Connection Method array based on bearer priority 
        * table.
        * @param aItem An item to be added.
        * @return TInt the index of the added item. The array contains an initial item,
        * so always SUBTRACT KCmInitIndex to get the connection method index number!!!
        */        
        TInt AddToArrayL( CPluginItem* aItem );
        
        /**
        * IAP is considered to be removed from the commsdat by third 
        * party (someone else external).
        * Seamlessness entries should also be removed.
        * @param aIapId Iap was removed        
        */        
        void CleanUpSeamlessnessTableL( TUint32 aIapId );
        
        /**
        * Embedded dest is considered to be removed from the commsdat 
        * by third party (someone else external).
        * Snap Metadata entries should also be removed.
        * @param aSnap Id was removed        
        */          
        void CleanUpSnapMetadataTableL( TUint32 aSnap );        
        
        /**
        * Embedded dest is considered to be removed from the commsdat 
        * by third party (someone else external).
        * Snap Metadata entries should also be removed.
        * @param aSnap Id was removed        
        */          
        void CleanUpEmbeddedL( TUint32 aDestId );        
        
        /**
        * Create and load plugin.
        * @param item where plugin has to be loaded.
        */
        void LoadPluginL( CPluginItem& aItem );
        
        
        /**
        * Create embedded destination plugin.
        * @param aItem - plugin item
        * @param TUint32 destination id
        */        
        void CreateEmbeddedDestinationPluginL( CPluginItem& aItem, 
                                               TInt aDestinationId );
        /**
        * Load embedded destination plugin.
        * @param aItem - plugin item
        */        
        void LoadEmbeddedDestinationPluginL( CPluginItem& aItem );
        
        /**
        * Check if there is destination with this name. 
        * Leaves with KErrAlreadyExists if name already exists
        * @param aName destination name
        */        
        void CheckIfDestinationNameExistL( const TDesC& aName );
        
        /**
        * Set or clear attribute
        * @aRecord record of which the attribute is to be set or clear
        * @aAttribute attribute to be set or clear
        * @aSet ETrue to set
        */        
        void SetAttribute( CommsDat::CMDBRecordBase* aRecord, 
                           TUint32 aAttribute, 
                           TBool aSet );


        /**
        * Delete all connection methods.
        * @since S60 3.2
        */
        void DeleteConnectionMethodsL();
        
        /**
        * Reference to a non-existing IAP found in the SNAP table.
        * Remove the SNAP record and the metadata record of this
        * IAP. IAP is most probably deleted by APEngine.
        * @param aSnapRecord SNAP record point to a non-existing IAP
        */
        void DeleteIAPRecordsL( CommsDat::CMDBRecordBase* aSnapRecord );
        
        /**
        * Check if the name of the passed record is the same as the name of 
        * network one, and update it if not.
        * @param aRecord record to be checked and updated if needed
        */
        void CheckIfNameModifiedL( CCDSnapRecord* aRecord );
        
        /**
        * Checks if there's another SNAP with Internet bit set.
        * Leaves with KErrAlreadyExists if so.
        */
        void CheckIfInternetExistsL();
        
        /**
        * Return the index value of the connection method.
        * @param aCmId id of the connection method
        * @return index value in iConnMethodArray
        */
        TInt ConnectionMethodIndex( TUint32 aCmId );
        
        /**
        * Retrieves the base of the priority counts in cm list
        * It is the data of the plugin in case of none VPN.
        * It is the data of the underlying cm in case of VPN over cm.
        * It is "wildcard" in case of VPN over destination. 
        * aRetBearer and aRetExtLevel are not filled in this case 
        * @param aItem the plugin list item which data is get
        * @param aRetPrio the default priority of the cm in return
        * @param aRetBearer the bearer of the cm in return
        * @param aRetExtLevel the extenson level of the cm in return
        * @return ETrue if the cm is virtual else EFalse
        */
        TBool PrioDataL( CPluginItem* aItem, TUint32& aRetPrio, 
                        TUint32& aRetBearer, TUint32& aRetExtLevel );
                        
                            
                            
        /**
        * Determines the position of the inserted item into the cm list.
        * This determination is based on the similarity of the bearer types.
        * The inserted position is just after the last item with the
        * same bearer type and virtuality - if exist else -1. 
        * @param aBearerType bearer type od the inserted cm
        * @param aIsVirtual virtuality of the inserted cm 
        * @return The position in the cm list where the item should be inserted.
        * If there is no cm with same bearer type in the list then it
        * returns -1;
        */
        TInt InsertSameMethodL( TUint32 aBearerType, TBool aIsVirtual );

        /**
        * Determines the position of the inserted item into the cm list.
        * This determination is based on the priority and the extension
        * level of the cm's. 
        * @param aPrio default bearer type of the inserted cm
        * @param aExtLevel extension level of the inserted cm 
        * @return The position in the cm list where the item should be inserted.
        */
        TInt InsertPriorityMethodL( TUint32 aPrio, TUint32 aExtLevel );
                                 
        /**
        * Checks if there's another SNAP with the same localised bit set.
        * Leaves with KErrAlreadyExists if so.
        */
        void CheckIfLocalisedDestExistL( TUint32 aValue );

        /**
        * Checks if there's another SNAP with the same purpose bit set.
        * Leaves with KErrAlreadyExists if so.
        */
        void CheckIfPurposeExistL( TUint32 aValue );

        static CGulIcon* IconL( TInt aIconIndex );
        static CMManager::TProtectionLevel ProtectionLevel( CCDSNAPMetadataRecord* aMetaDataRec );

        /**
        * Deletes all the recordsrelated to this Destination to be
        * deleted.
        * @param None
        * @return        
        */
        void DeleteRelatedRecordsL( TInt aCount );

        /**
        * Checks if this(current) destination is referenced by any
        * Virtual Connection Method.
        * @param None
        * Leaves with KErrLocked if it's referenced.
        */
        void CheckIfReferencedByVirtualCML();

        /**
        * Initializes the Destination AccessPoint record
        * deleted.
        * @param aDestAPRecord Record to be initialized
        * @return
        */
        void InitializeDestAPRecordL();

        /**
        * Sets the given (tag)id to the accesspoint record
        * @param aDestId Id to be set
        * @return
        */
        void SetIdL( TInt aDestId );

        /**
        * Checks that there isn't any destination with the given (tag)id.
        * @param aDestId Id to be checked
        * @return
        */
        void CheckIfDestinationIdExistsL( TUint32 aDestId );

        /**
        * Create or update the record of the accesspoint table for this 
        * destination.
        */
        void UpdateDestAPRecordL();

        /**
        * Creates an unique (tag)id to the record of the accesspoint table for this 
        * destination.
        */
        TUint32 CreateDestinationAPTagIdL();

        /**
        * Sets the default Tier Manager to the accesspoint record for this 
        * destination.
        */
        void SetDefaultTierManagerL();

        /**
        * Sets the default Meta Connection Provider to the accesspoint record for this 
        * destination.
        */
        void SetDefaultMCprL();

        /**
        * Sets the default Connection Provider to the accesspoint record for this 
        * destination.
        */
        void SetDefaultCprL();

        /**
        * Sets the default Subconnection Provider to the accesspoint record for this 
        * destination.
        */
        void SetDefaultSCprL();

        /**
        * Sets the default Protocol to the accesspoint record for this 
        * destination.
        */
        void SetDefaultProtocolL();
        
    //=========================================================================        
    // Member variables
    //
    private: 
        CCmManagerImpl&     iCmMgr;         //< Not owned
        CCmDestinationData* iData;
    private:
    
        friend class CCmManagerImpl;
    };
    
#endif // DESTINATIONIMPL_H
