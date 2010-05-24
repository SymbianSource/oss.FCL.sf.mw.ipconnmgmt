::---------------------------------------------------------------------------
:: Helper for FlexTimer building and testing
::
:: Commands:
::
::  BUILD_ALL       Build component to all targets
::  BUILD_ERROR     Inform user about failed build
::  BUILD_OK        Inform user about passed build
::  BUILD_TRACES    Build OST traces to targets WINSCW_UDEB and ARMV5_UDEB
::  CHECK_EPOCROOT  Check that epocroot is set correctly
::  CLEAN_NOLOGS    Reallyclean component, no output to screen
::  DEL_CTC_REPORTS Delete old CTC reports
::  GEN_CMT_REPORTS Generate CMT reports
::  GEN_COV_REPORTS Generate and deliver Coveruty Prevent reports
::  GEN_CS_REPORTS  Genrete CodeScanner reports
::  GEN_CTC_REPORTS Generate CTC reports
::  GEN_IMAGE       Make flash images
::  GEN_SYMBOLS     Generate symbol files
::  INSTRUMENT_CTC  Instrument CTC++ to component's WINSCW_UDEB target
::  RUN_TESTS       Compile test component, run test application and check 
::                  test results
::  SCHEDULER       Execute commands from QUEUE
::  SHOW_HELP       Show help of the script
::  START_UP        Enable delayed environment expansion
::  TEST_FAILED     Inform user about failed tests
::  THE_END         Stop execution of the script
::
::---------------------------------------------------------------------------
:: Change history:
::
:: 1.0  01-Apr-2010     First version in SVN
::
::---------------------------------------------------------------------------

@echo off
setlocal

set VERSION=1.0 [01/04/2010 15:08:42]

:: The source path is one dir up from the BAT's location
for /f %%i in ("%0") do set SOURCE_PATH=%%~dpi..

set MAKE_PATH=%SOURCE_PATH%\group\bld.inf
set TEST_MAKE_PATH=%SOURCE_PATH%\test\testflextimer\group\bld.inf

:: Commands for the component
set CMD_BUILD_ALL=sbs -b %MAKE_PATH%
set CMD_BUILD_CLEAN=sbs -b %MAKE_PATH% reallyclean
set CMD_BUILD_TRACE=sbs -b %MAKE_PATH% -c winscw_udeb.tracecompiler -c armv5_udeb.tracecompiler

:: Commands for test component
set CMD_TEST_BUILD=sbs -b %TEST_MAKE_PATH% -c winscw_udeb -c armv5
set CMD_TEST_RUN=start /wait %EPOCROOT%epoc32\release\winscw\udeb\atsinterface.exe -dtextshell -- -testmodule testflextimer

:: Commands and settings for CTC++
set CTC_OUTFILE=MON
set CTC_OUTPATH=.
set CMD_CTC_BUILD=ctcwrap -i m -v -2comp sbs.bat -b %MAKE_PATH% -c winscw_udeb
set CMD_CTC_CLEAN=sbs -b %MAKE_PATH% -c winscw_udeb reallyclean
set CMD_CTC_GEN=ctcpost %CTC_OUTPATH%\%CTC_OUTFILE%.sym %CTC_OUTPATH%\%CTC_OUTFILE%.dat ^| ctc2html

:: Command for CMT
set CMD_CMT_GEN=dir /s /b %SOURCE_PATH%\*.cpp %SOURCE_PATH%\*.inl ^| findstr /I /V "\\test\\" ^| cmt ^| cmt2html

:: Commands and settings for Coverity Prevent
set COV_OUTPATH=coverity
set CMD_COV_BUILD=cov-build --dir %COV_OUTPATH% sbs -b %MAKE_PATH%
set CMD_COV_ANALYZE=cov-analyze --dir %COV_OUTPATH% --all --symbian
set CMD_COV_DELIVER=cov-commit-defects --dir %COV_OUTPATH% --remote trwsim47 --user cadsw --password cov3rity --product flextimer --cva
set CMD_COV_SHOW_RESULTS=start http://trwsim47:5467

:: Commands for iMaker
set CMD_IMAKE_CORE=imaker vasco_ui core USE_SYMGEN=0
set CMD_IMAKE_ROFS2=imaker vasco_ui rofs2 USE_SYMGEN=0
set CMD_IMAKE_ROFS3=imaker vasco_ui rofs3 USE_SYMGEN=0
set CMD_IMAKE_UDAERASE=imaker vasco_ui udaerase USE_SYMGEN=0
set CMD_IMAKE_CORE_SYMBOLS=imaker vasco_ui romsymbol
set CMD_IMAKE_ROFS2_SYMBOLS=imaker vasco_ui rofs2-symbol
set CMD_IMAKE_ROFS3_SYMBOLS=imaker vasco_ui rofs3-symbol

:: Commands and settings for CodeScanner
set CODESCANNER_PATH=cs_output
set CMD_CODESCANNER=codescanner -t off

:: Python script for checking from test logs are tests passed
set PY_ALL_TESTS_PASSED=^
import re,sys;^
file = open('%EPOCROOT%/epoc32/winscw/c/logs/testframework/testreport.txt');^
txt = '\n'.join(file);^
reg1 = re.search('^\s*(?:Passed cases\: (\d+))$', txt, re.MULTILINE);^
reg2 = re.search('^\s*(?:Total cases\: (\d+))$', txt, re.MULTILINE);^
sys.exit(0 if reg1.group(1)==reg2.group(1) else 1)

:: Python script for parsing test result summary from test logs
set PY_PARSE_TEST_RESULTS=^
import re;^
file = open('%EPOCROOT%/epoc32/winscw/c/logs/testframework/testreport.txt');^
txt = '\n'.join(file);^
reg = re.findall('^\s*(.*cases\: \d+)$', txt, re.MULTILINE);^
print '\n'.join(reg);^
file.close()

::---------------------------------------------------------------------------
:: Parse input
::---------------------------------------------------------------------------

if not "%1"=="__INITIALIZED__" (
    goto START_UP

) else (

    if "%2"=="" (
        set QUEUE=CHECK_EPOCROOT CLEAN_NOLOGS BUILD_ALL BUILD_TRACES RUN_TESTS GEN_CMT_REPORTS GEN_CS_REPORTS
        goto SCHEDULER

    ) else if /i "%2"=="build" (
        set QUEUE=CHECK_EPOCROOT CLEAN_NOLOGS BUILD_ALL BUILD_TRACES
        goto SCHEDULER

    ) else if /i "%2"=="ctc" (
        rem - atsinterface does not work with CTC instrumentation, use consoleui instead
        rem - NOTE Emulator has to be closed manually
        set CMD_TEST_RUN=start /wait %EPOCROOT%epoc32\release\winscw\udeb\consoleui.exe -dtextshell -- -testmodule testflextimer -run all
        set QUEUE=CHECK_EPOCROOT INSTRUMENT_CTC DEL_CTC_REPORTS RUN_TESTS GEN_CTC_REPORTS
        goto SCHEDULER

    ) else if /i "%2"=="cov" (
        set QUEUE=CHECK_EPOCROOT CLEAN_NOLOGS GEN_COV_REPORTS
        goto SCHEDULER

    ) else if /i "%2"=="image" (
        set QUEUE=CHECK_EPOCROOT GEN_IMAGE GEN_SYMBOLS
        goto SCHEDULER

    ) else if /i "%2"=="test" (
        set QUEUE=CHECK_EPOCROOT GEN_CMT_REPORTS GEN_CS_REPORTS
        goto SCHEDULER

    ) else if /i "%2"=="help" goto SHOW_HELP
)

echo Illegal parameter %2

goto SHOW_HELP

::---------------------------------------------------------------------------
:START_UP
::---------------------------------------------------------------------------
:: Start a command shell with delayed environment expansion

cmd.exe /V:ON /C %0 __INITIALIZED__ %1

goto THE_END

::---------------------------------------------------------------------------
:SCHEDULER
::---------------------------------------------------------------------------
:: Run the build commands. Eat commands from QUEUE one by one

for /f "tokens=1*" %%i in ( "!QUEUE!" ) do (
    set QUEUE=%%j
    rem echo ====================================================================
    rem echo %%i
    rem echo ====================================================================
    goto %%i
)

goto BUILD_OK

::---------------------------------------------------------------------------
:BUILD_ALL
::---------------------------------------------------------------------------

call %CMD_BUILD_ALL%
if not "%errorlevel%"=="0" goto BUILD_ERROR

goto SCHEDULER

::---------------------------------------------------------------------------
:BUILD_TRACES
::---------------------------------------------------------------------------

call %CMD_BUILD_TRACE%
if not "%errorlevel%"=="0" goto BUILD_ERROR

goto SCHEDULER

::---------------------------------------------------------------------------
:CLEAN_NOLOGS
::---------------------------------------------------------------------------
:: Reallyclean FlexTimer component without showing compilation logs on screen

call %CMD_BUILD_CLEAN% > NUL 2>&1

goto SCHEDULER

::---------------------------------------------------------------------------
:INSTRUMENT_CTC
::---------------------------------------------------------------------------
:: Instrument FlexTimer WINSCW UDEB with CTC++

:: Remove old CTC symbols
del %CTC_OUTPATH%\%CTC_OUTFILE%.sym > NUL 2>&1

:: Clean old compilation
call %CMD_CTC_CLEAN% > NUL 2>&1

:: Instrument the code
call %CMD_CTC_BUILD%
if not "%errorlevel%"=="0" goto BUILD_ERROR

goto SCHEDULER

::---------------------------------------------------------------------------
:DEL_CTC_REPORTS
::---------------------------------------------------------------------------
:: Delete CTC generated test reports. SYM-file is deleted before 
:: instrumenting CTC.

del %CTC_OUTPATH%\%CTC_OUTFILE%.dat > NUL 2>&1
rmdir /q /s %CTC_OUTPATH%\CTCHTML > NUL 2>&1

goto SCHEDULER

::---------------------------------------------------------------------------
:RUN_TESTS
::---------------------------------------------------------------------------

:: Compile test component
call %CMD_TEST_BUILD%
if not "%errorlevel%"=="0" goto BUILD_ERROR

echo Testing...

:: Run test application
%CMD_TEST_RUN%
if not "%errorlevel%"=="0" goto TEST_FAILED

:: Verify test results
call python -c "%PY_ALL_TESTS_PASSED%"
if not "%errorlevel%"=="0" goto TEST_FAILED

goto SCHEDULER

::---------------------------------------------------------------------------
:GEN_CTC_REPORTS
::---------------------------------------------------------------------------

:: Generate CTC++ reports
call %CMD_CTC_GEN% 
if not "%errorlevel%"=="0" goto BUILD_ERROR

goto SCHEDULER

::---------------------------------------------------------------------------
:GEN_CMT_REPORTS
::---------------------------------------------------------------------------

call %CMD_CMT_GEN%

goto SCHEDULER

::---------------------------------------------------------------------------
:GEN_COV_REPORTS
::---------------------------------------------------------------------------

:: Delete the old results
rmdir /q /s %COV_OUTPATH% > NUL 2>&1

:: Build with Coverity Prevent
call %CMD_COV_BUILD%
if not "%errorlevel%"=="0" goto BUILD_ERROR

:: Analyze the code
call %CMD_COV_ANALYZE%
if not "%errorlevel%"=="0" goto BUILD_ERROR

:: Deliver the results
call %CMD_COV_DELIVER%
if not "%errorlevel%"=="0" goto BUILD_ERROR

:: Open the results
%CMD_COV_SHOW_RESULTS%

goto SCHEDULER

::---------------------------------------------------------------------------
:GEN_CS_REPORTS
::---------------------------------------------------------------------------

set __CODESCANNER_LAST_DIR=
set __CODESCANNER_PARAMETERS=

:: Find all inc and src -directories that are not in codescanner results nor
:: test code. Add '-i' parameter to every directory except the last one. (As
:: codescanner -h states.)

for /f "usebackq" %%i in (`dir /s /b %SOURCE_PATH%\src %SOURCE_PATH%\inc ^| findstr /V "\\test\\ \\%CODESCANNER_PATH%\\"`) do (
    if defined __CODESCANNER_LAST_DIR set __CODESCANNER_PARAMETERS=!__CODESCANNER_PARAMETERS! -i !__CODESCANNER_LAST_DIR!
    set __CODESCANNER_LAST_DIR=%%i
)

%CMD_CODESCANNER% %__CODESCANNER_PARAMETERS% %__CODESCANNER_LAST_DIR% %CODESCANNER_PATH%
if not "%errorlevel%"=="0" goto BUILD_ERROR

:: Open the results
start %CODESCANNER_PATH%\problemIndex.html

goto SCHEDULER

::---------------------------------------------------------------------------
:CHECK_EPOCROOT
::---------------------------------------------------------------------------

:: Check that the given parameter is a root dir.
for /f %%i in ("%EPOCROOT%") do (
    if /i not "%EPOCROOT%"=="%%~di\" (
        echo EPOCROOT is invalid: %EPOCROOT%
        echo It should be a root of certain drive letter e.g. T:\
        goto BUILD_ERROR
    )
)

goto SCHEDULER

::---------------------------------------------------------------------------
:GEN_IMAGE
::---------------------------------------------------------------------------

:: Go to the SDK drive
for /f %%i in ("%EPOCROOT%") do %%~di

:: Make image
call %CMD_IMAKE_CORE%
if not "%errorlevel%"=="0" goto BUILD_ERROR

call %CMD_IMAKE_ROFS2%
if not "%errorlevel%"=="0" goto BUILD_ERROR

call %CMD_IMAKE_ROFS3%
if not "%errorlevel%"=="0" goto BUILD_ERROR

call %CMD_IMAKE_UDAERASE%
if not "%errorlevel%"=="0" goto BUILD_ERROR

echo.
echo ======================================================================
echo IMAGES ARE READY
echo ======================================================================
echo.

goto SCHEDULER

::---------------------------------------------------------------------------
:GEN_SYMBOLS
::---------------------------------------------------------------------------

:: Go to the SDK drive
for /f %%i in ("%EPOCROOT%") do %%~di

:: Generate symbols
call %CMD_IMAKE_CORE_SYMBOLS%
if not "%errorlevel%"=="0" goto BUILD_ERROR

call %CMD_IMAKE_ROFS2_SYMBOLS%
if not "%errorlevel%"=="0" goto BUILD_ERROR

call %CMD_IMAKE_ROFS3_SYMBOLS%
if not "%errorlevel%"=="0" goto BUILD_ERROR

goto SCHEDULER

::---------------------------------------------------------------------------
:BUILD_ERROR
::---------------------------------------------------------------------------

echo.
echo Build failed
echo.

goto THE_END

::---------------------------------------------------------------------------
:BUILD_OK
::---------------------------------------------------------------------------

echo.
echo Build OK
echo.

goto THE_END

::---------------------------------------------------------------------------
:TEST_FAILED
::---------------------------------------------------------------------------

echo.
echo Testing failed
echo.

call python -c "%PY_PARSE_TEST_RESULTS%"

goto THE_END

::---------------------------------------------------------------------------
:SHOW_HELP
::---------------------------------------------------------------------------
echo.
echo A script for building and testing FlexTimer. Version %VERSION%
echo.
echo Usage:
echo    flextimer_build ^<option^>
echo.
echo Options:
echo.
echo    ^<none^>  Build component (with traces), build test component and run
echo            tests, generate static analysis
echo.
echo    build   Rebuild component (with traces)
echo.
echo    cov     Generate and deliver Coverity Prevent report
echo.
echo    ctc     Instrument CTC++, build test component and run tests, 
echo            generate test coverity reports
echo.
echo    image   Create CORE and ROFS1, ROFS2, ROFS3 and UDAERASE flash images
echo.
echo    help    Show this help
echo.

::---------------------------------------------------------------------------
:THE_END
::---------------------------------------------------------------------------
endlocal
