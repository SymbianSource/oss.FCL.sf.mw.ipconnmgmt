/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: S60Mcpr's implementation of the TCP receive window interface.
 * Reads the receive window sizes from Central repository.
 *
 */

/**
 @file s60tcprecvwin.cpp
 S60 MCPR's TCP receive window interface.
 */

#include <e32base.h>
#include <es_sock.h>

#include "s60tcprecvwin.h"

#include <networking/cfbearers.h>
#include <centralrepository.h>
#include <comms-infras/metatype.h>

const TUint KDefaultTcpRecvWinGprs = 32768;
const TUint KDefaultTcpRecvWinEGprs = 32768;
const TUint KDefaultTcpRecvWinUmts = 32768;
const TUint KDefaultTcpRecvWinHsdpa = 131070;
const TUint KDefaultTcpRecvWinWlan = 98302;
const TUint KDefaultTcpRecvWinEthernet = 65535;

using namespace ESock;

// -----------------------------------------------------------------------------
// CS60TCPReceiveWindowSize::CS60TCPReceiveWindowSize
// -----------------------------------------------------------------------------
//
CS60TCPReceiveWindowSize::CS60TCPReceiveWindowSize() :
    CTCPReceiveWindowSize()
    {
    //  Populate TCP window lookup table.
    Init();
    }

// -----------------------------------------------------------------------------
// CS60TCPReceiveWindowSize::Init
// -----------------------------------------------------------------------------
//
void CS60TCPReceiveWindowSize::Init()
    {
    TInt err = KErrNone;
    CRepository* repository = NULL;

    // No need to use CleanupStack because no possibility to leave
    TRAP( err, repository = CRepository::NewL( KCRUIDTcpWindowSizes ) );

    if ( err != KErrNone )
        {
        //TCP receive window size for GPRS
        iBearerInfoMap.Insert( KGprsBearer, KDefaultTcpRecvWinGprs );
        //TCP receive window size for EGPRS
        iBearerInfoMap.Insert( KEGprsBearer, KDefaultTcpRecvWinEGprs );
        //TCP receive window size for UMTS
        iBearerInfoMap.Insert( KUmtsBearer, KDefaultTcpRecvWinUmts );
        //TCP receive window size for HSDPA
        iBearerInfoMap.Insert( KHsdpaBearer, KDefaultTcpRecvWinHsdpa );
        //TCP receive window size for WLAN
        iBearerInfoMap.Insert( KWlanBearer, KDefaultTcpRecvWinWlan );
        //TCP receive window size for ethernet
        iBearerInfoMap.Insert( KEthernetBearer, KDefaultTcpRecvWinEthernet );
        //TCP receive window size for other bearer
        iBearerInfoMap.Insert( KDefaultBearer, KBearerDefaultWinSize );
        }
    else
        {
        SetInfoMapValue( KGprsBearer, repository, KDefaultTcpRecvWinGprs );
        SetInfoMapValue( KEGprsBearer, repository, KDefaultTcpRecvWinEGprs );
        SetInfoMapValue( KUmtsBearer, repository, KDefaultTcpRecvWinUmts );
        SetInfoMapValue( KHsdpaBearer, repository, KDefaultTcpRecvWinHsdpa );
        SetInfoMapValue( KWlanBearer, repository, KDefaultTcpRecvWinWlan );
        SetInfoMapValue( KEthernetBearer, repository, KDefaultTcpRecvWinEthernet );
        SetInfoMapValue( KDefaultBearerKey, repository, KBearerDefaultWinSize );
        }
    delete repository;
    }

// -----------------------------------------------------------------------------
// CS60TCPReceiveWindowSize::SetInfoMapValue
// -----------------------------------------------------------------------------
//
void CS60TCPReceiveWindowSize::SetInfoMapValue( TUint aKey,
                                                CRepository* aRepository,
                                                TInt aDefaultWin )
    {
    // Set an information map value to an element found in
    // the central repository
    TInt err = KErrNone;
    TInt win_size = 0;

    err = aRepository->Get( aKey, win_size );
    if ( err != KErrNone || win_size <= 0 )
        {
        win_size = aDefaultWin;
        }
    iBearerInfoMap.Insert( aKey, win_size );
    }

// -----------------------------------------------------------------------------
// CS60TCPReceiveWindowSize::SetTcpWin
// -----------------------------------------------------------------------------
//
void CS60TCPReceiveWindowSize::SetTcpWin( TUint aBearerType )
    {
    //Set the TCP Receive Window.
    TUint* i;
    i = static_cast<TUint*> ( iBearerInfoMap.Find( aBearerType ) );
    if ( i )
        {
        iWinSize = *i;
        }
    else
        {
        iWinSize = KBearerDefaultWinSize;
        }
    //Set the Max TCP receive Window.
    SetMaxWinSize( aBearerType );
    }

// -----------------------------------------------------------------------------
// CS60TCPReceiveWindowSize::SetMaxWinSize
// -----------------------------------------------------------------------------
//
void CS60TCPReceiveWindowSize::SetMaxWinSize( TUint aBearerType )
    {
    switch ( aBearerType )
        {
        case KGprsBearer:
        case KEGprsBearer:
        case KUmtsBearer:
        case KHsdpaBearer:
            iMaxWinSize = *static_cast<TUint*> ( iBearerInfoMap.Find( KHsdpaBearer ) );
            break;
        case KWlanBearer:
            iMaxWinSize = *static_cast<TUint*> ( iBearerInfoMap.Find( KWlanBearer ) );
            break;
        case KEthernetBearer:
        case KDefaultBearer:
        default:
            iMaxWinSize = KBearerDefaultMaxWinSize;
            break;
        }
    }

// -----------------------------------------------------------------------------
// CS60TCPReceiveWindowSize::~CS60TCPReceiveWindowSize
// -----------------------------------------------------------------------------
//
CS60TCPReceiveWindowSize::~CS60TCPReceiveWindowSize()
    {
    iBearerInfoMap.Close();
    }

// Register the variables of TCP receive window class,
// since it derives from SMetaData class
START_ATTRIBUTE_TABLE(CTCPReceiveWindowSize,CTCPReceiveWindowSize::ERealmId, CTCPReceiveWindowSize::iId)
  REGISTER_ATTRIBUTE(CTCPReceiveWindowSize, iMaxWinSize, TMetaNumber)
  REGISTER_ATTRIBUTE(CTCPReceiveWindowSize, iWinSize, TMetaNumber)
END_ATTRIBUTE_TABLE()

