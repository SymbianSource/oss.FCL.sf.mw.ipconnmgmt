#
# Copyright (c)  Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  icons makefile for project CMManager Framework
#
ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=\epoc32\data\z
endif


TARGETDIR=$(ZDIR)\resource\apps
ICONTARGETFILENAME=$(TARGETDIR)\cmmanager.mif

HEADERDIR=\epoc32\include
HEADERFILENAME=$(HEADERDIR)\cmmanager.mbg

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

# ----------------------------------------------------------------------------
# NOTE 1: DO NOT DEFINE MASK FILE NAMES! They are included automatically by
# MifConv if the mask detph is defined.
#
# NOTE 2: Usually, source paths should not be included in the bitmap
# definitions. MifConv searches for the icons in all icon directories in a
# predefined order, which is currently \s60\icons, \s60\bitmaps2, \s60\bitmaps.
# The directory \s60\icons is included in the search only if the feature flag
# __SCALABLE_ICONS is defined.
# ----------------------------------------------------------------------------

RESOURCE :
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME)  \
                /c8,8 qgn_indi_sett_protected_add.bmp\
                /c8,8 qgn_prop_set_conn_dest_add.bmp\
                /c8,8 qgn_prop_set_conn_dest_internet.bmp\
                /c8,8 qgn_prop_set_conn_dest_wap.bmp\
                /c8,8 qgn_prop_set_conn_dest_mms.bmp\
                /c8,8 qgn_prop_set_conn_dest_intranet.bmp\
                /c8,8 qgn_prop_set_conn_dest_operator.bmp\
                /c8,8 qgn_prop_set_conn_dest_icon1.bmp\
                /c8,8 qgn_prop_set_conn_dest_icon2.bmp\
                /c8,8 qgn_prop_set_conn_dest_icon3.bmp\
                /c8,8 qgn_prop_set_conn_dest_icon4.bmp\
                /c8,8 qgn_prop_set_conn_dest_icon5.bmp\
                /c8,8 qgn_prop_set_conn_dest_icon6.bmp\
                /c8,8 qgn_prop_set_conn_dest_default.bmp\
                /c8,8 qgn_prop_set_conn_dest_uncategorized.bmp\
                /c8,8 qgn_prop_set_conn_bearer_avail_wlan.bmp\
                /c8,8 qgn_prop_set_conn_bearer_avail_packetdata.bmp\
                /c8,8 qgn_prop_set_conn_wlan_easy.bmp\
                /c8,8 qgn_prop_set_conn_dest_internet_small.bmp\
                /c8,8 qgn_prop_set_conn_dest_wap_small.bmp\
                /c8,8 qgn_prop_set_conn_dest_mms_small.bmp\
                /c8,8 qgn_prop_set_conn_dest_intranet_small.bmp\
                /c8,8 qgn_prop_set_conn_dest_operator_small.bmp\
                /c8,8 qgn_prop_set_conn_dest_icon1_small.bmp\
                /c8,8 qgn_prop_set_conn_dest_icon2_small.bmp\
                /c8,8 qgn_prop_set_conn_dest_icon3_small.bmp\
                /c8,8 qgn_prop_set_conn_dest_icon4_small.bmp\
                /c8,8 qgn_prop_set_conn_dest_icon5_small.bmp\
                /c8,8 qgn_prop_set_conn_dest_icon6_small.bmp\
                /c8,8 qgn_prop_set_conn_dest_default_small.bmp\
                /c8,8 qgn_indi_cb_hot_add.bmp\
                /c8,8 qgn_indi_default_conn_add.bmp

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
