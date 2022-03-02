#include "globalapplication.h"
#include <QDebug>

GlobalApplication::GlobalApplication(int &argc,char **argv):
QApplication(argc,argv)
{

}

GlobalApplication::~GlobalApplication()
{

}

void GlobalApplication::setWindowInstance(QWidget *wnd)
{
     widget = wnd;
}

bool GlobalApplication::notify(QObject *obj, QEvent *e)
{
    const QMetaObject* objMeta = obj->metaObject();
    QString clName = objMeta->className();

    if(e->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
        if(keyEvent->key() == Qt::Key_F1)
        {
//            qDebug() << clName;
//            qDebug() << "F1";
        }
     }
    else if(e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseButtonDblClick)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
        if(mouseEvent->buttons() == Qt::LeftButton)
        {

        }
    }
    return QApplication::notify(obj,e);
}
