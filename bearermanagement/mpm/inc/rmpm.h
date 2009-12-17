/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This header defines the client API for MPM server
*
*/

/**
@file rmpm.h
Mobility Policy Manager server client interface definitions.
*/

#ifndef RMPM_H
#define RMPM_H

// INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <wlanmgmtcommon.h> // TWlanSsid
#include <e32property.h>    //publish and subscribe
#include <cmmanagerdef.h>
#include <extendedconnpref.h>

// FORWARD DECLARATIONS

// CONSTANTS
// Number of pointers allocated for client-server inter-thread reads/writes
const TUint KNumPtrs = 8;
// Event buffer, adjust size to the proper value
const TUint KMpmMessageLength   = 512;
const TUint KMpmMaxIAPCount     = 25;

// Definitions for Reading/Writing sort snap data 
// to publish and subscribe API
//
// UID of the category
const TUid KMPMSortSnapCategory = { 0x101f6d3a };

// Property type. Each key contains binary data in 
// TMpmSnapPubSubFormat format
const TInt KMPMSortSnapType = RProperty::EByteArray;

// Maximum property key value. Key values start from 0.
// MPM will always write data at least for uncategorised 
// entry (iap list may be empty). 
const TInt KMPMSortSnapMaxKey = 25;

// Read policy, no capabilities required
// This format is used instead of _LIT_SECURITY_POLICY_PASS, which 
// produces pclint warnings
_LIT_SECURITY_POLICY_C1( KMPMSortSnapReadPolicy, ECapability_None );

// Write policy
_LIT_SECURITY_POLICY_C1( KMPMSortSnapWritePolicy, ECapabilityNetworkControl );

// Definitions for Reading/Writing user connection data
// to publish and subscribe API
//
// UID of the category (note that uid is the same as sort snap category)
const TUid KMPMUserConnectionCategory = { 0x101f6d3a };

// Keys for this category
const TUint KMPMPSKeyUserConnectionSnap = 10001;
const TUint KMPMPSKeyUserConnectionIap = 10002;

// Property types
const TInt KMPMUserConnectionSnapType = RProperty::EInt;
const TInt KMPMUserConnectionIapType = RProperty::EInt;

// Read policy, no capabilities required
// This format is used instead of _LIT_SECURITY_POLICY_PASS, which 
// produces pclint warnings
_LIT_SECURITY_POLICY_C1( KMPMUserConnectionReadPolicy, ECapability_None );

// Write policy
_LIT_SECURITY_POLICY_C1( KMPMUserConnectionWritePolicy, ECapabilityNetworkControl );

// Definitions for Reading/Writing active connection data 
// to publish and subscribe API
//
// UID of the category
const TUid KMPMActiveConnectionCategory = { 0x101f6d3a };

// Property types
const TInt KMPMActiveConnectionIapType = RProperty::EInt;
const TInt KMPMActiveConnectionSnapType = RProperty::EInt;
const TInt KMPMActiveConnectionBearerType = RProperty::EInt;

// Keys for active connection category

/**
 * Iap Id of the active connection.
 * If set to zero then no active connection
 */
const TUint KMPMPSKeyActiveConnectionIap = 10003;

/**
 * Snap Id of the active connection.
 * If set to zero then possible iap connection
 */
const TUint KMPMPSKeyActiveConnectionSnap = 10004;

/**
 * Possible values:
 * 0,
 * KCommDbBearerVirtual,
 * KCommDbBearerWLAN,
 * KCommDbBearerWcdma,
 * KCommDbBearerUnknown,
 */
const TUint KMPMPSKeyActiveConnectionBearer = 10005;

// Read policy, no capabilities required
// This format is used instead of _LIT_SECURITY_POLICY_PASS, which 
// produces pclint warnings
_LIT_SECURITY_POLICY_C1( KMPMActiveConnectionReadPolicy, ECapability_None );

// Write policy
_LIT_SECURITY_POLICY_C1( KMPMActiveConnectionWritePolicy, ECapabilityNetworkControl );

// MACROS
_LIT( KPanicCategory, "MPM Client" );

// DATA TYPES
typedef RArray<TUint32>             RAvailableIAPList;
typedef TBuf8<KMpmMessageLength>    TMPMMessageBuffer;
typedef TUint32                     TConnectionId;

// Notification codes
enum TMpmNotifications
    {
    EMPMNotificationNone = 0, // NOP
    EMPMPreferredIAPAvailable,
    EMPMMobilityErrorNotification,
    EMPMClientErrorNotification,
    EMPMStartIAPNotification,
    EMPMStopIAPNotification
    };

// Asynchronous codes
enum TMpmAsynchCodes
    {
    EMPMChooseBestIAP = 1,
    EMPMReselectBestIAP,
    EMPMProcessError,
    EMPMWaitNotification,
    EMPMSortSNAP,
#ifdef _DEBUG
    EMPMDebugGenerateStopIAPNotification = 500,
    EMPMDebugSwitchConnMonEventsOn = 501,
    EMPMDebugSwitchConnMonEventsOff = 502,
    EMPMDebugShutdown = 503,
#endif
    };

enum TBMNeededAction
    {
    EIgnoreError = 0,
    EPropagateError,
    EDoReselection
    };

// CLASS DECLARATION
/**
*  Contains the sorted list of IAPs belonging to the specified SNAP. 
*  Used by method SortSNAP.
*
*  @since 3.2
*/
NONSHARABLE_CLASS( TMpmSnapBuffer )
    {
    public:
        // Zero values 
        inline void Reset();    
        inline const TInt Count() const;

    public:
        TUint   iCount;
        TUint32 iIapId[KMpmMaxIAPCount];
    };

// CLASS DECLARATION
/**
*  Contains the sorted list of IAPs belonging to the specified SNAP. 
*  Used by method SortSNAP.
*
*  @since 3.2
*/
NONSHARABLE_CLASS( TMpmSnapPubSubFormat )
    {
    public:
        // Snap id. 0 is used for uncategorised
        TUint32           iSnap;
        // Iap ids
        TMpmSnapBuffer  iSortedIaps;
    };
            
// CLASS DECLARATION
/**
*  Helper class.
*
*  @since 3.2
*/
NONSHARABLE_CLASS( TStartIAPNotifInfo )
    {
    public:
        // Zero values 
        inline void Reset();    
    public:
        TUint32         iIap;
        TConnectionId   iConnId; // useless
        TUint32         iServiceId;
    };
    
// CLASS DECLARATION
/**
*  Helper class.
*
*  @since 3.2
*/
NONSHARABLE_CLASS( TStopIAPNotifInfo )
    {
    public:
        // Zero values 
        inline void Reset();    
    public:
        TUint32         iIap;
        TConnectionId   iConnId;
    };
    
// CLASS DECLARATION
/**
*  Helper class.
*
*  @since 3.1
*/
NONSHARABLE_CLASS( TMpmNotification )
    {
    public:
        TMpmNotifications iMPMNotificationType;
        virtual TInt Length() = 0;
    };

// CLASS DECLARATION
/**
*  Helper class.
*
*  @since 3.1
*/
NONSHARABLE_CLASS( TMpmNotificationPrefIAPAvailable ) : 
    public TMpmNotification
    {
    public:
        inline TInt Length();

    public:
        TUint32      iOldIapId;
        TUint32      iNewIapId;
        TBool        iIsUpgrade;
        TBool        iIsSeamless;
        TUint32      iConnId;
    };
    
// CLASS DECLARATION
/**
*  Helper class.
*
*  @since 3.2
*/
NONSHARABLE_CLASS( TMpmNotificationError ) : public TMpmNotification
    {
    public:
        inline TInt Length();

    public:
        TInt    iError;
        TUint32 iConnId;
    };
    

// CLASS DECLARATION
/**
*  Helper class.
*
*  @since 3.2
*/
NONSHARABLE_CLASS( TMpmNotificationStartIAP ) : public TMpmNotification
    {
    public:
        inline TInt Length();
    public:
        TStartIAPNotifInfo iInfo;
    };
    
// CLASS DECLARATION
/**
*  Helper class.
*
*  @since 3.2
*/
NONSHARABLE_CLASS( TMpmNotificationStopIAP ) : public TMpmNotification
    {
    public:
        inline TInt Length();
    public:
        TStopIAPNotifInfo iInfo;
    };

// CLASS DECLARATION
/**
*  ChooseBestIAPPolicyResponse
*  Identical in size with the TConnPref but contains different fields
*  
*  @since 5.2
*/
struct SPolicyConnPref
    {
    TUint32 iIapId;
    TUint32 iNetId;
    TUint32 iSnapAPId;
    TUint32 iServiceId; // Is the original IAP's Service ID
    };

NONSHARABLE_CLASS( TPolicyConnPref ) : public TConnPref
    {
protected:
    inline SPolicyConnPref* PrefPtr() const {
    return reinterpret_cast<SPolicyConnPref*>(UserPtr());
    }
    
public:
    enum { EConnS60PolicyPref = 0x0f }; // TConnPref has defined up to 0x07.
    inline TPolicyConnPref::TPolicyConnPref()
    : TConnPref( TPolicyConnPref::EConnS60PolicyPref )
        {
        SetIapId( 0 );
        SetNetId( 0 );
        SetSnapAPId( 0 );
        SetServiceId( 0 );
        }

public:
    inline void SetIapId( TUint32 aIapId ) { PrefPtr()->iIapId = aIapId; };
    inline TUint32 IapId() const { return PrefPtr()->iIapId; };

    inline void SetNetId( TUint32 aNetId ) { PrefPtr()->iNetId = aNetId; };
    inline TUint32 NetId() const { return PrefPtr()->iNetId; };

    inline void SetSnapAPId( TUint32 aSnapAPId ) { PrefPtr()->iSnapAPId = aSnapAPId; };
    inline TUint32 SnapAPId() const { return PrefPtr()->iSnapAPId; };

    inline void SetServiceId( TUint32 aServiceId ) { PrefPtr()->iServiceId = aServiceId; };
    inline TUint32 ServiceId() const { return PrefPtr()->iServiceId; };
    };
    
// CLASS DECLARATION
/**
*  ChooseBestIAPPolicyResponse
*  Identical in size with the TConnPref but contains different fields
*  
*  @since 5.2
*/
struct SMpmConnPref
    {
    /** SNAP purpose. */
    CMManager::TSnapPurpose iSnapPurpose;
    /** SNAP id. */
    TUint32 iSnapId;
    /** IAP id. */
    TUint32 iIapId;
    /** Bearer set. */
    TUint32 iBearerSet;
    /** UI behaviour. */
    TUint32 iNoteBehaviour;
    /** Indicates whether UI disconnect dialog is shown. */
    TBool iDisconnectDialog;
    /** Indicates whether UI connection selection dialog is shown. */    
    TBool iConnSelectionDialog;
    /** Indicates whether forced roaming is enabled. */
    TBool iForcedRoaming;
    
    // The following variables are used for internal functionality of the MPM.
    // Client should not set these but leave it to MPM to fullfil them based
    // on the above variables.

    /**
     * Connection type that has been requested from the MPM. This is used
     * internally by MPM to select correct IAP in the requested manner.
     */
    TUint32 iConnType;
    
    /**
     * Indicates whether the given IAP is forced to be used or whether other
     * IAPs can be used. IAP is set to "mandated" when IAP id is given
     * through TMpmConnPref (meaning it was given as TExtendedConnPref
     * to RConnection::Start()). Otherwise IAP is not mandated
     * because in the old way existing connection was allowed to be
     * taken into use whenever an IAP was given.
     */
    TBool iMandateIap;

    /**
     * Access network id. Notice that this field always contains the access
     * network id and this is not mixed with SNAP id into the same field as
     * it used to be. SNAP id is in its own field. 
     */
    TUint32 iNetId;
    };

NONSHARABLE_CLASS( TMpmConnPref ) : public TConnPref
    {
    
protected:
    inline SMpmConnPref* PrefPtr() const {
        return reinterpret_cast< SMpmConnPref* >( UserPtr() );
    }

public:
    /**
     * Extension id for MPM connection preferences.
     * All connection preferences derived from TConnPref has an extension id. 
     * TConnPref has defined up to 0x07 and TPolicyconnPref has 0x0f.
     */
    enum { EConnPrefMpm = 0x0e };

    /**
     * Connection type that has been requested from the MPM.
     * This is used internally by MPM to select correct IAP
     * in the requested manner.
     */
    enum EConnType {
        /**
         * Default connection is requested, that is, nobody requests
         * specification connection.
         */
        EConnTypeDefault = 0,
        /**
         * User selects the connection by utilizing Connection Selection
         * Dialog.
         */
        EConnTypeImplicit,
        /**
         * Client selects the connection by passing it via connection
         * preferences.
         */
        EConnTypeExplicit
    };
    
    inline TMpmConnPref::TMpmConnPref()
    : TConnPref( TMpmConnPref::EConnPrefMpm )
        {
        SetSnapPurpose( CMManager::ESnapPurposeUnknown );
        SetSnapId( 0 );
        SetIapId( 0 );
        SetBearerSet( TExtendedConnPref::EExtendedConnBearerUnknown );
        SetNoteBehaviour( TExtendedConnPref::ENoteBehaviourDefault );
        SetDisconnectDialog( ETrue );
        SetConnSelectionDialog( EFalse );
        SetForcedRoaming( ETrue );
        SetConnType( EConnTypeDefault );
        SetMandateIap( EFalse );
        SetNetId( 0 );
        }

public:
    inline void SetSnapPurpose( CMManager::TSnapPurpose aSnapPurpose ) { PrefPtr()->iSnapPurpose = aSnapPurpose; };
    inline CMManager::TSnapPurpose SnapPurpose() const { return PrefPtr()->iSnapPurpose; };

    inline void SetSnapId( TUint32 aSnapId ) { PrefPtr()->iSnapId = aSnapId; };
    inline TUint32 SnapId() const { return PrefPtr()->iSnapId; };

    inline void SetIapId( TUint32 aIapId ) { PrefPtr()->iIapId = aIapId; };
    inline TUint32 IapId() const { return PrefPtr()->iIapId; };

    inline void SetBearerSet( TUint32 aBearerSet ) { PrefPtr()->iBearerSet = aBearerSet; };
    inline TUint32 BearerSet() const { return PrefPtr()->iBearerSet; };
    
    inline void SetNoteBehaviour( TUint32 aNoteBehaviour ) { PrefPtr()->iNoteBehaviour = aNoteBehaviour; };
    inline TUint32 NoteBehaviour() const { return PrefPtr()->iNoteBehaviour; };

    inline void SetDisconnectDialog( TBool aDisconnectDialog ) { PrefPtr()->iDisconnectDialog = aDisconnectDialog; };
    inline TBool DisconnectDialog() const { return PrefPtr()->iDisconnectDialog; };
    
    inline void SetConnSelectionDialog( TBool aConnSelectionDialog ) { PrefPtr()->iConnSelectionDialog = aConnSelectionDialog; };
    inline TBool ConnSelectionDialog() const { return PrefPtr()->iConnSelectionDialog; };

    inline void SetForcedRoaming( TBool aForcedRoaming ) { PrefPtr()->iForcedRoaming = aForcedRoaming; };
    inline TBool ForcedRoaming() const { return PrefPtr()->iForcedRoaming; };

    inline void SetConnType( TMpmConnPref::EConnType aConnType ) { PrefPtr()->iConnType = (TUint32)aConnType; };
    inline TMpmConnPref::EConnType ConnType() const { return (TMpmConnPref::EConnType)PrefPtr()->iConnType; };

    inline void SetMandateIap( TBool aMandateIap ) { PrefPtr()->iMandateIap = aMandateIap; };
    inline TBool MandateIap() const { return PrefPtr()->iMandateIap; };

    inline void SetNetId( TUint32 aNetId ) { PrefPtr()->iNetId = aNetId; };
    inline TUint32 NetId() const { return PrefPtr()->iNetId; };
    };

// CLASS DECLARATION
/**
*  Mobility Policy Manager server client interface.
*
*  @lib RMPM.lib
*  @since 3.0
*/
class RMPM : public RSessionBase
    {
    public:   // Constructor

        inline RMPM() : RSessionBase(), iConnected( EFalse ),
                        iPtrArray(), iIndex( 0 ) {};

    public: // New functions
 
        /**
         * Deprecated, doesn't do anything
         * @since 3.0
         * @return KErrNone
         */
        IMPORT_C TInt LaunchServer();

        /**
        * Connects client to a server session. It must be called
        * before using other session specific methods.
        * @since 3.0
        * @return KErrNone if successful, otherwise one of the
        * system-wide error codes
        */
        IMPORT_C TInt Connect();

        /**
        * Queries whether MPM is connected.
        * @since 5.2
        * @return ETrue if connected
        */
        IMPORT_C TBool Connected();

        /**
        * Connects client to a server session. It must be called
        * before using other session specific methods.
        * @since 5.2
        * @return KErrNone if successful, otherwise one of the
        * system-wide error codes
        */
        IMPORT_C void Connect( TRequestStatus& aStatus );

        /**
        * Returns MPM server version.
        * @since 3.0
        * @return Version of the MPM server
        */
        IMPORT_C TVersion Version() const;

        /**
        * Cancels asynchronous request. In the risk of deadlock use the SendCancelRequest()
        * @since 3.1
        * @param aReqToCancel request to be canceled
        */
        IMPORT_C void Cancel( TMpmAsynchCodes aReqToCancel ) const;

        /**
        * Asynchronous cancellation
        * @since 5.2
        * @param aReqToCancel request to be canceled
        * @return KErrNone if cancel message sending was successful, otherwise one of the
        * system-wide error codes
        */
        IMPORT_C TInt SendCancelRequest( TMpmAsynchCodes aReqToCancel ) const;

        /**
        * Closes MPM server session.
        * @since 3.0
        */        
        IMPORT_C void Close();
        
        /**
        * Initial select of the most prefered IAP for the connection.
        * @since 3.1
        * @param aConnPref Connection preferences from application 
        * @param aConnId Connection Id
        * @param aAppUid Application UId
        * @param aPolicyPref Connection preferences chosen by MPM
        * @param aStatus KErrNone if successful, otherwise one of the
        * system-wide error codes
        */
        IMPORT_C void ChooseBestIAP( TConnPref&             aConnPref,
                                     const TConnectionId    aConnId,
                                     const TUid             aAppUid,
                                     TConnPref&             aPolicyPref,
                                     TRequestStatus&        aStatus );

        /**
        * Re-selectBestIAP. Same as ChooseBestIAP but can be called multiple times.
        * resubmit of 
        * @since 5.2
        * @param aConnId Connection Id
        * @param aPolicyPref Connection preferences chosen by MPM
        * @param aStatus KErrNone if successful, otherwise one of the
        * system-wide error codes
        */
        IMPORT_C void ReselectBestIAP( const TConnectionId    aConnId,
                                       TConnPref&             aPolicyPref,
                                       TRequestStatus&        aStatus );

        /**
        * Error processing of connection initiation.
        * @since 3.1
        * @param aError Error code
        * @param aConnId Connection Id
        * @param aNeededAction Ignore error, propagate error or perform a reselection
        * @param aStatus KErrNone if successful, otherwise one of the
        * system-wide error codes
        */
        IMPORT_C void ProcessError( TInt&                   aError,
                                    const TConnectionId     aConnId,
                                    TBMNeededAction&        aNeededAction,
                                    TRequestStatus&         aStatus );

        /**
        * Error processing of connection initiation.
        * @deprecated In 5.2, aConnPref not required.
        * @since 3.1
        * @param aConnPref Connection preferences
        * @param aError Error code
        * @param aConnId Connection Id
        * @param aNeededAction Ignore error, propagate error or perform a reselection
        * @param aStatus KErrNone if successful, otherwise one of the
        * system-wide error codes
        */
        IMPORT_C void ProcessError( TConnPref&              aConnPref,
                                    TInt&                   aError,
                                    const TConnectionId     aConnId,
                                    TBMNeededAction&        aNeededAction,
                                    TRequestStatus&         aStatus );

        /**
        * Application leaves connection.
        * @since 3.1
        * @param aIapId IAP Id of the connection.
        * @param aConnId Connection Id
        * @return KErrNone if successful, otherwise one of the
        * system-wide error codes
        */
        IMPORT_C TInt ApplicationLeavesConnection( const TUint32       aIapId,
                                                   const TConnectionId aConnId );

        /**
        * The whole IAP connection is stopped.
        * @since 3.1
        * @param aIapId IAP Id of the connection.
        * @param aConnId Connection Id
        * @return KErrNone if successful, otherwise one of the
        * system-wide error codes
        */
        IMPORT_C TInt IAPConnectionStopped( const TUint32       aIapId,
                                            const TConnectionId aConnId );

        /**
        * Bearer manager provides message buffer for MPM notifications,
        * must be called before registering for notifications.
        * @since 3.1
        * @param aMpmNotification notification for BM
        * @param aStatus KErrNone if successful, otherwise one of the
        * system-wide error codes
        */
        IMPORT_C void WaitNotification( TMPMMessageBuffer& aMpmNotification,
                                        TRequestStatus&    aStatus );

        /**
        * Register for getting preferred IAP notifications.
        * @since 3.1
        * @param aConnId Connection Id
        * @return KErrNone if successful, otherwise one of the
        * system-wide error codes
        */
        IMPORT_C TInt RegisterPrefIAPNotif( const TConnectionId aConnId );

        /**
        * Register for getting preferred IAP notifications.
        * @since 5.2
        * @param aConnId Connection Id
        * @param aStatus KErrNone if successful, otherwise one of the
        * system-wide error codes
        */
        IMPORT_C void RegisterPrefIAPNotif( const TConnectionId aConnId,
                                            TRequestStatus&     aStatus );
        /**
        * Register for getting preferred IAP notifications.
        * @deprecated In 5.2, aConnPref not required.
        * @since 3.1
        * @param aConnPref Connection preferences
        * @param aConnId Connection Id
        * @return KErrNone if successful, otherwise one of the
        * system-wide error codes
        */
        IMPORT_C TInt RegisterPrefIAPNotif( const TConnPref&    aConnPref,
                                            const TConnectionId aConnId );

        /**
        * Unregister preferred IAP notifications.
        * @since 3.1
        * @param aConnId Connection Id
        */
        IMPORT_C void UnregisterPrefIAPNotif( const TConnectionId aConnId );

        /**
        * Unregister preferred IAP notifications.
        * @since 5.2
        * @param aConnId Connection Id
        */
        IMPORT_C void UnregisterPrefIAPNotif( const TConnectionId aConnId,
                                              TRequestStatus&     aStatus );

        /**
        * Sort available IAPs belonging to the specified SNAP.
        * @since 3.2
        * @param aSNAP Service Network Access Point
        * @param aSortedList on return contains the sorted list of IAPs
        * @return KErrNone if successful, otherwise one of the
        * system-wide error codes
        */
        IMPORT_C TInt SortSNAP( const TUint32   aSNAP, 
                                TMpmSnapBuffer& aSortedList );

        /**
        * Application joins connection.
        * @since 3.2
        * @param aIapId IAP Id of the connection.
        * @param aConnId Connection Id
        * @return KErrNone if successful, otherwise one of the
        * system-wide error codes
        */                
        IMPORT_C TInt ApplicationJoinsConnection( const TUint32       aIapId,
                                                  const TConnectionId aConnId );

        /**
        * The whole IAP connection is activated.
        * @since 3.2
        * @param aIapId IAP Id of the connection.
        * @param aConnId Connection Id
        * @return KErrNone if successful, otherwise one of the
        * system-wide error codes
        */        
        IMPORT_C TInt IAPConnectionActivated( const TUint32       aIapId,
                                              const TConnectionId aConnId );

        /**
        * The whole IAP connection is started.
        * @since 3.2
        * @param aIapId IAP Id of the connection.
        * @param aConnId Connection Id
        * @return KErrNone if successful, otherwise one of the
        * system-wide error codes
        */        
        IMPORT_C TInt IAPConnectionStarted( const TUint32       aIapId,
                                            const TConnectionId aConnId );

        /**
        * Application connection ends.
        * @since 3.2
        * @param aConnId Connection Id
        * @return KErrNone if successful, otherwise one of the
        * system-wide error codes
        */
        IMPORT_C TInt ApplicationConnectionEnds( 
                                        const TConnectionId aConnId ); 

        /**
        * Application migrates to carrier.
        * @since 3.2
        * @param aIapId IAP Id of the new carrier.
        * @param aConnId Connection Id
        * @return KErrNone if successful, otherwise one of the
        * system-wide error codes
        */                
        IMPORT_C TInt ApplicationMigratesToCarrier( const TUint32       aIapId,
                                                    const TConnectionId aConnId );

        /**
        * Application ignored the carrier.
        * @since 3.2
        * @param aIapId IAP Id of the new carrier.
        * @param aConnId Connection Id
        * @return KErrNone if successful, otherwise one of the
        * system-wide error codes
        */                
        IMPORT_C TInt ApplicationIgnoredTheCarrier( const TUint32       aIapId,
                                                    const TConnectionId aConnId );

        /**
        * Application accepted the carrier.
        * @since 3.2
        * @param aIapId IAP Id of the new carrier.
        * @param aConnId Connection Id
        * @return KErrNone if successful, otherwise one of the
        * system-wide error codes
        */                
        IMPORT_C TInt ApplicationAcceptedTheCarrier( const TUint32       aIapId,
                                                     const TConnectionId aConnId );

        /**
        * Application rejected the carrier.
        * @since 3.2
        * @param aIapId IAP Id of the new carrier.
        * @param aConnId Connection Id
        * @return KErrNone if successful, otherwise one of the
        * system-wide error codes
        */                
        IMPORT_C TInt ApplicationRejectedTheCarrier( const TUint32       aIapId,
                                                     const TConnectionId aConnId );

        /**
        * Displays connecting wait note for Easy WLAN connection.
        * @since 3.2
        * @param aSSID the id of selected wlan network
        */                
        IMPORT_C TInt EasyWlanSsid( const TWlanSsid& aSSID );

        /**
        * The whole IAP connection is started.
        * @since 5.2
        * @param aIapId IAP Id of the connection.
        * @param aConnId Connection Id
        * @param aStatus KErrNone if successful, otherwise one of the
        * system-wide error codes
        */        
        IMPORT_C void IAPConnectionStarted( const TUint32       aIapId,
                                            const TConnectionId aConnId,
                                            TRequestStatus&     aStatus );
                                            
        /**
        * The whole IAP connection is stopped.
        * @since 5.2
        * @param aIapId IAP Id of the connection.
        * @param aConnId Connection Id
        * @param aStatus KErrNone if successful, otherwise one of the
        * system-wide error codes
        */
        IMPORT_C void IAPConnectionStopped( const TUint32       aIapId,
                                            const TConnectionId aConnId,
                                            TRequestStatus&     aStatus );

        /**
        * Application joins connection.
        * @since 5.2
        * @param aIapId IAP Id of the connection.
        * @param aConnId Connection Id
        * @param aStatus KErrNone if successful, otherwise one of the
        * system-wide error codes
        */                
        IMPORT_C void ApplicationJoinsConnection( const TUint32       aIapId,
                                                  const TConnectionId aConnId,
                                                  TRequestStatus&     aStatus );
        /**
        * Application leaves connection.
        * @since 5.2
        * @param aIapId IAP Id of the connection.
        * @param aConnId Connection Id
        * @param aStatus KErrNone if successful, otherwise one of the
        * system-wide error codes
        */
        IMPORT_C void ApplicationLeavesConnection( const TUint32       aIapId,
                                                   const TConnectionId aConnId,
                                                   TRequestStatus&     aStatus );

        /**
        * Application connection ends.
        * @since 5.2
        * @param aConnId Connection Id
        * @param aStatus KErrNone if successful, otherwise one of the
        * system-wide error codes
        */
        IMPORT_C void ApplicationConnectionEnds( const TConnectionId aConnId,
                                                 TRequestStatus&     aStatus );

        /**
        * Application migrates to carrier.
        * @since 3.2
        * @param aIapId IAP Id of the new carrier.
        * @param aConnId Connection Id
        * @param aStatus KErrNone if successful, otherwise one of the
        * system-wide error codes
        */                
        IMPORT_C void ApplicationMigratesToCarrier( const TUint32       aIapId,
                                                    const TConnectionId aConnId,
                                                    TRequestStatus&     aStatus );

        /**
        * Application ignored the carrier.
        * @since 3.2
        * @param aIapId IAP Id of the new carrier.
        * @param aConnId Connection Id
        * @param aStatus KErrNone if successful, otherwise one of the
        * system-wide error codes
        */                
        IMPORT_C void ApplicationIgnoredTheCarrier( const TUint32       aIapId,
                                                    const TConnectionId aConnId,
                                                    TRequestStatus&     aStatus );

        /**
        * Application rejected the carrier.
        * @since 3.2
        * @param aIapId IAP Id of the rejected carrier.
        * @param aConnId Connection Id
        * @param aStatus KErrNone if successful, otherwise one of the
        * system-wide error codes
        */                
        IMPORT_C void ApplicationRejectedTheCarrier( const TUint32       aIapId,
                                                     const TConnectionId aConnId,
                                                     TRequestStatus&     aStatus );

        /**
        * Application accpeted the carrier.
        * @since 3.2
        * @param aIapId IAP Id of the accepted carrier.
        * @param aConnId Connection Id
        * @param aStatus KErrNone if successful, otherwise one of the
        * system-wide error codes
        */                
        IMPORT_C void ApplicationAcceptedTheCarrier( const TUint32       aIapId,
                                                     const TConnectionId aConnId,
                                                     TRequestStatus&     aStatus );

        /**
        * Asynchronously sort available IAPs belonging to the specified SNAP.
        * @since 5.2
        * @param aSNAP Service Network Access Point
        * @param aSortedList on return contains the sorted list of IAPs
        * @param aStatus KErrNone if successful, otherwise one of the
        * system-wide error codes
        * @param aMaxAgeSeconds how many seconds old availability data MPM is
        * allowed to use for priorization.
        */
        IMPORT_C void SortSNAP( const TUint32   aSNAP, 
                                TMpmSnapBuffer& aSortedList,
                                TRequestStatus& aStatus,
                                const TUint32   aMaxAgeSeconds = 5 );

#ifdef _DEBUG
        /**
         * Sends calls into MPM. Only for debugging purposes. Must not be compiled for UREL 
         */
        inline void SendReq( TMpmAsynchCodes aReq, TInt aValue );
#endif //_DEBUG
    private: // New functions

        TPtr8& NextPtr();       

    private: // Data
    
        // Is Client connected
        TBool iConnected;

        // Ptr array for IPC messaging
        RArray< TPtr8 > iPtrArray;

        // Used for indexing iPtrArray elements
        TInt iIndex;
    };

#include "rmpm.inl"

#endif // RMPM_H

// End of File
