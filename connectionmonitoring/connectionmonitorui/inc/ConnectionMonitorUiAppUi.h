/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Application UI class
*     
*
*/


#ifndef CONNECTIONMONITORUIAPPUI_H
#define CONNECTIONMONITORUIAPPUI_H

// INCLUDES
#include <aknViewAppUi.h>
#include <aknnavide.h>
#include <rconnmon.h>

#include "ConnectionMonitorUiLogger.h"

// FORWARD DECLARATIONS
class CConnectionMonitorUiContainer;
class CConnectionArray;
class CConnectionInfoBase;

class CFeatureManagerWrapper;
class CActiveWrapper;

// CONSTANTS

/**
* For iPeriodic Timer. The interval between events generated
* after the initial delay, in microseconds.
*/
LOCAL_D const TInt KTickInterval = 1000000;


// CLASS DECLARATION

/**
*
*/
class MActiveEventObserverInterface
    {
    public:
        /**
        * Called if MConnectionMonitorObserver is raised EventL event. 
        * @param aConnMonEvent The event information.
        * @param aIndex The index of connection in the iConnectionArray. 
        */
        virtual void OnEventL( const CConnMonEventBase& aConnMonEvent,
                               const TInt aIndex ) = 0;

        /**
        * Called by iPeriodic. It refreshes details of connections.
        */
        virtual void OnTimerEventL() = 0;
    };
  
/**
*
*/    
class MTimerExpiredObserverInterface     
    {
    public:
        /**
        * Called by iPeriodic.
        */
        virtual void OnTimerExpiredL() = 0;   
        
        /**
        * Gives back Timer interval.
        */
        virtual TInt GetTickInerval() const = 0; 
    };

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* - view architecture
* - status pane
* 
*/
class CConnectionMonitorUiAppUi : public CAknViewAppUi,
                                  public MConnectionMonitorObserver,
                                  public MTimerExpiredObserverInterface
    {
    public: // Constructors and destructor
        /**
        * EPOC default constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        ~CConnectionMonitorUiAppUi();

    public:
        /**
        * Returns pointer to AppUi object.
        */      
        static CConnectionMonitorUiAppUi* Static();

        /**
        * from MConnectionMonitorObserver
        */
        void EventL( const CConnMonEventBase &aConnMonEvent );

    public:
        /**
        * Register and start connection monitor engine callback notification,
        * and starts periodic timer.
        * @param aObserver Observer object
        */
        void StartConnEventNotification( 
                                    MActiveEventObserverInterface* aObserver );

        /**
        * Stops connection monitor notification and periodic timer
        * @param aObserver Observer object
        */
        void StopConnEventNotification( 
                                    MActiveEventObserverInterface* aObserver );

        /**
        * Starts iPeriodic.
        * @param aInterval Refresh interval
        */
        void StartTimerL( const TInt aInterval = KTickInterval );

        /**
        * Stops iPeriodic.
        */
        void StopTimer();

        /**
        * Timer callback of iPeridoc.
        */
        static TInt Tick( TAny* aObject );
        
        /**
        * called by Tick( TAny* aObject ).
        */
        TInt Tick();        

        /**
        * Delete and remove a connection from the View
        * @param aConnId the id of the currect selected connection
        * @param aConnInfo pointer to the current selected connection
        */
        void DeleteDetailsView( 
                            TInt aConnId, CConnectionInfoBase* aConnInfo );

#ifdef RD_CONTROL_PANEL
    
        /**
        * This function is used for querying whether the application
        * is launched in embedded mode or not.
        * @return ETrue:  The application is launched in embedded mode.
        *         EFalse: The application is launched in standalone mode.
        */
        TBool IsEmbedded() const;

#endif // RD_CONTROL_PANEL        

    protected:
        /**
        * from CCoeAppUi, for icon skinning
        */
        void HandleForegroundEventL( TBool aForeground );

    private:
        /**
        * From MEikMenuObserver
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * From CEikAppUi, takes care of command handling.
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CEikAppUi, handles key events.
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event. 
        * @return Response code ( EKeyWasConsumed, EKeyWasNotConsumed ). 
        */
        virtual TKeyResponse HandleKeyEventL(
                                const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * Called by Tick.
        */
        void OnTimerExpiredL( );
        
        /**
        * Gives back Timer interval.
        */        
        TInt GetTickInerval() const;         
        
        /**
        * Call one after the other RefreshDetails() function of connections
        * which are contained by iConnectionArray.
        */
        void RefreshAllConnectionsL();

        /**
        * Creates a CConnectionInfBase class, the given connection id. 
        * The Dynamic type of the returned class depends on the bearer type 
        * of the connection.
        * @params aConnectionId the id of the connection
		* @params aBearerType the bearer type of the connection
        * @return CConnectionInfBase instance, The Dynamic type of the returned
        * class depends on the bearer type of the connection.
        */
        CConnectionInfoBase* CreateConnectionInfoL( TUint aConnectionId, TConnMonBearerType aBearerType );
            
        /**
        * Initialize connection array
        */
        void InitializeConnectionArrayL();
        
        /**
        * Gets the IAP of Easy Wlan access point
        */
        void GetEasyWlanIAPNameL();

        /**
        * Shows connection summary note.
        * @param aConnection
        */        
        void ShowConnectionSummaryInformationNoteL( 
        							const CConnectionInfoBase* aConnection );
    private: // Data

        /**
        * For refresh details of connections.
        */
        RConnectionMonitor              iMonitor;       
       
        /**
        * Contains CConnectionInfoBase instances, which are represents 
        * the connections in the system.
        */
        CConnectionArray*               iConnectionArray; // Owned
        
        /**
        * Timer, to refresh details of connections
        */
        CPeriodic*                      iPeriodic;  // Owned
        
        /**
        * Actual observer instance. This observer handles refreshing details
        * of connection on the screen.
        */
        MActiveEventObserverInterface*  iObserver;  // Not owned
                
        /**
        * CFeatureManagerWrapper wraps FeatureManager to eliminate maintenance 
        * of existance of FeatureManager.
        * Owned.
        */
        CFeatureManagerWrapper*         iFeatureManagerWrapper;
        
        /**
        * The active object for info. refreshing. Owned.
        */
        CActiveWrapper*                 iActiveWrapper;

        /**
        * the name of Easy Wlan access point it there is, otherwise NULL
        */
        HBufC*                          iEasyWlanIAPName;        
                
        /**
        * A pointer to actual timer observer instance
        */
        MTimerExpiredObserverInterface* iTimerObserver;

        /**
        * Should the Main View be activeted during HandleForegroundEventL() method
        */
        TBool activateMainView;
        
        /**
         * An array to hold the connectionIds in situations when creating
         * connectionInfo object fails
         */
        RArray<TUint> iNewConnectionIdArray;
        
        /**
         * Connections waiting for first event
         */
        RArray<TInt> iConnectionsWaitingFirstEvent;
    };

#endif  // CONNECTIONMONITORUIAPPUI_H

// End of File
