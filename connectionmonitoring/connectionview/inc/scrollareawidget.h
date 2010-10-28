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

#ifndef SCROLLAREAWIDGET_H_
#define SCROLLAREAWIDGET_H_

#include <QObject>
#include <QGraphicsWidget>
#include <qnetworkconfigmanager.h>

class ScrollAreaWidget: public QGraphicsWidget
{
    Q_OBJECT
    
public:
    /* Constructor */
    ScrollAreaWidget(QGraphicsItem *parent = 0);
    
    /* Destructor */
    ~ScrollAreaWidget();
    
    /* 
     * Function to filter all the resizing events to get the widget to
     * resize itself when switched to landscape and back
     */
    bool eventFilter(QObject *object, QEvent *event);
};

#endif /* SCROLLAREAWIDGET_H_ */
