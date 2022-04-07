#ifndef COMBOBOXLINEEDIT_H
#define COMBOBOXLINEEDIT_H

#include <QWidget>
#include <QResizeEvent>
#include <QModelIndex>
#include "globalconfig.h"

namespace Ui {
class ComboboxLineedit;
}

class ComboboxLineedit : public QWidget
{
    Q_OBJECT

public:
    explicit ComboboxLineedit(QModelIndex index, QString sValue, QWidget *parent = nullptr);
    ~ComboboxLineedit();

    void setText(QString str);

    QString text();

    void addCombox(QString str);

    QModelIndex getModelIndex()
    {
        return m_index;
    }

    void OnRequestLinkFieldInfo();
private slots:
    void OnChangeCurrentText(const QString &str);
    void OnJumpButtonClicked();
protected:
    void resizeEvent(QResizeEvent *event);

    bool eventFilter(QObject *watched, QEvent *event);
private:
    Ui::ComboboxLineedit *ui;

    QModelIndex m_index;
};

#endif // COMBOBOXLINEEDIT_H
