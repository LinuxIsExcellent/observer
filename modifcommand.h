#ifndef MODIFCOMMAND_H
#define MODIFCOMMAND_H

#include <QUndoCommand>
#include <QStandardItemModel>
#include <QVariant>
#include <QModelIndex>

class ModifCommand : public QUndoCommand
{
public:
    explicit ModifCommand(QStandardItemModel *model, QModelIndex index,
                          QVariant oldData, QVariant data, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    QStandardItemModel *model;
    QModelIndex index;
    QVariant oldData;
    QVariant data;

};

#endif // MODIFCOMMAND_H
