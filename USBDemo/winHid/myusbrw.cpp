#include <Windows.h>
#include <WinBase.h>
#include <Dbt.h>
#include <SetupAPI.h>
#include <QString>
#include <QDebug>
#include <QThread>

extern"C"
{
    #include"winHid/hidsdi.h"
}
#include "myusbrw.h"

#define MAX_REPORT_BUF_LEN          65  // 报告的缓冲区大小(保告ID(1 byte) + 实际数据大小)
#define MAX_USB_REPORT_COUNT        32

QString MyDevPathName=""; // 用来保存找到的设备路径
bool    MyDevFound=FALSE; // 用来保存设备是否已经找到
DWORD   MyVid, MyPid, MyPvn; // 定义变量用来保存VID PID 协议版本号

bool    DataInSending=FALSE; // 正在发送数据的标志
bool    dataInRead = false;
UCHAR   WriteReportBuffer[MAX_REPORT_BUF_LEN]={0}; //发送报告的缓冲区，1字节报告ID+报告数据
UCHAR   ReadReportBuffer[MAX_REPORT_BUF_LEN]={0}; //接收报告的缓冲区，1字节报告ID+报告数据
OVERLAPPED  WriteOverlapped; // 发送报告用的OVERLAPPED（重叠IO操作结构体）
OVERLAPPED  ReadOverlapped; // 接收报告用的OVERLAPPED

DEV_BROADCAST_DEVICEINTERFACE DevBroadcastDeviceInterface;

myUsbRW::myUsbRW(QObject *parent) : QObject(parent)
{
    mhReadHandle = INVALID_HANDLE_VALUE;
    mhWriteHandle = INVALID_HANDLE_VALUE;
    WriteOverlapped.Offset = 0;
    WriteOverlapped.OffsetHigh = 0;
    WriteOverlapped.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
    ReadOverlapped.Offset = 0;
    ReadOverlapped.OffsetHigh = 0;
    ReadOverlapped.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
    mbReadPending = false;
}
myUsbRW::~myUsbRW()
{
    CloseHandle(ReadOverlapped.hEvent);
    CloseHandle(WriteOverlapped.hEvent);
}

bool myUsbRW::openUsbPort(int vendorId, int productId)
{
    mvendorID = vendorId;
    mproductID = productId;
    // TODO: Add your control notification handler code here
    //定义一个GUID的结构体HidGuid来保存HID设备的接口类GUID。
    GUID HidGuid;
    //定义一个DEVINFO的句柄hDevInfoSet来保存获取到的设备信息集合句柄
    HDEVINFO hDevInfoSet;
    //定义MemberIndex，表示当前搜索到第几个设备，0表示第一个设备。
    DWORD MemberIndex;
    //DevInterfaceData 用来保存设备的驱动接口信息
    SP_DEVICE_INTERFACE_DATA DevInterfaceData;
    //保存函数调用是否返回成功
    BOOL Result;
    //用来接收需要保存详细信息的缓冲区长度
    DWORD RequiredSize;
    //定义一个指向设备详细信息的结构体指针
    PSP_DEVICE_INTERFACE_DETAIL_DATA pDevDetailData;
    //定义一个用来保存打开设备的句柄
    HANDLE hDevHandle;
    //保存设备的属性
    HIDD_ATTRIBUTES DevAttributes;

    MyDevFound=false; //初始化设备未找到

    mhReadHandle = INVALID_HANDLE_VALUE;
    mhWriteHandle = INVALID_HANDLE_VALUE;

    DevInterfaceData.cbSize = sizeof(DevInterfaceData);
    DevAttributes.Size = sizeof(DevAttributes);

    HidD_GetHidGuid(&HidGuid);

    // 获取设备句柄列表
    hDevInfoSet=SetupDiGetClassDevs(&HidGuid,
                                    NULL,
                                    NULL,
                                    DIGCF_DEVICEINTERFACE|DIGCF_PRESENT);
    MemberIndex = 0;
    while(1)
    {
        // 获取设备接口信息
        Result=SetupDiEnumDeviceInterfaces(hDevInfoSet,
                                            NULL,
                                            &HidGuid,
                                            MemberIndex,
                                            &DevInterfaceData);
        // 获取信息失败退出检测
        if(Result==FALSE)
            break;

        MemberIndex++;

        // 获取设备接口详细信息
        Result=SetupDiGetDeviceInterfaceDetail(hDevInfoSet,
                                                &DevInterfaceData,
                                                NULL,
                                                NULL,
                                                &RequiredSize,
                                                NULL);

        // 分配内存
        pDevDetailData=(PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(RequiredSize);
        if(pDevDetailData==NULL)
        {
            SetupDiDestroyDeviceInfoList(hDevInfoSet);
            return false;
        }

        // 初始化
        pDevDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        // 获取设备属性
        Result=SetupDiGetDeviceInterfaceDetail(hDevInfoSet,
                                                &DevInterfaceData,
                                                pDevDetailData,
                                                RequiredSize,
                                                NULL,
                                                NULL);
        // 获取设备地址
        MyDevPathName= QString::fromWCharArray(pDevDetailData->DevicePath);
        free(pDevDetailData);

        if(Result==FALSE)
            continue;

        // 获取已打开设备的操作句柄
        hDevHandle=CreateFile(reinterpret_cast<const wchar_t *>(MyDevPathName.utf16()),
                                NULL,
                                FILE_SHARE_READ|FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
        if(hDevHandle != INVALID_HANDLE_VALUE)
        {
            // 获取设备属性
            Result = HidD_GetAttributes(hDevHandle,&DevAttributes);
            CloseHandle(hDevHandle);
            if(Result==FALSE)
                continue;
            // 过滤设备
            if(DevAttributes.VendorID == mvendorID)
                if(DevAttributes.ProductID == mproductID)
                {
                    MyDevFound = TRUE;
                    mhReadHandle=CreateFile(reinterpret_cast<const wchar_t *>(MyDevPathName.utf16()),
                                            GENERIC_READ,
                                            FILE_SHARE_READ|FILE_SHARE_WRITE,
                                            NULL,
                                            OPEN_EXISTING,
                                            FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
                                            NULL);
                    if(mhReadHandle != INVALID_HANDLE_VALUE)
                    {
                        QString qstrReadOpenInfo("open device with read succeed!");
                        qDebug() << qstrReadOpenInfo; //AddToInfOut("读访问打开设备成功!");
                        emit SignalSendLog(QString::fromLocal8Bit("读访问打开设备成功!"));
                    }
                    else
                    {
                        QString qstrReadOpenInfo("open device with read fail!");
                        qDebug() << qstrReadOpenInfo;
                        emit SignalSendLog(QString::fromLocal8Bit("读访问打开设备失败!"));
                        return false;// AddToInfOut("读访问打开设备失败!");
                    }

                    qDebug()<<"founded";

                    mhWriteHandle = CreateFile(reinterpret_cast<const wchar_t *>(MyDevPathName.utf16()),
                                            GENERIC_WRITE,
                                            FILE_SHARE_READ|FILE_SHARE_WRITE,
                                            NULL,
                                            OPEN_EXISTING,
                                            FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
                                            NULL);
                    if(mhWriteHandle != INVALID_HANDLE_VALUE)
                    {
                        QString qstrWtiteOpenInfo("open device with write succeed!");
                        qDebug() << qstrWtiteOpenInfo;
                        emit SignalSendLog(QString::fromLocal8Bit("写访问打开设备成功!"));
                    }
                    else
                    {
                        QString qstrWtiteOpenInfo("open device with write fail!");
                        qDebug() << qstrWtiteOpenInfo;
                        emit SignalSendLog(QString::fromLocal8Bit("写访问打开设备失败!"));
                        return false;
                    }
                    DataInSending = FALSE;
                    SetEvent(ReadOverlapped.hEvent);
                    break;
                }
        }
        else
            continue;
    }
    // 释放资源
    SetupDiDestroyDeviceInfoList(hDevInfoSet);
    if(MyDevFound)
    {
        return true;
    }
    else
    {
        emit SignalSendLog(QString::fromLocal8Bit("设备没有找到!"));
        return false;
    }
}

bool myUsbRW::closeUsbPort()
{
    if(INVALID_HANDLE_VALUE != mhReadHandle)
    {
        CloseHandle(mhReadHandle);
        mhReadHandle = INVALID_HANDLE_VALUE;
    }
    if(INVALID_HANDLE_VALUE != mhWriteHandle)
    {
        CloseHandle(mhWriteHandle);
        mhWriteHandle = INVALID_HANDLE_VALUE;
    }    
    return true;
}

void myUsbRW::writeData(char *data, int ilen)
{
    QByteArray baa = QByteArray((char *)data, ilen);
    BOOL Result;
    UINT LastError;    
    if(MyDevFound == FALSE)
    {
        emit SignalSendLog(QString::fromLocal8Bit("设备没有找到!"));
        return;
    }    
    if(mhWriteHandle == INVALID_HANDLE_VALUE)
    {
        emit SignalSendLog(QString::fromLocal8Bit("无效的写句柄，可能是打开设备时失败!"));
        return;
    }    
    if(DataInSending == TRUE)
    {
        emit SignalSendLog(QString::fromLocal8Bit("数据正在发放中，暂时不能发送!"));
        return;
    }

    //设置要发送报告的数据
    WriteReportBuffer[0]=0x00; //报告ID为0
    for(int idx =0; idx < ilen; idx ++)
    {
        WriteReportBuffer[idx] = baa[idx ];
    }
    QByteArray ba = QByteArray::fromRawData((char *)WriteReportBuffer, sizeof(WriteReportBuffer));
    //设置正在发送标志
    DataInSending = TRUE;
    //调用WriteFile函数发送数据
    Result = WriteFile(mhWriteHandle,
                       &WriteReportBuffer,
                       ilen + 1,
                       NULL,
                    &WriteOverlapped);

    //如果函数返回失败，则可能是真的失败，也可能是IO挂起了
    if(Result == FALSE)
    {
        //获取最后错误代码
        LastError = GetLastError();        
        //看是否是真的IO挂起
        if((LastError == ERROR_IO_PENDING)||(LastError == ERROR_SUCCESS))
        {
            DataInSending = FALSE;
            return;
        }
        //否则，是函数调用时发生错误，显示错误代码
        else
        {
            QString qstrErrorCode = QString::number(LastError);
            QString qstrErrorInfo = QString("send data error, errorcode: %1").arg(qstrErrorCode);
            emit SignalSendLog(qstrErrorInfo);
            DataInSending = FALSE;
            return;
        }        
    }
    //否则，函数返回成功
    else
    {
        QString qstrData(data);
        emit SignalSendLog(QString("send data :%1").arg(qstrData));
        DataInSending = FALSE;       
        return;
    }
}
int   myUsbRW::hid_read_timeout( unsigned char *data, size_t length, int milliseconds)
{
    DWORD bytes_read = 0;
    size_t copy_len = 0;
    BOOL res = FALSE;
    BOOL overlapped = FALSE;

    /* Copy the handle for convenience. */
    HANDLE ev = ReadOverlapped.hEvent;
    while(1)
    {
        if (!mbReadPending) {
            /* Start an Overlapped I/O read. */
            mbReadPending = TRUE;
            HidD_SetNumInputBuffers(mhReadHandle, MAX_REPORT_BUF_LEN);
            memset(ReadReportBuffer, 0, MAX_REPORT_BUF_LEN);
            ResetEvent(ev);
            bool bReportResult = HidD_GetInputReport(mhReadHandle, ReadReportBuffer, MAX_REPORT_BUF_LEN);

            res = ReadFile(mhReadHandle, ReadReportBuffer, (DWORD) MAX_REPORT_BUF_LEN, &bytes_read, &ReadOverlapped);

            if (!res) {
                if (GetLastError() != ERROR_IO_PENDING) {
                    /* ReadFile() has failed.
                       Clean up and return error. */
                    CancelIo(mhReadHandle);
                    mbReadPending = FALSE;
                    goto end_of_function;
                }
                overlapped = TRUE;
            }
        }
        else {
            overlapped = TRUE;
            qDebug()<<"error2";
        }

        if (overlapped) {
            if (milliseconds >= 0) {
                /* See if there is any data yet. */
                res = WaitForSingleObject(ev, milliseconds);
                if (res != WAIT_OBJECT_0) {
                    /* There was no data this time. Return zero bytes available,
                       but leave the Overlapped I/O running. */
                    qDebug()<<"error321"<<milliseconds;
                    QString qstrTimeOut = QString("revice data timeout %1 ms").arg(milliseconds);
                    emit SignalSendLog(qstrTimeOut);
                    mbReadPending = FALSE;
                    return 0;
                }
            }

            /* Either WaitForSingleObject() told us that ReadFile has completed, or
               we are in non-blocking mode. Get the number of bytes read. The actual
               data has been copied to the data[] array which was passed to ReadFile(). */
            res = GetOverlappedResult(mhReadHandle, &ReadOverlapped, &bytes_read, TRUE/*wait*/);
        }
        /* Set pending back to false, even if GetOverlappedResult() returned error. */
        mbReadPending = FALSE;

        if (res && bytes_read > 0) {
            if (ReadReportBuffer[0] == 0x0) {
                /* If report numbers aren't being used, but Windows sticks a report
                   number (0x0) on the beginning of the report anyway. To make this
                   work like the other platforms, and to make it work more like the
                   HID spec, we'll skip over this byte. */
                bytes_read--;
                copy_len = length > bytes_read ? bytes_read : length;
                memcpy(data, ReadReportBuffer+1, copy_len);
                mbaData = QByteArray((char *) ReadReportBuffer+1,copy_len);
                //qDebug()<<mbaData.toHex()<<copy_len<<"readdata";
                break;
            }
            else {
                /* Copy the whole buffer, report number and all. */
                copy_len = length > bytes_read ? bytes_read : length;
                memcpy(data, ReadReportBuffer, copy_len);
            }
            qDebug()<<"error4";
        }
    }



end_of_function:
    if (!res) {
        //register_error(dev, "GetOverlappedResult");
        return -1;
    }

    return (int) copy_len;
}
void myUsbRW::readDataWhile()
{
    DWORD Length;
    UINT i;
    while(1)
    {
        ResetEvent(ReadOverlapped.hEvent);
        mbaData.resize(0);
        qDebug()<<"mdf";
        if(MyDevFound == TRUE)
        {
            if(mhReadHandle == INVALID_HANDLE_VALUE)
            {
                qDebug()<<"ihv";
                break;
            }
            else
            {

                ReadFile(mhReadHandle,
                        ReadReportBuffer,
                        MAX_REPORT_BUF_LEN - 1,
                        NULL,
                        &ReadOverlapped);
            }            
            WaitForSingleObject(ReadOverlapped.hEvent,1);
            qDebug()<<MyDevFound<<"rf2";
            if(MyDevFound == FALSE)
                continue;
            GetOverlappedResult(mhReadHandle, &ReadOverlapped, &Length, TRUE);
            qDebug()<<"rf3";
            if(Length != 0)
            {
                //pAppDlg->AddToInfOut("读取报告：" + pAppDlg->itos(Length) + "字节");
                mbaData = QByteArray((char *) ReadReportBuffer+1,Length-1);
                qDebug()<<mbaData.toHex()<<Length<<"readdata";
                break;
            }
        }
        else
        {
            qDebug()<<"rf4";
            WaitForSingleObject(ReadOverlapped.hEvent, 5);
        }
    }
    return ;
}


int myUsbRW::readData(char *data,int ilen ,int milliseconds)
{
    DWORD bytes_read = 0;
    size_t copy_len = 0;
    BOOL res = FALSE;
    BOOL overlapped = FALSE;

    HANDLE ev = ReadOverlapped.hEvent;
//qDebug()<<DataInSending<<"+++281";
    if (!dataInRead) {
        /* Start an Overlapped I/O read. */
        dataInRead = TRUE;
        memset(data, 0, ilen);
        ResetEvent(ev);
        res = ReadFile(mhReadHandle, data, (DWORD) ilen, &bytes_read, &ReadOverlapped);

        if (!res) {
            if (GetLastError() != ERROR_IO_PENDING) {
                CancelIo(mhReadHandle);
                dataInRead = FALSE;
                //qDebug()<<DataInSending<<"+++295";
                goto end_of_function;
            }
            overlapped = TRUE;
        }
    }
    else {
        overlapped = TRUE;
    }

    if (overlapped) {
        if (milliseconds >= 0) {
            res = WaitForSingleObject(ev, milliseconds);
            if (res != WAIT_OBJECT_0) {
                dataInRead = FALSE;
                return 0;
            }
        }
        res = GetOverlappedResult(mhReadHandle, &ReadOverlapped, &bytes_read, TRUE/*wait*/);
    }
    dataInRead = FALSE;

    if (res && bytes_read > 0) {
        if (data == 0x0) {
            bytes_read--;
            copy_len = ilen > bytes_read ? bytes_read : ilen;
            memcpy(data, data+1, copy_len);
        }
        else {
            copy_len = ilen > bytes_read ? bytes_read : ilen;
            memcpy(data, data, copy_len);
        }
    }
    //qDebug()<<bytes_read<<res<<"+++66";

end_of_function:
    if (!res) {
        //register_error(dev, "GetOverlappedResult");
        dataInRead = FALSE;
        return -1;
    }
    dataInRead = FALSE;

    return (int) copy_len;
#if 0
    DWORD Length = 0;
    UINT i;
    bool res = true;
    ResetEvent(ReadOverlapped.hEvent);
    if(MyDevFound == TRUE)
    {
        if(mhReadHandle == INVALID_HANDLE_VALUE)
        {
            ;//pAppDlg->AddToInfOut("无效的读报告句柄，可能是打开设备时失败!");
        }
        else
        {
            res = ReadFile(mhReadHandle,
                           data,
                           64,
                           &Length,
                           &ReadOverlapped);
            qDebug()<<res<<Length<<"readData";
            if (!res)
            {
                if (GetLastError() != ERROR_IO_PENDING) {
                    //mportState = false;
                    return 0;
                }
            }
            mportState = true;
        }
        WaitForSingleObject(ReadOverlapped.hEvent,INFINITE);

        GetOverlappedResult(mhReadHandle, &ReadOverlapped, &Length, TRUE);
        if(Length != 0)
        {
            //pAppDlg->AddToInfOut("读取报告：" + pAppDlg->itos(Length) + "字节");
            for(i = 0; i< Length; i++)
            {
                ;//Str += pAppDlg->itos(ReadReportBuffer[i],16).Right(2) + " ";
            }
        }
    }
    else
    {
        WaitForSingleObject(ReadOverlapped.hEvent, INFINITE);
    }

    return Length;
#endif
}
