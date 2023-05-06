#include "winhid.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
//#include <linux/input.h>
//#include <linux/hidraw.h>
#include <QFileInfoList>
#include <debug.h>
#include <QFile>
#include <QDir>

#include "global.h"
#include "../server/db/DataManage.h"
#include <QTime>

winHid::winHid(QObject *parent)
    : SerialPort(parent)
    ,tmpPmanage(NULL)
    ,m_btpsSenseAvailable(false)
    ,m_fd(0)
{
    tmpPmanage = DataManage::getDataManage();
    start();
}

winHid::~winHid()
{
    quit = true; //如果不赋值，存在线程未完全退出，但资源已经被释放掉的情况，导致程序崩溃
    this->exit();
    wait();
}

bool winHid::portInit(const QString&portname)
{
    if(m_fd>0)
    {
        close(m_fd);
    }
    m_fd = open(portname.toStdString().c_str(),O_RDONLY);

    if(m_fd<0)
        return false;

    return true;
}

void winHid::notifyUIBtpsSenseConnect(bool v)
{
    //发送事件通知
    //qDebug()<<Q_FUNC_INFO<<v;
}

#define PRODUCT_VID 0x1001
#define PRODUCT_PID 0x1001

bool winHid::btpsSenseConnect()
{
    bool ret = false;
    struct hid_device_info *devs, *cur_dev;

    if (hid_init())
        return false;
    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;
    while (cur_dev) {
//        printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
        if ((cur_dev->vendor_id == PRODUCT_VID) && (cur_dev->product_id == PRODUCT_PID))
        {
            ret = true;
            break;
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
    return ret;
}

void winHid::senseConnectCheck()
{
    if(m_currentTm.elapsed() < 2500)
    {
        return;
    }
    m_currentTm.restart();

    bool result = btpsSenseConnect();
    if(result != m_btpsSenseAvailable)
    {
        //发送指令
        m_btpsSenseAvailable = result;
        if(result)
        {
            //初始化串口，打开操作
            m_handle = hid_open(PRODUCT_VID, PRODUCT_PID, NULL);
            if (!m_handle) {
                qDebug()<<"unable to open device\n";
            }
            notifyUIBtpsSenseConnect(true);
        }
        else
        {
            hid_close(m_handle);
            notifyUIBtpsSenseConnect(false);
        }
    }

}

void winHid::run()
{
    int nread=0;
    char buff[512];

    QByteArray receiveData;

    fd_set rfds;
    int retval=0;
    struct timeval tv;

    m_currentTm= QTime::currentTime();
    m_currentTm.start();

    while (!quit)
    {
        senseConnectCheck();

        if(m_btpsSenseAvailable)
        {
//            tv.tv_sec = 0;
//            tv.tv_usec = 1000*50;

            FD_ZERO(&rfds);
            FD_SET(m_fd,&rfds);
            nread = hid_read(m_handle, (unsigned char*) buff, sizeof(buff));
            if(nread>0)
            {
                receiveData=QByteArray::fromRawData(buff,nread);
                pushData(receiveData);
            }
        }

        qApp->processEvents();
        msleep(500);
    }

    if(m_fd>0)
    {
        close(m_fd);
    }
    qDebug()<<Q_FUNC_INFO<<"end";

}

void winHid::pushData(const QByteArray &d)
{
    tmpPmanage->receivePushData(d,tmpPmanage->getData(DataManage::BTPS_SENSE));
}

bool winHid::getSnedData(QByteArray &d)
{
//    return (tmpPmanage->getSendData(d));
    return false;
}
