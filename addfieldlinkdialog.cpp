#include "addfieldlinkdialog.h"
#include "ui_addfieldlinkdialog.h"
#include <QDebug>

AddFieldLinkDialog::AddFieldLinkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFieldLinkDialog)
{
    ui->setupUi(this);

    setWindowTitle("设置关联");
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    QIcon icon;
    icon.addFile(QStringLiteral("observer.ico"), QSize(), QIcon::Normal, QIcon::Off);
    setWindowIcon(icon);

    ui->msgLabel->setText("");
    ui->treeWidget->setHeaderHidden(true);
    ui->pushButton->setDisabled(true);

    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(OnClickTreeWidgetItem(QTreeWidgetItem *, int)));
}

AddFieldLinkDialog::~AddFieldLinkDialog()
{
    delete ui;
}

void AddFieldLinkDialog::OnShow(QWidget* widget, bool rootWidget/* = true*/, quint16 nIndex/* = 0*/)
{
    m_activeWidget = widget;
    m_bRootWidget = rootWidget;
    m_nIndex = nIndex;
    show();
}

void AddFieldLinkDialog::OnClickTreeWidgetItem(QTreeWidgetItem* item, int nIndex)
{
    if (item)
    {
        QString linkInfo;
        linkInfo = linkInfo + item->text(nIndex);
        qDebug() << item->text(nIndex);

        quint8 nLevel = 0;
        while (item->parent()) {
            item = item->parent();
            qDebug() << item->text(nIndex);
            linkInfo = item->text(nIndex) + "#" + linkInfo;
            nLevel++;
        }

        ui->msgLabel->setText(linkInfo);
        ui->pushButton->setDisabled(nLevel == 2 ? false : true);
        ui->pushButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    }
}

void AddFieldLinkDialog::OnSetProtoFieldLinkInfo(const test_2::send_field_link_info& proto)
{
    ui->treeWidget->clear();

    QList<QTreeWidgetItem *> items;

    QTreeWidgetItem* first = new QTreeWidgetItem;
    first->setText(0, tr("table"));
    items.append(first);

    QTreeWidgetItem* second = new QTreeWidgetItem;
    second->setText(0, tr("list"));
    items.append(second);

    ui->treeWidget->insertTopLevelItems(0, items);

    for (int i = 0; i < proto.table_size();++i)
    {
        test_2::table_field_list lists = proto.table(i);

        QTreeWidgetItem *itemFileName = new QTreeWidgetItem;
        itemFileName->setText(0, QString::fromStdString(lists.table_name()));
        first->addChild(itemFileName);

        for (int j = 0; j < lists.fields_size(); ++j)
        {
            std::string sFields = lists.fields(j);

            QTreeWidgetItem *itemFieldName = new QTreeWidgetItem;
            itemFieldName->setText(0, QString::fromStdString(sFields));
            itemFileName->addChild(itemFieldName);
        }
    }

    for (int i = 0; i < proto.list_size();++i)
    {
        test_2::table_field_list lists = proto.table(i);

        QTreeWidgetItem *itemFileName = new QTreeWidgetItem;
        itemFileName->setText(0, QString::fromStdString(lists.table_name()));
        second->addChild(itemFileName);

        for (int j = 0; j < lists.fields_size(); ++j)
        {
            std::string sFields = lists.fields(j);

            QTreeWidgetItem *itemFieldName = new QTreeWidgetItem;
            itemFieldName->setText(0, QString::fromStdString(sFields));
            itemFileName->addChild(itemFieldName);
        }
    }
}
