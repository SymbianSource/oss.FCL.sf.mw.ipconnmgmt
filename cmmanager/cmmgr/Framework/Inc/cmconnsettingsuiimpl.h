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
* Description:  Declaration of application selection dialog
*
*/

#ifndef C_CMCONNSETTINGSUIIMPL_H
#define C_CMCONNSETTINGSUIIMPL_H

#include <e32def.h>
#include <e32base.h>
#include <ConeResLoader.h>
#include <badesca.h>    // CDesCArrayFlat
#include <cmdefconnvalues.h>
#include <hlplch.h>

#include "cmmanager.hrh"
#include "cmapplicationsettingsui.h"

class TCmDCSettingSelection;
class CCmManagerImpl;
class CCmDestinationImpl;
class CCmPluginBase;

//CONSTS
const TInt KCmDefConnIndex = 0;
const TInt KCmUserDefinedIndex  = 1;

namespace CMManager
    {
    enum TCmDCSettingSelectionMode
        {
        EDCAlwaysAsk,         /**<
                            * The user selected Always Ask
                            */
        EDCAskOnce,         /**<
                            * The user selected Ask Once
                            */
        EDCDestination,       /**<
                            * The user selected a destination
                            */
        EDCConnectionMethod,   /**<
                            * The user selected a connection method
                            */
        EDCDefaultConnection   /**<
                            * The user selected a connection method
                            */
        };
    } //namespace CmManager
    
/**
 * Stores the details of a setting selection
 *
 *  @since S60 3.2
 */
NONSHARABLE_STRUCT( TCmDCSettingSelection ) //extended with Ask Once
    {
    /**
    * Selection that the user made
    */
    CMManager::TCmDCSettingSelectionMode iResult;
    
    /**
    * Id of the selected destination or connection method
    */
    TUint iId;
    // -----------------------------------------------------------------------------
    // TCmDCSettingSelection::operator==
    // -----------------------------------------------------------------------------
    //
    inline TBool TCmDCSettingSelection::operator==( TCmDCSettingSelection& aDCSettingSelection ) const
        {
        return ( (iResult == aDCSettingSelection.iResult) && ( iId == aDCSettingSelection.iId ) );
        };

    // -----------------------------------------------------------------------------
    // TCmDCSettingSelection::operator!=
    // -----------------------------------------------------------------------------
    //
    inline TBool TCmDCSettingSelection::operator!=( TCmDCSettingSelection& aDCSettingSelection ) const
        {
        return (!( (iResult == aDCSettingSelection.iResult) && ( iId == aDCSettingSelection.iId ) ));
        };    
    // -----------------------------------------------------------------------------
    // TCmDCSettingSelection::ConvertToDefConn
    // -----------------------------------------------------------------------------
    //
    inline TCmDefConnValue TCmDCSettingSelection::ConvertToDefConn( ) const
        {
        TCmDefConnValue defConnValue;
        defConnValue.iType = ECmDefConnAlwaysAsk;
        defConnValue.iId = 0;
        switch ( iResult )
            {
            case CMManager::EDCAlwaysAsk :       
                {
                defConnValue.iType = ECmDefConnAlwaysAsk;
                break;
                }
            case CMManager::EDCAskOnce :   
                {
                defConnValue.iType = ECmDefConnAskOnce;
                break;
                }
            case CMManager::EDCDestination :
                {
                defConnValue.iType = ECmDefConnDestination;
                defConnValue.iId = iId;
                break;
                }
            case CMManager::EDCConnectionMethod :
                {
                defConnValue.iType = ECmDefConnConnectionMethod;
                defConnValue.iId = iId;
                break;
                }
            case CMManager::EDCDefaultConnection : 
                {
                defConnValue.iType = ECmDefConnAlwaysAsk; //default value
                break;
                }
            default:
                {
                defConnValue.iType = ECmDefConnAlwaysAsk; //default value
                break;
                }
            };
        return defConnValue; 
        };
     };    
    

        
/**
 *  Implementation of application setting UI.
 *
 *  @lib cmmanage.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CCmConnSettingsUiImpl) : public CBase
    {

    public:

        IMPORT_C static CCmConnSettingsUiImpl* NewL( CCmManagerImpl* aCCmManagerImpl = NULL );
        static CCmConnSettingsUiImpl* NewWithoutTablesL();

        virtual ~CCmConnSettingsUiImpl();

    //=========================================================================    
    // UI API
    //
    public:

        /**
        * Launches the 'easy to use' Settings UI provided for applications
        * for handling Destinations and Connection Methods
        *
        * @since 3.2
        * @param aSelection If initialised to a valid value, sets the higlight to the
        * corresponding list item at startup. If a list of Destinations is displayed, and
        * the initial highlight is set to a Connection Method, the parent Destination
        * gets highlighted. On return, holds user selection.
        * @param aListItems A bit field which indicates which items shoud be
        *                   shown in the selection dialog. The bitmasks are 
        *                   defined in the TCmSelectionDialogItems enum.
        *
        * The function leaves with KErrNotSupported  if all the items which 
        * were set to be shown are not supported/enabled. 
        * E.g. Default Connection availability/support is controlled by the 
        * Feature Manager so if only EShowDefaultConnection is set and the 
        * Default Connection feature is not enabled than the function leaves
        * with KErrNotSupported.
        *
        * In other cases (when an item or items are not supported/enabled but
        * not all) the selected but not supported/enabled items are not added
        * to the list. The request to show such an item is silently ignored.
        *
        *
        * Special note about the EShowDestinations and EShowConnectionMethods
        * bitmasks:      
        *   The following enumerations controls how how destinations and 
        *   Connection Methods are presented in the selection UI.
        *   General rule about the EShowDestinations:
        *     Destinations which does not contain any Connection Methods are 
        *     not shown in the selection dialog. The same rule is applied if
        *     the Destination containes Connection Methods but all of them 
        *     are filtered out by the filtering criteria.
        *   EShowDestinations is set and EShowConnectionMethods is set:
        *     Destination list is added to the selection UI and a Connection
        *     Method can be selected inside a Destination.
        *   EShowDestinations is set and EShowConnectionMethods is not set:
        *     Destination list is added to the selection UI and no Connection
        *     Method can be selected inside a Destination. 
        *   EShowDestinations is not set and EShowConnectionMethods is set:
        *     Connection Method list is added to the selection UI. Connection
        *     Methods are ordered according to the global bearer type 
        *     priorities. Destination cannot be selected.
        *   EShowDestinations is not set and EShowConnectionMethods is not set:
        *     The selection UI will not contain any Destination or Connection 
        *     Method. E.g. If EShowAlwaysAsk and EShowDefaultConnection was 
        *     set then the selection UI will contain two entries (AlwaysAsk
        *     and DefaultConnection).
        *
        * @param aFilterArray Only Connection Methods with bearer types from
        *                     the aFilterArray will be showed. 
        *                     All Connection Methods will be listed if the 
        *                     array has no element.
        * @return TBool seleceted or not.
        *
        * @leave KErrArgument If the selection dialog would have zero 
        *                     elements based on the aListItems bitmask
        *                     (aListItems is equal to zero). 
        *
        * @leave KErrNotSupported 
        *                     If the selection dialog would have zero 
        *                     elements based on the aListItems bitmask
        *                     (all the selected elements in aListItems  
        *	                  are not supported/disabled).
        *                     E.g. aListItems = EShowDefaultConnection but 
        *                     Default Connection feature is not enabled.
        *
        * @leave KErrNotFound If the selection dialog would have zero 
        *                     elements caused by filtering out all the 
        *                     Connection Methods with the aFilterArray
        *                     and not requesting other list elements by 
        *                     the aListItems bitfield than an error note
        *                     is shown and the function call leaves with 
        *                     KErrNotFound error code. 
        *                     E.g. 
        *                       - aListItems = EShowConnectionMethods
        *                     but all Connection Methods are filtered 
        *                     out by the aFilterArray.
        */
        IMPORT_C TBool RunApplicationSettingsL( 
                                        TCmSettingSelection& aSelection, 
                                        TUint aListItems,
                                        TBearerFilterArray& aFilterArray);

        /**
        * Launches the Default Connection Radio Button Page
        * for use in Destinations View
        * @since 3.1U
        * @params aSelection User selection.
        * @returns ETrue if a selection was made
        */
        TBool RunDefaultConnecitonRBPageL ( TCmDCSettingSelection& aSelection );
        
        /**
        * Creates and returns a string which contains the default connection name
        * for use in Destinations View
        * @since 3.1U
        * @params aSelection User selection.
        * @returns the default connection name
        */        
        HBufC* CreateDefaultConnectionNameL( TCmDefConnValue aSelection );
        
        /**
        * Enables/Disables AlwaysAsk
        * 
        * @param ETrue enables
        * @since S60 3.2
        */
        inline void EnableAlwaysAsk( TBool aEnable ) 
                                        { iShowAlwaysAsk = aEnable; }
        
        /**
        * Enables/Disables EasyWlan
        * 
        * @param ETrue enables
        * @since S60 3.2
        */
        inline void EnableEasyWlan( TBool aEnable )
                                        { iShowEasyWlan = aEnable; }
        
        /**
        * Enables/Disables VirtualCMs
        * 
        * @param ETrue enables
        * @since S60 3.2
        */
        inline void EnableVirtualCMs( TBool aEnable )
                                        { iShowVirtualCMs = aEnable; }
        
        /**
        * Enables/Disables EmptyDestinations
        * 
        * @param ETrue enables
        * @since S60 3.2
        */
        inline void EnableEmptyDestinations( TBool aEnable ) 
                                        { iShowEmptyDestinations = aEnable; }
                                        
        /**
        * Sets a destination id which should be omitted from the list
        * typically this will be the parent destination of a virtual CM
        * 
        * @param aDestinationId
        * @since S60 3.2
        */
        inline void SetDestinationToOmit( TUint32 aDestinationId )
                                    { iDestinationToOmit = aDestinationId; }
        
        /**
        * Enables/Disables Ask Once
        * 
        * @param ETrue enables
        * @since S60 3.2
        */
        inline void EnableAskOnce( TBool aEnable ) 
                                        { iShowAskOnce = aEnable; }
                                        
        /**
        * Enables/Disables Uncategorised item
        * 
        * @param ETrue enables
        * @since S60 3.2
        */
        inline void EnableUncat( TBool aEnable ) 
                                        { iShowUncat = aEnable; }
                                        
        /**
        * Enables/Disables Default Connection item
        * 
        * @param ETrue enables
        * @since S60 3.2
        */
        inline void EnableDefConn( TBool aEnable ) 
                                        { iShowDefConn = aEnable; }



        /**
        * Sets the array of destinations to be used.
        * Typically this routine will be used by plugins having 
        * restrictions on possible destinations.
        * 
        * @param aDestinationArray
        * @since S60 3.2
        */
        IMPORT_C void SetDestinationArrayToUseL( RArray<TUint32> aDestinationArray );

        /**
        * Clear previously passed destinations and return to normal use.
        *
        * @since S60 3.2
        */
        IMPORT_C void ClearDestinationArrayToUse();
                                                
    private:

        CCmConnSettingsUiImpl();

        void ConstructL( CCmManagerImpl* aCCmManagerImpl = NULL );

        TInt LoadResourceL ();
        
        /**
        * Displays a set of destinations for selecting a network connection
        *
        * @param aSelection If initialised to a valid value, sets the higlight to the
        * corresponding list item at startup. If a list of Destinations is displayed, and
        * the initial highlight is set to a Connection Method, the parent Destination
        * gets highlighted. On return, holds user selection.
        * @return soft key selection
        */
        TBool ShowConnSelectRadioPageL( TCmDCSettingSelection& aSelection, 
                        const TInt aTitleStringResource, 
                        TBearerFilterArray& aFilterArray );

        /**
        * Displays a the Default Connection/User Defined radio button page 
        *
        * @param aSelection If initialised to a valid value, sets the higlight to the
        * corresponding list item at startup. If a list of Destinations is displayed, and
        * the initial highlight is set to a Connection Method, the parent Destination
        * gets highlighted. On return, holds user selection.
        * @return soft key selection
        */
    //    TBool ShowDefConnRadioPageL( TInt& aSelection );

        
        /**
        * Return array of destination of which have at least connection method
        * @param aItems array of destination names
        */
        void BuildDestinationArrayL( CDesCArrayFlat* aItems, 
                                        TBearerFilterArray& aFilterArray );

        /**
        * Return array of CMs of which have at least connection method
        * @param aItems array of destination names
        */
        void BuildCMArrayL( CDesCArrayFlat* aItems, 
                            TBearerFilterArray& aFilterArray );

        /**
        * Put the array of CMs into iConnMethods
        */
        void GetCMArrayL( );

        /**
        * Build uncategorized connection method array and append
        * 'Uncategorized' item if there's any.
        * @param aUncatArray array of uncategorized connection method ids.
        * @param aItems array of destination names
        */
        void BuildUncatArrayL( RPointerArray<CCmPluginBase>& aUncatArray,
                               CDesCArrayFlat* aItems, 
                               TBearerFilterArray& aFilterArray );
        /**
        * Show destination selection dialog.
        * @param aDestSelected index of selected destination
        * @param aItems array of destination names
        * @param aSelection If initialised to a valid value, sets the higlight to the
        * corresponding list item at startup. If a list of Destinations is displayed, and
        * the initial highlight is set to a Connection Method, the parent Destination
        * gets highlighted. On return, holds user selection.
        * @return ETrue if a selection was made
        */
        TBool UserSelectionDlgL( TInt& aDestSelected, 
                                 CDesCArrayFlat* aItems, 
                                 TCmDCSettingSelection& aSelection,
                                 const TInt aTitleStringResource,
                                 TBool aOpenDestination );

        /**
        * Show uncategorized connection method selection dialog
        * @param aUncatArray list of uncategorized connection methods.
        * @param aSelection If initialised to a valid value, sets the higlight to the
        * corresponding list item at startup. On return, holds user selection.
        * @return softkey selection.
        */
        TBool UserSelectedUncatItemL( RPointerArray<CCmPluginBase>& aUncatArray,
                                      TCmDCSettingSelection& aSelection, 
                                        const TInt aTitleStringResource );
                                      
        /**
        * A radio button page of all the connection methods belonging to
        * a the specified destination
        *
        * @param aDestinationId the destination from which a connection
        *                       method will be selected
        * @param aSelection If initialised to a valid value, sets the higlight to the
        * corresponding list item at startup. On return, holds user selection.
        * @return soft key selection
        */
        TBool ShowConnectionMethodsL( RPointerArray<CCmPluginBase>& aCmArray, 
                                      TCmDCSettingSelection& aSelection, 
                                      const TInt aTitleStringResource,
                                      TBearerFilterArray& aFilterArray );

        /**
        * Checks if a Connection method is virtual
        *
        * @since S60 3.2
        * @param aCmIapId the iap id of the CM
        * @return ETrue if virtual
        */
        TBool IsCmVirtualL( TUint32 aCmIapId );
                         
    private: // data

        /**
         * pointer to the CmManager - OWNED
         */
        CCmManagerImpl* iCmManagerImpl;
        
        /**
         * resource loader
         */
        RConeResourceLoader iResourceReader;
        
        /**
         * List of destination or CM IDs to show
         */ 
        RArray<TUint32> iItems;
        /**
         * Indicates whether iItems holds destinatos or CMs
         */
        TBool iAreDestinations;
                        
        /**
         * Indicates whether Always Ask should be shown in the page
         */
        TBool iShowAlwaysAsk;
        
        /**
         * Indicates whether EasyWlan should be shown in the page
         */
        TBool iShowEasyWlan;
        
        /**
         * Indicates whether virtual iaps should be shown in the page
         */
        TBool iShowVirtualCMs;
        
        /**
         * Indicates whether Empty destinations should be shown in the page
         */
        TBool iShowEmptyDestinations;
        
        /**
         * Indicates the id of destination to specifically omit (usually a parent destination)
         * if zero, it can be ignored
         */
        TUint iDestinationToOmit;
        
        /**
         * Indicates whether Ask Once item should be shown in the page
         */
        TBool iShowAskOnce;

        /**
         * Indicates whether Uncategorized item should be shown in the page
         */
        TBool iShowUncat;
        
        /**
         * Indicates whether Default Connection item should be shown in the page
         */
        TBool iShowDefConn;
                
        /**
         * Indicates whether we have an own CmManager that we have to create and destroy
         */
        TBool iOwnCmManager;

        /**
         * Indicates whether the selected destinaton should be opened
         */
        TBool iOpenDestination;
        
        /**
        * Indicates whether to use a passed list of destinations 
        * or use our own algorithm to get the list of possible destinations.
        */
        TBool iUsePassedDestinationArray;
        
        /**
        * An Array to hold the passed destinations.
        */
        RArray<TUint32> iPassedDestinations;
        
        /**
         * help context
         */        
        TCoeContextName iHelpContext;        

        /**
         * Indicates whether the tables should be created
         */
        TBool iCreateTables;
    };
#endif // C_CMCONNSETTINGSUIIMPL_H
