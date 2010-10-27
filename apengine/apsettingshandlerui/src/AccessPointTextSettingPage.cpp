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
* Description:  Implementation of CAccessPointTextSettingPage.
*
*/


// INCLUDE FILES


#include "AccessPointTextSettingPage.h"
#include <akntextsettingpage.h>

#include <AknInputLanguageInfo.h>  // for AVKON ELAF
#include <featmgr.h>
#include "ApSettingsHandlerLogger.h"

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CAccessPointTextSettingPage::CAccessPointTextSettingPage
// ---------------------------------------------------------
//
CAccessPointTextSettingPage::CAccessPointTextSettingPage
                                        ( TInt aResourceID,
                                          TDes& aText,
                                          TInt aTextSettingPageFlags,
                                          TBool aIsLatin )
    :CAknTextSettingPage( aResourceID, aText, aTextSettingPageFlags )
    {
    }




// ---------------------------------------------------------
// CAccessPointTextSettingPage::CAccessPointTextSettingPage
// ---------------------------------------------------------
//
CAccessPointTextSettingPage::CAccessPointTextSettingPage
                                    ( const TDesC* aSettingTitleText,
                                      TInt aSettingNumber,
                                      TInt aControlType,
                                      TInt aEditorResourceId,
                                      TInt aSettingPageResourceId,
                                      TDes& aText,
                                      TInt aTextSettingPageFlags,
                                      TBool aIsLatin )
    :CAknTextSettingPage( aSettingTitleText, aSettingNumber, aControlType,
    aEditorResourceId, aSettingPageResourceId, aText, aTextSettingPageFlags )
    {
    }

// ---------------------------------------------------------
// CAccessPointTextSettingPage::ConstructL
// ---------------------------------------------------------
//
void CAccessPointTextSettingPage::ConstructL()
    {
    }

// End of File
