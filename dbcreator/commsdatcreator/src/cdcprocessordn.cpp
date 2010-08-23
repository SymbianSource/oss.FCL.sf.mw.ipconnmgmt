/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of the class CProcessorDN
*
*/




// INCLUDE FILES

#include "cdcprocessordn.h"
#include "cdclogger.h"
#include "cdctagcontainer.h"

using namespace CMManager;

// CONSTANTS

_LIT16( KDefaultDestinationName, "Default destination" );
//_LIT16( KMetadataUserDefined, "UserDefined" );
//_LIT16( KMetadataInternet, "Internet" );

// constants for TSnapPurpose
_LIT16( KPurposeUnknown, "UserDefined" );
_LIT16( KPurposeInternet, "Internet" );
_LIT16( KPurposeOperator, "Operator" );
_LIT16( KPurposeMMS, "MMS" );
_LIT16( KPurposeIntranet, "Intranet" ); 


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CProcessorDN::NewL
// ---------------------------------------------------------
//
CProcessorDN* CProcessorDN::NewL( CReaderBase* aFileReader,
                                  RCmManagerExt& aCmManager,
                                  RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                                  RPointerArray< HBufC >& aPluginNames, 
                                  RPointerArray< RCmDestinationExt >& aDestArray,
                                  RPointerArray< HBufC >& aDestNames,
                                  RArray<TBool>& aCmInDest,
                                  TBool aIsFirstRound )
    {
    CProcessorDN* self = new ( ELeave ) CProcessorDN( aFileReader,
                                                      aCmManager,
                                                      aPluginArray,
                                                      aPluginNames,
                                                      aDestArray,
                                                      aDestNames,
                                                      aCmInDest,
                                                      aIsFirstRound );
    CleanupStack::PushL( self );

    // From base class
    self->ConstructL();
    CleanupStack::Pop( self ); // self
    return self;
    }
    
    
// ---------------------------------------------------------
// CProcessorDN::~CProcessorDN
// ---------------------------------------------------------
//
CProcessorDN::~CProcessorDN()
    {
    iDestination.Close();
    }
     
     
// ---------------------------------------------------------
// CProcessorDN::ConstructL
// ---------------------------------------------------------
//
void CProcessorDN::ConstructL()
    {
    }


// ---------------------------------------------------------
// CProcessorDN::CProcessorDN
// ---------------------------------------------------------
//
CProcessorDN::CProcessorDN( CReaderBase* aFileReader,
                            RCmManagerExt& aCmManager,
                            RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                            RPointerArray< HBufC >& aPluginNames, 
                            RPointerArray< RCmDestinationExt >& aDestArray,
                            RPointerArray< HBufC >& aDestNames,
                            RArray< TBool >& aCmInDest, 
                            TBool aIsFirstRound ) :
                                      CProcessorBase( aFileReader,
                                                      aCmManager,
                                                      aPluginArray,
                                                      aPluginNames,
                                                      aDestArray,
                                                      aDestNames ),
                                      iProtectionLevel( EProtLevel0 ),
                                      iIsFirstRound( aIsFirstRound )
    {
    iCmInDest = &aCmInDest;
    }


// ---------------------------------------------------------
// CProcessorDN::ProcessTagL
// ---------------------------------------------------------
//
void CProcessorDN::ProcessTagL( TBool /*aFieldIDPresent*/ )
    {
    
    iDestinationDropped = EFalse;
    
    iProtectionLevel = EProtLevel0;
    
    if( iIsFirstRound )
        {
        TInt dnId = -1;
        for ( TInt idx(0); idx < TagContainer().Count(); idx++)
            {
            if ( TagContainer().FieldId( idx ) == EDN_Id )
                {
                HBufC16* ptrTag = TagContainer().Tag(idx);
                TLex16 lex(ptrTag->Des());
                TInt err = lex.Val(dnId);
                if (err != KErrNone)
                    {
                    CLOG_WRITE( "The destinaton Id field is bad.");
                    User::Leave(err);    
                    }
                break;
                }
            }
        if (dnId == -1)
            {
            iDestination = iCmManager->CreateDestinationL( KDefaultDestinationName );        
            CLOG_WRITE( "Destinaton created without dnId.");
            }
        else
            {
            iDestination = iCmManager->CreateDestinationL( KDefaultDestinationName, dnId);        
            CLOG_WRITE_FORMAT( "Destinaton created. dnId:%d", dnId);
            }
        }
        
    HBufC *destName( NULL );
    TInt priority = 0;
    RPointerArray<HBufC> names;
    
    for (TInt idx(0); idx < TagContainer().Count(); idx++)
        {
        HBufC16* ptrTag = TagContainer().Tag(idx);
        TInt field =  TagContainer().FieldId( idx );
        
        if( iIsFirstRound && !iDestinationDropped )
            {
            switch ( field )
                {
                case EDN_Name:
                    {
                    // If more destinations exist with same name then one is kept
                    TRAPD( err, iDestination.SetNameL( *ptrTag ) );
                    if( KErrAlreadyExists == err )
                        {
                        CLOG_WRITE_FORMAT( 
                        "Error: Destination exist: %S. It is dropped!", ptrTag )
                        CLOG_WRITE( "\t \r\n" )
                        iDestinationDropped = ETrue;
                        }
                    else
                        {
                        destName = ptrTag->AllocLC();    
                        }
                    }
                break;
                case EDN_Metadata:
                    {
                    if( !ptrTag->Compare( KPurposeUnknown ) )
                        {
                        iDestination.SetMetadataL( ESnapMetadataPurpose, 
                                                   ESnapPurposeUnknown );
                        }
                    else if( !ptrTag->Compare( KPurposeInternet ) )
                        {
                        iDestination.SetMetadataL( 
                            ESnapMetadataPurpose, 
                            ESnapPurposeInternet );
                        }
                    else if( !ptrTag->Compare( KPurposeOperator ) )
                        {
                        iDestination.SetMetadataL( 
                            ESnapMetadataPurpose, 
                            ESnapPurposeOperator );
                        }
                    else if( !ptrTag->Compare( KPurposeMMS ) )
                        {
                        iDestination.SetMetadataL( 
                            ESnapMetadataPurpose, 
                            ESnapPurposeMMS );
                        }
                    else if( !ptrTag->Compare( KPurposeIntranet ) )
                        {
                        iDestination.SetMetadataL( 
                            ESnapMetadataPurpose, 
                            ESnapPurposeIntranet );
                        }
                    else
                        {
                        //no valid data is given - use default value
                        iDestination.SetMetadataL( ESnapMetadataPurpose, 
                                                   ESnapPurposeUnknown );
                        CLOG_WRITE( "! Error : Invalid Metadata. Default:User Defined");
                        }
                    }
                break;
                case EDN_Protection:
                    {
                    TPtrC16 protPtr = ptrTag->Right( ptrTag->Length() );
                    TLex16 lex( protPtr );
                    TUint32 prot( 0 );
                    if ( lex.Val( prot, EDecimal ) == KErrNone )
                        {
                        if( prot <= EProtLevel3 ) // prot is unsigned so no need
                                                  // to check lower boundary
                            {
                            iProtectionLevel = TProtectionLevel( prot );
                            }
                        else
                            {
                            //no valid data is given - use default value
                            iProtectionLevel = EProtLevel0;
                            CLOG_WRITE( "! Error : Invalid Protection level. Default:Level0");
                            }
                        }
                    }
                break;
                case EDN_Hidden:
                    {
                    if( !ptrTag->Compare( KStrYes ) )
                        {
                        iDestination.SetHiddenL( ETrue );
                        }
                    else if( !ptrTag->Compare( KStrNo ) )
                        {
                        iDestination.SetHiddenL( EFalse );
                        }
                    else
                        {
                        //no valid data is given - use default value
                        iDestination.SetHiddenL( EFalse );
                        CLOG_WRITE( "! Error : Invalid Hidden. Default:No");
                        }
                    }
                break;
                case EDN_HiddenAgent:
                    {
                    if( !ptrTag->Compare( KStrYes ) )
                        {
                        iDestination.SetMetadataL( ESnapMetadataHiddenAgent, ETrue );
                        }
                    else if( !ptrTag->Compare( KStrNo ) )
                        {
                        iDestination.SetMetadataL( ESnapMetadataHiddenAgent, EFalse );
                        }
                    else
                        {
                        iDestination.SetMetadataL( ESnapMetadataHiddenAgent, EFalse );
                        //no valid data is given - use default value
                        CLOG_WRITE( "! Error : Invalid HiddenAgent. Default:No");
                        }
                    }
                break;
                case EDN_Highlighted:
                    {
                    if( !ptrTag->Compare( KStrYes ) )
                        {
                        iDestination.SetMetadataL( ESnapMetadataHighlight, ETrue );
                        }
                    else if( !ptrTag->Compare( KStrNo ) )
                        {
                        iDestination.SetMetadataL( ESnapMetadataHighlight, EFalse );
                        }
                    else
                        {
                        iDestination.SetMetadataL( ESnapMetadataHighlight, EFalse );
                        //no valid data is given - use default value
                        CLOG_WRITE( "! Error : Invalid Highlighted. Default:No");
                        }
                    }
                break;
                } // switch
            } // if
         else if( !iIsFirstRound && !iDestinationDropped )
            {
            switch ( field )
                {
                case EDN_Name:
                 {
                 iDest = FindDestinationL( ptrTag );
                 if( iDest == NULL )
                     {
                     CLOG_WRITE_FORMAT( "Error: Destination not found: %S ", ptrTag )
                     iDestinationDropped = ETrue;
                     }
                 else
                     {
                     CLOG_WRITE_FORMAT( "\t Destination name: %S ", ptrTag )
                     }
                 }
                break;
            case EDN_IAPName:
                   {
                   TInt pos;
                   RCmConnectionMethodExt* plugin = FindPluginL( ptrTag, pos );
                   if( plugin )
                        {
                        TRAPD( err, iDest->AddConnectionMethodL( *plugin ) );
                        // Set the priority according to the order in which the IAPs are in the XML
                        TRAPD( err2 ,iDest->ModifyPriorityL( *plugin, priority ));
                        if ( err2 != KErrNone )
                        	{
                        	err2 = KErrNone; // to remove the compiler warnings
                        	}
                        
                        iDest->UpdateL();
                        priority++;
                        
                        if( err == KErrNone )
                            {
                            CLOG_WRITE_FORMAT( "\t Added connection method: %S ", ptrTag )
                            (*iCmInDest)[ pos ] = ETrue;
                            }
                        else
                            {
                            CLOG_WRITE_FORMAT( 
                            "Warning connection method could not added: %S ", ptrTag )
                            }
                        }
                     else
                        {
                        CLOG_WRITE_FORMAT( 
                        "Warning: plugin could not added: %S ", ptrTag )
                        }                       
                   }
                break;
            case EDN_EmbeddedDNName:
                    {
                    const RCmDestinationExt* embDestination = FindDestinationL( ptrTag );
                    if( embDestination )
                        {
                    
                        TRAPD( err, iDest->AddEmbeddedDestinationL( *embDestination ) );
                    
                        if( err == KErrNone )
                            {
                            CLOG_WRITE_FORMAT( "\t Added embedded destination: %S ", ptrTag )
                            }
                        else
                            {
                            CLOG_WRITE_FORMAT( 
                            "Warning embedded destination could not added: %S ", ptrTag )
                            }
                        }
                     else
                        {
                        CLOG_WRITE_FORMAT( 
                        "Warning embedded destination could not added: %S ", ptrTag )
                        }
                        
                    }
                break;
            default:
                {
                break;
                }
              } // switch
                
            } // if
            
        }//for      
    
        if( iIsFirstRound && !iDestinationDropped )
            {
            RCmDestinationExt* newItem = new (ELeave) 
            RCmDestinationExt( iDestination );
    
            iDest = newItem;

            iDestArray->Append( newItem );
            iDestName->Append( destName );
            CleanupStack::Pop( destName );
            }        
    }
     
     
// ---------------------------------------------------------
// CProcessorDN::ProcessAPL
// ---------------------------------------------------------
//
void CProcessorDN::ProcessAPL()
    {
    if( !iDestinationDropped )
        {
        if( iDest )
            {
            if( iIsFirstRound )
                {
                iDest->SetProtectionL( iProtectionLevel );
                }
            iDest->UpdateL();
            
            if( iIsFirstRound )
                {
                DoLogL();
                }
            else
                {
                CLOG_WRITE( "\t \r\n" )
                }
            }
        }
    }
    

// ---------------------------------------------------------
// CProcessorDN::DoLogL
// ---------------------------------------------------------
//
void CProcessorDN::DoLogL()
    {
    HBufC16* tmp;

    tmp = iDestination.NameLC();

    // Writes some data of the destination network into the log file.
    CLOG_WRITE( "Destination Network created:" )
    CLOG_WRITE_FORMAT( "\t Destination name: %S ", tmp )
    TUint32 uid = iDest->Id();
    CLOG_WRITE_FORMAT( "\t Destination ID: %d", uid );

    CLOG_WRITE( "\t \r\n" )

    CleanupStack::PopAndDestroy( tmp ); // Removes tmp.
    }

// End of File.
