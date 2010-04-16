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

#include <QTranslator>
#include <QLocale>
#include <QList>
#include <HbLabel>
#include <HbPushButton>
#include <HbMainWindow>
#include <HbGroupBox>
#include <HbListWidget>
#include <QHBoxLayout>
#include <HbScrollArea>
#include <HbView>
#include <HbIcon>
#include <HbAction>
#include <HbToolBar>
#include <HbStyleLoader>
#include <QGraphicsLinearLayout>
#include <QtCore/QSignalMapper>
#include <QtNetwork>
#include "connectionview.h"
#include "scrollareawidget.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "connectionviewTraces.h"
#endif



QTM_USE_NAMESPACE


ConnectionView::ConnectionView():
    mNetConfigurationManager(new QNetworkConfigurationManager(this)),
    mSignalMapper(new QSignalMapper(this)),
    mConnectionCount(0),
    mClosingTimer(0)
{
    OstTraceFunctionEntry0( CONNECTIONVIEW_CONNECTIONVIEW_ENTRY );
    // Install localization
    QTranslator *translator = new QTranslator(this);
    QString lang = QLocale::system().name(); 
    QString path = "Z:/resource/qt/translations/"; 
    translator->load("connectionview_" + lang, path);
    qApp->installTranslator(translator);
    
    // Register custom layout location
    bool registerStatus = HbStyleLoader::registerFilePath(":/layout/");
    Q_ASSERT(registerStatus);
    
    // Map the configurationChanged signal to a slot in order to get
    // information about the changes in the connections
    bool connectStatus = connect(
            mNetConfigurationManager,
            SIGNAL(configurationChanged(const QNetworkConfiguration&)),
            this,
            SLOT(handleConfigurationChanged(const QNetworkConfiguration&)));
    
    // Create the view and show it
    createView();
    OstTraceFunctionExit0( CONNECTIONVIEW_CONNECTIONVIEW_EXIT );
}


ConnectionView::~ConnectionView()
{
    OstTraceFunctionEntry0( DUP1_CONNECTIONVIEW_CONNECTIONVIEW_ENTRY );
    // other widgets are childs of this widget, so they will be
    // deleted along with mMainView. Add infolabel and scrollarea
    // again under mainlayout, since one of them is only there at the time
    // this way all the ui components are deleted at once
    mInfoLabel->setParentLayoutItem(mMainLayout);
    mScrollArea->setParentLayoutItem(mMainLayout);
    delete mMainView;
    OstTraceFunctionExit0( DUP1_CONNECTIONVIEW_CONNECTIONVIEW_EXIT );
}

/* !
     Quit the application when the timer is triggered
*/
void ConnectionView::timerEvent(QTimerEvent * /*event*/)
{
    killTimer(mClosingTimer);
    mClosingTimer = 0;
    qApp->quit();
}


/*!
    Builds the actual view for the main window
*/ 
void ConnectionView::createView()
{
    OstTraceFunctionEntry0( CONNECTIONVIEW_CREATEVIEW_ENTRY );
    
    // Create the mainView and the layout for the window
    mMainView = new HbView();
    addView(mMainView);
    setCurrentView(mMainView);
   
    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mMainLayout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mScrollArea = new HbScrollArea();
    mScrollArea->setScrollDirections(Qt::Vertical);
    mScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mMainLayout->addItem(mScrollArea);

    // Create the widgets and layouts for the scroll area
    ScrollAreaWidget *scrollContent = new ScrollAreaWidget();
    scrollContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mScrollArea->installEventFilter(scrollContent);
    
    mBoxLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mBoxLayout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollContent->setLayout(mBoxLayout);
    mMainView->setLayout(mMainLayout);
    mScrollArea->setContentWidget(scrollContent);
    
    // Create the toolbar and the disconnection action
    mToolBar = new HbToolBar();    
    HbIcon icon("qtg_mono_disconnect");
    mDisconnectAction = mToolBar->addAction(icon, hbTrId("txt_occ_button_disconnect_all"));
    mMainView->setToolBar(mToolBar);
    bool connectStatus = connect(
            mDisconnectAction,
            SIGNAL(triggered(bool)),
            this,
            SLOT(disconnectAll()));
    
    // label for informing the user that there are no connections
    mInfoLabel = new HbLabel;
    mInfoLabel->setObjectName("mInfoLabel");
    mInfoLabel->setPlainText(hbTrId("txt_occ_info_no_active_connections"));
    mInfoLabel->setAlignment(Qt::AlignCenter);
    
    // Create the actual groupboxes for all the active connections
    createGroupBoxesForConnections();  
    show();
    
    OstTraceFunctionExit0( CONNECTIONVIEW_CREATEVIEW_EXIT );
}


/*!
    Loops all the active cellular connections and creates groupboxes for those
*/ 
void ConnectionView::createGroupBoxesForConnections()
{
    OstTraceFunctionEntry0( CONNECTIONVIEW_CREATEGROUPBOXESFORCONNECTIONS_ENTRY );
    
    // disconnect all the old connections and then make the new connections
    mSignalMapper->disconnect();
    
    //Let's find active connections if any
    QList<QNetworkConfiguration> activeConfigurations;
    getActiveCellularConnections(activeConfigurations);
    
    // If there are no active connections, then the dialog is not shown to the user
    mConnectionCount = activeConfigurations.count();
    
    // Toolbar is shown only if there are more than one connections active
    if (mConnectionCount > 1) {
        mToolBar->show();
    } else {
        mToolBar->hide();
    }
    
    // if there are connections, then the label should be removed
    // and the connection boxes drawn
    if (mConnectionCount > 0) { 
        mMainLayout->removeItem(mInfoLabel);
        mInfoLabel->hide();
        mMainLayout->addItem(mScrollArea);

        for (int i=0; i<mConnectionCount; i++) {
            // Get the iap id and the iap name for the UI construction
            bool ok = true;
            int iapId = activeConfigurations[i].identifier().toInt(&ok);
            QString iapName = activeConfigurations[i].name();

            if (ok) {
                addGroupBox(iapId, iapName);
            }        
        }
        // This connection must be done only once, thus it's done here separately
        bool connectStatus = connect(
            mSignalMapper, 
            SIGNAL(mapped(int)), 
            this, 
            SLOT(disconnectSelectedIap(int)));
        
        // there are no connections, inform the user with the text 
    } else {
        mMainLayout->removeItem(mScrollArea);
        mInfoLabel->show();
        mMainLayout->addItem(mInfoLabel);
        mMainLayout->setAlignment(mInfoLabel, Qt::AlignCenter);
        // start the timer to close the application after 3 seconds
        if (mClosingTimer == 0) {
            mClosingTimer = startTimer(timerValue);
        }
    }
   
   OstTraceFunctionExit0( CONNECTIONVIEW_CREATEGROUPBOXESFORCONNECTIONS_EXIT );
}


/*
    Returns a list of all the available active cellular connections
*/
void ConnectionView::getActiveCellularConnections(QList<QNetworkConfiguration> &activeConfigurations)
{
    OstTraceFunctionEntry0( CONNECTIONVIEW_GETACTIVECELLULARCONNECTIONS_ENTRY );
    //Let's find active connections if any
    activeConfigurations.clear();

    QList<QNetworkConfiguration> configurations = 
        mNetConfigurationManager->allConfigurations(QNetworkConfiguration::Active);

    // Connection are open if they are in active state
    for (int i=0; i<configurations.count(); i++) {  
        if (configurations[i].type() == QNetworkConfiguration::InternetAccessPoint) {
            QString bearerName = configurations[i].bearerName();
            
            // WLAN connections are filtered out
            // TODO At some point QNetwork configuration will provide these constants
            if (bearerName==bearer2G || bearerName==bearerWCDMA || 
                bearerName==bearerHSPA || bearerName==bearerCDMA2000) {
                activeConfigurations.append(configurations[i]);
            }
        }
    }
    OstTraceFunctionExit0( CONNECTIONVIEW_GETACTIVECELLULARCONNECTIONS_EXIT );
}
    

/*!
    Handles the changes in the configurations whose states change to 
    active or discovered (meaning connected and disconnected)
*/ 
void ConnectionView::handleConfigurationChanged(const QNetworkConfiguration& config)
{
    OstTraceFunctionEntry0( CONNECTIONVIEW_HANDLECONFIGURATIONCHANGED_ENTRY );
    switch (config.state())
    {
        case QNetworkConfiguration::Undefined:
            //Nothing done
            break;
        case QNetworkConfiguration::Defined:
        case QNetworkConfiguration::Discovered:
        case QNetworkConfiguration::Active:
        {
            // If the Configuration change is to Discovered or Active, it means
            // that a connection was either opened or closed. Update the connections
            // on the ui
            QString bearerName = config.bearerName();
        
            // Update the view if there are changes in the cellular connections
            // TODO At some point QNetwork configuration will provide these constants
            if (bearerName==bearer2G || bearerName==bearerWCDMA || 
                bearerName==bearerHSPA || bearerName==bearerCDMA2000) {     
                // kill the application closing timer if it was started (there were no 
                // active connections)
                if (mClosingTimer != 0) {
                    killTimer(mClosingTimer);
                    mClosingTimer = 0;
                }
                clearGroupBoxView();
                // redraw the boxes
                createGroupBoxesForConnections();
                update();
            } 
            break;
        }
    }    
    OstTraceFunctionExit0( CONNECTIONVIEW_HANDLECONFIGURATIONCHANGED_EXIT );
}


/*!
    Adds one groupbox into the boxlayout for the given iap
*/ 
void ConnectionView::addGroupBox(int iapId, QString iapName)
{
    OstTraceFunctionEntry0( CONNECTIONVIEW_ADDGROUPBOX_ENTRY );
    // Create the groupbox and its layouts
    HbGroupBox *groupBox = new HbGroupBox();
    groupBox->setObjectName("groupBox");
    groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    groupBox->setHeading(hbTrId("txt_occ_subhead_connection_details"));
    groupBox->setCollapsable(true);
    
    // Create the disconnection button
    HbPushButton* button = new HbPushButton(
            hbTrId("txt_occ_button_disconnect"), mMainView);
    button->setSizePolicy(QSizePolicy::Preferred, 
                          QSizePolicy::Preferred, 
                          QSizePolicy::PushButton);
    
    // Create the horizontal layout for the labels
    QGraphicsLinearLayout *labelLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    labelLayout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    HbLabel *typeLabel = new HbLabel(hbTrId("txt_occ_list_name"));
    HbLabel *nameLabel = new HbLabel(iapName);
    nameLabel->setAlignment(Qt::AlignRight);
    labelLayout->addItem(typeLabel);
    labelLayout->addItem(nameLabel);
   
    HbWidget *labelWidget = new HbWidget();
    labelWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    labelWidget->setLayout(labelLayout);
    
    // a layout cannot be added directly to a layout, we need to use this container widget 
    HbWidget *boxWidget = new HbWidget();  
    boxWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QGraphicsLinearLayout *boxLayout = new QGraphicsLinearLayout(Qt::Vertical);
    boxLayout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    boxWidget->setLayout(boxLayout);

    // Add the label layout and the button into the groupbox
    boxLayout->addItem(labelWidget);
    boxLayout->addItem(button);
    boxLayout->setAlignment(button, Qt::AlignHCenter);
    groupBox->setContentWidget(boxWidget);
    groupBox->setCollapsed(true);
    
    // add the group box to the main layout
    mBoxLayout->addItem(groupBox);
    mBoxLayout->setAlignment(groupBox, Qt::AlignHCenter);
        
    // Expand the groupbox if connection count is 1 
    if (mConnectionCount == 1) {
        groupBox->setCollapsed(false);
    }
    
    // Connect the button to the mapper
    bool connectStatus = connect(
        button, 
        SIGNAL(clicked()), 
        mSignalMapper, 
        SLOT(map()));
    mSignalMapper->setMapping(button, iapId);
    OstTraceFunctionExit0( CONNECTIONVIEW_ADDGROUPBOX_EXIT );
}


/*!
    Closes the connection to the given iap
*/ 
void ConnectionView::disconnectSelectedIap(int iapId)
{   
    OstTraceFunctionEntry0( CONNECTIONVIEW_DISCONNECTSELECTEDIAP_ENTRY );
   
    // Get all the active connections 
    QList<QNetworkConfiguration> activeConfigurations;
    getActiveCellularConnections(activeConfigurations);
    int count = activeConfigurations.count();
            
    for (int i=0; i < count; i++) {
        bool ok = true;
        int loopedIapId = activeConfigurations[i].identifier().toInt(&ok);
        
        // if the looped iap matches the iap, stop the connection
        if (loopedIapId == iapId) {
            
            QNetworkSession *networkSession = new QNetworkSession(activeConfigurations[i], mMainView);

            bool connectStatus = connect(
                networkSession, 
                SIGNAL(opened()), 
                this, 
                SLOT(stopSessions()));
                      
            // This connection is done basically for memory handling
            // to be able to delete the session if one of the error signal is emitted
            connect(
                networkSession, 
                SIGNAL(error(QNetworkSession::SessionError)), 
                this, 
                SLOT(errorSessions(QNetworkSession::SessionError)));
            
            // open the session only if the connection succeeded, otherwise it's no use
            if (connectStatus) {
                // session needs to be opened in order to stop all the sessions of this configurations
                networkSession->open();
            } else {
                OstTrace1( CONNECTSTATUS, CONNECTIONVIEW_DISCONNECTSELECTEDIAP, "ConnectionView::disconnectSelectedIap failed;iapId=%d", iapId );
            }
   
            // the searched iap was found, stop looping
            break;
        }
    }
    OstTraceFunctionExit0( CONNECTIONVIEW_DISCONNECTSELECTEDIAP_EXIT );
}


/*!
    Closes all the connections to active iaps
*/ 
void ConnectionView::disconnectAll()
{
    OstTraceFunctionEntry0( CONNECTIONVIEW_DISCONNECTALL_ENTRY );
    
    // Get all the active connections 
    QList<QNetworkConfiguration> activeConfigurations;
    getActiveCellularConnections(activeConfigurations);
    int count = activeConfigurations.count();
    
    for (int i=0; i < count; i++) {
        QNetworkSession *networkSession = new QNetworkSession(activeConfigurations[i], mMainView);
        bool connectStatus = connect(
            networkSession, 
            SIGNAL(opened()), 
            this, 
            SLOT(stopSessions()));
        // This connection is done basically for memory handling
        // to be able to delete the session if one of the error signal is emitted
        connect(
            networkSession, 
            SIGNAL(error(QNetworkSession::SessionError)), 
            this, 
            SLOT(errorSessions(QNetworkSession::SessionError)));
        
        // open the session only if the connection succeeded, otherwise it's no use
        if (connectStatus) {
            // session needs to be opened in order to stop all the sessions of this configurations
            networkSession->open();
        }
    }
    OstTraceFunctionExit0( CONNECTIONVIEW_DISCONNECTALL_EXIT );
}


void ConnectionView::stopSessions() 
{
    OstTraceFunctionEntry0( CONNECTIONVIEW_STOPSESSIONS_ENTRY );

    // get the sender-session and stop the sessions
    QObject *senderObject = QObject::sender();
    if (senderObject != NULL) {
        QNetworkSession *session = qobject_cast<QNetworkSession *>(senderObject);
        session->stop();
        session->deleteLater();
    }
    OstTraceFunctionExit0( CONNECTIONVIEW_STOPSESSIONS_EXIT );
}

void ConnectionView::errorSessions(QNetworkSession::SessionError) 
{
    OstTraceFunctionEntry0( DUP2_CONNECTIONVIEW_ERRORSESSIONS_ENTRY );

    // something went wrong in session opening
    // get the sender-session and delete the sender
    QObject *senderObject = QObject::sender();
    if (senderObject != NULL) {
        senderObject->deleteLater();
    }
    OstTraceFunctionExit0( DUP1_CONNECTIONVIEW_ERRORSESSIONS_EXIT );
}

/*!
    Removes the items from the boxlayout in order to be costructed again.
*/ 
void ConnectionView::clearGroupBoxView()
{
    OstTraceFunctionEntry0( CONNECTIONVIEW_CLEARGROUPBOXVIEW_ENTRY );
    // Loop all the groupbox items through and remove them from the view
    int count = mBoxLayout->count();
    for (int i = 0; i < count; i++ )  {
        // store the pointer to the item to be able to delete it
        QGraphicsLayoutItem *item = mBoxLayout->itemAt(0);
        mBoxLayout->removeAt(0);
        delete item;
    }
    OstTraceFunctionExit0( CONNECTIONVIEW_CLEARGROUPBOXVIEW_EXIT );
}




