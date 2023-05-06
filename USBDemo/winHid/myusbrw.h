#ifndef MYUSBRW_H
#define MYUSBRW_H

#include <Windows.h>
#include <WinBase.h>
#include <Dbt.h>
#include <SetupAPI.h>
#include <QString>

#include <QObject>

class myUsbRW : public QObject
{
    Q_OBJECT
public:
    explicit myUsbRW(QObject *parent = 0);
    ~myUsbRW();
    bool openUsbPort(int vendorId,int productId);
    bool closeUsbPort();
    void writeData(char * data,int ilen);//写数据
    int  readData(char *data, int ilen, int milliseconds = 0);
    void readDataWhile();
    bool getPortState(){return mportState;}
    int  hid_read_timeout( unsigned char *data, size_t length, int milliseconds=0);
    bool mbReadPending;

    int mvendorID;
    int mproductID;
    HANDLE mhReadHandle; //用来保存读数据的设备句柄
    HANDLE mhWriteHandle; //用户保存写数据的设备句柄
    bool mportState;
    QByteArray mbaData;

signals:
    int readData(QByteArray data);//读数据信号
    void SignalSendLog(const QString &log);

public slots:
};

#endif // MYUSBRW_H
