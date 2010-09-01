/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Commsdatcreator related central repository keys
*
*/



#ifndef __CDC_COMMSDAT_CREATOR_CR_KEYS_H__
#define __CDC_COMMSDAT_CREATOR_CR_KEYS_H__

/** CommsDatCreator API */
/** Provides access CDCCommsDatCreator Startup Status CenRep key*/

const TUid KCRUidCommsDatCreator= {0x10281BC8};


/** 
* Defines the status of the CommsDat creation. 
*
* Integer type
*
* Default value: 0
*/
const TUint32 KCommsDatCreatorStartupStatus = 0x00000001;


/** 
* Defines the input file name for CommsDat creation. 
*
* String type
*
* Default value: VariantData.xml
*/
const TUint32 KCommsDatCreatorInputFileName = 0x00000002;

/** 
* Possible statuses of the CommsDat creation. 
*
* enum type
*
* Default value: 0
*/

enum TCommsDatCreatorStartupStatus
	{
	ECommsDatNotInitialised,
	ECommsDatInitialised,
	ECommsDatInitialisationFailed
	};



#endif // __CDC_COMMSDAT_CREATOR_CR_KEYS_H__