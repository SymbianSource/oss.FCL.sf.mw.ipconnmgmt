/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  TUN Driver plugin IF implementation class.
*
*/

#ifndef TUNDRIVER_PLUGIN_INCLUDED
#define TUNDRIVER_PLUGIN_INCLUDED

#include <cmplugintundriverdef.h>
#include <cmpluginlanbase.h>

class CCDVirtualIAPNextLayerRecord;

/**
 * TUN Driver Plugin IF implementation class
 * This class will be instantiated when the connection is created using the PluginBearer Id KPluginTUNDriverBearerTypeUid
 * @since S^3
 */
NONSHARABLE_CLASS(CCmPluginTUNDriver) : public CCmPluginLanBase
    {

    public:

        /**
        * This function is invoked by the cmmanager plugin framework to load TUNDriver plugin. 
        * This plugin will be identified based on the plugin UID from cmpluginproxy.cpp
        * @param aInitParam Initialization parameters.
        * @return CCmPluginTUNDriver.
        */
        static CCmPluginTUNDriver* NewL( TCmPluginInitParam* aInitParam );

        /**
        * Destructor. 
        */
        virtual ~CCmPluginTUNDriver();

        /**
        * This function is invoked by the cmmanager plugin framework while loading the plugin. 
        * CreateInstance of the plugin will invoke each time plugin type needs to instantiated 
        * @param aInitParam Initialization parameters.
        * @return CCmPluginBaseEng.
        */
        virtual CCmPluginBaseEng* CreateInstanceL( TCmPluginInitParam& aInitParam ) const;
        
        /**
        * This is function is invoked by cmmanager framework.
        * Once the plugin type is identified, identifies the right ServiceRecord and loads.
        */
        void LoadServiceSettingL();
        
    public:

        /**
        * This is function is invoked by cmmanager framework.
        * Check here if every data of the connection method is valid
        * before records were saved to CommsDat.
        * Set incorrect attribute id in iInvalidAttribute and leave
        * with KErrArgument.
        */
        void PrepareToUpdateRecordsL();
        
        /**
        * This is function is invoked by cmmanager framework. If Settings are enabled.
        * Currently Settings for TUNDriver is not supported.
        * @return KErrNotSupported.
        */
        virtual TInt RunSettingsL();

        /**
        * This is function is invoked by cmmanager framework.
        * Once the plugin type is identified, type of bearer and other attributes types are obtained. 
        * @param aAttribute Attribute type.
        * @return Integer attribute's value.
        */
        virtual TUint32 GetIntAttributeL( const TUint32 aAttribute ) const;
        
        /**
        * This is function is invoked by cmmanager framework.
        * Once the plugin type is identified, finds wether the IAP selected belongs to TUNDriver. 
        * @param aIapId IapId for the selected accesspoint
        * @return ETrue if IAP can be handled else EFalse.
        */
        virtual TBool CanHandleIapIdL( TUint32 aIapId ) const;
        
        /**
        * This is function is invoked by cmmanager framework.
        * Once the plugin type is identified, finds wether the IAP selected belongs to TUNDriver.
        * This is identified if service type is LANService and Bearer is VirtualBearer and
        * IfName is TunDriverIf and agent is TunDriverAgent. 
        * @param aIapRecord - iaprecord of type CCDIAPRecord from commsdatabase
        * @return ETrue if IAP can be handled else EFalse.
        */
        virtual TBool CanHandleIapIdL( CommsDat::CCDIAPRecord* aIapRecord ) const;
        
        /**
        * This is function is invoked by cmmanager framework when needs to initialzed with UI.
        * This feature is currently not supported by the plugin.
        * @param aManuallyConfigure Manual configuration on or off.
        * @return KErrNotSupported
        */
        TBool InitializeWithUiL( TBool aManuallyConfigure );

        /**
        * This is function is invoked by plugin destructor.
        * If the function is not defined then compiler will throw error for TUNDriver plugin construction.
        */
        virtual void AdditionalReset();

    protected:

        /**
        * This is function is invoked by cmmanager framework.
        * @return aName is LanService name and  aRecordId the service record Id.
        */
        virtual void ServiceRecordIdLC( HBufC* &aServiceName, TUint32& aRecordId );

        /**
        * This is function is invoked by cmmanager framework.
        * @param aBearerName Output parameter for bearer name.
        * @param aRecordId Output parameter for Bearer record Id.
        */
        virtual void BearerRecordIdLC( HBufC* &aBearerName, TUint32& aRecordId );

        /**
        * This is function is invoked by cmmanager framework.
        * Once the plugin type is identified, this invoked for the first time when plgun is loaded
        * while creating connection for TUNDriver plugin.
        */
        virtual void CreateNewServiceRecordL();
        
    private:

        /**
        * This is default constructor of the plugin. 
        * @param aInitParam Initialization parameters.
        */
        CCmPluginTUNDriver( TCmPluginInitParam* aInitParam );

        /**
        * This is second phase constructor of the plugin.
        * Builds the commsdatabase with the Tables required for TUNDriver plugin. 
        */
        void ConstructL();

        /**
        * This is function is invoked by cmmanager framework.
        * Returns the service record of the TUN driver plugin.
        * @return service record.
        */
        CommsDat::CCDLANServiceRecord& ServiceRecord()const;
    };

#endif // TUNDRIVER_PLUGIN_INCLUDED
