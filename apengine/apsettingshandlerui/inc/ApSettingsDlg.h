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
* Description: 
*     Declares dialog for access point view/edit.
*
*/


#ifndef APSETTINGS_DLG_H
#define APSETTINGS_DLG_H



// INCLUDES
#include <coecntrl.h>
#include <coeccntx.h>

#include <eiklbo.h>
#include <eikapp.h>
#include <eikdoc.h>

#include <AknDialog.h>
#include <ApSettingsHandlerCommons.h>
#include <ActiveApDb.h>
#include <ApAccessPointItem.h>
#include <ApEngineConsts.h>


// CONSTANTS
const TInt KBearers     = 4;


// FORWARD DECLARATIONS
class CTextOverrides;
class CEikStatusPane;
class CAknTitlePane;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CApSettingsModel;
class CApSettingsHandlerImpl;
class CFeatureManagerWrapper;


// CLASS DECLARATION
/**
* CApSettingsDlg dialog class
*
*/

const TInt KEndOfArray = -1;

const TInt KCsdAnalogue[]       = { KSpeedAutobaud, KSpeed9600, KSpeed14400, 
                                    KEndOfArray };

const TInt KCsdAnalogueWcdma[]  = { KSpeedAutobaud, KSpeed9600, KSpeed14400,
                                    KSpeed28800, KEndOfArray };

const TInt KCsdIsdn110[]        = { KSpeed9600, KSpeed14400, KEndOfArray };

const TInt KCsdIsdn110Wcdma[]   = { KSpeed9600, KSpeed14400, KSpeed28800, 
                                    KSpeed38400, KEndOfArray };

const TInt KCsdIsdn120[]        = { KSpeed9600, KSpeed14400, KEndOfArray };

const TInt KCsdIsdn120Wcdma[]   = { KSpeed9600, KSpeed14400, KSpeed28800, 
                                    KSpeed56000, KEndOfArray };

const TInt KHcsdAnalogue[]      = { KSpeedAutobaud, KSpeed9600, KSpeed14400,
                                    KSpeed19200, KSpeed28800, KEndOfArray };

const TInt KHcsdIsdn110[]       = { KSpeed9600, KSpeed14400, KSpeed19200, 
                                    KSpeed28800, KSpeed38400, KEndOfArray };

const TInt KHcsdIsdn120[]       = { KSpeed9600, KSpeed14400, KSpeed19200,
                                    KSpeed28800, KSpeed43200, KEndOfArray };

//Resources
const TInt KResCsdAnalogue[]        = { R_APUI_VIEW_SPEED_AUTODETECT,
                                        R_APUI_VIEW_SPEED_9600, 
                                        R_APUI_VIEW_SPEED_14400, KEndOfArray };

const TInt KResCsdAnalogueWcdma[]   = { R_APUI_VIEW_SPEED_AUTODETECT, 
                                        R_APUI_VIEW_SPEED_9600, 
                                        R_APUI_VIEW_SPEED_14400,
                                        R_APUI_VIEW_SPEED_28800, KEndOfArray };

const TInt KResCsdIsdn110[]         = { R_APUI_VIEW_SPEED_9600,
                                        R_APUI_VIEW_SPEED_14400, KEndOfArray };

const TInt KResCsdIsdn110Wcdma[]    = { R_APUI_VIEW_SPEED_9600,
                                        R_APUI_VIEW_SPEED_14400,
                                        R_APUI_VIEW_SPEED_28800,
                                        R_APUI_VIEW_SPEED_38400, KEndOfArray };

const TInt KResCsdIsdn120[]         = { R_APUI_VIEW_SPEED_9600, 
                                        R_APUI_VIEW_SPEED_14400, KEndOfArray };

const TInt KResCsdIsdn120Wcdma[]    = { R_APUI_VIEW_SPEED_9600, 
                                        R_APUI_VIEW_SPEED_14400,
                                        R_APUI_VIEW_SPEED_28800, 
                                        R_APUI_VIEW_SPEED_56000, KEndOfArray };

const TInt KResHcsdAnalogue[]       = { R_APUI_VIEW_SPEED_AUTODETECT, 
                                        R_APUI_VIEW_SPEED_9600, 
                                        R_APUI_VIEW_SPEED_14400,
                                        R_APUI_VIEW_SPEED_19200,
                                        R_APUI_VIEW_SPEED_28800, KEndOfArray };

const TInt KResHcsdIsdn110[]        = { R_APUI_VIEW_SPEED_9600, 
                                        R_APUI_VIEW_SPEED_14400, 
                                        R_APUI_VIEW_SPEED_19200, 
                                        R_APUI_VIEW_SPEED_28800, 
                                        R_APUI_VIEW_SPEED_38400, KEndOfArray };

const TInt KResHcsdIsdn120[]        = { R_APUI_VIEW_SPEED_9600, 
                                        R_APUI_VIEW_SPEED_14400, 
                                        R_APUI_VIEW_SPEED_19200,
                                        R_APUI_VIEW_SPEED_28800,
                                        R_APUI_VIEW_SPEED_43200, KEndOfArray };

NONSHARABLE_CLASS( CApSettingsDlg ) : public CAknDialog, 
                                      public MEikListBoxObserver, 
                                      public MActiveApDbObserver
    {

    public: // Constructors and destructor

        enum TSaveAction
            {
            EApCanSave,
            EApMustDelete,
            EApShallGoBack,
            EApNoAction
            };

        /**
        * Create and launch dialog.
        * @param aModel     Model to be used for handling access point data. 
        *                   Not owned. Must remain intact while the dialog
        *                   exists.
        * @param aHandler   The data handling object
        * @param aUid       The Wap Access Point ID of the access point 
        *                   to view/edit.
        * @param aItem      An optional CApAccessPointItem to edit
        * @return           The code returned by the dialogs' ExecuteLD
        */
        TInt ConstructAndRunLD( CApSettingsModel& aModel,
                                CApSettingsHandlerImpl& aHandler,
                                TUint32& aUid,
                                CApAccessPointItem* aItem = NULL );



        /**
        * Two-phase construction.
        * @param aIspFilter         The Isp filter to be used
        * @param aBearerFilter      The bearer filter to be used
        * @param aSortType          The sorting to be used
        * @param aEventStore        A reference to hold the events happened
        *                           while running this setting dialog.        
        * @param aNeedIpv6Support Specifies whether the caller would like to 
        *   have IPv6 support or not. If it is ETrue, in case of IPv6 support
        *   is available as a feature, it will be supported. If IPv6 feature 
        *   is not supported by the phone, it simply defaults to the normal 
        *   IPv4 version.
        *   If it is False, it uses the default IPv4 version independently 
        *   from IPv6 feature beeing available or not.
        */
        static CApSettingsDlg* NewL( TInt aIspFilter,
                                     TInt aBearerFilter,
                                     TInt aSortType,
                                     TUint32& aEventStore,
                                     TInt aNeedIpv6Support = EFalse
                                     );


        /**
        * Destructor.
        */
        ~CApSettingsDlg();


        // From CCoeControl
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType ) ;

    protected:
        /**
        * Constructor.
        * @param aIspFilter         The Isp filter to be used
        * @param aBearerFilter      The bearer filter to be used
        * @param aSortType          The sorting to be used
        * @param aEventStore        A reference to hold the events happened
        *                           while running this setting dialog.        
        * @param aNeedIpv6Support Specifies whether the caller would like to 
        *   have IPv6 support or not. If it is ETrue, in case of IPv6 support
        *   is available as a feature, it will be supported. If IPv6 feature 
        *   is not supported by the phone, it simply defaults to the normal 
        *   IPv4 version.
        *   If it is False, it uses the default IPv4 version independently 
        *   from IPv6 feature beeing available or not.
        */
        CApSettingsDlg(  TInt aIspFilter,
                         TInt aBearerFilter,
                         TInt aSortType,
                         TUint32& aEventStore,
                         TBool aNeedIpv6Support
                         );



    public: // New functions

        /**
        * Initialises texts.
        */
        void InitTextsL();


        /**
        * Handles listbox data change
        * @param aReRead Boolean indicating whether the data must be re-read
        * from the database or only 'value' or 'structural' changes
        */
        void HandleListboxDataChangeL( TBool aReRead );


        /**
        * Sets the text overrides
        * @param aOverrides The text overrides object to be used
        */
        void SetTextOverrides( CTextOverrides* aOverrides );


        /**
        * Handles database events, implementation of the
        * MActiveApDbObserver protocol
        * @param anEvent The event code
        */
        virtual void HandleApDbEventL( TEvent anEvent );

        // From CCoeControl
        virtual void GetHelpContext(TCoeHelpContext& aContext) const;

    private:

        // From CEikDialog
        virtual void PreLayoutDynInitL();

        // From CAknDialog
        TBool OkToExitL( TInt aButtonId );

        // From MEikCommandObserver
        virtual void ProcessCommandL( TInt aCommandId ) ;

        // From MEikMenuObserver
        virtual void DynInitMenuPaneL( TInt aResourceId, 
                                       CEikMenuPane* aMenuPane ) ;

        // From MEikListBoxObserver
        void HandleListBoxEventL( CEikListBox* aListBox, 
                                  TListBoxEvent aEventType );

    protected:  // New functions

        /**
        * Fills up the listbox with data
        * @param aItemArray     The itemarray to fill up
        * @param arr            A pointer to a TApMember array to be used as 
        *                       list elements
        * @param aLength        The number of elements in the above array
        * @param aRes           A pointer to an array of resource ID's to use 
        *                       for the elements of the TApMember array
        * @param aCompulsory    A pointer to an array of TInts indicating that
        *                       the elements of the TApMember array are 
        *                       compulsory or not
        */
        void    FillListWithDataL
                    (
                    CDesCArrayFlat& aItemArray,
                    const TApMember& arr,
                    TInt aLength,
                    const TInt* aRes,
                    const TInt* aCompulsory
                    );



        /**
        * Adds one textual listbox item for the given member
        * @param aItemArray     The itemarray to add the item        
        * @param aMember        A TApMember value specifying which member of 
        *                       the access point had to be added to the list
        * @param aRes           Resource ID for the 'title text' for this 
        *                       member
        * @param aCompulsory    a TInt indicating whether this member is 
        *                       compulsory or not
        */
        void    AddTextualListBoxItemL
                        (
                        CDesCArrayFlat& aItemArray,
                        TApMember aMember,
                        TInt aRes,
                        TInt aCompulsory
                        );


        /**
        * Adds one 'on/off' listbox item for the given member
        * @param aItemArray     The itemarray to add the item        
        * @param aMember        A TApMember value specifying which member of
        *                       the access point had to be added to the list
        * @param aRes           Resource ID for the 'title text' for this 
        *                       member
        * @param aCompulsory    TInt showing if the member is compulsory or not
        */
        void    AddOnOffListBoxItemL
                        (
                        CDesCArrayFlat& aItemArray,
                        TApMember aMember, TInt aRes,
                        TInt aCompulsory
                        );


        /**
        * Updates one 'on/off' listbox item for the given member
        * @param aMember A TApMember value specifying which member of the
        * access point had to be added to the list
        * @param aRes Resource ID for the 'title text' for this member
        * @param aPos The current position of the item in the list
        * @param aCompulsory a TInt indicating whether
        * this member is compulsory or not
        */
        void    UpdateOnOffListBoxItemL( TApMember aMember, TInt aRes,
                                         TInt aPos, TInt aCompulsory );


        /**
        * Updates one 'textual' listbox item for the given member
        * @param aMember A TApMember value specifying which member of the
        * access point had to be added to the list
        * @param aRes Resource ID for the 'title text' for this member
        * @param aPos The current position of the item in the list
        * @param aCompulsory a TInt indicating whether
        * this member is compulsory or not
        */
        void    UpdateTextualListBoxItemL( TApMember aMember, TInt aRes,
                                        TInt aPos, TInt aCompulsory );


        /**
        * Creates one 'textual' listbox item for the given member
        * @param aMember A TApMember value specifying which member of the
        * access point had to be added to the list
        * @param aRes Resource ID for the 'title text' for this member
        * @param aCompulsory a TInt indicating whether
        * this member is compulsory or not
        * @return A HBufC* pointing to the created listbox item text.
        */
        HBufC*  CreateTextualListBoxItemL( TApMember aMember, TInt aRes,
                                        TInt aCompulsory );


        /**
        * Creates one 'on/off' listbox item for the given member
        * @param aMember A TApMember value specifying which member of the
        * access point had to be added to the list
        * @param aRes Resource ID for the 'title text' for this member
        * @param aCompulsory a TInt indicating whether
        * this member is compulsory or not
        * @return A HBufC* pointing to the created listbox item text.
        */
        HBufC*  CreateOnOffListBoxItemL( TApMember aMember, TInt aRes,
                                        TInt aCompulsory );


        /**
        * Changes one setting. The setting, which is
        * highlighted as current in the listbox is changed.
        * @param aQuick a TBool indicating whether a 'quick' change is needed
        * or not. Quick changes are not bring up any setting dialog but simply
        * change the current value to the next available one.
        * This can be used only for 'on/off' type settings
        */
        void    ChangeSettingsL( TBool aQuick );


        /**
        * Sets the internal bearer type, list is re-arranged according to the
        * newly set bearer type
        * @param aBearer The new bearertype
        */
        void    SetBearerTypeL( TApBearerType aBearer );


        /**
        * Inverts one boolean member of the access point
        * @param aData the member to invert.
        * if the member does not exists,
        * or not a boolean memeber, code will _ASSERT_DEBUG
        */
        void    InvertSettingsL( TApMember aData );


        /**
        * Shows a popup setting page (radio buttons) for the given member
        * @param aDataMember The access point member which need to be changed
        * @return A boolean indicating whether the current setting
        * had been changed or not.
        */
        TBool   ShowPopupSettingPageL( TApMember aDataMember );


        /**
        * Shows a popup text setting page for the given member
        * @param aDataMember The access point member which need to be changed
        * @return A boolean indicating whether the current setting
        * had been changed or not.
        */
        TBool   ShowPopupTextSettingPageL( TApMember aData );


        /**
        * Gets the text for the passed (text-type) member.
        * @param aDataMember The access point member for which to get the text
        * @return a HBufC* to the newly allocated text.
        * Text is placed onto the CleanupStack. Ownership passed to caller.
        */
        HBufC*  GetTextLC( TApMember aData );


        /**
        * Gets the resource ID for the passed (text-type) member
        * along with the setting page flags
        * @param aDataMember    The access point member for which to get 
        *                       the resource ID
        * @param aTextSettingPageFlags A reference to hold the setting 
        *                       page flags for the given member
        */
        void    GetTextResId( TApMember aData, TInt& apage,
                              TInt& aTextSettingPageFlags );


        /**
        * Gets the resource ID for the passed (NOT text-type) member
        * @param aDataMember    The access point member for which to get 
        *                       the resource ID
        * @param aattr          A reference to hold the resource ID
        *                       for the given member
        */
        void    GetResId( TApMember aData, TInt& aattr );


        /**
        * Fills up a pop-up radio button setting page with the currently
        * valid and available choices for the given member.
        * @param aData The member whose chances are needed
        * @param aCurrvalue The current value of the setting
        * @return An array of choices for the given member,
        * pushed to the CleanupStack.
        */
        CDesCArrayFlat* FillPopupSettingPageLC( TApMember aData,
                                                TInt& aCurrvalue );


        /**
        * Updates the given member's data with the new
        * setting from the setting page.
        * @param aData the member to update
        * @param aCurrvalue The new value
        */
        void    UpdateFromPopupSettingPageL( TApMember aData, 
                                             TInt aCurrvalue );


        /**
        * Returns whether the access point in question can be saved or not.
        * If a compulsory item is missing, user is prompted whether to delete
        * it or to go back for re-editing.
        * @param aDataNeeded If the AP can not be saved because a compulsory
        * item is missing, this holds the TApMember which is not filled.
        * It lists always the first item causing the 'failure'.
        * @return a TSaveAction indicating whether it can be saved,
        * must re-edit or must be deleted.
        */
        TSaveAction CanSaveL( TApMember& aDataNeeded );


        /**
        * Handles IP address editing of the given field
        * Brings up the IP address editor page, fills up with the current
        * value, handles editing and after editing, it stores the new value
        * in the AP item.
        * @param aData The (IP-address) member which needs to be changed
        */
        void    ShowPopupIpAddrSettingPageL( TApMember aData );


        /**
        * Handles phone number editing of the given field
        * Brings up the phone number editor page, fills up with the current
        * value, handles editing and after editing, it stores the new value
        * in the AP item.
        * In case the entered phone number is invalid, an 'invalid number'
        * note is shown and original value is kept.
        * @param aData The (phone-number) member which needs to be changed
        */
        void    ShowPopupPhoneNumSettingPageL( TApMember aData );


        /**
        * Handles password setting
        * Brings up the password editor page, handles editing
        * and after editing, it stores the new value in the AP item.
        * @param a TApMember specifying the member
        */
        void    ShowPopupPasswordPageL( TApMember aData );


        /*
        * Reads up a Boolean type data, ensures that it is in the
        * required range (0 or 1)
        * @param aData The member to get
        * @param aInvert Whether the logic is inverted or not
        * @param aCurrvalue The current value
        */
        void    GetBoolDataValue( TApMember aData, TBool aInvert,
                                  TInt& aCurrvalue );
    private:

        /**
        * Does the bearer change according to
        * the current value of the selection
        * @param a TInt specifying the current value
        */
        void    DoBearerChangeL( TInt aCurrvalue );


        /**
        * Handles bearer change.
        * Sets AP item bearer type, re-reads AP data, re-arranges listbox
        * according to the new bearer type
        * @param aBearer A TApBearerType specifying the current bearer.
        */
        void    HandleBearerChangeL( TApBearerType aBearer );


        /*
        * This part holds functions available only in the case of 
        * 'multiple bearer support', that means that 
        * as long as the user does not saves the AP, multiple instances of
        * the same AP are kept with different bearers...
        */

        /**
        * Returns the array index for a bearer type
        * @param aBearer    The bearer type to get the index of
        * @return The index
        */
        TInt32  BearerToArrayIndexL( TApBearerType aBearer );

        
        /**
        * Returns the bearer type for an index
        * @param aIndex The index to get the bearer type for
        * @return The bearer type
        */
        TApBearerType ArrayIndexToBearer( TInt32 aIndex );


        /**
        * Fills the passed array with the possible bearer speed values.
        * Array MUST be empty to get correct result as speed values are
        * simply APPENDED to the end of the list.
        * Adds only valid and available values according to the current
        * bearer type and call type.
        * @param aItems The array to hold the bearer speed values
        * @param aCurrvalue The index of the current speed value
        */
        void    FillUpBearerSpeedValuesL( CDesCArrayFlat& aItems,
                                          TInt& aCurrvalue );


        /**
        * Tries to save current access point data.
        * Checks if the data can be saved: each compulsory fields are filled,
        * Name is valid and does not exist already.
        * If data can not be saved, pops up query about delete? Yes/No
        * If user choses delete, AP will be deleted.
        * If user chooses NO, the 'faulty' item will be highlighted in the
        * list and it will return to the setting list, giving a chance to edit
        * the faulty/missing data.
        * @return Whether the 'Save' procedure had been successfull or not.
        * It also means that if the user had choosen 'Delete', after deleting
        * the AP it will return 'ETrue' indicating successfull operation.
        */
        TBool   SaveDataL( );


        /**
        * Handles list structure changes.
        * Refreshes the list of members needed to represent the current AP and
        * the current settings.
        */
        void    HandleListStructureChangeL();


        /**
        * This function 'limits' the connection speed after a bearertype or
        * calltype change so the current bearer speed will be always valid.
        */
        void    LimitSpeed();


        /**
        * Selects one item of the listbox.
        * @param aItemIndex The index of the item to be select.
        */
        void    SelectItem( TInt aItemIndex );



        /*
        * Converts EParagraphDelimiter-s to /n-s
        * Returned text is owned by the caller!
        * @param aInText The original text where replacement is needed
        * @return A new HBufC (pushed to the CleanupStack) with
        * the modified text
        */
        HBufC* ConvertDelimToPerNLC( const TDesC& aInText );


        /*
        * Converts /n-s to EParagraphDelimiter-s
        * Returned text is owned by the caller!
        * @param aInText The original text where replacement is needed
        * @return A new HBufC (pushed to the CleanupStack) with
        * the modified text
        */
        HBufC* ConvertPerNToDelimLC( const TDesC& aInText );


        /**
        * Changes one setting. The setting, which is
        * highlighted as current in the listbox is changed.
        * @param aQuick a TBool indicating whether a 'quick' change is needed
        * or not. Quick changes are not bring up any setting dialog but simply
        * change the current value to the next available one.
        * This can be used only for 'on/off' type settings
        */
        void DoChangeSettingsL( TBool aQuick );


        /**
        * Converts the selected speed index to the current speed value
        * based on the bearer type (CSD/HSCSD) and whether it is analogue,
        * ISDN v110 or ISDN v120
        * @param aCurrvalue TInt The current selection index
        * @return The TApCallSpeed value
        */
        TApCallSpeed GetBearerSpeedForSelection( TInt aCurrvalue );


        /**
        * Calculates the index for the current speed value
        * based on the bearer type (CSD/HSCSD) and whether it is analogue,
        * ISDN v110 or ISDN v120
        * @return TInt the index of the current speed in the list
        */
        TInt GetCurrIndexFromBearerSpeed( );


        /**
        * Converts whitespace characters to spaces
        * Returned text is owned by the caller!
        * @param aInText The original text where replacement is needed
        * @return A new HBufC (pushed to the CleanupStack) with
        * the modified text
        */
        HBufC* ReplaceWhiteSpaceCharactersLC(  const TDesC& aInText );


        void ChangeNetworkGroupL();


        /**
        * Gets the shareddata value whether HSCSD is enabled or not
        * @return A TInt stating if it is enabled (1) or not (0)
        */
        TInt IsHSCSDEnabledL();


        /**
        * Gets the CSD settings array according to the passed variant
        * It sets member pointers to Field, Title & Compulsory values
        * @param variant    Variant
        */
        void GetCSDSettingArrays( TInt variant );


        /**
        * Gets the GPRS settings array according to the passed variant
        * It sets member pointers to Field, Title & Compulsory values
        * @param aVariant    Variant
        */
        void GetGPRSSettingArrays( TInt aVariant );

        
        /**
        * Gets the WLAN settings array according to the passed variant
        * It sets member pointers to Field, Title & Compulsory values
        * @param variant    Variant
        */
        void GetWLANSettingArraysL( TInt variant );


        /**
        * Asks a query if wcdma speed had been selected.
        * only checks if a speed-related member had been changed.
        * @param aMember    The member which had been changed
        * @param aCurrValue The current value of the setting
        * @return   Whether the query had been accepted or not. 
        *           Default return value is ETrue
        */
        TBool AskQueryIfWcdmaL( TApMember aMember, TInt aCurrvalue  );


        /**
        * Displays the domain name servers data query 
        * @param AIp6 Determines whether ip4 or ip6 query is to be launched.
        * Internally takes care about whether IP6 is supported or not.
        * If IP4 is selected, it uses IP address editor.
        * If IP6 is selected, it uses standard text editor.
        * @return The result of the dialog's ExecuteLD
        */
        TInt GetNameServersL( TBool aIp6 );


        /**
        * Adds one 'Menu' listbox item (which will open another setting list)
        * for the given member.
        * @param aMember A TApMember value specifying which member of the
        * access point had to be added to the list
        * @param aRes Resource ID for the 'title text' for this member
        * @param aCompulsory a TInt indicating whether
        * this member is compulsory or not
        */
        void AddMenuListBoxItemL( 
                                CDesCArrayFlat& aItemArray,
                                TApMember aMember,
                                TInt aRes,
                                TInt aCompulsory
                                );


        /**
        * Returns the current domain name server type
        * It can be 3: Dynamic, Well-known, User defined
        * Return values are 0, 1, 2, respectively
        */
        TInt GetDomainNameServersTypeL();


        /**
        * Pops up an IPv4 IP address multiline query
        * IP address is entered into IPv4 address editors
        * @return Returns the returned value of the multiline query
        */
        TInt GetIpv4NameServerL();


        /**
        * Pops up an IPv6 IP address multiline query
        * IP address is entered into TEXT editors
        * @return Returns the returned value of the multiline query
        */
        TInt GetIpv6NameServerL();


        /**
        * Returns the current domain name server type for IPv6 type
        * It can be 3: Dynamic, Well-known, User defined
        * Return values are 0, 1, 2, respectively
        */
        TInt GetIPv6DNSTypeL();


        /**
        * Returns the current domain name server type for IPv4 type
        * It can be 3: Dynamic, Well-known, User defined
        * Return values are 0, 1, 2, respectively
        */
        TInt GetIPv4DNSTypeL();


        /**
        * Strips non-number chars from the passed text
        * @return a HBufC8 holding the new text
        */
        HBufC* StripNonNumberLC( const TDesC16& aInText );


        /**
        * Returns the index for the given bearertype in the list of 
        * currently available bearer types.
        * Leaves with KErrNotSupported if the passed bearer type currently 
        * is not supported
        */
        TInt BearerType2BearerItemPosL( TApBearerType aBearer );


        /**
        * Returns the bearer type for the given position in the list of 
        * currently available bearer types.
        * @param The position in the list
        * @return The bearer type of the item bbeing in aPos position
        */
        TApBearerType BearerItemPos2BearerTypeL( TInt aPos );


        /**
        * Changes WLAN network name.
        */
        void ChangeWlanNetworkNameL();


        /**
        * Changes WLAN security settings.
        * @return TInt giving exit or shutdown required if any, otherwise, 0
        */
        TInt ChangeWlanSecuritySettingsL();


        /**
        * Changes WEP security settings.
        * Leaves on errors.
        * @return TInt giving exit or shutdown required if any, otherwise, 0
        */
        TInt ChangeWepSecuritySettingsL();


        /**
        * Changes WPA security settings.
        * Leaves on errors.
        * @return TInt giving exit or shutdown required if any, otherwise, 0
        */
        TInt ChangeWpaSecuritySettingsL();


        /**
        * Changes 802.1x security settings.
        * Leaves on errors.
        * @return TInt giving exit or shutdown required if any, otherwise, 0
        */
        TInt Change8021xSecuritySettingsL();


        /**
        * Returns whether WLAN is supported or not.
        * @return a boolean indicating whether WLAN is supported or not.
        */
        TBool IsWlanSupported();


        /**
        * Get the Wlan network name with a popup query
        * @return A boolean indicating whether the current setting
        * had been changed or not.
        */
        TBool QueryWlanNetworkNameL();


        /**
        * @return Whether the EApIspIPNetMask and EApIspIPGateway fields 
        * are visible in the settings or not.
        */
        TBool IsNetMaskAndGatewayVisibleL();

        
        /**
        * Returns the ad-hoc channel number asked from the user
        */
        TUint32 GetAdHocChannelL();
        
        
        /**
        * Returns whether the ad-hoc channel ID is visible or not
        */
        TBool IsAdhocChannelVisible();
        
        
        /**
        * Returns whether the security settings are filled correctly in case
        * of a WLAN access point or not.
        */
        TBool HasSecuritySettingsFilledL();
        
        
        /**
        * Limits the security mode when the network mode has changed.
        * In case of infrastructure, it should not be called
        * In case of Ad-hoc, it should be called and it will limit the
        * current security mode into the curren range, which is EOpen and EWep
        *
        */
        void LimitSecMode();
        
        
        /**
        * Returns whether the access point in question can be saved or not.
        * If a compulsory item is missing, user is prompted whether to quit 
        * without saving it or to go back for re-editing.
        * @param aDataNeeded If the AP can not be saved because a compulsory
        * item is missing, this holds the TApMember which is not filled.
        * It lists always the first item causing the 'failure'.
        * @return a TSaveAction indicating whether it can be saved,
        * must re-edit or must be quited(deleted).
        */
        TSaveAction CanSaveWlanCompulsoriesL( TApMember& aDataNeeded );
        
        /**
        * Restores Ipv4 settings to iApItem from iBackupApItem.
        */
        void RestoreIpv4SettingsL();
        
protected:  // Functions from base classes



    private: //data
        TUint32*                        iUid;

        TUint32                         iLevel;         // current level
        // when leaving level 0, the two indexes are stored: top & current
        // to enable getting back to the same position & state
        TInt                            iOldIndex;    // current index
        TInt                            iOldTopIndex;   // top index

        TInt                            iAdvIndex;

        TApBearerType                   iBearerType;
        TInt                            iIspFilter;
        TInt                            iBearerFilter;
        TInt                            iSortType;

        CEikStatusPane*                 iStatusPane;    // NOT owned
        CAknTitlePane*                  iTitlePane;     // NOT owned
        CAknNavigationControlContainer* iNaviPane;      // NOT owned
        CAknNavigationDecorator*        iNaviDecorator; // owned

        HBufC*                          iOldTitleText;

        CApSettingsModel*               iDataModel;     // NOT owned

        CAknSettingStyleListBox*        iList;      // owned through resources,
                                                    // destroyed automatically 
                                                    // by the dialog

        CApAccessPointItem*             iApItem;        // owned
        CApAccessPointItem*             iOldApItem;     // owned
        TUint32*                        iEventStore;

        CArrayPtrFlat<CApAccessPointItem>*  iBearerItemArray; // owned

        TApMember*                      iField;
        TApMember*                      iFieldAdvanced;
        TApMember*                      iFieldL2;
        TInt                            iFieldCount;
        TInt                            iAdvancedFieldCount;
        TInt                            iFieldCountL2;
        TInt*                           iTitles;
        TInt*                           iTitlesAdvanced;
        TInt*                           iTitlesL2;
        TInt*                           iCompulsory;
        TInt*                           iCompulsoryAdvanced;
        TInt*                           iCompulsoryL2;
        TApUiExitReason                 iExitReason;

        CDesCArrayFlat*                 iItemArray; // not owned

        CTextOverrides*                 iTextOverrides;// does not owns it, 
                                                       // just passed for usage
        CApSettingsHandlerImpl*         iHandler;

        TInt                            iReqIpvType;    // Default is IPv4
        TBool                           iL2Ipv4;        // Default is NO 
        TInt                            iVariant;       /// Local variant value
       
        /**
        * CFeatureManagerWrapper wraps FeatureManager to eliminate maintenance 
        * of existance of FeatureManager.
        * Owned.
        */
        CFeatureManagerWrapper*         iFeatureManagerWrapper; 


#ifdef __TEST_OOMDEBUG
        TBool                           iMemTestOn;
#endif // __TEST_OOMDEBUG
        TInt                            iSecSettingsExitReason;
        TApBearerType                   iOriginalBearerType;
        
        CApAccessPointItem*             iBackupApItem;     // owned
        TBool							iHelpSupported;
    };

#endif

// End of File
