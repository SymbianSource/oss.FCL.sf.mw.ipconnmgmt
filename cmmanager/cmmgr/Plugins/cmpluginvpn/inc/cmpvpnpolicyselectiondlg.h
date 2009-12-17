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
* Description:  Displays a selection list of VPN policies
*
*/

#ifndef CMPLUGINVPN_POLICYSELECTION_DLG_H
#define CMPLUGINVPN_POLICYSELECTION_DLG_H

// includes
#include <e32base.h>
#include <ConeResLoader.h>
#include <vpnapi.h>

// forward declarations
class CCmPluginBaseEng;

/**
 *  CmPluginPacketDataSettingsDlg dialog class
 *
 *  
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CmPluginVpnPolicySelectionDlg ) : public CBase
    {
    public: // Constructors and destructor
    
        static CmPluginVpnPolicySelectionDlg* NewL( 
                                        CCmPluginBaseEng& aCmPluginBaseEng );
        
        /**
        * Destructor.
        */
        virtual ~CmPluginVpnPolicySelectionDlg();
        
    public: // New functions
    
        /**
        * Shows the VPN policies in and a popup list and if a selection is
        * made, EVpnServicePolicyName is set
        * 
        * @since S60 3.2
        * @return ETrue if a selection was made
        */
        TBool ShowPolicySelectionListL();
        
        /**
        * Shows the VPN policies in and a radio button list and if a selection is
        * made, EVpnServicePolicyName is set
        * 
        * @since S60 3.2
        * @return ETrue if a selection was made
        */
        TBool ShowPolicySelectionDlgL();

    private: // Constructor
    
        /**
        * Constructor.
        */      
        CmPluginVpnPolicySelectionDlg( CCmPluginBaseEng& aCmPluginBaseEng );
        
        /**
        * Second phase constructor. Leaves on failure.
        */      
        void ConstructL();
    
    private: // New functions
            
        /**
        * Loads a resource file.
        *
        * @since S60 3.2
        * @param aResFileName the resource file name.
        */
        void LoadResourceL( const TDesC& aResFileName );
        
        /**
        * Pop-up note that VPN client is inaccessible
        *
        * @since S60 3.2
        */
        void VpnClientInaccessibleL();
        
    private: // data members
        
        /**
         * base class reference, NOT OWNED
         */
        CCmPluginBaseEng&   iCmPluginBaseEng;
        
        /**
         * resource reader
         */
        RConeResourceLoader iResourceReader;
        
        /**
         * VPN server
         */
        RVpnServ iVpnServ;
        
        /**
         * array of VPN policies - OWNED
         */
        CArrayFixFlat<TVpnPolicyInfo>* iPolicyInfoList;
        
        /**
         * flag for when vpn is not accessible. used to make sure the 
         * dialog indicating it is inaccessible is not displayed both 
         * on connect and close
         */
        TBool iVpnClientUnaccessible;
        
        /**
        * the number of VPN policies
        */
        TInt iNumPolicies;
    };

#endif // CMPLUGINVPN_POLICYSELECTION_DLG_H