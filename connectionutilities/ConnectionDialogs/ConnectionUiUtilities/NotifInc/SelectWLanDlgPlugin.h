/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of CSelectWLanDlgPlugin
*
*/



#ifndef __SELECT_WLAN_DLG_PLUGIN_H__
#define __SELECT_WLAN_DLG_PLUGIN_H__


// INCLUDES
#include "ConnectionDialogsNotifBase.h"
#include "ConnectionUiUtilitiesCommon.h"

//FORWARD DECLARATION
class CActiveSelectWLanDlgPlugin;
/**
 * SelectWLanDlg Plugin class
 */
NONSHARABLE_CLASS( CSelectWLanDlgPlugin ) : public CConnectionDialogsNotifBase
    {   
    public:

        /**
        * NewL function
        * @param  -
        * return CSelectWLanDlgPlugin*
        */
        static CSelectWLanDlgPlugin* NewL( 
                                        const TBool aResourceFileResponsible );
        
        /**
        * RegisterL register the client notifier function
        * @param  -
        * return TNotifierInfo
        */
        TNotifierInfo RegisterL();

        /**
        * Start the Notifier
        * @param  aBuffer    Buffer
        * @param  aReplySlot Identifies which message argument to use for the 
        *                    reply. This message argument will refer to a 
        *                    modifiable descriptor, a TDes8 type, into which
        *                    data can be returned. 
        * @param  aMessage   Message
        * return -
        */
        void StartL( const TDesC8& aBuffer, TInt aReplySlot, 
                     const RMessagePtr2& aMessage );

        /**
        * Cancel() the notifier
        * @param  -
        * return -
        */
        void Cancel();
        
        /**
        * Stops Timer of iActivePlugin
        */
        void StopTimer();    
        
        /**
        * Sets value of the iNetworkPrefs member
        * @param aSSID the id of selected wlan network
        * @param aConnectionMode connection mode of selected wlan network
        * @param aSecurityMode security mode of selected wlan network
        * @param aExtSecurityMode extended security mode of selected wlan network
        */
        void SetNetworkPrefsL( TWlanSsid& aSSID,
                               TWlanConnectionMode& aConnectionMode,
                               TWlanConnectionSecurityMode& iSecurityMode,
                               TWlanConnectionExtentedSecurityMode& aExtSecurityMode,
                               TBool& aProtectedSetupSupported );
                
    protected:
        /**
        * Constructor
        */        
        CSelectWLanDlgPlugin();

    public:
        /**
        * CompleteL the notifier is complete
        * @param  aStatus status
        * return  -
        */
        void CompleteL( TInt aStatus );

        /**
        * To inform that Other (hidden netw.) was selected.
        * @param  aValue value (ETrue/EFalse) for EWlanScanSSID. 
        */
        void SetEasyWlanToHiddenL( const TBool aValue );   

    private:

        // return value of NetworkPrefs
        TPckgBuf<TConnUiUiWlanNetworkPrefs> iNetworkPrefs;

        // owned  
        CActiveSelectWLanDlgPlugin* iActivePlugin;
    };
    
#endif // __SELECT_WLAN_DLG_PLUGIN_H__    

// End of File
