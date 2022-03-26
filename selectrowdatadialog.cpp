#include "selectrowdatadialog.h"
#include "ui_selectrowdatadialog.h"

#include <QCheckBox>
#include <QDebug>

SelectRowDataDialog::SelectRowDataDialog(int nIndex, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectRowDataDialog),
    m_nIndex(nIndex)
{
    ui->setupUi(this);

    m_pTargetTableView = static_cast<QTableView*>(parent);
    m_pTargetModel = static_cast<QStandardItemModel*>(m_pTargetTableView->model());

    m_model = new QStandardItemModel(ui->tableView);
    ui->tableView->setModel(m_model);

    ui->tableView->verticalHeader()->setHidden(true);

    m_vCheckBoxs.clear();

    m_model->setHorizontalHeaderItem(0, new QStandardItem(""));
    m_model->setHorizontalHeaderItem(1, new QStandardItem("名称"));
    m_model->setHorizontalHeaderItem(2, new QStandardItem("数量"));

    connect(ui->cancelBtn, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->confirmBtn, SIGNAL(clicked()), this, SLOT(OnConfirmBtnClicked()));

    this->setWindowFlags(Qt::Popup);
    setWindowTitle("筛选");

    InitDialog();
}

SelectRowDataDialog::~SelectRowDataDialog()
{
    for (auto& data : m_vCheckBoxs)
    {
        delete data;
        data = nullptr;
    }

    m_vCheckBoxs.clear();
    delete ui;
}

void SelectRowDataDialog::OnConfirmBtnClicked()
{
    QStringList stringList;
    for (int i = 0;i < m_vCheckBoxs.size(); ++i)
    {
        if (m_vCheckBoxs[i]->checkState() == Qt::Checked)
        {
            stringList << m_model->index(i, 1).data().toString();
        }
    }

    for (int row = 1; row < m_pTargetModel->rowCount(); ++row)
    {
        QString sTargetData = m_pTargetModel->index(row, m_nIndex).data().toString();
        m_pTargetTableView->setRowHidden(row, stringList.contains(sTargetData) == false);
    }

    close();
}

void SelectRowDataDialog::InitDialog()
{
    int nRow = m_pTargetModel->rowCount();

    QVector<QString> vFieldData;
    QMap<QString, int> mMap;
    int nEmpty = 0;
    for (int i = 1; i < nRow; ++i)
    {
        if (m_pTargetTableView->isRowHidden(i) == false)
        {
            QModelIndex index = m_pTargetModel->index(i, m_nIndex);
            QString sValue = "";
            if (index.isValid())
            {
                sValue = index.data().toString();
                if (sValue != "")
                {
                    if (mMap.find(sValue) != mMap.end())
                    {
                        mMap.find(sValue).value() = mMap.find(sValue).value() + 1;
                    }
                    else
                    {
                        mMap.insert(sValue, 1);
                        vFieldData.push_back(sValue);
                    }
                }
            }

            if (sValue == "")
            {
                nEmpty++;
            }
        }
    }

    if(nEmpty > 0)
    {
        mMap.insert("(空白)", nEmpty);
        vFieldData.push_front("(空白)");
    }

    //把全选放在最前面
    mMap.insert("(全选)", nRow);
    vFieldData.push_front("(全选)");

    int i = 0;
    for (auto data : vFieldData)
    {
        auto iter = mMap.find(data);
        if (iter != mMap.end())
        {
            QString sField = iter.key();
            QString sNum = QString::number(iter.value());

            QStandardItem* item = new QStandardItem();
            item->setFlags(Qt::ItemIsEnabled);
            m_model->setItem(i, 0, item);

            QCheckBox* checkBox = new QCheckBox();
            checkBox->setCheckState(Qt::Checked);
            ui->tableView->setIndexWidget(item->index(), checkBox);

            checkBox->setToolTip(QString::number(i));
            m_vCheckBoxs.push_back(checkBox);
            connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(OnAllCheckBoxStateChange(int)));

            QStandardItem* item1 = new QStandardItem(sField);
            item1->setFlags(Qt::ItemIsEnabled);
            item1->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            m_model->setItem(i, 1, item1);

            QStandardItem* item2 = new QStandardItem(sNum);
            item2->setFlags(Qt::ItemIsEnabled);
            item2->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            m_model->setItem(i, 2, item2);

            i++;
        }
    }

    ui->tableView->setColumnWidth(0, 20);
}

void SelectRowDataDialog::OnAllCheckBoxStateChange(int nState)
{
    QObject* sender = QObject::sender();
    int nIndex = 0;
    if (sender->metaObject()->className() == QStringLiteral("QCheckBox"))
    {
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(sender);
        nIndex = checkBox->toolTip().toInt();
    }

    if (nIndex == 0)
    {
        for (int i = 1;i < m_vCheckBoxs.size();++i)
        {
            disconnect(m_vCheckBoxs[i], SIGNAL(stateChanged(int)), this, SLOT(OnAllCheckBoxStateChange(int)));
            m_vCheckBoxs[i]->setCheckState(Qt::CheckState(nState));
            connect(m_vCheckBoxs[i], SIGNAL(stateChanged(int)), this, SLOT(OnAllCheckBoxStateChange(int)));
        }
    }
    else
    {
        //如果单个选中
        bool change = true;
        if (nState == Qt::Checked)
        {
            for (int i = 1;i < m_vCheckBoxs.size();++i)
            {
                if (m_vCheckBoxs[i]->checkState() != Qt::Checked)
                {
                    change = false;
                    break;
                }
            }
        }

        if (change)
        {
            disconnect(m_vCheckBoxs[0], SIGNAL(stateChanged(int)), this, SLOT(OnAllCheckBoxStateChange(int)));
            m_vCheckBoxs[0]->setCheckState(Qt::CheckState(nState));
            connect(m_vCheckBoxs[0], SIGNAL(stateChanged(int)), this, SLOT(OnAllCheckBoxStateChange(int)));
        }
    }

    bool noneChecked = true;
    for (int i = 1;i < m_vCheckBoxs.size();++i)
    {
        if (m_vCheckBoxs[i]->checkState() == Qt::Checked)
        {
            noneChecked = false;
            break;
        }
    }

    ui->confirmBtn->setDisabled(noneChecked);
}
