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
signals:
    void OnClickConfirmBtn(quint64 nTime);
private:
    Ui::ModifyServerTimeWidget *ui;
};

#endif // MODIFYSERVERTIMEWIDGET_H
