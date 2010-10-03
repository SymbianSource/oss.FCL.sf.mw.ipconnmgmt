/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of base plugin.
*
*/


// INCLUDE FILES

#include "ConnUiUtilsNotif.h"

#include "SelectWLanDlgPlugin.h"
#include "OfflineWlanNoteNotif.h"
#include "OfflineWlanDisabledNoteNotif.h"
#include "EasyWepDlgNotif.h"
#include "EasyWpaDlgNotif.h"
#include "WLANNetworkUnavailableNoteNotif.h"
#include "wlanpowersavetestnotif.h"
#include "easywapidlgnotif.h"

// CONSTANTS

LOCAL_D const TInt KPluginGranularity = 4;

// FORWARD DECLARATIONS

LOCAL_C void CreateNotifiersL( 
                            CArrayPtrFlat<MEikSrvNotifierBase2>* aNotifiers );

// ---------------------------------------------------------
// NotifierArray()
// Lib main entry point
// ---------------------------------------------------------
//
EXPORT_C CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
    {
    CArrayPtrFlat<MEikSrvNotifierBase2>* array = NULL;
    TRAP_IGNORE( array = new (ELeave) 
                    CArrayPtrFlat<MEikSrvNotifierBase2>( KPluginGranularity ); )

    if ( array )
        {
        TRAPD( err, CreateNotifiersL( array ) );
        if( err )
            {
            TInt count = array->Count();
            while( count-- )
                {
                (*array)[count]->Release();
                }
            delete array;
            array = NULL;
            }
        }

    return( array );
    }

// ---------------------------------------------------------
// CreateNotifiersL()
// ---------------------------------------------------------
//
LOCAL_C void CreateNotifiersL( 
                              CArrayPtrFlat<MEikSrvNotifierBase2>* aNotifiers )
    {
    MEikSrvNotifierBase2 *serNotify;
    TBool resourceFileResponsible = ETrue;

    serNotify = CSelectWLanDlgPlugin::NewL( resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );
    
    resourceFileResponsible = EFalse; // Only the first has to load resources

    serNotify = COfflineWlanNoteNotif::NewL( resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );
    
    serNotify = COfflineWlanDisabledNoteNotif::NewL( resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );      
    
    serNotify = CEasyWepDlgNotif::NewL( resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );      

    serNotify = CEasyWpaDlgNotif::NewL( resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );      

    serNotify = CWLANNetworkUnavailableNoteNotif::NewL( 
                                                    resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );      
    
    serNotify = CWlanPowerSaveQueryNotif::NewL( resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );
    
    serNotify = CEasyWapiDlgNotif::NewL( resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );    

    }
    
// End of File
