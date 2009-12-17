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
* Description:  Declaration of TSortSnapUtils 
*
*/



#ifndef __SORTSNAPUTILS_H__
#define __SORTSNAPUTILS_H__


// INCLUDES
#include <e32base.h>
#include <cmmanagerext.h>
#include <rmpm.h>


// CLASS DECLARATION

/**
 * TSortSnapUtils class,
 * utility class to use P&S mechanism to
 * avoid circular reference with MPM
 */     
class TSortSnapUtils 
    {
    public:
        enum TSortSnapUtilsState
            {
            EUninitialised = 0,
            EUseMPM,
            EUsePubSub
            } ;

        /**
        * Constructor
        */    
        TSortSnapUtils( RMPM& aMPM );
        
        /**
        * Destructor
        */    
        ~TSortSnapUtils();

        void GetDestIdArrayL( RCmManagerExt& aCmManagerExt, RArray<TUint32>& aDestArray );     

        /**
        * GetSortSNAPDataL
        * Gets snap data 
        * Reads P&S values first, then uses MPM if
        * unsuccessful. This is needed to avoid a deadlock situation
        * with MPM.
        * @param aSnapData - iSnap attribute of TMpmSnapPubSubFormat is the
        * input parameter, iSortedIaps attribute holds the iap ids if 
        * returned with KErrNone
        */
        TInt GetSortSnapData( TUint32 aSnapId, TMpmSnapBuffer& aSnapBuffer );  

    private:
        void InitDestIdArray();
       
    private:
        TUint32 iSnapIdArray[KMPMSortSnapMaxKey]; 
        TInt iSnapCount;
        TSortSnapUtilsState iSSUState;
        RMPM& iMPM; 
    };     


#endif

// End of File
