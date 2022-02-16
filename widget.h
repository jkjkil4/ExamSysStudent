#pragma once

#include <QWidget>

#include <QHostAddress>

class QUdpSocket;
class QTcpSocket;
class QDomElement;

/**
 * @brief   窗口控件
 */
class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void udpSendServerSearch();

    void onUdpReadyRead_SearchServerRetval(const QDomElement &elem);

public slots:
    void onUdpReadyRead();

private:
    QUdpSocket *mUdpSocket;
    QTcpSocket *mTcpSocket;
    QHostAddress mAddress;
};
