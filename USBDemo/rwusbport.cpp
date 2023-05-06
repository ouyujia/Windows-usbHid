#include <QDebug>
#include "rwusbport.h"
#define MAX_STR 255
#define bufLen 65
#define readBufLen 64
rwUsbPort::rwUsbPort(unsigned short vendor_id,unsigned short product_id, QWidget *parent)
{
    mvendor_id = vendor_id;
    mproduct_id=product_id;
    // rwUsbPort(parent);
    msleepTime = 20;
    mhandle = nullptr;
    mmyusb = new myUsbRW(this);
    m_mutexConnect = new QMutex();
    m_mutexDisconnect = new QMutex();
    SetConnectFlag(false);      // 默认未连接
    SetDisconnectFlag(true);   // 默认主动断开连接
    if(mmyusb)
    {
        connect(mmyusb, SIGNAL(SignalSendLog(const QString &)), this, SIGNAL(SignalSendLog(const QString &)));
    }
}

rwUsbPort::rwUsbPort(QWidget *parent)
{
    msleepTime = 20;
    mhandle = nullptr;
    mmyusb = new myUsbRW(this);
    m_mutexConnect = new QMutex();
    m_mutexDisconnect = new QMutex();
    SetConnectFlag(false);      // 默认未连接
    SetDisconnectFlag(true);   // 默认主动断开连接
    if(mmyusb)
    {
        connect(mmyusb, SIGNAL(SignalSendLog(const QString &)), this, SIGNAL(SignalSendLog(const QString &)));
    }
}

rwUsbPort::~rwUsbPort()
{

}

void rwUsbPort::SetUSBID(QString &VID, QString &PID)
{
    bool ok;
    mvendor_id = VID.toUShort(&ok,16);
    mproduct_id = PID.toUShort(&ok,16);
    QString info = QString("PID:%1 VID:%2").arg(VID).arg(PID);
    emit SignalSendLog(info);
}

void rwUsbPort::readPortInfo()
{  

}

void rwUsbPort::run()
{
    qDebug()<<"openusb-------------";

    bool bsta = true;
    while(GetDisconnectFlag() && (!GetConnectFlag()))
    {
        mmyusb->closeUsbPort();
        bsta = mmyusb->openUsbPort(mvendor_id,mproduct_id);
        qDebug()<<bsta<<"re open";
        if(!bsta)
        {
            qDebug()<<"openusb error"<<mvendor_id<<mproduct_id;//<<QTime::currentTime();
            //msleep(200);
            break;
        }
        else
        {
            SetConnectFlag(true);
            SetDisconnectFlag(false);
            ReadData();
        }
    }

}
//没有数据时，读出的是全0,此时返回false；
//有有效数据，则返回true；
bool rwUsbPort::checkData(unsigned char *data)
{
    bool bre = false;
    for(int idx =0; idx < readBufLen; idx++)
    {
        if(0 != data[idx])
        {
            bre = true;
            break;
        }
    }
    return bre;
}

void rwUsbPort::writeData(char * data,int ilen)
{
    if(!isRunning())return;
    msleepTime = 10;//有写入消息。
    int ichushu = ilen/64;
    int iYu = ilen%64;
    QByteArray ba;
    unsigned char buf[bufLen] = {0};
    for(int idx=0; idx<ichushu; idx++)
    {
        memset(buf,0,bufLen);
        memcpy(buf+1, (data + (idx *64)), 64); // 组装报告，第一个字节为报告ID(0x00)
        if(GetConnectFlag())
        {
            mmyusb->writeData((char *)buf,bufLen);
        }
        msleep(20); // 延时，确保接受端可以完成数据接收
    }
    if(0 != iYu) // 根据双方协议一个数据包为64字节，不足64的也需要发送
    {
        memset(buf,0,bufLen);
        memcpy_s(buf+1, bufLen, (data + (ichushu *64)), iYu);
        if(GetConnectFlag())
        {
            mmyusb->writeData((char *)buf, bufLen);
        }
    }

    ba = QByteArray((const char*)buf, bufLen);
    qDebug()<<"Data:" << ba.toHex();

}

void rwUsbPort::ReadData()
{
    unsigned char buf[readBufLen];
    int isec;
    QByteArray data;
    while(true)
    {
        isec = mgetDataTime.msecsTo(QTime::currentTime());
//        if(isec>3200)
//        {
//            SetConnectFlag(false);
//            SetDisconnectFlag(false);
//            mhandle = nullptr;
//            mmyusb->closeUsbPort();
//            qDebug()<<"usbLost1";
//            break;//超过5s无数据，则认为断链。
//        }
        memset(buf,0,readBufLen);
        mmyusb->hid_read_timeout(buf,readBufLen,2000);
        data.clear();
        data = mmyusb->mbaData;
        if(data.length() < 1 )
        {
            //emit SignalSendLog(QString::fromLocal8Bit("数据读取失败"));
            msleep(msleepTime);
          continue;
//                    break;
        }
        emit readData(data);
        msleep(msleepTime);
        mgetDataTime = QTime::currentTime();
        msleepTime = 20;
    }
}

void rwUsbPort::DisconnectUSB()
{
    if(mmyusb)
    {
        mmyusb->closeUsbPort();
        emit SignalSendLog(QString::fromLocal8Bit("已关闭连接"));
    }
}

void rwUsbPort::SetConnectFlag(bool bConnectFlag)
{
    m_mutexConnect->lock();
    m_bConnectFlag = bConnectFlag;
    m_mutexConnect->unlock();
}

bool rwUsbPort::GetConnectFlag() const
{
    bool bConnect = false;
    m_mutexConnect->lock();
    bConnect = m_bConnectFlag;
    m_mutexConnect->unlock();
    return bConnect;
}

void rwUsbPort::SetDisconnectFlag(bool bDisconnectFlag)
{
    m_mutexDisconnect->lock();
    m_bDisconnectFlag = bDisconnectFlag;
    m_mutexDisconnect->unlock();
}

bool rwUsbPort::GetDisconnectFlag() const
{
    bool bDisconnect = false;
    m_mutexDisconnect->lock();
    bDisconnect = m_bDisconnectFlag;
    m_mutexDisconnect->unlock();
    return bDisconnect;
}


