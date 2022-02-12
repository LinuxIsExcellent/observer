#ifndef MODIFYSERVERTIMEWIDGET_H
#define MODIFYSERVERTIMEWIDGET_H

#include <QWidget>

namespace Ui {
class ModifyServerTimeWidget;
}

class ModifyServerTimeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModifyServerTimeWidget(QWidget *parent = nullptr);
    ~ModifyServerTimeWidget();

    void SetTime(quint64 nTime);

    void TimeTick();

    void ShowTimeInfo();
signals:
    void OnClickConfirmBtn(quint64 nTime);
private:
    Ui::ModifyServerTimeWidget *ui;

    quint64     m_nTime;    //时间戳
};

#endif // MODIFYSERVERTIMEWIDGET_H
