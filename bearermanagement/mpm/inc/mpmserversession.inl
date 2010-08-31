/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Inline functions for CMPMServerSession class
*
*/

// -----------------------------------------------------------------------------
// CMPMServerSession::SetConfirmDlgRoamingPtrNull
// -----------------------------------------------------------------------------
//
inline void CMPMServerSession::SetConfirmDlgRoamingPtrNull()
    {
    iConfirmDlgRoaming = NULL;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::MyServer
// -----------------------------------------------------------------------------
//
inline CMPMServer& CMPMServerSession::MyServer()
    {
    return iMyServer;
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::ConnectionId
// -----------------------------------------------------------------------------
//
inline TConnectionId CMPMServerSession::ConnectionId() const
    {
    return iConnId;
    }
    
// -----------------------------------------------------------------------------
// CMPMServerSession::StoredIapInfo
// -----------------------------------------------------------------------------
//
inline TStoredIapInfo& CMPMServerSession::StoredIapInfo()
    {
    return iStoredIapInfo;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::SetUserConnection
// -----------------------------------------------------------------------------
//
inline void CMPMServerSession::SetUserConnection()
    {
    iUserConnection = ETrue;        
    }
        
// -----------------------------------------------------------------------------
// CMPMServerSession::ClearUserConnection
// -----------------------------------------------------------------------------
//
inline void CMPMServerSession::ClearUserConnection()
    {
    iUserConnection = EFalse;
    }
        
// -----------------------------------------------------------------------------
// CMPMServerSession::UserConnection
// -----------------------------------------------------------------------------
//
inline TBool CMPMServerSession::UserConnection() const
    {
    return iUserConnection;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::VpnUserConnectionUsed
// -----------------------------------------------------------------------------
//
inline TBool CMPMServerSession::VpnUserConnectionUsed() const
    {
    return iVpnUserConnectionUsed;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::AppUid
// -----------------------------------------------------------------------------
//
inline TUint32 CMPMServerSession::AppUid() const
    {
    return iAppUid;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::PreferredIapRequested
// -----------------------------------------------------------------------------
//
inline TBool CMPMServerSession::PreferredIapRequested() const
    {
    return iPreferredIAPRequested;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::ChooseBestIapCalled
// -----------------------------------------------------------------------------
//
inline TBool CMPMServerSession::ChooseBestIapCalled() const
    {
    return ( iIapSelection != NULL );
    }
