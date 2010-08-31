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
*       Declaration of embedded destination interface implementation
*       for "Embedded Destination" plugin
*
*/

#ifndef EMBEDDEDDESTINATION_PLUGIN_H
#define EMBEDDEDDESTINATION_PLUGIN_H

#include <cmpluginbaseeng.h>
#include <cmpluginembdestinationdef.h>

/**
*  CCmPluginEmbDestination defines the functionality of Embedded Destination(ED)
*  bearer support.
*  Most of the API is inherited from CCmPluginBaseEng class.
*/
NONSHARABLE_CLASS( CCmPluginEmbDestination ) : public CCmPluginBaseEng
    {
    public: // Constructors and destructor
        /**
         * Two phased constructor.
         * @return Returns the ED plugin object.
         */
        static CCmPluginEmbDestination* NewL( TCmPluginInitParam* aInitParam );

        /**
         * Destructor.
         */
        virtual ~CCmPluginEmbDestination();

    public: // From CCmPluginBaseEng
        /**
         * Creates a new instance of ED bearer plugin. The instance
         * created represents a bearer, not a particular ED.
         * @param aInitParam Initialization data.
         * @return Returns CCmPluginBaseEng type pointer which represents pure
         * bearer instance for the cmm server.
         */
        virtual CCmPluginBaseEng* CreateInstanceL(
                TCmPluginInitParam& aInitParam ) const;

        /**
         * Following GetBearerInfoXXXL methods return the values of the
         * requested attributes. These values are ED specific
         * so they don't vary between EDs.
         * @param aAttribute An attribute identifier.
         * @return Returns the value requested. If not found leaves with
         * KErrNotFound error code.
         */

        virtual TUint32 GetBearerInfoIntL( TUint32 aAttribute ) const;

        virtual TBool GetBearerInfoBoolL( TUint32 aAttribute ) const;

        virtual HBufC* GetBearerInfoStringL( TUint32 aAttribute ) const;

        virtual HBufC8* GetBearerInfoString8L( TUint32 aAttribute ) const;

        /**
         * Does not do anything. Embedded destination does not support this
         * functionality.
         * @return Returns always EFalse.
         */
        virtual TBool CanHandleIapIdL( TUint32 aIapId ) const;

        /**
         * Does not do anything. Embedded destination does not support this
         * functionality.
         * @return Returns always EFalse.
         */
        virtual TBool CanHandleIapIdL(
                CommsDat::CCDIAPRecord* aIapRecord ) const;

    public: // From CCmPluginBaseEng
        /**
         * Following methods do not do anything. Embedded destination does not
         * support these functionalities.
         */
        virtual void PreparePluginToLoadRecordsL();

        virtual void PreparePluginToUpdateRecordsL(
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );

        virtual void CreateServiceRecordL();

        void LoadServiceRecordL();

        virtual TUint32 ServiceRecordId() const;

        virtual void ServiceRecordNameLC( HBufC* &aServiceName );

        virtual void UpdateServiceRecordL(
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );

        virtual void CreateBearerRecordsL();

        virtual void LoadBearerRecordsL();

        virtual void BearerRecordIdL( TUint32& aRecordId );

        virtual void BearerRecordNameLC( HBufC*& aBearerName );

        virtual void UpdateBearerRecordsL(
                RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
                RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray );

        void DeleteBearerRecordsL();

        virtual void ResetBearerRecords();

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

    private: // From CCmPluginBaseEng

        /**
         * Following methods do not do anything. Embedded destination does not
         * support these functionalities.
         */
        virtual void PrepareToCopyDataL( CCmPluginBaseEng* aCopyInstance );

        CommsDat::CCDRecordBase* CopyServiceRecordL();

        virtual void CopyBearerRecordsL( CCmPluginBaseEng* aCopyInstance );

    private: // Constructors
        /**
        * Constructor.
        */
        CCmPluginEmbDestination( TCmPluginInitParam* aInitParam );

        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();

        /**
         * Gets the protection level of this destination.
         * @param aProtLevel Parameter to return the protection level.
         */
        void ProtectionLevelL( TUint32& aProtLevel );

        /**
         * Gets the information if this destination is hidden or not.
         */
        void IsHiddenL( TBool& aHidden );

    private:
        /**
         * Element id of SNAP metadata record
         */
        CommsDat::TMDBElementId iDestMetadataTableId;
	};

#endif // EMBEDDEDDESTINATION_PLUGIN_H
