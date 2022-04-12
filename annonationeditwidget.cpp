#include "annonationeditwidget.h"
#include "ui_annonationeditwidget.h"
#include <QDebug>

AnnonationEditWidget::AnnonationEditWidget(QString sField, QString str/* = ""*/, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnnonationEditWidget),
    m_sField(sField)
{
    ui->setupUi(this);
    SetText(str);

    m_bModify = false;
    this->setWindowFlags(Qt::Popup);
    activateWindow();
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

void AnnonationEditWidget::closeEvent(QCloseEvent *)
{
    if (m_bModify)
    {
        TabWidgetCell* tabWidgetCell = dynamic_cast<TabWidgetCell*>(parent());
        if (tabWidgetCell)
        {
            tabWidgetCell->OnSaveAnnonations("###field_sequence###", ui->textEdit->toPlainText(), m_sField);
        }
    }
}
