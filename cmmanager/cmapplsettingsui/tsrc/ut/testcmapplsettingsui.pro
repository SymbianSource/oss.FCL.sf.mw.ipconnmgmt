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
# Project file for CM Application Settings UI Unit Tests.

TEMPLATE = app

TARGET = testcmapplsettingsui

CONFIG += hb qtestlib

INCLUDEPATH += . ../../inc
DEPENDPATH += .

HEADERS += \
    hbautotest.h \
    testcmapplsettingsui.h
SOURCES += \
    hbautotest.cpp \
    testcmapplsettingsui.cpp

LIBS += -lcmapplsettingsui

symbian: {
    TARGET.CAPABILITY = ALL -TCB
}
