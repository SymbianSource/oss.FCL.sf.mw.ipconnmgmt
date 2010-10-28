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
# Description: Connection view build file
#
#
TEMPLATE = app
TARGET = connview

CONFIG += hb no_icon

QT += network

DEPENDPATH += . inc src
INCLUDEPATH += .

# Input
HEADERS += inc/connectionview.h \
           traces/OstTraceDefinitions.h \
           inc/scrollareawidget.h
SOURCES += src/connectionview.cpp \
           src/main.cpp \
           src/scrollareawidget.cpp

TRANSLATIONS = connectionview.ts

RESOURCES = res/connectionview.qrc

SYMBIAN_PLATFORMS = WINSCW ARMV5

symbian {
    TARGET.CAPABILITY = NetworkServices NetworkControl
    TARGET.UID3 = 0x2002C36A
    BLD_INF_RULES.prj_exports += "rom/connectionview.iby CORE_MW_LAYER_IBY_EXPORT_PATH(connectionview.iby)"
	BLD_INF_RULES.prj_exports += "rom/connectionview_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(connectionview_resources.iby)"
}