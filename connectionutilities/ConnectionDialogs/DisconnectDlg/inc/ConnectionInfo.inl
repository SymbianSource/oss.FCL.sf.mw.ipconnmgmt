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
*      Inline method definitions of the class CConnectionInfo.
*
*/



// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CConnectionInfo::GetConnId
// ---------------------------------------------------------
//
TUint CConnectionInfo::GetConnId() const
    {
    return iConnId;
    }


// ---------------------------------------------------------
// CConnectionInfo::GetBearerType
// ---------------------------------------------------------
//
TInt CConnectionInfo::GetBearerType() const
    {
    return iBearerType;
    }

    
// ---------------------------------------------------------
// CConnectionInfo::GetIAPId
// ---------------------------------------------------------
//
TInt CConnectionInfo::GetIAPId() const
    {
    return iIAPId;
    }

// ---------------------------------------------------------
// CConnectionInfo::GetStartTime
// ---------------------------------------------------------
//
TTime CConnectionInfo::GetStartTime() const
    {
    return iStartTime;
    }

// ---------------------------------------------------------
// CConnectionInfo::SetConnId
// ---------------------------------------------------------
//
void CConnectionInfo::SetConnId( TUint aConnId )
    {
    iConnId = aConnId;
    }


// ---------------------------------------------------------
// CConnectionInfo::SetIapNameL
// ---------------------------------------------------------
//
void CConnectionInfo::SetIapNameL( const TDesC& aIapName )
    {
    iIapName = aIapName.AllocL();
    }


// ---------------------------------------------------------
// CConnectionInfo::SetClientInfo
// ---------------------------------------------------------
//
void CConnectionInfo::SetClientInfo( TConnMonClientEnum aClientInfo )
    {
    iClientInfo = aClientInfo;
    }


// ---------------------------------------------------------
// CConnectionInfo::SetBearerType
// ---------------------------------------------------------
//
void CConnectionInfo::SetBearerType( TInt aBearerType )
    {
    iBearerType = aBearerType;
    }

    
// ---------------------------------------------------------
// CConnectionInfo::SetIAPId
// ---------------------------------------------------------
//
void CConnectionInfo::SetIAPId( TInt aIAPId )
    {
    iIAPId = aIAPId;
    }  

// ---------------------------------------------------------
// CConnectionInfo::SetStartTime
// ---------------------------------------------------------
//    
void CConnectionInfo::SetStartTime( TTime aTime )
    {
    iStartTime = aTime;
    }

// End of File
