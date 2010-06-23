/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Handles IAP selection logic
*
*/

#ifndef MPMIAPSELECTION_H
#define MPMIAPSELECTION_H

#include <e32base.h>

#include "mpmserversession.h"
#include "mpmcommsdataccess.h"

class CMPMConfirmDlgStarting;
class TMpmConnPref;

/**
 * Handles IAP selection. MPM Server session
 * uses the class to select the best available 
 * IAP.
 *
 *  @lib mpmserver.exe
 *  @since S60 v3.2
 */
class CMPMIapSelection : public CBase
    {

public:
    // Enumeration describes session state at choosing best IAP.
    // Choosing best IAP may use asyncrous services and once 
    // and these states define at which point the choosing iap is
    // proceeding.
    // 
    enum TChooseIapState
        {
        ENoConnection,
        EExplicitConnection,
        EImplicitConnection
        };

    enum TImplicitConnectionState
        {
        EImplicitStart,
        EImplicitWlanScan,
        EImplicitUserSelection,
        EImplicitWlanQuery
        };


public: // CMPMServerSession interface    
    /**
     * Two-phased constructor.
     */
    static CMPMIapSelection* NewL( CMPMCommsDatAccess*  aCommsDatAccess,
                                   CMPMServerSession*   aSession,
                                   CConnectionUiUtilities* aConnUiUtils );
    
    /**
     * Destructor.
     */
    virtual ~CMPMIapSelection();

    /**
     * Chooses best available IAP when connection is starting.
     * @param aChooseIapPref Connection preferences.
     *
     * @since S60 v3.2
     */
    void ChooseIapL( const TMpmConnPref& aChooseIapPref );

    /**
     * Handles IAP selection for explicit connection.
     * If necessary performs Wlan scan. 
     *
     * @since S60 v3.2
     */
    void ExplicitConnectionL();

public:
    /**
     * Returns true if given iap is a wlan iap.
     *
     * @param aIapId Id of Iap
     * @return ETrue if iap aIap is wlan
     * @since symbian^4
     */
    TBool IsIapWlanL( TUint32 aIapId );

    /**
     * Part of selecting best IAP at connection start. Called when WLAN scan
     * is completed. Continues IAP selection according to the iChooseIapState.
     *
     * @since 3.1
     * @param aStatus Error code of completing the request.
     */
    void ChooseIapWLANScanCompletedL( TInt aStatus );

    /**
     * Completing choose iap request. Uses corresponding function
     * of CMPMServerSession
     *
     * @since 3.2
     * @param aError Message status
     * @param aPolicyPref Policy preference to be returned, NULL if no 
     * preference should be passed
     */
    void ChooseIapComplete( TInt aError, const TMpmConnPref* aPolicyPref );

    /**
     * Stops displaying the starting dialog.
     *
     * @since 3.2
     */
    void StopDisplayingStartingDlg();

    /**
     * Selects the best IAP for the connection.
     *
     * @since 3.0
     * @param aMpmConnPref Connection preferences
     * @param aAvailableIAPs Array of available IAPs
     * @return KErrNone if successful, otherwise one of the
     * system-wide error codes
     */
    void ChooseBestIAPL( TMpmConnPref&          aMpmConnPref,
                         const RArray<TUint32>& aAvailableIAPs);

    /**
     * Selects the best IAP for the connection.
     * @since 3.2
     * @param aMpmConnPref Connection preferences.
     * @param aAvailableIAPs Array of available IAPs.
     * @param aNextBestExists ETrue if there is another available 
     * IAP inside the destination that could be used.
     * @return KErrNone if successful, otherwise one of the
     * system-wide error codes.
     */
    void ChooseBestIAPL( TMpmConnPref&           aMpmConnPref,
                         const RArray<TUint32>&  aAvailableIAPs, 
                         TBool&                  aNextBestExists );

    /**
     * Sets confirmation dialog ponter to NULL.
     *
     * @since 3.0
     */
    void SetConfirmDlgStartingPtrNull();

    /**
     * Returns reference to CMPMServerSession
     *
     * @since S60 3.2
     */
    CMPMServerSession* Session();

    /**
     * Handles user IAP/SNAP selection error and completes the request.
     *
     * @since 3.2
     * @param aError Error value 
     *
     */
    void HandleUserSelectionError( TInt aError );

    /**
     * Handles user IAP/SNAP selection and completes the request.
     *
     * @since 3.2
     * @param aIsIap ETrue if selection is iap
     * @param aId Id of IAP or SNAP record. May be 0, when error
     * occurred in selection
     * @param aError Error value 
     *
     */
    void HandleUserSelectionL( TBool aIsIap, TUint32 aId, TInt aError  );

    /**
     * Get original MPM connection preferences
     * @since 5.2
     */
     TMpmConnPref MpmConnPref();
     
     /**
      * Completing implicit connection.
      * @since 3.2
      */
     void CompleteImplicitConnectionL();
         

    // internal functions
private:
    /**
     * Handles IAP selection for implicit connection.
     * If necessary performs Wlan scan. 
     *
     * @since 3.2
     *
     */
    void ImplicitConnectionCheckWlanScanNeededL();

    /**
     * Completes IAP selection with explicit 
     * SNAP connection. If necessary, displays 
     * confirmation query.
     *
     * @since S60 v3.2
     */
    void CompleteExplicitSnapConnectionL();

    /**
     * Check whether GPRS services are allowed or not.
     * @since 3.2
     * @param aIapLanOrWlan IAP service type is either LANService or not 
     * @return ETrue if GPRS services are allowed
     */
    TBool CheckGprsServicesAllowedL( TBool aIapLanOrWlan );

    /**
     * Handles implicit connection IAP selection
     * @since 3.2
     */
    void ImplicitConnectionIapSelectionL();

    /**
     * Handles implicit connection start
     * @since 3.2
     */
    void ImplicitConnectionL();

    /**
     * Handles implicit connection wlan note displaying
     * @since 3.2
     */
    void ImplicitConnectionWlanNoteL();
    
private:

    /**
     * C++ default constructor.
     */
    CMPMIapSelection( CMPMCommsDatAccess*   aCommsDatAccess,
                      CMPMServerSession*    aSession,
                      CConnectionUiUtilities* aConnUiUtils );

    /**
     * ConstructL
     */
    void ConstructL();

private:

    // Connection preferences used for connection selection.
    // This is stored when ChooseIapL() is called and is used by methods 
    // selecting the best iap. This member variable is also used 
    // to store selection before writing it back to client message.
    TMpmConnPref iChooseIapPref;

    // Keeps state of choosing iap
    TChooseIapState iChooseIapState;

    // Used for commsdat related functionalities
    CMPMCommsDatAccess* iCommsDatAccess;

    // Handle to connection UI utilities
    CConnectionUiUtilities* iConnUiUtils;

    // Contains state info whether PrefIapnotifs can be sent 
    // and saved IAP info structure.
    TStoredIapInfo iStoredIapInfo;

    // Stored available Iaps
    RAvailableIAPList iStoredAvailableIaps;

    // pointer to Session
    CMPMServerSession* iSession;

    // Pointer to Confirmation dialog.
    CMPMConfirmDlgStarting* iConfirmDlgStarting;

    // Stores the boolean value whether next best iaps 
    // exists in explicit connection start.
    TBool iNextBestExists;

    // Iap selected by user.
    TUint32 iUserSelectionIapId;

    // Snap selected by user.
    TUint32 iUserSelectionSnapId;

    // State of impilict connection start
    TImplicitConnectionState iImplicitState;

    // True if connection is roaming (set when displaying offline note)
    TBool iIsRoaming;

    // True if user has selected "Known and new" for join wlan networks
	// setting in Connectivity settings plugin.
    TBool iNewWlansAllowed;
    };

#endif // MPMIAPSELECTION_H
