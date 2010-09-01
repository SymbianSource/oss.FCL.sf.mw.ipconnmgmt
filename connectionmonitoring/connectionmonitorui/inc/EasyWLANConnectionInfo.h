/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Represents a WLAN connection
*     
*
*/


#ifndef EASYWLANCONNECTIONINFO_H_INCLUDED
#define EASYWLANCONNECTIONINFO_H_INCLUDED

// INCLUDES
#include "WlanConnectionInfo.h"

// CONSTANTS

// FORWARD DECLARATIONS
class RConnectionMonitor;

// CLASS DECLARATION
/**
* Represents a WLAN connection.
*/
class CEasyWlanConnectionInfo : public CWlanConnectionInfo
    {
    public:
        /**
        * Static constructor
        * @param aConnectionId The id of the given connection.
        * @param aMonitor for refreshing details
        * @param aConnectionBearerType the bearer type of connection
        * @param aActiveWrapper the for refreshing
        */
        static CEasyWlanConnectionInfo* NewL( TInt aConnectionId, 
                              RConnectionMonitor* const aConnectionMonitor,
                              TConnMonBearerType aConnectionBearerType,
                              CActiveWrapper* aActiveWrapper );

        /**
        * Destructor
        */
        virtual ~CEasyWlanConnectionInfo();
        
    public:
        /**
        * Give back a copy of connection. 
        */
        virtual CConnectionInfoBase* DeepCopyL();
    
        /**
        * Refresh iListBoxItemText text, connection ListBox has a 
        * reference to this text.
        */
        virtual void RefreshConnectionListBoxItemTextL();

        /**
        * Refresh details array of connection. 
        * DetailsListBox has a reference to this array.
        */
        virtual void RefreshDetailsArrayL();

    protected:  // Constructors
        /**
        * Constructor
        * @param aConnectionId The id of the given connection.
        * @param aMonitor for refreshing details
        * @param aConnectionBearerType the bearer type of connection
        * @param aActiveWrapper the for refreshing
        */
        CEasyWlanConnectionInfo( TInt aConnectionId, 
                             RConnectionMonitor* const aConnectionMonitor,
                             TConnMonBearerType aConnectionBearerType,
                             CActiveWrapper* aActiveWrapper );

        /**
        * Second phase construction
        */
        void ConstructL();

    protected:
        /**
        * Creates an array with requrired details of connection to
        * the listbox which visulaize details of connection. This member
        * is called at initialization.
        */
        virtual void ToArrayDetailsL();

    };

#endif // EASYWLANCONNECTIONINFO_H_INCLUDED
//End of File.