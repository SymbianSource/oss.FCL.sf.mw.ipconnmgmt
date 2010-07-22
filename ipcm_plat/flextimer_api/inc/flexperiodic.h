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
 * Description:  CFlexPeriodic class for Flexible Timer server access.
 *
 */

/*
 * %version: 1 %
 */

#ifndef CFLEXPERIODIC_H
#define CFLEXPERIODIC_H

// INCLUDE FILES
#include "flextimer.h"

// CLASS DECLARATION
/**
 *  Class providing access to flexible periodic timer service.
 *  This class inherits CFlexTimer and provides user with a periodic
 *  timer very similar to CPeriodic with the exception that both
 *  the initial delay and the consequent intervals have flexible
 *  windows of opportunity during which they may expire.
 *  
 *  The flexible window sizes are set with Configure-function. If these window
 *  sizes are not explicitly set by the user, a default window size is used.
 *  See RFlexTimer for details about default window. 
 *  
 *  Note that when CFlexPeriodic expires next expiry interval always starts 
 *  at that moment. This means that over time, timer expiry can start to 
 *  "slide" when comparing to CPeriodic. 
 *  
 *  For example: CPeriodic is started at 8 o'clock with 30 minute interval.
 *  It will then expire at 8.30, 9.00, ... 15.30, 16.00
 *  
 *  If CFlexPeriodic is used as a timer it can possibly expire at
 *  8.30, 9.00, 9.28, 9.55, ... 15.45, 16.12 etc. 
 *
 *  @see CFlexTimer
 *  @see RFlexTimer
 *  @see CPeriodic
 *
 *  Example:
 *  
 *  Class definition:
 *  @code
 *  // Forward declaration
 *  class CFlexPeriodic;
 *
 *  // Class definition
 *  class CMyNetworkServiceMonitor : public CBase
 *      {
 *      public: // Members
 *
 *          // Destructor
 *          virtual ~CMyNetworkServiceMonitor();
 *
 *          // Two-phased constructor
 *          static CMyNetworkServiceMonitor* NewL(
 *              TTimeIntervalMicroSeconds aWindow,
 *              TTimeIntervalMicroSeconds aInterval );
 *
 *      private: // Members
 *
 *          // Constructor
 *          CMyNetworkServiceMonitor();
 *
 *          // 2nd phase constuctor
 *          void ConstructL(
 *              TTimeIntervalMicroSeconds aWindow,
 *              TTimeIntervalMicroSeconds aInterval );
 *
 *          // Callback function for periodic timer. A function for TCallBack.
 *          static TInt DoMonitoring( TAny* aArgument );
 *
 *          // Callback function for timer server errors. A function for 
 *          // TCallBack.
 *          static TInt HandleError( TAny* aArgument );
 *
 *          // Does the network server monitoring.
 *          // Implementation is not provided by this example.
 *          DoNetworkServiceMonitoring();
 *
 *      private: // Data
 *
 *          // Owns. Flexible periodic timer.
 *          CFlexPeriodic* iTimer;
 *      };
 *  @endcode
 *
 *  Class implementation:
 *  @code
 *  // ---------------------------------------------------------------------------
 *  //
 *  CMyNetworkServiceMonitor::CMyNetworkServiceMonitor() : iTimer( NULL )
 *      {
 *      // Nothing to do
 *      }
 *
 *  // ---------------------------------------------------------------------------
 *  //
 *  CMyNetworkServiceMonitor::~CMyNetworkServiceMonitor()
 *      {
 *      if ( iTimer != NULL )
 *          {
 *          iTimer->Cancel(); // Cancel pending timer
 *          delete iTimer;
 *          }
 *      }
 *
 *  // ---------------------------------------------------------------------------
 *  //
 *  CMyNetworkServiceMonitor* CMyNetworkServiceMonitor::NewL(
 *      TTimeIntervalMicroSeconds aWindow,
 *      TTimeIntervalMicroSeconds aInterval )
 *      {
 *      CMyNetworkServiceMonitor* self =
 *          new (ELeave) CMyNetworkServiceMonitor;
 *      CleanupStack::PushL( self );
 *      self->ConstructL( aWindow, aInterval );
 *      CleanupStack::Pop( self );
 *      return self;
 *      }
 *
 *  // ---------------------------------------------------------------------------
 *  //
 *  void CMyNetworkServiceMonitor::ConstructL(
 *      TTimeIntervalMicroSeconds aWindow,
 *      TTimeIntervalMicroSeconds aInterval )
 *      {
 *      // Constants
 *      const TTimeIntervalMicroSeconds KNoWindow( 0 );
 *      const TTimeIntervalMicroSeconds KImmediately( 0 );
 *
 *      iTimer = CFlexPeriodic::NewL( CActive::EPriorityStandard );
 *
 *      // Push timer to cleanup stack due configuration may leave
 *      CleanupStack::PushL( iTimer );
 *
 *      // Set delay window 0 minute
 *      User::LeaveIfError( 
 *          iTimer->Configure( KNoWindow, aWindow ) );
 *
 *      // Start the timer, 1st call immediately
 *      iTimer->Start(
 *          KImmediately,
 *          aInterval,
 *          TCallBack( DoMonitoring, this ),
 *          TCallBack( HandleError, this ) );
 *
 *      CleanupStack::Pop( iTimer );
 *      }
 *
 *  // ---------------------------------------------------------------------------
 *  //
 *  TInt CMyNetworkServiceMonitor::DoMonitoring( TAny* aArgument )
 *      {
 *      CMyNetworkServiceMonitor* monitor =
 *          static_cast<CMyNetworkServiceMonitor*>( aArgument );
 *
 *      monitor->DoNetworkServerMonitoring();
 *      return KErrNone;
 *      }
 *     
 *  // ---------------------------------------------------------------------------
 *  //
 *  TInt CMyNetworkServiceMonitor::HandleError( TAny* aArgument )
 *      {
 *      CMyNetworkServiceMonitor* monitor =
 *          static_cast<CMyNetworkServiceMonitor*>( aArgument );
 *
 *      // Handling of the timer server error (e.g. closing up the application)
 *      // here.
 *          .
 *          .
 *          .
 *      return KErrNone;
 *      }     
 *  @endcode
 */
class CFlexPeriodic : public CFlexTimer
    {
public:

    /**
     * A leaving constructor for the object.
     * 
     * @param aPriority Priority of the active object. Type CActive::TPriority
     * 
     * @return A pointer to a CFlexPeriodic object on success.
     * 
     * @leave KErrMemory Not enough memory
     */
    static IMPORT_C CFlexPeriodic* NewL( TInt aPriority );

    /**
     * Destructor for the object.
     */
    virtual IMPORT_C ~CFlexPeriodic();

    /**
     * Starts the periodic timer. After the timer has been started, it first
     * calls aCallBack function after the aDelay time has expired. Thereafter
     * the aCallBack function is called periodically after anInterval from the
     * previous expiry has elapsed. All the expirations happen within timing
     * tolerancies indicated by the flexibility windows. The flexibility
     * window sizes are set with a Configure-function. The Configure needs to
     * be called before Starting the periodic timer. 32-bit delay and interval
     * values.

     * @param aDelay is the initial delay between this Start-function and the
     * first timer expiry. This value presents the maximum delay - flexibility
     * is applied to a time window opening prior to this.
     * @param anInterval is the size of the intervals after the initial delay.
     * This value presents the maximum interval - flexibility is applied to
     * a time window opening prior to this. Interval must be at least one 
     * microsecond.
     * @param aCallBack is a reference to a function that is executed at each
     * expiry of the timer. NULL call back function reference is prohibited.
     * @param aCallBackError optional, but recommended parameter is a 
     * reference to a function that is executed if error occurs somewhere 
     * in FlexTimer system (e.g. memory allocation failed). 
     * If user does not provide this argument and error occurs, client 
     * is paniced. Recommended action in case of error is to hold back all 
     * actions for a while and give system some time to recover and free 
     * resources.
     *
     * @panic CFlexPeriodic  6 aDelay is negative
     * @panic CFlexPeriodic  7 aInterval is zero or negative
     * @panic CFlexPeriodic 31 aCallBack.iFunction is NULL
     * @panic RFlexTimer    15 Start() has been called twice without 
     * cancelling the timer first
     *
     * @see Configure
     */
    IMPORT_C void Start( TTimeIntervalMicroSeconds32 aDelay,
                         TTimeIntervalMicroSeconds32 anInterval,
                         TCallBack aCallBack,
                         TCallBack aCallBackError = TCallBack() );
    
    /**
     * This function overloads the Start-function with 64-bit delay and
     * interval values.
     *
     * @param aDelay is the initial delay between this Start-function and the
     * first timer expiry. This value presents the maximum delay - flexibility
     * is applied to a time window opening prior to this.
     * @param anInterval is the size of the intervals after the initial delay.
     * This value presents the maximum interval - flexibility is applied to
     * a time window opening prior to this. Interval must be at least one 
     * microsecond.
     * @param aCallBack is a reference to a function that is executed at each
     * expiry of the timer. NULL call back function reference is prohibited.
     * @param aCallBackError optional, but recommended parameter is a 
     * reference to a function that is executed if error occurs somewhere 
     * in FlexTimer system (e.g. memory allocation failed). 
     * If user does not provide this argument and error occurs, client 
     * is paniced. Recommended action in case of error is to hold back all 
     * actions for a while and give system some time to recover and free 
     * resources.
     *
     * @panic CFlexPeriodic  6 aDelay is negative
     * @panic CFlexPeriodic  7 aInterval is zero or negative
     * @panic CFlexPeriodic 31 aCallBack.iFunction is NULL
     * @panic RFlexTimer    15 Start() has been called twice without 
     * cancelling the timer first
     * @panic RFlexTimer    24 aDelay or aInterval is too long (over 730 days)
     *
     * @see Configure
     */
    IMPORT_C void Start( TTimeIntervalMicroSeconds aDelay,
                         TTimeIntervalMicroSeconds anInterval,
                         TCallBack aCallBack,
                         TCallBack aCallBackError = TCallBack() );

    /**
     * Sets the window sizes inside which the timer can expire. Must be called
     * before timer is started. If configure is called after the timer has
     * been Started, this function returns an error code.
     * 
     * The window sizes set with this function override the default window
     * sizes. @see RFlexTimer::Configure
     *
     * @param aDelayWindow is the flexibility window size in 32-bit
     * microseconds for the initial delay.
     * @param aIntervalWindow is the flexibility window size in 32-bit
     * microseconds for the intervals after the initial delay.
     *
     * @return KErrNone on success. KErrInUse, if timer has been
     * started already. In case of error, the window sizes not established
     * into the timer and need to be configured again.
     *
     * @panic CFlexPeriodic  8 aDelayWindow is negative
     * @panic CFlexPeriodic  9 aIntervalWindow is negative
     */
    IMPORT_C TInt Configure( TTimeIntervalMicroSeconds32 aDelayWindow,
                             TTimeIntervalMicroSeconds32 aIntervalWindow );

    /**
     * This function overloads the Configure-function with 64-bit parameters.
     *
     * @param aDelayWindow is the flexibility window size in 64-bit 
     * microseconds for the initial delay.
     * @param aIntervalWindow is the flexibility window size in 64-bit
     * microseconds for the intervals after the initial delay.
     *
     * @return KErrNone on success. KErrInUse, if timer has been
     * started already. In case of error, the window sizes not established
     * into the timer and need to be configured again.
     *
     * @panic CFlexPeriodic  8 aDelayWindow is negative
     * @panic CFlexPeriodic  9 aIntervalWindow is negative
     * @panic RFlexTimer    24 aDelayWindow or aIntervalWindow is too long 
     * (over 730 days)
     */
    IMPORT_C TInt Configure( TTimeIntervalMicroSeconds aDelayWindow,
                             TTimeIntervalMicroSeconds aIntervalWindow );

protected:

    /**
     * Inherited from CActive.
     */
    virtual void RunL();
    
private:

    /**
     * Constructs the object. The second phase of the construction.
     */
    void ConstructL();

    /**
     * Private constructor for the object.
     * @param aPriority The priority of the active object. If timing is
     * critical, it should be higher than that of all other active objects
     * owned by the scheduler.
     */
    CFlexPeriodic( TInt aPriority );

private:
    
    /**
     * No definition. This function is not to be used through CFlexPeriodic.
     * @see CFlexTimer
     */
    void After( TTimeIntervalMicroSeconds32 aInterval );
    
    /**
     * No definition. This function is not to be used through CFlexPeriodic.
     * @see CFlexTimer
     */
    void After( TTimeIntervalMicroSeconds aInterval );

    /**
     * No definition. This function is not to be used through CFlexPeriodic.
     * @see CFlexTimer
     */
    void At( const TTime& aTime );

    /**
     * No definition. This function is not to be used through CFlexPeriodic.
     * @see CFlexTimer
     */
    void AtUTC( const TTime& aTime );
    
    /**
     * No definition. This function is not to be used through CFlexPeriodic.
     * @see CFlexTimer
     */
    TInt Configure( TTimeIntervalMicroSeconds32 aWindowSize );

    /**
     * Interval value that is used after the initial delay.
     */
    TTimeIntervalMicroSeconds iInterval;

    /**
     * Flex window size that is used after the initial delay.
     */
    TTimeIntervalMicroSeconds iIntervalWindow;

    /**
     * The callback function which is called at the completion of
     * flextimer server requests.
     */
    TCallBack iCallBack;
    
    /**
     * The callback function which is called if error accurs
     * somewhere in FlexTimerSystem. i.e. Error code is returned
     * to RunL.
     */
    TCallBack iCallBackError;
    
    /**
     * This member stores information, whether the interval configuration
     * should be sent to the server or not.
     */
    TBool   iSendConfigure;

    };

#endif /* CFLEXPERIODIC_H */
