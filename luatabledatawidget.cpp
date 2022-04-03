#include "luatabledatawidget.h"
#include "annonationeditwidget.h"
#include "stringtotableview.h"
#include "tabledelegate.h"
#include "selectrowdatadialog.h"
#include <QComboBox>
#include <regex>

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
        quint32 nIndex = m_tableView->horizontalHeader()->logicalIndexAt(pos);
        if (nIndex < 0){
            //m_tableView->clearSelection();
            return;
        }

        QString sField = m_standardItemModel->horizontalHeaderItem(nIndex)->text();
        m_tableCellMenu->clear();
        m_tableCellMenu->addAction("编辑批注", this, [=](){
//            m_annonationWidget->OnShow(pt.x(), pt.y());

            QString sToolTips = m_standardItemModel->horizontalHeaderItem(nIndex)->toolTip();
            m_annonationWidget->OnShow(pos.x(), pos.y(), sField, sToolTips);
        });


        m_tableCellMenu->addAction("增加关联", this, [=](){
            m_mainWindow->OnOpenAddLinkFieldDialog("###field_sequence###", this, sField, true);
        });

        m_tableCellMenu->addAction("筛选", this, [=](){
            SelectRowDataDialog* dialog = new SelectRowDataDialog(nIndex, m_tableView);
            dialog->show();
            dialog->move(pt);
        });

        m_tableCellMenu->exec(pt);
    });

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
        QString sField = m_standardItemModel->horizontalHeaderItem(index.column())->text();
        if (m_mFieldTypes.find(sField) != m_mFieldTypes.end() && m_mFieldTypes.find(sField).value() == LUA_TTABLE)
        {
            m_tableCellMenu->addAction(tr("数据展开"), this, [=](){
                StringToTableView* dialog = new StringToTableView(m_standardItemModel, index, sField, &m_mFieldSquence, this);
                dialog->show();
            });
        }

//        //如果有对应的关联
//        //弹出跳转到关联的菜单
//        if (index.data(Qt::UserRole+2) == QVariant(DelegateModel::EditAndCombox) && index.data(Qt::UserRole+3).toString() != "")
//        {
//            m_tableCellMenu->addAction(tr("跳转到关联的表"), this, [=](){
//                m_mainWindow->OnJumpLinkTable(index.data(Qt::UserRole+3).toString(), index.data().toString());
//            });
//        }

        m_tableCellMenu->exec(pt);
    });
}

void LuaTableDataWidget::SetFieldLink(QString sIndex, QString sField, QString sFieldLink)
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

    //修改对应列的关联信息
    for (int i = 0; i < m_tableView->model()->columnCount(); ++i)
    {
        int nVisualIndex = m_tableView->horizontalHeader()->visualIndex(i);
        if (m_tableView->model()->headerData(i, Qt::Horizontal).toString() == sField)
        {
            for (int row = 1; row < m_standardItemModel->rowCount(); ++row)
            {
                QStandardItem* dataItem = m_standardItemModel->item(1, i);
                if (dataItem)
                {
                    dataItem->setData(QVariant(DelegateModel::EditAndCombox), Qt::UserRole+2);
                    dataItem->setData(QVariant(sFieldLink), Qt::UserRole+3);
                }
            }
        }
    }

    m_bHeadIndexChange = true;
    ChangeDataModify();
}

void LuaTableDataWidget::InsertSquenceInfo(QString sIndex, QVector<FIELDINFO> vFieldInfos)
{
    FIELDSQUENCE fieldSquence;
    fieldSquence.sIndex = sIndex;
    fieldSquence.vSFieldSquences = vFieldInfos;

    m_mFieldSquence.insert(sIndex, fieldSquence);
}

void LuaTableDataWidget::OnItemDataChange(QStandardItem *item)
{
    TabWidgetCell::OnItemDataChange(item);
}

void LuaTableDataWidget::OnSaveAnnonations(QString sIndex, QString str, QString sField)
{
    //把备注设置到容器里面
    bool isHas = false;
    if (m_mFieldSquence.find(sIndex) != m_mFieldSquence.end())
    {
        for (auto & field : m_mFieldSquence[sIndex].vSFieldSquences)
        {
            isHas = true;
            if (field.sFieldName == sField)
            {
                field.sFieldAnnonation = str;
                break;
            }
        }
    }

    if (!isHas)
    {
        QMap<QString, quint16> mFieldSortMap;
        QVector<FIELDINFO> vFieldInfos;
        vFieldInfos.resize(m_tableView->model()->columnCount());

        for (int i = 0; i < m_tableView->model()->columnCount(); ++i)
        {
            int nVisualIndex = m_tableView->horizontalHeader()->visualIndex(i);
            mFieldSortMap.insert(m_tableView->model()->headerData(i, Qt::Horizontal).toString(), nVisualIndex);

            FIELDINFO fieldInfo;
            fieldInfo.sFieldName = m_tableView->model()->headerData(i, Qt::Horizontal).toString();
            if (fieldInfo.sFieldName == sField)
            {
                fieldInfo.sFieldAnnonation = str;
            }

            vFieldInfos[nVisualIndex] = fieldInfo;
        }

        InsertSquenceInfo(sIndex, vFieldInfos);
    }

    m_bHeadIndexChange = true;
    ChangeDataModify();

    //修改表头的备注
    for (int i = 0; i < m_standardItemModel->columnCount(); i++)
    {
        if (m_standardItemModel->horizontalHeaderItem(i)->text() == sField)
        {
            m_standardItemModel->horizontalHeaderItem(i)->setToolTip(str);
        }
    }
}

void LuaTableDataWidget::GlobalKeyPressEevent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Escape)
    {
        m_annonationWidget->OnQuit();
    }
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
void LuaTableDataWidget::ModifyFieldSquences(QString sIndex, QMap<QString, quint16> mFieldSortMap)
{
    bool isHas = false;
    if (m_mFieldSquence.find(sIndex) != m_mFieldSquence.end())
    {
        FIELDSQUENCE& fieldSquence = m_mFieldSquence.find(sIndex).value();

        qSort(fieldSquence.vSFieldSquences.begin(), fieldSquence.vSFieldSquences.end(),
              [=](const FIELDINFO& a, const FIELDINFO& b)
                  {
                      quint16 aSort = mFieldSortMap.find(a.sFieldName).value();
                      quint16 bSort = mFieldSortMap.find(b.sFieldName).value();
                      return aSort < bSort;
                  }
              );

        isHas = true;
    }

    if (!isHas)
    {
        QVector<FIELDINFO> vFieldInfos;
        vFieldInfos.resize(mFieldSortMap.size());

        for (QMap<QString, quint16>::iterator iter = mFieldSortMap.begin(); iter != mFieldSortMap.end(); ++iter)
        {
            FIELDINFO info;
            info.sFieldName = iter.key();

            vFieldInfos[iter.value()] = info;
        }

        InsertSquenceInfo("###field_sequence###", vFieldInfos);
    }
}

void LuaTableDataWidget::OnTableViewSectionMoved(int, int, int)
{
    m_bHeadIndexChange = true;
    ChangeDataModify();

    QVector<QString> sFields;

    //按照当前的表格显示顺序排序
    sFields.resize(m_tableView->model()->columnCount());
    QMap<QString, quint16> mFieldSortMap;
    for (int i = 0; i < m_tableView->model()->columnCount(); ++i)
    {
        int nVisualIndex = m_tableView->horizontalHeader()->visualIndex(i);
        m_tableView->model()->headerData(i, Qt::Horizontal).toString();
        mFieldSortMap.insert(m_tableView->model()->headerData(i, Qt::Horizontal).toString(), nVisualIndex);
    }

    ModifyFieldSquences("###field_sequence###", mFieldSortMap);
}

FIELDINFO* LuaTableDataWidget::GetFieldInfos(QString sIndex, quint16 nIndex)
{
    FIELDINFO* pInfo = nullptr;
    if (m_mFieldSquence.find(sIndex) != m_mFieldSquence.end())
    {
        pInfo = &m_mFieldSquence.find(sIndex).value().vSFieldSquences[nIndex];
    }

    return pInfo;
}

QVector<FIELDINFO>* LuaTableDataWidget::GetFieldInfos(QString sIndex)
{
    QVector<FIELDINFO>* vPInfo = nullptr;
    if (m_mFieldSquence.find(sIndex) != m_mFieldSquence.end())
    {
        vPInfo = &m_mFieldSquence.find(sIndex).value().vSFieldSquences;
    }

    return vPInfo;
}

void LuaTableDataWidget::Flush()
{
    if(m_tableView && m_tableData.dataList.count() > 0)
    {
        m_standardItemModel->clear();

        QVector<quint16> vNLevels;
        QVector<FIELDINFO>* vSFieldSquences = GetFieldInfos("###field_sequence###");

        //设置表头
        for (int i = 0; i < m_mFieldLists.count(); ++i)
        {
            QString strField = m_mFieldLists[i];
            QStandardItem* fieldKeyItem = new QStandardItem(strField);

            if (i >= 0 && vSFieldSquences && i < vSFieldSquences->size())
            {
                fieldKeyItem->setToolTip((*vSFieldSquences)[i].sFieldAnnonation);
            }

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
            item->setFlags(Qt::ItemIsEnabled);
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
                    strFieldValue = strFieldValue.replace('\"', "\\\"");
                }

                strFieldValue = strFieldValue.replace('\n',"\\n");

                QStandardItem* dataItem = new QStandardItem(strFieldValue);
                if (vSFieldSquences)
                {
                    for (int i = 0;i < (*vSFieldSquences).size(); ++i)
                    {
                        if ((*vSFieldSquences)[i].sFieldName == strFieldName)
                        {
                            if ((*vSFieldSquences)[i].sFieldLink != "")
                            {
                                dataItem->setData(QVariant(DelegateModel::EditAndCombox), Qt::UserRole+2);
                                dataItem->setData(QVariant((*vSFieldSquences)[i].sFieldLink), Qt::UserRole+3);
                            }
                        }
                    }
                }

                m_standardItemModel->setItem(i + 1, visualColumn, dataItem);
            }
        }

        SetRowAndColParam();
    }
}

void LuaTableDataWidget::SetProtoData(const test_2::table_data& proto)
{
    if(m_tableView)
    {
        m_tableData.sTableName = QString::fromStdString(proto.table_name());
        m_tableData.nRow = proto.row_count();
        m_tableData.nColumn = proto.column_count();

        m_mFieldSquence.clear();
        //TODO 二维深层次展开的时候需要用上
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

    m_bTableDataChange = false;
    m_bHeadIndexChange = false;
    ChangeDataModify();
}

void LuaTableDataWidget::OnRequestSaveData()
{
    qDebug() << "请求保存数据";
    //如果表的信息有变化
    if (m_bHeadIndexChange || m_bTableDataChange)
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
            for (int col = 0; col < m_standardItemModel->rowCount();++col)
            {
                FIELDINFO fieldInfo;
                fieldInfo.sFieldName = QString::number(m_tableView->columnWidth(col));

                colFieldSquence.vSFieldSquences.push_back(fieldInfo);
            }

            m_mFieldSquence.remove(colInfoKey);
            m_mFieldSquence.insert(colInfoKey, colFieldSquence);
        }

        //请求保存表的信息
        test_2::client_save_table_info_request quest;
        quest.set_table_name(m_tableData.sTableName.toStdString());

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
                }
            }
        }

        std::string output;
        quest.SerializeToString(&output);

        m_mainWindow->OnSndServerMsg(0, test_2::client_msg::REQUSET_SAVE_TABLE_DATA, output);
    }

    //保存之后清空undo栈
    clearUndoStack();
}

void LuaTableDataWidget::SetRowAndColParam()
{
    disconnect(m_tableView->horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this, SLOT(OnColResized(int, int, int)));
    disconnect(m_tableView->verticalHeader(), SIGNAL(sectionResized(int, int, int)), this, SLOT(OnRowResized(int, int, int)));
    if (m_tableView && m_standardItemModel && m_standardItemModel->rowCount() > 0)
    {
        if (m_mFieldSquence.find("###row_height###") != m_mFieldSquence.end())
        {
            FIELDSQUENCE fieldSquence = m_mFieldSquence.find("###row_height###").value();

            for (int row = 0; row < m_standardItemModel->rowCount();++row)
            {
                if (row < fieldSquence.vSFieldSquences.size())
                {
                    int nHeight = fieldSquence.vSFieldSquences[row].sFieldName.toInt();
                    if (nHeight > 0)
                    {
                        m_tableView->setRowHeight(row, nHeight);
                    }
                }
            }
        }

        if (m_mFieldSquence.find("###col_width###") != m_mFieldSquence.end())
        {
            FIELDSQUENCE fieldSquence = m_mFieldSquence.find("###col_width###").value();

            for (int col = 0; col < m_standardItemModel->rowCount();++col)
            {
                if (col < fieldSquence.vSFieldSquences.size())
                {
                    int nWidth = fieldSquence.vSFieldSquences[col].sFieldName.toInt();
                    if (nWidth > 0)
                    {

                        m_tableView->setColumnWidth(col, nWidth);
                    }
                }
            }
        }
    }

    connect(m_tableView->horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this, SLOT(OnColResized(int, int, int)));
    connect(m_tableView->verticalHeader(), SIGNAL(sectionResized(int, int, int)), this, SLOT(OnRowResized(int, int, int)));
}
