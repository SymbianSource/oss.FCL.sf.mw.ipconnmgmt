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

TEMPLATE = app
TARGET = indicatorobserver
CONFIG += hb no_icon mobility
MOBILITY += bearer

INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE \
               ./inc \
               ./traces
               
LIBS += -lxqsettingsmanager

QT += core \
      network 

QT -= gui

HEADERS += inc/indicatorobserver.h \
           traces/OstTraceDefinitions.h
    
SOURCES += src/indicatorobserver.cpp \
           src/main.cpp
    
symbian*: {
	TARGET.UID3 = 0x2002C36C
    TARGET.CAPABILITY = NetworkServices NetworkControl ReadUserData
    SYMBIAN_PLATFORMS = WINSCW ARMV5
    BLD_INF_RULES.prj_exports += "rom/indicatorobserver.iby CORE_MW_LAYER_IBY_EXPORT_PATH(indicatorobserver.iby)"
}