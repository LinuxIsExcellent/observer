#ifndef TABLEVIEWCELL_H
#define TABLEVIEWCELL_H

#include <QWidget>

namespace Ui {
class TableViewCell;
}

class TableViewCell : public QWidget
{
    Q_OBJECT

public:
    explicit TableViewCell(QWidget *parent = nullptr);
    ~TableViewCell();

    void SetText(QString str);
private:
    Ui::TableViewCell *ui;
};

#endif // TABLEVIEWCELL_H
