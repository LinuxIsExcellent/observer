#include "comboboxlineedit.h"
#include "ui_comboboxlineedit.h"
#include <QDebug>

ComboboxLineedit::ComboboxLineedit(QModelIndex index, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComboboxLineedit),
    m_index(index)
{
    ui->setupUi(this);
    connect(ui->comboBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(OnChangeCurrentText(const QString &)));

    GlobalConfig::getInstance()->GetMainWindow();
}

ComboboxLineedit::~ComboboxLineedit()
{
    delete ui;
}

void ComboboxLineedit::setText(QString str)
{
    ui->lineEdit->setText(str);
    int nIndex = ui->comboBox->findText(str);
    if(nIndex >= 0)
    {
        ui->comboBox->setCurrentIndex(nIndex);
    }
}

QString ComboboxLineedit::text()
{
    return ui->lineEdit->text();
}

void ComboboxLineedit::addCombox(QString str)
{
    ui->comboBox->addItem(str);
}

void ComboboxLineedit::resizeEvent(QResizeEvent *)
{
    ui->comboBox->setFixedWidth(geometry().width() - 20);
    ui->comboBox->setFixedHeight(geometry().height());
    ui->lineEdit->setFixedWidth(geometry().width() - 33);
    ui->lineEdit->setFixedHeight(geometry().height());
    ui->jumpBtn->setFixedHeight(geometry().height());
}

void ComboboxLineedit::OnChangeCurrentText(const QString &str)
{
    qDebug() << "str" << str;
}
