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
 @file s60tcprecvwin.h
 S60 MCPR's TCP receive window interface.
 */

#ifndef S60TCPRECVWIN_H
#define S60TCPRECVWIN_H

#include <tcprecvwin.h>
#include <e32hashtab.h> 


const TUid KCRUIDTcpWindowSizes =
    {
    0x101F6D4A
    };
const TUint KWlanBearer = 0x3C;
const TUint KDefaultBearerKey = 0xFF;

class CRepository;
/**
 * Default TCP receive window implementation. 
 * 
 */
class CS60TCPReceiveWindowSize : public CTCPReceiveWindowSize
    {

public:
    //Default constructor 
    CS60TCPReceiveWindowSize();

    //Destructor
    ~CS60TCPReceiveWindowSize();

    //Populates TCP window lookup table for different bearers
    void Init();

    //Set TCP window size
    void SetTcpWin( TUint aBearerType );

    //Set max TCP receive window for a network bearer
    void SetMaxWinSize( TUint aBearerType );

private:

    void SetInfoMapValue( TUint aKey,
                          CRepository* aRepository,
                          TInt aDefaultWin );

    //Associative array for storing TCP window size.
    RHashMap<TUint, TUint> iBearerInfoMap;

    };

#endif //S60TCPRECVWIN_H
