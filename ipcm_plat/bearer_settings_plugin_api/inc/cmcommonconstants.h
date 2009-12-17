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
* Description:  Common constants, enumerations are declared here
*
*/

#ifndef CMCOMMONCONSTANTS_H
#define CMCOMMONCONSTANTS_H

// UID of general settings app, in which help texts are included
const TUid KHelpUidPlugin = { 0x100058EC };

// Array granularities
const TInt KCmArraySingularGranularity = 1;
const TInt KCmArraySmallGranularity     = 2;
const TInt KCmArrayMediumGranularity    = 5;
const TInt KCmArrayBigGranularity       = 10;
const TInt KCmMaxPasswordLength         = 50;

// For destination selection type dialogs
const TUint32 KDestMaxId = 0xff;
const TUint32 KDestItemNewConnection = 0x100;
const TUint32 KDestItemUncategorized = 0x101;
const TUint32 KDestItemEasyWlan      = 0x102;
const TUint32 KDestItemAlwaysAsk     = 0x103;
const TUint32 KDestItemAskOnce       = 0x104;
const TUint32 KDestItemDefaultConnection = 0x105;

const TInt KDestinationNameMaxLength = 30;
const TInt KConnMethodNameMaxLength = 30;

const TInt KDestReservedIconIndexNewConn = 0;
const TInt KDestReservedIconIndexProtected = 1;
const TInt KDestReservedIconIndexUncategorized = 2;
const TInt KDestReservedIconIndexDefaultConn = 3;
const TInt KDestReservedIconIndexCount = 4;//always modify if you add one

const TInt KCmReservedIconIndexProtected = 0;
const TInt KCmReservedIconIndexDefaultConn = 1;
const TInt KCmReservedIconIndexCount = 2;

_LIT( KManagerIconFilename, "z:cmmanager.mbm" );
_LIT( KACMManagerResDirAndFileName, "z:cmmanager.rsc" );

const TInt KDialogUserBack = 0;
const TInt KDialogUserExit = 1;
const TInt KDialogUserDelete = 2;
const TInt KDialogUserDeleteConfirmed = 3;

// ENUMERATIONS
enum TCmPanicCodes
    {
    ECmNullPointer,
    ECmWrongBearerType
    };

// UI constants
// 3 chars are reserved for the icon index, plus one for the '\t' char
const TInt KNumberOfStaticCharsIn1LineFormattedString = 4;
// 3 chars are reserved for the icon index, plus two for the '\t' chars
const TInt KNumberOfStaticCharsIn2LineFormattedString = 5;
_LIT( KSingleNumberAsString, "%d" );

const TUint32 KSettingsListBoxItemPrefix = '\t';

_LIT( KSettingsListBoxItemPostfix, "\t\t" );
_LIT( KTxtCompulsory, "\t*" );
_LIT( KCmNewLine, "\n" );


#endif // CMCOMMONCONSTANTS_H
