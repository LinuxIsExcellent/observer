#ifndef MODIFCOMMAND_H
#define MODIFCOMMAND_H

#include <QUndoCommand>
#include <QStandardItemModel>
#include <QVariant>
#include <QModelIndex>

typedef struct ModifInfo
{
    QModelIndex index;
    QVariant oldData;
    QVariant data;
}MODIFINFO;

typedef QVector<MODIFINFO> ModifCommandList;

class ModifCommand : public QUndoCommand
{
public:
    explicit ModifCommand(QStandardItemModel *model, QModelIndex index,
                          QVariant oldData, QVariant data, QUndoCommand *parent = nullptr);

    explicit ModifCommand(QStandardItemModel *model, ModifCommandList list, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    QStandardItemModel *model;
    QModelIndex index;
    QVariant oldData;
    QVariant data;

    ModifCommandList m_commandList;
};

#endif // MODIFCOMMAND_H
