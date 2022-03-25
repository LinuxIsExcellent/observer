#ifndef COMBOBOXLINEEDIT_H
#define COMBOBOXLINEEDIT_H

#include <QWidget>
#include <QResizeEvent>
#include <QModelIndex>

namespace Ui {
class ComboboxLineedit;
}

class ComboboxLineedit : public QWidget
{
    Q_OBJECT

public:
    explicit ComboboxLineedit(QModelIndex index, QWidget *parent = nullptr);
    ~ComboboxLineedit();

    void setText(QString str);

    QString text();

    void addCombox(QString str);

    QModelIndex getModelIndex()
    {
        return m_index;
    }

private slots:
    void OnChangeCurrentText(const QString &str);
protected:
    void resizeEvent(QResizeEvent *event);

private:
    Ui::ComboboxLineedit *ui;

    QModelIndex m_index;
};

#endif // COMBOBOXLINEEDIT_H