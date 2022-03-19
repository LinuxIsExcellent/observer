#include "logindialog.h"
#include "ui_logindialog.h"
#include <QStringListModel>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QMouseEvent>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    QDesktopWidget* desktopWidget = QApplication::desktop();
    if(desktopWidget)
    {
        this->setGeometry((desktopWidget->width() - width()) / 2, (desktopWidget->height() - height()) / 2, width(), height());
    }

    connect(ui->connect_btn, SIGNAL(clicked()), this, SLOT(OnConnectBtnClicked()));
    connect(ui->quit_btn, SIGNAL(clicked()), this, SLOT(OnclickCloseBtn()));
    connect(ui->close_button, SIGNAL(clicked()), this, SLOT(OnclickCloseBtn()));

    connect(ui->comboBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(OnShowProgramServerLists(const QString &)));
    connect(ui->listView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(OnListViewDoubleClicked(const QModelIndex &)));

    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    InitDialog();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::OnShowError(QMessageBox::Icon icon, QString str)
{
    QMessageBox information(icon, tr("警告"), str, QMessageBox::Ok);
    information.exec();
}

void LoginDialog::InitDialog()
{
    ui->comboBox->clear();

    const qMapPrograms* mapProgram = GlobalConfig::getInstance()->GetProgramData();
    if(mapProgram)
    {
        for (qMapPrograms::const_iterator it = mapProgram->begin();it != mapProgram->end() ; ++it) {
            ui->comboBox->addItem(it.value().sProgramName, it.key());
        }
    }


    OnShowProgramServerLists();
}

void LoginDialog::OnShowProgramServerLists(const QString &)
{
    QString programName = ui->comboBox->currentData().toString();

    const qMapPrograms* mapProgram = GlobalConfig::getInstance()->GetProgramData();
    if (mapProgram)
    {
        QStringList list_str;

        if(mapProgram->contains(programName))
        {
            m_sProgram = mapProgram->value(programName);
            for (QVector<sServerInfo>::const_iterator it = m_sProgram.serverList.begin();it != m_sProgram.serverList.end() ; ++it)
            {
                list_str.push_back(it->name);
            }
        }

        QStringListModel *model;
        model = new QStringListModel(this);

        qDebug() << "list_str = " << list_str;
        model->setStringList(list_str);

        ui->listView->setModel(model);
    }
}

void LoginDialog::OnListViewDoubleClicked(const QModelIndex & model)
{
    int nRow = model.row();

    if(nRow < 0 || m_sProgram.serverList.size() <= 0 || model.row() >= m_sProgram.serverList.size()) return;

    ((MainWindow*)parent())->OnClickConnectServerBtn(m_sProgram.serverList[model.row()].ip, m_sProgram.serverList[model.row()].port);
}

void LoginDialog::OnConnectBtnClicked()
{
    int nRow = ui->listView->selectionModel()->currentIndex().row();
    if(nRow < 0 || m_sProgram.serverList.size() <= 0 || nRow >= m_sProgram.serverList.size())
    {
        qDebug() << "请选择有效行号";
        return;
    }

    ((MainWindow*)parent())->OnClickConnectServerBtn(m_sProgram.serverList[nRow].ip, m_sProgram.serverList[nRow].port);
}

void LoginDialog::OnclickCloseBtn()
{
    QApplication::setQuitOnLastWindowClosed(true);
    ((MainWindow*)parent())->close();
}

void LoginDialog::mouseMoveEvent(QMouseEvent* e)
{
    if (m_bPressed && (e->buttons() && Qt::LeftButton))
    {
        this->move(e->globalPos() - m_ptPress);  //移动事件进行进行位置计算
        e->accept();
    }
}


void LoginDialog::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        m_bPressed = true;
        m_ptPress = e->globalPos() - this->pos();  //转为具体的坐标位置
        e->accept();
        this->move(this->pos());
//        setWindowOpacity(0.4);
    }
}

void LoginDialog::mouseReleaseEvent(QMouseEvent*)
{
    m_bPressed = false;
//    setWindowOpacity(1.0);
}
