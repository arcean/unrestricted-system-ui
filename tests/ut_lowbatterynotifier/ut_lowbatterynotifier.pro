include(../coverage.pri)
include(../common_top.pri)
TARGET = ut_lowbatterynotifier

SOURCES += \
    ut_lowbatterynotifier.cpp \
    $$SRCDIR/lowbatterynotifier.cpp \
    stubbase.cpp

HEADERS += \
    ut_lowbatterynotifier.h \
    $$SRCDIR/lowbatterynotifier.h \
    $$SRCDIR/contextframeworkcontext.h \
    $$SRCDIR/applicationcontext.h

include(../common_bot.pri)
