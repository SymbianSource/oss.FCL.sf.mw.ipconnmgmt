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
# Connection Settings Shim project file.
#

TEMPLATE = lib
TARGET = connection_settings_shim
TARGET.CAPABILITY = ALL -TCB

#BUILD_DLL macro is used to define export macro
DEFINES += BUILD_CONNECTIONSETTINGSSHIMDLL_DLL
DEPENDPATH += .

# Temporary solution to fix tracecompiler
# When tracecompiler is fixed, this can be removed
symbian: {
    MMP_RULES += "USERINCLUDE traces"
}

#Store generated files to their own directory
MOC_DIR = build
RCC_DIR = build

symbian*::LIBS += -lcmmanager

# Sources
HEADERS += \
    ../../ipcm_plat/connection_settings_shim_api/inc/cmconnectionmethod_shim.h \
    inc/cmconnectionmethod_shim_s60_p.h \
    ../../ipcm_plat/connection_settings_shim_api/inc/cmdestination_shim.h \
    inc/cmdestination_shim_s60_p.h \
    ../../ipcm_plat/connection_settings_shim_api/inc/cmmanager_shim.h \
    inc/cmmanager_shim_s60_p.h \
    ../../ipcm_plat/connection_settings_shim_api/inc/cmmanagerdefines_shim.h \
    traces/OstTraceDefinitions.h
    
SOURCES += \
    src/cmconnectionmethod_shim.cpp \
    src/cmconnectionmethod_shim_s60.cpp \
    src/cmdestination_shim.cpp \
    src/cmdestination_shim_s60.cpp \
    src/cmmanager_shim.cpp \
    src/cmmanager_shim_s60.cpp

# Temporary export directory definition
# This can be removed when the default has changed to "." from ".."
mmpRuleDeffile = \
    "$${LITERAL_HASH}ifdef WINSCW" \
    "DEFFILE ./bwins/connection_settings_shim.def" \
    "$${LITERAL_HASH}else" \
    "DEFFILE ./eabi/connection_settings_shim.def" \
    "$${LITERAL_HASH}endif"
 
symbian:
{ 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x2002E6D1
    MMP_RULES += mmpRuleDeffile
    BLD_INF_RULES.prj_exports += "rom/connection_settings_shim.iby CORE_MW_LAYER_IBY_EXPORT_PATH(connection_settings_shim.iby)"
}
