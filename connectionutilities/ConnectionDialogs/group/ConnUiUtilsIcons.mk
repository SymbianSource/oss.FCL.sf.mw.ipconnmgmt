#
# Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  Makefile for icons of ConnectionUiUtilities
#

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=\epoc32\data\z
endif

TARGETDIR=$(ZDIR)\resource\apps
HEADERDIR=\epoc32\include
ICONTARGETFILENAME=$(TARGETDIR)\connectionuiutilities.mif
HEADERFILENAME=$(HEADERDIR)\connectionuiutilities.mbg

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE :
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
		/c8,8 qgn_indi_wlan_signal_low_add.bmp \
		/c8,8 qgn_indi_wlan_signal_med_add.bmp \
		/c8,8 qgn_indi_wlan_signal_good_add.bmp \
		/c8,8 qgn_indi_wlan_secure_network_add.bmp \
		/c8,8 qgn_prop_snote_wlan_adhoc.bmp \
		/c8,8 qgn_prop_snote_wlan_infra.bmp \
		/c8,8 qgn_indi_wifi_protected_add.bmp

# THIS SHOULD BE qgn.indi.wlan.protected.setup.add, BUT IT IS STILL MISSING!!!


FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
