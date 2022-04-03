#include "tabledelegate.h"
#include <QLineEdit>
#include <QDebug>
#include "comboboxlineedit.h"

TableDelegate::TableDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

// 创建编辑器
QWidget *TableDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &,
                                     const QModelIndex & index) const
{
    int nDataType = index.model()->data(index, Qt::UserRole+2).toInt();
    if (nDataType == DelegateModel::EditAndCombox)
    {
        QString str = index.model()->data(index, Qt::UserRole+3).toString();    //获取关联的数据
        QModelIndex no_const_index = const_cast<QModelIndex&>(index);

        ComboboxLineedit* editor = new ComboboxLineedit(no_const_index, parent);
        QString value = index.model()->data(index, Qt::DisplayRole).toString();
        editor->setText(value);

        emit beginEdit();
        return editor;
    }
    else
    {
        QLineEdit *editor = new QLineEdit(parent);
        emit beginEdit();

        return editor;
    }
    return nullptr;
}

// 为编辑器设置数据
void TableDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    int nDataType = index.model()->data(index, Qt::UserRole+2).toInt();
    if (nDataType == DelegateModel::EditAndCombox)
    {
        ComboboxLineedit *edit = qobject_cast<ComboboxLineedit*>(editor);
        if (edit)
        {
            edit->setText(index.model()->data(index, Qt::EditRole).toString());
        }
    }
    else
    {
        QLineEdit *edit = qobject_cast<QLineEdit*>(editor);
        if (edit)
        {
            edit->setText(index.model()->data(index, Qt::EditRole).toString());
        }
    }
}

// 将数据写入到模型
void TableDelegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model, const QModelIndex &index) const
{
    int nDataType = index.model()->data(index, Qt::UserRole+2).toInt();
    if (nDataType == DelegateModel::EditAndCombox)
    {
        ComboboxLineedit *edit = qobject_cast<ComboboxLineedit*>(editor);
        if (edit)
        {
            model->setData(index, edit->text());
//            model->setData(index, edit->text(), Qt::UserRole);
        }
    }
    else
    {
        QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
        if (edit)
        {
            model->setData(index, edit->text());
        }
    }
}
