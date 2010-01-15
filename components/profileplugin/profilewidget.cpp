/*
 * profile.cpp
 *
 * This file is part of duistatusindicatormenu
 *
 * Copyright (C) 2009 Nokia Corporation. All rights reserved.
 *
 * This software, including documentation, is protected by copyright
 * controlled by Nokia Corporation. All rights are reserved.
 * Copying, including reproducing, storing, adapting or translating,
 * any or all of this material requires the prior written consent of
 * Nokia Corporation. This material also contains confidential
 * information which may not be disclosed to others without the prior
 * written consent of Nokia.
 */

#include "profilewidget.h"
#include "profiledatainterface.h"
#include "profilebuttons.h"

#include <DuiButton>
#include <DuiButtonGroup>
#include <DuiContainer>
#include <DuiControlPanelIf>
#include <DuiGridLayoutPolicy>
#include <DuiLayout>
#include <DuiLinearLayoutPolicy>
#include <DuiLocale>
#include <DuiStatusIndicatorMenuPluginInterface>

#include <QGraphicsLinearLayout>


ProfileWidget::ProfileWidget(DuiStatusIndicatorMenuInterface &statusIndicatorMenu, QGraphicsItem *parent) :
        DuiWidget(parent),
        statusIndicatorMenu(statusIndicatorMenu),
        dataIf(NULL),
        profileButtons(NULL)
{
    Q_UNUSED(statusIndicatorMenu);
    dataIf = new ProfileDataInterface();
    connect(dataIf, SIGNAL(currentProfileNameChanged(QString)), profileButtons, SLOT(selectProfile(int)));

    QGraphicsLinearLayout *mainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    setLayout(mainLayout);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Create a container for the profiles
    initProfileButtons();

    mainLayout->addItem(profileButtons);

}

ProfileWidget::~ProfileWidget()
{
    delete dataIf;
    dataIf = NULL;
}

void ProfileWidget::initProfileButtons()
{
    profileButtons = new ProfileButtons();
    //% "Profiles"
    profileButtons->setTitle (qtTrId ("qtn_prof_profile"));
    QMap<int, QString> map;
    QList<ProfileDataInterface::ProfileData> l = dataIf->getProfilesData();
    for (int i = 0; i < l.count(); ++i) {
        ProfileDataInterface::ProfileData d = l.at(i);
        map.insert(d.profileId, d.profileName);
    }
    profileButtons->init(map, dataIf->getCurrentProfile());
    connect(profileButtons, SIGNAL(headerClicked()), this, SLOT(showProfileModificationPage()));
    connect(profileButtons, SIGNAL(profileSelected(int)), dataIf, SLOT(setProfile(int)));
}

void ProfileWidget::showProfileModificationPage()
{
    // instantiate the interface
    DuiControlPanelIf cpIf;
    // check the interface is valid
    if (!cpIf.isValid())
        return;
    cpIf.appletPage("Profile");
}
