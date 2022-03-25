#ifndef SELECTROWDATADIALOG_H
#define SELECTROWDATADIALOG_H

#include <QDialog>

namespace Ui {
class SelectRowDataDialog;
}

class SelectRowDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectRowDataDialog(QWidget *parent = nullptr);
    ~SelectRowDataDialog();

private:
    Ui::SelectRowDataDialog *ui;
};

#endif // SELECTROWDATADIALOG_H
