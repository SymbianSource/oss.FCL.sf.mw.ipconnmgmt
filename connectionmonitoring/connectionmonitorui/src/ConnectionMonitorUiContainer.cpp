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
* Description:  container control class
*
*
*/


// INCLUDE FILES
#include <aknnavi.h>    //for CNaviPane
#include <aknnavide.h>  //for CAknNavigationDecorator

#include <AknIconArray.h>
#include <aknlists.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <AknsUtils.h>
#include <AknGlobalNote.h>

#include "ConnectionArray.h"
#include "ConnectionMonitorUiContainer.h"
#include "FeatureManagerWrapper.h"
#include "ActiveWrapper.h"

#include <ConnectionMonitorUi.rsg>
#include <ConnectionMonitorUi.mbg>
#include <data_caging_path_literals.hrh>

#include  <hlplch.h>
#include <csxhelp/cmon.hlp.hrh>

// CONSTANTS 
/**
* UID of ConnectionMonitorUi App.
*/
LOCAL_D const TUid KUidConnMonUI = { 0x101F84D0 };


// CONSTANTS
// Granularity of the model array.
/**
* Granularity of icon array
*/
LOCAL_D const TInt KGranularity = 5;


_LIT( KEmpty, "");  ///< Empty string

// ROM folder
_LIT( KDriveZ, "z:" );

// Name of the MBM file containing bitmaps
_LIT( KConnectionMonitorUiMBMFile, "ConnectionMonitorUi.mbm" );

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CConnectionMonitorUiContainer::CConnectionMonitorUiContainer
// ---------------------------------------------------------
//
CConnectionMonitorUiContainer::CConnectionMonitorUiContainer(
                                  CConnectionArray* const aConnectionArray,
                                  RConnectionMonitor* aMonitor,
                                  CActiveWrapper* aActiveWrapper )
    : CCoeControl(), 
    iMonitor( aMonitor ),
    iConnectionArray( aConnectionArray ), 
    isWlanSupported( EFalse ),
    iActiveWrapper( aActiveWrapper )
    {
    }

// ---------------------------------------------------------
// CConnectionMonitorUiContainer::ConstructL
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CConnectionMonitorUiContainer::ConstructL( TInt aSelectedItem, const TRect& aRect )
    {
    CMUILOGGER_ENTERFN( "CConnectionMonitorUiContainer::ConstructL" );

    CreateWindowL(); 
    
    iListBox = new( ELeave ) CAknDoubleGraphicStyleListBox;
    TInt flags( EAknGenericListBoxFlags );
    iListBox->ConstructL( this, flags );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
                                               CEikScrollBarFrame::EOff,
                                               CEikScrollBarFrame::EAuto );
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray
                                         /*ELbmDoesNotOwnItemArray*/ );
    iListBox->Model()->SetItemTextArray( iConnectionArray );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    
    TInt actCount = iConnectionArray->MdcaCount() - 1;
    
    if ( actCount > KErrNotFound )
		{
		iListBox->SetCurrentItemIndex( aSelectedItem > actCount ? actCount : aSelectedItem );	    
		}

    SetIconsL();

    HBufC* noConn = StringLoader::LoadLC( R_QTN_CMON_NO_ACTIVE_CONNECTIONS,
                                          iEikonEnv );

    CListBoxView *lbview = iListBox->View();
    lbview->SetListEmptyTextL( *noConn );
    CleanupStack::PopAndDestroy( noConn );

    GetPanesL();
    SetRect( aRect );    
    ActivateL();

    isWlanSupported = FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );
    iOldConnectionCount = iConnectionArray->MdcaCount();

    CMUILOGGER_LEAVEFN( "CConnectionMonitorUiContainer::ConstructL" );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiContainer::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CConnectionMonitorUiContainer::OfferKeyEventL(
                                                   const TKeyEvent& aKeyEvent,
                                                   TEventCode aType )
    {
    TKeyResponse result( EKeyWasNotConsumed );
    if ( aType == EEventKey )
        {
        TInt index = iListBox->CurrentItemIndex();
        if ( ( aKeyEvent.iCode == EKeyBackspace ) && ( index != -1 ) )
            {
            if ( !( *iConnectionArray )[index]->IsSuspended() )
                {
                EndConnectionL();
                result = EKeyWasConsumed;
                }
            }
        else
            {
            result = iListBox->OfferKeyEventL( aKeyEvent, aType );
            }
        }
    return result;
    }

// ---------------------------------------------------------
// CConnectionMonitorUiContainer::~CConnectionMonitorUiContainer
// ---------------------------------------------------------
//
CConnectionMonitorUiContainer::~CConnectionMonitorUiContainer()
    {
    delete iListBox;
    delete iNaviDecorator;
    }

// ---------------------------------------------------------
// CConnectionMonitorUiContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CConnectionMonitorUiContainer::SizeChanged()
    {
    iListBox->SetRect( Rect() );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiContainer::CountComponentControls
// ---------------------------------------------------------
//
TInt CConnectionMonitorUiContainer::CountComponentControls() const
    {
    return 1; // only iListBox
    }

// ---------------------------------------------------------
// CConnectionMonitorUiContainer::ComponentControl( TInt aIndex ) const
// ---------------------------------------------------------
//
CCoeControl* CConnectionMonitorUiContainer::ComponentControl(
                                                          TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iListBox;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CConnectionMonitorUiContainer::OnEventL
// ---------------------------------------------------------
//
void CConnectionMonitorUiContainer::OnEventL(
                                       const CConnMonEventBase& aConnMonEvent,
                                       const TInt aIndex )
    {
    CMUILOGGER_ENTERFN( "CConnectionMonitorUiContainer::OnEventL" );
    CMUILOGGER_WRITE_F( "OnEventL type: %d", aConnMonEvent.EventType() );
    CMUILOGGER_WRITE_F( "OnEventL index: %d", aIndex );

    TBool validIndex =( iListBox->Model()->NumberOfItems() > KErrNotFound ) &&
                      ( aIndex > KErrNotFound ) &&
                      ( iListBox->Model()->NumberOfItems() >= aIndex );

    if ( aConnMonEvent.ConnectionId() )
        {
        switch ( aConnMonEvent.EventType() )
            {
            case EConnMonCreateConnection:
                {
                CMUILOGGER_WRITE( "OnEventL EConnMonCreateConnection" );
                iListBox->HandleItemAdditionL();
                break;
                }
            case EConnMonDeleteConnection:
                {
                CMUILOGGER_WRITE( "OnEventL EConnMonDeleteConnection" );
    			CMUILOGGER_WRITE_F( "validIndex: %b", validIndex );
                
                if ( validIndex )
                    {
                    TBool current
                        ( aIndex == ( TUint )iListBox->CurrentItemIndex() );
                    AknListBoxUtils::HandleItemRemovalAndPositionHighlightL(
                                iListBox, aIndex, ETrue );
                    }
                iListBox->UpdateScrollBarsL();
                break;
                }
            case EConnMonConnectionStatusChange:
            case EConnMonCreateSubConnection:
            case EConnMonDeleteSubConnection:
                {
                CMUILOGGER_WRITE( "OnEventL StatusChange SubConnection" );
                if ( validIndex )
                    {
                    iListBox->DrawItem( aIndex );
                    }
                break;
                }
            default:
                {
                CMUILOGGER_WRITE( "OnEventL default" );
                break;
                }
            }
        }
    PushAndRefreshNaviPaneL();

    CMUILOGGER_LEAVEFN( "CConnectionMonitorUiContainer::OnEventL" );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiContainer::OnTimerEventL
// ---------------------------------------------------------
//
void CConnectionMonitorUiContainer::OnTimerEventL()
    {
    iListBox->DrawNow();
    //Update the scrollbar only if connection count has changed
    //
    TInt iNewConnectionCount = iConnectionArray->MdcaCount();
    if ( iOldConnectionCount != iNewConnectionCount )
        {
        iListBox->UpdateScrollBarsL();
        }
    iOldConnectionCount = iNewConnectionCount;
    PushAndRefreshNaviPaneL();
    }

// ---------------------------------------------------------
// CConnectionMonitorUiContainer::DrawFirstAllListBoxItem
// ---------------------------------------------------------
//
void CConnectionMonitorUiContainer::DrawFirstAllListBoxItem()
    {
    TInt count = iListBox->Model()->NumberOfItems() - 1;
    for ( TInt i = count; i > -1; --i )
        {
        iListBox->DrawItem( i );
        }
    }

// ---------------------------------------------------------
// CConnectionMonitorUiContainer::SelectedConnection
// ---------------------------------------------------------
//
TInt CConnectionMonitorUiContainer::SelectedConnection()
    {
    return iListBox->CurrentItemIndex();
    }

// ---------------------------------------------------------
// CConnectionMonitorUiContainer::EndConnectionL
// ---------------------------------------------------------
//
void CConnectionMonitorUiContainer::EndConnectionL()
    {
    CMUILOGGER_ENTERFN( "CConnectionMonitorUiContainer::EndConnectionL " );
    TInt index = iListBox->CurrentItemIndex();
    if ( ( index >= 0 ) && ( index < iConnectionArray->MdcaCount() ) )
        {
        CConnectionInfoBase* connection = ( *iConnectionArray )[index];
        TUint connectionId = connection->GetConnectionId();
        HBufC* connectionName = NULL;
        HBufC* promptConnectionName = NULL;

        // Get the bearertype of a single connection
        TConnMonBearerType bearer;
        bearer = connection->GetBearerType();

        if ( isWlanSupported ) // WLAN ON
            {
            if ( ( bearer == EBearerWLAN ) && connection->IsEasyWLAN() )
                {
                /* check and load the SSID for EasyWLAN */
                connectionName = connection->GetWlanNetworkNameLC();
                CMUILOGGER_WRITE_F( "GetWlanNetworkNameLC: %S",
                                    connectionName );
                }
            else
                {
                connectionName = connection->GetNameLC();
                CMUILOGGER_WRITE_F( "GetNameLC: %S", connectionName );
                }
            }
        else // WLAN OFF
            {
            connectionName = connection->GetNameLC();
            CMUILOGGER_WRITE_F( "GetNameLC: %S", connectionName );
            }

        if ( bearer >= EBearerExternalCSD )
            {
            CMUILOGGER_WRITE( "R_QTN_CMON_CONF_END_MODEM_CONN" );
            promptConnectionName = StringLoader::LoadLC( 
                                            R_QTN_CMON_CONF_END_MODEM_CONN );
            }
        else
            {
            CMUILOGGER_WRITE( "R_QTN_CMON_CONF_END_SINGLE_CONN" );
            promptConnectionName = StringLoader::LoadLC( 
                                            R_QTN_CMON_CONF_END_SINGLE_CONN,
                                            *connectionName );
            }

        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
        TInt ret = dlg->ExecuteLD( R_END_CONN_CONFIRMATION_QUERY,
                                   *promptConnectionName );

        CleanupStack::PopAndDestroy( promptConnectionName );
        CleanupStack::PopAndDestroy( connectionName );
        
        TInt connStatus;
        iActiveWrapper->StartGetIntAttribute( connectionId,
                                             iMonitor,
                                             KConnectionStatus,
                                             ( ( TInt& )( connStatus ) ) );
        CMUILOGGER_WRITE_F( "KConnectionStatus status.Int(): %d",
                            iActiveWrapper->iStatus.Int() );
        CMUILOGGER_WRITE_F( "KConnectionStatus connStatus: %d",
                            connStatus );
        CMUILOGGER_WRITE_F( "ret ret: %d", ret );
                           
        if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
            {
            if ( KErrNone == iActiveWrapper->iStatus.Int() )
                {                
                connection->SetDeletedFromCMUI();
                
                connection->RefreshConnectionListBoxItemTextL();
                DrawFirstAllListBoxItem();
                
                iMonitor->SetBoolAttribute( connectionId, 0,
                                            KConnectionStop, ETrue );
                CMUILOGGER_WRITE( "after SetBoolAttribute" );                                            
                }
            }            
        else
            {
            if ( KErrNotFound == iActiveWrapper->iStatus.Int() )
                {
                /* It is already closed */
                /* An information note must be displayed */
                CAknGlobalNote * note = CAknGlobalNote::NewL();
                CleanupStack::PushL( note );
                HBufC* informationText =
                    StringLoader::LoadLC( R_QTN_CMON_INFO_CONN_ALREADY_END );

                note->ShowNoteL(EAknGlobalInformationNote, *informationText );
                CleanupStack::PopAndDestroy( informationText );
                CleanupStack::PopAndDestroy( note );
                CMUILOGGER_WRITE( "It is already closed" );
                }
            }
        }
    CMUILOGGER_LEAVEFN( "CConnectionMonitorUiContainer::EndConnectionL " );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiContainer::EndAllConnectionL
// ---------------------------------------------------------
//
void CConnectionMonitorUiContainer::EndAllConnectionL()
    {
    CMUILOGGER_ENTERFN( "EndAllConnectionL - start" );

    const TInt KOnlyOneConnection = 1;
    const TInt KFirstConnection = 0;
	CConnectionArray* tempConnArray =  iConnectionArray->DeepCopyL();	
	CleanupStack::PushL( tempConnArray );
	
    TInt connectionCount = tempConnArray->MdcaCount();
    HBufC* promptTxt = NULL;
    if ( connectionCount == KOnlyOneConnection )
        {
        HBufC* temp = ( *tempConnArray )[KFirstConnection]->GetNameLC();
        TName iapName = *temp;
        CleanupStack::PopAndDestroy( temp );
        promptTxt = StringLoader::LoadLC( R_QTN_CMON_CONF_END_SINGLE_CONN,
                                       iapName );
        }
    else
        {
        promptTxt = StringLoader::LoadLC( R_QTN_CMON_CONF_END_ALL_CONNS,
                                       connectionCount );
        TPtr temp( promptTxt->Des() );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( temp );
                                       
        }
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    TInt ret = dlg->ExecuteLD( R_END_CONN_CONFIRMATION_QUERY, *promptTxt );
    CleanupStack::PopAndDestroy( promptTxt );

    CConnectionInfoBase* connection = NULL;
    TUint connectionId( 0 );

    CMUILOGGER_WRITE_F( "ret: %d", ret );

    /* End connections one by one, First CSD, after GPRS and WLAN. */
    if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
        {
        for ( TUint i = 0; i < iConnectionArray->MdcaCount(); ++i )
            {
            connection = ( *iConnectionArray )[i];
            connection->SetAllDeletedFromCMUI();
            connection->RefreshConnectionListBoxItemTextL();
            }
              
        DrawFirstAllListBoxItem();
        
        TConnMonBearerType bearer;
        for ( TUint i = 0; i < connectionCount; ++i )
            {
            CMUILOGGER_WRITE_F( "i: %d", i );

            connection = ( *tempConnArray )[i];

			bearer = connection->GetBearerType();
		    switch ( bearer )
		        {
		        case EBearerCSD:
		        case EBearerExternalCSD:
		        case EBearerWcdmaCSD:
		        case EBearerExternalWcdmaCSD:
		        case EBearerHSCSD:
		        case EBearerExternalHSCSD:
		            {
		            connectionId = connection->GetConnectionId();
	            	iMonitor->SetBoolAttribute( connectionId, 0,
	                                        	KConnectionStop, ETrue );	
		            CMUILOGGER_WRITE_F( "CSD i: %d", i );
		            CMUILOGGER_WRITE_F( "connectionId: %d", connectionId );
		            break;
		            }		        
		        default :
		            {
		            break;
		            }
		        }
            }
            
        for ( TUint i = 0; i < connectionCount; ++i )
            {
            CMUILOGGER_WRITE_F( "i: %d", i );

            connection = ( *tempConnArray )[i];

			bearer = connection->GetBearerType();
		    switch ( bearer )
		        {
                case EBearerGPRS:
                case EBearerWCDMA:
                case EBearerEdgeGPRS:
                case EBearerExternalGPRS:
                case EBearerExternalWCDMA:
                case EBearerExternalEdgeGPRS:
                case EBearerWLAN:
		            {
		            connectionId = connection->GetConnectionId();
	            	iMonitor->SetBoolAttribute( connectionId, 0,
	                                        	KConnectionStop, ETrue );	
		            CMUILOGGER_WRITE_F( "GPRS i: %d", i );
		            CMUILOGGER_WRITE_F( "connectionId: %d", connectionId );
		            break;
		            }		        
		        default :
		            {
		            break;
		            }
		        }
            }
        }
     else
        {
        for ( TUint i = 0; 
             ( ( i < connectionCount ) && 
               ( KErrNotFound != iActiveWrapper->iStatus.Int() ) ); ++i )
            {
            connection = ( *tempConnArray )[i];
            connectionId = connection->GetConnectionId();
        	TInt connStatus;
        	iActiveWrapper->StartGetIntAttribute( connectionId,
                                                  iMonitor,
                                                  KConnectionStatus,
                                                  ( ( TInt& )( connStatus ) ) );
            }
            
        if ( KErrNotFound == iActiveWrapper->iStatus.Int() )
            {
            /* It is already closed */
            /* An information note must be displayed */
            CAknGlobalNote * note = CAknGlobalNote::NewL();
            CleanupStack::PushL( note );
            HBufC* informationText =
                StringLoader::LoadLC( R_QTN_CMON_INFO_CONN_ALREADY_END );

            note->ShowNoteL(EAknGlobalInformationNote, *informationText );
            CleanupStack::PopAndDestroy( informationText );
            CleanupStack::PopAndDestroy( note );
            CMUILOGGER_WRITE( "It is already closed in All" );
            }
        }
        
    CleanupStack::PopAndDestroy( tempConnArray );
        
    CMUILOGGER_LEAVEFN( "EndAllConnectionL - end" );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiContainer::ListBox
// ---------------------------------------------------------
//
CAknDoubleGraphicStyleListBox* CConnectionMonitorUiContainer::ListBox() const
    {
    return iListBox;
    }

// ---------------------------------------------------------
// CConnectionMonitorUiContainer::SetIconsL()
// ---------------------------------------------------------
//
void CConnectionMonitorUiContainer::SetIconsL()
    {
    CMUILOGGER_ENTERFN( "SetIconsL" );
    
    TFileName iconsFileName;

    iconsFileName.Append( KDriveZ );

    iconsFileName.Append( KDC_APP_BITMAP_DIR );

    iconsFileName.Append( KConnectionMonitorUiMBMFile );

    CArrayPtr< CGulIcon >* icons = new( ELeave ) CAknIconArray( KGranularity );
    CleanupStack::PushL( icons );

    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();

    icons->AppendL( AknsUtils::CreateGulIconL( skinInstance,
                            KAknsIIDQgnPropWmlGprs,
                            iconsFileName,
                            EMbmConnectionmonitoruiQgn_prop_wml_gprs,
                            EMbmConnectionmonitoruiQgn_prop_wml_gprs_mask ) );

    icons->AppendL( AknsUtils::CreateGulIconL( skinInstance,
                            KAknsIIDQgnPropWmlCsd,
                            iconsFileName,
                            EMbmConnectionmonitoruiQgn_prop_wml_csd,
                            EMbmConnectionmonitoruiQgn_prop_wml_csd_mask ) );

    icons->AppendL( AknsUtils::CreateGulIconL( skinInstance,
                        KAknsIIDQgnPropWmlHscsd,
                        iconsFileName,
                        EMbmConnectionmonitoruiQgn_prop_wml_hscsd,
                        EMbmConnectionmonitoruiQgn_prop_wml_hscsd_mask ) );

    icons->AppendL( AknsUtils::CreateGulIconL( skinInstance,
                  KAknsIIDQgnPropEmpty,
                  iconsFileName,
                  EMbmConnectionmonitoruiQgn_prop_empty,
                  EMbmConnectionmonitoruiQgn_prop_empty_mask ) );

    icons->AppendL( AknsUtils::CreateGulIconL( skinInstance,
              KAknsIIDQgnPropWmlSms,
              iconsFileName,
              EMbmConnectionmonitoruiQgn_prop_cmon_gprs_suspended,
              EMbmConnectionmonitoruiQgn_prop_cmon_gprs_suspended_mask ) );

    icons->AppendL( AknsUtils::CreateGulIconL( skinInstance,
                  KAknsIIDQgnPropWlanBearer,
                  iconsFileName,
                  EMbmConnectionmonitoruiQgn_prop_wlan_bearer,
                  EMbmConnectionmonitoruiQgn_prop_wlan_bearer_mask ) );

    iListBox->ItemDrawer()->FormattedCellData()->SetIconArrayL( icons );
    CleanupStack::Pop( icons ); // icons
    CMUILOGGER_LEAVEFN( "SetIconsL" );        
    
    }

// ---------------------------------------------------------------------------
// void CConnectionMonitorUiContainer::HandleResourceChange( TInt aType )
// Handle resource change events.
// ---------------------------------------------------------------------------
//
void CConnectionMonitorUiContainer::HandleResourceChange( TInt aType )
    {
    CMUILOGGER_ENTERFN( "HandleResourceChange" );
    CMUILOGGER_WRITE_F( "aType: %d", aType );            
    
    CCoeControl::HandleResourceChange( aType );
    
    if ( aType == KAknsMessageSkinChange )
        {
        TInt error( KErrNone );
        TRAP( error,  SetIconsL() );
        CMUILOGGER_WRITE_F( "KAknsMessageSkinChange error: %d", error );
        SizeChanged();
        }
        
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, 
                                           mainPaneRect );        
        SetRect( mainPaneRect );                
        DrawNow();                
        }        
    CMUILOGGER_LEAVEFN( "HandleResourceChange" );        
    }

// ---------------------------------------------------------
// CConnectionMonitorUiContainer::GetPanesL
// ---------------------------------------------------------
//
void CConnectionMonitorUiContainer::GetPanesL()
    {
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();

    iNaviPane =  STATIC_CAST( CAknNavigationControlContainer*,
               statusPane->ControlL( TUid::Uid(EEikStatusPaneUidNavi ) ) );

    PushAndRefreshNaviPaneL();
    }

// ---------------------------------------------------------
// CConnectionMonitorUiContainer::PushAndRefreshNaviPaneL
// ---------------------------------------------------------
//
void CConnectionMonitorUiContainer::PushAndRefreshNaviPaneL()
    {
    CMUILOGGER_ENTERFN( "PushAndRefreshNaviPaneL - start" );

    HBufC* textOfNaviPane = NULL;
    TUint countOfConnection = iConnectionArray->NumberOfActiveConnections();
    CMUILOGGER_WRITE_F( "countOfConnection: %d", countOfConnection );

    if ( iNaviDecorator )
        {
        PopNaviPane();
        }

    if ( countOfConnection )
        {
        if ( countOfConnection == 1 )
            {
            textOfNaviPane = StringLoader::LoadLC(
                                         R_QTN_CMON_NAVI_ONE_ACTIVE_CONN );
            }
        else
            {
            textOfNaviPane = StringLoader::LoadLC(
                                            R_QTN_CMON_NAVI_MANY_ACTIVE_CONNS,
                                            countOfConnection );
            }
        }
    else
        {
        textOfNaviPane =  KEmpty().AllocLC();
        }

    TPtr temp( textOfNaviPane->Des() );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( temp );
        
    iNaviDecorator = iNaviPane->CreateNavigationLabelL( *textOfNaviPane );
    CleanupStack::PopAndDestroy( textOfNaviPane );

    iNaviPane->PushL( *iNaviDecorator );
    CMUILOGGER_LEAVEFN( "PushAndRefreshNaviPaneL - end" );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiContainer::PopNaviPane
// ---------------------------------------------------------
//
void CConnectionMonitorUiContainer::PopNaviPane()
    {
    iNaviPane->Pop( iNaviDecorator );
    delete iNaviDecorator;
    iNaviDecorator = NULL;
    }   
    
// ---------------------------------------------------------
// CConnectionMonitorUiContainer::FocusChanged
// ---------------------------------------------------------
//
void CConnectionMonitorUiContainer::FocusChanged( TDrawNow aDrawNow )
    {
    CMUILOGGER_ENTERFN( "CConnectionMonitorUiContainer::FocusChanged" );
    CCoeControl::FocusChanged( aDrawNow );    
    if ( iListBox )
        {
        iListBox->SetFocus( IsFocused() );
        }        
    CMUILOGGER_LEAVEFN( "CConnectionMonitorUiContainer::FocusChanged" );    
    }

// ---------------------------------------------------------
// CConnectionMonitorUiContainer::GetHelpContext()
// ---------------------------------------------------------
//
void CConnectionMonitorUiContainer::GetHelpContext( TCoeHelpContext& aContext) const
    {
    aContext.iMajor = KUidConnMonUI;
    aContext.iContext = KCMON_HLP_CMON_MAIN_VIEW;
    }


// End of File
