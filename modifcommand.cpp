#include "modifcommand.h"

ModifCommand::ModifCommand(QStandardItemModel *model, QModelIndex index,
                           QVariant oldData, QVariant data, QUndoCommand *parent) : QUndoCommand(parent)
{
    this->model = model;
    this->index = index;
    this->oldData = oldData;
    this->data = data;
}

ModifCommand::ModifCommand(QStandardItemModel *model, ModifCommandList list, QUndoCommand *parent)  : QUndoCommand(parent)
{
    this->model = model;
    m_commandList = list;
}

void ModifCommand::undo()
{
    if (m_commandList.size() == 0)
    {
        model->setData(index, oldData);
    }
    //如果是一堆ModelIndex
    else if (m_commandList.size() > 0)
    {
        for (auto command : m_commandList)
        {
            model->setData(command.index, command.oldData);
        }
    }
}

void ModifCommand::redo()
{
    if (m_commandList.size() == 0)
    {
        model->setData(index, data);
        QString str = "row:" + QString::number(index.row() + 1) + " col:" + QString::number(index.column() + 1) + " value:" + data.toString();
        setText(str);
    }
    else if (m_commandList.size() > 0)
    {
        QString str;
        for (int i = 0; i < m_commandList.size(); ++i)
        {
            auto command = m_commandList[i];
            model->setData(command.index, command.data);
            str = str + "row:" + QString::number(command.index.row() + 1) + " col:" + QString::number(command.index.column() + 1) + " value:" + command.data.toString();

            if (i < m_commandList.size() - 1)
            {
                 str = str + " && ";
            }
        }

        setText(str);
    }
}
