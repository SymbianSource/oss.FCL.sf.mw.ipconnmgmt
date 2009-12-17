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
* Description:  Declaration of class CAccessPointTextSettingPage.
*
*/


#ifndef ACCESSPOINT_TEXT_SETTING_PAGE_H
#define ACCESSPOINT_TEXT_SETTING_PAGE_H

// INCLUDE FILES
#include <akntextsettingpage.h>


// FORWARD DECLARATION
class CApSelectorListboxModel;


// CLASS DECLARATION

/**
* Model for the access point listbox. It is an array of CApSelectorItem
* pointers; items are owned.
*/
NONSHARABLE_CLASS( CAccessPointTextSettingPage ) : public CAknTextSettingPage
    {
public:
    /**
    * Simple constructor depending only on a single resource Id. 
    * Editor resource is given via the link in the setting page resource.
    *
    * @param aResourceID Setting Page to use (if present)
    * @param aText Reference to text for editing
    * @param aTextSettingPageFlags option flags for miscellaneous things
    * @param aIsLatin Giving whether the editor should be Latin only
    */
    CAccessPointTextSettingPage( TInt aResourceID, TDes& aText,
                                 TInt aTextSettingPageFlags = 0,
                                 TBool aIsLatin = EFalse );
    /**
    * Constructor that allows separate setting page and editor resources
    *
    * This constructor allows the use of setting page using only the editor 
    * resource. Other combinations are also possible
    *
    * In all cases the number (if supplied i.e. <> 0 ) is used.
    *
    *   Editor    Setting Page 
    *  Resource    Resource
    *   present     present     Both are used (but text & number overridden)
    *    = 0        present     Editor resource is used via SP resource 
    *                           (Effectively like the other constructor)
    *   present      = 0        Default Avkon SP resource if used 
    *                           + this editor resource
    *    = 0         = 0        uses default resource for both SP and editor.
    *                           This is OK if:
    *   i) control type is present,
    *   ii) a default resource exists 
    *       ( OK for text, integer, date, time, duration )
    *
    * Note: THe first argument is a TDesC* (rather than TDesC&) because 
    * the other constructor cannot initialize such a member without allocation
    * or having an internal dummy buffer.
    * Note that this buffer must be owned by the client until 
    * ExecuteLD has been called.
    *
    * Rules for text and numbers: The rules are the same for both:  (non-zero
    * length) text or number other than EAknSettingPageNoOrdinalDisplayed if 
    * given in this constructor will not override resource (unless that is 
    * zero length or EAknSettingPageNoOrdinalDisplayed). Note, however, 
    * that text or number given via the specific API for setting them, 
    * WILL override resource.
    * It is assumed that number from resource is very rare. Special text 
    * is somewhat more likely.
    *
    * @param aSettingTitleText      Text at top of setting pane; 
    *                               OWNED EXTERNALLY
    * @param aSettingNumber         Number at top left (if present)
    * @param aControlType           Determines the type constructed 
    *                               and how its resource is read
    * @param aEditorResourceId      Editor resource to use in the setting page
    *                               (if present)
    * @param aSettingPageResourceId Setting Page to use (if present)
    * @param aText                  Reference to text for editing
    * @param aTextSettingPageFlags  option flags for miscellaneous things
    * @param aIsLatin               Gives whether the editor should accept 
    *                               only Latin input 
    */
    CAccessPointTextSettingPage( const TDesC* aSettingTitleText,
                                 TInt aSettingNumber,
                                 TInt aControlType,
                                 TInt aEditorResourceId,
                                 TInt aSettingPageResourceId,
                                 TDes& aText,
                                 TInt aTextSettingPageFlags = 0,
                                 TBool aIsLatin = EFalse );



    /**
    * 2nd stage construction for this specific type.  Call BaseConstructL in
    * CAknSettingPage, where the object is constructed from resource.
    *
    */
    virtual void ConstructL();

    TBool iIsLatin;
    };


#endif

// End of file
