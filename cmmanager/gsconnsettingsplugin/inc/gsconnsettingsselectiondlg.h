/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Header file for CGSConnSettingsSelectionDlg class
*
*/

#ifndef GSCONNSETTINGSSELECTIONDLG_H
#define GSCONNSETTINGSSELECTIONDLG_H

#include <aknradiobuttonsettingpage.h>

class CGSConnSettingsSelectionDlg : public CAknRadioButtonSettingPage
    {
    
public:
    
    /**
     * Symbian OS two-phased constructor
     * 
     * @param aResourceID               Text at top of setting pane
     * @param aCurrentSelectionIndex    Current slected item
     * @param aItemArray                Pointer to array of loaded resource texts for radio buttons
     * @return 
     * 
    */
    static CGSConnSettingsSelectionDlg* NewL(TInt aResourceID, 
            TInt& aCurrentSelectionIndex, 
            const MDesCArray* aItemArray );
    
    /**
     * C++ default constructor.
     *
     * @param aResourceID               Text at top of setting pane
     * @param aCurrentSelectionIndex    Current slected item
     * @param aItemArray                Pointer to array of loaded resource texts for radio buttons
    */
    CGSConnSettingsSelectionDlg( 
            TInt aResourceID, 
            TInt& aCurrentSelectionIndex, 
            const MDesCArray* aItemArray );
			
    /**
     * Destructor.
     */
    virtual ~CGSConnSettingsSelectionDlg();
    
private:

    //From CAknListBoxSettingPage
    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);
    
    //From CAknSettingPage
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType);

    };
#endif // GSCONNSETTINGSSELECTIONDLG_H
