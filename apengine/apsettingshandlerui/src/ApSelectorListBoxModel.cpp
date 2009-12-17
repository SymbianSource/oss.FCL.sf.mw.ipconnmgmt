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
* Description:  Implementation of CApSelectorListboxModel.
*
*/


// INCLUDE FILES

#include "ApSelectorListBoxModel.h"
#include "ApListItem.h"
#include <ApSettingsHandlerCommons.h>

#include <featmgr.h>

#include "ApSettingsHandlerLogger.h"

// CONSTANTS
const TInt  KGPRS_ICON_ID        = 0;
// No icon for CDMA (yet?) -> use GPRS icon.
const TInt  KCDMA_ICON_ID        = KGPRS_ICON_ID;
const TInt  KCSD_ICON_ID         = 1;
const TInt  KHSCSD_ICON_ID       = 2;
// const TInt  KSMS_ICON_ID       = 3;   // DEPRECATED
const TInt  KWLAN_ICON_ID        = 4;
// fith icon element is the KEASY_WLAN_ICON_ID  but it is not shown 
// in our UI, but is used by other modules using our 
// resources!!! const TInt  KEASY_WLAN_ICON_ID   = 5;

const TInt  KPROTECTED_ICON_ID   = 6;
// get real icons !!! now using GPRS...
#ifdef __TEST_LAN_BEARER
    const TInt  KLAN_ICON_ID         = KGPRS_ICON_ID;
#endif // __TEST_LAN_BEARER


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CApSelectorListboxModel::CApSelectorListboxModel
// ---------------------------------------------------------
//
CApSelectorListboxModel::CApSelectorListboxModel( TGraphicType aGraphicType )
:CApListItemList(),
iOffset( 0 ),
iFeatureInitialised(EFalse),
iProtectionSupported(EFalse)
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelListboxModel::CApSelectorListboxModel<->")
    iGraphicType = aGraphicType;
    }

// ---------------------------------------------------------
// CApSelectorListboxModel::~CApSelectorListboxModel
// ---------------------------------------------------------
//
CApSelectorListboxModel::~CApSelectorListboxModel()
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelListboxModel::~CApSelectorListboxModel<->")
    ResetAndDestroy(); // deletes all elements...
    }

// ---------------------------------------------------------
// CApSelectorListboxModel::MdcaCount
// ---------------------------------------------------------
//
TInt CApSelectorListboxModel::MdcaCount() const
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelListboxModel::MdcaCount<->")
    
    return Count();
    }

// ---------------------------------------------------------
// CApSelectorListboxModel::MdcaPoint
// ---------------------------------------------------------
//
TPtrC CApSelectorListboxModel::MdcaPoint( TInt aIndex ) const
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelListboxModel::MdcaPoint")
    
    // We need to cast away the const-ness from the buffer.
    // It was definitely made for formatting!
    FormatListboxText( aIndex,
        ( MUTABLE_CAST(  TBuf<KMaxApSelectorListboxName>&, iBuf ) ) );

    APSETUILOGGER_LEAVEFN( EListbox,"SelListboxModel::MdcaPoint")
    return iBuf;
    }


// ---------------------------------------------------------
// CApSelectorListboxModel::Item4Uid
// ---------------------------------------------------------
//
TInt CApSelectorListboxModel::Item4Uid( TUint32 aUid, TInt& aErr ) const
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelListboxModel::Item4Uid")
    
    aErr = KErrNone;
    TInt retval( KErrNotFound );
    for ( TInt i=0; i<Count(); i++ )
        {
        if ( At( i )->Uid() == aUid )
            {
            retval = i;
            break;
            }
        }
    if ( retval == KErrNotFound )
        {
        aErr = KErrNotFound;
        }
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelListboxModel::Item4Uid")
    return retval;
    }



// ---------------------------------------------------------
// CApSelectorListboxModel::SetOwnIconOffset
// ---------------------------------------------------------
//
void CApSelectorListboxModel::SetOwnIconOffset( TInt aOffset )
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelListboxModel::SetOwnIconOffset<->")
    
    iOffset = aOffset;
    }


// ---------------------------------------------------------
// CApSelectorListboxModel::FormatListboxText
// ---------------------------------------------------------
//
void CApSelectorListboxModel::FormatListboxText( TInt aIndex,
                                                TDes& aBuf ) const
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelListboxModel::FormatListboxText")
    
    if ( !iFeatureInitialised )
        {
        TRAPD(err, FeatureManager::InitializeLibL());
        if (err == KErrNone)
            {
            CApSelectorListboxModel* tmp = CONST_CAST(CApSelectorListboxModel*, this);
            tmp->iProtectionSupported = FeatureManager::FeatureSupported( 
                                            KFeatureIdSettingsProtection );
            FeatureManager::UnInitializeLib();
            tmp->iFeatureInitialised = ETrue;
            }
        }
    switch ( iGraphicType )
        {
        case EGraphicBearer:
            {
            TInt iIconId;
            switch ( At( aIndex )->BearerType() )
                {
                case EApBearerTypeCDMA:
                    {
                    iIconId = KCDMA_ICON_ID;
                    break;
                    }
                case EApBearerTypeGPRS:
                    {
                    iIconId = KGPRS_ICON_ID;
                    break;
                    }
                case EApBearerTypeCSD:
                    {
                    iIconId = KCSD_ICON_ID;
                    break;
                    }
                case EApBearerTypeHSCSD:
                    {
                    iIconId = KHSCSD_ICON_ID;
                    break;
                    }
                case EApBearerTypeWLAN:
                    {
                    iIconId = KWLAN_ICON_ID;
                    break;
                    }
#ifdef __TEST_LAN_BEARER
                case EApBearerTypeLAN:
                    {
                    iIconId = KLAN_ICON_ID;
                    break;
                    }
#endif // __TEST_LAN_BEARER
                case EApBearerTypeLANModem:
                    { // LANMODEM get real icon ID
                    iIconId = KWLAN_ICON_ID;
                    break;
                    }
                default:
                    {
                    // in release, shall not leave here,
                    // it is checked when filled...
                    __ASSERT_DEBUG( EFalse, Panic( EInvalidBearerType ) );
                    iIconId = KGPRS_ICON_ID;
                    break;
                    }
                }
            if ( iProtectionSupported )
                {
                if ( At( aIndex )->IsReadOnly() )
                    {
                    TInt iProtectedIconId = KPROTECTED_ICON_ID;
                    aBuf.Format(
                                KListItemFormatGraphicProtected,
                                iOffset+iIconId,
                                Min( At( aIndex )->Name().Length(),
                                KMaxListItemNameLength ),
                                &At( aIndex )->Name(),
                                iOffset+iProtectedIconId
                                );
                    }
                else
                    { // protection supported, but not protected
                    aBuf.Format(
                                KListItemFormatGraphicBearer,
                                iOffset+iIconId,
                                Min( At( aIndex )->Name().Length(),
                                KMaxListItemNameLength ),
                                &At( aIndex )->Name()
                                );
                    }
                }
            else
                { // protection not supported
                aBuf.Format(
                            KListItemFormatGraphicBearer,
                            iOffset+iIconId,
                            Min( At( aIndex )->Name().Length(),
                            KMaxListItemNameLength ),
                            &At( aIndex )->Name()
                            );
                }
            break;
            }
        case EGraphicNone:
            {
            aBuf.Format(
                        KListItemFormatGraphicNone,
                        Min( At( aIndex )->Name().Length(),
                        KMaxListItemNameLength ),
                        &At( aIndex )->Name()
                        );
            break;
            }
        case EGraphicOnOff:
            {
            if ( iProtectionSupported )
                {
                TInt ProtectedIconId( 0 );
                if ( At( aIndex )->IsReadOnly() )
                    {
                    ProtectedIconId = KPROTECTED_ICON_ID;
                    aBuf.Format( 
                                KListItemFormatGraphicOnOffWithLock,
                                1, 
                                Min( At( aIndex )->Name().Length(), 
                                KMaxListItemNameLength ), 
                                &At( aIndex )->Name(),
                                iOffset + ProtectedIconId
                                );
                    }
                else
                    {
                    aBuf.Format(
                            KListItemFormatGraphicOnOff, 
                            1, 
                            Min( At( aIndex )->Name().Length(), 
                            KMaxListItemNameLength ), 
                            &At( aIndex )->Name() 
                            ); // magic Number, does not works with anything else
                    }
                }
            else
                {
                aBuf.Format(
                            KListItemFormatGraphicOnOff,
                            1,
                            Min( At( aIndex )->Name().Length(),
                            KMaxListItemNameLength ),
                            &At( aIndex )->Name()
                            );
                }
            break;
            }
        case EGraphicProtection:
            {
            if ( iProtectionSupported )
                {
                TInt ProtectedIconId( 0 );
                if ( At( aIndex )->IsReadOnly() )
                    {
                    ProtectedIconId = 0;
                    aBuf.Format( 
                                KListItemFormatGraphicWithLock,
                                Min( At( aIndex )->Name().Length(), 
                                KMaxListItemNameLength ), 
                                &At( aIndex )->Name(),
                                iOffset + ProtectedIconId
                                );
                    }
                else
                    {
                    aBuf.Format(
                                KListItemFormatGraphicNone, 
                                Min( At( aIndex )->Name().Length(), 
                                KMaxListItemNameLength ), 
                                &At( aIndex )->Name() 
                                ); 
                    }                
                }
            else
                {
                aBuf.Format(
                            KListItemFormatGraphicNone, 
                            Min( At( aIndex )->Name().Length(), 
                            KMaxListItemNameLength ), 
                            &At( aIndex )->Name() 
                            );
                }
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EInvalidBitmapType ) );
            break;
            }
        }
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelListboxModel::FormatListboxText")
    }

// End of File
