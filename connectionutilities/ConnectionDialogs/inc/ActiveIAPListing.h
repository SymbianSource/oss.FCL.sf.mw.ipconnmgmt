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
* Description:  Declaration of Active Iap Listing class
*
*/



#ifndef __ACTIVEIAPLISTING_H__
#define __ACTIVEIAPLISTING_H__


// INCLUDES
#include <e32base.h>
#include <agentdialog.h>


// FORWARD DECLARATION
class CCommsDatabase;
class CConnectionInfoArray;
class CAknWaitDialog;
#ifndef __WINS__
class CWlanMgmtClient;
#endif


enum TLanType
    {
    ENotDefined,
    ELan,
    EEasyWLan,
    EWLan
    };



// CLASS DECLARATION


/**
 * CActiveIAPListing class,
 */     
NONSHARABLE_CLASS( CActiveIAPListing ) : public CActive
    {
    public:
        /**
        * StartSearchIAPsL setactive and starts active object
        */    
        void BaseStartSearchIAPsL( TInt aResourceId );

        /**
        * Destructor
        */    
        virtual ~CActiveIAPListing();
    
        /**
        * Timer callback of iPeridoc.
        */
        static TInt Tick( TAny* aObject );    
    
        /**
        * Cancels the active object
        */
        void Cancel();
        
        /**
        * Gives back, refresh interval of scan for network
        */
        TInt GetRefreshInterval( const TUid aRepositoryUid, TUint32 aKey );

              
    protected:
        /**
        * ConstructL 
        */
        void BaseConstructL();
    
        /**
        * Constructor
        * @param  aPlugin a pointer to notifier plugin    
        * @param aIAPId iap id
        * @param aPrefs preferences pointer        
        */    
        CActiveIAPListing( const TConnectionPrefs& aPrefs );
    
        /**
        * Starts Timer
        * @param aTickInterval refersh interval
        */
        void StartTimerL( TInt aTickInterval );
    
        /**
        * Stops Timer
        */
        void StopTimer();                                            
        
        /**
        * DoCancel from CActive
        */    
        virtual void DoCancel();

        /**
        * RunL from CActive
        */
        virtual void RunL();
  
        void CreateArraysL( TInt aResourceId, TBool aChangeConnection, 
                            TUint32 aIAPId );


        /**
        * BuildConnectionListL
        * @param    -
        */
        void BuildConnectionListL( TInt aResourceId, TBool aChangeConnection, 
                                   TUint32 aIAPId );


        /**
        * Get the type of LAN of the access point.
        * @param aServiceId The Service ID of the access point
        * @param aIsInfrastructure  Output, it tells if the connection mode is 
        *                           'Infrastructure'
        * @return one of the TLanType enumerations
        */
        TLanType GetTypeOfLanL( TUint32 aServiceId, TBool& aIsInfrastructure );

        
        /**
        * Merges iWlanIAPs and iAvailableIaps
        */
        void MergeArraysL( TInt aResourceId );
    
        /**
        * SortNameListL
        * @param -
        */
        void SortNameListL( TInt aResourceId );

    protected:
    
        // Pointer to Comms database owned
        CCommsDatabase* iDb;
    
        // Timer, to refresh iaps of networks. Owned.
        CPeriodic* iPeriodic;
            
        CAknWaitDialog* iWaitDialog;    
              
#ifndef __WINS__
    
        // used for wlan networks info, owned
        CWlanMgmtClient* iWlanMgmt;
    
#endif // !__WINS__

        // contains all iaps from db except wlan iaps, owned
        CConnectionInfoArray *iIAPs;   

        // contains all wlan iaps from db, owned
        CConnectionInfoArray *iWlanIAPs;   
    
        // all available iaps, owned
        CConnectionInfoArray *iActIAPs;   
    
        // Available iaps from WlanMgmnt
        RArray<TUint> iAvailableIaps;
    
        // used for complete runl, not owned.
        TRequestStatus* iClientStatus;  
    
        TBool iIsWLANFeatureSupported;
    
        // Selected preferences
        TConnectionPrefs iPrefs;        
    };     


#endif

// End of File
