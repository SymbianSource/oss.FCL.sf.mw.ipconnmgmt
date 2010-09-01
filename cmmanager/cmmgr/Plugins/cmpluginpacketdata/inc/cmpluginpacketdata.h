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
* Description:  Declaration of CCmPluginPacketData.
*
*/

#ifndef PACKETDATA_PLUGIN_H
#define PACKETDATA_PLUGIN_H

#include <cmpluginbaseeng.h>
#include <cmpluginpacketdatadef.h>

/**
*  CCmPluginPacketData dialog class
*  @since S60 v3.2
*/
NONSHARABLE_CLASS( CCmPluginPacketData ) : public CCmPluginBaseEng
    {

    public: // Constructors and destructor

        static CCmPluginPacketData* NewL( TCmPluginInitParam* aInitParam );

        virtual ~CCmPluginPacketData();

        virtual CCmPluginBaseEng* CreateInstanceL( TCmPluginInitParam& aInitParam ) const;
        
    public: // From CCmPluginBaseEng
        
        /**
        * From CCmPluginBaseEng
        * see base class
        *
        * @since S60 3.2
        * @param aAttribute
        */
        virtual TUint32 GetIntAttributeL( const TUint32 aAttribute ) const;

        /**
        * From CCmPluginBaseEng
        * see base class
        *
        * @since S60 3.2
        * @param aAttribute
        */
		virtual TBool GetBoolAttributeL( const TUint32 aAttribute ) const;

		/**
        * From CCmPluginBaseEng
        * see base class
        *
        * @since S60 3.2
        * @param aAttribute
        */
		virtual HBufC* GetStringAttributeL( const TUint32 aAttribute ) const;

		/**
        * From CCmPluginBaseEng
        * see base class
        *
        * @since S60 3.2
        * @param aAttribute
        * @param aValue
        */
		virtual void SetIntAttributeL( const TUint32 aAttribute, 
		                                                    TUint32 aValue );

		
		/**
        * From CCmPluginBaseEng
        * see base class
        *
        * @since S60 3.2
        * @param aAttribute
        * @param aValue
        */
		virtual void SetBoolAttributeL( const TUint32 aAttribute, 
		                                                    TBool aValue );

		/**
        * From CCmPluginBaseEng
        * see base class
        *
        * @since S60 3.2
        * @param aAttribute
        * @param aValue
        */
		virtual void SetStringAttributeL( const TUint32 aAttribute, 
		                                            const TDesC16& aValue );
        
        /**
        * From CCmPluginBaseEng
        * see base class
        *
        * @since S60 3.2
        * @param aIapId
        */
        virtual void PrepareToUpdateRecordsL();
        
        /**
        * From CCmPluginBaseEng
        * see base class
        *
        * @since S60 3.2
        * @param aIapId
        */
		virtual TBool CanHandleIapIdL( TUint32 aIapId ) const;
        
        /**
        * From CCmPluginBaseEng
        * see base class
        *
        * @since S60 3.2
        * @param aIapRecord
        */
		virtual TBool CanHandleIapIdL( CommsDat::CCDIAPRecord *aIapRecord ) const;
        
        /**
        * From CCmPluginBaseEng
        * see base class
        *
        * @since S60 3.2
        */
		virtual TInt RunSettingsL();
        
	    /**
        * From CCmPluginBaseEng
        * see base class
        *
        * @since S60 3.2
        */
		virtual void LoadServiceSettingL();

	    /**
        * From CCmPluginBaseEng
        * see base class
        *
        * @since S60 3.2
        */
		virtual TBool InitializeWithUiL( TBool aManuallyConfigure );
	    
        /**
        * From CCmPluginBaseEng
        * see base class
        *
        * @since S60 3.2
        * @param aServiceName
        * @param aRecordId
        */
		virtual void ServiceRecordIdLC( HBufC* &aServiceName, 
                                                        TUint32& aRecordId );

	    /**
        * From CCmPluginBaseEng
        * see base class
        *
        * @since S60 3.2
        * @param aBearerName
        * @param aRecordId
        */
		virtual void BearerRecordIdLC( HBufC* &aBearerName, 
                                                        TUint32& aRecordId );
                                       
    public: // New functions
    
        /**
        * @param aPdpType PDP type to be set
        */     
        void SetPDPTypeL( RPacketContext::TProtocolType aPdpType );

        /**
        * @since S60 3.2
        * @return the service record
        */
		CommsDat::CCDWCDMAPacketServiceRecord& ServiceRecord() const;
        
    protected: // From CCmPluginBaseEng
    
        /**
        * From CCmPluginBaseEng
        *
        * @since S60 3.2
		*/      
		virtual void AdditionalReset();

        /**
        * From CCmPluginBaseEng
        *
        * @since S60 5.2
		*/      
        virtual void CreateAdditionalRecordsL();

        /**
        * From CCmPluginBaseEng
        *
        * @since S60 5.2
		*/      
        virtual void DeleteAdditionalRecordsL();

        /**
        * From CCmPluginBaseEng
        *
        * @since S60 5.2
		*/      
        virtual void LoadAdditionalRecordsL();

        /**
        * From CCmPluginBaseEng
        *
        * @since S60 5.2
		*/      
        virtual void UpdateAdditionalRecordsL();

	private: // Constructors

        /**
        * Constructor
        *
        * @since S60 3.2
        * @param aInitParam
        * @param aOutgoing
        */      
		CCmPluginPacketData( TCmPluginInitParam* aInitParam, 
		                     TBool aOutgoing );

        /**
        * Second phase constructor    
        *
        * @since S60 3.2
		*/      
		void ConstructL();

    private: // from CCmPluginBaseEng
    
        /**
        * Second phase constructor    
        *
        * @since S60 3.2
		*/      
		virtual void CreateNewServiceRecordL();
		
		virtual void PrepareToCopyDataL( CCmPluginBaseEng* aDestInst ) const;
        
        /**
        * Sets the daemon name. Starts DHCP if set.
        *
        * @since S60 3.2
        */
        void SetDaemonNameL();

    private:
    
	    /**
        * @since S60 3.2
        * @return if there is network coverage
        */
		TBool CheckNetworkCoverageL() const;
        
	private: // Data
	    
        TBool   iOutgoing;  ///< EPacketDataOutGoing

        CommsDat::CCDUmtsR99QoSAndOnTableRecord* iPacketDataQoSRecord; // Owned
	};

#endif // PACKETDATA_PLUGIN_H
