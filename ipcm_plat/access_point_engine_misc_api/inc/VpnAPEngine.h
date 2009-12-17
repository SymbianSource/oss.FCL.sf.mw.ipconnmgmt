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
*     Declares the main handler, CVpnAPEngine and the public API for 
*     the Vpn Access Point Engine.
*
*/


#ifndef VPN_AP_ENGINE_H
#define VPN_AP_ENGINE_H

// Deprecation warning
#warning This header file has been deprecated. Will be removed in one of the next SDK releases.
 
 
// INCLUDES
#include    <e32base.h>

// CONSTANTS


// FORWARD DECLARATIONS
class CCommsDatabase;
class CVpnApItem;

// CLASS DECLARATION
/**
* CVpnApEngine class: It is the main handler for the VPN Access Point Engine.
* It contains the only public API for other (sub)systems.
*
*/
NONSHARABLE_CLASS( CVpnApEngine ) : public CBase
    {

    public: // Constructors and destructor


        /**
        * Two-phased constructor. Leaves on failure.
        * This function shall be used to create the VPN access point engine
        * object. This object is a thin layer above the VpnApEngine which 
        * uses Access Point Engine (ApEngine) and it provides a 
        * simple interface to the VPN ApEngine enabling creation, deletion 
        * and editing of VPN access points.
        * @param aDb a pointer to a CCommsDataBase object, might be NULL
        * @return The constructed CVpnApEngine object.
        *
        * @deprecated
        */
        IMPORT_C static CVpnApEngine* NewLC( CCommsDatabase* aDb = NULL );



        /**
        * Destructor.
        *
        * @deprecated
        */
        IMPORT_C ~CVpnApEngine( );



    protected:
        /**
        * Second-phase constructor.
        * @param aDb The database
        *
        * @deprecated
        */
        IMPORT_C void ConstructL( CCommsDatabase* aDb );


    private:
        /**
        * Constructor.
        *
        * @deprecated
        */
        IMPORT_C CVpnApEngine( );


    public: // New functions

        /**
        * Creates a new VPN from the passed CVpnApItem
        * @param aVpnApItem a CVpnApItem holding the data
        * @return The WAP ID of the succesfully created VPN AP.
        *
        * @deprecated
        */
        IMPORT_C TUint32 CreateVpnApL( CVpnApItem& aVpnApItem );


        /**
        * Creates a new VPN from the passed VPN data
        * @param aName The name of the access point.
        * @param aIapId A TUint32 holding the real internet access point ID
        * @param aPolicy a TDesC& Holding the policy for the VPN
        * @param aProxyServerAddress a TDesC* holding the proxy server address
        * @param aProxyPort A TUint32 specifying the proxy's port number
        * @return The created VPN access point's WAP ID
        *
        * @deprecated
        */
        IMPORT_C TUint32 CreateVpnApL( TDesC& aName, 
                                       TUint32 aIapId, 
                                       TDesC& aPolicy, 
                                       TDesC* aProxyServerAddress = NULL, 
                                       TUint32 aProxyPort = 0 );


        /**
        * Deletes the VPN access poitn indentified by it's WAP ID
        * @param aVpnApId A TUint32 The WAP ID of the vpn access point to be 
        *   deleted
        * Might Leave on error.
        *
        * @deprecated
        */
        IMPORT_C void DeleteVpnL( TUint32 aVpnApId );



        /**
        * Reads up one VPN access point's data from the database.
        * Expects that CVpnApEngine has been created and the database had
        * been opened.
        * In case of error, function leaves with error code.
        * <br>Error conditions:
        *   - Insufficient memory,
        *   - AP does not exists,
        *   - Server connection failure.
        *
        * @param aUid The WAP Uid of the VPN access point to be read
        * @param aApItem A reference to a CApAccessPointItem to hold the data.
        * @return no return value.
        *
        * @deprecated
        */
        IMPORT_C void VpnDataL( TUint32 aUid, CVpnApItem& aVpnApItem );



        // update
        /**
        * Updates data stored in an existing VPN access point.
        * Expects that CVpnApEngine has been created and the database had
        * been opened.
        * In case of error, no data is stored.
        * <br>Error conditions:
        *   - Insufficient memory
        *   - Server connection failure
        *   - Lock already present by other task
        *   - Uid does not exists.
        *   - Attempting to update read-only records will result
        *     in KErrAccessDenied
        *
        * @param aApItem A reference to a CApAccessPointItem holding the data
        * @return No return value.
        *
        * @deprecated
        */
        IMPORT_C void UpdateVpnDataL( CVpnApItem& aVpnApItem );


        /**
        * Gives whether the access point identified by the passed WAP ID
        * is a VPN access point or not.
        * @param aUid The UID of the access point
        *
        * @deprecated
        */
        IMPORT_C TBool IsVpnApL( const TUint32 aUid );


        /**
        * Component Validation Test.
        * @return KErrNone.
        *
        * @deprecated
        */
        IMPORT_C static TInt Cvt();


    protected:  // New functions
        /**
        * Creates a new VPN from the passed VPN item
        * @param aIsNew Booleasn stating whether it is a new AP or not
        * @param aVpnApItem A CVpnApItem holding the VPN access point data
        * @return The created VPN access point's WAP ID
        */
        TUint32 DoCreateVpnApL( TBool aIsNew, CVpnApItem& aVpnApItem );
        
        
        /**
        * Writes the VPN data part from the passed VPN item
        * @param aIsNew Booleasn stating whether it is a new AP or not
        * @param aVpnApItem A CVpnApItem holding the VPN access point data
        */
        void WriteVpnDataL( TBool aIsNew, CVpnApItem& aVpnApItem );


        /**
        * Creates a virtual bearer record if needed
        * @return The virtual bearer record ID
        */
        TUint32 CreateVirtBearerIfNeededL();


        /**
        * Writes the VPN-IAP data part from the passed VPN item
        * @param aIsNew Booleasn stating whether it is a new AP or not
        * @param aVpnApItem A CVpnApItem holding the VPN access point data
        * @return The IAP ID for the VPN part
        */
        TUint32 WriteVpnIapRecordL( TBool aIsNew, CVpnApItem& aVpnApItem );


        /**
        * Gets the location record ID
        * @return The ID of the location record 
        */
        TUint32 GetLocationIdL();


        /**
        * Writes the VPN-WAP data part from the passed VPN item
        * @param aIsNew Booleasn stating whether it is a new AP or not
        * @param aVpnApItem A CVpnApItem holding the VPN access point data
        */
        void    WriteVpnWapApDataL( TBool aIsNew, CVpnApItem& aVpnApItem );


        /**
        * Writes the VPN-WAP bearer data part from the passed VPN item
        * @param aIsNew Booleasn stating whether it is a new AP or not
        * @param aVpnApItem A CVpnApItem holding the VPN access point data
        */
        void    WriteVpnWapBearerDataL( TBool aIsNew, CVpnApItem& aVpnApItem );


        /**
        * Reads up the VPN-WAP bearer data part into the passed VPN item
        * @param aUid The WAP-bearer part's record ID
        * @param aVpnApItem A CVpnApItem to hold the readed data
        */
        void    ReadVpnWapBearerDataL( TUint32 aUid, CVpnApItem& aVpnApItem );


        /**
        * Reads up the VPN-WAP data part into the passed VPN item
        * The WAP record ID should already been set by an earlier call to
        * ReadVpnWapBearerDataL
        * @param aVpnApItem A CVpnApItem to hold the readed data
        */
        void    ReadVpnWapApDataL( CVpnApItem& aVpnApItem );


        /**
        * Reads up the VPN-IAP data part into the passed VPN item
        * The IAP record ID should already been set by an earlier call to
        * ReadVpnWapApDataL
        * @param aVpnApItem A CVpnApItem to hold the readed data
        */
        void    ReadVpnIapRecordL( CVpnApItem& aVpnApItem );


        /**
        * Reads up the VPN data part into the passed VPN item
        * The VPN record ID should already been set by an earlier call to
        * ReadVpnIapRecordL
        * @param aVpnApItem A CVpnApItem to hold the readed data
        */
        void    ReadVpnDataL( CVpnApItem& aVpnApItem );


        /**
        * Writes the proxy data of the passed VPN access point item 
        * @param aIsNew Boolean stating whether the access point is new
        * @param aVpnApItem A CVpnApItem holding the current VPN data
        */
        void    WriteProxyDataL( TBool aIsNew, const CVpnApItem& aVpnApItem );


        /**
        * Reads the proxy data into the passed VPN access point item 
        * @param aVpnApItem A CVpnApItem holding the current VPN data
        */
        void    ReadProxyDataL( CVpnApItem& aVpnApItem );


        /**
        * Deletes the VPN proxy data
        * @param aIspId The Isp record ID for which the proxy data should be
        * deleted 
        */
        void    RemoveVpnProxyDataL( TUint32 aIspId );


        /**
        * Deletes the VPN data
        * @param aUid The UID of the VPN access point to be deleted.
        */
        void    DoDeleteVpnL( TUint32 aUid );



        /**
        * Removes a record from a table
        * @param aTable The table to remove from
        * @param aUid   The record ID to remove
        */
        void    RemoveRecordL( const TDesC& aTable, TUint32 aUid );


        /**
        * Writes the network data of the passed VPN access point item 
        * @param aIsNew Boolean stating whether the access point is new
        * @param aVpnApItem A CVpnApItem holding the current network data
        */
        void    WriteNetworkDataL( TBool aIsNew, CVpnApItem& aVpnApItem );


        /**
        * Reads up the network data into the passed VPN access point item
        * @param aVpnApItem A CVpnApItem holding the current VPN data
        */
        void    ReadNetworkDataL( CVpnApItem& aVpnApItem );


        /**
        * Removes a network data
        * @param aIspId The Network record ID which should be deleted
        */
        void    RemoveNetworkDataL( TUint32 aIspId );


    protected:  // Functions from base classes


    private: //data 
        TBool               iOwnDb;
        CCommsDatabase*     iDb;
    };

#endif 

// End of File
