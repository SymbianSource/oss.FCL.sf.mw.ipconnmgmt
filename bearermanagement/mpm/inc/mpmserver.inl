/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Inline functions for CMPMServer class
*
*/

// -----------------------------------------------------------------------------
// TMPMBlackListConnId::Iap
// -----------------------------------------------------------------------------
//
inline TUint32 TMPMBlackListConnId::Iap( TInt aIndex) const
    {
    return iBlackListIap[aIndex];
    }

// -----------------------------------------------------------------------------
// TMPMBlackListConnId::Category
// -----------------------------------------------------------------------------
//
inline TBlacklistCategory TMPMBlackListConnId::Category( TInt aIndex) const
    {
    return iCategory[aIndex];
    }

// -----------------------------------------------------------------------------
// CMPMServer::SetEvents
// -----------------------------------------------------------------------------
//
inline void CMPMServer::SetEvents( CMPMConnMonEvents* aEvents )
    {
    iEvents = aEvents;
    }


// -----------------------------------------------------------------------------
// CMPMServer::Events
// -----------------------------------------------------------------------------
//
inline CMPMConnMonEvents* CMPMServer::Events()
    {
    return iEvents;
    }

// -----------------------------------------------------------------------------
// CMPMServer::SetWLANScanRequired
// -----------------------------------------------------------------------------
//
inline void CMPMServer::SetWLANScanRequired( const TBool aValue )
    {
    iWLANScanRequired = aValue;
    }


// -----------------------------------------------------------------------------
// CMPMServer::IsWLANScanRequired
// -----------------------------------------------------------------------------
//
inline TBool CMPMServer::IsWLANScanRequired() const
    {
    return iWLANScanRequired;
    }

// -----------------------------------------------------------------------------
// CMPMServer::DisconnectQueue
// -----------------------------------------------------------------------------
//
inline CArrayPtrFlat<CMPMDisconnectDlg>* CMPMServer::DisconnectQueue()
    {
    return iDisconnectQueue;
    }

// -----------------------------------------------------------------------------
// CMPMServer::RoamingQueue
// -----------------------------------------------------------------------------
//
inline CArrayPtrFlat<CMPMConfirmDlgRoaming>* CMPMServer::RoamingQueue()
    {
    return iRoamingQueue;
    }

// -----------------------------------------------------------------------------
// CMPMServer::StartingQueue
// -----------------------------------------------------------------------------
//
inline CArrayPtrFlat<CMPMConfirmDlgStarting>* CMPMServer::StartingQueue()
    {
    return iStartingQueue;
    }

// -----------------------------------------------------------------------------
// CMPMServer::IncrementConnections
// -----------------------------------------------------------------------------
//
inline void CMPMServer::IncrementConnections()
    {
    iConnectionCounter++;
    }


// -----------------------------------------------------------------------------
// CMPMServer::DecrementConnections
// -----------------------------------------------------------------------------
//
inline void CMPMServer::DecrementConnections()
    {
    if ( ConnectionCounter() > 0 )
        {
        iConnectionCounter--;
        }
    }


// -----------------------------------------------------------------------------
// CMPMServer::ConnectionCounter
// -----------------------------------------------------------------------------
//
inline TUint CMPMServer::ConnectionCounter()
    {
    return iConnectionCounter;
    }


// -----------------------------------------------------------------------------
// CMPMServer::SetConnectionCounter
// -----------------------------------------------------------------------------
//
inline void CMPMServer::SetConnectionCounter( const TUint aValue )
    {
    iConnectionCounter = aValue;
    }

// -----------------------------------------------------------------------------
// CMPMServer::SetUserConnection
// -----------------------------------------------------------------------------
//
inline void CMPMServer::SetUserConnection()
    {
    iUserConnection = ETrue;
    }

// -----------------------------------------------------------------------------
// CMPMServer::ClearUserConnection
// -----------------------------------------------------------------------------
//
inline void CMPMServer::ClearUserConnection()
    {
    iUserConnection = EFalse;
    }

// -----------------------------------------------------------------------------
// CMPMServer::UserConnection
// -----------------------------------------------------------------------------
//
inline TBool CMPMServer::UserConnection() const
    {
    return iUserConnection;
    }

// -----------------------------------------------------------------------------
// CMPMServer::SetUserConnPref
// -----------------------------------------------------------------------------
//
inline void CMPMServer::SetUserConnPref( const TMpmConnPref& aUserConnPref )
    {
    iUserConnPref = aUserConnPref;
    }
        
// -----------------------------------------------------------------------------
// CMPMServer::UserConnPref
// -----------------------------------------------------------------------------
//
inline const TMpmConnPref* CMPMServer::UserConnPref() const
    {
    return &iUserConnPref;
    }

// -----------------------------------------------------------------------------
// CMPMServer::CsIdWatcher
// -----------------------------------------------------------------------------
//
inline CMpmCsIdWatcher* CMPMServer::CsIdWatcher()
    {
    return iMpmCsIdWatcher;
    }

// -----------------------------------------------------------------------------
// CMPMServer::DedicatedClients
// -----------------------------------------------------------------------------
//
inline RArray<TUint32>& CMPMServer::DedicatedClients()
    {
    return iDedicatedClients;
    }

// -----------------------------------------------------------------------------
// CMPMServer::CommsDatAccess
// -----------------------------------------------------------------------------
//
inline CMPMCommsDatAccess* CMPMServer::CommsDatAccess()
    {
    return iCommsDatAccess;
    }

// -----------------------------------------------------------------------------
// CMPMServer::RoamingWatcher()
// -----------------------------------------------------------------------------
//
inline CMPMRoamingWatcher* CMPMServer::RoamingWatcher() const
    {
    return iRoamingWatcher;
    }

// -----------------------------------------------------------------------------
// CMPMServer::ConnUiUtils()
// -----------------------------------------------------------------------------
//
inline CConnectionUiUtilities* CMPMServer::ConnUiUtils() const
    {
    return iConnUiUtils;
    }

// End of File
