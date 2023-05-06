/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_6;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit_VID;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *lineEdit_PID;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *pBtn_connect;
    QPushButton *pBtn_disconnect;
    QSpacerItem *horizontalSpacer_4;
    QTextEdit *textEdit_info;
    QLineEdit *lineEdit_write;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pBtn_send;
    QPushButton *pBtn_clean;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QStringLiteral("Widget"));
        Widget->resize(373, 322);
        verticalLayout = new QVBoxLayout(Widget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(Widget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        lineEdit_VID = new QLineEdit(Widget);
        lineEdit_VID->setObjectName(QStringLiteral("lineEdit_VID"));

        horizontalLayout->addWidget(lineEdit_VID);


        horizontalLayout_6->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(Widget);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        lineEdit_PID = new QLineEdit(Widget);
        lineEdit_PID->setObjectName(QStringLiteral("lineEdit_PID"));

        horizontalLayout_2->addWidget(lineEdit_PID);


        horizontalLayout_6->addLayout(horizontalLayout_2);


        verticalLayout->addLayout(horizontalLayout_6);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);

        pBtn_connect = new QPushButton(Widget);
        pBtn_connect->setObjectName(QStringLiteral("pBtn_connect"));

        horizontalLayout_4->addWidget(pBtn_connect);

        pBtn_disconnect = new QPushButton(Widget);
        pBtn_disconnect->setObjectName(QStringLiteral("pBtn_disconnect"));

        horizontalLayout_4->addWidget(pBtn_disconnect);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_4);

        textEdit_info = new QTextEdit(Widget);
        textEdit_info->setObjectName(QStringLiteral("textEdit_info"));

        verticalLayout->addWidget(textEdit_info);

        lineEdit_write = new QLineEdit(Widget);
        lineEdit_write->setObjectName(QStringLiteral("lineEdit_write"));

        verticalLayout->addWidget(lineEdit_write);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        pBtn_send = new QPushButton(Widget);
        pBtn_send->setObjectName(QStringLiteral("pBtn_send"));

        horizontalLayout_3->addWidget(pBtn_send);

        pBtn_clean = new QPushButton(Widget);
        pBtn_clean->setObjectName(QStringLiteral("pBtn_clean"));

        horizontalLayout_3->addWidget(pBtn_clean);


        horizontalLayout_5->addLayout(horizontalLayout_3);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_5);

        verticalSpacer = new QSpacerItem(20, 24, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "Widget", 0));
        label->setText(QApplication::translate("Widget", "VID:", 0));
        lineEdit_VID->setText(QApplication::translate("Widget", "69A7", 0));
        label_2->setText(QApplication::translate("Widget", "PID:", 0));
        lineEdit_PID->setText(QApplication::translate("Widget", "9803", 0));
        pBtn_connect->setText(QApplication::translate("Widget", "\350\277\236\346\216\245", 0));
        pBtn_disconnect->setText(QApplication::translate("Widget", "\346\226\255\350\277\236", 0));
        pBtn_send->setText(QApplication::translate("Widget", "\345\217\221\351\200\201", 0));
        pBtn_clean->setText(QApplication::translate("Widget", "\346\270\205\351\231\244", 0));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
