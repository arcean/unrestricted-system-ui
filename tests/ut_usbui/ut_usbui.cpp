/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of system-ui.
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
#include <MApplication>
#include <MSceneWindow>
#include <MNotification>
#include <MDialog>
#include <QTest>

#define DEBUG
#include <debug.h>

#include "ut_usbui.h"
#include "usbmode_stub.h"

using namespace Maemo;

int   argc = 1;
char *argv[] = {
    (char *) "./ut_usbui",
    NULL };

MApplication    *m_App;

void
Ut_UsbUi::initTestCase ()
{
    m_App = new MApplication (argc, argv);
    m_subject = new UsbUi;
    m_subject->m_logic->setMode(QmUSBMode::Undefined);

    QTest::qWait (100);
}

void
Ut_UsbUi::cleanupTestCase ()
{
    delete m_App;
}

static bool dialog_visible;

void
MDialog::appear (MSceneWindow::DeletionPolicy policy)
{
    SYS_DEBUG ("");
    Q_UNUSED (policy);
    dialog_visible = true;
}

void
MSceneWindow::disappear ()
{
    SYS_DEBUG ("");
    dialog_visible = false;
}

void
Ut_UsbUi::show_hide_dialog ()
{
    dialog_visible = false;
    // Set mode to ask
    m_subject->m_logic->setDefaultMode(QmUSBMode::Ask);

    // Emit the ask signal...
    m_subject->m_logic->test_emitModeChanged(QmUSBMode::Ask);
    QTest::qWait (200);

    QCOMPARE (dialog_visible, true);

    // Emit the disconnect signal
    m_subject->m_logic->test_emitModeChanged(QmUSBMode::Disconnected);
    QTest::qWait (200);

    QCOMPARE (dialog_visible, false);

    // Emit the moderequest signal
    m_subject->m_logic->test_emitModeChanged(QmUSBMode::ModeRequest);
    QTest::qWait (200);

    QCOMPARE (dialog_visible, true);

    // Emit the disconnect signal
    m_subject->m_logic->test_emitModeChanged(QmUSBMode::Disconnected);
    QTest::qWait (200);

    QCOMPARE (dialog_visible, false);
}

void
Ut_UsbUi::usbnotifications ()
{
    // Init to some known state ...
    m_subject->m_logic->test_emitModeChanged(QmUSBMode::Connected);
    if (m_subject->m_notification)
        m_subject->m_notification->remove ();

    m_subject->m_logic->test_emitModeChanged(QmUSBMode::OviSuite);
    QTest::qWait (200);

    QCOMPARE (m_subject->m_notification->isPublished (), true);

    if (m_subject->m_notification)
        m_subject->m_notification->remove ();

    m_subject->m_logic->test_emitModeChanged(QmUSBMode::MassStorage);
    QTest::qWait (200);

    QCOMPARE (m_subject->m_notification->isPublished (), true);
}

void
Ut_UsbUi::testdialogbuttoncallbacks ()
{
    // Go to connected state...
    m_subject->m_logic->test_emitModeChanged(QmUSBMode::Connected);
    QTest::qWait (100);

    // Call the Ovi Suite callback function...
    m_subject->OviSuiteSelected ();

    QVERIFY (m_subject->m_logic->getMode () == QmUSBMode::OviSuite);

    // Call the Mass Storage callback function...
    m_subject->MassStorageSelected ();

    QVERIFY (m_subject->m_logic->getMode () == QmUSBMode::MassStorage);
}

QTEST_APPLESS_MAIN (Ut_UsbUi)
