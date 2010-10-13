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
* Description:  Implementation of CCmSettingsUiImpl.
*
*/

#include <cmmanager.rsg>

#include "cmsettingsuiimpl.h"
#include "cmmanagerimpl.h"
#include "destdlg.h"
#include <cmcommonconstants.h>
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
