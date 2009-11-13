#include "pinstubs.h"
#include "ut_pincodequerybusinesslogic.h"

#include <QDebug>

/* ------------ SIM ------------------*/
SIM::SIM(QObject *parent) :
        QObject(parent),
        simStatus(SIM::NotReady),
        error(SIMErrorNone)
{
    qRegisterMetaType<SIM::SIMStatus>("SIM::SIMStatus");
    qRegisterMetaType<SIMError>("SIMError");
}

SIM::~SIM()
{
}

void SIM::status()
{
    qDebug() << Q_FUNC_INFO;
    emit statusComplete(simStatus, error);
}

void SIM::emitStatusChanged()
{
    qDebug() << Q_FUNC_INFO;
    emit statusChanged(simStatus);
}

/* ------------ SIM Security ------------------*/
SIMSecurity::SIMSecurity(QObject *parent) :
        QObject(parent),
        pin(QString("1234")),
        puk(QString("1234")),
        queryState(Enabled),
        pinAttempts(4),
        pukAttempts(10),
        success(true),
        error(SIMErrorNone)
{
    qRegisterMetaType<SIMSecurity::PINType>("SIMSecurity::PINType");
    qRegisterMetaType<SIMSecurity::PINQuery>("SIMSecurity::PINQuery");
}

SIMSecurity::~SIMSecurity()
{
}

void SIMSecurity::changePIN(PINType type, const QString &oldCode, const QString &newCode)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(type);
    if(oldCode != pin){
        emit changePINComplete(false, SIMErrorWrongPassword);
    } else {
        emit changePINComplete(true, SIMErrorNone);
        pin = newCode;
    }
}

void SIMSecurity::disablePINQuery(PINType type, const QString &code)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(type);
    if(code != pin){
        emit enablePINQueryComplete(SIMErrorWrongPassword);
    } else {
        emit enablePINQueryComplete(SIMErrorNone);
        queryState = SIMSecurity::Disabled;
    }
}

void SIMSecurity::enablePINQuery(PINType type, const QString &code)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(type);
    if(code != pin){
        emit enablePINQueryComplete(SIMErrorWrongPassword);
    } else {
        emit enablePINQueryComplete(SIMErrorNone);
        queryState = SIMSecurity::Enabled;
    }
}

void SIMSecurity::pinQueryState(PINType type)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(type);
    emit pinQueryStateComplete(queryState, error);
}

void SIMSecurity::verifyPIN(PINType type, const QString &code)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(type);
    if(code != pin){
        emit verifyPINComplete(false, SIMErrorWrongPassword);
    } else {
        emit verifyPINComplete(true, SIMErrorNone);
    }
}

void SIMSecurity::verifyPUK(PUKType type, const QString &PUKCode, const QString &newPINCode)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(type);

    if(PUKCode != puk){
        emit verifyPUKComplete(false, SIMErrorWrongPassword);
    } else {
        emit verifyPUKComplete(true, SIMErrorNone);
        pin = newPINCode;
    }
}

void SIMSecurity::pinAttemptsLeft(PINType type)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(type);
    emit pinAttemptsLeftComplete(pinAttempts, error);
}

void SIMSecurity::pukAttemptsLeft(PUKType type)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(type);
    emit pukAttemptsLeftComplete(pukAttempts, error);
}

void SIMSecurity::emitVerifyPINRequired(SIMSecurity::PINType type)
{
    qDebug() << Q_FUNC_INFO;
    emit verifyPINRequired(type);
}

void SIMSecurity::emitVerifyPUKRequired(SIMSecurity::PUKType type)
{
    qDebug() << Q_FUNC_INFO;
    emit verifyPUKRequired(type);
}


/* ------------ SIM Lock ------------------*/
SIMLock::SIMLock(QObject *parent) :
        QObject(parent),
        status(SimLockStatusOk),
        level(SIMLock::LevelGlobal),
        error(SIMLockErrorNone),
        code(QString("1234"))
{
    qRegisterMetaType<SIMLock::SIMLockStatus>("SIMLock::SIMLockStatus");
    qRegisterMetaType<SIMLock::SIMLockLevel>("SIMLock::SIMLockLevel");
    qRegisterMetaType<SIMLockError>("SIMLockError");
}

SIMLock::~SIMLock()
{
}

void SIMLock::simLockStatus()
{
    qDebug() << Q_FUNC_INFO;
    emit simLockStatusComplete(status, error);
}

void SIMLock::simLockUnlock(SIMLock::SIMLockLevel level, const QString &unlockCode)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(level);
    if(unlockCode != code){
        emit simLockUnlockComplete(SIMLockErrorWrongPassword);
    } else {
        emit simLockUnlockComplete(SIMLockErrorNone);
    }
}

/*------------ PinCodeQuery DBus adaptor ---------*/

PinCodeQueryDBusAdaptor::PinCodeQueryDBusAdaptor(QObject* parent) :
        QObject(parent)
{
}

PinCodeQueryDBusAdaptor::~PinCodeQueryDBusAdaptor()
{
}

void PinCodeQueryDBusAdaptor::pinQueryStateCompletedResponse(SIMSecurity::PINQuery state, SIMError error)
{
    qDebug() << Q_FUNC_INFO << state << error;
    this->state = state;
    this->error = error;
}

void PinCodeQueryDBusAdaptor::pinQueryEnabledResponse(SIMSecurity::PINQuery queryState)
{
    qDebug() << Q_FUNC_INFO << queryState;
    this->state = queryState;
}

void PinCodeQueryDBusAdaptor::pinQueryDoneResponse(bool queryOk)
{
    qDebug() << Q_FUNC_INFO << queryOk;
    this->queryStarted = queryOk;
}
