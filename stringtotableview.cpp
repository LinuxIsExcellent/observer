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

StringToTableView::StringToTableView(QStandardItemModel *model, QModelIndex index, QString sTableName, QMap<QString, FIELDSQUENCE>* pMFieldSquence, QWidget *parent, int nLevel) :
    QDialog(parent),
    ui(new Ui::StringToTableView),
    m_sTableName(sTableName),
    m_nLevel(nLevel)
{
    ui->setupUi(this);

    this->model = model;
    this->index = index;
    this->m_mFieldSquence = pMFieldSquence;

    m_sData = index.data().toString();

    setWindowTitle("表");
//    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
//    setWindowFlag(Qt::WindowTitleHint, true);
//    setModal(true);

    m_standardItemModel = new QStandardItemModel(ui->tableView);
    ui->tableView->setModel(m_standardItemModel);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->tableView->verticalHeader()->hide();
    m_standardItemModel->setHorizontalHeaderItem(0, new QStandardItem(""));
    m_standardItemModel->setHorizontalHeaderItem(1, new QStandardItem(""));

    connect(m_standardItemModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(OnItemDataChange(QStandardItem *)));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(OnCancelButtonClicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(OnConfirmButtonClicked()));

    //实现交换两行的效果
    ui->tableView->viewport()->installEventFilter(this);
    ui->tableView->setDragDropMode(QAbstractItemView::DragDrop);
    ui->tableView->setDragEnabled(true);
    ui->tableView->setAcceptDrops(true);

    //初始化菜单栏
    m_tableCellMenu = new QMenu(this);

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

        m_tableCellMenu->clear();
        if (index.column() == 1)
        {
            auto rowData = m_vRowDatas[index.row()];
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
                    StringToTableView* dialog = new StringToTableView(m_standardItemModel, index, sSubTableIndex, this->m_mFieldSquence, this, m_nLevel + 1);
                    dialog->show();
                });
            }
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
            undoStack->push(new ModifCommand(m_standardItemModel, index, oldData, data));
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
    if (item->index().row() >= m_vRowDatas.size())
    {
        qDebug() << "m_vRowDatas error " << item->index();
        return;
    }

    m_vRowDatas[item->index().row()].sField = item->index().data().toString();
    qDebug() << "m_vRowDatas = " << m_vRowDatas;
    OnChangeData();
    m_bDataChange = true;
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
    for (int i = 0; i < m_vRowDatas.size(); ++i) {
        auto data = m_vRowDatas[i];

        if (data.nKeyType == LUA_TNUMBER)
        {
            sResult = sResult + "[" + data.sKey + "] = ";
        }
        else if (data.nKeyType == LUA_TSTRING)
        {
            sResult = sResult + data.sKey + " = ";
        }

        if (data.nType == LUA_TSTRING)
        {
            sResult = sResult + '"' + data.sField + '"';
        }
        else
        {
            sResult = sResult + data.sField;
        }

        if (i < m_vRowDatas.size() - 1)
        {
            sResult = sResult + ", ";
        }
    }

    sResult = sResult + "}";

    m_sData = sResult;
    SetParam();
}

void StringToTableView::OnSaveData()
{
    if (m_nLevel == 0)
    {
        TabWidgetCell* tabWidget = dynamic_cast<TabWidgetCell* >(parent());
        if (tabWidget)
        {
            tabWidget->ChangeModelIndexData(index, m_sData);
        }
    }
    else
    {
        StringToTableView* view = static_cast<StringToTableView*>(parent());
        if (view)
        {
            view->ChangeModelIndexData(index, m_sData);
        }
    }
}

void StringToTableView::Flush()
{
    if (m_vRowDatas.size() <= 0) return;

    int row = 0;
    disconnect(m_standardItemModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(OnItemDataChange(QStandardItem *)));

    m_standardItemModel->clear();
    m_standardItemModel->setHorizontalHeaderItem(0, new QStandardItem(""));
    m_standardItemModel->setHorizontalHeaderItem(1, new QStandardItem(""));

    for (auto data : m_vRowDatas)
    {
        std::string sKeyValue = data.sKey.toStdString();

        if(data.nKeyType == LUA_TSTRING && sKeyValue.find_first_not_of("-.0123456789") == std::string::npos)
        {
            sKeyValue = std::string("\"") + sKeyValue + std::string("\"");
        }

        QStandardItem* keyItem = new QStandardItem(QString::fromStdString(sKeyValue));
        m_standardItemModel->setItem(row, 0, keyItem);

        QStandardItem* dataItem = new QStandardItem(data.sField);
        m_standardItemModel->setItem(row, 1, dataItem);

        row++;
    }

    ui->tableView->resizeColumnsToContents();

    connect(m_standardItemModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(OnItemDataChange(QStandardItem *)));
}

std::string StringToTableView::ParseLuaTableToString(lua_State *L, QString sTableKey)
{
    if (L == NULL) return "";

    std::string sValueTable = "{";
    if (!(lua_type(L, -1) == LUA_TTABLE))
    {
        qDebug () << "is not a string";
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
//            sKey = '\"' + QString::fromStdString(lua_tostring(L, -2)) + '\"';
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
        else if (nValueType == LUA_TNIL)
        {
            sField = QString::number(lua_tointeger(L, -1));
        }
        else if (nValueType == LUA_TNUMBER)
        {
            sField = QString::number(lua_tonumber(L, -1));
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
            sValueTable = sValueTable + "[\"" + sKey + "]\"";
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
//            sKey = '\"' + QString::fromStdString(lua_tostring(L, -2)) + '\"';
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
        else if (nValueType == LUA_TNIL)
        {
            sField = QString::number(lua_tointeger(L, -1));
        }
        else if (nValueType == LUA_TNUMBER)
        {
            sField = QString::number(lua_tonumber(L, -1));
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
}


void StringToTableView::keyPressEvent(QKeyEvent *ev)
{
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
}

//撤销
void StringToTableView::undo()
{
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

void StringToTableView::ChangeModelIndexData(QModelIndex index, QString sData)
{
    if (m_standardItemModel)
    {
        QVariant oldData = index.data();
        QVariant data = QVariant(sData);

        if (data != oldData)
        {
            undoStack->push(new ModifCommand(m_standardItemModel, index, oldData, data));
            m_standardItemModel->setData(index, data);
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
                undoStack->push(new ModifCommand(m_standardItemModel, commandList));
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
