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
* Description: 
*     Defines dialog for access point view/edit.
*
*/


// INCLUDE FILES
#include "APSettingsHandlerUIVariant.hrh"


#include <aknnavide.h>
#include <akntitle.h>
#include <AknQueryDialog.h>
#include <akntextsettingpage.h>
#include <aknpopupsettingpage.h>
#include <aknradiobuttonsettingpage.h>
#include <aknpasswordsettingpage.h>
#include <aknmfnesettingpage.h>

#include <ApAccessPointItem.h>
#include <utf.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <nifvar_internal.h>
#endif


#include "ApSettingsLookups.h"
#include "ApSettingsDlg.h"
#include "ApSettingsModel.h"
#include <apsetui.rsg>

#include "ApSettingsHandlerUI.hrh"
#include "ApsettingshandleruiImpl.h"
#include "ApSettingsHandlerConsts.h"
#include <ApSettingsHandlerCommons.h>

#include "ApSettingsHandlerLogger.h"

#include <commonphoneparser.h>

#include "ApNetSelPopupList.h"

#include "AccessPointTextSettingPage.h"


#include <featmgr.h>

#include "FeatureManagerWrapper.h"
#include "ApMultiLineDataQueryDialog.h"

#include <ConnectionUiUtilities.h>
#include <wlanmgmtcommon.h>
#include <centralrepository.h>

// CONSTANTS
// General Settings UID, Hardcoded
// const TUid KGeneralSettingsUid={0X100058EC};
//        const TInt KOptionalDefaultValue    = 1;
        


const TInt KIspCallAnalogue         = 0;
const TInt KIspCallIsdn110          = 1;
const TInt KIspCallIsdn120          = 2;

const TInt KBearerListGprs          = 0;
const TInt KBearerListWLAN          = 1;
const TInt KBearerListCSD           = 2;
const TInt KBearerListHSCSD         = 3;



// They provide the indexes.
const TInt KBearerArrayCSD           = 0;
const TInt KBearerArrayGPRS          = 1;
const TInt KBearerArrayHSCSD         = 2;
const TInt KBearerArrayWLAN          = 3;

const TInt KBoolMaxNumber           = 1;

_LIT( KEmptyText, "" );
_LIT( KTxtListItemFormat, " \t%S\t\t%S" );
_LIT( KTxtMenuListItemFormat, " \t%S\t\t" );
_LIT( KTxtCompulsory, "\t*" );

_LIT( KTxtNumber, "%d" );


_LIT( KKnownNameServer1, "fec0:000:0000:ffff::1" );
_LIT( KKnownNameServer2, "fec0:000:0000:ffff::2" );


const TInt KStaredPasswordLength = 4;

const TUint32 KDefAdhocChannel = 7;


const TInt  KItemIndex0 = 0;
const TInt  KItemIndex1 = 1;
const TInt  KItemIndex2 = 2;
const TInt  KItemIndex3 = 3;

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CApSettingsDlg::ConstructAndRunLD
// Constructs the dialog and runs it.
// ---------------------------------------------------------
//
TInt CApSettingsDlg::ConstructAndRunLD( CApSettingsModel& aModel,
                                        CApSettingsHandlerImpl& aHandler,
                                        TUint32& aUid,
                                        CApAccessPointItem* aItem )
    {
    return 0;
    }



// Destructor
CApSettingsDlg::~CApSettingsDlg()
    {
    }



// ---------------------------------------------------------
// CApSettingsDlg::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
//                                 TEventCode aType )
//  Handles key events
// ---------------------------------------------------------
//
TKeyResponse CApSettingsDlg::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                             TEventCode aType )
    {
    User::Leave( KErrNotSupported );
    }



// ---------------------------------------------------------
// CApSettingsDlg::NewL
// Two-phase dconstructor, second phase is ConstructAndRunLD
// ---------------------------------------------------------
//
CApSettingsDlg* CApSettingsDlg::NewL( TInt aIspFilter,
                                      TInt aBearerFilter,
                                      TInt aSortType,
                                      TUint32& aEventStore,
                                      TInt aReqIpvType
                                      )
    {
    return NULL;
    }




// Constructor
CApSettingsDlg::CApSettingsDlg( TInt aIspFilter, TInt aBearerFilter, 
                                TInt aSortType, TUint32& aEventStore,
                                TInt aReqIpvType )
:
iLevel( 0 ),
iOldIndex( 0 ),
iOldTopIndex( 0 ),
iAdvIndex( 0 ),
iBearerType( EApBearerTypeAll ),
iIspFilter( aIspFilter ),
iBearerFilter( aBearerFilter ),
iSortType( aSortType ),
iEventStore( &aEventStore ),
iFieldCount( 0 ),
iAdvancedFieldCount( 0 ),
iExitReason( EExitNone ),
iReqIpvType( aReqIpvType ),
iL2Ipv4( ETrue ),
iSecSettingsExitReason(0)
    {
    }


// ---------------------------------------------------------
// CApSettingsDlg::OkToExitL( TInt aButtonId)
// called by framework when the OK button is pressed
// ---------------------------------------------------------
//
TBool CApSettingsDlg::OkToExitL( TInt aButtonId )
    {
    return EFalse;
    }




// ---------------------------------------------------------
// CApSettingsDlg::InitTextsL
// called before the dialog is shown
// to initialize localized textual data
// ---------------------------------------------------------
//
void CApSettingsDlg::InitTextsL()
    {
    }




// ---------------------------------------------------------
// CApSettingsDlg::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CApSettingsDlg::DynInitMenuPaneL
( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    }



// ---------------------------------------------------------
// CApSettingsDlg::HandleListboxDataChangeL
// called before the dialog is shown to initialize listbox data
// ---------------------------------------------------------
//
void CApSettingsDlg::HandleListboxDataChangeL( TBool aReRead )
    {
    }



// ---------------------------------------------------------
// CApSettingsDlg::SetTextOverrides
// ---------------------------------------------------------
//
void CApSettingsDlg::SetTextOverrides( CTextOverrides* aOverrides )
    {
    }



// ---------------------------------------------------------
// CApSettingsDlg::HandleApDbEventL
// called by the active access point framework
// ---------------------------------------------------------
//
void CApSettingsDlg::HandleApDbEventL( TEvent anEvent )
    {
    }



// ---------------------------------------------------------
// CApSettingsDlg::GetHelpContext()
// ---------------------------------------------------------
//
void CApSettingsDlg::GetHelpContext(TCoeHelpContext& aContext) const
    {
    }


// ---------------------------------------------------------
// CApSettingsDlg::PreLayoutDynInitL();
// called by framework before dialog is shown
// ---------------------------------------------------------
//
void CApSettingsDlg::PreLayoutDynInitL()
    {
    }




// From MEikCommandObserver
void CApSettingsDlg::ProcessCommandL( TInt aCommandId )
    {
    }


// From MEikListBoxObserver
void CApSettingsDlg::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                          TListBoxEvent aEventType )
    {
    }




//----------------------------------------------------------
// CApSettingsDlg::FillListWithDataL
//----------------------------------------------------------
//
void CApSettingsDlg::FillListWithDataL( CDesCArrayFlat& aItemArray,
                                        const TApMember& arr, TInt aLength,
                                        const TInt* aRes,
                                        const TInt* aCompulsory )
    {
    }


//----------------------------------------------------------
// CApSettingsDlg::AddTextualListBoxItemL
//----------------------------------------------------------
//
void CApSettingsDlg::AddTextualListBoxItemL( CDesCArrayFlat& aItemArray,
                                             TApMember aMember, TInt aRes,
                                             TInt aCompulsory )
    {
    }


//----------------------------------------------------------
// CApSettingsDlg::AddOnOffListBoxItem
//----------------------------------------------------------
//
void CApSettingsDlg::AddOnOffListBoxItemL( CDesCArrayFlat& aItemArray,
                                           TApMember aMember, TInt aRes,
                                           TInt aCompulsory )
    {
    }


//----------------------------------------------------------
// CApSettingsDlg::UpdateOnOffListBoxItemL
//----------------------------------------------------------
//
void CApSettingsDlg::UpdateOnOffListBoxItemL( TApMember aMember, TInt aRes, 
                                              TInt aPos, TInt aCompulsory )
    {
    }


//----------------------------------------------------------
// CApSettingsDlg::TextualListBoxItemL
//----------------------------------------------------------
//
void CApSettingsDlg::UpdateTextualListBoxItemL( TApMember aMember, TInt aRes,
                                                TInt aPos, TInt aCompulsory  )
    {
    }


//----------------------------------------------------------
// CApSettingsDlg::CreateTextualListBoxItem
//----------------------------------------------------------
//
HBufC*  CApSettingsDlg::CreateTextualListBoxItemL( TApMember aMember, 
                                                   TInt aRes, 
                                                   TInt aCompulsory )
    {
    return NULL;
    }



//----------------------------------------------------------
// CApSettingsDlg::CreateOnOffListBoxItemL
//----------------------------------------------------------
//
HBufC*  CApSettingsDlg::CreateOnOffListBoxItemL( TApMember aMember, TInt aRes,
                                                 TInt aCompulsory )
    {
    return NULL;
    }



//----------------------------------------------------------
// CApSettingsDlg::ChangeSettingsL
//----------------------------------------------------------
//
void CApSettingsDlg::ChangeSettingsL( TBool aQuick )
    {
    }



//----------------------------------------------------------
// CApSettingsDlg::SetBearerTypeL
//----------------------------------------------------------
//
void CApSettingsDlg::SetBearerTypeL( TApBearerType aBearer )
    {
    }



//----------------------------------------------------------
// CApSettingsDlg::InvertSettingsL
//----------------------------------------------------------
//
void CApSettingsDlg::InvertSettingsL( TApMember aDataMember )
    {
    }


//----------------------------------------------------------
// CApSettingsDlg::ShowPopupSettingPageL
//----------------------------------------------------------
//
TBool CApSettingsDlg::ShowPopupSettingPageL( TApMember aData )
    {
    return EFalse;
    }







//----------------------------------------------------------
// CApSettingsDlg::ShowPopupTextSettingPageL
//----------------------------------------------------------
//
TBool CApSettingsDlg::ShowPopupTextSettingPageL( TApMember aData )
    {
    return EFalse;
    }



//----------------------------------------------------------
// CApSettingsDlg::GetTextLC
//----------------------------------------------------------
//
HBufC* CApSettingsDlg::GetTextLC( TApMember aData )
    {
    return NULL;
    }


//----------------------------------------------------------
// CApSettingsDlg::GetTextResId
//----------------------------------------------------------
//
void CApSettingsDlg::GetTextResId( TApMember aData, TInt& apage, 
                                   TInt& aTextSettingPageFlags )
    {
    }



//----------------------------------------------------------
// CApSettingsDlg::GetResId
//----------------------------------------------------------
//
void CApSettingsDlg::GetResId( TApMember aData, TInt& aattr )
    {
    }




// ---------------------------------------------------------
// CApSettingsDlg::FillPopupSettingPageL
// ---------------------------------------------------------
//
CDesCArrayFlat* CApSettingsDlg::FillPopupSettingPageLC( TApMember aData, 
                                                        TInt& aCurrvalue )
    {
    return NULL;
    }




// ---------------------------------------------------------
// CApSettingsDlg::UpdateFromPopupSettingPageL
// ---------------------------------------------------------
//
void CApSettingsDlg::UpdateFromPopupSettingPageL( TApMember aData, 
                                                  TInt aCurrvalue )
    {
    }



// ---------------------------------------------------------
// CApSettingsDlg::CanSaveL( )
// ---------------------------------------------------------
//
CApSettingsDlg::TSaveAction CApSettingsDlg::CanSaveL( TApMember& aDataNeeded )
    {
    User::Leave( KErrNotSupported );
    }






//----------------------------------------------------------
// CApSettingsDlg::ShowPopupIpAddrSettingPageL
//----------------------------------------------------------
//
void CApSettingsDlg::ShowPopupIpAddrSettingPageL( TApMember aData )
    {
    }






//----------------------------------------------------------
// CApSettingsDlg::ShowPopupPhoneNumSettingPageL
//----------------------------------------------------------
//
void CApSettingsDlg::ShowPopupPhoneNumSettingPageL( TApMember aData )
    {
    }




//----------------------------------------------------------
// CApSettingsDlg::ShowPopupPasswordPageL
//----------------------------------------------------------
//
void CApSettingsDlg::ShowPopupPasswordPageL( TApMember aData )
    {
    }



//----------------------------------------------------------
// CApSettingsDlg::GetBoolDataValue
//----------------------------------------------------------
//
void CApSettingsDlg::GetBoolDataValue( TApMember aData, TBool aInvert, 
                                       TInt& aCurrvalue)
    {
    }


//----------------------------------------------------------
// CApSettingsDlg::DoBearerChangeL
//----------------------------------------------------------
//
void CApSettingsDlg::DoBearerChangeL( TInt aCurrvalue)
    {
    }



//----------------------------------------------------------
// CApSettingsDlg::HandleBearerChangeL
//----------------------------------------------------------
//
void CApSettingsDlg::HandleBearerChangeL( TApBearerType aBearer)
    {
    }


//----------------------------------------------------------
// CApSettingsDlg::BearerToArrayIndexL
//----------------------------------------------------------
//
TInt32 CApSettingsDlg::BearerToArrayIndexL( TApBearerType aBearer)
    {
    return 0;
    }

/*
//----------------------------------------------------------
// CApSettingsDlg::ArrayIndexToBearer
//----------------------------------------------------------
//
TApBearerType CApSettingsDlg::ArrayIndexToBearer( TInt32 aIndex)
    {
    APSETUILOGGER_ENTERFN( ESettings,"Settings::ArrayIndexToBearer")
    
    TApBearerType retval( EApBearerTypeGPRS );
    switch( aIndex )
        {
        case KBearerArrayCSD:
            {
            retval = EApBearerTypeCSD;
            break;
            }
        case KBearerArrayGPRS:
            {
            retval = EApBearerTypeGPRS;
            break;
            }
        case KBearerArrayHSCSD:
            {
            retval = EApBearerTypeHSCSD;
            break;
            }
        case KBearerArrayWLAN:
            {
            if ( IsWlanSupported() )
                {
                retval = EApBearerTypeWLAN;
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EInvalidBearerType ) );
            break;
            }
        }
    
    APSETUILOGGER_LEAVEFN( ESettings,"Settings::ArrayIndexToBearer")
    return retval;
    }

*/


//----------------------------------------------------------
// CApSettingsDlg::FillUpBearerSpeedValuesL
//----------------------------------------------------------
//
void CApSettingsDlg::FillUpBearerSpeedValuesL( CDesCArrayFlat& aItems, 
                                               TInt& aCurrvalue )
    {
    }



//----------------------------------------------------------
// CApSettingsDlg::SaveDataL
//----------------------------------------------------------
//
TBool CApSettingsDlg::SaveDataL( )
    {
    return EFalse;
    }



//----------------------------------------------------------
// CApSettingsDlg::HandleListStructureChangeL
//----------------------------------------------------------
//
void CApSettingsDlg::HandleListStructureChangeL()
    {
    }



//----------------------------------------------------------
// CApSettingsDlg::LimitSpeed
//----------------------------------------------------------
//
void CApSettingsDlg::LimitSpeed()
    {
    }


//----------------------------------------------------------
// CApSettingsDlg::SelectItem
//----------------------------------------------------------
//
void CApSettingsDlg::SelectItem( TInt aItemIndex )
    {
    }





// Converting EParagraphDelimiters to \ns
// ---------------------------------------------------------
// CApSettingsDlg::ConvertDelimToPerNLC
// ---------------------------------------------------------
//
HBufC* CApSettingsDlg::ConvertDelimToPerNLC( const TDesC& aInText )
    {
    return NULL;
    }



// Converting \ns to EParagraphDelimiters
// ---------------------------------------------------------
// CApSettingsDlg::ConvertPerNToDelimLC
// ---------------------------------------------------------
//

HBufC* CApSettingsDlg::ConvertPerNToDelimLC( const TDesC& aInText )
    {
    return NULL;
    }


//----------------------------------------------------------
// CApSettingsDlg::DoChangeSettingsL
//----------------------------------------------------------
//
void CApSettingsDlg::DoChangeSettingsL( TBool aQuick )
    {
    }




//----------------------------------------------------------
// CApSettingsDlg::GetSpeedForSelection
//----------------------------------------------------------
//
TApCallSpeed CApSettingsDlg::GetBearerSpeedForSelection( TInt aCurrvalue )
    {
    User::Leave( KErrNotSupported );
    }



//----------------------------------------------------------
// CApSettingsDlg::GetCurrIndexFromBearerSpeed
//----------------------------------------------------------
//
TInt CApSettingsDlg::GetCurrIndexFromBearerSpeed( )
    {
    return 0;
    }


//----------------------------------------------------------
// CApSettingsDlg::ReplaceNonPrintingCharactersLC
//----------------------------------------------------------
//
HBufC* CApSettingsDlg::ReplaceWhiteSpaceCharactersLC(  const TDesC& aInText )
    {
    return NULL;
    }



//----------------------------------------------------------
// CApSettingsDlg::ChangeNetworkGroupL
//----------------------------------------------------------
//
void CApSettingsDlg::ChangeNetworkGroupL()
    {
    }




//----------------------------------------------------------
// CApSettingsDlg::IsHSCSDEnabledL
//----------------------------------------------------------
//
TInt CApSettingsDlg::IsHSCSDEnabledL()
    {
    return EFalse;
    }







//----------------------------------------------------------
// CApSettingsDlg::GetCSDSettingArrays
//----------------------------------------------------------
//
void CApSettingsDlg::GetCSDSettingArrays( TInt variant )
    {
    }



//----------------------------------------------------------
// CApSettingsDlg::GetGPRSSettingArrays
//----------------------------------------------------------
//
void CApSettingsDlg::GetGPRSSettingArrays( TInt variant )
    {
    }




//----------------------------------------------------------
// CApSettingsDlg::GetWLANSettingArraysL
//----------------------------------------------------------
//
void CApSettingsDlg::GetWLANSettingArraysL( TInt /*aVariant*/ )
    {
    }



//----------------------------------------------------------
// CApSettingsDlg::GetNameServersL
//----------------------------------------------------------
//
TInt CApSettingsDlg::GetNameServersL( TBool aIp6 )
    {
    return 0;
    }






//----------------------------------------------------------
// CApSettingsDlg::AddMenuListBoxItemL
//----------------------------------------------------------
//
void CApSettingsDlg::AddMenuListBoxItemL( CDesCArrayFlat& aItemArray,
                                          TApMember /*aMember*/, TInt aRes,
                                          TInt aCompulsory )
    {
    }


//----------------------------------------------------------
// CApSettingsDlg::AskQueryIfWcdmaL
//----------------------------------------------------------
//
TBool CApSettingsDlg::AskQueryIfWcdmaL( TApMember aMember, TInt aCurrvalue  )
    {
    return EFalse;
    }



//----------------------------------------------------------
// CApSettingsDlg::GetDomainNameServersTypeL
//----------------------------------------------------------
//
TInt CApSettingsDlg::GetDomainNameServersTypeL()
    {
    return 0;
    }



//----------------------------------------------------------
// CApSettingsDlg::GetIpv4NameServerL
//----------------------------------------------------------
//
TInt CApSettingsDlg::GetIpv4NameServerL()
    {
    return 0;
    }


//----------------------------------------------------------
// CApSettingsDlg::GetIpv6NameServerL
//----------------------------------------------------------
//
TInt CApSettingsDlg::GetIpv6NameServerL()
    {
    return 0;
    }



//----------------------------------------------------------
// CApSettingsDlg::GetIPv6DNSTypeL
//----------------------------------------------------------
//
TInt CApSettingsDlg::GetIPv6DNSTypeL()
    {
    return 0;
    }





//----------------------------------------------------------
// CApSettingsDlg::GetIPv4DNSTypeL
//----------------------------------------------------------
//
TInt CApSettingsDlg::GetIPv4DNSTypeL()
    {
    return 0;
    }





// ---------------------------------------------------------
// CApSettingsDlg::StripNonNumberLC
// ---------------------------------------------------------
//
HBufC* CApSettingsDlg::StripNonNumberLC( const TDesC16& aInText )
    {
    return NULL;
    }
    

//----------------------------------------------------------
// CApSettingsDlg::BearerType2BearerItemPosL
//----------------------------------------------------------
//
TInt CApSettingsDlg::BearerType2BearerItemPosL( TApBearerType aBearer )
    {
    return 0;
    }


//----------------------------------------------------------
// CApSettingsDlg::BearerItemPos2BearerTypeL
//----------------------------------------------------------
//
TApBearerType CApSettingsDlg::BearerItemPos2BearerTypeL( TInt aPos )
    {
    User::Leave( KErrNotSupported );
    }



//----------------------------------------------------------
// CApSettingsDlg::ChangeWlanNetworkNameL
//----------------------------------------------------------
//
void CApSettingsDlg::ChangeWlanNetworkNameL()
    {
    }


//----------------------------------------------------------
// CApSettingsDlg::ChangeWlanSecuritySettings
//----------------------------------------------------------
//
TInt CApSettingsDlg::ChangeWlanSecuritySettingsL()
    {
    return 0;
    }


//----------------------------------------------------------
// CApSettingsDlg::ChangeWepSecuritySettingsL
//----------------------------------------------------------
//
TInt CApSettingsDlg::ChangeWepSecuritySettingsL()
    {
    return 0;
    }



//----------------------------------------------------------
// CApSettingsDlg::ChangeWpaSecuritySettingsL
//----------------------------------------------------------
//
TInt CApSettingsDlg::ChangeWpaSecuritySettingsL()
    {
    return 0;
    }


//----------------------------------------------------------
// CApSettingsDlg::Change8021xSecuritySettingsL
//----------------------------------------------------------
//
TInt CApSettingsDlg::Change8021xSecuritySettingsL()
    {
    return 0;
    }




//----------------------------------------------------------
// CApSettingsDlg::IsWlanSupported
//----------------------------------------------------------
//
TBool CApSettingsDlg::IsWlanSupported()
    {
    return EFalse;
    }
    
    

//----------------------------------------------------------
// CApSettingsDlg::QueryWlanNetworkNameL
//----------------------------------------------------------
//
TBool CApSettingsDlg::QueryWlanNetworkNameL()
    {
    return EFalse;
    }


//----------------------------------------------------------
// CApSettingsDlg::IsNetMaskAndGatewayVisible
//----------------------------------------------------------
//
TBool CApSettingsDlg::IsNetMaskAndGatewayVisibleL()
    {
    return EFalse;
    }



//----------------------------------------------------------
// CApSettingsDlg::GetAdHocChannelL
//----------------------------------------------------------
//
TUint32 CApSettingsDlg::GetAdHocChannelL()
    {
    return 0;
    }
    
    
    
//----------------------------------------------------------
// CApSettingsDlg::IsAdhocChannelVisible
//----------------------------------------------------------
//
TBool CApSettingsDlg::IsAdhocChannelVisible() 
    {
    return EFalse;
    }
    
    

//----------------------------------------------------------
// CApSettingsDlg::HasSecuritySettingsFilledL
//----------------------------------------------------------
//
TBool CApSettingsDlg::HasSecuritySettingsFilledL()
    {
    return EFalse;
    }



//----------------------------------------------------------
// CApSettingsDlg::LimitSecMode
//----------------------------------------------------------
//
void CApSettingsDlg::LimitSecMode()
    {
    }




// ---------------------------------------------------------
// CApSettingsDlg::CanSaveWlanCompulsoriesL( )
// ---------------------------------------------------------
//
CApSettingsDlg::TSaveAction CApSettingsDlg::CanSaveWlanCompulsoriesL( 
                                                TApMember& aDataNeeded )
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// CApSettingsDlg::RestoreIpv4SettingsL()
// ---------------------------------------------------------
//
void CApSettingsDlg::RestoreIpv4SettingsL()
    {
    }


    
// End of File
