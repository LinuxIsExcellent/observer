#include "lualistdatawidget.h"
#include "stringtotableview.h"

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

        m_tableCellMenu->clear();
        int nRow = index.row();
        if (nRow >= 0 && nRow < m_mDataList.size())
        {
            if (m_mDataList[nRow].nType == LUA_TTABLE)
            {
                m_tableCellMenu->addAction(tr("数据展开"), this, [=](){
                    StringToTableView* dialog = new StringToTableView(m_standardItemModel, index, "", nullptr, this);
                    dialog->show();
                });
            }
        }

        m_tableCellMenu->exec(pt);
    });
}



void LuaListDataWidget::SetProtoData(const test_2::send_lua_list_data_notify& proto)
{
    m_mDataList.clear();

    for (int i = 0; i < proto.filed_types_size();++i)
    {
        FieldKeyType fkt;
        test_2::field_type_key_value fieldKeyType = proto.filed_types(i);

        fkt.sKey = QString::fromStdString(fieldKeyType.key());
        fkt.sValue = QString::fromStdString(fieldKeyType.value());
        fkt.nType = fieldKeyType.type();

        m_mDataList.push_back(fkt);
    }

    Flush();
}

void LuaListDataWidget::Flush()
{
    if(m_tableView && m_mDataList.count() > 0)
    {
        m_standardItemModel->setHorizontalHeaderItem(0, new QStandardItem("key"));
        m_standardItemModel->setHorizontalHeaderItem(1, new QStandardItem("value"));
        m_standardItemModel->setHorizontalHeaderItem(2, new QStandardItem("desc"));

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

            m_standardItemModel->setItem(i, 0, keyItem);
            m_standardItemModel->setItem(i, 1, new QStandardItem(sValue));
            m_standardItemModel->setItem(i, 2, new QStandardItem(QString::number(m_mDataList[i].nType)));
        }

        m_bTableDataChange = false;

        ChangeDataModify();
    }
}

void LuaListDataWidget::OnRequestSaveData()
{
    //如果数据有变化
    if (m_bTableDataChange && m_mainWindow)
    {
        QAbstractItemModel* model = m_tableView->model();

        test_2::save_lua_list_data_request quest;
        quest.set_table_name(m_sName.toStdString());

        for (int i = 0; i < model->rowCount(); ++i)
        {
            test_2::field_type_key_value* fieldTypeKeyValue = quest.add_filed_types();
            if (fieldTypeKeyValue)
            {
                QString strKey = model->data(model->index(i, 0)).toString();
                QString strValue = model->data(model->index(i, 1)).toString();
                quint16 nType = model->data(model->index(i, 2)).toUInt();

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
}
