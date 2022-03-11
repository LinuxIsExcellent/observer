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

StringToTableView::StringToTableView(QStandardItemModel *model, QModelIndex index, QWidget *parent, int nLevel) :
    QDialog(parent),
    ui(new Ui::StringToTableView),
    m_nLevel(nLevel)
{
    ui->setupUi(this);

    this->model = model;
    this->index = index;

    m_sData = index.data().toString();

    setWindowTitle("表");
//    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
//    setWindowFlag(Qt::WindowTitleHint, true);
//    setModal(true);

    m_standardItemModel = new QStandardItemModel(ui->tableView);
    ui->tableView->setModel(m_standardItemModel);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    m_standardItemModel->setHorizontalHeaderItem(0, new QStandardItem("字段名"));
    m_standardItemModel->setHorizontalHeaderItem(1, new QStandardItem("字段值"));

    connect(m_standardItemModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(OnItemDataChange(QStandardItem *)));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(OnCancelButtonClicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(OnConfirmButtonClicked()));

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
                    StringToTableView* dialog = new StringToTableView(m_standardItemModel, index, this, m_nLevel + 1);
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
        m_standardItemModel->setData(index, index.data(Qt::DisplayRole), Qt::UserRole);
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

void StringToTableView::OnSaveData()
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

    qDebug() << "sResult = " << sResult;
    qDebug() << "m_sData = " << m_sData;

    if (m_nLevel == 0)
    {
        TabWidgetCell* tabWidget = dynamic_cast<TabWidgetCell* >(parent());
        if (tabWidget)
        {
            tabWidget->ChangeModelIndexData(index, sResult);
        }
    }
    else
    {
        StringToTableView* view = static_cast<StringToTableView*>(parent());
        if (view)
        {
            view->ChangeModelIndexData(index, sResult);
        }
    }
}

void StringToTableView::Flush()
{
    if (m_vRowDatas.size() <= 0) return;

    int row = 0;
    for (auto data : m_vRowDatas)
    {
        QStandardItem* keyItem = new QStandardItem(data.sKey);
        m_standardItemModel->setItem(row, 0, keyItem);

        QStandardItem* dataItem = new QStandardItem(data.sField);
        m_standardItemModel->setItem(row, 1, dataItem);

        row++;
    }

    ui->tableView->resizeColumnsToContents();
    m_bDataChange = false;
}

std::string StringToTableView::ParseLuaTableToString(lua_State *L)
{
    if (L == NULL) return "";

    std::string sValueTable = "{";
    if (!(lua_type(L, -1) == LUA_TTABLE))
    {
        qDebug () << "is not a string";
        return "";
    }

    lua_pushnil(L);
    bool has_field = false;
    // LOG_INFO("lua_rawlen - " + std::to_string(lua_rawlen(L, -2)));
    while(lua_next(L, -2))
    {
        std::string sKey = "";

        if (lua_type(L, -2) == LUA_TNUMBER || lua_type(L, -2) == LUA_TNIL)
        {
            sKey = "[" + std::to_string(lua_tointeger(L, -2)) + "]";
        }
        else if (lua_type(L, -2) == LUA_TSTRING)
        {
            std::string strKey = lua_tostring(L, -2);
            if (strKey.find_first_not_of("-.0123456789") == std::string::npos)
            {
                sKey = std::string("[\"") + lua_tostring(L, -2) + std::string("\"]");
            }
            else
            {
                sKey = lua_tostring(L, -2);
            }
        }

        sValueTable = sValueTable + sKey + " = ";

        // 如果key值是一个table
        if (lua_type(L, -1) == LUA_TTABLE)
        {
            sValueTable = sValueTable + ParseLuaTableToString(L);
        }
        else if (lua_type(L, -1) == LUA_TSTRING)
        {
            sValueTable = sValueTable + std::string("\"") + lua_tostring(L, -1) + std::string("\"");
            // cout << "sValue = " << lua_tostring(L, -1) << endl;
        }
        else if (lua_type(L, -1) == LUA_TBOOLEAN)
        {
            sValueTable = sValueTable + std::to_string(lua_toboolean(L, -1));
            // cout << "sValue = " << lua_toboolean(L, -1) << endl;
        }
        else if (lua_type(L, -1) == LUA_TNIL)
        {
            sValueTable = sValueTable + std::to_string(lua_tointeger(L, -1));
        }
        else if (lua_type(L, -1) == LUA_TNUMBER)
        {
            double num = lua_tonumber(L, -1);
            sValueTable = sValueTable + GlobalConfig::getInstance()->doubleToString(num);
        }

        sValueTable = sValueTable + ", ";
        lua_pop(L, 1);

        has_field = true;
    }

    // 最后一个", "要去掉，暂时找不到怎么判断lua_next中的元素全部遍历结束的方法，如果有办法判断可以在lua_next里面加方法处理
    if(has_field)
    {
        sValueTable = sValueTable.erase(sValueTable.length() - 2, 2);
    }

    sValueTable = sValueTable + "}";
    return sValueTable;
}

void StringToTableView::SetParam()
{    
    lua_State *L = luaL_newstate();
    if (L == NULL) return;

    m_sData = "temp_table = " + m_sData;
    int ret = luaL_dostring(L, m_sData.toStdString().c_str());
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

    //置空栈顶
    lua_pushnil(L);

    int row = 0;
    while(lua_next(L, -2))
    {
        RowInfo info;

        //字段部分
        QString sKey = "";
        if (lua_type(L, -2) == LUA_TNUMBER)
        {
            sKey = QString::number(lua_tonumber(L, -2));
        }
        else
        {
            sKey = '\"' + QString::fromStdString(lua_tostring(L, -2)) + '\"';
        }


        info.nKeyType = lua_type(L, -2);
        info.sKey = sKey;

        //数据部分
        QString sField = "";
        int nType = lua_type(L, -1);

        if (nType == LUA_TTABLE)
        {
            sField = QString::fromStdString(ParseLuaTableToString(L));
        }
        else if (nType == LUA_TSTRING)
        {
            sField = '\"' + QString::fromStdString(lua_tostring(L, -1)) + '\"';
        }
        else if (nType == LUA_TBOOLEAN)
        {
            sField = QString(lua_toboolean(L, -1));
        }
        else if (nType == LUA_TNIL)
        {
            sField = QString::number(lua_tointeger(L, -1));
        }
        else if (nType == LUA_TNUMBER)
        {
            sField = QString::number(lua_tonumber(L, -1));
        }

        info.sField = sField;
        info.nType = nType;

        m_vRowDatas.push_back(info);

        row++;
        lua_pop(L, 1);
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
