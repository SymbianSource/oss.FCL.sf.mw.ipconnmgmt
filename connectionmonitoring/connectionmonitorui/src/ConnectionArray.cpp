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
* Description:  Contains and handles CConnectionInfo instances
*     
*
*/


// INCLUDE FILES

#include "ConnectionArray.h"
#include "ConnectionMonitorUiLogger.h"

// CONSTANTS
/**
* Count of allowed connections.
*/
const TUint KConnArrayGranuality = KConnMonMaxClientUids;   


// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CConnectionArray::~CConnectionArray
// ---------------------------------------------------------
//
CConnectionArray::~CConnectionArray()
    {
    if ( iConnectionArray )
        {
        iConnectionArray->ResetAndDestroy();
        }
    delete iConnectionArray;
    }


// ---------------------------------------------------------
// CConnectionArray::CConnectionArray
// ---------------------------------------------------------
//
CConnectionArray::CConnectionArray()
    {
    }


// ---------------------------------------------------------
// CConnectionArray::ConstructL
// ---------------------------------------------------------
//
void CConnectionArray::ConstructL()
    {
    iConnectionArray = new ( ELeave ) 
                    CArrayPtrFlat<CConnectionInfoBase>( KConnArrayGranuality );
    }


// ---------------------------------------------------------
// CConnectionArray::MdcaCount
// ---------------------------------------------------------
//
TInt  CConnectionArray::MdcaCount() const
    {
    return iConnectionArray->Count();
    }


// ---------------------------------------------------------
// CConnectionArray::MdcaPoint
// ---------------------------------------------------------
//
TPtrC CConnectionArray::MdcaPoint( TInt aIndex ) const
    {
    return ( *iConnectionArray )[aIndex]->GetConnectionListBoxItemText();
    }


// ---------------------------------------------------------
// CConnectionArray::DeepCopyL
// ---------------------------------------------------------
//
CConnectionArray* CConnectionArray::DeepCopyL()
	{
	CConnectionArray* temp = new ( ELeave ) CConnectionArray();
	CleanupStack::PushL( temp );
    temp->ConstructL();
    TInt count = iConnectionArray->Count();
    CConnectionInfoBase* tempConnInfo = NULL;
    for ( TInt i = 0; i < count; ++i )
	    {
	    tempConnInfo = ( *iConnectionArray )[i]->DeepCopyL();
		CleanupStack::PushL( tempConnInfo );	    
	    temp->AppendL( tempConnInfo );
		CleanupStack::Pop( tempConnInfo );	    
	    }
	CleanupStack::Pop( temp );
	return temp;
	}

// ---------------------------------------------------------
// CConnectionArray::GetArrayIndex
// ---------------------------------------------------------
//
TInt CConnectionArray::GetArrayIndex( TUint aConnectionId ) const
    {
    CMUILOGGER_WRITE_F( "GetArrayIndex: %d", aConnectionId );
    TBool found( EFalse );
    TInt index( 0 );
    TInt count = iConnectionArray->Count();
    TUint connectionId( 0 );

    while ( ( index <  count ) && !found  )
        {
        connectionId = ( *iConnectionArray )[index]->GetConnectionId();
        if ( connectionId == aConnectionId )
            {
            found = ETrue;
            }
        else
            {
            ++index;            
            }
        }

    CMUILOGGER_WRITE_F( "Found: %d", found );

    return ( found ? index : KErrNotFound );
    }


// ---------------------------------------------------------
// CConnectionArray::GetArrayIndex
// ---------------------------------------------------------
//
TInt CConnectionArray::GetArrayIndex( CConnectionInfoBase* aConnection ) const
    {
    CMUILOGGER_WRITE_F( "GetArrayIndex: %d", 
                       aConnection->GetConnectionId() );
    TBool found( EFalse );
    TInt index( 0 );
    TInt count = iConnectionArray->Count();

    while ( ( index <  count ) && !found  )
        {
        if ( ( *iConnectionArray )[index] == aConnection )
            {
            found = ETrue;
            }
        else
            {
            ++index;            
            }        
        }

    CMUILOGGER_WRITE_F( "Found: %d", found );

    return ( found ? index : KErrNotFound );        
    }


// ---------------------------------------------------------
// CConnectionArray::AppendL
// ---------------------------------------------------------
//
void CConnectionArray::AppendL( CConnectionInfoBase* aConnection )
    {
    CMUILOGGER_ENTERFN( "CConnectionArray::AppendL" );
    if ( aConnection )
        {
        TUint connectionId = aConnection->GetConnectionId();
        TInt index = GetArrayIndex( connectionId );

        if ( index == KErrNotFound )
            {
            if ( aConnection->GetStatus() != EConnectionClosed &&
                aConnection->GetStatus() != EConnectionUninitialized )
                {
                iConnectionArray->AppendL( aConnection );
                }
            }
        }

    CMUILOGGER_LEAVEFN( "CConnectionArray::AppendL" );
    }


// ---------------------------------------------------------
// CConnectionArray::Delete
// ---------------------------------------------------------
//
void CConnectionArray::Delete( TUint aConnectionId )
    {
    CMUILOGGER_WRITE_F( "Deleting connection: %d", aConnectionId );
    TInt count = iConnectionArray->Count();
    TUint connectionId( 0 );
    TBool deleted( EFalse );
    for ( TInt i = 0; (i < count) && !deleted; ++i )
        {
        connectionId = ( *iConnectionArray )[i]->GetConnectionId();
        if ( connectionId == aConnectionId )
            {
            delete ( *iConnectionArray )[i];
            iConnectionArray->Delete( i );
            iConnectionArray->Compress();
            deleted = ETrue;
            }
        }
    CMUILOGGER_WRITE_F( "Found and deleted: %b", deleted );
    }


// ---------------------------------------------------------
// CConnectionArray::Delete
// ---------------------------------------------------------
//
void CConnectionArray::Delete( CConnectionInfoBase* aConnection )
    {
    CMUILOGGER_WRITE_F( "Deleting connection: %d", 
                        aConnection->GetConnectionId() );
    TInt count = iConnectionArray->Count();
    TBool deleted( EFalse );
    for ( TInt i = 0; (i < count) && !deleted; ++i )
        {
        if ( ( *iConnectionArray )[i] == aConnection )
            {
            delete ( *iConnectionArray )[i];
            iConnectionArray->Delete( i );
            iConnectionArray->Compress();
            deleted = ETrue;
            }
        }
    CMUILOGGER_WRITE_F( "Found and deleted: %b", deleted );
    }


// ---------------------------------------------------------
// CConnectionArray::Reset
// ---------------------------------------------------------
//
void CConnectionArray::Reset()
    {
    iConnectionArray->ResetAndDestroy();
    }

// ---------------------------------------------------------
// CConnectionArray::operator[]
// ---------------------------------------------------------
//
CConnectionInfoBase* CConnectionArray::operator[]( TUint aIndex ) const
    {
    return ( *iConnectionArray )[aIndex];
    }
    
// ---------------------------------------------------------
// CConnectionArray::At
// ---------------------------------------------------------
//
CConnectionInfoBase* CConnectionArray::At( TInt aIndex ) const
    {
    CMUILOGGER_ENTERFN( "CConnectionArray::At()" );
    CConnectionInfoBase* temp( NULL );
    TInt count = iConnectionArray->Count();
    CMUILOGGER_WRITE_F( "count %d", count );
    
    if ( count  && ( aIndex < count ) )
        {
        temp = ( *iConnectionArray )[aIndex];
        }
    
    CMUILOGGER_LEAVEFN( "CConnectionArray::At()" );
    return temp;
    }    

// ---------------------------------------------------------
// CConnectionArray::NumberOfActiveConnections
// ---------------------------------------------------------
//
TInt CConnectionArray::NumberOfActiveConnections() const 
    {
    TInt count( 0 );

    CMUILOGGER_WRITE_F(
         "NumberOfActiveConnections: all in array: [%d]\n",
         MdcaCount() );

    // we need to count all and only the open connection
    for ( TInt i = 0; i < MdcaCount(); ++i )
        {
         if ( ( *iConnectionArray )[i]->IsAlive() )
             {
             ++count;
             }
        }

    CMUILOGGER_WRITE_F( "Active connections: [%d]\n", count );
    return count;
    }    

// ---------------------------------------------------------
// CConnectionArray::NumberOfSuspendedConnections
// ---------------------------------------------------------
//
TInt CConnectionArray::NumberOfSuspendedConnections() const 
    {
    TInt count( 0 );
    
    CMUILOGGER_WRITE_F(
         "Number of all connection in array: [%d]\n", MdcaCount() );

    // we need to count all and only the open connection
    for ( TInt i = 0; i < MdcaCount(); ++i )
        {
         if ( ( *iConnectionArray )[i]->IsSuspended() )
             {
             ++count;
             }
        }
    
    CMUILOGGER_WRITE_F( "Suspended connections: [%d]\n", count );
    return count;
    }


// End of File
