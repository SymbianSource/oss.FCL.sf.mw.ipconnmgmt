/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of class CApSettingsModel.
*
*/


#ifndef APSETTINGS_MODEL_H
#define APSETTINGS_MODEL_H

// INCLUDE FILES
#include <ApDataHandler.h>
#include <ApUtils.h>
#include <ApSelect.h>

#include <ApSettingsHandlerCommons.h>

#include <ApProtHandler.h> //for protection of settings


// FORWARD DECLARATION
class CActiveApDb;
class CApListItem;
class CApListItemList;
class CApAccessPointItem;
class CApDataHandler;
class CApSelect;
class CAknWaitDialog;
class CApSettingsDeleteOperation;
class CApNetworkItemList;
class CFeatureManagerWrapper;
class CVpnApEngine;
class CEikonEnv;

class CWEPSecuritySettings;

class CWPASecuritySettings;



/**
* A structure to hold filtering parameters
* First, it is possible to specify which ISP-type we are looking for. 
* The possible values are:
* <br>
* KEApIspTypeInternetOnly:  Filter allows only access points that are 
*                           capable of only internet access.
* <br>
* KEApIspTypeWAPOnly:       Filter allows only access points that are 
*                           capable of only WAP access.
* <br>
* KEApIspTypeInternetAndWAP:Filter allows only access points that are 
*                           capable of internet AND WAP access.
* <br>
* KEApIspTypeWAPMandatory:  Filter allows only access points that are 
*                           capable of WAP access AND contain mandatory
*                           WAP settings.
* <br>
* KEApIspTypeAll:           No filtering is done for the isp-type, 
*                           all types of access points are shown.
* <br>
* It is possible to combine them by simply adding/or-ing together the values,
* filtering will be done in such a way that only access points with the 
* listed ISP-types will be shown.
* Second filtering possibility (aBearerFilter) is the desired bearer type.
* The possible values are the values of the TApBearerType enum type and their 
* combinations, as in case of the ISP type.
* The ordering of the list can be specified with the aSortType parameter. 
* It can have the following values:
* KEApSortUidAscending:     The sorting is done according to the ID 
*                           of the access points in ascending order.
* KEApSortUidDescending:    The sorting is done according to the ID 
*                           of the access points in descending order.
* KEApSortNameAscending:    The sorting is done according to the name 
*                           of the access points in ascending order.
* KEApSortNameDescending:   The sorting is done according to the name 
*                           of the access points in descending order.
*/
typedef struct
    {
        TInt                iIspFilter;
        TInt                iBearerFilter;
        TInt                iSortType;
    }TApSettingsParams;







// CLASS DECLARATION

/**
* Model for the access point settings ui.
*/
NONSHARABLE_CLASS( CApSettingsModel ) : public CBase
    {
    public:     // Construct / destruct

        /**
        * Two-phased constructor. Leaves on failure.
        * The following three parameters specify the filtering and ordering 
        * criteria for the selection list creation. First, it is possible to 
        * specify which ISP-type we are looking for. The possible values are:
        * <br>
        * KEApIspTypeInternetOnly:  Filter allows only access points that are
        *                           capable of only internet access.
        * <br>
        * KEApIspTypeWAPOnly:       Filter allows only access points that are 
        *                           capable of only WAP access.
        * <br>
        * KEApIspTypeInternetAndWAP:Filter allows only access points that are
        *                           capable of internet AND WAP access.
        * <br>
        * KEApIspTypeWAPMandatory:  Filter allows only access points that are 
        *                           capable of WAP access AND contain 
        *                           mandatory WAP settings.
        * <br>
        * KEApIspTypeAll:           No filtering is done for the isp-type, all
        *                           types of access points are shown.
        * <br>
        * It is possible to combine them by simply adding/or-ing together the 
        * values, filtering will be done in such a way that only access points 
        * with the listed ISP-types will be shown.
        * Second filtering possibility (aBearerFilter) is the desired bearer 
        * type. The possible values are the values of the TApBearerType enum 
        * type and their combinations, as in case of the ISP type.
        * The ordering of the list can be specified with the aSortType 
        * parameter. It can have the following values:
        * KEApSortUidAscending:     The sorting is done according to the ID 
        *                           of the access points in ascending order.
        * KEApSortUidDescending:    The sorting is done according to the ID 
        *                           of the access points in descending order.
        * KEApSortNameAscending:    The sorting is done according to the name 
        *                           of the access points in ascending order.
        * KEApSortNameDescending:   The sorting is done according to the name 
        *                           of the access points in descending order.
        * Caution! The ordering values can not be combined!
        * @param aIspTypeFilter Filtering criteria on ISP type
        * @param aBearerFilter Filtering criteria on bearer type
        * @param aSortType Specifies the sort order to use.
        * @param aReqIpvType Specifies whether the caller would like to 
        *   have IPv4, IPv6 or both access points. If it is IPv6 or Both, 
        *   in case of IPv6 support is available as a feature, it will be 
        *   supported. If IPv6 feature is not supported by the phone, it 
        *   simply defaults to the normal IPv4 version.
        *   If it is IPv4, it uses the default IPv4 version independently 
        *   from IPv6 feature beeing available or not.
        * @param aVpnFilterType a TVpnFilterType representing the possible 
        *   additional VPN filtering.
        * @param aVariant The local variant value
        * @param aIncludeEasyWlan a TBool indicating whether to include the
        *   EasyWlan AP in the list or not
        * @param aNoEdit Gives wether editing is allowed or not
        * @return The constructed listbox.
        */
        static CApSettingsModel* NewL(
                                      TInt aIspFilter,
                                      TInt aBearerFilter,
                                      TInt aSortType,
                                      TInt aReqIpvType,
                                      TVpnFilterType aVpnFilterType,
                                      TInt aVariant,
                                      TBool aIncludeEasyWlan,
                                      TBool aNoEdit
                                     );


        /**
        * Two-phased constructor. Leaves on failure.
        * The following three parameters specify the filtering and ordering 
        * criteria for the selection list creation. First, it is possible to 
        * specify which ISP-type we are looking for. The possible values are:
        * <br>
        * KEApIspTypeInternetOnly:  Filter allows only access points that are
        *                           capable of only internet access.
        * <br>
        * KEApIspTypeWAPOnly:       Filter allows only access points that are 
        *                           capable of only WAP access.
        * <br>
        * KEApIspTypeInternetAndWAP:Filter allows only access points that are
        *                           capable of internet AND WAP access.
        * <br>
        * KEApIspTypeWAPMandatory:  Filter allows only access points that are 
        *                           capable of WAP access AND contain 
        *                           mandatory WAP settings.
        * <br>
        * KEApIspTypeAll:           No filtering is done for the isp-type, all
        *                           types of access points are shown.
        * <br>
        * It is possible to combine them by simply adding/or-ing together the 
        * values, filtering will be done in such a way that only access points 
        * with the listed ISP-types will be shown.
        * Second filtering possibility (aBearerFilter) is the desired bearer 
        * type. The possible values are the values of the TApBearerType enum 
        * type and their combinations, as in case of the ISP type.
        * The ordering of the list can be specified with the aSortType 
        * parameter. It can have the following values:
        * KEApSortUidAscending:     The sorting is done according to the ID 
        *                           of the access points in ascending order.
        * KEApSortUidDescending:    The sorting is done according to the ID 
        *                           of the access points in descending order.
        * KEApSortNameAscending:    The sorting is done according to the name 
        *                           of the access points in ascending order.
        * KEApSortNameDescending:   The sorting is done according to the name 
        *                           of the access points in descending order.
        * Caution! The ordering values can not be combined!
        * @param aDb The database to work on
        * @param aIspTypeFilter Filtering criteria on ISP type
        * @param aBearerFilter Filtering criteria on bearer type
        * @param aSortType Specifies the sort order to use.
        * @param aReqIpvType Specifies whether the caller would like to 
        *   have IPv4, IPv6 or both access points. If it is IPv6 or Both, 
        *   in case of IPv6 support is available as a feature, it will be 
        *   supported. If IPv6 feature is not supported by the phone, it 
        *   simply defaults to the normal IPv4 version.
        *   If it is IPv4, it uses the default IPv4 version independently 
        *   from IPv6 feature beeing available or not.
        * @param aVpnFilterType a TVpnFilterType representing the possible 
        *   additional VPN filtering.
        * @param aVariant The local variant value
        * @param aIncludeEasyWlan a TBool indicating whether to include the
        *   EasyWlan AP in the list or not
        * @param aNoEdit Gives wether editing is allowed or not        
        * @return The constructed listbox model.
        */
        static CApSettingsModel* NewL(
                                      CActiveApDb& aDb,
                                      TInt aIspFilter,
                                      TInt aBearerFilter,
                                      TInt aSortType,
                                      TInt aReqIpvType,
                                      TVpnFilterType aVpnFilterType,
                                      TInt aVariant,
                                      TBool aIncludeEasyWlan,
                                      TBool aNoEdit
                                     );

        /**
        * Destructor.
        */
        virtual ~CApSettingsModel();


    protected:
        /**
        * Constructor.
        * @param aIspTypeFilter Filtering criteria on ISP type
        * @param aBearerFilter Filtering criteria on bearer type
        * @param aSortType Specifies the sort order to use.
        * @param aReqIpvType Specifies whether the caller would like to 
        *   have IPv4, IPv6 or both access points. If it is IPv6 or Both, 
        *   in case of IPv6 support is available as a feature, it will be 
        *   supported. If IPv6 feature is not supported by the phone, it 
        *   simply defaults to the normal IPv4 version.
        *   If it is IPv4, it uses the default IPv4 version independently 
        *   from IPv6 feature beeing available or not.
        * @param aVariant   The variant
        * @param aVpnFilterType a TVpnFilterType representing the possible 
        *   additional VPN filtering.
        * @param aVariant The local variant value
        * @param aIncludeEasyWlan a TBool indicating whether to include the
        *   EasyWlan AP in the list or not
        * @param aNoEdit Gives wether editing is allowed or not
        */
        CApSettingsModel(
                        TInt aIspFilter,
                        TInt aBearerFilter,
                        TInt aSortType,
                        TInt aReqIpvType,
                        TInt aVariant,
                        TVpnFilterType aVpnFilterType,
                        TBool aIncludeEasyWlan,
                        TBool aNoEdit
                        );


        /**
        * Second phase constructor. Leaves on failure.
        * @param aDb Database to use if any, NULL otherwise.
        */
        virtual void ConstructL( CActiveApDb* aDb );


    public:
        /**
        * Returns the database object
        */
        CActiveApDb*     Database();


        /**
        * Gets the APs corresponding to the filtering criteria,
        * already ordered.
        * @param aFailedLocked a boolean indicating if the operation
        * had failed because the DB was locked
        * @param aList a reference to the list of APs.
        * The passed list will be erased, elements destroyed,
        * new elements created in it. The list owns the created elements
        * and the caller maintaines ownership of the list.
        * @param aIspFilter The isp filter to be used
        * @param aBearerFilter The bearer filter to be used
        * @param aSortType The ordering to be used.
        * @param aReqIpvType Specifies whether the caller would like to 
        *   have IPv4, IPv6 or both access points. If it is IPv6 or Both, 
        *   in case of IPv6 support is available as a feature, it will be 
        *   supported. If IPv6 feature is not supported by the phone, it 
        *   simply defaults to the normal IPv4 version.
        *   If it is IPv4, it uses the default IPv4 version independently 
        *   from IPv6 feature beeing available or not.
        * @return the number of items in the list.
        */
        TInt AllListItemDataL
                (
                TBool& aFailedLocked,
                CApListItemList& aList,
                TInt aIspFilter,
                TInt aBearerFilter,
                TInt aSortType,
                TInt aReqIpvType
                );

                
        /**
        * Gets the APs corresponding to the filtering criteria,
        * already ordered.
        * @param aFailedLocked a boolean indicating if the operation
        * had failed because the DB was locked
        * @param aList a reference to the list of APs.
        * The passed list will be erased, elements destroyed,
        * new elements created in it. The list owns the created elements
        * and the caller maintaines ownership of the list.
        * @param aIspFilter The isp filter to be used
        * @param aBearerFilter The bearer filter to be used
        * @param aSortType The ordering to be used.
        * @param aReqIpvType Specifies whether the caller would like to 
        *   have IPv4, IPv6 or both access points. If it is IPv6 or Both, 
        *   in case of IPv6 support is available as a feature, it will be 
        *   supported. If IPv6 feature is not supported by the phone, it 
        *   simply defaults to the normal IPv4 version.
        *   If it is IPv4, it uses the default IPv4 version independently 
        *   from IPv6 feature beeing available or not.        
        * @param aVpnFilterType
        * @param aIncludeEasyWlan a TBool indicating whether to include the
        *   EasyWlan AP in the list or not        
        * @return the number of items in the list.
        */
        TInt AllListItemDataL
                (
                TBool& aFailedLocked,
                CApListItemList& aList,
                TInt aIspFilter,
                TInt aBearerFilter,
                TInt aSortType,
                TInt aReqIpvType,
                TVpnFilterType aVpnFilterType,
                TBool aIncludeEasyWlan
                );


        /**
        * Gets the APs corresponding to the filtering criteria,
        * already ordered.
        * @param aFailedLocked a boolean indicating if the operation
        * had failed because the DB was locked
        * @param aList a reference to the list of APs.
        * The passed list will be erased, elements destroyed,
        * new elements created in it. The list owns the created elements
        * and the caller maintaines ownership of the list.
        * @return the number of items in the list.
        */
        TInt AllListItemDataL( TBool& aFailedLocked, CApListItemList& aList );


        /**
        * Reads up one access point data
        * @param aUid The UID of the access point to read
        * @param aApItem A reference to a CApAccessPointItem to hold the
        * access point item readed from the database
        */
        void AccessPointDataL( TUint32 aUid, CApAccessPointItem& aApItem );


        /**
        * Updates one access point item in the database
        * @param aApItem A reference to a CApAccessPointItem holding the
        * access point item to be updated in the database
        */
        TBool UpdateAccessPointDataL( CApAccessPointItem& aApItem );


        /**
        * Returns a pointer to the CApDataHandler object
        */
        CApDataHandler* DataHandler();


        /**
        * Returns a pointer to the CApUtils object
        */
        CApUtils*   ApUtils();


        /**
        * Sets the filtering parameters for the model
        * @param aParams The filtering parameters to be used
        */
        void SetParams( TApSettingsParams aParams );


        /**
        * Gets the filtering parameters of the model
        * @return The used filtering parameters
        */
        TApSettingsParams Params();


        /**
        * Returns a pointer to the VPN engine object
        */
        CVpnApEngine* VpnEngine();


        /**
        * Launches help
        */
        void LaunchHelpL();

        /**
        * Deletes an access point specified by its UID
        * @param aUid The UID of the access point to delete.
        */
        void RemoveApL( TUint32 aUid );


        /**
        * Starts a transaction
        * @param aWrite A boolean indicating whether read or write lock is
        * needed.
        * @param aShowNote A boolean indicating whether to show
        * 'Cannot access database' note or not if DB is locked
        * @param aRetry A boolean indicating whether to retry or not
        * if DB is locked
        * @param aLFFSChecking Whether low disk space is checked or not
        * @return TTransactionResult containing whther it is our
        * own transaction or not or it has failed with KErrLocked
        */
        TTransactionResult StartTransactionLC
                                (
                                TBool aWrite,
                                TBool aShowNote = ETrue,
                                TBool aRetry = ETrue,
                                TBool aLFFSChecking = ETrue
                                );

        /**
        * Gets write lock on the database
        */
        void  GetWriteLockL();


        /**
        * Cleanup function for the transaction.
        * In case of leave, it will be called and
        * transaction will be rolled back
        */
        static void RollbackTransactionOnLeave( TAny* aDb );


        /**
        * Commits the transaction on the database
        */
        TInt CommitTransaction();


        /**
        * Check if Disk space goes below critical level if allocating
        * more disk space.
        * @param aShowErrorNote If ETrue, a warning note is shown.
        * @param aBytesToWrite Amount of disk space to be allocated (use an
        * estimate if exact amount is not known).
        * @return ETrue if allocation would go below critical level.
        */
        TBool FFSSpaceBelowCriticalLevelL( TBool aShowErrorNote,
                                           TInt aBytesToWrite /*=0*/ );


        /**
        * Gets the network list
        * @param aFailedLocked a boolean indicating if the operation
        * had failed because the DB was locked
        * @param aList a reference to the list of networks.
        * The passed list will be erased, elements destroyed,
        * new elements created in it. The list owns the created elements
        * and the caller must maintaine ownership of the list.
        * Leaves on error (Database and memory errors)
        * @return the number of items in the list.
        */
        TInt NetWorkListDataL( TBool& aFailedLocked, 
                               CApNetworkItemList& aList );


        /**
        * Returns the model's required IPv type (4, 6 or both, bitfield)
        */
        TInt RequestedIPvType();


        /**
        * Returns whether the Select menu item should be deleted or not
        * @param aResourceId The ID of the current menu resource
        * @param aCount The number of items in the list
        */
        TBool Need2DeleteSelect( TInt aResourceId, TInt aCount );
        

        /**
        * Returns whether the Open menu item should be deleted or not
        * @param aResourceId The ID of the current menu resource
        * @param aCount The number of items in the list
        * @param aUid The UID of the current item in the list
        * @param aNeedsNone Gives whether NONE is displayed as an item or not
        */
        TBool Need2DeleteOpen( TInt aResourceId, TInt aCount, 
                               TUint32 aUid, TBool aNeedsNone );


        /**
        * Returns whether the Delete menu item should be deleted or not
        * @param aResourceId The ID of the current menu resource
        * @param aCount The number of items in the list
        * @param aUid The UID of the current item in the list
        * @param aNeedsNone Gives whether NONE is displayed as an item or not
        */
        TBool Need2DeleteDeleteL( TInt aResourceId, TInt aCount, 
                                  TUint32 aUid, TBool aNeedsNone );


        /**
        * Returns tha the New/Use existing menu item should be deleted or not
        * @param aResourceId The ID of the current menu resource
        * @param aCount The number of items in the list
        */
        TBool Need2DeleteNewUseExisting(  TInt aResourceId, TInt aCount );


        /**
        * @param aResourceId The ID of the current menu resource
        * Returns whether the New menu item should be deleted or not
        */
        TBool Need2DeleteNewL(  TInt aResourceId );


        /**
        * Returns whether the Help menu item should be deleted or not
        */
        TBool Need2DeleteHelp(  TInt aResourceId );


    protected:
    


        
    public:
        /**
        * Returns the name of the first VPN accesspoint with which the access
        * point with the passed WAP UID is associated.
        * Name is placed on the CleanupStack if not null.
        * Should not be called upon VPN access points!!!
        * @param aUid The WAP UID of the normal access point
        * @return NULL or a pointer to the name of the associated access point.
        */
        HBufC* GetLinkedVpnAccessPointLC( TUint32 aUid );

        
        /*
        * Returns the current EikEnvironment
        */
        CEikonEnv* EikEnv();



        /**
        * Creates a new access point from the passed item.
        * @param aApItem A reference to a CApAccessPointItem holding the
        * access point item to be added to the database
        * @return The UID of the newly created and saved access point.
        */
        TUint32 CreateFromDataL( CApAccessPointItem& aApItem );


        /**
        * Handles WEP security settings
        * Leaves on errors
        * Return TInt giving exit or shutdown required if any, otherwise, 0
        */
        TInt ChangeWepSettingsL( CApAccessPointItem* aApItem );


        /**
        * Handles WPA security settings
        * Leaves on errors
        * Return TInt giving exit or shutdown required if any, otherwise, 0
        */
        TInt ChangeWpaSettingsL( CApAccessPointItem* aApItem );


        /**
        * Handles 802.1x security settings
        * Leaves on errors
        * Return TInt giving exit or shutdown required if any, otherwise, 0
        */
        TInt Change8021xSettingsL( CApAccessPointItem* aApItem );

        /**
        * Resets all WEP and WPA settings to NULL, thus forcing their 
        * re-load
        */
        void ClearWEPAndWPASettings();

        /*
        * Writes the WLAN settings part
        */
        void WriteWlanL( CApAccessPointItem& aApItem, TBool aIsNew );


        /*
        * Loads the WLAN settings part
        */
        void LoadWlanL( CApAccessPointItem& aApItem );

        /**
        * Returns whether the passed item has the security settings
        * filled correctly or not.
        */
        TBool HasWlanSecSettingsFilledL( CApAccessPointItem& aApItem );
        
        
        /**
        * Update the security mode to reflect current state.
        * Called after the user changed security settings
        * @param aApItem The access point item containing the current settings
        */
        void UpdateSecurityModeL( CApAccessPointItem& aApItem );


    private:    // data
        CActiveApDb*                iDb;
        CApDataHandler*             iApData;
        CApUtils*                   iUtils;
        CApSettingsDeleteOperation* iop;
        TApSettingsParams           iParams;
        TBool                       iDbOwned;
        TInt                        iReqIpvType;
        TVpnFilterType              iVpnFilterType;
        CVpnApEngine*               iVpnApEngine;
        TInt                        iVariant;       /// Local variant value
        TBool                       iIncludeEasyWlan;

        CEikonEnv*                  iEikEnv;        /// Eikon environment

    public:
        CWEPSecuritySettings*       iWepSecSettings;    ///< WEP settings data
        TInt                        iWepUiExitReason;   ///< WEP UI exit reason

        CWPASecuritySettings*       iWpaSecSettings;    ///< WPA settings data
        TInt                        iWpaUiExitReason;   ///< WPA UI exit reason
        TBool                       iNoEdit;
        
        /**
        * CFeatureManagerWrapper wraps FeatureManager to eliminate maintenance 
        * of existance of FeatureManager.
        * Owned.
        */
        CFeatureManagerWrapper*     iFeatureManagerWrapper;
        
        TBool                       iInRefresh;
        TBool                       iNeedRefresh;
        TBool                       iHelpSupported;
        TBool                       iProtectionSupported;
    };


#endif

// End of file
