#include "annonationeditwidget.h"
#include "ui_annonationeditwidget.h"
#include <QDebug>

AnnonationEditWidget::AnnonationEditWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnnonationEditWidget)
{
    ui->setupUi(this);
    m_bModify = false;
//    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    connect(ui->textEdit, &QTextEdit::textChanged, this, [=] () {
        m_bModify = true;
    });
}

AnnonationEditWidget::~AnnonationEditWidget()
{
    delete ui;
}

void AnnonationEditWidget::SetText(QString& str)
{
    ui->textEdit->setPlainText(str);
}

QString AnnonationEditWidget::GetText()
{
    return ui->textEdit ? ui->textEdit->toPlainText() : "";
}

void AnnonationEditWidget::OnShow(quint32 x, quint32 y, quint32 nIndex, QString str/* = ""*/)
{
    show();

    m_nIndex = nIndex;
    setGeometry(x, y, 200, 230);
    ui->textEdit->clear();

    if(!str.isEmpty())
    {
        SetText(str);
    }

    m_bModify = false;
}

void AnnonationEditWidget::OnQuit()
{
    if (m_bModify)
    {
        emit SaveAnnonationsSignal(ui->textEdit->toPlainText(), m_nIndex);
    }
    this->hide();
}
