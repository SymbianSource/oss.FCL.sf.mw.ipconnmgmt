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
#

TEMPLATE = app
TARGET = tst_applsettingsui
TARGET.CAPABILITY = ALL -TCB
DEPENDPATH += .
INCLUDEPATH += \
    . \
    inc \
    ../../inc \
    ../../../../inc
CONFIG += hb
RESOURCES = res/tst_applsettingsui.qrc
DOCML += res/tst_applsettingsui.docml
MOC_DIR = moc

# Input
HEADERS += \
    inc/applsettester.h
SOURCES += \
    src/applsettester.cpp \
    src/tst_applsettingsui.cpp

symbian*: {
    LIBS += -lcmapplsettingsui
    TARGET.UID3 = 0x2002E69A
}

symbian: {
    BLD_INF_RULES.prj_exports += "rom/tst_applsettingsui.iby CORE_MW_LAYER_IBY_EXPORT_PATH(tst_applsettingsui.iby)"
    BLD_INF_RULES.prj_exports += "rom/tst_applsettingsui_stub.sis /epoc32/data/z/system/install/tst_applsettingsui_stub.sis"
} 
