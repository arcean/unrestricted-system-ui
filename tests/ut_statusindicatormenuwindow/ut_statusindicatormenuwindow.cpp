/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of systemui.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/
#include "ut_statusindicatormenuwindow.h"
#include "statusindicatormenuwindow.h"
#include "pluginlist_stub.h"
#include <MApplication>
#include <QtTest/QtTest>
#include <X11/Xutil.h>
#include <MOverlay>
#include <MButton>
#include <QGraphicsLinearLayout>

// X stubs to avoid crashes
void XSetWMProperties(Display *, Window, XTextProperty *, XTextProperty *, char **, int, XSizeHints *, XWMHints *, XClassHint *)
{
}

QPair<void*, bool> gSetVisible(0, false);
void QWidget::setVisible(bool visible)
{
    gSetVisible.first = this;
    gSetVisible.second = visible;
}

void Ut_StatusIndicatorMenuWindow::init()
{
    statusIndicatorMenuWindow = new StatusIndicatorMenuWindow;
    gSetVisible.first = 0;
    gSetVisible.second = false;
}

void Ut_StatusIndicatorMenuWindow::cleanup()
{
    delete statusIndicatorMenuWindow;
}

void Ut_StatusIndicatorMenuWindow::initTestCase()
{
    int argc = 1;
    char *app_name = (char *)"./ut_statusarea";
    app = new MApplication(argc, &app_name);
}

void Ut_StatusIndicatorMenuWindow::cleanupTestCase()
{
    delete app;
}

void Ut_StatusIndicatorMenuWindow::testMakeVisible()
{
    statusIndicatorMenuWindow->hide();
    QVERIFY(gSetVisible.first == statusIndicatorMenuWindow && !gSetVisible.second);
    statusIndicatorMenuWindow->makeVisible();
    QVERIFY(gSetVisible.first == statusIndicatorMenuWindow && gSetVisible.second);
}

void Ut_StatusIndicatorMenuWindow::testCloseButtonOverlay()
{
    QVERIFY(statusIndicatorMenuWindow->closeButtonOverlay);
    QCOMPARE(statusIndicatorMenuWindow->closeButtonOverlay->objectName(), QString("closeButtonOverlay"));

    // Check the number of children of overlay and its layout
    QCOMPARE(statusIndicatorMenuWindow->closeButtonOverlay->childItems().count(), 4);
    QCOMPARE(statusIndicatorMenuWindow->closeButtonOverlay->layout()->count(), 3);

    // Check the type of the children of the overlay
    QVERIFY(dynamic_cast<EventEaterWidget *> (statusIndicatorMenuWindow->closeButtonOverlay->layout()->itemAt(0)));
    QVERIFY(dynamic_cast<MButton *> (statusIndicatorMenuWindow->closeButtonOverlay->layout()->itemAt(1)));
    QVERIFY(dynamic_cast<EventEaterWidget *> (statusIndicatorMenuWindow->closeButtonOverlay->layout()->itemAt(2)));

    // Test status indicator menu window visibility
    QVERIFY(gSetVisible.first != statusIndicatorMenuWindow && !gSetVisible.second);
    connect(this, SIGNAL(clicked()), statusIndicatorMenuWindow, SLOT(hide()));
    emit clicked();
    QVERIFY(gSetVisible.first == statusIndicatorMenuWindow && !gSetVisible.second);

    // Test that status indicator menu close overlay accept events outside close button
    EventEaterWidget *overlayWidget = new EventEaterWidget;
    QGraphicsSceneMouseEvent event;
    overlayWidget->mousePressEvent(&event);
    QCOMPARE(event.isAccepted(), true);
    delete overlayWidget;
}

QTEST_APPLESS_MAIN(Ut_StatusIndicatorMenuWindow)
