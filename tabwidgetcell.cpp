#include "tabwidgetcell.h"
#include "ui_tabwidgetcell.h"
TabWidgetCell::TabWidgetCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabWidgetCell)
{
    ui->setupUi(this);
    m_bHeadIndexChange = false;
    m_bTableDataChange = false;

    //初始化上边的widget和layout
    m_topWidget = new QWidget(this);

    m_tableView = new QTableView(this);
    m_rightButtonList = new QListWidget(this);

    hlayout_top = new QHBoxLayout(m_topWidget);

    hlayout_top->addWidget(m_tableView);
    hlayout_top->addWidget(m_rightButtonList);

    hlayout_top->setStretchFactor(m_tableView, 10);
    hlayout_top->setStretchFactor(m_rightButtonList, 2);

    m_topWidget->setLayout(hlayout_top);

    //初始化下边的widget和layout
    m_bottomButtonList = new QListWidget(this);

    vlayout_all = new QVBoxLayout(this);
    vlayout_all->addWidget(m_topWidget);
    vlayout_all->addWidget(m_bottomButtonList);

    vlayout_all->setStretchFactor(m_topWidget, 10);
    vlayout_all->setStretchFactor(m_bottomButtonList, 1);

    setLayout(vlayout_all);

    QPushButton* resizeContentBtn = new QPushButton(this);
    QListWidgetItem *item = new QListWidgetItem(m_rightButtonList);
    if (item)
    {
        resizeContentBtn->setText(tr("全展开"));
        m_rightButtonList->addItem(item);
        m_rightButtonList->setItemWidget(item, resizeContentBtn);

        connect(resizeContentBtn, SIGNAL(clicked()), m_tableView, SLOT(resizeColumnsToContents()));
    }

    QPushButton* sectionMovableBtn = new QPushButton(this);
    QListWidgetItem *item1 = new QListWidgetItem(m_rightButtonList);
    if (item)
    {
        sectionMovableBtn->setText(tr("移动列"));
        m_rightButtonList->addItem(item1);
        m_rightButtonList->setItemWidget(item1, sectionMovableBtn);

        connect(sectionMovableBtn, SIGNAL(clicked()), this, SLOT(sectionMovableBtnClicked()));
    }
    m_tableView->setAlternatingRowColors(true);

    connect(m_tableView->horizontalHeader(), SIGNAL(sectionMoved(int, int, int)), this, SLOT(OnTableViewSectionMoved(int, int, int)));
}

TabWidgetCell::~TabWidgetCell()
{
    delete ui;
}

void TabWidgetCell::OnTableViewSectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    m_bHeadIndexChange = true;
//    qDebug() << "logicalIndex = " << logicalIndex;
//    qDebug() << "oldVisualIndex = " << oldVisualIndex;
//    qDebug() << "newVisualIndex = " << newVisualIndex;

//    //当前显示的表头顺序
//    for (int i = 0; i < m_tableView->model()->columnCount(); ++i)
//    {
//        int nVisualIndex = m_tableView->horizontalHeader()->visualIndex(i);
//        qDebug() << m_tableView->model()->headerData(nVisualIndex, Qt::Horizontal).toString();
//    }
}

void TabWidgetCell::sectionMovableBtnClicked()
{
    QObject* sender = QObject::sender();
    QPushButton* btn = nullptr;
    if (sender->metaObject()->className() == QStringLiteral("QPushButton"))
    {
        btn = qobject_cast<QPushButton*>(sender);
    }

    if(m_tableView->horizontalHeader()->sectionsMovable())
    {
        m_tableView->horizontalHeader()->setSectionsMovable(false);
        if(btn)
        {
            btn->setText(tr("移动列"));
        }
    }
    else
    {
        m_tableView->horizontalHeader()->setSectionsMovable(true);
        if(btn)
        {
            btn->setText(tr("不可移动"));
        }
    }
}

void TabWidgetCell::SetDataModify(bool modify)
{
    if (m_tabWidget)
    {
        int nIndex = m_tabWidget->indexOf(this);
        if (modify)
        {
            m_tabWidget->setTabText(nIndex, m_tableData.sTableName + "*");
        }
        else
        {
            m_tabWidget->setTabText(nIndex, m_tableData.sTableName);
        }
    }
}

void TabWidgetCell::OnItemDataChange(QStandardItem *item)
{
    if (item)
    {
        m_bTableDataChange = true;
        int nRow = item->index().row();
        if (nRow <= m_vBRowDataChange.count())
        {
            m_vBRowDataChange[nRow] = MODIFY;
        }
        else
        {
            qDebug() << "修改的行的index超过了m_vBRowDataChange里面应有的行数";
        }
        SetDataModify(true);
    }
}

void TabWidgetCell::Flush()
{
    if(m_tableView && m_tableData.dataList.count() > 0)
    {
        QStandardItemModel *student_model = new QStandardItemModel();

        //设置表头
        for (int i = 0; i < m_mFieldLists.count(); ++i)
        {
            QString strField = m_mFieldLists[i];

            int visualColumn = i;
            student_model->setHorizontalHeaderItem(visualColumn, new QStandardItem(strField));
        }

        //利用setModel()方法将数据模型与QTableView绑定
        m_tableView->setModel(student_model);

        for (int i = 0; i < m_tableData.dataList.count(); ++i)
        {
            ROWDATA rowData = m_tableData.dataList[i];
            for (int j = 0; j < rowData.dataList.count(); ++j) {
                VALUEPAIR pairValue = rowData.dataList[j];
                QString strFieldName = pairValue.sField;
                QString strFieldValue = pairValue.sValue;

                auto iter = m_mFieldNames.find(strFieldName);
                int visualColumn = j;

                if(iter != m_mFieldNames.end())
                {
                    visualColumn = iter.value();
                }

                student_model->setItem(i, visualColumn, new QStandardItem(strFieldValue));
            }
        }

        m_bTableDataChange = false;
        SetDataModify(false);
        connect(student_model, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(OnItemDataChange(QStandardItem *)));
    }
}

void TabWidgetCell::SetProtoData(test_2::table_data& proto)
{
    if(m_tableView)
    {
        m_tableData.sTableName = QString::fromStdString(proto.table_name());
        m_tableData.nRow = proto.row_count();
        m_tableData.nColumn = proto.column_count();

        //TODO 二维深层次展开的时候需要用上
        for (int i = 0; i < proto.filed_sequences_size();++i)
        {
            test_2::field_squence fieldSquence = proto.filed_sequences(i);
            for (int j = 0; j < fieldSquence.levels_size();++j)
            {
                int nLevel = fieldSquence.levels(j);
            }

            for (int j = 0; j < fieldSquence.fields_size();++j)
            {
                QString strField = QString::fromStdString(fieldSquence.fields(j));
            }
        }

        //设置表头
        m_mFieldLists.clear();
        m_mFieldNames.clear();
        m_mFieldTypes.clear();
        for (int i = 0; i < proto.filed_names_size();++i)
        {
            QString strField = QString::fromStdString(proto.filed_names(i));
            m_mFieldLists.push_back(strField);

            m_mFieldNames.insert(strField, i);
            int nType = proto.filed_types(i);
            m_mFieldTypes.insert(strField, nType);
        }

        m_tableData.sTableName = QString::fromStdString(proto.table_name());
        m_tableData.nRow = proto.row_count();
        m_tableData.nColumn = proto.column_count();

        m_tableData.dataList.clear();

        for (int i = 0; i < proto.row_lists_size();++i)
        {
            m_vBRowDataChange.push_back(NONE);

            test_2::row_data row_data = proto.row_lists(i);

            ROWDATA rowData;
            rowData.id = QString::fromStdString(row_data.key()).toShort();

            for (int j = 0; j < row_data.pair_size(); ++j) {
                test_2::pair_value pair = row_data.pair(j);

                QString strFieldName = QString::fromStdString(pair.key());

                VALUEPAIR pairValue;
                pairValue.sField = strFieldName;
                pairValue.sValue = QString::fromStdString(pair.value());

                rowData.dataList.push_back(pairValue);
            }

            m_tableData.dataList.push_back(rowData);
        }
    }

    Flush();
}

void TabWidgetCell::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_S  &&  ev->modifiers() == Qt::ControlModifier)
    {
       OnRequestSaveData();
       return;
    }

    QWidget::keyPressEvent(ev);
}

void TabWidgetCell::OnRequestSaveData()
{
    qDebug() << "请求保存数据";
    //如果数据有变化
    if (m_bTableDataChange && m_mainWindow)
    {
        QAbstractItemModel* model = m_tableView->model();

        qDebug() << model->rowCount();
        qDebug() << model->columnCount();

        test_2::client_save_table_data_request quest;
        quest.set_table_name(m_tableData.sTableName.toStdString());

        for (int i = 0; i < model->rowCount(); ++i)
        {
            test_2::row_data* row_lists = quest.add_row_lists();

            for (int j = 0; j < model->columnCount(); ++j)
            {
                test_2::pair_value* pairValue = row_lists->add_pair();
                if (pairValue)
                {
                    QString strField = model->headerData(j, Qt::Horizontal).toString();
                    QString strValue = model->data(model->index(i, j)).toString();
                    if (strField == "id")
                    {
                        row_lists->set_key(strValue.toStdString());
                    }
                    pairValue->set_key(strField.toStdString());
                    pairValue->set_value(strValue.toStdString());
                }
            }
        }

        std::string output;
        quest.SerializeToString(&output);

        m_mainWindow->OnSndServerMsg(0, test_2::client_msg::REQUSET_SAVE_TABLE_DATA, output);
    }
}
