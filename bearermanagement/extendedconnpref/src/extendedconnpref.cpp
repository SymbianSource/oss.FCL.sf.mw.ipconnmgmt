/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Extended Connection Preferences interface
*
*/


/**
@file extendedconnpref.cpp
Extended Connection Preferences interface.
*/

// INCLUDE FILES
#include "extendedconnpref.h"
#include <comms-infras/metatype.h>

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TExtendedConnPref, TExtendedConnPref::EUid, TExtendedConnPref::ETypeId )
    REGISTER_ATTRIBUTE( TExtendedConnPref, iSnapPurpose, TMetaNumber )
    REGISTER_ATTRIBUTE( TExtendedConnPref, iSnapId, TMetaNumber )
    REGISTER_ATTRIBUTE( TExtendedConnPref, iIapId, TMetaNumber )
    REGISTER_ATTRIBUTE( TExtendedConnPref, iBearerSet, TMetaNumber )
    REGISTER_ATTRIBUTE( TExtendedConnPref, iNoteBehaviour, TMetaNumber )
    REGISTER_ATTRIBUTE( TExtendedConnPref, iDisconnectDialog, TMetaNumber )
    REGISTER_ATTRIBUTE( TExtendedConnPref, iConnSelectionDialog, TMetaNumber )
    REGISTER_ATTRIBUTE( TExtendedConnPref, iForcedRoaming, TMetaNumber )
END_ATTRIBUTE_TABLE()


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TExtendedConnPref::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C TExtendedConnPref::TExtendedConnPref():
    iSnapPurpose(CMManager::ESnapPurposeUnknown),
    iSnapId(0),
    iIapId(0),
    iBearerSet( EExtendedConnBearerUnknown ),
    iNoteBehaviour( ENoteBehaviourDefault ),
    iDisconnectDialog( ETrue ),
    iConnSelectionDialog( EFalse ),
    iForcedRoaming( ETrue )
    {
    }

// -----------------------------------------------------------------------------
// TExtendedConnPref::SetSnapPurpose
// -----------------------------------------------------------------------------
//
EXPORT_C void TExtendedConnPref::SetSnapPurpose( CMManager::TSnapPurpose aSnapPurpose )
    {
    iSnapPurpose = aSnapPurpose;
    }

// -----------------------------------------------------------------------------
// TExtendedConnPref::SnapPurpose
// -----------------------------------------------------------------------------
//
EXPORT_C CMManager::TSnapPurpose TExtendedConnPref::SnapPurpose() const
    {
    return iSnapPurpose;
    }

// -----------------------------------------------------------------------------
// TExtendedConnPref::SetSnapId
// -----------------------------------------------------------------------------
//
EXPORT_C void TExtendedConnPref::SetSnapId( TUint32 aSnapId )
    {
    iSnapId = aSnapId;
    }

// -----------------------------------------------------------------------------
// TExtendedConnPref::SnapId
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 TExtendedConnPref::SnapId() const
    {
    return iSnapId;
    }

// -----------------------------------------------------------------------------
// TExtendedConnPref::SetIapId
// -----------------------------------------------------------------------------
//
EXPORT_C void TExtendedConnPref::SetIapId( TUint32 aIapId )
    {
    iIapId = aIapId;
    }

// -----------------------------------------------------------------------------
// TExtendedConnPref::IapId
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 TExtendedConnPref::IapId() const
    {
    return iIapId;
    }

// -----------------------------------------------------------------------------
// TExtendedConnPref::SetBearerSet
// -----------------------------------------------------------------------------
//
EXPORT_C void TExtendedConnPref::SetBearerSet( const TUint32 aBearerSet )
    {
    iBearerSet = aBearerSet;
    }

// -----------------------------------------------------------------------------
// TExtendedConnPref::BearerSet
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 TExtendedConnPref::BearerSet() const
    {
    return iBearerSet;
    }

// -----------------------------------------------------------------------------
// TExtendedConnPref::SetNoteBehaviour
// -----------------------------------------------------------------------------
//
EXPORT_C void TExtendedConnPref::SetNoteBehaviour( const TUint32 aNoteBehaviour )
    {
    iNoteBehaviour = aNoteBehaviour;
    }

// -----------------------------------------------------------------------------
// TExtendedConnPref::NoteBehaviour
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 TExtendedConnPref::NoteBehaviour() const
    {
    return iNoteBehaviour;
    }

// -----------------------------------------------------------------------------
// TExtendedConnPref::SetDisconnectDialog
// -----------------------------------------------------------------------------
//
EXPORT_C void TExtendedConnPref::SetDisconnectDialog( TBool aDisconnectDialog )
    {
    iDisconnectDialog = aDisconnectDialog;
    }

// -----------------------------------------------------------------------------
// TExtendedConnPref::DisconnectDialog
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TExtendedConnPref::DisconnectDialog() const
    {
    return iDisconnectDialog;
    }

// -----------------------------------------------------------------------------
// TExtendedConnPref::SetConnSelectionDialog
// -----------------------------------------------------------------------------
//
EXPORT_C void TExtendedConnPref::SetConnSelectionDialog( TBool aConnSelectionDialog )
    {
    iConnSelectionDialog = aConnSelectionDialog;
    }

// -----------------------------------------------------------------------------
// TExtendedConnPref::ConnSelectionDialog
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TExtendedConnPref::ConnSelectionDialog() const
    {
    return iConnSelectionDialog;
    }

// -----------------------------------------------------------------------------
// TExtendedConnPref::SetForcedRoaming
// -----------------------------------------------------------------------------
//
EXPORT_C void TExtendedConnPref::SetForcedRoaming( TBool aForcedRoaming )
    {
    iForcedRoaming = aForcedRoaming;
    }

// -----------------------------------------------------------------------------
// TExtendedConnPref::ForcedRoaming
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TExtendedConnPref::ForcedRoaming() const
    {
    return iForcedRoaming;
    }

//  End of File
