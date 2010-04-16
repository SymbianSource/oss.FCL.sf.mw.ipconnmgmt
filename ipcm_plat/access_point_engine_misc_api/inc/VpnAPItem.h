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
*     Declares the main handler, CVpnAPItem and the API for 
*     the Vpn Access Point Engine.
*
*/


#ifndef VPN_AP_ITEM_H
#define VPN_AP_ITEM_H

// Deprecation warning
#warning This header file has been deprecated and will be fully removed between weeks 12 and 18 of 2010. See Polonium CR #153 for details.
 
 
// INCLUDES
#include    <e32base.h>

// CONSTANTS
enum TVpnApMember
    {
    // WAP Access Point Table
    EApVpnWapAccessPointID,         ///< The Uid of the VPN access point.
                                    ///< READ ONLY, assigned by the database.
                                    ///< TUint32
    EApVpnWapAccessPointName,       ///< The name of the access point, TDesC16

    EApVpnRealWapID,                ///< WAP ID of the real IAP ap, TUint32
    EApVpnIapID,                    ///< IAP ID of the VPN IAP record, TUint32
    EApVpnRealIapID,                ///< IAP ID of the real IAP record used
                                    ///< to create connection, TUint32
    EApVpnRealIapName,              ///< IAP name of the real IAP record used
                                    ///< to create connection, Text
    EApVpnPolicy,                   ///< The VPN policy to be used, TDesC16
    EApVpnPolicyId,                 ///< The VPN policy to be used, TDesC16
    EApVpnProxyServerAddress,       ///< Proxy server address, long text
    EApVpnProxyPortNumber,          ///< Proxy port number,  TUint32

    EApVpnHomeNetworkID,            ///< The ID of the home netwokr, TUint32
    EApVpnIapNetworkID,             ///< The ID of the home netwokr, TUint32
    EApVpnIapNetworkName,           ///< The name of the home netwokr, TDesC16
    };





// FORWARD DECLARATIONS
class CCommsDatabase;

// CLASS DECLARATION
/**
* This class encapsulates VPN related data in addition to a 'normal' 
* access point. It provides Get/Set methods for every member.
* Member set is not limited by the API.
* It is used to read-write VPN data to and form CommsDb.
* Main user is CVpnApEngine class but also UI might use this one.
*/
NONSHARABLE_CLASS( CVpnApItem ) : public CBase
    { // holds VPN specific data

    public:  // Constructors and destructor
        /**
        * Two-phased constructor. Leaves on failure, places instance
        * on cleanup stack. Instance is initialised to default values.
        * @return The constructed CApAccessPointItem.
        *
        * @deprecated
        */
        IMPORT_C static CVpnApItem* NewLC();


        /**
        * Destructor.
        *
        * @deprecated
        */
        IMPORT_C virtual ~CVpnApItem();

        
    protected:      // Constructors

        /**
        * Second-phase constructor.
        *
        * @deprecated
        */
        IMPORT_C void ConstructL();


    private:    // Data
        /**
        * C++ default constructor.
        *
        * @deprecated
        */
        IMPORT_C CVpnApItem();



    public:  // other public, exported functions
        /**
        * Copies the data from another CVpnApItem.
        * Substitute for the "assignment operator". It requires allocation
        * thus can leave.
        * Wap Access Point Uid is not copied.
        *
        * @deprecated
        */
        IMPORT_C void CopyFromL( const CVpnApItem& aCopyFrom  );


        /**
        * Equality operator. Wap Access Point Uid is not counted.
        *
        * @deprecated
        */
        IMPORT_C TBool operator==( const CVpnApItem& aitem ) const;


        /**
        * Inequality operator. Wap Access Point Uid is not counted.
        *
        * @deprecated
        */
        IMPORT_C TBool operator!=( const CVpnApItem& aitem ) const;



        /**
        * This function reads up a 16-bit text value from the specified column.
        * @param aColumn Enum value indicating the column to read.
        * @param aValue Reference to a 16-bit descriptor to hold the value
        * @return no return value
        *
        * @deprecated
        */
        IMPORT_C void ReadTextL( const TVpnApMember aColumn, TDes16& aValue );


        /**
        * This function reads up a long text value from the specified column.
        * The ownership is not passed to the caller. It means that the caller
        * should not delete the received value.
        * If the caller must take the ownership for any reason, must make a
        * copy and use that and delete the copy when no longer needed.
        * @param aColumn Enum value indicating the column to read.
        * @return A const HBufC* to the value.
        *
        * @deprecated
        */
        IMPORT_C const HBufC* ReadConstLongTextL( const TVpnApMember aColumn );


        /**
        * This function reads up a TUint32 value from the specified column.
        * If the specified column does not exist (that is, not part of the
        * database structure), it will Panic in debug builds and it will
        * return KErrInvalidColumn in release builds without Panicking.
        * Panic is used in debug builds because it indicates a programming
        * error. In release, it 'notifies' the caller about the error through
        * the return value but enables system to continue operation.
        * @param aColumn Enum value indicating the column to read.
        * @param aValue A reference to a TUint32 to hold the value
        * @return Error code.
        *
        * @deprecated
        */
        IMPORT_C TInt ReadUint( const TVpnApMember aColumn, TUint32& aValue );

        /**
        * This function reads up a boolean value from the specified column.
        * If the specified column does not exist (that is, not part of the
        * database structure), it will Panic in debug builds and it will
        * return KErrInvalidColumn in release builds without Panicking.
        * Panic is used in debug builds because it indicates a programming
        * error. In release, it 'notifies' the caller about the error through
        * the return value but enables system to continue operation.
        * @param aColumn Enum value indicating the column to read.
        * @param aValue A reference to a boolean to hold the value
        * @return Error code.
        *
        * @deprecated
        */
        IMPORT_C TInt ReadBool( const TVpnApMember aColumn, TBool& aValue );



        /**
        * This function writes a 16-bit text value to the specified column.
        * If the specified column does not exist (that is, not part of the
        * database structure), it will Panic in debug builds and it will
        * return KErrInvalidColumn in release builds without Panicking.
        * Panic is used in debug builds because it indicates a programming
        * error. In release, it 'notifies' the caller about the error through
        * the return value but enables system to continue operation.
        * This function can Leave if the copying of the text does not succeed.
        * @param aColumn Enum value indicating the column to write.
        * @param aValue Reference to a 16-bit text descriptor to the value
        * @return Error code.
        *
        * @deprecated
        */
        IMPORT_C TInt WriteTextL( const TVpnApMember aColumn,
                                  const TDesC16& aValue );



        /**
        * This function writes a long text value to the specified column.
        * If the specified column does not exist (that is, not part of the
        * database structure), it will Panic in debug builds and it will
        * return KErrInvalidColumn in release builds without Panicking.
        * Panic is used in debug builds because it indicates a programming
        * error. In release, it 'notifies' the caller about the error through
        * the return value but enables system to continue operation.
        * This function can Leave if the copying of the text does not succeed.
        * @param aColumn Enum value indicating the column to write.
        * @param aValue Reference to a 16-bit text descriptor to the value
        * @return Error code.
        *
        * @deprecated
        */
        IMPORT_C TInt WriteLongTextL( const TVpnApMember aColumn,
                                      const TDesC& aValue );


        /**
        * This function writes a TUint32 value to the specified column.
        * If the specified column does not exist (that is, not part of the
        * database structure), it will Panic in debug builds and it will
        * return KErrInvalidColumn in release builds without Panicking.
        * Panic is used in debug builds because it indicates a programming
        * error. In release, it 'notifies' the caller about the error through
        * the return value but enables system to continue operation.
        * @param aColumn Enum value indicating the column to write.
        * @param aValue A const reference to a TUint32 value
        * @return Error code.
        *
        * @deprecated
        */
        IMPORT_C TInt WriteUint( const TVpnApMember aColumn,
                                 const TUint32& aValue );

        /**
        * This function writes a boolean value to the specified column.
        * If the specified column does not exist (that is, not part of the
        * database structure), it will Panic in debug builds and it will
        * return KErrInvalidColumn in release builds without Panicking.
        * Panic is used in debug builds because it indicates a programming
        * error. In release, it 'notifies' the caller about the error through
        * the return value but enables system to continue operation.
        * @param aColumn Enum value indicating the column to write.
        * @param aValue A const reference to a boolean value
        * @return Error code.
        *
        * @deprecated
        */
        IMPORT_C TInt WriteBool( const TVpnApMember aColumn,
                                 const TBool& aValue );



        // Query
        // COMMON
        /**
        * This function returns the ID of the access point
        * @return The ID of this access point
        *
        * @deprecated
        */
        IMPORT_C TUint32 WapUid() const;


        /**
        * This function returns the name of the connection
        * Ownership is not passed.
        * @return The name of the connection
        *
        * @deprecated
        */
        IMPORT_C const TDesC& Name() const;

protected:
        /**
        * Reallocates a passed 16-bit descriptor. Passed descriptor is deleted,
        * new one is AllocL-ed
        * Passed descriptor MUST NOT BE on the CleanupStack
        */
        void ReAllocL(HBufC*& aDes, const TDesC16& aValue);

        
        /**
        * Removes invalid chars from the passed text
        * Invalid chars are: TAB
        * @return a HBufC16 holding the new text
        */
        HBufC* RemoveInvalidCharsLC( const TDesC16& aInText );


        /**
        * Returns whether this VPN access point has proxy data or not.
        * 
        */
        TBool HasProxy() const;

private:    // Data


        HBufC*      iName;
        TUint32     iVpnTableId;
        TUint32     iVpnIapId;
        TUint32     iRealIapId;

        TUint32     iVpnRealWapId;
        TUint32     iVpnWapId;

        TUint32     iVirtualBearerID;

        HBufC*      iPolicyId;

        HBufC*      iPolicy;
        HBufC*      iProxyServerAddress;
        HBufC*      iRealIapName;
        TUint32     iProxyPort;
        TUint32     iHomeNetwork;
        TUint32     iVpnIapNetwork;
        HBufC*      iVpnIapNetworkName;


    protected:  // Friend classes
        friend class CVpnApEngine;

    };



#endif 

// End of File
