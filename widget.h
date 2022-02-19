#pragma once

#include <QWidget>

#include <QHostAddress>

class QUdpSocket;
class QTcpSocket;
class QDomElement;
class QStackedLayout;

class LoginView;
class EnterView;

/**
 * @brief   窗口控件
 */
class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget() override;

    bool parseUdpDatagram(const QByteArray &array);
    bool parseTcpDatagram(const QByteArray &array);
    qint64 tcpSendDatagram(const QByteArray &array);

public slots:
    void udpSendSearchServer();
    void onUdpReadyRead();

public slots:
    void onConnectServer();

    void onTcpConnected();
    void onTcpDisconnected();
    void onTcpReadyRead();
    void onTcpError(QAbstractSocket::SocketError err);

private:
    QUdpSocket *mUdpSocket;
    QTcpSocket *mTcpSocket;
    QHostAddress mAddress;
    QStackedLayout *mStkLayout;

    QByteArray mTcpBuffer;

    LoginView *mLoginView;
    EnterView *mEnterView;
};
