#include "luatabledatawidget.h"
#include "annonationeditwidget.h"

LuaTableDataWidget::LuaTableDataWidget(QWidget *parent) : TabWidgetCell(parent)
{
    m_bHeadIndexChange = false;

    connect(m_tableView->horizontalHeader(), SIGNAL(sectionMoved(int, int, int)), this, SLOT(OnTableViewSectionMoved(int, int, int)));

    QPushButton* sectionMovableBtn = new QPushButton(this);
    QListWidgetItem *item1 = new QListWidgetItem(m_rightButtonList);
    if (item1)
    {
        sectionMovableBtn->setText(tr("移动列"));
        m_rightButtonList->addItem(item1);
        m_rightButtonList->setItemWidget(item1, sectionMovableBtn);

        connect(sectionMovableBtn, SIGNAL(clicked()), this, SLOT(sectionMovableBtnClicked()));
    }

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
}

void LuaTableDataWidget::GlobalKeyPressEevent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Escape)
    {
        m_annonationWidget->OnQuit();
    }
}

void LuaTableDataWidget::AddAnnotation()
{
    m_annonationWidget->OnShow(m_tableCellMenu->geometry().x(), m_tableCellMenu->geometry().y());
}

void LuaTableDataWidget::sectionMovableBtnClicked()
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

//调整表的字段顺序
void LuaTableDataWidget::ModifyFieldSquences(QVector<quint16>& vNLevels, QVector<QString>& vSFieldSquences)
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

void LuaTableDataWidget::OnTableViewSectionMoved(int, int, int)
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

void LuaTableDataWidget::Flush()
{
    if(m_tableView && m_tableData.dataList.count() > 0)
    {
        m_standardItemModel->clear();
        //设置表头
        for (int i = 0; i < m_mFieldLists.count(); ++i)
        {
            QString strField = m_mFieldLists[i];
            QStandardItem* fieldKeyItem = new QStandardItem(strField);
            fieldKeyItem->setToolTip(strField);
            m_standardItemModel->setHorizontalHeaderItem(i, fieldKeyItem);

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
            m_standardItemModel->setItem(0, i, item);
        }

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
                m_standardItemModel->setItem(i + 1, visualColumn, dataItem);
            }
        }

        m_bTableDataChange = false;
        m_bHeadIndexChange = false;
        SetDataModify(false);
    }
}

void LuaTableDataWidget::SetProtoData(test_2::table_data& proto)
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
//            m_vBRowDataChange.push_back(NONE);

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

void LuaTableDataWidget::OnRequestSaveData()
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
