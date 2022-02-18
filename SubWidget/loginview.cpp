#include "loginview.h"
#include "ui_loginview.h"

#include <QStyle>

LoginView::LoginView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginView)
{
    ui->setupUi(this);
    ui->btnFlush->setIcon(QApplication::style()->standardIcon(QStyle::StandardPixmap::SP_BrowserReload));
    connect(ui->btnFlush, SIGNAL(clicked()), this, SIGNAL(flushServer()));
    connect(ui->btnConnect, SIGNAL(clicked()), this, SIGNAL(connectServer()));

    clearServer();
}

LoginView::~LoginView()
{
    delete ui;
}

void LoginView::setBtnEnabled(bool enabled) {
    ui->btnConnect->setEnabled(enabled);
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
