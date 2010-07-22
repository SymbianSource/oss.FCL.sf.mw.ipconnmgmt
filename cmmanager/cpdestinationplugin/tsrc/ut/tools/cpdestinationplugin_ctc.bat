::
:: Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
:: All rights reserved.
:: This component and the accompanying materials are made available
:: under the terms of "Eclipse Public License v1.0"
:: which accompanies this distribution, and is available
:: at the URL "http://www.eclipse.org/legal/epl-v10.html".
::
:: Initial Contributors:
:: Nokia Corporation - initial contribution.
::
:: Contributors:
::
:: Description:
:: Script running Control Panel Destination plugin test cases with CTC analysis.

@echo off

:: -----------------------------------------------------------------------------
:: Instructions
:: -----------------------------------------------------------------------------
:: 
:: Remember to use the test CommsDat that can be set and restored with the
:: helper scripts:
:: - commsdat_set.bat
:: - commsdat_restore.bat

call .\commsdat_set.bat

::-----------------------------------------------------------------------------
:: Set the local variables
::-----------------------------------------------------------------------------

:: Control Panel Destination plugin base directory
set cpdestinationplugin_dir=..\..\..
pushd %cpdestinationplugin_dir%

:: CTC instrumentation excludes
set ctc_excludes=-C EXCLUDE=* -C NO_EXCLUDE=src\*.cpp -C SKIP_FUNCTION_NAME=CpAddDestinationEntryItemData::createSettingView,CpDestinationEntryItemData::showErrorNote,CpIapItem::showErrorNote

::-----------------------------------------------------------------------------
:: Remove old coverage data
::-----------------------------------------------------------------------------

echo Cleaning up...
del MON.* ctcerr.txt profile.txt
rd /s /q CTCHTML
echo ...done

::-----------------------------------------------------------------------------
:: Build & instrument code
::-----------------------------------------------------------------------------

echo Building code and instrumenting..
call del MON.* ctcerr.txt profile.txt
call make distclean
call qmake
call ctcwrap -i d -2comp %ctc_excludes% sbs -c winscw_udeb
echo ...done

::-----------------------------------------------------------------------------
:: Build test code
::-----------------------------------------------------------------------------

echo Building test code...
call cd tsrc\ut
call make distclean
call qmake
call sbs -c winscw_udeb
echo ...done

::-----------------------------------------------------------------------------
:: Execute the tests
::-----------------------------------------------------------------------------

echo Running tests..
cd ..\..
call \epoc32\release\winscw\udeb\testcpdestinationplugin.exe
echo ...done

::-----------------------------------------------------------------------------
:: Create coverage results
::-----------------------------------------------------------------------------

echo Creating results...
call ctcpost -p profile.txt
call ctc2html -i profile.txt
echo ...done

call .\commsdat_restore.bat

popd
