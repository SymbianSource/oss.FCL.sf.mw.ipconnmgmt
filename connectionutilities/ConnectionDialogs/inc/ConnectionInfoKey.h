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
* Description:  Declaration of class CConnectionInfoKey
*
*/


#ifndef __CCONNECTIONINFOKEY_H__
#define __CCONNECTIONINFOKEY_H__


// INCLUDES
#include <e32base.h>


// FORWARD DECLARATION
class CConnectionInfo;


// CLASS DECLARATION
/**
 * Connection information key class
 */
NONSHARABLE_CLASS( CConnectionInfoKey ) : public TKeyArrayFix
    {
    public:

        /**
        * SetPtr function
        * @param aPtr   Pointer used to initialize the class pointer
        */
        void SetPtr( CArrayPtrFlat<CConnectionInfo>* aPtr );

        /**
        * Compare function
        * @param aLeft  Left element to be compared
        * @param aRight Right element to be compared
        * @return TInt  Result of comparison of the elements
        */
        virtual TInt Compare( TInt aLeft, TInt aRight ) const;

        /**
        * TConnectionInfoKey constructor
        * @return -
        */
        CConnectionInfoKey( TBool aIsWLANFeatureSupported );

        /**
        * ~CConnectionInfoKey() destructor
        * @param  -
        */
        ~CConnectionInfoKey();
        
        /**
        * NewL function
        * @param aNameEasyWLAN  Name of Easy WLAN connection
        * return CConnectionInfoKey*
        */
        static CConnectionInfoKey* NewL( TDesC &aNameEasyWLAN );

        /**
        * NewL function
        * return CConnectionInfoKey*
        */
        static CConnectionInfoKey* NewL();


    private:

        /**
        * Symbian default constructor.
        * @param aNameEasyWLAN  Name of Easy WLAN connection
        */      
        void ConstructL( TDesC &aNameEasyWLAN );

        /**
        * Symbian default constructor.
        */      
        void ConstructL();


    protected:
        // Pointer to class containing info over the connection 
        CArrayPtrFlat<CConnectionInfo> *iPtr;

        // Tells if WLan feature is supported at runtime
        TBool iIsWLANFeatureSupported;          

        // Name of Easy WLAN connection
        HBufC*  iNameEasyWLAN;
    };


#endif

// End of File
