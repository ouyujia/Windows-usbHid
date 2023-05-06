#ifndef RWUSBPORT_H
#define RWUSBPORT_H

#include <QObject>
#include <QThread>
#include <QTime>
#include <QString>
#include <QMutex>
#include "winHid/hidapi.h"
#include "winHid/myusbrw.h"

class rwUsbPort: public QThread
{
    Q_OBJECT
public:
    rwUsbPort(QWidget *parent = 0);
    rwUsbPort(unsigned short vendor_id,unsigned short product_id, QWidget *parent = 0);
    ~rwUsbPort();    
    void SetUSBID(QString &VID, QString &PID);
    void writeData(char * data,int ilen);//写数据
    void ReadData();
    void DisconnectUSB();
    void SetConnectFlag(bool bConnectFlag);
    bool GetConnectFlag()const;
    void SetDisconnectFlag(bool bDisconnectFlag);
    bool GetDisconnectFlag()const;
signals:
    void readData(QByteArray data);//读数据信号
    void SignalSendLog(const QString &log);

protected:
    void run();
private:
    int msleepTime;//
    QTime mgetDataTime;
    hid_device *mhandle;
    unsigned short mvendor_id;
    unsigned short mproduct_id;

    bool checkData(unsigned char * data);
    void readPortInfo();
    myUsbRW * mmyusb;
    QMutex *m_mutexConnect;
    QMutex *m_mutexDisconnect;
    bool m_bConnectFlag;    // 设备连接标志位 false : 设备未连接 true :设备已连接
    bool m_bDisconnectFlag; // 设备主动断开连接标志位 true:设备断开连接
};

#endif // RWUSBPORT_H
