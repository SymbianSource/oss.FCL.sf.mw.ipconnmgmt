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
* Description:  Declaration of the CVpnApItem class.
*
*/


// INCLUDE FILES

#include    <f32file.h>
#include    <bautils.h>
#include    <barsc.h>
#include    <apengine.rsg>
#include    <txtetext.h>
#include    <data_caging_path_literals.hrh>

#include    "VpnApItem.h"
#include    "ApEngineLogger.h"
#include    "ApEngineConsts.h"
#include    "ApEngineCommons.h"



// CONSTANTS
const TInt KApItemInitialTxtBufSize = 0;
const TInt KVpnDefProxyPortNumber = 80;

// DUPLCIATE as I can not touch original, but do need the file here, too...
/// ROM drive.
_LIT( KApEngineResDriveZ, "z:" );
/// ApEngine resource file name.
_LIT( KApEngineResFileName, "ApEngine.rsc" );



// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CVpnApItem::NewLC
// ---------------------------------------------------------
//
EXPORT_C CVpnApItem* CVpnApItem::NewLC()
    {
    return NULL;
    }


// ---------------------------------------------------------
// CVpnApItem::~CVpnApItem
// ---------------------------------------------------------
//
EXPORT_C CVpnApItem::~CVpnApItem()
    {
    }

        
// ---------------------------------------------------------
// CVpnApItem::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CVpnApItem::ConstructL()
    {
    }


// ---------------------------------------------------------
// CVpnApItem::CVpnApItem
// ---------------------------------------------------------
//
EXPORT_C CVpnApItem::CVpnApItem()
    {
    }



// ---------------------------------------------------------
// CVpnApItem::CopyFromL
// ---------------------------------------------------------
//
EXPORT_C void CVpnApItem::CopyFromL( const CVpnApItem& aCopyFrom  )
    {
    }




// ---------------------------------------------------------
// CVpnApItem::operator==
// ---------------------------------------------------------
//
EXPORT_C TBool CVpnApItem::operator==( const CVpnApItem& aItem ) const
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CVpnApItem::operator!=
// ---------------------------------------------------------
//
EXPORT_C TBool CVpnApItem::operator!=( const CVpnApItem& aItem ) const
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CVpnApItem::ReadTextL
// ---------------------------------------------------------
//
EXPORT_C void CVpnApItem::ReadTextL( const TVpnApMember aColumn, 
                                     TDes16& aValue )
    {
    }



// ---------------------------------------------------------
// CVpnApItem::ReadConstLongTextL
// ---------------------------------------------------------
//
EXPORT_C const HBufC* CVpnApItem::ReadConstLongTextL( 
                                         const TVpnApMember aColumn 
                                                    )
    {
    return NULL;
    }


// ---------------------------------------------------------
// CVpnApItem::ReadUint
// ---------------------------------------------------------
//
EXPORT_C TInt CVpnApItem::ReadUint( const TVpnApMember aColumn, 
                                    TUint32& aValue )
    {
    return KErrNotSupported;
    }



// ---------------------------------------------------------
// CVpnApItem::ReadBool
// ---------------------------------------------------------
//
EXPORT_C TInt CVpnApItem::ReadBool( const TVpnApMember /*aColumn*/, 
                                    TBool& /*aValue*/ )
    {
    return KErrNotSupported;
    }




// ---------------------------------------------------------
// CVpnApItem::WriteTextL
// ---------------------------------------------------------
//
EXPORT_C TInt CVpnApItem::WriteTextL( const TVpnApMember aColumn,
                                      const TDesC16& aValue )
    {
    return KErrNotSupported;
    }



// ---------------------------------------------------------
// CVpnApItem::WriteLongTextL
// ---------------------------------------------------------
//
EXPORT_C TInt CVpnApItem::WriteLongTextL( const TVpnApMember aColumn,
                                          const TDesC& aValue )
    {
    return KErrNotSupported;
    }



// ---------------------------------------------------------
// CVpnApItem::WriteUint
// ---------------------------------------------------------
//
EXPORT_C TInt CVpnApItem::WriteUint( const TVpnApMember aColumn,
                                     const TUint32& aValue )
    {
    return KErrNotSupported;
    }



// ---------------------------------------------------------
// CVpnApItem::WriteBool
// ---------------------------------------------------------
//
EXPORT_C TInt CVpnApItem::WriteBool( const TVpnApMember /*aColumn*/,
                                     const TBool& /*aValue*/ )
    {
    return KErrNotSupported;
    }


// Query
// COMMON

// ---------------------------------------------------------
// CVpnApItem::WapUid
// ---------------------------------------------------------
//
EXPORT_C TUint32 CVpnApItem::WapUid() const
    {
    return 0;
    }


// ---------------------------------------------------------
// CVpnApItem::Name
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CVpnApItem::Name() const
    {
    User::Leave( KErrNotSupported );
    }



// ---------------------------------------------------------
// CVpnApItem::ReAllocL
// ---------------------------------------------------------
//
void CVpnApItem::ReAllocL(HBufC*& aDes, const TDesC16& aValue)
    {
    }



// ---------------------------------------------------------
// CVpnApItem::RemoveInvalidCharsLC
// ---------------------------------------------------------
//
HBufC* CVpnApItem::RemoveInvalidCharsLC( const TDesC16& aInText )
    {
    return NULL;
    }



// ---------------------------------------------------------
// CVpnApItem::HasProxy
// ---------------------------------------------------------
//
TBool CVpnApItem::HasProxy() const
    {
    return EFalse;
    }

//  End of File
