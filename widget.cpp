#include "widget.h"

#include <QStackedLayout>

#include <QXmlStreamWriter>
#include <QDomElement>

#include <QUdpSocket>
#include <QTcpSocket>
#include <QNetworkInterface>

#include "SubWidget/loginview.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent),
      mUdpSocket(new QUdpSocket(this)), mTcpSocket(new QTcpSocket(this)),
      mStkLayout(new QStackedLayout),
      mLoginView(new LoginView(this))
{
    mStkLayout->addWidget(mLoginView);
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

    connect(mLoginView, &LoginView::flushServer, this, &Widget::udpSendServerSearch);

    udpSendServerSearch();
}

Widget::~Widget()
{
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
            onUdpReadyRead_SearchServerRetval(root);
        }
    }
}

void Widget::onUdpReadyRead_SearchServerRetval(const QDomElement &elem) {
    QString address = elem.attribute("Address");
    quint16 port = elem.attribute("Port").toUShort();
    QString name = elem.text();
    mLoginView->addServer(address, port, name);
}
