/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CCmSettingsUiImpl.
*
*/

#include <cmmanager.rsg>

#include "cmsettingsuiimpl.h"
#include "cmmanagerimpl.h"
#include "destdlg.h"
#include <cmcommonui.h>
#include <cmpluginbase.h>
#include <cmmanagerdef.h>
#include <cmcommonconstants.h>
#include <cmpluginwlandef.h>
#include "selectdestinationdlg.h"
#include <data_caging_path_literals.hrh>

//-----------------------------------------------------------------------------
//  CCmSettingsUiImpl::NewL()
//-----------------------------------------------------------------------------
//
CCmSettingsUiImpl* CCmSettingsUiImpl::NewL()
    {
    CCmSettingsUiImpl* self = new (ELeave) CCmSettingsUiImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

//-----------------------------------------------------------------------------
//  CCmSettingsUiImpl::CCmSettingsUiImpl
//-----------------------------------------------------------------------------
//
CCmSettingsUiImpl::CCmSettingsUiImpl():
    iResourceReader( *CCoeEnv::Static() )
    {
    }

//-----------------------------------------------------------------------------
// CCmSettingsUiImpl::LoadResourceL
// Load the resource for the dll module
// ----------------------------------------------------------------------------
//
TInt CCmSettingsUiImpl::LoadResourceL ()
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

    iResourceReader.OpenL (resourceFileNameBuf);
    return err;
    }

//-----------------------------------------------------------------------------
//  CCmSettingsUiImpl::~CCmSettingsUiImpl
//-----------------------------------------------------------------------------
//
CCmSettingsUiImpl::~CCmSettingsUiImpl()
    {
    delete iCmManagerImpl;
    iResourceReader.Close();    
    }

//-----------------------------------------------------------------------------
//  CCmSettingsUiImpl::ConstructL
//-----------------------------------------------------------------------------
//
void CCmSettingsUiImpl::ConstructL()
    {
    iCmManagerImpl = CCmManagerImpl::NewL();
    LoadResourceL();
    AknsUtils::SetAvkonSkinEnabledL( ETrue );
    }
    
//-----------------------------------------------------------------------------
//  CCmSettingsUiImpl::RunSettingsL
//-----------------------------------------------------------------------------
//
TInt CCmSettingsUiImpl::RunSettingsL()
    {
    CDestDlg* destDlg = new (ELeave) CDestDlg;

    TUint32 selected = 0;
    // No need to push it on to the cleanup stack as CEikDialog::PrepareLC
    // does this favour for us.
    
    TBool exiting = EFalse;
    
    TInt result = destDlg->ConstructAndRunLD( 
                                        iCmManagerImpl, 0, selected, exiting );
        
    return ( exiting ) ? KDialogUserExit: KDialogUserBack;
    }    

//-----------------------------------------------------------------------------
//  CCmSettingsUiImpl::SelectDestinationDlgL
//-----------------------------------------------------------------------------
//
TBool CCmSettingsUiImpl::SelectDestinationDlgL( TUint32& aDestinationId )
    {
    CSelectDestinationDlg* dlg = CSelectDestinationDlg::NewL( aDestinationId,
                                                              *iCmManagerImpl );
                                                              
    return dlg->ExecuteLD( R_CMMANAGER_LIST_QUERY_WITH_MSGBOX );    
    }
//-----------------------------------------------------------------------------
//  CCmSettingsUiImpl::AddCmL
//-----------------------------------------------------------------------------
//
TUint32 CCmSettingsUiImpl::AddCmL( TUint32& aDestUid, TUint32 aBearerType )
    {
    // Add new connection method
    TUint32 addedIapId(0);

    CCmDestinationImpl* destImpl = iCmManagerImpl->DestinationL( aDestUid );
    CleanupStack::PushL(destImpl);
    
    // Check first if parent destination is protected
    if ( destImpl->ProtectionLevel() == CMManager::EProtLevel1 )
        {
        TCmCommonUi::ShowNoteL( R_CMWIZARD_CANNOT_PERFORM_FOR_PROTECTED,
                                TCmCommonUi::ECmErrorNote );
        }
    else
        {
        // Automatically check for available conn methods?
    
        TBool manualConfigure = ETrue;    
        if (aBearerType == KUidWlanBearerType)
            {
            TInt retval =
                    TCmCommonUi::ShowConfirmationQueryL( R_CMWIZARD_AUTOMATIC_CHECK );
    
            manualConfigure = ( retval == EAknSoftkeyYes || retval == EAknSoftkeyOk )
                                      ?  EFalse : ETrue;
            }
            
        CCmPluginBase* plugin = destImpl->CreateConnectionMethodL( aBearerType );
        CleanupStack::PushL( plugin );
       
        // Bearer-specific UI-supported initialization is done by plug-ins
        if ( plugin->InitializeWithUiL( manualConfigure ) )
            {
            destImpl->UpdateL();  // commit changes

            RArray<TUint32> additionalCms;
            CleanupClosePushL (additionalCms);
            plugin->GetAdditionalUids( additionalCms );
            //if there are additional cms created, move them to the target destination as well
            for ( TInt i = 0; i<additionalCms.Count(); i++)
                {
                CCmPluginBase* cm = iCmManagerImpl->GetConnectionMethodL( additionalCms[i] );
                CleanupStack::PushL(cm);
                iCmManagerImpl->CopyConnectionMethodL( *destImpl, *cm );
                CleanupStack::PopAndDestroy( cm );
                }
            CleanupStack::PopAndDestroy( &additionalCms );   
            addedIapId = plugin->GetIntAttributeL( CMManager::ECmIapId );
            }
        CleanupStack::PopAndDestroy(plugin);
        }    
    CleanupStack::PopAndDestroy(destImpl);
    
    return addedIapId;
    }

//-----------------------------------------------------------------------------
//  CCmSettingsUiImpl::EditCmL
//-----------------------------------------------------------------------------
//
TInt CCmSettingsUiImpl::EditCmL( TUint32 aCmId )
    {
    // Edit connection method  
    TInt ret( KDialogUserBack );
    
    CCmPluginBase* cm = iCmManagerImpl->GetConnectionMethodL( aCmId );
    CleanupStack::PushL( cm );
    
    cm->ReLoadL();
    if ( cm->GetBoolAttributeL( CMManager::ECmProtected ) ||
         cm->GetBoolAttributeL( CMManager::ECmDestination ) )
        {
        TCmCommonUi::ShowNoteL( R_CMMANAGERUI_INFO_PROTECTED_CANNOT_EDIT,
                                            TCmCommonUi::ECmErrorNote );
        }
   else if ( cm->GetBoolAttributeL( CMManager::ECmBearerHasUi ) )
        {
        // check if the cm is in use
        if ( cm->GetBoolAttributeL( CMManager::ECmConnected ) )
            {
            TCmCommonUi::ShowNoteL( R_QTN_SET_NOTE_AP_IN_USE_EDIT,
                                    TCmCommonUi::ECmErrorNote );
            }
        else
            {
            //Makes sure that the commsdat notifier is initialized.           
            cm->CmMgr().StartCommsDatNotifierL();
            ret = cm->RunSettingsL();
            }
        }
    CleanupStack::PopAndDestroy(cm);
    return ret;
    }

