#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

TEMPLATE = lib
CONFIG += plugin
CONFIG += hb

# Directories
DEPENDPATH += .
INCLUDEPATH += .\inc

# Sources
HEADERS += inc\cellularindicatorplugin.h \
           traces\OstTraceDefinitions.h

SOURCES += src\cellularindicatorplugin.cpp

symbian*: {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID3 = 0x2002C3AB
    SYMBIAN_PLATFORMS = WINSCW ARMV5
    BLD_INF_RULES.prj_exports += "rom/cellularindicatorplugin.iby CORE_MW_LAYER_IBY_EXPORT_PATH(cellularindicatorplugin.iby)"
    BLD_INF_RULES.prj_exports += "rom/cellularindicatorplugin_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(cellularindicatorplugin_resources.iby)"
    pluginstub.sources = cellularindicatorplugin.dll
    pluginstub.path = /resource/plugins/indicators
    DEPLOYMENT += pluginstub
}

TRANSLATIONS = cellularindicatorplugin.ts

