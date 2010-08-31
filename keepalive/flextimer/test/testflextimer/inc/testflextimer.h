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
* Description: STIF normal test module declaration
*
*/

#ifndef TESTFLEXTIMER_H
#define TESTFLEXTIMER_H

// INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>
#include <NormalHardcodedAssert.h>

// MACROS
#define TEST_MODULE_VERSION_MAJOR 0
#define TEST_MODULE_VERSION_MINOR 0
#define TEST_MODULE_VERSION_BUILD 0

// Logging path
_LIT( KTestFlexTimerLogPath, "\\logs\\testframework\\TestFlexTimer\\" ); 
// Log file
_LIT( KTestFlexTimerLogFile, "TestFlexTimer.txt" ); 
_LIT( KTestFlexTimerLogFileWithTitle, "TestFlexTimer_[%S].txt" );

_LIT( KTestFlexTimerPanicCategory, "FLEX-TEST" );

// Function pointer related internal definitions
#define GETPTR &  
#define ENTRY(str,func) {_S(str), GETPTR func,0,0,0}
#define FUNCENTRY(func) {_S(#func), GETPTR func,0,0,0}
#define OOM_ENTRY(str,func,a,b,c) {_S(str), GETPTR func,a,b,c}
#define OOM_FUNCENTRY(func,a,b,c) {_S(#func), GETPTR func,a,b,c}


// Forward declaration
class CTestFlexTimer;

// A typedef for function that does the actual testing,
// function is a type 
// TInt CDemoModule::<NameOfFunction> ( TTestResult& aResult, CTestFlexTimer *cb )
typedef TInt (*TestFunction)(TTestResult&, CTestFlexTimer*);    

const TInt KTimerAccuracyMicroSeconds = 1000 * 1000; //1s - accuracy of symbian At timer.
const double KDefaultWindowMultiplier = 0.2; //default window. defined in flextimersession.cpp 
const TUint KTickInMicroSeconds( 1000000 / 64 ); // One tick = 1/64 sec


// CLASS DECLARATION

/**
*  An internal structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib ?library
*  @since ?Series60_version
*/
class TCaseInfoInternal
    {
    public:
        const TText*    iCaseName;
        TestFunction    iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;
    };

// CLASS DECLARATION

/**
*  A structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib ?library
*  @since ?Series60_version
*/
class TCaseInfo
    {
    public:
        TPtrC           iCaseName;    
        TestFunction    iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;

    TCaseInfo( const TText* a ) : iCaseName( (TText*) a )
        {        
        };
    };

//Exteded result that can "return" also status code to CB function 
class TExtendedTestResult
    {
    public:
        TTestResult *iTestResult; //the real test result
        TInt iStatusCode; //Return code from timer
        TBool iCaseAlreadyFailed;
        TInt iTimerID;
        TInt64 iTimerStartedTime;
        TInt64 iTimerStoppedTime;
        TInt64 iTimerExpiryTime;

    TExtendedTestResult(TTestResult *aResult) : iTestResult( aResult ), iStatusCode(0), iCaseAlreadyFailed(EFalse), 
            iTimerID(0), iTimerStartedTime(0), iTimerStoppedTime(0) {};
    };
// CLASS DECLARATION

/**
*  This a TestFlexTimer class.
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CTestFlexTimer) : public CTestModuleBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestFlexTimer* NewL();

        /**
        * Destructor.
        */
        virtual ~CTestFlexTimer();
  
    public: // Functions from base classes

        
        /**
        * From CTestModuleBase InitL is used to initialize the TestFlexTimer. 
        *       It is called once for every instance of TestFlexTimer after
        *       its creation.
        * @since ?Series60_version
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first 
        *               created instance of TestFlexTimer.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases 
        *   from TestFlexTimer. 
        * @since ?Series60_version
        * @param aTestCaseFile Test case file (optional)
        * @param aTestCases  Array of TestCases returned to test framework
        * @return Symbian OS error code
        */
        TInt GetTestCasesL( const TFileName& aTestCaseFile, 
                            RPointerArray<TTestCaseInfo>& aTestCases );

        /**
        * From CTestModuleBase RunTestCaseL is used to run an individual 
        *   test case. 
        * @since ?Series60_version
        * @param aCaseNumber Test case number
        * @param aTestCaseFile Test case file (optional)
        * @param aResult Test case result returned to test framework (PASS/FAIL)
        * @return Symbian OS error code (test case execution error, which is 
        *           not reported in aResult parameter as test case failure).
        */   
        TInt RunTestCaseL( const TInt aCaseNumber, 
                           const TFileName& aTestCaseFile,
                           TTestResult& aResult );

        /**
        * From CTestModuleBase; OOMTestQueryL is used to specify is particular
        * test case going to be executed using OOM conditions
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailureType OOM failure type (optional)
        * @param aFirstMemFailure The first heap memory allocation failure value (optional)
        * @param aLastMemFailure The last heap memory allocation failure value (optional)
        * @return TBool
        */
        virtual TBool OOMTestQueryL( const TFileName& /* aTestCaseFile */, 
                                     const TInt /* aCaseNumber */, 
                                     TOOMFailureType& /* aFailureType */,
                                     TInt& /* aFirstMemFailure */, 
                                     TInt& /* aLastMemFailure */ );

        /**
        * From CTestModuleBase; User may add implementation for OOM test 
        * environment initialization. Usually no implementation is required.
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        */
        virtual void OOMTestInitializeL( const TFileName& /* aTestCaseFile */, 
                                    const TInt /* aCaseNumber */ ); 

        /**
        * From CTestModuleBase; OOMHandleWarningL. User may add implementation 
        * for OOM test warning handling. Usually no implementation is required.
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailNextValue FailNextValue for OOM test execution (optional)
        * @return None
        */
        virtual void OOMHandleWarningL( const TFileName& /* aTestCaseFile */,
                                        const TInt /* aCaseNumber */, 
                                        TInt& /* aFailNextValue */);

        /**
        * From CTestModuleBase; OOMTestFinalizeL may be used to finalize OOM
        * test environment. Usually no implementation is required.
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        */
        virtual void OOMTestFinalizeL( const TFileName& /* aTestCaseFile */, 
                                       const TInt /* aCaseNumber */ );

        /**
         * Method used to log version of test module
         */
        void SendTestModuleVersion();


    private:

        /**
        * C++ default constructor.
        */
        CTestFlexTimer();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Check for settings and create logger with test case title in file name.
        */
        void CreateTitleLoggerL(void);

        /**
        * Check for logger settings and delete title logger.
        */
        void DeleteTitleLogger(void);

        /**
         * Function returning test case name and pointer to test case function.
         * @since ?Series60_version
         * @param aCaseNumber test case number
         * @return TCaseInfo 
         */
        const TCaseInfo Case( const TInt aCaseNumber ) const;

        /**
        * Start monitoring FlexTimer server crashes
        * 
        * @param aResult The result of the test.
        * @param aCallback Pointer to the STIF test class
        * @return The status of execution of the test. KErrNone if the test
        *         was executed ok, any system wide errorcode otherwise.
        */
        static TInt StartServerMonitoringL(
            TTestResult& aResult, 
            CTestFlexTimer* aCallback );

        /**
        * Stop monitoring FlexTimer server crashes and fail the test case if
        * a crash has happened.
        * 
        * @param aResult The result of the test.
        * @param aCallback Pointer to the STIF test class
        * @return The status of execution of the test. KErrNone if the test
        *         was executed ok, any system wide errorcode otherwise.
        */
        static TInt StopServerMonitoringL(
            TTestResult& aResult, 
            CTestFlexTimer* aCallback );
        
        /**
        * Start second FlexTimerServer. 
        * 
        * Increase test coverage by testing server side destructors.
        * 
        * Starting of the server fails with panic "FlexTimerServer" 23 
        * (EFlexTimerServerStartServer).
        * 
        * @param aResult The result of the test.
        * @param aCallback Pointer to the STIF test class
        * @return The status of execution of the test. KErrNone if the test
        *         was executed ok, any system wide errorcode otherwise.
        */
        static TInt StartSecondServerL(
            TTestResult& aResult, 
            CTestFlexTimer* aCallback );
        
        /**
        * Changes Test client thread priority to EPriorityMuchMore
        * 
        * @param aResult The result of the test, always Passed.
        * @param aCallback Pointer to the STIF test class
        * @return The status of execution of the test. KErrNone always
        */
        static TInt SetThreadPriorityMuchMore(
            TTestResult& aResult, 
            CTestFlexTimer* aCallback );
        
        /**
        * Changes Test client thread priority to EPriorityNormal
        * 
        * @param aResult The result of the test, always Passed.
        * @param aCallback Pointer to the STIF test class
        * @return The status of execution of the test. KErrNone always
        */
        static TInt SetThreadPriorityNormal(
            TTestResult& aResult, 
            CTestFlexTimer* aCallback );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

        CStifLogger * iLog;

        // Standard logger
        CStifLogger *iStdLog;

        // Logger for currently running test case
        CStifLogger *iTCLog;
        
        // Flag saying if test case title should be added to log file name
        TBool iAddTestCaseTitleToLogName;
        
        // Flag saying if test module version was already sent
        TBool iVersionLogged;

        // Reserved pointer for future extension
        //TAny* iReserved;

        // Pointer to test (function) to be executed
        TestFunction iMethod;
        
        //CTimer CB failed case
        TBool iCTimerCBFailedCase;
        
    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // TESTFLEXTIMER_H

// End of File
