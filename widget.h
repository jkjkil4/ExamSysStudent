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

public slots:
    void udpSendServerSearch();
    void onUdpReadyRead();

public slots:
    void onConnectServer();

    void onTcpConnected();
    void onTcpDisconnected();
    void onTcpError(QAbstractSocket::SocketError err);

private:
    QUdpSocket *mUdpSocket;
    QTcpSocket *mTcpSocket;
    QHostAddress mAddress;
    QStackedLayout *mStkLayout;

    LoginView *mLoginView;
    EnterView *mEnterView;
};
