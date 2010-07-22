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
 * Description:  CFlexTimer class for Flexible Timer server access.
 *
 */

/*
 * %version: 1 %
 */

#ifndef CFLEXTIMER_H
#define CFLEXTIMER_H

// INCLUDE FILES
#include <e32base.h> 
#include "rflextimer.h"

// CLASS DECLARATION
/**
 *  Class providing access to flexible timer service.
 *  This class uses RFlexTimer to provide user with timeout services. Each
 *  timeout that is requested with this class has a time window during which
 *  it may expire. The size of this window needs to be configured by the user.
 *  If not configured, a default window size is used. See RFlexTimer for 
 *  details about default window. 
 *  
 *  This class can not be instantiated; it needs to be inherited by a
 *  user's derived class. The user also needs to implement the RunL
 *  function.
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
 *  </tr>
 *  <tr>
 *      <td>@c KErrNone</td>
 *      <td>Timer has expired normally</td>
 *      <td align="center">X</td>
 *      <td align="center">X</td>  
 *      <td align="center">X</td>
 *  </tr>
 *  <tr>
 *      <td>@c KErrCancel</td>
 *      <td>Timer has been cancelled</td>
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
 *  </tr>
 *  </table>
 *  
 *  Any object derived from this class needs to connect to the flextimer
 *  server by calling CFlexTimer::ConstructL() function before calling 
 *  configuration or timeout functions. Otherwise any timeout function will
 *  result in panic.
 *
 *  @see RFlexTimer
 *  @see CTimer
 *
 *  Example:
 *  
 *  Class definition:
 *  @code
 *  class CMyNetworkServiceMonitor : public CFlexTimer
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
 *          // Handles an active object's request completion event
 *          virtual void RunL();
 *
 *      private: // Members
 *
 *          // Private constructor
 *          CMyNetworkServiceMonitor( TTimeIntervalMicroSeconds aInterval );
 *
 *          // Second phase constructor
 *          void ConstructL( TTimeIntervalMicroSeconds aWindow );
 *
 *          // Does the network server monitoring.
 *          // Implementation is not provided by this example.
 *          DoNetworkServiceMonitoring();
 *
 *      private: // Data
 *
 *          TTimeIntervalMicroSeconds iInterval;
 *      };
 *  @endcode
 *
 *  Class implementation:
 *  @code
 *  CMyNetworkServiceMonitor::CMyNetworkServiceMonitor( TTimeIntervalMicroSeconds aInterval ) :
 *      CFlexTimer( CActive::EPriorityStandard ),
 *      iInterval( aInterval )
 *      {
 *      // Nothing to do
 *      }
 *
 *  // ---------------------------------------------------------------------------
 *  //
 *  CMyNetworkServiceMonitor::~CMyNetworkServiceMonitor()
 *      {
 *      Cancel(); // DoCancel() is implemented by CFlexTimer
 *      }
 *
 *  // ---------------------------------------------------------------------------
 *  //
 *  void CMyNetworkServiceMonitor::ConstructL( TTimeIntervalMicroSeconds aWindow )
 *      {
 *      const TTimeIntervalMicroSeconds KImmediately( 0 );
 *
 *      CFlexTimer::ConstructL();
 *      CActiveScheduler::Add( this );
 *
 *      Configure( aWindow );
 *
 *      After( KImmediately );  // Kick-off the timer
 *      }
 *
 *  // ---------------------------------------------------------------------------
 *  //
 *  CMyNetworkServiceMonitor* CMyNetworkServiceMonitor::NewL(
 *      TTimeIntervalMicroSeconds aWindow,
 *      TTimeIntervalMicroSeconds aInterval   )
 *      {
 *      CMyNetworkServiceMonitor* self =
 *          new (ELeave) CMyNetworkServiceMonitor( aInterval );
 *      CleanupStack::PushL( self );
 *      self->ConstructL( aWindow );
 *      CleanupStack::Pop( self );
 *      return self;
 *      }
 *
 *  // ---------------------------------------------------------------------------
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
 *          // Refresh the timer
 *          After( iInterval );
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
class CFlexTimer : public CActive
    {
public:

    /**
     * Destructs the object.
     */
    IMPORT_C virtual ~CFlexTimer();

    /**
     * Fire timer at latest on the given interval. 32-bit interval value.
     * @param aInterval the interval value until which the timer must expire.
     * 
     * @panic CFlexTimer 10 CFlexTimer::ConstructL() has not been called yet
     * @panic RFlexTimer 15 Timer is already active. Wait it to expire or
     * cancel it first.
     * @panic RFlexTimer  1 aInterval is negative
     *
     * Example:
     * @code
     *      const TTimeIntervalMicroSeconds32 KInterval32( 600000000 ); // 10 min
     *      After( KInterval32 );
     * @endcode
     */
    IMPORT_C void After( TTimeIntervalMicroSeconds32 aInterval );
    
    /**
     * Fire timer at latest on the given interval. 64-bit interval value.
     *
     * @param aInterval the interval value until which the timer must expire.
     * 
     * @panic CFlexTimer 10 CFlexTimer::ConstructL() has not been called yet
     * @panic RFlexTimer  1 aInterval is negative
     * @panic RFlexTimer 15 Timer is already active. Wait it to expire or
     * cancel it first.
     * @panic RFlexTimer 24 aInterval is too big (over 730 days)
     * 
     * Example:
     * @code
     *      const TTimeIntervalMicroSeconds KInterval64( 600000000 ); // 10 min
     *      After( KInterval64 );
     * @endcode
     */
    IMPORT_C void After( TTimeIntervalMicroSeconds aInterval );

    /**
     * Fire timer at latest by the given time value. The time value provided
     * here is related to HomeTime. @see TTime. If the system
     * time changes before the timer requested with At-function expires, it
     * will cancel itself. This will result in the aStatus argument to have
     * KErrAbort-value.
     *
     * @param aTime indicates the latest time when this timer should be fired.
     *
     * @panic CFlexTimer 10 CFlexTimer::ConstructL() has not been called yet
     * @panic RFlexTimer 3  aTime is in the past
     * @panic RFlexTimer 15 Timer is already active. Wait it to expire or
     * cancel it first.
     * @panic RFlexTimer 24 aTime is too far to the future (over 730 days)
     * 
     * Example:
     * @code
     *      const TTimeIntervalMinutes KWaitTime( 5 );
     *
     *      TTime now;
     *      now.HomeTime();
     *
     *      At( nowUtc + KWaitTime );
     * @endcode
     */
    IMPORT_C void At( const TTime& aTime );

    /**
     * Fire timer at latest by the given UTC (Coordinated Universal Time)
     * time value. If the system
     * time changes before the timer requested with At-function expires, it
     * will cancel itself. This will result in the aStatus argument to have
     * KErrAbort-value.
     * @param aTime indicates the latest UTC time when this timer should be
     * fired. 
     *
     * @panic CFlexTimer 10 CFlexTimer::ConstructL() has not been called yet
     * @panic RFlexTimer 3  aTime is in the past
     * @panic RFlexTimer 15 Timer is already active. Wait it to expire or
     * cancel it first.
     * @panic RFlexTimer 24 aTime is too far to the future (over 730 days)
     *
     * Example:
     * @code
     *      const TTimeIntervalMinutes KWaitTime( 5 );
     *
     *      TTime nowUtc;
     *      nowUtc.UniversalTime();
     *
     *      AtUTC( nowUtc + KWaitTime );
     * @endcode
     */
    IMPORT_C void AtUTC( const TTime& aTime );

    /**
     * Sets the window size in which flexibility is possible for the timer.
     * @see RFlexTimer::Configure
     * @param aWindowSize is the window size in 32-bit microseconds in which
     * timer flexiblity is possible.
     * @return KErrNone on success. KErrInUse, if timer has been
     * started already.
     *
     * @panic CFlexTimer 10 CFlexTimer::ConstructL() has not been called yet
     * @panic RFlexTimer 5  aWindowSize is negative
     * 
     * Example:
     * @code
     *     const TTimeIntervalMicroSeconds32 KWindow32( 120000000 ); // 2 mins
     *     Configure( KWindow32 );
     * @endcode
     */
    IMPORT_C TInt Configure( TTimeIntervalMicroSeconds32 aWindowSize );
    
    /**
     * Sets the window size in which flexibility is possible for the timer.
     * @see RFlexTimer::Configure
     * @param aWindowSize is the window size in 64-bit microseconds in which
     * timer flexiblity is possible.
     * @return KErrNone on success. KErrInUse, if timer has been
     * started already.
     *
     * @panic CFlexTimer 10 CFlexTimer::ConstructL() has not been called yet
     * @panic RFlexTimer 5  aWindowSize is negative
     * @panic RFlexTimer 24 aWindowSize is too big (max 730 days)
     * 
     * Example:
     * @code
     *      const TTimeIntervalMicroSeconds KWindow64( 120000000 ); // 2 mins
     *      Configure( KWindow64 );
     * @endcode
     */
    IMPORT_C TInt Configure( TTimeIntervalMicroSeconds aWindowSize );

protected:
    
    /**
     * 2nd phase constructor for this the object. Connects to the flextimer
     * server and creates a session. This function needs to be called by the
     * object that is derived from this class.
     */
    IMPORT_C void ConstructL();

    /**
     * Constructs the object.
     */
    IMPORT_C CFlexTimer( TInt aPriority );

    /**
     * Inherited from CActive.
     */
    IMPORT_C virtual void DoCancel();

private:

    /**
     * The timer resource used by this object.
     */
    RFlexTimer iTimer;

    };

#endif // CFLEXTIMER_H


