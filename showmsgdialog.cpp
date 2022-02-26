#include "showmsgdialog.h"
#include "ui_showmsgdialog.h"

ShowMsgDialog::ShowMsgDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowMsgDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog/* | Qt::WindowStaysOnTopHint*/);
    ui->textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
//    ui->textEdit->setDisabled(true);
    ui->textEdit->setReadOnly(true);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(OnclickCloseBtn()));
}

ShowMsgDialog::~ShowMsgDialog()
{
    delete ui;
}

void ShowMsgDialog::SetCloseBtnDisable(bool disable)
{
    ui->pushButton->setDisabled(disable);
}

void ShowMsgDialog::ClearTextEditLine()
{
    ui->textEdit->clear();
}


void ShowMsgDialog::AppendMsg(const QString& str)
{
    ui->textEdit->append(str);
}

void ShowMsgDialog::OnclickCloseBtn()
{
    close();
}

//void ShowMsgDialog::mouseMoveEvent(QMouseEvent* e)
//{
//    if (m_bPressed && (e->buttons() && Qt::LeftButton))
//    {
//        this->move(e->globalPos() - m_ptPress);  //移动事件进行进行位置计算
//        e->accept();
//    }
//}


//void ShowMsgDialog::mousePressEvent(QMouseEvent* e)
//{
//    if (e->button() == Qt::LeftButton)
//    {
//        m_bPressed = true;
//        m_ptPress = e->globalPos() - this->pos();  //转为具体的坐标位置
//        e->accept();
//        this->move(this->pos());
//    }
//}

//void ShowMsgDialog::mouseReleaseEvent(QMouseEvent*)
//{
//    m_bPressed = false;
//}
