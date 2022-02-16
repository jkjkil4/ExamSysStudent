#include "widget.h"

#include <QXmlStreamWriter>
#include <QDomElement>

#include <QUdpSocket>
#include <QTcpSocket>
#include <QNetworkInterface>

Widget::Widget(QWidget *parent)
    : QWidget(parent),
      mUdpSocket(new QUdpSocket(this)), mTcpSocket(new QTcpSocket(this))
{
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
    mUdpSocket->writeDatagram(array, QHostAddress::Broadcast, 40565);
}

void Widget::onUdpReadyRead_SearchServerRetval(const QDomElement &elem) {
    QString address = elem.text();
    quint16 port = elem.attribute("Port").toUShort();
    Q_UNUSED(address)
    Q_UNUSED(port)
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
