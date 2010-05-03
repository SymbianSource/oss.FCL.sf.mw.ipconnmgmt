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
* Description: 
*
*/

#ifndef __CONNECTIONVIEW_H__
#define __CONNECTIONVIEW_H__

#include <QObject>
#include <HbMainWindow>
#include <HbDialog>
#include <QStringList>
#include <HbWidget>
#include <QtNetwork>
#include <QGraphicsLinearLayout>
#include <HbGroupBox>
#include <HbScrollArea>
#include <HbToolBar>
#include <HbLabel>
#include <qnetworksession.h>
#include <qnetworkconfigmanager.h>

QTM_USE_NAMESPACE

class QSignalMapper;

const QString bearer2G = "2G";
const QString bearerWCDMA = "WCDMA";
const QString bearerHSPA = "HSPA";
const QString bearerCDMA2000 = "CDMA2000";
const int timerValue = 3000;

class ConnectionView: public HbMainWindow
    {
    Q_OBJECT
    
    public:
        /* Constructor */
        ConnectionView();
        
        /* Destructor */
        ~ConnectionView();
        
        /**
         * This is the re-implemented timerEvent function used to trigger
         * closing of the application. It is received every time the set timer expires.
         */
        void timerEvent(QTimerEvent * /*event*/);
        
    private slots: 
        /* Handles the changes in the cellular data connections */
        void handleConfigurationChanged(const QNetworkConfiguration& config);
       
        /* Disonnects the iap given as a parameter */
        void disconnectSelectedIap(int iapId);
       
        /* Disconnects all the cellular iaps */
        void disconnectAll();
       
        /* 
         * Does the actual stopping of the sessions, once the session's 
         * opened-signal is received 
         */
        void stopSessions();
        
        /* Deletes the sender when the session opening fails */
        void errorSessions(QNetworkSession::SessionError);

    private:     
        
        /* Constructs the view */
        void createView();
        
        /* 
         * Creates group boxes into the view for all the active cellular 
         * connections.
         */
        void createGroupBoxesForConnections();
        
        /*
         * Updates the list of active cellular connections into the
         * list received as a reference parameter.
         */
        void getActiveCellularConnections(QList<QNetworkConfiguration> &activeConfigurations);
        
        /*
         * Creates a single groupbox for the iap given as a paremeters. Attaches the
         * group boxes disconnect button to the disconnectSelectedIap slot
         */
        void addGroupBox(int iapId, QString iapName);
        
        /* Clears the groupboxes from the view */
        void clearGroupBoxView();

    private:
        /* Pointer to network configuration manager */
        QNetworkConfigurationManager* mNetConfigurationManager;
        
        /* Pointer to the main view of the window */
        HbView *mMainView;
        
        /* Pointer to the secondary view of the window */
        HbView *mNoConnView;
        
        /* The main vertical layout which contains the group box container */
        QGraphicsLinearLayout* mMainLayout;
        
        /* The scroll area of the view */
        HbScrollArea *mScrollArea;
        
        /* The layout into which the groupboxes are inserted */
        QGraphicsLinearLayout *mBoxLayout;
        
        /* 
         * Pointer to the signalmapper used in mapping the disconnect buttons clicks
         * to the disconnectSelectedIap function
         */
        QSignalMapper *mSignalMapper;
        
        /* The amount of active cellular data connections */
        int mConnectionCount;
        
        /* The toolbar on the view */
        HbToolBar *mToolBar;
        
        /* The toolbar action which can be used to disconnect all the connections */
        HbAction *mDisconnectAction;
        
        /* The id of the timer used to close the application */
        int mClosingTimer;
        
        /* definition for unit test purposes */
        friend class TestConnectionView;
    
    };


#endif // __CONNECTIONVIEW_H__
