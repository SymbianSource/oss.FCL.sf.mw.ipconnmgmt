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
* Description:  Implementation of class CNetworkInfoKey.
*
*/


// INCLUDE FILES
#include "NetworkInfoKey.h"
#include "NetworkInfoArray.h"
#include "NetworkInfoBase.h"
#include "ConnectionDialogsLogger.h"


// ================= MEMBER FUNCTIONS =======================
//
// ---------------------------------------------------------
// CNetworkInfoKey::CNetworkInfoKey()
// ---------------------------------------------------------
//
CNetworkInfoKey::CNetworkInfoKey()
: TKeyArrayFix( 0, ECmpNormal )
    {
    }


// ---------------------------------------------------------
// CNetworkInfoKey::~CNetworkInfoKey()
// ---------------------------------------------------------
//
CNetworkInfoKey::~CNetworkInfoKey()
    {
    }


// ---------------------------------------------------------
// CNetworkInfoKey::SetPtr( CArrayPtrFlat<CNetworkInfoBase>* aPtr )
// ---------------------------------------------------------
//
void CNetworkInfoKey::SetPtr( CNetworkInfoArray* aPtr )
    {
    CLOG_ENTERFN( "CNetworkInfoKey::SetPtr" );      
    
    iPtr = aPtr;

    CLOG_LEAVEFN( "CNetworkInfoKey::SetPtr" );      
    }


// ---------------------------------------------------------
// CNetworkInfoKey::Compare( TInt aLeft, TInt aRight ) const
// ---------------------------------------------------------
//
TInt CNetworkInfoKey::Compare( TInt aLeft, TInt aRight ) const
    {
    CLOG_ENTERFN( "CNetworkInfoKey::Compare" );      
    
    TInt retval( 0 );

    CNetworkInfoArray* array = ( CNetworkInfoArray* )iPtr;
    retval = array->At( aLeft )->NetworkName().CompareC( 
                                        array->At( aRight )->NetworkName() );

    CLOG_WRITEF( _L( "aLeft %d" ), aLeft );  
    CLOG_WRITEF( _L( "aRight %d" ), aRight );      
    CLOG_WRITEF( _L( "retval %d" ), retval );  
        
    CLOG_LEAVEFN( "CNetworkInfoKey::Compare" );          

    return retval;
    }    


// End of File
