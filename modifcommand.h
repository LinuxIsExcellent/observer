#ifndef MODIFCOMMAND_H
#define MODIFCOMMAND_H

#include <QUndoCommand>
#include <QStandardItemModel>
#include <QVariant>
#include <QModelIndex>
#include <QTableView>

#define COLHIDDENWIDTH 5

typedef struct ModifInfo
{
    QModelIndex index;
    QVariant oldData;
    QVariant data;
}MODIFINFO;

typedef QVector<MODIFINFO> ModifCommandList;

enum ModifCommandType
{
    singleModelIndex = 1,
    ListModelIndex = 2,
    hiddenRow = 3,
    hiddenCol = 4,  //为了让界面上知道这个中间还有列，所以并不是隐藏，而是把列宽缩小到5
    expandCol = 5,  //取消隐藏，默认展开到最适合的大小
};

class ModifCommand : public QUndoCommand
{
public:
    explicit ModifCommand(QStandardItemModel *model, QModelIndex index,
                          QVariant oldData, QVariant data, ModifCommandType type, Qt::ItemDataRole role = Qt::EditRole, QUndoCommand *parent = nullptr);

    explicit ModifCommand(QStandardItemModel *model, ModifCommandList list, ModifCommandType type, Qt::ItemDataRole role = Qt::EditRole, QUndoCommand *parent = nullptr);

    explicit ModifCommand(QStandardItemModel *model, QList<int> nRows, QTableView* tableView, ModifCommandType type, QUndoCommand *parent = nullptr);

    explicit ModifCommand(QStandardItemModel *model, QMap<int, int> colWidth, QTableView* tableView, ModifCommandType type, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    QStandardItemModel *model;
    QModelIndex index;
    QVariant oldData;
    QVariant data;
    ModifCommandType m_type;
    Qt::ItemDataRole m_role;
    QList<int> m_setRows;
    QMap<int, int> m_colWidth;

    ModifCommandList m_commandList;

    QTableView* m_tableView;
};

#endif // MODIFCOMMAND_H
