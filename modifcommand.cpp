#include "modifcommand.h"

ModifCommand::ModifCommand(QStandardItemModel *model, QModelIndex index,
                           QVariant oldData, QVariant data, QUndoCommand *parent) : QUndoCommand(parent)
{
    this->model = model;
    this->index = index;
    this->oldData = oldData;
    this->data = data;
}

void ModifCommand::undo()
{
    model->setData(index, oldData);
}

void ModifCommand::redo()
{
    model->setData(index, data);

    QString str = "row:" + QString::number(index.row() + 1) + " col:" + QString::number(index.column() + 1) + " value:" + data.toString();
    setText(str);
}
