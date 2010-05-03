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
# Project info file for Control Panel WLAN AP plugin. 
#

TEMPLATE = lib
TARGET = cpwlanapplugin
DEPENDPATH += .
INCLUDEPATH += .

CONFIG += hb plugin

MOC_DIR = moc
OBJECTS_DIR = obj
RCC_DIR = rcc

LIBS += -lcpframework -lconnection_settings_shim

# Sources
HEADERS += inc/cpwlanapplugin.h \
           inc/cpwlanapview.h \
           inc/cpwlanapadvancedview.h \
           traces/OstTraceDefinitions.h
SOURCES += src/cpwlanapplugin.cpp \
           src/cpwlanapview.cpp \
           src/cpwlanapadvancedview.cpp

symbian:
{
TARGET.EPOCALLOWDLLDATA = 1
TARGET.UID3 = 0x2002C369
TARGET.CAPABILITY = CAP_GENERAL_DLL

deploy.path = C:
qtplugins.path = /resource/qt/plugins/controlpanel/bearerap
qtplugins.sources += qmakepluginstubs/cpwlanapplugin.qtplugin

for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin $$deploy.path$$qtplugins.path/$$basename(qtplugin)"

BLD_INF_RULES.prj_exports += "rom/cpwlanapplugin.iby CORE_MW_LAYER_IBY_EXPORT_PATH(cpwlanapplugin.iby)"
}

plugin.sources = cpwlanapplugin.dll
plugin.path = /resource/qt/plugins/controlpanel/bearerap
DEPLOYMENT += plugin
