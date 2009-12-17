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
* Description:  Declaration of WLAN Coverage Check class
*
*/

#ifndef CMWLANCOVERAGECHECK_H
#define CMWLANCOVERAGECHECK_H

// INCLUDES
#include <e32base.h>
#include <cmpluginwlandef.h>


// FORWARD DECLARATION
class CCmPluginWlanDataArray;
 
using namespace CMManager;

/**
 *  WLAN Coverage Check
 *
 *  ?more_complete_description
 *
 *  @since S60 3.2
 */
class CCmWlanCoverageCheck : public CActive
    {
    enum EWlanCoverageState
        {
        EServiceStatus,
        EScanning,
        EDone,                        
        };
        
    public:
    
        CCmWlanCoverageCheck();
        ~CCmWlanCoverageCheck();
        
        TInt GetCoverageL();

        TBool ScanForPromptedSsidL( TWlanSsid aEnteredSsid, TBool aBroadcast,
                                TWlanNetMode& aNetworkMode,
                                TWlanConnectionExtentedSecurityMode& aSecurityMode,
                                TBool& aProtectedSetupSupported );

        /**
        * Returns whether the passed ssid and length results in a hidden or 
        * a non-hidden ssid.
        *
        * @since S60 3.2
        * @param aSsidLength The length of the ssid
        * @param aSsid The ssid
        * @return ETrue if the ssid is hidden.
        */
        TBool IsHiddenSsid( TUint aSsidLength, const TUint8* aSsid ) const;
        
        /**
        * Adds the passed WLAN network data to the container array
        * if it is not already there, and sets the highest signal 
        * strength if it is already in the array.
        *
        * @since S60 3.2
        * @param aArray The array to be checked
        * @param aRXLevel The signal level of this WLAN
        * @param aNetworkName The network name (SSID) of this WLAN.
        */
        void AddToArrayIfNeededL( 
                            CCmPluginWlanDataArray& aArray, 
                            TInt aRXLevel, 
                            TDesC& aNetworkName ) const;
                
                
    protected:  // from CActive
    
        virtual void DoCancel();
        virtual void RunL();

    private:

        TUint32                 iProgState;
        TInt                    iCoverage;
        CActiveSchedulerWait    iWait;    
    };


#endif // CMWLANCOVERAGECHECK_H
