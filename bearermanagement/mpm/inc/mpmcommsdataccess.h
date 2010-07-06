/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: MPM CommsDat Access class definitions
*
*/

/**
@file mpmcommsdataccess.h
Mobility Policy Manager CommsDat Access class definitions.
*/

#ifndef MPMCOMMSDATACCESS_H
#define MPMCOMMSDATACCESS_H


//  INCLUDES
//
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif
#include <e32base.h>
#include <commsdattypesv1_1.h>
#include <cmpluginwlandef.h>
#include <cmmanager.h>

#include "mpmserversession.h"
#include "mpmlogger.h"

class CCDWlanServiceRecord;

using namespace CommsDat;
using namespace CMManager;

const TInt KMpmDoubleLength = 2;

// Modem bearer names for WLAN Access Points
_LIT( KModemBearerWLAN, "WLANBearer" );

// Length for allocating space for commsdat text fields
//
const TInt KCommsDatTextBufferLength = KMaxTextLength + 1;

enum TWlanIapType
    {
    ENotWlanIap,
    EWlanIap
    };

// CLASS DECLARATION
/**
*  
*  @lib MPMServer.exe
*  @since 
*/
class CMPMCommsDatAccess : public CBase
    {

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMPMCommsDatAccess* NewL();

        /**
        * Destructor.
        */
        virtual ~CMPMCommsDatAccess();

    public: // New methods

        /**
        * Validates the given IAP.
        * @since 3.1
        * @param aConnId Connection Id
        * @param aIapId IAP Id to be validated
        * @param aNetId Network ID for the IAP
        * @param aLanOrWlan IAP uses either LAN or WLAN bearer
        * @param aSession Handle to session class
        */

        void ValidateIapL( const TConnectionId      aConnId, 
                           TUint32&                 aIapId, 
                           TUint32&                 aNetId, 
                           TBool&                   aLanOrWlan,
                           CMPMServerSession&       aSession );

        /**
        * Checks the bearer type of given IAP.
        * @since 3.1
        * @param aIapId IAP Id to be checked
        * @param aLanOrWlan IAP uses either LAN or WLAN bearer
        * @param aAvailableIAPList List of IAPs
        */
        void CheckBearerL( TUint32&                 aIapId, 
                           TBool&                   aLanOrWlan, 
                           const RAvailableIAPList& aAvailableIAPList,
                           CMPMServerSession&       aSession );
    
        /**
        * Find all snap ids
        *
        * @since 3.2
        * @param aSnapIds Array which will be set to 
        * contain the snap ids
        */
        void FindAllSnapsL( RArray<TUint>& aSnapIds );

        /**
        * Dump CommsDat IAPs to log in order to support testing.
        * @since 3.0
        */
        void DumpIAPsL() const;

        /**
        * Dump CommsDat Snaps to log in order to support 
        * testing.
        * @since 3.1
        */
        void DumpSnapsL();

        /**
        * Determine the priority for the Iap Id.
        * @since 3.1
        * @param aDNIaps List of found DN-IAP entries
        * @param aAvailableIAPs List of IAPs
		* @param aSession Handle to session class.
        */
        void DeterminePrioritiesL( RArray<TNetIap>&         aDNIaps, 
                                   const RAvailableIAPList& aAvailableIAPs,
                                   CMPMServerSession&       aSession );

        /**
        * Finds the service type for the IAP and returns the global 
        * bearer priority for that service type.
        * @since 3.1
        * @param aIapId Given Iap Id
        * @param aPriority Global bearer priority for the service type
        */
        void GlobalBearerPriorityL( const TUint32    aIapId,
                                    TUint32&         aPriority);

        /**
        * Checks if there is a WLAN IAP in CommsDat.
        * @since 3.2
        * @param Handle to session class.
        */
        void CheckWLANIapL(CMPMServerSession& aSession);

        /**
        * Checks if the HiddenAgent metadata is set for this IAP.
        * @since 3.2
        * @param aIapId Given Iap Id 
        * return ETrue if HiddenAgent metadata is set, otherwise EFalse.
        */
        TBool CheckHiddenAgentL( const TUint32 aIapId ) const;

        /**
        * Checks if the given IAP Id shares the same SSID 
        * as the active WLAN connection.
        * @since 3.2
        * @param aSSID the id of active wlan network.
        * @param aIapId IAP Id to be checked.
        * @param aMatchFound ETrue if the given IAP Id shares the same SSID 
        * as the active WLAN connection, otherwise EFalse.
        * @param aSession Handle to session class.
        */
        void MatchSSIDL( const TWlanSsid&   aSSID,
                         TUint32            aIapId, 
                         TBool&             aMatchFound,
                         CMPMServerSession& aSession );
        /**
        * Determines whether the IAP is a virtual IAP.
        * @since 3.2
        * @param aVirtualIapId Given virtual Iap Id
        */
        TBool IsVirtualIapL( const TUint32 aVirtualIapId ) const;

        /**
        * Determines whether the SNAP contains at least the 
        * specified number of WLAN IAPs.
        * @since 3.2
        * @param aSnapRecord Id of the SNAP record
        * @param aIapPath Chain of Iaps traversed this far. 
        * @param aMinNrWlan Minimum number of WLAN IAPs to search for 
        * Used for detecting loops in configuration
        */
        TBool SnapContainsWlanL( TUint32		aSnapRecord, 
        						 RArray<TUint>& aIapPath,
        						 TInt 			aMinNrWlan );
        
        /**
        * Calls SearchDNEntries for SNAP and if the SNAP contains
        * embedded SNAP also calls SearchDNEntries for that.
        * @since 3.2
        * @param aSnap SNAP id
        * @param aDestNetIaps Iaps of original SNAP are store here
        * @param aEmbeddedIaps Iaps of embedded SNAP are store here
        */
        void SearchDNEntriesWithEmbeddedL( TUint32 aSnap, 
                                           RArray<TNetIap>& aDestNetIaps,
                                           RArray<TNetIap>& aEmbeddedIaps );

        /**
        * Checks if given IAP is WLAN.
        * @since 3.2
        * @param aIapId IAP Id to be checked
        * @return One of TWlanIapType values
        */
        TWlanIapType CheckWlanL( TUint32 aIapId ) const;

        /**
        * Removes categorised IAPs from the list of available IAPs.
        * @since 3.2
        * @param aIAPList List of IAPs
        */
        void RemoveCategorisedIapsL( RAvailableIAPList& aIAPList );

       /**
        * Get service id of an iap.
        * @since 3.2
        * @param aIapId Id of an Iap.
        */
       TUint32 GetIapServiceIdL( TUint32 aIapId );

       /**
        * Returns ElementId of the record with attribute and flags bit 
        * removed. 
        * 
        * THIS METHOD MUST BE USED INSTEAD OF COMMSDAT ElementId() 
        * in order to clear possible attribute/flag bits. 
        * Otherwise Find may fail if incorrect element id is used.
        *
        * @since 3.2
        * @param aRecord A record to get the element id from
        * @return Element id value
        */        
        static TUint32 GetRealElementId( CMDBRecordBase* aRecord );
        
        /**
         * Check if the Wlan background scan is on.
         * @return ETrue if set.
         */
        TBool WlanBackgroundScanIsOnL();

       /**
        * Returns true if destination is internet or 
        * iap belongs to internet destination.
        * @since 5.1
        * @param aIapId IAP id
        * @param aSnapId Snap id
        * @return True or false depending on destination type.
        */
        TBool IsInternetSnapL( TUint32 aIapId, TUint32 aSnapId );

        /**
         * Returns true if destination is intranet.
         * @param aSnapId Snap id
         * @return True or false depending on destination type.
         */
         TBool IsIntranetSnapL( TUint32 aSnapId );

        /**
        * Used to get bearer type of the iap.
        *
        * @since 5.0
        * @param aIapId Iap id of the connection.
        * @return Type of the bearer.
        */
        TMPMBearerType GetBearerTypeL( TUint32 aIapId );

       /**
        * Check if snap is empty.
        *
        * @since 5.0
        * @param aSnapId Snap id of the connection.
        * @return ETrue if snap is empty.
        */
        TBool IsSnapEmptyL( TUint32 aSnapId );

       /**
        * Verifies that Iap exists. Leaves with error code otherwise
        * @since 9.1
        * @param aIapId Iap
        */        
        void VerifyIapExistsL( TUint32 aIapId );

        /**
         * Gets SnapTierTagId
         * @param aDbs commsdat session
         * @param ElementId
         * @return TagId
         */
        TInt TierLinktoTagIdL( CMDBSession* aDbs, TUint32 aElementId ); 

        /**
         * Maps SNAP RecId to SNAP AP
         * @since 9.1
         * @param aApRecId netid
         * @return SNAP AP
         */
         TInt MapAPRecIdtoSnapAPL( TInt aApRecId );

        /**
         * Maps NetId to SNAP AP
         * @since 9.1
         * @param aNetId netid
         * @return SNAP AP
         */
         TInt MapNetIdtoSnapAPL( TInt aNetId );

        /**
         * Maps SNAP AP to NetId
         * @since 9.1
         * @param aAP APId
         * @return NetId
         */
         TInt MapSnapAPtoNetIdL( TInt aAP );


        /**
        * Finds DN-IAP entries based on given Snap Id.
        * @since 3.1
        * @param aSnapId Given Snap Id
        * @param aDestNetIds List of found DN-IAP entries
        */
        void SearchDNEntriesL( const TUint32&   aSnapId,
                               RArray<TNetIap>& aDestNetIds );

        /**
         * Get current forced roam setting
         * @since 5.2
         * @return Current forced roam setting in commsdat.
         */
        TCmUsageOfWlan ForcedRoamingL() const;
                
        /**
         * Read current general connection settings from CommsDat
         * @since 5.2
         * @return Current general connection settings from CommsDat.
         */        
        TCmGenConnSettings ReadGenConnSettingsL() const;
        
        /**
         * Write general connection settings to CommsDat
         * @since 5.2
         * @param aGenConnSettings General connection settings to written to CommsDat
         */
        void WriteGenConnSettingsL(TCmGenConnSettings& aGenConnSettings);
        
        /**
        * Returns Snap id of destination requested in Snap purpose.
        * @since 5.2
        * @return Snap id.
        */
        TUint32 DestinationIdL( CMManager::TSnapPurpose aSnapPurpose );
        
        /**
        * Returns the default connection values
        * @since symbian^4
        * @param aType The type of the id
        * @param aId The id of the snap or the iap
        */
        void GetDefaultConnectionL( TCmDefConnType& aType, TUint32& aId );
        	
        /**
        * Finds the real IAP behind the virtual IAP.
        * @since 3.1
        * @param aVirtualIapId Given virtual Iap Id
        * @param aRealIapId Real Iap Id
        * @param aAvailableIAPList List of IAPs
        * @param aSession Handle to session class
        */
        void FindRealIapL( const TUint32             aVirtualIapId,
                           TUint32&                  aRealIapId, 
                           const RAvailableIAPList&  aAvailableIAPList,
                           CMPMServerSession&        aSession );
    private:

       /**
        * Loads Iap record based on record id.
        * @since 3.2
        * @param aIapId Id of an Iap.
        * @param aDb DB session.
        * @return Iap record
        */
        CCDIAPRecord* LoadIapRecordLC( const TUint32 aIapId, CMDBSession* aDb ) const;

        /**
        * Finds access network Id based on given IAP Id.
        * @since 3.1
        * @param aIapId Given IAP Id
        * @param aNetId Found access network Id
        */
        void SearchAccessNetIdL( const TUint32&  aIapId,
                                 TUint32&        aNetId );

        /**
        * Checks if given virtual IAP may use underlying WLAN IAP.
        * @since 3.2
        * @param aVirtualIapId IAP Id to be checked
        * @param aIapPath Chain of Iaps traversed this far. 
        * Used for detecting loops in configuration
        * @return ETrue WLAN IAP exists under the virtual IAP, 
        * otherwise EFalse.
        */
        TBool WlanUnderVirtualL( const TUint32 aVirtualIapId, RArray<TUint>& aIapPath  );        

       /**
        * loads linked record using element id.
        * @since 3.2
        * @param aSession DB Session to use
        * @param aElementId Link element id 
        * @return Pointer to allocated record structure.
        */        
        CCDRecordBase* LoadLinkedRecordL( CMDBSession&  aSession, 
                                          TMDBElementId aElementId );

       /**
        * Returns true if destination is internet.
        * @since 5.1
        * @param aDb Handle to database session. 
        * @param aSnapId Destination id
        * @return True or false depending on destination type.
        */
        TBool IsInternetSnapL( CMDBSession& aDb, TUint32 aSnapId );

       /**
        * Check whether bearer type is wlan
        *
        * @since 5.0
        * @param aWlanArray Array of wlan bearers
        * @param aBearerId Bearer id
        * @return Returns ETrue in case of wlan bearer
        */
        TBool IsWlanBearer(RArray<TUint32>& aWlanArray, TInt aBearerId);

       /**
        * Builds wlan bearer array
        * 
        * @since 5.0
        * @param aDb Handle to database session
        * @param aWlanArray Array of wlan bearers
        */
        void BuildWlanArrayL(CMDBSession& aDb, RArray<TUint32>& aWlanArray);

        /**
        * C++ default constructor.
        */
        CMPMCommsDatAccess();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    };

#endif // MPMCOMMSDATACCESS_H

// End of File
