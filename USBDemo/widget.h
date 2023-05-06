#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include "rwusbport.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void SiganlAndSlot();

public slots:
    void SlotStartConnect();
    void SlotDisconnect();
    void SlotReceiveUsbData(QByteArray data);
    void SlotCleanInfo();
    void SlotSendData();
    void AddLog(const QString &info);

private:
    Ui::Widget *ui;
    rwUsbPort * musbPort;
};

#endif // WIDGET_H
