/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CActiveWrapper header file
*
*/


#ifndef __ACTIVE_WRAPPER_H__
#define __ACTIVE_WRAPPER_H__

//  INCLUDES
#include <e32base.h>
#include <rconnmon.h>

// CONSTANTS

// CLASS DECLARATION

/**
*  Definition of CActiveWrapper
*
*  @since S60 v3.2
*/
NONSHARABLE_CLASS( CActiveWrapper ) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CActiveWrapper* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CActiveWrapper();

    public: // New functions

        /**
        * Starts the active object and gets the value for a string attribute.
        * @param aConnectionId Connection id.
        * @param aConnectionMonitor RConnectionMonitor
        * @param aAttribute Identifies the attribute to be retrived.
        * @param aValue On completion, contains the requested string.
        */
        void StartGetStringAttribute( TInt const aConnectionId, 
                            RConnectionMonitor* const aConnectionMonitor,
                            TUint const aAttribute,
                            TDes& aValue );

        /**
        * Starts the active object and gets the value for a TInt attribute.
        * @param aConnectionId Connection id.
        * @param aConnectionMonitor RConnectionMonitor
        * @param aAttribute Identifies the attribute to be retrived.
        * @param aValue On completion, contains the requested TInt attribute.
        */
        void StartGetIntAttribute( TInt const aConnectionId,
                            RConnectionMonitor* const aConnectionMonitor,
                            TUint const aAttribute,
                            TInt& aValue );

        /**
        * Starts the active object and gets the value for a TUint attribute.
        * @param aConnectionId Connection id.
        * @param aConnectionMonitor RConnectionMonitor
        * @param aAttribute Identifies the attribute to be retrived.
        * @param aValue On completion, contains the requested TUint attribute.
        */
        void StartGetUintAttribute( TInt const aConnectionId,
                            RConnectionMonitor* const aConnectionMonitor,
                            TUint const aAttribute,
                            TUint& aValue );

        /**
        * Starts the active object and gets the value for a packaged 
        * attribute (TConnMonClientEnumBuf).
        * @param aConnectionId Connection id.
        * @param aConnectionMonitor RConnectionMonitor
        * @param aValue On completion, contains the requested packaged attribute.
        */
        void StartGetConnSharings( TInt const aConnectionId,
                            RConnectionMonitor* const aConnectionMonitor,
                            TConnMonClientEnumBuf& aValue );

        /**
        * Starts the active object and gets the value for a packaged 
        * attribute (TConnMonTimeBuf).
        * @param aConnectionId Connection id.
        * @param aConnectionMonitor RConnectionMonitor
        * @param aValue On completion, contains the requested packaged attribute.
        */
        void StartGetConnTime( TInt const aConnectionId,
                            RConnectionMonitor* const aConnectionMonitor,
                            TConnMonTimeBuf& aValue );

        /**
        * Starts the active object and gets the value for a TInt attribute
        * for the used Bearer type.
        * @param aConnectionId Connection id.
        * @param aConnectionMonitor RConnectionMonitor
        * @param aAttribute Identifies the attribute to be retrived.
        * @param aValue On completion, contains the requested TInt attribute.
        */
        void StartGetBearerType( TInt const aConnectionId,
                            RConnectionMonitor& aConnectionMonitor,
                            TUint const aAttribute,
                            TInt& aValue );

        /**
        * Starts the active object and gathers information on 
        * currently active connections.
        * @param aConnectionCount On completion, contains the number of active connections.
        * @param aConnectionMonitor RConnectionMonitor
        */
        void StartGetConnectionCount( TUint& aConnectionCount,
                            RConnectionMonitor& aConnectionMonitor );
                            
    public: // Functions from base classes (CActive)

        void DoCancel();
        void RunL();

    private:

        /**
        * C++ default constructor.
        */
        CActiveWrapper();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // data
        
        /**
        * Own: sync. helper in async. requests
        */
        CActiveSchedulerWait    iWait;
    };

#endif // __ACTIVE_WRAPPER_H__

            
// End of File
