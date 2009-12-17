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
*      Declares the CConnectionCArray
*
*/


// INCLUDE FILES
#include "ConnectionCArray.h"
#include "ConnectionInfo.h"
#include "ConnectionDialogsLogger.h"
#include "connectioninfosortkey.h"

// CONSTANTS
LOCAL_D const TUint KConnArrayGranularity = KConnMonMaxClientUids; 
                                                // from rconnmon.h


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CConnectionCArray::NewL
// ---------------------------------------------------------
//
CConnectionCArray* CConnectionCArray::NewL()
    {
    CLOG_ENTERFN("CConnectionCArray::NewL");    
    CConnectionCArray* self = new( ELeave ) CConnectionCArray();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); // self
    CLOG_LEAVEFN("CConnectionCArray::NewL");    

    return self;
    }


// ---------------------------------------------------------
// CConnectionCArray::CConnectionCArray
// ---------------------------------------------------------
//
CConnectionCArray::CConnectionCArray()
    {
    }


// ---------------------------------------------------------
// CConnectionCArray::ConstructL
// ---------------------------------------------------------
//
void CConnectionCArray::ConstructL()
    {
    CLOG_ENTERFN("CConnectionCArray::ConstructL");    
    
    iConnArray = new( ELeave )CArrayPtrFlat<CConnectionInfo>( 
                                                    KConnArrayGranularity );
    CLOG_LEAVEFN("CConnectionCArray::ConstructL");    
    }


// ---------------------------------------------------------
// CConnectionCArray::~CConnectionCArray
// ---------------------------------------------------------
//
CConnectionCArray::~CConnectionCArray()
    {
    CLOG_ENTERFN("CConnectionCArray::~CConnectionCArray");    
    
    if ( iConnArray )
        {
        iConnArray->ResetAndDestroy();
        }
    delete iConnArray;

    CLOG_LEAVEFN("CConnectionCArray::~CConnectionCArray");    
    }

// ---------------------------------------------------------
// CConnectionCArray::Count
// ---------------------------------------------------------
//
TInt CConnectionCArray::Count() const
    {
    return iConnArray->Count();
    }


// ---------------------------------------------------------
// CConnectionCArray::AppendL
// ---------------------------------------------------------
//
void CConnectionCArray::AppendL( const CConnectionInfo* aInfo )
    {
    iConnArray->AppendL( CONST_CAST( CConnectionInfo* , aInfo ) );
    }


// ---------------------------------------------------------
// CConnectionCArray::ResetAndDestroy
// ---------------------------------------------------------
//
void CConnectionCArray::ResetAndDestroy()
    {
    iConnArray->ResetAndDestroy();
    }


// ---------------------------------------------------------
// CConnectionCArray::At
// ---------------------------------------------------------
//
CConnectionInfo* CConnectionCArray::At( TInt aIndex ) const
    {
    return iConnArray->At( aIndex );
    }
    
    
// ---------------------------------------------------------
// CConnectionCArray::GetArrayIndex
// ---------------------------------------------------------
//
TInt CConnectionCArray::GetArrayIndex( TUint aConnectionId ) const
    {
    CLOG_WRITEF( _L( "GetArrayIndex: %d" ), aConnectionId );
    TBool found( EFalse );
    TInt index( 0 );
    TInt count = iConnArray->Count();
    TUint connectionId( 0 );

    while( ( index <  count ) && !found  )
        {
        connectionId = ( *iConnArray )[index]->GetConnId();
        if( connectionId == aConnectionId )
            {
            found = ETrue;
            }
        else
            {
            ++index;            
            }
        }

    CLOG_WRITEF( _L("Found: %d"), found );

    return ( found ? index : KErrNotFound );
    }

// ---------------------------------------------------------
// CConnectionCArray::SortL
// ---------------------------------------------------------
//
void CConnectionCArray::Sort() const
    {
    TConnectionInfoSortKey key;
    key.SetPtr( iConnArray );
    iConnArray->Sort( key );
    }
    

// End of File
