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

#include "ut_dbusinterfacenotificationsource.h"
#include "dbusinterfacenotificationsource.h"
#include "dbusinterfacenotificationsourceadaptor.h"
#include "genericnotificationparameterfactory.h"
#include "notificationwidgetparameterfactory.h"
#include "notification_stub.h"
#include "notificationgroup_stub.h"
#include "notificationmanager_stub.h"

// DBusInterfaceNotificationSourceAdaptor stubs (used by NotificationManager)
DBusInterfaceNotificationSourceAdaptor::DBusInterfaceNotificationSourceAdaptor(DBusInterfaceNotificationSource *parent) : QDBusAbstractAdaptor(parent)
{
}

DBusInterfaceNotificationSourceAdaptor::~DBusInterfaceNotificationSourceAdaptor()
{
}

bool DBusInterfaceNotificationSourceAdaptor::removeNotification(uint, uint)
{
    return true;
}

uint DBusInterfaceNotificationSourceAdaptor::addNotification(uint, uint, const QString &)
{
    return 0;
}

uint DBusInterfaceNotificationSourceAdaptor::addNotification(uint, uint, const QString &, const QString &, const QString &, const QString &, const QString &, uint)
{
    return 0;
}

bool DBusInterfaceNotificationSourceAdaptor::updateNotification(uint, uint, const QString &)
{
    return true;
}

bool DBusInterfaceNotificationSourceAdaptor::updateNotification(uint, uint, const QString &, const QString &, const QString &, const QString &, const QString &, uint)
{
    return true;
}

uint DBusInterfaceNotificationSourceAdaptor::addGroup(uint, const QString &)
{
    return 0;
}

uint DBusInterfaceNotificationSourceAdaptor::addGroup(uint, const QString &, const QString &, const QString &, const QString &, const QString &, uint)
{
    return 0;
}

bool DBusInterfaceNotificationSourceAdaptor::removeGroup(uint, uint)
{
    return true;
}

bool DBusInterfaceNotificationSourceAdaptor::updateGroup(uint, uint, const QString &)
{
    return true;
}

bool DBusInterfaceNotificationSourceAdaptor::updateGroup(uint, uint, const QString &, const QString &, const QString &, const QString &, const QString &, uint)
{
    return true;
}

uint DBusInterfaceNotificationSourceAdaptor::notificationUserId()
{
    return 1;
}

QList<uint> DBusInterfaceNotificationSourceAdaptor::notificationIdList(uint notificationUserId)
{
    QList<uint> tmp;
    Q_UNUSED(notificationUserId);
    return tmp;
}

QList < MNotificationProxy >  DBusInterfaceNotificationSourceAdaptor::notificationList(uint)
{
    QList<MNotificationProxy> l;
    return l;
}

QList<MNotificationGroupProxy> DBusInterfaceNotificationSourceAdaptor::notificationGroupList(uint)
{
    QList<MNotificationGroupProxy> l;
    return l;
}

void Ut_DBusInterfaceNotificationSource::initTestCase()
{
}

void Ut_DBusInterfaceNotificationSource::cleanupTestCase()
{
}

void Ut_DBusInterfaceNotificationSource::init()
{
    gDefaultNotificationManagerStub.stubReset();

    manager = new NotificationManager();
    source = new DBusInterfaceNotificationSource(*manager);
}

void Ut_DBusInterfaceNotificationSource::cleanup()
{
    delete source;
    delete manager;
}

void Ut_DBusInterfaceNotificationSource::testAddNotification()
{
    source->addNotification(1, 2, "event");
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("addNotification").parameter<uint>(0), (uint)1);
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("addNotification").parameter<uint>(2), (uint)2);
    NotificationParameters params = gDefaultNotificationManagerStub.stubLastCallTo("addNotification").parameter<NotificationParameters>(1);
    QCOMPARE(params.value(GenericNotificationParameterFactory::eventTypeKey()), QVariant("event"));

    source->addNotification(3, 4, "event", "summary", "body", "action", "imageURI", 1);
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("addNotification").parameter<uint>(0), (uint)3);
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("addNotification").parameter<uint>(2), (uint)4);
    params = gDefaultNotificationManagerStub.stubLastCallTo("addNotification").parameter<NotificationParameters>(1);
    QCOMPARE(params.value(GenericNotificationParameterFactory::eventTypeKey()), QVariant("event"));
    QCOMPARE(params.value(GenericNotificationParameterFactory::countKey()), QVariant("1"));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::summaryKey()), QVariant("summary"));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::bodyKey()), QVariant("body"));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::imageIdKey()), QVariant("imageURI"));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::actionKey()), QVariant("action"));

    source->addNotification(5, 6, "event", "summary", "body", "action", "imageURI", 42);
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("addNotification").parameter<uint>(0), (uint)5);
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("addNotification").parameter<uint>(2), (uint)6);
    params = gDefaultNotificationManagerStub.stubLastCallTo("addNotification").parameter<NotificationParameters>(1);
    QCOMPARE(params.value(GenericNotificationParameterFactory::countKey()), QVariant("42"));
}

void Ut_DBusInterfaceNotificationSource::testUpdateNotification()
{
    source->updateNotification(1, 2, "event");
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("updateNotification").parameter<uint>(0), (uint)1);
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("updateNotification").parameter<uint>(1), (uint)2);
    NotificationParameters params = gDefaultNotificationManagerStub.stubLastCallTo("updateNotification").parameter<NotificationParameters>(2);
    QCOMPARE(params.value(GenericNotificationParameterFactory::eventTypeKey()), QVariant("event"));

    source->updateNotification(3, 4, "event", "summary", "body", "action", "imageURI", 42);
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("updateNotification").parameter<uint>(0), (uint)3);
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("updateNotification").parameter<uint>(1), (uint)4);

    params = gDefaultNotificationManagerStub.stubLastCallTo("updateNotification").parameter<NotificationParameters>(2);
    QCOMPARE(params.value(GenericNotificationParameterFactory::eventTypeKey()), QVariant("event"));
    QCOMPARE(params.value(GenericNotificationParameterFactory::countKey()), QVariant("42"));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::summaryKey()), QVariant("summary"));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::bodyKey()), QVariant("body"));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::imageIdKey()), QVariant("imageURI"));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::actionKey()), QVariant("action"));
}

void Ut_DBusInterfaceNotificationSource::testRemoveNotification()
{
    source->removeNotification(1, 2);
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("removeNotification").parameter<uint>(0), (uint)1);
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("removeNotification").parameter<uint>(1), (uint)2);
}

void Ut_DBusInterfaceNotificationSource::testAddGroup()
{
    source->addGroup(1, "event");
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("addGroup").parameter<uint>(0), (uint)1);
    NotificationParameters params = gDefaultNotificationManagerStub.stubLastCallTo("addGroup").parameter<NotificationParameters>(1);
    QCOMPARE(params.value(GenericNotificationParameterFactory::eventTypeKey()), QVariant("event"));

    source->addGroup(2, "event", "summary", "body", "action", "imageURI", 1);
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("addGroup").parameter<uint>(0), (uint)2);

    params = gDefaultNotificationManagerStub.stubLastCallTo("addGroup").parameter<NotificationParameters>(1);
    QCOMPARE(params.value(GenericNotificationParameterFactory::eventTypeKey()), QVariant("event"));
    QCOMPARE(params.value(GenericNotificationParameterFactory::countKey()), QVariant("1"));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::summaryKey()), QVariant("summary"));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::bodyKey()), QVariant("body"));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::imageIdKey()), QVariant("imageURI"));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::actionKey()), QVariant("action"));
}

void Ut_DBusInterfaceNotificationSource::testUpdateGroup()
{
    source->updateGroup(1, 2, "event");
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("updateGroup").parameter<uint>(0), (uint)1);
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("updateGroup").parameter<uint>(1), (uint)2);
    NotificationParameters params = gDefaultNotificationManagerStub.stubLastCallTo("updateGroup").parameter<NotificationParameters>(2);
    QCOMPARE(params.value(GenericNotificationParameterFactory::eventTypeKey()), QVariant("event"));

    source->updateGroup(3, 4, "event", "summary", "body", "action", "imageURI", 42);
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("updateGroup").parameter<uint>(0), (uint)3);
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("updateGroup").parameter<uint>(1), (uint)4);
    params = gDefaultNotificationManagerStub.stubLastCallTo("updateGroup").parameter<NotificationParameters>(2);
    QCOMPARE(params.value(GenericNotificationParameterFactory::eventTypeKey()), QVariant("event"));
    QCOMPARE(params.value(GenericNotificationParameterFactory::countKey()), QVariant("42"));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::summaryKey()), QVariant("summary"));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::bodyKey()), QVariant("body"));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::imageIdKey()), QVariant("imageURI"));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::actionKey()), QVariant("action"));
}

void Ut_DBusInterfaceNotificationSource::testRemoveGroup()
{
    source->removeGroup(1, 2);
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("removeGroup").parameter<uint>(0), (uint)1);
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("removeGroup").parameter<uint>(1), (uint)2);
}

void Ut_DBusInterfaceNotificationSource::testNotificationIdList()
{
    QList<uint> tmp;
    tmp << 1 << 2 << 3 << 4 << 5;
    gDefaultNotificationManagerStub.stubSetReturnValue("notificationIdList", tmp);

    QList<uint> result = source->notificationIdList(10);
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("notificationIdList").parameter<uint>(0), (uint)10);

    QCOMPARE(result, tmp);
}

void Ut_DBusInterfaceNotificationSource::testNotificationList()
{
    source->notificationList(10);
    QCOMPARE(gDefaultNotificationManagerStub.stubCallCount("notificationList"), 1);
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("notificationList").parameter<uint>(0), (uint)10);
}

void Ut_DBusInterfaceNotificationSource::testNotificationGroupList()
{
    source->notificationGroupList(10);
    QCOMPARE(gDefaultNotificationManagerStub.stubCallCount("notificationGroupList"), 1);
    QCOMPARE(gDefaultNotificationManagerStub.stubLastCallTo("notificationGroupList").parameter<uint>(0), (uint)10);
}

void Ut_DBusInterfaceNotificationSource::testUpdateGroupWithEmptyStrings()
{
    source->updateGroup(3, 4, "event", "summary", "body", "action", "imageURI", 42);
    NotificationParameters params = gDefaultNotificationManagerStub.stubLastCallTo("updateGroup").parameter<NotificationParameters>(2);
    QCOMPARE(params.value(NotificationWidgetParameterFactory::summaryKey()), QVariant("summary"));

    source->updateGroup(3, 4, "event", "", "", "", "", 42);
    params = gDefaultNotificationManagerStub.stubLastCallTo("updateGroup").parameter<NotificationParameters>(2);
    QCOMPARE(params.value(GenericNotificationParameterFactory::eventTypeKey()), QVariant("event"));
    QCOMPARE(params.value(GenericNotificationParameterFactory::countKey()), QVariant("42"));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::summaryKey()), QVariant(""));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::bodyKey()), QVariant(""));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::imageIdKey()), QVariant(""));
    QCOMPARE(params.value(NotificationWidgetParameterFactory::actionKey()), QVariant(""));
}

QTEST_APPLESS_MAIN(Ut_DBusInterfaceNotificationSource)
