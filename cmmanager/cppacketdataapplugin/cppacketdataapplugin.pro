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
# Project info file for Control Panel packet data AP plugin.
#

TEMPLATE = lib
TARGET = cppacketdataapplugin
DEPENDPATH += .
INCLUDEPATH += .

# Temporary solution to fix tracecompiler
# When tracecompiler is fixed, this can be removed
symbian: {
    MMP_RULES += "USERINCLUDE traces"
}

CONFIG += hb plugin

MOC_DIR = moc
OBJECTS_DIR = obj
RCC_DIR = rcc

LIBS += -lcpframework -lconnection_settings_shim

# Sources
HEADERS += inc/cppacketdataapplugin.h \
           inc/cppacketdataapview.h \
           inc/cppacketdataapadvancedview.h \
           traces/OstTraceDefinitions.h
SOURCES += src/cppacketdataapplugin.cpp \
           src/cppacketdataapview.cpp \
           src/cppacketdataapadvancedview.cpp

# Include plugin utilities
include(../cppluginutils/cppluginutils.pri)

TRANSLATIONS = cpapplugin.ts

symbian:
{
TARGET.EPOCALLOWDLLDATA = 1
TARGET.UID3 = 0x2002E694
TARGET.CAPABILITY = CAP_GENERAL_DLL

deploy.path = C:
qtplugins.path = /resource/qt/plugins/controlpanel/bearerap
qtplugins.sources += qmakepluginstubs/cppacketdataapplugin.qtplugin

for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin $$deploy.path$$qtplugins.path/$$basename(qtplugin)"

BLD_INF_RULES.prj_exports += "rom/cppacketdataapplugin.iby CORE_MW_LAYER_IBY_EXPORT_PATH(cppacketdataapplugin.iby)"
BLD_INF_RULES.prj_exports += "rom/cppacketdataapplugin_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(cppacketdataapplugin_resources.iby)"
}

plugin.sources = cppacketdataapplugin.dll
plugin.path = /resource/qt/plugins/controlpanel/bearerap
DEPLOYMENT += plugin
