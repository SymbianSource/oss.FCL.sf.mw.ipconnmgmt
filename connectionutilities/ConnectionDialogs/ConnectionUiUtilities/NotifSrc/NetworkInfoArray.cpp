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
* Description:  NetworkInfoArray contains and handles CNetworkInfoBase 
*  and derived class instances
*     
*
*/


// INCLUDE FILES

#include "NetworkInfoArray.h"
#include "ConnectionDialogsLogger.h"


// CONSTANTS
/**
* Granularity of NetworkInfoArray array
*/
LOCAL_D const TUint KConnArrayGranularity = 3;


// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CNetworkInfoArray::CNetworkInfoArray
// ---------------------------------------------------------
//
CNetworkInfoArray::CNetworkInfoArray()
: iNetworkInfoArray( NULL )
    {
    }


// ---------------------------------------------------------
// CNetworkInfoArray::~CNetworkInfoArray
// ---------------------------------------------------------
//
CNetworkInfoArray::~CNetworkInfoArray()
    {
    CLOG_ENTERFN( "CNetworkInfoArray::~CNetworkInfoArray" );  

    if( iNetworkInfoArray )
        {
        iNetworkInfoArray->ResetAndDestroy();
        }
    delete iNetworkInfoArray;

    CLOG_LEAVEFN( "CNetworkInfoArray::~CNetworkInfoArray" );  
    }


// ---------------------------------------------------------
// CNetworkInfoArray::NewL
// ---------------------------------------------------------
//
CNetworkInfoArray* CNetworkInfoArray::NewL()
    {
    CLOG_ENTERFN( "CNetworkInfoArray::NewL" );  

    CNetworkInfoArray* self = new ( ELeave ) CNetworkInfoArray();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    CLOG_LEAVEFN( "CNetworkInfoArray::NewL" );

    return self;
    }


// ---------------------------------------------------------
// CNetworkInfoArray::ConstructL
// ---------------------------------------------------------
//
void CNetworkInfoArray::ConstructL()
    {
    CLOG_ENTERFN( "CNetworkInfoArray::ConstructL" );  

    iNetworkInfoArray = new( ELeave )
                    CArrayPtrFlat<CNetworkInfoBase>( KConnArrayGranularity );

    CLOG_LEAVEFN( "CNetworkInfoArray::ConstructL" );
    }


// ---------------------------------------------------------
// CNetworkInfoArray::MdcaCount
// ---------------------------------------------------------
//
TInt  CNetworkInfoArray::MdcaCount() const
    {
    CLOG_WRITE( "CNetworkInfoArray::MdcaCount" );
    CLOG_WRITEF( _L( "Count : %d" ),  iNetworkInfoArray->Count() );
    return iNetworkInfoArray->Count();
    }


// ---------------------------------------------------------
// CNetworkInfoArray::MdcaPoint
// ---------------------------------------------------------
//
TPtrC CNetworkInfoArray::MdcaPoint( TInt aIndex ) const
    {
    CLOG_WRITEF( _L( "CNetworkInfoArray::MdcaPoint aIndex: %d" ),  aIndex );
    return ( *iNetworkInfoArray )[aIndex]->GetNetworkListBoxItemText();
    }


// ---------------------------------------------------------
// CNetworkInfoArray::GetArrayIndex
// ---------------------------------------------------------
//
TInt CNetworkInfoArray::GetArrayIndex( TName& aNetworkName ) const
    {
    CLOG_ENTERFN( "CNetworkInfoArray::GetArrayIndex" );

    TBool found( EFalse );
    TInt index( 0 );

    while( ( index < iNetworkInfoArray->Count() ) && !found  )
        {
        if( ( *iNetworkInfoArray )[index]->NetworkName().Compare(
                                                            aNetworkName ) )
            {
            ++index;
            }
        else
            {
            found = ETrue;
            }
        }

    CLOG_LEAVEFN( "CNetworkInfoArray::GetArrayIndex" );  

    return ( found ? index : KErrNotFound );
    }


// ---------------------------------------------------------
// CNetworkInfoArray::AppendL
// ---------------------------------------------------------
//
void CNetworkInfoArray::AppendL( CNetworkInfoBase* aNetwork )
    {
    CLOG_ENTERFN( "CNetworkInfoArray::AppendL " );

    if ( aNetwork )
        {
        iNetworkInfoArray->AppendL( aNetwork );
        }

    CLOG_LEAVEFN( "CNetworkInfoArray::AppendL " );  
    }


// ---------------------------------------------------------
// CNetworkInfoArray::Reset
// ---------------------------------------------------------
//
void CNetworkInfoArray::Reset()
    {
    iNetworkInfoArray->ResetAndDestroy();
    }


// ---------------------------------------------------------
// CNetworkInfoArray::operator[]
// ---------------------------------------------------------
//
CNetworkInfoBase* CNetworkInfoArray::operator[]( TUint aIndex ) const
    {
    return ( *iNetworkInfoArray )[aIndex];
    }


// ---------------------------------------------------------
// CNetworkInfoArray::At
// ---------------------------------------------------------
//
CNetworkInfoBase* CNetworkInfoArray::At( TInt aIndex ) const
    {
    CLOG_ENTERFN( "CNetworkInfoArray::At " );

    CNetworkInfoBase* temp = NULL;
    if ( ( aIndex < iNetworkInfoArray->Count() ) && 
         ( aIndex > KErrNotFound ) )
        {
        temp = ( *iNetworkInfoArray )[aIndex];
        }

    CLOG_LEAVEFN( "CNetworkInfoArray::At " );

    return temp;
    }


// ---------------------------------------------------------
// CNetworkInfoArray::Sort
// ---------------------------------------------------------
//    
TInt CNetworkInfoArray::Sort( TKeyArrayFix& aKey )
    {
    return iNetworkInfoArray->Sort( aKey );
    }
    

// End of File
