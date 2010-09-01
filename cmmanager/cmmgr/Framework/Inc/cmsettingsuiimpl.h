/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of setting UI
*
*/

#ifndef C_CMSETTINGSUIIMPL_H
#define C_CMSETTINGSUIIMPL_H

#include <e32def.h>
#include <e32base.h>
#include <ConeResLoader.h>

class CCmManagerImpl;

/**
 *  Implementation of setting UI
 *
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CCmSettingsUiImpl) : public CBase
    {
    public:

        /** Epoc constructor */
        static CCmSettingsUiImpl* NewL();

        /** Destructor */
        virtual ~CCmSettingsUiImpl();

    //=========================================================================    
    // UI API
    //
    public:

        /**
        * Launches the Settings UI 
        * for handling Destinations and Connection Methods
        * @since 3.2
        */
        TInt RunSettingsL();
        
        /**
        * Launches a dialog with a list of Destination
        * @since 3.2
        * @param  aDestinationId ID of the selected Destination
        * @return TBool
        */
        TBool SelectDestinationDlgL( TUint32& aDestinationId );
        
        /**
        * Launches the dialog for adding connection methods
        * @since 3.2.3
        * @param  aDestUid destination id for the new cm
        * @param  aBearerType bearer type for the new cm
        * @return TUint32 id of the new cm
        */
        TUint32 AddCmL( TUint32& aDestUid, TUint32 aBearerType );

        /**
        * Launches the dialog for editing connection methods
        * @since 3.2.3
        * @param  aCmId id of the cm to edit
        * @return TInt - KDialogUserExit or KDialogUserBack
        */
        TInt EditCmL( TUint32 aCmId );


    private:

        /** First stage constructor */
        CCmSettingsUiImpl();

        /** Epoc constructor */
        void ConstructL();

        /** Load resource */
        TInt LoadResourceL ();

    private: // data

        CCmManagerImpl* iCmManagerImpl;
        RConeResourceLoader iResourceReader;
    };

#endif // C_CMSETTINGSUIIMPL_H
