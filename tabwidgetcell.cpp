#include "tabwidgetcell.h"
#include "ui_tabwidgetcell.h"
#include <QTableWidget>
#include <QMimeData>
#include <QClipboard>

#include "tabledelegate.h"
#include "modifcommand.h"

TabWidgetCell::TabWidgetCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabWidgetCell)
{
    ui->setupUi(this);
    m_bTableDataChange = false;

    //初始化上边的widget和layout
    m_topWidget = new QWidget(this);

    m_tableView = new QTableView(this);
    m_rightButtonList = new QListWidget(this);

    m_annonationWidget = new AnnonationEditWidget(this);
    m_annonationWidget->raise();
    m_annonationWidget->hide();

    connect(m_annonationWidget, SIGNAL(SaveAnnonationsSignal(QString, QString, QString)), this, SLOT(OnSaveAnnonations(QString, QString, QString)));

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

    m_bottomButtonList->setViewMode(QListView::ListMode);
    m_bottomButtonList->setFlow(QListView::LeftToRight);
    m_bottomButtonList->setLayoutDirection(Qt::RightToLeft);

    //底层的widget
    ui->bottom_widget->setLayout(vlayout_all);
//    setLayout(vlayout_all);

    QPushButton* resizeContentBtn = new QPushButton(this);
    QListWidgetItem *item = new QListWidgetItem(m_rightButtonList);
    if (item)
    {
        resizeContentBtn->setText(tr("全展开"));
        m_rightButtonList->addItem(item);
        m_rightButtonList->setItemWidget(item, resizeContentBtn);

        connect(resizeContentBtn, SIGNAL(clicked()), m_tableView, SLOT(resizeColumnsToContents()));
    }

    m_saveDataButton = new QPushButton(this);
    m_saveDataButton->setDisabled(true);

    QListWidgetItem *item1 = new QListWidgetItem(m_bottomButtonList);
    if (item1)
    {
        m_saveDataButton->setText(tr("保存数据"));
        m_bottomButtonList->addItem(item1);
        m_bottomButtonList->setItemWidget(item1, m_saveDataButton);

        connect(m_saveDataButton, SIGNAL(clicked()), this, SLOT(OnSaveButtonClicked()));
    }

    m_bottomButtonList->setStyleSheet(R"(
                QListWidget { outline: none; border:1px solid gray; color: black; }
                QListWidget::Item { width: 75px; height: 50px; }
                                )");

    m_tableView->setAlternatingRowColors(true);
    m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tableView->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);

    m_standardItemModel = new QStandardItemModel(m_tableView);

    connect(m_standardItemModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(OnItemDataChange(QStandardItem *)));
    m_tableView->setModel(m_standardItemModel);

    //实现交换两行的效果
    m_tableView->viewport()->installEventFilter(this);
    m_tableView->setDragDropMode(QAbstractItemView::DragDrop);
    m_tableView->setDragEnabled(true);
    m_tableView->setAcceptDrops(true);
//    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    //初始化菜单栏
    m_tableCellMenu = new QMenu(this);

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
            m_standardItemModel->insertRows(nIndex, 1);
            m_bTableDataChange = true;
            ChangeDataModify();
        });

        m_tableCellMenu->addAction("增加行", this, [=](){
            m_standardItemModel->insertRows(nIndex + 1, 1);
            m_bTableDataChange = true;
            ChangeDataModify();
        });

        m_tableCellMenu->addAction("删除行", this, [=](){
            m_standardItemModel->removeRows(nIndex, 1);
            m_bTableDataChange = true;
            ChangeDataModify();
        });

        m_tableCellMenu->exec(pt);
    });

    //*********************实现表格的撤销功能****************************//
    undoStack = new QUndoStack(this);

    /* 创建Action */
    undoAction = undoStack->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcuts(QKeySequence::Undo);
    redoAction = undoStack->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcuts(QKeySequence::Redo);

    /* 给表格设置委托 */
    TableDelegate *delegate = new TableDelegate(this);

    m_tableView->setItemDelegate(delegate);
    connect(delegate, &TableDelegate::beginEdit, this, [ = ]()
    {
        QModelIndex index = m_tableView->currentIndex();
        disconnect(m_standardItemModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(OnItemDataChange(QStandardItem *)));
        m_standardItemModel->setData(index, index.data(Qt::DisplayRole), Qt::UserRole);
        qDebug() << "index.data(Qt::DisplayRole) = " << index.data(Qt::DisplayRole);
        connect(m_standardItemModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(OnItemDataChange(QStandardItem *)));
    });
    connect(delegate, &TableDelegate::closeEditor, this, [ = ](QWidget *editor)
    {
        if (editor->objectName() == "ComboboxLineedit")
        {
            ComboboxLineedit* lineEdit = static_cast<ComboboxLineedit*>(editor);
            if (lineEdit)
            {
                QModelIndex index = lineEdit->getModelIndex();
                m_tableView->setCurrentIndex(index);
            }
        }

        QModelIndex index = m_tableView->currentIndex();
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
}

TabWidgetCell::~TabWidgetCell()
{
    delete ui;
}

void TabWidgetCell::ChangeDataModify()
{
    if (m_tabWidget)
    {
        int nIndex = m_tabWidget->indexOf(this);
        if (IsTableDataChange())
        {
            m_tabWidget->setTabText(nIndex, m_sName + "*");
            m_saveDataButton->setDisabled(false);
        }
        else
        {
            m_tabWidget->setTabText(nIndex, m_sName);
            m_saveDataButton->setDisabled(true);
        }
    }
}

void TabWidgetCell::OnSaveButtonClicked()
{
    OnRequestSaveData();
}

//void TabWidgetCell::onCurrentChanged(const QModelIndex& current, const QModelIndex& previous)
//{
//    qDebug() << "current = " << current;
//    qDebug() << "previous = " << previous;
//}

void TabWidgetCell::OnItemDataChange(QStandardItem *item)
{
    if (item)
    {
        int nRow = item->index().row();
        int nCol = item->index().column();

        //拖拽一个单元格，但是不修改其它数据的时候，itemChanged信号会传一个row = 0, col = 0的数据过来
        // 因为第1行的row是固定的表头，所以不可能会变化。
        //可以用row = 0, col = 0来判定是否只是稍微拖拽一下，并没有改变数据
        if (nRow > 0 && nCol > 0)
        {
            m_bTableDataChange = true;
        }

        ChangeDataModify();
    }
}

void TabWidgetCell::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_S  &&  ev->modifiers() == Qt::ControlModifier)
    {
       OnRequestSaveData();
       return;
    }

    if (ev->key() == Qt::Key_W  &&  ev->modifiers() == Qt::ControlModifier)
    {
        if (m_mainWindow && m_tabWidget)
        {
            m_mainWindow->OnCloseTabWidgetSlot(m_tabWidget->currentIndex());
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

    QWidget::keyPressEvent(ev);
}

void TabWidgetCell::resizeEvent(QResizeEvent *event)
{
    ui->bottom_widget->setGeometry(0, 0, width(), height());
}

bool TabWidgetCell::eventFilter(QObject *obj, QEvent *eve)
{
    if (obj == m_tableView->viewport())
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

                QModelIndex index = m_tableView->indexAt(((QDropEvent*)eve)->pos());
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

                    m_bTableDataChange = true;
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

//撤销
void TabWidgetCell::undo()
{
    if (!undoStack->canUndo()) return;
    if (undoStack->canUndo())
    {
        undoAction->trigger();
    }

    if (!undoStack->canUndo())
    {
        m_bTableDataChange = false;
        ChangeDataModify();
    }
}

//返回撤销
void TabWidgetCell::redo()
{
    if (!undoStack->canRedo()) return;
    if (undoStack->canRedo())
    {
        redoAction->trigger();
    }
}

void TabWidgetCell::copy()
{
    QModelIndexList selectList = m_tableView->selectionModel()->selectedIndexes();
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

void TabWidgetCell::paste()
{
    QModelIndexList selectList = m_tableView->selectionModel()->selectedIndexes();

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

                    m_bTableDataChange = true;

                    m_standardItemModel->setData(modelIndex, colStringList[col]);
                }
            }
        }
    }

    if (commandList.size() > 0)
    {
        undoStack->push(new ModifCommand(m_standardItemModel, commandList));
    }
}

void TabWidgetCell::ChangeModelIndexData(QModelIndex index, QString sData)
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

QString TabWidgetCell::GetRowColumnHeightData()
{
    if (m_tableView && m_standardItemModel && m_standardItemModel->rowCount() > 0)
    {
        for (int row = 0; row < m_standardItemModel->rowCount();++row)
        {
            qDebug() << m_tableView->rowHeight(row);
        }

        for (int col = 0; col < m_standardItemModel->columnCount();++col)
        {
            qDebug() << m_tableView->columnWidth(col);
        }

        for (int row = 0; row < m_standardItemModel->rowCount(); ++row)
        {
            for (int col = 0; col < m_standardItemModel->columnCount(); ++col)
            {
                QStandardItem *item = m_standardItemModel->item(row, col);
                if(item)
                {
                    qDebug() << item->background().color().name();
                }
            }
        }
    }

    return "";
}
