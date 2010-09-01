/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Defines common UI methods.
*
*/

#include <aknlists.h>       // CAknDoubleLargeGraphicPopupMenuStyleListBox
#include <aknPopup.h>       // CAknPopupList
#include <AknQueryDialog.h>
#include <aknnotedialog.h>
#include <badesca.h>        // CDesCArray
#include <e32def.h>
#include <e32std.h>
#include <eikfrlb.h>        // CEikFormattedCellListBox
#include <eiklbm.h>         // ELbmDoesNotOwnItemArray
#include <gulicon.h>
#include <StringLoader.h>
#include <cmmanager.rsg>

#include <cmcommonconstants.h>
#include <cmcommonui.h>
#include "cmlogger.h"
#include "cmmanagerimpl.h"
#include "cmmanager.hrh"
#include <cmconnectionmethoddef.h>

using namespace CMManager;

const TInt KDefaultListBoxFlags = 0;


// ---------------------------------------------------------------------------
// TCmCommonUi::CCmDesCArray::CCmDesCArray
// ---------------------------------------------------------------------------
//
TCmCommonUi::CCmDesCArray::CCmDesCArray()
    : CDesCArrayFlat( KCmArraySmallGranularity )
    , iUids( KCmArraySmallGranularity )
    {
    }

// ---------------------------------------------------------------------------
// TCmCommonUi::CCmDesCArray::~CCmDesCArray
// ---------------------------------------------------------------------------
//
TCmCommonUi::CCmDesCArray::~CCmDesCArray()
    {
    iUids.Close();
    }

// ---------------------------------------------------------------------------
// TCmCommonUi::CCmDesCArray::AppendL
// ---------------------------------------------------------------------------
//
void TCmCommonUi::CCmDesCArray::AppendL( TUint aUid, const TDesC& aItem )
    {
    iUids.AppendL( aUid );
    CDesCArray::AppendL( aItem );
    }

// ---------------------------------------------------------------------------
// TCmCommonUi::CCmDesCArray::Uid
// ---------------------------------------------------------------------------
//
TUint TCmCommonUi::CCmDesCArray::Uid( TInt aIndex ) const
    {
    return iUids[ aIndex ];
    }

// ---------------------------------------------------------------------------
// TCmCommonUi::ShowConfirmationQueryWithInputL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TCmCommonUi::ShowConfirmationQueryWithInputL( TInt aResId, 
                                                            TDes& aInput )
    {
    CLOG_ENTERFN_PTR( "ShowConfirmationQueryWithInputL" );
    HBufC* prompt = CEikonEnv::Static()->AllocReadResourceAsDes16LC( aResId );
    
    CAknTextQueryDialog* dlg =
                 CAknTextQueryDialog::NewL( aInput, CAknQueryDialog::ENoTone );
    
    // Handle the allowable enterable length of the text
    CLOG_WRITE_1_PTR( NULL, "max length input: [%d   ]", aInput.MaxLength() );
    dlg->SetMaxLength( aInput.MaxLength() );
    
    TInt retval = dlg->ExecuteLD( R_TEXT_QUERY, *prompt );
    
    CleanupStack::PopAndDestroy( prompt );
    
    return retval;
    }

// ---------------------------------------------------------------------------
// TCmCommonUi::ShowConfirmationQueryL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TCmCommonUi::ShowConfirmationQueryL( TInt aResId )
    {
    CLOG_ENTERFN_PTR( "ShowConfirmationQueryL" );
    HBufC* prompt = CEikonEnv::Static()->AllocReadResourceL( aResId );

    // Ownership (of 'prompt') is handed over to callee. That's why we don't
    // bother with cleanup stack either - let it be the callee's problem.
    return ShowConfirmationQueryL( prompt );
    }    
    
// ---------------------------------------------------------------------------
// TCmCommonUi::ShowConfirmationQueryL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TCmCommonUi::ShowConfirmationQueryL( TInt aResId, 
                                                   const TDesC& aString )
    {
    CLOG_ENTERFN_PTR( "ShowConfirmationQueryL" );
    HBufC* prompt = StringLoader::LoadL( aResId, aString );
    
    // Ownership (of 'prompt') is handed over to callee.
    return ShowConfirmationQueryL( prompt );
    }    
    
// ---------------------------------------------------------------------------
// TCmCommonUi::ShowAvailableMethodsL
// ---------------------------------------------------------------------------
//
TUint32 TCmCommonUi::ShowAvailableMethodsL(
                                    const RArray<TUint32>& aAvailableBearers,
                                    const CCmManagerImpl& aCmManager )
    {
    CLOG_ENTERFN_PTR( "ShowAvailableMethodsL" );

    TUint32 retval = 0;

    // Create listbox and popup list and perform some initialization on them.
    CEikFormattedCellListBox* listBox =
                    new (ELeave) CAknDoubleLargeGraphicPopupMenuStyleListBox();
    CleanupStack::PushL( listBox );
    
    CAknPopupList* popupList =
         CAknPopupList::NewL( listBox,
                              R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
                              AknPopupLayouts::EMenuDoubleLargeGraphicWindow );
    CleanupStack::PushL( popupList );

    listBox->ConstructL( popupList, KDefaultListBoxFlags );
    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL(
                                                   CEikScrollBarFrame::EOff,
                                                   CEikScrollBarFrame::EAuto );

    // Create the icon and item text arrays.
    CArrayPtr<CGulIcon>* icons =
                new(ELeave) CArrayPtrFlat<CGulIcon>( KCmArraySmallGranularity );
    CleanupStack::PushL( icons );

    CCmDesCArray* itemArray = new (ELeave) CCmDesCArray();
    listBox->Model()->SetItemTextArray( itemArray );
    // just to be on the safe side: next line is not mandatory, but advisable
    listBox->Model()->SetOwnershipType( ELbmOwnsItemArray );

    // Note: we don't have to put 'itemArray' on to the cleanup stack, as the
    // model has already taken this object over.

    // Fill up the arrays
    for ( TInt i = 0; i < aAvailableBearers.Count(); i++ )
        {
        TUint32 bearerUid = aAvailableBearers[i];

        // Add icons to the icon array
        CGulIcon* icon = (CGulIcon*)(aCmManager.GetBearerInfoIntL(
                                                  bearerUid,
                                                  ECmBearerAvailableIcon ) );

        if( !icon )
            {
            continue;
            }
                
        CleanupStack::PushL( icon );
        icons->AppendL( icon );
        CleanupStack::Pop( icon );  // icon array took over ownership

        // Add item text to the array.
        // Query the name of the bearer first.
        HBufC* firstLine = aCmManager.GetBearerInfoStringL(
                                                  bearerUid,
                                                  ECmBearerAvailableName );
        CleanupStack::PushL( firstLine );

        // Query the availability string of the bearer second.
        HBufC* secondLine = aCmManager.GetBearerInfoStringL(
                                                  bearerUid,
                                                  ECmBearerAvailableText );
        CleanupStack::PushL( secondLine );

        // Third, combine these two strings along with an icon index.
        HBufC* listBoxLine = FormattedTextForListBoxLC( i,
                                                        *firstLine,
                                                        *secondLine );

        // Finally add the resulting string to the item array.
        itemArray->AppendL( bearerUid, *listBoxLine );
        
        CLOG_WRITE_1_PTR( NULL, 
                          "ShowAvailableMethodsL new listbox item: [%S]",
                          listBoxLine );

        // Clean-up: firstLine, secondLine, listBoxLine.
        CleanupStack::PopAndDestroy( 3, firstLine );
        }

    // Handing over the ownership of the icon array to the listbox
    listBox->ItemDrawer()->FormattedCellData()->SetIconArrayL( icons );
    
    CleanupStack::Pop( icons ); // ownership already handed over

    // Set title
    HBufC* title = StringLoader::LoadLC( R_CMWIZARD_AVAILABLE_METHODS );
    popupList->SetTitleL( *title );
    CleanupStack::PopAndDestroy( title );
    
    if ( popupList->ExecuteLD() )
        {
        retval = itemArray->Uid( listBox->CurrentItemIndex() );
        }

    CleanupStack::Pop( popupList ); // ExecuteLD has already destroyed it
    CleanupStack::PopAndDestroy( listBox );

    CLOG_WRITE_1_PTR( NULL, "ShowAvailableMethodsL exit, retval: [%d]", retval );

    return retval;
    }

// ---------------------------------------------------------------------------
// TCmCommonUi::SelectBearerL
// ---------------------------------------------------------------------------
//
TUint32 TCmCommonUi::SelectBearerL( const CCmManagerImpl& aCmManager )
    {
    CLOG_ENTERFN_PTR( "SelectBearerL" );
    TUint32 retval = 0;

    CEikFormattedCellListBox* listBox =
                    new (ELeave) CAknSingleGraphicPopupMenuStyleListBox();
    CleanupStack::PushL( listBox );
    
    CAknPopupList* popupList =
         CAknPopupList::NewL( listBox,
                              R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
                              AknPopupLayouts::EMenuGraphicWindow );
    CleanupStack::PushL( popupList );

    listBox->ConstructL( popupList, KDefaultListBoxFlags );
    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL(
                                                   CEikScrollBarFrame::EOff,
                                                   CEikScrollBarFrame::EAuto );

    RArray<TUint32> bearers( KCmArrayMediumGranularity );
    CleanupClosePushL( bearers );
    aCmManager.SupportedBearersL( bearers );
    
    TInt i;
    
    for ( i = 0; i < bearers.Count(); ++i )
        {
        if( !aCmManager.GetBearerInfoBoolL( bearers[i],
                                            ECmBearerHasUi ) )
            {
            bearers.Remove( i );
            --i;
            }
            
        }

    CArrayPtr<CGulIcon>* icons =
              new(ELeave) CArrayPtrFlat<CGulIcon>( KCmArrayMediumGranularity );
    CleanupStack::PushL( icons );

    // Note: it's interesting that we, in fact, set up an *ordered* list of
    // icons. Ordering of the icon array is based on the order of supported
    // bearers.
    for ( i = 0; i < bearers.Count(); ++i )
        {
        CGulIcon* icon = (CGulIcon*)(aCmManager.GetBearerInfoIntL(
                                                bearers[i],
                                                ECmBearerIcon ) );

        if( !icon )
            {
            bearers.Remove( i );
            --i;
            continue;
            }
            
        CleanupStack::PushL( icon );

        icons->AppendL( icon );

        CleanupStack::Pop( icon );  // icon array took over ownership
        }

    // Handing over the ownership of the icon array to the listbox
    listBox->ItemDrawer()->FormattedCellData()->SetIconArrayL( icons );
    
    CleanupStack::Pop( icons ); // ownership already handed over

    CCmDesCArray* itemArray = new (ELeave) CCmDesCArray();
    listBox->Model()->SetItemTextArray( itemArray );
    // just to be on the safe side: next line is not mandatory, but advisable
    listBox->Model()->SetOwnershipType( ELbmOwnsItemArray );

    // Note: we don't have to put 'itemArray' on to the cleanup stack, as the
    // model has already taken this object over.

    for ( TInt i = 0; i < bearers.Count(); i++ )
        {
        // Here we take advantage of the fact that our icon array is ordered.
        // That is, the indices of the icons in the array will always be
        // in sync with those in the item text array (i.e. each bearer will
        // have the right icon associated with it).
        HBufC* bearerName = aCmManager.GetBearerInfoStringL( bearers[i],
                                                      ECmBearerSupportedName );
        CleanupStack::PushL( bearerName );

        HBufC* lineText = FormattedTextForListBoxLC( i, *bearerName );

        itemArray->AppendL( bearers[i], *lineText );

        CLOG_WRITE_1_PTR( NULL, "SelectBearerL new listbox item: [%S]",
                               lineText );
        
        CleanupStack::PopAndDestroy( 2, bearerName ); // lineText, bearerName
        }

    CleanupStack::PopAndDestroy( &bearers );

    // Set title
    HBufC* title = StringLoader::LoadLC( R_CMWIZARD_SELECT_CONN_METHOD );
    popupList->SetTitleL( *title );
    CleanupStack::PopAndDestroy( title );
    
    if ( popupList->ExecuteLD() )
        {
        retval = itemArray->Uid( listBox->CurrentItemIndex() );
        }

    CleanupStack::Pop( popupList ); // ExecuteLD has already destroyed it
    CleanupStack::PopAndDestroy( listBox );

    CLOG_WRITE_1_PTR( NULL, "SelectBearerL exit, retval: [%d]", retval );

    return retval;
    }

// ---------------------------------------------------------------------------
// TCmCommonUi::ShowConfirmationQueryL
// ---------------------------------------------------------------------------
//
TInt TCmCommonUi::ShowConfirmationQueryL( HBufC* aPrompt )
    {
    CLOG_ENTERFN_PTR( "ShowConfirmationQueryL" );

    CleanupStack::PushL( aPrompt );
    
    CAknQueryDialog* dlg = 
            CAknQueryDialog::NewL( CAknQueryDialog::EConfirmationTone );
    TInt retval = dlg->ExecuteLD( R_CONFIRMATION_QUERY, *aPrompt );

    CleanupStack::PopAndDestroy( aPrompt );

    return retval;
    }

// ---------------------------------------------------------------------------
// TCmCommonUi::ShowMemoryFullConfirmationQueryL
// ---------------------------------------------------------------------------
//
TInt TCmCommonUi::ShowMemoryFullConfirmationQueryL()
    {
    CLOG_ENTERFN_PTR( "ShowMemoryFullConfirmationQueryL" );

    //  need to check if EikonEnv exists in this context
    CEikonEnv* eikPtr = CEikonEnv::Static();
    TInt retval( KErrNone ); 
    if( eikPtr )
        {        
        HBufC* prompt = eikPtr->AllocReadResourceL( R_QTN_MEMLO_MEMORY_FULL );        
        CleanupStack::PushL( prompt );
        CAknQueryDialog* dlg = 
            CAknQueryDialog::NewL( CAknQueryDialog::EConfirmationTone );
        retval = dlg->ExecuteLD( R_MEMORY_FULL_CONFIRMATION_QUERY, *prompt );
        CleanupStack::PopAndDestroy( prompt );
        }

    return retval;
    }

// ---------------------------------------------------------------------------
// TCmCommonUi::ShowNoteL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TCmCommonUi::ShowNoteL( TInt aResId, TCmNoteType aNoteType )
    {
    CLOG_ENTERFN_PTR( "ShowNoteL" );
    HBufC* prompt = CEikonEnv::Static()->AllocReadResourceLC( aResId );

    TInt result = ShowNoteL( *prompt, aNoteType );

    CleanupStack::PopAndDestroy( prompt );

    return result;
    }    
    
// ---------------------------------------------------------------------------
// TCmCommonUi::ShowNoteL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TCmCommonUi::ShowNoteL( TInt aResId, const TDesC& aString,
                                      TCmNoteType aNoteType )
    {
    CLOG_ENTERFN_PTR( "ShowNoteL" );
    HBufC* prompt = StringLoader::LoadLC( aResId, aString );
    
    TInt result = ShowNoteL( *prompt, aNoteType );

    CleanupStack::PopAndDestroy( prompt );

    return result;
    }

// ---------------------------------------------------------------------------
// TCmCommonUi::ShowNoteL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TCmCommonUi::ShowNoteL( const TDesC& aPrompt, TCmNoteType aNoteType )
    {
    CLOG_ENTERFN_PTR( "ShowNoteL" );

    CAknNoteDialog* dlg = new (ELeave) CAknNoteDialog(
                             CAknNoteDialog::ENoTone,
                             CAknNoteDialog::ELongTimeout );

    TInt resId;
    switch ( aNoteType )
        {
        case ECmOkNote:
            {
            resId = R_OK_NOTE;
            break;
            }
        case ECmWarningNote:
            {
            resId = R_WARNING_NOTE;
            break;
            }
        case ECmErrorNote:
            {
            resId = R_ERROR_NOTE;
            break;
            }
        default:
            {
            resId = R_INFO_NOTE;
            }
        }

    // no need to worry about LC (PrepareLC), RunLD takes care of it
    dlg->PrepareLC( resId );
    dlg->SetTextL( aPrompt );

    return dlg->RunLD();
    }

// ---------------------------------------------------------------------------
// TCmCommonUi::FormattedTextForListBoxLC
// ---------------------------------------------------------------------------
//
HBufC* TCmCommonUi::FormattedTextForListBoxLC( TInt aIconIndex,
                                               const TDesC& aLineText )
    {
    HBufC* formattedString =
                HBufC::NewL( KNumberOfStaticCharsIn1LineFormattedString +
                             aLineText.Length() );

    TPtr des = formattedString->Des();
    des.AppendFormat( KSingleNumberAsString, aIconIndex );
    des.Append( TChar( KSettingsListBoxItemPrefix ) );
    des.Append( aLineText );

    CleanupStack::PushL( formattedString );

    return formattedString;
    }

// ---------------------------------------------------------------------------
// TCmCommonUi::FormattedTextForListBoxLC
// ---------------------------------------------------------------------------
//
HBufC* TCmCommonUi::FormattedTextForListBoxLC( TInt aIconIndex,
                                               const TDesC& aFirstLine,
                                               const TDesC& aSecondLine )
    {
    HBufC* formattedString =
                HBufC::NewL( KNumberOfStaticCharsIn2LineFormattedString +
                             aFirstLine.Length() +
                             aSecondLine.Length() );

    TPtr des = formattedString->Des();
    des.AppendFormat( KSingleNumberAsString, aIconIndex );
    des.Append( TChar( KSettingsListBoxItemPrefix ) );
    des.Append( aFirstLine );
    des.Append( TChar( KSettingsListBoxItemPrefix ) );
    des.Append( aSecondLine );

    CleanupStack::PushL( formattedString );

    return formattedString;
    }

// ---------------------------------------------------------------------------
// TCmCommonUi::FormattedTextForListBoxLC
// ---------------------------------------------------------------------------
//
HBufC* TCmCommonUi::FormattedTextForListBoxLC( TInt aIconIndex,
                                               TInt aFirstLineResId,
                                               TInt aSecondLineResId )
    {
    HBufC* secondLine = StringLoader::LoadLC( aSecondLineResId );

    HBufC* formattedString = FormattedTextForListBoxL( aIconIndex,
                                                       aFirstLineResId,
                                                       *secondLine );

    CleanupStack::PopAndDestroy( secondLine );

    CleanupStack::PushL( formattedString );

    return formattedString;
    }


// ---------------------------------------------------------------------------
// TCmCommonUi::FormattedTextForListBoxLC
// ---------------------------------------------------------------------------
//
HBufC* TCmCommonUi::FormattedTextForListBoxLC( TInt aIconIndex,
                                               TInt aFirstLineResId,
                                               TInt aSecondLineResId,
                                               TInt aReplacementInSecondLine )
    {
    HBufC* secondLine = StringLoader::LoadLC( aSecondLineResId,
                                              aReplacementInSecondLine );

    HBufC* formattedString = FormattedTextForListBoxL( aIconIndex,
                                                       aFirstLineResId,
                                                       *secondLine );

    CleanupStack::PopAndDestroy( secondLine );

    CleanupStack::PushL( formattedString );

    return formattedString;
    }


// ---------------------------------------------------------------------------
// TCmCommonUi::FormattedTextForListBoxL
// ---------------------------------------------------------------------------
//
HBufC* TCmCommonUi::FormattedTextForListBoxL( TInt aIconIndex,
                                               TInt aFirstLineResId,
                                               const TDesC& aSecondLine )
    {
    HBufC* firstLine = StringLoader::LoadLC( aFirstLineResId );
    
    HBufC* formattedString =
                HBufC::NewL( KNumberOfStaticCharsIn2LineFormattedString +
                             firstLine->Length() +
                             aSecondLine.Length() );

    TPtr des = formattedString->Des();
    des.AppendFormat( KSingleNumberAsString, aIconIndex );
    des.Append( TChar( KSettingsListBoxItemPrefix ) );
    des.Append( *firstLine );
    des.Append( TChar( KSettingsListBoxItemPrefix ) );
    des.Append( aSecondLine );
    
    CleanupStack::PopAndDestroy( firstLine );

    return formattedString;
    }


// --------------------------------------------------------------------------
// TCmCommonUi::ShowPopupListL
// --------------------------------------------------------------------------
//
EXPORT_C TBool TCmCommonUi::ShowPopupListL( const TDesC& aTitle,
                                            CDesCArrayFlat* aTextItems,
                                            CArrayPtr<CGulIcon>* aIconArray,
                                            const TUint aSoftKeys,
                                            TInt& aSelection )
    {
    TBool retVal ( EFalse );
    
    // Prepare the dialog
    CEikFormattedCellListBox* list = NULL;
    if ( aIconArray )
        {
        list = new ( ELeave ) CAknSingleGraphicPopupMenuStyleListBox;
        }
    else
        {
        list = new ( ELeave ) CAknSinglePopupMenuStyleListBox;
        }
        
    CleanupStack::PushL( list );
    
    CAknPopupList* popupList = CAknPopupList::NewL(
                                       list, aSoftKeys,
                                       AknPopupLayouts::EMenuWindow );
    CleanupStack::PushL( popupList );

    // initialise listbox.
    list->ConstructL( popupList, CEikListBox::ELeftDownInViewRect );
    list->CreateScrollBarFrameL( ETrue );
    list->ScrollBarFrame()->SetScrollBarVisibilityL(
                                               CEikScrollBarFrame::EOff,
                                               CEikScrollBarFrame::EAuto );
                                               
    // Handing over the ownership of the icon array to the listbox
    if ( aIconArray )
        {
        list->ItemDrawer()->FormattedCellData()->SetIconArrayL( aIconArray );
        }
	
	// Items
    list->Model()->SetItemTextArray( aTextItems );
   
    // Title
    popupList->SetTitleL( aTitle );
    
    // Show the dialog
    if ( popupList->ExecuteLD() )
        {
        aSelection = list->CurrentItemIndex();
        retVal = ETrue;
        }

    CleanupStack::Pop( popupList );
    CleanupStack::PopAndDestroy( list );
    
    return retVal;
    }

// ---------------------------------------------------------------------------
// TCmCommonUi::ShowAlwaysAskNoteL
// ---------------------------------------------------------------------------
//
TBool TCmCommonUi::ShowAlwaysAskNoteL( TCmDefConnValue aOldDefConn,
                                        CCmManagerImpl& aCmManagerImpl )
    {
    TCmDefConnValue newConn;
    aCmManagerImpl.ReadDefConnL( newConn );
    if ( newConn != aOldDefConn )
        {
         ShowNoteL( R_CMMANAGERUI_CONF_DEFAULT_ALWAYS_ASK,
                                                TCmCommonUi::ECmOkNote );
        return ETrue;
        }
    return EFalse;
    }
   
    
