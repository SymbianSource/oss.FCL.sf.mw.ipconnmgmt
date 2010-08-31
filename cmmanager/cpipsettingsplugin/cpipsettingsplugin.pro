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

TEMPLATE = lib
TARGET = cpipsettingsplugin
DEPENDPATH += .
INCLUDEPATH += . ../inc ../../inc

CONFIG += hb plugin

MOC_DIR = moc
OBJECTS_DIR = obj
RCC_DIR = rcc

LIBS += -lcpframework

# Sources
HEADERS += inc/cpipsettingsplugin.h \
           inc/cpipsettingsview.h
SOURCES += src/cpipsettingsplugin.cpp \
           src/cpipsettingsview.cpp

TRANSLATIONS = cpipsettingsplugin.ts

# Temporary solution to fix tracecompiler
# When tracecompiler is fixed, this can be removed
symbian: {

    MMP_RULES += "USERINCLUDE traces"

}

symbian:
{
TARGET.EPOCALLOWDLLDATA = 1
TARGET.UID3 = 0x2002BC8F
TARGET.CAPABILITY = CAP_GENERAL_DLL

deploy.path = C:
qtplugins.path = /resource/qt/plugins/controlpanel
qtplugins.sources += qmakepluginstubs/cpipsettingsplugin.qtplugin

for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin $$deploy.path$$qtplugins.path/$$basename(qtplugin)"

BLD_INF_RULES.prj_exports += "rom/cpipsettingsplugin.iby CORE_MW_LAYER_IBY_EXPORT_PATH(cpipsettingsplugin.iby)"
BLD_INF_RULES.prj_exports += "rom/cpipsettingsplugin_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(cpipsettingsplugin_resources.iby)"
}

plugin.sources = cpipsettingsplugin.dll
plugin.path = \resource\qt\plugins\controlpanel
DEPLOYMENT += plugin
