#include "comboboxlineedit.h"
#include "ui_comboboxlineedit.h"
#include <QDebug>
#include "mainwindow.h"

ComboboxLineedit::ComboboxLineedit(QModelIndex index, QString sValue, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComboboxLineedit),
    m_index(index)
{
    ui->setupUi(this);

    ui->lineEdit->setText(sValue);

    connect(ui->comboBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(OnChangeCurrentText(const QString &)));
    connect(ui->jumpBtn, SIGNAL(clicked()), this, SLOT(OnJumpButtonClicked()));
    ui->jumpBtn->setToolTip(tr("跳转到字段关联的表"));
    ui->comboBox->setToolTip(tr("展开关联字段现有项"));
    ui->comboBox->installEventFilter(this);

    OnRequestLinkFieldInfo();
}

ComboboxLineedit::~ComboboxLineedit()
{
    delete ui;
}

bool ComboboxLineedit::eventFilter(QObject *watched, QEvent *event)
{
    if (qobject_cast<QComboBox*>(watched) == ui->comboBox && event->type() == QEvent::MouseButtonPress)
    {
        MainWindow* mainWindow = GlobalConfig::getInstance()->GetMainWindow();
        if (mainWindow)
        {
            QString sLinkInfo = m_index.data(Qt::UserRole+3).toString();
            QVector<COMBOXFIELDINFO>* vFieldInfo = mainWindow->GetComboxFieldInfoByKey(sLinkInfo);
            if(vFieldInfo)
            {
                disconnect(ui->comboBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(OnChangeCurrentText(const QString &)));

                ui->comboBox->clear();
                QStandardItemModel *model = new QStandardItemModel();
                for (int i = 0; i < vFieldInfo->size();++i)
                {
                    QStandardItem *item = new QStandardItem((*vFieldInfo)[i].sValue);;
                    item->setToolTip((*vFieldInfo)[i].sDesc);
                    model->appendRow(item);

                    addCombox((*vFieldInfo)[i].sValue);
                }

                ui->comboBox->setModel(model);

                connect(ui->comboBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(OnChangeCurrentText(const QString &)));
            }
        }

        int nIndex = ui->comboBox->findText(text());
        if(nIndex >= 0)
        {
            ui->comboBox->setCurrentIndex(nIndex);
        }
    }
    return false;
}

void ComboboxLineedit::OnRequestLinkFieldInfo()
{
    QString sLinkInfo = m_index.data(Qt::UserRole+3).toString();
    MainWindow* mainWindow = GlobalConfig::getInstance()->GetMainWindow();
    if (mainWindow)
    {
        mainWindow->OnRequestFieldInfoByLink(sLinkInfo);
    }
}

void ComboboxLineedit::setText(QString str)
{
    if (str != ui->lineEdit->text())
    {
        ui->lineEdit->setText(str);

        if (m_index.isValid())
        {
            QStandardItemModel* standardModel = (QStandardItemModel*)m_index.model();
            if (standardModel)
            {
                QStandardItem* standardItem = standardModel->item(m_index.row(), m_index.column());
                if (standardItem)
                {
                    emit standardModel->itemChanged(standardItem);
                }
            }
        }
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
    setText(str);
}
