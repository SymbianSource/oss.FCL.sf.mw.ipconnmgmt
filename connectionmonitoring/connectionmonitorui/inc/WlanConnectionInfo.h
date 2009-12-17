/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Represents a WLAN connection
*     
*
*/


#ifndef WLANCONNECTIONINFO_H_INCLUDED
#define WLANCONNECTIONINFO_H_INCLUDED

// INCLUDES
#include "ConnectionInfoBase.h"

// CONSTANTS

// FORWARD DECLARATIONS
class RConnectionMonitor;

// Calibration of WLAN signal strength
enum TWlanSignalStrength
    {
    EWlanSignalStrengthMax = 60,
    EWlanSignalStrengthGood = 74,
    EWlanSignalStrengthLow = 87,
    EWlanSignalStrengthMin = 100,
    EWlanSignalUnavailable = 9999
    };

// WLAN Network modes
enum TWlanNetworkMode
    {
    EWlanNetworkModeInfrastructure = 0,
    EWlanNetworkModeAdHoc,
    EWlanNetworkModeSecInfrastructure,
    EWlanNetworkModeUnavailable
    };

// WLAN security modes
enum TWlanSecurityMode
    {
    EWlanSecurityModeOpenNetwork = 0,
    EWlanSecurityModeWEP,
    EWlanSecurityMode8021x,
    EWlanSecurityModeWPA,
    EWlanSecurityWpaPsk,
    EWlanSecurityModeUnavailable
    };

// CONSTANTS

const TReal32 KDiffOfWlanSignalStrength = 
                            EWlanSignalStrengthMin - EWlanSignalStrengthMax;


// CLASS DECLARATION
/**
* Represents a WLAN connection.
*/
class CWlanConnectionInfo : public CConnectionInfoBase
    {
    public:
        /**
        * Static constructor
        * @param aConnectionId The id of the given connection.
        * @param aMonitor for refreshing details
        * @param aConnectionBearerType the bearer type of connection
        * @param aEasyWlanIAPName Name of the Easy WLAN access point
        * @param aActiveWrapper the for refreshing
        */
        static CWlanConnectionInfo* NewL( TInt aConnectionId, 
                              RConnectionMonitor* const aConnectionMonitor,
                              TConnMonBearerType aConnectionBearerType,
                              const HBufC* aEasyWlanIAPName,
                              CActiveWrapper* aActiveWrapper );

        /**
        * Destructor
        */
        virtual ~CWlanConnectionInfo();
        
    public:    
        /**
        * Give back a copy of connection. 
        */
        virtual CConnectionInfoBase* DeepCopyL();
    
        /**
        * Refreshing all details of connection. Leaves if refreshing 
        * any of detais is failed.
        */
        virtual void RefreshDetailsL();

        /**
        * Refresh iListBoxItemText text, connection ListBox has a 
        * reference to this text.
        */
        virtual void RefreshConnectionListBoxItemTextL();

        /**
        * Refresh details array of connection. DetailsListBox has a 
        * reference to this array.
        */
        virtual void RefreshDetailsArrayL();

        /**
        * Gives back the icon id of connection 
        * @return the icon id of connection 
        */
        virtual TUint GetIconId() const;

        /**
        * Gives back the connection is EasyWLAN or not.
        * @return ETrue if the selected conenction EasyWLAN,
        * EFalse otherwise
        */
        virtual TBool IsEasyWLAN() const;
        
        /**
        * Gives back current Name of Network ( SSID )
        * @return iNetworkName
        */
        virtual HBufC* GetWlanNetworkNameLC() const;

        /**
        * Gives back current SignalStrength
        * @return iSignalStrength
        */        
        TWlanSignalStrength GetWlanSignalStrength() const;

        /**
        * Gives back current NetworkMode
        * @return iNetworkMode
        */
        TWlanNetworkMode GetWlanNetworkMode() const;

        /**
        * Gives back current SecurityMode
        * @return iSecurityMode
        */        
        TWlanSecurityMode GetWlanSecurityMode() const;

        /**
        * Gives back current TxPower
        * @return iTxPower
        */        
        TUint GetWlanTxPower() const;

    protected:  // Constructors
        /**
        * Constructor
        * @param aConnectionId The id of the given connection.
        * @param aMonitor for refreshing details
        * @param aConnectionBearerType the bearer type of connection
        * @param aActiveWrapper the for refreshing
        */
        CWlanConnectionInfo( TInt aConnectionId, 
                             RConnectionMonitor* const aConnectionMonitor,
                             TConnMonBearerType aConnectionBearerType,
                             CActiveWrapper* aActiveWrapper );

        /**
        * Second phase construction
        */
        void ConstructL();

    protected:
        /**
        * Creates an array with requrired details of connection to
        * the listbox which visulaize details of connection. This member
        * is called at initialization.
        */
        virtual void ToArrayDetailsL();

        /**
        * Refreshing NetworkName. Leaves if did not get valid data from
        * ConnectionMonitor.
        */
        void RefreshNetworkNameL();
        
        /**
        * Refreshing SignalStrength. Leaves if did not get valid data from
        * ConnectionMonitor.
        */
        void RefreshSignalStrength();

        /**
        * Refreshing NetworkMode. Leaves if did not get valid data from
        * ConnectionMonitor.
        */
        void RefreshNetworkMode();

        /**
        * Refreshing SecurityMode. Leaves if did not get valid data from
        * ConnectionMonitor.
        */
        void RefreshSecurityMode();

        /**
        * Refreshing TxPower. Leaves if did not get valid data from
        * ConnectionMonitor.
        */
        void RefreshTxPowerL();

        /**
        * Gives back the string representation of SignalStrength.
        */
        HBufC* ToStringSignalStrengthLC() const;

        /**
        * Gives back the string representation of Mode.
        */
        HBufC* ToStringNetworkModeLC() const;

        /**
        * Gives back the string representation of Security.
        */
        HBufC* ToStringSecurityModeLC() const;

        /**
        * Gives back the string representation of TxPower.
        */
        HBufC* ToStringTxPowerLC() const;

    private:

    protected:

        /**
        * The name of network (SSID)
        */
        TName                       iWlanNetworkName;
        
        /**
        * Signale strength of WLAN connection
        */
        TWlanSignalStrength         iSignalStrength;
        
        /**
        * Network mode of WLAN conenction
        */ 
        TWlanNetworkMode            iNetworkMode;
    
        /**
        * Security mode of WLAN conenction
        */ 
        TWlanSecurityMode           iSecurityMode;

        /**
        * TxPower of WLAN conenction
        */
        TUint                        iTxPower;
        
        /**
        * ETrue if the selected conenction EasyWLAN
        */ 
        TBool                        iEasyWlan;
    };

#endif // WLANCONNECTIONINFO_H_INCLUDED
//End of File.