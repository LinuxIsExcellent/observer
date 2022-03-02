#include "qloadingwidget.h"
#include "ui_qloadingwidget.h"
#include <QMovie>
#include <QLabel>
#include <QDesktopWidget>

QLoadingWidget::QLoadingWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QLoadingWidget)
{
    ui->setupUi(this);

    setWindowTitle("设置关联");
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
//    setWindowFlags(Qt::FramelessWindowHint);//无边框
    setAttribute(Qt::WA_TranslucentBackground);//背景透明

    setModal(true);

    //屏幕居中显示
    int frmX = width();
    int frmY = height();

    QDesktopWidget w;
    int deskWidth = w.width();
    int deskHeight = w.height();

    QPoint movePoint(deskWidth / 2 - frmX / 2, deskHeight / 2 - frmY / 2);
    move(movePoint);

//    //加载gif图片
//    QMovie *movie = new QMovie(":/new/prefix1/res/loading");
//    ui->label->setMovie(movie);
//    movie->start();
}

QLoadingWidget::~QLoadingWidget()
{
    delete ui;
}
