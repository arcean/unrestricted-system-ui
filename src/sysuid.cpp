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
/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */

#include <MApplicationWindow>
#include <MLocale>
#include <MTheme>
#include <MLocale>

#include <QDBusConnection>
#include <QDebug>

#include "usbui.h"
#include "sysuid.h"
#include "sysuidrequest.h"
#include "systemuigconf.h"
#include "batterybusinesslogic.h"
#include "batterybusinesslogicadaptor.h"
#include "lockscreenbusinesslogic.h"
#include "lockscreenbusinesslogicadaptor.h"
#include "shutdownbusinesslogic.h"
#include "statusarearenderer.h"
#include "notificationmanager.h"
#include "mcompositornotificationsink.h"
#include "ngfnotificationsink.h"
#include "contextframeworkcontext.h"

#undef DEBUG
#define WARNING
#include "debug.h"

#include <QX11Info>
#include <X11/Xutil.h>

#define TRANSLATION_CATALOG "systemui"

namespace
{
const QString themeDir = "/usr/share/themes/base/meegotouch/sysuid/";
const QString styleDir = themeDir + "style/";
const QString svgDir = themeDir + "svg/";
}

Sysuid* Sysuid::m_Sysuid = NULL;

Sysuid::Sysuid () : QObject (),
        m_applicationWindow(new MApplicationWindow)
{
    SYS_DEBUG ("Starting sysuidaemon");

    m_Sysuid = this;

    MTheme::addPixmapDirectory (themeDir, M::Recursive);
    MTheme::loadCSS (styleDir + "sysuid.css");
    MTheme::loadCSS (styleDir + "unlockscreen.css");

    // Load translation of System-UI
    retranslate ();

    m_applicationWindow->setTranslucentBackground (true);
    m_applicationWindow->setWindowOpacity (0.0);
    m_applicationWindow->setWindowFlags (Qt::FramelessWindowHint |
                                         Qt::CustomizeWindowHint |
                                         Qt::WindowStaysOnTopHint);
    m_SystemUIGConf   = new SystemUIGConf (this);
    m_ShutdownLogic   = new ShutdownBusinessLogic (this);
    m_BatteryLogic    = new BatteryBusinessLogic (m_SystemUIGConf, this);
    m_UsbUi           = new UsbUi (this);

    m_notificationManager = new NotificationManager(3000);
    m_compositorNotificationSink = new MCompositorNotificationSink;
    m_ngfNotificationSink = new NGFNotificationSink;

    // D-Bus registration and stuff
    new ShutdownBusinessLogicAdaptor (this, m_ShutdownLogic);
    new BatteryBusinessLogicAdaptor (this, m_BatteryLogic);

    QDBusConnection bus = QDBusConnection::sessionBus ();
    if (!bus.registerService (dbusService ())) {
        qCritical () << Q_FUNC_INFO << "failed to register dbus service";
        abort();
    }
    if (!bus.registerObject (dbusPath (), sysuid ())) {
        qCritical () << Q_FUNC_INFO << "failed to register dbus object";
        abort();
    }
    // Show status area window when sysui daemon starts
    m_statusAreaWindow = new StatusAreaRenderer;

    connect (m_statusAreaWindow, SIGNAL (statusIndicatorMenuVisibilityChanged (bool)),
             m_compositorNotificationSink, SLOT (setDisabled (bool)));
    // Connect the notification signals for the compositor notification sink
    connect (m_notificationManager, SIGNAL (notificationUpdated (const Notification &)),
             m_compositorNotificationSink, SLOT (addNotification (const Notification &)));
    connect (m_notificationManager, SIGNAL (notificationRemoved (uint)),
             m_compositorNotificationSink, SLOT (removeNotification (uint)));
    connect (m_compositorNotificationSink, SIGNAL (notificationRemovalRequested (uint)),
             m_notificationManager, SLOT (removeNotification (uint)));
    connect (m_notificationManager, SIGNAL (notificationRestored (const Notification &)),
             m_compositorNotificationSink, SIGNAL (notificationAdded (Notification)));

    // Connect the notification signals for the feedback notification sink
    connect (m_notificationManager, SIGNAL (notificationUpdated (const Notification &)),
             m_ngfNotificationSink, SLOT (addNotification (const Notification &)));
    connect (m_notificationManager, SIGNAL (notificationRemoved (uint)),
              m_ngfNotificationSink, SLOT (removeNotification (uint)));
    connect (m_applicationWindow, SIGNAL (orientationChangeFinished (const M::Orientation &)),
             this, SIGNAL (orientationChangeFinished (const M::Orientation &)));

    // Subscribe to a context property for getting information about the video recording status
    ContextFrameworkContext context;
    useMode = QSharedPointer<ContextItem> (context.createContextItem ("Use.Mode"));
    connect (useMode.data (), SIGNAL (contentsChanged ()),
             this, SLOT (applyUseMode ()));
    applyUseMode ();

    // Restore persistent notifications after all the signal connections are made to the notification sinks
    m_notificationManager->restorePersistentData ();

    /*
     * The screen locking is implemented in this separate class, because it is
     * bound to the system bus (MCE wants to contact us on the system bus).
     */
    new SysUidRequest;
}

Sysuid::~Sysuid ()
{
    m_Sysuid = NULL;
    delete m_statusAreaWindow;
    delete m_applicationWindow;
}

Sysuid* Sysuid::sysuid ()
{
    return m_Sysuid;
}

QString Sysuid::dbusService ()
{
    return QString ("com.nokia.systemui");
}

QString Sysuid::dbusPath ()
{
    return QString ("/");
}

/*!
 * Please note that in the libdui 0.19.4 manipulating theh MLocale in this
 * function might cause an endless recursion. I added a protection for brake the
 * recursion.
 *
 * FIXME: Once MLocale is working as it should be this function could be
 * eliminated.
 */
void Sysuid::retranslate ()
{
    static bool      running = false;

    SYS_DEBUG ("*** running = %s", running ? "true" : "false");
    if (running)
        return;
    running = true;

    MLocale        locale;

    SYS_DEBUG (" lang = %s", SYS_STR (locale.language ()));

    // Install engineering english
    locale.installTrCatalog (TRANSLATION_CATALOG ".qm");
    // Install real translation
    locale.installTrCatalog (TRANSLATION_CATALOG);

    MLocale::setDefault (locale);

    running = false;
}

NotificationManager &Sysuid::notificationManager ()
{
    return *m_notificationManager;
}

MCompositorNotificationSink& Sysuid::compositorNotificationSink ()
{
    return *m_compositorNotificationSink;
}

MApplicationWindow &Sysuid::applicationWindow ()
{
    return *m_applicationWindow;
}

void Sysuid::applyUseMode ()
{
    bool videoRecording =
        useMode->value ().toString () == "recording";

    m_compositorNotificationSink->setApplicationEventsEnabled (!videoRecording);
    m_ngfNotificationSink->setApplicationEventsEnabled (!videoRecording);
}

