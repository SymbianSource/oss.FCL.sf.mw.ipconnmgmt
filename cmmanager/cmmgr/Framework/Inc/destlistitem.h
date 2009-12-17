/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Declaration of the CDestSelect class.
*
*/


#ifndef CDESTLISTITEM_H
#define CDESTLISTITEM_H

#include <e32base.h>
#include <cmmanagerdef.h>

/**
 *  Representation of a destinations list item.
 *  This class is used to hold the main information of a destination.
 *
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CDestListItem ) : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor. Leaves on failure, places instance
        * on cleanup stack.
        * @param aUid The ID of the destination
        * @param aName The name of the destination
        * @param aNumOfCMs the number of connection methods belonging to the destination
        * @param aIconIndex The icon index of the destination
        * @param aIsProtected Whether the given item is read-only or not
        * @return The constructed CDestListItem.
        */
         static CDestListItem* NewLC( TUint32 aUid,
                                      HBufC* aName,
                                      TInt aNumOfCMs, 
                                      TUint aIconIndex, 
                                      CMManager::TProtectionLevel aProtectionLevel,
                                      TBool aIsDefault );

        /**
        * Destructor.
        */
        virtual ~CDestListItem();

    protected:      // Constructors
    
        /**
        * C++ default constructor.
        */
        CDestListItem( HBufC* aName );
        
        /**
        * Second-phase constructor.
        * @param aUid The ID of the destination
        * @param aNumOfCMs the number of connection methods belonging to the destination
        * @param aIconIndex The icon index of the destination
        * @param aIsProtected Whether the given item is read-only or not
        * @return The constructed CDestListItem.
        */
        void ConstructL( TUint32 aUid,
                         TInt aNumOfCMs, 
                         TUint aIconIndex, 
                         CMManager::TProtectionLevel aProtectionLevel,
                         TBool aIsDefault );
    public: // New functions

        /**
        * Gets the name of the 'current' destination
        * Ownership of the returned text is not passed.
        * @return The name of the current destination
        */
        const TDesC& Name();

        /**
        * Gets the UID of the current destination
        * @return The UID of the current destination
        */
        TUint32 Uid();

        /**
        * Gets the number of connection mehtods belonging to the current destination
        * @return The number of connection mehtods belonging to the current destination
        */
        TInt NumOfCMs();
         
        /**
        * Gets the IconIndex of the current destination
        * @return The IconIndex of the current destination
        */
        TUint IconIndex();

        /**
        * Returns the destination's protected flag
        * @return ETrue if the destination is protected
        */ 
        TUint IsProtected();
        /**
        * Returns the Protection Level
        * @return TProtectionLevel
        */ 
        CMManager::TProtectionLevel GetProtectionLevel();
        TUint IsDefault();

    private:    // Data

        TUint32         iUid;           ///< The ID of the destination
        HBufC*          iName;          ///< The name of the destination
        TInt            iNumOfCMs;      ///< The number of connection mehtods belonging to the
                                        //// destination
        TUint           iIconIndex;     ///< The iIconIndex of the destination
        TBool           iIsProtected;   ///< The IsProtected flag of the destination
        TBool           iIsDefault;   ///< The IsDefault flag of the destination
        CMManager::TProtectionLevel iProtectionLevel;
    };

#endif      // CDESTLISTITEM_H
