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
#include    <ApEngine.rsg>
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
    CLOG( ( EApItem, 0, _L( "-> CVpnApItem::NewL" ) ) );

    CVpnApItem* self = new ( ELeave ) CVpnApItem;
    CleanupStack::PushL( self );
    self->ConstructL();

    CLOG( ( EApItem, 1, _L( "<- CVpnApItem::NewL" ) ) );
    
    return self;
    }


// ---------------------------------------------------------
// CVpnApItem::~CVpnApItem
// ---------------------------------------------------------
//
EXPORT_C CVpnApItem::~CVpnApItem()
    {
    CLOG( ( EApItem, 0, _L( "<-> CVpnApItem::~CVpnApItem" ) ) );

    delete iName;
    delete iPolicy;
    delete iProxyServerAddress;
    delete iRealIapName;
    delete iPolicyId;
    delete iVpnIapNetworkName;

    }

        
// ---------------------------------------------------------
// CVpnApItem::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CVpnApItem::ConstructL()
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApItem::ConstructL" ) ) );
   
    // as we are in ConstructL called from NewLC,
    // 'this' is already on the CleanupStack,
    // so do not push members onto it!
    iName = HBufC::NewL( KApItemInitialTxtBufSize );
    iPolicy = HBufC::NewL( KApItemInitialTxtBufSize );
    iProxyServerAddress = HBufC::NewL( KApItemInitialTxtBufSize );
    iRealIapName = HBufC::NewL( KApItemInitialTxtBufSize );
    iPolicyId = HBufC::NewL( KApItemInitialTxtBufSize );
    iVpnIapNetworkName = HBufC::NewL( KApItemInitialTxtBufSize );

    RFs fs;
    CleanupClosePushL<RFs>( fs );
    User::LeaveIfError( fs.Connect() );

    // can't use resource here because it is not added yet....
    TFileName resourceFile;
    resourceFile.Append( KApEngineResDriveZ );
    resourceFile.Append( KDC_RESOURCE_FILES_DIR );
    resourceFile.Append( KApEngineResFileName );
    BaflUtils::NearestLanguageFile( fs, resourceFile );

    RResourceFile rf;
    CleanupClosePushL<RResourceFile>( rf );
    rf.OpenL( fs, resourceFile );
    rf.ConfirmSignatureL( 0 );
    HBufC8* readBuffer = rf.AllocReadLC( R_APNG_DEFAULT_VPN_AP_NAME );
    // as we are expecting HBufC16...
    __ASSERT_DEBUG( ( readBuffer->Length()%2 ) == 0,
                    ApCommons::Panic( EWrongResourceFormat ) );
    const TPtrC16 ptrReadBuffer( (TText16*) readBuffer->Ptr(),
                                 ( readBuffer->Length() + 1 ) >> 1 );
    HBufC16* textBuffer=HBufC16::NewL( ptrReadBuffer.Length() );
    *textBuffer=ptrReadBuffer;
    CleanupStack::PopAndDestroy( readBuffer ); // readbuffer
    CleanupStack::PushL( textBuffer );
    
    WriteTextL( EApVpnWapAccessPointName, *textBuffer );

    WriteUint( EApVpnProxyPortNumber, KVpnDefProxyPortNumber );

    CleanupStack::PopAndDestroy( 3 ); // textbuffer, fs, rf

    CLOG( ( EApItem, 0, _L( "<- CVpnApItem::ConstructL" ) ) );

    }


// ---------------------------------------------------------
// CVpnApItem::CVpnApItem
// ---------------------------------------------------------
//
EXPORT_C CVpnApItem::CVpnApItem()
    {
    CLOG( ( EApItem, 0, _L( "<-> CVpnApItem::CVpnApItem" ) ) );
    }



// ---------------------------------------------------------
// CVpnApItem::CopyFromL
// ---------------------------------------------------------
//
EXPORT_C void CVpnApItem::CopyFromL( const CVpnApItem& aCopyFrom  )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApItem::CopyFromL" ) ) );

    iVpnTableId = aCopyFrom.iVpnTableId;
    iVpnIapId = aCopyFrom.iVpnIapId;
    iRealIapId = aCopyFrom.iRealIapId;
    iVpnRealWapId = aCopyFrom.iVpnRealWapId;
    iVirtualBearerID = aCopyFrom.iVirtualBearerID;
    iProxyPort = aCopyFrom.iProxyPort;
    iHomeNetwork = aCopyFrom.iHomeNetwork;

    WriteTextL( EApVpnWapAccessPointName, *aCopyFrom.iName );
    WriteTextL( EApVpnPolicy, *aCopyFrom.iPolicy );
    WriteLongTextL( EApVpnProxyServerAddress, *aCopyFrom.iProxyServerAddress );
    WriteTextL( EApVpnRealIapName, *aCopyFrom.iRealIapName );
    WriteTextL( EApVpnPolicyId, *aCopyFrom.iPolicyId );
    WriteTextL( EApVpnIapNetworkName, *aCopyFrom.iVpnIapNetworkName );

    CLOG( ( EApItem, 0, _L( "<- CVpnApItem::CopyFromL" ) ) );
    }




// ---------------------------------------------------------
// CVpnApItem::operator==
// ---------------------------------------------------------
//
EXPORT_C TBool CVpnApItem::operator==( const CVpnApItem& aItem ) const
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApItem::operator==" ) ) );

    TBool retval( EFalse );

    if ( ( iVpnTableId == aItem.iVpnTableId ) &&
         ( iVpnIapId == aItem.iVpnIapId ) &&
         ( iRealIapId == aItem.iRealIapId ) &&
         ( iVpnRealWapId == aItem.iVpnRealWapId ) &&
         ( iVirtualBearerID == aItem.iVirtualBearerID ) &&
         ( iProxyPort == aItem.iProxyPort ) &&
         ( iHomeNetwork == aItem.iHomeNetwork )
         )
        {
        if (
           ( iName->Compare( *aItem.iName ) == 0 )
           && ( iPolicy->Compare( *aItem.iPolicy ) == 0 )
           && ( iProxyServerAddress->Compare( 
                            *aItem.iProxyServerAddress ) == 0 )
           && ( iRealIapName->Compare( *aItem.iRealIapName ) == 0 )
           && ( iPolicyId->Compare( *aItem.iPolicyId ) == 0 )
           && ( iVpnIapNetworkName->Compare( 
                            *aItem.iVpnIapNetworkName ) == 0 )
            )
            {
            retval = ETrue;
            }
        }

    CLOG( ( EApItem, 0, _L( "<- CVpnApItem::operator==" ) ) );

    return ( retval );
    }


// ---------------------------------------------------------
// CVpnApItem::operator!=
// ---------------------------------------------------------
//
EXPORT_C TBool CVpnApItem::operator!=( const CVpnApItem& aItem ) const
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApItem::operator!=" ) ) );

    TBool ret = ( *this == aItem );

    CLOG( ( EApItem, 1, _L( "<- CVpnApItem::operator!=" ) ) );
    return ( !ret );
    }



// ---------------------------------------------------------
// CVpnApItem::ReadTextL
// ---------------------------------------------------------
//
EXPORT_C void CVpnApItem::ReadTextL( const TVpnApMember aColumn, 
                                     TDes16& aValue )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApItem::ReadTextL" ) ) );

    switch ( aColumn )
        {
        case EApVpnWapAccessPointName:
            {
            aValue.SetLength( iName->Length() );
            aValue.Format( KFormat, iName );
            break;
            }
        case EApVpnProxyServerAddress:
            {
            // make it large enough to hold
            aValue.SetLength( iProxyServerAddress->Length() );
            aValue.Format( KFormat, iProxyServerAddress );
            break;
            }
        case EApVpnPolicy:
            {
            aValue.SetLength( iPolicy->Length() );
            aValue.Format( KFormat, iPolicy );
            break;
            }
        case EApVpnRealIapName:
            {
            aValue.SetLength( iRealIapName->Length() );
            aValue.Format( KFormat, iRealIapName );
            break;
            }
        case EApVpnPolicyId:
            {
            aValue.SetLength( iPolicyId->Length() );
            aValue.Format( KFormat, iPolicyId );
            break;
            }
        case EApVpnIapNetworkName:
            {
            aValue.SetLength( iVpnIapNetworkName->Length() );
            aValue.Format( KFormat, iVpnIapNetworkName );
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
            User::Leave( KErrInvalidColumn );
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CVpnApItem::ReadTextL" ) ) );
    }



// ---------------------------------------------------------
// CVpnApItem::ReadConstLongTextL
// ---------------------------------------------------------
//
EXPORT_C const HBufC* CVpnApItem::ReadConstLongTextL( 
                                         const TVpnApMember aColumn 
                                                    )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApItem::ReadConstLongTextL" ) ) );

    HBufC* retval = NULL;
    switch ( aColumn )
        {
        case EApVpnProxyServerAddress:
            {
            retval = iProxyServerAddress;
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
            User::Leave( KErrInvalidColumn );
            break;
            }
        }
    
    CLOG( ( EApItem, 1, _L( "<- CVpnApItem::ReadConstLongTextL" ) ) );
    return retval;
    }


// ---------------------------------------------------------
// CVpnApItem::ReadUint
// ---------------------------------------------------------
//
EXPORT_C TInt CVpnApItem::ReadUint( const TVpnApMember aColumn, 
                                    TUint32& aValue )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApItem::ReadUint" ) ) );

    TInt retval ( KErrNone );
    switch ( aColumn )
    {
        case EApVpnWapAccessPointID:
            {
            aValue = iVpnWapId;
            break;
            }
        case EApVpnIapID:
            {
            aValue = iVpnIapId;
            break;
            }
        case EApVpnRealIapID:
            {
            aValue = iRealIapId;
            break;
            }
        case EApVpnRealWapID:
            {
            aValue = iVpnRealWapId;
            break;
            }
        case EApVpnProxyPortNumber:
            {
            aValue = iProxyPort;
            break;
            }
        case EApVpnHomeNetworkID:
            {
            aValue = iHomeNetwork;
            break;
            }
        case EApVpnIapNetworkID:
            {
            aValue = iVpnIapNetwork;
            break;
            }
        default :
            {
            __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
            retval = KErrInvalidColumn;
            break;
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CVpnApItem::ReadUint" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// CVpnApItem::ReadBool
// ---------------------------------------------------------
//
EXPORT_C TInt CVpnApItem::ReadBool( const TVpnApMember /*aColumn*/, 
                                    TBool& /*aValue*/ )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApItem::ReadBool" ) ) );
    
    // This is just a placeholder API for possible expansion...

    TInt retval( KErrNone );

    __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
    retval = KErrInvalidColumn;

    CLOG( ( EApItem, 1, _L( "<- CVpnApItem::ReadBool" ) ) );
    return retval;
    }




// ---------------------------------------------------------
// CVpnApItem::WriteTextL
// ---------------------------------------------------------
//
EXPORT_C TInt CVpnApItem::WriteTextL( const TVpnApMember aColumn,
                                      const TDesC16& aValue )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApItem::WriteTextL" ) ) );

    TInt retval( KErrNone );
    switch ( aColumn )
    {
        case EApVpnWapAccessPointName:
            {
            HBufC* sgd = aValue.AllocLC();
            sgd->Des().Trim();
            ReAllocL( iName, *sgd);
            CleanupStack::PopAndDestroy( sgd ); // sgd
            break;
            }
        case EApVpnPolicy:
            {
            ReAllocL( iPolicy, aValue);
            break;
            }
        case EApVpnProxyServerAddress:
            {
            ReAllocL( iProxyServerAddress, aValue);
            break;
            }
        case EApVpnRealIapName:
            {
            ReAllocL( iRealIapName, aValue );
            break;
            }
        case EApVpnPolicyId:
            {
            ReAllocL( iPolicyId, aValue );
            break;
            }
        case EApVpnIapNetworkName:
            {
            ReAllocL( iVpnIapNetworkName, aValue );
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
            retval = KErrInvalidColumn;
            break;
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CVpnApItem::WriteTextL" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// CVpnApItem::WriteLongTextL
// ---------------------------------------------------------
//
EXPORT_C TInt CVpnApItem::WriteLongTextL( const TVpnApMember aColumn,
                                          const TDesC& aValue )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApItem::WriteLongTextL" ) ) );

    TInt retval( KErrNone );
    switch ( aColumn )
        {
        case EApVpnProxyServerAddress:
            {
            ReAllocL( iProxyServerAddress, aValue);
            break;
            }
        default:
            {
            // unknown column...
            __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
            retval = KErrInvalidColumn;
            break;
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CVpnApItem::WriteLongTextL" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// CVpnApItem::WriteUint
// ---------------------------------------------------------
//
EXPORT_C TInt CVpnApItem::WriteUint( const TVpnApMember aColumn,
                                     const TUint32& aValue )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApItem::WriteUint" ) ) );

    TInt retval( KErrNone );
    switch ( aColumn )
        {
        case EApVpnWapAccessPointID:
            {
            iVpnWapId = aValue;
            break;
            }
        case EApVpnIapID:
            {
            iVpnIapId = aValue;
            break;
            }
        case EApVpnRealIapID:
            {
            iRealIapId= aValue;
            break;
            }
        case EApVpnRealWapID:
            {
            iVpnRealWapId = aValue;
            break;
            }
        case EApVpnProxyPortNumber:
            {
            iProxyPort = aValue;
            break;
            }
        case EApVpnHomeNetworkID:
            {
            iHomeNetwork = aValue;
            break;
            }
        case EApVpnIapNetworkID:
            {
            iVpnIapNetwork = aValue;
            break;
            }
        default :
            {
            __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
            retval = KErrInvalidColumn;
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CVpnApItem::WriteUint" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// CVpnApItem::WriteBool
// ---------------------------------------------------------
//
EXPORT_C TInt CVpnApItem::WriteBool( const TVpnApMember /*aColumn*/,
                                     const TBool& /*aValue*/ )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApItem::WriteBool" ) ) );

    TInt retval( KErrNone );

    // This is just a placeholder API for possible expansion...
    
    __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
    retval = KErrInvalidColumn;

    CLOG( ( EApItem, 1, _L( "<- CVpnApItem::WriteBool" ) ) );
    return retval;
    }


// Query
// COMMON

// ---------------------------------------------------------
// CVpnApItem::WapUid
// ---------------------------------------------------------
//
EXPORT_C TUint32 CVpnApItem::WapUid() const
    {
    CLOG( ( EApItem, 0, _L( "<-> CVpnApItem::WapUid" ) ) );

    return iVpnWapId;
    }


// ---------------------------------------------------------
// CVpnApItem::Name
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CVpnApItem::Name() const
    {
    CLOG( ( EApItem, 0, _L( "<-> CVpnApItem::Name" ) ) );

    return *iName;
    }



// ---------------------------------------------------------
// CVpnApItem::ReAllocL
// ---------------------------------------------------------
//
void CVpnApItem::ReAllocL(HBufC*& aDes, const TDesC16& aValue)
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApItem::ReAllocL" ) ) );

    // now remove 'invalid' chars (TABs) as they cause problems
    // in other places, like listboxes...
    HBufC* tmp = RemoveInvalidCharsLC( aValue );
    delete aDes;
    aDes = tmp;
    CleanupStack::Pop( tmp );

    CLOG( ( EApItem, 1, _L( "<- CVpnApItem::ReAllocL" ) ) );
    }



// ---------------------------------------------------------
// CVpnApItem::RemoveInvalidCharsLC
// ---------------------------------------------------------
//
HBufC* CVpnApItem::RemoveInvalidCharsLC( const TDesC16& aInText )
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApItem::RemoveInvalidCharsLC" ) ) );

    TInt size = aInText.Length();
    HBufC* aOutText = HBufC::NewLC( size );
    TPtr ptr = aOutText->Des();

    for ( TInt ii=0; ii<size; ++ii )
        {
        TText ch = aInText[ii];
        if ( ch == CEditableText::ETabCharacter )
            {
            ch = ' ';
            }
        ptr.Append( ch );
        }

    CLOG( ( EApItem, 1, _L( "<- CVpnApItem::RemoveInvalidCharsLC" ) ) );
    return aOutText;
    }



// ---------------------------------------------------------
// CVpnApItem::HasProxy
// ---------------------------------------------------------
//
TBool CVpnApItem::HasProxy() const
    {
    CLOG( ( EApItem, 0, _L( "-> CVpnApItem::HasProxy" ) ) );
    
    TBool retval ( EFalse );

    if ( ( iProxyServerAddress->Compare( KDynIpAddress ) 
         && iProxyServerAddress->Compare( KEmpty ) )
         && ( iProxyPort != 0 ) )
        {
        retval = ETrue;
        }

    CLOG( ( EApItem, 0, _L( "<- CVpnApItem::HasProxy" ) ) );

    return retval;
    }

//  End of File
