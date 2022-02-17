#pragma once

#include <QWidget>

#include <QHostAddress>

class QUdpSocket;
class QTcpSocket;
class QDomElement;
class QStackedLayout;

class LoginView;

/**
 * @brief   窗口控件
 */
class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:
    void udpSendServerSearch();
    void onUdpReadyRead();

public:
    void onUdpReadyRead_SearchServerRetval(const QDomElement &elem);

private:
    QUdpSocket *mUdpSocket;
    QTcpSocket *mTcpSocket;
    QHostAddress mAddress;
    QStackedLayout *mStkLayout;

    LoginView *mLoginView;
};
