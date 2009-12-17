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

#ifndef WLANCOVERAGECHECK_H
#define WLANCOVERAGECHECK_H

// INCLUDES
#include <e32base.h>
//#include <cmpluginwlandef.h>



/**
 *  WLAN Coverage Check
 *
 *  ?more_complete_description
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CWlanCoverageCheck ) : public CActive
    {
    enum EWlanCoverageState
        {
        EServiceStatus,
        EScanning,
        EDone,                        
        };
        
    public:
    
        CWlanCoverageCheck();
        ~CWlanCoverageCheck();

        TBool ScanForPromptedSsidL( TWlanSsid aEnteredSsid, TBool aBroadcast,
                                TWlanConnectionMode& aNetworkMode,
                                TWlanConnectionSecurityMode& aSecurityMode,
                                TWlanConnectionExtentedSecurityMode& aExtSecurityMode,
                                TBool& aProtectedSetupSupported );
        
    protected:  // from CActive
    
        virtual void DoCancel();
        virtual void RunL();

    private:
        TUint32                 iProgState;
        CActiveSchedulerWait    iWait;    
    };


#endif // WLANCOVERAGECHECK_H
