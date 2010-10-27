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
*     Declares the main handler, CApSettingsHandler and public API for the 
*     Access Point settings.
*
*/


#ifndef APSETTINGS_UI_H
#define APSETTINGS_UI_H

// INCLUDES
#include <ApSettingsHandlerCommons.h>
#include <ApEngineConsts.h>
#include <ApSelect.h>

// CONSTANTS


// FORWARD DECLARATIONS
class CActiveApDb;
class CTextOverrides;

class CApSettingsHandlerImpl;

// CLASS DECLARATION
/**
* CApSettingsUi class: It is the main handler for the Access Point 
* settings. It contains the only public API for other (sub)systems.
*
*/
class CApSettingsUi : public CBase
    {

    public: // Constructors and destructor



        /**
        * Two-phased constructor. Leaves on failure.
        * This function shall be used to create the access point 
        * settings/selection handling UI object.It is customizable through the
        * parameters. This object is a small layer above the UI part and
        * it provides a standard interface to select, edit, create access 
        * points. It has two main parts, Select and Edit. The first parameter, 
        * aStartWithSelection gives whether we would like to select or edit.
        * In case of selection, there is the possibility to use this module in
        * many applications. For this, two layouts are implemented, one is 
        * list pane used by General Settings which provides the list in a 
        * list-pane. The other layout is for the other applications requiring 
        * access point selection, a popup-list style as the decision was that 
        * General Settings will use list-pane and all other apps. popup-list 
        * style. It is the caller who selects the style so if spec. says 
        * otherwise, this module still can be used.
        * It is also possible to start the UI without selection, so if any 
        * module has a 'linked/default/etc.' access point and stored it's UID,
        * the module can simply call the editor part of the UI without having
        * to select the AP once more.
        * In case of editing only, (aStartWithSelection is EFalse), all other 
        * construction parameters are ignored and after construction caller 
        * can simply call DoRunSettingsL(..).
        * In case of selection, a lot of other parameters can be specified to
        * fulfil all requirements. There are three types of Options menu 
        * available: EApSettingsSelMenuNormal, EApSettingsSelMenuSelectOnly,
        * EApSettingsSelMenuSelectNormal.
        * <br>
        *-EApSettingsSelMenuSelectOnly: Only Select is visible, no way to edit,
        *   delete, create, etc.
        * <br>
        * -EApSettingsSelMenuNormal:Options menu specified in General Settings,
        *   contains Edit, Delete, Create new, etc. menu items.
        * <br>
        * -EApSettingsSelMenuSelectNormal: Same as in EApSettingsSelMenuNormal 
        *   BUT the first item is Select. This one makes it possible to select 
        *   an access point and also possible to edit, create, delete, etc.
        * <br>
        * The following three parameters specify the filtering and ordering 
        * criteria for the selection list creation. First, it is possible to 
        * specify which ISP-type we are looking for. The possible values are:
        * <br>
        * KEApIspTypeInternetOnly: Filter allows only access points that are 
        *   capable of only internet access.
        * <br>
        * KEApIspTypeWAPOnly: Filter allows only access points that are 
        *   capable of only WAP access.
        * <br>
        * KEApIspTypeInternetAndWAP: Filter allows only access points that are 
        *   capable of internet AND WAP access.
        * <br>
        * KEApIspTypeWAPMandatory: Filter allows only access points that are 
        *   capable of WAP access AND contain mandatory WAP settings.
        * <br>
        * KEApIspTypeAll: No filtering is done for the isp-type, all types of
        *   access points are shown.
        * <br>
        * It is possible to combine them by simply adding/or-ing together the 
        * values, filtering will be done in such a way that only access points 
        * with the listed ISP-types will be shown.
        * Second filtering possibility (aBearerFilter) is the desired bearer 
        * type. The possible values are the values of the TApBearerType enum 
        * type and their combinations, as in case of the ISP type.
        * The ordering of the list can be specified with the aSortType 
        * parameter. It can have the following values:
        * KEApSortUidAscending: The sorting is done according to the ID of 
        *   the access points in ascending order.
        * KEApSortUidDescending: The sorting is done according to the ID of 
        *   the access points in descending order.
        * KEApSortNameAscending: The sorting is done according to the name of 
        *   the access points in ascending order.
        * KEApSortNameDescending: The sorting is done according to the name of 
        *   the access points in descending order.
        * Caution! The ordering values can not be combined!
        * @param aStartWithSelection A boolean value indicating whether to 
        *   select or edit an ap.
        * @param aListType A TSelectionListType enum indicating the desired
        *   list type.
        * @param aSelMenuType A TSelectionMenuType enum indicating the desired
        *   menu type.
        * @param aIspTypeFilter Filtering criteria on ISP type
        * @param aBearerFilter Filtering criteria on bearer type
        * @param aSortType Specifies the sort order to use.
        * @param aReqIpvType Specifies whether the caller would like to 
        *   have IPv4, IPv6 or both access points. If it is IPv6 or Both, 
        *   in case of IPv6 support is available as a feature, it will be 
        *   supported. If IPv6 feature is not supported by the phone, it 
        *   simply defaults to the normal IPv4 version.
        *   If it is IPv4, it uses the default IPv4 version independently 
        *   from IPv6 feature beeing available or not.
        * @param aVpnFilterType a TVpnFilterType representing the possible 
        *   additional VPN filtering.
        * @return The constructed CApSettingsUi object.
        *
        * @deprecated
        */
        IMPORT_C static CApSettingsUi* NewLC(
                        TBool aStartWithSelection,
                        TSelectionListType aListType,
                        TSelectionMenuType aSelMenuType,
                        TInt aIspFilter,
                        TInt aBearerFilter,
                        TInt aSortType,
                        TInt aReqIpvType,
                        TVpnFilterType aVpnFilterType );


        /**
        * Destructor.
        *
        * @deprecated
        */
        IMPORT_C ~CApSettingsUi();


    protected:

        /**
        * Constructor.
        * @param aStartWithSelection A boolean value indicating whether to 
        *   select or edit an ap.
        * @param aListType A TSelectionListType enum indicating the desired
        *   list type.
        * @param aSelMenuType A TSelectionMenuType enum indicating the desired
        *   menu type.
        * @param aNeedIpv6Support Whether IPv6 is supported or not
        */
        CApSettingsUi( TBool aStartWithSelection,
                       TSelectionListType aListType,
                       TSelectionMenuType aSelMenuType,
                       TBool aNeedIpv6Support );



        /**
        * Second-phase constructor.
        * @param aIspTypeFilter Filtering criteria on ISP type
        * @param aBearerFilter Filtering criteria on bearer type
        * @param aSortType Specifies the sort order to use.
        * @param aVpnFilterType a TVpnFilterType representing the possible 
        *   additional VPN filtering.
        *
        * @deprecated
        */
        IMPORT_C void ConstructL(
                                TInt aIspFilter,
                                TInt aBearerFilter,
                                TInt aSortType,
                                TVpnFilterType aVpnFilterType
                                );


    public: // New functions

        /**
        * Starts the settings component.
        * Creates dialog(s) from scratch, launches and executes them.
        * Can leave with error codes.
        * @param TUint32 aHighlight The UID of the AP to highlight by default
        * @param TUint32 aSelected The UID of the access point selected 
        * by the user.
        * @return The accomplished task's flag-codes:
        * <br>
        * KApUiEventNone,
        * <br>
        * KApUiEventSelected,
        * <br>
        * KApUiEventCreatedBlank,
        * <br>
        * KApUiEventCreatedUsedOld,
        * <br>
        * KApUiEventEdited,
        * <br>
        * KApUiEventDeleted,
        * <br>
        * KApUiEventExitRequested
        * <br>and their combinations. ( Bitwise OR )
        * <br>
        * aSelected will be modified only if KApUiEventSelected is included 
        * in the flags.
        * <br>
        * In case it returns KApUiEventExitRequested, the user had choosen
        * the Exit option from the Options menu which means
        * that the caller application MUST exit!
        * <br>
        * Returns after completion!
        *
        * @deprecated
        */
        IMPORT_C TInt RunSettingsL( TUint32 aHighLight, TUint32& aSelected );


    private: //data
        TBool                   iStartWithSelection;
        TSelectionListType      iListType;
        TSelectionMenuType      iSelMenuType;

        TInt                    iReqIpvType;    // Default is IPv4

        CApSettingsHandlerImpl* iImpl;          // owned
        

    };

#endif

// End of File
