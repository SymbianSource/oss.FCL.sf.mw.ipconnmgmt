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
*       Declaration of WLAN interface implementation
*       for WLAN plugin
*
*/


#ifndef WLAN_PLUGIN_H
#define WLAN_PLUGIN_H

#include <commsdat_partner.h>
#include <cmpluginbaseeng.h>
#include <cmpluginwlandef.h>

class CCDWlanServiceRecord;


/**
*  CCmPluginWlan defines the functionality of wlan bearer support.
*  Most of the API is inherited from CCmPluginBaseEng class.
*/
NONSHARABLE_CLASS( CCmPluginWlan ) : public CCmPluginBaseEng
    {

    public: // Constructors and destructor
        /**
         * Two phased constructor.
         * @return Returns the Wlan plugin object.
         */
        static CCmPluginWlan* NewL( TCmPluginInitParam* aInitParam );

        /**
        * Destructor.
        */
        virtual ~CCmPluginWlan();

    public: // From CCmPluginBaseEng
        /**
         * Creates a new instance of Wlan bearer plugin. The instance
         * created represents a bearer, not a connection method.
         * @param aInitParam Initialization data.
         * @return Returns CCmPluginBaseEng type pointer which represents pure
         * bearer instance for the cmm server.
         */

        virtual CCmPluginBaseEng* CreateInstanceL(
                TCmPluginInitParam& aInitParam ) const;

        /**
         * Following GetBearerInfoXXXL methods return the values of the
         * requested attributes. These values are Wlan specific
         * so they don't vary between Wlan Connection Methods.
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
         * @param aIapId IAPId of the Connection Method to be checked.
         * @return ETrue if plug-in can handle the IAP, otherwise EFalse.
         */
        TBool CanHandleIapIdL( TUint32 aIapId ) const;

        /**
        * Checks if the plug-in can handle the given IAP record.
        * @param aIapRecord IAP record to be checked
        * @return ETrue if plug-in can handle the IAP, otherwise EFalse.
        */
        TBool CanHandleIapIdL( CommsDat::CCDIAPRecord *aIapRecord ) const;

    public: // From CCmPluginBaseEng concerning a Connection Method
        /**
         * Before generic side starts to request plugins to load records
         * it calls this to give a possibility make some preparations
         * if needed.
         */
        virtual void PreparePluginToLoadRecordsL();

        /**
         * Before generic side starts to request plugins to update records
         * it calls this to give a possibility make some preparations
         * if needed.
         * @param aGenRecordArray An array of generic records to be updated.
         * @param aBearerSpecRecordArray An array of bearer specific records
         * to be updated.
         */
        virtual void PreparePluginToUpdateRecordsL(
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );

        /**
         * Creates the CCDLANServiceRecord and CCDWlanServiceRecord type service
         * records.
         */
        virtual void CreateServiceRecordL();

        /**
         * Loads the service record(which is in wlan case LAN service type).
         * Service record pointer is stored to plugins generic side
         * implementation(CCmPluginBaseEng).
         */
        void LoadServiceRecordL();

        /**
         * Returns the service record id of this Connection Method.
         * @return Returns the id of the service record id of this Connection Method.
         */
        virtual TUint32 ServiceRecordId() const;

        /**
        * Returns the service record name of the Connection Method.
        * @param aServiceName Name of the service record which CM's iap record
        * points to.
        */
        virtual void ServiceRecordNameLC( HBufC* &aServiceName );

        /**
         * Writes the service record data to CommsDat.
         * @param aGenRecordArray An array containing pointers to service
         * record of the Connection Method.
         * @param aBearerSpecRecordArray An array containing pointers to bearer
         * specific records of the Connection Method.
         */
        virtual void UpdateServiceRecordL(
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );

        /**
         * Loads Wlan specific records of the Connection Method.
         */
        virtual void LoadBearerRecordsL();

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
        virtual void BearerRecordNameLC( HBufC*& aBearerName );

        /**
         * Writes Wlan specific record data to CommsDat.
         * @param aGenRecordArray An array containing pointer to service
         * record of the Connection Method.
         * @param aBearerSpecRecordArray An array containing pointers to bearer
         * specific records to be written.
         */
        virtual void UpdateBearerRecordsL(
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );

        /**
        * Deletes Wlan specific records of the Connection Method.
        */
        void DeleteBearerRecordsL();

        /**
         * Releases the memory reserved by the Wlan specific records
         * of the Connection Method.
         */
        virtual void ResetBearerRecords();

        /**
         * Creates a copy of Wlan specific records and adds them to
         * aRecordArray.
         * @param aRecordArray Pointer array where to add bearer specific
         * records.
         */
        virtual void GetBearerSpecificRecordsL(
                RPointerArray<CommsDat::CCDRecordBase>& aRecordArray );

        /**
         * Following GetBearerXXXXAttribute methods get only the
         * fields in records in pointer arrays(parameters).
         * @param aAttribute Identifier of the requested value.
         * @param aGenRecordArray An array containing pointers to generic
         *      records of the Connection Method.
         * @param aBearerSpecRecordArray An array containing pointers to bearer
         *      specific records of the Connection Method. aAttribute
         *      parameter should identify one field(integer, boolean string)
         *      in one of these records.
         * @return Returns the requested value. In error case leaves with
         *      system-wide error code.
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
                TUint32 aAttribute, TUint32 aValue,
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );

        virtual void SetBearerBoolAttributeL(
                TUint32 aAttribute, TBool aValue,
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );

        virtual void SetBearerStringAttributeL(
                TUint32 aAttribute, const TDesC16& aValue,
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );

        virtual void SetBearerString8AttributeL(
                TUint32 aAttribute, const TDesC8& aValue,
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );

        /**
         * Bearer is requested by the server if it uses CommsDat tables which
         * should be observed for possible changes to be able to keep the
         * data in memory in up to date.
         * @param aTableIdArray A reference to an array where to add those
         * table ids.
         */
        virtual void GetBearerTableIdsToBeObservedL(
                RArray<TUint32>& aTableIdArray ) const;

    private: // construction
        /**
        * Constructor.
        */
        CCmPluginWlan( TCmPluginInitParam* aInitParam );

        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();

    private: // from base class

        /**
        * From CCmPluginBaseEng
        * Creates the WLAN service record used by this WLAN
        */
        void CreateWlanServiceRecordL();

    private: // new functions

        /**
        * Checks if there are any wlan networks awailable.
        * @return Returns ETrue if any wlan networks found. Otherwise
        * returns EFalse.
        */
        TBool CheckNetworkCoverageL() const;

        /**
         * Sets the daemon name for the Connection Method.
         * @param aGenRecordArray An array containing pointers to generic
         * records of the Connection Method.
         * @param aBearerSpecRecordArray An array containing pointers to bearer
         * specific records of the Connection Method.
         */
        void SetDaemonNameL(
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );

        /**
         * Returns a reference to service record of the Connection Method.
         * @return Returns a reference to service record of the Connection
         * Method.
         */
        CommsDat::CCDLANServiceRecord& ServiceRecord() const;

        /**
         * Makes needed preparations before copying records.
         * @param aCopyInstance Instance of the plugin where to copy.
         */
        virtual void PrepareToCopyDataL( CCmPluginBaseEng* aCopyInstance );

        /**
         * Copies the LAN service record.
         * @return Returns a pointer to service record created.
         */
        CommsDat::CCDRecordBase* CopyServiceRecordL();

        /**
         * Copies the Wlan service record.
         * @param aSourceRecord A record where to copy from.
         * @param aDestRecord A record where to copy to.
         */
        void CopyWlanServiceRecordL( CCDWlanServiceRecord* aSourceRecord,
                                     CCDWlanServiceRecord* aDestRecord);

        /**
         * Copies the bearer specific records to copy instance.
         * @param aCopyInstance Instance of the plugin where to copy.
         */
        virtual void CopyBearerRecordsL( CCmPluginBaseEng* aCopyInstance );

        /**
         * Adds Wlan service record with default values to record array.
         * @param aRecordArray The array where to add the record.
         */
        void GetDefaultWlanServiceRecordL(
                RPointerArray<CommsDat::CCDRecordBase>& aRecordArray );

        /**
         * Checks if the given attributes are in the correct range.
         * @param aAttribute Identifies the attribute to be checked.
         * @param aValue A value to be checked.
         */
        void CheckSecurityAttributeValidityL( const TUint32 aAttribute,
                                              const TUint32 aValue );

        /**
         * Convert a 8-bit string in ascii format to hex format. Ownership of
         * new string is transfered.
         */
        HBufC8* ConvertAsciiToHexL( const TDesC8& aSource );

        /**
         * Convert a 8-bit string in hex format to ascii format. Ownership of
         * new string is transfered.
         */
        HBufC8* ConvertHexToAsciiL( const TDesC8& aSource );

        /**
         * Translates a hex char (0-1, a-f, A-F) to its 10-base numerical
         * value (0-15).
         */
        TUint8 ConvertHexCharToNumberL( const TUint8 aHexChar );

    private: // Data

        /**
         * WLan service record.
         */
        CCDWlanServiceRecord* iWlanServiceRecord;

        /**
         * WLAN table id.
         */
        CommsDat::TMDBElementId iWlanTableId;

        /**
         * Boolean to save the WAPI support info.
         */
        TBool iWAPISupported;
    };

#endif // WLAN_PLUGIN_H
