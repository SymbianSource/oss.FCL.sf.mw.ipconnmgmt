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
* Description:  Inline method definitions of the class CNetworkInfoBase
*     
*
*/


#ifndef NETWORKINFOBASE_INL
#define NETWORKINFOBASE_INL
#include <utf.h>

// ================= MEMBER FUNCTIONS =======================
//
// ---------------------------------------------------------
// CNetworkInfoBase::NetworkName
// ---------------------------------------------------------
//
TPtrC CNetworkInfoBase::NetworkName() const
    {
    return iNetworkName;
    }

// ---------------------------------------------------------
// CNetworkInfoBase::SignalStrength
// ---------------------------------------------------------
//
TSignalStrength CNetworkInfoBase::SignalStrength()  const
    {
    return iSignalStrength;
    }

// ---------------------------------------------------------
// CNetworkInfoBase::NetworkMode
// ---------------------------------------------------------
//
TNetworkMode CNetworkInfoBase::NetworkMode()  const
    {
    return iNetworkMode;
    }

// ---------------------------------------------------------
// CNetworkInfoBase::SecurityMode
// ---------------------------------------------------------
//
TWlanConnectionSecurityMode CNetworkInfoBase::SecurityMode() const
    {
    return iSecurityMode;
    }

// ---------------------------------------------------------
// CNetworkInfoBase::ProtectedSetupSupported
// ---------------------------------------------------------
//
TBool CNetworkInfoBase::ProtectedSetupSupported() const
    {
    return iProtectedSetupSupported;
    }

// ---------------------------------------------------------
// CNetworkInfoBase::ExtendedSecurityMode
// ---------------------------------------------------------
//
TWlanConnectionExtentedSecurityMode CNetworkInfoBase::ExtendedSecurityMode() const
    {
    return iExtendedSecurityMode;
    }

// ---------------------------------------------------------
// CNetworkInfoBase::GetNetworkListBoxItemText
// ---------------------------------------------------------
//
TPtrC CNetworkInfoBase::GetNetworkListBoxItemText() const
    {
    return iListBoxItemText;
    }


// ---------------------------------------------------------
// CNetworkInfoBase::OthersEntry
// ---------------------------------------------------------
//
TBool CNetworkInfoBase::OthersEntry() const
    {
    return iOthersEntry;
    }


// ---------------------------------------------------------
// CNetworkInfoBase::SetNetworkName
// ---------------------------------------------------------
//
void CNetworkInfoBase::SetNetworkName( TDes8& aNetworkName ) 
    { 
    CnvUtfConverter::ConvertToUnicodeFromUtf8( iNetworkName, aNetworkName);
    }


// ---------------------------------------------------------
// CNetworkInfoBase::SetSignalStrength
// ---------------------------------------------------------
//
void CNetworkInfoBase::SetSignalStrength( TSignalStrength aStrength )
    { 
    iSignalStrength = aStrength; 
    }


// ---------------------------------------------------------
// CNetworkInfoBase::SetNetworkMode
// ---------------------------------------------------------
//
void CNetworkInfoBase::SetNetworkMode( TNetworkMode aNetworkMode ) 
    { 
    iNetworkMode = aNetworkMode; 
    }


// ---------------------------------------------------------
// CNetworkInfoBase::SetSecurityMode
// ---------------------------------------------------------
//
void CNetworkInfoBase::SetSecurityMode( 
                                    TWlanConnectionSecurityMode aSecurityMode )
    { 
    iSecurityMode = aSecurityMode; 
    }


// ---------------------------------------------------------
// CNetworkInfoBase::SetOthersEntry
// ---------------------------------------------------------
//
void CNetworkInfoBase::SetOthersEntry( TBool aOthersEntry ) 
    { 
    iOthersEntry = aOthersEntry; 
    }


// ---------------------------------------------------------
// CNetworkInfoBase::SetProtectedSetupSupported
// ---------------------------------------------------------
//
void CNetworkInfoBase::SetProtectedSetupSupported( 
                                            TBool aProtectedSetupSupported )
    { 
    iProtectedSetupSupported = aProtectedSetupSupported; 
    }

// ---------------------------------------------------------
// CNetworkInfoBase::SetExtendedSecurityMode
// ---------------------------------------------------------
//
void CNetworkInfoBase::SetExtendedSecurityMode( 
                  TWlanConnectionExtentedSecurityMode aExtendedSecurityMode )
    { 
    iExtendedSecurityMode = aExtendedSecurityMode; 
    }                 

#endif // NETWORKINFOBASE_INL

// End of File
