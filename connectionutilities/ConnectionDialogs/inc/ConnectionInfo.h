/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of class CConnectionInfo
*
*/


#ifndef __CCONNECTIONINFO_H__
#define __CCONNECTIONINFO_H__

// INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
 * Information over the connection
 */
NONSHARABLE_CLASS( CConnectionInfo ) : public CBase
    {
    public:
        /**
        * NewL function
        * @param aName      Name of the connection
        * @param aId        ID of the connection
        * @param aListItem  List item
        * return CConnectionInfo*
        */
        static CConnectionInfo* NewL( TDesC& aName, TUint32 aId, 
                                      TDesC &aListItem );

        /**
        * NewL function
        * @param aName       Name of the connection
        * @param aId         ID of the connection
        * @param aVPN_iap_id ID of the AP related to the VPN AP
        * @param aListItem   List item
        * return CConnectionInfo*
        */
        static CConnectionInfo* NewL( TDesC& aName, TUint32 aId, 
                                      TUint32 aVPN_iap_id, TDesC &aListItem );

        /**
        * NewL function
        * @param aId         ID of the connection
        * @param aDestinationId Destination Id
        * @param aListItem   List item
        * return CConnectionInfo*
        */
        static CConnectionInfo* NewL( TUint32 aId, TUint32 aDestinationId,
                                      TDesC &aListItem );


    private:
        /**
        * Constructor of the CConnectionInfo class
        * @param aId     ID of the connection
        * @param aIsVPN  ETrue if the connection is VPN
        * @param aVPN_iap_id ID of the AP related to the VPN AP
        * @param aBearer Bearer
        * @return -
        */
        CConnectionInfo( TUint32 aId, TBool aIsVPN = EFalse, 
                         TUint32 aVPN_iap_id = 0, TUint32 aDestinationId = 0 );

    public:
        /**
        * ~CConnectionInfo() destructor
        * @param  -
        */
        ~CConnectionInfo();

    private:
        /**
        * Symbian default constructor.
        * @param aName      Name of the connection
        * @param aListItem  List item
        */      
        void ConstructL( TDesC &aName, TDesC &aListItem );

        /**
        * Symbian default constructor.
        * @param aListItem  List item
        */      
        void ConstructL( TDesC &aListItem );

    public:
        /**
        * Id() function
        * Returns iId member
        * @return iId member
        */
        inline const TUint32 Id() const;

        /**
        * ListItem() function
        * Returns iListItem descriptor
        * @return iListItem descriptor
        */
        inline const TDesC& ListItem() const;

        /**
        * Name() function
        * Returns iName descriptor
        * @return iName descriptor
        */
        inline const TDesC& Name() const;

        /**
        * VPNIapId() function
        * Returns iVPN_iap_id member
        * @return iVPN_iap_id member
        */
        inline const TUint32 VPNIapId() const;


        /**
        * VPNIapId() function
        * Returns iDestinationId member
        * @return iDestinationId member
        */
        inline const TUint32 DestinationId() const;


        /**
        * IsVPN() function
        * Tells if connection is VPN
        * @return ETrue if connection is VPN
        */
        inline const TBool IsVPN() const;


    private:    // Data

        // Name of the connection
        HBufC*  iName;

        // ID of the connection
        const TUint32 iId;

        // List item
        HBufC*  iListItem;

        // Tells if the connection is VPN or not
        const TBool iIsVPN;

        // ID of the AP related to the VPN connection
        const TUint32 iVPN_iap_id;

        // ID of the Destination
        const TUint32 iDestinationId;
    };

#include "ConnectionInfo.inl"

#endif

// End of File
