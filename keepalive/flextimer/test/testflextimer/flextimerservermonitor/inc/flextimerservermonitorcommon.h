/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:
 *      Common definitions for FlexTimerServerMonitor client/server
 *      communication.
 */
/*
 * %version: 1 %
 */
#ifndef FLEXTIMERSERVERMONITORCOMMON_H
#define FLEXTIMERSERVERMONITORCOMMON_H

/**
 * Function codes used in message passing between client and server.
 */
enum TFlexTimerServerMonitorRequests
    {
    EFlexTimerServerMonitorStartMonitoring = 1, ///< Start monitoring
    EFlexTimerServerMonitorHasServerCrashed,    ///< Has monitored server crashed
    EFlexTimerServerMonitorStopMonitoring       ///< Stop monitoring
    };

/**
 * Panic category for FlexTimerServerMonitor server panics
 */
_LIT( KFlexTimerServerMonitorPanicCategory, "TestFlexTimer-Monitor" );

/**
 * Panic codes for panic category TestFlexTimer-Monitor
 */
enum TFlexTimerServerMonitorPanicCode
    {
    EActiveSchedulerCreationFailed = 1,
    EServerCreationFailed,
    EStartingServerFailed,
    ECleanupStackCreationFailed
    };


const TUint KFlexTimerServerMonitorMajorVersionNumber = 0; ///< Major version
const TUint KFlexTimerServerMonitorMinorVersionNumber = 1; ///< Minor version
const TUint KFlexTimerServerMonitorBuildVersionNumber = 1; ///< Build version

/**
 * Name of the FlexTimer server monitor process
 */
_LIT( KFlexTimerServerMonitorServerName, "FlexTimerServerMonitorServer" );

/**
 * Name of the FlexTimer server monitor process executable
 */
_LIT( KFlexTimerServerMonitorServerExe, "flextimerservermonitorserver.exe" );

/**
 * Number of the message slots for FlexTimer server monitor process.
 * 
 * Due there are no asynchronous messaging, the number of slots can be one.
 */
const TInt KFlexTimerServerMonitorServerMessageSlots = 1;

#endif // FLEXTIMERSERVERMONITORCOMMON_H
