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
* Description:  Implementation of application setting UI.
*
*/

#include <coemain.h>
#include <featmgr.h>
#include <data_caging_path_literals.hrh>
#include <StringLoader.h>
#include <cmmanager.rsg>
#include <AknsUtils.h>
#include <ConnectionUiUtilities.h>


#include <cmcommonconstants.h>
#include "cmapplicationsettingsui.h"
#include <cmmanager.h>
#include "cmmanagerimpl.h"
#include "cmdestinationimpl.h"
#include "cmmanagerimpl.h"
#include <cmpluginbaseeng.h>
#include <cmpluginbase.h>
#include "cmconnsettingsuiimpl.h"
#include "cmconnselectrbpage.h"

using namespace CMManager;

typedef struct
    {
    TInt                iCmIapId;
    TInt                iCmDefaultPriority;
    }TCmListItem;

//---------------------------------------------------------------------------
//  CCmConnSettingsUiImpl::NewL
//---------------------------------------------------------------------------
//
EXPORT_C CCmConnSettingsUiImpl* CCmConnSettingsUiImpl::NewL( CCmManagerImpl* aCCmManagerImpl )
    {
    CCmConnSettingsUiImpl* self = 
                                new (ELeave) CCmConnSettingsUiImpl();
    CleanupStack::PushL( self );
    self->ConstructL( aCCmManagerImpl );
    CleanupStack::Pop( self );   // self
    return self;
    }
    
//---------------------------------------------------------------------------
//  CCmConnSettingsUiImpl::NewL
//---------------------------------------------------------------------------
//
CCmConnSettingsUiImpl* CCmConnSettingsUiImpl::NewWithoutTablesL()
    {
    CCmConnSettingsUiImpl* self = 
                                new (ELeave) CCmConnSettingsUiImpl();
    CleanupStack::PushL( self );
    self->iCreateTables = EFalse;
    self->ConstructL( NULL );
    CleanupStack::Pop( self );   // self
    return self;
    }    

//---------------------------------------------------------------------------
//  CCmConnSettingsUiImpl::TCmDCSettingSelection
//---------------------------------------------------------------------------
//
CCmConnSettingsUiImpl::CCmConnSettingsUiImpl()
    : iResourceReader( *CCoeEnv::Static() )
    , iAreDestinations ( ETrue )
    , iShowAlwaysAsk( EFalse )
    , iShowEasyWlan( EFalse )
    , iShowVirtualCMs( ETrue )
    , iShowEmptyDestinations( EFalse )
    , iShowAskOnce( EFalse )
    , iShowUncat( EFalse )
    , iShowDefConn( EFalse )
    , iUsePassedDestinationArray( EFalse )
    , iHelpContext ( KSET_HLP_APP_DEST_ASSOC_VIEW )
    , iCreateTables( ETrue )
    {
    }

//---------------------------------------------------------------------------
//  CCmConnSettingsUiImpl::~TCmDCSettingSelection
//---------------------------------------------------------------------------
//
CCmConnSettingsUiImpl::~CCmConnSettingsUiImpl()
    {
    if ( iOwnCmManager )
        {
        delete iCmManagerImpl;
        }    
    iResourceReader.Close();
    iItems.Close();
    iPassedDestinations.Close();
    }

//---------------------------------------------------------------------------
// CCmConnSettingsUiImpl::LoadResourceL
// Load the resource for the dll module
//---------------------------------------------------------------------------
//
TInt CCmConnSettingsUiImpl::LoadResourceL ()
    {    
    // Add resource file.
    TParse* fp = new(ELeave) TParse(); 
    TInt err = fp->Set( KACMManagerResDirAndFileName, 
                        &KDC_RESOURCE_FILES_DIR, 
                        NULL ); 
    if ( err != KErrNone)
        {
        User::Leave( err );
        }   

    TFileName resourceFileNameBuf = fp->FullName();
    delete fp;

    iResourceReader.OpenL( resourceFileNameBuf );
    return err;
    }

//---------------------------------------------------------------------------
//  CCmConnSettingsUiImpl::ConstructL()
//---------------------------------------------------------------------------
//
void CCmConnSettingsUiImpl::ConstructL( CCmManagerImpl* aCCmManagerImpl )
    {
    iOwnCmManager = !(aCCmManagerImpl);
    if ( iOwnCmManager )
        {
        if (iCreateTables)
            {
            iCmManagerImpl = CCmManagerImpl::NewL();            
            }
        else
            {
            iCmManagerImpl = CCmManagerImpl::NewL(EFalse);                            
            }
        }
    else
        {
        iCmManagerImpl = aCCmManagerImpl;
        }
    LoadResourceL();
    AknsUtils::SetAvkonSkinEnabledL( ETrue );
    }

//---------------------------------------------------------------------------
// CCmConnSettingsUiImpl::ShowConnSelectRadioPageL
//---------------------------------------------------------------------------
//
TBool CCmConnSettingsUiImpl::ShowConnSelectRadioPageL( 
                                           TCmDCSettingSelection& aSelection, 
                                           const TInt aTitleStringResource,
                                           TBearerFilterArray& aFilterArray )
    {
    TBool retVal( EFalse );
//    aSelection.iResult = EDCAlwaysAsk;

    CDesCArrayFlat* items = 
                    new ( ELeave ) CDesCArrayFlat( KCmArrayBigGranularity );
    CleanupStack::PushL( items );
    
    if (iAreDestinations)
        {
        BuildDestinationArrayL( items, aFilterArray );        
        }
    else    
        {
        BuildCMArrayL( items, aFilterArray ); 
               
        }
    // Uncategorized connection methods
    RPointerArray<CCmPluginBase> uncatArray( KCmArraySmallGranularity );
    CCmManagerImpl::CleanupResetAndDestroyPushL< RPointerArray<CCmPluginBase> >( uncatArray );
    if ( iShowUncat )
        {
        BuildUncatArrayL( uncatArray, items,
                    aFilterArray);
        }
    //appending items in reverse order of priority    
    // Check if Ask Once should be shown 
    if ( iShowAskOnce )
        {
        HBufC* buf = StringLoader::LoadLC( R_CMMANAGERUI_SETT_ASK_ONCE );
        items->InsertL( 0, *buf );
        iItems.InsertL( KDestItemAskOnce, 0 );
        CleanupStack::PopAndDestroy( buf );    
        }

    // Check if Always Ask should be shown
    if ( iShowAlwaysAsk )
        {
        HBufC* buf = StringLoader::LoadLC( R_CMMANAGERUI_SETT_ALWAYS_ASK );
        items->InsertL( 0, *buf );
        iItems.InsertL( KDestItemAlwaysAsk, 0 );
        CleanupStack::PopAndDestroy( buf );    
        }
    // Check if Default Connection should be shown 
    //feautre flag
    if ( iCmManagerImpl->IsDefConnSupported() )
        {
        //client's selection
        if ( iShowDefConn )
            {
            HBufC* buf = StringLoader::LoadLC( R_CMMANAGERUI_SETT_DEFAULT_CONNECTION );
            items->InsertL( 0, *buf );
            iItems.InsertL( KDestItemDefaultConnection, 0 );
            CleanupStack::PopAndDestroy( buf );    
            } 
        }
    if (!items->Count())
        {
        User::Leave(KErrConnectionNotFound);           
        }
    
    TInt listIndex(0);
    
    if (iAreDestinations)
        {
        switch ( aSelection.iResult )
                {
                case EDCDestination:
                    {
                    TInt newIndex = iItems.Find(aSelection.iId);
                    if (newIndex >=0)
                        {
                        listIndex = newIndex;
                        }
                    break;
                    }
                case EDCConnectionMethod:
                    {
                    TInt err(KErrNotFound);
                    CCmPluginBase* cm = NULL;
                    TRAP( err, cm = iCmManagerImpl->GetConnectionMethodL(aSelection.iId));
                    if ( err == KErrNone )
                        {
                        // connection method is valid
                        CleanupStack::PushL( cm );
                        TUint32 parentId = 0;
                        CCmDestinationImpl* parent = cm->ParentDestination();
                        if (parent)
                            {
                            // it has a parent, get it's id
                            parentId = parent->Id();
                            delete parent;
                            }
                        else
                            {
                            // no parent means Uncategorised
                            parentId = KDestItemUncategorized;
                            }
                        // get the index based on id    
                        TInt tempIndex = iItems.Find( parentId );
                        if ( tempIndex != KErrNotFound )
                            {
                            // found destination id in list
                            listIndex = tempIndex;
                            }                            
                        CleanupStack::PopAndDestroy( cm );
                        }
                    break;
                    }
                case EDCDefaultConnection:
                    {
                    TInt newIndex = iItems.Find(KDestItemDefaultConnection);
                    if (newIndex >=0)
                        {
                        listIndex = newIndex;
                        }
                    break;
                    }
                case EDCAlwaysAsk:
                    {
                    TInt newIndex = iItems.Find(KDestItemAlwaysAsk);
                    if (newIndex >=0)
                        {
                        listIndex = newIndex;
                        }
                    break;
                    }
                case EDCAskOnce:
                    {
                    TInt newIndex = iItems.Find(KDestItemAskOnce);
                    if (newIndex >=0)
                        {
                        listIndex = newIndex;
                        }
                    break;
                    }                
                default: // use default value (0)
                    {
                    break;
                    }                            
                }
            }
         else
            {
            if (aSelection.iResult == EDCConnectionMethod)
                {
                TInt newIndex = iItems.Find(aSelection.iId);
                if (newIndex >=0)
                    {
                    listIndex = newIndex;
                    }
                }
            }
    
    if ( aTitleStringResource == R_CMMANAGERUI_DEFAULT_CONNECTION_TITLE )
        {
        // It is not allowed to choose Connection Methods 
        // in Default connection -dialog.
        //
        iOpenDestination = EFalse;
        }
    
    if ( UserSelectionDlgL( listIndex, items, aSelection, 
                            aTitleStringResource,
                            iOpenDestination) )
        {
        // Handle selection
        
        switch ( aSelection.iResult )
            {
            case EDCConnectionMethod:
                {
                // from the first list?
                if (!iAreDestinations)
                    {
                    aSelection.iId = iItems[listIndex];
                    retVal = ETrue;                    
                    }
                else
                    {
                    // Uncategorised?
                    TUint32 destId = iItems[ listIndex ];
                    if ( destId == KDestItemUncategorized )
                        {
                        TBearerFilterArray emptyFilter;
                        retVal = ShowConnectionMethodsL( uncatArray, 
                                        aSelection, aTitleStringResource, emptyFilter );
                        }
                    else
                        {
                        RPointerArray<CCmPluginBase> cmArray ( KCmArrayMediumGranularity );
                        if ( iShowVirtualCMs ) //create a flat list to show cms in embedded destinations
                            {
                            iCmManagerImpl->CreateFlatCMListLC( destId, cmArray);
                            }
                         else
                            {
                            //append only the cms in the destination
                            iCmManagerImpl->AppendCmListLC( destId, cmArray ); 
                            }
                        retVal = ShowConnectionMethodsL( cmArray, 
                                        aSelection, aTitleStringResource,
                                        aFilterArray );
                        CleanupStack::PopAndDestroy( &cmArray );
                        }                    
                    }
                break;
                }
            case EDCDestination:
                {
                // Store the destination Id
                aSelection.iId = iItems[listIndex];
                retVal = ETrue;
                break;
                }
            case EDCAlwaysAsk:
                {
                // nothing more to do
                retVal = ETrue;
                break;
                }        
            case EDCDefaultConnection:
                {
                // nothing more to do
                retVal = ETrue;
                break;
                }            
            case EDCAskOnce:
                {
                // nothing more to do
                retVal = ETrue;
                break;
                }            
            default:
                {
                break;
                }
            }
        }

    CleanupStack::PopAndDestroy( &uncatArray );
    CleanupStack::PopAndDestroy( items );

    return retVal;
    }


//---------------------------------------------------------------------------
//  CCmConnSettingsUiImpl::RunSettingsL()
//---------------------------------------------------------------------------
//
EXPORT_C TBool CCmConnSettingsUiImpl::RunApplicationSettingsL( 
                                        TCmSettingSelection& aSelection, 
                                        TUint aListItems,
                                        TBearerFilterArray& aFilterArray)
    {
    iItems.Close();
    iAreDestinations = ETrue;
    if (!aListItems)
        {
        User::Leave(KErrArgument);        
        }
    if ( !iCmManagerImpl->IsDefConnSupported() &&
         !(aListItems & EShowAlwaysAsk) &&  
         !(aListItems & EShowConnectionMethods) &&  
         !(aListItems & EShowDestinations) 
         )
        {
        // default conn would be the only one but not supported
        User::Leave(KErrNotSupported);   
        }

    if ( aListItems & EShowDefaultConnection )
        {
        EnableDefConn( ETrue );        
        }
    else
        {
        EnableDefConn( EFalse );                
        }
    if (aListItems & EShowAlwaysAsk)
        {
        EnableAlwaysAsk( ETrue );
        }
    else
        {
        EnableAlwaysAsk( EFalse );
        }
    if ( (aListItems & EShowConnectionMethods) && (aListItems & EShowDestinations) )
        {
        iOpenDestination = ETrue;
        iShowUncat = ETrue;
        }
    else
        {
        iOpenDestination = EFalse;
        }
    if ( (aListItems & EShowConnectionMethods) && !(aListItems & EShowDestinations) )
        {
        iAreDestinations = EFalse;
        GetCMArrayL( );            
        }
    else if ( aListItems & EShowDestinations )
        {
        if (iUsePassedDestinationArray)
            {
            // now fill up iItems
            for( TInt i=0; i<iPassedDestinations.Count(); i++)
                {
                iItems.Append(iPassedDestinations[i]);
                }
            }
        else
            {
            iCmManagerImpl->AllDestinationsL( iItems );
            TInt i( 0 );
            TInt count = iItems.Count();
            for ( i=0; i < count; i++ )
                {
                if ( iItems[i] > 0 && iItems[i] < 255 )
                    {
                    CCmDestinationImpl* dest = 
        	                            iCmManagerImpl->DestinationL( iItems[i] );
                    CleanupStack::PushL( dest );
        	        if ( dest->IsHidden() )
                        {
                        iItems.Remove(i);
                        i--;
                        count = count-1;
                        }
                    CleanupStack::PopAndDestroy();
                    }
                }
            iItems.Compress();
            }
        }

    TBool ret = EFalse;
    TCmDCSettingSelection userDefSelection;

    // convert selection
    switch ( aSelection.iResult )
        {
        case EDestination: 
            {
            userDefSelection.iResult = EDCDestination; 
            userDefSelection.iId = aSelection.iId;        
            break;
            }
            
        case EConnectionMethod: 
            {
            userDefSelection.iResult = EDCConnectionMethod; 
            userDefSelection.iId = aSelection.iId;        
            break;
            }

        case EDefaultConnection: 
            {
            userDefSelection.iResult = EDCDefaultConnection; 
            userDefSelection.iId = 0;
            break;
            }

        case EAlwaysAsk: 
            {
            userDefSelection.iResult = EDCAlwaysAsk; 
            userDefSelection.iId = 0;
            break;
            }

        default: 
            {
            // do not leave, perhaps there was no default selection
            }                            
        }
    
    
    
    if ( ShowConnSelectRadioPageL( userDefSelection, 
            R_CMMANAGERUI_SETT_DESTINATION , aFilterArray))
        {
        switch ( userDefSelection.iResult )
            {
            case EDCDestination:
                {
                aSelection.iResult = EDestination;
                aSelection.iId = userDefSelection.iId;
                break;
                }
            case EDCConnectionMethod:
                {
                aSelection.iResult = EConnectionMethod;
                aSelection.iId = userDefSelection.iId;
                break;
                }
            case EDCDefaultConnection:
                {
                aSelection.iResult = EDefaultConnection;
                aSelection.iId = 0;
                break;
                }
            case EDCAlwaysAsk:
                {
                aSelection.iResult = EAlwaysAsk;
                aSelection.iId = 0;
                break;
                }
            default: //should not ever get here, unhandled branch
                {
                User::Leave( KErrGeneral );
                break;
                }                            
            }
        ret = ETrue;
        }
    else
        {
        ret = EFalse;
        }
    return ret;        
    }    

    
//---------------------------------------------------------------------------
//  CCmConnSettingsUiImpl::RunDefaultConnecitonRBPageL()
//---------------------------------------------------------------------------
//
TBool CCmConnSettingsUiImpl::RunDefaultConnecitonRBPageL( 
                                          TCmDCSettingSelection& aSelection )
    {
    iHelpContext = KSET_HLP_DEFAULT_CONN;
    iItems.Close();
    iOpenDestination = ETrue;
    iCmManagerImpl->AllDestinationsL( iItems );
    EnableAlwaysAsk( EFalse );
    EnableAskOnce( EFalse );
    EnableUncat( EFalse );
    TBearerFilterArray aFilterArray;
    iAreDestinations = ETrue;
    iShowEmptyDestinations = ETrue;
    return  ShowConnSelectRadioPageL( aSelection , 
        R_CMMANAGERUI_DEFAULT_CONNECTION_TITLE, aFilterArray );
    }
//---------------------------------------------------------------------------
//  CCmConnSettingsUiImpl::GetCMArrayL()
//---------------------------------------------------------------------------
//
void CCmConnSettingsUiImpl::GetCMArrayL( )
    {
    iCmManagerImpl->ConnectionMethodL( iItems, ETrue, EFalse,EFalse, ETrue );
    TInt count = iItems.Count();

    if ( !iShowVirtualCMs )
        {
        // Don't show virtual CMs
        for ( TInt i = 0; i < count; i++ )
            {
            if ( IsCmVirtualL( iItems[i] ) )
                {
                iItems.Remove( i );
                count--;
                i--;
                }
            }
        }
    }

//---------------------------------------------------------------------------
//  CCmConnSettingsUiImpl::BuildCMArrayL()
//---------------------------------------------------------------------------
//
void CCmConnSettingsUiImpl::BuildCMArrayL( CDesCArrayFlat* aItems, 
                                           TBearerFilterArray& aFilterArray )
    {
    // Add the New Connection Item        
    for ( TInt i = 0; i < iItems.Count(); i++ )
        {
        CCmPluginBase* cm = NULL;
        TInt err(KErrNone);
        TRAP( err, cm = iCmManagerImpl->GetConnectionMethodL(iItems[i]));
        if (err == KErrNoMemory)
        {
            User::Leave(KErrNoMemory);
        }
        else if (err != KErrNone)
        {
            continue;
        }                            
        CleanupStack::PushL( cm );
        if (aFilterArray.Count())
            {
            TUint cmBearerType = cm->GetIntAttributeL( ECmBearerType );
            if ( aFilterArray.Find( cmBearerType ) == KErrNotFound )
                {
                CleanupStack::PopAndDestroy( cm );            
                iItems.Remove(i);
                i--;
                continue;    
                }
            }
        // CM Iap Id
        TUint32 cmId = cm->GetIntAttributeL( ECmId );
                                
        // Should virtual bearer types be shown?
        if ( iShowVirtualCMs || !IsCmVirtualL( cmId ) )
            {
            // CM name
            HBufC* cmName = cm->GetStringAttributeL( ECmName );
            CleanupStack::PushL( cmName );
            // embedded destination has a special text format
            aItems->AppendL( *cmName );
            CleanupStack::PopAndDestroy( cmName );
            }
        CleanupStack::PopAndDestroy( cm );            
        }
    }

//---------------------------------------------------------------------------
//  CCmConnSettingsUiImpl::BuildDestinationArray()
//---------------------------------------------------------------------------
//
void CCmConnSettingsUiImpl::BuildDestinationArrayL( CDesCArrayFlat* aItems, 
                                         TBearerFilterArray& aFilterArray )
    {
    /*
     * The behavior of this function is as follow.
     * 1. If aFilterArray.Count() == 0 (This just means no filter provided by user) then the function will
     *    show all of available destinations (if iShowVirtualCMs == ETrue; Otherwise, destination with only
     *    virtualCMs will be filted out) (default).
     * 2. If aFilterArray.Count() != 0 then destination with only unavailable bearer-type IAPs will be 
     *    filted out.
     */
    TInt count = iItems.Count();           
    // Add the New Connection Item
    for ( TInt i = 0; i < count; i++ )
        {
        // Get the destinations from their IDs
        CCmDestinationImpl* dest = 
                            iCmManagerImpl->DestinationL( iItems[i] );
        CleanupStack::PushL( dest );

        TInt cmCount = dest->ConnectionMethodCount();
        TInt validCmCount = cmCount;
        
        if ( !iUsePassedDestinationArray )
            {
            // Check if empty destinations should be displayed
            if ( !iShowVirtualCMs || aFilterArray.Count())
                {            
                for ( TInt j = 0; j < cmCount; j++ )
                    {
                    CCmPluginBase& cm = *dest->GetConnectionMethodL(j);
                    // check if this destination only contains virtual CMs
                    if (!iShowVirtualCMs)// See if the virtual CM (Default in case no filter provided by user)
                        {
                        // CM Iap Id
                        TUint32 cmIapId = cm.GetIntAttributeL( ECmId );                
                        // real CMs were found
                        if ( IsCmVirtualL( cmIapId ) )
                            {
                            validCmCount--;
                            continue;
                            }
                        // We can not break the loop with else case here
                        // for cm might be filted out by aFilterArray
                        }
                    
                    if (aFilterArray.Count()) // See if the CM is filtered out
                        {                       
                        TUint cmBearerType = cm.GetIntAttributeL( ECmBearerType );
                        if ( aFilterArray.Find( cmBearerType ) == KErrNotFound )
                            {
                            validCmCount--;
                            }
                        else
                            {
                            break;//there is at least one CM in this dest    
                            }                    
                        }
                    }
                }
            }
        
        // check if the destination should be omitted
        // check if empty destinations should be displayed
        
        // We can use a destination to make a connection only
        // if it contains something else than an empty embedded
        // destination. Otherwise, consider it empty.
        TBool canUseDestToConnect = dest->CanUseToConnectL();
        
        if ( iDestinationToOmit != iItems[i] && 
                            ( ( validCmCount && canUseDestToConnect )
                               || iShowEmptyDestinations ) )
            {
            // Check whether the MMS SNAP is in question. 
            // It should not be shown on the UI in the destinations list
            TInt snapMetadata = 0;
            TRAPD( metaErr, snapMetadata = dest->MetadataL( CMManager::ESnapMetadataPurpose ));
            if ( metaErr == KErrNone && snapMetadata == CMManager::ESnapPurposeMMS )
                {
                iItems.Remove(i);
                count--;
                i--;
                }
            else
                {
                HBufC* destName = dest->NameLC();

                aItems->AppendL( *destName );
                CleanupStack::PopAndDestroy( destName );
                }
            }
        else
            {
            iItems.Remove(i);
            count--;
            i--;
            }
            
        CleanupStack::PopAndDestroy( dest );
        }
    }
    
//---------------------------------------------------------------------------
//  CCmConnSettingsUiImpl::BuildUncatArrayL()
//---------------------------------------------------------------------------
//
void CCmConnSettingsUiImpl::BuildUncatArrayL( RPointerArray<CCmPluginBase>& aUncatArray,
                                           CDesCArrayFlat* aItems, 
                                           TBearerFilterArray& aFilterArray )
    {
    if ( iShowEasyWlan )
        {
        // Get the uncategorised CMs including EasyWLAN
        iCmManagerImpl->AppendUncatCmListL( aUncatArray, ETrue, ETrue, ETrue );
        }
    else
        {
        iCmManagerImpl->AppendUncatCmListL( aUncatArray );
        }
    
    // Remove hidden Connection Methods
    for ( TInt i = 0; i < aUncatArray.Count(); i++ )
        {
        if ( aUncatArray[i]->GetBoolAttributeL( ECmHidden ) )
            {
            delete aUncatArray[i];
            aUncatArray.Remove(i);
            i--;
            }
        }
        
    if (aFilterArray.Count())
        {
        for ( TInt i = 0; i < aUncatArray.Count(); i++ )
            {
            CCmPluginBase* cm = aUncatArray[i];
            TUint cmBearerType = cm->GetIntAttributeL( ECmBearerType );
            if ( aFilterArray.Find( cmBearerType ) == KErrNotFound )
                {
                delete aUncatArray[i];
                aUncatArray.Remove(i);
                i--;
                }
            }
        }
    TInt count = aUncatArray.Count();

    if ( !iShowVirtualCMs )
        {
        // Don't show virtual CMs
        for ( TInt i = 0; i < count; i++ )
            {
            if ( IsCmVirtualL( aUncatArray[i]->GetIntAttributeL( ECmId ) ) )
                {
                delete aUncatArray[i];
                aUncatArray.Remove( i );
                count--;
                i--;
                }
            }
        }
    // Show the 'Uncategorised' destination
    if ( count )
        {
        HBufC* buf = StringLoader::LoadLC( R_CMMANAGERUI_DEST_UNCATEGORIZED );
        aItems->AppendL( *buf );
        CleanupStack::PopAndDestroy( buf );        
        iItems.AppendL( KDestItemUncategorized );
        }
    }

//---------------------------------------------------------------------------
//  CCmConnSettingsUiImpl::UserSelectionDlgL()
//---------------------------------------------------------------------------
//
TBool CCmConnSettingsUiImpl::UserSelectionDlgL( 
                                        TInt& aDestSelected, 
                                        CDesCArrayFlat* aItems, 
                                        TCmDCSettingSelection& aSelection, 
                                        const TInt aTitleStringResource, 
                                        TBool aOpenDestination)
    {
    // Should an item be highlighted?
    if ( aSelection.iResult == EDCDestination )
        {
        for ( TInt i = 0; i < iItems.Count(); i++ )
            {
            if ( iItems[i] == aSelection.iId )
                {
                aDestSelected = i;
                }
            }
        }
    
    HBufC* buf = StringLoader::LoadLC( aTitleStringResource );
    TInt dialogResourceId = R_RADIO_BUTTON_SETTING_PAGE_OK_CANCEL;
    if ( aOpenDestination ) // OPTIONS/CANCEL is available only if we can open
                            //destinations
        {
        dialogResourceId = R_RADIO_BUTTON_SETTING_PAGE;
        }
    CCMConnSelectRBPage* page = 
                        new ( ELeave ) CCMConnSelectRBPage( dialogResourceId,
                                                         aDestSelected, 
                                                         aItems,
                                                         *iCmManagerImpl,
                                                         aSelection.iResult,
                                                         iItems,
                                                         iAreDestinations,
                                                         aOpenDestination ,
                                                         iHelpContext );
    CleanupStack::PushL( page );
    page->SetSettingTextL( *buf );
    CleanupStack::Pop( page );
    
    CleanupStack::PopAndDestroy( buf );
    buf = NULL;
    
    return page->ExecuteLD( CAknSettingPage::EUpdateWhenChanged );
    }    

//---------------------------------------------------------------------------
//  CCmConnSettingsUiImpl::UserSelectedUncatItemL()
//---------------------------------------------------------------------------
//
TBool CCmConnSettingsUiImpl::UserSelectedUncatItemL(
                                           RPointerArray<CCmPluginBase>& aUncatArray,
                                           TCmDCSettingSelection& aSelection, 
                                        const TInt aTitleStringResource )
    {
    TBool retVal( EFalse );
    CDesCArrayFlat* items = 
                    new ( ELeave ) CDesCArrayFlat( KCmArrayBigGranularity );
    CleanupStack::PushL( items );

    TInt cmSelected( 0 );

    TInt count = aUncatArray.Count();
    for ( TInt i = 0; i < count; ++i )     
        {
        CCmPluginBase* cm = NULL;
        TRAPD( err,
               cm = iCmManagerImpl->GetConnectionMethodL( aUncatArray[i]->GetIntAttributeL( ECmId) ) 
               );
        
        if ( !err )
            {
            CleanupStack::PushL( cm );
            HBufC* cmName = cm->GetStringAttributeL( ECmName );
            CleanupStack::PushL( cmName );
            items->AppendL( *cmName );
            
            // Virtual CMs have underlying CMs, highlight
            if ( cm->GetBoolAttributeL( ECmVirtual ) )
                {
                TUint32 iapId (0);
                iapId = cm->GetIntAttributeL( ECmNextLayerIapId );
                if ( iapId && iapId == aSelection.iId )
                    {
                    cmSelected = i;
                    }
                }            
            CleanupStack::PopAndDestroy( 2, cm );
            }
        else
            {
            //not handled iap, remove from list
            aUncatArray.Remove( i );
            count--;
            i--;
            }
        }
    
    
    CAknRadioButtonSettingPage* rbPage = 
                new ( ELeave ) CAknRadioButtonSettingPage(
                                    R_RADIO_BUTTON_SETTING_PAGE_OK_CANCEL, 
                                    cmSelected, 
                                    items );
    CleanupStack::PushL( rbPage );
                        
    HBufC* buf = StringLoader::LoadLC( aTitleStringResource );    
    rbPage->SetSettingTextL( *buf );
    CleanupStack::PopAndDestroy( buf );
    
    if ( rbPage->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
        {
        CCmPluginBase* cm = 
                iCmManagerImpl->GetConnectionMethodL( aUncatArray[cmSelected]->GetIntAttributeL( ECmId) );

        // ECmIapId can be surely queried here. It won't leave, but PushL to be
        // on the safe side
        CleanupStack::PushL( cm );
        aSelection.iId = cm->GetIntAttributeL( ECmId );
        CleanupStack::PopAndDestroy( cm );

        aSelection.iResult = EDCConnectionMethod; 
        
        retVal = ETrue;
        }
    CleanupStack::Pop( rbPage );
    CleanupStack::PopAndDestroy( items );
    
    return retVal;
    }


//---------------------------------------------------------------------------
//  CCmConnSettingsUiImpl::ShowConnectionMethodsL()
//---------------------------------------------------------------------------
//
TBool CCmConnSettingsUiImpl::ShowConnectionMethodsL( 
                    RPointerArray<CCmPluginBase>& aCmArray, TCmDCSettingSelection& aSelection, 
                    const TInt aTitleStringResource,
                    TBearerFilterArray& aFilterArray  )
    {
    TInt retVal ( EFalse );
    
    // List texts
    CDesCArrayFlat* items = 
            new ( ELeave ) CDesCArrayFlat( KCmArrayMediumGranularity );
    CleanupStack::PushL( items );
    
    // keeps track of the CMs in the list
    
    // idArray is used to put valid IAP ID.
    // aFilterArray might filt some of IAPs (e.g., virtual IAPs) out. In this case,
    // aCmArray can not be used to select IAP. So, idArray may be used for IAP selection
    // for it only stores valid IAPs (i.e., not filted-out IAPs).
    RArray<TUint32> idArray;
    TInt cmSelected( 0 );
    TInt j( 0 );
    
    // Collect item texts
    TInt cmCount = aCmArray.Count();
    for ( TInt i = 0; i < cmCount; i++ )
        {
        // CM info
        if (aFilterArray.Count())
            {
            TUint cmBearerType = aCmArray[i]->GetIntAttributeL( ECmBearerType );
            if ( aFilterArray.Find( cmBearerType ) == KErrNotFound )
                {
                continue;    
                }
            }
        
        // CM Iap Id
        TUint32 cmIapId = aCmArray[i]->GetIntAttributeL( ECmId );
        
        // Should virtual bearer types be shown?
        if ( iShowVirtualCMs || !IsCmVirtualL( cmIapId ) )
            {
            // CM name
            HBufC* name = aCmArray[i]->GetStringAttributeL( ECmName );
            CleanupStack::PushL( name );
            
            items->AppendL( *name );
            idArray.Append( cmIapId );
            
             // Virtual CMs have underlying CMs, highlight
            if ( cmIapId == aSelection.iId )
                {
                cmSelected = j;
                j++;
                }
            
            CleanupStack::PopAndDestroy( name );
            }
        }
    
    // create the dialog             
    CAknRadioButtonSettingPage* dlg = 
                            new ( ELeave ) CAknRadioButtonSettingPage(
                                    R_RADIO_BUTTON_SETTING_PAGE_OK_CANCEL,
                                    cmSelected, 
                                    items );
    CleanupStack::PushL( dlg );
    
    // Title
    HBufC* title = StringLoader::LoadLC ( aTitleStringResource );
    dlg->SetSettingTextL( *title );
    
    // Show the dialog
    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
        {
        aSelection.iId = idArray[cmSelected];
        aSelection.iResult = EDCConnectionMethod; 
        retVal = ETrue;
        }
    idArray.Close();
    
    CleanupStack::PopAndDestroy( title );
    CleanupStack::Pop( dlg );
    CleanupStack::PopAndDestroy( items );
    
    return retVal;
    }


//---------------------------------------------------------------------------
//  CCmConnSettingsUiImpl::IsCmVirtualL
//---------------------------------------------------------------------------
//
TBool CCmConnSettingsUiImpl::IsCmVirtualL( TUint32 aIapId )
    {
    // check if bearer is virtual
    TInt bearerType = iCmManagerImpl->GetConnectionMethodInfoIntL( 
                                                aIapId, ECmBearerType );
    return iCmManagerImpl->GetBearerInfoBoolL( bearerType, ECmVirtual );
    }


//---------------------------------------------------------------------------
//  CCmConnSettingsUiImpl::CreateDefaultConnectionNameL
//---------------------------------------------------------------------------
//
HBufC* CCmConnSettingsUiImpl::CreateDefaultConnectionNameL( 
                                            TCmDefConnValue aSelection )
    {
    HBufC* ret = NULL;
    switch ( aSelection.iType )
        {
        case ECmDefConnDestination:
            {
            CCmDestinationImpl* dest = 
                        iCmManagerImpl->DestinationL( aSelection.iId );
            CleanupStack::PushL( dest );
            ret = dest->NameLC();
            CleanupStack::Pop( ret );
            CleanupStack::PopAndDestroy( dest );
            break;
            }
        case ECmDefConnConnectionMethod:
            {
            ret = iCmManagerImpl->GetConnectionMethodInfoStringL( 
                                                aSelection.iId, ECmName );
            break;
            }
        }
    return ret;
    }
   


//---------------------------------------------------------------------------
//  CCmConnSettingsUiImpl::SetDestinationArrayToUseL
//---------------------------------------------------------------------------
//
EXPORT_C void CCmConnSettingsUiImpl::SetDestinationArrayToUseL(
                                         RArray<TUint32> aDestinationArray )
    {
    iPassedDestinations.Close();
    for (TInt i=0; i<aDestinationArray.Count(); i++)
        {
        iPassedDestinations.AppendL(aDestinationArray[i]);
        }
    
    iUsePassedDestinationArray = ETrue;
    }


//---------------------------------------------------------------------------
//  CCmConnSettingsUiImpl::ClearDestinationArrayToUse
//---------------------------------------------------------------------------
//
EXPORT_C void CCmConnSettingsUiImpl::ClearDestinationArrayToUse()
    {
    iPassedDestinations.Close();
    iUsePassedDestinationArray = EFalse;
    }


