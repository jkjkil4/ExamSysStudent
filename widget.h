#pragma once

#include <QWidget>

#include <QHostAddress>
#include <QEventLoop>

class QUdpSocket;
class QTcpSocket;
class QDomElement;
class QStackedLayout;

class LoginView;
class ExamView;

/**
 * @brief   窗口控件
 */
class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget() override;

    void resetSockets();

    bool parseUdpDatagram(const QByteArray &array);
    bool parseTcpDatagram(const QByteArray &array);
    qint64 tcpSendDatagram(const QByteArray &array);

    bool waitForUpload();

public slots:
    void onSendStuProc();
    void onSendStuAns();
    void onSendStuFinish();
    void onReconnect();
    void onLogout();

    void udpSendSearchServer();
    void onUdpReadyRead();

public slots:
    void onConnectServer();

    void onTcpConnected();
    void onTcpDisconnected();
    void onTcpReadyRead();
    void onTcpError(QAbstractSocket::SocketError err);

protected:
    void closeEvent(QCloseEvent *ev) override;

private:
    QUdpSocket *mUdpSocket;
    QTcpSocket *mTcpSocket;
    QHostAddress mAddress;
    QStackedLayout *mStkLayout;

    LoginView *mLoginView;
    ExamView *mExamView;

    QByteArray mTcpBuffer;

    // 用于等待作答上传完成
    QEventLoop mEventLoopWaitUpload;
    // 用于等待提交完成
    QEventLoop mEventLoopWaitFinish;
};
