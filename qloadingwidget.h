#ifndef QLOADINGWIDGET_H
#define QLOADINGWIDGET_H

#include <QDialog>

namespace Ui {
class QLoadingWidget;
}

class QLoadingWidget : public QDialog
{
    Q_OBJECT

public:
    explicit QLoadingWidget(QWidget *parent = nullptr);
    ~QLoadingWidget();

private:
    Ui::QLoadingWidget *ui;
};

#endif // QLOADINGWIDGET_H
