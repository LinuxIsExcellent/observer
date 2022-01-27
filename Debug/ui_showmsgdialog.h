/********************************************************************************
** Form generated from reading UI file 'showmsgdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHOWMSGDIALOG_H
#define UI_SHOWMSGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ShowMsgDialog
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QTextEdit *textEdit;

    void setupUi(QDialog *ShowMsgDialog)
    {
        if (ShowMsgDialog->objectName().isEmpty())
            ShowMsgDialog->setObjectName(QStringLiteral("ShowMsgDialog"));
        ShowMsgDialog->resize(400, 300);
        verticalLayout = new QVBoxLayout(ShowMsgDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(4, 4, 4, 4);
        widget = new QWidget(ShowMsgDialog);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setMinimumSize(QSize(0, 35));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(4, 4, 4, 4);
        horizontalSpacer = new QSpacerItem(344, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton = new QPushButton(widget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setMinimumSize(QSize(31, 31));
        pushButton->setMaximumSize(QSize(31, 31));

        horizontalLayout->addWidget(pushButton);


        verticalLayout->addWidget(widget);

        textEdit = new QTextEdit(ShowMsgDialog);
        textEdit->setObjectName(QStringLiteral("textEdit"));

        verticalLayout->addWidget(textEdit);


        retranslateUi(ShowMsgDialog);

        QMetaObject::connectSlotsByName(ShowMsgDialog);
    } // setupUi

    void retranslateUi(QDialog *ShowMsgDialog)
    {
        ShowMsgDialog->setWindowTitle(QApplication::translate("ShowMsgDialog", "Dialog", Q_NULLPTR));
        pushButton->setText(QApplication::translate("ShowMsgDialog", "X", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ShowMsgDialog: public Ui_ShowMsgDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHOWMSGDIALOG_H
