#ifndef STATUSINDICATORMENUWINDOW_STUB
#define STATUSINDICATORMENUWINDOW_STUB

#include "statusindicatormenuwindow.h"
#include <stubbase.h>

#ifdef HAVE_QMSYSTEM
#include <qmlocks.h>
#endif

// 1. DECLARE STUB
// FIXME - stubgen is not yet finished
class StatusIndicatorMenuWindowStub : public StubBase {
  public:
  virtual void StatusIndicatorMenuWindowConstructor(QWidget *parent);
  virtual void StatusIndicatorMenuWindowDestructor();
  virtual void makeVisible();
  virtual void displayActive();
  virtual void displayInActive();
#ifdef HAVE_QMSYSTEM
  virtual void setWindowStateAccordingToDeviceLockState(Maemo::QmLocks::Lock what, Maemo::QmLocks::State how);
#endif
};

// 2. IMPLEMENT STUB
void StatusIndicatorMenuWindowStub::StatusIndicatorMenuWindowConstructor(QWidget *parent) {
  Q_UNUSED(parent);

}
void StatusIndicatorMenuWindowStub::StatusIndicatorMenuWindowDestructor() {

}
void StatusIndicatorMenuWindowStub::makeVisible() {
  stubMethodEntered("makeVisible");
}

void StatusIndicatorMenuWindowStub::displayActive() {
  stubMethodEntered("displayActive");
}

void StatusIndicatorMenuWindowStub::displayInActive() {
  stubMethodEntered("displayInActive");
}

#ifdef HAVE_QMSYSTEM
void StatusIndicatorMenuWindowStub::setWindowStateAccordingToDeviceLockState(Maemo::QmLocks::Lock what, Maemo::QmLocks::State how) {
  QList<ParameterBase*> params;
  params.append( new Parameter<Maemo::QmLocks::Lock>(what));
  params.append( new Parameter<Maemo::QmLocks::State>(how));
  stubMethodEntered("setWindowStateAccordingToDeviceLockState",params);
}
#endif

// 3. CREATE A STUB INSTANCE
StatusIndicatorMenuWindowStub gDefaultStatusIndicatorMenuWindowStub;
StatusIndicatorMenuWindowStub* gStatusIndicatorMenuWindowStub = &gDefaultStatusIndicatorMenuWindowStub;


// 4. CREATE A PROXY WHICH CALLS THE STUB
StatusIndicatorMenuWindow::StatusIndicatorMenuWindow(QWidget *parent) {
  gStatusIndicatorMenuWindowStub->StatusIndicatorMenuWindowConstructor(parent);
}

StatusIndicatorMenuWindow::~StatusIndicatorMenuWindow() {
  gStatusIndicatorMenuWindowStub->StatusIndicatorMenuWindowDestructor();
}

void StatusIndicatorMenuWindow::makeVisible() {
  gStatusIndicatorMenuWindowStub->makeVisible();
}

void StatusIndicatorMenuWindow::displayActive() {
  gStatusIndicatorMenuWindowStub->displayActive();
}

void StatusIndicatorMenuWindow::displayInActive() {
  gStatusIndicatorMenuWindowStub->displayInActive();
}

#ifdef HAVE_QMSYSTEM
void StatusIndicatorMenuWindow::setWindowStateAccordingToDeviceLockState(Maemo::QmLocks::Lock what, Maemo::QmLocks::State how) {
  gStatusIndicatorMenuWindowStub->setWindowStateAccordingToDeviceLockState(what, how);
}
#endif

#endif
