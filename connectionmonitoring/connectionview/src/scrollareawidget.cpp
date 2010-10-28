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

#include <QGraphicsItem>
#include <QEvent>
#include <QGraphicsSceneResizeEvent>
#include "scrollareawidget.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "scrollareawidgetTraces.h"
#endif


ScrollAreaWidget::ScrollAreaWidget(QGraphicsItem *parent):
    QGraphicsWidget(parent)
{
    OstTraceFunctionEntry0( SCROLLAREAWIDGET_SCROLLAREAWIDGET_ENTRY ); 
    OstTraceFunctionExit0( SCROLLAREAWIDGET_SCROLLAREAWIDGET_EXIT );
}

ScrollAreaWidget::~ScrollAreaWidget() 
{
    OstTraceFunctionEntry0( DUP1_SCROLLAREAWIDGET_SCROLLAREAWIDGET_ENTRY );
    OstTraceFunctionExit0( DUP1_SCROLLAREAWIDGET_SCROLLAREAWIDGET_EXIT );
}

/*!
    Filters the resizing events to get the widget resized when switching to 
    landscape and back
*/
bool ScrollAreaWidget::eventFilter(QObject *object, QEvent *event)
{
    OstTraceFunctionEntry0( SCROLLAREAWIDGET_EVENTFILTER_ENTRY );
    
    if(event->type() == QEvent::GraphicsSceneResize){
        resize(((QGraphicsSceneResizeEvent*)event)->newSize());
        OstTraceFunctionExit0( SCROLLAREAWIDGET_EVENTFILTER_EXIT );
        return true;
    } else {
        return QGraphicsWidget::eventFilter(object, event);
    }
}
