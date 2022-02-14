/********************************************************************************
** Form generated from reading UI file 'tabwidgetcell.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TABWIDGETCELL_H
#define UI_TABWIDGETCELL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TabWidgetCell
{
public:
    QWidget *bottom_widget;

    void setupUi(QWidget *TabWidgetCell)
    {
        if (TabWidgetCell->objectName().isEmpty())
            TabWidgetCell->setObjectName(QStringLiteral("TabWidgetCell"));
        TabWidgetCell->resize(400, 300);
        bottom_widget = new QWidget(TabWidgetCell);
        bottom_widget->setObjectName(QStringLiteral("bottom_widget"));
        bottom_widget->setGeometry(QRect(120, 150, 120, 80));

        retranslateUi(TabWidgetCell);

        QMetaObject::connectSlotsByName(TabWidgetCell);
    } // setupUi

    void retranslateUi(QWidget *TabWidgetCell)
    {
        TabWidgetCell->setWindowTitle(QApplication::translate("TabWidgetCell", "Form", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class TabWidgetCell: public Ui_TabWidgetCell {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TABWIDGETCELL_H
