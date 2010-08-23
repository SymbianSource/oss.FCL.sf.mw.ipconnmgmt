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
# CM Manager Application Settings UI project file.
#

TEMPLATE = lib
TARGET = cmapplsettingsui
TARGET.CAPABILITY = ALL -TCB
DEFINES += BUILD_CMAPPLSETTINGSUI_DLL 
DEPENDPATH += .
INCLUDEPATH += inc

# Temporary solution to fix tracecompiler
# When tracecompiler is fixed, this can be removed
symbian: {
    MMP_RULES += "USERINCLUDE traces"
}

CONFIG += hb
RESOURCES = res/cmapplsettingsui.qrc

DOCML += res/cmradiodialog.docml

TRANSLATIONS = cmapplsettingsui.ts

#Store generated files to their own directory
MOC_DIR = build
RCC_DIR = build

HEADERS += \
    ../../ipcm_plat/connection_settings_ui_api/inc/cmapplsettingsui.h \
    inc/cmapplsettingsui_p.h \
    inc/cmradiodialog.h \
    inc/cmsettingdata.h \
    traces/OstTraceDefinitions.h
    
SOURCES += \
    src/cmapplsettingsui.cpp \
    src/cmapplsettingsui_p.cpp \
    src/cmsettingdata.cpp \
    src/cmradiodialog.cpp

# Temporary export directory definition
# This can be removed when the default has changed to "." from ".."
mmpRuleDeffile = \
    "$${LITERAL_HASH}ifdef WINSCW" \
    "DEFFILE ./bwins/cmapplsettingsui.def" \
    "$${LITERAL_HASH}else" \
    "DEFFILE ./eabi/cmapplsettingsui.def" \
    "$${LITERAL_HASH}endif"

symbian: {
    TARGET.UID3 = 0x2002DC89
    TARGET.EPOCALLOWDLLDATA = 1
    MMP_RULES += mmpRuleDeffile
    SYMBIAN_PLATFORMS = WINSCW ARMV5
    BLD_INF_RULES.prj_exports += "rom/cmapplsettingsui.iby CORE_MW_LAYER_IBY_EXPORT_PATH(cmapplsettingsui.iby)"
    BLD_INF_RULES.prj_exports += "rom/cmapplsettingsui_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(cmapplsettingsui_resources.iby)"
    LIBS += -lcmmanager -lconnection_settings_shim
}
