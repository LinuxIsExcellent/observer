#include "tabwidgetcell.h"
#include "ui_tabwidgetcell.h"
#include <QTableWidget>

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
    if (item1)
    {
        sectionMovableBtn->setText(tr("移动列"));
        m_rightButtonList->addItem(item1);
        m_rightButtonList->setItemWidget(item1, sectionMovableBtn);

        connect(sectionMovableBtn, SIGNAL(clicked()), this, SLOT(sectionMovableBtnClicked()));
    }
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tableView->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);

    //初始化菜单栏
    m_tableCellMenu = new QMenu(this);

    //增加数据单元格的菜单
    connect(m_tableView, &QAbstractItemView::customContextMenuRequested, m_tableView,[=](const QPoint& pos){
        int nHeight = m_tableView->horizontalHeader()->height();
        int nWidth = m_tableView->verticalHeader()->width();
        //mapToGlobal获取m_tableView全局坐标
        //m_tableView->pos()获取m_tableView在父窗口中的相对坐标
        //pos鼠标点击时在表格中的相对位置
        QPoint pt = m_tableView->parentWidget()->mapToGlobal(m_tableView->pos()) + pos + QPoint(nWidth, nHeight);
        //判断鼠标右击位置是否是空白处，空白处则取消上一个选中焦点，不弹出菜单
        QModelIndex index = m_tableView->indexAt(pos);
        if (!index.isValid()){
            //m_tableView->clearSelection();
            return;
        }

        m_tableCellMenu->clear();
        m_tableCellMenu->addAction(index.data().toString(), this, SLOT(slot_function1()));

        m_tableCellMenu->exec(pt);
    });

    //增加列表头的菜单
    connect(m_tableView->horizontalHeader(), &QAbstractItemView::customContextMenuRequested, m_tableView->horizontalHeader(),[=](const QPoint& pos){
        //mapToGlobal获取m_tableView全局坐标
        //m_tableView->pos()获取m_tableView在父窗口中的相对坐标
        //pos鼠标点击时在表格中的相对位置
        QPoint pt = m_tableView->parentWidget()->mapToGlobal(m_tableView->pos()) + pos;
        //判断鼠标右击位置是否是空白处，空白处则取消上一个选中焦点，不弹出菜单
        int nIndex = m_tableView->horizontalHeader()->logicalIndexAt(pos);
        qDebug() << "index = " << nIndex;
        if (nIndex < 0){
            //m_tableView->clearSelection();
            return;
        }

        m_tableCellMenu->clear();
        m_tableCellMenu->addAction("编辑批注", this, SLOT(AddAnnotation()));
        m_tableCellMenu->addAction("增加关联", this, SLOT(slot_function1()));

        m_tableCellMenu->exec(pt);
    });

    //增加行表头的菜单
    connect(m_tableView->verticalHeader(), &QAbstractItemView::customContextMenuRequested, m_tableView->verticalHeader(),[=](const QPoint& pos){
        //mapToGlobal获取m_tableView全局坐标
        //m_tableView->pos()获取m_tableView在父窗口中的相对坐标
        //pos鼠标点击时在表格中的相对位置
        QPoint pt = m_tableView->parentWidget()->mapToGlobal(m_tableView->pos()) + pos;
        //判断鼠标右击位置是否是空白处，空白处则取消上一个选中焦点，不弹出菜单
        int nIndex = m_tableView->verticalHeader()->logicalIndexAt(pos);
        qDebug() << "index = " << nIndex;
        if (nIndex < 0){
            //m_tableView->clearSelection();
            return;
        }

        m_tableCellMenu->clear();
        m_tableCellMenu->addAction("插入行", this, [=](){
            qDebug() << "插入一行 :" << nIndex;
        });

        m_tableCellMenu->addAction("增加行", this, [=](){
            qDebug() << "增加一行 :" << nIndex;
        });

        m_tableCellMenu->exec(pt);
    });

    connect(m_tableView->horizontalHeader(), SIGNAL(sectionMoved(int, int, int)), this, SLOT(OnTableViewSectionMoved(int, int, int)));
}

TabWidgetCell::~TabWidgetCell()
{
    delete ui;
}

void TabWidgetCell::AddAnnotation()
{
    qDebug() << "add annotation";
}

//调整表的字段顺序
void TabWidgetCell::ModifyFieldSquences(QVector<quint16>& vNLevels, QVector<QString>& vSFieldSquences)
{
    bool isHas = false;
    for (auto& data : m_vFieldSquence)
    {
        if (data.vNLevels == vNLevels)
        {
            data.vSFieldSquences = vSFieldSquences;

            isHas = true;
            break;
        }
    }

    if (!isHas)
    {
        FIELDSQUENCE fieldSquence;
        fieldSquence.vNLevels = vNLevels;
        fieldSquence.vSFieldSquences = vSFieldSquences;

        m_vFieldSquence.push_back(fieldSquence);
    }
}

void TabWidgetCell::OnTableViewSectionMoved(int, int, int)
{
    m_bHeadIndexChange = true;
    SetDataModify(true);

    QVector<QString> sFields;

    //按照当前的表格显示顺序排序
    sFields.resize(m_tableView->model()->columnCount());
    for (int i = 0; i < m_tableView->model()->columnCount(); ++i)
    {
        int nVisualIndex = m_tableView->horizontalHeader()->visualIndex(i);

        sFields[nVisualIndex] = m_tableView->model()->headerData(i, Qt::Horizontal).toString();
    }

    QVector<quint16> vNLevels;

    ModifyFieldSquences(vNLevels, sFields);
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
//            m_vBRowDataChange[nRow] = MODIFY;
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
        QStandardItemModel *student_model = nullptr;
        if (m_tableView->model())
        {
            student_model = qobject_cast<QStandardItemModel*>(m_tableView->model());
        }
        else
        {
            student_model = new QStandardItemModel();
        }

        //设置表头
        for (int i = 0; i < m_mFieldLists.count(); ++i)
        {
            QString strField = m_mFieldLists[i];
            student_model->setHorizontalHeaderItem(i, new QStandardItem(strField));

            QString dataTypeStr;
            switch (m_mFieldTypes.find(strField).value()) {
                case LUA_TTABLE:
                {
                    dataTypeStr = "表";
                    break;
                }
                case LUA_TNIL:
                case LUA_TNUMBER:
                {
                    dataTypeStr = "数字";
                    break;
                }
                case LUA_TSTRING:
                {
                    dataTypeStr = "字符串";
                    break;
                }
                case LUA_TBOOLEAN:
                {
                    dataTypeStr = "0为否，1为是";
                    break;
                }
            }

            QStandardItem* item = new QStandardItem(dataTypeStr);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            student_model->setItem(0, i, item);
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

                if(m_mFieldTypes.find(strFieldName).value() == LUA_TSTRING)
                {
                    strFieldValue = strFieldValue.replace('\n',"\\n");
                    strFieldValue = strFieldValue.replace('\"', "\\\"");
                }
                QStandardItem* dataItem = new QStandardItem(strFieldValue);
                student_model->setItem(i + 1, visualColumn, dataItem);
            }
        }

        m_tableView->update();
        m_bTableDataChange = false;
        m_bHeadIndexChange = false;
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

        m_vFieldSquence.clear();
        //TODO 二维深层次展开的时候需要用上
        for (int i = 0; i < proto.filed_sequences_size();++i)
        {
            test_2::field_squence fieldSquence = proto.filed_sequences(i);

            FIELDSQUENCE squence;
            for (int j = 0; j < fieldSquence.levels_size();++j)
            {
                int nLevel = fieldSquence.levels(j);

                squence.vNLevels.push_back(nLevel);
            }


            for (int j = 0; j < fieldSquence.fields_size();++j)
            {
                QString strField = QString::fromStdString(fieldSquence.fields(j));

                squence.vSFieldSquences.push_back(strField);
            }

            m_vFieldSquence.push_back(squence);
        }

//        qDebug() << "深度队列： " << m_vFieldSquence;

        //设置表头
        m_mFieldLists.clear();
        m_mFieldNames.clear();
        m_mFieldTypes.clear();
        for (int i = 0; i < proto.filed_names_size();++i)
        {
            QString strField = QString::fromStdString(proto.filed_names(i));
            m_mFieldLists.push_back(strField);

            m_mFieldNames.insert(strField, i);
            test_2::field_type_pair type_pairs = proto.filed_types(i);
            m_mFieldTypes.insert(strField, type_pairs.lua_type());
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
    //如果表的信息有变化
    if (m_bHeadIndexChange)
    {
        //请求保存表的信息
        test_2::client_save_table_info_request quest;
        quest.set_table_name(m_tableData.sTableName.toStdString());

        for (auto data : m_vFieldSquence)
        {
            test_2::field_squence* field_squence = quest.add_filed_sequences();
            if (field_squence)
            {
                for (auto nLevel : data.vNLevels)
                {
                    field_squence->add_levels(nLevel);
                }

                for (auto sData : data.vSFieldSquences)
                {
                    std::string* sField = field_squence->add_fields();
                    if (sField)
                    {
                        *sField = sData.toStdString();
                    }
                }
            }
        }

        std::string output;
        quest.SerializeToString(&output);

        m_mainWindow->OnSndServerMsg(0, test_2::client_msg::REQUEST_SAVE_TABLE_INFO, output);
    }
    //如果数据有变化
    if (m_bTableDataChange && m_mainWindow)
    {
        QAbstractItemModel* model = m_tableView->model();

        test_2::client_save_table_data_request quest;
        quest.set_table_name(m_tableData.sTableName.toStdString());

        for (int i = 1; i < model->rowCount(); ++i)
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

                    if(m_mFieldTypes.find(strField).value() == LUA_TSTRING)
                    {
                        strValue = strValue.replace("\\n", "\n");
                        strValue = strValue.replace("\\\"", "\"");
                    }

                    pairValue->set_key(strField.toStdString());
                    pairValue->set_value(strValue.toStdString());

                    qDebug() << strField << " = " << strValue;
                }
            }
        }

        std::string output;
        quest.SerializeToString(&output);

        m_mainWindow->OnSndServerMsg(0, test_2::client_msg::REQUSET_SAVE_TABLE_DATA, output);
    }
}
