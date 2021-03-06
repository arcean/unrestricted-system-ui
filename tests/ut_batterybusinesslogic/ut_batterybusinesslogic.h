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
#ifndef UT_BATTERYBUSINESSLOGIC_H
#define UT_BATTERYBUSINESSLOGIC_H

#include <QObject>

class BatteryBusinessLogic;

class Ut_BatteryBusinessLogic : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();

    void testInitBattery();
    void testLowBatteryAlert();
    void testBatteryStateChanged();
    void testChargingStateChanged();
    void testBatteryChargerEvent();
    void testPSMStateChanged();
    void testLowBatteryNotifierConnection();
    void testWhenChargingStopsThenNotificationRemoved();
    void testWhenChargingStopsWhenConnectedToWallChargerThenNotificationRemoved();
    void testWhenChargingStopsMoreThanNSecondAfterBeingStartedThenNotificationNotRemoved();
    void testWhenChargingStartsWhenRemoveChargerNotifiedThenNotificationRemoved();
    void testWhenChargingStopsAndBatteryIsLowNotifierIsCreated();
    void testWhenBatteryFullWhenChargingNotifiedThenNotificationRemoved();
    void testSetTouchScreenLockActive();
    void testWhenStateChargingLowBatteryNotificationRemoved();

private:
    BatteryBusinessLogic *m_logic;
};

#endif
