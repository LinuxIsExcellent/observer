#include "selectrowdatadialog.h"
#include "ui_selectrowdatadialog.h"

SelectRowDataDialog::SelectRowDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectRowDataDialog)
{
    ui->setupUi(this);
}

SelectRowDataDialog::~SelectRowDataDialog()
{
    delete ui;
}
