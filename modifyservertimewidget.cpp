#include "modifyservertimewidget.h"
#include "ui_modifyservertimewidget.h"
#include <QDebug>

ModifyServerTimeWidget::ModifyServerTimeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModifyServerTimeWidget)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this,[=](){
        emit OnClickConfirmBtn(ui->dateTimeEdit->dateTime().toTime_t());
        this->hide();
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

void ModifyServerTimeWidget::ShowTimeInfo()
{
    QDateTime time = QDateTime::fromTime_t(m_nTime);
    ui->calendarWidget->setMinimumDate(time.date());
    ui->dateTimeEdit->setDateTime(time);
}

void ModifyServerTimeWidget::TimeTick()
{
    m_nTime = ui->dateTimeEdit->dateTime().toTime_t() + 1;
    ShowTimeInfo();
}

void ModifyServerTimeWidget::SetTime(quint64 nTime)
{
    m_nTime = nTime;
    ui->dateTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    ShowTimeInfo();
}
