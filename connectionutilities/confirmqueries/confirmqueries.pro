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
# Description: Prompt Dialog build file
#
#

TEMPLATE = lib
TARGET = cellularpromptdialogplugin
CONFIG += hb plugin

# directories
INCLUDEPATH += .
DEPENDPATH += .
DESTDIR = $${HB_BUILD_DIR}/plugins/devicedialogs


HEADERS += inc/cellularpromptdialog.h  \
           inc/cellularpromptdialogplugin.h \
           traces/OstTraceDefinitions.h

SOURCES += src/cellularpromptdialog.cpp \
           src/cellularpromptdialogplugin.cpp

symbian: {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID3 = 0x2002C2F7
    BLD_INF_RULES.prj_exports += "rom/promptdialogplugin.iby CORE_APP_LAYER_IBY_EXPORT_PATH(promptdialogplugin.iby)"
    BLD_INF_RULES.prj_exports += "rom/promptdialogplugin_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(promptdialogplugin_resources.iby)"

    pluginstub.sources = cellularpromptdialogplugin.dll
    pluginstub.path = /resource/plugins/devicedialogs
    DEPLOYMENT += pluginstub
}
TRANSLATIONS = cellularpromptdialog.ts

RESOURCES += res/promptdialog.qrc

DOCML += \
    res/prompt_home.docml \
    res/prompt_abroad.docml