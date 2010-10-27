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
* Description:  Declaration of the CVpnApEngine class.
*
*/


// INCLUDE FILES

#include    "VpnApEngine.h"
#include    "VpnApItem.h"
#include    "ApEngineLogger.h"
#include    "ApEngineConsts.h"
#include    "ApEngineCommons.h"

#include    <ApDataHandler.h>
#include    <ApAccessPointItem.h>
#include    <ApUtils.h>

#include    <txtetext.h>

#include    <vpnapi.h>
#include    <vpnapidefs.h>


// CONSTANTS
_LIT( KVpnVirtualBearerName, "vpnbearer" );
_LIT( KVpnVirtualBearerAgent, "vpnconnagt.agt" );
_LIT( KVpnVirtualBearerNif, "tunnelnif" );
_LIT( KDefProxyProtocolName, "http" );                      // required !
_LIT( KDefWapGatewayIpAddress, "0.0.0.0" );



// constatnts needed for the virtualbearer table
const TInt KNoTimeout = -1;
const TUint32 KLastSessionTimeout = 3;


// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CVpnApEngine::NewLC
// ---------------------------------------------------------
//
EXPORT_C CVpnApEngine* CVpnApEngine::NewLC( CCommsDatabase* aDb )
    {
    return NULL;
    }


// ---------------------------------------------------------
// CVpnApEngine::~CVpnApEngine
// ---------------------------------------------------------
//
EXPORT_C CVpnApEngine::~CVpnApEngine()
    {
    }

        
// ---------------------------------------------------------
// CVpnApEngine::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CVpnApEngine::ConstructL( CCommsDatabase* aDb )
    {
    }


// ---------------------------------------------------------
// CVpnApEngine::CVpnApEngine
// ---------------------------------------------------------
//
EXPORT_C CVpnApEngine::CVpnApEngine()
:iOwnDb( EFalse )
    {
    }



// ---------------------------------------------------------
// CVpnApEngine::CreateVpnApL
// ---------------------------------------------------------
//
EXPORT_C TUint32 CVpnApEngine::CreateVpnApL( CVpnApItem& aVpnApItem )
    {
    return 0;
    }





// ---------------------------------------------------------
// CVpnApEngine::CreateVpnL
// ---------------------------------------------------------
//
EXPORT_C TUint32 CVpnApEngine::CreateVpnApL(  TDesC& aName, 
                                              TUint32 aIapId, 
                                              TDesC& aPolicy, 
                                              TDesC* aProxyServerAddress, 
                                              TUint32 aProxyPort )
    {
    return 0;
    }




// ---------------------------------------------------------
// CVpnApEngine::DeleteVpnL
// ---------------------------------------------------------
//
EXPORT_C void CVpnApEngine::DeleteVpnL( TUint32 aVpnApId )
    {
    }






// ---------------------------------------------------------
// CVpnApEngine::VpnDataL
// ---------------------------------------------------------
//
EXPORT_C void CVpnApEngine::VpnDataL( TUint32 aUid, 
                                      CVpnApItem& aVpnApItem )
    {
    }




// ---------------------------------------------------------
// CVpnApEngine::UpdateVpnDataL
// ---------------------------------------------------------
//
EXPORT_C void CVpnApEngine::UpdateVpnDataL( CVpnApItem& aVpnApItem )
    {
    }




// ---------------------------------------------------------
// CVpnApEngine::IsVpnApL()
// ---------------------------------------------------------
//
EXPORT_C TBool CVpnApEngine::IsVpnApL( const TUint32 aUid )
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CVpnApEngine::Cvt()
// ---------------------------------------------------------
//
EXPORT_C TInt CVpnApEngine::Cvt()
    {
    return KErrNone;
    }




// ---------------------------------------------------------
// CVpnApEngine::DoCreateVpnApL
// ---------------------------------------------------------
//
TUint32 CVpnApEngine::DoCreateVpnApL( TBool aIsNew, CVpnApItem& aVpnApItem )
    {
    return 0;
    }


// ---------------------------------------------------------
// CVpnApEngine::WriteVpnDataL
// ---------------------------------------------------------
//
void CVpnApEngine::WriteVpnDataL( TBool aIsNew, CVpnApItem& aVpnApItem )
    {
    }




// ---------------------------------------------------------
// CVpnApEngine::CreateVirtBearerIfNeededL
// ---------------------------------------------------------
//
TUint32 CVpnApEngine::CreateVirtBearerIfNeededL()
    {
    return 0;
    }



// ---------------------------------------------------------
// CVpnApEngine::WriteVpnIapRecordL
// ---------------------------------------------------------
//
TUint32 CVpnApEngine::WriteVpnIapRecordL( TBool aIsNew, 
                                          CVpnApItem& aVpnApItem )
    {
    return 0;
    }




// ---------------------------------------------------------
// CVpnApEngine::GetLocationIdL
// ---------------------------------------------------------
//
TUint32 CVpnApEngine::GetLocationIdL()
    {
    return 0;    
    }



// ---------------------------------------------------------
// CVpnApEngine::WriteVpnWapApDataL
// ---------------------------------------------------------
//
void CVpnApEngine::WriteVpnWapApDataL( TBool aIsNew, CVpnApItem& aVpnApItem )
    {
    }



// ---------------------------------------------------------
// CVpnApEngine::WriteWapBearerDataL
// ---------------------------------------------------------
//
void CVpnApEngine::WriteVpnWapBearerDataL( TBool aIsNew, 
                                           CVpnApItem& aVpnApItem )
    {
    }




// ---------------------------------------------------------
// CVpnApEngine::ReadVpnWapBearerDataL
// ---------------------------------------------------------
//
void CVpnApEngine::ReadVpnWapBearerDataL( TUint32 aUid, 
                                          CVpnApItem& aVpnApItem )
    {
    }




// ---------------------------------------------------------
// CVpnApEngine::ReadVpnWapApDataL
// ---------------------------------------------------------
//
void CVpnApEngine::ReadVpnWapApDataL( CVpnApItem& aVpnApItem )
    {
    }



// ---------------------------------------------------------
// CVpnApEngine::ReadVpnIapRecordL
// ---------------------------------------------------------
//
void CVpnApEngine::ReadVpnIapRecordL( CVpnApItem& aVpnApItem )
    {
    }



// ---------------------------------------------------------
// CVpnApEngine::ReadVpnDataL
// ---------------------------------------------------------
//
void CVpnApEngine::ReadVpnDataL( CVpnApItem& aVpnApItem )
    {
    }











// ---------------------------------------------------------
// CVpnApEngine::WriteProxyDataL
// ---------------------------------------------------------
//
void CVpnApEngine::WriteProxyDataL( TBool aIsNew, 
                                      const CVpnApItem& aVpnApItem )
    {
    }



// ---------------------------------------------------------
// CVpnApEngine::ReadProxyDataL
// ---------------------------------------------------------
//
void CVpnApEngine::ReadProxyDataL( CVpnApItem& aVpnApItem )
    {
    }




// ---------------------------------------------------------
// CVpnApEngine::RemoveVpnProxyDataL
// ---------------------------------------------------------
//
void CVpnApEngine::RemoveVpnProxyDataL( TUint32 aIspId )
    {
    }



// ---------------------------------------------------------
// CVpnApEngine::DoDeleteVpnL
// ---------------------------------------------------------
//
void CVpnApEngine::DoDeleteVpnL( TUint32 aUid )
    {
    }



// ---------------------------------------------------------
// CVpnApEngine::RemoveRecordL
// ---------------------------------------------------------
//
void CVpnApEngine::RemoveRecordL( const TDesC& aTable, TUint32 aUid )
    {
    }





// ---------------------------------------------------------
// CVpnApEngine::WriteNetworkDataL
// ---------------------------------------------------------
//
void CVpnApEngine::WriteNetworkDataL( TBool aIsNew, 
                                        CVpnApItem& aVpnApItem )
    {
    }


// ---------------------------------------------------------
// CVpnApEngine::ReadNetworkDataL
// ---------------------------------------------------------
//
void CVpnApEngine::ReadNetworkDataL( CVpnApItem& aVpnApItem )
    {    
  }



// ---------------------------------------------------------
// CVpnApEngine::RemoveNetworkDataL
// ---------------------------------------------------------
//
void CVpnApEngine::RemoveNetworkDataL( TUint32 aIspId )
    {
    }

//  End of File

