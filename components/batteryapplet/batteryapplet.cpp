#include "batteryapplet.h"
#include "batterywidget.h"
#include "batterybrief.h"

#include "dcpbattery.h"
#include "batterywidget.h"

#include <dcpwidget.h>

#include <QtGui>
#include <QDebug>

#include <DuiTheme>
#include <DuiAction>

#define DEBUG
#include "../debug.h"

Q_EXPORT_PLUGIN2(batteryapplet, BatteryApplet)

const QString cssDir = "/usr/share/duicontrolpanel/themes/style/";

BatteryApplet::BatteryApplet ():
	m_MainWidget (0)
{
}

BatteryApplet::~BatteryApplet ()
{
    SYS_DEBUG ("Destroying %p", this);
}

void BatteryApplet::init()
{
    DuiTheme::loadCSS(cssDir + "batteryapplet.css");
}

DcpWidget* BatteryApplet::constructWidget(int widgetId)
{
    Q_UNUSED(widgetId);
    return pageMain();
}

DcpWidget* 
BatteryApplet::pageMain()
{
    SYS_DEBUG ("Starting on %p", this);
    if (m_MainWidget == NULL)
        m_MainWidget = new BatteryWidget();

    SYS_DEBUG ("----------------Wow!");
    SYS_DEBUG ("Returning %p", m_MainWidget);
    return m_MainWidget;
}

QString BatteryApplet::title() const
{
    //% "Battery"
    return qtTrId ("qtn_ener_battery");
}

QVector<DuiAction*> BatteryApplet::viewMenuItems()
{
    QVector<DuiAction*> vector;
    //% "Help"
    DuiAction* helpAction = new DuiAction(qtTrId ("qtn_comm_help"), pageMain());
    vector.append(helpAction);
    helpAction->setLocation(DuiAction::ApplicationMenu);
    return vector;
}

DcpBrief* BatteryApplet::constructBrief(int partId)
{
    Q_UNUSED(partId);
    return new BatteryBrief();
}

