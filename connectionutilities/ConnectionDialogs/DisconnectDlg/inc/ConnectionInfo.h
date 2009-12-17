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
* Description: 
*      Declares the connection info.
*
*/


#ifndef DDLG_CONNECTIONINFO_H
#define DDLG_CONNECTIONINFO_H

// INCLUDES
#include <e32base.h>
#include <gulicon.h>
#include <rconnmon.h>
#include <apgicnfl.h>

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Contains attributes of connection.
*/
NONSHARABLE_CLASS( CConnectionInfo ) : public CBase
    {
    public: // Constructors and destructor

        /**
        * Constructor.
        */
        static CConnectionInfo* NewLC();

        /**
        * Destructor.
        */
        virtual ~CConnectionInfo();

    private: // Constructors

        /**
        * Constructor.
        */
        CConnectionInfo();

        /**
        * Constructor.
        */
        void ConstructL();

    public: // New functions

        // Getter functions

        /**
        * Returns the connection id
        * @param -
        * @return the connection id
        */
        inline TUint GetConnId() const;
        
        /**
        * Returns the application name which uses the connection
        * @param aUseSharedIcon in Output returns if the connection should use
        *                       the shared icon
        * @return the application name
        */
        TPtrC GetAppNameL( TBool &aUseSharedIcon );

        /**
        * Returns the Internet Accesspoint name through 
        * the connection is estabilished
        * @param -
        * @return Internet Accespoint Name
        */
        TPtrC GetIapNameL();

        /**
        * Returns the bearer type of the connection
        * @param -
        * @return the bearer type
        */
        inline TInt GetBearerType() const;
        
        /**
        * Returns the IAP id of the connection
        * @param -
        * @return the IAP id
        */
        inline TInt GetIAPId() const;
        
        /**
        * Returns the icon of the application which uses the connection
        * @param aUseSharedIcon Says if the connection should use the shared 
        *                       icon
        * @return the application icon
        */
        CGulIcon* GetAppIconL( TBool aUseSharedIcon );
        
        /**
        * Returns the start time of the connection.
        * @param -
        * @return The connection start time.
        */
        inline TTime GetStartTime() const;

        // Setter functions

        /**
        * Set the connection id
        * @param the connection id
        * @return -
        */
        inline void SetConnId( TUint aConnId );
        
        /**
        * Set the Internet Accespoint name
        * through the connection is estabilish
        * @param The name of the Internet Accesspoint
        * @return -
        */
        inline void SetIapNameL( const TDesC& aIapName );

        /**
        * Set the client info of the application which uses the connection
        * @param the client information
        * @return -
        */
        inline void SetClientInfo( TConnMonClientEnum aClientInfo );

        /**
        * Set the bearer type of the given connection
        * @param the bearer type
        * @return -
        */
        inline void SetBearerType( TInt aBearerType );
        
        /**
        * Set the IAP id of the given connection
        * @param the IAP id 
        * @return -
        */
        inline void SetIAPId( TInt aIAPId );
        
        /**
        * Set the start time of the connection.
        * @param The start time of the connection.
        * @return -
        */
        inline void SetStartTime(TTime aTime);
        
        
        

    private: // data

        TUint               iConnId;     ///< id of icon represents connection
        TInt                iBearerType; ///< bearer type of connection
        TConnMonClientEnum  iClientInfo; ///< info from clients, use connection
        HBufC*              iAppName;    ///< Owned. Apps which use connection.
        HBufC*              iIapName;    ///< Owned. Access point name.
        CGulIcon*           iAppIcon;    ///< Owned. Used for icon.
        TUint               iIAPId;      ///< Id of AP
        TTime               iStartTime;  ///< The start time of the connection
        
    };


#include "ConnectionInfo.inl"

#endif // DDLG_CONNECTIONINFO_H

// End Of File
