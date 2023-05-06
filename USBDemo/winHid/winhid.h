#ifndef LINUXHID_H
#define LINUXHID_H

#include <QMutex>
#include <QWaitCondition>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <QQueue>
#include <QReadWriteLock>
#include <functional>
#include <QTime>
#include "../server/port/SerialPort.h"
#include <QObject>
#include "hidapi.h"

class DataManage;

class winHid : public SerialPort
{
    Q_OBJECT
public:
    explicit winHid(QObject *parent = 0);
    ~winHid();

    void run();
    inline void pushData(const QByteArray & d);
    inline bool getSnedData(QByteArray & d);
    bool getBtpsSensorAvailable(){return m_btpsSenseAvailable;}

private:
    void senseConnectCheck();
    bool btpsSenseConnect();
    void notifyUIBtpsSenseConnect(bool);
    bool portInit(const QString&portname);

private:
    DataManage* tmpPmanage;

    QTime m_currentTm;
    bool m_btpsSenseAvailable;
    QString m_portname;

    int m_fd;
    hid_device *m_handle;
};

#endif // LINUXHID_H
