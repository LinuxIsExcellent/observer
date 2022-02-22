#ifndef ADDFIELDLINKDIALOG_H
#define ADDFIELDLINKDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "msg.pb.h"
#include <google/protobuf/text_format.h>

namespace Ui {
class AddFieldLinkDialog;
}

class AddFieldLinkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddFieldLinkDialog(QWidget *parent = nullptr);
    ~AddFieldLinkDialog();

    void OnShow(QWidget* widget, bool rootWidget = true, quint16 nIndex = 0);

    void OnSetProtoFieldLinkInfo(const test_2::send_field_link_info& proto);
private slots:
    void OnClickTreeWidgetItem(QTreeWidgetItem* item, int nIndex);

private:
    Ui::AddFieldLinkDialog *ui;

    QWidget*     m_activeWidget;     //激活打开的界面
    bool         m_bRootWidget;     //是否是顶层界面
    quint16      m_nIndex;          //索引
};

#endif // ADDFIELDLINKDIALOG_H
