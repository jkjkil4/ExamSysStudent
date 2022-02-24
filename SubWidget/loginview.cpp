#include "loginview.h"
#include "ui_loginview.h"

#include <QStyle>
#include <QMenu>
#include <QMessageBox>

LoginView::LoginView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginView)
{
    ui->setupUi(this);
    ui->btnFlush->setIcon(QApplication::style()->standardIcon(QStyle::StandardPixmap::SP_BrowserReload));
    connect(ui->btnFlush, SIGNAL(clicked()), this, SIGNAL(flushServer()));
    connect(ui->btnConnect, SIGNAL(clicked()), this, SIGNAL(connectServer()));
    connect(ui->btnAbout, &QPushButton::clicked, this, &LoginView::onBtnAboutClicked);

    clearServer();
}

LoginView::~LoginView()
{
    delete ui;
}

void LoginView::setViewEnabled(bool enabled) {
    ui->centralWidget->setEnabled(enabled);
}

void LoginView::clearStuInfo() {
    ui->editName->clear();
    ui->editPwd->clear();
}

void LoginView::clearServer() {
    mListServer.clear();
    ui->cbbServer->clear();
    ui->cbbServer->addItem("(请选择)");
}
void LoginView::addServer(const QString &address, quint16 port, const QString &name) {
    mListServer << Server{ address, port, name };
    ui->cbbServer->addItem(name + QString(" (%1:%2)").arg(address).arg(port));
}

QString LoginView::stuName() { return ui->editName->text().trimmed(); }
QString LoginView::stuPwd() { return ui->editPwd->text(); }
const LoginView::Server* LoginView::currentServer() {
    int ind = ui->cbbServer->currentIndex();
    if(ind == 0)
        return nullptr;
    ind--;
    return &mListServer[ind];
}

void LoginView::onBtnAboutClicked() {
    QAction actAbout("关于");
    connect(&actAbout, &QAction::triggered, [this] {
        QMessageBox::about(
                    this, "关于",
                    "ExamSysStudent v0.1<br>"
                    "作者: jkjkil4<br>"
                    "gitee: <a href=https://gitee.com/jkjkil4/ExamSysStudent>https://gitee.com/jkjkil4/ExamSysStudent</a><br>"
                    "github: <a href=https://github.com/jkjkil4/ExamSysStudent>https://github.com/jkjkil4/ExamSysStudent</a>");
    });

    QAction actAboutQt("关于Qt");
    connect(&actAboutQt, &QAction::triggered, [this] {
        QMessageBox::aboutQt(this);
    });

    QMenu menu;
    menu.addAction(&actAbout);
    menu.addAction(&actAboutQt);
    menu.move(mapToGlobal(ui->btnAbout->pos() + QPoint(0, ui->btnAbout->height())));
    menu.exec();
}
