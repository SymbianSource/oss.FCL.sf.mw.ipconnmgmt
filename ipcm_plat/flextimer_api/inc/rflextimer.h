/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  RFlexTimer class for Flexible Timer server access.
 *
 */

/*
 * %version: 1 %
 */

#ifndef RFLEXTIMER_H
#define RFLEXTIMER_H

// INCLUDE FILES
#include <e32std.h> 

// CLASS DECLARATION
/**
 *  Resource class providing access to flexible timer server.
 *  
 *  This timer class is in many ways similar to RTimer in its use. The
 *  main difference is the option to set a timer expiry window. When the timer
 *  expiry window has been set to a value greater than zero, requested
 *  timeouts may expire at any point of time inside this window. This timer
 *  should be used for timed air-interface operations, such as keep-alive
 *  messages. The use of normal timers is recommended if the timeouts are not
 *  related to data transmission or reception activity in the device.
 *  
 *  The purpose for a flexible timer is to allow the system the opportunity to
 *  align timed behaviour that uses radio resources. For example, when a
 *  radio device is otherwise in an idle state, simple keep-alive messaging
 *  can be grouped together so that all traffic happens in single bursts
 *  instead of being distributed evenly along time timeline. This maximises
 *  radio idle time and therefore also battery lifetime.
 *  
 *  This class defines a client interface for flexible timer server
 *  (CFlexTimerServer). When a user requests timeout with RFlexTimer, the
 *  server will expire the request non-deterministically within an time window
 *  configured by the user. Timer resolution is one second.
 *  
 *  The timer returns one of the following values for TRequestStatus or any
 *  other system-wide error codes:
 *  
 *  <table border=1 bordercolor="#84B0C7" cellspacing="0" align="center">
 *  <tr>
 *      <th>Return value</th>
 *      <th>Description</th>
 *      <th>@c At()</th>
 *      <th>@c AtUTC()</th>
 *      <th>@c After()</th>
 *      <th>@c AfterTicks()</th>
 *  </tr>
 *  <tr>
 *      <td>@c KErrNone</td>
 *      <td>Timer has expired normally</td>
 *      <td align="center">X</td>
 *      <td align="center">X</td>  
 *      <td align="center">X</td>
 *      <td align="center">X</td>
 *  </tr>
 *  <tr>
 *      <td>@c KErrCancel</td>
 *      <td>Timer has been cancelled by @c Cancel()</td>
 *      <td align="center">X</td>
 *      <td align="center">X</td>
 *      <td align="center">X</td>
 *      <td align="center">X</td>
 *  </tr>
 *  <tr>
 *      <td>@c KErrAbort</td>
 *      <td>Timer has been aborted due time change</td>
 *      <td align="center">X</td>
 *      <td align="center">X</td>
 *      <td align="center">&nbsp;</td>
 *      <td align="center">&nbsp;</td>
 *  </tr>
 *  </table>
 *  
 *  Example: A user uses Configure-function to set the timer window to
 *  2,000,000 microseconds. Next the user requests a timeout with an interval
 *  value of 3,000,000 microseconds. Now the timer may expire at any moment
 *  after 1,000,000 microseconds and at latest after 3,000,000 microseconds
 *  (+ possible timer inaccuracy (timer resolution) ) has passed from the 
 *  timeout request. 
 *
 *  Passing a negative timer interval value or a past time instant 
 *  to timer functions will cause a panic . Restarting a timer that is already
 *  running will cause panic. Cancel must be used before starting timer again.
 *
 *  @see CFlexTimer
 *  @see CFlexPeriodic
 *  @see RTimer
 *
 *  Examples:
 *
 *  Synchronous usage of RFlexTimer:
 *  @code
 *      const TTimeIntervalMicroSeconds KInterval( 900000000 ); // 15 mins
 *      const TTimeIntervalMicroSeconds KWindow( 300000000 ); // 5 mins
 *
 *      TRequestStatus status;
 *
 *      RFlexTimer timer;
 *      User::LeaveIfError( timer.Connect() );
 *
 *      timer.Configure( KWindow );
 *      timer.After( status, KInterval );
 *
 *      User::WaitForRequest( status );     // Wait till the timer expires
 *
 *      // Check the request's status
 *      if ( status == KErrNone )
 *          {
 *          // Timer was expired ok
 *          }
 *      else if ( status == KErrAborted )
 *          {
 *          // Only for At() and AtUTC()
 *          // Timer was aborted by time change
 *          }
 *      else if ( status == KErrCancel )
 *          {
 *          // timer.Cancel() was called while the timer was active
 *          }
 *      else
 *          {
 *          // Some other error has happened
 *          }
 *          .
 *          .
 *          .
 *      timer.Close();
 *  @endcode
 *
 *  Asynchronous usage of RFlexTimer (consider using CFlexTimer or 
 *  CFlexPeriodic instead of own implementation):
 *  
 *  Class definition
 *  @code
 *      class CMyNetworkServiceMonitor : public CActive
 *      {
 *      public: // Members
 *
 *          // Public two-phased constructor
 *          static CMyNetworkServiceMonitor* NewL(
 *              TTimeIntervalMicroSeconds aWindow,
 *              TTimeIntervalMicroSeconds aInterval );
 *
 *          // Destructor
 *          virtual ~CMyNetworkServiceMonitor();
 *
 *      protected: // From CActive
 *
 *          // Cancel outstanding request. Called by CActive::Cancel().
 *          virtual void DoCancel();
 *
 *          // Handle request completion event  
 *          virtual void RunL();
 *      
 *      private: // Members
 *
 *          // Constructor
 *          CMyNetworkServiceMonitor( TTimeIntervalMicroSeconds aInterval );
 *
 *          // Second phase constructor
 *          void ConstructL( TTimeIntervalMicroSeconds aWindow );
 *
 *          // Does the network server monitoring.
 *          // Implementation is not provided by this example.
 *          void DoNetworkServiceMonitoring();
 *      
 *      private: // Data
 *          RFlexTimer iTimer;
 *          TTimeIntervalMicroSeconds iInterval;
 *      };
 *  @endcode
 *
 *  @code
 *  CMyNetworkServiceMonitor::CMyNetworkServiceMonitor( 
 *      TTimeIntervalMicroSeconds aInterval ):
 *      CActive( CActive::EPriorityStandard ),
 *      iInterval( aInterval )
 *      {
 *      // Nothing to do
 *      } 
 *
 *  // -----------------------------------------------------------------------------
 *  //
 *  void CMyNetworkServiceMonitor::ConstructL( 
 *      TTimeIntervalMicroSeconds aWindow )
 *      {
 *      User::LeaveIfError( iTimer.Connect() );
 *      iTimer.Configure( aWindow );
 *      }
 *
 *  // -----------------------------------------------------------------------------
 *  //
 *  CMyNetworkServiceMonitor* CMyNetworkServiceMonitor::NewL( 
 *      TTimeIntervalMicroSeconds aWindow,
 *      TTimeIntervalMicroSeconds aInterval )
 *      {
 *      CMyNetworkServerMonitor* self =
 *          new (ELeave) CMyNetworkServerMonitor( aInterval );
 *      CleanupStack::PushL( self );
 *      self->ConstructL( aWindow );
 *      CleanupStack::Pop( self );
 *
 *      iTimer.After( TTimeIntervalMicroSeconds( 0 ) );
 *      SetActive();
 *
 *      return self;
 *      }
 *
 *  // -----------------------------------------------------------------------------
 *  //
 *  CMyNetworkServiceMonitor::~CMyNetworkServiceMonitor()
 *      {
 *      Cancel();       // Calls CActive::Cancel()
 *      iTimer.Close(); // Close the timer handle
 *      }
 *      
 *  // -----------------------------------------------------------------------------
 *  //
 *  void CMyNetworkServiceMonitor::DoCancel()
 *      {
 *      iTimer.Cancel();
 *      }
 *
 *  // -----------------------------------------------------------------------------
 *  //
 *  void CMyNetworkServiceMonitor::RunL()
 *      {
 *      // Note! If used RFlexTimer::At() or RFlexTimer::AtUTC()
 *      // iStatus == KErrAbort should also be handled.
 *      
 *      if ( iStatus == KErrNone )
 *          {
 *          // Do the network server monitor actions
 *          DoNetworkServiceMonitoring();
 *
 *           // Refresh the timer
 *          iTimer.After( iInterval );
 *          SetActive();
 *          }
 *      else if ( iStatus == KErrCancel )
 *          {
 *          // Timer was cancelled. Do not activate it again... 
 *          }
 *      else
 *          {
 *          // Handle the error by implementing RunError()
 *          // See also: CActive::RunError()
 *          User::Leave( iStatus );
 *          }
 *      }
 *  @endcode
 */
class RFlexTimer : public RSessionBase
    {
public:
    // Constructors and destructors

    /**
     * Constructs the object.
     */
    IMPORT_C RFlexTimer();

    /**
     * Destructs the object.
     */
    IMPORT_C ~RFlexTimer();

    /**
     * Connects to the timer server. This function needs to be called before
     * any timeouts can be requested.
     * 
     * @return KErrNone on success. KErrNotSupported if client's and server's
     * versions don't match. Otherwise returns one of the system-wide error
     * codes.
     * @panic RFlexTimer 33 Connected is called twice without closing the
     * handle first
     * 
     * Example:
     * @code
     *      RFlexTimer timer;
     *      User::LeaveIfError( timer.Connect() );
     *          .
     *          . // Set timer/wait for expiration.
     *          .
     *      timer.Close();  // Close the handle
     * @endcode
     */
    IMPORT_C TInt Connect();

    /**
     * Cancels the timer. Active timer will be completed with status 
     * KErrCancel. If there are no active timer, Cancel() does nothing.
     * 
     * @panic KERN-EXEC 0 Cancel() was called before Connect()
     * 
     * Example:
     * @code
     *      RFlexTimer timer;
     *      User::LeaveIfError( timer.Connect() );
     *          .
     *          .
     *          .
     *      // Oops, no need to wait the timer expiration
     *      timer.Cancel(); // Cancel the pending timer    
     *          .
     *          .
     *          .  
     *      timer.Close();  // Close the handle
     * @endcode
     */
    IMPORT_C void Cancel();

    /**
     * An asynchronous timeout request to the flexible timer server.
     * Fire timer at latest on the given 32-bit interval.
     * 
     * @param aStatus active object to be used for getting responses.	
     * @param aInterval the interval value until which the timer must expire.
     * 
     * @panic RFlexTimer 1  aInterval is negative
     * @panic RFlexTimer 15 Timer is already active. Wait it to expire or
     * cancel it first.
     *
     * Example:
     * @code
     *      const TTimeIntervalMicroSeconds32 KInterval32( 300000000 ); // 5 mins  
     *      const TTimeIntervalMicroSeconds32 KWindow32( 120000000 ); // 2 mins  
     *      
     *      TRequestStatus status;
     *      RFlexTimer timer;
     *      
     *      User:LeaveIfError( timer.Connect() );
     *      timer.Configure( KWindow32 );
     *      timer.After( status, KInterval32 );
     *      
     *      User::WaitForRequest( status ); // Wait timer to expire, synchronous
     *          .
     *          .
     *          .
     *      timer.Close();  // Close the handle
     * @endcode 
     */
    IMPORT_C void After( TRequestStatus& aStatus,
                         TTimeIntervalMicroSeconds32 aInterval );
    
    /**
     * An asynchronous timeout request to the flexible timer server.
     * Fire timer at latest on the given 64-bit interval.
     * @param aStatus active object to be used for getting responses.   
     * @param aInterval the interval value until which the timer must expire.   
     * @panic RFlexTimer 1  aInterval is negative
     * @panic RFlexTimer 15 Timer is already active. Wait it to expire or
     * cancel it first.
     * @panic RFlexTimer 24 aInterval is too big (max 730 days)
     * 
     * Example:
     * @code
     *      const TTimeIntervalMicroSeconds KInterval64( 300000000 ); // 5 mins
     *
     *      TRequestStatus status;
     *      RFlexTimer timer;
     *      
     *      User::LeaveIfError( timer.Connect() );
     *      timer.After( status, KInterval64 );
     *      
     *      User::WaitForRequest( status ); // Wait timer to expire, synchronous
     *          .
     *          .
     *          .
     *      timer.Close();  // Close the handle
     * @endcode
     */
    IMPORT_C void After( TRequestStatus& aStatus,
                         TTimeIntervalMicroSeconds aInterval );

    /**
     * An asynchronous timeout request to the flexible timer server.
     * Fire timer at latest after the given number of system ticks.
     * 
     * By default the system tick is 1/64 second. The exact value for one
     * system tick can be retrieve via Symbian OS HAL API:
     * 
     * @code
     *      TInt tickInMicroSeconds;
     *      HAL::Get( HAL::ESystemTickPeriod, tickInMicroSeconds );
     * @endcode
     * 
     * @param aStatus active object to be used for getting responses.
     * @param aTicks the interval value until which the timer must expire. 	
     * @panic RFlexTimer 2  aTicks is negative
     * @panic RFlexTimer 15 Timer is already active. Wait it to expire or
     * cancel it first.
     * 
     * Example:
     * @code
     *      const TInt KIntervalInTicks( 57600 ); // 15 mins (1 tick = 15625 microseconds)
     *
     *      TRequestStatus status;
     *      RFlexTimer timer;
     *      
     *      User::LeaveIfError( timer.Connect() );
     *      timer.AfterTicks( status, KIntervalInTicks );
     *      
     *      User::WaitForRequest( status ); // Wait timer to expire, synchronous
     *          .
     *          .
     *          .
     *      timer.Close();  // Close the handle
     * @endcode
     */
    IMPORT_C void AfterTicks( TRequestStatus& aStatus, TInt aTicks );

    /**
     * An asynchronous timeout request to the flexible timer server.
     * Fire timer between at latest by the given time value. If the system
     * time changes before the timer requested with At-function expires, it
     * will cancel itself. This will result in the aStatus argument to have
     * KErrAbort-value.
     * @param aStatus active object to be used for getting responses.
     * @param aTime indicates the latest system-time when this timer should
     * be fired.
     * @panic RFlexTimer 3  aTime is in the past
     * @panic RFlexTimer 15 Timer is already active. Wait it to expire or
     * cancel it first.
     * @panic RFlexTimer 24 aTime is too far in the future (max 730 days)
     * 
     * Example:
     * @code
     *      const TTimeIntervalMinutes KWaitTime( 5 );
     *      
     *      TRequestStatus status;
     *      RFlexTimer timer;
     *      User::LeaveIfError( timer.Connect() );
     *      
     *      TTime now;
     *      now.HomeTime();
     *
     *      timer.At( status, now + KWaitTime );
     *      
     *      User::WaitForRequest( status ); // Wait timer to expire, synchronous
     *          .
     *          .
     *          .
     *      timer.Close();  // Close the handle
     * @endcode
     */
    IMPORT_C void At( TRequestStatus& aStatus, const TTime& aTime );

    /**
     * An asynchronous timeout request to the flexible timer server.
     * Fire timer between at latest by the given UTC (Coordinated Universal
     * Time) time value. If the system time changes before the timer requested
     * with AtUTC-function expires, it will cancel itself. This will result in
     * the aStatus argument to have KErrAbort-value.
     * @param aStatus active object to be used for getting responses.
     * @param aTime indicates the latest UTC time when this timer should be
     * fired. 
     * @panic RFlexTimer 4  aTime is in the past
     * @panic RFlexTimer 15 Timer is already active. Wait it to expire or
     * cancel it first.
     * @panic RFlexTimer 24 aTime is too far in the future (max 730 days)
     * 
     * Example:
     * @code
     *      const TTimeIntervalMinutes KWaitTime( 5 );
     *
     *      TRequestStatus status;
     *      RFlexTimer timer;
     *      User::LeaveIfError( timer.Connect() );
     *
     *      TTime nowUtc;
     *      nowUtc.UniversalTime();
     *
     *      timer.At( status, nowUtc + KWaitTime );
     *
     *      User::WaitForRequest( status ); // Wait timer to expire, synchronous
     *          .
     *          .
     *          .
     *      timer.Close();  // Close the handle
     * @endcode
     */
    IMPORT_C void AtUTC( TRequestStatus& aStatus, const TTime& aTime );

    /**
     * Sets the window size in which alignment is possible for the timer.
     * This is a synchronous command - it will return only after the server
     * has completed the configuration message. If the timer is already
     * running, it will return KErrInUse.
     * 
     * If user is not calling this function in prior to start timer, default
     * value will be used as time window. Default value is currently 20%
     * of total timer running time 
     * (0.2 * (requested_expiry_time - current_time)) and this value is in 
     * subject to change.
     * 
	 * If the user wishes to restore the default window size behaviour,
     * a new timer is needed. 
	 *
     * Giving zero value as aWindowSize parameter, means that timer is fired
     * precisely at requested time.
	 *
     * @param aWindowSize is the window size in 32-bit microseconds in which
     * alignment is possible.
     *
     * @return KErrNone on success, KErrInUse if timer is set (wait it to be
     * expired or cancel it before configuring). Otherwise returns one of the 
     * system-wide error codes.
     *
     * @panic RFlexTimer 5  aWindowSize is negative
     * 
     * Example:
     * @code
     *      const TTimeIntervalMicroSeconds32 KWindow32( 120000000 ); // 2 mins
     *
     *      RFlexTimer timer;
     *      User::LeaveIfError( timer.Connect() );
     *      
     *      timer.Configure( KWindow32 );
     *          .
     *          .
     *          .
     *      timer.Close();  // Close the handle
     * @endcode
     */
    IMPORT_C TInt Configure( TTimeIntervalMicroSeconds32 aWindowSize );
    
    /**
     * This function overloads the Configure-function with 64-bit parameters.
     * 
     * @param aWindowSize is the window size in 64-bit microseconds in which
     * alignment is possible.
     * 
     * @return KErrNone on success, KErrInUse if timer is set, wait it to 
     * expire or cancel it before configuring. Otherwise returns one of the 
     * system-wide error codes.
     * 
     * @panic RFlexTimer 5  aWindowSize is negative
     * @panic RFlexTimer 24 aWindowSize is too big (max 730 days)
     * 
     * @see TInt Configure( TTimeIntervalMicroSeconds32 )
     * 
     * Example:
     * @code
     *      const TTimeIntervalMicroSeconds KWindow64( 120000000 ); // 2 mins
     *
     *      RFlexTimer timer;
     *      User::LeaveIfError( timer.Connect() );
     *      
     *      timer.Configure( KWindow64 );
     *          .
     *          .
     *          .
     *      timer.Close();  // Close the handle
     * @endcode
     */
    IMPORT_C TInt Configure( TTimeIntervalMicroSeconds aWindowSize );

private:
    
    /**
     * Gets the version number.
     * @return The version.
     */
    TVersion Version() const;

    /**
     * Connects to the server. If server does not exist, it is created.
     * @return KErrNone on success. Otherwise returns one of the system-wide
     * error codes.
     */
    TInt StartServer();

    };

#endif // RFLEXTIMER_H

// End of File
