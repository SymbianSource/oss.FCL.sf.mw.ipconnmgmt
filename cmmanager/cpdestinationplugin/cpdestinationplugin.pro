# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
# Control Panel Destinations & Access points plugin project file

TEMPLATE = lib
TARGET = cpdestinationplugin
DEPENDPATH += .
INCLUDEPATH += . 
CONFIG += hb plugin
MOC_DIR = moc
OBJECTS_DIR = obj
RCC_DIR = rcc
LIBS += -lcpframework \
        -lconnection_settings_shim
symbian*::LIBS += -lcmmanager
RESOURCES = res/cpdestinationplugin.qrc

# Sources
HEADERS += inc/cpiapitem.h \
    inc/cpdestinationplugin.h \
    inc/cpdestinationgroup.h \
    inc/cpadddestinationentryitemdata.h \
    inc/cpdestinationentryitem.h \
    traces/OstTraceDefinitions.h
SOURCES += src/cpiapitem.cpp \
    src/cpadddestinationentryitemdata.cpp \
    src/cpdestinationentryitem.cpp \
    src/cpdestinationplugin.cpp \
    src/cpdestinationgroup.cpp

TRANSLATIONS = cpdestinationplugin.ts

symbian:
{ 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x2002BC90
    TARGET.CAPABILITY = CAP_GENERAL_DLL
     
    deploy.path = C:
    qtplugins.path = /resource/qt/plugins/controlpanel
    qtplugins.sources += qmakepluginstubs/cpdestinationplugin.qtplugin
    
    for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin $$deploy.path$$qtplugins.path/$$basename(qtplugin)"
    BLD_INF_RULES.prj_exports += "rom/cpdestinationplugin.iby CORE_MW_LAYER_IBY_EXPORT_PATH(cpdestinationplugin.iby)"
    BLD_INF_RULES.prj_exports += "rom/cpdestinationplugin_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(cpdestinationplugin_resources.iby)"
}

plugin.sources = cpdestinationplugin.dll
plugin.path = /resource/qt/plugins/controlpanel
DEPLOYMENT += plugin
