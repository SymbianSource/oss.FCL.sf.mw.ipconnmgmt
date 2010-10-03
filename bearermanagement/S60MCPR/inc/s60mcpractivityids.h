/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: S60 MCPR's activities.
*
*/

/**
@file s60mcpractivityids.h
S60 MCPR's activity id's.
*/

#ifndef S60MCPRACTIVITYIDS_H
#define S60MCPRACTIVITYIDS_H

enum TS60MCprActivities
    {
    ECFActivityS60McprServiceIdMessage2Handler = ESock::ECFActivityCustom + 10,
    ECFActivityS60McprStopIAP,
    ECFActivityS60McprDataClientIdle,
    ECFActivityS60DataClientStatusChangeConsumeActivity
    };

#endif //S60MCPRACTIVITYIDS_H
