#include "modifcommand.h"
#include <QDebug>

ModifCommand::ModifCommand(QStandardItemModel *model, QModelIndex index,
                           QVariant oldData, QVariant data, ModifCommandType type, Qt::ItemDataRole role /*= Qt::EditRole*/, QUndoCommand *parent) : QUndoCommand(parent)
{
    this->model = model;
    this->index = index;
    this->oldData = oldData;
    this->data = data;
    this->m_type = type;
    this->m_role = role;
}

ModifCommand::ModifCommand(QStandardItemModel *model, ModifCommandList list, ModifCommandType type, Qt::ItemDataRole role /*= Qt::EditRole*/, QUndoCommand *parent)  : QUndoCommand(parent)
{
    this->model = model;
    m_commandList = list;
    this->m_type = type;
    this->m_role = role;
}

ModifCommand::ModifCommand(QStandardItemModel *model, QList<int> nRows, QTableView* tableView, ModifCommandType type, QUndoCommand *parent/* = nullptr*/) : QUndoCommand(parent)
{
    this->model = model;
    this->m_setRows = nRows;
    this->m_type = type;
    this->m_tableView = tableView;
}

ModifCommand::ModifCommand(QStandardItemModel *model, QMap<int, int> colWidth, QTableView* tableView, ModifCommandType type, QUndoCommand *parent/* = nullptr*/) : QUndoCommand(parent)
{
    this->model = model;
    this->m_colWidth = colWidth;
    this->m_type = type;
    this->m_tableView = tableView;
}

void ModifCommand::undo()
{
    switch (m_type) {
        case ModifCommandType::singleModelIndex :
        {
            model->setData(index, oldData, m_role);
            break;
        }

        case ModifCommandType::ListModelIndex :
        {
            for (auto command : m_commandList)
            {
                model->setData(command.index, command.oldData, m_role);
            }
            break;
        }

        case ModifCommandType::hiddenRow :
        {
            if (m_tableView)
            {
                for (auto row : m_setRows)
                {
                    m_tableView->setRowHidden(row, false);
                }
            }
            break;
        }

        case ModifCommandType::hiddenCol :
        {
            if (m_tableView)
            {
                for (auto iter = m_colWidth.begin(); iter != m_colWidth.end(); ++iter)
                {
                    m_tableView->setColumnWidth(iter.key(), iter.value());
                }
            }
            break;
        }

        case ModifCommandType::expandCol :
        {
            if (m_tableView)
            {
                for (auto iter = m_colWidth.begin(); iter != m_colWidth.end(); ++iter)
                {
                    m_tableView->setColumnWidth(iter.key(), iter.value());
                }
            }
            break;
        }
    }
}

void ModifCommand::redo()
{
    switch (m_type) {
        case ModifCommandType::singleModelIndex :
        {
            model->setData(index, data, m_role);
            QString str = "row:" + QString::number(index.row() + 1) + " col:" + QString::number(index.column() + 1) + " value:" + data.toString();
            setText(str);
            break;
        }

        case ModifCommandType::ListModelIndex :
        {
            QString str;
            for (int i = 0; i < m_commandList.size(); ++i)
            {
                auto command = m_commandList[i];
                model->setData(command.index, command.data, m_role);
                str = str + "row:" + QString::number(command.index.row() + 1) + " col:" + QString::number(command.index.column() + 1) + " value:" + command.data.toString();

                if (i < m_commandList.size() - 1)
                {
                     str = str + " && ";
                }
            }

            setText(str);
            break;
        }

        case ModifCommandType::hiddenRow :
        {
            if (m_tableView)
            {
                QString str;
                str = str + "hidden row : ";
                for (auto row : m_setRows)
                {
                    str = str + row + " ";
                    m_tableView->setRowHidden(row, true);
                }

                setText(str);
            }

            break;
        }

        case ModifCommandType::hiddenCol :
        {
            if (m_tableView)
            {
                QString str;
                str = str + "hidden col ";
                for (auto iter = m_colWidth.begin(); iter != m_colWidth.end(); ++iter)
                {
                    str = str + QString::number(iter.key()) + ":" + QString::number(iter.value()) + " ";
                    m_tableView->setColumnWidth(iter.key(), COLHIDDENWIDTH);
                }
                setText(str);
            }
            break;
        }

        case ModifCommandType::expandCol :
        {
            if (m_tableView)
            {
                QString str;
                str = str + "expand col ";
                for (auto iter = m_colWidth.begin(); iter != m_colWidth.end(); ++iter)
                {
                    str = str + QString::number(iter.key()) + " ";
                    m_tableView->resizeColumnToContents(iter.key());
                }
                setText(str);
            }
            break;
        }
    }
}
