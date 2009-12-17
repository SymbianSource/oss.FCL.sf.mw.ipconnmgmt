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
* Description:  Shows the "Select destination:" dialog, where the user can 
*                select one of the available destinations.
*
*/

#include <AknUtils.h>
#include <badesca.h>
#include <StringLoader.h>
#include <cmmanager.rsg>
#include "selectdestinationdlg.h"
#include <cmcommonconstants.h>
#include <cmcommonui.h>
#include "cmdestinationimpl.h"
#include "cmlogger.h"
#include "cmmanagerimpl.h"

#include <cmmanagerdef.h>

// ----------------------------------------------------------------------------
// CSelectDestinationDlg::CSelectDestinationDlg
// ----------------------------------------------------------------------------
//
CSelectDestinationDlg::CSelectDestinationDlg( TInt aDummyIndex,
                                              TUint32& aDestinationId )
    : CAknListQueryDialog( &aDummyIndex )
    , iDestinationId( aDestinationId )
    , iDestinations( KCmArrayMediumGranularity )
    {
    CLOG_CREATE;
    }

// ----------------------------------------------------------------------------
// CSelectDestinationDlg::ConstructL
// ----------------------------------------------------------------------------
//
void CSelectDestinationDlg::ConstructL( CCmManagerImpl& aCmManager,
                                        RArray<TUint32>* aDestArray )
    {
    LOGGER_ENTERFN( "CSelectDestinationDlg::ConstructL" );
    RArray<TUint32> destinations( KCmArrayMediumGranularity );
    CleanupClosePushL( destinations );

    aCmManager.AllDestinationsL( destinations );

    TBool mmsFound = EFalse;
    for ( TInt i = 0; i < destinations.Count(); i++ )
        {
        CCmDestinationImpl* dest = aCmManager.DestinationL( destinations[i] );
        CleanupStack::PushL( dest );   
             
        // Don't count the current destination
        if ( dest->Id() == iDestinationId )
            {
            CleanupStack::PopAndDestroy( dest );
            continue;
            }
               
        // Do not give the option to copy into a Level 1 Protected Destination
        if ( dest->ProtectionLevel() == CMManager::EProtLevel1 )
            {
            CleanupStack::PopAndDestroy( dest );
            continue;
            }

        // check if the destination is valid as parent (e.g. for VPN)
        if ( aDestArray && aDestArray->Find( dest->Id() ) == KErrNotFound )
            {
            CleanupStack::PopAndDestroy( dest );
            continue;
            }
        
        // Skip the MMS Snap, it it not shown in the list
        if ( !mmsFound )
            {
            TInt snapMetadata = 0;
            TRAPD(metaErr, snapMetadata = dest->MetadataL( CMManager::ESnapMetadataPurpose ));
            if ( metaErr == KErrNone && snapMetadata == CMManager::ESnapPurposeMMS )
                {
                CleanupStack::PopAndDestroy( dest );
                mmsFound = ETrue;
                continue;
                }
            }
        
        iDestinations.AppendL( dest );
        CleanupStack::Pop( dest );
        }
    CleanupStack::PopAndDestroy( &destinations );
    }
    
// ----------------------------------------------------------------------------
// CSelectDestinationDlg::~CSelectDestinationDlg
// ----------------------------------------------------------------------------
//
CSelectDestinationDlg::~CSelectDestinationDlg()
    {
    for ( TInt i = 0; i < iDestinations.Count(); i++ )
        {
        delete iDestinations[i];
        }
    iDestinations.Close();
    CLOG_CLOSE;
    }

// ----------------------------------------------------------------------------
// CSelectDestinationDlg::NewL
// ----------------------------------------------------------------------------
//
CSelectDestinationDlg* CSelectDestinationDlg::NewL( TUint32& aDestinationId,
                                                    CCmManagerImpl& aCmManager,
                                                    RArray<TUint32>* aDestArray )
    {
    CSelectDestinationDlg* self = NewLC( aDestinationId, aCmManager, aDestArray );

    CleanupStack::Pop( self );

    return self;
    }


// ----------------------------------------------------------------------------
// CSelectDestinationDlg::NewLC
// ----------------------------------------------------------------------------
//
CSelectDestinationDlg* CSelectDestinationDlg::NewLC( TUint32& aDestinationId,
                                                     CCmManagerImpl& aCmManager,
                                                     RArray<TUint32>* aDestArray )
    {
    CSelectDestinationDlg* self =
                    new (ELeave) CSelectDestinationDlg( 0, aDestinationId );

    CleanupStack::PushL( self );

    self->ConstructL( aCmManager, aDestArray );

    return self;
    }


// ----------------------------------------------------------------------------
// CSelectDestinationDlg::OkToExitL
// ----------------------------------------------------------------------------
//
TBool CSelectDestinationDlg::OkToExitL( TInt aButtonId )
    {
    TBool result = EFalse;
    
    if ( aButtonId == EAknSoftkeySelect || aButtonId == EAknSoftkeyOk )
        {
        iDestinationId = iDestinations[ ListBox()->CurrentItemIndex() ]->Id();

        result = ETrue;
        }
    else if ( aButtonId == EAknSoftkeyCancel )
        {
        iDestinationId = 0;

        result = ETrue;
        }

    CLOG_WRITE_2( "CSelectDestinationDlg::OkToExitL dest ID: [%d], result: [%d]",
                   iDestinationId, result );

    return result;
    }


// ----------------------------------------------------------------------------
// CSelectDestinationDlg::PreLayoutDynInitL
// ----------------------------------------------------------------------------
//
void CSelectDestinationDlg::PreLayoutDynInitL()
    {
    LOGGER_ENTERFN( "CSelectDestinationDlg::PreLayoutDynInitL" );
    // Let the base class do its job first
    CAknListQueryDialog::PreLayoutDynInitL();

    // Set the description field on the query
    HBufC* desc =
              StringLoader::LoadLC( R_CMWIZARD_SELECT_DESTINATION_DESCRIPTION );

    MessageBox()->SetMessageTextL( desc );
    CleanupStack::PopAndDestroy( desc );

    // Specify the item text array
    CDesCArrayFlat* itemArray =
                      new (ELeave) CDesCArrayFlat( KCmArrayMediumGranularity );
    SetItemTextArray( itemArray );
    SetOwnershipType( ELbmOwnsItemArray );

    // Note: we don't have to put 'itemArray' on to the cleanup stack, as the
    // model has already taken this object over.
    
    for ( TInt i = 0; i < iDestinations.Count(); i++ )
        {       
        // Here we take advantage of the fact that our icon array is ordered.
        // That is, the indices of the icons in the array will always be
        // in sync with those in the item text array (i.e. each bearer will
        // have the right icon associated with it).
        HBufC* destName = iDestinations[i]->NameLC();
        HBufC* lineText =
            TCmCommonUi::FormattedTextForListBoxLC( i, *destName );

        itemArray->AppendL( *lineText );
        CleanupStack::PopAndDestroy( 2, destName ); // destName, lineText
        }

    // Icons ...
    SetIconsL();
    }


// ----------------------------------------------------------------------------
// CSelectDestinationDlg::SetIconsL()
// ----------------------------------------------------------------------------
//
void CSelectDestinationDlg::SetIconsL()
    {
    CArrayPtr<CGulIcon>* icons =
             new (ELeave) CArrayPtrFlat<CGulIcon>( KCmArrayMediumGranularity );
    CleanupStack::PushL( icons );

    for ( TInt i = 0; i < iDestinations.Count(); i++ )
        {
        CGulIcon* icon = iDestinations[i]->IconL();
        CleanupStack::PushL( icon );

        icons->AppendL( icon );

        CleanupStack::Pop( icon );  // icon array took over ownership
        }

    SetIconArrayL( icons ); // ownership passed, too

    CleanupStack::Pop( icons );
    }


// ----------------------------------------------------------------------------
// void CSelectDestinationDlg::HandleResourceChange
// ----------------------------------------------------------------------------
//
void CSelectDestinationDlg::HandleResourceChange( TInt aType )
    {
    if ( aType == KAknsMessageSkinChange )
        {
        CAknListQueryDialog::HandleResourceChange( aType );

        TRAP_IGNORE( SetIconsL() );

        SizeChanged();
        }
    else
        {
        if ( aType == KEikDynamicLayoutVariantSwitch )
            {
            TRect mainPaneRect;
            AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                                               mainPaneRect );

            TAknLayoutRect layoutRect;
            layoutRect.LayoutRect( TRect( TPoint( 0, 0 ), 
                                   mainPaneRect.Size() ),
                                   AKN_LAYOUT_WINDOW_list_gen_pane( 0 ) );

            ListBox()->SetRect( layoutRect.Rect() );
            }

        // Base call
        CAknListQueryDialog::HandleResourceChange( aType );
        }
    }
