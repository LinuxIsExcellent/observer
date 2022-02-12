#include "tabwidgetcell.h"
#include "ui_tabwidgetcell.h"
#include <QTableWidget>

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

    m_tableView->setAlternatingRowColors(true);
    m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tableView->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);

    m_standardItemModel = new QStandardItemModel(m_tableView);
    m_tableView->setModel(m_standardItemModel);
    connect(m_standardItemModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(OnItemDataChange(QStandardItem *)));

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
}

TabWidgetCell::~TabWidgetCell()
{
    delete ui;
}

void TabWidgetCell::AddAnnotation()
{
    qDebug() << "add annotation";
}

void TabWidgetCell::SetDataModify(bool modify)
{
    if (m_tabWidget)
    {
        int nIndex = m_tabWidget->indexOf(this);
        if (modify)
        {
            m_tabWidget->setTabText(nIndex, m_sName + "*");
        }
        else
        {
            m_tabWidget->setTabText(nIndex, m_sName);
        }
    }
}

void TabWidgetCell::OnItemDataChange(QStandardItem *item)
{
    if (item)
    {
        m_bTableDataChange = true;
        int nRow = item->index().row();
//        if (nRow <= m_vBRowDataChange.count())
//        {
//            m_vBRowDataChange[nRow] = MODIFY;
//        }
//        else
//        {
//            qDebug() << "修改的行的index超过了m_vBRowDataChange里面应有的行数";
//        }
        SetDataModify(true);
    }
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
