#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
# Project file for Connection Settings Shim Unit Tests.

TEMPLATE = app

TARGET = testcmmgrshim

CONFIG += hb qtestlib

INCLUDEPATH += .
DEPENDPATH += .

HEADERS += \
    testcmmgrshim.h
SOURCES += \
    testcmmgrshim.cpp

LIBS += -lconnection_settings_shim

symbian: {
    SYMBIAN_PLATFORMS = WINSCW ARMV5
    TARGET.CAPABILITY = ALL -TCB
}
