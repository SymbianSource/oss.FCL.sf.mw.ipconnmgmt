/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
* 
* Description:
*
*/

// INCLUDE FILES
#include <StifTestInterface.h>
#include "TestFlexTimer.h"
#include "SettingServerClient.h"
#include "testflexperiodic.h"
#include "testcflextimer.h"
#include "testrflextimer.h"
#include "rflextimerservermonitor.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestFlexTimer::CTestFlexTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTestFlexTimer::CTestFlexTimer()
    {

    }

// -----------------------------------------------------------------------------
// CTestFlexTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// Note: If OOM test case uses STIF Logger, then STIF Logger must be created
// with static buffer size parameter (aStaticBufferSize). Otherwise Logger 
// allocates memory from heap and therefore causes error situations with OOM 
// testing. For more information about STIF Logger construction, see STIF Users 
// Guide.
// -----------------------------------------------------------------------------
//
void CTestFlexTimer::ConstructL()
    {
    //Read logger settings to check whether test case name is to be
    //appended to log file name.
    RSettingServer settingServer;
    TInt ret = settingServer.Connect();
    if(ret != KErrNone)
        {
        User::Leave(ret);
        }
    // Struct to StifLogger settigs.
    TLoggerSettings loggerSettings; 
    // Parse StifLogger defaults from STIF initialization file.
    ret = settingServer.GetLoggerSettings(loggerSettings);
    if(ret != KErrNone)
        {
        User::Leave(ret);
        } 
    // Close Setting server session
    settingServer.Close();
    iAddTestCaseTitleToLogName = loggerSettings.iAddTestCaseTitle;

    iStdLog = CStifLogger::NewL( KTestFlexTimerLogPath, 
                                 KTestFlexTimerLogFile);
    iLog = iStdLog;
    
    iVersionLogged = EFalse;
    }

// -----------------------------------------------------------------------------
// CTestFlexTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTestFlexTimer* CTestFlexTimer::NewL()
    {
    CTestFlexTimer* self = new (ELeave) CTestFlexTimer;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CTestFlexTimer::~CTestFlexTimer()
    { 
    iLog = NULL;
    delete iStdLog;
    iStdLog = NULL;
    delete iTCLog;
    iTCLog = NULL;
    }

// -----------------------------------------------------------------------------
// CTestFlexTimer::InitL
// InitL is used to initialize the Test Module.
// -----------------------------------------------------------------------------
//
TInt CTestFlexTimer::InitL( 
    TFileName& /*aIniFile*/, 
    TBool /*aFirstTime*/ )
    {
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CTestFlexTimer::CreateTitleLoggerL
// Check for settings and create logger with test case title in file name.
// -----------------------------------------------------------------------------
//
void CTestFlexTimer::CreateTitleLoggerL(void)
    {
    //Open new log file with test case title in file name
    if(iAddTestCaseTitleToLogName)
        {
        // Check if there is no test case logger already created.
        if(iTCLog) 
            { 
            delete iTCLog; 
            iTCLog = NULL; 
            } 
            
        TFileName logFileName;
        TName title;
        TestModuleIf().GetTestCaseTitleL(title);
        
        logFileName.Format(KTestFlexTimerLogFileWithTitle, &title);

        iTCLog = CStifLogger::NewL(KTestFlexTimerLogPath, 
                                 logFileName);
        iLog = iTCLog; 
        }
    }

// -----------------------------------------------------------------------------
// CTestFlexTimer::CreateStdLoggerL
// Check for logger settings and create standard logger.
// -----------------------------------------------------------------------------
//
void CTestFlexTimer::DeleteTitleLogger(void)
    {
    //Delete title logger
    if(iAddTestCaseTitleToLogName)
        {
        iLog = iStdLog;
        delete iTCLog;
        iTCLog = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CTestFlexTimer::GetTestCasesL
// GetTestCases is used to inquire test cases from the Test Module. Test
// cases are stored to array of test cases. The Test Framework will be 
// the owner of the data in the RPointerArray after GetTestCases return
// and it does the memory deallocation. 
// -----------------------------------------------------------------------------
//
TInt CTestFlexTimer::GetTestCasesL( 
    const TFileName& /*aConfig*/, 
    RPointerArray<TTestCaseInfo>& aTestCases )
    {
    // Loop through all test cases and create new
    // TTestCaseInfo items and append items to aTestCase array    
    for( TInt i = 0; Case(i).iMethod != NULL; i++ )
        {
        // Allocate new TTestCaseInfo from heap for a testcase definition.
        TTestCaseInfo* newCase = new( ELeave ) TTestCaseInfo();
    
        // PushL TTestCaseInfo to CleanupStack.    
        CleanupStack::PushL( newCase );

        // Set number for the testcase.
        // When the testcase is run, this comes as a parameter to RunTestCaseL.
        newCase->iCaseNumber = i;

        // Set title for the test case. This is shown in UI to user.
        newCase->iTitle.Copy( Case(i).iCaseName );

        // Append TTestCaseInfo to the testcase array. After appended 
        // successfully the TTestCaseInfo object is owned (and freed) 
        // by the TestServer. 
        User::LeaveIfError(aTestCases.Append ( newCase ) );
        
        // Pop TTestCaseInfo from the CleanupStack.
        CleanupStack::Pop( newCase );
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestFlexTimer::RunTestCaseL
// RunTestCaseL is used to run an individual test case specified 
// by aTestCase. Test cases that can be run may be requested from 
// Test Module by GetTestCases method before calling RunTestCase.
// -----------------------------------------------------------------------------
//
TInt CTestFlexTimer::RunTestCaseL(
    const TInt aCaseNumber,
    const TFileName& /*aConfig*/,
    TTestResult& aResult )
    {
    SendTestModuleVersion();

    TInt execStatus = KErrNone;

    // Get the pointer to test case function
    TCaseInfo tmp = Case( aCaseNumber );

    _LIT( KLogInfo, "Starting testcase [%S]" );
    iLog->Log( KLogInfo, &tmp.iCaseName);

    // Check that case number was valid
    if ( tmp.iMethod != NULL )
        {
        //Open new log file with test case title in file name
        CreateTitleLoggerL();
    
        // Valid case was found, call it via function pointer
        iMethod = tmp.iMethod;        

        // Create and install an active scheduler for timers
        CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
        CleanupStack::PushL( scheduler );
        CActiveScheduler::Install( scheduler );
        //execStatus  = ( this->*iMethod )( aResult );
        TRAPD(err, execStatus  = ( *iMethod )( aResult, this ));

        CleanupStack::PopAndDestroy( scheduler );

        // Test case was executed
        DeleteTitleLogger();

        User::LeaveIfError(err);
        }
    else
        {
        // Valid case was not found, return error.
        execStatus = KErrNotFound;
        }

    // Return case execution status (not the result of the case execution)
    return execStatus;
    }

// -----------------------------------------------------------------------------
// CTestFlexTimer::OOMTestQueryL
// Used to check if a particular test case should be run in OOM conditions and 
// which memory allocations should fail.    
//
// NOTE: This method is virtual and must be implemented only if test case
// should be executed using OOM conditions.  
// -----------------------------------------------------------------------------
//
TBool CTestFlexTimer::OOMTestQueryL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt /* aCaseNumber */, 
                                TOOMFailureType& /* aFailureType */, 
                                TInt& /* aFirstMemFailure */, 
                                TInt& /* aLastMemFailure */ ) 
    {
    _LIT( KOOMTestQueryL, "CTestFlexTimer::OOMTestQueryL" );
    iLog->Log( KOOMTestQueryL ); 

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CTestFlexTimer::OOMTestInitializeL
// Used to perform the test environment setup for a particular OOM test case. 
// Test Modules may use the initialization file to read parameters for Test 
// Module initialization but they can also have their own configure file or 
// some other routine to initialize themselves.  
//
// NOTE: This method is virtual and must be implemented only if test case
// should be executed using OOM conditions.  
// -----------------------------------------------------------------------------
//
void CTestFlexTimer::OOMTestInitializeL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt /* aCaseNumber */ )
    {
    }

// -----------------------------------------------------------------------------
// CTestFlexTimer::OOMHandleWarningL
// In some cases the heap memory allocation should be skipped, either due to
// problems in the OS code or components used by the code being tested, or even 
// inside the tested components which are implemented this way on purpose (by 
// design), so it is important to give the tester a way to bypass allocation 
// failures.
//
// NOTE: This method is virtual and must be implemented only if test case
// should be executed using OOM conditions.  
// -----------------------------------------------------------------------------
//
void CTestFlexTimer::OOMHandleWarningL( 
                                const TFileName& /* aTestCaseFile */,
                                const TInt /* aCaseNumber */, 
                                TInt& /* aFailNextValue */ )
    {
    }

// -----------------------------------------------------------------------------
// CTestFlexTimer::OOMTestFinalizeL
// Used to perform the test environment cleanup for a particular OOM test case.
//
// NOTE: This method is virtual and must be implemented only if test case
// should be executed using OOM conditions.  
// -----------------------------------------------------------------------------
//                  
void CTestFlexTimer::OOMTestFinalizeL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt /* aCaseNumber */ )
    {
    }

//-----------------------------------------------------------------------------
// CTemplateScriptTestFlexTimer::SendTestModuleVersion
// Method used to send version of test module
//-----------------------------------------------------------------------------
//
void CTestFlexTimer::SendTestModuleVersion()
	{
	if(iVersionLogged)
		{
		return;
		}
	
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_MODULE_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_MODULE_VERSION_MINOR;
	moduleVersion.iBuild = TEST_MODULE_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("TestFlexTimer.dll");

	TBool newVersionOfMethod = ETrue;
	TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName, newVersionOfMethod);
	iVersionLogged = ETrue;
	}

// ---------------------------------------------------------------------------
// This function contains an array of all available test cases 
// i.e pair of case name and test function. If case specified by parameter
// aCaseNumber is found from array, then that item is returned.
//
// The reason for this rather complicated function is to specify all the
// test cases only in one place. It is not necessary to understand how
// function pointers to class member functions works when adding new test
// cases. See function body for instructions how to add new test case.
// ---------------------------------------------------------------------------
//
const TCaseInfo CTestFlexTimer::Case( const TInt aCaseNumber ) const 
    {
    static TCaseInfoInternal const KCases[] =
        {
        // To add new test cases, add new items to this array
        
        // NOTE: When compiled to GCCE, there must be Classname::
        // declaration in front of the method name, e.g. 
        // CDemoModule::PrintTest. Otherwise the compiler
        // gives errors.

        // Start FlexTimer server monitoring
        ENTRY( "FlexTimerServer: Start crash monitoring", CTestFlexTimer::StartServerMonitoringL ),
	    ENTRY( "FlexTimerClient: Change Priority: MuchMore", CTestFlexTimer::SetThreadPriorityMuchMore ),
        ENTRY( "FlexTimerClient: Change Priority: Normal", CTestFlexTimer::SetThreadPriorityNormal ),

        // CFlexPeriodic tests
        ENTRY( "CFlexPeriodic: Start one timer", CTestFlexPeriodic::StartOneTimerL ),
        ENTRY( "CFlexPeriodic: Start one timer with error CB", CTestFlexPeriodic::StartOneTimerWithErrorCbL ),
        ENTRY( "CFlexPeriodic: Restart timer in callback", CTestFlexPeriodic::CallbackRestartL ),
        ENTRY( "CFlexPeriodic: 32bit configure", CTestFlexPeriodic::ConfigureWindow32L ),
        ENTRY( "CFlexPeriodic: 64bit configure", CTestFlexPeriodic::ConfigureWindow64L ),
        ENTRY( "CFlexPeriodic: Mixed bit configure", CTestFlexPeriodic::ConfigureWindowMixL ),
        ENTRY( "CFlexPeriodic: Configure after start", CTestFlexPeriodic::ConfigureAfterStartL ),
        ENTRY( "CFlexPeriodic: Start timer twice", CTestFlexPeriodic::StartAfterStartL ),
        ENTRY( "CFlexPeriodic: Start timer in callback", CTestFlexPeriodic::StartInCallbackL ),
        ENTRY( "CFlexPeriodic: Start timer with negative delay (32 bit)", CTestFlexPeriodic::StartWithNegativeDelay32L ),
        ENTRY( "CFlexPeriodic: Start timer with zero interval (32 bit)", CTestFlexPeriodic::StartWithZeroInterval32L ),
        ENTRY( "CFlexPeriodic: Start timer with negative interval (32 bit)", CTestFlexPeriodic::StartWithNegativeInterval32L ),
        ENTRY( "CFlexPeriodic: Start timer with negative delay (64 bit)", CTestFlexPeriodic::StartWithNegativeDelay64L ),
        ENTRY( "CFlexPeriodic: Start timer with zero interval (64 bit)", CTestFlexPeriodic::StartWithZeroInterval64L ),
        ENTRY( "CFlexPeriodic: Start timer with negative interval (64 bit)", CTestFlexPeriodic::StartWithNegativeInterval64L ),
        ENTRY( "CFlexPeriodic: Configure timer with negative delay window (32 bit)", CTestFlexPeriodic::ConfigureWithNegativeDelayWindow32L ),
        ENTRY( "CFlexPeriodic: Configure timer with negative interval window (32 bit)", CTestFlexPeriodic::ConfigureWithNegativeIntervalWindow32L ),
        ENTRY( "CFlexPeriodic: Configure timer with negative delay window (64 bit)", CTestFlexPeriodic::ConfigureWithNegativeDelayWindow64L ),
        ENTRY( "CFlexPeriodic: Configure timer with negative interval window (64 bit)", CTestFlexPeriodic::ConfigureWithNegativeIntervalWindow64L ),
        ENTRY( "CFlexPeriodic: Start timer with min and max values", CTestFlexPeriodic::StartWithMinAndMaxL ),
        ENTRY( "CFlexPeriodic: Start timer with maximum delay (64 bit)", CTestFlexPeriodic::StartWithMaximumDelay64L ),
        ENTRY( "CFlexPeriodic: Start timer with maximum interval (64 bit)", CTestFlexPeriodic::StartWithMaximumInterval64L ),
        ENTRY( "CFlexPeriodic: Start timer NULL callback function (32 bit)", CTestFlexPeriodic::StartWithNullCallBack32L ),
        ENTRY( "CFlexPeriodic: Start timer NULL callback function (64 bit)", CTestFlexPeriodic::StartWithNullCallBack64L ),
        ENTRY( "CFlexPeriodic: Client crash", CTestFlexPeriodic::ClientCrashL ),
        //CFlexTimer tests
        ENTRY( "CFlexTimer: Create and destroy a timer - After", CTestCFlexTimer::CreateAndDestroyCFlexTimerL ),
        ENTRY( "CFlexTimer: Create and expire a timer - After", CTestCFlexTimer::CreateAndExpireCFlexTimerL ),
        ENTRY( "CFlexTimer: Create two timers and expire those - overlapping - After 32bit", CTestCFlexTimer::CreateAndExpireTwoOverlappingAfterTimersL ),
        ENTRY( "CFlexTimer: Create two timers and expire those - overlapping - After 64bit", CTestCFlexTimer::CreateAndExpireTwoOverlappingAfterTimers64L ),
        ENTRY( "CFlexTimer: Create two timers and expire those - non overlapping - After", CTestCFlexTimer::CreateAndExpireTwoNonOverlappingAfterTimersL ),
        ENTRY( "CFlexTimer: Create zillion timers and expire them at rapid sequence", CTestCFlexTimer::HumongousAmountOfTimeridelidilidousL ),
        ENTRY( "CFlexTimer: Create overlapping After and At timers and non-overlapping AtUTC", CTestCFlexTimer::CreateAndExpireThreeTimersL ),
        ENTRY( "CFlexTimer: Start and cancel timers", CTestCFlexTimer::NormalCancelL ),
        ENTRY( "CFlexTimer: Cancel timer that would have triggered another timer", CTestCFlexTimer::CancelTimerThatWouldHaveTriggeredAnotherTimerL ),
        ENTRY( "CFlexTimer: Configure 50 pct and 10 pct windows - overlap - 1", CTestCFlexTimer::ConfigureCTimerOverlap1L ),
        ENTRY( "CFlexTimer: Configure 50 pct and 10 pct windows - overlap - 2", CTestCFlexTimer::ConfigureCTimerOverlap2L ),
        ENTRY( "CFlexTimer: Configure 0 pct and 100 pct windows - overlap - 3", CTestCFlexTimer::ConfigureCTimerOverlap3L ),
        ENTRY( "CFlexTimer: Configure 0 pct and 10 pct windows - non-overlap - 1", CTestCFlexTimer::ConfigureCTimerNonOverlap1L ),
        ENTRY( "CFlexTimer: Configure 50 pct and 50 pct windows - non-overlap - 2", CTestCFlexTimer::ConfigureCTimerNonOverlap2L ),     
        ENTRY( "CFlexTimer: System Time Changes", CTestCFlexTimer::SystemTimeChangesL ),
        ENTRY( "CFlexTimer: Time zone changes", CTestCFlexTimer::TimeZoneChangesL ),
        ENTRY( "CFlexTimer: Dumbass parameters for CFlexTimer", CTestCFlexTimer::DumbAssParametersForCTimerL ),
        ENTRY( "CFlexTimer: 32bit After called with zero interval", CTestCFlexTimer::DumbAssParametersForCTimerZero32bitL ),
        ENTRY( "CFlexTimer: 64bit After called with zero interval", CTestCFlexTimer::DumbAssParametersForCTimerZero64bitL ),
        ENTRY( "CFlexTimer: 64bit After called with max interval", CTestCFlexTimer::DumbAssParametersForCTimer64MaxL ),
        ENTRY( "CFlexTimer: 64bit After called with three year interval", CTestCFlexTimer::DumbAssParametersForCTimer64ThreeYearsL ),
        ENTRY( "CFlexTimer: 64bit After called with one year interval", CTestCFlexTimer::DumbAssParametersForCTimer64OneYearL ),
        ENTRY( "CFlexTimer: After (32bit) called with negative value", CTestCFlexTimer::AfterWithNegativeValueL ),
        ENTRY( "CFlexTimer: At with current time", CTestCFlexTimer::AtWithCurrentTimeL ),
        ENTRY( "CFlexTimer: AtUTC with current time", CTestCFlexTimer::AtUTCWithCurrentTimeL ),
        ENTRY( "CFlexTimer: At with three year timer", CTestCFlexTimer::AtWithThreeYearsL ),
        ENTRY( "CFlexTimer: At with one year timer", CTestCFlexTimer::AtWithOneYearL ),
        ENTRY( "CFlexTimer: At() with time in the past", CTestCFlexTimer::AtWithTimeInThePastL ),
        ENTRY( "CFlexTimer: Cancel timer that is already expired", CTestCFlexTimer::CancelExpiredTimerL ),
        ENTRY( "CFlexTimer: Left side of the window is in the past", CTestCFlexTimer::ConfigureLeftSideInThePastL ),
        ENTRY( "CFlexTimer: Left side of the window is in the past", CTestCFlexTimer::ConfigureLeftSideInThePastWindowCheckL ),
        ENTRY( "CFlexTimer: Configure(32bit) called with negative window size", CTestCFlexTimer::ConfigureWithNegativeWindowSize32L ),
        ENTRY( "CFlexTimer: Configure(64bit) called with negative window size", CTestCFlexTimer::ConfigureWithNegativeWindowSize64L ),
        ENTRY( "CFlexTimer: Configure(32bit) called with max positive value", CTestCFlexTimer::ConfigureWithMaxWindow32L ),
        ENTRY( "CFlexTimer: Configure(64bit) called with three year value", CTestCFlexTimer::ConfigureWithThreeYearWindow64L ),
        ENTRY( "CFlexTimer: Configure(64bit) called with max positive value", CTestCFlexTimer::ConfigureWithMaxWindow64L ),
        ENTRY( "CFlexTimer: Configure called while timer is already in use", CTestCFlexTimer::ConfigureWhileTimerIsInUseL ),
        ENTRY( "CFlexTimer: Timer is started twise", CTestCFlexTimer::TimerIsStartedTwiceL ),
        ENTRY( "CFlexTimer: Two timers, while high CPU load.", CTestCFlexTimer::CpuLoadCreateAndExpireTwoOverlappingAfterTimersL ),
        ENTRY( "CFlexTimer: Many timers, while high CPU load.", CTestCFlexTimer::CpuLoadManyTimersL ),
        // RFlexTimer tests
        ENTRY( "RFlexTimer: Start timer with After (32 bit)", CTestRFlexTimer::ExpireAfter32L ),
        ENTRY( "RFlexTimer: Start timer with After (64 bit)", CTestRFlexTimer::ExpireAfter64L ),
        ENTRY( "RFlexTimer: Start timer with AfterTicks", CTestRFlexTimer::ExpireAfterTicksL ),
        ENTRY( "RFlexTimer: Start timer with At", CTestRFlexTimer::ExpireAtL ),
        ENTRY( "RFlexTimer: Start timer with AtUTC", CTestRFlexTimer::ExpireAtUtcL ),
        ENTRY( "RFlexTimer: Call After (32 bit) before Connect", CTestRFlexTimer::After32WithoutConnect ),
        ENTRY( "RFlexTimer: Call After (64 bit) before Connect", CTestRFlexTimer::After64WithoutConnect ),
        ENTRY( "RFlexTimer: Call AfterTicks before Connect", CTestRFlexTimer::AfterTicksWithoutConnect ),
        ENTRY( "RFlexTimer: Call At before Connect", CTestRFlexTimer::AtWithoutConnect ),
        ENTRY( "RFlexTimer: Call AtUTC before Connect", CTestRFlexTimer::AtUtcWithoutConnect ),
        ENTRY( "RFlexTimer: Call Configure (32 bit) before Connect", CTestRFlexTimer::Configure32WithoutConnect ),
        ENTRY( "RFlexTimer: Call Configure (64 bit) before Connect", CTestRFlexTimer::Configure64WithoutConnect ),
        ENTRY( "RFlexTimer: Call Cancel before Connect", CTestRFlexTimer::CancelWithoutConnect ),
        ENTRY( "RFlexTimer: Call After twice", CTestRFlexTimer::CallAfterTwiceL ),
        ENTRY( "RFlexTimer: Call AfterTicks twice", CTestRFlexTimer::CallAfterTicksTwiceL ),
        ENTRY( "RFlexTimer: Call At twice", CTestRFlexTimer::CallAtTwiceL ),
        ENTRY( "RFlexTimer: Call AtUTC twice", CTestRFlexTimer::CallAtUtcTwiceL ),
        ENTRY( "RFlexTimer: Call Connect twice", CTestRFlexTimer::CallConnectTwiceL ),
        ENTRY( "RFlexTimer: Reconnect to the server", CTestRFlexTimer::ReconnectL ),
        ENTRY( "RFlexTimer: Cancel before start", CTestRFlexTimer::CancelWithoutStart ),
        ENTRY( "RFlexTimer: Call AfterTicks with negative ticks", CTestRFlexTimer::NegativeTicksInAfterTicksL ),
        ENTRY( "RFlexTimer: Call AfterTicks with zero ticks", CTestRFlexTimer::ZeroTicksInAfterTicksL ),
        ENTRY( "RFlexTimer: Change time while AfterTicks running", CTestRFlexTimer::ChangeTimeWhileAfterTicksL ),
        ENTRY( "RFlexTimer: Test AfterTicks windows", CTestRFlexTimer::TestAfterTicksWindowL ),
        // High priority client tests
        ENTRY( "FlexTimerClient: Change Priority: MuchMore", CTestFlexTimer::SetThreadPriorityMuchMore ),
        ENTRY( "CFlexTimer: Two timers, while high CPU load.", CTestCFlexTimer::CpuLoadCreateAndExpireTwoOverlappingAfterTimersL ),
        ENTRY( "CFlexTimer: Many timers, while high CPU load.", CTestCFlexTimer::CpuLoadManyTimersL ),
        ENTRY( "FlexTimerClient: Change Priority: Normal", CTestFlexTimer::SetThreadPriorityNormal ),
        // FlexTimerServer tests
        ENTRY( "FlexTimerServer: Stop crash monitoring", CTestFlexTimer::StopServerMonitoringL ),

        ENTRY( "FlexTimerServer: Start second server", CTestFlexTimer::StartSecondServerL ),
        };
    
    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) / 
                               sizeof( TCaseInfoInternal ) )
        {

        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"" );
        null.iMethod = NULL;
        null.iIsOOMTest = EFalse;
        null.iFirstMemoryAllocation = 0;
        null.iLastMemoryAllocation = 0;
        return null;

        } 

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
    return tmp;
    }

// ========== Server level test cases ==========

// ---------------------------------------------------------------------------
// TEST CASE: Start monitoring the FlexTimer server
// ---------------------------------------------------------------------------
TInt CTestFlexTimer::StartServerMonitoringL(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    RFlexTimerServerMonitor monitor;
    User::LeaveIfError( monitor.Connect() );
    User::LeaveIfError( monitor.StartMonitoring() );
    monitor.Close();

    aResult.SetResult( KErrNone, _L("Test case passed") );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Stop monitoring the FlexTimer server and check has the server
// crashded.
// ---------------------------------------------------------------------------
TInt CTestFlexTimer::StopServerMonitoringL(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );
    
    RFlexTimerServerMonitor monitor;
    User::LeaveIfError( monitor.Connect() );
    
    TBool hasServerCrashed; 
    User::LeaveIfError( monitor.GetStatus( hasServerCrashed ) );
    
    User::LeaveIfError( monitor.StopMonitoring() );
    monitor.Close();

    // By default everything has gone ok 
    aResult.SetResult( KErrNone, _L("Test case passed") );
    if ( hasServerCrashed )
        {
        aResult.SetResult( 
            KErrGeneral,
            _L("Test case failed. FlexTimerServer has crashed.") );
        }
    
    return KErrNone;        
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start second FlexTimerServer. Main purpose of this test case is 
// to increase test coverage.
// ---------------------------------------------------------------------------
TInt CTestFlexTimer::StartSecondServerL(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    __UHEAP_MARK;

    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    // Make sure that the server is up'n'running
    RFlexTimer timer;
    timer.Connect();
    timer.Close();
    
    // Open handle to the server process
    RProcess flexTimerServer;
    User::LeaveIfError( flexTimerServer.Create( 
            _L("FlexTimerServer.exe"), 
            _L(""), 
            EOwnerThread ) );

    // Kick the server runnin'
    flexTimerServer.Resume();

    // Wait until the server process has been started.
    TRequestStatus status;
    flexTimerServer.Rendezvous( status );
    User::WaitForRequest( status );

    // Kill the server - if it has been able to been created
    flexTimerServer.Kill( 0 );
    
    // Clean up
    flexTimerServer.Close();

    aResult.SetResult( KErrNone, _L("Test case passed") );
    
    __UHEAP_MARKEND;

    return KErrNone;
    }
// ---------------------------------------------------------------------------
// TEST CASE: This test case can be used to change client priority to 
// EPriorityMuchMore.
// ---------------------------------------------------------------------------
TInt CTestFlexTimer::SetThreadPriorityMuchMore(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    RThread thisThread;
    
    thisThread.SetPriority( EPriorityMuchMore );

    aResult.SetResult( KErrNone, _L("Test case passed") );
    return KErrNone;
    }
// ---------------------------------------------------------------------------
// TEST CASE: This test case can be used to change client priority to 
// EPriorityMuchMore.
// ---------------------------------------------------------------------------
TInt CTestFlexTimer::SetThreadPriorityNormal(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    RThread thisThread;
    
    thisThread.SetPriority( EPriorityNormal );

    aResult.SetResult( KErrNone, _L("Test case passed") );
    return KErrNone;
    }
// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point
// Returns: CTestModuleBase*: Pointer to Test Module object
// -----------------------------------------------------------------------------
//
EXPORT_C CTestModuleBase* LibEntryL()
    {
    return CTestFlexTimer::NewL();
    }

// -----------------------------------------------------------------------------
// SetRequirements handles test module parameters(implements evolution
// version 1 for test module's heap and stack sizes configuring).
// Returns: TInt: Symbian error code.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SetRequirements( CTestModuleParam*& /*aTestModuleParam*/, 
                                TUint32& /*aParameterValid*/ )
    {

    /* --------------------------------- NOTE ---------------------------------
    USER PANICS occurs in test thread creation when:
    1) "The panic occurs when the value of the stack size is negative."
    2) "The panic occurs if the minimum heap size specified is less
       than KMinHeapSize".
       KMinHeapSize: "Functions that require a new heap to be allocated will
       either panic, or will reset the required heap size to this value if
       a smaller heap size is specified".
    3) "The panic occurs if the minimum heap size specified is greater than
       the maximum size to which the heap can grow".
    Other:
    1) Make sure that your hardware or Symbian OS is supporting given sizes.
       e.g. Hardware might support only sizes that are divisible by four.
    ------------------------------- NOTE end ------------------------------- */

    // Normally STIF uses default heap and stack sizes for test thread, see:
    // KTestThreadMinHeap, KTestThreadMinHeap and KStackSize.
    // If needed heap and stack sizes can be configured here by user. Remove
    // comments and define sizes.

/*
    aParameterValid = KStifTestModuleParameterChanged;

    CTestModuleParamVer01* param = CTestModuleParamVer01::NewL();
    // Stack size
    param->iTestThreadStackSize= 16384; // 16K stack
    // Heap sizes
    param->iTestThreadMinHeap = 4096;   // 4K heap min
    param->iTestThreadMaxHeap = 1048576;// 1M heap max

    aTestModuleParam = param;
*/
    return KErrNone;

    }

//  End of File
