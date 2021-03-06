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
# Qt project file for ipconnmgmt package.
#

TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS  += \
    ipcm_plat \
    connectionutilities \
    cmmanager \
    connectionmonitoring

symbian {
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include \"./group/bld.inf\""
}
