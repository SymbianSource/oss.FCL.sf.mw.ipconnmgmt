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
* Description:  VPN plugin IF implementation class.
*
*/

#ifndef VPN_PLUGIN_INCLUDED
#define VPN_PLUGIN_INCLUDED

#include <e32base.h>
#include <StringLoader.h>
#include <in_sock.h>

#include <cmpluginbaseeng.h>

#include <cmpluginvpndef.h>

using namespace CommsDat;

class CCDVirtualIAPNextLayerRecord;

/**
 *  VPN Plugin IF implementation class
 *  @since S60 3.2
 */
NONSHARABLE_CLASS(CCmPluginVpn) : public CCmPluginBaseEng
    {

    public:     // Constructors and destructor

        /**
        * Two phased constructor. Leaves on failure.
        * @return The created object.
        */      
        static CCmPluginVpn* NewL( TCmPluginInitParam* aInitParam );

        /**
        * Destructor.
        */      
        virtual ~CCmPluginVpn();
        
        virtual CCmPluginBaseEng* CreateInstanceL( TCmPluginInitParam& aInitParam ) const;
        
    public: 
        
        /**
        * Gets the value for a TInt attribute.
        * @param aAttribute Identifies the attribute to be retrived.
        * @param aValue On completion, contains the requested TInt attribute.
        * @return None.
        */
        virtual TUint32 GetIntAttributeL( const TUint32 aAttribute ) const;

        /**
        * Gets the value for a TBool attribute.
        * @param aAttribute Identifies the attribute to be retrived.
        * @param aValue On completion, contains the requested TBool attribute.
        * @return None.
        */
        virtual TBool GetBoolAttributeL( const TUint32 aAttribute ) const;

        /**
        * Gets the value for a String16 attribute.
        * @param aAttribute Identifies the attribute to be retrived.
        * @param aValue On completion, contains the requested TDes16 attribute.
        * @return None.
        */
        virtual HBufC* GetStringAttributeL( const TUint32 aAttribute ) const;

		/**
		* Gets the value for a String8 attribute.
		* @param aAttribute Identifies the attribute to be retrived.
        * @return copy of the requested attribute. Ownership is passed.
		*/
		virtual HBufC8* GetString8AttributeL( const TUint32 aAttribute ) const;

        /**
        * Sets the value for a TInt attribute.
        * @param aAttribute Identifies the attribute to be set.
        * @param aValue The value to be set.
        * @return None.
        */
        virtual void SetIntAttributeL( const TUint32 aAttribute, TUint32 aValue );

        /**
        * Sets the value for a TBool attribute.
        * @param aAttribute Identifies the attribute to be set.
        * @param aValue The value to be set.
        * @return None.
        */
        virtual void SetBoolAttributeL( const TUint32 aAttribute, TBool aValue );

        /**
        * Sets the value for a String16 attribute.
        * @param aAttribute Identifies the attribute to be set.
        * @param aValue The value to be set.
        * @return None.
        */
        virtual void SetStringAttributeL( const TUint32 aAttribute, 
                                          const TDesC16& aValue );

		/**
		* Sets the value for a String8 attribute.
		* @param aAttribute Identifies the attribute to be set.
		* @param aValue The value to be set.
        * @return None.
		*/
		virtual void SetString8AttributeL( const TUint32 aAttribute, 
		                                   const TDesC8& aValue );


		void UpdatePolicyNameL();
		
        /**
        * Puts the currently available bindable destinations into the 
        * passed array. The array is emptied before appending the values.
        * It contains only real destinations, no uncategorised 
        * or any other, created ones (Always ask, DefaultConnection, etc)
        */
        void BindableDestinationsL( RArray<TUint32>& aDestinationArray );
        
        /**
        * Change the VPN connection method name when using 
        * 'Search for WLAN' functionality
        */
        HBufC* GenerateVPNPointToWLANNetworkLC(RResourceFile& rf, HBufC*& aName);
		
    public: // From CCmPluginBaseEng        

        /**
        * Restore the original value of the attribute from commsdat field.
        * Base implementation can be used only with attributes 
        * stored directly in commsdat.
        * @param aAttribute attribute to be restored
        */
        virtual void RestoreAttributeL( const TUint32 aAttribute );
        
        /**
        * Override the load to be able to handle post-processing
        *
        */
        virtual void LoadL( TUint32 aIapId );
        
        virtual TBool CanHandleIapIdL( TUint32 aIapId ) const;
        virtual TBool CanHandleIapIdL( CCDIAPRecord *aIapRecord ) const;
        
        virtual TInt RunSettingsL();
        virtual void LoadServiceSettingL();
        virtual void LoadAdditionalRecordsL();
        
        virtual void UpdateAdditionalRecordsL();

        virtual TBool InitializeWithUiL( TBool aManuallyConfigure  );
        
        virtual void ServiceRecordIdLC( HBufC* &aServiceName, 
                                        TUint32& aRecordId );

        virtual void BearerRecordIdLC( HBufC* &aBearerName, 
                                       TUint32& aRecordId );
                                       
        virtual void PrepareToUpdateRecordsL();
        
        virtual TBool IsLinkedToIap( TUint32 aIapId );
        virtual TBool IsLinkedToSnap( TUint32 aSnapId );
        
        virtual void CreateNewL();

        /**
        * From CCmPluginBaseEng
        * Deletes additional records, in this case, the WLAN records
        *
        * @since S60 3.2
        */
        void DeleteAdditionalRecordsL();
        
    protected:
    
        virtual void AdditionalReset();
    
    private: // Constructors

        /**
        * Constructor.
        */      
        CCmPluginVpn( TCmPluginInitParam* aInitParam );

        /**
        * Second phase constructor. Leaves on failure.
        * @param
        */      
        void ConstructL();

    private:
    
        CCDVPNServiceRecord& ServiceRecord() const;

        TBool ServiceRecordExists() const;
        
        virtual void CreateNewServiceRecordL();
        virtual void CreateAdditionalRecordsL();
        
        virtual void UpdateServiceRecordL();
        
        /**
        * Displays a list of installed VPN policies for user selection
        * and updates the setting
        *
        * @since S60 3.2
        * @return the soft key selection
        */
        TBool ShowPolicySelectionDlgL();
        
        /**
        * Shows a dialog for selecting the underlying connection method
        *
        * @since S60 3.2
        * @return ETrue if success
        */
        TBool ShowRealConnectionSelectionDlgL();
        
        /**
        * Checks whether there are connection methods or destinations which
        * the VPN connection method can bind to
        * 
        * @since S60 3.2
        * @return ETrue found
        */
        TBool BindableRealConnectionsExistL();

        void GenerateDefaultCmNameL();  
        
        /**
        * Returns whether the passed destination can be used as an underlying
        * connection method for this particular CM.
        * It can be used if it does not points to VPN in any way, directly or
        * indirectly, it has at least one, non-virtual CM and
        * it does not links back to our parent destination.
        * The returned value is a bit field defined in the src file and 
        * it holds the following informations: 
        * IsEmpty, ContainsVPN(in any link), LinksBackToPArentDest and
        * HasNonVirtualCM to be able to decide if it is suitable.
        * Used only internally.
        */
        TUint32 CanDestUsedAsUnderlyingConnectionL( TUint32 aDestinationId );


        /**
        * Checks whether the destination/CM passed to it has a VPN connection
        * up-stream (there is a path from a VPN to this item through indirections).
        * 
        * @param aId The id of the destination/connmethod in question.
        * @param aIsDestination ETrue if the passed id is to be considered 
        *                       that of a destination, EFalse otherwise.
        * @return ETrue if this entity is pointed to (in/directly) by a VPN.
        */
        TBool HasUpstreamVpnL( TUint32 aId, TBool aIsDestination );

        /**
        * Checks and converts the id of the destination if needed
        * 
        * @param aDestination the id of the destination
        * @since S60 5.2
        * @return the correct id of the destination 
        *         or leaves with the error code KErrArgument 
        */                    
        TInt CheckValidityAndConvertDestinationIdL( TUint32 aDestinationId );
        
        /**
        * Retrieves default APs from the Tier Record of given identifier.
        * 
        * @param aTierIdentifier identifier for tier record.
        * @since S60 5.2
        * @return an elementid id of the default AP record in tier record. 
        */                    
        const TMDBElementId DefaultAPRecordL( const TInt aTierIdentifier = KAfInet );

    public:                
        /**
        * Scans aDests for destinations that may be valid parents 
        * for this plugin and removes those that may not.
        * @param aDests The array of destination IDs to be filtered.
        */
        virtual void FilterPossibleParentsL( RArray<TUint32>& aDests );

        // finds the parent destination of the connmethod with the given ID
        TUint32 FindParentDestinationL( TUint32 aCmId );
        
    private: // Data
    
        CCmPluginBaseEng*       iRealCM;
        CCDVirtualIAPNextLayerRecord*   iVirtualRecord;
        TMDBElementId           iVirtualTableId;
        HBufC*                  iPolicyName;
        
        /**
         * An array of the bindable destinations, 
         * including (if supported) the uncategorised destation and easy wlan
         */
        RArray<TUint32> iBindableMethods;
    };

#endif // VPN_PLUGIN_INCLUDED
