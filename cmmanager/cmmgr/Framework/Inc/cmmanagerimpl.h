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
* Description:  Implementation of connection method manager
*
*/

#ifndef CMMANAGERIMPL_H
#define CMMANAGERIMPL_H

// User Includes
#include "cmmanager.hrh"
#include "cmobjectpool.h"
#include "cmconnectionmethodinfo.h"
#include <datamobilitycommsdattypes.h>
#include <cmmanagerdef.h>
#include <cmdefconnvalues.h>
#include <cmgenconnsettings.h>

// System Includes
#include <e32std.h>
#include <e32base.h>
#include <e32cmn.h>
#include <f32file.h>    // RFs
#include <barsc.h>      // RResourceFile
#include <metadatabase.h>
#include <commsdattypesv1_1.h>

// Forward Declarations
class CCmTransactionHandler;
class CCmDestination;
class CCmDestinationExt;
class CCmDestinationImpl;
class CCmPluginBaseEng;
class CCmPluginBase;
class CImplementationInformation;
class TBearerPriority;
class RConeResourceLoader;
class CCmConnectionMethodInfo;
class CGulIcon;
class CCDGlobalBearerTypePriorizationRecord;
class CCDDataMobilitySelectionPolicyRecord;
class TCmPluginInitParam;
class CCmDestinationData;
class RCmManagerExt;
class MCmCommsDatWatcher;
class CCmCommsDatNotifier;

// DATA TYPES
typedef RPointerArray<CImplementationInformation>   RImplInfoPtrArray;
typedef CCDGlobalBearerTypePriorizationRecord       CCmBearerPriRec;
typedef CCDDataMobilitySelectionPolicyRecord        CCDSnapRecord;

typedef struct
    {
    RResourceFile       iFile;  // resource file handler
    TFileName           iFName; // resource filename
    TInt                iRefs;  // number of references to this res file
    }TResourceFiles;
    
/*#ifdef _DEBUG
void DumIapTableL( CommsDat::CMDBSession& aSession );
#define DUMP_IAP_TBL( session )  DumIapTableL( (session) );
#else   // _DEBUG
#define DUMP_IAP_TBL( session )
#endif  // _DEBUG*/

/**
 *  Implementation of connection method manager
 *
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCmManagerImpl ) : public CBase
      {          
    // Construction/Destruction
    public:
    
        /** Epoc constructor */
        static CCmManagerImpl* NewL( TBool aCreateTables = ETrue );
        
        /** Destructor */
        virtual ~CCmManagerImpl();
        
    protected:
    
        /** First stage constructor */
        CCmManagerImpl();
    
        /** Epoc constructor */
        void ConstructL(); // second-phase constructor
        
    public: // Transaction handling API
    
        void OpenTransactionLC( TBool aSetAttribs = ETrue );
        void CommitTransactionL( TInt aError );
        void RollbackTransaction();
        
        // This will be called when the first UI view is on
        void StartCommsDatNotifierL();
        
    public: // Basic CM attribute query API
    
        /**
        * GetBearerInfoXXX function can be used to query
        * bearer informantion that doesn't belong
        * to a specific connection method, such as 
        * ECmCoverage or ECmDefaultPriority etc.
        * HBuf ownership is passed to the caller
        */
        IMPORT_C TUint32 GetBearerInfoIntL( TUint32 aBearerType,
                                            TUint32 aAttribute ) const;
        IMPORT_C TBool GetBearerInfoBoolL( TUint32 aBearerType,
                                           TUint32 aAttribute ) const;
        IMPORT_C HBufC* GetBearerInfoStringL( TUint32 aBearerType,
                                              TUint32 aAttribute ) const;
        IMPORT_C HBufC8* GetBearerInfoString8L( TUint32 aBearerType,
                                                TUint32 aAttribute ) const;
                    
        /**
        * GetConnectionMethodInfoXXX function can be used to 
        * query any, non-bearer specific information about
        * a given connection method. This can be e.g. ECmBearerType,
        * ECmName, ECmStartPage etc.
        * HBuf ownership is passed to the caller
        */
        IMPORT_C TUint32 GetConnectionMethodInfoIntL( 
                                                TUint32 aCmId,
                                                TUint32 aAttribute ) const;
        IMPORT_C TBool GetConnectionMethodInfoBoolL( 
                                                TUint32 aCmId,
                                                TUint32 aAttribute ) const;
        IMPORT_C HBufC* GetConnectionMethodInfoStringL( 
                                                TUint32 aCmId,
                                                TUint32 aAttribute ) const;
        IMPORT_C HBufC8* GetConnectionMethodInfoString8L( 
                                                TUint32 aCmId,
                                                TUint32 aAttribute ) const;
                
                    
    //=========================================================================
    // Creation/Destruction API
    //
    public:
              
        /**
        * Creates a Destination by the given name
        *
        * @since 3.2
        * @param aName the name of the new destination
        * @return CCmDestination*, ownership is passed to the caller
        */
        CCmDestinationImpl* CreateDestinationL( const TDesC& aName );

        /**
        * Creates a Destination by the given name
        *
        * @since 3.2
        * @param aName the name of the new destination
        * @param aDestId Predefined id for the destination
        * @return CCmDestination*, ownership is passed to the caller
        */
        CCmDestinationImpl* CreateDestinationL( const TDesC& aName,
                                                TUint32      aDestId );

        
        /**
        * Creates a connection method does not belong to any destination
        *
        * @since 3.2
        * @param aImplementationUid - implementation uid of the connection 
        * method
        * @param aParentDestination - the destination which holds the 
        *                             connection method, NULL means there is
        *                             no parent
        * @return CCmPluginBase*
        */
        CCmPluginBase* CreateConnectionMethodL( TUint32 aBearerType,
                            CCmDestinationImpl* aParentDestination = NULL );

        /**
        * Creates a connection method does not belong to any destination
        *
        * @since 3.2
        * @param aImplementationUid - implementation uid of the connection 
        * method
        * @param aConnMethodId Predefined id for the connection method
        * @param aParentDestination - the destination which holds the 
        *                             connection method, NULL means there is
        *                             no parent
        * @return CCmPluginBase*
        */
        CCmPluginBase* CreateConnectionMethodL( TUint32 aBearerType,
        		            TUint32 aConnMethodId,             
                            CCmDestinationImpl* aParentDestination = NULL );
        
        /**
        * Creates a connection method does not belong to any destination
        *
        * @since 3.2
        * @param pluginBaseEng - Object to put in the pool
        * @return CCmPluginBase*
        */
        CCmPluginBase* CreateConnectionMethodL( 
                            CCmPluginBaseEng* pluginBaseEng );

        /**
        * Insert a connection method does not belong to any destination
        * API doesn't use CCmPluginBase. So, nothing returns
        *
        * @param pluginBaseEng - Object to put in the pool
        */
        void InsertConnectionMethodL( 
                            CCmPluginBaseEng* pluginBaseEng );
        
        /**
        * Creates a connection method does not belong to any destination
        *
        * @since 3.2
        * @param aBearerType - implementation uid of the connection 
        * method
        * @param aParams - the destination which holds the 
        *                             connection method, NULL means there is
        *                             no parent
        * @return CCmPluginBaseEng*
        */
        CCmPluginBaseEng* DoCreateConnectionMethodL( TUint32 aBearerType,
                                       TCmPluginInitParam& aParams );

        /**
        * Creates a connection method does not belong to any destination
        *
        * @since 3.2
        * @param aBearerType Implementation uid of the connection 
        * method
        * @param aParams Initialization params for the plugin.
        * @param aConnMethodId Predefined id for the connection method.
        * @return CCmPluginBaseEng*
        */
        CCmPluginBaseEng* DoCreateConnectionMethodL( TUint32 aBearerType,
            TCmPluginInitParam& aParams, TUint32 aConnMethodId );

    //=========================================================================
    // Getter-Setter API
    //
    public:    
            
        /**
        * Calls FeatureManager::FeatureSupported directly.
        *
        * @since 3.2
        * @param aFeature feature ID
        * @return feature support status
        * @since 3.2
        */
        static TBool FeatureSupported( TInt aFeature );
        
        /**
        * Obsolete
        */
        IMPORT_C CCmPluginBaseEng* ConnectionMethodL( TUint32 aCmId );

        /**
        * Return the connection method queried by it's ID
        *
        * @since 3.2
        * @param aCmId
        * @return CCmPluginBase*
        */
        IMPORT_C CCmPluginBase* GetConnectionMethodL( TUint32 aCmId );
        
        /**
        * Return the list of connection methods do not belongs to any 
        * destination
        *
        * @since 3.2
        * @param aCMArray on return it is the array of IAP ids.
        * @param aCheckBearerType If ETrue only connection methods with 
        *                         supported bearer types are returned
        * @param aLegacyOnly when set to ETrue only Legacy IAP ids will
        *                         be returned
        * @param aEasyWlan when set to ETrue EasyWlan id is returned, too
        * @param aSortByBearer if ETrue the CMs will be ordered according
        *                      to global bearer priority
        */        
        IMPORT_C void ConnectionMethodL( RArray<TUint32>& aCMArray,
                                         TBool aCheckBearerType = ETrue,
                                         TBool aLegacyOnly = ETrue,
                                         TBool aEasyWlan = EFalse,
                                         TBool aSortByBearer = EFalse );
        /**
        * Return an array of all destinations
        *
        * @since 3.2
        * @param aDestArray on return it returns an array of destination ids
        */
        IMPORT_C void AllDestinationsL( RArray<TUint32>& aDestArray );

        /**
        * Return the number of destinations
        *
        * @since 3.2
        * @return the number of destinations
        */
        inline TUint DestinationCountL() { return iDestinationCount; }

        /**
        * Return the destination implementation to which a specified 
        * connection method belongs, search is by destination ID ( the
        * id which is made by RECORD_FROM_ELEMENT_ID from the elementID)
        * If the  destination implementation is allready in the  object
        * pool returns it. If it is not in the pool than puts it in the
        * pool using the GetDestinationL funciton.
        *
        * @since 3.2
        * @param aId
        * @return a pointer to a destination
        */
        IMPORT_C CCmDestinationImpl* DestinationL( TUint32 aId );
        
        /**
        * Removes the destination implementation 
        * from the object pool
        *
        * @since 3.2
        * @param aId
        */
        IMPORT_C void RemoveDestFromPool( CCmDestinationImpl* aDestination );
        
        /**
        * Removes the destination implementation 
        * from the object pool
        *
        * @since 3.2
        * @param aId
        */
        void RemoveDestFromPool( CCmDestinationData* aDestination );
        
        /**
        * Removes the cm 
        * from the object pool
        *
        * @since 3.2
        */
        void RemoveCMFromPoolL( CCmPluginBaseEng* aCM );
        
        /**
        * Removes the cm 
        * from the object pool
        *
        * @since 3.2
        */
        void RemoveCMFromPoolL( CCmPluginBase* aCM );

        /**
        * The destination have been updated if it was a new destination
        * than the zero id must be changed to the new one 
        *
        * @param aDestination - the destination to change
        */         
        void DestinationUpdated( CCmDestinationImpl* aDestination );
              
        /**
        * Copie a connection method belonging to one destination to 
        * another. The connection method becomes shared.
        *
        * @since 3.2
        * @param aTargetDestination target destination
        * @param aConnectionMethodId connection method to be copied
        * @return index in the Connection Method list
        */
        TInt CopyConnectionMethodL( CCmDestinationImpl& aTargetDestination,
                                    CCmPluginBase& aConnectionMethod );
        
        /**
        * Move a connection method belonging to one destination to 
        * another. The connection method is removed from the source destination
        *
        * @since 3.2
        * @param aSourceDestination source destination
        * @param aTargetDestination target destination
        * @param aConnectionMethod connection method to be moved
        * @return index in the Connection Method list which the connection 
        *         method has been moved to
        */
        TInt MoveConnectionMethodL( CCmDestinationImpl& aSourceDestination,
                                    CCmDestinationImpl& aTargetDestination,
                                    CCmPluginBase& aConnectionMethod );
        
        /**
        * Remove connection method from the destination
        *
        * @since 3.2
        * @param aDestination destination the connection method is attached to
        * @param aConnectionMethod connection method to be removed
        */
        void RemoveConnectionMethodL( CCmDestinationImpl& aDestination,
                                      CCmPluginBase& aConnectionMethod );

        /**
        * Return the copy of the bearer priority array. Ownership is passed.
        *
        * @since 3.2
        * @param aArray bearer priority array
        */          
        void BearerPriorityArrayL( RArray<TBearerPriority>& aArray ) const;
        
        /*
        * Update bearer priority array
        *
        * @since 3.2
        * @param aArray array with new global bearer priority
        */          
        void UpdateBearerPriorityArrayL( const RArray<TBearerPriority>& aArray );
        
        /**
        * Clean up passed global bearer priority array. Delete
        * the elements and calls Reset and Close on the array.
        *
        * @since 3.2
        * @param aArray array to be cleaned up
        */
        void CleanupGlobalPriorityArray( RArray<TBearerPriority>& aArray ) const;
        
        /**
        * Search service type in the global priority table and
        * Return its priority.
        *
        * @since 3.2
        * @param aUiPriority ETrue if UI priority is requested
        * @param aServiceType serched service type 
        * @return priority of the service type or KErrNotFound
        */
        TInt GlobalBearerPriority( TBool aUiPriority, 
                                   const TDesC& aServiceType ) const;
                                   
        /*
        * Query all of the supported bearer types.
        *
        * @since 3.2
        * @param aArray list of all supported bearer types.
        */          
        void SupportedBearersL( RArray<TUint32>& aArray ) const;
        
        /*
        * Return "Uncategorized" icon.
        * This fucton leaves if the client does not have a valid UI context
        *
        * @since 3.2
        * @return CGulIcon icon of uncategorized connection methods.
        */          
        CGulIcon* UncategorizedIconL() const;

        /**
        * Returns the id of the EasyWlan connection method.
        *
        * @since S60 3.2
        * @return id of the EasyWLan connection method. 0 if not found
        */
        IMPORT_C TUint32 EasyWlanIdL();

        /**
        * Create a SNAP record object and load it if record id not 0.
        * @param aRecordId record Id. 0 means create only
        * @return created/loaded SNAP record
        */
        CCDSnapRecord* SNAPRecordL( TUint32 aRecordId ) const;

        CCDSnapRecord* CreateSNAPRecordL( TUint32 aRecordId ) const;
    
        /**
        * Tells the CmManager that the given default connection is deleted,
        * and it sets Default Connection to Always Ask (default value).
        *
        * @since S60 3.2
        */
        void HandleDefConnDeletedL( TCmDefConnValue& aDCSetting );
        /**
        * Returns the default connection method/SNAP.
        *
        * @since S60 3.2
        */
        void ReadDefConnL( TCmDefConnValue& aDCSetting );   
        /**
        * Stores the default connection method/SNAP.
        *
        * @since S60 3.2
        */
        void WriteDefConnL( const TCmDefConnValue& aDCSetting );  
        /**
        * Returns the default connection method/SNAP.
        *
        * @since S60 3.2
        */
        void ReadDefConnWoTransL( TCmDefConnValue& aDCSetting );   
        /**
        * Stores the default connection method/SNAP.
        *
        * @since S60 3.2
        */
        void WriteDefConnWoTransL( const TCmDefConnValue& aDCSetting );  
        /**
        * Returs ETrue if Default Connection feature is supported.
        *
        * @since S60 3.2
        */        
        TBool IsDefConnSupported();
        /**
        * Check if memory low so that process can continue
        */
        TBool IsMemoryLow(); 
        /**
        * Returns the general connection settings.
        */
        void ReadGenConnSettingsL( TCmGenConnSettings& aGenConnSettings );
        /**
        * Replaces the general connection settings.
        */
        void ReplaceGenConnSettingsL( const TCmGenConnSettings& aGenConnSettings );
        /**
        * Writes the general connection settings.
        */
        void WriteGenConnSettingsL( const TCmGenConnSettings& aGenConnSettings );
        
                
    //=========================================================================
    // Resource handling
    //
    public:
        
        /** add resource file */
        void AddResourceFileL( TFileName& aName );
        
        /** 
        * Remove resource file from array only if
        * the reference count is zero.
        * @param aName name of the resource file to be removed
        */
        void RemoveResourceFile( TFileName& aName );

        /**
        * Read string from resource
        *
        * @since 3.2
        * @param aResourceId resource id of the localized string
        * @return localized string
        */
        HBufC* AllocReadL( TInt aResourceId ) const;    
    
    //=========================================================================
    // CM related functions
    //
    public:
        
        /**
        * Return the number of references to the given connection method id
        * in the DN_IAP table
        *
        * @since 3.2
        * @param aId ID of the connection method        
        * @param aRecSet entire record set of the detination table
        * @return number of references to the connection method in DN_IAP table
        */
        TUint32 NumOfConnMethodReferencesL( 
            TUint32 aId,
            CommsDat::CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>* aRecSet = NULL 
            );
        
        /**
        * Indicates is at least one non-"level 1 protected" destinations
        *
        * @since S60 3.2
        * @return ETrue if at least one level 1 protected destination
        */
        TBool HasUnprotectedDestinationsL();
        
        /**
        * Walks through every connection methods to find out
        * if the passed connection method is linked to any of them.
        * @param aPlugin connection method to be checked
        * @return ETrue if linked
        */
        TBool IsIapLinkedL( const CCmPluginBaseEng& aPlugin );
        
        /**
        * Removes all references to this CM from SNAP table.
        */
        void RemoveAllReferencesL( const CCmPluginBaseEng& aPlugin );

        /**
        * Removes all references to this CM from SNAP table without
        * transaction to commsdat.
        */
        void RemoveAllReferencesWoTransL( const CCmPluginBaseEng& aPlugin );

        /**
        * Return the bearer type of connection method.
        *
        * @since 3.2
        * @param aId id of the connection method
        * @return bearer type of the connection method
        */
        TUint32 BearerTypeFromCmIdL( TUint32 aCmId );
                
        /**
        * Return the bearer type of connection method.
        *
        * @since 3.2
        * @param aIapRecord CommsDat record of the connection method
        * @return bearer type of the connection method
        */
        TUint32 BearerTypeFromIapRecordL( CommsDat::CCDIAPRecord* aIapRecord ) const;

    private:
    
        /**
        * Find and create a connection method.
        *
        * @since 3.2
        * @param aCmId id of the connection method
        * @aParentDest parent destination of the connection method. Can be NULL.
        */
        CCmPluginBase* DoFindConnMethodL( TUint32 aCmId, 
                              CCmDestinationImpl* aParentDest );
    
        /**
        * Find and create a connection method.
        *
        * @since 3.2
        * @param aCmId id of the connection method
        * @aParentDest parent destination of the connection method. Can be NULL.
        */
        CCmPluginBaseEng* DoFindConnMethL( TUint32 aCmId, 
                               CCmDestinationImpl* aParentDest );

        /**
        * Check the existence of tables we need.
        * Creates if it is not exist, get the table id otherwise
        */
        void CheckTablesL();

    public:
    
        /**
        * Return the destination network table ID
        *
        * @since 3.2
        * @return iDestinationTableId element id of the destination table
        */        
        inline CommsDat::TMDBElementId DestinationTableId() const { return iDestinationTableId; };
        
        /**
        * Return Global Bearer Type Priorization Table ID
        *
        * @since 3.2
        * @return element id of the bearer type priority.
        */        
        inline CommsDat::TMDBElementId BearerTypePriorizationTableId() const { return iBearerTypePriorizationTableId; };
        
        /**
        * Return the Destination network - Icon table ID
        *
        * @since 3.2
        * @return element id of the icon table.
        */        
        inline CommsDat::TMDBElementId IconTableId() const { return iIconTableId; };
        
        /**
        * Return true if help is on in the FeatureManager
        *
        * @since 3.2
        * @return TBool.
        */        
        inline TBool IsHelpOn() const { return iHelp; };

        /**
        * Return the Seamlessness table ID
        *
        * @since 3.2
        * @return element id of the seamlessness table.
        */        
        inline CommsDat::TMDBElementId SeamlessnessTableId() const { return iSeamlessnessTableId; };
        
        /**
        * Checks if the current environment has the given capability.
        * Leaves with KErrPermissionDenied if capability is missing. 
        *
        * @since 3.2
        * @param aCapability capability to be checked
        */
        static void HasCapabilityL( TCapability aCapability );
        
        /**
        * Return an array of all IAP records.
        * @return array of all IAP records
        */
        CommsDat::CMDBRecordSet<CommsDat::CCDIAPRecord>* AllIapsL();
        
        /**
        * Return all the SNAP record. Filtering can be made on 
        * passed SNAP elemented id.
        * @return array of SNAP records
        */
        CommsDat::CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>* 
                    CCmManagerImpl::AllSNAPRecordsL( TUint32 aSnapElementId );
        
    public:
    
        CommsDat::CMDBSession& Session() const;

        /**
        * Wraps this implementation to an RCmManagerExt. OWNERSHIP IS NOT PASSED
        * , SO NEVER CLOSE aCmManagerExt!!!
        *
        * @since 3.2
        * @param aCmManagerExt class to wrap implementation into
        */
        
        IMPORT_C void WrapCmManager( RCmManagerExt& aCmManagerExt );
     
        /** Remove the virtual methods from the passed list
        *
        * @since 3.2
        * @param aCmDataArray array that contains the connection methods to be filtered
        */
        IMPORT_C void FilterOutVirtualsL( RPointerArray<CCmPluginBase>& aCmDataArray );

        /** Create a 'flat list' of connection methods of a given destination
        * ( connection methods inside embedded destinations which are in the destination,
        * are also present in the list ) and
        * order them according to the bearer type prioritisation rules
        * and return the result in aCmArray.        
        *
        * @since 3.2
        * @param aDestination destination that contains the connection methods
        * for uncategorised connection methods, use KDestItemUncategorized
        * @param aCmArray array to return the result
        */

        IMPORT_C void CreateFlatCMListL( TUint32 aDestination ,
                                        RPointerArray<CCmPluginBase>& aCmArray );        
                                        
        /** Create a 'flat list' of connection methods of a given destination
        * ( connection methods inside embedded destinations which are in the destination,
        * are also present in the list ) and
        * order them according to the bearer type prioritisation rules
        * and return the result in aCmArray.        
        * LC version        
        *
        * @since 3.2
        * @param aDestination destination that contains the connection methods
        * for uncategorised connection methods, use KDestItemUncategorized
        * @param aCmArray array to return the result
        */
        IMPORT_C void CreateFlatCMListLC( TUint32 aDestination ,
                                        RPointerArray<CCmPluginBase>& aCmArray );        
      
        
        /** Get a list of uncategorised connection methods in an RPointerArray 
        * contains the parameters passed to ConnectionMethodL
        *
        * @since 3.2
        * @param aCmArray the returned list of uncategorised connection methods
        * @param aCheckBearerType see ConnectionMethodL
        * @param aLegacyOnly see ConnectionMethodL
        * @param aEasyWlan see ConnectionMethodL
        * @param aSortByBearer see ConnectionMethodL
        */
        void AppendUncatCmListL(RPointerArray<CCmPluginBase>& aCmArray,
                                         TBool aCheckBearerType = ETrue,
                                         TBool aLegacyOnly = ETrue,
                                         TBool aEasyWlan = EFalse,
                                         TBool aSortByBearer = EFalse );
        
        /** Get a list of uncategorised connection methods in an RPointerArray 
        * contains the parameters passed to ConnectionMethodL
        * LC version
        *
        * @since 3.2
        * @param aCmArray the returned list of uncategorised connection methods
        * @param aCheckBearerType see ConnectionMethodL
        * @param aLegacyOnly see ConnectionMethodL
        * @param aEasyWlan see ConnectionMethodL
        * @param aSortByBearer see ConnectionMethodL
        */
        void AppendUncatCmListLC(RPointerArray<CCmPluginBase>& aCmArray,
                                         TBool aCheckBearerType = ETrue,
                                         TBool aLegacyOnly = ETrue,
                                         TBool aEasyWlan = EFalse,
                                         TBool aSortByBearer = EFalse );
        
        /**
        * Returns a list of references to the connection methods in the destination. 
        *
        * @since 3.2
        * @param aPluginArray A pointer array to hold the values
        * ownership is passed
        */
        void AppendCmListL( TUint32 aDestintaionId, RPointerArray<CCmPluginBase>& aCmArray);
        
        /**
        * Returns a list of references to the connection methods in the destination. 
        *
        * @since 3.2
        * @param aPluginArray A pointer array to hold the values
        * ownership is passed
        */
        // and puts it on the CleanupStack
        void AppendCmListLC( TUint32 aDestintaionId, RPointerArray<CCmPluginBase>& aCmArray);

        /**
        * Returns a list of references to the connection methods in the supported uid list . 
        *
        * @since 3.2
        * @param aPluginArray A pointer array to hold the values
        * ownership is passed
        */
        void AppendCmListFromUidsL( RArray<TUint32>& cmIdList, RPointerArray<CCmPluginBase>& aCMArray );        

        /**
        * Returns internet destination id. If internety destination is not found then -1 is returned.
        *
        * @since 5.0
        * @return Internet destination id
        */
        TUint32 GetInternetDestinationIdL();
        
        /**
        * Register aWatcher as watcher for change of CommsDat
        */
        IMPORT_C void WatcherRegisterL( MCmCommsDatWatcher* aWatcher );
        
        /**
         * UnRegister current watcher
         */
        IMPORT_C void WatcherUnRegister();
        
        /*
         * Find if aDest is still existed if some change is watched
         */
        IMPORT_C TBool DestinationStillExistedL( CCmDestinationImpl* aDest );
        
        /*
         * Find if an Iap is still existed if some change is watched
         * 
         * The Iap, as parameter, is contained in the instance aPlugin
         */        
        IMPORT_C TBool IapStillExistedL( const CCmPluginBaseEng& aPlugin );
        
        /*
         * Find if an Iap is still in aDest if some change is watched
         * 
         * The Iap, as parameter, is contained in the instance aPlugin
         */
        IMPORT_C TBool IsIapStillInDestL( CCmDestinationImpl* aDest, const CCmPluginBaseEng& aPlugin );

    private:
            
        /**
        * Creates bearer priority array
        *
        * @since 3.2
        */          
        void CreateBearerPriorityArrayL();   
        
        /**
        * Look for the given resource file in the resource
        * file array and return the index value of it.
        *
        * @since 3.2
        * @param aName resource file name, not localized
        * @return index of the resource file in the array
        */
        TInt FindResourceFile( TFileName& aName ) const;
        
        /**
        * Build an array of all bearer specific plugins.
        *
        * @since 3.2
        */
        void BuildPluginArrayL();
    
        /**
        * Return the copy of the bearer priority array. Ownership is passed.
        *
        * @since 3.2
        * @param aArray bearer priority array
        */          
        void CopyBearerPriorityArrayL( RArray<TBearerPriority>& aArray ) const;
        
        /**
        * Return if the IAP record points to an EasyWLan service record.
        *
        * @since 3.2
        * @param aIapRecord IAP record to be checked
        * @return ETrue if AP is EasyWLan
        */
        TBool IsEasyWLanL( CommsDat::CCDIAPRecord* aIapRecord ) const;
        
        /**
        * Replaces the Default Connection record with the new one.
        *
        * @since 3.2
        * @param aDCSetting Default Connection value to set
        */
        void ReplaceDefConnRecordL( const TCmDefConnValue aDCSetting );
        
        /**
        * Combines two lists of CCmPluginBase pointers into one, according to default
        * priority order
        *
        * @since 3.2
        * @param aCmArray first list of connection methods to combine
        * @param aCmLinkedArray second list of connection methods to combine
        */
        void CombineArraysForPriorityOrderL( 
                                        RPointerArray<CCmPluginBase>& aCmArray , 
                                        RPointerArray<CCmPluginBase>& aCmLinkedArray );        


        /**
        * Gets the destination for the given element ID
        *
        * @since 3.2
        * @param aCmId The element ID of the destination
        * @return Id of the parent
        */
        TUint32 ParentDestinationL( TUint32 aCmId );
                
        /**
        * Returns the default general connection values.
        *
        * @since S^3
        * @param aGenConnSettings The general connection settings structure
        */
        void SetGenConnSettingsToDefault( TCmGenConnSettings& aGenConnSettings );
        
        /** 
         * Leaves if no sufficient is available for creation operations
         * 
         * @since S^3
         */
        void CheckAvailableSpaceL();
        
	    /**
         * Get Snap Table ID from CommsDat
         */
        TUint32 GetSnapTableIdL();

        /**
         * Check if the attribute is bearerspecific
         *
         * @since S^3
         * @param aAttribute  attribute-id to be checked
         * @return ETrue if aAttribute is bearerspecific, otherwise EFalse
         */
        TBool IsBearerSpecific( TUint32 aAttribute ) const;
        
        /**
        * Returns CenRep key KCellularDataUsageSettingAutomaticInHomeNetwork value.
        *
        * @since S^3
        * @return CenRep key KCellularDataUsageSettingAutomaticInHomeNetwork value
        */
        TInt HomeOperatorSetting() const;
        
#include "cmmanagerimpl.inl"               
    //=========================================================================
    // Member data
        
    private:
    
        CCmTransactionHandler*          iTrans;
        
        RFs                             iFs;
        CArrayPtrFlat<TResourceFiles>*  iResourceFiles;
        
        // Destination network table ID
        CommsDat::TMDBElementId iDestinationTableId;
        //Global Bearer Type Priorization Table
        CommsDat::TMDBElementId iBearerTypePriorizationTableId;
        // Destination network - Icon table
        CommsDat::TMDBElementId iIconTableId;
        // Destination network - Icon table
        CommsDat::TMDBElementId iSeamlessnessTableId;        
        // Default Connection table
        CommsDat::TMDBElementId iDefConnTableId;        
        // Bearer priorities
        RArray<TBearerPriority>* iBearerPriorityArray;
        
        CArrayPtrFlat<const CCmPluginBaseEng>* iPlugins;
        
        CCmConnectionMethodInfo* iPluginImpl;
        
        TUint iDestinationCount;
        
        CCMObjectPool* iObjectPool;
              
        // for the FeatureManager
        TBool iIsFeatureManagerInitialised;
        TBool iHelp;
        TBool iDefConnSupported;
        TBool iCreateTables;
        TBool iWLan;
               
        // For CommsDat watcher
        CCmCommsDatNotifier* iCommsDatIapNotifier;
        CCmCommsDatNotifier* iCommsDatSnapNotifier;
    private:
    
        friend class CCmDestinationImpl;
        friend class CCMObjectPool;
        friend class CCmConnectionMethodInfo;
    };

#endif // CMMANAGERIMPL_H
