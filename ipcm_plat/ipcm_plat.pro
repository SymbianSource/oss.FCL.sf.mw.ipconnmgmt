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
# Project file for ipcm_plat.
#

TEMPLATE = subdirs
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>"

include(connection_settings_shim_api/connection_settings_shim_api.pri)
include(connection_settings_ui_api/connection_settings_ui_api.pri)
include(bearer_settings_ui_plugin_api/bearer_settings_ui_plugin_api.pri)
include(wlan_security_settings_ui_plugin_api/wlan_security_settings_ui_plugin_api.pri)

for(filename,PLATFORM_HEADERS){
    BLD_INF_RULES.prj_exports *= "$$filename MW_LAYER_PLATFORM_EXPORT_PATH($$basename(filename))"
  }
