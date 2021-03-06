#ifndef ADDFIELDLINKDIALOG_H
#define ADDFIELDLINKDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "tabwidgetcell.h"
#include "msg.pb.h"
#include <google/protobuf/text_format.h>

namespace Ui {
class AddFieldLinkDialog;
}

class TabWidgetCell;

class AddFieldLinkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddFieldLinkDialog(QWidget *parent = nullptr);
    ~AddFieldLinkDialog();

    void OnShow(QString sIndex, TabWidgetCell* widget, QString sField, QString sAlreadyLink, bool rootWidget = true);

    void OnSetProtoFieldLinkInfo(const test_2::send_field_link_info& proto);
private slots:
    void OnClickTreeWidgetItem(QTreeWidgetItem* item, int nIndex);

    void OnPushButtonClicked();
signals:
    void OnFieldLinkChange(QString sField) const;
private:
    Ui::AddFieldLinkDialog *ui;

    TabWidgetCell*     m_activeWidget;     //激活打开的界面    
    bool         m_bRootWidget;     //是否是顶层界面
    QString      m_sField;          //索引
    QString      m_sIndex;          //表的外围信息索引
};

#endif // ADDFIELDLINKDIALOG_H
