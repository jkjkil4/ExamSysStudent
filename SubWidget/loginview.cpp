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
}

LoginView::~LoginView()
{
    delete ui;
}

void LoginView::clearServer() {
    mListServer.clear();
    ui->cbbServer->clear();
    ui->cbbServer->addItem("(无)");
}
void LoginView::addServer(const QString &address, quint16 port, const QString &name) {
    mListServer << Server{ address, port, name };
    ui->cbbServer->addItem(name + QString(" (%1:%2)").arg(address).arg(port));
}
