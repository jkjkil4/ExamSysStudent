#include "widget.h"

#include <QStackedLayout>
#include <QMessageBox>

#include <QXmlStreamWriter>
#include <QDomElement>

#include <QUdpSocket>
#include <QTcpSocket>
#include <QNetworkInterface>

#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QMetaEnum>

#include "SubWidget/loginview.h"
#include "SubWidget/enterview.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent),
      mUdpSocket(new QUdpSocket(this)), mTcpSocket(new QTcpSocket(this)),
      mStkLayout(new QStackedLayout),
      mLoginView(new LoginView(this)), mEnterView(new EnterView(this))
{
    mStkLayout->addWidget(mLoginView);
    mStkLayout->addWidget(mEnterView);
    mStkLayout->setCurrentWidget(mLoginView);
    mStkLayout->setMargin(0);
    setLayout(mStkLayout);
    setWindowTitle("考试系统（考生端）");
    resize(1240, 760);

    // 获取本机IP
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    for(const QHostAddress &address : qAsConst(addresses)) {
        if(address != QHostAddress::LocalHost && address.toIPv4Address()) {
            mAddress = address;
            break;
        }
    }
    if(mAddress.isNull())
        mAddress = QHostAddress::LocalHost;

    mUdpSocket->bind(mAddress, 40565, QUdpSocket::ShareAddress);

    connect(mUdpSocket, &QUdpSocket::readyRead, this, &Widget::onUdpReadyRead);
    connect(mTcpSocket, &QTcpSocket::connected, this, &Widget::onTcpConnected);
    connect(mTcpSocket, &QTcpSocket::disconnected, this, &Widget::onTcpDisconnected);
    connect(mTcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, &Widget::onTcpError);

    connect(mLoginView, &LoginView::flushServer, this, &Widget::udpSendServerSearch);
    connect(mLoginView, &LoginView::connectServer, this, &Widget::onConnectServer);

    udpSendServerSearch();
}

Widget::~Widget()
{
    if(mTcpSocket->state() != QTcpSocket::UnconnectedState)
        mTcpSocket->disconnectFromHost();
}

void Widget::udpSendServerSearch() {
    QByteArray array;
    QXmlStreamWriter xml(&array);
    xml.writeStartDocument();
    xml.writeStartElement("ESDatagram");
    xml.writeAttribute("Type", "SearchServer");
    xml.writeCharacters(mAddress.toString());
    xml.writeEndElement();
    xml.writeEndDocument();
    mLoginView->clearServer();
    mUdpSocket->writeDatagram(array, QHostAddress::Broadcast, 40565);
}

void Widget::onUdpReadyRead() {
    QByteArray datagram;
    while (mUdpSocket->hasPendingDatagrams()) {
        datagram.resize(int(mUdpSocket->pendingDatagramSize()));
        mUdpSocket->readDatagram(datagram.data(), datagram.size());
        QDomDocument doc;
        if(!doc.setContent(datagram))
            continue;
        QDomElement root = doc.documentElement();
        if(root.tagName() != "ESDatagram")
            continue;

        QString type = root.attribute("Type");
        if(type == "SearchServerRetval") {
            QString address = root.attribute("Address");
            quint16 port = root.attribute("Port").toUShort();
            QString name = root.text();
            mLoginView->addServer(address, port, name);
        } else if(type == "VerifyErr") {
            mLoginView->setBtnEnabled(true);
            QMessageBox::critical(this, "连接错误", root.text());
        } else if(type == "VerifySucc") {
            if(mStkLayout->currentWidget() == mLoginView)
                mStkLayout->setCurrentWidget(mEnterView);
        }
    }
}

void Widget::onConnectServer() {
    // 检查是否选择服务端
    const LoginView::Server *srv = mLoginView->currentServer();
    if(!srv) {
        QMessageBox::information(this, "提示", "请选择考试");
        return;
    }

    // 连接服务端
    if(mTcpSocket->state() != QTcpSocket::UnconnectedState)
        mTcpSocket->disconnectFromHost();
    mLoginView->setBtnEnabled(false);
    mTcpSocket->connectToHost(QHostAddress(srv->address), srv->port);
}

void Widget::onTcpConnected() {
    QString pwd = mLoginView->stuPwd();
    uint salt = QRandomGenerator::global()->generate();
    QByteArray verify;
    QDataStream ds(&verify, QIODevice::WriteOnly);
    ds << mTcpSocket->localAddress().toIPv4Address() << mTcpSocket->localPort() << pwd << salt;

    QByteArray array;
    QXmlStreamWriter xml(&array);
    xml.writeStartDocument();
    xml.writeStartElement("ESDatagram");
    xml.writeAttribute("Type", "TcpVerify");
    xml.writeAttribute("StuName", mLoginView->stuName());
    xml.writeAttribute("Salt", QString::number(salt));
    xml.writeCharacters(QCryptographicHash::hash(verify, QCryptographicHash::Md5).toHex());
    xml.writeEndElement();
    xml.writeEndDocument();
    mTcpSocket->write(array);
}
void Widget::onTcpDisconnected() {
    mLoginView->setBtnEnabled(true);
}
void Widget::onTcpError(QAbstractSocket::SocketError err) {
    if(err == QAbstractSocket::RemoteHostClosedError)
        return;
    mLoginView->setBtnEnabled(true);
    QMessageBox::critical(this, "连接错误", QMetaEnum::fromType<QAbstractSocket::SocketError>().valueToKey(err));
}
