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
* Description:  Represents a connection
*     
*
*/


#ifndef GPRSCONNECTIONINFO_H_INCLUDED
#define GPRSCONNECTIONINFO_H_INCLUDED

// INCLUDES
#include    <rconnmon.h>
#include    <stdlib.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include    <commsdat.h>
#else
#include    <commsdat.h>
#include    <commsdat_partner.h>
#endif
#include    "ConnectionInfoBase.h"

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Represents a connection.
*/
class CGprsConnectionInfo : public CConnectionInfoBase      
    {
    public: 
        /**
        * Static constructor
        * @param aConnectionId The id of the given connection.
        * @param aMonitor for refreshing details
        * @param aConnectionBearerType the bearer type of connection
        * @param aActiveWrapper the for refreshing
        */
        static CGprsConnectionInfo* NewL( TInt aConnectionId, 
                                  RConnectionMonitor* const aConnectionMonitor,
                                  TConnMonBearerType aConnectionBearerType,
                                  CActiveWrapper* aActiveWrapper );
                
        /**
        * Destructor
        */
        virtual ~CGprsConnectionInfo();
    public:    
        /**
        * Give back a copy of connection. 
        */
        virtual CConnectionInfoBase* DeepCopyL();
    
        /**
        * Refresh required details of connection. 
        */
        virtual void RefreshDetailsL();

        /**
        * Refresh iListBoxItemText text, connection ListBox has a reference to
        * this text.
        */
        virtual void RefreshConnectionListBoxItemTextL();

        /**
        * Refresh details array of connection. DetailsListBox has a reference
        * to this array.
        */
        virtual void RefreshDetailsArrayL();

        /**
        * Gives back the icon id of connection type
        * @return the icon id of connection type
        */
        virtual TUint GetIconId() const;

    protected:
        /**
        * Constructor
        * @param aConnectionId The id of the given connection.
        * @param aMonitor for refreshing details
        * @param aConnectionBearerType the bearer type of connection
        * @param aActiveWrapper the for refreshing
        */
        CGprsConnectionInfo( TInt aConnectionId, 
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

        /**
        * Creates a string from Access Point name of the connection.
        * @return string representation count of Access Point name of 
        * the connection.
        */
        HBufC* ToStringApNameLC() const;

    private:

    protected: // Data members
    
        TBuf<CommsDat::KMaxTextLength> iApName;    ///< Used AccessPoint name
        
    };


#endif // GPRSCONNECTIONINFO_H_INCLUDED

// Enf of File
