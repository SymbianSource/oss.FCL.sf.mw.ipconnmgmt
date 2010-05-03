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
* Description:
*
*/



#ifndef __ACTIVE_WRAPPER_H__
#define __ACTIVE_WRAPPER_H__

//  INCLUDES
#include <e32base.h>
#include "ConnectionUiUtilitiesClient.h"


// CONSTANTS
const TUint KNumberOfWrappedDialogs = 7;

enum TWrappedDialogs
    {
    ESearchWlan = 0, 
    EOffLineWlan,
    EWlanDisabled,
    EWlanNetwUnavail,
    EWlanEasyWep,
    EWlanEasyWpa,
    EWlanEasyWapi
    };



// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
NONSHARABLE_CLASS( CActiveWrapper ) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CActiveWrapper* NewL( const TUint aIndexOfNote );
        
        /**
        * Destructor.
        */
        virtual ~CActiveWrapper();

    public: // New functions
        
        void StartSearchWLANNetwork( 
                        TPckgBuf<TConnUiUiWlanNetworkPrefs>& aNetworkPrefs );

        void StartSearchWLANNetworkAsync( TRequestStatus& aStatus, 
                                TWlanSsid& aSSID,
                                TWlanConnectionMode& aConnectionMode,
                                TWlanConnectionSecurityMode& aSecurityMode,
                                TWlanConnectionExtentedSecurityMode& aExtSecurityMode, 
                                TBool& aProtectedSetupSupported );

        void StartGenericNote( const TUint aIndexOfNote, 
                               const TUint32 aConnMId  = 0,
                               const TUint32 aDestId = 0 );

        TInt StartEasyWepDlg( TPckgBuf< TWepKeyData >& aKey  );

        void StartEasyWepDlgAsync( TRequestStatus& aStatus, TDes* aKey, TBool& aHex );
    
        TInt StartEasyWpaDlg( TPckgBuf< TBuf< KEasyWpaQueryMaxLength > >& aKey );
    
        void StartEasyWpaDlgAsync( TRequestStatus& aStatus, TDes* aKey );
    
        TInt StartEasyWapiDlg( TPckgBuf< TBuf< KEasyWapiQueryMaxLength > >& aKey );

    public: // Functions from base classes

        void DoCancel();
        void RunL();
        
    protected:  // New functions
        

    protected:  // Functions from base classes
        

    private:

        /**
        * C++ default constructor.
        */
        CActiveWrapper( const TUint aIndexOfNote );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    protected:  // Data

        RConnectionUiUtilitiesSession iNotif; ///< notifier client
        
        CActiveSchedulerWait    iWait;

        TPckgBuf<TConnUiUiWlanNetworkPrefs> iNetworkPrefs;
        TWlanSsid* iSSID;
        TWlanConnectionMode* iConnectionMode;
        TWlanConnectionSecurityMode* iSecurityMode;
        TWlanConnectionExtentedSecurityMode* iExtSecurityMode;
        TBool* iProtectedSetupSupported; 

        TRequestStatus* iRS;
        TUint iIndexOfNote;
                
        
        TPckgBuf< TWepKeyData > iWepKey;//used only for async version
        
        TPckgBuf< TBuf< KEasyWpaQueryMaxLength > > iWpaKey;//used only for async version
        
        TDes* iKey;
        TBool* iHex;
        
    };

#endif      // __ACTIVE_WRAPPER_H__
            
// End of File
