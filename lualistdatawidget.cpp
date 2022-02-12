#include "lualistdatawidget.h"

LuaListDataWidget::LuaListDataWidget(QWidget *parent) : TabWidgetCell(parent)
{

}

void LuaListDataWidget::SetProtoData(test_2::send_lua_list_data_notify& proto)
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
            m_standardItemModel->setItem(i, 0, keyItem);
            m_standardItemModel->setItem(i, 1, new QStandardItem(m_mDataList[i].sValue));
            m_standardItemModel->setItem(i, 2, new QStandardItem(QString::number(m_mDataList[i].nType)));
        }

        m_bTableDataChange = false;
        SetDataModify(false);
    }
}

void LuaListDataWidget::OnRequestSaveData()
{
    //如果数据有变化
    if (m_bTableDataChange && m_mainWindow)
    {
        qDebug() << "save data";
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
    }
}
