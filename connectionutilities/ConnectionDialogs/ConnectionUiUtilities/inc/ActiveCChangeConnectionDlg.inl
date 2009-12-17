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
* Description: 
*      Inline method definitions of the class CActiveCChangeConnectionDlg.
*
*
*/



#ifndef __ACTIVECCHANGECONNECTIONDLG__INL
#define __ACTIVECCHANGECONNECTIONDLG__INL


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// void CActiveCChangeConnectionDlg::SetSelected
// ----------------------------------------------------------------------------
//
void CActiveCChangeConnectionDlg::SetSelected( TBool aSelected ) 
    { 
    iSelected = aSelected; 
    }


// ----------------------------------------------------------------------------
// void CActiveCChangeConnectionDlg::ActIAPs
// ----------------------------------------------------------------------------
//
CConnectionInfoArray* CActiveCChangeConnectionDlg::ActIAPs() 
    { 
    return iActIAPs;   
    }
        

#endif // __ACTIVECCHANGECONNECTIONDLG__INL

// End of File