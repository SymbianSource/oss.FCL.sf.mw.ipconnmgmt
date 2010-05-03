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
*      Declaration of Lan interface implementation
*      for "LAN" plugin
*
*
*/


#ifndef LAN_PLUGIN_H
#define LAN_PLUGIN_H

#include <cmpluginbaseeng.h>
#include <cmpluginlandef.h>


/**
*  CCmPluginLan defines the functionality of lan bearer support.
*  Most of the API is inherited from CCmPluginBaseEng class.
*/
NONSHARABLE_CLASS( CCmPluginLan ) : public CCmPluginBaseEng
    {
    public: // Constructors and destructor
        /**
         * Two phased constructor.
         * @return Returns the LAN plugin object.
         */
        static CCmPluginLan* NewL( TCmPluginInitParam* aInitParam );

        /**
         * Destructor.
         */
        virtual ~CCmPluginLan();

    public: // From CCmPluginBaseEng
        /**
         * Creates a new instance of LAN bearer plugin. The instance
         * created represents a bearer, not a connection method.
         * @param aInitParam Initialization data.
         * @return Returns CCmPluginBaseEng type pointer which represents pure
         * bearer instance for the cmm server.
         */
        virtual CCmPluginBaseEng* CreateInstanceL(
                TCmPluginInitParam& aInitParam ) const;

        /**
         * Following GetBearerInfoXXXL methods return the values of the
         * requested attributes. These values are LAN specific
         * so they don't vary between LAN Conneciton Methods.
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
        virtual TBool CanHandleIapIdL( TUint32 aIapId ) const;

        /**
        * Checks if the plug-in can handle the given IAP record.
        * @param aIapRecord IAP record to be checked
        * @return ETrue if plug-in can handle the IAP, otherwise EFalse.
        */
        virtual TBool CanHandleIapIdL(
                CommsDat::CCDIAPRecord* aIapRecord ) const;

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
         * Creates the CCDLANServiceRecord type service record.
         */
        virtual void CreateServiceRecordL();

        /**
         * Loads the service record. Service record pointer is stored to
         * plugins generic side implementation(CCmPluginBaseEng).
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
         * Creates LAN specific default records.
         */
        virtual void CreateBearerRecordsL();

        /**
         * Loads LAN specific records of the Connection Method.
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
         * Writes LAN specific record data to CommsDat.
         * @param aGenRecordArray An array containing pointers to service
         * record of the Connection Method.
         * @param aBearerSpecRecordArray An array containing pointers to bearer
         * specific records to be written.
         */
        virtual void UpdateBearerRecordsL(
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );

        /**
        * Deletes LAN specific records of the Connection Method.
        */
        void DeleteBearerRecordsL();

        /**
         * Releases the memory reserved by the LAN specific records
         * of the Connection Method.
         */
        virtual void ResetBearerRecords();

        /**
         * Creates a copy of LAN specific records and adds them to
         * aRecordArray.
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

    private: // Constructors
        /**
        * Constructor.
        */
        CCmPluginLan( TCmPluginInitParam* aInitParam );

        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();

    private:
        /**
         * Sets the daemon name for the Connection Method.
         */
        void SetDaemonNameL(
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );

        /**
         * Returns a reference to service record of the Connection Method.
         */
        CommsDat::CCDLANServiceRecord& ServiceRecord() const;

        /**
         * Makes needed preparations before copying records.
         * @param aCopyInstance Instance of the plugin where to copy.
         */
        virtual void PrepareToCopyDataL( CCmPluginBaseEng* aCopyInstance );

        /**
         * Copies the service record.
         * @return Returns a pointer to service record created.
         */
        CommsDat::CCDRecordBase* CopyServiceRecordL();

        /**
         * Copies the bearer specific records to copy instance.
         * @param aCopyInstance Instance of the plugin where to copy.
         */
        virtual void CopyBearerRecordsL( CCmPluginBaseEng* aCopyInstance );

        /**
         * Adds LAN bearer record with default values to record array.
         * @param aRecordArray The array where to add the bearer record.
         */
        void GetDefaultLanBearerRecordL(
                RPointerArray<CommsDat::CCDRecordBase>& aRecordArray );

    private: // Data

        /**
         * Pointer to LAN bearer record.
         */
        CommsDat::CCDLANBearerRecord* iBearerRecord;
    };

#endif // LAN_PLUGIN_H
