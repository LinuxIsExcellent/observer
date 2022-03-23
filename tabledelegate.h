#ifndef TABLEDELEGATE_H
#define TABLEDELEGATE_H

#include <QStyledItemDelegate>
#include "comboboxlineedit.h"

extern "C"
{
     #include "lua.h"
     #include "lauxlib.h"
     #include "lualib.h"
}

enum DelegateModel
{
    NormalEdit = 1,         //普通的lineEdit
    EditAndCombox = 2,      //ComboboxLineedit
    EditAndExpandDataButton = 3,
};

class TableDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TableDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,
                          const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

signals:
    void beginEdit()const;

};

#endif // TABLEDELEGATE_H
