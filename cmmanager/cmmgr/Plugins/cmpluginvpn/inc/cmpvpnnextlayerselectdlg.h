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
* Description:  Popup for selecting the underlying connection method or 
*                destination for a VPN connection method
*
*/

#ifndef CMPVPN_NEXT_LAYER_SELECT_DLG_H
#define CMPVPN_NEXT_LAYER_SELECT_DLG_H

#include <e32base.h>
#include <eikmobs.h>  // MEikMenuObserver
#include <aknPopup.h> // CAknPopupList

#include <ConeResLoader.h> // resource reader

class CEikFormattedCellListBox;
class CEikMenuBar;
class CEikonEnv;
class CCmPluginBaseEng;
class CCmManagerImpl;
class CGulIcon;


    
/**
 *  CmPluginVpnNextLayerSelectDlg dialog class
 *
 *  
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CmPluginVpnNextLayerSelectDlg ) : public CAknPopupList, 
                                                     public MEikMenuObserver
    {
    public: // Constructors and destructor
    
        static CmPluginVpnNextLayerSelectDlg* NewL( 
                                        CCmPluginBaseEng& aCmPluginBaseEng,
                                        RArray<TUint32>& aBindableMethods,
                                        TBool& aSnapSelected,
                                        TUint32& aNextLayerId );
        
        /**
        * Destructor.
        */
        virtual ~CmPluginVpnNextLayerSelectDlg();
    
    private: // Constructor
    
        /**
        * Constructor.
        */      
        CmPluginVpnNextLayerSelectDlg( CCmPluginBaseEng& aCmPluginBaseEng,
                                       RArray<TUint32>& aBindableMethods,
                                       TBool& aSnapSelected,
                                       TUint32& aNextLayerId );
        
        /**
        * Second phase constructor. Leaves on failure.
        */      
        void ConstructL();

    public: // From MEikMenuObserver
        
        /**
         * See base class
         */
        void ProcessCommandL( TInt aCommandId );
        
        /**
         * See base class
         */
        void SetEmphasis( CCoeControl* aMenuControl, TBool aEmphasis );
        
        /**
         * See base class
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    public: // From CCoeControl
    
        /**
        * Returns the help context
        * From CCoeControl
        * 
        * @since S60 3.2
        * @param aContext the returned help context
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;
        
         /**
        * From CCoeControl, makes control visible/invisible.
        * @param aVisible ETrue if visible, EFalse if invisible.
        */
        void MakeVisible( TBool aVisible );
        
        /**
        * From CCoeControl
        *
        * @since S60 3.2
        * @param aKeyEvent The key event.
        * @param aType Key event type.
        * @return key response (was the key event consumed?).
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                     TEventCode aType );
                                     
        // From MEikListBoxObserver
        void HandleListBoxEventL( CEikListBox* aListBox, 
                                  TListBoxEvent aEventType );
                                   
    public: // new functions
   
        /**
        * Display the Options menu
        *
        *
        * @since S60 3.2
        */
        void DisplayMenuL();

        /**
        * Hide the Options menu.
        *
        *
        * @since S60 3.2
        */
        void HideMenu();

        /**
        * Is the Options menu opened?
        *
        *
        * @since S60 3.2
        * @return ETrue if showing
        */
        TBool MenuShowing() const;
        
    private: // new functions
    
        /*
        * Load the given resource file
        *
        * @since S60 3.2
        * @param aResFileName
        */
        void LoadResourceL( const TDesC& aResFileName );
        
        /*
        * Formats the texts for the list items using given resources
        *
        * @param aFirstLineResId the resource Id for the first line
        * @param aSecondLineResId the resource Id for the second line
        * @param aIconIndex the icon's index value
        * @return the formatted text
        */
        HBufC* FormatListItemTextsLC( TInt aFirstLineResId,
                                      TInt aSecondLineResId,
                                      TInt aIconIndex );
        
        
        
        /**
        */
        HBufC* FormatListItemTextsLC( TInt aFirstLineResId,
                                      const TDesC& aSecondLineText,
                                      TInt aIconIndex );
                                      
        /**
        */
        HBufC* FormatListItemTextsLC( const TDesC& aFirstLineResId,
                                      const TDesC& aSecondLineText,
                                      TInt aIconIndex );
                                      
        /**
        */
        HBufC* FormatListItemTextsL( const TDesC& aFirstLineText,
                                     const TDesC& aSecondLineText,
                                     TInt aIconIndex );
                                     
        /**
        * Displays a radio button selection dialog to select the 
        * underlying connection method
        *
        * @since S60 3.2
        * @param aDestinationId the destination id to search
        * @param aCmId the selection as a return value
        * @return ETrue if selection made EFalse if cancelled
        */
        TBool ShowCMSelectionDlgL( TUint32 aDestinationId, 
                                   TUint32& aCmId );

        void InitialiseL();
        
        void SetTextsAndIconsL();
        
        void AppendDestinationTextsL( CCmDestinationImpl& aDestination,
                                 TInt aNumCms,
                                 CDesCArray& aItems,
                                 CArrayPtr<CGulIcon>& aIcons );
                                       
        void AppendUncatDestinationL( CDesCArray& aItems,
                                      CArrayPtr<CGulIcon>& aIcons );
                                    
        void AppendEasyWlanL( CDesCArray& aItems,
                              CArrayPtr<CGulIcon>& aIcons );
                           

    private: // data members
  
        /**
         * Eikon environment
         */
        CEikonEnv& iMyEikonEnv;        
  
        /**
         * Owned list box
         */
        CEikFormattedCellListBox* iMyListBox;
        
        /**
         * Owned menu bar.
         */
        CEikMenuBar* iMenuBar;
        
        /**
         * base class not owned
         */
        CCmPluginBaseEng& iCmPluginBaseEng;
        
        /**
         * Result of selection: ETrue if a SNAP was selected
         */
        TBool& iSnapSelected;
        
        /**
         * Result of selection: The ID of the connection method or SNAP
         */
        TUint32& iNextLayerId;
                 
        /**
         * Resource reader
         */
        RConeResourceLoader iResourceReader;
                 
        /**
         * An array containing all of the destination's ids
         */
        RArray<TUint32>& iDestinations;
        
        /**
         * The id of EasyWlan ( zero if none )
         */
        TUint32 iEasyWlanId;
        
        /**
         * access to the cmmanager
         */
        CCmManagerImpl& iCmManager; // not owned
        
        /**
         * Flag to determine if there are uncategorised connection methods
         */
        TBool iUncatItems;
        
    };
#endif // CMPVPN_NEXT_LAYER_SELECT_DLG_H
