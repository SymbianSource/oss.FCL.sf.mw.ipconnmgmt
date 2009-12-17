/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of the class CProcessorBase
*
*/




// INCLUDE FILES

#include "cdcprocessorbase.h"
#include "cdcreaderbase.h"
#include "cdclogger.h"

#include <cmmanagerext.h>
#include <cmconnectionmethoddef.h>
#include <cmconnectionmethodext.h>
#include <cmdestinationext.h>
#include <datamobilitycommsdattypes.h>


using namespace CMManager;


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CProcessorBase::CProcessorBase
// Constructor.
// ---------------------------------------------------------
//
CProcessorBase::CProcessorBase( CReaderBase* aFileReader,
                                RCmManagerExt& aCmManager,
                                RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                                RPointerArray< HBufC >& aPluginNames ) :
        iFileReader( aFileReader ), 
        iProxyEnabled( EFalse ),
        iIsProtected( EFalse )
    {
    iCmManager = &aCmManager;
    
    iPluginArray = &aPluginArray;
    iPluginName = &aPluginNames;
    }


CProcessorBase::CProcessorBase( CReaderBase* aFileReader,
                        RCmManagerExt& aCmManager,
                        RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                        RPointerArray< HBufC >& aPluginNames,
                        RPointerArray< RCmDestinationExt >& aDestArray,
                        RPointerArray< HBufC >& aDestNames ) :
        iFileReader( aFileReader ), 
        iProxyEnabled( EFalse ),
        iIsProtected( EFalse )
    {
    iCmManager = &aCmManager;
    
    iPluginArray = &aPluginArray;
    iPluginName = &aPluginNames;
    
    iDestArray = &aDestArray;
    iDestName = &aDestNames;
    }


// ---------------------------------------------------------
// CProcessorBase::~CProcessorBase
// Destructor.
// ---------------------------------------------------------
//
CProcessorBase::~CProcessorBase()
    {
    iPlugin.Close();
    if( iName )
        {
        delete iName;
        }
    }


// ---------------------------------------------------------
// CProcessorBase::ParseFileL
// ---------------------------------------------------------
//
//PseudoCode:
// 0010 locates the next access point within the text file
// 0020 parses the location and reads the input data to TagContainer
// 0030 from TagContainer creates Access Point Item Object
// 0040 using the Access Point Item Object creates a new AP record in CommsDat
// 0050 clears TagContainer
//
void CProcessorBase::ParseFileL()
    {
    //LocateAP : locates the access points within the text file
    while ( iFileReader->LocateAPL() )
        {
        //ParseAPL : parses the location, reads the input data to TagContainer
        iFileReader->ParseAPL();
        
        //from TagContainer creates Access Point Item Object
        ProcessTagL( iFileReader->FieldIDPresent() );
                
        //adds the Access Point Item Object to CommsDB
        ProcessAPL();
            
        //clear TagContainer
        TagContainer().Clear();
            
        }//while    
        
    CLOG_WRITE( "---------------------\n" )
    }

// ---------------------------------------------------------
// CProcessorPacketData::ProcessTagL
// ---------------------------------------------------------
//
void CProcessorBase::ProcessTagL( TBool /*aFieldIDPresent*/ )
    {
    iProxyEnabled= EFalse;
    iIsProtected = EFalse;

    Reset();
    TInt aCMId = -1;
    for ( TInt idx(0); idx < TagContainer().Count(); idx++)
        {
        if ( TagContainer().FieldId( idx ) == ECmId)
            {
            HBufC16* ptrTag = TagContainer().Tag(idx);
            TLex16 lex(ptrTag->Des());
            TInt err = lex.Val(aCMId);
            if (err != KErrNone)
                {
                User::Leave(err);    
                }
            break;
            }
        }
    if (aCMId == -1)
        {
        iPlugin = iCmManager->CreateConnectionMethodL( iBearerType );        
        CLOG_WRITE( "ConnectionMethod created without ECmId.");
        }
    else
        {
        iPlugin = iCmManager->CreateConnectionMethodL( iBearerType, aCMId);        
        CLOG_WRITE_FORMAT( "ConnectionMethod created. ECmId:%d", aCMId);
        }
     
    for ( TInt idx(0); idx < TagContainer().Count(); idx++)
        {
        HBufC16* ptrTag = TagContainer().Tag(idx);
        TInt field =  TagContainer().FieldId( idx );

        TBool processed = SetGeneralFieldL( field, ptrTag );
        if( !processed )
            {
            ProcessSpecialFieldsL( field, ptrTag, idx );
            }
        }
        
    AddToPluginArrayL();
    
    }

// --------------------------------------------------------
// CProcessorBase::TagContainer()
// --------------------------------------------------------
CTagContainer& CProcessorBase::TagContainer()
    {
    return iFileReader->TagContainer();    
    }


// --------------------------------------------------------
// CProcessorBase::AddToPluginArrayL()
// --------------------------------------------------------
void CProcessorBase::AddToPluginArrayL()
    {
    if( !iName )
        {
        CLOG_WRITE( "Error: No plugin name !!!" )
        }    
    else
        {
        const RCmConnectionMethodExt* newItem = new (ELeave) 
                RCmConnectionMethodExt( iPlugin );
    
        iPluginArray->Append( newItem );
        iPluginName->Append( iName->AllocL() );
        
        delete iName;
        iName = NULL;
        
        iPlugin.Close();
        }
    }

// --------------------------------------------------------
// CProcessorBase::GetLastPluginL()
// --------------------------------------------------------
RCmConnectionMethodExt& CProcessorBase::GetLastPluginL()
    {
    return GetPluginL( PluginCount() - 1 );
                
    }    

// --------------------------------------------------------
// CProcessorBase::GetPluginL()
// --------------------------------------------------------
RCmConnectionMethodExt& CProcessorBase::GetPluginL( TInt aPos )
    {

    return *(iPluginArray->operator[]( aPos ));
                
    }    

// --------------------------------------------------------
// CProcessorBase::PluginCount()
// --------------------------------------------------------
TInt CProcessorBase::PluginCount()
    {

    return iPluginArray->Count();
                
    }    

// ---------------------------------------------------------
// CProcessorBase::FindDestinationL
// ---------------------------------------------------------
//
RCmDestinationExt* CProcessorBase::FindDestinationL( HBufC* aName )
    {
    for( TInt i(0); i < iDestName->Count(); i++ )
        {
        HBufC *name = (*iDestName)[i];
        if( aName->Compare( *name  ) == 0 )
            {
            return (*iDestArray)[ i ];
            }
        }
    CLOG_WRITE_FORMAT("Warning: Destination not found: %S", aName );
    return NULL;
    }

// ---------------------------------------------------------
// CProcessorBase::GetDestinationIdL
// ---------------------------------------------------------
//
TInt CProcessorBase::GetDestinationIdL( HBufC16* aName )
    {
    RCmDestinationExt* dest = FindDestinationL( aName );
    
    TInt ret( KErrNotFound );
    if( dest != NULL )
        {
        ret = dest->Id();
        }
        
    return ret;    
    }

// ---------------------------------------------------------
// CProcessorBase::FindPluginL
// ---------------------------------------------------------
// 
RCmConnectionMethodExt* CProcessorBase::FindPluginL( HBufC* aName )
    {
    TInt pos;
    return FindPluginL( aName, pos );
    } 
 
// ---------------------------------------------------------
// CProcessorBase::FindPluginL
// ---------------------------------------------------------
//
RCmConnectionMethodExt* CProcessorBase::FindPluginL( HBufC* aName, TInt& aPos )
    {
    RCmConnectionMethodExt* ret( NULL );
    
    aPos = CheckPluginName( aName );
    if ( aPos != KErrNotFound )
        {
        ret = (*iPluginArray)[ aPos ];
        }
    else
        {
        CLOG_WRITE_FORMAT("Warning: Connection method not found: %S", aName );
        }

    return ret;
    }

// ---------------------------------------------------------
// CProcessorBase::CheckPluginName
// ---------------------------------------------------------
//
TInt CProcessorBase::CheckPluginName( HBufC* aName )
    {
    TInt ret( KErrNotFound );
    
    for( TInt i = 0; i < iPluginName->Count() && ret == KErrNotFound; i++ )
        {
        HBufC *name = (*iPluginName)[i];
        if( aName->Compare( *name  ) == 0 )
            {
            ret = i;
            }
        }
    return ret;
    }



// ---------------------------------------------------------
// CProcessorBase::GetPluginIdL
// ---------------------------------------------------------
//
TInt CProcessorBase::GetPluginIdL( HBufC16* aName )
    {
    RCmConnectionMethodExt* cm = FindPluginL( aName );
    
    TInt ret( KErrNotFound );
    if( cm != NULL )
        {
        ret = cm->GetIntAttributeL( CMManager::ECmIapId );
        }
        
    return ret;    
    }

    
// ---------------------------------------------------------
// CProcessorBase::ProcessAPL
// ---------------------------------------------------------
//
void CProcessorBase::ProcessAPL()
    {
    RCmConnectionMethodExt& plugin = GetLastPluginL();
    if( iIsProtected )
        {
        plugin.SetBoolAttributeL( ECmProtected, ETrue );
        }
        
    // Writes into the log file.
    DoLogL();
    }
    
// ---------------------------------------------------------
// CProcessorBase::ProcessAPL
// ---------------------------------------------------------
//
TBool CProcessorBase::SetGeneralFieldL( TInt aField, HBufC* aPtrTag )
    {
    TBool processed( ETrue );
    switch( aField )
        {
        case ECmName:     
            {
            // If input is not empty
            if ( aPtrTag->CompareF( KStrEmpty ) != 0 )
                {
                TInt pos = CheckPluginName( aPtrTag );
                if( pos != KErrNotFound )
                    {
                    CLOG_WRITE_FORMAT("Warning: plugin exist. It might be renamed ! : %S", aPtrTag );
                    }
                iPlugin.SetStringAttributeL( aField, *aPtrTag );
                if( !iName )
                    {
                    iName = aPtrTag->AllocL();
                    }
                }
            break;
            }
        case ECmStartPage:
            {
            // If input is not empty
            if ( aPtrTag->CompareF( KStrEmpty ) != 0 )
                {
                iPlugin.SetStringAttributeL( aField, *aPtrTag );
                }
            break;
            }
        case ECmHidden:              //fall through
        case ECmMetaHiddenAgent:     //fall through
        case ECmMetaHighlight:       //fall through
            {
            if( aPtrTag->CompareF( KStrNo ) == 0 )
                {
                iPlugin.SetBoolAttributeL( aField, EFalse );
                }
            else if( aPtrTag->CompareF( KStrYes ) == 0 )
                {
                iPlugin.SetBoolAttributeL( aField, ETrue );
                }
            else
                {
                iPlugin.SetBoolAttributeL( aField, EFalse );
                switch ( aField )
                    {
                    case ECmHidden:
                        CLOG_WRITE( "! Error : Invalid Hidden. Default:No");
                        break;
                    case ECmMetaHiddenAgent:
                        CLOG_WRITE( "! Error : Invalid HiddenAgent. Default:No");
                        break;
                    case ECmMetaHighlight:
                        CLOG_WRITE( "! Error : Invalid Highlight. Default:No");
                        break;
                    }
                }
            break;
            }
        case ECmSeamlessnessLevel:
            {
            if ( aPtrTag->CompareF( KStrNotRoamedTo ) == 0 )
                {
                iPlugin.SetIntAttributeL( aField,
                                        CMManager::ESeamlessnessNotRoamedTo );
                }
            else if ( aPtrTag->CompareF( KStrConfirmFirst ) == 0 )
                {
                iPlugin.SetIntAttributeL( aField,
                                        CMManager::ESeamlessnessConfirmFirst );
                }
            else if ( aPtrTag->CompareF( KStrShowProgress ) == 0 )
                {
                iPlugin.SetIntAttributeL( aField,
                                        CMManager::ESeamlessnessShowprogress );
                }
            else if ( aPtrTag->CompareF( KStrFullySeamless ) == 0 )
                {
                iPlugin.SetIntAttributeL( aField,
                                        CMManager::ESeamlessnessFullySeamless );
                }
            break;
            }
            
        case ECmProtected:
            {
            // Protection
            if ( aPtrTag->CompareF( KStrYes ) == 0 )
                {
                iIsProtected = ETrue;
                }
            else if ( aPtrTag->CompareF( KStrNo ) != 0 )
                {
                CLOG_WRITE( "! Error : Invalid Protected value. Default:No" )
                }
            break;
            }
            
        default:
            {
            processed = EFalse;
            }        
        }
        
    return processed;
    }

// ---------------------------------------------------------
// CProcessorBase::DoLogL
// ---------------------------------------------------------
//

void CProcessorBase::DoLogL()
    {
    
    TInt lastPos = iPluginArray->Count() - 1;
    
    CLOG_WRITE( "Internet Access Point created:" )
    CLOG_WRITE_FORMAT( "\t Connection name: %S ", (*iPluginName)[ lastPos ] );
    
    if ( iIsProtected )
        {
        CLOG_WRITE( "\t Protected" )
        }
    else
        {
        CLOG_WRITE( "\t Not protected" )
        }
    }
// ---------------------------------------------------------
// CProcessorBase::SetAddressFromServerL
// ---------------------------------------------------------
//

void CProcessorBase::SetAddressFromServerL( TInt aIPField1, 
                                           TInt aIPField2, 
                                           TInt aAddrFromSField,
                                           TInt aIPField1IP6,
                                           TInt aIPField2IP6,
                                           TInt aAddrFromSFieldIP6 )
    {
        //calculate and set Ip6AddrFromServer
        HBufC *pIp1 = NULL;        
        HBufC *pIp2 = NULL;        
        pIp1 = iPlugin.GetStringAttributeL( aIPField1IP6 );
        CleanupStack::PushL( pIp1 );
        pIp2 = iPlugin.GetStringAttributeL( aIPField2IP6 );
        CleanupStack::PushL( pIp2 );
        _LIT( KDynIpv6Address, "0:0:0:0:0:0:0:0" );
        if ( ( pIp1->Compare( KDynIpv6Address ) == 0 ) &&
             ( pIp2->Compare( KDynIpv6Address ) == 0 ) )
            {
            iPlugin.SetBoolAttributeL( aAddrFromSFieldIP6, ETrue );
            }
        else
            {
            iPlugin.SetBoolAttributeL( aAddrFromSFieldIP6, EFalse );
            }
        CleanupStack::PopAndDestroy( 2,pIp1 );

        //calculate and set AddrFromServer
        pIp1 = iPlugin.GetStringAttributeL( aIPField1 );
        CleanupStack::PushL( pIp1 );
        pIp2 = iPlugin.GetStringAttributeL( aIPField2 );
        CleanupStack::PushL( pIp2 );    
        _LIT( KDynIpAddress, "0.0.0.0" );
        if ( ( pIp1->Compare( KDynIpAddress ) == 0 ) &&
             ( pIp2->Compare( KDynIpAddress ) == 0 ) )
            {
            iPlugin.SetBoolAttributeL( aAddrFromSField, ETrue );
            }
        else
            {
            iPlugin.SetBoolAttributeL( aAddrFromSField, EFalse );
            }
        CleanupStack::PopAndDestroy( 2,pIp1 );        
    }

// End of File.
