#ifndef SELECTROWDATADIALOG_H
#define SELECTROWDATADIALOG_H

#include <QDialog>
#include <QTableView>
#include <QStandardItemModel>
#include <QCheckBox>

namespace Ui {
class SelectRowDataDialog;
}

class SelectRowDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectRowDataDialog(int nIndex, QWidget *parent = nullptr);
    ~SelectRowDataDialog();

    void InitDialog();
private slots:
    void OnAllCheckBoxStateChange(int nState);

    void OnConfirmBtnClicked();
private:
    Ui::SelectRowDataDialog *ui;

    int m_nIndex;
    QTableView* m_pTargetTableView;
    QStandardItemModel* m_pTargetModel;

    QStandardItemModel* m_model;

    QCheckBox* m_allCheckBox;
    QVector<QCheckBox*> m_vCheckBoxs;
};

#endif // SELECTROWDATADIALOG_H
