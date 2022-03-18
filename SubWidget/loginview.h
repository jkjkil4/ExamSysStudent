#pragma once

#include <QWidget>

namespace Ui {
class LoginView;
}

/**
 * @brief   登录界面
 */
class LoginView : public QWidget
{
    Q_OBJECT

public:
    struct Server
    {
        QString address;
        quint16 port;
        QString name;
    };

    explicit LoginView(QWidget *parent = nullptr);
    ~LoginView() override;

    /**
     * @brief   设置界面是否可用
     * @param   enabled     是否可用
     */
    void setViewEnabled(bool enabled);

    /**
     * @brief   清空考生信息
     */
    void clearStuInfo();

    /**
     * @brief   清空服务端
     */
    void clearServer();
    /**
     * @brief   添加服务端
     * @param   address   服务端地址
     * @param   port      服务端端口
     * @param   name      服务端名称
     */
    void addServer(const QString &address, quint16 port, const QString &name);

    /**
     * @brief   得到考生名
     * @return  考生名
     */
    QString stuName();
    /**
     * @brief   得到考生密码
     * @return  考生密码
     */
    QString stuPwd();
    /**
     * @brief   当前选中的服务端
     * @return  服务端数据指针，若为nullptr表示没有选中
     */
    const Server* currentServer();

signals:
    void flushServer();
    void connectServer();

public slots:
    void onBtnAboutClicked();

private:
    Ui::LoginView *ui;

    QList<Server> mListServer;
};
