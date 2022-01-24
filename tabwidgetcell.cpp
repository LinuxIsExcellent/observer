#include "tabwidgetcell.h"
#include "ui_tabwidgetcell.h"

TabWidgetCell::TabWidgetCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabWidgetCell)
{
    ui->setupUi(this);
    m_bHeadIndexChange = false;

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

    QPushButton* sectionMovableBtn = new QPushButton(this);
    QListWidgetItem *item1 = new QListWidgetItem(m_rightButtonList);
    if (item)
    {
        sectionMovableBtn->setText(tr("移动列"));
        m_rightButtonList->addItem(item1);
        m_rightButtonList->setItemWidget(item1, sectionMovableBtn);

        connect(sectionMovableBtn, SIGNAL(clicked()), this, SLOT(sectionMovableBtnClicked()));
    }
    m_tableView->setAlternatingRowColors(true);

    connect(m_tableView->horizontalHeader(), SIGNAL(sectionMoved(int, int, int)), this, SLOT(OnTableViewSectionMoved(int, int, int)));
}

TabWidgetCell::~TabWidgetCell()
{
    delete ui;
}

void TabWidgetCell::OnTableViewSectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    m_bHeadIndexChange = true;
    qDebug() << "logicalIndex = " << logicalIndex;
    qDebug() << "oldVisualIndex = " << oldVisualIndex;
    qDebug() << "newVisualIndex = " << newVisualIndex;

    //当前显示的表头顺序
    for (int i = 0; i < m_tableView->model()->columnCount(); ++i)
    {
        int nVisualIndex = m_tableView->horizontalHeader()->visualIndex(i);
        qDebug() << m_tableView->model()->headerData(nVisualIndex, Qt::Horizontal).toString();
    }
}

void TabWidgetCell::sectionMovableBtnClicked()
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

void TabWidgetCell::SetProtoData(test_2::table_data& proto)
{
    if(m_tableView)
    {
        QStandardItemModel *student_model = new QStandardItemModel();

        for (int i = 0; i < proto.filed_names_size();++i)
        {
            QString strField = QString::fromStdString(proto.filed_names(i));
            m_mFieldNames.insert(strField, i);

            int nType = proto.filed_types(i);
            m_mFieldTypes.insert(strField, nType);

            student_model->setHorizontalHeaderItem(i, new QStandardItem(strField));
        }

        //利用setModel()方法将数据模型与QTableView绑定
        m_tableView->setModel(student_model);

        for (int i = 0; i < proto.row_lists_size();++i)
        {
            test_2::row_data row_data = proto.row_lists(i);

            for (int j = 0; j < row_data.pair_size(); ++j) {
                test_2::pair_value pair = row_data.pair(j);
                auto iter = m_mFieldNames.find(QString::fromStdString(pair.key()));
                if(iter != m_mFieldNames.end())
                {
                    student_model->setItem(i, iter.value(), new QStandardItem(QString::fromStdString(pair.value())));
                }
            }
        }
    }
}
