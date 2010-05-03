/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*        Declaration of WLAN Coverage Check class
*
*/

#ifndef CMWLANCOVERAGECHECK_H
#define CMWLANCOVERAGECHECK_H

// INCLUDES
#include <e32base.h>
#include <cmpluginwlandef.h>

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

        TBool GetCoverageL();

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

    protected:  // from CActive

        virtual void DoCancel();
        virtual void RunL();

    private:

        TUint32                 iProgState;
        TBool                   iCoverage;
        CActiveSchedulerWait    iWait;
    };


#endif // CMWLANCOVERAGECHECK_H
