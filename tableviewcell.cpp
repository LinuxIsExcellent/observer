#include "tableviewcell.h"
#include "ui_tableviewcell.h"

TableViewCell::TableViewCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableViewCell)
{
    ui->setupUi(this);
}

TableViewCell::~TableViewCell()
{
    delete ui;
}

void TableViewCell::SetText(QString str)
{
    ui->lineEdit->setText(str);
}
