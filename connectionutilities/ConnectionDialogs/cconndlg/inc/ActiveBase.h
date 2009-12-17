/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of a CActive base class 
*
*/



#ifndef __ACTIVEBASE_H__
#define __ACTIVEBASE_H__


// INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
 * CActiveBase class, an active object for selection plugin
 */     
NONSHARABLE_CLASS( CActiveBase ) : public CActive
    {
    public:
        /**
        * NewL function
        */
        static CActiveBase* NewL();
    
        /**
        * Destructor
        */    
        virtual ~CActiveBase();
    
        /**
        * Cancels the active object
        */
        void Cancel();

        /**
        * Timer callback of iPeridoc.
        */
        static TInt Tick( TAny* aObject );    
    
        
    protected:
        /**
        * ConstructL 
        */
        void ConstructL();
    
        /**
        * Constructor
        */    
        CActiveBase();
    
        /**
        * DoCancel from CActive
        */    
        virtual void DoCancel();

        /**
        * RunL from CActive
        */
        virtual void RunL();

        /**
        * Starts Timer
        * @param aTickInterval refersh interval
        */
        void StartTimerL( TInt aTickInterval );
    
        /**
        * Stops Timer
        */
        void StopTimer();                                            
        

    protected:
    
        // Timer, to refresh iaps of networks. Owned.
        CPeriodic* iPeriodic;
            
        // used for complete runl, not owned.
        TRequestStatus* iClientStatus;  
    };     


#endif

// End of File
