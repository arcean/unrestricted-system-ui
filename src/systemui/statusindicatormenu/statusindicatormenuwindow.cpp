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
#include <QGraphicsSceneMouseEvent>
#include <MSceneManager>
#include <MSceneWindow>
#include <MApplicationExtensionArea>
#include <MEscapeButtonPanel>
#include <MButton>
#include <MOverlay>
#include <MApplicationIfProxy>
#include <MPannableViewport>
#include <QGraphicsLinearLayout>
#include "notificationarea.h"
#include "statusindicatormenuwindow.h"
#include <MWidgetView>
#include <QGraphicsAnchorLayout>
#include "statusindicatormenuwindowstyle.h"

void EventEaterWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
}

PannedWidgetController::PannedWidgetController(QGraphicsItem *parent) :
    MWidgetController(parent),
    bottommostWidget_(NULL)
{
}

const QGraphicsWidget *PannedWidgetController::bottommostWidget() const
{
    return bottommostWidget_;
}

void PannedWidgetController::setBottommostWidget(const QGraphicsWidget *widget)
{
    bottommostWidget_ = widget;
}

void PannedWidgetController::setGeometry(const QRectF &rect)
{
    MWidgetController::setGeometry(rect);
    emit positionOrSizeChanged();
}

void PannedWidgetController::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
}

void PannedWidgetController::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (bottommostWidget_) {
        // Test that the y position of the event is bigger than the bottom edge of the bottommost widget.
        // The calculations are done in this widget's coordinate space.
        if (event->pos().y() > bottommostWidget_->y() + bottommostWidget_->geometry().height()) {
            emit pressedOutSideContents();
        }
    }
}

const QString StatusIndicatorMenuWindow::CONTROL_PANEL_SERVICE_NAME = "com.nokia.DuiControlPanel";

StatusIndicatorMenuWindow::StatusIndicatorMenuWindow(QWidget *parent) :
    MWindow(parent),
    sceneWindow(new MSceneWindow),
    statusBar(new MStatusBar),
    pannableViewport(NULL),
    closeButtonOverlay(NULL),
    backgroundWidget(new MWidgetController)
{
    setTranslucentBackground(true);
    setWindowTitle("Status Indicator Menu");
    connect(this, SIGNAL(displayEntered()), this, SLOT(displayActive()));
    connect(this, SIGNAL(displayExited()), this, SLOT(displayInActive()));

#ifdef HAVE_QMSYSTEM
    /*
     * We need to receive updates when device lock state changes
     * to prevent status indicator menu opening when device lock is on
     */
    connect (&qmLocks, SIGNAL(stateChanged (Maemo::QmLocks::Lock, Maemo::QmLocks::State)), this,
                                   SLOT(setWindowStateAccordingToDeviceLockState(Maemo::QmLocks::Lock, Maemo::QmLocks::State)));
    if (qmLocks.getState(Maemo::QmLocks::Device) != Maemo::QmLocks::Locked) {
        deviceLocked = false;
    } else {
        deviceLocked = true;
    }
#endif

    // Show status bar
    sceneManager()->appearSceneWindowNow(statusBar);

    // Create close button overlay
    closeButtonOverlay = createCloseButtonOverlay();

    // Create the pannable area background widget
    backgroundWidget->setView(new MWidgetView(backgroundWidget));
    backgroundWidget->setObjectName("StatusIndicatorMenuWindowBackground");
    QGraphicsAnchorLayout *backgroundLayout = new QGraphicsAnchorLayout;
    backgroundLayout->setContentsMargins(0, 0, 0, 0);
    backgroundLayout->setSpacing(0);
    backgroundLayout->addCornerAnchors(backgroundWidget, Qt::TopLeftCorner, backgroundLayout, Qt::TopLeftCorner);
    backgroundLayout->setMaximumHeight(0);

    // Put all the stuff into the scene window layout
    pannableViewport = createPannableArea();
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addItem(createTopRow());
    layout->addItem(backgroundLayout);
    layout->addItem(pannableViewport);
    sceneWindow->setLayout(layout);

    sceneWindow->setObjectName("StatusIndicatorMenuWindow");
    sceneManager()->appearSceneWindowNow(sceneWindow);

    // Set the X window type, so that the window does not appear in the switcher and
    // home screen can provide the correct UI flow
    setAttribute(Qt::WA_X11NetWmWindowTypeMenu);
}

StatusIndicatorMenuWindow::~StatusIndicatorMenuWindow()
{
    delete sceneWindow;
    delete statusBar;
    delete closeButtonOverlay;
}

QGraphicsWidget* StatusIndicatorMenuWindow::createTopRow()
{
    // Create an extension area for the top row plugins
    MApplicationExtensionArea *extensionArea = new MApplicationExtensionArea("com.meego.core.MStatusIndicatorMenuExtensionInterface/1.0");
    connect(extensionArea, SIGNAL(extensionInstantiated(MApplicationExtensionInterface*)), this, SLOT(setStatusIndicatorMenuInterface(MApplicationExtensionInterface*)));
    extensionArea->setObjectName("StatusIndicatorMenuTopRowExtensionArea");
    extensionArea->setInProcessFilter(QRegExp("/statusindicatormenu-(alarms|internetconnection|presence|profile).desktop$"));
    extensionArea->setOutOfProcessFilter(QRegExp("$^"));
    extensionArea->setOrder((QStringList() << "statusindicatormenu-alarms.desktop" << "statusindicatormenu-internetconnection.desktop" << "statusindicatormenu-presence.desktop" << "statusindicatormenu-profile.desktop"));
    extensionArea->init();

    // Create a button for accessing the full settings
    //% "Settings"
    MButton *settingsButton = new MButton(qtTrId("qtn_stat_menu_settings"));
    settingsButton->setObjectName("StatusIndicatorMenuTopRowExtensionButton");
    settingsButton->setViewType(MButton::iconType);
    settingsButton->setIconID("icon-m-status-menu-settings");
    connect(settingsButton, SIGNAL(clicked()), this, SLOT(launchControlPanelAndHide()));

    // Put the extension area and the settings button to a horizontal layout
    QGraphicsLinearLayout *topRowLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    topRowLayout->setContentsMargins(0, 0, 0, 0);
    topRowLayout->setSpacing(0);
    topRowLayout->addStretch();
    topRowLayout->addItem(extensionArea);
    topRowLayout->addItem(settingsButton);
    topRowLayout->addStretch();

    // Create a container widget for extension area and settings button layout
    MWidgetController *topRowWidget = new MWidgetController;
    topRowWidget->setView(new MWidgetView(topRowWidget));
    topRowWidget->setObjectName("StatusIndicatorMenuExtensionAreaWidget");
    topRowWidget->setLayout(topRowLayout);

    return topRowWidget;
}

MApplicationExtensionArea* StatusIndicatorMenuWindow::createVerticalExtensionArea()
{
    // Create an extension area for the call ui and transfer ui plugins
    MApplicationExtensionArea *extensionArea = new MApplicationExtensionArea("com.meego.core.MStatusIndicatorMenuExtensionInterface/1.0");
    extensionArea->setObjectName("StatusIndicatorMenuVerticalExtensionArea");
    extensionArea->setInProcessFilter(QRegExp("/statusindicatormenu-(call|transfer).desktop$"));
    extensionArea->setOutOfProcessFilter(QRegExp("$^"));
    extensionArea->setOrder((QStringList() << "statusindicatormenu-call.desktop" << "statusindicatormenu-transfer.desktop"));
    connect(extensionArea, SIGNAL(extensionInstantiated(MApplicationExtensionInterface*)), this, SLOT(setStatusIndicatorMenuInterface(MApplicationExtensionInterface*)));
    extensionArea->init();
    return extensionArea;
}

MPannableViewport* StatusIndicatorMenuWindow::createPannableArea()
{
    // Create pannable area contents
    MApplicationExtensionArea *extensionArea = createVerticalExtensionArea();

    QGraphicsLinearLayout *contentLayout = new QGraphicsLinearLayout(Qt::Vertical);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    contentLayout->addItem(extensionArea);

    const StatusIndicatorMenuWindowStyle *style = static_cast<const StatusIndicatorMenuWindowStyle *> (MTheme::style("StatusIndicatorMenuWindowStyle", ""));
    if(style && style->notificationArea()) {
        NotificationArea *notificationArea = new NotificationArea;
        connect(notificationArea, SIGNAL(bannerClicked()), this, SLOT(hideStatusIndicatorMenu()));
        contentLayout->addItem(notificationArea);
    }

    MWidgetController *contentWidget = new MWidgetController;
    contentWidget->setView(new MWidgetView(contentWidget));
    contentWidget->setObjectName("StatusIndicatorMenuContentWidget");
    contentWidget->setLayout(contentLayout);

    QGraphicsLinearLayout *pannableLayout = new QGraphicsLinearLayout(Qt::Vertical);
    pannableLayout->setContentsMargins(0, 0, 0, 0);
    pannableLayout->setSpacing(0);
    pannableLayout->addItem(contentWidget);
    QGraphicsWidget *closeButtonRow = createCloseButtonRow();
    pannableLayout->addItem(closeButtonRow);
    pannableLayout->addStretch();

    // Create a container widget for the pannable area
    PannedWidgetController *pannedWidget = new PannedWidgetController;
    pannedWidget->setView(new MWidgetView(pannedWidget));
    pannedWidget->setLayout(pannableLayout);
    pannedWidget->setBottommostWidget(closeButtonRow);
    connect(pannedWidget, SIGNAL(positionOrSizeChanged()), this, SLOT(setPannabilityAndLayout()));
    connect(pannedWidget, SIGNAL(pressedOutSideContents()), this, SLOT(hide()));

    // Setup the pannable viewport
    MPannableViewport *pannableViewport = new MPannableViewport;
    pannableViewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pannableViewport->setWidget(pannedWidget);
    return pannableViewport;
}

QGraphicsWidget* StatusIndicatorMenuWindow::createCloseButtonRow()
{
    // Create a close button for the pannable area
    MButton *closeButton = new MButton;
    closeButton->setViewType("icon");
    closeButton->setObjectName("StatusIndicatorMenuCloseButton");
    closeButton->setIconID("icon-m-framework-close");
    connect(closeButton, SIGNAL(clicked()), this, SLOT(hide()));

    // Add two overlay widgets that will not allow mouse events to pass through them next to the close button
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Horizontal);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addItem(new EventEaterWidget);
    layout->addItem(closeButton);
    layout->addItem(new EventEaterWidget);

    // Create the area itself
    MWidgetController *closeButtonArea = new MWidgetController;
    closeButtonArea->setView(new MWidgetView(closeButtonArea));
    closeButtonArea->setObjectName("CloseButtonArea");
    closeButtonArea->setLayout(layout);

    return closeButtonArea;
}

MOverlay *StatusIndicatorMenuWindow::createCloseButtonOverlay()
{
    // Create a close button
    MButton *closeButton = new MButton;
    closeButton->setViewType("icon");
    closeButton->setObjectName("StatusIndicatorMenuCloseButton");
    closeButton->setIconID("icon-m-framework-close");
    connect(closeButton, SIGNAL(clicked()), this, SLOT(hide()));

    // Add two overlay widgets that will not allow mouse events to pass through them next to the close button
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Horizontal);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addItem(new EventEaterWidget);
    layout->addItem(closeButton);
    layout->addItem(new EventEaterWidget);

    // Create the overlay itself
    MOverlay *closeButtonOverlay = new MOverlay;
    closeButtonOverlay->setLayout(layout);
    closeButtonOverlay->setObjectName("CloseButtonOverlay");
    sceneManager()->appearSceneWindowNow(closeButtonOverlay);
    sceneManager()->disappearSceneWindowNow(closeButtonOverlay);

    return closeButtonOverlay;
}

void StatusIndicatorMenuWindow::setPannabilityAndLayout()
{
    QGraphicsWidget *pannableWidget = pannableViewport->widget();

    // Enable pannability if there is too much content to fit on the screen
    bool viewportShouldBePannable = pannableWidget->effectiveSizeHint(Qt::PreferredSize).height() > pannableViewport->geometry().height();
    pannableViewport->setEnabled(viewportShouldBePannable);

    // Appear or disappear the close button overlay based on close area position
    const QGraphicsWidget *closeButtonRow = static_cast<PannedWidgetController *>(pannableViewport->widget())->bottommostWidget();
    qreal screenHeight = sceneManager()->visibleSceneSize().height();
    qreal closeButtonRowBottomYPos = closeButtonRow->mapToItem(sceneWindow, QPointF(0, closeButtonRow->geometry().height())).y();

    if (closeButtonRowBottomYPos <= screenHeight) {
        sceneManager()->disappearSceneWindowNow(closeButtonOverlay);
    } else {
        sceneManager()->appearSceneWindowNow(closeButtonOverlay);
    }

    // Make the pannable area background window extend from the top of the pannable viewport halfway to the bottom of the close button row
    qreal viewPortYPos = pannableViewport->mapToItem(sceneWindow, QPointF()).y();
    qreal backgroundHeight = (closeButtonRowBottomYPos - viewPortYPos) / 2;
    if (backgroundHeight < 0) {
        backgroundHeight = 0;
    }
    backgroundWidget->setMinimumHeight(backgroundHeight);
    backgroundWidget->setMaximumHeight(backgroundHeight);
}

void StatusIndicatorMenuWindow::displayActive()
{
    emit visibilityChanged(true);
}

void StatusIndicatorMenuWindow::displayInActive()
{
    pannableViewport->updatePosition(QPointF(0,0));
    emit visibilityChanged(false);
    // Hide the window when the it is obscured by another view
    // Note: Dialogs and notifications won't close it anyways,
    // as they are not supposed to be full screen and don't completely
    // obstruct the status menu window fully.
    hide();
}

void StatusIndicatorMenuWindow::setStatusIndicatorMenuInterface(MApplicationExtensionInterface *extension)
{
    MStatusIndicatorMenuExtensionInterface *menuExtension = static_cast<MStatusIndicatorMenuExtensionInterface *>(extension);
    menuExtension->setStatusIndicatorMenuInterface(*this);
}

void StatusIndicatorMenuWindow::makeVisible()
{
#ifdef HAVE_QMSYSTEM
    if (deviceLocked) {
        return;
    }
#endif
    if (!isVisible()) {
        // If status indicator window is not visible, then show it
        show();
    } else {
        // Otherwise, raise it
        raise();
    }
}

void StatusIndicatorMenuWindow::showStatusIndicatorMenu()
{
#ifdef HAVE_QMSYSTEM
    if (deviceLocked) {
        return;
    }
#endif
    sceneWindow->appear();
}

void StatusIndicatorMenuWindow::hideStatusIndicatorMenu()
{
    hide();
}

void StatusIndicatorMenuWindow::launchControlPanelAndHide()
{
    MApplicationIfProxy mApplicationIfProxy(CONTROL_PANEL_SERVICE_NAME, this);

    if (mApplicationIfProxy.connection().isConnected()) {
        mApplicationIfProxy.launch();
    } else {
        qWarning() << "Could not launch" << CONTROL_PANEL_SERVICE_NAME << "- DBus not connected?";
    }

    hideStatusIndicatorMenu();
}

#ifdef HAVE_QMSYSTEM
void StatusIndicatorMenuWindow::setWindowStateAccordingToDeviceLockState(Maemo::QmLocks::Lock what, Maemo::QmLocks::State how)
{
    if (what == Maemo::QmLocks::Device) {
        if (how == Maemo::QmLocks::Unlocked) {
            deviceLocked = false;
        } else {
            deviceLocked = true;
            if (isVisible()) {
                hideStatusIndicatorMenu();
            }
        }
    }
}

#endif