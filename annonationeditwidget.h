#ifndef ANNONATIONEDITWIDGET_H
#define ANNONATIONEDITWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QKeyEvent>
#include <windows.h>        //注意头文件
#include <windowsx.h>

namespace Ui {
class AnnonationEditWidget;
}

class AnnonationEditWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AnnonationEditWidget(QWidget *parent = nullptr);
    ~AnnonationEditWidget();

    void SetText(QString& str);
    QString GetText();

    void OnQuit();

    void OnShow(quint32 x, quint32 y, QString str = "");
protected:
//    virtual void keyPressEvent(QKeyEvent *ev);

//    bool nativeEvent(const QByteArray& EventType, void* pMessage, long* pResult);

//    void mousePressEvent(QMouseEvent *event);

//    void mouseMoveEvent(QMouseEvent *event);

//    void mouseReleaseEvent(QMouseEvent *event);
private:
    Ui::AnnonationEditWidget *ui;

    qint32     m_boundaryWidth;        //调整拉伸生效的范围
    QPoint     m_clickPos;
    bool       m_bPressed;
};

#endif // ANNONATIONEDITWIDGET_H
