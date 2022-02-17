#pragma once

#include <QWidget>

namespace Ui {
class LoginView;
}

class LoginView : public QWidget
{
    Q_OBJECT

public:
    explicit LoginView(QWidget *parent = nullptr);
    ~LoginView();

    void clearServer();
    void addServer(const QString &address, quint16 port, const QString &name);

signals:
    void flushServer();

private:
    Ui::LoginView *ui;

    struct Server
    {
        QString address;
        quint16 port;
        QString name;
    };
    QList<Server> mListServer;
};
