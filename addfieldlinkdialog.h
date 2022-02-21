#ifndef ADDFIELDLINKDIALOG_H
#define ADDFIELDLINKDIALOG_H

#include <QDialog>

namespace Ui {
class AddFieldLinkDialog;
}

class AddFieldLinkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddFieldLinkDialog(QWidget *parent = nullptr);
    ~AddFieldLinkDialog();

private:
    Ui::AddFieldLinkDialog *ui;
};

#endif // ADDFIELDLINKDIALOG_H
