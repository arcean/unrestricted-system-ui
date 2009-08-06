#include "pincodequeryui.h"

#include <DuiWidget>
#include <DuiLayout>
#include <DuiApplicationPage>
#include <DuiGridLayoutPolicy>
#include <DuiButton>
#include <DuiLabel>
#include <DuiLocale>
#include <DuiTextEdit>
#include <DuiTheme>
#include <DuiSceneManager>
#include <DuiContainer>
#include <DuiGrid>
#include <QStringList>
#include <QSizePolicy>
#include <QDebug>

//TODO: fetch the data elsewhere
namespace {
    const QStringList EmergencyCallPhoneNumbers = (QStringList() << "112" << "911");
}

PinCodeQueryUI::PinCodeQueryUI()
{
    setFullscreen(true);
    setTitle("PIN code query:");
    createContent();
}

PinCodeQueryUI::~PinCodeQueryUI()
{
}

void PinCodeQueryUI::createContent()
{
    DuiApplicationPage::createContent();
    DuiWidget *panel = centralWidget();

    //create the widgets and numpad.
    createWidgetItems();

    //create mainLayout
    DuiLayout *mainLayout = new DuiLayout(panel);
    //mainLayout->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    //create landscapePolicy and portraitPolicy for mainLayout
    landscapePolicy = new DuiGridLayoutPolicy(mainLayout);
    portraitPolicy = new DuiGridLayoutPolicy(mainLayout);

    //attach widget items to landscape and portrait policies
    landscapePolicy->addItemAtPosition(headerLabel, 0, 0, 1, 5);
    landscapePolicy->addItemAtPosition(emergencyCallButton, 1, 0, 1, 4);
    landscapePolicy->addItemAtPosition(entryTextEdit, 2, 0, 1, 3);
    landscapePolicy->addItemAtPosition(backspaceButton, 2, 3, 1, 1);
    landscapePolicy->addItemAtPosition(enterButton, 3, 0, 1, 2);
    landscapePolicy->addItemAtPosition(cancelButton, 3, 2, 1, 2);
    landscapePolicy->addItemAtPosition(numpadLayout, 1, 4, 3, 1);
    portraitPolicy->addItemAtPosition(headerLabel, 0, 0, 1, 4);
    portraitPolicy->addItemAtPosition(emergencyCallButton, 1, 0, 1, 4);
    portraitPolicy->addItemAtPosition(entryTextEdit, 2, 0, 1, 3);
    portraitPolicy->addItemAtPosition(backspaceButton, 2, 3);
    portraitPolicy->addItemAtPosition(numpadLayout, 3, 0, 1, 4);
    portraitPolicy->addItemAtPosition(enterButton, 4, 0, 1, 2);
    portraitPolicy->addItemAtPosition(cancelButton, 4, 2, 1, 2);

    //set column and row sizes
    int fullWidth = DuiSceneManager::instance()->visibleSceneRect().width();
    int fullHeight = DuiSceneManager::instance()->visibleSceneRect().height();
    for(int lsCol=0; lsCol<5; ++lsCol) {
	if(lsCol < 4)
            landscapePolicy->setColumnFixedWidth(lsCol, fullWidth/8);
	else
	    landscapePolicy->setColumnFixedWidth(lsCol, fullWidth/2);
    }
    for(int lsRow=0; lsRow<4;++lsRow) {
	landscapePolicy->setRowFixedHeight(lsRow, fullHeight/4);
    }
    for(int ptCol=0; ptCol<4; ++ptCol) {
	portraitPolicy->setColumnFixedWidth(ptCol, fullHeight/4);
    }
    for(int ptRow=0; ptRow<5; ++ptRow) {
	portraitPolicy->setRowFixedHeight(ptRow, fullWidth/5);
    }
	    
    orientationChanged(DuiSceneManager::instance()->orientation());

//    DuiTheme::loadCSS("pinquery.css");

    // HACK: make all buttons visible :)
    /*
    requestPageMove(QPointF(0, -1), QPointF(0, 1), false);
    restorePageMove();
   */


    // Get informed about orientation changes
    connect(DuiSceneManager::instance(),
            SIGNAL(orientationChanged(const Dui::Orientation &)),
            this, SLOT(orientationChanged(const Dui::Orientation &)));

}

DuiButton *PinCodeQueryUI::getEmergencyBtn()
{
    return emergencyCallButton;
}
DuiButton *PinCodeQueryUI::getCancelBtn()
{
    return cancelButton;
}
DuiButton *PinCodeQueryUI::getEnterBtn()
{
    return enterButton;
}
DuiTextEdit *PinCodeQueryUI::getCodeEntry()
{
    return entryTextEdit;
}
void PinCodeQueryUI::setHeader(QString header)
{
    headerLabel->setText(header);
}


void PinCodeQueryUI::createWidgetItems()
{    
    emergencyCallButton = new DuiButton(QString(trid("qtn_cell_emergency_call", "Emergency call")), 0 );
    emergencyCallButton->setObjectName("emergencyCallButton");
    connect(emergencyCallButton, SIGNAL(released()), this, SLOT(buttonReleased()));
    qDebug() << "hiding emergenceCall button" ;
   emergencyCallButton->setVisible(false);

    entryTextEdit = new DuiTextEdit(DuiTextEditModel::SingleLine, "", 0);
    entryTextEdit->setObjectName("codeEntry");
    entryTextEdit->setMaskedInput(true);

    enterButton = new DuiButton(QString(trid("qtn_cell_enter", "Enter")), 0);
    enterButton->setObjectName("enterButton");
    connect(enterButton, SIGNAL(released()), this, SLOT(buttonReleased()));

    cancelButton = new DuiButton(QString(trid("qtn_comm_cancel", "Cancel")), 0);
    cancelButton->setObjectName("cancelButton");
    connect(cancelButton, SIGNAL(released()), this, SLOT(buttonReleased()));

    backspaceButton = new DuiButton(0);
    backspaceButton->setObjectName("backspaceButton");
    backspaceButton->setIconID("Icon-back");
    connect(backspaceButton, SIGNAL(released()), this, SLOT(buttonReleased()));

    headerLabel = new DuiLabel(0);
    headerLabel->setAlignment(Qt::AlignCenter);    
    
    createNumpad();
}

void PinCodeQueryUI::createNumpad()
{
    //create numpadLayout and policy for that
    numpadLayout = new DuiLayout(0);
    DuiGridLayoutPolicy *numpadLayoutPolicy = new DuiGridLayoutPolicy(numpadLayout);

    int values[] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9,
        0
    };

    int colSpan = 1;
    for (unsigned int i=0; i<sizeof(values)/sizeof(values[0]); ++i) {
	
	if(values[i] == 0)
	    colSpan = 3;

        QString str1 = QString("qtn_cell_dialer_").append(QString::number(values[i]));
        QString str2 = QString::number(values[i]);
        QString str3 = QString("numpadButton" + str2);
        DuiButton *num = new DuiButton(QString(trid(str1.toLatin1(), str2.toLatin1())), 0);
        num->setObjectName(str3);
        connect(num, SIGNAL(released()), this, SLOT(buttonReleased()));
        numpadLayoutPolicy->addItemAtPosition(num, (i/3), i%3, 1, colSpan); 
    }
}

void PinCodeQueryUI::buttonReleased()
{
    DuiButton* button = static_cast<DuiButton*>(this->sender());

    //Check if the button was a numpad button       
    if(button->objectName().left(button->objectName().length()-1) == "numpadButton") {
        entryTextEdit->insert(button->objectName().right(1));
        qDebug() << "text now: " << entryTextEdit->text();
        checkEntry(); //Check the current entry
    }

    //Check if the button was backspace
    else if(button->objectName() == QString("backspaceButton")) {
        entryTextEdit->setText(entryTextEdit->text().left(entryTextEdit->text().length()-1));
        checkEntry(); //Check the current entry
    }


}

void PinCodeQueryUI::checkEntry()
{
    if(emergencyCallButton == NULL)
        return;

    QString pinCode = entryTextEdit->text();

    //check if the entered opin code is an emergency call phone number
    if( EmergencyCallPhoneNumbers.contains(pinCode)) {
        emergencyCallButton->setVisible(true);
        entryTextEdit->setMaskedInput(false);
    }
    else if(emergencyCallButton->isVisible()) {
        emergencyCallButton->setVisible(false);
        entryTextEdit->setMaskedInput(true);
    }

}

void PinCodeQueryUI::orientationChanged(const Dui::Orientation &orientation)
{
     qDebug() << "OrientationChanged" ;
    if (orientation == Dui::Portrait) {
        portraitPolicy->activate();
    } else {
        landscapePolicy->activate();
    }
}

