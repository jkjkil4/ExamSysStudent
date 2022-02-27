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
#include "SubWidget/examview.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent),
      mUdpSocket(new QUdpSocket(this)), mTcpSocket(new QTcpSocket(this)),
      mStkLayout(new QStackedLayout),
      mLoginView(new LoginView(this)), mExamView(new ExamView(this)),
      mMulticastAddress("239.255.43.21")
{
    mStkLayout->addWidget(mLoginView);
    mStkLayout->addWidget(mExamView);
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
    mUdpSocket->joinMulticastGroup(mMulticastAddress);

    connect(mUdpSocket, &QUdpSocket::readyRead, this, &Widget::onUdpReadyRead);
    connect(mTcpSocket, &QTcpSocket::connected, this, &Widget::onTcpConnected);
    connect(mTcpSocket, &QTcpSocket::disconnected, this, &Widget::onTcpDisconnected);
    connect(mTcpSocket, &QTcpSocket::readyRead, this, &Widget::onTcpReadyRead);
    connect(mTcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, &Widget::onTcpError);

    connect(mLoginView, &LoginView::flushServer, this, &Widget::udpSendSearchServer);
    connect(mLoginView, &LoginView::connectServer, this, &Widget::onConnectServer);

    connect(mExamView, &ExamView::sendStuProcRequested, this, &Widget::onSendStuProc);
    connect(mExamView, &ExamView::sendStuAnsRequested, this, &Widget::onSendStuAns);
    connect(mExamView, &ExamView::logout, this, &Widget::onLogout);

    udpSendSearchServer();
}

Widget::~Widget()
{
    if(mTcpSocket->state() != QTcpSocket::UnconnectedState)
        mTcpSocket->disconnectFromHost();
}

bool Widget::parseUdpDatagram(const QByteArray &array) {
    QDomDocument doc;
    if(!doc.setContent(array))
        return false;
    QDomElement root = doc.documentElement();
    if(root.tagName() != "ESDtg")
        return false;

    QString type = root.attribute("Type");
    if(type == "SearchServerRetval") {
        QString address = root.attribute("Address");
        quint16 port = root.attribute("Port").toUShort();
        QString name = root.text();
        mLoginView->addServer(address, port, name);
    } else if(type == "VerifyErr") {
        mLoginView->setViewEnabled(true);
        QMessageBox::critical(this, "连接错误", root.text());
    } else if(type == "UpdTime") {
        if(QHostAddress(root.attribute("Address")) == mTcpSocket->peerAddress() && root.attribute("Port").toUShort() == mTcpSocket->peerPort()) {
            QDateTime dateTime = QDateTime::fromString(root.text(), "yyyy/M/d H:m:s");
            if(dateTime.isValid())
                mExamView->setCurDateTime(dateTime);
        }
    } else return false;

    return true;
}
bool Widget::parseTcpDatagram(const QByteArray &array) {
    QDomDocument doc;
    if(!doc.setContent(array))
        return false;
    QDomElement root = doc.documentElement();
    if(root.tagName() != "ESDtg")
        return false;

    QString type = root.attribute("Type");
    if(type == "VerifySucc") {
        if(mStkLayout->currentWidget() == mLoginView) {
            // 设置当前控件
            mExamView->clear();
            mStkLayout->setCurrentWidget(mExamView);
            // 发送获取试卷数据请求
            QByteArray array;
            QXmlStreamWriter xml(&array);
            xml.writeStartDocument();
            xml.writeStartElement("ESDtg");
            xml.writeAttribute("Type", "ExamDataRequest");
            xml.writeAttribute("StuAnsRequest", "1");
            xml.writeEndElement();
            xml.writeEndDocument();
            tcpSendDatagram(array);
        }
    } else if(type == "ExamData") {
        if(mStkLayout->currentWidget() == mExamView) {
            QDomElement elemQuesList;
            QDomElement elemStuAns;
            QDomNode node = root.firstChild();
            while(!node.isNull()) {
                QDomElement elem = node.toElement();
                if(!elem.isNull()) {
                    if(elem.tagName() == "QuesList") {
                        elemQuesList = elem;
                    } else if(elem.tagName() == "StuAns") {
                        elemStuAns = elem;
                    }
                }
                node = node.nextSibling();
            }

            // 设置考试信息
            mExamView->setExamName(root.attribute("Name"));

            // 设置时间显示
            const QString dateTimeFmt = "yyyy/M/d H:m:s";
            mExamView->setStartDateTime(QDateTime::fromString(root.attribute("StartDateTime"), dateTimeFmt));
            mExamView->setEndDateTime(QDateTime::fromString(root.attribute("EndDateTime"), dateTimeFmt));
            mExamView->setCurDateTime(QDateTime::fromString(root.attribute("CurDateTime"), dateTimeFmt));

            // 设置考生信息
            mExamView->setStuName(mLoginView->stuName());

            // 加载试卷
            mExamView->readXmlQues(elemQuesList);
            // 加载作答记录
            do {
                if(elemStuAns.isNull())
                    break;
                QDomDocument docStuAns;
                if(!docStuAns.setContent(elemStuAns.text()))
                    break;
                mExamView->readXmlStuAns(docStuAns.documentElement());
            } while(false);
            if(!elemStuAns.isNull()) {
                QDomElement docStuAns;

            }
        }
    } else if(type == "StuAnsReceived") {
        mExamView->setLastUploadDateTime(QDateTime::fromString(root.text(), "yyyy/M/d H:m:s"));
    } else return false;

    return true;
}
qint64 Widget::tcpSendDatagram(const QByteArray &array) {
    int len = array.length();
    return mTcpSocket->write(QByteArray((char*)&len, 4) + array);
}

void Widget::udpSendSearchServer() {
    QByteArray array;
    QXmlStreamWriter xml(&array);
    xml.writeStartDocument();
    xml.writeStartElement("ESDtg");
    xml.writeAttribute("Type", "SearchServer");
    xml.writeCharacters(mAddress.toString());
    xml.writeEndElement();
    xml.writeEndDocument();
    mLoginView->clearServer();
    mUdpSocket->writeDatagram(array, QHostAddress::Broadcast, 40565);
}

void Widget::onSendStuProc() {
    QByteArray array;
    QXmlStreamWriter xml(&array);
    xml.writeStartDocument();
    xml.writeStartElement("ESDtg");
    xml.writeAttribute("Type", "AnsProc");
    xml.writeCharacters(QString::number(mExamView->proc()));
    xml.writeEndElement();
    xml.writeEndDocument();
    tcpSendDatagram(array);
}
void Widget::onSendStuAns() {
    onSendStuProc();
    //mTcpSocket->flush();
    QByteArray array;
    QXmlStreamWriter xml(&array);
    xml.writeStartDocument();
    xml.writeStartElement("ESDtg");
    xml.writeAttribute("Type", "StuAns");
    xml.writeAttribute("Time", mExamView->curDateTime().toString("yyyy/M/d H:m:s"));
    mExamView->writeXmlStuAns(xml);
    xml.writeEndElement();
    xml.writeEndDocument();
    tcpSendDatagram(array);
}
void Widget::onLogout() {
    mTcpSocket->disconnectFromHost();
    udpSendSearchServer();
    mLoginView->clearStuInfo();
    mStkLayout->setCurrentWidget(mLoginView);
}

void Widget::onUdpReadyRead() {
    QByteArray datagram;
    while (mUdpSocket->hasPendingDatagrams()) {
        datagram.resize(int(mUdpSocket->pendingDatagramSize()));
        mUdpSocket->readDatagram(datagram.data(), datagram.size());
        parseUdpDatagram(datagram);
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
    mLoginView->setViewEnabled(false);
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
    xml.writeStartElement("ESDtg");
    xml.writeAttribute("Type", "TcpVerify");
    xml.writeAttribute("StuName", mLoginView->stuName());
    xml.writeAttribute("Salt", QString::number(salt));
    xml.writeCharacters(QCryptographicHash::hash(verify, QCryptographicHash::Md5).toHex());
    xml.writeEndElement();
    xml.writeEndDocument();
    mTcpSocket->write(array);
}
void Widget::onTcpDisconnected() {
    mLoginView->setViewEnabled(true);
}
void Widget::onTcpReadyRead() {
    mTcpBuffer += mTcpSocket->readAll();

    if(mTcpBuffer.length() < 4)
        return;
    int len = *reinterpret_cast<int*>(mTcpBuffer.data());
    while(mTcpBuffer.length() >= 4 + len) {
        parseTcpDatagram(mTcpBuffer.mid(4, len));
        mTcpBuffer.remove(0, 4 + len);

        if(mTcpBuffer.length() < 4)
            break;
        len = *reinterpret_cast<int*>(mTcpBuffer.data());
    }
}
void Widget::onTcpError(QAbstractSocket::SocketError err) {
    if(err == QAbstractSocket::RemoteHostClosedError)
        return;
    mLoginView->setViewEnabled(true);
    QMessageBox::critical(this, "连接错误", QMetaEnum::fromType<QAbstractSocket::SocketError>().valueToKey(err));
}
