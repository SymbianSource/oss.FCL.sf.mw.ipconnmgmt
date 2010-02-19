/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Confirm Dialog implementation
*
*/

/**
@file mpmconfirmdlgstarting.cpp
Starting Dialog implementation
*/

// INCLUDE FILES

#include "mpmconfirmdlgstarting.h"
#include "mpmconnmonevents.h"
#include "mpmlogger.h"
#include "mpmiapselection.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPMConfirmDlgStarting::NewL
// -----------------------------------------------------------------------------
//
CMPMConfirmDlgStarting* CMPMConfirmDlgStarting::NewL( 
    CMPMIapSelection&                   aIapSelection,
    const TUint32                       aConnId,
    const TUint32                       aSnapId,
    const TUint32                       aIAP,
    const CMPMConfirmDlg::TDialogType   aDialogType, 
    const TMpmConnPref&                 aPref,
    CMPMServer&                         aServer,
    CMPMServerSession&                  aSession,
    const TInt                          aIapState )
    {
    CMPMConfirmDlgStarting* self = new ( ELeave ) CMPMConfirmDlgStarting( 
                                                          aIapSelection,
                                                          aConnId,
                                                          aSnapId,
                                                          aIAP,
                                                          aDialogType,
                                                          aPref,
                                                          aServer,
                                                          aSession,
                                                          aIapState);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CMPMConfirmDlgStarting::CMPMConfirmDlgStarting
// -----------------------------------------------------------------------------
//
CMPMConfirmDlgStarting::CMPMConfirmDlgStarting( 
    CMPMIapSelection&                   aIapSelection,
    const TUint32                       aConnId,
    const TUint32                       aSnapId,
    const TUint32                       aIAP,
    const CMPMConfirmDlg::TDialogType   aDialogType, 
    const TMpmConnPref&                 aPref,
    CMPMServer&                         aServer,
    CMPMServerSession&                  aSession,
    const TInt                          aIapState )
    : iIapSelection( aIapSelection ),
      iConnId( aConnId ),
      iSnapId( aSnapId ),
      iIAP( aIAP ),
      iDialogType( aDialogType ),
      iConfirmDlg( NULL ),
      iPref( aPref ),
      iServer( aServer ),
      iSession ( aSession ),
      iIapState( aIapState )
    {
    }


// -----------------------------------------------------------------------------
// CMPMConfirmDlgStarting::ConstructL
// -----------------------------------------------------------------------------
//
void CMPMConfirmDlgStarting::ConstructL()
    {
    CArrayPtrFlat<CMPMDialogBase>* queue = ((CArrayPtrFlat<CMPMDialogBase>*)iServer.StartingQueue());
    iConfirmDlg = CMPMConfirmDlg::NewL( *queue,
                                        *this, 
                                        iSnapId, 
                                        iIAP, 
                                        iDialogType );
    }


// -----------------------------------------------------------------------------
// CMPMConfirmDlgStarting::~CMPMConfirmDlgStarting
// -----------------------------------------------------------------------------
//
CMPMConfirmDlgStarting::~CMPMConfirmDlgStarting()
    {
    MPMLOGSTRING2( "CMPMConfirmDlgStarting<0x%x>::~CMPMConfirmDlgStarting", 
        iConnId )
    delete iConfirmDlg;
    }


// -----------------------------------------------------------------------------
// CMPMConfirmDlgStarting::ConfirmDlgResponse
// -----------------------------------------------------------------------------
//
void CMPMConfirmDlgStarting::ConfirmDlgResponse( 
    const TInt                      aError,
    const TMsgQueryLinkedResults    aResult )
    {
    MPMLOGSTRING4( "CMPMConfirmDlgStarting<0x%x>::ConfirmDlgResponse\
 error: %d result: %d", iConnId, aError, aResult )

    // Store value to be verified in destructor 
    // 
    iMsgQuery = aResult;
    iError = aError;
   
    // First case, we are asking the confirmation dialog when the connection 
    // is starting. 
    // 

    if ( aError == KErrNone )
        {
        switch (aResult)
            {
            case EMsgQueryThisTime:
                UserSelectedConnectThisTime();
                break;

            case EMsgQueryAutomatically:
                UserSelectedConnectAutomatically();
                break;

            case EMsgQueryCancelled:
                UserSelectedCancel( KErrCancel );
                break;

            default:
                UserSelectedCancel( KErrCancel );
                break;            
            }

        } 
    else
        {
        UserSelectedCancel( aError );
        }
    
    // We are done
    // 
    iIapSelection.SetConfirmDlgStartingPtrNull();
    delete this;
    }


// -----------------------------------------------------------------------------
// CMPMConfirmDlgStarting::ConfirmDlgResponse
// -----------------------------------------------------------------------------
//
TBool CMPMConfirmDlgStarting::ConfirmDlgResponse( MMPMConfirmDlg* aDlg )
    {
    TBool ret( EFalse );
    CMPMConfirmDlgStarting* dlg = (CMPMConfirmDlgStarting*)aDlg;
    if ( dlg && iIAP == dlg->iIAP )
        {
        ConfirmDlgResponse( dlg->iError, dlg->iMsgQuery );
        ret = ETrue;
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CMPMConfirmDlgStarting::UserSelectedCancel
// -----------------------------------------------------------------------------
//
void CMPMConfirmDlgStarting::UserSelectedCancel( const TInt aError )
    {
    MPMLOGSTRING2( "CMPMConfirmDlgStarting<0x%x>::UserSelectedCancel", 
        iConnId )
    MPMLOGSTRING3( "CMPMConfirmDlgStarting<0x%x>::UserSelectedCancel:\
 Complete %d", iConnId, aError )
    iIapSelection.ChooseIapComplete( aError, NULL );
    }


void CMPMConfirmDlgStarting::UserSelectedConnectThisTime()
    {
    MPMLOGSTRING2( "CMPMConfirmDlgStarting<0x%x>::UserSelectedConnectThisTime", iConnId )
    
    TBool wlanNeeded( EFalse );
    TRAPD( err, wlanNeeded = iIapSelection.StartWlanQueryIfNeededL( iIAP ) )
    if( err == KErrNone )
        {
        if( !wlanNeeded )
            {
            if ( iIapState == CMPMIapSelection::EImplicitConnection )
                {
                TRAP( err, iIapSelection.CompleteImplicitConnectionL() )
                
                if ( err )
                    {
                    MPMLOGSTRING2( "iIapSelection.CompleteImplicitConnectionL failed with %d", 
                    err )
                    iIapSelection.ChooseIapComplete( err, NULL );
                    }
                }
            else
                {
                iServer.AppendBMConnection( iConnId, 
                                            iSnapId, 
                                            iIAP, 
                                            EStarting,
                                            iSession );

                // Complete the ChooseBestIAP message with 
                // KErrNone if user selected yes.
                // 
                MPMLOGSTRING2( "CMPMConfirmDlgStarting<0x%x>::UserSelectedConnectThisTime:\
Complete KErrNone", iConnId )
                iIapSelection.ChooseIapComplete( KErrNone, &iPref );                        
                }
            }
        
        }
    else
        {
        MPMLOGSTRING3( "CMPMConfirmDlgStarting<0x%x>::UserSelectedConnectThisTime:\
 StartWlanQueryIfNeededL failed with %d", iConnId, err )
        iIapSelection.ChooseIapComplete( err, &iPref );       
        }
    }


void CMPMConfirmDlgStarting::UserSelectedConnectAutomatically()
    {
    MPMLOGSTRING2( "CMPMConfirmDlgStarting<0x%x>::UserSelectedConnectAutomatically", iConnId )
    //Store selected value to commsdat if we are in home network
    if (iDialogType == CMPMConfirmDlg::EConfirmDlgHomeNetwork )
        {
        TCmGenConnSettings genConnSettings;

        TRAPD(errorCode,genConnSettings = iServer.CommsDatAccess()->ReadGenConnSettingsL()); // call a function

        //If reading of database failed we do not write back to the database to prevent random values
        if (errorCode == KErrNone)
            {
            genConnSettings.iCellularDataUsageHome = ECmCellularDataUsageAutomatic;        
            TRAP_IGNORE(iServer.CommsDatAccess()->WriteGenConnSettingsL( genConnSettings )); 
            }
        } 
    
    TBool wlanNeeded( EFalse );
    TRAPD( err, wlanNeeded = iIapSelection.StartWlanQueryIfNeededL( iIAP ) )
    if( err == KErrNone )
        {
        if( !wlanNeeded )
            {
            if ( iIapState == CMPMIapSelection::EImplicitConnection )
                {
                TRAP( err, iIapSelection.CompleteImplicitConnectionL() )
                
                if ( err )
                    {
                    MPMLOGSTRING2( "iIapSelection.CompleteImplicitConnectionL failed with %d", 
                    err )
                    iIapSelection.ChooseIapComplete( err, NULL );
                    }
                }
            else
                {
                iServer.AppendBMConnection( iConnId, 
                                            iSnapId, 
                                            iIAP, 
                                            EStarting,
                                            iSession );

                 // Complete the ChooseBestIAP message with 
                 // KErrNone if user selected yes.
                 // 
                 MPMLOGSTRING2( "CMPMConfirmDlgStarting<0x%x>::UserSelectedConnectAutomatically:\
Complete KErrNone", iConnId )
                iIapSelection.ChooseIapComplete( KErrNone, &iPref );    
                }
            }
        }
    else
        {
        MPMLOGSTRING3( "CMPMConfirmDlgStarting<0x%x>::UserSelectedConnectAutomatically:\
 StartWlanQueryIfNeededL failed with %d", iConnId, err )
        iIapSelection.ChooseIapComplete( err, &iPref );       
        }
       
    }


// End of file
