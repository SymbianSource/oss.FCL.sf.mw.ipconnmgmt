/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for bearer and bearer group mapping.
*
*/

#include "ConnMonBearerGroupManager.h"
#include "log.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CConnMonBearerGroupManager::CConnMonBearerGroupManager()
    {
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CConnMonBearerGroupManager* CConnMonBearerGroupManager::NewL()
    {
    CConnMonBearerGroupManager* bgm = new( ELeave ) CConnMonBearerGroupManager();

    CleanupStack::PushL( bgm );
    bgm->ConstructL();
    CleanupStack::Pop( bgm );

    return bgm;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
void CConnMonBearerGroupManager::ConstructL()
    {
    // Create group masks for each bearer
    // NOTE: Masks must be appended in correct order to be able to use
    // TConnMonDetailedBearer enumeration as indexes

    iBearerGroupMasks1.Reset();
    // EBearerUnknown
    TUint unknown( 0 );
    User::LeaveIfError( iBearerGroupMasks1.Append( unknown ) );

    //EBearerCSD
    TUint csd = EBearerGroupCellular | EBearerGroupCircuitSwitched | EBearerGroup2G;
    User::LeaveIfError( iBearerGroupMasks1.Append( csd ) );

    // EBearerWCDMA
    TUint wcdma = EBearerGroupCellular | EBearerGroupCellularPacket | EBearerGroup3G;
    User::LeaveIfError( iBearerGroupMasks1.Append( wcdma ) );

    // EBearerLAN
    TUint lan = EBearerGroupNonCellular | EBearerGroupLANBased | EBearerGroupWired;
    User::LeaveIfError( iBearerGroupMasks1.Append( lan ) );

    // EBearerCDMA2000
    TUint cdma2000 = 0;
    User::LeaveIfError( iBearerGroupMasks1.Append( cdma2000 ) );

    // EBearerGPRS
    TUint gprs = EBearerGroupCellular | EBearerGroupCellularPacket |
                 EBearerGroup2G | EBearerGroup2GPacket;
    User::LeaveIfError( iBearerGroupMasks1.Append( gprs ) );

    // EBearerHSCSD
    TUint hscsd = EBearerGroupCellular | EBearerGroupCircuitSwitched | EBearerGroup2G;
    User::LeaveIfError( iBearerGroupMasks1.Append( hscsd ) );

    // EBearerEdgeGPRS
    TUint egprs = EBearerGroupCellular | EBearerGroupCellularPacket |
                  EBearerGroup2G | EBearerGroup2GPacket;
    User::LeaveIfError( iBearerGroupMasks1.Append( egprs ) );

    // EBearerWLAN
    TUint wlan = EBearerGroupNonCellular | EBearerGroupLANBased | EBearerGroupWLAN;
    User::LeaveIfError( iBearerGroupMasks1.Append( wlan ) );

    // EBearerBluetooth
    TUint bt = EBearerGroupNonCellular | EBearerGroupIPOverConnectivityProtocol;
    User::LeaveIfError( iBearerGroupMasks1.Append( bt ) );

    // EBearerVirtual
    TUint virt = EBearerGroupVirtual;
    User::LeaveIfError( iBearerGroupMasks1.Append( virt ) );

    // EBearerVirtualVPN
    TUint virtVPN = EBearerGroupVirtual;
    User::LeaveIfError( iBearerGroupMasks1.Append( virtVPN ) );

    // EBearerWcdmaCSD
    TUint wcdmaCSD = EBearerGroupCellular | EBearerGroupCircuitSwitched | EBearerGroup3G;
    User::LeaveIfError( iBearerGroupMasks1.Append( wcdmaCSD ) );

    // EBearerHSDPA
    TUint hsdpa = EBearerGroupCellular | EBearerGroupCellularPacket |
                  EBearerGroup3G | EBearerGroup3GPacket | EBearerGroupHSPA;
    User::LeaveIfError( iBearerGroupMasks1.Append( hsdpa ) );

    // EBearerHSUPA (== EBearerHSDPA in this sense)
    TUint hsupa = hsdpa;
    User::LeaveIfError( iBearerGroupMasks1.Append( hsupa ) );

    // EBearerHSxPA (== EBearerHSDPA and EBearerHSUPA in this sense)
    TUint hsxpa = hsdpa;
    User::LeaveIfError( iBearerGroupMasks1.Append( hsxpa ) );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CConnMonBearerGroupManager::~CConnMonBearerGroupManager()
    {
    iBearerGroupMasks1.Close();
    iBearerGroupMasks2.Close();
    }

// ---------------------------------------------------------------------------
// GetBearerGroups
// ---------------------------------------------------------------------------
//
TInt CConnMonBearerGroupManager::GetBearerGroups(
        const TConnMonBearerInfo aBearer,
        TUint& aBearerGroups1,
        TUint& aBearerGroups2 )
    {
    // Check first group mask
    if ( iBearerGroupMasks1.Count() > 0 )
        {
        if ( iBearerGroupMasks1.Count() >= aBearer )
            {
            aBearerGroups1 = iBearerGroupMasks1[aBearer];
            }
        else
            {
            return KErrNotFound;
            }
        }
    else
        {
        return KErrNotFound;
        }

    // And second
    if ( iBearerGroupMasks2.Count() > 0 )
        {
        if ( iBearerGroupMasks2.Count() >= aBearer )
            {
            aBearerGroups2 = iBearerGroupMasks2[aBearer];
            }
        else
            {
            return KErrNotFound;
            }
        }

    return KErrNone;
    }

// End of file
