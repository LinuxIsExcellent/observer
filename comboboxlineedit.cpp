#include "comboboxlineedit.h"
#include "ui_comboboxlineedit.h"
#include <QDebug>
#include "mainwindow.h"

ComboboxLineedit::ComboboxLineedit(QModelIndex index, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComboboxLineedit),
    m_index(index)
{
    ui->setupUi(this);
    connect(ui->comboBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(OnChangeCurrentText(const QString &)));
    connect(ui->jumpBtn, SIGNAL(clicked()), this, SLOT(OnJumpButtonClicked()));
    ui->jumpBtn->setToolTip(tr("跳转到字段关联的表"));
    ui->comboBox->setToolTip(tr("展开关联字段现有项"));
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

void ComboboxLineedit::OnJumpButtonClicked()
{
    QString sLinkInfo = m_index.data(Qt::UserRole+3).toString();
    QString sField = text();

    MainWindow* mainWindow = GlobalConfig::getInstance()->GetMainWindow();
    if (mainWindow)
    {
        mainWindow->OnJumpLinkTable(sLinkInfo, sField);
    }
}

void ComboboxLineedit::OnChangeCurrentText(const QString &str)
{
    qDebug() << "str" << str;
}
