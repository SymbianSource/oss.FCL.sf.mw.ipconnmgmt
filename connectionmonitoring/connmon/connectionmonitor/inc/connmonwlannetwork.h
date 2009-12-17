/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Removed from rconnmon.h because this will be deprecated.
*
*/

#ifndef CONNMONWLANNETWORK_H
#define CONNMONWLANNETWORK_H

#include <e32base.h>
#include <rconnmon.h>

/**
 * TConnMonWLANNetwork is a class, incapsulated WLAN network abstraction
 * Implements MDesSerializer interface to be able to be transferred through processes (client/server)
 * in object's size-independent manner, to meet backward/forward compatibility
 *
 * @deprecated
 * @since 3.1
 */
class TConnMonWLANNetwork: public MDesSerializer<TConnMonWLANNetwork>
    {
public:
    // Length of the WLAN network name
    static const TUint KMaxNameLength = 32;

    // Amount of additional fields in the buffer
    static const TUint KNumberOfExtraFields = 0;

    // Defines max size of the object in TChar's
    static const TUint KMaxSize = (
            sizeof(TUint)           // length of name
            +sizeof(TBuf<KMaxNameLength>) // iName
            +sizeof(TUint)          // iConnectionMode
            +sizeof(TUint)          // iSignalStrength
            +sizeof(TUint)          // iSecurityMode
            +sizeof(TUint)          // length of buffer
            +sizeof(TInt)*KNumberOfExtraFields
            +sizeof(RArray<TInt> )  // Array of extra fields
            ) / sizeof(TChar) + 1;    // +1 is for rounding to bigger integer

    // Defines shifts of fields inside the object, packed to a descriptor
    enum FieldsShifts
        {
        ENameLength =     0,
        EName =           1,
        // Relatively to iName's end
        EConnectionMode = 1,
        ESignalStrength = 2,
        ESecurityMode =   3,
        EBufCounter =     4,
        EBufBase =        4/*,
        EBufParam0 =      5 */
        };

    /**
     * Default constructor
     * @param aName name of the network
     * @param aConectionMode ConnectionMode of the network
     * @param aSignalStrength Signal strength of the network
     * @param aSecurityMode Security mode of the network
     * @param aBuf Buffer of the additional fields
     */
    IMPORT_C TConnMonWLANNetwork(
            const TBuf<KMaxNameLength>& aName,
            TUint aConnectionMode,
            TUint aSignalStrength,
            TUint aSecurityMode,
            const RArray<TInt>& aBuf );

    /**
     * Destructor
     */
    IMPORT_C virtual ~TConnMonWLANNetwork();

    /**
     * Compares two networks by iSignalStrength;
     *
     * @param aFirst to be compared with aSecond
     * @return 1 if aFirst less than aSecond; 0 if they are equal; -1 otherwise
     */
    IMPORT_C static TInt LessBySignal(
            const TConnMonWLANNetwork& aFirst,
            const TConnMonWLANNetwork& aSecond );

    /**
     * Getters Name(), ConnectionMode(), SignalStrength(), SecurityMode(), Buf()
     *
     * @return copy of the fields
     */
    inline TBuf<KMaxNameLength> Name()
        {
        return iName;
        };
    inline TUint ConnectionMode()
        {
        return iConnectionMode;
        };
    inline TUint SignalStrength()
        {
        return iSignalStrength;
        };
    inline TUint SecurityMode()
        {
        return iSecurityMode;
        };
    inline RArray<TInt> Buf()
        {
        return iBuf;
        };

    // From MDesSerializer

    /**
     * See MDesSerializer
     */
    IMPORT_C const HBufC* ToBuf() const;

    /**
     * See MDesSerializer
     */
    IMPORT_C static TConnMonWLANNetwork FromPtrC( const TPtrC& aPtrC );

private: // Data
    TBuf<KMaxNameLength> iName; // Network name
    TUint iConnectionMode;      // See TConnMonNetworkMode
    TUint iSignalStrength;      // Signal strength
    TUint iSecurityMode;        // See TConnMonSecurityMode
    RArray<TInt> iBuf;          // For additional fields
    };

/**
 * Template for the package of array of items of class TConnMonWLANNetwork
 */
class ConnMonWLANNetworksArrayPckg
    {
public:
    /**
     * Constructs the package of array of objects TConnMonWLANNetwork on the heap
     *
     * @param aRef Array of objects of class TConnMonWLANNetwork
     * @param aBufSize Size of the buffer, intended to contain the package
     */
    ConnMonWLANNetworksArrayPckg(
            const RArray<TConnMonWLANNetwork>& aRef,
            TUint aBufSize );

    /**
     * Constructs empty package on the heap
     *
     * @param aBufSize Size of the buffer, intended to contain the package
     */
    IMPORT_C ConnMonWLANNetworksArrayPckg( TUint aBufSize );

    /**
     * Destruction
     */
    IMPORT_C virtual ~ConnMonWLANNetworksArrayPckg();

    /**
     * Unpacks package to an array of objects of class TConnMonWLANNetwork
     *
     * @param aRef Array, intended to containing the unpacked objects
     */
    IMPORT_C void UnpackToL( RArray<TConnMonWLANNetwork>& aRef ) const;

    /**
     * Gets iBuf
     *
     * @return iBuf
     */
    IMPORT_C HBufC* Buf() const;

private: // Data
    /**
     * Buffer on the heap, containing the package
     */
    HBufC* iBuf;
    };

// Array of the WLAN networks
typedef RArray<TConnMonWLANNetwork> RConnMonWLANNetworksArray;

#endif // CONNMONWLANNETWORK_H

// End-of-file
