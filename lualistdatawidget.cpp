#include "lualistdatawidget.h"
#include "tabledelegate.h"

LuaListDataWidget::LuaListDataWidget(QWidget *parent) : TabWidgetCell(parent)
{
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

        if (index.column() != 2)
        {
            return;
        }

        m_tableCellMenu->clear();
        int nRow = index.row();
        if (nRow >= 0 && nRow < m_mDataList.size())
        {
            if (m_mDataList[nRow].nType == LUA_TTABLE)
            {
                m_tableCellMenu->addAction(tr("数据展开"), this, [=](){
                    StringToTableView* dialog = new StringToTableView(m_standardItemModel, index, m_mDataList[nRow].sKey, &m_mFieldSquence, this, this);
                    dialog->show();
                });
            }
        }

        m_tableCellMenu->exec(pt);
    });
}

LuaListDataWidget::~LuaListDataWidget()
{
}

void LuaListDataWidget::OnShowTableWithLinkMsg(QString sField, QString)
{
    for (int row = 0; row < m_standardItemModel->rowCount(); ++row)
    {
        QStandardItem* item = m_standardItemModel->item(row, 1);
        if (item)
        {
            if (item->index().isValid())
            {
                if (item->index().data().toString() == sField)
                {
                    QScrollBar *vScrollbar = m_tableView->verticalScrollBar();
                    if (vScrollbar)
                    {
                        vScrollbar->setSliderPosition(row);
                    }

                    m_tableView->setCurrentIndex(item->index());
                    return;
                }
            }
        }
    }
}

void LuaListDataWidget::SetProtoData(const test_2::send_lua_list_data_notify& proto)
{
    m_mDataList.clear();

    QString sLinkInfo = QString::fromStdString(proto.link_info());
    for (int i = 0; i < proto.filed_types_size();++i)
    {
        FieldKeyType fkt;
        test_2::field_type_key_value fieldKeyType = proto.filed_types(i);

        fkt.sKey = QString::fromStdString(fieldKeyType.key());
        fkt.sValue = QString::fromStdString(fieldKeyType.value());
        fkt.nType = fieldKeyType.type();

        m_mDataList.push_back(fkt);
    }

    for (int i = 0; i < proto.filed_sequences_size();++i)
    {
        test_2::field_squence fieldSquence = proto.filed_sequences(i);

        QString sIndex = QString::fromStdString(fieldSquence.index());

        FIELDSQUENCE squence;
        for (int j = 0; j < fieldSquence.infos_size();++j)
        {
            FIELDINFO fieldInfo;

            fieldInfo.sFieldName = QString::fromStdString(fieldSquence.infos(j).field_name());
            fieldInfo.sFieldAnnonation = QString::fromStdString(fieldSquence.infos(j).field_desc());
            fieldInfo.sFieldLink = QString::fromStdString(fieldSquence.infos(j).field_link());

            squence.vSFieldSquences.push_back(fieldInfo);
        }

        m_mFieldSquence.insert(sIndex, squence);
    }

    disconnect(m_standardItemModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(OnItemDataChange(QStandardItem *)));
    Flush();

    if (sLinkInfo != "")
    {
        QStringList stringList = sLinkInfo.split("#");
        if (stringList.size() == 4)
        {
            QString sFieldName = stringList[2];
            QString sField = stringList[3];
            OnShowTableWithLinkMsg(sFieldName, sField);
        }
    }
    connect(m_standardItemModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(OnItemDataChange(QStandardItem *)));
}

void LuaListDataWidget::Flush()
{
    if(m_tableView && m_mDataList.count() > 0)
    {
        m_standardItemModel->setHorizontalHeaderItem(0, new QStandardItem(tr("备注")));
        m_standardItemModel->setHorizontalHeaderItem(1, new QStandardItem(tr("字段")));
        m_standardItemModel->setHorizontalHeaderItem(2, new QStandardItem(tr("值")));
        m_standardItemModel->setHorizontalHeaderItem(3, new QStandardItem(tr("类型")));

        FIELDSQUENCE* squence = nullptr;

        if(m_mFieldSquence.find("###field_sequence###") != m_mFieldSquence.end())
        {
            squence = &m_mFieldSquence.find("###field_sequence###").value();
        }


        for (int i = 0; i < m_mDataList.size(); ++i)
        {
            QStandardItem* keyItem = new QStandardItem(m_mDataList[i].sKey);
            keyItem->setFlags(Qt::ItemIsEnabled);

            QString sValue = m_mDataList[i].sValue;

            if(m_mDataList[i].nType == LUA_TSTRING)
            {
                sValue = sValue.replace('\"', "\\\"");
            }

            sValue = sValue.replace('\n',"\\n");

            QString sAnnonation = "";
            if (squence && i < squence->vSFieldSquences.size())
            {
                sAnnonation = squence->vSFieldSquences[i].sFieldAnnonation;
            }

            m_standardItemModel->setItem(i, 0, new QStandardItem(sAnnonation));
            m_standardItemModel->setItem(i, 1, keyItem);
            m_standardItemModel->setItem(i, 2, new QStandardItem(sValue));
            m_standardItemModel->setItem(i, 3, new QStandardItem(QString::number(m_mDataList[i].nType)));
        }

        m_tableView->setColumnHidden(3, true);
        SetRowAndColParam();

        m_bTableDataChange = false;

        ChangeDataModify();

    }
}

void LuaListDataWidget::SetFieldLink(QString sIndex, QString sField, QString sFieldLink)
{
    if (m_mFieldSquence.find(sIndex) != m_mFieldSquence.end())
    {
        for (auto & field : m_mFieldSquence[sIndex].vSFieldSquences)
        {
            if (field.sFieldName == sField)
            {
                field.sFieldLink = sFieldLink;
                break;
            }
        }
    }

    m_bTableDataChange = true;
    TabWidgetCell::ChangeDataModify();
}

void LuaListDataWidget::OnItemDataChange(QStandardItem *item)
{
    TabWidgetCell::OnItemDataChange(item);

    bool isChange = false;
    if (item && item->index().isValid())
    {
        int nCol = item->index().column();
        int nRow = item->index().row();
        if(nCol == 0)
        {
            bool isHas = false;

            QString sFieldName = "";
            QStandardItem *itemField = m_standardItemModel->item(nRow, 1);
            if (itemField && itemField->index().isValid())
            {
                sFieldName = itemField->index().data().toString();
            }
            if (m_mFieldSquence.size() > 0 && m_mFieldSquence.find("###field_sequence###") != m_mFieldSquence.end())
            {
                FIELDSQUENCE& squence = m_mFieldSquence.find("###field_sequence###").value();
                for (auto& data : squence.vSFieldSquences)
                {
                    if (data.sFieldName == sFieldName)
                    {
                        isHas = true;
                        if (data.sFieldAnnonation != item->index().data().toString())
                        {
                            data.sFieldAnnonation = item->index().data().toString();
                            isChange = true;
                        }
                        break;
                    }
                }
            }

            if (isHas == false)
            {
                FIELDSQUENCE squence;
                squence.sIndex = "###field_sequence###";

                for (int row = 0; row < m_standardItemModel->rowCount(); ++row)
                {
                    QStandardItem *itemAnnonation = m_standardItemModel->item(row, 0);
                    QStandardItem *itemField = m_standardItemModel->item(row, 1);

                    QString sAnnonation = "";
                    if (itemAnnonation && itemAnnonation->index().isValid())
                    {
                        sAnnonation = itemAnnonation->index().data().toString();
                    }

                    QString sField = "";
                    if (itemField && itemField->index().isValid())
                    {
                        sField = itemField->index().data().toString();
                    }

                    FIELDINFO info;
                    info.sFieldName = sField;
                    info.sFieldAnnonation = sAnnonation;

                    squence.vSFieldSquences.push_back(info);
                }

                m_mFieldSquence.insert("###field_sequence###", squence);
                isChange = true;
            }
        }
    }

    if (isChange)
    {
        m_bTableDataChange = true;
        ChangeDataModify();
    }
}

bool LuaListDataWidget::OnRequestSaveData()
{
    if (!TabWidgetCell::OnRequestSaveData()) return false;
    //如果数据有变化
    if (m_bTableDataChange && m_mainWindow)
    {
        //请求保存表的外围信息
        {
            //保存表的行和列
            if (m_tableView && m_standardItemModel && m_standardItemModel->rowCount() > 0)
            {
                QString rowInfoKey = "###row_height###";
                FIELDSQUENCE rowFieldSquence;
                rowFieldSquence.sIndex = rowInfoKey;
                for (int row = 0; row < m_standardItemModel->rowCount();++row)
                {
                    FIELDINFO fieldInfo;
                    fieldInfo.sFieldName = QString::number(m_tableView->rowHeight(row));

                    rowFieldSquence.vSFieldSquences.push_back(fieldInfo);
                }

                m_mFieldSquence.remove(rowInfoKey);
                m_mFieldSquence.insert(rowInfoKey, rowFieldSquence);


                QString colInfoKey = "###col_width###";
                FIELDSQUENCE colFieldSquence;
                colFieldSquence.sIndex = colInfoKey;
                for (int col = 0; col < m_standardItemModel->columnCount();++col)
                {
                    FIELDINFO fieldInfo;
                    fieldInfo.sFieldName = QString::number(m_tableView->columnWidth(col));

                    colFieldSquence.vSFieldSquences.push_back(fieldInfo);
                }

                m_mFieldSquence.remove(colInfoKey);
                m_mFieldSquence.insert(colInfoKey, colFieldSquence);
            }

            if (true)
            {
                test_2::client_save_table_info_request quest;
                quest.set_table_name(m_sName.toStdString());

                for (auto iter = m_mFieldSquence.begin(); iter != m_mFieldSquence.end();++ iter)
                {
                    test_2::field_squence* field_squence = quest.add_field_squences();
                    if (field_squence)
                    {
                        field_squence->set_index(iter.key().toStdString());
                        for (auto sData : iter.value().vSFieldSquences)
                        {
                            test_2::field_info* fieldInfo = field_squence->add_infos();
                            if (fieldInfo)
                            {
                                fieldInfo->set_field_name(sData.sFieldName.toStdString());
                                fieldInfo->set_field_desc(sData.sFieldAnnonation.toStdString());
                                fieldInfo->set_field_link(sData.sFieldLink.toStdString());
                            }
                        }
                    }
                }

                std::string output;
                quest.SerializeToString(&output);

                m_mainWindow->OnSndServerMsg(0, test_2::client_msg::REQUEST_SAVE_TABLE_INFO, output);
            }
        }

        QAbstractItemModel* model = m_tableView->model();

        test_2::save_lua_list_data_request quest;
        quest.set_table_name(m_sName.toStdString());

        for (int i = 0; i < model->rowCount(); ++i)
        {
            test_2::field_type_key_value* fieldTypeKeyValue = quest.add_filed_types();
            if (fieldTypeKeyValue)
            {
                QString strKey = model->data(model->index(i, 1)).toString();
                QString strValue = model->data(model->index(i, 2)).toString();
                quint16 nType = model->data(model->index(i, 3)).toUInt();

                fieldTypeKeyValue->set_key(strKey.toStdString());
                fieldTypeKeyValue->set_value(strValue.toStdString());
                fieldTypeKeyValue->set_type(nType);
            }
        }

        std::string output;
        quest.SerializeToString(&output);

        m_mainWindow->OnSndServerMsg(0, test_2::client_msg::REQUSET_SAVE_LUA_LIST_DATA, output);

        //保存之后清空undo栈
        clearUndoStack();
    }

    return true;
}

void LuaListDataWidget::CheckItemDataTypeIsCorrect(QStandardItem *item)
{
    if(item)
    {
        int nRow = item->row();
        int nCol = item->column();

        int nIndex = nRow * 10000 + nCol;
        if (nRow < m_mDataList.size())
        {
            if (!GlobalConfig::getInstance()->CheckStrIsCorrectType(item->index().data().toString(), m_mDataList[nRow].nType))
            {
                item->setForeground(Qt::red);
                m_mTypeCheck[nIndex] = true;
            }
            else
            {
                if(m_mTypeCheck.find(nIndex) != m_mTypeCheck.end())
                {
                    m_mTypeCheck[nIndex] = false;
                    item->setForeground(Qt::black);
                }
            }
        }
    }
}
