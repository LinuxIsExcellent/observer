#include "annonationeditwidget.h"
#include "ui_annonationeditwidget.h"
#include <QDebug>

AnnonationEditWidget::AnnonationEditWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnnonationEditWidget)
{
    ui->setupUi(this);
//    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
//    setWindowFlags(Qt::WindowStaysOnBottomHint);

    m_bPressed = false;
    m_boundaryWidth = 4;
    this->setStyleSheet("backgroud:#D1EEEE");
}

AnnonationEditWidget::~AnnonationEditWidget()
{
    delete ui;
}

//bool AnnonationEditWidget::nativeEvent(const QByteArray& EventType, void* pMessage, long* pResult)
//{
//    MSG* msg = (MSG*)pMessage;
//    switch(msg->message)
//    {
//        case WM_NCHITTEST:
//            int xPos = GET_X_LPARAM(msg->lParam) - this->frameGeometry().x();
//            int yPos = GET_Y_LPARAM(msg->lParam) - this->frameGeometry().y();
//            if(xPos < m_boundaryWidth && yPos<m_boundaryWidth)                    //左上角
//                /**pResult = HTTOPLEFT*/;
//            else if(xPos>=width()-m_boundaryWidth&&yPos<m_boundaryWidth)          //右上角
//                *pResult = HTTOPRIGHT;
//            else if(xPos<m_boundaryWidth&&yPos>=height()-m_boundaryWidth)         //左下角
//                *pResult = HTBOTTOMLEFT;
//            else if(xPos>=width()-m_boundaryWidth&&yPos>=height()-m_boundaryWidth)//右下角
//                *pResult = HTBOTTOMRIGHT;
//            else if(xPos < m_boundaryWidth)                                     //左边
//                /**pResult =  HTLEFT*/;
//            else if(xPos>=width()-m_boundaryWidth)                              //右边
//                *pResult = HTRIGHT;
//            else if(yPos<m_boundaryWidth)                                       //上边
//                /**pResult = HTTOP*/;
//            else if(yPos>=height()-m_boundaryWidth)                             //下边
//                *pResult = HTBOTTOM;
//            else              //其他部分不做处理，返回false，留给其他事件处理器处理
//               return false;
//            return true;
//    }

//    return false;         //此处返回false，留给其他事件处理器处理
//}

//void AnnonationEditWidget::mousePressEvent(QMouseEvent *event)
//{
//    if (event->button() == Qt::LeftButton)
//    {
//        m_clickPos = event->pos();

//        m_bPressed = true;
//    }
//}

//void AnnonationEditWidget::mouseMoveEvent(QMouseEvent *event)
//{
//    if (event->button() == Qt::LeftButton)
//    {
//        move(event->pos() + pos() - m_clickPos);
//        if (m_bPressed)
//        {
//            this->move(event->globalPos() - m_clickPos);
//        }
//    }
//}

//void AnnonationEditWidget::mouseReleaseEvent(QMouseEvent*)
//{
//    m_bPressed = false;
//}

void AnnonationEditWidget::SetText(QString& str)
{
    ui->textEdit->setPlainText(str);
}

QString AnnonationEditWidget::GetText()
{
    return ui->textEdit ? ui->textEdit->toPlainText() : "";
}

void AnnonationEditWidget::OnShow(quint32 x, quint32 y, QString str/* = ""*/)
{
    show();

    setGeometry(x, y, 200, 230);
    ui->textEdit->clear();

    if(!str.isEmpty())
    {
        SetText(str);
    }
}

void AnnonationEditWidget::OnQuit()
{
    qDebug() << "on quit";

    this->hide();
}

//void AnnonationEditWidget::keyPressEvent(QKeyEvent *ev)
//{
//    if (ev->key() == Qt::Key_Escape)
//    {
//       OnQuit();
//       return;
//    }
//}
