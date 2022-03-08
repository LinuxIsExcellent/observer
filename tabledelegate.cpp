#include "tabledelegate.h"
#include <QLineEdit>

TableDelegate::TableDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

// 创建编辑器
QWidget *TableDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &,
                                     const QModelIndex &) const
{
    QLineEdit *editor = new QLineEdit(parent);

    emit beginEdit();

    return editor;
}

// 为编辑器设置数据
void TableDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit*>(editor);
    if (edit)
    {
        edit->setText(index.model()->data(index, Qt::EditRole).toString());
        return;
    }
}

// 将数据写入到模型
void TableDelegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    if (edit)
    {
        model->setData(index, edit->text());
        return;
    }
}
