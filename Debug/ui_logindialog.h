/********************************************************************************
** Form generated from reading UI file 'logindialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QLabel *label;
    QComboBox *comboBox;
    QListView *listView;
    QPushButton *connect_btn;
    QPushButton *quit_btn;
    QPushButton *close_button;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName(QStringLiteral("LoginDialog"));
        LoginDialog->resize(400, 300);
        label = new QLabel(LoginDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(80, 20, 71, 41));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        comboBox = new QComboBox(LoginDialog);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(80, 60, 121, 31));
        listView = new QListView(LoginDialog);
        listView->setObjectName(QStringLiteral("listView"));
        listView->setGeometry(QRect(80, 100, 191, 101));
        connect_btn = new QPushButton(LoginDialog);
        connect_btn->setObjectName(QStringLiteral("connect_btn"));
        connect_btn->setGeometry(QRect(210, 250, 75, 23));
        quit_btn = new QPushButton(LoginDialog);
        quit_btn->setObjectName(QStringLiteral("quit_btn"));
        quit_btn->setGeometry(QRect(300, 250, 75, 23));
        close_button = new QPushButton(LoginDialog);
        close_button->setObjectName(QStringLiteral("close_button"));
        close_button->setGeometry(QRect(360, 10, 31, 31));

        retranslateUi(LoginDialog);

        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QApplication::translate("LoginDialog", "Dialog", Q_NULLPTR));
        label->setText(QApplication::translate("LoginDialog", "\351\200\211\346\213\251\351\241\271\347\233\256", Q_NULLPTR));
        connect_btn->setText(QApplication::translate("LoginDialog", "login", Q_NULLPTR));
        quit_btn->setText(QApplication::translate("LoginDialog", "quit", Q_NULLPTR));
        close_button->setText(QApplication::translate("LoginDialog", "X", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
