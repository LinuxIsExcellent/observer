#include "addfieldlinkdialog.h"
#include "ui_addfieldlinkdialog.h"

AddFieldLinkDialog::AddFieldLinkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFieldLinkDialog)
{
    ui->setupUi(this);
}

AddFieldLinkDialog::~AddFieldLinkDialog()
{
    delete ui;
}
