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
* Description:  Declaration of the CCmListItem class.
*
*/

#ifndef CCMLISTITEM_H
#define CCMLISTITEM_H

//  INCLUDES
#include <e32base.h>

/**
 *  Representation of a list item.
 *  This class is used to hold the main information of a connection method.
 *  It is used in CCmSelect.
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCmListItem ) :public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor. Leaves on failure, places instance
        * on cleanup stack.
        * @param aUid The ID of the connection method
        * @param aName The name of the connection method
        * @param aPriority The priority of the connection method
        * @param aIconIndex The icon index of the connection method's 
        * bearer's icon
        * @param aIsProtected Whether the given item is read-only or not
        * @param aIsSingleLine Whether the given item has one line only      
        * @return The constructed CCmListItem.
        */
        static CCmListItem* NewLC( TUint32 aUid,
                                    HBufC* aName,
                                    TInt aPriority, 
                                    TUint aIconIndex, 
                                    TBool aIsProtected, 
                                    TBool aIsVPNOverDestination,
                                    TBool aIsSingleLine, 
                                    TBool aIsDefault);
        /**
        * Destructor.
        */
        virtual ~CCmListItem();

    protected:      // Constructors
    
        /**
        * C++ default constructor.
        */
        CCmListItem( HBufC* aName );

        /**
        * Second-phase constructor.
        * @param aUid The ID of the connection method
        * @param aPriority The priority of the connection method
        * @param aIconIndex The icon index of the connection method's 
        * bearer's icon
        * @param aIsProtected Whether the given item is read-only or not
        * @param aIsSingleLine Whether the given item has one line only
        */
        void ConstructL( TUint32 aUid,
                         TInt aPriority, 
                         TUint aIconIndex, 
                         TBool aIsProtected, 
                         TBool aIsVPNOverDestination,
                         TBool aIsSingleLine, 
                         TBool aIsDefault);
    
    public: // New functions

        /**
        * Gets the name of the 'current' connection method
        * Ownership of the returned text is not passed.
        * @return The name of the current connection method
        */
        const TDesC& Name();

        /**
        * Gets the UID of the current connection method
        * @return The UID of the current connection method
        */
        TUint32 Uid();

        /**
        * Gets the Priority of the current connection method
        * @return The Priority of the current connection method
        */
        TInt Priority();
         
        /**
        * Gets the IconIndex of the current connection method's bearer icon
        * @return The IconIndex of the current connection method's bearer icon
        */
        TUint IconIndex();
        
        /**
        * Returns the connection method's protected flag
        * @return ETrue if the connection method is protected
        */       
        TBool IsProtected();
        
        /**
        * Returns the connection method's destination flag
        * @return ETrue if the item has one line only
        */       
        TBool IsSingleLine();

        /**
        * Returns the connection method's default flag
        * @return ETrue if the item is the default connection
        */       
        TBool IsDefault();

        /**
        * Used to figure out whether connection is VPN over destination.
        * @return ETrue if the item is VPN over destination
        */       
        TBool IsVpnOverDestination();
                
    private:    // Data

        TUint32         iUid;            // The ID of the access point
        HBufC*          iName;           // The name of the access point
        TInt            iPriority;       // The Priority of the access point
        TUint           iIconIndex;      // The IconIndex of the access point
        TBool           iIsProtected;    // The IsProtected flag of the access point
        TBool           iIsSingleLine;   // ETrue if the item has one line only
        TBool           iIsDefault;      // ETrue if the item is the default connection
        TBool           iIsVpnOverDestination; // ETrue if the item is VPN over destination
    };

#endif      // CCMLISTITEM_H
