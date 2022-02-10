#include "modifyservertimewidget.h"
#include "ui_modifyservertimewidget.h"
#include <QDebug>

ModifyServerTimeWidget::ModifyServerTimeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModifyServerTimeWidget)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this,[=](){

    });

    connect(ui->pushButton_2, &QPushButton::clicked, this,[=](){
        this->hide();
    });

    connect(ui->calendarWidget, &QCalendarWidget::clicked, this, [=](const QDate &date) {
        ui->dateTimeEdit->setDate(date);
    });
}

ModifyServerTimeWidget::~ModifyServerTimeWidget()
{
    delete ui;
}

void ModifyServerTimeWidget::SetTime(quint64 nTime)
{
    QDateTime time = QDateTime::fromTime_t(nTime);
    ui->calendarWidget->setMinimumDate(time.date());
    ui->dateTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    ui->dateTimeEdit->setDateTime(time);
}
