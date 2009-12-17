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
* Description: 
*      Declaration of Lan interface implementation 
*      for "LAN" plugin
*      
*
*/


#ifndef LAN_PLUGIN_H
#define LAN_PLUGIN_H

// INCLUDES

#include <cmpluginlandef.h>
#include <cmpluginlanbase.h>

#include <e32base.h>
#include <eikdoc.h>
#include <apparc.h>


// FORWARD DECLARATION
class CommsDat::CCDLANServiceRecord;

// CLASS DECLARATION

/**
*  LAN Plugin IF implementation class
*  @since Series60_3.2
*/
NONSHARABLE_CLASS(CCmPluginLan) : public CCmPluginLanBase
    {

    public:     // Constructors and destructor

        /**
        * Two phased constructor. Leaves on failure.
        * @param - aUrl
        * @return The created object.
        */      
		static CCmPluginLan* NewL( TCmPluginInitParam* aInitParam );

        /**
        * Destructor.
        */      
        virtual ~CCmPluginLan();

        virtual CCmPluginBaseEng* CreateInstanceL( TCmPluginInitParam& aInitParam ) const;
        
    public: // From CCmPluginBaseEng

		/**
		* Gets the value for a TInt attribute.
		* @param aAttribute Identifies the attribute to be retrived.
        * @return contains the requested TInt attribute.
		*/
        virtual TUint32 GetIntAttributeL( const TUint32 aAttribute ) const;
        
        /**
        * Checks if the plug-in can handle the given AP.
        * @param aIapId IAPId of the AP to be checked
        * @return ETrue if plug-in can handle the IAP
        */
        virtual TBool CanHandleIapIdL( TUint32 aIapId ) const;
        
        /**
        * Checks if the plug-in can handle the given AP.
        * @param aIapRecord IAP record to be checked
        * @return ETrue if plug-in can handle the IAP
        */
        virtual TBool CanHandleIapIdL( CommsDat::CCDIAPRecord* aIapRecord ) const;

        virtual void AdditionalReset();
        
    private: // Constructors

        /**
        * Constructor.
        */      
		CCmPluginLan( TCmPluginInitParam* aInitParam );

        /**
        * Second phase constructor. Leaves on failure.
		* @param
        */      
		void ConstructL();

    private: // Data

	};

#endif // LAN_PLUGIN_H
