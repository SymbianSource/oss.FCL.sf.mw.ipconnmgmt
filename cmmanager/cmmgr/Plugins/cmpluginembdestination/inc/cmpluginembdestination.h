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
* Description:  Declaration of CCmPluginEmbDestination
*
*/

#ifndef EMBEDDEDDESTINATION_PLUGIN_H
#define EMBEDDEDDESTINATION_PLUGIN_H

#include <cmpluginbaseeng.h>
#include <cmpluginembdestinationdef.h>

/**
 *  Embedded Destination Plugin IF implementation class
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CCmPluginEmbDestination) : public CCmPluginBaseEng
    {
    public:     // Constructors and destructor

        /**
        * Two phased constructor. Leaves on failure.
        * @param - aInitParam initialization parameters for plug-ins.
        * @return The created object.
        */      
        static CCmPluginEmbDestination* NewL( TCmPluginInitParam* aInitParam );

        /**
        * Destructor.
        */      
        virtual ~CCmPluginEmbDestination();

        virtual CCmPluginBaseEng* CreateInstanceL( 
                                        TCmPluginInitParam& aInitParam ) const;

    public: // From CCmPluginBaseEng

        virtual TUint32 GetIntAttributeL( const TUint32 aAttribute ) const;
        virtual TBool GetBoolAttributeL( const TUint32 aAttribute ) const;
        virtual HBufC* GetStringAttributeL( const TUint32 aAttribute ) const;
        virtual HBufC8* GetString8AttributeL( const TUint32 aAttribute ) const;
        virtual void SetBoolAttributeL( const TUint32 aAttribute, 
                                        TBool aValue );
        virtual void SetStringAttributeL( const TUint32 aAttribute, 
                                          const TDesC16& aValue );
        virtual void UpdateL();
        virtual CCmDestinationImpl* Destination() const;

        virtual TBool CanHandleIapIdL( TUint32 aIapId ) const;
        virtual TBool CanHandleIapIdL( CommsDat::CCDIAPRecord *aIapRecord ) const;

        virtual TBool DeleteL( TBool aForced,
                               TBool aOneRefAllowed = ETrue );
        virtual void LoadL( TUint32 aIapId );
        virtual void CreateNewL();

        virtual TInt RunSettingsL();

        /**
        * From CCmPluginBaseEng
        * see base class definition
        *
        * @since S60 3.2
        */
        virtual TBool InitializeWithUiL( TBool aManuallyConfigure );

        virtual TBool IsMultipleReferencedL();

        virtual void LoadServiceSettingL();
        virtual void LoadBearerSettingL();

        virtual void ServiceRecordIdLC( HBufC* &aServiceName, 
                                        TUint32& aRecordId );

        virtual void BearerRecordIdLC( HBufC* &aBearerName, 
                                       TUint32& aRecordId);

        virtual void AdditionalReset(){};

        virtual CCmPluginBaseEng* CreateCopyL() const;
        
        virtual TBool IsLinkedToSnap( TUint32 aSnapId );

    private: // Constructors

        /**
        * Constructor.
        */      
        CCmPluginEmbDestination( TCmPluginInitParam* aInitParam );

        /**
        * Second phase constructor. Leaves on failure.
        * @param
        */      
        void ConstructL();

    private: // From CCmPluginBaseEng

        virtual void CreateNewServiceRecordL();
        virtual void CreateNewBearerRecordL();
        virtual void CopyAdditionalDataL( CCmPluginBaseEng* aDestInst ) const;

    private: // Data

        CCmDestinationImpl* iDestination;
        TInt                iLoadResult;
        TUint32             iCmId;
	};

#endif // EMBEDDEDDESTINATION_PLUGIN_H
