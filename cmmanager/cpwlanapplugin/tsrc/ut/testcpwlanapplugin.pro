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
# Project file for Control Panel WLAN AP plugin unit tests.

TEMPLATE = app

TARGET = testcpwlanapplugin

CONFIG += hb qtestlib

INCLUDEPATH += . ../../inc
DEPENDPATH += .

HEADERS += \
    hbautotest.h \
    testcpwlanapplugin.h
SOURCES += \
    hbautotest.cpp \
    testcpwlanapplugin.cpp

LIBS += \
    -lcpwlanapplugin \
    -lconnection_settings_shim

symbian: {
    TARGET.CAPABILITY = ALL -TCB
}
