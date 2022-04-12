#include "stringtotableview.h"
#include "globalconfig.h"
#include "ui_stringtotableview.h"
#include <QMessageBox>
#include <QMimeData>
#include <QClipboard>
#include <QKeyEvent>

#include "tabwidgetcell.h"
#include "tabledelegate.h"
#include "modifcommand.h"

StringToTableView::StringToTableView(QStandardItemModel *model, QModelIndex index, QString sTableName, QMap<QString, FIELDSQUENCE>* pMFieldSquence, TabWidgetCell* cellWidget, QWidget *parent, int nLevel) :
    QDialog(parent),
    ui(new Ui::StringToTableView),
    m_sTableName(sTableName),
    m_nLevel(nLevel)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Window);

    this->model = model;
    this->m_index = index;
    this->m_mFieldSquence = pMFieldSquence;
    this->m_cellWidget = cellWidget;

    m_sData = index.data().toString();

    setWindowTitle("表");
//    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
//    setWindowFlag(Qt::WindowTitleHint, true);
//    setModal(true);

    m_standardItemModel = new QStandardItemModel(ui->tableView);
    ui->tableView->setModel(m_standardItemModel);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);

//    ui->tableView->verticalHeader()->hide();

    connect(m_standardItemModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(OnItemDataChange(QStandardItem *)));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(OnCancelButtonClicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(OnConfirmButtonClicked()));

    //实现交换两行的效果
    ui->tableView->viewport()->installEventFilter(this);
    ui->tableView->setDragDropMode(QAbstractItemView::DragDrop);
    ui->tableView->setDragEnabled(true);
    ui->tableView->setAcceptDrops(true);

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableView->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;color: black;}");

    //初始化菜单栏
    m_tableCellMenu = new QMenu(this);

    connect(ui->tableView->verticalHeader(), &QAbstractItemView::customContextMenuRequested, ui->tableView->verticalHeader(),[=](const QPoint& pos){
        //mapToGlobal获取m_tableView全局坐标
        //m_tableView->pos()获取m_tableView在父窗口中的相对坐标
        //pos鼠标点击时在表格中的相对位置
        QPoint pt = ui->tableView->parentWidget()->mapToGlobal(ui->tableView->pos()) + pos;
        //判断鼠标右击位置是否是空白处，空白处则取消上一个选中焦点，不弹出菜单
        int nIndex = ui->tableView->verticalHeader()->logicalIndexAt(pos);
        qDebug() << "index = " << nIndex;
        if (nIndex < 0){
            //m_tableView->clearSelection();
            return;
        }

        m_tableCellMenu->clear();
        m_tableCellMenu->addAction("插入行", this, [=](){
            m_standardItemModel->insertRows(nIndex, 1);
            m_bDataChange = true;
        });

        m_tableCellMenu->addAction("增加行", this, [=](){
            m_standardItemModel->insertRows(nIndex + 1, 1);
            m_bDataChange = true;
        });

        m_tableCellMenu->addAction("删除行", this, [=](){
            m_standardItemModel->removeRows(nIndex, 1);
            m_bDataChange = true;
        });


        m_tableCellMenu->exec(pt);
    });

    connect(ui->tableView->horizontalHeader(), &QAbstractItemView::customContextMenuRequested, ui->tableView->horizontalHeader(),[=](const QPoint& pos){
        //mapToGlobal获取m_tableView全局坐标
        //m_tableView->pos()获取m_tableView在父窗口中的相对坐标
        //pos鼠标点击时在表格中的相对位置
        QPoint pt = ui->tableView->parentWidget()->mapToGlobal(ui->tableView->pos()) + pos;
        //判断鼠标右击位置是否是空白处，空白处则取消上一个选中焦点，不弹出菜单
        int nIndex = ui->tableView->horizontalHeader()->logicalIndexAt(pos);
        if (nIndex != 2){
            //m_tableView->clearSelection();
            return;
        }

        QString sField = m_standardItemModel->horizontalHeaderItem(nIndex)->text();
        m_tableCellMenu->clear();

        m_tableCellMenu->addAction("增加关联", this, [=](){
            QString sAlreadyLink = "";
            if (m_mFieldSquence)
            {
                if (m_mFieldSquence->find(m_sTableName) != m_mFieldSquence->end())
                {
                    for (auto & field : (*m_mFieldSquence)[m_sTableName].vSFieldSquences)
                    {
                        if (field.sFieldName == sField)
                        {
                            sAlreadyLink = field.sFieldLink;
                            break;
                        }
                    }
                }
            }

            MainWindow* mainWindow = GlobalConfig::getInstance()->GetMainWindow();
            if (mainWindow)
            {
                mainWindow->OnOpenAddLinkFieldDialog(m_sTableName, m_cellWidget, sField, sAlreadyLink, true);
            }
        });

        m_tableCellMenu->exec(pt);
    });

    //增加数据单元格的菜单
    connect(ui->tableView, &QAbstractItemView::customContextMenuRequested, ui->tableView,[=](const QPoint& pos){
        int nHeight = ui->tableView->horizontalHeader()->height();
        int nWidth = ui->tableView->verticalHeader()->width();
        //mapToGlobal获取m_tableView全局坐标
        //m_tableView->pos()获取m_tableView在父窗口中的相对坐标
        //pos鼠标点击时在表格中的相对位置
        QPoint pt = ui->tableView->parentWidget()->mapToGlobal(ui->tableView->pos()) + pos + QPoint(nWidth, nHeight);
        //判断鼠标右击位置是否是空白处，空白处则取消上一个选中焦点，不弹出菜单
        QModelIndex index = ui->tableView->indexAt(pos);
        if (!index.isValid()){
            return;
        }

        QString sField = m_standardItemModel->data(m_standardItemModel->index(index.row(), 1)).toString();

        m_tableCellMenu->clear();

        if (index.column() == 2 && GlobalConfig::getInstance()->CheckStrIsLuaTable(index.data().toString(), false))
        {
            bool isNew = true;
            for (int row = 0; row < m_vRowDatas.size(); ++row)
            {
                if (sField == m_vRowDatas[row].sKey)
                {
                    ROWINFO rowData = m_vRowDatas[index.row()];

                    if (rowData.nType == LUA_TTABLE)
                    {
                        m_tableCellMenu->addAction(tr("数据展开"), this, [=](){
                            QString sSubTableIndex = "";
                            if(rowData.nKeyType == LUA_TNUMBER || rowData.nKeyType == LUA_TNIL)
                            {
                                sSubTableIndex = m_sTableName + "%ARRAY";
                            }
                            else
                            {
                                sSubTableIndex = m_sTableName + "#" + rowData.sKey;
                            }
                            StringToTableView* dialog = new StringToTableView(m_standardItemModel, index, sSubTableIndex, this->m_mFieldSquence, m_cellWidget, this, m_nLevel + 1);
                            dialog->show();
                        });
                    }

                    isNew = false;
                    break;
                }
            }

            if (isNew)
            {
                bool is_ok = false;
                sField.toInt(&is_ok);

                QString sSubTableIndex = "";
                if (is_ok)
                {
                    sSubTableIndex = m_sTableName + "%ARRAY";
                }
                else
                {
                    sSubTableIndex = m_sTableName + "#" + sField;
                }

                m_tableCellMenu->addAction(tr("数据展开"), this, [=](){
                    StringToTableView* dialog = new StringToTableView(m_standardItemModel, index, sSubTableIndex, this->m_mFieldSquence, m_cellWidget, this, m_nLevel + 1);
                    dialog->show();
                });
            }
        }

        if (index.column() == 1)
        {
            m_tableCellMenu->addAction("增加关联", this, [=](){
                QString sAlreadyLink = "";
                if (m_mFieldSquence)
                {
                    if (m_mFieldSquence->find(m_sTableName) != m_mFieldSquence->end())
                    {
                        for (auto & field : (*m_mFieldSquence)[m_sTableName].vSFieldSquences)
                        {
                            if (field.sFieldName == sField)
                            {
                                sAlreadyLink = field.sFieldLink;
                                break;
                            }
                        }
                    }
                }

                MainWindow* mainWindow = GlobalConfig::getInstance()->GetMainWindow();
                if (mainWindow)
                {
                    mainWindow->OnOpenAddLinkFieldDialog(m_sTableName, m_cellWidget, sField, sAlreadyLink, true);
                }
            });
        }

        m_tableCellMenu->exec(pt);
    });

    if (m_nLevel > 0)
    {
        move(20 + parent->pos().x(), 20 + parent->pos().y());
    }

    //*********************实现表格的撤销功能****************************//
    undoStack = new QUndoStack(this);

    /* 创建Action */
    undoAction = undoStack->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcuts(QKeySequence::Undo);
    redoAction = undoStack->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcuts(QKeySequence::Redo);

    /* 给表格设置委托 */
    TableDelegate *delegate = new TableDelegate(this);

    ui->tableView->setItemDelegate(delegate);
    connect(delegate, &TableDelegate::beginEdit, this, [ = ]()
    {
        QModelIndex index = ui->tableView->currentIndex();
        disconnect(m_standardItemModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(OnItemDataChange(QStandardItem *)));
        m_standardItemModel->setData(index, index.data(Qt::DisplayRole), Qt::UserRole);
        connect(m_standardItemModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(OnItemDataChange(QStandardItem *)));
    });
    connect(delegate, &TableDelegate::closeEditor, this, [ = ]()
    {
        QModelIndex index = ui->tableView->currentIndex();
        QVariant data = index.data(Qt::DisplayRole);
        QVariant oldData = index.data(Qt::UserRole);
        if (data != oldData)
        {
            undoStack->push(new ModifCommand(m_standardItemModel, index, oldData, data, ModifCommandType::singleModelIndex));
        }
    });

    /* 创建UndoView */
//    undoView = new QUndoView(undoStack);
//    undoView->setWindowTitle(tr("Command List"));
//    undoView->show();
//    undoView->setAttribute(Qt::WA_QuitOnClose, false);

    //*********************实现表格的撤销功能****************************//

    SetParam();
}

StringToTableView::~StringToTableView()
{
    delete ui;
}

void StringToTableView::OnItemDataChange(QStandardItem * item)
{
    if (item && item->index().isValid())
    {
        QString sField = m_standardItemModel->data(m_standardItemModel->index(item->index().row(), 1)).toString();
        bool is_ok = false;
        sField.toInt(&is_ok);

        if (item->column() == 0 && is_ok == false)
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
                if (m_mFieldSquence && m_mFieldSquence->size() > 0 && m_mFieldSquence->find(m_sTableName) != m_mFieldSquence->end())
                {
                    FIELDSQUENCE& squence = m_mFieldSquence->find(m_sTableName).value();
                    for (auto& data : squence.vSFieldSquences)
                    {
                        if (data.sFieldName == sFieldName)
                        {
                            isHas = true;
                            if (data.sFieldAnnonation != item->index().data().toString())
                            {
                                data.sFieldAnnonation = item->index().data().toString();
                            }
                            break;
                        }
                    }
                }

                if (isHas == false)
                {
                    FIELDSQUENCE squence;
                    squence.sIndex = m_sTableName;

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

                    m_mFieldSquence->insert(m_sTableName, squence);
                }
            }
        }

        m_bDataChange = true;
    }
}

void StringToTableView::OnConfirmButtonClicked()
{
    if(m_bDataChange)
    {
        OnSaveData();
    }
    close();
}

void StringToTableView::OnCancelButtonClicked()
{
    if (m_bDataChange)
    {
        QMessageBox box(QMessageBox::Warning,QString::fromLocal8Bit("保存修改？"),QString::fromLocal8Bit("表被修改，是否保存？"));
        QPushButton *saveButton = (box.addButton(QString::fromLocal8Bit("保存"),QMessageBox::AcceptRole));
        QPushButton *quitButton = (box.addButton(QString::fromLocal8Bit("退出"),QMessageBox::AcceptRole));
        QPushButton *cancelButton = (box.addButton(QString::fromLocal8Bit("取消"),QMessageBox::RejectRole));
        cancelButton->hide();

        box.exec();

        //请求保存再关闭界面
        if( box.clickedButton() == saveButton )
        {
            //请求保存修改 TODO
            OnSaveData();
            close();
        }
        //直接关闭界面
        else if ( box.clickedButton() == quitButton )
        {
            close();
        }
        //退出message对话框（直接关闭messageBox对话框）
        else if ( box.clickedButton() == cancelButton )
        {
            return;
        }
    }
    else
    {
        close();
    }
}

void StringToTableView::OnChangeData()
{
    QString sResult = "{";
//    QString sResult = "{";
    for (int i = 0; i < m_standardItemModel->rowCount(); i++)
    {
        QVariant vKey = m_standardItemModel->data(m_standardItemModel->index(i, 1));
        QVariant vValue = m_standardItemModel->data(m_standardItemModel->index(i, 2));

        bool is_ok = false;
        vKey.toInt(&is_ok);
        if (!is_ok)
        {
//            FIELDINFO fieldInfo;
//            fieldInfo.sFieldName = vKey.toString().remove("\"");
//            vNewKeySquence.push_back(fieldInfo);

            if(vKey.toString().toStdString().find_first_not_of("-.0123456789\"") == std::string::npos)
            {
                sResult = sResult + "[" + vKey.toString() + "] = ";
            }
            else
            {
                sResult = sResult + vKey.toString() + " = ";
            }
        }
        else
        {
            sResult = sResult + "[" + vKey.toString() + "] = ";
        }

        sResult = sResult + vValue.toString();

        if (i < m_standardItemModel->rowCount() - 1)
        {
            sResult = sResult + ", ";
        }
    }

    sResult = sResult + "}";
    m_sData = sResult;
//    SetParam();
}

void StringToTableView::OnSaveData()
{
    //重新设置table里面的key的排序，数据部分不用考虑
    QVector<FIELDINFO> vNewKeySquence;
    QString sResult = "{";
    for (int i = 0; i < m_standardItemModel->rowCount(); i++)
    {
        QVariant vKey = m_standardItemModel->data(m_standardItemModel->index(i, 1));
        QVariant vValue = m_standardItemModel->data(m_standardItemModel->index(i, 2));

        bool is_ok = false;
        vKey.toInt(&is_ok);
        if (!is_ok)
        {
            FIELDINFO fieldInfo;
            fieldInfo.sFieldName = vKey.toString().remove("\"");
            vNewKeySquence.push_back(fieldInfo);

            if(vKey.toString().toStdString().find_first_not_of("-.0123456789\"") == std::string::npos)
            {
                sResult = sResult + "[" + vKey.toString() + "] = ";
            }
            else
            {
                sResult = sResult + vKey.toString() + " = ";
            }
        }
        else
        {
            sResult = sResult + "[" + vKey.toString() + "] = ";
        }

        sResult = sResult + vValue.toString();

        if (i < m_standardItemModel->rowCount() - 1)
        {
            sResult = sResult + ", ";
        }
    }

    sResult = sResult + "}";

    bool fieldSquenceChange = false;
    if (vNewKeySquence.size() > 0)
    {
        if (m_mFieldSquence && m_mFieldSquence->size() > 0 && m_mFieldSquence->find(m_sTableName) != m_mFieldSquence->end())
        {
            FIELDSQUENCE& squence = m_mFieldSquence->find(m_sTableName).value();
            //删除squence之前部分的交集
            for (int i = 0; i < vNewKeySquence.size(); ++i)
            {
                for (int j = 0; j < squence.vSFieldSquences.size();)
                {
                    //找出交集的部分,并且赋值到vNewKeySquence中
                    if (vNewKeySquence[i].sFieldName == squence.vSFieldSquences[j].sFieldName)
                    {
                        vNewKeySquence[i].sFieldAnnonation = squence.vSFieldSquences[j].sFieldAnnonation;
                        vNewKeySquence[i].sFieldLink = squence.vSFieldSquences[j].sFieldLink;

                        squence.vSFieldSquences.remove(j);
                    }
                    else
                    {
                        ++j;
                    }
                }
            }

            //把vNewKeySquence通过逆序头插入squence中
            for (int i = vNewKeySquence.size() - 1; i >= 0; --i)
            {
                squence.vSFieldSquences.push_front(vNewKeySquence[i]);
                fieldSquenceChange = true;
            }
        }
        //直接插入
        else if (m_mFieldSquence)
        {
            FIELDSQUENCE squence;
            squence.sIndex = m_sTableName;
            squence.vSFieldSquences = vNewKeySquence;
            m_mFieldSquence->insert(m_sTableName, squence);

            fieldSquenceChange = true;
        }
    }


    if (m_nLevel == 0)
    {
        TabWidgetCell* tabWidget = dynamic_cast<TabWidgetCell* >(parent());
        if (tabWidget)
        {
            tabWidget->ChangeModelIndexData(m_index, sResult);

            if (fieldSquenceChange)
            {
                tabWidget->SetDataModify();
            }
        }
    }
    else
    {
        StringToTableView* view = static_cast<StringToTableView*>(parent());
        if (view)
        {
            view->ChangeModelIndexData(m_index, sResult);

            if (fieldSquenceChange)
            {
                view->SetDataModify();
            }
        }
    }
}

void StringToTableView::Flush()
{
    if (m_vRowDatas.size() <= 0) return;

    disconnect(m_standardItemModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(OnItemDataChange(QStandardItem *)));

    m_standardItemModel->clear();

    m_standardItemModel->setHorizontalHeaderItem(0, new QStandardItem(tr("备注")));
    m_standardItemModel->setHorizontalHeaderItem(1, new QStandardItem(tr("字段")));
    m_standardItemModel->setHorizontalHeaderItem(2, new QStandardItem(tr("值")));

    FIELDSQUENCE* squence = nullptr;

    if(m_mFieldSquence && m_mFieldSquence->find(m_sTableName) != m_mFieldSquence->end())
    {
        squence = &m_mFieldSquence->find(m_sTableName).value();
    }

    for (int row = 0; row < m_vRowDatas.size(); ++row)
    {
        auto data = m_vRowDatas[row];

        QString sQValue = data.sField;
        sQValue = sQValue.replace('\n',"\\n");

        std::string sKeyValue = data.sKey.toStdString();

        if(data.nKeyType == LUA_TSTRING && sKeyValue.find_first_not_of("-.0123456789") == std::string::npos)
        {
            sKeyValue = std::string("\"") + sKeyValue + std::string("\"");
        }

        QString sAnnonation = "";
        QString sLinkInfo = "";
        if (squence && row < squence->vSFieldSquences.size())
        {
            sAnnonation = squence->vSFieldSquences[row].sFieldAnnonation;
            sLinkInfo = squence->vSFieldSquences[row].sFieldLink;
        }

        QStandardItem* descItem = new QStandardItem(sAnnonation);

        bool is_ok = false;
        data.sKey.toInt(&is_ok);
        if (is_ok)
        {
            descItem->setEnabled(false);
        }

        m_standardItemModel->setItem(row, 0, descItem);

        QStandardItem* keyItem = new QStandardItem(QString::fromStdString(sKeyValue));
        m_standardItemModel->setItem(row, 1, keyItem);

        QStandardItem* dataItem = new QStandardItem(sQValue);

        if (sLinkInfo != "")
        {
            dataItem->setData(QVariant(DelegateModel::EditAndCombox), Qt::UserRole+2);
            dataItem->setData(QVariant(sLinkInfo), Qt::UserRole+3);
        }

        m_standardItemModel->setItem(row, 2, dataItem);
    }    

    ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    connect(m_standardItemModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(OnItemDataChange(QStandardItem *)));
    m_bDataChange = false;
}

std::string StringToTableView::ParseLuaTableToString(lua_State *L, QString sTableKey)
{
    if (L == NULL) return "";

    std::string sValueTable = "{";
    if (!(lua_type(L, -1) == LUA_TTABLE))
    {
        qDebug () << "is not a table";
        return "";
    }

    QVector<ROWINFO> vKeyValueData;
    QVector<ROWINFO> vArrayValueData;

    // LOG_INFO("lua_rawlen - " + std::to_string(lua_rawlen(L, -2)));
    lua_pushnil(L);
    while(lua_next(L, -2))
    {
        RowInfo info;

        int nKeyType = lua_type(L, -2);
        int nValueType = lua_type(L, -1);

        //字段部分
        QString sKey = "";
        QString sSubTableKey = "";
        if (nKeyType == LUA_TNUMBER || nKeyType == LUA_TNIL)
        {
            sKey = QString::number(lua_tonumber(L, -2));
            sSubTableKey = sTableKey + "%ARRAY";
        }
        else
        {
            sKey = QString::fromStdString(lua_tostring(L, -2));
            sSubTableKey = sTableKey + "#" + sKey;
        }

        info.nKeyType = nKeyType;
        info.sKey = sKey;
        info.nType = nValueType;

        //数据部分
        QString sField = "";

        if (nValueType == LUA_TTABLE)
        {
            sField = QString::fromStdString(ParseLuaTableToString(L, sSubTableKey));
        }
        else if (nValueType == LUA_TSTRING)
        {
            sField = '\"' + QString::fromStdString(lua_tostring(L, -1)) + '\"';
        }
        else if (nValueType == LUA_TBOOLEAN)
        {
            sField = QString(lua_toboolean(L, -1));
        }
        else if (nValueType == LUA_TNIL || nValueType == LUA_TNUMBER)
        {
            sField = QString::fromStdString(lua_tostring(L, -1));
        }

        info.sField = sField;

        if (nKeyType == LUA_TNUMBER || nKeyType == LUA_TNIL)
        {
            vArrayValueData.push_back(info);
        }
        else
        {
            vKeyValueData.push_back(info);
        }

        lua_pop(L, 1);
    }

    //键值对部分进行排序
    if (m_mFieldSquence && m_mFieldSquence->size() > 0 && m_mFieldSquence->find(sTableKey) != m_mFieldSquence->end())
    {
        FIELDSQUENCE fieldSquence = m_mFieldSquence->find(sTableKey).value();

        QMap<QString, quint16> mFieldSquence;
        for (int i = 0; i < fieldSquence.vSFieldSquences.size(); ++i)
        {
            mFieldSquence.insert(fieldSquence.vSFieldSquences[i].sFieldName, i);
        }

        std::sort(vKeyValueData.begin(), vKeyValueData.end(),
              [=](const ROWINFO& a, const ROWINFO& b)
                  {
                      auto iterA = mFieldSquence.find(a.sKey);
                      auto iterB = mFieldSquence.find(b.sKey);


                      quint16 aSort = 9999;
                      quint16 bSort = 9999;

                      if (iterA != mFieldSquence.end())
                      {
                          aSort = iterA.value();
                      }

                      if (iterB != mFieldSquence.end())
                      {
                          bSort = iterB.value();
                      }

                      return aSort < bSort;
                  }
              );
    }

    //数组部分进行排序
    std::sort(vArrayValueData.begin(), vArrayValueData.end(),
          [=](const ROWINFO& a, const ROWINFO& b)
              {
                  a.sKey.toDouble();
                  b.sKey.toDouble();

                  return a.sKey.toDouble() < b.sKey.toDouble();
              }
          );

    int nFlag = 1;
    bool isCompleteArray = true;
    for (int i = 0; i < vArrayValueData.size(); ++i)
    {
        if (nFlag != vArrayValueData[i].sKey.toInt()){
            isCompleteArray = false;
            break;
        }

        nFlag++;
    }

    for (int i = 0; i < vKeyValueData.size(); ++i)
    {
        std::string sKey = vKeyValueData[i].sKey.toStdString();
        if (sKey.find_first_not_of("-.0123456789") == std::string::npos)
        {
            sValueTable = sValueTable + "[\"" + sKey + "\"]";
        }
        else
        {
            sValueTable = sValueTable + sKey;
        }

        sValueTable = sValueTable + " = " + vKeyValueData[i].sField.toStdString();

        if (i < vKeyValueData.size() - 1 || vArrayValueData.size() > 0)
        {
            sValueTable = sValueTable + ", ";
        }
    }

    for (int i = 0; i < vArrayValueData.size(); ++i)
    {
        if (!isCompleteArray)
        {
            sValueTable = sValueTable + "[" + vArrayValueData[i].sKey.toStdString() + "]" + " = ";
        }

        sValueTable = sValueTable + vArrayValueData[i].sField.toStdString();

        if (i < vArrayValueData.size() - 1)
        {
            sValueTable = sValueTable + ", ";
        }
    }

    sValueTable = sValueTable + "}";
    return sValueTable;
}

void StringToTableView::SetParam()
{    
    lua_State *L = luaL_newstate();
    if (L == NULL) return;

    m_vRowDatas.clear();

    QString sTempTableName = "temp_table = " + m_sData;
    int ret = luaL_dostring(L, sTempTableName.toStdString().c_str());
    if (ret)
    {
        qCritical() << lua_tostring(L,-1);
        qDebug() << "m_sData not a table = " << m_sData;
        return;
    }

    lua_getglobal(L, "temp_table");
//    ParseLuaTable(L);

    if (!lua_istable(L, -1))
    {
        qDebug() << "temp_table is not a lua table";
        return;
    }

    QVector<ROWINFO> vKeyValueData;
    QVector<ROWINFO> vArrayValueData;

    //置空栈顶
    lua_pushnil(L);

    while(lua_next(L, -2))
    {
        RowInfo info;

        int nKeyType = lua_type(L, -2);
        int nValueType = lua_type(L, -1);

        //字段部分
        QString sKey = "";
        QString sSubTableKey = "";
        if (nKeyType == LUA_TNUMBER || nKeyType == LUA_TNIL)
        {
            sKey = QString::number(lua_tonumber(L, -2));
            sSubTableKey = m_sTableName + "%ARRAY";
        }
        else
        {
            sKey = QString::fromStdString(lua_tostring(L, -2));
            sSubTableKey = m_sTableName + "#" + sKey;
        }


        info.nKeyType = nKeyType;
        info.sKey = sKey;
        info.nType = nValueType;

        //数据部分
        QString sField = "";

        if (nValueType == LUA_TTABLE)
        {
            sField = QString::fromStdString(ParseLuaTableToString(L, sSubTableKey));
        }
        else if (nValueType == LUA_TSTRING)
        {
            sField = '\"' + QString::fromStdString(lua_tostring(L, -1)) + '\"';
        }
        else if (nValueType == LUA_TBOOLEAN)
        {
            sField = QString(lua_toboolean(L, -1));
        }
        else if (nValueType == LUA_TNIL || nValueType == LUA_TNUMBER)
        {
            sField = QString::fromStdString(lua_tostring(L, -1));
        }

        info.sField = sField;

        if (nKeyType == LUA_TNUMBER || nKeyType == LUA_TNIL)
        {
            vArrayValueData.push_back(info);
        }
        else
        {
            vKeyValueData.push_back(info);
        }

        lua_pop(L, 1);
    }

    //键值对部分进行排序
    if (m_mFieldSquence && m_mFieldSquence->size() > 0 && m_mFieldSquence->find(m_sTableName) != m_mFieldSquence->end())
    {
        FIELDSQUENCE fieldSquence = m_mFieldSquence->find(m_sTableName).value();

        QMap<QString, quint16> mFieldSquence;
        for (int i = 0; i < fieldSquence.vSFieldSquences.size(); ++i)
        {
            mFieldSquence.insert(fieldSquence.vSFieldSquences[i].sFieldName, i);
        }

        std::sort(vKeyValueData.begin(), vKeyValueData.end(),
              [=](const ROWINFO& a, const ROWINFO& b)
                  {
                      auto iterA = mFieldSquence.find(a.sKey);
                      auto iterB = mFieldSquence.find(b.sKey);


                      quint16 aSort = 9999;
                      quint16 bSort = 9999;

                      if (iterA != mFieldSquence.end())
                      {
                          aSort = iterA.value();
                      }

                      if (iterB != mFieldSquence.end())
                      {
                          bSort = iterB.value();
                      }

                      return aSort < bSort;
                  }
              );
    }

    //数组部分进行排序
    std::sort(vArrayValueData.begin(), vArrayValueData.end(),
          [=](const ROWINFO& a, const ROWINFO& b)
              {
                  a.sKey.toDouble();
                  b.sKey.toDouble();

                  return a.sKey.toDouble() < b.sKey.toDouble();
              }
          );

    for (auto data : vKeyValueData)
    {
        m_vRowDatas.push_back(data);
    }

    for (auto data : vArrayValueData)
    {
        m_vRowDatas.push_back(data);
    }

    Flush();

    lua_close(L);
}


void StringToTableView::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_S  &&  ev->modifiers() == Qt::ControlModifier)
    {
        if(m_bDataChange)
        {
            OnSaveData();
        }

        return;
    }

    if (ev->key() == Qt::Key_Z  &&  ev->modifiers() == Qt::ControlModifier)
    {
        undo();
        return;
    }

    if (ev->key() == Qt::Key_Y  &&  ev->modifiers() == Qt::ControlModifier)
    {
        redo();
        return;
    }

    if (ev->key() == Qt::Key_C  &&  ev->modifiers() == Qt::ControlModifier)
    {
        copy();
        return;
    }

    if (ev->key() == Qt::Key_V  &&  ev->modifiers() == Qt::ControlModifier)
    {
        paste();
        return;
    }
}

//撤销
void StringToTableView::undo()
{
    if (!undoStack->canUndo()) return;

    if (undoStack->canUndo())
    {
        undoAction->trigger();
    }

    if (!undoStack->canUndo())
    {
        m_bDataChange = false;
    }
}

//返回撤销
void StringToTableView::redo()
{
    if (undoStack->canRedo())
    {
        redoAction->trigger();
    }
}

void StringToTableView::copy()
{
    QModelIndexList selectList = ui->tableView->selectionModel()->selectedIndexes();
    int min_row = 99999999;
    int max_row = -1;

    int min_col = 99999999;
    int max_col = -1;

    //赛选出最大最小的行列
    for (auto modelIndex : selectList)
    {
        if (modelIndex.row() >= max_row)
        {
            max_row = modelIndex.row();
        }

        if (modelIndex.row() <= min_row)
        {
            min_row = modelIndex.row();
        }

        if (modelIndex.column() >= max_col)
        {
            max_col = modelIndex.column();
        }

        if (modelIndex.column() <= min_col)
        {
            min_col = modelIndex.column();
        }
    }

    if (max_row < 0 || max_col < 0)
    {
        return;
    }

    QVector<QVector<QModelIndex>> modelTables;
    modelTables.resize(max_row + 1);
    for(auto& data : modelTables)
    {
        data.resize(max_col + 1);
    }

    QString strCopy = "";
    for (int row = min_row; row <= max_row; ++row)
    {
        QString strRowCopy = "";
        for (int col = min_col; col <= max_col; ++col)
        {
            QModelIndex modelIndex = m_standardItemModel->index(row, col);
            if (selectList.contains(modelIndex))
            {
                strRowCopy = strRowCopy + modelIndex.data().toString();
            }

            if (col < max_col)
            {
                strRowCopy = strRowCopy + "\t";
            }
        }
        strRowCopy = strRowCopy + "\n";

        strCopy = strCopy + strRowCopy;
    }

    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(strCopy);
}

void StringToTableView::paste()
{
    QModelIndexList selectList = ui->tableView->selectionModel()->selectedIndexes();

    int min_row = 99999999;
    int max_row = -1;

    int min_col = 99999999;
    int max_col = -1;

    //赛选出最大最小的行列
    for (auto modelIndex : selectList)
    {
        if (modelIndex.row() >= max_row)
        {
            max_row = modelIndex.row();
        }

        if (modelIndex.row() <= min_row)
        {
            min_row = modelIndex.row();
        }

        if (modelIndex.column() >= max_col)
        {
            max_col = modelIndex.column();
        }

        if (modelIndex.column() <= min_col)
        {
            min_col = modelIndex.column();
        }
    }

    if (max_row < 0 || max_col < 0)
    {
        return;
    }

    QVector<QVector<QModelIndex>> modelTables;
    modelTables.resize(max_row + 1);
    for(auto& data : modelTables)
    {
        data.resize(max_col + 1);
    }

    QClipboard* clipboard = QApplication::clipboard();
    QString copyStr = clipboard->text();
    QStringList rowStringList = copyStr.split("\n");

    ModifCommandList commandList;
    for (int row = 0; row < rowStringList.size(); ++row)
    {
        QString rowStr = rowStringList[row];
        if(!rowStr.isEmpty())
        {
            QStringList colStringList = rowStr.split("\t");
            for (int col = 0; col < colStringList.size(); ++col)
            {
                QModelIndex modelIndex = m_standardItemModel->index(row + min_row, col + min_col);
                if (selectList.contains(modelIndex))
                {
                    QVariant oldData = modelIndex.data();
                    QVariant data = QVariant(colStringList[col]);

                    ModifInfo sourceInfo;
                    sourceInfo.index = modelIndex;
                    sourceInfo.oldData = oldData;
                    sourceInfo.data = data;

                    commandList.push_front(sourceInfo);

                    m_bDataChange = true;

                    m_standardItemModel->setData(modelIndex, colStringList[col]);
                }
            }
        }
    }

    if (commandList.size() > 0)
    {
        undoStack->push(new ModifCommand(m_standardItemModel, commandList, ModifCommandType::ListModelIndex));
    }
}

void StringToTableView::ChangeModelIndexData(QModelIndex index, QString sData)
{
    if (m_standardItemModel)
    {
        QVariant oldData = index.data();
        QVariant data = QVariant(sData);

        if (data.toString() != oldData.toString())
        {
            m_standardItemModel->setData(index, data);
            undoStack->push(new ModifCommand(m_standardItemModel, index, oldData, data, ModifCommandType::singleModelIndex));

//            OnChangeData();
            m_bDataChange = true;
        }
    }
}

bool StringToTableView::eventFilter(QObject *obj, QEvent *eve)
{
    if (obj == ui->tableView->viewport())
    {
        if (eve->type() == QEvent::Drop)
        {
            const QMimeData* mime = ((QDropEvent*)eve)->mimeData();
            QByteArray encodedata =  mime->data("application/x-qabstractitemmodeldatalist");
            if (encodedata.isEmpty())
            {
                return false;
            }

            QDataStream stream(&encodedata, QIODevice::ReadOnly);
            ModifCommandList commandList;
            while(!stream.atEnd())
            {
                int row, col;

                QMap<int, QVariant> roleDataMap;
                stream >> row >> col >> roleDataMap;

                QModelIndex index = ui->tableView->indexAt(((QDropEvent*)eve)->pos());
                int targetRow = index.row();

                if (targetRow >= 0 && targetRow != row)
                {
                    //只能交换同一列的数据，不能跨列拖拽
                    //交换单元格的数据
                    QModelIndex sourceIndex = m_standardItemModel->index(row, col);
                    QModelIndex targetIndex = m_standardItemModel->index(targetRow, col);
                    QVariant targetData = targetIndex.data();
                    QVariant suorceData = sourceIndex.data();

                    m_standardItemModel->setData(sourceIndex, targetData);
                    m_standardItemModel->setData(targetIndex, roleDataMap.find(0).value());

                    ModifInfo sourceInfo;
                    sourceInfo.index = sourceIndex;
                    sourceInfo.oldData = suorceData;
                    sourceInfo.data = targetData;

                    ModifInfo targetInfo;
                    targetInfo.index = targetIndex;
                    targetInfo.oldData = targetData;
                    targetInfo.data = suorceData;

                    commandList.push_front(sourceInfo);
                    commandList.push_front(targetInfo);

                    m_bDataChange = true;
                }
            }

            if (commandList.size() > 0)
            {
                undoStack->push(new ModifCommand(m_standardItemModel, commandList, ModifCommandType::ListModelIndex));
            }

            return true;
        }
        else
        {
            return QWidget::eventFilter(obj, eve);
        }
    }

    return QWidget::eventFilter(obj, eve);
}
